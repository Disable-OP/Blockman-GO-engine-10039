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
		auto anvil = m_anvilManager->getAnvil(chunkCoordToAnvilCoord(chunk->m_posX), chunkCoordToAnvilCoord(chunk->m_posZ));
		if (!anvil->placeChunk(chunk.get()))
		{
			LordLogError("Failed to store chunk %d,%d", chunk->m_posX, chunk->m_posZ);
		}
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