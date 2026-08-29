#include "ChunkReadableStorageFile.h"
#include "WorldGenerator/AnvilManager.h"

namespace BLOCKMAN
{
        ChunkPtr ChunkReadableStorageFile::provideChunk(int x, int z)
        {
                auto chunk = readChunkFromAnvil(x, z);
                prepareChunk(chunk);
                return chunk;
        }

        void ChunkReadableStorageFile::storeChunk(const ChunkPtr & chunk)
        {
                if (!chunk || chunk->isNonexistent())
                {
                        return;
                }
                const int anvilX = chunkCoordToAnvilCoord(chunk->m_posX);
                const int anvilZ = chunkCoordToAnvilCoord(chunk->m_posZ);
                auto anvil = m_anvilManager->getAnvil(anvilX, anvilZ);
                if (!anvil)
                {
                        // Region file does not exist yet (first boot of a new
                        // world). getAnvil() only LOADS existing files — it
                        // never creates one — so on a fresh world it returns
                        // null here. Create the region file on demand.
                        // (Without this, `anvil->placeChunk()` below locked a
                        // mutex at nullptr+0x8028 and SIGSEGV'd the whole app
                        // during Server::init on every first launch.)
                        anvil = m_anvilManager->createNewAnvilFile(anvilX, anvilZ);
                }
                if (anvil && anvil->placeChunk(chunk.get()))
                {
                        return;
                }
                LordLogError("Failed to store chunk %d,%d", chunk->m_posX, chunk->m_posZ);
        }

        void ChunkReadableStorageFile::flushStorage()
        {
                m_anvilManager->saveAllAnvilsInCache();
        }

        ChunkPtr ChunkReadableStorageFile::readChunkFromAnvil(int x, int z)
        {
                auto anvil = m_anvilManager->getAnvil(chunkCoordToAnvilCoord(x), chunkCoordToAnvilCoord(z));
                if (!anvil)
                {
                        return LORD::make_shared<NonexistentChunk>(x, z);
                }
                ChunkPtr chunk(anvil->extractChunk(x, z));
                if (!chunk)
                {
                        return LORD::make_shared<NonexistentChunk>(x, z);
                }
                return chunk;
        }

        void ChunkReadableStorageFile::prepareChunk(const ChunkPtr & chunk)
        {
                chunk->m_pWorld = m_world;
                chunk->generateSkylightMap();
                for (auto section : chunk->m_pSections)
                {
                        if (section)
                        {
                                section->removeInvalidBlocks();
                        }
                }
                chunk->onChunkLoad();
                chunk->m_isTerrainPopulated = true;
                for (auto& pair : chunk->m_tileEntityMap)
                {
                        pair.second->setWorldObj(m_world);
                }
        }
}