#include "ChunkService.h"
#include "Chunk/IChunkProvider.h"
#include "Chunk/IChunkStorage.h"
#include "Chunk/Chunk.h"
#include "World/World.h"

namespace BLOCKMAN
{
	ChunkPtr ChunkService::getChunk(int x, int z)
	{
		ChunkPtr chunk = m_cache.get(x, z);
		if (chunk)
		{
			return chunk;
		}
		auto iter = m_loadingTaskFutures.find({ x,z });
		if (iter == m_loadingTaskFutures.end()
			|| iter->second.cancel()
			|| iter->second.isCanceled())
		{
			chunk = m_chunkProvider->provideChunk(x, z);
			LordAssert(chunk);
			// Server-authoritative worldgen: if the local provider
			// couldn't find the chunk (disk miss → NonexistentChunk),
			// fire the onChunkMiss hook. On the client this sends
			// C2SRequestChunk; on the server it's a no-op (the server
			// IS the authority, so a miss means "doesn't exist").
			if (chunk->isNonexistent())
			{
				onChunkMiss(x, z);
			}
			m_cache.add(chunk);
			prepareChunk(chunk);
		}
		else
		{
			chunk = iter->second.get();
			LordAssert(chunk);
		}
		return chunk;
	}

	// TODO: task cannot be canceled
	Future<ChunkPtr> ChunkService::getChunkAsync(int x, int z)
	{
		if (!m_supportAsync) {
			LordLogError("async unsupported but call getChunkAsync, check it", x, z);
			auto promise = LORD::make_shared<Promise<ChunkPtr>>();
			return promise->getFuture();
		}

		auto promise = LORD::make_shared<Promise<ChunkPtr>>();
		promise->changeToRunning();
		auto iter = m_loadingTaskFutures.find({ x, z });
		if (iter != m_loadingTaskFutures.end())
		{
			LordLogWarning("async get chunk task for %d,%d is already in queue.", x, z);
			// TODO:
			promise->setValue(getChunk(x, z));
		}
		else if (auto chunk = m_cache.get(x, z))
		{
			promise->setValue(chunk);
		}
		else
		{
			auto future = m_workerThread.addTask(&IChunkProvider::provideChunk, m_chunkProvider, x, z);
			future.onFinish([=] (const ChunkPtr& chunk) {
				m_cache.add(chunk);
				m_loadingTaskFutures.erase({ x, z });
				prepareChunk(chunk);
				promise->setValue(chunk);
			}).onFail([=](const std::exception_ptr& eptr) {
				try
				{
					std::rethrow_exception(eptr);
				}
				catch (std::exception& e)
				{
					LordLogError("Exception occured while loading chunk: %s", e.what());
				}
				catch (...)
				{
					LordLogError("Exception occured while loading chunk");
				}
				m_loadingTaskFutures.erase({ x, z });
				promise->setException(eptr);
			}).onCancel([=]() {
				m_loadingTaskFutures.erase({ x, z });
				promise->cancel();
			});
			m_loadingTaskFutures[{x, z}] = std::move(future);
		}
		return promise->getFuture();
	}

	void ChunkService::saveChunk(int x, int z, bool is_flush)
	{
		if (auto chunk = m_cache.get(x, z))
		{
			m_chunkStorage->storeChunk(chunk);
			if (is_flush)
			{
				m_chunkStorage->flushStorage();
			}	
		}
	}

	void ChunkService::saveAllChunks(bool is_flush)
	{
		for (const auto& chunk : m_cache)
		{
			m_chunkStorage->storeChunk(chunk);
		}
		if (is_flush)
		{
			m_chunkStorage->flushStorage();
		}
	}

	void ChunkService::prepareChunk(const ChunkPtr & chunk)
	{
		for (const auto& pair : *m_world->getLoadedEntityMap())
		{
			Entity* entity = pair.second;
			if (int(Math::Floor(entity->position.x / 16.0f)) == chunk->m_posX
				&& int(Math::Floor(entity->position.z / 16.0f)) == chunk->m_posZ)
			{
				if (entity->addedToChunk && chunkInCache(entity->chunkCoord.x, entity->chunkCoord.z))
				{
					m_world->getChunkFromChunkCoords(entity->chunkCoord.x, entity->chunkCoord.z)->removeEntityAtIndex(entity, entity->chunkCoord.y);
				}
				chunk->addEntity(entity);
			}
		}

		for (auto& pair : chunk->m_tileEntityMap)
		{
			m_world->addTileEntity(pair.second);
		}
	}
}