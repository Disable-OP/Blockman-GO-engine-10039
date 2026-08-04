/********************************************************************
filename: 	WorldInfo.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-11-3
*********************************************************************/
#ifndef __WORLD_INFO_HEADER__
#define __WORLD_INFO_HEADER__

#include "BM_TypeDef.h"

#include "Core.h"
using namespace LORD;

namespace BLOCKMAN
{

class WorldSettings;

class WorldInfo
{
public:
	WorldInfo();
	WorldInfo(const String levelName, const WorldSettings& setttings);
	WorldInfo(const WorldInfo& rhs);
	/*
	WorldInfo(NBTTagCompound par1NBTTagCompound)
	{
	}*/

	i64		getSeed() const { return m_randomSeed; }
	int		getSpawnX() const { return m_spawnX; }
	int		getSpawnY() const {	return m_spawnY;	}
	int		getSpawnZ() const {	return m_spawnZ; }
	bool	isTimeStopped() const{ return m_timeStopped; }
	i64		getWorldTotalTime() const { return m_totalTime; }
	i64		getWorldTime() const { return m_worldTime; }
	i64		getSizeOnDisk() const { return m_sizeOnDisk; }
	int		getVanillaDimension() const { return m_dimension; }
	int		getSaveVersion() { return m_saveVersion; }
	i64		getLastTimePlayed() const { return m_lastTimePlayed; }
	int		getThunderTime() const { return m_thunderTime; }
	int		getRainTime() const { return m_rainTime; }
	bool	isMapFeaturesEnabled() const { return m_mapFeaturesEnabled; }
	bool	isThundering() const { return m_thundering; }
	bool	isRaining() const { return m_raining; }
	bool	isHardcoreModeEnabled() const { return m_hardcore; }
	bool	areCommandsAllowed() const { return m_allowCommands; }
	bool	isInitialized() const { return m_initialized; }
	TERRAIN_TYPE getTerrainType() const { return m_terrainType; }
	GAME_TYPE getGameType() const { return m_theGameType; }
	const String& getWorldName() const { return m_levelName; }
	const String& getGeneratorOptions() const { return m_generatorOptions; }
	
	void setSpawnX(int x) { m_spawnX = x; }
	void setSpawnY(int y) { m_spawnY = y; }
	void setSpawnZ(int z) { m_spawnZ = z; }

	void incrementTotalWorldTime(i64 t) { m_totalTime = t; }
	void setWorldTime(i64 time) { m_worldTime = time; }
	void setSpawnPosition(const BlockPos& pos)
	{
		m_spawnX = pos.x;
		m_spawnY = pos.y;
		m_spawnZ = pos.z;
	}
	void setTimeStopped(bool stopped) { m_timeStopped = stopped; }
	void setWorldName(const String& name) { m_levelName = name; }
	void setSaveVersion(int version) { m_saveVersion = version; }
	void setThundering(bool thundering) { m_thundering = thundering; }
	void setThunderTime(int par1) { m_thunderTime = par1; }
	void setRaining(bool raining) { m_raining = raining; }
	void setRainTime(int raintime) { m_rainTime = raintime; }
	void setGameType(GAME_TYPE gametype) { m_theGameType = gametype; }
	void setTerrainType(TERRAIN_TYPE terrainType) { m_terrainType = terrainType; }
	void setServerInitialized(bool inited) { m_initialized = inited; }
	String getMapId() const { return m_mapId; }
	// GameRules getGameRulesInstance() { return m_theGameRules; }

	/**
	* Adds this WorldInfo instance to the crash report.
	*/
	/*public void addToCrashReport(CrashReportCategory par1CrashReportCategory)
	{
		par1CrashReportCategory.addCrashSectionCallable("Level seed", new CallableLevelSeed(this));
		par1CrashReportCategory.addCrashSectionCallable("Level generator", new CallableLevelGenerator(this));
		par1CrashReportCategory.addCrashSectionCallable("Level generator options", new CallableLevelGeneratorOptions(this));
		par1CrashReportCategory.addCrashSectionCallable("Level spawn location", new CallableLevelSpawnLocation(this));
		par1CrashReportCategory.addCrashSectionCallable("Level time", new CallableLevelTime(this));
		par1CrashReportCategory.addCrashSectionCallable("Level dimension", new CallableLevelDimension(this));
		par1CrashReportCategory.addCrashSectionCallable("Level storage version", new CallableLevelStorageVersion(this));
		par1CrashReportCategory.addCrashSectionCallable("Level weather", new CallableLevelWeather(this));
		par1CrashReportCategory.addCrashSectionCallable("Level game mode", new CallableLevelGamemode(this));
	}*/

protected:
	/** Holds the seed of the currently world. */
	i64		m_randomSeed = 0;
	/** ID for this world type. */
	TERRAIN_TYPE m_terrainType = TERRAIN_TYPE_INVALID;
	/** The int version of the ChunkProvider that generated this world. */
	int		m_generateVersion = 0;
	String	m_generatorOptions;

	/** The spawn zone position X coordinate. */
	int		m_spawnX = 0;
	int		m_spawnY = 0;
	int		m_spawnZ = 0;

	bool	m_timeStopped = false;

	/** Total time for this world. */
	i64		m_totalTime = 0;

	/** The current world time in ticks, ranging from 0 to 23999. */
	i64		m_worldTime = 0;

	/** The last time the player was in this world. */
	i64		m_lastTimePlayed = 0;

	/** The size of entire save of current world on the disk, isn't exactly. */
	i64		m_sizeOnDisk = 0;
	// NBTTagCompound playerTag;
	int		m_dimension = 0;

	/** The name of the save defined at world creation. */
	String	m_levelName;

	/** Introduced in beta 1.3, is the save version for future control. */
	int		m_saveVersion = 0;

	/** True if it's raining, false otherwise. */
	bool	m_raining = false;

	/** Number of ticks until next rain. */
	int		m_rainTime = 0;

	/** Is thunderbolts failing now? */
	bool	m_thundering = false;

	/** Number of ticks untils next thunderbolt. */
	int		m_thunderTime = 0;

	String m_mapId;

	/** The Game Type. */
	GAME_TYPE m_theGameType = GAME_TYPE_INVALID;

	/** Whether the map features (e.g. strongholds) generation is enabled or disabled. */
	bool m_mapFeaturesEnabled = false;

	/** Hardcore mode flag */
	bool m_hardcore = false;
	bool m_allowCommands = false;
	bool m_initialized = false;
	// GameRules m_theGameRules;
};

}

#endif