#include "MonsterSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"

namespace BLOCKMAN
{
	std::map<int, MonsterSetting*> MonsterSetting::m_monsterSettingMap = {};
	
	void MonsterSetting::unloadSetting()
	{
		for (auto iter = m_monsterSettingMap.begin(); iter != m_monsterSettingMap.end(); iter++)
		{
			MonsterSetting* pSetting = iter->second;
			delete pSetting;
		}

		m_monsterSettingMap.clear();
	}

	MonsterSetting * MonsterSetting::getMonsterSetting(int MonsterId)
	{
		auto iter = m_monsterSettingMap.find(MonsterId);
		if (iter != m_monsterSettingMap.end())
		{
			return iter->second;
		}

		return nullptr;
	}

	void MonsterSetting::addMonsterSetting(MonsterSetting setting)
	{
		MonsterSetting * monsterSetting = getMonsterSetting(setting.monsterId);
		if (!monsterSetting)
		{
			monsterSetting = new MonsterSetting();
			monsterSetting->monsterId = setting.monsterId;
			monsterSetting->monsterType = setting.monsterType;
			m_monsterSettingMap.insert(std::pair<int, MonsterSetting*>(monsterSetting->monsterId, monsterSetting));
		}
		monsterSetting->isAutoAttack = setting.isAutoAttack;
		monsterSetting->monsterBtTree = setting.monsterBtTree;
		monsterSetting->attackDistance = setting.attackDistance;
		monsterSetting->attackCd = setting.attackCd;
		monsterSetting->attackCount = setting.attackCount;
		monsterSetting->moveSpeed = setting.moveSpeed;
		monsterSetting->attackType = setting.attackType;
		monsterSetting->patrolDistance = setting.patrolDistance;
		monsterSetting->skillId = setting.skillId;
		monsterSetting->skillCd = setting.skillCd;
	}
}