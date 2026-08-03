#ifndef __GAME_RULE_SETTING_HEADER__
#define __GAME_RULE_SETTING_HEADER__

#include <map>
#include "Core.h"
#include "GameTypeSetting.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GameRule
	{
	public:
		bool isCanSprint = true;
		bool isCreatureCollision = false;
		bool isActorAction = true;
		bool isGunAction = true;
		bool isGunNorAttack = true;
		bool isBlockmanCollision = true;
		bool isBlockmanLongHit = false;
		bool isHaveLifebar = false;
		int  blockmanLongHitTimes = -1;
		int  hurtProtectTime = 10;
		bool isHasSeason = false;
	};

	class GameRuleSetting
	{
	public:
		static bool	loadSetting(bool isClient);
		static void	unloadSetting();
		static GameRule* getGameRule();
		static void resetCurGameRule() { m_curGameRule = NULL; }

	private:
		static GameRule* m_curGameRule;
		static std::map<ClientGameType, GameRule*> m_GameRuleMap;
	};
}

#endif // !__GAME_RULE_SETTING_HEADER__
