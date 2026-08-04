#include "BiomeDecorator.h"
#include "WorldGenerator.h"
#include "BM_TypeDef.h"
#include "BiomeGen.h"

#include "World/World.h"

namespace BLOCKMAN
{

BiomeDecorator::BiomeDecorator(BiomeGenBase* pBiome)
{
	m_pGen_clay			= LordNew WorldGenClay(4);
	m_pGen_sand			= LordNew WorldGenSand(7, BLOCK_ID_SAND);
	m_pGen_gravelAsSand = LordNew WorldGenSand(6, BLOCK_ID_GRAVEL);
	m_pGen_dirt			= LordNew WorldGenMinable(BLOCK_ID_DIRT, 32);
	m_pGen_gravel		= LordNew WorldGenMinable(BLOCK_ID_GRAVEL, 32);
	m_pGen_coal			= LordNew WorldGenMinable(BLOCK_ID_ORE_COAL, 16);
	m_pGen_iron			= LordNew WorldGenMinable(BLOCK_ID_ORE_IRON, 8);
	m_pGen_gold			= LordNew WorldGenMinable(BLOCK_ID_ORE_GOLD, 8);
	m_pGen_redStone		= LordNew WorldGenMinable(BLOCK_ID_ORE_REDSTONE, 7);
	m_pGen_diamond		= LordNew WorldGenMinable(BLOCK_ID_ORE_DIAMOND, 7);
	m_pGen_lapis		= LordNew WorldGenMinable(BLOCK_ID_ORE_LAPIS, 6);
	m_pGen_plant_yellow	= LordNew WorldGenFlowers(BLOCK_ID_PLANT_YELLOW);
	m_pGen_plant_red	= LordNew WorldGenFlowers(BLOCK_ID_PLANT_RED);
	m_pGen_deadBush		= LordNew WorldGenDeadBush(BLOCK_ID_DEADBUSH);
	m_pGen_mushroom_brown= LordNew WorldGenFlowers(BLOCK_ID_MUSHROOM_BROWN);
	m_pGen_mushroom_red	= LordNew WorldGenFlowers(BLOCK_ID_MUSHROOM_RED);
	m_pGen_bigMushroom	= LordNew WorldGenBigMushroom();
	m_pGen_reed			= LordNew WorldGenReed();
	m_pGen_cactus		= LordNew WorldGenCactus();
	m_pGen_waterlily	= LordNew WorldGenWaterlily();
	m_pGen_pumpkim		= LordNew WorldGenPumpkin();
	m_pGen_waterMoving	= LordNew WorldGenLiquids(BLOCK_ID_WATERMOVING);
	m_pGen_lavaMoving	= LordNew WorldGenLiquids(BLOCK_ID_LAVAMOVING);

	m_pWorld = NULL;
	m_amount_waterlily	= 0;
	m_amount_trees		= 0;
	m_amount_flowers	= 2;
	m_amount_grass		= 1;
	m_amount_deadBush	= 0;
	m_amount_mushrooms	= 0;
	m_amount_reeds		= 0;
	m_amount_cactus		= 0;
	m_amount_sand		= 1;
	m_amount_gravelAsSand = 3;
	m_amount_clay		= 1;
	m_bGenerateLakes	= true;
	m_amount_bigMushroom = 0;
	m_pBiome			= pBiome;

}

BiomeDecorator::~BiomeDecorator()
{
	LordSafeDelete(m_pGen_clay);
	LordSafeDelete(m_pGen_sand);
	LordSafeDelete(m_pGen_gravelAsSand);
	LordSafeDelete(m_pGen_dirt);
	LordSafeDelete(m_pGen_gravel);
	LordSafeDelete(m_pGen_coal);
	LordSafeDelete(m_pGen_iron);
	LordSafeDelete(m_pGen_gold);
	LordSafeDelete(m_pGen_redStone);
	LordSafeDelete(m_pGen_diamond);
	LordSafeDelete(m_pGen_lapis);
	LordSafeDelete(m_pGen_plant_yellow);
	LordSafeDelete(m_pGen_plant_red);
	LordSafeDelete(m_pGen_deadBush);
	LordSafeDelete(m_pGen_mushroom_brown);
	LordSafeDelete(m_pGen_mushroom_red);
	LordSafeDelete(m_pGen_bigMushroom);
	LordSafeDelete(m_pGen_pumpkim);
	LordSafeDelete(m_pGen_reed);
	LordSafeDelete(m_pGen_cactus);
	LordSafeDelete(m_pGen_waterlily);
	LordSafeDelete(m_pGen_waterMoving);
	LordSafeDelete(m_pGen_lavaMoving);
}

void BiomeDecorator::decorate(World* pWorld, Random& rand, int x, int z)
{
	if (m_pWorld != NULL)
	{
		LordException("Already decorating!");
	}

	m_pWorld = pWorld;
	m_pRandom = &rand;
	m_chunkX = x;
	m_chunkZ = z;
	decorate();
	m_pWorld = NULL;
	m_pRandom = NULL;
}

void BiomeDecorator::decorate()
{
	generateOres();

	int x = 0;
	int y = 0;
	int z = 0;

	for (int i = 0; i < m_amount_gravelAsSand; ++i)
	{
		x = m_chunkX + m_pRandom->nextInt(16) + 8;
		z = m_chunkZ + m_pRandom->nextInt(16) + 8;
		m_pGen_sand->generate(m_pWorld, *m_pRandom, x, m_pWorld->getTopSolidOrLiquidBlock(x, z), z);
	}

	for (int i = 0; i < m_amount_clay; ++i)
	{
		x = m_chunkX + m_pRandom->nextInt(16) + 8;
		z = m_chunkZ + m_pRandom->nextInt(16) + 8;
		m_pGen_clay->generate(m_pWorld, *m_pRandom, x, m_pWorld->getTopSolidOrLiquidBlock(x, z), z);
	}

	for (int i = 0; i < m_amount_sand; ++i)
	{
		x = m_chunkX + m_pRandom->nextInt(16) + 8;
		z = m_chunkZ + m_pRandom->nextInt(16) + 8;
		m_pGen_sand->generate(m_pWorld, *m_pRandom, x, m_pWorld->getTopSolidOrLiquidBlock(x, z), z);
	}

	int amount_trees = m_amount_trees;
	if (m_pRandom->nextInt(10) == 0)
		++amount_trees;
		
	for (int i = 0; i < amount_trees; ++i)
	{
		x = m_chunkX + m_pRandom->nextInt(16) + 8;
		z = m_chunkZ + m_pRandom->nextInt(16) + 8;
		WorldGenerator* pWorldGen = m_pBiome->getRandomWorldGenForTrees(*m_pRandom);
		pWorldGen->setScale(1.0, 1.0, 1.0);
		pWorldGen->generate(m_pWorld, *m_pRandom, x, m_pWorld->getHeightValue(x, z), z);
	}

	for (int i = 0; i < m_amount_bigMushroom; ++i)
	{
		x = m_chunkX + m_pRandom->nextInt(16) + 8;
		z = m_chunkZ + m_pRandom->nextInt(16) + 8;
		m_pGen_bigMushroom->generate(m_pWorld, *m_pRandom, x, m_pWorld->getHeightValue(x, z), z);
	}

	for (int i = 0; i < m_amount_flowers; ++i)
	{
		x = m_chunkX + m_pRandom->nextInt(16) + 8;
		y = m_pRandom->nextInt(128);
		z = m_chunkZ + m_pRandom->nextInt(16) + 8;
		m_pGen_plant_yellow->generate(m_pWorld, *m_pRandom, x, y, z);

		if (m_pRandom->nextInt(4) == 0)
		{
			x = m_chunkX + m_pRandom->nextInt(16) + 8;
			y = m_pRandom->nextInt(128);
			z = m_chunkZ + m_pRandom->nextInt(16) + 8;
			m_pGen_plant_red->generate(m_pWorld, *m_pRandom, x, y, z);
		}
	}

	for (int i = 0; i < m_amount_grass; ++i)
	{
		x = m_chunkX + m_pRandom->nextInt(16) + 8;
		y = m_pRandom->nextInt(128);
		z = m_chunkZ + m_pRandom->nextInt(16) + 8;
		WorldGenerator* pWorldGen = m_pBiome->getRandomWorldGenForGrass(*m_pRandom);
		pWorldGen->generate(m_pWorld, *m_pRandom, x, y, z);
		LordSafeDelete(pWorldGen);
	}

	for (int i = 0; i < m_amount_deadBush; ++i)
	{
		x = m_chunkX + m_pRandom->nextInt(16) + 8;
		y = m_pRandom->nextInt(128);
		z = m_chunkZ + m_pRandom->nextInt(16) + 8;
		m_pGen_deadBush->generate(m_pWorld, *m_pRandom, x, y, z);
	}

	for (int i = 0; i < m_amount_waterlily; ++i)
	{
		x = m_chunkX + m_pRandom->nextInt(16) + 8;
		z = m_chunkZ + m_pRandom->nextInt(16) + 8;

		int y = 0;
		for (y = m_pRandom->nextInt(128); y > 0; --y)
		{
			if (m_pWorld->getBlockId(BlockPos(x, y - 1, z)) != 0)
				break;
		}

		m_pGen_waterlily->generate(m_pWorld, *m_pRandom, x, y, z);
	}

	for (int i = 0; i < m_amount_mushrooms; ++i)
	{
		if (m_pRandom->nextInt(4) == 0)
		{
			x = m_chunkX + m_pRandom->nextInt(16) + 8;
			z = m_chunkZ + m_pRandom->nextInt(16) + 8;
			y = m_pWorld->getHeightValue(x, z);
			m_pGen_mushroom_brown->generate(m_pWorld, *m_pRandom, x, y, z);
		}

		if (m_pRandom->nextInt(8) == 0)
		{
			x = m_chunkX + m_pRandom->nextInt(16) + 8;
			z = m_chunkZ + m_pRandom->nextInt(16) + 8;
			y = m_pRandom->nextInt(128);
			m_pGen_mushroom_red->generate(m_pWorld, *m_pRandom, x, y, z);
		}
	}

	if (m_pRandom->nextInt(4) == 0)
	{
		x = m_chunkX + m_pRandom->nextInt(16) + 8;
		y = m_pRandom->nextInt(128);
		z = m_chunkZ + m_pRandom->nextInt(16) + 8;
		m_pGen_mushroom_brown->generate(m_pWorld, *m_pRandom, x, y, z);
	}

	if (m_pRandom->nextInt(8) == 0)
	{
		x = m_chunkX + m_pRandom->nextInt(16) + 8;
		y = m_pRandom->nextInt(128);
		z = m_chunkZ + m_pRandom->nextInt(16) + 8;
		m_pGen_mushroom_red->generate(m_pWorld, *m_pRandom, x, y, z);
	}

	for (int i = 0; i < m_amount_reeds; ++i)
	{
		x = m_chunkX + m_pRandom->nextInt(16) + 8;
		z = m_chunkZ + m_pRandom->nextInt(16) + 8;
		y = m_pRandom->nextInt(128);
		m_pGen_reed->generate(m_pWorld, *m_pRandom, x, y, z);
	}

	for (int i = 0; i < 10; ++i)
	{
		x = m_chunkX + m_pRandom->nextInt(16) + 8;
		y = m_pRandom->nextInt(128);
		z = m_chunkZ + m_pRandom->nextInt(16) + 8;
		m_pGen_reed->generate(m_pWorld, *m_pRandom, x, y, z);
	}

	if (m_pRandom->nextInt(32) == 0)
	{
		x = m_chunkX + m_pRandom->nextInt(16) + 8;
		y = m_pRandom->nextInt(128);
		z = m_chunkZ + m_pRandom->nextInt(16) + 8;
		m_pGen_pumpkim->generate(m_pWorld, *m_pRandom, x, y, z);
	}

	for (int i = 0; i < m_amount_cactus; ++i)
	{
		x = m_chunkX + m_pRandom->nextInt(16) + 8;
		y = m_pRandom->nextInt(128);
		z = m_chunkZ + m_pRandom->nextInt(16) + 8;
		m_pGen_cactus->generate(m_pWorld, *m_pRandom, x, y, z);
	}

	if (m_bGenerateLakes)
	{
		for (int i = 0; i < 50; ++i)
		{
			x = m_chunkX + m_pRandom->nextInt(16) + 8;
			y = m_pRandom->nextInt(m_pRandom->nextInt(120) + 8);
			z = m_chunkZ + m_pRandom->nextInt(16) + 8;
			m_pGen_waterMoving->generate(m_pWorld, *m_pRandom, x, y, z);
		}

		for (int i = 0; i < 20; ++i)
		{
			x = m_chunkX + m_pRandom->nextInt(16) + 8;
			y = m_pRandom->nextInt(m_pRandom->nextInt(m_pRandom->nextInt(112) + 8) + 8);
			z = m_chunkZ + m_pRandom->nextInt(16) + 8;
			m_pGen_lavaMoving->generate(m_pWorld, *m_pRandom, x, y, z);
		}
	}
}

void BiomeDecorator::genStandardOre1(int blockCount, WorldGenerator* pWorldGen, int minHeight, int maxHeight)
{
	for (int i = 0; i < blockCount; ++i)
	{
		int x = m_chunkX + m_pRandom->nextInt(16);
		int y = m_pRandom->nextInt(maxHeight - minHeight) + minHeight;
		int z = m_chunkZ + m_pRandom->nextInt(16);
		pWorldGen->generate(m_pWorld, *m_pRandom, x, y, z);
	}
}

void BiomeDecorator::genStandardOre2(int blockCount, WorldGenerator* pWorldGen, int minHeight, int maxHeight)
{
	for (int i = 0; i < blockCount; ++i)
	{
		int x = m_chunkX + m_pRandom->nextInt(16);
		int y = m_pRandom->nextInt(maxHeight) + m_pRandom->nextInt(maxHeight) + (minHeight - maxHeight);
		int z = m_chunkZ + m_pRandom->nextInt(16);
		pWorldGen->generate(m_pWorld, *m_pRandom, x, y, z);
	}
}

void BiomeDecorator::generateOres()
{
	genStandardOre1(20, m_pGen_dirt, 0, 128);
	genStandardOre1(10, m_pGen_gravel, 0, 128);
	genStandardOre1(20, m_pGen_coal, 0, 128);
	genStandardOre1(20, m_pGen_iron, 0, 64);
	genStandardOre1(2, m_pGen_gold, 0, 32);
	genStandardOre1(8, m_pGen_redStone, 0, 16);
	genStandardOre1(1, m_pGen_diamond, 0, 16);
	genStandardOre2(1, m_pGen_lapis, 16, 16);
}

BiomeEndDecorator::BiomeEndDecorator(BiomeGenBase* pBiomeGenBase)
	: BiomeDecorator(pBiomeGenBase)
{
	m_pGen_spike = LordNew WorldGenSpikes(BLOCK_ID_WHITE_STONE);
}

BiomeEndDecorator::~BiomeEndDecorator()
{
	LordSafeDelete(m_pGen_spike);
}

void BiomeEndDecorator::decorate()
{
	generateOres();

	if (m_pRandom->nextInt(5) == 0)
	{
		int x = m_chunkX + m_pRandom->nextInt(16) + 8;
		int z = m_chunkZ + m_pRandom->nextInt(16) + 8;
		int y = m_pWorld->getTopSolidOrLiquidBlock(x, z);
		m_pGen_spike->generate(m_pWorld, *m_pRandom, x, y, z);
	}

	if (m_chunkX == 0 && m_chunkZ == 0)
	{
		/*
		EntityDragon var4 = new EntityDragon(this.currentWorld);
		var4.setLocationAndAngles(0.0D, 128.0D, 0.0D, this.randomGenerator.nextFloat() * 360.0F, 0.0F);
		this.currentWorld.spawnEntityInWorld(var4);
		*/
	}
}


}