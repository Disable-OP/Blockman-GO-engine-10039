#include "GenLayer.h"
#include "BiomeGen.h"

namespace BLOCKMAN 
{

GenLayer::GenLayer(i64 seed)
{
	m_parent = NULL;
	m_chunkSeed = 0;
	m_genSeed = 0;
	m_baseSeed = seed;
	m_baseSeed *= m_baseSeed * 6364136223846793005LL + 1442695040888963407LL;
	m_baseSeed += seed;
	m_baseSeed *= m_baseSeed * 6364136223846793005LL + 1442695040888963407LL;
	m_baseSeed += seed;
	m_baseSeed *= m_baseSeed * 6364136223846793005LL + 1442695040888963407LL;
	m_baseSeed += seed;
	m_refCount = 0;
}

GenLayer::~GenLayer()
{
	if (m_parent)
	{
		--m_parent->m_refCount;
		if (m_parent->m_refCount == 0)
			LordDelete(m_parent);

		m_parent = NULL;
	}
}

void GenLayer::setParent(GenLayer* p)
{
	if (m_parent != NULL)
	{
		m_parent->m_refCount--;
	}

	m_parent = p;
	if (m_parent != NULL)
	{
		m_parent->m_refCount++;
	}
}

void GenLayer::initWorldGenSeed(i64 seed)
{
	m_genSeed = seed;

	if (m_parent != NULL)
	{
		m_parent->initWorldGenSeed(seed);
	}

	m_genSeed *= m_genSeed * 6364136223846793005LL + 1442695040888963407LL;
	m_genSeed += m_baseSeed;
	m_genSeed *= m_genSeed * 6364136223846793005LL + 1442695040888963407LL;
	m_genSeed += m_baseSeed;
	m_genSeed *= m_genSeed * 6364136223846793005LL + 1442695040888963407LL;
	m_genSeed += m_baseSeed;
}

void GenLayer::initChunkSeed(i64 seed1, i64 seed2)
{
	m_chunkSeed = m_genSeed;
	m_chunkSeed *= m_chunkSeed * 6364136223846793005LL + 1442695040888963407LL;
	m_chunkSeed += seed1;
	m_chunkSeed *= m_chunkSeed * 6364136223846793005LL + 1442695040888963407LL;
	m_chunkSeed += seed2;
	m_chunkSeed *= m_chunkSeed * 6364136223846793005LL + 1442695040888963407LL;
	m_chunkSeed += seed1;
	m_chunkSeed *= m_chunkSeed * 6364136223846793005LL + 1442695040888963407LL;
	m_chunkSeed += seed2;
}

int GenLayer::nextInt(int threshold)
{
	int result = (int)((m_chunkSeed >> 24) % (i64)threshold);

	if (result < 0)
	{
		result += threshold;
	}

	m_chunkSeed *= m_chunkSeed * 6364136223846793005LL + 1442695040888963407LL;
	m_chunkSeed += m_genSeed;
	return result;
}

GenLayerArr GenLayer::initializeAllBiomeGenerators(i64 seed, TERRAIN_TYPE type)
{
	GenLayerIsland*		genLyaer_island = LordNew GenLayerIsland(1LL);
	GenLayerFuzzyZoom*	genLayer_fuzzy = LordNew GenLayerFuzzyZoom(2000LL, genLyaer_island);
	GenLayerAddIsland*	genLayer_addIsland = LordNew GenLayerAddIsland(1LL, genLayer_fuzzy);
	GenLayerZoom*		genLayer_zoom = LordNew GenLayerZoom(2001LL, genLayer_addIsland);
	genLayer_addIsland	= LordNew GenLayerAddIsland(2LL, genLayer_zoom);
	GenLayerAddSnow*	genLayer_addSnow = LordNew GenLayerAddSnow(2LL, genLayer_addIsland);
	genLayer_zoom		= LordNew GenLayerZoom(2002LL, genLayer_addSnow);
	genLayer_addIsland	= LordNew GenLayerAddIsland(3LL, genLayer_zoom);
	genLayer_zoom		= LordNew GenLayerZoom(2003LL, genLayer_addIsland);
	genLayer_addIsland	= LordNew GenLayerAddIsland(4L, genLayer_zoom);
	GenLayerAddMushroomIsland* genLayer_addMushroomIsland = LordNew GenLayerAddMushroomIsland(5L, genLayer_addIsland);

	i8 c = 4;
	if (type == TERRAIN_TYPE_LARGE_BIOMES)
	{
		c = 6;
	}

	GenLayer*			genLayer_magnify = GenLayerZoom::magnify(1000LL, genLayer_addMushroomIsland, 0);
	GenLayerRiverInit*	genLayer_riverInit = LordNew GenLayerRiverInit(100LL, genLayer_magnify);
	genLayer_magnify	= GenLayerZoom::magnify(1000LL, genLayer_riverInit, c + 2);
	GenLayerRiver*		genLayer_river = LordNew GenLayerRiver(1LL, genLayer_magnify);
	GenLayerSmooth*		genLayer_smooth = LordNew GenLayerSmooth(1000LL, genLayer_river); // var16 is the last node.
	GenLayer*			genLayer_magnify2 = GenLayerZoom::magnify(1000LL, genLayer_addMushroomIsland, 0);
	GenLayerBiome*		genLayer_biome = LordNew GenLayerBiome(200LL, genLayer_magnify2, type);
	genLayer_magnify2	= GenLayerZoom::magnify(1000LL, genLayer_biome, 2);
	GenLayer*			genLayer_hills = LordNew GenLayerHills(1000LL, genLayer_magnify2);

	for (int i = 0; i < c; ++i)
	{
		genLayer_hills = LordNew GenLayerZoom((i64)(1000 + i), genLayer_hills);

		if (i == 0)
			genLayer_hills = LordNew GenLayerAddIsland(3LL, genLayer_hills);

		if (i == 1)
			genLayer_hills = LordNew GenLayerShore(1000LL, genLayer_hills);

		if (i == 1)
			genLayer_hills = LordNew GenLayerSwampRivers(1000LL, genLayer_hills);
	}

	GenLayerSmooth*		genLayer_smooth1 = LordNew GenLayerSmooth(1000LL, genLayer_hills);
	GenLayerRiverMix*	genLayer_riverMix = LordNew GenLayerRiverMix(100LL, genLayer_smooth1, genLayer_smooth);
	GenLayerVoronoiZoom* genLayer_voronoiZoom = LordNew GenLayerVoronoiZoom(10LL, genLayer_riverMix);
	genLayer_riverMix->initWorldGenSeed(seed);
	genLayer_voronoiZoom->initWorldGenSeed(seed);

	GenLayerArr result;
	result.push_back(genLayer_riverMix);
	result.push_back(genLayer_voronoiZoom);
	result.push_back(genLayer_riverMix);
	return result;
}

int* GenLayerIsland::getInts(int areax, int areay, int areaw, int areah)
{
	int* pResult = (int*)LordMalloc(sizeof(int) * areaw * areah);
	memset(pResult, 0, sizeof(int)*areaw*areah);

	for (int i = 0; i < areah; ++i)
	{
		for (int j = 0; j < areaw; ++j)
		{
			initChunkSeed((i64)(areax + j), (i64)(areay + i));
			pResult[j + i * areaw] = nextInt(10) == 0 ? 1 : 0;
		}
	}

	if (areax > -areaw && areax <= 0 && areay > -areah && areay <= 0)
	{
		pResult[-areax + -areay * areaw] = 1;
	}

	return pResult;
}

GenLayerFuzzyZoom::GenLayerFuzzyZoom(i64 seed, GenLayer* parent)
	: GenLayer(seed)
{
	setParent(parent);
}

int* GenLayerFuzzyZoom::getInts(int areax, int areay, int areaw, int areah)
{
	int ax = areax >> 1;
	int ay = areay >> 1;
	int aw = (areaw >> 1) + 3;
	int ah = (areah >> 1) + 3;
	int* array0 = m_parent->getInts(ax, ay, aw, ah);
	int* array1 = (int*)LordMalloc(sizeof(int) * aw * 2 * ah * 2);
	memset(array1, 0, sizeof(int) * aw * 2 * ah * 2);
	int i0 = aw << 1;
	int i1;

	for (int i = 0; i < ah - 1; ++i)
	{
		i1 = i << 1;
		int i2 = i1 * i0;
		int i3 = array0[0 + (i + 0) * aw];
		int i4 = array0[0 + (i + 1) * aw];

		for (int j = 0; j < aw - 1; ++j)
		{
			initChunkSeed((i64)((j + ax) << 1), (i64)((i + ay) << 1));
			int i5 = array0[j + 1 + (i + 0) * aw];
			int i6 = array0[j + 1 + (i + 1) * aw];
			array1[i2] = i3;
			array1[(i2++) + i0] = choose(i3, i4);
			array1[i2] = choose(i3, i5);
			array1[(i2++) + i0] = choose(i3, i5, i4, i6);
			i3 = i5;
			i4 = i6;
		}
	}

	int* array2 = (int*)LordMalloc(sizeof(int) * areaw * areah);
	
	for (i1 = 0; i1 < areah; ++i1)
	{
		memcpy(array2 + i1*areaw, array1 + (i1 + (areay & 1)) * (aw << 1) + (areax & 1), areaw*sizeof(int));
	}
	LordSafeFree(array1);
	LordSafeFree(array0);
	return array2;
}

int GenLayerFuzzyZoom::choose(int i0, int i1, int i2, int i3)
{
	int randi = nextInt(4);
	return randi == 0 ? i0 : (randi == 1 ? i1 : (randi == 2 ? i2 : i3)); 
}

GenLayerAddIsland::GenLayerAddIsland(i64 seed, GenLayer* parent)
	: GenLayer(seed)
{
	setParent(parent);
}

int* GenLayerAddIsland::getInts(int areax, int areay, int areaw, int areah)
{
	int ax = areax - 1;
	int ay = areay - 1;
	int aw = areaw + 2;
	int ah = areah + 2;
	int* array0 = m_parent->getInts(ax, ay, aw, ah);
	int* array1 = (int*)LordMalloc(sizeof(int) * areaw * areah);
	memset(array1, 0, sizeof(int)*areaw*areah);

	for (int i = 0; i < areah; ++i)
	{
		for (int j = 0; j < areaw; ++j)
		{
			int i0 = array0[j + 0 + (i + 0) * aw];
			int i1 = array0[j + 2 + (i + 0) * aw];
			int i2 = array0[j + 0 + (i + 2) * aw];
			int i3 = array0[j + 2 + (i + 2) * aw];
			int i4 = array0[j + 1 + (i + 1) * aw];
			initChunkSeed((i64)(j + areax), (i64)(i + areay));

			if (i4 == 0 && (i0 != 0 || i1 != 0 || i2 != 0 || i3 != 0))
			{
				int i5 = 1;
				int i6 = 1;

				if (i0 != 0 && nextInt(i5++) == 0)
				{
					i6 = i0;
				}
				if (i1 != 0 && nextInt(i5++) == 0)
				{
					i6 = i1;
				}
				if (i2 != 0 && nextInt(i5++) == 0)
				{
					i6 = i2;
				}
				if (i3 != 0 && nextInt(i5++) == 0)
				{
					i6 = i3;
				}
				if (nextInt(3) == 0)
				{
					array1[j + i * areaw] = i6;
				}
				else if (i6 == BIOME_GEN_ICE_PLAINS)
				{
					array1[j + i * areaw] = BIOME_GEN_RFOZEN_OCEAN;
				}
				else
				{
					array1[j + i * areaw] = 0;
				}
			}
			else if (i4 > 0 && (i0 == 0 || i1 == 0 || i2 == 0 || i3 == 0))
			{
				if (nextInt(5) == 0)
				{
					if (i4 == BIOME_GEN_ICE_PLAINS)
					{
						array1[j + i * areaw] = BIOME_GEN_RFOZEN_OCEAN;
					}
					else
					{
						array1[j + i * areaw] = 0;
					}
				}
				else
				{
					array1[j + i * areaw] = i4;
				}
			}
			else
			{
				array1[j + i * areaw] = i4;
			}
		}
	}

	LordSafeFree(array0);

	return array1;
}

GenLayerZoom::GenLayerZoom(i64 seed, GenLayer* parent)
	: GenLayer(seed)
{
	setParent(parent);
}

int* GenLayerZoom::getInts(int areax, int areay, int areaw, int areah)
{
	int ax = areax >> 1;
	int ay = areay >> 1;
	int aw = (areaw >> 1) + 3;
	int ah = (areah >> 1) + 3;
	int* array0 = m_parent->getInts(ax, ay, aw, ah);
	int* array1 = (int*)LordMalloc(sizeof(int) * aw * 2 * ah * 2);
	memset(array1, 0, sizeof(int)*aw * 2 * ah * 2);
	int i0 = aw << 1;
	int i1;

	for (int i = 0; i < ah - 1; ++i)
	{
		i1 = i << 1;
		int i2 = i1 * i0;
		int i3 = array0[0 + (i + 0) * aw];
		int i4 = array0[0 + (i + 1) * aw];

		for (int j = 0; j < aw - 1; ++j)
		{
			initChunkSeed((i64)((j + ax) << 1), (i64)((i + ay) << 1));
			int i5 = array0[j + 1 + (i + 0) * aw];
			int i6 = array0[j + 1 + (i + 1) * aw];
			array1[i2] = i3;
			array1[i2++ + i0] = choose(i3, i4);
			array1[i2] = choose(i3, i5);
			array1[i2++ + i0] = modeOrRandom(i3, i5, i4, i6);
			i3 = i5;
			i4 = i6;
		}
	}

	int* array2 = (int*)LordMalloc(sizeof(int) * areaw * areah);

	for (i1 = 0; i1 < areah; ++i1)
	{
		memcpy(array2 + i1*areaw, array1 + (i1 + (areay & 1)) * (aw << 1) + (areax & 1), sizeof(int)*areaw);
	}
	LordSafeFree(array1);
	LordSafeFree(array0);

	return array2;
}

int GenLayerZoom::modeOrRandom(int i0, int i1, int i2, int i3)
{
	if (i1 == i2 && i2 == i3)
	{
		return i1;
	}
	else if (i0 == i1 && i0 == i2)
	{
		return i0;
	}
	else if (i0 == i1 && i0 == i3)
	{
		return i0;
	}
	else if (i0 == i2 && i0 == i3)
	{
		return i0;
	}
	else if (i0 == i1 && i2 != i3)
	{
		return i0;
	}
	else if (i0 == i2 && i1 != i3)
	{
		return i0;
	}
	else if (i0 == i3 && i1 != i2)
	{
		return i0;
	}
	else if (i1 == i0 && i2 != i3)
	{
		return i1;
	}
	else if (i1 == i2 && i0 != i3)
	{
		return i1;
	}
	else if (i1 == i3 && i0 != i2)
	{
		return i1;
	}
	else if (i2 == i0 && i1 != i3)
	{
		return i2;
	}
	else if (i2 == i1 && i0 != i3)
	{
		return i2;
	}
	else if (i2 == i3 && i0 != i1)
	{
		return i2;
	}
	else if (i3 == i0 && i1 != i2)
	{
		return i2;
	}
	else if (i3 == i1 && i0 != i2)
	{
		return i2;
	}
	else if (i3 == i2 && i0 != i1)
	{
		return i2;
	}
	else
	{
		int randi = nextInt(4);
		return randi == 0 ? i0 : (randi == 1 ? i1 : (randi == 2 ? i2 : i3));
	}
}

GenLayer* GenLayerZoom::magnify(i64 seed, GenLayer* pLayer, int count)
{
	for (int i = 0; i < count; ++i)
	{
		pLayer = LordNew GenLayerZoom(seed + (i64)i, pLayer);
	}

	return pLayer;
}

GenLayerAddSnow::GenLayerAddSnow(i64 seed, GenLayer* parent)
	: GenLayer(seed)
{
	setParent(parent);
}

int* GenLayerAddSnow::getInts(int areax, int areay, int areaw, int areah)
{
	int ax = areax - 1;
	int ay = areay - 1;
	int aw = areaw + 2;
	int ah = areah + 2;
	int* array0 = m_parent->getInts(ax, ay, aw, ah);
	int* array1 = (int*)LordMalloc(sizeof(int) * areaw * areah);
	memset(array1, 0, sizeof(int)*areaw*areah);


	for (int i = 0; i < areah; ++i)
	{
		for (int j = 0; j < areaw; ++j)
		{
			int i0 = array0[j + 1 + (i + 1) * aw];
			initChunkSeed((i64)(j + areax), (i64)(i + areay));

			if (i0 == 0)
			{
				array1[j + i * areaw] = 0;
			}
			else
			{
				int randi = nextInt(5);

				if (randi == 0)
					randi = BIOME_GEN_ICE_PLAINS;
				else
					randi = 1;

				array1[j + i * areaw] = randi;
			}
		}
	}
	LordSafeFree(array0);

	return array1;
}

GenLayerAddMushroomIsland::GenLayerAddMushroomIsland(i64 seed, GenLayer* parent)
	: GenLayer(seed)
{
	setParent(parent);
}

int* GenLayerAddMushroomIsland::getInts(int arax, int arawy, int areaw, int areah)
{
	int ax = arax - 1;
	int ay = arawy - 1;
	int aw = areaw + 2;
	int ah = areah + 2;
	int* array0 = m_parent->getInts(ax, ay, aw, ah);
	int* array1 = (int*)LordMalloc(sizeof(int) * areaw * areah);
	memset(array1, 0, sizeof(int)*areaw*areah);

	for (int i = 0; i < areah; ++i)
	{
		for (int j = 0; j < areaw; ++j)
		{
			int i0 = array0[j + 0 + (i + 0) * aw];
			int i1 = array0[j + 2 + (i + 0) * aw];
			int i2 = array0[j + 0 + (i + 2) * aw];
			int i3 = array0[j + 2 + (i + 2) * aw];
			int i4 = array0[j + 1 + (i + 1) * aw];
			initChunkSeed((i64)(j + arax), (i64)(i + arawy));

			if (i4 == 0 && i0 == 0 && i1 == 0 && i2 == 0 && i3 == 0 && nextInt(100) == 0)
			{
				array1[j + i * areaw] = BIOME_GEN_MUSHROOM_ISLAND;
			}
			else
			{
				array1[j + i * areaw] = i4;
			}
		}
	}

	LordSafeFree(array0);

	return array1;
}

GenLayerRiverInit::GenLayerRiverInit(i64 seed, GenLayer* parent)
	: GenLayer(seed)
{
	setParent(parent);
}

int* GenLayerRiverInit::getInts(int areax, int areay, int areaw, int areah)
{
	int* array0 = m_parent->getInts(areax, areay, areaw, areah);
	int* area1 = (int*)LordMalloc(sizeof(int) * areaw * areah);
	memset(area1, 0, sizeof(int) * areaw * areah);

	for (int i = 0; i < areah; ++i)
	{
		for (int j = 0; j < areaw; ++j)
		{
			initChunkSeed((i64)(j + areax), (i64)(i + areay));
			area1[j + i * areaw] = array0[j + i * areaw] > 0 ? nextInt(2) + 2 : 0;
		}
	}

	LordSafeFree(array0);

	return area1;
}

GenLayerRiver::GenLayerRiver(i64 seed, GenLayer* parent)
	: GenLayer(seed)
{
	setParent(parent);
}

int* GenLayerRiver::getInts(int areax, int areay, int areaw, int areah)
{
	int ax = areax - 1;
	int ay = areay - 1;
	int aw = areaw + 2;
	int ah = areah + 2;
	int* array0 = m_parent->getInts(ax, ay, aw, ah);
	int* array1 = (int*)LordMalloc(sizeof(int) * areaw * areah);
	memset(array1, 0, sizeof(int)*areaw*areah);

	for (int i = 0; i < areah; ++i)
	{
		for (int j = 0; j < areaw; ++j)
		{
			int i0 = array0[j + 0 + (i + 1) * aw];
			int i1 = array0[j + 2 + (i + 1) * aw];
			int i2 = array0[j + 1 + (i + 0) * aw];
			int i3 = array0[j + 1 + (i + 2) * aw];
			int i4 = array0[j + 1 + (i + 1) * aw];

			if (i4 != 0 && i0 != 0 && i1 != 0 && i2 != 0 && i3 != 0 && i4 == i0 && i4 == i2 && i4 == i1 && i4 == i3)
			{
				array1[j + i * areaw] = -1;
			}
			else
			{
				array1[j + i * areaw] = BIOME_GEN_RIVER;
			}
		}
	}

	LordSafeFree(array0);

	return array1;
}

GenLayerSmooth::GenLayerSmooth(i64 seed, GenLayer* parent)
	: GenLayer(seed)
{
	setParent(parent);
}

int* GenLayerSmooth::getInts(int areax, int areay, int areaw, int areah)
{
	int ax = areax - 1;
	int ay = areay - 1;
	int aw = areaw + 2;
	int ah = areah + 2;
	int* array0 = m_parent->getInts(ax, ay, aw, ah);
	int* array1 = (int*)LordMalloc(sizeof(int) * areaw * areah);
	memset(array1, 0, sizeof(int)*areaw*areah);

	for (int i = 0; i < areah; ++i)
	{
		for (int j = 0; j < areaw; ++j)
		{
			int i0 = array0[j + 0 + (i + 1) * aw];
			int i1 = array0[j + 2 + (i + 1) * aw];
			int i2 = array0[j + 1 + (i + 0) * aw];
			int i3 = array0[j + 1 + (i + 2) * aw];
			int i4 = array0[j + 1 + (i + 1) * aw];

			if (i0 == i1 && i2 == i3)
			{
				initChunkSeed((i64)(j + areax), (i64)(i + areay));

				if (nextInt(2) == 0)
					i4 = i0;
				else
					i4 = i2;
			}
			else
			{
				if (i0 == i1)
					i4 = i0;

				if (i2 == i3)
					i4 = i2;
			}

			array1[j + i * areaw] = i4;
		}
	}

	LordSafeFree(array0);

	return array1;
}

GenLayerBiome::GenLayerBiome(i64 seed, GenLayer* parent, TERRAIN_TYPE type)
	: GenLayer(seed)
{
	setParent(parent);

	m_allowedBiomes.push_back(BiomeGenBase::desert);
	m_allowedBiomes.push_back(BiomeGenBase::forest);
	m_allowedBiomes.push_back(BiomeGenBase::extremeHills);
	m_allowedBiomes.push_back(BiomeGenBase::swampland);
	m_allowedBiomes.push_back(BiomeGenBase::plains);
	m_allowedBiomes.push_back(BiomeGenBase::taiga);
	m_allowedBiomes.push_back(BiomeGenBase::jungle);

	if (type == TERRAIN_TYPE_DEFAULT_1_1)
	{
		m_allowedBiomes.clear();
		m_allowedBiomes.push_back(BiomeGenBase::desert);
		m_allowedBiomes.push_back(BiomeGenBase::forest);
		m_allowedBiomes.push_back(BiomeGenBase::extremeHills);
		m_allowedBiomes.push_back(BiomeGenBase::swampland);
		m_allowedBiomes.push_back(BiomeGenBase::plains);
		m_allowedBiomes.push_back(BiomeGenBase::taiga);
	}
}

int* GenLayerBiome::getInts(int areax, int areay, int areaw, int areah)
{
	int* array0 = m_parent->getInts(areax, areay, areaw, areah);
	int* array1 = (int*)LordMalloc(sizeof(int) * areaw * areah);
	memset(array1, 0, sizeof(int) * areaw*areah);

	for (int i = 0; i < areah; ++i)
	{
		for (int j = 0; j < areaw; ++j)
		{
			initChunkSeed((i64)(j + areax), (i64)(i + areay));
			int i0 = array0[j + i * areaw];

			if (i0 == 0)
				array1[j + i * areaw] = 0;
			else if (i0 == BIOME_GEN_MUSHROOM_ISLAND)
				array1[j + i * areaw] = i0;
			else if (i0 == 1)
				array1[j + i * areaw] = m_allowedBiomes[nextInt(m_allowedBiomes.size())]->m_ID;
			else
			{
				int i1 = m_allowedBiomes[nextInt(m_allowedBiomes.size())]->m_ID;

				if (i1 == BIOME_GEN_TAIGA)
					array1[j + i * areaw] = i1;
				else
					array1[j + i * areaw] = BIOME_GEN_ICE_PLAINS;
			}
		}
	}
	LordSafeFree(array0);

	return array1;
}

GenLayerHills::GenLayerHills(i64 seed, GenLayer* parent)
	: GenLayer(seed)
{
	setParent(parent);
}

int* GenLayerHills::getInts(int areax, int areay, int areaw, int areah)
{
	int* array0 = m_parent->getInts(areax - 1, areay - 1, areaw + 2, areah + 2);
	int* array1 = (int*)LordMalloc(sizeof(int) * areaw * areah);
	memset(array1, 0, sizeof(int) * areaw*areah);

	for (int i = 0; i < areah; ++i)
	{
		for (int j = 0; j < areaw; ++j)
		{
			initChunkSeed((i64)(j + areax), (i64)(i + areay));
			int i0 = array0[j + 1 + (i + 1) * (areaw + 2)];

			if (nextInt(3) == 0)
			{
				int i1 = i0;

				if (i0 == BIOME_GEN_DESERT)
					i1 = BIOME_GEN_DESERT_HILLS;
				else if (i0 == BIOME_GEN_FOREST)
					i1 = BIOME_GEN_FOREST_HILLS;
				else if (i0 == BIOME_GEN_TAIGA)
					i1 = BIOME_GEN_TAIGA_HILLS;
				else if (i0 == BIOME_GEN_PLAINS)
					i1 = BIOME_GEN_FOREST;
				else if (i0 == BIOME_GEN_ICE_PLAINS)
					i1 = BIOME_GEN_ICE_MOUNTAINS;
				else if (i0 == BIOME_GEN_JUNGLE)
					i1 = BIOME_GEN_JUNGLEHILLS;

				if (i1 == i0)
					array1[j + i * areaw] = i0;
				else
				{
					int i2 = array0[j + 1 + (i + 1 - 1) * (areaw + 2)];
					int i3 = array0[j + 1 + 1 + (i + 1) * (areaw + 2)];
					int i4 = array0[j + 1 - 1 + (i + 1) * (areaw + 2)];
					int i5 = array0[j + 1 + (i + 1 + 1) * (areaw + 2)];

					if (i2 == i0 && i3 == i0 && i4 == i0 && i5 == i0)
					{
						array1[j + i * areaw] = i1;
					}
					else
					{
						array1[j + i * areaw] = i0;
					}
				}
			}
			else
			{
				array1[j + i * areaw] = i0;
			}
		}
	}
	LordSafeFree(array0);

	return array1;
}

GenLayerShore::GenLayerShore(i64 seed, GenLayer* parent)
	: GenLayer(seed)
{
	setParent(parent);
}

int* GenLayerShore::getInts(int areax, int areay, int areaw, int areah)
{
	int* array0 = m_parent->getInts(areax - 1, areay - 1, areaw + 2, areah + 2);
	int* array1 = (int*)LordMalloc(sizeof(int) * areaw * areah);
	memset(array1, 0, sizeof(int) * areaw * areah);

	for (int i = 0; i < areah; ++i)
	{
		for (int j = 0; j < areaw; ++j)
		{
			initChunkSeed((i64)(j + areax), (i64)(i + areay));
			int i0 = array0[j + 1 + (i + 1) * (areaw + 2)];
			int i1;
			int i2;
			int i3;
			int i4;

			if (i0 == BIOME_GEN_MUSHROOM_ISLAND)
			{
				i1 = array0[j + 1 + (i + 1 - 1) * (areaw + 2)];
				i2 = array0[j + 1 + 1 + (i + 1) * (areaw + 2)];
				i3 = array0[j + 1 - 1 + (i + 1) * (areaw + 2)];
				i4 = array0[j + 1 + (i + 1 + 1) * (areaw + 2)];

				if (i1 != BIOME_GEN_OCEAN &&
					i2 != BIOME_GEN_OCEAN && 
					i3 != BIOME_GEN_OCEAN && 
					i4 != BIOME_GEN_OCEAN)
				{
					array1[j + i * areaw] = i0;
				}
				else
				{
					array1[j + i * areaw] = BIOME_GEN_MUSHROOM_ISLAND_SHORE;
				}
			}
			else if (i0 != BIOME_GEN_OCEAN &&
				i0 != BIOME_GEN_RIVER && 
				i0 != BIOME_GEN_SWAMP_LAND && 
				i0 != BIOME_GEN_EXTREME_HILLS)
			{
				i1 = array0[j + 1 + (i + 1 - 1) * (areaw + 2)];
				i2 = array0[j + 1 + 1 + (i + 1) * (areaw + 2)];
				i3 = array0[j + 1 - 1 + (i + 1) * (areaw + 2)];
				i4 = array0[j + 1 + (i + 1 + 1) * (areaw + 2)];

				if (i1 != BIOME_GEN_OCEAN &&
					i2 != BIOME_GEN_OCEAN &&
					i3 != BIOME_GEN_OCEAN &&
					i4 != BIOME_GEN_OCEAN)
				{
					array1[j + i * areaw] = i0;
				}
				else
				{
					array1[j + i * areaw] = BIOME_GEN_BEACH;
				}
			}
			else if (i0 == BIOME_GEN_EXTREME_HILLS)
			{
				i1 = array0[j + 1 + (i + 1 - 1) * (areaw + 2)];
				i2 = array0[j + 1 + 1 + (i + 1) * (areaw + 2)];
				i3 = array0[j + 1 - 1 + (i + 1) * (areaw + 2)];
				i4 = array0[j + 1 + (i + 1 + 1) * (areaw + 2)];

				if (i1 == BIOME_GEN_EXTREME_HILLS &&
					i2 == BIOME_GEN_EXTREME_HILLS && 
					i3 == BIOME_GEN_EXTREME_HILLS &&
					i4 == BIOME_GEN_EXTREME_HILLS)
				{
					array1[j + i * areaw] = i0;
				}
				else
				{
					array1[j + i * areaw] = BIOME_GEN_EXTREME_HILLS_EDGE;
				}
			}
			else
			{
				array1[j + i * areaw] = i0;
			}
		}
	}

	LordSafeFree(array0);

	return array1;
}

GenLayerSwampRivers::GenLayerSwampRivers(i64 seed, GenLayer* parent)
	: GenLayer(seed)
{
	setParent(parent);
}

int* GenLayerSwampRivers::getInts(int areax, int areay, int areaw, int areah)
{
	int* array0 = m_parent->getInts(areax - 1, areay - 1, areaw + 2, areah + 2);
	int* array1 = (int*)LordMalloc(sizeof(int) * areaw * areah);
	memset(array1, 0, sizeof(int) * areaw * areah);

	for (int i = 0; i < areah; ++i)
	{
		for (int j = 0; j < areaw; ++j)
		{
			initChunkSeed((i64)(j + areax), (i64)(i + areay));
			int i0 = array0[j + 1 + (i + 1) * (areaw + 2)];

			if ((i0 != BIOME_GEN_SWAMP_LAND || nextInt(6) != 0) && 
				(i0 != BIOME_GEN_JUNGLE && i0 != BIOME_GEN_JUNGLEHILLS || nextInt(8) != 0))
			{
				array1[j + i * areaw] = i0;
			}
			else
			{
				array1[j + i * areaw] = BIOME_GEN_RIVER;
			}
		}
	}

	LordSafeFree(array0);

	return array1;
}

GenLayerRiverMix::GenLayerRiverMix(i64 seed, GenLayer* parent1, GenLayer* parent2)
	: GenLayer(seed)
{
	m_pBiomePatternGeneratorChain = parent1;
	m_pRiverPatternGeneratorChain = parent2;
	m_pBiomePatternGeneratorChain->addRef();
	m_pRiverPatternGeneratorChain->addRef();
}

GenLayerRiverMix::~GenLayerRiverMix()
{
	int ref = m_pBiomePatternGeneratorChain->subRef();
	if (!ref)
		LordSafeDelete(m_pBiomePatternGeneratorChain);
	ref = m_pRiverPatternGeneratorChain->subRef();
	if (!ref)
		LordSafeDelete(m_pRiverPatternGeneratorChain);
}

void GenLayerRiverMix::initWorldGenSeed(i64 seed)
{
	m_pBiomePatternGeneratorChain->initWorldGenSeed(seed);
	m_pRiverPatternGeneratorChain->initWorldGenSeed(seed);
	GenLayer::initWorldGenSeed(seed);
}

int* GenLayerRiverMix::getInts(int areax, int areay, int areaw, int areah)
{
	int* array0 = m_pBiomePatternGeneratorChain->getInts(areax, areay, areaw, areah);
	int* array1 = m_pRiverPatternGeneratorChain->getInts(areax, areay, areaw, areah);
	int* array2 = (int*)LordMalloc(sizeof(int) * areaw * areah);
	memset(array2, 0, sizeof(int) * areah * areaw);

	for (int i = 0; i < areaw * areah; ++i)
	{
		if (array0[i] == BIOME_GEN_OCEAN)
		{
			array2[i] = array0[i];
		}
		else if (array1[i] >= 0)
		{
			if (array0[i] == BIOME_GEN_ICE_PLAINS)
			{
				array2[i] = BIOME_GEN_FROZEN_RIVER;
			}
			else if (array0[i] != BIOME_GEN_MUSHROOM_ISLAND &&
				array0[i] != BIOME_GEN_MUSHROOM_ISLAND_SHORE)
			{
				array2[i] = array1[i];
			}
			else
			{
				array2[i] = BIOME_GEN_MUSHROOM_ISLAND_SHORE;
			}
		}
		else
		{
			array2[i] = array0[i];
		}
	}

	LordSafeFree(array0);
	LordSafeFree(array1);

	return array2;
}

GenLayerVoronoiZoom::GenLayerVoronoiZoom(i64 seed, GenLayer* parent)
	: GenLayer(seed)
{
	setParent(parent);
}

int* GenLayerVoronoiZoom::getInts(int areax, int areay, int areaw, int areah)
{
	areax -= 2;
	areay -= 2;
	i8 zoom = 2;
	int i0 = 1 << zoom;
	int ax = areax >> zoom;
	int ay = areay >> zoom;
	int aw = (areaw >> zoom) + 3;
	int ah = (areah >> zoom) + 3;
	int* array0 = m_parent->getInts(ax, ay, aw, ah);
	int i1 = aw << zoom;
	int i2 = ah << zoom;
	int* array1 = (int*)LordMalloc(sizeof(int) * i1 * i2);
	memset(array1, 0, sizeof(int) * i1 * i2);
	int i3;

	for (int i = 0; i < ah - 1; ++i)
	{
		i3 = array0[0 + (i + 0) * aw];
		int i9 = array0[0 + (i + 1) * aw];

		for (int j = 0; j < aw - 1; ++j)
		{
			double d0 = (double)i0 * 0.9;
			initChunkSeed((i64)((j + ax) << zoom), (i64)((i + ay) << zoom));
			double d1 = ((double)nextInt(1024) / 1024.0 - 0.5) * d0;
			double d2 = ((double)nextInt(1024) / 1024.0 - 0.5) * d0;
			initChunkSeed((i64)((j + ax + 1) << zoom), (i64)((i + ay) << zoom));
			double d3 = ((double)nextInt(1024) / 1024.0 - 0.5) * d0 + (double)i0;
			double d4 = ((double)nextInt(1024) / 1024.0 - 0.5) * d0;
			initChunkSeed((i64)((j + ax) << zoom), (i64)((i + ay + 1) << zoom));
			double d5 = ((double)nextInt(1024) / 1024.0 - 0.5) * d0;
			double d6 = ((double)nextInt(1024) / 1024.0 - 0.5) * d0 + (double)i0;
			initChunkSeed((i64)((j + ax + 1) << zoom), (i64)((i + ay + 1) << zoom));
			double d7 = ((double)nextInt(1024) / 1024.0 - 0.5) * d0 + (double)i0;
			double d8 = ((double)nextInt(1024) / 1024.0 - 0.5) * d0 + (double)i0;
			int i4 = array0[j + 1 + (i + 0) * aw];
			int i5 = array0[j + 1 + (i + 1) * aw];

			for (int k = 0; k < i0; ++k)
			{
				int i6 = ((i << zoom) + k) * i1 + (j << zoom);

				for (int l = 0; l < i0; ++l)
				{
					double d9 = ((double)k - d2) * ((double)k - d2) + ((double)l - d1) * ((double)l - d1);
					double d10 = ((double)k - d4) * ((double)k - d4) + ((double)l - d3) * ((double)l - d3);
					double d11 = ((double)k - d6) * ((double)k - d6) + ((double)l - d5) * ((double)l - d5);
					double d12 = ((double)k - d8) * ((double)k - d8) + ((double)l - d7) * ((double)l - d7);

					if (d9 < d10 && d9 < d11 && d9 < d12)
					{
						array1[i6++] = i3;
					}
					else if (d10 < d9 && d10 < d11 && d10 < d12)
					{
						array1[i6++] = i4;
					}
					else if (d11 < d9 && d11 < d10 && d11 < d12)
					{
						array1[i6++] = i9;
					}
					else
					{
						array1[i6++] = i5;
					}
				}
			}

			i3 = i4;
			i9 = i5;
		}
	}

	int* array2 = (int*)LordMalloc(sizeof(int) * areaw * areah);

	for (i3 = 0; i3 < areah; ++i3)
	{
		memcpy(array2 + i3 * areaw, array1 + (i3 + (areay & i0 - 1)) * (aw << zoom) + (areax & i0 - 1), areaw * sizeof(int));
	}

	LordSafeFree(array1);
	LordSafeFree(array0);

	return array2;
}

}