#include "gui_realTimeRank.h"
#include "Util/LanguageKey.h"
#include "GUISummaryItem.h"
#include "UI/GUIWindowManager.h"
#include "GUI/RootGuiLayout.h"
#include "GUI/GuiDef.h"
#include "game.h"

namespace BLOCKMAN
{
	gui_realTimeRank::gui_realTimeRank()
		: gui_layout("RealTimeRank.json")
	{

	}

	gui_realTimeRank::~gui_realTimeRank()
	{
		clearRealTimeRankDate();
	}

	void gui_realTimeRank::onLoad()
	{
		m_RealTimeRankTotalLayout = getWindow<GUILayout>("RealTimeRank");
		m_FiveColumnRankLayout = getWindow<GUILayout>("RealTimeRank-Five-Rank");
		m_FourColumnRankLayout = getWindow<GUILayout>("RealTimeRank-Four-Rank");
		m_ThreeColumnRankLayout = getWindow<GUILayout>("RealTimeRank-Three-Rank");
		initFiveColumnRealTimeRank();
		initFourColumnRealTimeRank();
		initThreeColumnRealTimeRank();
		initRealTimeRankListInfo();
		showRealTimeRankByType(NONE_RANK_TYPE);
	}

	void gui_realTimeRank::onUpdate(ui32 nTimeElapse)
	{
		reduceRequestRefreshRankTime(nTimeElapse);
	}

	void gui_realTimeRank::onShow()
	{

	}

	void gui_realTimeRank::initFiveColumnRealTimeRank()
	{
		m_FiveColumnRank_Column1 = getWindow<GUIStaticText>("RealTimeRank-Five-Rank-Title-One");
		m_FiveColumnRank_Column2 = getWindow<GUIStaticText>("RealTimeRank-Five-Rank-Title-Two");
		m_FiveColumnRank_Column3 = getWindow<GUIStaticText>("RealTimeRank-Five-Rank-Title-Three");
		m_FiveColumnRank_Column4 = getWindow<GUIStaticText>("RealTimeRank-Five-Rank-Title-Four");
		m_FiveColumnRank_Column5 = getWindow<GUIStaticText>("RealTimeRank-Five-Rank-Title-Five");

		RealTimeRankLang* rankLang = RealTimeRankLangSetting::getRankLang(GameClient::CGame::Instance()->GetGameType());
		if (!rankLang)
			return;

		m_FiveColumnRank_Column1->SetText(getString(rankLang->ColumnOne));
		m_FiveColumnRank_Column2->SetText(getString(rankLang->ColumnTwo));
		m_FiveColumnRank_Column3->SetText(getString(rankLang->ColumnThree));
		m_FiveColumnRank_Column4->SetText(getString(rankLang->ColumnFour));
		m_FiveColumnRank_Column5->SetText(getString(rankLang->ColumnFive));
	}

	void gui_realTimeRank::initFourColumnRealTimeRank()
	{
		m_FourColumnRank_Column1 = getWindow<GUIStaticText>("RealTimeRank-Four-Rank-Title-One");
		m_FourColumnRank_Column2 = getWindow<GUIStaticText>("RealTimeRank-Four-Rank-Title-Two");
		m_FourColumnRank_Column3 = getWindow<GUIStaticText>("RealTimeRank-Four-Rank-Title-Three");
		m_FourColumnRank_Column4 = getWindow<GUIStaticText>("RealTimeRank-Four-Rank-Title-Four");

		RealTimeRankLang* rankLang = RealTimeRankLangSetting::getRankLang(GameClient::CGame::Instance()->GetGameType());
		if (!rankLang)
			return;
	
		m_FourColumnRank_Column1->SetText(getString(rankLang->ColumnOne));
		m_FourColumnRank_Column2->SetText(getString(rankLang->ColumnTwo));
		m_FourColumnRank_Column3->SetText(getString(rankLang->ColumnThree));
		m_FourColumnRank_Column4->SetText(getString(rankLang->ColumnFour));
	}

	void gui_realTimeRank::initThreeColumnRealTimeRank()
	{
		m_ThreeColumnRank_Column1 = getWindow<GUIStaticText>("RealTimeRank-Three-Rank-Title-One");
		m_ThreeColumnRank_Column2 = getWindow<GUIStaticText>("RealTimeRank-Three-Rank-Title-Two");
		m_ThreeColumnRank_Column3 = getWindow<GUIStaticText>("RealTimeRank-Three-Rank-Title-Three");

		RealTimeRankLang* rankLang = RealTimeRankLangSetting::getRankLang(GameClient::CGame::Instance()->GetGameType());
		if (!rankLang)
			return;

		m_ThreeColumnRank_Column1->SetText(getString(rankLang->ColumnOne));
		m_ThreeColumnRank_Column2->SetText(getString(rankLang->ColumnTwo));
		m_ThreeColumnRank_Column3->SetText(getString(rankLang->ColumnThree));
	}

	void  gui_realTimeRank::initRealTimeRankListInfo()
	{
		m_realTimeRankList_FiveColumn = getWindow<GUIListBox>("RealTimeRank-Five-Rank-List-Info");
		m_realTimeRankList_FourColumn = getWindow<GUIListBox>("RealTimeRank-Four-Rank-List-Info");
		m_realTimeRankList_ThreeColumn = getWindow<GUIListBox>("RealTimeRank-Three-Rank-List-Info");
	}

	void gui_realTimeRank::showRealTimeRank()
	{
		if (RootGuiLayout::Instance())
		{
			RootGuiLayout::Instance()->showRealTimeRank();
			showRealTimeRankByType(getRealTimeRankType());
		}
	}

	void gui_realTimeRank::showRealTimeRankByType(REAL_TIME_RANK_TYPE type)
	{
		switch (type)
		{
			case NONE_RANK_TYPE:
				m_RealTimeRankTotalLayout->SetVisible(false);
				m_FiveColumnRankLayout->SetVisible(false);
				m_FourColumnRankLayout->SetVisible(false);
				m_ThreeColumnRankLayout->SetVisible(false);
				break;
			case FIVE_RANK_TYPE:
				m_RealTimeRankTotalLayout->SetVisible(true);
				m_FiveColumnRankLayout->SetVisible(true);
				m_FourColumnRankLayout->SetVisible(false);
				m_ThreeColumnRankLayout->SetVisible(false);
				break;
			case FOUR_RANK_TYPE:
				m_RealTimeRankTotalLayout->SetVisible(true);
				m_FiveColumnRankLayout->SetVisible(false);
				m_FourColumnRankLayout->SetVisible(true);
				m_ThreeColumnRankLayout->SetVisible(false);
				break;
			case THREE_RANK_TYPE:
				m_RealTimeRankTotalLayout->SetVisible(true);
				m_FiveColumnRankLayout->SetVisible(false);
				m_FourColumnRankLayout->SetVisible(false);
				m_ThreeColumnRankLayout->SetVisible(true);
				break;
		}
	}

	REAL_TIME_RANK_TYPE  gui_realTimeRank::getRealTimeRankType()
	{
		REAL_TIME_RANK_TYPE rankType = NONE_RANK_TYPE;
		RealTimeRankLang* rankLang = RealTimeRankLangSetting::getRankLang(GameClient::CGame::Instance()->GetGameType());
		if (!rankLang)
			return rankType;

		switch ((REAL_TIME_RANK_TYPE)rankLang->ColumnCount)
		{
			case 3:
				rankType = THREE_RANK_TYPE;
				break;
			case 4:
				rankType = FOUR_RANK_TYPE;
				break;
			case 5:
				rankType = FIVE_RANK_TYPE;
				break;
		}
		return rankType;
	}

	void gui_realTimeRank::updateRealTimeRankData(String result)
	{
		bool bRet = false;
		clearRealTimeRankDate();
		bRet = parseRealTimeRankResult(result);
		if (bRet)
		{
			refreshRealTimeRankInfo();
			m_realTimeRankRefreshTime = 10 * 1000;
		}

	}
	bool gui_realTimeRank::parseRealTimeRankResult(const String& rankResult)
	{
		if (rankResult.length() == 0)
			return false;
		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(rankResult.c_str());

		if (doc->HasParseError())
		{
			LordLogError("The RealTimeRank result content  HasParseError.");
			LordDelete(doc);
			return false;
		}

		if (!doc->HasMember("own") || !doc->HasMember("ranks"))
		{
			LordLogError("The RealTimeRank result content missed some field.");
			LordDelete(doc);
			return false;
		}

		auto ownData = doc->FindMember("own")->value.GetObject();
		RealTimeRankData* pData = getRealTimeRankData(ownData);
		if (pData == NULL)
		{
			LordDelete(doc);
			return false;
		}
		m_realTimeRankListData.push_back(pData);

		rapidjson::Value ranksDayData = doc->FindMember("ranks")->value.GetArray();
		for (size_t i = 0; i < ranksDayData.Size(); ++i)
		{
			auto obj = ranksDayData[i].GetObject();
			RealTimeRankData* pData = getRealTimeRankData(obj);
			if (pData == NULL)
			{
				LordDelete(doc);
				return false;
			}
			m_realTimeRankListData.push_back(pData);
		}

		LordDelete(doc);
		return true;
	}

	RealTimeRankData* gui_realTimeRank::getRealTimeRankData(rapidjson::GenericObject<false, rapidjson::GenericValue<rapidjson::UTF8<char>, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>>> &data)
	{
		if (!data.HasMember("column_1") || !data.HasMember("column_2") || !data.HasMember("column_3"))
		{
			LordLogError("The rank result content missed some field.");
			return nullptr;
		}

		String column_1 = data.FindMember("column_1")->value.GetString();
		String column_2 = data.FindMember("column_2")->value.GetString();
		String column_3 = data.FindMember("column_3")->value.GetString();

		String column_4 = "";
		String column_5 = "";

		if (data.HasMember("column_4"))
			column_4 = data.FindMember("column_4")->value.GetString();

		if (data.HasMember("column_5"))
			column_5 = data.FindMember("column_5")->value.GetString();

		return new RealTimeRankData(column_1, column_2, column_3, column_4, column_5);
	}
	void gui_realTimeRank::clearRealTimeRankDate()
	{
		for (auto iter = m_realTimeRankListData.begin(); iter != m_realTimeRankListData.end(); iter++)
			delete *iter;
		m_realTimeRankListData.clear();
	}

	void gui_realTimeRank::refreshRealTimeRankInfo()
	{
		switch (getRealTimeRankType())
		{
		case FIVE_RANK_TYPE:
			refreshFiveColumnRealTimeRank();
			break;
		case FOUR_RANK_TYPE:
			refreshFourColumnRealTimeRank();
			break;
		case THREE_RANK_TYPE:
			refreshThreeColumnRealTimeRank();
			break;
		}
	}

	void gui_realTimeRank::refreshFiveColumnRealTimeRank()
	{
		showRealTimeRank();

		list<RealTimeRankData>::iterator rankData;
		m_realTimeRankList_FiveColumn->ClearAllItem();
		int count = 0;
		for (auto rankData = m_realTimeRankListData.begin(); rankData != m_realTimeRankListData.end(); rankData++)
		{
			String strLayoutName = StringUtil::Format("ToolBar-Real-Time-Rank-List-Info-Item-%d", count).c_str();
			String str_column_1 = StringUtil::Format("ToolBar-Real-Time-Rank-List-Info-Item-0ne-%d", count).c_str();
			String str_column_2 = StringUtil::Format("ToolBar-Real-Time-Rank-List-Info-Item-Two-%d", count).c_str();
			String str_column_3 = StringUtil::Format("ToolBar-Real-Time-Rank-List-Info-Item-Three-%d", count).c_str();
			String str_column_4 = StringUtil::Format("ToolBar-Real-Time-Rank-List-Info-Item-Four-%d", count).c_str();
			String str_column_5 = StringUtil::Format("ToolBar-Real-Time-Rank-List-Info-Item-Five-%d", count).c_str();
			String strImage = StringUtil::Format("ToolBar-Real-Time-Rank-List-Image-%d", count).c_str();

			GUILayout* listInfoItemLayout = (GUILayout*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_LAYOUT, strLayoutName.c_str());
			listInfoItemLayout->SetTouchable(false);
			listInfoItemLayout->SetArea(UDim(0, 0), UDim(0, 0), UDim(1, 0), UDim(0, 40));
			listInfoItemLayout->SetVerticalAlignment(VA_CENTRE);
			listInfoItemLayout->SetHorizontalAlignment(HA_CENTRE);

			GUIStaticText* column_1 = (GUIStaticText*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_TEXT, str_column_1.c_str());
			column_1->SetTouchable(false);

			column_1->SetText(StringUtil::Format("%s %s", count == 0 ? "▢FFFFD700" : "▢FFA8C0D1", (*rankData)->column_1.c_str()).c_str());
			column_1->SetTextVertAlign(VA_CENTRE);
			column_1->SetTextHorzAlign(HA_CENTRE);
			column_1->SetArea(UDim(0, 0), UDim(0, 0), UDim(0.15f, 0), UDim(1, 0));
			listInfoItemLayout->AddChildWindow(column_1);

			GUIStaticText* column_2 = (GUIStaticText*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_TEXT, str_column_2.c_str());
			column_2->SetTouchable(false);
			column_2->SetText(StringUtil::Format("%s %s", count == 0 ? "▢FFFFD700" : "▢FFA8C0D1", (*rankData)->column_2.c_str()).c_str());
			column_2->SetTextVertAlign(VA_CENTRE);
			column_2->SetTextHorzAlign(HA_CENTRE);
			column_2->SetArea(UDim(0.15f, 0), UDim(0, 0), UDim(0.3f, 0), UDim(1, 0));
			listInfoItemLayout->AddChildWindow(column_2);

			GUIStaticText* column_3 = (GUIStaticText*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_TEXT, str_column_3.c_str());
			column_3->SetTouchable(false);
			column_3->SetText(StringUtil::Format("%s %s", count == 0 ? "▢FFFFD700" : "▢FFA8C0D1", (*rankData)->column_3.c_str()).c_str());
			column_3->SetTextVertAlign(VA_CENTRE);
			column_3->SetTextHorzAlign(HA_CENTRE);
			column_3->SetArea(UDim(0.455f, 0), UDim(0, 0), UDim(0.15f, 0), UDim(1, 0));
			listInfoItemLayout->AddChildWindow(column_3);

			GUIStaticText* column_4 = (GUIStaticText*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_TEXT, str_column_4.c_str());
			column_4->SetTouchable(false);
			column_4->SetText(StringUtil::Format("%s %s", count == 0 ? "▢FFFFD700" : "▢FFA8C0D1", (*rankData)->column_4.c_str()).c_str());
			column_4->SetTextVertAlign(VA_CENTRE);
			column_4->SetTextHorzAlign(HA_CENTRE);
			column_4->SetArea(UDim(0.605f, 0), UDim(0, 0), UDim(0.16f, 0), UDim(1, 0));
			listInfoItemLayout->AddChildWindow(column_4);

			GUIStaticText* column_5 = (GUIStaticText*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_TEXT, str_column_5.c_str());
			column_5->SetTouchable(false);
			column_5->SetText(StringUtil::Format("%s %s", count == 0 ? "▢FFFFD700" : "▢FFA8C0D1", (*rankData)->column_5.c_str()).c_str());
			column_5->SetTextVertAlign(VA_CENTRE);
			column_5->SetTextHorzAlign(HA_CENTRE);
			column_5->SetArea(UDim(0.765f, 0), UDim(0, 0), UDim(0.235f, 0), UDim(1, 0));
			listInfoItemLayout->AddChildWindow(column_5);

			GUIStaticImage* rankStepView = (GUIStaticImage*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, strImage.c_str());
			rankStepView->SetTouchable(false);
			rankStepView->SetImage("set:creepy_aliens.json image:rank_title_top_space");
			rankStepView->SetArea(UDim(0, 3), UDim(0, 44), UDim(0.98f, 0), UDim(0, 2));
			listInfoItemLayout->AddChildWindow(rankStepView);

			m_realTimeRankList_FiveColumn->AddItem(listInfoItemLayout, false);
			count++;
		}
	}

	void gui_realTimeRank::refreshFourColumnRealTimeRank()
	{
		showRealTimeRank();

		list<RealTimeRankData>::iterator rankData;
		m_realTimeRankList_FourColumn->ClearAllItem();
		int count = 0;
		for (auto rankData = m_realTimeRankListData.begin(); rankData != m_realTimeRankListData.end(); rankData++)
		{
			String strLayoutName = StringUtil::Format("ToolBar-Real-Time-Rank-List-Info-Item-%d", count).c_str();
			String str_column_1 = StringUtil::Format("ToolBar-Real-Time-Rank-List-Info-Item-One-%d", count).c_str();
			String str_column_2 = StringUtil::Format("ToolBar-Real-Time-Rank-List-Info-Item-Two-%d", count).c_str();
			String str_column_3 = StringUtil::Format("ToolBar-Real-Time-Rank-List-Info-Item-Three-%d", count).c_str();
			String str_column_4 = StringUtil::Format("ToolBar-Real-Time-Rank-List-Info-Item-Four-%d", count).c_str();
			String strImage = StringUtil::Format("ToolBar-Real-Time-Rank-List-Image-%d", count).c_str();

			GUILayout* listInfoItemLayout = (GUILayout*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_LAYOUT, strLayoutName.c_str());
			listInfoItemLayout->SetTouchable(false);
			listInfoItemLayout->SetArea(UDim(0, 0), UDim(0, 0), UDim(1, 0), UDim(0, 40));
			listInfoItemLayout->SetVerticalAlignment(VA_CENTRE);
			listInfoItemLayout->SetHorizontalAlignment(HA_CENTRE);

			GUIStaticText* column_1 = (GUIStaticText*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_TEXT, str_column_1.c_str());
			column_1->SetTouchable(false);

			column_1->SetText(StringUtil::Format("%s %s", count == 0 ? "▢FFFFD700" : "▢FFA8C0D1", (*rankData)->column_1.c_str()).c_str());
			column_1->SetTextVertAlign(VA_CENTRE);
			column_1->SetTextHorzAlign(HA_CENTRE);
			column_1->SetArea(UDim(0, 0), UDim(0, 0), UDim(0.15f, 0), UDim(1, 0));
			listInfoItemLayout->AddChildWindow(column_1);

			GUIStaticText* column_2 = (GUIStaticText*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_TEXT, str_column_2.c_str());
			column_2->SetTouchable(false);
			column_2->SetText(StringUtil::Format("%s %s", count == 0 ? "▢FFFFD700" : "▢FFA8C0D1", (*rankData)->column_2.c_str()).c_str());
			column_2->SetTextVertAlign(VA_CENTRE);
			column_2->SetTextHorzAlign(HA_CENTRE);
			column_2->SetArea(UDim(0.15f, 0), UDim(0, 0), UDim(0.4f, 0), UDim(1, 0));
			listInfoItemLayout->AddChildWindow(column_2);

			GUIStaticText* column_3 = (GUIStaticText*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_TEXT, str_column_3.c_str());
			column_3->SetTouchable(false);
			column_3->SetText(StringUtil::Format("%s %s", count == 0 ? "▢FFFFD700" : "▢FFA8C0D1", (*rankData)->column_3.c_str()).c_str());
			column_3->SetTextVertAlign(VA_CENTRE);
			column_3->SetTextHorzAlign(HA_CENTRE);
			column_3->SetArea(UDim(0.55f, 0), UDim(0, 0), UDim(0.225f, 0), UDim(1, 0));
			listInfoItemLayout->AddChildWindow(column_3);

			GUIStaticText* column_4 = (GUIStaticText*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_TEXT, str_column_4.c_str());
			column_4->SetTouchable(false);
			column_4->SetText(StringUtil::Format("%s %s", count == 0 ? "▢FFFFD700" : "▢FFA8C0D1", (*rankData)->column_4.c_str()).c_str());
			column_4->SetTextVertAlign(VA_CENTRE);
			column_4->SetTextHorzAlign(HA_CENTRE);
			column_4->SetArea(UDim(0.775f, 0), UDim(0, 0), UDim(0.225f, 0), UDim(1, 0));
			listInfoItemLayout->AddChildWindow(column_4);

			GUIStaticImage* rankStepView = (GUIStaticImage*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, strImage.c_str());
			rankStepView->SetTouchable(false);
			rankStepView->SetImage("set:creepy_aliens.json image:rank_title_top_space");
			rankStepView->SetArea(UDim(0, 0), UDim(0, 44), UDim(0.90f, 0), UDim(0, 2));
			listInfoItemLayout->AddChildWindow(rankStepView);

			m_realTimeRankList_FourColumn->AddItem(listInfoItemLayout, false);
			count++;
			
		}
	}

	void gui_realTimeRank::refreshThreeColumnRealTimeRank()
	{
		showRealTimeRank();

		RealTimeRankLang* rankLang = RealTimeRankLangSetting::getRankLang(GameClient::CGame::Instance()->GetGameType());
		if (!rankLang)
			return;

		m_ThreeColumnRank_Column1->SetText(getString(rankLang->ColumnOne));
		m_ThreeColumnRank_Column2->SetText(getString(rankLang->ColumnTwo));
		m_ThreeColumnRank_Column3->SetText(getString(rankLang->ColumnThree));

		list<RealTimeRankData>::iterator rankData;
		m_realTimeRankList_ThreeColumn->ClearAllItem();
		int count = 0;
		for (auto rankData = m_realTimeRankListData.begin(); rankData != m_realTimeRankListData.end(); rankData++)
		{
			String strLayoutName = StringUtil::Format("ToolBar-Real-Time-Rank-List-Info-Item-%d", count).c_str();
			String str_column_1 = StringUtil::Format("ToolBar-Real-Time-Rank-List-Info-Item-One-%d", count).c_str();
			String str_column_2 = StringUtil::Format("ToolBar-Real-Time-Rank-List-Info-Item-Two-%d", count).c_str();
			String str_column_3 = StringUtil::Format("ToolBar-Real-Time-Rank-List-Info-Item-Three-%d", count).c_str();
			String strImage = StringUtil::Format("ToolBar-Real-Time-Rank-List-Image-%d", count).c_str();

			GUILayout* listInfoItemLayout = (GUILayout*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_LAYOUT, strLayoutName.c_str());
			listInfoItemLayout->SetTouchable(false);
			listInfoItemLayout->SetArea(UDim(0, 0), UDim(0, 0), UDim(1, 0), UDim(0, 40));
			listInfoItemLayout->SetVerticalAlignment(VA_CENTRE);
			listInfoItemLayout->SetHorizontalAlignment(HA_CENTRE);

			GUIStaticText* column_1 = (GUIStaticText*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_TEXT, str_column_1.c_str());
			column_1->SetTouchable(false);

			column_1->SetText(StringUtil::Format("%s %s", count == 0 ? "▢FFFFD700" : "▢FFA8C0D1", (*rankData)->column_1.c_str()).c_str());
			column_1->SetTextVertAlign(VA_CENTRE);
			column_1->SetTextHorzAlign(HA_CENTRE);
			column_1->SetArea(UDim(0, 0), UDim(0, 0), UDim(0.2f, 0), UDim(1, 0));
			listInfoItemLayout->AddChildWindow(column_1);

			GUIStaticText* column_2 = (GUIStaticText*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_TEXT, str_column_2.c_str());
			column_2->SetTouchable(false);
			column_2->SetText(StringUtil::Format("%s %s", count == 0 ? "▢FFFFD700" : "▢FFA8C0D1", (*rankData)->column_2.c_str()).c_str());
			column_2->SetTextVertAlign(VA_CENTRE);
			column_2->SetTextHorzAlign(HA_CENTRE);
			column_2->SetArea(UDim(0.2f, 0), UDim(0, 0), UDim(0.45f, 0), UDim(1, 0));
			listInfoItemLayout->AddChildWindow(column_2);

			GUIStaticText* column_3 = (GUIStaticText*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_TEXT, str_column_3.c_str());
			column_3->SetTouchable(false);
			column_3->SetText(StringUtil::Format("%s %s", count == 0 ? "▢FFFFD700" : "▢FFA8C0D1", (*rankData)->column_3.c_str()).c_str());
			column_3->SetTextVertAlign(VA_CENTRE);
			column_3->SetTextHorzAlign(HA_CENTRE);
			column_3->SetArea(UDim(0.65f, 0), UDim(0, 0), UDim(0.35f, 0), UDim(1, 0));
			listInfoItemLayout->AddChildWindow(column_3);

			GUIStaticImage* rankStepView = (GUIStaticImage*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, strImage.c_str());
			rankStepView->SetTouchable(false);
			rankStepView->SetImage("set:creepy_aliens.json image:rank_title_top_space");
			rankStepView->SetArea(UDim(0, 0), UDim(0, 44), UDim(0.90f, 0), UDim(0, 2));
			listInfoItemLayout->AddChildWindow(rankStepView);

			m_realTimeRankList_ThreeColumn->AddItem(listInfoItemLayout, false);
			count++;
		}
	}

	void gui_realTimeRank::reduceRequestRefreshRankTime(ui32 nTimeElapse)
	{
		if (m_RealTimeRankTotalLayout && m_FiveColumnRankLayout && m_FourColumnRankLayout && m_ThreeColumnRankLayout) {
			if (m_RealTimeRankTotalLayout->IsVisible() && (m_FiveColumnRankLayout->IsVisible() || m_FourColumnRankLayout->IsVisible() || m_ThreeColumnRankLayout->IsVisible()))
			{
				if (m_realTimeRankRefreshTime >= 0)
				{
					m_realTimeRankRefreshTime = m_realTimeRankRefreshTime - nTimeElapse;
				}

				if (m_realTimeRankRefreshTime < 0)
				{
					m_realTimeRankRefreshTime = 10 * 1000;
					GameClient::CGame::Instance()->getNetwork()->getSender()->sendRequestUpdateRealTimeRank();
				}
			}
		}
	}

	void gui_realTimeRank::isColseRealTimeRankOp(bool status)
	{
		if (!status)
		{
			m_realTimeRankRefreshTime = 0;
			showRealTimeRankByType(NONE_RANK_TYPE);
		}
		else
		{
			showRealTimeRankByType(getRealTimeRankType());
			if (m_RealTimeRankTotalLayout && m_FiveColumnRankLayout && m_FourColumnRankLayout && m_ThreeColumnRankLayout) {
				if (m_RealTimeRankTotalLayout->IsVisible() && (m_FiveColumnRankLayout->IsVisible() || m_FourColumnRankLayout->IsVisible() || m_ThreeColumnRankLayout->IsVisible()))
				{
					GameClient::CGame::Instance()->getNetwork()->getSender()->sendRequestUpdateRealTimeRank();
				}
			}
		}
	}

	bool gui_realTimeRank::isShow()
	{
		if (m_RealTimeRankTotalLayout && m_FiveColumnRankLayout && m_FourColumnRankLayout && m_ThreeColumnRankLayout) {
			if (m_RealTimeRankTotalLayout->IsVisible() && (m_FiveColumnRankLayout->IsVisible() || m_FourColumnRankLayout->IsVisible() || m_ThreeColumnRankLayout->IsVisible()))
			{
				return true;
			}
		}
		return false;
	}

}