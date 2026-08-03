#pragma once
#include <map>
#include "Core.h"
#include "Item/Items.h"
#include "Network/protocol/PacketDefine.h"

using namespace LORD;
namespace BLOCKMAN
{
	class MonsterSetting
	{
	public:
		static void unloadSetting();
		static MonsterSetting* getMonsterSetting(int monsterId);
		static void addMonsterSetting(MonsterSetting setting);

		int					monsterId = 0;
		String		        monsterBtTree = "";
		int					monsterType = 0;
		int					isAutoAttack = 0;
		float				attackDistance = 0.0f;
		float				attackCd = 0.0f;
		int					attackCount = 2;
		int					moveSpeed = 0;
		int					attackType = 0;
		float				patrolDistance = 0.0f;
		int					skillId = 0;
		float				skillCd = 0;

	private:
		static std::map<int, MonsterSetting*> m_monsterSettingMap;
	};
}
