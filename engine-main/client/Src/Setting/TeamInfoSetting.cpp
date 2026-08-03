#include "TeamInfoSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"
#include "Setting/GameTypeSetting.h"

using namespace LORD;
namespace BLOCKMAN  
{
	std::map<int, TeamInfoSetting*> TeamInfoSetting::m_TeamInfoSettingMap = {};

	bool TeamInfoSetting::loadSetting()
	{
		TeamInfoSetting* pSetting = NULL;
		m_TeamInfoSettingMap.clear();
		CsvReader* pReader = new CsvReader();
		std::string iconColumName;
		std::string nameColumName;
		std::string colorColumName;

		std::string path = (PathUtil::ConcatPath(Root::Instance()->getRootPath(), "Media/Setting/teamInfo.csv")).c_str();
		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordLogError("Can not open team info setting file %s", path.c_str());
			return false;
		}

		pReader->Read(); // discard the second line

		while (pReader->Read())
		{
			pSetting = new TeamInfoSetting();
			String gameType;
			pReader->GetString("GameType", gameType);
			ClientGameType type = GameTypeSetting::GetGameType(gameType);
			pSetting->m_gameType = (int)type;
			for (int i = (int)ToolBarTeamId::INVALID; i < (int)ToolBarTeamId::TOTAL; i++)
			{
				iconColumName = StringUtil::Format("TeamIcon%d", i).c_str();
				nameColumName = StringUtil::Format("TeamName%d", i).c_str();
				colorColumName = StringUtil::Format("TeamColor%d", i).c_str();
				pReader->GetString(iconColumName, pSetting->TeamInfoArray[i].TeamIcon);
				pReader->GetString(nameColumName, pSetting->TeamInfoArray[i].TeamName);
				pReader->GetString(colorColumName, pSetting->TeamInfoArray[i].TeamColor);
			}

			m_TeamInfoSettingMap[pSetting->m_gameType] = pSetting;
		}

		pReader->Close();
		LordSafeDelete(pReader)
		return true;
	}

	const TeamInfo& TeamInfoSetting::getTeamInfo(int gameType, int teamId)
	{
		TeamInfoSetting* pSetting;
		auto iter = m_TeamInfoSettingMap.find(gameType);
		if (iter != m_TeamInfoSettingMap.end())
		{
			pSetting = iter->second;
			if(teamId < 0 || teamId >= (int)ToolBarTeamId::TOTAL)
				return  pSetting->TeamInfoArray[0];
			else
				return pSetting->TeamInfoArray[teamId];
		}
		else
		{
			pSetting = m_TeamInfoSettingMap[1];
			if (teamId < 0 || teamId >= (int)ToolBarTeamId::TOTAL)
				return  pSetting->TeamInfoArray[0];
			else
				return pSetting->TeamInfoArray[teamId];
		}
	}

	void TeamInfoSetting::unloadSetting()
	{
		for (auto iter = m_TeamInfoSettingMap.begin(); iter != m_TeamInfoSettingMap.end(); iter++)
		{
			TeamInfoSetting* pSetting = iter->second;
			delete pSetting;
		}

		m_TeamInfoSettingMap.clear();
	}
}
