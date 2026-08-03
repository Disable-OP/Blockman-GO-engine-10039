#pragma once
#include "Core.h"
#include "TypeDef.h"
#include "UI/GUIString.h"

using namespace LORD;

namespace GameClient
{
	struct HungerGameResult
	{
		HungerGameResult(int kills, int exp, int goldCoins, int activeValues):
			kills(kills),
			exp(exp),
			goldCoins(goldCoins),
			activeValues(activeValues)
		{
		}

		~HungerGameResult() {}
		HungerGameResult(LORD::String & gameResult);

		int kills = 0;
		int exp = 0;
		int goldCoins = 0;
		int activeValues = 0;
	};

	struct ResultEntry
	{
		ResultEntry(String& name, int rank, int killNum, int isWin);
		~ResultEntry();

		using ResultEntryPtr = std::shared_ptr<ResultEntry>;
		using ResultEntryListPtr = std::shared_ptr<list<ResultEntryPtr>::type>;

		ResultEntry(const String& gameResult);
		static ResultEntryListPtr getResultEntryList(const String& gameResult);
		static int getIsWin(const String& gameResult);
		static bool CompareResultEntry(const ResultEntryPtr a, const ResultEntryPtr b);

		int						isWin = 0;
		bool					isSelf = false;
		Byte					rank = 0;
		Byte					killNum = 0;
		String					name = "";
		String					jewelKnight = "";
		int						reward = 0;
		int						todayGetReward = 0;
		int						canGetReward = 0;
		int						vip = 0;
		ui64					user_id = 0;
		int						build_war_score = 0;
		Byte					adSwitch = 0;
		std::map<int, int>		pointMap;
	};
}

