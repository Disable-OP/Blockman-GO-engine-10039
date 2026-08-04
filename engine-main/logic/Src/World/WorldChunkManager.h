/********************************************************************
filename: 	WorldChunkManager.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-11-11
*********************************************************************/
#ifndef __WORLD_CHUNK_MANAGER_HEADER__
#define __WORLD_CHUNK_MANAGER_HEADER__

#include "BM_TypeDef.h"
#include "BM_Container_def.h"

#include "Util/ChunkPosition.h"

namespace BLOCKMAN
{

class GenLayer;
class BiomeCache;
class BiomeGenBase;
class World;
class Random;

class WorldChunkManager : public ObjectAlloc
{
protected:
	GenLayer* genBiomes = nullptr;

	/** A GenLayer containing the indices into BiomeGenBase.biomeList[] */
	GenLayer* biomeIndexLayer = nullptr;

	/** The BiomeCache object for this world. */
	BiomeCache* biomeCache = nullptr;

	/** A list of biomes that the player can spawn in. */
	BiomeGenArr biomesToSpawnIn;

	WorldChunkManager();

	void initialize();
	void initialize(i64 seed, TERRAIN_TYPE terrainType);

public:
	WorldChunkManager(i64 seed, TERRAIN_TYPE terrainType);
	WorldChunkManager(World* pWorld);
	virtual ~WorldChunkManager();

	/** Gets the list of valid biomes for the player to spawn in. */
	BiomeGenArr& getBiomesToSpawnIn() { return biomesToSpawnIn;	}

	/** Returns the BiomeGenBase related to the x, z position on the world. */
	virtual BiomeGenBase* getBiomeGenAt(int x, int z);

	/** Returns a list of rainfall values for the specified blocks. Args: listToReuse, x, z, width, length.	*/
	virtual float* getRainfall(float* arrToReuse, int arrLen, int x, int z, int width, int length);

	/** Return an adjusted version of a given temperature based on the y height */
	float getTemperatureAtHeight(float temperature, int y) { return temperature; }

	/** Returns a list of temperatures to use for the specified blocks.  Args: listToReuse, x, y, width, length */
	virtual float* getTemperatures(float* arrToReuse, int arrLen, int x, int z, int width, int length);

	/** Returns an array of biomes for the location input. */
	virtual BiomeGenBase** getBiomesForGeneration(BiomeGenBase** arr, int arrLen, int x, int z, int width, int length);

	/** Returns biomes to use for the blocks and loads the other data like temperature and humidity onto the
	* WorldChunkManager Args: oldBiomeList, x, z, width, depthm */
	virtual BiomeGenBase** loadBlockGeneratorData(BiomeGenBase** arr, int arrLen, int x, int y, int width, int depth);

	/** Return a list of biomes for the specified blocks. Args: listToReuse, x, y, width, length, cacheFlag (if false,
	* don't check biomeCache to avoid infinite loop in BiomeCacheBlock) */
	virtual BiomeGenBase** getBiomeGenAt(BiomeGenBase** arr, int arrLen, int x, int y, int width, int length, bool cache);

	/** checks given Chunk's Biomes against List of allowed ones */
	virtual bool areBiomesViable(int x, int z, int range, const BiomeGenArr& lst);

	/** Finds a valid position within a range, that is in one of the listed biomes. Searches {x,z} +-range blocks.
	* Strongly favors positive y positions.	*/
	virtual ChunkPosition findBiomePosition(int x, int z, int range, const BiomeGenArr& lst, Random& rand);

	/** Calls the WorldChunkManager's biomeCache.cleanupCache() */
	void cleanupCache();
};

class WorldChunkManagerHell : public WorldChunkManager
{
protected:
	/** The biome generator object. */
	BiomeGenBase* biomeGenerator = nullptr;
	float hellTemperature = 0.f;

	/** The rainfall in the world */
	float rainfall = 0.f;

public:
	WorldChunkManagerHell(BiomeGenBase* biomeGenBase, float temperature, float rainfall);
	~WorldChunkManagerHell();

	/** Returns the BiomeGenBase related to the x, z position on the world.	*/
	BiomeGenBase* getBiomeGenAt(int x, int z);

	/** Returns an array of biomes for the location input.*/
	BiomeGenBase** getBiomesForGeneration(BiomeGenBase** arr, int len, int x, int z, int width, int length);

	/** Returns a list of temperatures to use for the specified blocks.  Args: listToReuse, x, y, width, length	*/
	float* getTemperatures(float* arr, int len, int x, int z, int width, int length);

	/** Returns a list of rainfall values for the specified blocks. Args: listToReuse, x, z, width, length.	*/
	float* getRainfall(float* arr, int len, int x, int z, int width, int length);

	/** Returns biomes to use for the blocks and loads the other data like temperature and humidity onto the
	* WorldChunkManager Args: oldBiomeList, x, z, width, depth */
	BiomeGenBase** loadBlockGeneratorData(BiomeGenBase** arr, int len, int x, int z, int width, int length);

	/** Return a list of biomes for the specified blocks. Args: listToReuse, x, y, width, length, cacheFlag (if false,
	* don't check biomeCache to avoid infinite loop in BiomeCacheBlock)	*/
	BiomeGenBase** getBiomeGenAt(BiomeGenBase** arr, int len, int x, int z, int width, int length, bool cacheFlag);

	/** Finds a valid position within a range, that is in one of the listed biomes. Searches {x,z} +-range blocks.
	* Strongly favors positive y positions.	*/
	ChunkPosition findBiomePosition(int x, int z, int range, const BiomeGenArr& lst, Random& rand);

	/** checks given Chunk's Biomes against List of allowed ones */
	bool areBiomesViable(int x, int z, int range, const BiomeGenArr& lst);
};

}

#endif
