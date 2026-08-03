#include "BiomeGen.h"
#include "BM_TypeDef.h"
#include "WorldGenerator.h"
#include "BiomeDecorator.h"

#include "World/World.h"

namespace BLOCKMAN
{
BiomeGenArr BiomeGenBase::biomeList;

BiomeGenBase* BiomeGenBase::ocean			= NULL;
BiomeGenBase* BiomeGenBase::plains			= NULL;
BiomeGenBase* BiomeGenBase::desert			= NULL;
BiomeGenBase* BiomeGenBase::extremeHills	= NULL;
BiomeGenBase* BiomeGenBase::forest			= NULL;
BiomeGenBase* BiomeGenBase::taiga			= NULL;
BiomeGenBase* BiomeGenBase::swampland		= NULL;
BiomeGenBase* BiomeGenBase::river			= NULL;	
BiomeGenBase* BiomeGenBase::hell			= NULL;
BiomeGenBase* BiomeGenBase::sky				= NULL;
BiomeGenBase* BiomeGenBase::frozenOcean		= NULL;
BiomeGenBase* BiomeGenBase::frozenRiver		= NULL;
BiomeGenBase* BiomeGenBase::icePlains		= NULL;
BiomeGenBase* BiomeGenBase::iceMountains	= NULL;
BiomeGenBase* BiomeGenBase::mushroomIsland	= NULL;
BiomeGenBase* BiomeGenBase::mushroomIslandShore = NULL;
BiomeGenBase* BiomeGenBase::beach			= NULL;
BiomeGenBase* BiomeGenBase::desertHills		= NULL;
BiomeGenBase* BiomeGenBase::forestHills		= NULL;
BiomeGenBase* BiomeGenBase::taigaHills		= NULL;
BiomeGenBase* BiomeGenBase::extremeHillsEdge = NULL;
BiomeGenBase* BiomeGenBase::jungle			= NULL;
BiomeGenBase* BiomeGenBase::jungleHills		= NULL;

void BiomeGenBase::initialize()
{
	biomeList.resize(64, NULL);
	ocean = LordNew BiomeGenOcean(0);	
	ocean->setColor(112); 
	ocean->setBiomeName("Ocean");
	ocean->setMinMaxHeight(-1.F, 0.4F);

	plains = LordNew BiomeGenPlains(1);
	plains->setColor(9286496); 
	plains->setBiomeName("Plains");
	plains->setTemperatureRainfall(0.8F, 0.4F);

	desert = LordNew BiomeGenDesert(2);
	desert->setColor(16421912); 
	desert->setBiomeName("Desert"); 
	desert->setDisableRain();
	desert->setTemperatureRainfall(2.0F, 0.0F);
	desert->setMinMaxHeight(0.1F, 0.2F);

	extremeHills = LordNew BiomeGenHills(3);
	extremeHills->setColor(6316128); 
	extremeHills->setBiomeName("Extreme Hills"); 
	extremeHills->setMinMaxHeight(0.3F, 1.5F);
	extremeHills->setTemperatureRainfall(0.2F, 0.3F);

	forest = LordNew BiomeGenForest(4);
	forest->setColor(353825); 
	forest->setBiomeName("Forest");
	forest->setBiomeColor(5159473); 
	forest->setTemperatureRainfall(0.7F, 0.8F);

	taiga = LordNew BiomeGenTaiga(5);
	taiga->setColor(747097);
	taiga->setBiomeName("Taiga"); 
	taiga->setBiomeColor(5159473); 
	taiga->setEnableSnow();
	taiga->setTemperatureRainfall(0.05F, 0.8F);
	taiga->setMinMaxHeight(0.1F, 0.4F);

	swampland = LordNew BiomeGenSwamp(6);
	swampland->setColor(522674);
	swampland->setBiomeName("Swampland"); 
	swampland->setBiomeColor(9154376); 
	swampland->setMinMaxHeight(-0.2F, 0.1F); 
	swampland->setTemperatureRainfall(0.8F, 0.9F);

	river = LordNew BiomeGenRiver(7);
	river->setColor(255);
	river->setBiomeName("River"); 
	river->setMinMaxHeight(-0.5F, 0.0F);

	hell = LordNew BiomeGenHell(8);
	hell->setColor(16711680);
	hell->setBiomeName("Hell"); 
	hell->setDisableRain(); 
	hell->setTemperatureRainfall(2.0F, 0.0F);

	sky = LordNew BiomeGenEnd(9);
	sky->setColor(8421631); 
	sky->setBiomeName("Sky");
	sky->setDisableRain();

	frozenOcean = LordNew BiomeGenOcean(10); 
	frozenOcean->setColor(9474208); 
	frozenOcean->setBiomeName("FrozenOcean");
	frozenOcean->setEnableSnow(); 
	frozenOcean->setMinMaxHeight(-1.0F, 0.5F); 
	frozenOcean->setTemperatureRainfall(0.0F, 0.5F);

	frozenRiver = LordNew BiomeGenRiver(11);
	frozenRiver->setColor(10526975);
	frozenRiver->setBiomeName("FrozenRiver");
	frozenRiver->setEnableSnow(); 
	frozenRiver->setMinMaxHeight(-0.5F, 0.0F); 
	frozenRiver->setTemperatureRainfall(0.0F, 0.5F);

	icePlains = LordNew BiomeGenSnow(12);
	icePlains->setColor(16777215);
	icePlains->setBiomeName("Ice Plains");
	icePlains->setEnableSnow(); 
	icePlains->setTemperatureRainfall(0.0F, 0.5F);

	iceMountains = LordNew BiomeGenSnow(13);
	iceMountains->setColor(10526880);
	iceMountains->setBiomeName("Ice Mountains");
	iceMountains->setEnableSnow();
	iceMountains->setMinMaxHeight(0.3F, 1.3F);
	iceMountains->setTemperatureRainfall(0.0F, 0.5F);

	mushroomIsland = LordNew BiomeGenMushroomIsland(14); 
	mushroomIsland->setColor(16711935);
	mushroomIsland->setBiomeName("MushroomIsland");
	mushroomIsland->setTemperatureRainfall(0.9F, 1.0F);
	mushroomIsland->setMinMaxHeight(0.2F, 1.0F);

	mushroomIslandShore = LordNew BiomeGenMushroomIsland(15);
	mushroomIslandShore->setColor(10486015);
	mushroomIslandShore->setBiomeName("MushroomIslandShore");
	mushroomIslandShore->setTemperatureRainfall(0.9F, 1.0F);
	mushroomIslandShore->setMinMaxHeight(-1.0F, 0.1F);

	beach = LordNew BiomeGenBeach(16);
	beach->setColor(16440917);
	beach->setBiomeName("Beach");
	beach->setTemperatureRainfall(0.8F, 0.4F);
	beach->setMinMaxHeight(0.0F, 0.1F);

	desertHills = LordNew BiomeGenDesert(17);
	desertHills->setColor(13786898); 
	desertHills->setBiomeName("DesertHills");
	desertHills->setDisableRain(); 
	desertHills->setTemperatureRainfall(2.0F, 0.0F);
	desertHills->setMinMaxHeight(0.3F, 0.8F);

	forestHills = LordNew BiomeGenForest(18); 
	forestHills->setColor(2250012); 
	forestHills->setBiomeName("ForestHills");
	forestHills->setBiomeColor(5159473);
	forestHills->setTemperatureRainfall(0.7F, 0.8F); 
	forestHills->setMinMaxHeight(0.3F, 0.7F);

	taigaHills = LordNew BiomeGenTaiga(19);
	taigaHills->setColor(1456435);
	taigaHills->setBiomeName("TaigaHills");
	taigaHills->setEnableSnow(); 
	taigaHills->setBiomeColor(5159473);
	taigaHills->setTemperatureRainfall(0.05F, 0.8F);
	taigaHills->setMinMaxHeight(0.3F, 0.8F);

	extremeHillsEdge = LordNew BiomeGenHills(20);
	extremeHillsEdge->setColor(7501978);
	extremeHillsEdge->setBiomeName("Extreme Hills Edge");
	extremeHillsEdge->setMinMaxHeight(0.2F, 0.8F);
	extremeHillsEdge->setTemperatureRainfall(0.2F, 0.3F);

	jungle = LordNew BiomeGenJungle(21);
	jungle->setColor(5470985);
	jungle->setBiomeName("Jungle");
	jungle->setBiomeColor(5470985);
	jungle->setTemperatureRainfall(1.2F, 0.9F); 
	jungle->setMinMaxHeight(0.2F, 0.4F);

	jungleHills = LordNew BiomeGenJungle(22);
	jungleHills->setColor(2900485);
	jungleHills->setBiomeName("JungleHills");
	jungleHills->setBiomeColor(5470985);
	jungleHills->setTemperatureRainfall(1.2F, 0.9F);
	jungleHills->setMinMaxHeight(1.8F, 0.5F);
}

void BiomeGenBase::uninitialize()
{
	LordSafeDelete(jungleHills);
	LordSafeDelete(jungle);
	LordSafeDelete(extremeHillsEdge);
	LordSafeDelete(taigaHills);
	LordSafeDelete(forestHills);
	LordSafeDelete(desertHills);
	LordSafeDelete(beach);
	LordSafeDelete(mushroomIslandShore);
	LordSafeDelete(mushroomIsland);
	LordSafeDelete(iceMountains);
	LordSafeDelete(icePlains);
	LordSafeDelete(frozenRiver);
	LordSafeDelete(frozenOcean);
	LordSafeDelete(sky);
	LordSafeDelete(hell);
	LordSafeDelete(river);
	LordSafeDelete(swampland);
	LordSafeDelete(taiga);
	LordSafeDelete(forest);
	LordSafeDelete(extremeHills);
	LordSafeDelete(desert);
	LordSafeDelete(plains);
	LordSafeDelete(ocean);
}

BiomeGenBase::BiomeGenBase(int id)
{
	m_topBlock = BLOCK_ID_GRASS;
	m_fillerBlock = BLOCK_ID_DIRT;
	m_BiomeColor = 0x4EE031;
	m_minHeight = 0.1F;
	m_maxHeight = 0.3F;
	m_temperature = 0.5F;
	m_rainfall = 0.5F;
	m_waterColor = 0xFFFFFF;
	//spawnableMonsterList = new ArrayList();
	//spawnableCreatureList = new ArrayList();
	//spawnableWaterCreatureList = new ArrayList();
	//spawnableCaveCreatureList = new ArrayList();
	m_bEnableRain = true;
	m_pGen_trees	= LordNew WorldGenTrees(false);
	m_pGen_bigTrees = LordNew WorldGenBigTree(false);
	m_pGen_forest	= LordNew WorldGenForest(false);
	m_pGen_swamp	= LordNew WorldGenSwamp();
	m_pGen_taiga1 = LordNew WorldGenTaiga1();
	m_pGen_taiga2 = LordNew WorldGenTaiga2(false);
	m_ID = id;
	biomeList[id] = this;
	m_pDecorator = createBiomeDecorator();
}

BiomeGenBase::~BiomeGenBase()
{
	LordSafeDelete(m_pGen_trees);
	LordSafeDelete(m_pGen_bigTrees);
	LordSafeDelete(m_pGen_forest);
	LordSafeDelete(m_pGen_swamp);
	LordSafeDelete(m_pDecorator);
	LordSafeDelete(m_pGen_taiga1);
	LordSafeDelete(m_pGen_taiga2);
}

BiomeDecorator* BiomeGenBase::createBiomeDecorator()
{
	return LordNew BiomeDecorator(this);
}

void BiomeGenBase::setTemperatureRainfall(float temperature, float rainfall)
{
	if (temperature > 0.1F && temperature < 0.2F)
	{
		LordException("Please avoid temperatures in the range 0.1 - 0.2 because of snow");
	}
	m_temperature = temperature;
	m_rainfall = rainfall;
}

WorldGenerator* BiomeGenBase::getRandomWorldGenForTrees(Random& rand)
{
	if (rand.nextInt(10) == 0)
		return m_pGen_bigTrees;
	else
		return m_pGen_trees;
}

WorldGenerator* BiomeGenBase::getRandomWorldGenForGrass(Random& rand)
{
	return LordNew WorldGenTallGrass(BLOCK_ID_TALL_GRASS, 1);
}

int BiomeGenBase::getSkyColorByTemp(float temperature)
{
	temperature /= 3.0F;

	if (temperature < -1.0F)
	{
		temperature = -1.0F;
	}

	if (temperature > 1.0F)
	{
		temperature = 1.0F;
	}

	return Color::HSBtoRGB(0.62222224F - temperature * 0.05F, 0.5F + temperature * 0.1F, 1.0F);
}

void BiomeGenBase::decorate(World* pWorld, Random& rand, int x, int z)
{
	m_pDecorator->decorate(pWorld, rand, x, z);
}

BiomeGenDesert::BiomeGenDesert(int id)
	: BiomeGenBase(id)
{
	// spawnableCreatureList.clear();
	m_topBlock = BLOCK_ID_SAND;
	m_fillerBlock = BLOCK_ID_SAND;
	m_pDecorator->m_amount_trees	= -999;
	m_pDecorator->m_amount_deadBush = 2;
	m_pDecorator->m_amount_reeds	= 50;
	m_pDecorator->m_amount_cactus	= 10;
}

void BiomeGenDesert::decorate(World* pWorld, Random& rand, int x, int z)
{
	BiomeGenBase::decorate(pWorld, rand, x, z);

	if (rand.nextInt(1000) == 0)
	{
		int ix = x + rand.nextInt(16) + 8;
		int iz = z + rand.nextInt(16) + 8;
		WorldGenDesertWells* pGenerator = LordNew WorldGenDesertWells();
		pGenerator->generate(pWorld, rand, ix, pWorld->getHeightValue(ix, iz) + 1, iz);
	}
}

BiomeGenForest::BiomeGenForest(int id)
	: BiomeGenBase(id)
{
	// spawnableCreatureList.add(new SpawnListEntry(EntityWolf.class, 5, 4, 4));
	m_pDecorator->m_amount_trees = 10;
	m_pDecorator->m_amount_grass = 2;
}

WorldGenerator* BiomeGenForest::getRandomWorldGenForTrees(Random& rand)
{
	if (rand.nextInt(5) == 0)
	{
		return m_pGen_forest;
	}
	else if(rand.nextInt(10) == 0)
	{
		return m_pGen_bigTrees;
	}
	else
	{
		return m_pGen_trees;
	}
}

BiomeGenJungle::BiomeGenJungle(int id)
	: BiomeGenBase(id)
{
	m_pDecorator->m_amount_trees = 50;
	m_pDecorator->m_amount_grass = 25;
	m_pDecorator->m_amount_flowers = 4;
	// spawnableMonsterList.add(new SpawnListEntry(EntityOcelot.class, 2, 1, 1));
	// spawnableCreatureList.add(new SpawnListEntry(EntityChicken.class, 10, 4, 4));
	m_genShrub		= LordNew WorldGenShrub(3, 0);
	m_genHugeTrees	= LordNew WorldGenHugeTrees(false, 10, 3, 3);
	m_genTrees		= LordNew WorldGenTrees(false, 4, 3, 3, true);
}

BiomeGenJungle::~BiomeGenJungle()
{
	LordSafeDelete(m_genShrub);
	LordSafeDelete(m_genHugeTrees);
	LordSafeDelete(m_genTrees);
}

WorldGenerator* BiomeGenJungle::getRandomWorldGenForTrees(Random& rand)
{
	if (rand.nextInt(10) == 0)
	{
		return m_pGen_bigTrees;
	}
	else if (rand.nextInt(2) == 0)
	{
		return m_genShrub;
	}
	else if(rand.nextInt(3) == 0)
	{
		WorldGenHugeTrees* genHugeTrees = dynamic_cast<WorldGenHugeTrees*>(m_genHugeTrees);
		genHugeTrees->setParam(10 + rand.nextInt(20), 3, 3);
		return m_genHugeTrees;
	}
	else
	{
		WorldGenTrees* genTrees = dynamic_cast<WorldGenTrees*>(m_genTrees);
		genTrees->setParam(4 + rand.nextInt(7), 3, 3, true);
		return m_genTrees;
	}
	return NULL;
}

WorldGenerator* BiomeGenJungle::getRandomWorldGenForGrass(Random& rand)
{
	if (rand.nextInt(4) == 0)
		return LordNew WorldGenTallGrass(BLOCK_ID_TALL_GRASS, 2);
	else
		return LordNew WorldGenTallGrass(BLOCK_ID_TALL_GRASS, 1);
}

void BiomeGenJungle::decorate(World* pWorld, Random& rand, int x, int z)
{
	BiomeGenBase::decorate(pWorld, rand, x, z);
	WorldGenVines* pGenerator = LordNew WorldGenVines();

	for (int i = 0; i < 50; ++i)
	{
		int ix = x + rand.nextInt(16) + 8;
		int iy = 64;
		int iz = z + rand.nextInt(16) + 8;
		pGenerator->generate(pWorld, rand, ix, iy, iz);
	}
	LordSafeDelete(pGenerator);
}

BiomeGenTaiga::BiomeGenTaiga(int id)
	: BiomeGenBase(id)
{
	// spawnableCreatureList.add(new SpawnListEntry(EntityWolf.class, 8, 4, 4));
	m_pDecorator->m_amount_trees = 10;
	m_pDecorator->m_amount_grass = 1;
}

WorldGenerator* BiomeGenTaiga::getRandomWorldGenForTrees(Random& rand)
{
	if (rand.nextInt(3) == 0)
		return m_pGen_taiga1;
	else
		return m_pGen_taiga2;
}

BiomeGenBeach::BiomeGenBeach(int id)
	: BiomeGenBase(id)
{
	// spawnableCreatureList.clear();
	m_topBlock = BLOCK_ID_SAND;
	m_fillerBlock = BLOCK_ID_SAND;
	m_pDecorator->m_amount_trees	= -999;
	m_pDecorator->m_amount_deadBush = 0;
	m_pDecorator->m_amount_reeds	= 0;
	m_pDecorator->m_amount_cactus	= 0;
}

BiomeGenEnd::BiomeGenEnd(int id)
	: BiomeGenBase(id)
{
	// spawnableMonsterList.clear();
	// spawnableCreatureList.clear();
	// spawnableWaterCreatureList.clear();
	// spawnableCaveCreatureList.clear();
	// spawnableMonsterList.add(new SpawnListEntry(EntityEnderman.class, 10, 4, 4));
	m_topBlock = BLOCK_ID_DIRT;
	m_fillerBlock = BLOCK_ID_DIRT;
	LordSafeDelete(m_pDecorator); // fuck!
	m_pDecorator = LordNew BiomeEndDecorator(this);
}

BiomeGenEnd::~BiomeGenEnd()
{
	LordSafeDelete(m_pDecorator);
}

BiomeGenHell::BiomeGenHell(int id)
	: BiomeGenBase(id)
{
	/*
	spawnableMonsterList.clear();
	spawnableCreatureList.clear();
	spawnableWaterCreatureList.clear();
	spawnableCaveCreatureList.clear();
	spawnableMonsterList.add(new SpawnListEntry(EntityGhast.class, 50, 4, 4));
	spawnableMonsterList.add(new SpawnListEntry(EntityPigZombie.class, 100, 4, 4));
	spawnableMonsterList.add(new SpawnListEntry(EntityMagmaCube.class, 1, 4, 4));
	*/
}

BiomeGenHills::BiomeGenHills(int id)
	: BiomeGenBase(id)
{
	m_pGenerator = LordNew WorldGenMinable(BLOCK_ID_SILVER_FISH, 8);
}

BiomeGenHills::~BiomeGenHills()
{
	LordSafeDelete(m_pGenerator);
}

void BiomeGenHills::decorate(World* pWorld, Random& rand, int x, int z)
{
	BiomeGenBase::decorate(pWorld, rand, x, z);
	int count = 3 + rand.nextInt(6);
	int ix;
	int iy;
	int iz;

	for (int i = 0; i < count; ++i)
	{
		ix = x + rand.nextInt(16);
		iy = rand.nextInt(28) + 4;
		iz = z + rand.nextInt(16);
		int blockID = pWorld->getBlockId(BlockPos(ix, iy, iz));

		if (blockID == BLOCK_ID_STONE)
		{
			pWorld->setBlock(BlockPos(ix, iy, iz), BLOCK_ID_ORE_EMERALD, 0, 2);
		}
	}

	for (int i = 0; i < 7; ++i)
	{
		ix = x + rand.nextInt(16);
		iy = rand.nextInt(64);
		iz = z + rand.nextInt(16);
		m_pGenerator->generate(pWorld, rand, ix, iy, iz);
	}
}

BiomeGenMushroomIsland::BiomeGenMushroomIsland(int id)
	: BiomeGenBase(id)
{
	m_pDecorator->m_amount_trees	= -100;
	m_pDecorator->m_amount_flowers	= -100;
	m_pDecorator->m_amount_grass	= -100;
	m_pDecorator->m_amount_mushrooms = 1;
	m_pDecorator->m_amount_bigMushroom = 1;
	m_topBlock = BLOCK_ID_MYCELIUM;
	// spawnableMonsterList.clear();
	// spawnableCreatureList.clear();
	// spawnableWaterCreatureList.clear();
	// spawnableCreatureList.add(new SpawnListEntry(EntityMooshroom.class, 8, 4, 8));
}

BiomeGenOcean::BiomeGenOcean(int id)
	: BiomeGenBase(id)
{
	// spawnableCreatureList.clear();
}

BiomeGenPlains::BiomeGenPlains(int id)
	: BiomeGenBase(id)
{
	// spawnableCreatureList.add(new SpawnListEntry(EntityHorse.class, 5, 2, 6));
	m_pDecorator->m_amount_trees	= -999;
	m_pDecorator->m_amount_flowers	= 4;
	m_pDecorator->m_amount_grass	= 10;
}

BiomeGenRiver::BiomeGenRiver(int id)
	: BiomeGenBase(id)
{
	//this.spawnableCreatureList.clear();
}

BiomeGenSnow::BiomeGenSnow(int id)
	: BiomeGenBase(id)
{
}

BiomeGenSwamp::BiomeGenSwamp(int id)
	: BiomeGenBase(id)
{
	m_pDecorator->m_amount_trees		= 2;
	m_pDecorator->m_amount_flowers		= -999;
	m_pDecorator->m_amount_deadBush		= 1;
	m_pDecorator->m_amount_mushrooms	= 8;
	m_pDecorator->m_amount_reeds		= 10;
	m_pDecorator->m_amount_clay			= 1;
	m_pDecorator->m_amount_waterlily	= 4;
	m_waterColor = 0xE0FFAE;
	// spawnableMonsterList.add(new SpawnListEntry(EntitySlime.class, 1, 1, 1));
}

WorldGenerator* BiomeGenSwamp::getRandomWorldGenForTrees(Random& rand)
{
	return m_pGen_swamp;
}

}