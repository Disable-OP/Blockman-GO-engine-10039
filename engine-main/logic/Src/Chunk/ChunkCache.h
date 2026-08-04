#pragma once

#include "Core.h"
#include "Chunk/RangeChunkCache.h"
#include "sparsehash/dense_hash_map"
#include "Util/FIFOCache.h"
#include "Std/iterator.h"

namespace BLOCKMAN
{
	class Chunk;
	using ChunkPtr = std::shared_ptr<Chunk>;

	namespace CHUNK_CACHE_DETAIL
	{
		using WeakChunkPtr = std::weak_ptr<Chunk>;

		static constexpr size_t log2i(size_t num)
		{
			return num < 2 ? 0 : log2i(num >> 1) + 1;
		}

		template<size_t range>
		struct CoordHash
		{
			static_assert(range > 0 && ((range & (range - 1)) == 0), "range parameter of CoordHash must be power of 2");

			static constexpr size_t mask = range - 1;
			static constexpr size_t shift = log2i(range);
			std::size_t operator()(const std::pair<int, int>& coord) const
			{
				return ((coord.second & mask) << shift) + (coord.first & mask);
			}
		};

		using ChunkHashMap = google::dense_hash_map<std::pair<int, int>, WeakChunkPtr, CoordHash<64>>;

		class ChunkCacheIterator
		{
			friend bool operator==(const ChunkCacheIterator& lhs, const ChunkCacheIterator& rhs)
			{
				return lhs.m_iter == rhs.m_iter;
			}
			friend bool operator!=(const ChunkCacheIterator& lhs, const ChunkCacheIterator& rhs)
			{
				return lhs.m_iter != rhs.m_iter;
			}
		public:
			using value_type = ChunkPtr;
			using difference_type = size_t;
			using reference = const ChunkPtr&;
			using pointer = const ChunkPtr*;
			using iterator_category = std::input_iterator_tag;

			ChunkCacheIterator() = default;
			ChunkCacheIterator(const ChunkCacheIterator&) = default;

			ChunkCacheIterator(const ChunkHashMap::const_iterator& iter, const ChunkHashMap::const_iterator& end);

			reference operator*() const
			{
				return m_value;
			}
			pointer operator->() const
			{
				return &m_value;
			}
			ChunkCacheIterator& operator++();
			ChunkCacheIterator operator++(int)
			{
				ChunkCacheIterator ret = *this;
				++(*this);
				return ret;
			}

		private:
			ChunkPtr m_value;
			ChunkHashMap::const_iterator m_iter;
			ChunkHashMap::const_iterator m_end;
		};

		class ChunkCache
		{
		public:
			using iterator = ChunkCacheIterator;
			using const_iterator = ChunkCacheIterator;
			using reverse_iterator = std::reverse_iterator<iterator>;
			using const_reverse_iterator = std::reverse_iterator<const_iterator>;

			ChunkCache(int range)
				: m_nearbyChunkCache(range)
			{
				m_posToChunk.set_empty_key({ (std::numeric_limits<int>::max)(), (std::numeric_limits<int>::max)() });
				m_posToChunk.set_deleted_key({ (std::numeric_limits<int>::min)(), (std::numeric_limits<int>::min)() });
			}

			bool inCache(int x, int z) const;
			ChunkPtr get(int x, int z);
			void add(const ChunkPtr& chunk);
			void vacuum();

			void setCenter(int x, int z)
			{
				m_nearbyChunkCache.setCenter(x, z);
			}

			void minimize()
			{
				m_nearbyChunkCache.clear();
				m_recentChunkCache.clear();
				vacuum();
			}

			iterator begin()
			{
				return iterator(m_posToChunk.begin(), m_posToChunk.end());
			}

			iterator end()
			{
				return iterator(m_posToChunk.end(), m_posToChunk.end());
			}

			const_iterator cbegin() const
			{
				return iterator(m_posToChunk.begin(), m_posToChunk.end());
			}

			const_iterator cend() const
			{
				return iterator(m_posToChunk.end(), m_posToChunk.end());
			}

		private:
			static constexpr size_t MAX_TEMP_CHUNK_BUFFER_SIZE = 256;
			RangeChunkCache m_nearbyChunkCache;
			FIFOCache<std::pair<int, int>, ChunkPtr> m_recentChunkCache{ MAX_TEMP_CHUNK_BUFFER_SIZE };
			ChunkHashMap m_posToChunk;
		};
	}

	using CHUNK_CACHE_DETAIL::ChunkCacheIterator;
	using CHUNK_CACHE_DETAIL::ChunkCache;
}