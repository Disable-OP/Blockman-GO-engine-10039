#ifndef __RANK_LANG_SETTING_HEADER__
#define __RANK_LANG_SETTING_HEADER__

#include <list>
#include "Core.h"
#include "Setting/GameTypeSetting.h"

using namespace LORD;

namespace BLOCKMAN
{
	class RankLang
	{
	public:
		ClientGameType gameType;
		int rankType = 0;
		String title;
		String leftSubTitle;
		String leftScore;
		String leftMyRank;
		String leftMyScore;
		String rightSubTitle;
		String rightScore;
		String rightMyRank;
		String rightMyScore;
	};

	class RankLangSetting
	{
	public:
		static bool loadSetting();
		static void	unloadSetting();
		static RankLang* getRankLang(ClientGameType gameType, int rankType = 0);

	private:
		static std::list<RankLang*> m_rankLangs;
	};

	class RealTimeRankLang
	{
	public:
		ClientGameType gameType;
		int  ColumnCount;
		String ColumnOne;
		String ColumnTwo;
		String ColumnThree;
		String ColumnFour;
		String ColumnFive;
	};

	class RealTimeRankLangSetting
	{
	public:
		static bool loadSetting();
		static void	unloadSetting();
		static RealTimeRankLang* getRankLang(ClientGameType gameType);

	private:
		static std::list<RealTimeRankLang*> m_realTimeRankLangs;
	};
}
#endif // !__RANK_LANG_SETTING_HEADER__