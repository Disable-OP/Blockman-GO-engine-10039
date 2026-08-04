#include "WorldProvider.h"
#include "World.h"
#include "WorldChunkManager.h"

#include "Block/Block.h"
#include "Block/BlockManager.h"
#include "WorldGenerator/FlatGeneratorInfo.h"
#include "WorldGenerator/BiomeGen.h"
#include "Chunk/IChunkProvider.h"
#include "WorldGenerator/MapGenerate.h"
#include "Chunk/ChunkProviderEnd.h"
#include "Chunk/ChunkProviderFlat.h"
#include "Chunk/ChunkProviderGenerate.h"
#include "Chunk/ChunkProviderHell.h"

namespace BLOCKMAN
{

float WorldProvider::MOON_PHASE_FACTORS[8] = {1.0F, 0.75F, 0.5F, 0.25F, 0.0F, 0.25F, 0.5F, 0.75F};

WorldProvider::WorldProvider()
	: worldObj(NULL)
	, terrainType(TERRAIN_TYPE_INVALID)
	, generateOptions(StringUtil::BLANK)
	, worldChunkMgr(NULL)
	, isHellWorld(false)
	, hasNoSky(false)
	, lightBrightnessTable(NULL)
	, dimensionId(0)
	, colorsSunriseSunset(NULL)
{
	lightBrightnessTable = (float*)LordMalloc(sizeof(float) * 16);
	memset(lightBrightnessTable, 0, sizeof(float) * 16);
	colorsSunriseSunset = (float*)LordMalloc(sizeof(float) * 4);
	memset(colorsSunriseSunset, 0, sizeof(float) * 4);
}

WorldProvider::~WorldProvider()
{
	LordSafeFree(lightBrightnessTable);
	LordSafeFree(colorsSunriseSunset);
	LordSafeDelete(worldChunkMgr);
}

void WorldProvider::registerWorld(World* par1World)
{
	worldObj = par1World;
	terrainType = par1World->getWorldInfo().getTerrainType();
	generateOptions = par1World->getWorldInfo().getGeneratorOptions();
	registerWorldChunkManager();
	generateLightBrightnessTable();
}

void WorldProvider::generateLightBrightnessTable()
{
	float var1 = 0.0F;

	for (int var2 = 0; var2 <= 15; ++var2)
	{
		float var3 = 1.0F - (float)var2 / 15.0F;
		lightBrightnessTable[var2] = (1.0F - var3) / (var3 * 3.0F + 1.0F) * (1.0F - var1) + var1;
	}
}

void WorldProvider::registerWorldChunkManager()
{
	if (worldObj->getWorldInfo().getTerrainType() == TERRAIN_TYPE_FLAT)
	{
		FlatGeneratorInfo* var1 = FlatGeneratorInfo::createFlatGeneratorFromString(worldObj->getWorldInfo().getGeneratorOptions());
		worldChunkMgr = LordNew WorldChunkManagerHell(BiomeGenBase::biomeList[var1->getBiome()], 0.5F, 0.5F);
	}
	else
	{
		worldChunkMgr = LordNew WorldChunkManager(worldObj);
	}
}

IChunkProvider* WorldProvider::createChunkGenerator()
{
	IChunkProvider* pResult = NULL;
	if (terrainType == TERRAIN_TYPE_FLAT)
	{
		pResult = LordNew ChunkProviderFlat(worldObj, generateOptions);
	}
	else
	{
		pResult = LordNew ChunkProviderGenerate(worldObj, worldObj->getSeed(), worldObj->getWorldInfo().isMapFeaturesEnabled());
	}
	return pResult;
}

bool WorldProvider::canCoordinateBeSpawn(int x, int z) const
{
	int var3 = worldObj->getFirstUncoveredBlock(x, z);
	return var3 == BLOCK_ID_GRASS;
}

float WorldProvider::calculateCelestialAngle(i64 allTicks, float rdt)
{
	/*
		0.78  0.79  0.79  0.80  0.80  0.81  0.81  0.81  0.82  0.82  0.83  0.83
		0.83  0.84  0.84  0.85  0.85  0.85  0.86  0.86  0.87  0.87  0.87  0.88
		0.88  0.89  0.89  0.89  0.90  0.90  0.90  0.91  0.91  0.91  0.92  0.92
		0.93  0.93  0.93  0.94  0.94  0.94  0.95  0.95  0.95  0.96  0.96  0.96
		0.96  0.97  0.97  0.97  0.98  0.98  0.98  0.99  0.99  0.99  0.99  1.00
		0.00  0.00  0.01  0.01  0.01  0.01  0.02  0.02  0.02  0.03  0.03  0.03
		0.04  0.04  0.04  0.04  0.05  0.05  0.05  0.06  0.06  0.06  0.07  0.07
		0.07  0.08  0.08  0.09  0.09  0.09  0.10  0.10  0.10  0.11  0.11  0.11
		0.12  0.12  0.13  0.13  0.13  0.14  0.14  0.15  0.15  0.15  0.16  0.16
		0.17  0.17  0.17  0.18  0.18  0.19  0.19  0.19  0.20  0.20  0.21  0.21
		0.22  0.22  0.22  0.23  0.23  0.24  0.24  0.25  0.25  0.26  0.26  0.26
		0.27  0.27  0.28  0.28  0.29  0.29  0.30  0.30  0.31  0.31  0.31  0.32
		0.32  0.33  0.33  0.34  0.34  0.35  0.35  0.36  0.36  0.37  0.37  0.38
		0.38  0.39  0.39  0.40  0.40  0.41  0.41  0.42  0.42  0.43  0.43  0.44
		0.44  0.45  0.45  0.46  0.46  0.47  0.47  0.48  0.48  0.49  0.49  0.50
		0.50  0.50  0.51  0.51  0.52  0.52  0.53  0.53  0.54  0.54  0.55  0.55
		0.56  0.56  0.57  0.57  0.58  0.58  0.59  0.59  0.60  0.60  0.61  0.61
		0.62  0.62  0.63  0.63  0.64  0.64  0.65  0.65  0.66  0.66  0.67  0.67
		0.68  0.68  0.69  0.69  0.69  0.70  0.70  0.71  0.71  0.72  0.72  0.73
		0.73  0.74  0.74  0.74  0.75  0.75  0.76  0.76  0.77  0.77  0.78  0.78
	*/
	// 20 ticks == 1s
	// 24000 ticks = 1200s (20 mins)
	int dayTick = (int)(allTicks % 24000L);

	// -0.25f ~ 0.75f
	float result = ((float)dayTick + rdt) / 24000.0F - 0.25F;

	if (result < 0.0F)
	{
		++result;
	}

	if (result > 1.0F)
	{
		--result;
	}

	float temp = result;
	result = 1.0F - ((Math::Cos(result * Math::PI) + 1.0f) / 2.0f);
	result = temp + (result - temp) / 3.0F;
	return result;
}

float* WorldProvider::calcSunriseSunsetColors(float angle, float rdt)
{
	float var3 = 0.4F;
	float var4 = Math::Cos_tab(angle * (float)Math::PI * 2.0F) - 0.0F;
	float var5 = -0.0F;

	if (var4 >= var5 - var3 && var4 <= var5 + var3)
	{
		float var6 = (var4 - var5) / var3 * 0.5F + 0.5F;
		float var7 = 1.0F - (1.0F - Math::Sin_tab(var6 * (float)Math::PI)) * 0.99F;
		var7 *= var7;
		colorsSunriseSunset[0] = var6 * 0.3F + 0.7F;
		colorsSunriseSunset[1] = var6 * var6 * 0.7F + 0.2F;
		colorsSunriseSunset[2] = var6 * var6 * 0.0F + 0.2F;
		colorsSunriseSunset[3] = var7;
		return colorsSunriseSunset;
	}
	else
	{
		return NULL;
	}
}

Color WorldProvider::getFogColor(float angle, float rdt)
{
	float var3 = Math::Cos_tab(angle * (float)Math::PI * 2.0F) * 2.0F + 0.5F;

	if (var3 < 0.0F)
	{
		var3 = 0.0F;
	}

	if (var3 > 1.0F)
	{
		var3 = 1.0F;
	}

	float var4 = 0.7529412F;
	float var5 = 0.84705883F;
	float var6 = 1.0F;
	var4 *= var3 * 0.94F + 0.06F;
	var5 *= var3 * 0.94F + 0.06F;
	var6 *= var3 * 0.91F + 0.09F;
	return Color(var4, var5, var6);
}

WorldProvider* WorldProvider::getProviderForDimension(int dimension)
{
	if (dimension == -1)
	{
		return LordNew WorldProviderHell();
	}
	else if (dimension == 0)
	{
		return LordNew WorldProviderSurface();
	}
	else if (dimension == 1)
	{
		return LordNew WorldProviderEnd();
	}
	return NULL;
}

void WorldProviderHell::registerWorldChunkManager()
{
	worldChunkMgr = LordNew WorldChunkManagerHell(BiomeGenBase::hell, 1.0F, 0.0F);
	isHellWorld = true;
	hasNoSky = true;
	dimensionId = -1;
}

void WorldProviderHell::generateLightBrightnessTable()
{
	float var1 = 0.1F;

	for (int var2 = 0; var2 <= 15; ++var2)
	{
		float var3 = 1.0F - (float)var2 / 15.0F;
		lightBrightnessTable[var2] = (1.0F - var3) / (var3 * 3.0F + 1.0F) * (1.0F - var1) + var1;
	}
}

IChunkProvider* WorldProviderHell::createChunkGenerator()
{
	return LordNew ChunkProviderHell(worldObj, worldObj->getSeed());
}

IChunkProvider* WorldProviderSurface::createChunkGenerator()
{
	return WorldProvider::createChunkGenerator();
}

void WorldProviderEnd::registerWorldChunkManager()
{
	worldChunkMgr = LordNew WorldChunkManagerHell(BiomeGenBase::sky, 0.5F, 0.0F);
	dimensionId = 1;
	hasNoSky = true;
}

IChunkProvider* WorldProviderEnd::createChunkGenerator()
{
	return LordNew ChunkProviderEnd(worldObj, worldObj->getSeed());
}

Color WorldProviderEnd::getFogColor(float par1, float par2)
{
	int var3 = 10518688;
	float var4 = Math::Cos_tab(par1 * (float)Math::PI * 2.0F) * 2.0F + 0.5F;

	if (var4 < 0.0F)
	{
		var4 = 0.0F;
	}

	if (var4 > 1.0F)
	{
		var4 = 1.0F;
	}

	float var5 = (float)(var3 >> 16 & 255) / 255.0F;
	float var6 = (float)(var3 >> 8 & 255) / 255.0F;
	float var7 = (float)(var3 & 255) / 255.0F;
	var5 *= var4 * 0.0F + 0.15F;
	var6 *= var4 * 0.0F + 0.15F;
	var7 *= var4 * 0.0F + 0.15F;
	return Color(var5, var6, var7);
}

bool WorldProviderEnd::canCoordinateBeSpawn(int par1, int par2)
{
	int var3 = worldObj->getFirstUncoveredBlock(par1, par2);
	return var3 == 0 ? false : BlockManager::sBlocks[var3]->getMaterial().blocksMovement();
}

}