/********************************************************************
filename: 	BiomeDecorator.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-11-12
*********************************************************************/
#ifndef __BIOME_DECORATOR_HEADER__
#define __BIOME_DECORATOR_HEADER__

#include "Util/Random.h"

namespace BLOCKMAN
{

class World;
class BiomeGenBase;
class WorldGenerator;

class BiomeDecorator : public ObjectAlloc
{
public:
	BiomeDecorator(BiomeGenBase* pBiome);
	virtual ~BiomeDecorator();

	/** Decorates the world. Calls code that was formerly (pre-1.8) in ChunkProviderGenerate.populate */
	virtual void decorate(World* pWorld, Random& rand, int x, int z);

protected:
	/** The method that does the work of actually decorating chunks	*/
	void decorate();

	/** Standard ore generation helper. Generates most ores.*/
	void genStandardOre1(int blockCount, WorldGenerator* pWorldGen, int minHeight, int maxHeight);

	/** Standard ore generation helper. Generates Lapis Lazuli. */
	void genStandardOre2(int blockCount, WorldGenerator* pWorldGen, int minHeight, int maxHeight);

	/** Generates ores in the current chunk */
	void generateOres();

public:
	/** True if decorator should generate surface lava & water */
	bool m_bGenerateLakes = false;

protected:
	/** The world the BiomeDecorator is currently decorating */
	World* m_pWorld = nullptr;

	/** The Biome Decorator's random number generator. */
	Random* m_pRandom = nullptr;

	/** The X-coordinate of the chunk currently being decorated */
	int m_chunkX = 0;

	/** The Z-coordinate of the chunk currently being decorated */
	int m_chunkZ = 0;

	/** The biome generator object. */
	BiomeGenBase* m_pBiome = nullptr;

	/** The clay generator. */
	WorldGenerator* m_pGen_clay = nullptr;
	WorldGenerator* m_pGen_sand = nullptr;
	WorldGenerator* m_pGen_gravelAsSand = nullptr;
	WorldGenerator* m_pGen_dirt = nullptr;
	WorldGenerator* m_pGen_gravel = nullptr;
	WorldGenerator* m_pGen_coal = nullptr;
	WorldGenerator* m_pGen_iron = nullptr;
	WorldGenerator* m_pGen_gold = nullptr;
	WorldGenerator* m_pGen_redStone = nullptr;
	WorldGenerator* m_pGen_diamond = nullptr;
	WorldGenerator* m_pGen_lapis = nullptr;
	WorldGenerator* m_pGen_plant_yellow = nullptr;
	WorldGenerator* m_pGen_plant_red = nullptr;
	WorldGenerator* m_pGen_deadBush = nullptr;
	WorldGenerator* m_pGen_mushroom_brown = nullptr;
	WorldGenerator* m_pGen_mushroom_red = nullptr;
	WorldGenerator* m_pGen_bigMushroom = nullptr;
	WorldGenerator* m_pGen_reed = nullptr;
	WorldGenerator* m_pGen_cactus = nullptr;
	WorldGenerator* m_pGen_waterlily = nullptr;
	WorldGenerator* m_pGen_pumpkim = nullptr;
	WorldGenerator* m_pGen_waterMoving = nullptr;
	WorldGenerator* m_pGen_lavaMoving = nullptr;

public:
	/** Amount per chunk. */
	int m_amount_waterlily = 0;
	int m_amount_trees = 0;
	int m_amount_flowers = 0;
	int m_amount_grass = 0;
	int m_amount_deadBush = 0;
	int m_amount_mushrooms = 0;
	int m_amount_reeds = 0;
	int m_amount_cactus = 0;
	int m_amount_sand = 0;
	int m_amount_gravelAsSand = 0;
	int m_amount_clay = 0;
	int m_amount_bigMushroom = 0;
};


class BiomeEndDecorator : public BiomeDecorator
{
public:
	BiomeEndDecorator(BiomeGenBase* pBiomeGenBase);
	~BiomeEndDecorator();
	void decorate();

protected:
	WorldGenerator* m_pGen_spike = nullptr;
};

}

#endif