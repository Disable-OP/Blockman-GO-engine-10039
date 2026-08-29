#pragma once

#include "Core.h"
#include "Chunk/IChunkProvider.h"
#include "Chunk/IChunkStorage.h"

namespace BLOCKMAN
{
        class AnvilManager;
        class World;

        class ChunkReadableStorageFile : public IChunkProvider, public IChunkStorage
        {
        protected:
                std::unique_ptr<AnvilManager> m_anvilManager;

                ChunkReadableStorageFile(World* world)
                        : m_world(world)
                {}

        private:
                World* m_world;

                static int chunkCoordToAnvilCoord(int chunkCoord)
                {
                        return chunkCoord >= 0 ? chunkCoord >> 5 : -((-chunkCoord - 1) >> 5) - 1;
                }
                virtual ChunkPtr provideChunk(int x, int z) override;
                virtual void flushStorage() override;

        protected:
                // Server-authoritative persistence: the server-side provider
                // (ChunkProviderServerFile) subclasses this storage and needs
                // read/write access to implement disk-first / generate-on-miss
                // chunk loading. Previously private; made protected (additive —
                // external callers still go through the public IChunkStorage
                // interface, whose virtual dispatch is unaffected).
                virtual void storeChunk(const ChunkPtr& chunk) override;
                ChunkPtr readChunkFromAnvil(int x, int z);
                void prepareChunk(const ChunkPtr& chunk);
        };
}