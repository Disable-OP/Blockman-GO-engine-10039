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
#include "Stream/ZlibStream.h"
#include "Object/Root.h"

#include <chrono>

using std::chrono::steady_clock;

namespace {

// Encode a chunk into a zlib-compressed NBT byte blob, using the same
// wire format the Anvil region files use. The client passes this blob
// straight to its ChunkReadableStorageFile decoder, so disk and network
// share one decoder.
//
// Implementation: call BLOCKMAN::serialize(ChunkWithMeta*, OutputStream&)
// directly with a ZlibOutputStream. This is exactly what Anvil::placeChunk
// does internally — we just skip the Anvil object (which is abstract and
// has private m_chunkDatas).
bool encodeChunkToBlob(BLOCKMAN::Chunk* chunk, LORD::vector<LORD::ui8>::type& outBlob)
{
        if (!chunk || chunk->isNonexistent())
        {
                outBlob.clear();
                return false;
        }

        // ChunkWithMeta is { int version; Chunk* chunk; } — the NBT wrapper
        // used by the Anvil region format. serialize() writes it as compressed
        // NBT into the output stream.
        BLOCKMAN::ChunkWithMeta chunkWithMeta = { 0, chunk };

        // ZlibOutputStream takes vector<char>, not vector<ui8>. Use a temp
        // vector<char> then copy to the ui8 blob.
        LORD::vector<char>::type charBlob;
        {
                LORD::ZlibOutputStream os(charBlob);
                try
                {
                        BLOCKMAN::serialize(&chunkWithMeta, os);
                }
                catch (const BLOCKMAN::InvalidNbtFormatError& e)
                {
                        LordLogError("Failed to encode chunk (%d, %d): %s", chunk->m_posX, chunk->m_posZ, e.what());
                        return false;
                }
                catch (const std::exception& e)
                {
                        LordLogError("Exception encoding chunk (%d, %d): %s", chunk->m_posX, chunk->m_posZ, e.what());
                        return false;
                }
                // os destructor runs here (end of scope) → flush() → data in charBlob
        }
        outBlob.assign(reinterpret_cast<const LORD::ui8*>(charBlob.data()),
                       reinterpret_cast<const LORD::ui8*>(charBlob.data()) + charBlob.size());
        return !outBlob.empty();
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

        // Server-authoritative worldgen, population step: decorate the chunk
        // (trees, ores, plants...) BEFORE serializing it, so the client always
        // receives a fully-populated chunk. populateChunk is a no-op when the
        // chunk is already populated (flag persisted in Anvil NBT). This runs
        // on the server tick thread (packet dispatch happens in
        // ServerNetwork::logicTick), same as every other world mutation.
        world->populateChunk(packet->m_chunkX, packet->m_chunkZ);

        auto chunk = chunkService->getChunk(packet->m_chunkX, packet->m_chunkZ);
        if (!chunk || chunk->isNonexistent())
        {
                // Chunk doesn't exist (and the generator declined to make one —
                // e.g. outside world border). Drop silently; the client will
                // re-request if it still wants the chunk.
                return;
        }

        LORD::vector<LORD::ui8>::type blob;
        if (!encodeChunkToBlob(chunk.get(), blob))
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

        // Population step first — see handlePacket(C2SPacketRequestChunk).
        for (size_t i = 0; i + 1 < coords.size(); i += 2)
        {
                world->populateChunk(coords[i], coords[i + 1]);
        }

        for (size_t i = 0; i + 1 < coords.size(); i += 2)
        {
                auto chunk = chunkService->getChunk(coords[i], coords[i + 1]);
                if (!chunk || chunk->isNonexistent())
                {
                        continue;
                }

                LORD::vector<LORD::ui8>::type blob;
                if (!encodeChunkToBlob(chunk.get(), blob))
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
