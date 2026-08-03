#pragma once
#include <map>
#include "Core.h"
using namespace LORD;

namespace BLOCKMAN
{
	enum class ClientGameType
	{
		Invalid = 0,
		COMMON = 1,
		BED_WAR = 2,
		Murder_Mystery = 3,
		Zombie_Infecting = 4,
		Jail_Break = 5,
		Sand_Digger = 6,
		Chicken = 7,
		Hide_And_Seek = 8,
		Egg_War = 9,
		Creepy_Aliens = 10,
		Tiny_Town = 11,
		Parkour = 12,
		Flag_War = 13,
		Build_War = 14,
		Jewel_Knight = 15,
		Throw_Pot = 16,
		Sky_Royale = 17,
		Budokai = 18,
		Super_Wall = 19,
		SnowmanBattle = 20,
		Tycoon = 21,
		GunBattleHall = 22,
		GBStrike = 23,
		WatchCar = 24,
		HASHall = 25,
		HASHidden = 26,
		HASChase = 27,
		Rancher = 30,
		RanchersExplore = 31,
		ExploreMap = 32,
		PixelGunHall = 33,
		PixelGunGameTeam = 34,
		PixelGunGamePerson = 35,
		PixelGunGame1v1 = 36,
		BirdSimulator = 37,
	};

	class GameTypeSetting
	{
	public:
		static ClientGameType GetGameType(String& gameType);
		static bool IsCarFree(String& gameType);
		
	private:
		static std::map<String, ClientGameType>  m_sGameNameToType;
	};
};