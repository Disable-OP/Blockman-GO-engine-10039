#pragma once
#include "gui_layout.h"
#include "UI/GUIButton.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIListBox.h"
#include "rapidjson/document.h"
#include "UI/GUILayout.h"
#include "Setting/RankLangSetting.h"

using namespace LORD;

namespace BLOCKMAN
{
	struct RealTimeRankData
	{
		String column_1;
		String column_2;
		String column_3;
		String column_4;
		String column_5;

		RealTimeRankData(String c_1, String c_2, String c_3, String c_4, String c_5)
			: column_1(c_1)
			, column_2(c_2)
			, column_3(c_3)
			, column_4(c_4)
			, column_5(c_5)
		{
		}
	};

	enum REAL_TIME_RANK_TYPE
	{
		NONE_RANK_TYPE,
		FIVE_RANK_TYPE,
		FOUR_RANK_TYPE,
		THREE_RANK_TYPE,
	};

	class gui_realTimeRank : public gui_layout
	{
	public:
		gui_realTimeRank();
		virtual ~gui_realTimeRank();
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		void onShow() override;
		
		void initFiveColumnRealTimeRank();
		void initFourColumnRealTimeRank();
		void initThreeColumnRealTimeRank();
		void initRealTimeRankListInfo();
		void showRealTimeRank();
		void showRealTimeRankByType(REAL_TIME_RANK_TYPE type);
		void updateRealTimeRankData(String result);
		bool parseRealTimeRankResult(const String& rankResult);
		RealTimeRankData* getRealTimeRankData(rapidjson::GenericObject<false, rapidjson::GenericValue<rapidjson::UTF8<char>, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>>> &data);
		void clearRealTimeRankDate();
		void refreshRealTimeRankInfo();
		void refreshFiveColumnRealTimeRank();
		void refreshFourColumnRealTimeRank();
		void refreshThreeColumnRealTimeRank();
		REAL_TIME_RANK_TYPE getRealTimeRankType();
		void reduceRequestRefreshRankTime(ui32 nTimeElapse);
		void isColseRealTimeRankOp(bool status);
		bool isShow();

	private:

		int m_realTimeRankRefreshTime = 10 * 1000;
		std::list<RealTimeRankData*> m_realTimeRankListData;

		GUILayout* m_RealTimeRankTotalLayout = nullptr;
		GUILayout* m_FiveColumnRankLayout = nullptr;
		GUILayout* m_FourColumnRankLayout = nullptr;
		GUILayout* m_ThreeColumnRankLayout = nullptr;

		GUIListBox* m_realTimeRankList_FiveColumn = nullptr;
		GUIListBox* m_realTimeRankList_FourColumn = nullptr;
		GUIListBox* m_realTimeRankList_ThreeColumn = nullptr;

		GUIStaticText* m_FiveColumnRank_Column1 = nullptr;
		GUIStaticText* m_FiveColumnRank_Column2 = nullptr;
		GUIStaticText* m_FiveColumnRank_Column3 = nullptr;
		GUIStaticText* m_FiveColumnRank_Column4 = nullptr;
		GUIStaticText* m_FiveColumnRank_Column5 = nullptr;

		GUIStaticText* m_FourColumnRank_Column1 = nullptr;
		GUIStaticText* m_FourColumnRank_Column2 = nullptr;
		GUIStaticText* m_FourColumnRank_Column3 = nullptr;
		GUIStaticText* m_FourColumnRank_Column4 = nullptr;

		GUIStaticText* m_ThreeColumnRank_Column1 = nullptr;
		GUIStaticText* m_ThreeColumnRank_Column2 = nullptr;
		GUIStaticText* m_ThreeColumnRank_Column3 = nullptr;
	};
}