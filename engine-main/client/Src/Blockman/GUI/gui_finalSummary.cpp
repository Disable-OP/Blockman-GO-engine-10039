#include "gui_finalSummary.h"
#include "gui_layout.h"
#include "UI/GUIWindowManager.h"
#include "UI/GUIButton.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIEditBox.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIMutualNonoverlappingGroup.h"
#include "GUI/RootGuiLayout.h"
#include "GUI/GuiDef.h"
#include "Network/ClientNetwork.h"
#include "Network/ClientPacketSender.h"
#include "Util/LanguageKey.h"
#include "Util/ClientCmdMgr.h"
#include "Util/StringUtil.h"
#include "Setting/UIDisplaySetting.h"
#include "GUISummaryItem.h"
#include "GuiItemSlot.h"
#include "game.h"
#include "ShellInterface.h"

namespace BLOCKMAN {
	gui_finalSummary::gui_finalSummary() :
		gui_layout("FinalSummary.json")
	{
	}

	gui_finalSummary::~gui_finalSummary()
	{
	}

	void gui_finalSummary::onLoad()
	{
		m_resultEntryList = NULL;
		m_rankList = getWindow<GUIListBox>("FinalSummary-AllRank-Content-List");
		LORD::GUIButton *exitBtn = getWindow<GUIButton>("FinalSummary-Exit");
		exitBtn->subscribeEvent(EventButtonClick, std::bind(&gui_finalSummary::onExitClick, this, std::placeholders::_1));
		exitBtn->SetText(getString(LanguageKey::GUI_EXIT));

		LORD::GUIButton *closeBtn = getWindow<GUIButton>("FinalSummary-Close");
		closeBtn->subscribeEvent(EventButtonClick, std::bind(&gui_finalSummary::onBackClick, this, std::placeholders::_1));
		closeBtn->SetText(getString(LanguageKey::GUI_BACK));
		closeBtn->SetVisible(GameClient::CGame::Instance()->GetGameType() == ClientGameType::Build_War);

		//LORD::GUIButton *shareBtn = getWindow<GUIButton>("FinalSummary-Share");
		//shareBtn->SetVisible(false);    // 分享按钮暂时没有
		//shareBtn->subscribeEvent(EventButtonClick, std::bind(&gui_finalSummary::onShareClick, this, std::placeholders::_1));
		//shareBtn->SetText(getString(LanguageKey::GUI_SHARE));

		LORD::GUIButton *adBtn = getWindow<GUIButton>("FinalSummary-Ad");
		adBtn->subscribeEvent(EventButtonClick, std::bind(&gui_finalSummary::onAdClick, this, std::placeholders::_1));
		getWindow<GUIStaticText>("FinalSummary-AdText")->SetText(getString(LanguageKey::GUI_ADVERTISE_TIP));
		getWindow<GUIButton>("FinalSummary-AdClose")->subscribeEvent(EventButtonClick, std::bind(&gui_finalSummary::onAdTipCloseClick, this, std::placeholders::_1));

		if (UIDisplaySetting::getUIDisplay()->isShowBackHall)
		{
			getWindow<GUIButton>("FinalSummary-Continue")->SetVisible(false);
			LORD::GUIButton *backHallBtn = getWindow<GUIButton>("FinalSummary-BackHall");
			backHallBtn->SetVisible(true);
			backHallBtn->subscribeEvent(EventButtonClick, std::bind(&gui_finalSummary::onBackHallClick, this, std::placeholders::_1));
			backHallBtn->SetText(getString(LanguageKey::GUI_BACK_HALL));
		}
		else
		{
			getWindow<GUIButton>("FinalSummary-BackHall")->SetVisible(false);
			LORD::GUIButton *contiueBtn = getWindow<GUIButton>("FinalSummary-Continue");
			contiueBtn->SetVisible(true);
			contiueBtn->subscribeEvent(EventButtonClick, std::bind(&gui_finalSummary::onContiueClick, this, std::placeholders::_1));
			contiueBtn->SetText(getString(LanguageKey::GUI_CONTINUE));
		}
	
		LORD::GUIWindow *rewardTip = getWindow("FinalSummary-Self-Reward-Tip");
		rewardTip->subscribeEvent(EventWindowClick, std::bind(&gui_finalSummary::onRewardTipClick, this, std::placeholders::_1));

		LORD::GUIStaticText *title = getWindow<GUIStaticText>("FinalSummary-Title");
		title->SetText(getString(LanguageKey::WORDART_SETTLEMENT));

		LORD::GUIStaticText *myRankTitle = getWindow<GUIStaticText>("FinalSummary-Self-RankTitle");
		myRankTitle->SetText(getString(LanguageKey::DEAD_SUMMARY_RANK));

		LORD::GUIStaticText *rankTitle = getWindow<GUIStaticText>("FinalSummary-AllRank-Title-Rank");
		rankTitle->SetText(getString(LanguageKey::FINAL_SUMMARY_RANK));

		LORD::GUIStaticText *nameTitle = getWindow<GUIStaticText>("FinalSummary-AllRank-Title-Name");
		nameTitle->SetText(getString(LanguageKey::FINAL_SUMMARY_PLAYER));

		LORD::GUIWindow *selfPanel = getWindow("FinalSummary-Self-RankWindow");
		selfPanel->subscribeEvent(EventWindowClick, std::bind(&gui_finalSummary::onSelfPanelClick, this, std::placeholders::_1));

		LORD::GUIStaticText *myRewardTitle = getWindow<GUIStaticText>("FinalSummary-Self-RewardTitle");
		myRewardTitle->SetText(getString(LanguageKey::DEAD_SUMMARY_REWARD));
		LORD::GUIStaticText *rewardTitle = getWindow<GUIStaticText>("FinalSummary-AllRank-Title-Reward");
		rewardTitle->SetText(getString(LanguageKey::FINAL_SUMMARY_REWARD));

		m_rewardProgress = getWindow<GUIProgressBar>("FinalSummary-Self-Reward-Slider-Slider");
		m_rewardText = getWindow<GUIStaticText>("FinalSummary-Self-Reward-Slider-Text");
		m_jewelKnightName = getWindow<GUIStaticText>("FinalSummary-Self-JewelKnight");
		m_jewelKnightName->SetVisible(false);
		m_jewelKnightName->SetText("");

		getWindow<GUIStaticText>("FinalSummary-Self-GuessRewardTitle")->SetVisible(GameClient::CGame::Instance()->GetGameType() == ClientGameType::Build_War);
		getWindow<GUIStaticText>("FinalSummary-Self-GuessResultTitle")->SetVisible(GameClient::CGame::Instance()->GetGameType() == ClientGameType::Build_War);
		getWindow<GUIStaticImage>("FinalSummary-Result")->SetVisible(GameClient::CGame::Instance()->GetGameType() != ClientGameType::Build_War);
		getWindow<GUIStaticText>("FinalSummary-AllRank-Title-Score")->SetVisible(GameClient::CGame::Instance()->GetGameType() == ClientGameType::Build_War);

		getWindow<GUIStaticText>("FinalSummary-Self-GuessRewardTitle")->SetText(getString(LanguageKey::GUI_BUILD_WAR_GUESS_REWARD_TITLE));
		getWindow<GUIStaticText>("FinalSummary-Self-GuessResultTitle")->SetText(getString(LanguageKey::GUI_BUILD_WAR_GUESS_RESULT_TITLE));
		getWindow<GUIStaticText>("FinalSummary-AllRank-Title-Score")->SetText(getString(LanguageKey::FINAL_SUMMARY_SCORE));

		m_subscriptionGuard.add(ReceiveFinalSummaryEvent::subscribe(std::bind(&gui_finalSummary::receiveFinalSummary, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(ReceiveFinalSummaryExtraEvent::subscribe(std::bind(&gui_finalSummary::receiveFinalSummaryExtra, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
	}

	void gui_finalSummary::onShow()
	{
		RefreshUI();
	}

	void gui_finalSummary::onUpdate(ui32 nTimeElapse)
	{

	}

	bool gui_finalSummary::onExitClick(const EventArgs & events)
	{
		GameClient::CGame::Instance()->exitGame();
		playSoundByType(ST_Click);
		return true;
	}

	bool gui_finalSummary::onBackClick(const EventArgs & events)
	{
		getParent()->showMainControl();
		playSoundByType(ST_Click);
		// GameClient::CGame::Instance()->getNetwork()->getSender()->sendBuildWarOpenGuessResult();
		return false;
	}

	bool gui_finalSummary::onShareClick(const EventArgs & events)
	{
		getParent()->showMainControl();
		playSoundByType(ST_Click);
		return true;
	}

	bool gui_finalSummary::onAdClick(const EventArgs & events)
	{
		playSoundByType(ST_Click);
		getWindow<GUIStaticImage>("FinalSummary-AdTipBg")->SetVisible(false);
		getWindow<GUIButton>("FinalSummary-AdClose")->SetTouchable(false);
		GameClient::CGame::Instance()->getShellInterface()->playAdVideo();
		return true;
	}

	bool gui_finalSummary::onAdTipCloseClick(const EventArgs & events)
	{
		getWindow<GUIStaticImage>("FinalSummary-AdTipBg")->SetVisible(false);
		getWindow<GUIButton>("FinalSummary-AdClose")->SetTouchable(false);
		return true;
	}

	bool gui_finalSummary::onContiueClick(const EventArgs & events)
	{
		getParent()->showMainControl();
		playSoundByType(ST_Click);
		if (m_isNextServer)
		{
			GameClient::CGame::Instance()->getShellInterface()->nextGame();
		}
		return true;
	}

	bool gui_finalSummary::onBackHallClick(const EventArgs & events)
	{
		getParent()->showMainControl();
		playSoundByType(ST_Click);
		GameClient::CGame::Instance()->getNetwork()->getSender()->sendClickBackHall();
		return true;
	}

	bool gui_finalSummary::onSelfPanelClick(const EventArgs & events)
	{
		if (!m_selfResultEntry)
			return false;

		auto window = getWindow("FinalSummary-Self-RankBg");
		auto rect = window->GetUnclippedOuterRect();
		LORD::UVector2 pos = UVector2(UDim(0.0f, rect.left + window->GetPixelSize().x),
			UDim(0.0f, rect.top - window->GetPixelSize().y / 2));

		playSoundByType(ST_Click);
		getParent()->showSummarPoint(pos, m_selfResultEntry->pointMap);
		return true;
	}

	void gui_finalSummary::RefreshUI()
	{
		if (GameClient::CGame::Instance()->GetGameType() != ClientGameType::Chicken) {
			RefreshAllRank();
		}
	}

	void gui_finalSummary::RefreshSelf()
	{
		String	txt;
		String	bgImage;
		String	image;
		String  tenImage;
		Color	textColor;
		Color	borderColor;

		GuiSummaryItem::GetImageByRank(m_selfResultEntry->rank, bgImage, image, tenImage, textColor, borderColor);

		LORD::GUIStaticText *nameTxt = getWindow<GUIStaticText>("FinalSummary-Self-Name");
		nameTxt->SetText(m_selfResultEntry->name.c_str());
		nameTxt->SetTextColor(textColor);
		nameTxt->SetTextBoader(borderColor);

		LORD::GUIStaticImage *rankBgImg = getWindow<GUIStaticImage>("FinalSummary-Self-RankBg");
		rankBgImg->SetImage(bgImage.c_str());
		LORD::GUIStaticImage *rankImg = getWindow<GUIStaticImage>("FinalSummary-Self-Rank");
		LORD::GUIWindow *doubleRank = getWindow("FinalSummary-Self-DoubleRank");
		if (m_selfResultEntry->rank > 9)
		{
			doubleRank->SetVisible(true);
			rankImg->SetVisible(false);
			getWindow<GUIStaticImage>("FinalSummary-Self-DoubleRank-Ten")->SetImage(tenImage.c_str());
			getWindow<GUIStaticImage>("FinalSummary-Self-DoubleRank-One")->SetImage(image.c_str());
		}
		else
		{
			doubleRank->SetVisible(false);
			rankImg->SetVisible(true);
			rankImg->SetImage(image.c_str());
		}

		GUIString FinanSumaryWin = "set:summary.json image:BigWinEN";
		GUIString FinanSumaryLose = "set:summary.json image:BigLoseEN";
		GUIString FinanSumaryDraw = "set:summary.json image:BigDogfallEN";

		if (LanguageManager::Instance()->getCurrentLanguage().compare("zh_CN") == 0) {
			FinanSumaryWin = "set:summary.json image:BigWinCN";
		    FinanSumaryLose = "set:summary.json image:BigLoseCN";
			FinanSumaryDraw = "set:summary.json image:BigDogfallCN";
		}

		getWindow<GUIStaticImage>("FinalSummary-Result")->SetImage(m_selfResultEntry->isWin == 1 ? FinanSumaryWin :
			m_selfResultEntry->isWin == 0 ? FinanSumaryLose : FinanSumaryDraw);

		LORD::GUIStaticText *rewardTxt = getWindow<GUIStaticText>("FinalSummary-Self-Reward");
		txt = StringUtil::ToString(m_selfResultEntry->reward);
		rewardTxt->SetText(txt.c_str());

		m_rewardProgress->SetProgress(m_selfResultEntry->todayGetReward == 0 ? 0 : (float)m_selfResultEntry->todayGetReward / m_selfResultEntry->canGetReward);
		txt = StringUtil::Format("%d/%d", m_selfResultEntry->todayGetReward, m_selfResultEntry->canGetReward);
		m_rewardText->SetText(txt.c_str());

		String vipIconRes = "";
		switch (m_selfResultEntry->vip)
		{
		case  1:
			vipIconRes = "set:summary.json image:VIP";
			break;
		case  2:
			vipIconRes = "set:summary.json image:VIPPlus";
			break;
		case  3:
			vipIconRes = "set:summary.json image:MVP";
			break;
		default:
			vipIconRes = "";
			break;
		}
		getWindow<GUIStaticImage>("FinalSummary-Self-VipIcon")->SetImage(vipIconRes.c_str());
		if (m_selfResultEntry->jewelKnight.length() > 0)
		{
			m_jewelKnightName->SetVisible(true);
			m_jewelKnightName->SetText(StringUtil::Format(getString(LanguageKey::GUI_SETTLEMENT_JEWEL_KNIGHT).c_str(), m_selfResultEntry->jewelKnight.c_str()).c_str());
		}
#if LORD_PLATFORM == LORD_PLATFORM_ANDROID
		getWindow<GUIButton>("FinalSummary-Ad")->SetVisible(m_selfResultEntry->adSwitch == 1);
		getWindow<GUIButton>("FinalSummary-AdClose")->SetTouchable(m_selfResultEntry->adSwitch == 1);

		if (m_selfResultEntry->adSwitch == 1)
		{
			GameClient::CGame::Instance()->getShellInterface()->reportAdVideoShow();
		}
#endif
	}

	void gui_finalSummary::RefreshAllRank()
	{
		m_rankList->ClearAllItem();
		m_rankList->SetItemHeight(66.0f);

		if (m_resultEntryList)
		{
			int i = 1;
			for (auto iter = m_resultEntryList->begin(); iter != m_resultEntryList->end(); iter++,i++)
			{
				if ((*iter)->isSelf)
				{
					m_selfResultEntry = *iter;
					RefreshSelf();
				}
	
				GuiSummaryItem* pSummaryItem = dynamic_cast<GuiSummaryItem*>(GUIWindowManager::Instance()->CreateGUIWindow(GWT_SUMMARY_ITEM, GUIString("SummaryItem") + StringUtil::ToString(i).c_str()));
				pSummaryItem->RefreshItem(*iter);
				pSummaryItem->SetPosition(UVector2(UDim(0.0f, 0.0f), UDim(0.0f, 0.0f)));
				pSummaryItem->SetSize(UVector2(UDim(1.0f, 0.0f), UDim(0.0f, 66.0f)));
				m_rankList->AddItem(pSummaryItem, true);
			}

			m_rankList->SetAllowScroll(true);
			m_rankList->SetTouchable(true);
		}
	}

	bool gui_finalSummary::receiveFinalSummary(const String& gameResult, bool isNextServer)
	{
		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::Chicken) {
			RootGuiLayout::Instance()->showDeadSummaryGui(gameResult, isNextServer);
		}
		else 
		{
			getParent()->showFinalSummaryGui();
			setIsNextServer(isNextServer);
			auto result = ResultEntry::getResultEntryList(gameResult);
			m_resultEntryList = result;
		}

		return true;
	}

	bool gui_finalSummary::receiveFinalSummaryExtra(bool guess_right, String guess_name, int guess_reward)
	{
		getWindow<GUIStaticText>("FinalSummary-Self-GuessName")->SetText(guess_name.c_str());
		getWindow<GUIStaticText>("FinalSummary-Self-GuessReward")->SetText(StringUtil::ToString(guess_reward).c_str());

		if (guess_right)
		{
			getWindow<GUIStaticImage>("FinalSummary-Self-GuessRewardRightIcon")->SetImage("set:buildwar.json image:gou");
		}
		else
		{
			getWindow<GUIStaticImage>("FinalSummary-Self-GuessRewardRightIcon")->SetImage("set:buildwar.json image:cha");
		}
		return true;
	}

	void gui_finalSummary::setIsNextServer(bool isNextServer)
	{
		this->m_isNextServer = isNextServer;
	}

	bool gui_finalSummary::onRewardTipClick(const EventArgs & events)
	{
		if (!m_selfResultEntry)
			return false;

		auto window = getWindow("FinalSummary-Self-Reward-Tip");
		auto rect = window->GetUnclippedOuterRect();
		LORD::UVector2 pos = UVector2(UDim(0.0f, rect.right + 10),
			UDim(0.0f, rect.top - 100));

		playSoundByType(ST_Click);
		getParent()->showSummaryRewardTip(pos, m_selfResultEntry->canGetReward);
		return true;
	}

}
