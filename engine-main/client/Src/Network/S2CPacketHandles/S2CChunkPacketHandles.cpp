#include "S2CChunkPacketHandles.h"

#include "Network/ClientNetwork.h"
#include "Network/ClientPacketSender.h"
#include "Network/protocol/S2CPackets.h"

#include "cWorld/Blockman.h"
#include "cWorld/WorldClient.h"
#include "game.h"

#include "Chunk/Chunk.h"
#include "Chunk/ChunkService.h"
#include "World/World.h"

#include "WorldGenerator/Anvil.h"          // ChunkWithMeta
#include "Nbt/Serializer.h"                // deserialize<T>(pObject, InputStream&)
#include "Stream/ZlibStream.h"             // ZlibInputStream

#include "Object/Root.h"

#include <cstring>

using namespace LORD;     // for Vector3i, ui8, make_shared, etc.
using namespace BLOCKMAN; // for Blockman, Chunk, ChunkPtr, ChunkWithMeta, etc.

// ============================================================================
// Client-side handlers for the server-authoritative worldgen packets.
//
// Wire format produced by the server (C2SChunkPacketHandles::encodeChunkToBlob):
//   - The chunk is wrapped in a ChunkWithMeta struct { int version; Chunk* chunk; }
//   - That struct is NBT-encoded (TAG_COMPOUND with "DataVersion" + "Level")
//   - The NBT stream is zlib-compressed
//   - The compressed bytes are placed in S2CPacketChunkData::m_blob
//
// The decode path mirrors the disk path: zlib-inflate → NBT-parse → construct
// Chunk. We then call ChunkService::injectChunk() to add it to the cache,
// which lets the next getChunk(x, z) hit the cache instead of triggering
// another C2SRequestChunk round-trip.
// ============================================================================

void S2CChunkPacketHandles::handlePacket(std::shared_ptr<S2CPacketWorldInfo>& packet)
{
        // The server sends this once on connect, before any chunk data. We use
        // it to override the client's local hardcoded spawn point with the
        // server's authoritative one. The client's Blockman::init() picks the
        // local seed; that's fine — the client never runs worldgen (chunks
        // come from the server), the seed is only used for client-side random
        // state (mob spawning, particle seeds, etc).
        auto bm = Blockman::Instance();
        if (!bm || !bm->m_pWorld) return;

        Vector3i serverSpawn(packet->m_spawnX, packet->m_spawnY, packet->m_spawnZ);
        bm->m_pWorld->setSpawnLocation(serverSpawn);

        LordLogInfo("S2CPacketWorldInfo: spawn=(%d, %d, %d) worldType=%d dimension=%d seedHash=%llu",
                packet->m_spawnX, packet->m_spawnY, packet->m_spawnZ,
                (int)packet->m_worldType, (int)packet->m_dimension,
                (unsigned long long)packet->m_worldSeedHash);
}

void S2CChunkPacketHandles::handlePacket(std::shared_ptr<S2CPacketChunkData>& packet)
{
        auto bm = Blockman::Instance();
        if (!bm || !bm->m_pWorld || !bm->m_pWorld->getChunkService()) return;
        if (packet->m_blob.empty()) return;

        // The blob is the same format as an Anvil chunk record: zlib-compressed
        // NBT. ZlibInputStream takes vector<char>; m_blob is vector<ui8>. Use
        // a memcpy to convert (same bits, different type).
        std::vector<char> charBlob(packet->m_blob.size());
        if (!charBlob.empty())
        {
                std::memcpy(charBlob.data(), packet->m_blob.data(), packet->m_blob.size());
        }

        // Decompress + NBT-parse in one step: deserialize<T>(pObject, InputStream&)
        // wraps the input stream in an NbtParser, which uses ZlibInputStream
        // internally as its StreamType.
        ChunkWithMeta chunkWithMeta = { 0, nullptr };
        bool ok = false;
        try
        {
                ZlibInputStream is(charBlob);
                ok = deserialize(&chunkWithMeta, is);
        }
        catch (const StreamError& e)
        {
                LordLogError("S2CPacketChunkData: stream error decoding chunk (%d, %d): %s",
                        packet->m_chunkX, packet->m_chunkZ, e.what());
                ok = false;
        }
        catch (const InvalidNbtFormatError& e)
        {
                LordLogError("S2CPacketChunkData: NBT error decoding chunk (%d, %d): %s",
                        packet->m_chunkX, packet->m_chunkZ, e.what());
                ok = false;
        }

        if (!ok || !chunkWithMeta.chunk)
        {
                LordLogWarning("S2CPacketChunkData: failed to decode chunk (%d, %d)",
                        packet->m_chunkX, packet->m_chunkZ);
                // Safe-delete the chunk if deserialize allocated one but failed partway.
                if (chunkWithMeta.chunk) LordSafeDelete(chunkWithMeta.chunk);
                return;
        }

        // Hand ownership of the raw Chunk* to a shared_ptr (the cache stores
        // shared_ptr<Chunk>). The chunk's m_posX/m_posZ come from the NBT
        // (set by the disk format), but we overwrite with the packet's coords
        // to be safe (the server may have rotated/transformed the chunk).
        chunkWithMeta.chunk->m_posX = packet->m_chunkX;
        chunkWithMeta.chunk->m_posZ = packet->m_chunkZ;
        chunkWithMeta.chunk->m_pWorld = bm->m_pWorld;

        ChunkPtr chunkPtr(chunkWithMeta.chunk);

        // Inject into the cache. The next getChunk(x, z) on the client will
        // hit the cache and return this chunk instead of querying the disk
        // provider (which would return NonexistentChunk and re-fire a
        // C2SRequestChunk — we don't want that loop).
        bm->m_pWorld->getChunkService()->injectChunk(chunkPtr);

        LordLogInfo("S2CPacketChunkData: injected chunk (%d, %d), %zu bytes",
                packet->m_chunkX, packet->m_chunkZ, packet->m_blob.size());
}

