#include "ChunkCache.h"
#include "Chunk/Chunk.h"

namespace BLOCKMAN
{
	namespace CHUNK_CACHE_DETAIL
	{
		bool ChunkCache::inCache(int x, int z) const
		{
			// checking m_nearbyChunkCache is redundent. but m_nearbyChunkCache is much faster, so check m_nearbyChunkCache first
			if (m_nearbyChunkCache.inCache(x, z))
			{
				return true;
			}
			auto iter = m_posToChunk.find({ x, z });
			return iter != m_posToChunk.end() && iter->second.use_count() > 0;
		}

		ChunkPtr ChunkCache::get(int x, int z)
		{
			// checking m_nearbyChunkCache is redundent. but m_nearbyChunkCache is much faster, so check m_nearbyChunkCache first
			if (m_nearbyChunkCache.inCache(x, z))
			{
				return m_nearbyChunkCache.get(x, z);
			}

			auto iter = m_posToChunk.find({ x, z });
			if (iter == m_posToChunk.end()) {
				return nullptr;
			}

			if (auto chunk = iter->second.lock())
			{
				return chunk;
			}
			return nullptr;
		}

		void ChunkCache::add(const ChunkPtr & chunk)
		{
			if (m_nearbyChunkCache.inRange(chunk->m_posX, chunk->m_posZ))
			{
				m_nearbyChunkCache.add(chunk->m_posX, chunk->m_posZ, chunk);
			}
			m_recentChunkCache[{chunk->m_posX, chunk->m_posZ}] = chunk;
			auto& weakPtr = m_posToChunk[{chunk->m_posX, chunk->m_posZ}];
			if (weakPtr.use_count() > 0)
			{
				LordLogWarning("Chunk at %d,%d is loaded again while there already exists a copy of this chunk in the memory", chunk->m_posX, chunk->m_posZ);
			}
			weakPtr = chunk;
		}

		void ChunkCache::vacuum()
		{
			for (auto iter = m_posToChunk.begin(); iter != m_posToChunk.end(); ++iter)
			{
				if (iter->second.use_count() == 0)
				{
					m_posToChunk.erase(iter);
				}
			}
			m_posToChunk.resize(0);
		}

		ChunkCacheIterator::ChunkCacheIterator(const ChunkHashMap::const_iterator & iter, const ChunkHashMap::const_iterator & end)
			: m_iter(iter)
			, m_end(end)
		{
			while (m_iter != m_end)
			{
				m_value = m_iter->second.lock();
				if (m_value)
				{
					break;
				}
				++m_iter;
			}
		}

		ChunkCacheIterator & ChunkCacheIterator::operator++()
		{
			do
			{
				++m_iter;
				if (m_iter == m_end)
				{
					break;
				}
				m_value = m_iter->second.lock();
			} while (!m_value);
			return *this;
		}
	}
}
