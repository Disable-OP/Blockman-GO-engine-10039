#ifndef __RANGE_CHUNK_CACHE_HEADER__
#define __RANGE_CHUNK_CACHE_HEADER__

#include <memory>

namespace BLOCKMAN
{
	class Chunk;
	using ChunkPtr = std::shared_ptr<Chunk>;

	class RangeChunkCache
	{
	private:
		size_t m_range = 0;
		size_t m_rangeBitWitdh = 0;
		int m_centerX = 0;
		int m_centerZ = 0;
		ChunkPtr* m_cache = nullptr;

		ChunkPtr& at(int x, int z)
		{
			x &= m_range - 1;
			z &= m_range - 1;
			return m_cache[z << m_rangeBitWitdh | x];
		}

		const ChunkPtr& at(int x, int z) const
		{
			x &= m_range - 1;
			z &= m_range - 1;
			return m_cache[z << m_rangeBitWitdh | x];
		}

	public:
		using iterator = ChunkPtr*;
		using const_iterator = const ChunkPtr*;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;

		RangeChunkCache(size_t rangeAtLeast)
			: m_centerX(0)
			, m_centerZ(0)
			, m_rangeBitWitdh(0)
			, m_range(1)
		{
			while (m_range < rangeAtLeast)
			{
				++m_rangeBitWitdh;
				m_range <<= 1;
			}
			m_cache = new ChunkPtr[m_range * m_range]{};
		}

		~RangeChunkCache()
		{
			delete[] m_cache;
		}

		bool inRange(int x, int z) const
		{
			int halfRange = m_range >> 1;
			return x - m_centerX < halfRange && x - m_centerX >= -halfRange 
				&& z - m_centerZ < halfRange && z - m_centerZ >= -halfRange;
		}

		bool inCache(int x, int z) const
		{
			return inRange(x, z) && at(x, z);
		}

		void setCenter(int newCenterX, int newCenterZ)
		{
			if (m_centerX == newCenterX && m_centerZ == newCenterZ)
			{
				return;
			}
			int halfRange = m_range / 2;
			for (int x = m_centerX - halfRange; x < m_centerX + halfRange; ++x)
			{
				for (int z = m_centerZ - halfRange; z < m_centerZ + halfRange; ++z)
				{
					if (x - newCenterX < halfRange && x - newCenterX >= -halfRange
						&& z - newCenterZ < halfRange && z - newCenterZ >= -halfRange)
					{
						continue;
					}
					at(x, z) = nullptr;
				}
			}
			m_centerX = newCenterX;
			m_centerZ = newCenterZ;
		}

		ChunkPtr get(int x, int z)
		{
			return at(x, z);
		}

		void add(int x, int z, ChunkPtr chunk)
		{
			at(x, z) = chunk;
		}

		void clear()
		{
			delete[] m_cache;
			m_cache = new ChunkPtr[m_range * m_range]{};
		}
	};
}

#endif // !__RANGE_CHUNK_CACHE_HEADER__
