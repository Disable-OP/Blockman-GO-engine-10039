/********************************************************************
filename: 	C2SChunkPacketHandles.cpp
file path:	engine-main/server/src/Network/C2SPacketHandles/C2SChunkPacketHandles.cpp

version:	1
author:		Blockman-GO-engine-10039 contributors
date:		2026-08-03

purpose:	Server-side handlers for the server-authoritative worldgen
            chunk-request packets (see docs/WORLDGEN.md).

            When a client requests a chunk (C2SPacketRequestChunk or
            C2SPacketRequestChunkBulk), the server:
              1. Looks up the chunk via the world's ChunkService. This
                 triggers ChunkProviderGenerate (real WorldGenerator) on
                 cache miss, or loads from Anvil on disk hit.
              2. Encodes the chunk to a compressed byte blob using the
                 same NBT+zlib format the Anvil region files use (so the
                 client can decode with the existing ChunkReadableStorageFile
                 decoder).
              3. Sends an S2CPacketChunkData back to the requesting peer.

            Per-tick throttling lives in ClientPeer (configured in
            ServerNetwork); here we just serve the request.
*********************************************************************/
#include "C2SChunkPacketHandles.h"

#include "Network/ClientPeer.h"
#include "Network/protocol/S2CPackets.h"
#include "Network/ServerNetwork.h"
#include "Server.h"
#include "Blockman/World/ServerWorld.h"
#include "World/World.h"
#include "Chunk/Chunk.h"
#include "Chunk/ChunkService.h"
#include "WorldGenerator/Anvil.h"
#include "WorldGenerator/AnvilManager.h"
#include "Object/Root.h"

#include <chrono>

using std::chrono::steady_clock;

namespace {

// Encode a chunk into a zlib-compressed NBT byte blob, using the same
// wire format the Anvil region files use. The client passes this blob
// straight to its ChunkReadableStorageFile decoder, so disk and network
// share one decoder.
//
// Returns true on success; fills `outBlob`.
bool encodeChunkToBlob(BLOCKMAN::Chunk* chunk, LORD::vector<LORD::ui8>::type& outBlob)
{
	if (!chunk || chunk->isNonexistent())
	{
		outBlob.clear();
		return false;
	}

	// Use a transient Anvil as an encoder: placeChunk() compresses the
	// chunk into the Anvil's in-memory chunkDatas map, which we then
	// copy out. This reuses the proven NBT+zlib encoder instead of
	// hand-rolling a second one.
	//
	// The Anvil is constructed with a null region dir (we never flush
	// it to disk); we only use it as an in-memory encoder.
	BLOCKMAN::Anvil encoder(0, 0, /*regionDir*/ "");
	if (!encoder.placeChunk(chunk))
	{
		return false;
	}

	// placeChunk stored the compressed bytes at the chunk's index.
	// Since we constructed the Anvil at (0,0) and the chunk may be at
	// any (x,z), we can't directly look it up by index — instead we
	// ask the Anvil to serialize the whole region (which is just our
	// one chunk) into a stream and take the chunk-data portion.
	//
	// Simpler: use the Anvil's serialize() which writes the full region
	// file format (header + sectors). The client's decoder expects just
	// the per-chunk compressed payload, so we instead reach into the
	// Anvil's m_chunkDatas via the public extractChunk path.
	//
	// The cleanest public API is: call extractChunk(x,z) which reads
	// back from m_chunkDatas — but that returns a Chunk*, not bytes.
	//
	// Pragmatic approach: we add a tiny helper on Anvil to expose the
	// compressed bytes. For now, since Anvil::m_chunkDatas is protected,
	// we use a transient subclass to grab the bytes.
	//
	// See AnvilEncoderAccessor below.
	return false;  // placeholder until encoder accessor is wired
}

// Subclass to expose Anvil's protected compressed-chunk map for encoding.
class AnvilEncoderAccessor : public BLOCKMAN::Anvil
{
public:
	using Anvil::Anvil;  // inherit constructors

	bool encodeChunk(BLOCKMAN::Chunk* chunk, LORD::vector<LORD::ui8>::type& outBlob)
	{
		if (!placeChunk(chunk))
		{
			return false;
		}
		auto idx = calculateChunkIndex(chunk->m_posX, chunk->m_posZ);
		auto it = this->m_chunkDatas.find(idx);
		if (it == this->m_chunkDatas.end())
		{
			return false;
		}
		outBlob = it->second.compressedChunk;
		return !outBlob.empty();
	}
};

bool encodeChunkToBlobV2(BLOCKMAN::Chunk* chunk, LORD::vector<LORD::ui8>::type& outBlob)
{
	if (!chunk || chunk->isNonexistent())
	{
		return false;
	}
	AnvilEncoderAccessor encoder(0, 0, "");
	return encoder.encodeChunk(chunk, outBlob);
}

} // anonymous namespace


void C2SChunkPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketRequestChunk>& packet)
{
	auto world = Server::Instance()->getWorld();
	if (!world || !world->getChunkService())
	{
		return;
	}

	auto chunkService = world->getChunkService();
	auto chunk = chunkService->getChunk(packet->m_chunkX, packet->m_chunkZ);
	if (!chunk || chunk->isNonexistent())
	{
		// Chunk doesn't exist (and the generator declined to make one —
		// e.g. outside world border). Drop silently; the client will
		// re-request if it still wants the chunk.
		return;
	}

	LORD::vector<LORD::ui8>::type blob;
	if (!encodeChunkToBlobV2(chunk.get(), blob))
	{
		LordLogWarning("Failed to encode chunk (%d, %d) for client %llu",
			packet->m_chunkX, packet->m_chunkZ, (unsigned long long)clientPeer->getRakssid());
		return;
	}

	auto s2c = LORD::make_shared<S2CPacketChunkData>();
	s2c->m_chunkX = packet->m_chunkX;
	s2c->m_chunkZ = packet->m_chunkZ;
	s2c->m_blob = std::move(blob);
	s2c->encode();

	Server::Instance()->getNetwork()->sendPacket(s2c, clientPeer->getRakssid(), /*reliable=*/true);
}

void C2SChunkPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketRequestChunkBulk>& packet)
{
	auto world = Server::Instance()->getWorld();
	if (!world || !world->getChunkService())
	{
		return;
	}

	auto chunkService = world->getChunkService();
	const auto& coords = packet->m_coords;

	// coords is a flat array of (chunkX, chunkZ) pairs.
	if (coords.size() % 2 != 0)
	{
		LordLogWarning("C2SPacketRequestChunkBulk: odd coord count %zu from client %llu",
			coords.size(), (unsigned long long)clientPeer->getRakssid());
		return;
	}

	for (size_t i = 0; i + 1 < coords.size(); i += 2)
	{
		auto chunk = chunkService->getChunk(coords[i], coords[i + 1]);
		if (!chunk || chunk->isNonexistent())
		{
			continue;
		}

		LORD::vector<LORD::ui8>::type blob;
		if (!encodeChunkToBlobV2(chunk.get(), blob))
		{
			continue;
		}

		auto s2c = LORD::make_shared<S2CPacketChunkData>();
		s2c->m_chunkX = coords[i];
		s2c->m_chunkZ = coords[i + 1];
		s2c->m_blob = std::move(blob);
		s2c->encode();

		Server::Instance()->getNetwork()->sendPacket(s2c, clientPeer->getRakssid(), /*reliable=*/true);
	}
}
