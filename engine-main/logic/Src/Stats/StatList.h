/********************************************************************
filename: 	StatList.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2017-02-28
*********************************************************************/
#ifndef __STAT_LIST_HEADER__
#define __STAT_LIST_HEADER__

#include "Core.h"
using namespace LORD;

namespace BLOCKMAN
{

class StatBase;
class StatCrafting;

class StatList
{
public:
	typedef map<int, StatBase*>::type StatBaseMap;
	typedef list<StatBase*>::type StatBaseLst;
	typedef vector<StatCrafting*>::type ItemStateArr;
	typedef set<StatCrafting*>::type ItemStateSet;

	static const int MAX_STAT_SIZE = 2048;
	
public:
	static StatBaseLst allStats;			// = new ArrayList();
	static StatBaseLst generalStats;		// = new ArrayList();
	static ItemStateArr itemStats;			// = new ArrayList();
	/** Tracks one-off stats. */
	static StatBaseMap oneShotStats;		// = new HashMap();
	/** Tracks the number of times a given block or item has been mined. */
	static ItemStateSet objectMineStats;	// = new ArrayList();

	/** times the game has been started */
	static StatBase* startGameStat;			// = (new StatBasic(1000, "stat.startGame")).initIndependentStat().registerStat();
	/** times a world has been created */
	static StatBase* createWorldStat;		// = (new StatBasic(1001, "stat.createWorld")).initIndependentStat().registerStat();
	/** the number of times you have loaded a world */
	static StatBase* loadWorldStat;			// = (new StatBasic(1002, "stat.loadWorld")).initIndependentStat().registerStat();
	/** number of times you've joined a multiplayer world */
	static StatBase* joinMultiplayerStat;	// = (new StatBasic(1003, "stat.joinMultiplayer")).initIndependentStat().registerStat();
	/** number of times you've left a game */
	static StatBase* leaveGameStat;			// = (new StatBasic(1004, "stat.leaveGame")).initIndependentStat().registerStat();
	/** number of minutes you have played */
	static StatBase* minutesPlayedStat;		// = (new StatBasic(1100, "stat.playOneMinute", StatBase.timeStatType)).initIndependentStat().registerStat();
	/** distance you've walked */
	static StatBase* distanceWalkedStat;	// = (new StatBasic(2000, "stat.walkOneCm", StatBase.distanceStatType)).initIndependentStat().registerStat();
	/** distance you have swam */
	static StatBase* distanceSwumStat;		// = (new StatBasic(2001, "stat.swimOneCm", StatBase.distanceStatType)).initIndependentStat().registerStat();
	/** the distance you have fallen */
	static StatBase* distanceFallenStat;	// = (new StatBasic(2002, "stat.fallOneCm", StatBase.distanceStatType)).initIndependentStat().registerStat();
	/** the distance you've climbed */
	static StatBase* distanceClimbedStat;	// = (new StatBasic(2003, "stat.climbOneCm", StatBase.distanceStatType)).initIndependentStat().registerStat();
	/** the distance you've flown */
	static StatBase* distanceFlownStat;		// = (new StatBasic(2004, "stat.flyOneCm", StatBase.distanceStatType)).initIndependentStat().registerStat();
	/** the distance you've dived */
	static StatBase* distanceDoveStat;		// = (new StatBasic(2005, "stat.diveOneCm", StatBase.distanceStatType)).initIndependentStat().registerStat();
	/** the distance you've traveled by blockman */
	static StatBase* distanceByMinecartStat;// = (new StatBasic(2006, "stat.minecartOneCm", StatBase.distanceStatType)).initIndependentStat().registerStat();
	/** the distance you've traveled by boat */
	static StatBase* distanceByBoatStat;	// = (new StatBasic(2007, "stat.boatOneCm", StatBase.distanceStatType)).initIndependentStat().registerStat();
	/** the distance you've traveled by pig */
	static StatBase* distanceByPigStat;		// = (new StatBasic(2008, "stat.pigOneCm", StatBase.distanceStatType)).initIndependentStat().registerStat();
	/** the times you've jumped */
	static StatBase* jumpStat;				// = (new StatBasic(2010, "stat.jump")).initIndependentStat().registerStat();
	/** the distance you've dropped (or times you've fallen?) */
	static StatBase* dropStat;				// = (new StatBasic(2011, "stat.drop")).initIndependentStat().registerStat();
	/** the amount of damage you've dealt */
	static StatBase* damageDealtStat;		// = (new StatBasic(2020, "stat.damageDealt", StatBase.field_111202_k)).registerStat();
	/** the amount of damage you have taken */
	static StatBase* damageTakenStat;		// = (new StatBasic(2021, "stat.damageTaken", StatBase.field_111202_k)).registerStat();
	/** the number of times you have died */
	static StatBase* deathsStat;				// = (new StatBasic(2022, "stat.deaths")).registerStat();
	/** the number of mobs you have killed */
	static StatBase* mobKillsStat;			// = (new StatBasic(2023, "stat.mobKills")).registerStat();
	/** counts the number of times you've killed a player */
	static StatBase* playerKillsStat;		// = (new StatBasic(2024, "stat.playerKills")).registerStat();
	static StatBase* fishCaughtStat;			// = (new StatBasic(2025, "stat.fishCaught")).registerStat();
	static StatBase** mineBlockStatArray;	// = initMinableStats("stat.mineBlock", 16777216);
	/** Tracks the number of items a given block or item has been crafted. */
	static StatBase** objectCraftStats;
	/** Tracks the number of times a given block or item has been used. */
	static StatBase** objectUseStats;
	/** Tracks the number of times a given block or item has been broken. */
	static StatBase** objectBreakStats;
	static bool blockStatsInitialized;
	static bool itemStatsInitialized;

protected:
	/*** Initializes statistic fields related to minable items and blocks. */
	static StatBase** initMinableStats(const String& name, int beginID);
	/** Initializes statistic fields related to usable items and blocks. */
	static StatBase** initUsableStats(StatBase** stats, const String& name, int beginID, int from, int to);
	static StatBase** initBreakStats(StatBase** stats, const String& name, int beginID, int from, int to);
	/** Forces all dual blocks to count for each other on the stats list */
	static void replaceAllSimilarBlocks(StatBase** stats);
	/** Forces stats for one block to add to another block, such as idle and active furnaces */
	static void replaceSimilarBlocks(StatBase** stats, int from, int to);
	
public:
	static void initialize();
	static void unInitialize();

	/** This method simply NOPs. It is presumably used to call the static constructors on server start. */
	static void nopInit() {}
	/** Initializes statistic fields related to breakable items and blocks. */
	static void initBreakableStats();
	static void initStats();

	/** Initializes statistics related to craftable items. Is only called after both block and item stats have been initialized. */
	static void initCraftableStats();
	
	static StatBase* getOneShotStat(int id);
};


}

#endif