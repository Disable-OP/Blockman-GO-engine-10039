#include "gui_rank.h"
#include "game.h"
#include "GUISummaryItem.h"
#include "Util/LanguageKey.h"
#include "UI/GUIWindowManager.h"
#include "GUI/GuiDef.h"
#include "Setting/RankLangSetting.h"

namespace BLOCKMAN
{
	gui_rank::gui_rank()
		: gui_layout("Rank.json")
	{
		m_selfDayRank = NULL;
		m_selfWeekRank = NULL;
	}

	gui_rank::~gui_rank()
	{
		clearData();
	}

	void gui_rank::onLoad()
	{
		LORD::GUIButton *exitBtn = getWindow<GUIButton>("Rank-Title-BtnClose");
		exitBtn->subscribeEvent(EventButtonClick, std::bind(&gui_rank::onExitClick, this, std::placeholders::_1));
		resetRankLang();
		refreshRankLang();

		m_leftRankList = getWindow<GUIListBox>("Rank-LeftContent-List");
		m_rightRankList = getWindow<GUIListBox>("Rank-RightContent-List");
		m_dayRankIcon = getWindow<GUIStaticImage>("Rank-LeftBottom-SelfRankIcon");
		m_dayRankIconTen = getWindow<GUIStaticImage>("Rank-LeftBottom-SelfRankIconTen");
		m_dayRankText = getWindow<GUIStaticText>("Rank-LeftBottom-SelfRankText");
		m_weekRankIcon = getWindow<GUIStaticImage>("Rank-RightBottom-SelfRankIcon");
		m_weekRankIconTen = getWindow<GUIStaticImage>("Rank-RightBottom-SelfRankIconTen");
		m_weekRankText = getWindow<GUIStaticText>("Rank-RightBottom-SelfRankText");
		m_dayScoreText = getWindow<GUIStaticText>("Rank-LeftBottom-SelfScore");
		m_weekScoreText = getWindow<GUIStaticText>("Rank-RightBottom-SelfScore");

		m_subscriptionGuard.add(ReceiveRankResultEvent::subscribe(std::bind(&gui_rank::receiveRankResult, this, std::placeholders::_1)));
		m_selfDayRank = NULL;
		m_selfWeekRank = NULL;
		
	}

	void gui_rank::onUpdate(ui32 nTimeElapse)
	{

	}

	void gui_rank::onShow()
	{
		RefreshUI();
	}

	bool gui_rank::onExitClick(const EventArgs & events)
	{
		getParent()->hideRank();
		return true;
	}

	void gui_rank::RefreshUI()
	{
		refreshRankLang();
		refreshSelf();
		refreshContent(m_leftRankList, m_DayRankList);
		refreshContent(m_rightRankList, m_WeekRankList);
	}

	void gui_rank::resetRankLang()
	{
		// title
		getWindow<GUIStaticText>("Rank-Title-Text")->SetText(getString(LanguageKey::GUI_RANK_DEFAULT_TITLE_NAME));
		
		// left
		getWindow<GUIStaticText>("Rank-ViceTitle-Left-Text")->SetText(getString(LanguageKey::GUI_RANK_VICE_TITLE_LEFT_NAME));
		getWindow<GUIStaticText>("Rank-LeftContent-Title-Rank")->SetText(getString(LanguageKey::FINAL_SUMMARY_RANK));
		getWindow<GUIStaticText>("Rank-LeftContent-Title-Name")->SetText(getString(LanguageKey::FINAL_SUMMARY_PLAYER));
		getWindow<GUIStaticText>("Rank-LeftContent-Title-Score")->SetText(getString(LanguageKey::GUI_RANK_SCORE));
		getWindow<GUIStaticText>("Rank-LeftBottom-SelfRankName")->SetText(getString(LanguageKey::GUI_RANK_DAY_RANK));
		getWindow<GUIStaticText>("Rank-LeftBottom-SelfScoreName")->SetText(getString(LanguageKey::GUI_RANK_SELF_SCORE));
		
		// right
		getWindow<GUIStaticText>("Rank-ViceTitle-Right-Text")->SetText(getString(LanguageKey::GUI_RANK_VICE_TITLE_RIGHT_NAME));
		getWindow<GUIStaticText>("Rank-RightContent-Title-Rank")->SetText(getString(LanguageKey::FINAL_SUMMARY_RANK));
		getWindow<GUIStaticText>("Rank-RightContent-Title-Name")->SetText(getString(LanguageKey::FINAL_SUMMARY_PLAYER));
		getWindow<GUIStaticText>("Rank-RightContent-Title-Score")->SetText(getString(LanguageKey::GUI_RANK_SCORE));
		getWindow<GUIStaticText>("Rank-RightBottom-SelfRankName")->SetText(getString(LanguageKey::GUI_RANK_WEEK_RANK));
		getWindow<GUIStaticText>("Rank-RightBottom-SelfScoreName")->SetText(getString(LanguageKey::GUI_RANK_SELF_SCORE));
	}

	void gui_rank::refreshRankLang()
	{
		RankLang* rankLang = RankLangSetting::getRankLang(GameClient::CGame::Instance()->GetGameType(), m_RankSubdivideType);
		if (!rankLang)
		{
			resetRankLang();
			return;
		}
		getWindow<GUIStaticText>("Rank-Title-Text")->SetText(getString(rankLang->title));
		getWindow<GUIStaticText>("Rank-ViceTitle-Left-Text")->SetText(getString(rankLang->leftSubTitle));
		getWindow<GUIStaticText>("Rank-LeftContent-Title-Score")->SetText(getString(rankLang->leftScore));
		getWindow<GUIStaticText>("Rank-LeftBottom-SelfRankName")->SetText(getString(rankLang->leftMyRank));
		getWindow<GUIStaticText>("Rank-LeftBottom-SelfScoreName")->SetText(getString(rankLang->leftMyScore));
		getWindow<GUIStaticText>("Rank-ViceTitle-Right-Text")->SetText(getString(rankLang->rightSubTitle));
		getWindow<GUIStaticText>("Rank-RightContent-Title-Score")->SetText(getString(rankLang->rightScore));
		getWindow<GUIStaticText>("Rank-RightBottom-SelfRankName")->SetText(getString(rankLang->rightMyRank));
		getWindow<GUIStaticText>("Rank-RightBottom-SelfScoreName")->SetText(getString(rankLang->rightMyScore));
	}

	void gui_rank::refreshSelf()
	{
		refreshSelfDay();
		refreshSelfWeek();
	}

	void gui_rank::refreshContent(GUIListBox* list, std::list<RankData*>& data)
	{
		if (list == NULL)
			return;

		list->ClearAllItem();
		list->SetItemHeight(80.0f);
		static int i = 1;
		for (auto iter = data.begin(); iter != data.end(); iter++, i++)
		{
			GuiSummaryItem* pSummaryItem = dynamic_cast<GuiSummaryItem*>(GUIWindowManager::Instance()->CreateGUIWindow(GWT_SUMMARY_ITEM, GUIString("RankItem") + StringUtil::ToString(i).c_str()));
			pSummaryItem->RefreshItem(*iter);
			pSummaryItem->SetPosition(UVector2(UDim(0.0f, 0.0f), UDim(0.0f, 0.0f)));
			pSummaryItem->SetSize(UVector2(UDim(1.0f, 0.0f), UDim(0.0f, 66.0f)));
			list->AddItem(pSummaryItem, true);
		}

		list->SetAllowScroll(true);
		list->SetTouchable(true);
	}

	void gui_rank::refreshSelfDay()
	{
		int dayRank = 0;
		int dayScore = 0;
		if (m_selfDayRank != NULL)
		{
			dayRank = m_selfDayRank->rank;
			dayScore = m_selfDayRank->score;
		}

		String	txt;
		String	bgImage;
		String	image;
		String  tenImage;
		Color	textColor;
		Color	borderColor;

		m_dayScoreText->SetText(StringUtil::ToThousandthString(dayScore).c_str());
		m_dayRankIcon->SetVisible(false);
		m_dayRankIconTen->SetVisible(false);
		m_dayRankText->SetVisible(true);
		m_dayRankText->SetText(dayRank <= 3000 && dayRank > 0 ? StringUtil::ToThousandthString(dayRank).c_str() : getString(LanguageKey::GUI_RANK_MY_RANK_NOT_RANK));
	}

	void gui_rank::refreshSelfWeek()
	{
		int weekRank = 0;
		int weekScore = 0;
		if (m_selfWeekRank != NULL)
		{
			weekRank = m_selfWeekRank->rank;
			weekScore = m_selfWeekRank->score;
		}

		String	txt;
		String	bgImage;
		String	image;
		String  tenImage;
		Color	textColor;
		Color	borderColor;

		m_weekScoreText->SetText(StringUtil::ToThousandthString(weekScore).c_str());

		m_weekRankIcon->SetVisible(false);
		m_weekRankIconTen->SetVisible(false);
		m_weekRankText->SetVisible(true);
		m_weekRankText->SetText(weekRank <= 3000 && weekRank  > 0 ? StringUtil::ToThousandthString(weekRank).c_str() : getString(LanguageKey::GUI_RANK_MY_RANK_NOT_RANK));
	}

	bool gui_rank::receiveRankResult(const String& rankResult)
	{
		bool bRet = false;
		clearData();
		bRet = parseRankResult(rankResult);
		return bRet;
	}

	RankData* gui_rank::getRankData(rapidjson::GenericObject<false, rapidjson::GenericValue<rapidjson::UTF8<char>, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>>> &data)
	{
		if (!data.HasMember("rank") || !data.HasMember("score") ||
			!data.HasMember("name") || !data.HasMember("userId") || !data.HasMember("vip"))
		{
			LordLogError("The rank result content missed some field.");
			return nullptr;
		}

		String name = data.FindMember("name")->value.GetString();
		ui64 userId = data.FindMember("userId")->value.GetInt();
		int rank = data.FindMember("rank")->value.GetInt();
		int score = data.FindMember("score")->value.GetInt();
		int vip = data.FindMember("vip")->value.GetInt();

		return new RankData(name, userId, score, rank, vip);
	}

	void gui_rank::clearData()
	{
		if (m_selfDayRank != NULL)
		{
			delete m_selfDayRank;
			m_selfDayRank = NULL;
		}

		if (m_selfWeekRank != NULL)
		{
			delete m_selfWeekRank;
			m_selfWeekRank = NULL;
		}

		for (auto iter = m_DayRankList.begin(); iter != m_DayRankList.end(); iter++)
			delete *iter;
		m_DayRankList.clear();

		for (auto iter = m_WeekRankList.begin(); iter != m_WeekRankList.end(); iter++)
			delete *iter;
		m_WeekRankList.clear();

		m_RankSubdivideType = 0;
	}

	bool gui_rank::parseRankResult(const String& rankResult)
	{
		if (rankResult.length() == 0)
			return false;
		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(rankResult.c_str());

		if (doc->HasParseError())
		{
			LordLogError("The rank result content  HasParseError.");
			LordDelete(doc);
			return false;
		}

		if (!doc->HasMember("own") || !doc->HasMember("ranks"))
		{
			LordLogError("The rank result content missed some field.");
			LordDelete(doc);
			return false;
		}

		if (doc->HasMember("subType")) 
		{
			m_RankSubdivideType = doc->FindMember("subType")->value.GetInt();
		}

		auto ownData = doc->FindMember("own")->value.GetObject();
		if (!ownData.HasMember("day") || !ownData.HasMember("week"))
		{
			LordLogError("The rank result content missed some field.");
			LordDelete(doc);
			return false;
		}

		auto ownDayData = ownData.FindMember("day")->value.GetObject();
		m_selfDayRank = getRankData(ownDayData);
		if(m_selfDayRank == NULL)
		{
			LordDelete(doc);
			return false;
		}

		auto ownWeekData = ownData.FindMember("week")->value.GetObject();
		m_selfWeekRank = getRankData(ownWeekData);
		if (m_selfWeekRank == NULL)
		{
			LordDelete(doc);
			return false;
		}

		auto ranksData = doc->FindMember("ranks")->value.GetObject();
		if (!ranksData.HasMember("day") || !ranksData.HasMember("week"))
		{
			LordLogError("The rank result content missed some field.");
			LordDelete(doc);
			return false;
		}

		rapidjson::Value ranksDayData = ranksData.FindMember("day")->value.GetArray();
		for (size_t i = 0; i < ranksDayData.Size(); ++i)
		{
			auto obj = ranksDayData[i].GetObject();
			RankData* pData = getRankData(obj);
			if (pData == NULL)
			{
				LordDelete(doc);
				return false;
			}
			m_DayRankList.push_back(pData);
		}

		rapidjson::Value ranksWeekData = ranksData.FindMember("week")->value.GetArray();
		for (size_t i = 0; i < ranksWeekData.Size(); ++i)
		{
			auto obj = ranksWeekData[i].GetObject();
			RankData* pData = getRankData(obj);
			if (pData == NULL)
			{
				LordDelete(doc);
				return false;
			}
			m_WeekRankList.push_back(pData);
		}

		LordDelete(doc);
		return true;
	}
}
