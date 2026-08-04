/********************************************************************
filename: 	BiomeGen.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-11-11
*********************************************************************/
#ifndef __BIOME_GEN_HEADER__
#define __BIOME_GEN_HEADER__

#include "BM_Container_def.h"

namespace BLOCKMAN
{

class BiomeDecorator;
class WorldGenTrees;
class WorldGenBigTree;
class WorldGenForest;
class WorldGenSwamp;
class WorldGenerator;
class WorldGenTaiga1;
class WorldGenTaiga2;
class Random;
class World;

class BiomeGenBase : public ObjectAlloc
{
public:
	static BiomeGenArr biomeList;
	static BiomeGenBase* ocean;
	static BiomeGenBase* plains;
	static BiomeGenBase* desert;
	static BiomeGenBase* extremeHills;
	static BiomeGenBase* forest;
	static BiomeGenBase* taiga;
	static BiomeGenBase* swampland;
	static BiomeGenBase* river;
	static BiomeGenBase* hell;
	static BiomeGenBase* sky;
	static BiomeGenBase* frozenOcean;
	static BiomeGenBase* frozenRiver;
	static BiomeGenBase* icePlains;
	static BiomeGenBase* iceMountains;
	static BiomeGenBase* mushroomIsland;
	static BiomeGenBase* mushroomIslandShore;
	static BiomeGenBase* beach;
	static BiomeGenBase* desertHills;
	static BiomeGenBase* forestHills;
	static BiomeGenBase* taigaHills;
	static BiomeGenBase* extremeHillsEdge;
	static BiomeGenBase* jungle;
	static BiomeGenBase* jungleHills;

	static void initialize();
	static void uninitialize();

	String m_name;

	int m_color = 0;
	int m_BiomeColor = 0;

	/** The block expected to be on the top of this biome */
	i8 m_topBlock = 0;

	/** The block to fill spots in when not on the top */
	i8 m_fillerBlock = 0;
	
	/** The minimum height of this biome. Default 0.1. */
	float m_minHeight = 0.f;

	/** The maximum height of this biome. Default 0.3. */
	float m_maxHeight = 0.f;

	/** The temperature of this biome. */
	float m_temperature = 0.f;

	/** The rainfall in this biome. */
	float m_rainfall = 0.f;

	/** Color tint applied to water depending on biome */
	int m_waterColor = 0;

	/** The biome decorator. */
	BiomeDecorator* m_pDecorator = nullptr;

	/** The id number to this biome, and its index in the biomeList array. */
	int m_ID = 0;
 
protected:
	/** Holds the classes of IMobs (hostile mobs) that can be spawned in the biome.	*/
	//List spawnableMonsterList;

	/** Holds the classes of any creature that can be spawned in the biome as friendly creature. */
	//List spawnableCreatureList;

	/** Holds the classes of any aquatic creature that can be spawned in the water of the biome. */
	//List spawnableWaterCreatureList;
	//List spawnableCaveCreatureList;

	/** Set to true if snow is enabled for this biome. */
	bool m_bEnableSnow = false;

	/** Is true (default) if the biome support rain (desert and nether can't have rain) */
	bool m_bEnableRain = false;

	/** The generators. */
	WorldGenTrees*		m_pGen_trees = nullptr;
	WorldGenBigTree*	m_pGen_bigTrees = nullptr;
	WorldGenForest*		m_pGen_forest = nullptr;
	WorldGenSwamp*		m_pGen_swamp = nullptr;
	WorldGenTaiga1*		m_pGen_taiga1 = nullptr;
	WorldGenTaiga2*		m_pGen_taiga2 = nullptr;

	BiomeGenBase(int id);

	/** Allocate a new BiomeDecorator for this BiomeGenBase */
	BiomeDecorator* createBiomeDecorator();

	/** Sets the temperature and rainfall of this biome. */
	void setTemperatureRainfall(float temperature, float rainfall);
	void setMinMaxHeight(float minHeight, float maxHeight) { m_minHeight = minHeight; m_maxHeight = maxHeight; }
	void setDisableRain() {	m_bEnableRain = false; }
	void setEnableSnow() { m_bEnableSnow = true; }
	void setBiomeName(const String& str) { m_name = str; }
	void setBiomeColor(int color) { m_BiomeColor = color; }
	void setColor(int color) { m_color = color;	}

public:
	virtual ~BiomeGenBase();
	
	/** Gets a WorldGen appropriate for this biome. */
	virtual WorldGenerator* getRandomWorldGenForTrees(Random& rand);

	/** Gets a WorldGen appropriate for this biome. */
	virtual WorldGenerator* getRandomWorldGenForGrass(Random& rand);

	/** takes temperature, returns color */
	virtual int getSkyColorByTemp(float temperature);

	/** Returns the correspondent list of the EnumCreatureType informed. */
	// public List getSpawnableList(EnumCreatureType par1EnumCreatureType);

	/** Returns true if the biome have snowfall instead a normal rain. */
	bool getEnableSnow() { return m_bEnableSnow; }

	/** Return true if the biome supports lightning bolt spawn, either by have the bolts enabled and have rain enabled. */
	bool canSpawnLightningBolt() { return m_bEnableSnow ? false : m_bEnableRain; }

	/** Checks to see if the rainfall level of the biome is extremely high */
	bool isHighHumidity() {	return m_rainfall > 0.85F; }

	/** returns the chance a creature has to spawn. */
	float getSpawningChance() { return 0.1F; }

	/** Gets an integer representation of this biome's rainfall */
	int getIntRainfall() { return (int)(m_rainfall * 65536.0F); }

	/** Gets an integer representation of this biome's temperature */
	int getIntTemperature() { return (int)(m_temperature * 65536.0F); }

	/** Gets a floating point representation of this biome's rainfall */
	float getFloatRainfall() { return m_rainfall; }

	/** Gets a floating point representation of this biome's temperature */
	float getFloatTemperature() { return m_temperature; }

	/** add by zhouyou, type is the color multiplier type. */
	virtual int getColorMultiplierType() { return 0; }

	virtual void decorate(World* pWorld, Random& rand, int x, int z);
};

class BiomeGenDesert : public BiomeGenBase
{
public:
	BiomeGenDesert(int id);
	void decorate(World* pWorld, Random& rand, int x, int z);
};


class BiomeGenForest : public BiomeGenBase
{
public:
	BiomeGenForest(int id);
	/** Gets a WorldGen appropriate for this biome. */
	WorldGenerator* getRandomWorldGenForTrees(Random& rand);
};


class BiomeGenJungle : public BiomeGenBase
{
public:
	BiomeGenJungle(int id);
	virtual ~BiomeGenJungle();

	/** Gets a WorldGen appropriate for this biome.*/
	WorldGenerator* getRandomWorldGenForTrees(Random& rand);

	/** Gets a WorldGen appropriate for this biome. */
	WorldGenerator* getRandomWorldGenForGrass(Random& rand);

	void decorate(World* pWorld, Random& rand, int x, int z);

protected:
	WorldGenerator* m_genShrub = nullptr;
	WorldGenerator* m_genHugeTrees = nullptr;
	WorldGenerator* m_genTrees = nullptr;
};

class BiomeGenTaiga : public BiomeGenBase
{
public:
	BiomeGenTaiga(int id);
	/** Gets a WorldGen appropriate for this biome. */
	WorldGenerator* getRandomWorldGenForTrees(Random& rand);
};

class BiomeGenBeach : public BiomeGenBase
{
public:
	BiomeGenBeach(int id);
};

class BiomeGenEnd : public BiomeGenBase
{
public:
	BiomeGenEnd(int id);
	virtual ~BiomeGenEnd();
	virtual int getSkyColorByTemp(float temperature) { return 0; }
};

class BiomeGenHell : public BiomeGenBase
{
public:
	BiomeGenHell(int id);
};

class BiomeGenHills : public BiomeGenBase
{
protected:
	WorldGenerator* m_pGenerator = nullptr;

public:
	BiomeGenHills(int id);
	virtual ~BiomeGenHills();
	void decorate(World* pWorld, Random& rand, int x, int z);
};

class BiomeGenMushroomIsland : public BiomeGenBase
{
public:
	BiomeGenMushroomIsland(int id);
};

class BiomeGenOcean :public BiomeGenBase
{
public:
	BiomeGenOcean(int id);
};

class BiomeGenPlains : public BiomeGenBase
{
public:
	BiomeGenPlains(int id);
};

class BiomeGenRiver : public BiomeGenBase
{
public:
	BiomeGenRiver(int id);
};

class BiomeGenSnow : public BiomeGenBase
{
public:
	BiomeGenSnow(int id);
};

class BiomeGenSwamp : public BiomeGenBase
{
public:
	BiomeGenSwamp(int id);
	WorldGenerator* getRandomWorldGenForTrees(Random& rand);
	virtual int getColorMultiplierType() { return 1; }
};

}

#endif