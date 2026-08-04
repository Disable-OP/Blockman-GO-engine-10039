#include "WorldInfo.h"
#include "WorldSettings.h"

namespace BLOCKMAN
{

WorldInfo::WorldInfo()
	: m_terrainType(TERRAIN_TYPE_DEFAULT)
	, m_generatorOptions("")
	//, m_theGameRules()
{
	m_mapId = "map-debug";
}

/*
WorldInfo(NBTTagCompound par1NBTTagCompound)
{
}*/

WorldInfo::WorldInfo(const String levelName, const WorldSettings& setttings)
	: m_levelName(levelName)
{
	// m_theGameRules = new GameRules();

	m_generateVersion = 0;
	m_spawnX = 0;
	m_spawnY = 0;
	m_spawnZ = 0;
	m_timeStopped = false;
	m_totalTime = 0;
	m_worldTime = 0;
	m_lastTimePlayed = 0;
	m_sizeOnDisk = 0;
	m_dimension = 0;
	m_saveVersion = 0;
	m_raining = 0;
	m_rainTime = 0;
	m_thundering = 0;
	m_thunderTime = 0;

	m_randomSeed = setttings.getSeed();
	m_theGameType = setttings.getGameType();
	m_mapFeaturesEnabled = setttings.isMapFeaturesEnabled();
	m_hardcore = setttings.getHardcoreEnabled();
	m_terrainType = setttings.getTerrainType();
	m_generatorOptions = setttings.getGenerateOptions();
	m_allowCommands = setttings.areCommandsAllowed();
	m_initialized = false;
	m_mapId = levelName;
}

WorldInfo::WorldInfo(const WorldInfo& rhs)
{
	//m_theGameRules = new GameRules();
	m_randomSeed = rhs.m_randomSeed;
	m_terrainType = rhs.m_terrainType;
	m_generatorOptions = rhs.m_generatorOptions;
	m_theGameType = rhs.m_theGameType;
	m_mapFeaturesEnabled = rhs.m_mapFeaturesEnabled;
	m_spawnX = rhs.m_spawnX;
	m_spawnY = rhs.m_spawnY;
	m_spawnZ = rhs.m_spawnZ;
	m_timeStopped = rhs.m_timeStopped;
	m_totalTime = rhs.m_totalTime;
	m_worldTime = rhs.m_worldTime;
	m_lastTimePlayed = rhs.m_lastTimePlayed;
	m_sizeOnDisk = rhs.m_sizeOnDisk;
	// m_playerTag = par1WorldInfo.playerTag;
	m_dimension = rhs.m_dimension;
	m_levelName = rhs.m_levelName;
	m_saveVersion = rhs.m_saveVersion;
	m_rainTime = rhs.m_rainTime;
	m_raining = rhs.m_raining;
	m_thunderTime = rhs.m_thunderTime;
	m_thundering = rhs.m_thundering;
	m_hardcore = rhs.m_hardcore;
	m_allowCommands = rhs.m_allowCommands;
	m_initialized = rhs.m_initialized;
	m_mapId = rhs.m_mapId;
	// m_theGameRules = par1WorldInfo.theGameRules;
}

}