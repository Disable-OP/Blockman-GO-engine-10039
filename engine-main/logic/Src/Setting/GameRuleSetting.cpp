#include "GameRuleSetting.h"
#include "LogicSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"

namespace BLOCKMAN
{
	GameRule* GameRuleSetting::m_curGameRule = NULL;
	std::map<ClientGameType, GameRule*>  GameRuleSetting::m_GameRuleMap = {};

	bool GameRuleSetting::loadSetting(bool isClient)
	{
		m_GameRuleMap.clear();
		GameRule* pGameRule = NULL;
		CsvReader* pReader = new CsvReader();

		std::string path = PathUtil::ConcatPath(Root::Instance()->getRootPath(), "Media/Setting/GameRule.csv").c_str();
		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordLogError("Can not open game rule setting file %s", path.c_str());
			return false;
		}
		pReader->Read(); // discard the second line
		while (pReader->Read())
		{
			String gameType;
			pReader->GetString("GameType", gameType);
			ClientGameType type = GameTypeSetting::GetGameType(gameType);
			if (m_GameRuleMap.find(type) != m_GameRuleMap.end())
				continue;
			pGameRule = new GameRule();
			pReader->GetBool("IsCanSprint", pGameRule->isCanSprint);
			pReader->GetBool("IsCreatureCollision", pGameRule->isCreatureCollision);
			pReader->GetBool("IsActorAction", pGameRule->isActorAction);
			pReader->GetBool("IsGunAction", pGameRule->isGunAction);
			pReader->GetBool("IsGunNorAttack", pGameRule->isGunNorAttack);
			pReader->GetBool("IsBlockmanCollision", pGameRule->isBlockmanCollision);
			pReader->GetBool("IsBlockmanLongHit", pGameRule->isBlockmanLongHit);
			pReader->GetBool("IsHaveLifebar", pGameRule->isHaveLifebar);
			pReader->GetInt("BlockmanLongHitTimes", pGameRule->blockmanLongHitTimes);
			pReader->GetInt("HurtProtectTime", pGameRule->hurtProtectTime);
			pReader->GetBool("IsHasSeason", pGameRule->isHasSeason);
			
			m_GameRuleMap[type] = pGameRule;
		}

		pReader->Close();
		LordSafeDelete(pReader);
		return true;
	}

	void GameRuleSetting::unloadSetting()
	{
		for (auto iter = m_GameRuleMap.begin(); iter != m_GameRuleMap.end(); iter++)
		{
			delete iter->second;
		}
		m_GameRuleMap.clear();
		m_curGameRule = NULL;
	}

	GameRule * GameRuleSetting::getGameRule()
	{
		if (m_curGameRule == NULL)
		{
			auto pRule = m_GameRuleMap.find(LogicSetting::Instance()->getGameType());
			if (pRule == m_GameRuleMap.end())
			{
				pRule = m_GameRuleMap.find(ClientGameType::COMMON);
			}
			m_curGameRule = pRule->second;
		}
		return m_curGameRule;
	}
}