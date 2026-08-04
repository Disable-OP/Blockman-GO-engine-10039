#include "gui_toolBar.h"
#include "UI/GUIStaticText.h"
#include "Util/LanguageKey.h"
#include "UI/GUIStaticImage.h"
#include "Util/ClientEvents.h"
#include "game.h"
#include "GUICommonTeamInfo.h"
#include "cWorld/Blockman.h"
#include "World/Shop.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Inventory/Backpack.h"
#include "ShellInterface.h"
#include "World/Ranch.h"
#include "UI/GUIProgressBar.h"
#include "Util/UICommon.h"
#include "Util/RedDotManager.h"
#include "Setting/GuideSetting.h"
#include "Setting/SeasonSetting.h"
#include "Setting/UIDisplaySetting.h"
#include "Object/Root.h"
#include "World/BirdSimulator.h"

using namespace LORD;

namespace BLOCKMAN
{

	const gui_toolBar::ViewId  gui_toolBar::ALL_VIEW_ID[] = {
		ViewId::SETTING,
		ViewId::CHAT,
		ViewId::ACTION,
		ViewId::APP_SHOP
	};

	gui_toolBar::gui_toolBar()
		: gui_layout("ToolBar.json")
	{

	}

	gui_toolBar::~gui_toolBar()
	{
		m_subscriptionGuard.unsubscribeAll();
		m_broadcasts.clear();
	}

	void gui_toolBar::onLoad()
	{
		m_playerListLayout = getWindow<GUILayout>("ToolBar-Player-List");
		m_playerList = getWindow<GUIListBox>("ToolBar-Player-List-Info");
		m_teamListLayout = getWindow<GUILayout>("ToolBar-Team-List");
		m_gameInfoLayout = getWindow<GUILayout>("ToolBar-GameInfo");
		m_ranchDataLayout = getWindow<GUILayout>("ToolBar-Ranch");

		m_btnRanchOrder = getWindow<GUIButton>("ToolBar-Ranch-Order");
		m_btnRanchOperate = getWindow<GUIButton>("ToolBar-Ranch-Operate");
		m_btnPlayVideo = getWindow<GUIButton>("ToolBar-PlayVideo");
		m_broadcastLayout = getWindow<GUILayout>("ToolBar-Broadcast");
		m_tvBroadcast = getWindow<GUIStaticText >("ToolBar-Broadcast-Content");

		m_birdLayout = getWindow<GUILayout>("ToolBar-Bird");
		m_birdMiniTip = getWindow<GUIButton>("ToolBar-Bird-MiniTip");
		
		m_pixelhall = getWindow<GUILayout>("ToolBar-PixelGunHall");
		m_pixelhall_lv_txt = getWindow<GUIStaticText>("ToolBar-PixelGunHall-LvTxt");
		m_pixelhall_lv_pro = getWindow<GUIProgressBar>("ToolBar-PixelGunHall-LVProgressBar");
		m_pixelhall_rank_txt = getWindow<GUIStaticText>("ToolBar-PixelGunHall-RankTxt");
		m_pixelhall_rank_img = getWindow<GUIStaticImage>("ToolBar-PixelGunHall-RankImg");
		m_pixelhall_honor_txt = getWindow<GUIStaticText>("ToolBar-PixelGunHall-HonorTxt");
		m_pixelhall_yaoshi_txt = getWindow<GUIStaticText>("ToolBar-PixelGunHall-YaoshiTxt");
		m_pixelhall_lv_exp_txt = getWindow<GUIStaticText>("ToolBar-PixelGunHall-LvExp");

		m_pixel_gungame_person_rank_txt = getWindow<GUIStaticText>("ToolBar-PixelGunGamePersonRankTxt");
		m_pixel_gungame_person_time_txt = getWindow<GUIStaticText>("ToolBar-PixelGunGamePersonTimeTxt");
		m_pixel_gungame_person_kill_num_txt = getWindow<GUIStaticText>("ToolBar-PixelGunGamePersonKillNum");

		m_pixel_gungame_1v1_time_txt = getWindow<GUIStaticText>("ToolBar-PixelGunGame1v1TimeTxt");
		m_pixel_gungame_1v1_red_kill_num_txt = getWindow<GUIStaticText>("ToolBar-PixelGunGame1v1RedKillNum");
		m_pixel_gungame_1v1_blue_kill_num_txt = getWindow<GUIStaticText>("ToolBar-PixelGunGame1v1BlueKillNum");

		m_pixel_gungame_team_time_txt = getWindow<GUIStaticText>("ToolBar-PixelGunGameTeamTimeTxt");
		m_pixel_gungame_team_red_kill_num_txt = getWindow<GUIStaticText>("ToolBar-PixelGunGameTeamRedKillNum");
		m_pixel_gungame_team_blue_kill_num_txt = getWindow<GUIStaticText>("ToolBar-PixelGunGameTeamBlueKillNum");
		m_pixel_gungame_team_red_player_num_txt = getWindow<GUIStaticText>("ToolBar-PixelGunGameTeamRedPlayerNum");
		m_pixel_gungame_team_blud_player_num_txt = getWindow<GUIStaticText>("ToolBar-PixelGunGameTeamBluePlayerNum");
		m_pixel_gungame_team_red_select_img = getWindow<GUIStaticImage>("ToolBar-PixelGunGameTeamRedSelfImg");
		m_pixel_gungame_team_blue_select_img = getWindow<GUIStaticImage>("ToolBar-PixelGunGameTeamBlueSelfImg");
		m_pixel_gungame_1v1_red_select_img = getWindow<GUIStaticImage>("ToolBar-PixelGunGame1v1RedSelect");
		m_pixel_gungame_1v1_blue_select_img = getWindow<GUIStaticImage>("ToolBar-PixelGunGame1v1BlueSelect");

		m_btnRanchOrder->subscribeEvent(EventButtonClick, std::bind(&gui_toolBar::onClick, this, std::placeholders::_1, ViewId::RANCH_ORDER));
		m_btnRanchOperate->subscribeEvent(EventButtonClick, std::bind(&gui_toolBar::onClick, this, std::placeholders::_1, ViewId::RANCH_OPERATE));
		m_btnPlayVideo->subscribeEvent(EventButtonClick, std::bind(&gui_toolBar::onClick, this, std::placeholders::_1, ViewId::PLAY_VIDEO));
		m_birdMiniTip->subscribeEvent(EventButtonClick, std::bind(&gui_toolBar::onClick, this, std::placeholders::_1, ViewId::BIRD_MINI_TIP));

		m_btnRanchOrder->SetText(getString(LanguageKey::GUI_RANCH_MAIN_ORDER));
		m_btnRanchOperate->SetText(getString(LanguageKey::GUI_RANCH_MAIN_MAIN));

		m_checkBoxMap[ViewId::CHAT] = getWindow<GUICheckBox>("ToolBar-Chat");
		m_checkBoxMap[ViewId::ACTION] = getWindow<GUICheckBox>("ToolBar-Action");
		m_checkBoxMap[ViewId::SETTING] = getWindow<GUICheckBox>("ToolBar-Setting");
		m_checkBoxMap[ViewId::APP_SHOP] = getWindow<GUICheckBox>("ToolBar-CheckAppShop");

		for (ViewId id : ALL_VIEW_ID)
		{
			m_checkBoxMap[id]->subscribeEvent(EventCheckStateChanged, std::bind(&gui_toolBar::onCheckStateChanged, this, std::placeholders::_1, id));
		}
		
		getWindow("ToolBar-Setting-Red")->SetVisible(false);
		getWindow<GUILayout>("ToolBar-Action-View")->SetVisible(false);
		
		getWindow<GUIStaticText>("ToolBar-Player-List-Munber-Name")->SetText(getString(LanguageKey::TOOL_BAR_PLAYER_LIST_MUNBER_NAME));
		m_checkBoxMap[ViewId::APP_SHOP]->SetText("");//getString(LanguageKey::GUI_STR_APP_SHOP_SHOP));
		structColor(getWindow<GUIStaticText>("ToolBar-Player-List-Munber-Value"), 0, 0);

		using namespace  std::placeholders;
		m_subscriptionGuard.add(UserInEvent::subscribe(std::bind(&gui_toolBar::onUserIn, this, _1, _2, _3, _4, _5, _6)));
		m_subscriptionGuard.add(UserOutEvent::subscribe(std::bind(&gui_toolBar::onUserOut, this, _1)));
		m_subscriptionGuard.add(GameStatusEvent::subscribe(std::bind(&gui_toolBar::setGameStatus, this, _1)));
		m_subscriptionGuard.add(PlayerLifeStatusEvent::subscribe(std::bind(&gui_toolBar::onPlayerLifeStatusChange, this, _1, _2)));
		m_subscriptionGuard.add(GetAllPlayerTeamInfoEvent::subscribe(std::bind(&gui_toolBar::getAllPlayerTeamInfo, this, _1)));
		m_subscriptionGuard.add(TeamBedDestroyedEvent::subscribe(std::bind(&gui_toolBar::onBedWarDestroyBed, this, _1)));
		m_subscriptionGuard.add(PlayerChangeTeamEvent::subscribe(std::bind(&gui_toolBar::onPlayerChangeTeam, this, _1, _2)));
		m_subscriptionGuard.add(ShowMinimapEvent::subscribe(std::bind(&gui_toolBar::showMinimap, this, _1)));
		m_subscriptionGuard.add(ShowRedPointEvent::subscribe(std::bind(&gui_toolBar::showRedPoint, this)));
		m_subscriptionGuard.add(UpdateGameMonsterInfoEvent::subscribe(std::bind(&gui_toolBar::refreshGameExtraInfo, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		m_subscriptionGuard.add(ServerSyncGameTypeEvent::subscribe(std::bind(&gui_toolBar::onGameTypeUpdate, this)));
		m_subscriptionGuard.add(HideRechargeBtnEvent::subscribe(std::bind(&gui_toolBar::onHideRecharge, this)));
		m_subscriptionGuard.add(RanchDataChangeEvent::subscribe(std::bind(&gui_toolBar::onRanchDataChange, this)));
		m_subscriptionGuard.add(BroadcastReceiveEvent::subscribe(std::bind(&gui_toolBar::onBroadcastReceive, this, std::placeholders::_1)));
		
		getWindow<GUIButton>("ToolBar-Currency-Diamond-Add")->subscribeEvent(EventButtonClick, std::bind(&gui_toolBar::onAddDiamond, this, std::placeholders::_1));		getWindow<GUIButton>("ToolBar-Perspece")->subscribeEvent(EventButtonClick, std::bind(&gui_toolBar::onClickSwitchPerspece, this, std::placeholders::_1));
		updatePerspeceIcon(getParent()->getViewNum());

		m_gameTimeInfo = getWindow<GUIStaticText>("ToolBar-GameTime-Info");
		if (m_gameTimeInfo)
		{
			m_gameTimeInfo->SetText("");
		}
		m_subscriptionGuard.add(SyncGameTimeShowUiEvent::subscribe(std::bind(&gui_toolBar::showGameTimeUI, this, std::placeholders::_1, std::placeholders::_2)));
		//m_gameTimeInfo->SetVisible(true);
		m_gameTimeInfo->SetVisible(false);

		//CreepyAliens
		m_extraInfo = getWindow<GUILayout>("ToolBar-Game-Extra-Info");
		m_extraInfo->SetVisible(false);

		m_countDownTip = getWindow<GUIStaticText>("ToolBar-CountDown-Info");
		if (m_countDownTip)
		{
			m_countDownTip->SetText("");
		}
		m_monsterCount = getWindow<GUIStaticText>("ToolBar-Monster-Count");
		if (m_monsterCount)
		{
			m_monsterCount->SetText("");
		}
		m_refreshInfo = getWindow<GUIStaticText>("ToolBar-Refresh-Info");
		if (m_refreshInfo)
		{
			m_refreshInfo->SetText("");
		}
		m_refreshExtraInfo = getWindow<GUIStaticText>("ToolBar-Refresh-Extra-Info");
		if (m_refreshExtraInfo)
		{
			m_refreshExtraInfo->SetText("");
		}

		//RealTimeRank
		bool isShowRealTimeRank = false;
		RealTimeRankLang* rankLang = RealTimeRankLangSetting::getRankLang(GameClient::CGame::Instance()->GetGameType());
		if (rankLang)
			isShowRealTimeRank = true;
		m_teamListLayout->SetArea(UDim(isShowRealTimeRank ? 0.26f : 0.15f, 0), UDim(0, 0), UDim(isShowRealTimeRank ? 0.45f : 0.54f, 0), UDim(1.f, 0));

		getWindow<GUIButton>("ToolBar-Real-Time-Rank-Btn")->SetVisible(isShowRealTimeRank);
		getWindow<GUIStaticText>("ToolBar-Btn-Name")->SetText(getString(LanguageKey::TOOL_BAR_REAL_TIEM_RANK_BTN_NAME));
		getWindow<GUIButton>("ToolBar-Real-Time-Rank-Btn")->subscribeEvent(EventButtonClick, std::bind(&gui_toolBar::onClickRealTimeRank, this, std::placeholders::_1));

		initKillNumber();
		// BuildWar
		getWindow("ToolBar-BuildWar-Tip")->SetVisible(GameClient::CGame::Instance()->GetGameType() == ClientGameType::Build_War);
		getWindow<GUIStaticText>("ToolBar-BuildWar-Title")->SetText(getString(LanguageKey::TOOL_BAR_BUILDWAR_TTILE));
		m_subscriptionGuard.add(ShowBuildWarPlayerNum::subscribe(std::bind(&gui_toolBar::showBuildWarPlayerNum, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(ShowBuildWarLeftTime::subscribe(std::bind(&gui_toolBar::showBuildWarLeftTime, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));

		// RanchEx
		// getWindow("ToolBar-RanchEx-Tip")->SetVisible(GameClient::CGame::Instance()->GetGameType() == ClientGameType::RanchersExplore);
		m_subscriptionGuard.add(ShowRanchExTip::subscribe(std::bind(&gui_toolBar::showRanchExTip, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(GameVideoChangeEvent::subscribe(std::bind(&gui_toolBar::onGameVideoChange, this, std::placeholders::_1)));

		// PixelGun
		m_subscriptionGuard.add(ShowPixelGunHallInfo::subscribe(std::bind(&gui_toolBar::showPixelGunHallInfo, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5)));
		m_subscriptionGuard.add(UpdateCurrentSeasonInfoEvent::subscribe(std::bind(&gui_toolBar::showPlayerHonorInfo, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)));
		m_subscriptionGuard.add(ShowPixelGunGamePerson::subscribe(std::bind(&gui_toolBar::showPixelGunGamePerson, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		m_subscriptionGuard.add(ShowPixelGunGame1v1::subscribe(std::bind(&gui_toolBar::showPixelGunGame1v1, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)));
		m_subscriptionGuard.add(ShowPixelGunGameTeam::subscribe(std::bind(&gui_toolBar::showPixelGunGameTeam, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
			std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8)));

		onGameTypeUpdate();
	}

	void gui_toolBar::initKillNumber()
	{
		m_killNumberWindow = getWindow("ToolBar-KillNumber");
		m_killNumberWindow->SetVisible(false);
		getWindow<GUIStaticText>("ToolBar-KillNumber-LeftTitle")->SetText(getString(LanguageKey::TOOL_BAR_MEMBER_LEFT));
		getWindow<GUIStaticText>("ToolBar-KillNumber-KillTitle")->SetText(getString(LanguageKey::TOOL_BAR_KILL_NUMBER));
		m_subscriptionGuard.add(MemberLeftAndKillUpdateEvent::subscribe(std::bind(&gui_toolBar::refreshKillNumber, this, std::placeholders::_1, std::placeholders::_2)));
	}

	bool gui_toolBar::refreshKillNumber(int left, int kill)
	{
		m_killNumberWindow->SetVisible(true);
		getWindow<GUIStaticText>("ToolBar-KillNumber-LeftNumber")->SetText(StringUtil::ToString(left).c_str());
		getWindow<GUIStaticText>("ToolBar-KillNumber-KillNumber")->SetText(StringUtil::ToString(kill).c_str());
		return false;
	}

	void gui_toolBar::onUpdate(ui32 nTimeElapse)
	{
		structColor(getWindow<GUIStaticText>("ToolBar-Player-List-Munber-Value"), m_listUserInfo.size(), m_totalMember);
		auto player = Blockman::Instance() ? Blockman::Instance()->m_pPlayer : nullptr;
		if (m_checkBoxMap[ViewId::APP_SHOP] && Blockman::Instance()->getWorld()->getShop())
		{
			m_checkBoxMap[ViewId::APP_SHOP]->SetVisible(Blockman::Instance()->getWorld()->getShop()->getGoods().size() > 0 && player && !player->capabilities.isWatchMode);
		}

		if (player)
		{
			getWindow<GUIStaticImage>("ToolBar-Currency-Diamond-Icon")->SetImage(UICommon::getCurrencyIconByType(1).c_str());
			Wallet * wallet = player->m_wallet;
			if (wallet)
			{
				getWindow("ToolBar-Currency-Money")->SetVisible(wallet->getCurrency() >= 0);
				getWindow("ToolBar-Currency-Money-Value")->SetText(StringUtil::ToString(wallet->getCurrency()).c_str());
				getWindow<GUIStaticImage>("ToolBar-Currency-Money-Icon")->SetImage(UICommon::getCurrencyIcon().c_str());
				getWindow("ToolBar-Currency-Diamond")->SetVisible(wallet->getDiamonds() >= 0);
				getWindow("ToolBar-Currency-Diamond-Value")->SetText(StringUtil::ToString(wallet->getDiamonds()).c_str());
			}
			else
			{
				getWindow("ToolBar-Currency-Money")->SetVisible(false);
				getWindow("ToolBar-Currency-Diamond")->SetVisible(false);
			}
			auto backpack = player->getBackpack();
			if (backpack && backpack->getMaxCapacity() != -1 && backpack->getCapacity() != -1)
			{
				getWindow("ToolBar-Backpack")->SetVisible(true);
				getWindow("ToolBar-Backpack-Value")->SetText(StringUtil::Format("%d/%d", backpack->getCapacity(), backpack->getMaxCapacity()).c_str());
			}
			else if (backpack && backpack->getMaxCapacity() == -1 && backpack->getCapacity() != -1)
			{
				getWindow("ToolBar-Backpack")->SetVisible(true);
				getWindow("ToolBar-Backpack-Value")->SetText(StringUtil::ToString(backpack->getCapacity()).c_str());
			}
			else
			{
				getWindow("ToolBar-Backpack")->SetVisible(false);
				getWindow("ToolBar-Backpack-Value")->SetText("0/0");
			}
		}
		if (gameTimeCountdown > 0)
		{
			gameTimeCountdown = gameTimeCountdown - nTimeElapse;

			if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::Creepy_Aliens)
			{
				String msg = StringUtil::Format(getString(LanguageKey::GUI_COUNT_DOWN_INFO).c_str(), (int)(gameTimeCountdown / 1000 + 1));
				m_countDownTip->SetText("");
				m_countDownTip->SetText(msg.c_str());
			}
			else
			{
				String msg = StringUtil::Format(getString(LanguageKey::GUI_HIDE_AND_SEEK_GAME_TIME_INFO).c_str(), (int)(gameTimeCountdown / 1000 + 1));
				m_gameTimeInfo->SetText("");
				m_gameTimeInfo->SetText(msg.c_str());
				m_gameTimeInfo->SetTextScale(1.6f);
			}
		}

		if (m_buildWarLeftTime > 0)
		{
			m_buildWarLeftTime = m_buildWarLeftTime - nTimeElapse;

			String msg = StringUtil::Format(getString(LanguageKey::TOOL_BAR_BUILDWAR_LEFTTIME).c_str(), (int)(m_buildWarLeftTime / 1000 + 1));
			getWindow<GUIStaticText>("ToolBar-BuildWar-LeftTime")->SetText(msg.c_str());
		}

		if (m_ranchExLeftTime > 0)
		{
			m_ranchExLeftTime = m_ranchExLeftTime - nTimeElapse;

			// String msg = StringUtil::Format(getString(LanguageKey::TOOL_BAR_RANCHEX_LEFTTIME).c_str(), (int)(m_ranchExLeftTime / 1000 + 1));
			
			getWindow<GUIStaticText>("ToolBar-RanchEx-LeftTime-Value")->SetText(StringUtil::ToString((int)(m_ranchExLeftTime / 1000 + 1)).c_str());
		}

		if (m_pixel_gun_game_person_time > 0)
		{
			m_pixel_gun_game_person_time = m_pixel_gun_game_person_time - nTimeElapse;

			String time_person_msg = StringUtil::Format("%d", (int)(m_pixel_gun_game_person_time / 1000 + 1));
			m_pixel_gungame_person_time_txt->SetText(time_person_msg.c_str());
			m_pixel_gungame_person_time_txt->SetVisible(true);
		}
		else
		{
			m_pixel_gungame_person_time_txt->SetVisible(false);
		}

		if (m_pixel_gun_game_1v1_time > 0)
		{
			m_pixel_gun_game_1v1_time = m_pixel_gun_game_1v1_time - nTimeElapse;

			String time_1v1_msg = StringUtil::Format("%d", (int)(m_pixel_gun_game_1v1_time / 1000 + 1));
			m_pixel_gungame_1v1_time_txt->SetText(time_1v1_msg.c_str());
			m_pixel_gungame_1v1_time_txt->SetVisible(true);
		}
		else
		{
			m_pixel_gungame_1v1_time_txt->SetVisible(false);
		}

		if (m_pixel_gun_game_team_time > 0)
		{
			m_pixel_gun_game_team_time = m_pixel_gun_game_team_time - nTimeElapse;

			String time_team_msg = StringUtil::Format("%d", (int)(m_pixel_gun_game_team_time / 1000 + 1));
			m_pixel_gungame_team_time_txt->SetText(time_team_msg.c_str());
			m_pixel_gungame_team_time_txt->SetVisible(true);
		}
		else
		{
			m_pixel_gungame_team_time_txt->SetVisible(false);
		}

		ranchViewVisibleManager();
		getWindow("ToolBar-RanchEx-Tip")->SetVisible(GameClient::CGame::Instance()->GetGameType() == ClientGameType::RanchersExplore);
		m_pixelhall->SetVisible(GameClient::CGame::Instance()->GetGameType() == ClientGameType::PixelGunHall);
		getWindow("ToolBar-PixelGunGamePerson")->SetVisible(GameClient::CGame::Instance()->GetGameType() == ClientGameType::PixelGunGamePerson);
		getWindow("ToolBar-PixelGunGame1v1")->SetVisible(GameClient::CGame::Instance()->GetGameType() == ClientGameType::PixelGunGame1v1);
		getWindow("ToolBar-PixelGunGameTeam")->SetVisible(GameClient::CGame::Instance()->GetGameType() == ClientGameType::PixelGunGameTeam);

		onBroadcastPlay(nTimeElapse);
		playVoideVisible();
		updateBirdData();
	}

	bool gui_toolBar::onCheckStateChanged(const  EventArgs& eventArgs, const ViewId viewId)
	{

		if (!Blockman::Instance()->m_pPlayer)
		{
			m_checkBoxMap[viewId]->SetCheckedNoEvent(false);
			return false;
		}

		bool isCheck = m_checkBoxMap[viewId]->GetChecked();
		for (ViewId id : ALL_VIEW_ID)
		{
			if (!isCheck || viewId != id)
			{
				m_checkBoxMap[id]->SetCheckedNoEvent(false);
			}
		}

		if (isCheck)
		{
			switch (viewId)
			{
			case ViewId::SETTING:
				getParent()->showMenu();
				getWindow("ToolBar-Setting-Red")->SetVisible(false);
				break;
			case ViewId::CHAT:
				playSoundByType(ST_OpenPanel);
				getParent()->showChatGui();
				break;
			case ViewId::ACTION:
				getWindow<GUILayout>("ToolBar-Action-View")->SetVisible(true);
				break;
			case ViewId::APP_SHOP:
				playSoundByType(ST_OpenPanel);
				getParent()->showAppShop();
				break;
			default:
				break;

			}
			m_playerListLayout->SetVisible(false);
		}
		else
		{
			getParent()->showMainControl();
		}
		getWindow<GUICheckBox>("ToolBar-AppShopIcon")->SetChecked(m_checkBoxMap[ViewId::APP_SHOP]->GetChecked());
		return true;
	}

	bool gui_toolBar::onUserIn(const  ui64 platformUserId, const  String& userName, const  String& teamName, const int teamId, const int curMunber, const int totalMunber)
	{
		if (userName == "")
			return false;

		for (auto userInfo : m_listUserInfo)
		{
			if (userInfo.userId == platformUserId)
				return false;
		}

		static unsigned count = 0;
		count++;
		String strLayoutName = StringUtil::Format("ToolBar-Player-List-Info-Item-%d", count).c_str();
		String strIconName = StringUtil::Format("ToolBar-Player-List-Info-Item-Team-Icon-%d", count).c_str();
		String strUserName = StringUtil::Format("ToolBar-Player-List-Info-Item-User-Name-%d", count).c_str();

		if (GUIWindowManager::Instance()->GetGUIWindow(strLayoutName.c_str()) || !m_playerList)
		{
			return false;
		}

		m_totalMember = totalMunber;
		m_listUserInfo.push_back(UserInfo(platformUserId, userName, teamName, teamId, strLayoutName, true));

		GUILayout* listInfoItemLayout = (GUILayout*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_LAYOUT, strLayoutName.c_str());
		listInfoItemLayout->SetTouchable(false);
		listInfoItemLayout->SetArea(UDim(0, 0), UDim(0, 0), UDim(1, 0), UDim(0, 40));
		listInfoItemLayout->SetVerticalAlignment(VA_CENTRE);

		GUIStaticImage* temaIconView = (GUIStaticImage*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, strIconName.c_str());
		temaIconView->SetTouchable(false);
		temaIconView->SetArea(UDim(0, 0), UDim(0, 0), UDim(0, 30), UDim(0, 30));
		String iconName = getTeamIcon((ToolBarTeamId)teamId);
		temaIconView->SetImage(iconName.c_str());
		listInfoItemLayout->AddChildWindow(temaIconView);

		GUIStaticText* userNameView = (GUIStaticText*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_TEXT, strUserName.c_str());
		userNameView->SetTouchable(false);
		userNameView->SetText(("▢FFA8C0D1" + userName).c_str());
		userNameView->SetTextVertAlign(VA_CENTRE);
		userNameView->SetArea(UDim(0, 40.0f), UDim(0, 0), UDim(1.0f, -40.0f), UDim(0, 30.0f));
		listInfoItemLayout->AddChildWindow(userNameView);
		m_playerList->AddItem(listInfoItemLayout, false);
		structColor(getWindow<GUIStaticText>("ToolBar-Player-List-Munber-Value"), curMunber, totalMunber);
		m_teamTotalMember[(ToolBarTeamId)teamId] = m_teamTotalMember[(ToolBarTeamId)teamId] + 1;

		bool isSelfTeam = IsTeammate(platformUserId, teamId);
		changeTeamUI((ToolBarTeamId)teamId, true, isSelfTeam);
		setSelfTeam(platformUserId, teamId);
		changeGameInfo();
		return true;
	}

	bool gui_toolBar::onUserOut(const ui64 platformUserId)
	{
		if (!m_playerList)
		{
			return false;
		}

		list<UserInfo>::iterator user;
		for (user = m_listUserInfo.begin(); user != m_listUserInfo.end(); user++)
		{
			UserInfo item = *user;
			if (item.userId == platformUserId)
			{
				m_listUserInfo.remove(*user);
				if (m_teamTotalMember[(ToolBarTeamId)item.teamId] > 0)
					m_teamTotalMember[(ToolBarTeamId)item.teamId] = m_teamTotalMember[(ToolBarTeamId)item.teamId] - 1;
				bool isSelfTeam = IsTeammate(platformUserId, item.teamId);
				changeTeamUI((ToolBarTeamId)item.teamId, false, isSelfTeam);
				m_playerList->DeleteItem(getIndex(item.teamViewName));
				break;
			}
		}
		changeGameInfo();
		return true;
	}

	bool gui_toolBar::setGameStatus(const int status)
	{
		m_gameStatus = status == 1 ? GRME_ROUND_STATUS_START : GRME_ROUND_STATUS_WAIT;

		if (m_playerListLayout)
		{
			m_playerListLayout->SetVisible(m_gameStatus != GRME_ROUND_STATUS_START && !m_isShowMinimap && GameClient::CGame::Instance()->GetGameType() != ClientGameType::Jewel_Knight);
		}

		if (m_gameStatus == GRME_ROUND_STATUS_START && !getParent()->isLoading())
		{
			getParent()->showMainControl();
		}

		if (m_gameStatus == GRME_ROUND_STATUS_WAIT)
		{
			ClearTeamInfo();

			if (Blockman::Instance()->m_pPlayer->isWatchMode())
			{
				GameClient::CGame::Instance()->getNetwork()->getSender()->sendRebirth();
			}
		}

		onGameTypeUpdate();

		return true;
	}

	bool gui_toolBar::onPlayerLifeStatusChange(const ui64 platformUserId, const bool isLife)
	{
		if (!m_playerList)
		{
			return false;
		}
		list<UserInfo>::iterator user;
		for (user = m_listUserInfo.begin(); user != m_listUserInfo.end(); user++)
		{
			if (user->userId == platformUserId)
			{
				user->isLife = isLife;
				bool isSelfTeam = IsTeammate(platformUserId, user->teamId);
				changeTeamUI((ToolBarTeamId)user->teamId, false, isSelfTeam);
				break;
			}
		}
		return true;
	}

	GuiCommonTeamInfo* gui_toolBar::createTeamUI(ToolBarTeamId teamId, bool bCreateNew)
	{
		if (teamId == ToolBarTeamId::INVALID)
		{
			return NULL;
		}

		String strLayoutName = StringUtil::Format("ToolBar-CommonTeamInfo-%d", teamId).c_str();
		GuiCommonTeamInfo* teamInfoItemLayout = (GuiCommonTeamInfo*)GUIWindowManager::Instance()->GetGUIWindow(strLayoutName.c_str());
		if (!teamInfoItemLayout && bCreateNew)
		{
			teamInfoItemLayout = dynamic_cast<GuiCommonTeamInfo*>(GUIWindowManager::Instance()->CreateGUIWindow(GWT_COMMON_TEAM_INFO, strLayoutName.c_str()));
			teamInfoItemLayout->SetTouchable(false);
			teamInfoItemLayout->SetRefreshIconStatus(true);
			//float width = teamInfoItemLayout->getWindowWidth();
			//teamInfoItemLayout->SetArea(UDim(0, width * m_teamListLayout->GetChildCount()), UDim(0, 0), UDim(0, width), UDim(1.0f, 0));
			teamInfoItemLayout->SetArea(UDim(0.25f * m_teamListLayout->GetChildCount(), 0.f), UDim(0, 0), UDim(0.25f, 0), UDim(1.0f, 0));
			m_teamListLayout->AddChildWindow(teamInfoItemLayout);
		}

		return teamInfoItemLayout;
	}

	void gui_toolBar::changeTeamUI(ToolBarTeamId teamId, bool bCreateNew, bool isSelfTeam)
	{
		if (!UIDisplaySetting::getUIDisplay()->isShowTeamUI)
			return;

		GuiCommonTeamInfo* teamInfoItemLayout = createTeamUI(teamId, bCreateNew);
		if (!teamInfoItemLayout)
			return;

		String icon = getTeamIcon(teamId);
		//String info = getTeamInfo(teamId, getTeamMunber((int)teamId), m_teamTotalMember[teamId]);
		String teamName = getTeamName(teamId);
		String member = getMemberString(getTeamMunber((int)teamId), m_teamTotalMember[teamId]);
		teamInfoItemLayout->ShowContent(icon, teamName, member, isSelfTeam);
	}

	void gui_toolBar::resetCheckBox()
	{
		for (ViewId id : ALL_VIEW_ID)
		{
			m_checkBoxMap[id]->SetCheckedNoEvent(false);
		}
		m_playerListLayout->SetVisible(m_gameStatus != GRME_ROUND_STATUS_START && !m_isShowMinimap && GameClient::CGame::Instance()->GetGameType() != ClientGameType::Jewel_Knight);
		getWindow<GUILayout>("ToolBar-Action-View")->SetVisible(false);
		getWindow<GUICheckBox>("ToolBar-AppShopIcon")->SetChecked(false);
	}

	void gui_toolBar::openMenu()
	{
		playSoundByType(ST_OpenPanel);
		m_checkBoxMap[ViewId::SETTING]->SetChecked(true);
	}

	int gui_toolBar::getIndex(String viewName)
	{
		return  m_playerList->getContainerWindow()->GetChildIndex(viewName.c_str());
	}

	int gui_toolBar::getTeamMunber(int teamId)
	{
		list<UserInfo>::iterator user;
		int count = 0;
		for (user = m_listUserInfo.begin(); user != m_listUserInfo.end(); user++)
		{
			UserInfo item = *user;
			count = item.teamId == teamId && item.isLife ? count + 1 : count;
		}
		return count;
	}

	bool gui_toolBar::onRanchDataChange()
	{
		if (UICommon::checkRanchParam() && Blockman::Instance()->m_pPlayer->m_ranch->getInfo())
		{
			RanchInfo* ranchInfo = Blockman::Instance()->m_pPlayer->m_ranch->getInfo();
			m_ranchDataLayout->SetVisible(true);
			getWindow<GUIStaticText>("ToolBar-Ranch-Exp-Level")->SetText(StringUtil::ToString(ranchInfo->level).c_str());
			getWindow<GUIStaticText>("ToolBar-Ranch-Prosperity-Value")->SetText(StringUtil::ToString(ranchInfo->prosperity).c_str());
			getWindow<GUIStaticText>("ToolBar-Ranch-Id-Value")->SetText(StringUtil::ToString((i64)GameClient::CGame::Instance()->getPlatformUserId()).c_str());
			float exp = (float)ranchInfo->exp;
			float nextExp = (float)ranchInfo->nextExp;
			float progress = exp / nextExp;
			getWindow<GUIProgressBar>("ToolBar-Ranch-Exp")->SetProgress(progress);
		}
		return true;
	}

	bool gui_toolBar::onBroadcastReceive(const BroadcastMessage & message)
	{
		m_broadcasts.push_back(message);
		return true;
	}

	void gui_toolBar::onBroadcastPlay(i32 time)
	{
		if (m_broadcastMoveTime == 0 && m_broadcastShowTime == 0 && m_broadcasts.size() > 0)
		{
			BroadcastMessage message = m_broadcasts[0];
			m_broadcastShowTime = message.stayTime;
			m_broadcastMoveTime = message.rollTime;
			m_tvBroadcast->SetText(message.msg.c_str());
			m_broadcasts.erase(m_broadcasts.begin());
		}

		if (m_broadcastMoveTime > 0)
		{
			m_broadcastMoveTime = (m_broadcastMoveTime - time) > 0 ? (m_broadcastMoveTime - time) : 0;
			float p = (float) m_broadcastMoveTime / 5000.f;
			m_tvBroadcast->SetPosition(UVector2(UDim(p, 0), UDim(0, 0)));
		}
		else
		{
			m_tvBroadcast->SetPosition(UVector2(UDim(0, 0), UDim(0, 0)));
		}

		if (m_broadcastMoveTime == 0 && m_broadcastShowTime > 0)
		{
			m_broadcastShowTime = (m_broadcastShowTime - time) > 0 ? (m_broadcastShowTime - time) : 0;
		}

		m_broadcastLayout->SetVisible(m_broadcastShowTime != 0);
	}

	bool gui_toolBar::onHideRecharge()
	{
		auto btnRecharge = getWindow<GUIButton>("ToolBar-Currency-Diamond-Add");
		if (btnRecharge)
		{
			btnRecharge->SetVisible(false);
		}
		return true;
	}


	int gui_toolBar::getSelfTeamId()
	{
		ui64 selfUserId = GameClient::CGame::Instance()->getPlatformUserId();
		list<UserInfo>::iterator user;
		for (user = m_listUserInfo.begin(); user != m_listUserInfo.end(); user++)
		{
			if (user->userId == selfUserId)
			{
				return user->teamId;
			}
		}

		return 0;
	}

	bool gui_toolBar::onClick(const EventArgs & events, ViewId viewId)
	{
		switch (viewId)
		{
		case ViewId::RANCH_OPERATE:
			RootGuiLayout::Instance()->showRanch();
			break;
		case ViewId::RANCH_ORDER:
			if (UICommon::checkRanchParam() && Blockman::Instance()->m_pPlayer->m_ranch->getInfo())
			{
				RanchInfo* ranchInfo = Blockman::Instance()->m_pPlayer->m_ranch->getInfo();
				if (ranchInfo->level >= ranchInfo->taskStartLevel)
				{
					RootGuiLayout::Instance()->showRanchOrder();
				}
				else
				{
					String str = StringUtil::Format(getString("gui_ranch_order_start_level").c_str(), ranchInfo->taskStartLevel);
					ShowRanchCommonTipEvent::emit(str);
				}
			}
			break;
		case ViewId::PLAY_VIDEO: 
			{
				const auto guide = GuideSetting::getGuideVideoItem(m_gameVideoId);
				if (guide)
				{
					if (guide->type == 1)
					{
						String path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), guide->path);
						GameClient::CGame::Instance()->getShellInterface()->playVideo(guide->type, path);
					}
					else
					{
						GameClient::CGame::Instance()->getShellInterface()->playVideo(guide->type, guide->path);
					}
				}
			}
			break;
		case ViewId::BIRD_MINI_TIP:
			UICommon::onMiniTipClick();
			break;
		}

		return true;
	}

	bool gui_toolBar::IsTeammate(const ui64 platformUserId, int teamId)
	{
		if (IsSelf(platformUserId))
			return true;

		int selfTeamId = getSelfTeamId();
		return selfTeamId != 0 && selfTeamId == teamId;
	}

	bool gui_toolBar::IsSelf(const ui64 platformUserId)
	{
		ui64 selfUserId = GameClient::CGame::Instance()->getPlatformUserId();
		return platformUserId == selfUserId;
	}

	void gui_toolBar::structColor(GUIStaticText* textView, int curMunber, int totalMunber)
	{
		textView->SetText(getMemberString(curMunber, totalMunber).c_str());
	}

	String gui_toolBar::getMemberString(int curMember, int totalMember)
	{
		if (curMember == totalMember)
		{//red  FFFF0000 //blue FF0000FF //Green FF00FF00  //Yellow		FFFFFF00  //Write FFFFFFFF
			return StringUtil::Format("▢FF00FF00%d/%d", totalMember, totalMember);
		}
		else
		{
			return StringUtil::Format("▢FFFF0000%d▢FFFFFFFF/%d", curMember, totalMember);
		}
	}

	void gui_toolBar::setSelfTeam(const ui64 platformUserId, const int teamId)
	{
		if (!IsSelf(platformUserId))
			return;
		Blockman::Instance()->m_pPlayer->setTeamId(teamId);
	}

	String gui_toolBar::getTeamIcon(ToolBarTeamId teamId)
	{
		ClientGameType gameType = GameClient::CGame::Instance()->GetGameType();
		const TeamInfo& teamInfo = TeamInfoSetting::getTeamInfo((int)gameType, (int)teamId);
		return teamInfo.TeamIcon.c_str();
	}

	String gui_toolBar::getTeamInfo(ToolBarTeamId teamId, int curMember, int totalMember)
	{
		String teamName = getTeamName(teamId);
		String member = getMemberString(curMember, totalMember);
		return StringUtil::Format("%s %s", teamName.c_str(), member.c_str());
	}

	bool gui_toolBar::onBedWarDestroyBed(int teamId)
	{
		GuiCommonTeamInfo* teamInfoItemLayout = createTeamUI((ToolBarTeamId)teamId, false);
		if (!teamInfoItemLayout)
			return false;

		String icon = getTeamIcon(ToolBarTeamId::INVALID);
		teamInfoItemLayout->SetIcon(icon);
		teamInfoItemLayout->SetRefreshIconStatus(false);
		return true;
	}

	String gui_toolBar::getTeamName(ToolBarTeamId teamId)
	{
		ClientGameType gameType = GameClient::CGame::Instance()->GetGameType();
		const TeamInfo& teamInfo = TeamInfoSetting::getTeamInfo((int)gameType, (int)teamId);
		return getString(teamInfo.TeamName.c_str()).c_str();
	}

	bool gui_toolBar::setPlayerTeamInfo(int teamId, int userId)
	{
		list<UserInfo>::iterator user;
		for (user = m_listUserInfo.begin(); user != m_listUserInfo.end(); user++)
		{
			if (user->userId == userId)
			{
				user->teamId = teamId;
				user->isLife = true;
				return true;
			}
		}

		return false;
	}

	bool gui_toolBar::getAllPlayerTeamInfo(const String& teamInfo)
	{
		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(teamInfo.c_str());

		if (doc->HasParseError())
		{
			LordLogError("The allPlayerTeamInfo HasParseError.");
			LordDelete(doc);
			return false;
		}

		m_teamTotalMember.clear();
		rapidjson::Value arr = doc->GetArray();
		for (size_t i = 0; i < arr.Size(); ++i)
		{
			if (!arr[i].HasMember("teamId") || !arr[i].HasMember("userId"))
			{
				LordLogError("The allPlayerTeamInfo content missed some field.");
				LordDelete(doc);
				return false;
			}

			int teamId = arr[i].FindMember("teamId")->value.GetInt();
			int userId = arr[i].FindMember("userId")->value.GetInt();

			setPlayerTeamInfo(teamId, userId);
			setSelfTeam(userId, teamId);
			auto it = m_teamTotalMember.find((ToolBarTeamId)teamId);
			if (it != m_teamTotalMember.end())
			{
				m_teamTotalMember[(ToolBarTeamId)teamId]++;
			}
			else
			{
				m_teamTotalMember[(ToolBarTeamId)teamId] = 1;
			}
		}

		RefreshTeamInfo();
		LordDelete(doc);
		return true;
	}

	void gui_toolBar::RefreshTeamInfo()
	{
		ClearTeamInfo();
		//create self team UI first
		ui64 selfUserId = GameClient::CGame::Instance()->getPlatformUserId();
		list<UserInfo>::iterator user;
		for (user = m_listUserInfo.begin(); user != m_listUserInfo.end(); user++)
		{
			if (IsTeammate(user->userId, user->teamId))
			{
				changeTeamUI((ToolBarTeamId)user->teamId, true, true);
				break;
			}
		}

		//and then create other team ui
		for (user = m_listUserInfo.begin(); user != m_listUserInfo.end(); user++)
		{
			if (!IsTeammate(user->userId, user->teamId))
			{
				changeTeamUI((ToolBarTeamId)user->teamId, true, false);
			}
		}
	}

	void gui_toolBar::ClearTeamInfo()
	{
		m_teamListLayout->CleanupChildren();
	}

	bool gui_toolBar::onPlayerChangeTeam(ui64 platformUserId, int teamId)
	{
		bool bFind = false;
		list<UserInfo>::iterator user;
		setSelfTeam(platformUserId, teamId);
		for (user = m_listUserInfo.begin(); user != m_listUserInfo.end(); user++)
		{
			if (user->userId == platformUserId && user->teamId != teamId)
			{
				m_teamTotalMember[(ToolBarTeamId)user->teamId]--;
				m_teamTotalMember[(ToolBarTeamId)teamId]++;
				user->teamId = teamId;
				bFind = true;
				break;
			}
		}

		if (bFind)
			RefreshTeamInfo();

		return bFind;
	}

	bool gui_toolBar::onClickSwitchPerspece(const EventArgs & events)
	{
		if (!Blockman::Instance()->m_pPlayer)
		{
			return false;
		}
		switchPerspece();
		return true;
	}

	void  gui_toolBar::switchPerspece() {
		getParent()->switchSerpece();
	}

	void gui_toolBar::updatePerspeceIcon(int ViewNum) {
		String imageRes = "set:new_gui_material.json image:one_perspec_bg";
		switch (ViewNum)
		{
		case 0:
			imageRes = "set:new_gui_material.json image:one_perspec_bg";
			break;
		case 1:
			imageRes = "set:new_gui_material.json image:three_perspec_back_bg";
			break;
		case 2:
			imageRes = "set:new_gui_material.json image:three_perspec_face_bg";
			break;
		default:
			imageRes = "set:new_gui_material.json image:one_perspec_bg";
			break;
		}
		getWindow<GUIButton>("ToolBar-Perspece")->SetNormalImage(imageRes.c_str());
		getWindow<GUIButton>("ToolBar-Perspece")->SetPushedImage(imageRes.c_str());
	}

	bool gui_toolBar::showMinimap(float mapWidth)
	{
		getWindow("ToolBar")->SetArea(UDim(0, 0), UDim(0, 0), UDim(1.f, -mapWidth), UDim(0, 54.0f));
		m_playerListLayout->SetArea(UDim(0, mapWidth), UDim(1.f, mapWidth), UDim(0.f, 250.f), UDim(0, 310.f));
		m_isShowMinimap = mapWidth != 0;
		return true;
	}

	bool gui_toolBar::showGameTimeUI(bool isShow, int gameTime)
	{
		if (!isShow)
		{
			m_gameTimeInfo->SetVisible(false);
			m_extraInfo->SetVisible(false);
			gameTimeCountdown = 0;
		}
		else
		{
			m_gameTimeInfo->SetVisible(true);
			m_extraInfo->SetVisible(GameClient::CGame::Instance()->GetGameType() == ClientGameType::Creepy_Aliens);
			gameTimeCountdown = gameTime * 1000;
		}
		return true;
	}

	bool gui_toolBar::showRedPoint()
	{
		getWindow("ToolBar-Setting-Red")->SetVisible(true);
		return true;
	}

	bool  gui_toolBar::refreshGameExtraInfo(int checkPointNum, int surplusMonster, int surplusCheckPointNum)
	{
		if (checkPointNum >= 0)
		{
			String checkPointNumContent = StringUtil::Format(getString(LanguageKey::GUI_CHECK_POINT_NUMBER).c_str(), checkPointNum);
			m_monsterCount->SetText(checkPointNumContent.c_str());
		}

		if (surplusMonster >= 0)
		{
			String surplusMonsterContent = StringUtil::Format(getString(LanguageKey::GUI_SURPLUS_MONSTER).c_str(), surplusMonster);
			m_refreshInfo->SetText(surplusMonsterContent.c_str());
		}

		if (surplusCheckPointNum >= 0)
		{
			String surplusCheckPointNumContent = StringUtil::Format(getString(LanguageKey::GUI_SURPLUS_CHECK_POINT_NUMBER).c_str(), surplusCheckPointNum);
			m_refreshExtraInfo->SetText(surplusCheckPointNumContent.c_str());
		}

		return true;
	}

	bool gui_toolBar::onClickRealTimeRank(const EventArgs & events)
	{
		if (!Blockman::Instance()->m_pPlayer)
		{
			return false;
		}

		if (getParent()->isShowRealTimeRankStatus())
		{
			getParent()->isColseRealTimeRank(false);
			getParent()->hideRealTimeRank();
		}
		else
		{
			getParent()->showRealTimeRank();
			getParent()->isColseRealTimeRank(true);
		}
		return true;
	}

	bool gui_toolBar::onGameTypeUpdate()
	{
		bool isShowRealTimeRank = false;
		RealTimeRankLang* rankLang = RealTimeRankLangSetting::getRankLang(GameClient::CGame::Instance()->GetGameType());
		if (rankLang)
			isShowRealTimeRank = true;

		getWindow<GUIButton>("ToolBar-Real-Time-Rank-Btn")->SetVisible(isShowRealTimeRank);

		if (m_playerListLayout)
		{
			m_playerListLayout->SetVisible(m_gameStatus != GRME_ROUND_STATUS_START && !m_isShowMinimap && GameClient::CGame::Instance()->GetGameType() != ClientGameType::Jewel_Knight);
		}

		m_birdLayout->SetVisible(false);
		m_gameInfoLayout->SetVisible(false);
		m_ranchDataLayout->SetVisible(false);
		m_btnRanchOperate->SetVisible(false);
		m_btnRanchOrder->SetVisible(false);

		switch (GameClient::CGame::Instance()->GetGameType())
		{
		case ClientGameType::Jewel_Knight:
			if (m_gameStatus == GRME_ROUND_STATUS_START)
			{
				m_gameInfoLayout->SetVisible(false);
				getWindow("ToolBar-GameInfo-Name")->SetText("");
				m_teamListLayout->SetArea(UDim(0.15f, 0), UDim(0, 0), UDim(0.45f, 0), UDim(1.f, 0));
			}
			else
			{
				m_gameInfoLayout->SetVisible(true);
				getWindow("ToolBar-GameInfo-Name")->SetText(getString(LanguageKey::GUI_JEWEL_KNIGHT_GAME_NAME));
				m_teamListLayout->SetArea(UDim(0.30f, 0), UDim(0, 0), UDim(0.54f, 0), UDim(1.f, 0));
			}
			break;
		case ClientGameType::Rancher:
			m_ranchDataLayout->SetVisible(UICommon::checkRanchParam() && Blockman::Instance()->m_pPlayer->m_ranch->getInfo());
			break;
		case  ClientGameType::BirdSimulator:
			m_birdLayout->SetVisible(true);
			break;
		default:
			
			getWindow("ToolBar-GameInfo-Name")->SetText("");
			//m_teamListLayout->SetArea(UDim(0.15f, 0), UDim(0, 0), UDim(0.7344f, 0), UDim(1.f, 0));
			m_teamListLayout->SetArea(UDim(isShowRealTimeRank ? 0.26f : 0.15f, 0), UDim(0, 0), UDim(isShowRealTimeRank ? 0.45f : 0.54f, 0), UDim(1.f, 0));

			break;
		}
		return true;
	}

	bool gui_toolBar::showBuildWarPlayerNum(ui8 cur_num, ui8 max_num)
	{
		String player_num_txt = StringUtil::Format(getString(LanguageKey::TOOL_BAR_BUILDWAR_PLAYERNUM).c_str(), cur_num, max_num);
		getWindow<GUIStaticText>("ToolBar-BuildWar-PlayerNum")->SetText(player_num_txt.c_str());
		return true;
	}
	bool gui_toolBar::showBuildWarLeftTime(bool show, ui16 left_time, const String & theme)
	{
		if (show)
		{
			getWindow<GUILayout>("ToolBar-BuildWar-Theme-Tip")->SetVisible(true);
			getWindow<GUIStaticText>("ToolBar-BuildWar-Theme")->SetText(theme.c_str());
			if (left_time >= 0)
			{
				m_buildWarLeftTime = (ui32)(left_time * 1000);
			}
		}
		else
		{
			getWindow<GUIStaticText>("ToolBar-BuildWar-LeftTime")->SetVisible(false);
			getWindow<GUIStaticImage>("ToolBar-BuildWar-LeftTime-Icon")->SetVisible(false);
			m_buildWarLeftTime = 0;
		}

		return false;
	}

	bool gui_toolBar::showRanchExTip(ui16 type, ui16 num)
	{
		switch (type)
		{
			// left time
		case 1:
			if (num > 0)
			{
				getWindow<GUILayout>("ToolBar-RanchEx-LeftTime-Layout")->SetVisible(true);
				m_ranchExLeftTime = (ui32)(num * 1000);
			}
			else
			{
				getWindow<GUILayout>("ToolBar-RanchEx-LeftTime-Layout")->SetVisible(false);
			}
			break;
			// room num
		case 2:
			if (num > 0)
			{
				getWindow<GUIStaticText>("ToolBar-RanchEx-NotFindRoom")->SetVisible(true);
				String room_num_txt = StringUtil::Format(getString(LanguageKey::TOOL_BAR_RANCHEX_ROOM_INFO).c_str(), num);
				getWindow<GUIStaticText>("ToolBar-RanchEx-NotFindRoom")->SetText(room_num_txt.c_str());
			}
			else
			{
				getWindow<GUIStaticText>("ToolBar-RanchEx-NotFindRoom")->SetVisible(false);
			}
			break;
			// box num
		case 3:
			if (num > 0)
			{
				getWindow<GUILayout>("ToolBar-RanchEx-NotFindBox-Layout")->SetVisible(true);
				// String box_num_txt = StringUtil::Format(getString(LanguageKey::TOOL_BAR_RANCHEX_BOX_INFO).c_str(), num);
				getWindow<GUIStaticText>("ToolBar-RanchEx-NotFindBox-Value")->SetText(StringUtil::ToString(num).c_str());
			}
			else
			{
				getWindow<GUILayout>("ToolBar-RanchEx-NotFindBox-Layout")->SetVisible(false);
			}
			break;
		default:
			break;
		}

		return true;
	}

	void gui_toolBar::changeGameInfo()
	{
		structColor(getWindow<GUIStaticText>("ToolBar-GameInfo-Num"), m_listUserInfo.size(), m_totalMember);
	}

	bool gui_toolBar::onAddDiamond(const EventArgs)
	{
		GameClient::CGame::Instance()->getShellInterface()->onRecharge(1);
		return true;
	}

	bool gui_toolBar::onGameVideoChange(i32 videoId)
	{
		m_gameVideoId = videoId;
		return true;
	}

	bool gui_toolBar::showPixelGunHallInfo(int lv, int cur_exp, int max_exp, int yaoshi, bool is_max)
	{
		String lv_txt = StringUtil::Format("%d", lv);
		m_pixelhall_lv_txt->SetText(lv_txt.c_str());
		
		if (!is_max)
		{
			if (max_exp > 0)
			{
				String exp_txt = StringUtil::Format("%d/%d", cur_exp, max_exp);
				m_pixelhall_lv_exp_txt->SetText(exp_txt.c_str());
				m_pixelhall_lv_exp_txt->SetVisible(true);
				m_pixelhall_lv_pro->SetProgress((float)cur_exp / (float)max_exp);
				m_pixelhall_lv_exp_txt->SetVisible(true);
			}
			else
			{
				m_pixelhall_lv_exp_txt->SetVisible(false);
				m_pixelhall_lv_pro->SetProgress(0.f);
			}
		}
		else
		{
			m_pixelhall_lv_pro->SetProgress(0.f);
			m_pixelhall_lv_exp_txt->SetText("MAX");
			m_pixelhall_lv_exp_txt->SetVisible(true);
		}

		String yaoshi_txt = StringUtil::Format("%d", yaoshi);
		m_pixelhall_yaoshi_txt->SetText(yaoshi_txt.c_str());
		return true;
	}

	bool gui_toolBar::showPixelGunGamePerson(int time, int rank, int kill_num)
	{
		m_pixel_gun_game_person_time = time * 1000;

		String rankTxt = StringUtil::Format("%d", rank);
		m_pixel_gungame_person_rank_txt->SetText(rankTxt.c_str());

		String killNumTxt = StringUtil::Format("%d", kill_num);
		m_pixel_gungame_person_kill_num_txt->SetText(killNumTxt.c_str());

		return true;
	}

	bool gui_toolBar::showPixelGunGame1v1(int time, int red_kill_num, int blue_kill_num, int self_team)
	{
		m_pixel_gun_game_1v1_time = time * 1000;

		String redKillNumTxt = StringUtil::Format("%d", red_kill_num);
		m_pixel_gungame_1v1_red_kill_num_txt->SetText(redKillNumTxt.c_str());

		String blueKillNumTxt = StringUtil::Format("%d", blue_kill_num);
		m_pixel_gungame_1v1_blue_kill_num_txt->SetText(blueKillNumTxt.c_str());

		if (self_team == PTT_RED)
		{
			m_pixel_gungame_1v1_red_select_img->SetVisible(true);
			m_pixel_gungame_1v1_blue_select_img->SetVisible(false);
		}
		else if (self_team == PTT_BLUE)
		{
			m_pixel_gungame_1v1_red_select_img->SetVisible(false);
			m_pixel_gungame_1v1_blue_select_img->SetVisible(true);
		}
		else
		{
			m_pixel_gungame_1v1_red_select_img->SetVisible(false);
			m_pixel_gungame_1v1_blue_select_img->SetVisible(false);
		}


		return true;
	}

	bool gui_toolBar::showPixelGunGameTeam(int time, int red_kill_num, int red_cur_player_num, int red_max_player_num, int blue_kill_num, int blue_cur_player_num, int blue_max_player_num, int self_team)
	{
		m_pixel_gun_game_team_time = time * 1000;

		String redKillNumTxt = StringUtil::Format("%d", red_kill_num);
		m_pixel_gungame_team_red_kill_num_txt->SetText(redKillNumTxt.c_str());

		String blueKillNumTxt = StringUtil::Format("%d", blue_kill_num);
		m_pixel_gungame_team_blue_kill_num_txt->SetText(blueKillNumTxt.c_str());

		String redPlayerNumTxt = StringUtil::Format("%d/%d", red_cur_player_num, red_max_player_num);
		m_pixel_gungame_team_red_player_num_txt->SetText(redPlayerNumTxt.c_str());

		String bluePlayerNumTxt = StringUtil::Format("%d/%d", blue_cur_player_num, blue_max_player_num);
		m_pixel_gungame_team_blud_player_num_txt->SetText(bluePlayerNumTxt.c_str());

		if (self_team == PTT_RED)
		{
			m_pixel_gungame_team_red_select_img->SetVisible(true);
			m_pixel_gungame_team_blue_select_img->SetVisible(false);
		}
		else if (self_team == PTT_BLUE)
		{
			m_pixel_gungame_team_red_select_img->SetVisible(false);
			m_pixel_gungame_team_blue_select_img->SetVisible(true);
		}
		else
		{
			m_pixel_gungame_team_red_select_img->SetVisible(false);
			m_pixel_gungame_team_blue_select_img->SetVisible(false);
		}
		

		return true;
	}

	bool gui_toolBar::showPlayerHonorInfo(int honorId, int rank, int honor, int endTime)
	{

		String honor_txt = StringUtil::Format("%d", honor);
		m_pixelhall_honor_txt->SetText(honor_txt.c_str());

		String rank_txt = StringUtil::Format("NO:%d", rank);
		m_pixelhall_rank_txt->SetText(rank_txt.c_str());

		Season* season = SeasonSetting::findSeasonByHonorId((SeasonHonorId)honorId);
		if (season)
		{
			m_pixelhall_rank_img->SetImage(season->HonorIcon.c_str());
		}
		return true;
	}
	
	void gui_toolBar::ranchViewVisibleManager()
	{
		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::Rancher)
		{
			m_btnRanchOperate->GetChildByIndex(0)->SetVisible(RedDotManager::checkRanchMain());
			m_btnRanchOrder->GetChildByIndex(0)->SetVisible(RedDotManager::checkOrderMain());
			if (UICommon::isRanchIconVisible())
			{
				m_btnRanchOperate->SetVisible(true);
				m_btnRanchOrder->SetVisible(true);
			}
			else
			{
				m_btnRanchOperate->SetVisible(false);
				m_btnRanchOrder->SetVisible(false);
			}
		}
	}

	void gui_toolBar::playVoideVisible()
	{
		m_btnPlayVideo->SetVisible(GameClient::CGame::Instance()->GetGameType() == ClientGameType::Rancher && m_gameVideoId != 0);
	}

	bool gui_toolBar::updateBirdData()
	{
		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::BirdSimulator && UICommon::checkBirdParam())
		{
			i64 score = Blockman::Instance()->m_pPlayer->m_birdSimulator->getScore();
			i64 eggTicket = Blockman::Instance()->m_pPlayer->m_birdSimulator->getEggTicket();
			getWindow("ToolBar-Bird-Score-Value")->SetText(StringUtil::ToString(score).c_str());
			getWindow("ToolBar-Bird-EggTicket-Value")->SetText(StringUtil::ToString(eggTicket).c_str());
			BirdScope scope = Blockman::Instance()->m_pPlayer->m_birdSimulator->getScopeByPosition(Blockman::Instance()->m_pPlayer->position);
			if (UICommon::checkBirdMiniTip(scope))
			{
				m_birdMiniTip->SetVisible(true);
				m_birdMiniTip->SetText(UICommon::getBirdMiniTipText(scope).c_str());
				m_birdMiniTip->SetNormalImage(UICommon::checkBirdConvert() ? "set:bird_task.json image:frame_pre" : "set:bird_task.json image:frame");
				m_birdMiniTip->SetPushedImage(UICommon::checkBirdConvert() ? "set:bird_task.json image:frame_pre" : "set:bird_task.json image:frame");
				m_birdMiniTip->SetProperty(GWP_TEXT_SHADOW_COLOR, UICommon::checkBirdConvert() ? "0.298039 0 0.0196078 1" : "0.101961 0.298039 0.262745 1");
			}
			else
			{
				m_birdMiniTip->SetVisible(false);
			}
		}
		return false;
	}

}