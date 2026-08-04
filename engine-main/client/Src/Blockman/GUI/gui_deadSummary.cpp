#include "gui_deadSummary.h"
#include "gui_layout.h"
#include "UI/GUIButton.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIEditBox.h"
#include "UI/GUIStaticImage.h"
#include "GUI/RootGuiLayout.h"
#include "Network/ClientNetwork.h"
#include "Network/ClientPacketSender.h"
#include "Util/LanguageKey.h"
#include "Util/ClientCmdMgr.h"
#include "GUISummaryItem.h"
#include "game.h"
#include "ShellInterface.h"
#include "Setting/GameTypeSetting.h"

namespace BLOCKMAN {
	gui_deadSummary::gui_deadSummary() :
		gui_layout("DeadSummary.json")
	{
	}

	gui_deadSummary::~gui_deadSummary()
	{
	}

	void gui_deadSummary::onLoad()
	{
		LORD::GUIButton *exitBtn = getWindow<GUIButton>("DeadSummary-Content-Exit");
		exitBtn->subscribeEvent(EventButtonClick, std::bind(&gui_deadSummary::onExitClick, this, std::placeholders::_1));
		exitBtn->SetText(getString(LanguageKey::GUI_EXIT));

		LORD::GUIButton *watchBattleBtn = getWindow<GUIButton>("DeadSummary-Content-WatchBattle");
		watchBattleBtn->subscribeEvent(EventButtonClick, std::bind(&gui_deadSummary::onWatchBattleClick, this, std::placeholders::_1));
		watchBattleBtn->SetText(getString(LanguageKey::DEAD_SUMMARY_WATCH_BATTLE));

		LORD::GUIButton *contiueBtn = getWindow<GUIButton>("DeadSummary-Content-Contince");
		contiueBtn->subscribeEvent(EventButtonClick, std::bind(&gui_deadSummary::onContiueClick, this, std::placeholders::_1));
		contiueBtn->SetText(getString(LanguageKey::GUI_CONTINUE));

		LORD::GUIButton *adBtn = getWindow<GUIButton>("DeadSummary-Content-Ad");
		adBtn->subscribeEvent(EventButtonClick, std::bind(&gui_deadSummary::onAdClick, this, std::placeholders::_1));
		getWindow<GUIStaticText>("DeadSummary-ContentAd-Text")->SetText(getString(LanguageKey::GUI_ADVERTISE));

		LORD::GUIWindow *rewardTip = getWindow("DeadSummary-Content-Reward-Tip");
		rewardTip->subscribeEvent(EventWindowClick, std::bind(&gui_deadSummary::onRewardTipClick, this, std::placeholders::_1));

		LORD::GUIStaticText *nameTxt = getWindow<GUIStaticText>("DeadSummary-Content-Name");
		nameTxt->subscribeEvent(EventWindowClick, std::bind(&gui_deadSummary::onNameClick, this, std::placeholders::_1));

		LORD::GUIStaticText *title = getWindow<GUIStaticText>("DeadSummary-Title");
		title->SetText(getString(LanguageKey::WORDART_SETTLEMENT));
		title->SetScale({ 1.5, 1.5, 1.5 });

		LORD::GUIStaticText *rewardTitle = getWindow<GUIStaticText>("DeadSummary-Content-RewardTitle");
		rewardTitle->SetText(getString(LanguageKey::DEAD_SUMMARY_REWARD));
		rewardTitle->SetScale({ 1.5, 1.5, 1.5 });

		LORD::GUIStaticText *rankTitle = getWindow<GUIStaticText>("DeadSummary-Content-RankTitle");
		rankTitle->SetText(getString(LanguageKey::DEAD_SUMMARY_RANK));
		rankTitle->SetScale({ 1.5, 1.5, 1.5 });

		m_rewardProgress = getWindow<GUIProgressBar>("DeadSummary-Content-Reward-Slider");
		m_rewardText = getWindow<GUIStaticText>("DeadSummary-Content-Reward-Slider-Text");
		m_subscriptionGuard.add(ReceiveDeadSummaryEvent::subscribe(std::bind(&gui_deadSummary::receiveDeadSummary, this, std::placeholders::_1, std::placeholders::_2)));

		//chicken ui
		eliminatedTipTitle = getWindow<GUIStaticText>("DeadSummary-Content-Eliminated-Tip");
		eliminatedTipTitle->SetText("");

		m_deadSummaryContentEliminated = getWindow<GUILayout>("DeadSummary-Content-Eliminated");
		m_deadSummaryContentEliminated->SetVisible(GameClient::CGame::Instance()->GetGameType() == ClientGameType::Chicken);

		LORD::GUIStaticText *eliminatedRankTitle = getWindow<GUIStaticText>("DeadSummary-Content-Eliminated-RankTitle");
		eliminatedRankTitle->SetText(getString(LanguageKey::DEAD_SUMMARY_ELIMINATED_RANK));
		eliminatedRankTitle->SetScale({ 1.5, 1.5, 1.5 });
	}

	void gui_deadSummary::onUpdate(ui32 nTimeElapse)
	{

	}

	bool gui_deadSummary::onExitClick(const EventArgs & events)
	{
		GameClient::CGame::Instance()->exitGame();
		playSoundByType(ST_Click);
		return true;
	}

	bool gui_deadSummary::onWatchBattleClick(const EventArgs & events)
	{
		getParent()->showMainControl(); 
		playSoundByType(ST_Click);
		return true;
	}

	bool gui_deadSummary::onContiueClick(const EventArgs & events)
	{
		getParent()->showMainControl();
		playSoundByType(ST_Click);
		if (m_isNextServer)
		{
			GameClient::CGame::Instance()->getShellInterface()->nextGame();
		}
		return true;
	}

	bool gui_deadSummary::onAdClick(const EventArgs & events)
	{
		playSoundByType(ST_Click);
		GameClient::CGame::Instance()->getShellInterface()->playAdVideo();
		return true;
	}

	bool gui_deadSummary::onNameClick(const EventArgs & events)
	{
		if (!m_gameResult)
			return false;

		auto window = getWindow("DeadSummary-Content-Name");
		auto rect = window->GetUnclippedOuterRect();
		LORD::UVector2 pos = UVector2(UDim(0.0f, rect.left + window->GetPixelSize().x),
			UDim(0.0f, rect.top - window->GetPixelSize().y / 2));

		playSoundByType(ST_Click);
		getParent()->showSummarPoint(pos, m_gameResult->pointMap);
		return true;
	}

	bool gui_deadSummary::onRewardTipClick(const EventArgs & events)
	{
		if (!m_gameResult)
			return false;

		auto window = getWindow("DeadSummary-Content-Reward-Tip");
		auto rect = window->GetUnclippedOuterRect();
		LORD::UVector2 pos = UVector2(UDim(0.0f, rect.right + 10),
			UDim(0.0f, rect.top - 100));

		playSoundByType(ST_Click);
		getParent()->showSummaryRewardTip(pos, m_gameResult->canGetReward);
		return true;
	}

	void gui_deadSummary::onShow()
	{
		RefreshUI();
		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::Chicken)
		{
			RefreshEliminatedUI();
		}
	}

	void gui_deadSummary::RefreshEliminatedUI() {
		if (!m_gameResult)
			return;

		int     killNum = m_gameResult->killNum;
		int		rank = m_gameResult->rank;
		String	bgImage;
		String	image;
		String	tenImage;
		Color	textColor;
		Color	borderColor;

		GuiSummaryItem::GetImageByRank(killNum, bgImage, image, tenImage, textColor, borderColor);

		LORD::GUIStaticImage *EliminatedRankBgImg = getWindow<GUIStaticImage>("DeadSummary-Content-Eliminated-RankBg");
		EliminatedRankBgImg->SetImage(bgImage.c_str());
		LORD::GUIStaticImage *rankImg = getWindow<GUIStaticImage>("DeadSummary-Content-Eliminated-Rank");
		LORD::GUIWindow *doubleRank = getWindow("DeadSummary-Content-Eliminated-DoubleRank");
		if (killNum > 9)
		{
			doubleRank->SetVisible(true);
			rankImg->SetVisible(false);
			getWindow<GUIStaticImage>("DeadSummary-Content-Eliminated-DoubleRank-Ten")->SetImage(tenImage.c_str());
			getWindow<GUIStaticImage>("DeadSummary-Content-Eliminated-DoubleRank-one")->SetImage(image.c_str());
		}
		else
		{
			doubleRank->SetVisible(false);
			rankImg->SetVisible(true);
			rankImg->SetImage(image.c_str());
		}

		if (rank == 1)
		{
			eliminatedTipTitle->SetText(getString(LanguageKey::DEAD_SUMMARY_CHICKEN_TIP));
			eliminatedTipTitle->SetScale({ 1.5, 1.5, 1.5 });
		}
		else {
			eliminatedTipTitle->SetText(getString(LanguageKey::DEAD_SUMMARY_CHICKEN_TIP_LLUCK));
			eliminatedTipTitle->SetScale({ 1.5, 1.5, 1.5 });
		}

		if (rank <= 2 && rank > 0) {
			getWindow<GUIButton>("DeadSummary-Content-WatchBattle")->SetVisible(false);
		}
	}

	void gui_deadSummary::RefreshUI()
	{
		if (!m_gameResult)
			return;

		int		rank	= m_gameResult->rank;     
		int		reward	= m_gameResult->reward;
		String	name	= m_gameResult->name;

		String	txt;
		String	bgImage;
		String	image;
		String	tenImage;
		Color	textColor;
		Color	borderColor;

		GuiSummaryItem::GetImageByRank(rank, bgImage, image, tenImage, textColor, borderColor);
		LORD::GUIStaticText *nameTxt = getWindow<GUIStaticText>("DeadSummary-Content-Name");
		nameTxt->SetText(name.c_str());
		nameTxt->SetTextColor(textColor);
		nameTxt->SetTextBoader(borderColor);

		LORD::GUIStaticText *rewardTxt = getWindow<GUIStaticText>("DeadSummary-Content-Reward");
		txt = StringUtil::ToString(reward);
		rewardTxt->SetText(txt.c_str());

		m_rewardProgress->SetProgress(m_gameResult->canGetReward == 0 ? 0 : (float)m_gameResult->todayGetReward / m_gameResult->canGetReward);
		txt = StringUtil::Format("%d/%d", m_gameResult->todayGetReward, m_gameResult->canGetReward);
		m_rewardText->SetText(txt.c_str());

		LORD::GUIStaticImage *rankBgImg = getWindow<GUIStaticImage>("DeadSummary-Content-RankBg");
		rankBgImg->SetImage(bgImage.c_str());
		LORD::GUIStaticImage *rankImg = getWindow<GUIStaticImage>("DeadSummary-Content-Rank");
		LORD::GUIWindow *doubleRank = getWindow("DeadSummary-Content-DoubleRank");
		if (rank > 9)
		{
			doubleRank->SetVisible(true);
			rankImg->SetVisible(false);
			getWindow<GUIStaticImage>("DeadSummary-Content-DoubleRank-Ten")->SetImage(tenImage.c_str());
			getWindow<GUIStaticImage>("DeadSummary-Content-DoubleRank-One")->SetImage(image.c_str());
		}
		else
		{
			doubleRank->SetVisible(false);
			rankImg->SetVisible(true);
			rankImg->SetImage(image.c_str());
		}

		String vipIconRes = "";
		switch (m_gameResult->vip)
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
		getWindow<GUIStaticImage>("DeadSummary-Content-VipIcon")->SetImage(vipIconRes.c_str());

#if LORD_PLATFORM == LORD_PLATFORM_ANDROID
		getWindow<GUIButton>("DeadSummary-Content-Ad")->SetVisible(m_gameResult->adSwitch == 1);

		if (m_gameResult->adSwitch == 1)
		{
			GameClient::CGame::Instance()->getShellInterface()->reportAdVideoShow();
		}
#endif
	}

	bool gui_deadSummary::receiveDeadSummary(const String& gameResult, bool isNextServer)
	{
		getParent()->showDeadSummaryGui();
		setIsNextServer(isNextServer);
		auto pResult = LORD::make_shared<ResultEntry>(gameResult);
		m_gameResult = pResult;
		
		return true;
	}

	void gui_deadSummary::setIsNextServer(bool isNextServer)
	{
		this->m_isNextServer = isNextServer;
	}

	void gui_deadSummary::showDeadSummary(const String& gameResult, bool isNextServer) {
		getParent()->showDeadSummaryGui();
		setIsNextServer(isNextServer);
		auto result = ResultEntry::getResultEntryList(gameResult);
		if (result)
		{
			int i = 1;
			for (auto iter = result->begin(); iter != result->end(); iter++, i++)
			{
				if ((*iter)->isSelf)
				{
					m_gameResult = (*iter);
				}
			}
		}
		//auto pResult = LORD::make_shared<ResultEntry>(gameResult);
		//m_gameResult = pResult;
	}
}
