#ifndef __CHUNK_SERVICE_SERVER_HEADER__
#define __CHUNK_SERVICE_SERVER_HEADER__

#include "Chunk/ChunkService.h"

namespace BLOCKMAN
{
        class ChunkServiceServer : public ChunkService
        {
        public:
                ChunkServiceServer(World* world)
                        : ChunkService(world, 64, false)
                {}

                // chunkInCache: intentionally NOT overridden anymore.
                //
                // The old override was `return getChunk(x, z) != nullptr;` — a full
                // SYNCHRONOUS GENERATION inside a predicate that every caller (World::
                // chunkExists, entity code, blockExists) assumes is a cheap, pure
                // check. That broke the vanilla contract of chunkExists() and caused
                // unbounded recursion during worldgen:
                //
                //   ChunkService::getChunk -> ChunkProviderServerFile::provideChunk
                //   -> ChunkProviderGenerate::provideChunk -> MapGenCaves::generate
                //   -> generateCaveNode -> World::getBiomeGenForCoords(far x,z)
                //   -> World::blockExists -> chunkExists -> chunkInCache -> getChunk
                //   -> (that chunk is uncached -> GENERATES it -> its caves query
                //       yet another uncached chunk -> ...) until the 8 MB thread
                //       stack overflows and the process dies (SIGSEGV, verified via
                //   full tombstone from the Redroid arm64 CI run).
                //
                // The base-class implementation (m_cache.inCache(x, z)) is a pure
                // hash-map lookup: unloaded chunk -> false, and getBiomeGenForCoords
                // falls back to the biome layers (pure computation, no chunk load).
        };
}

#endif