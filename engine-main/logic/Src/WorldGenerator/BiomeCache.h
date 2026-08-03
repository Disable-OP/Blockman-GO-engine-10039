/********************************************************************
filename: 	BiomeCache.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-11-14
*********************************************************************/
#ifndef __BIOME_CACHE_HEADER__
#define __BIOME_CACHE_HEADER__

#include "BM_Container_def.h"

namespace BLOCKMAN
{

class WorldChunkManager;
class BiomeCache;
class BiomeGenBase;

class BiomeCacheBlock : public ObjectAlloc
{
public:
	/** An array of chunk temperatures saved by this cache. */
	float* temperatureValues = nullptr;

	/** An array of chunk rainfall values saved by this cache. */
	float* rainfallValues = nullptr;

	/** The array of biome types stored in this BiomeCacheBlock. */
	BiomeGenArr biomes;

	/** The x coordinate of the BiomeCacheBlock. */
	int xPosition = 0;

	/** The z coordinate of the BiomeCacheBlock. */
	int zPosition = 0;

	/** The last time this BiomeCacheBlock was accessed, in milliseconds. */
	i64 lastAccessTime = 0;

	/** The BiomeCache object that contains this BiomeCacheBlock */
	BiomeCache* theBiomeCache = nullptr;

	BiomeCacheBlock(BiomeCache* pBiomeCache, int x, int z);
	~BiomeCacheBlock();

	/** Returns the BiomeGenBase related to the x, z position from the cache block. */
	BiomeGenBase* getBiomeGenAt(int x, int z);
};

class BiomeCache : public ObjectAlloc
{
public:
	typedef map<i64, BiomeCacheBlock*>::type BiomeCacheBlockMap;

protected:
	/** Reference to the WorldChunkManager */
	WorldChunkManager* m_chunkManager = nullptr;

	/** The last time this BiomeCache was cleaned, in milliseconds. */
	i64 m_lastCleanupTime = 0;

	/** The map of keys to BiomeCacheBlocks. Keys are based on the chunk x, z coordinates as (x | z << 32). */
	BiomeCacheBlockMap m_cacheMap;

public:
	BiomeCache(WorldChunkManager* pWorldChunkManager);
	~BiomeCache();

	/** Returns a biome cache block at location specified.*/
	BiomeCacheBlock* getBiomeCacheBlock(int x, int z);

	/** Returns the BiomeGenBase related to the x, z position from the cache. */
	BiomeGenBase* getBiomeGenAt(int x, int z);

	/** Removes BiomeCacheBlocks from this cache that haven't been accessed in at least 30 seconds. */
	void cleanupCache();

	/** Returns the array of cached biome types in the BiomeCacheBlock at the given location. */
	BiomeGenArr& getCachedBiomes(int x, int z);

	/** Get the world chunk manager object for a biome list. */
	static WorldChunkManager* getChunkManager(BiomeCache* pBiomeCache);
};

}

#endif