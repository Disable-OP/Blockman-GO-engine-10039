#include "WorldSettings.h"
#include "WorldInfo.h"

namespace BLOCKMAN
{

WorldSettings::WorldSettings(i64 seed, GAME_TYPE gameType, bool mapFeatures, bool hardcore, TERRAIN_TYPE terrainType)
{
	m_generateOptions = "";
	m_seed = seed;
	m_gameType = gameType;
	m_mapFeaturesEnabled = mapFeatures;
	m_hardcoreEnabled = hardcore;
	m_terrainType = terrainType;
}

WorldSettings::WorldSettings(const WorldInfo& worldInfo)
{
	m_generateOptions = "";
	m_seed = worldInfo.getSeed();
	m_gameType = worldInfo.getGameType();
	m_mapFeaturesEnabled = worldInfo.isMapFeaturesEnabled();
	m_hardcoreEnabled = worldInfo.isHardcoreModeEnabled();
	m_terrainType = worldInfo.getTerrainType();
}

}