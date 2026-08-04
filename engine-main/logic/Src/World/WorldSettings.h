/********************************************************************
filename: 	WorldSetting.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-11-5
*********************************************************************/
#ifndef __WORLD_SETTING_HEADER__
#define __WORLD_SETTING_HEADER__

#include "BM_TypeDef.h"

#include "Core.h"
using namespace LORD;

namespace BLOCKMAN
{

class WorldInfo;

class WorldSettings : public ObjectAlloc
{
public:
	WorldSettings(i64 seed, GAME_TYPE gameType, bool mapFeatures, bool hardcore, TERRAIN_TYPE terrainType);
	WorldSettings(const WorldInfo& worldInfo);

	void enableBonusChest() { m_bonusChestEnabled = true; }
	void enableCommands()	{ m_commandsAllowed = true; }
	void func_82750_a(const String& options) { m_generateOptions = options; }

	bool isBonusChestEnabled() const { return m_bonusChestEnabled; }
	i64 getSeed() const { return m_seed; }
	GAME_TYPE getGameType() const { return m_gameType; }
	bool getHardcoreEnabled() const { return m_hardcoreEnabled; }
	bool isMapFeaturesEnabled() const { return m_mapFeaturesEnabled; }
	TERRAIN_TYPE getTerrainType() const { return m_terrainType; }
	bool areCommandsAllowed() const { return m_commandsAllowed; }
	const String& getGenerateOptions() const { return m_generateOptions; }
	
protected:
	/** The seed for the map. */
	i64 m_seed = 0;

	/** The EnumGameType. */
	GAME_TYPE m_gameType = GAME_TYPE_INVALID;

	/** Switch for the map features. 'true' for enabled, 'false' for disabled. */
	bool m_mapFeaturesEnabled = false;

	/** True if hardcore mode is enabled */
	bool m_hardcoreEnabled = false;
	TERRAIN_TYPE m_terrainType = TERRAIN_TYPE_INVALID;

	/** True if Commands (cheats) are allowed. */
	bool m_commandsAllowed = false;

	/** True if the Bonus Chest is enabled. */
	bool m_bonusChestEnabled = false;
	String m_generateOptions;
};

}

#endif
