#include "StatList.h"
#include "BM_TypeDef.h"

#include "Stats.h"
#include "Item/Recipes.h"
#include "Item/Item.h"
#include "Block/Block.h"
#include "Block/BlockManager.h"
#include "Item/Recipes.h"
#include "Item/ItemStack.h"
#include "Inventory/CraftingManager.h"

namespace BLOCKMAN
{

StatList::StatBaseMap StatList::oneShotStats;
StatList::StatBaseLst StatList::allStats;
StatList::StatBaseLst StatList::generalStats;
StatList::ItemStateArr StatList::itemStats;
StatList::ItemStateSet StatList::objectMineStats;

StatBase* StatList::startGameStat = NULL;			
StatBase* StatList::createWorldStat = NULL;
StatBase* StatList::loadWorldStat = NULL;
StatBase* StatList::joinMultiplayerStat = NULL;
StatBase* StatList::leaveGameStat = NULL;
StatBase* StatList::minutesPlayedStat = NULL;
StatBase* StatList::distanceWalkedStat = NULL;
StatBase* StatList::distanceSwumStat = NULL;
StatBase* StatList::distanceFallenStat = NULL;
StatBase* StatList::distanceClimbedStat = NULL;
StatBase* StatList::distanceFlownStat = NULL;
StatBase* StatList::distanceDoveStat = NULL;
StatBase* StatList::distanceByMinecartStat = NULL;
StatBase* StatList::distanceByBoatStat = NULL;
StatBase* StatList::distanceByPigStat = NULL;
StatBase* StatList::jumpStat = NULL;
StatBase* StatList::dropStat = NULL;
StatBase* StatList::damageDealtStat = NULL;
StatBase* StatList::damageTakenStat = NULL;
StatBase* StatList::deathsStat = NULL;
StatBase* StatList::mobKillsStat = NULL;
StatBase* StatList::playerKillsStat = NULL;
StatBase* StatList::fishCaughtStat = NULL;
StatBase** StatList::mineBlockStatArray = NULL;
StatBase** StatList::objectCraftStats = NULL;
StatBase** StatList::objectUseStats = NULL;
StatBase** StatList::objectBreakStats = NULL;
bool StatList::blockStatsInitialized = false;
bool StatList::itemStatsInitialized = false;

void StatList::initialize()
{
	startGameStat			= LordNew StatBasic(1000, "stat.startGame"); startGameStat->initIndependentStat(); startGameStat->registerStat();
	createWorldStat			= LordNew StatBasic(1001, "stat.createWorld"); createWorldStat->initIndependentStat(); createWorldStat->registerStat();
	loadWorldStat			= LordNew StatBasic(1002, "stat.loadWorld"); loadWorldStat->initIndependentStat(); loadWorldStat->registerStat();
	joinMultiplayerStat		= LordNew StatBasic(1003, "stat.joinMultiplayer"); joinMultiplayerStat->initIndependentStat(); joinMultiplayerStat->registerStat();
	leaveGameStat			= LordNew StatBasic(1004, "stat.leaveGame"); leaveGameStat->initIndependentStat(); leaveGameStat->registerStat();
	minutesPlayedStat		= LordNew StatBasic(1100, "stat.playOneMinute", StatBase::timeStatType); minutesPlayedStat->initIndependentStat(); minutesPlayedStat->registerStat();
	distanceWalkedStat		= LordNew StatBasic(2000, "stat.walkOneCm", StatBase::distanceStatType); distanceWalkedStat->initIndependentStat(); distanceWalkedStat->registerStat();
	distanceSwumStat		= LordNew StatBasic(2001, "stat.swimOneCm", StatBase::distanceStatType); distanceSwumStat->initIndependentStat(); distanceSwumStat->registerStat();
	distanceFallenStat		= LordNew StatBasic(2002, "stat.fallOneCm", StatBase::distanceStatType); distanceFallenStat->initIndependentStat(); distanceFallenStat->registerStat();
	distanceClimbedStat		= LordNew StatBasic(2003, "stat.climbOneCm", StatBase::distanceStatType); distanceClimbedStat->initIndependentStat(); distanceClimbedStat->registerStat();
	distanceFlownStat		= LordNew StatBasic(2004, "stat.flyOneCm", StatBase::distanceStatType); distanceFlownStat->initIndependentStat(); distanceFlownStat->registerStat();
	distanceDoveStat		= LordNew StatBasic(2005, "stat.diveOneCm", StatBase::distanceStatType); distanceDoveStat->initIndependentStat(); distanceDoveStat->registerStat();
	distanceByMinecartStat	= LordNew StatBasic(2006, "stat.minecartOneCm", StatBase::distanceStatType); distanceByMinecartStat->initIndependentStat(); distanceByMinecartStat->registerStat();
	distanceByBoatStat		= LordNew StatBasic(2007, "stat.boatOneCm", StatBase::distanceStatType); distanceByBoatStat->initIndependentStat(); distanceByBoatStat->registerStat();
	distanceByPigStat		= LordNew StatBasic(2008, "stat.pigOneCm", StatBase::distanceStatType); distanceByPigStat->initIndependentStat(); distanceByPigStat->registerStat();
	jumpStat				= LordNew StatBasic(2010, "stat.jump"); jumpStat->initIndependentStat(); jumpStat->registerStat();
	dropStat				= LordNew StatBasic(2011, "stat.drop"); dropStat->initIndependentStat(); dropStat->registerStat();
	damageDealtStat			= LordNew StatBasic(2020, "stat.damageDealt", StatBase::field_111202_k); damageDealtStat->registerStat();
	damageTakenStat			= LordNew StatBasic(2021, "stat.damageTaken", StatBase::field_111202_k); damageTakenStat->registerStat();
	deathsStat				= LordNew StatBasic(2022, "stat.deaths"); deathsStat->registerStat();
	mobKillsStat			= LordNew StatBasic(2023, "stat.mobKills"); mobKillsStat->registerStat();
	playerKillsStat			= LordNew StatBasic(2024, "stat.playerKills"); playerKillsStat->registerStat();
	fishCaughtStat			= LordNew StatBasic(2025, "stat.fishCaught"); fishCaughtStat->registerStat();

	mineBlockStatArray = initMinableStats("stat.mineBlock", 16777216);

	initStats();
	initBreakableStats();
}

void StatList::unInitialize()
{
	for (ItemStateArr::iterator it = itemStats.begin(); it != itemStats.end(); ++it)
	{
		StatCrafting* pCrafting = *it;
		if (!pCrafting)
			continue;
		LordDelete(pCrafting);
	}
	itemStats.clear();
    oneShotStats.clear();

	for (int i = 0; i < MAX_STAT_SIZE; ++i)
		LordSafeDelete(objectCraftStats[i]);
	LordSafeFree(objectCraftStats);

	for (int i = 0; i < MAX_STAT_SIZE; ++i)
		LordSafeDelete(objectBreakStats[i]);
	LordSafeFree(objectBreakStats);

	for (int i = 0; i < MAX_STAT_SIZE; ++i)
		LordSafeDelete(mineBlockStatArray[i]);
	LordSafeFree(mineBlockStatArray);

	LordSafeFree(objectUseStats);

	LordSafeDelete(startGameStat);
	LordSafeDelete(createWorldStat);
	LordSafeDelete(loadWorldStat);
	LordSafeDelete(joinMultiplayerStat);
	LordSafeDelete(leaveGameStat);
	LordSafeDelete(minutesPlayedStat);
	LordSafeDelete(distanceWalkedStat);
	LordSafeDelete(distanceSwumStat);
	LordSafeDelete(distanceFallenStat);
	LordSafeDelete(distanceClimbedStat);
	LordSafeDelete(distanceFlownStat);
	LordSafeDelete(distanceDoveStat);
	LordSafeDelete(distanceByMinecartStat);
	LordSafeDelete(distanceByBoatStat);
	LordSafeDelete(distanceByPigStat);
	LordSafeDelete(jumpStat);
	LordSafeDelete(dropStat);
	LordSafeDelete(damageDealtStat);
	LordSafeDelete(damageTakenStat);
	LordSafeDelete(deathsStat);
	LordSafeDelete(mobKillsStat);
	LordSafeDelete(playerKillsStat);
	LordSafeDelete(fishCaughtStat);

	// delete the FurnaceRecipes.
	LordDelete(FurnaceRecipes::smelting());
}

void StatList::initBreakableStats()
{
	objectUseStats = initUsableStats(objectUseStats, "stat.useItem", 16908288, 0, 256);
	objectBreakStats = initBreakStats(objectBreakStats, "stat.breakItem", 16973824, 0, 256);
	blockStatsInitialized = true;
    initCraftableStats();
}

void StatList::initStats()
{
	objectUseStats = initUsableStats(objectUseStats, "stat.useItem", 16908288, 256, 512); // 497
	objectBreakStats = initBreakStats(objectBreakStats, "stat.breakItem", 16973824, 256, 512); //398
	itemStatsInitialized = true;
    initCraftableStats();
}

void StatList::initCraftableStats()
{
	typedef set<int>::type IDS;
	if (blockStatsInitialized && itemStatsInitialized)
	{
		IDS ids;
		CraftingManager::RecipeList& recipes = CraftingManager::Instance()->getRecipeList();

		for (CraftingManager::RecipeList::iterator it = recipes.begin(); it != recipes.end(); ++it)
		{
			IRecipe* pRecipe = *it;

			if (pRecipe->getRecipeOutput())
			{
				ids.insert(pRecipe->getRecipeOutput()->itemID);
			}
		}

		BlockItemStackMap& blockStackMap = FurnaceRecipes::smelting()->getSmeltingList();
		for (BlockItemStackMap::iterator it = blockStackMap.begin(); it != blockStackMap.end(); ++it)
		{
			ItemStackPtr pStack = it->second;
			if (pStack)
				ids.insert(pStack->itemID);
		}
		
		objectCraftStats = (StatBase**)LordMalloc(sizeof(StatBase*)* MAX_STAT_SIZE);
		memset(objectCraftStats, 0, sizeof(StatBase*)* MAX_STAT_SIZE);

		for (IDS::iterator it = ids.begin(); it != ids.end(); ++it)
		{
			int id = *it;
			if (Item::itemsList[id])
			{
				// String var3 = StatCollector.translateToLocalFormatted("stat.craftItem", new Object[]{ Item.itemsList[var5.intValue()].getStatName() });
				// todo.
				String craftname = Item::itemsList[id]->getStatName();
				StatCrafting* pCrafting = LordNew StatCrafting(0x1010000 + id, craftname, id);
				pCrafting->registerStat();
				objectCraftStats[id] = pCrafting;
			}
		}

		replaceAllSimilarBlocks(objectCraftStats);
	}
}

StatBase** StatList::initMinableStats(const String& name, int beginID)
{
	StatBase** pResult = (StatBase**)LordMalloc(sizeof(StatBase*)* MAX_STAT_SIZE);
	memset(pResult, 0, sizeof(StatBase*)* MAX_STAT_SIZE);

	Block** blocks = BlockManager::sBlocks;
	for (int i = 0; i < BlockManager::MAX_BLOCK_COUNT; ++i)
	{
		if (blocks[i] && blocks[i]->getEnableStats())
		{
			// todo.
			// String var4 = StatCollector.translateToLocalFormatted(name, new Object[]{ Block.blocksList[i].getLocalizedName() });
			String statname = blocks[i]->getLocalizedName();
			
			StatCrafting* pStatCrafting = LordNew StatCrafting(beginID + i, statname, i);
			pStatCrafting->registerStat();
			pResult[i] = pStatCrafting;

			objectMineStats.insert(pStatCrafting);
		}
	}

	replaceAllSimilarBlocks(pResult);
	return pResult;
}

StatBase** StatList::initUsableStats(StatBase** stats, const String& name, int beginID, int start, int end)
{
	if (stats == NULL)
	{
		stats = (StatBase**)LordMalloc(sizeof(StatBase*)* MAX_STAT_SIZE);
		memset(stats, 0, sizeof(StatBase*)* MAX_STAT_SIZE);
	}

    if (StatList::oneShotStats.size() > 0) {
        StatList::oneShotStats.clear();
    }
    
	for (int i = start; i < end; ++i)
	{
		if (Item::itemsList[i])
		{
			// String var6 = StatCollector.translateToLocalFormatted(par1Str, new Object[]{ Item.itemsList[i].getStatName() });
			String craftname = Item::itemsList[i]->getStatName(); // todo.
			StatCrafting* pCrafting = LordNew StatCrafting(beginID + i, craftname, i);
			pCrafting->registerStat();
			stats[i] = pCrafting;
			itemStats.push_back(pCrafting);
		}
	}

	replaceAllSimilarBlocks(stats);
	return stats;
}

StatBase** StatList::initBreakStats(StatBase** stats, const String& par1Str, int beginID, int start, int end)
{
	if (!stats)
	{
		stats = (StatBase**)LordMalloc(sizeof(StatBase*)* MAX_STAT_SIZE);
		memset(stats, 0, sizeof(StatBase*)* MAX_STAT_SIZE);
	}

    if (StatList::oneShotStats.size() > 0) {
        StatList::oneShotStats.clear();
    }
    
	for (int i = start; i < end; ++i)
	{
		if (Item::itemsList[i] && Item::itemsList[i]->isDamageable())
		{
			// String var6 = StatCollector.translateToLocalFormatted(par1Str, new Object[]{ Item.itemsList[i].getStatName() });
			String craftname = Item::itemsList[i]->getStatName();
			StatCrafting* pCrafting = LordNew StatCrafting(beginID + i, craftname, i);
			pCrafting->registerStat();
			stats[i] = pCrafting;
		}
	}

	replaceAllSimilarBlocks(stats);
	return stats;
}

void StatList::replaceAllSimilarBlocks(StatBase** stats)
{
	replaceSimilarBlocks(stats, BLOCK_ID_WATERSTILL, BLOCK_ID_WATERMOVING);
	replaceSimilarBlocks(stats, BLOCK_ID_LAVASTILL, BLOCK_ID_LAVAMOVING);
	replaceSimilarBlocks(stats, BLOCK_ID_PUMPKIN_LANTERN, BLOCK_ID_PUMPKIN);
	replaceSimilarBlocks(stats, BLOCK_ID_FURNACE_BURNING, BLOCK_ID_FURNACE_IDLE);
	replaceSimilarBlocks(stats, BLOCK_ID_ORE_REDSTONE_GLOWING, BLOCK_ID_ORE_REDSTONE);
	replaceSimilarBlocks(stats, BLOCK_ID_REDSTONE_REPEATER_ACTIVE, BLOCK_ID_REDSTONE_REPEATER_IDLE);
	replaceSimilarBlocks(stats, BLOCK_ID_TORCH_REDSTONE_ACTIVE, BLOCK_ID_TORCH_REDSTONE_IDLE);
	replaceSimilarBlocks(stats, BLOCK_ID_MUSHROOM_RED, BLOCK_ID_MUSHROOM_BROWN);
	replaceSimilarBlocks(stats, BLOCK_ID_STONE_DOUBLE_SLAB, BLOCK_ID_STONE_SINGLE_SLAB);
	replaceSimilarBlocks(stats, BLOCK_ID_WOOD_DOUBLE_SLAB, BLOCK_ID_WOOD_SINGLE_SLAB);
	replaceSimilarBlocks(stats, BLOCK_ID_GRASS, BLOCK_ID_DIRT);
	replaceSimilarBlocks(stats, BLOCK_ID_TILLED_FIELD, BLOCK_ID_DIRT);
	replaceSimilarBlocks(stats, BLOCK_ID_REDSAND_STONE_DOUBLESLAB, BLOCK_ID_REDSAND_STONE_SINGLESLAB);
}

void StatList::replaceSimilarBlocks(StatBase** stats, int from, int to)
{
	if (stats[from] && stats[to])
	{
		stats[to] = stats[from];
	}
	else
	{
		allStats.remove(stats[from]);
		objectMineStats.erase((StatCrafting*)stats[from]);
		generalStats.remove(stats[from]);
		stats[from] = stats[to];
	}
}

StatBase* StatList::getOneShotStat(int id)
{
	StatBaseMap::iterator it = oneShotStats.find(id);
	if (it == oneShotStats.end())
		return NULL;
	return it->second;
}

}
