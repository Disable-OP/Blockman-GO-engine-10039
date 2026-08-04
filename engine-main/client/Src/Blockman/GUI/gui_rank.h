#pragma once
#include "gui_layout.h"
#include "UI/GUIButton.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIListBox.h"
#include "GUI/RootGuiLayout.h"
#include "rapidjson/document.h"

using namespace LORD;

namespace BLOCKMAN
{
	struct RankData
	{
		String name;
		ui64 userId;
		int score;
		int rank;
		int vip;

		RankData(String n, ui64 uId, int s, int r, int v)
			: name(n)
			, userId(uId)
			, score(s)
			, rank(r)
			, vip(v)
		{
		}
	};

	class gui_rank : public gui_layout
	{
	public:
		gui_rank();
		virtual ~gui_rank();
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		void onShow() override;
		bool onExitClick(const EventArgs & events);
		void RefreshUI();
		void resetRankLang();
		void refreshRankLang();
		void refreshSelf();
		void refreshContent(GUIListBox* list, std::list<RankData*>& data);
		
	private:
		GUIListBox*									m_leftRankList = nullptr;
		GUIListBox*									m_rightRankList = nullptr;
		GUIStaticImage*								m_dayRankIcon = nullptr;
		GUIStaticImage*								m_dayRankIconTen = nullptr;
		GUIStaticImage*								m_weekRankIcon = nullptr;
		GUIStaticImage*								m_weekRankIconTen = nullptr;
		GUIStaticText*								m_dayRankText = nullptr;
		GUIStaticText*								m_weekRankText = nullptr;
		GUIStaticText*								m_dayScoreText = nullptr;
		GUIStaticText*								m_weekScoreText = nullptr;

		RankData*									m_selfDayRank = nullptr;
		RankData*									m_selfWeekRank = nullptr;
		std::list<RankData*>						m_DayRankList;
		std::list<RankData*>						m_WeekRankList;
		SubscriptionGuard							m_subscriptionGuard;
		
		int                                         m_RankSubdivideType = 0;//榜单内容分类

		void refreshSelfDay();
		void refreshSelfWeek();
		bool parseRankResult(const String& rankResult);
		bool receiveRankResult(const String& rankResult);
		RankData* getRankData(rapidjson::GenericObject<false, rapidjson::GenericValue<rapidjson::UTF8<char>, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>>> &data);
		void clearData();

		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}
	};
}
