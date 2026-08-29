#pragma once

/********************************************************************
filename:       ChunkProviderServerFile.h
file path:      engine-main/server/src/Blockman/Chunk/ChunkProviderServerFile.h

purpose:        The server's authoritative chunk provider: DISK-FIRST,
                GENERATE-ON-MISS.

                provideChunk(x, z):
                  1. Try to load the chunk from the Anvil region files
                     (player edits + previously generated + populated
                     chunks persist across app restarts).
                  2. On disk miss, run the real vanilla overworld generator
                     (ChunkProviderGenerate: biome terrain, caves, ravines,
                     surface) and immediately store the result, so the
                     world is durable and a crash mid-session loses at most
                     the not-yet-generated area (which regenerates
                     deterministically from the same seed).

                Population (trees, ores, plants — BiomeDecorator) is a
                SEPARATE step driven by ServerWorld::populateChunk, because
                vanilla decoration of chunk (x,z) requires the +x/+z
                neighbours to exist first.

                The class inherits ChunkReadableStorageFileServer so it IS
                both the IChunkProvider and the IChunkStorage registered on
                the ChunkService — a single Anvil region-file manager handles
                all reads AND writes (no two caches racing on the same .mca
                file).
*********************************************************************/

#include "Chunk/ChunkProviderGenerate.h"
#include "Blockman/Chunk/ChunkReadableStorageFileServer.h"
#include "Util/PathUtil.h"

namespace BLOCKMAN
{
        class ChunkProviderServerFile : public ChunkReadableStorageFileServer
        {
        public:
                ChunkProviderServerFile(World* world, const LORD::String& regionDir, i64 seed)
                        : ChunkReadableStorageFileServer(world, regionDir)
                        , m_generator(LORD::make_shared<ChunkProviderGenerate>(world, seed, /*mapFeatures=*/false))
                {
                        // AnvilManager::saveAnvil does NOT create the region directory —
                        // without this, every storeChunk() silently fails on first boot.
                        PathUtil::CreateDir(regionDir);
                }

                virtual ChunkPtr provideChunk(int x, int z) override
                {
                        // 1) Disk first — chunk was generated/edited/populated before.
                        //    readChunkFromAnvil returns a NonexistentChunk on miss.
                        ChunkPtr chunk = readChunkFromAnvil(x, z);
                        if (chunk && !chunk->isNonexistent())
                        {
                                // Same post-load fixup the plain storage provider does:
                                // world back-pointer, skylight map, section refcounts,
                                // tile-entity world objects, TerrainPopulated flag.
                                prepareChunk(chunk);
                                return chunk;
                        }

                        // 2) Miss — generate with the vanilla overworld generator and
                        //    persist immediately. storeChunk() skips nonexistent chunks
                        //    and writes through the shared Anvil manager.
                        // NOTE: no base prepareChunk() here on purpose — it would set
                        // TerrainPopulated=true and permanently skip decoration. The
                        // generator already assigns the world pointer + skylight map;
                        // ServerWorld::populateChunk decorates and re-saves afterwards.
                        chunk = m_generator->provideChunk(x, z);
                        if (chunk && !chunk->isNonexistent())
                        {
                                storeChunk(chunk);
                        }
                        return chunk;
                }

        private:
                std::shared_ptr<ChunkProviderGenerate> m_generator;
        };
}
