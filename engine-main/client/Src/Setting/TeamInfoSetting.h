#pragma once
#include <map>
#include <string>

namespace BLOCKMAN
{
	enum class ToolBarTeamId
	{
		INVALID = 0,
		RED,
		BLUE,
		GREEN,
		YELLOW,
		PURPLE,
		ORANGE,

		TOTAL
	};

	struct TeamInfo
	{
		std::string TeamIcon;
		std::string TeamName;
		std::string TeamColor;

		TeamInfo(): 
		TeamIcon(""),
		TeamName(""),
		TeamColor("")
		{
		}
	};

	class TeamInfoSetting
	{
	public:
		static bool loadSetting();
		static const TeamInfo& getTeamInfo(int gameType, int teamId);
		static void unloadSetting();

		int m_gameType;

	private:
		static std::map<int, TeamInfoSetting*> m_TeamInfoSettingMap;
		TeamInfo TeamInfoArray[(int)ToolBarTeamId::TOTAL];
	};

}

