#include "BiomeCache.h"

#include "World/WorldChunkManager.h"

namespace BLOCKMAN
{
BiomeCacheBlock::BiomeCacheBlock(BiomeCache* pBiomeCache, int x, int z)
{
	theBiomeCache = pBiomeCache;
	temperatureValues = (float*)LordMalloc(sizeof(float) * 256);
	memset(temperatureValues, 0, sizeof(float) * 256);
	rainfallValues = (float*)LordMalloc(sizeof(float) * 256);
	memset(rainfallValues, 0, sizeof(float) * 256);
	biomes.resize(256, NULL);
	lastAccessTime = 0;
	xPosition = x;
	zPosition = z;
	BiomeCache::getChunkManager(pBiomeCache)->getTemperatures(temperatureValues, 256, x << 4, z << 4, 16, 16);
	BiomeCache::getChunkManager(pBiomeCache)->getRainfall(rainfallValues, 256, x << 4, z << 4, 16, 16);
	BiomeCache::getChunkManager(pBiomeCache)->getBiomeGenAt(&biomes[0], 256, x << 4, z << 4, 16, 16, false);
}

BiomeCacheBlock::~BiomeCacheBlock()
{
	LordSafeFree(temperatureValues);
	LordSafeFree(rainfallValues);
}

BiomeGenBase* BiomeCacheBlock::getBiomeGenAt(int x, int z)
{
	LordAssert((x & 15 | (z & 15) << 4) < 256);
	return biomes[x & 15 | (z & 15) << 4];
}

BiomeCache::BiomeCache(WorldChunkManager* pWorldChunkManager)
{
	m_chunkManager = pWorldChunkManager;
}

BiomeCache::~BiomeCache()
{
	BiomeCacheBlock* pBiome = NULL;
	for (BiomeCacheBlockMap::iterator it = m_cacheMap.begin(); it!=m_cacheMap.end(); ++it)
	{
		pBiome = it->second;
		if (!pBiome)
			continue;
		LordDelete(pBiome);
	}
	m_cacheMap.clear();
}

BiomeCacheBlock* BiomeCache::getBiomeCacheBlock(int x, int z)
{
	x >>= 4;
	z >>= 4;
	i64 var3 = (i64)x & 0xFFFFFFFFLL | ((i64)z & 0xFFFFFFFFLL) << 32;

	BiomeCacheBlock* pResult = NULL;
	BiomeCacheBlockMap::iterator it = m_cacheMap.find(var3);
	if (it == m_cacheMap.end())
	{
		pResult = LordNew BiomeCacheBlock(this, x, z);
		m_cacheMap.insert(std::make_pair(var3, pResult));
	}
	else
		pResult = it->second;
	
	return pResult;
}

BiomeGenBase* BiomeCache::getBiomeGenAt(int x, int z)
{
	return getBiomeCacheBlock(x, z)->getBiomeGenAt(x, z);
}

void BiomeCache::cleanupCache()
{
	i64 currentTime = 0;
	i64 deltaTime = currentTime - m_lastCleanupTime;

	if (deltaTime > 7500L || deltaTime < 0L)
	{
		m_lastCleanupTime = currentTime;

		for (BiomeCacheBlockMap::iterator it = m_cacheMap.begin(); it != m_cacheMap.end(); )
		{
			BiomeCacheBlock* pBlock = it->second;
			i64 idel_time = currentTime - pBlock->lastAccessTime;
			if (idel_time > 30000LL || idel_time < 0LL)
			{
				BiomeCacheBlockMap::iterator jt = it;
				++it;
				m_cacheMap.erase(it);
				LordSafeDelete(pBlock);
			}
			else
				++it;
		}
	}
}

BiomeGenArr& BiomeCache::getCachedBiomes(int x, int z)
{
	return getBiomeCacheBlock(x, z)->biomes;
}

WorldChunkManager* BiomeCache::getChunkManager(BiomeCache* pBiomeCache)
{
	return pBiomeCache->m_chunkManager;
}

}
