#include "WorldChunkManager.h"
#include "World.h"

#include "WorldGenerator/BiomeCache.h"
#include "WorldGenerator/BiomeGen.h"
#include "WorldGenerator/GenLayer.h"

#include <algorithm>

namespace BLOCKMAN
{

void WorldChunkManager::initialize()
{
	biomeCache = LordNew BiomeCache(this);
	biomesToSpawnIn.push_back(BiomeGenBase::forest);
	biomesToSpawnIn.push_back(BiomeGenBase::plains);
	biomesToSpawnIn.push_back(BiomeGenBase::taiga);
	biomesToSpawnIn.push_back(BiomeGenBase::taigaHills);
	biomesToSpawnIn.push_back(BiomeGenBase::forestHills);
	biomesToSpawnIn.push_back(BiomeGenBase::jungle);
	biomesToSpawnIn.push_back(BiomeGenBase::jungleHills);

	genBiomes = NULL;
	biomeIndexLayer = NULL;
}

void WorldChunkManager::initialize(i64 seed, TERRAIN_TYPE terrainType)
{
	initialize();

	/*
		pay attention!
		deadloop key line in GenLayer::initializeAllBiomeGenerators()
		genLayer_magnify	= GenLayerZoom::magnify(1000LL, genLayer_riverInit, c + 2);

		by wanglei
	*/

#if (LORD_PLATFORM != LORD_PLATFORM_LINUX)
	GenLayerArr genLayerArr = GenLayer::initializeAllBiomeGenerators(seed, terrainType);
	genBiomes = genLayerArr[0];
	biomeIndexLayer = genLayerArr[1];
#endif
}

WorldChunkManager::WorldChunkManager()
{
	initialize();
}

WorldChunkManager::WorldChunkManager(i64 seed, TERRAIN_TYPE terrainType)
{
	initialize(seed, terrainType);
}

WorldChunkManager::~WorldChunkManager()
{
	biomesToSpawnIn.clear();
	LordSafeDelete(biomeIndexLayer);
	//LordSafeDelete(genBiomes);
	LordSafeDelete(biomeCache); 
}

WorldChunkManager::WorldChunkManager(World* pWorld)
{
	initialize(pWorld->getSeed(), pWorld->getWorldInfo().getTerrainType());
}

BiomeGenBase* WorldChunkManager::getBiomeGenAt(int x, int z)
{
	return biomeCache->getBiomeGenAt(x, z);
}

float* WorldChunkManager::getRainfall(float* arrToReuse, int arrLen, int x, int z, int width, int length)
{
	// IntCache.resetIntCache();
	if (arrToReuse == NULL || arrLen < width * length)
	{
		arrToReuse = (float*)LordMalloc(sizeof(float)*width*length);
		memset(arrToReuse, 0, sizeof(float)*width*length);
	}

	int* pBiomeIndexArr = biomeIndexLayer->getInts(x, z, width, length);

	for (int i = 0; i < width * length; ++i)
	{
		float rainfall = (float)BiomeGenBase::biomeList[pBiomeIndexArr[i]]->getIntRainfall() / 65536.0F;

		if (rainfall > 1.0F)
		{
			rainfall = 1.0F;
		}

		arrToReuse[i] = rainfall;
	}

	LordSafeFree(pBiomeIndexArr);

	return arrToReuse;
}

float* WorldChunkManager::getTemperatures(float* arrToReuse, int arrLen, int x, int y, int width, int length)
{
	// IntCache.resetIntCache();

	if (arrToReuse == NULL || arrLen < width * length)
	{
		arrToReuse = (float*)LordMalloc(sizeof(float) * width * length);
		memset(arrToReuse, 0, sizeof(float)*width*length);
	}

	int* pBiomeIndexArr = biomeIndexLayer->getInts(x, y, width, length);

	for (int i = 0; i < width * length; ++i)
	{
		float temperature = (float)BiomeGenBase::biomeList[pBiomeIndexArr[i]]->getIntTemperature() / 65536.0F;

		if (temperature > 1.0F)
		{
			temperature = 1.0F;
		}

		arrToReuse[i] = temperature;
	}
	LordSafeFree(pBiomeIndexArr);

	return arrToReuse;
}

BiomeGenBase** WorldChunkManager::getBiomesForGeneration(BiomeGenBase** arr, int arrLen, int x, int z, int width, int length)
{
	// IntCache.resetIntCache();

	if (arr == NULL || arrLen < width * length)
	{
		arr = (BiomeGenBase**)LordMalloc(sizeof(BiomeGenBase*) * length * width);
		memset(arr, 0, sizeof(BiomeGenBase*)*width*length);
	}

	int* pBiomeIndexArr = genBiomes->getInts(x, z, width, length);

	for (int i = 0; i < width * length; ++i)
	{
		arr[i] = BiomeGenBase::biomeList[pBiomeIndexArr[i]];
	}

	LordSafeFree(pBiomeIndexArr);

	return arr;
}

BiomeGenBase** WorldChunkManager::loadBlockGeneratorData(BiomeGenBase** arr, int arrLen, int x, int z, int width, int length)
{
	return getBiomeGenAt(arr, arrLen, x, z, width, length, true);
}

BiomeGenBase** WorldChunkManager::getBiomeGenAt(BiomeGenBase** arr, int arrLen, int x, int z, int width, int length, bool cache)
{
	//IntCache.resetIntCache();

	if (arr == NULL || arrLen < width * length)
	{
		LordSafeFree(arr);
		arr = (BiomeGenBase**)LordMalloc(sizeof(BiomeGenBase*) * width * length);
		memset(arr, 0, sizeof(BiomeGenBase*)*width*length);
	}

	if (cache && width == 16 && length == 16 && (x & 15) == 0 && (z & 15) == 0)
	{
		static int count = 0;
		count++;
		BiomeGenArr& biomeGenarr = biomeCache->getCachedBiomes(x, z);
		memcpy(arr, &(biomeGenarr[0]), sizeof(BiomeGenBase*) * width * length);
		return arr;
	}
	else
	{
		int* pBiomeIndexArr = biomeIndexLayer->getInts(x, z, width, length);

		for (int i = 0; i < width * length; ++i)
		{
			arr[i] = BiomeGenBase::biomeList[pBiomeIndexArr[i]];
		}
		LordSafeFree(pBiomeIndexArr);

		return arr;
	}
}

bool WorldChunkManager::areBiomesViable(int x, int z, int range, const BiomeGenArr& lst)
{
	// IntCache.resetIntCache();
	int beginX = (x - range) >> 2;
	int beginZ = (z - range) >> 2;
	int endX = (x + range) >> 2;
	int endZ = (z + range) >> 2;
	int width = endX - beginX + 1;
	int length = endZ - beginZ + 1;
	int* pBiomeIndexArr = genBiomes->getInts(beginX, beginZ, width, length);

	for (int i = 0; i < width * length; ++i)
	{
		BiomeGenBase* biome = BiomeGenBase::biomeList[pBiomeIndexArr[i]];

		if (std::find(lst.begin(), lst.end(), biome) != lst.end())
		{
			LordSafeFree(pBiomeIndexArr);
			return false;
		}
	}

	LordSafeFree(pBiomeIndexArr);
	return true;
}

ChunkPosition WorldChunkManager::findBiomePosition(int x, int z, int range, const BiomeGenArr& lst, Random& rand)
{
	// IntCache.resetIntCache();
	int beginX = (x - range) >> 2;
	int beginZ = (z - range) >> 2;
	int endX = (x + range) >> 2;
	int endZ = (z + range) >> 2;
	int width = endX - beginX + 1;
	int length = endZ - beginZ + 1;
	int* pBiomeIndexArr = genBiomes->getInts(beginX, beginZ, width, length);
	ChunkPosition position;
	bool found = false;
	int bound = 0;

	for (int i = 0; i < width * length; ++i)
	{
		int ix = (beginX + i % width) << 2;
		int iz = (beginZ + i / width) << 2;
		BiomeGenBase* biome = BiomeGenBase::biomeList[pBiomeIndexArr[i]];

		if (std::find(lst.begin(), lst.end(), biome) != lst.end() &&
			(found == false || rand.nextInt(bound + 1) == 0))
		{
			position = ChunkPosition(ix, 0, iz);
			found = true;
			++bound;
		}
	}
	LordSafeFree(pBiomeIndexArr);

	return position;
}

void WorldChunkManager::cleanupCache()
{
	biomeCache->cleanupCache();
}

WorldChunkManagerHell::WorldChunkManagerHell(BiomeGenBase* biomeGenBase, float temperature, float rainfall)
{
	biomeGenerator = biomeGenBase;
	hellTemperature = temperature;
	this->rainfall = rainfall;
}

WorldChunkManagerHell::~WorldChunkManagerHell()
{

}

BiomeGenBase* WorldChunkManagerHell::getBiomeGenAt(int x, int z)
{
	return biomeGenerator;
}

BiomeGenBase** WorldChunkManagerHell::getBiomesForGeneration(BiomeGenBase** arr, int len, int x, int z, int width, int length)
{
	if (arr == NULL || len < width * length)
	{
		arr = (BiomeGenBase**)LordMalloc(sizeof(BiomeGenBase*) * width * length);
		memset(arr, 0, sizeof(BiomeGenBase*)*width*length);
	}

	for (int i = 0; i < width*length; ++i)
	{
		arr[i] = biomeGenerator;
	}
	return arr;
}

float* WorldChunkManagerHell::getTemperatures(float* arr, int len, int x, int z, int width, int length)
{
	if (arr == NULL || len < width * length)
	{
		arr = (float*)LordMalloc(sizeof(float) * width * length);
		memset(arr, 0, sizeof(float)*width*length);
	}

	for (int i = 0; i < width*length; ++i)
		arr[i] = hellTemperature;

	return arr;
}

float* WorldChunkManagerHell::getRainfall(float* arr, int len, int x, int z, int width, int length)
{
	if (arr == NULL || len < width * length)
	{
		arr = (float*)LordMalloc(sizeof(float) * width * length);
		memset(arr, 0, sizeof(float)*width*length);
	}

	for (int i = 0; i < width*length; ++i)
		arr[i] = rainfall;
	return arr;
}

BiomeGenBase** WorldChunkManagerHell::loadBlockGeneratorData(BiomeGenBase** arr, int len, int x, int z, int width, int length)
{
	if (arr == NULL || len < width * length)
	{
		arr = (BiomeGenBase**)LordMalloc(sizeof(BiomeGenBase*) * width * length);
		memset(arr, 0, sizeof(BiomeGenBase*)*width*length);
	}

	for (int i = 0; i < width* length; ++i)
		arr[i] = biomeGenerator;
	return arr;
}

BiomeGenBase** WorldChunkManagerHell::getBiomeGenAt(BiomeGenBase** arr, int len, int x, int z, int width, int length, bool cacheFlag)
{
	return loadBlockGeneratorData(arr, len, x, z, width, length);
}

ChunkPosition WorldChunkManagerHell::findBiomePosition(int x, int z, int range, const BiomeGenArr& lst, Random& rand)
{
	ChunkPosition result;
	if (std::find(lst.begin(), lst.end(), biomeGenerator) != lst.end())
	{
		result = ChunkPosition(x - range + rand.nextInt(range * 2 + 1), 0, z - range + rand.nextInt(range * 2 + 1));
	}
	return result;
}

bool WorldChunkManagerHell::areBiomesViable(int x, int z, int range, const BiomeGenArr& lst)
{
	if (std::find(lst.begin(), lst.end(), biomeGenerator) != lst.end())
	{
		return true;
	}
	return false;
}

}