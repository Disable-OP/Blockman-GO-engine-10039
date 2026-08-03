#include "GameTypeSetting.h"

namespace BLOCKMAN
{
	std::map<String, ClientGameType>  GameTypeSetting::m_sGameNameToType =
	{
		{ "g1008", ClientGameType::BED_WAR },
		{ "g1009", ClientGameType::Murder_Mystery },
		{ "g1013", ClientGameType::Zombie_Infecting },
		{ "g1014", ClientGameType::Jail_Break },
		{ "g1015", ClientGameType::Sand_Digger },
		{ "g1016", ClientGameType::Chicken },
		{ "g1017", ClientGameType::Hide_And_Seek },
		{ "g1018", ClientGameType::Egg_War },
		{ "g1019", ClientGameType::Creepy_Aliens },
		{ "g1020", ClientGameType::Tiny_Town },
		{ "g1021", ClientGameType::Parkour},
		{ "g1022", ClientGameType::Flag_War },
		{ "g1026", ClientGameType::Throw_Pot },
		{ "g1023", ClientGameType::Build_War },
		{ "g1024", ClientGameType::Jewel_Knight },
		{ "g1027", ClientGameType::Sky_Royale },
		{ "g1028", ClientGameType::Budokai },
		{ "g1025", ClientGameType::Tycoon },
		{ "g1029", ClientGameType::Super_Wall },
		{ "g1030", ClientGameType::SnowmanBattle },
		{ "g1031", ClientGameType::Rancher },
		{ "g1032", ClientGameType::GunBattleHall },
		{ "g1033", ClientGameType::GBStrike },
		{ "g1035", ClientGameType::RanchersExplore },
		{ "g1036", ClientGameType::WatchCar },
		{ "g1037", ClientGameType::HASHall },
		{ "g1038", ClientGameType::HASHidden },
		{ "g1039", ClientGameType::HASChase },
		{ "g1040", ClientGameType::ExploreMap },
		{ "g1041", ClientGameType::BirdSimulator },
		{ "g1042", ClientGameType::PixelGunHall },
		{ "g1043", ClientGameType::PixelGunGameTeam },
		{ "g1044", ClientGameType::PixelGunGamePerson },
		{ "g1045", ClientGameType::PixelGunGame1v1 },
	};

	ClientGameType GameTypeSetting::GetGameType(String& gameType)
	{
		auto iter = m_sGameNameToType.find(gameType);
		if (iter != m_sGameNameToType.end())
			return iter->second;

		return ClientGameType::COMMON;
	}

	bool GameTypeSetting::IsCarFree(String & gameType)
	{
		ClientGameType gt = GetGameType(gameType);
		if (gt == ClientGameType::Jail_Break)
		{
			return false;
		}

		return true;
	}

}