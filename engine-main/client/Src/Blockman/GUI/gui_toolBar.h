#pragma once
#ifndef __GUI_TOOL_BAR_HEADER__
#define __GUI_TOOL_BAR_HEADER__
#include "Core.h"
#include "gui_layout.h"
#include "UI/GUIWindow.h"
#include "RootGuiLayout.h"
#include "UI/GUIListBox.h"
#include "UI/GUILayout.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIProgressBar.h"
#include "Util/SubscriptionGuard.h"
#include "GUI/GuiDef.h"
#include "Setting/TeamInfoSetting.h"
#include "Setting/RankLangSetting.h"
#include "Util/BroadcastManager.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GuiCommonTeamInfo;
	class gui_toolBar : public gui_layout
	{
	private:
		enum class ViewId
		{
			SETTING,
			CHAT,
			ACTION,
			APP_SHOP,
			RANCH_OPERATE,
			RANCH_ORDER,
			PLAY_VIDEO,
			BIRD_MINI_TIP
		};

		struct UserInfo {
			ui64 userId = 0;
			String userName;
			String teamName;
			String teamViewName;
			bool isLife = false;
			int teamId = 0;
			int curMunber = 0;
			int totalMunber = 0;
			UserInfo(ui64 userId, String userName, String teamName, int teamId, String teamViewName, bool isLife) {
				this->userId = userId;
				this->userName = userName;
				this->teamName = teamName;
				this->teamId = teamId;
				this->teamViewName = teamViewName;
				this->isLife = isLife;
			}
			bool operator==(const UserInfo& other) const {
				return userId == other.userId;
			}

		};

	public:
		enum PixelTeamType
		{
			PTT_BLUE = 1,
			PTT_RED = 2,
		};

	public:
		gui_toolBar();
		virtual ~gui_toolBar();
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		void resetCheckBox();
		void openMenu();
		bool onBedWarDestroyBed(int teamId);
		bool getAllPlayerTeamInfo(const String& teamInfo);
		int getSelfTeamId();
		bool onClick(const EventArgs & events, ViewId viewId);
		bool onClickSwitchPerspece(const EventArgs & events);
		void switchPerspece();
		void updatePerspeceIcon(int ViewNum);
		bool showMinimap(float mapWidth);
		bool showGameTimeUI(bool isShow, int gameTime);
		bool showRedPoint();
		bool refreshGameExtraInfo(int checkPointNum, int surplusMonster, int surplusCheckPointNum);
		bool onClickRealTimeRank(const EventArgs & events);
		bool showBuildWarPlayerNum(ui8 cur_num, ui8 max_num);
		bool showBuildWarLeftTime(bool show, ui16 left_time, const String& theme);
		bool showRanchExTip(ui16 type, ui16 num);
		bool onGameTypeUpdate();
		bool onAddDiamond(const EventArgs);
		bool onGameVideoChange(i32 videoId);
		bool showPixelGunHallInfo(int lv, int cur_exp, int max_exp, int yaoshi, bool is_max);
		bool showPixelGunGamePerson(int time, int rank, int kill_num);
		bool showPixelGunGame1v1(int time, int red_kill_num, int blue_kill_num, int self_team);
		bool showPixelGunGameTeam(int time, int red_kill_num, int red_cur_player_num, int red_max_player_num, int blue_kill_num, int blue_cur_player_num, int blue_max_player_num, int self_team);
		bool showPlayerHonorInfo(int honorId, int rank, int honor, int endTime);

	private:
		bool onCheckStateChanged(const  EventArgs& eventArgs, const  ViewId viewId);
		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}
		bool onUserIn(const  ui64 platformUserId, const  String& userName, const  String& teamName, const int team, const int curMunber, const int totalMunber);
		bool onUserOut(const ui64 platformUserId);
		bool setGameStatus(const int status);
		bool onPlayerLifeStatusChange(const ui64 platformUserId, const bool isLife);
		void structColor(GUIStaticText* textView, int curMunber, int totalMunber);
		void changeTeamUI(ToolBarTeamId teamId, bool bCreateNew, bool isSelfTeam = false);
		bool setPlayerTeamInfo(int teamId, int userId);
		void RefreshTeamInfo();
		void ClearTeamInfo();
		bool onPlayerChangeTeam(ui64 platformUserId, int teamId);
		void initKillNumber();
		bool refreshKillNumber(int left, int kill);
	
		int getIndex(String viewName);
		int getTeamMunber(int teamId);
		bool onRanchDataChange();
		bool onBroadcastReceive(const BroadcastMessage& message);
		void onBroadcastPlay(i32 time);
		bool onHideRecharge();
		

	private:
		GuiCommonTeamInfo* createTeamUI(ToolBarTeamId teamId, bool bCreateNew);
		bool IsTeammate(const ui64 platformUserId, int teamId);
		bool IsSelf(const ui64 platformUserId);
		String getTeamIcon(ToolBarTeamId teamId);
		String getTeamInfo(ToolBarTeamId teamId, int curMember, int totalMember);
		String getTeamName(ToolBarTeamId teamId);
		String getMemberString(int curMember, int totalMember);
		void  setSelfTeam(const ui64 platformUserId, const int teamId);
		void changeGameInfo();
		void ranchViewVisibleManager();
		void playVoideVisible();
		bool updateBirdData();

	private:
		int m_gameVideoId = 0;
		bool m_isShowMinimap = false;
		int m_totalMember = 0;
		int m_gameStatus = GRME_ROUND_STATUS_WAIT;
		static const ViewId ALL_VIEW_ID[];
		int gameTimeCountdown = 0;
		int realTimeRankRefreshTime = 10 * 1000;
		ui32 m_buildWarLeftTime = 0;
		ui32 m_ranchExLeftTime = 0;
		i32 m_broadcastShowTime = 0;
		i32 m_broadcastMoveTime = 0;

		int m_pixel_gun_game_person_time = 0;
		int m_pixel_gun_game_1v1_time = 0;
		int m_pixel_gun_game_team_time = 0;

		list<UserInfo>::type m_listUserInfo;
		map<ToolBarTeamId, int>::type  m_teamTotalMember;
		map<ViewId, GUICheckBox*>::type m_checkBoxMap;

		SubscriptionGuard m_subscriptionGuard;
		GUILayout* m_teamListLayout = nullptr;
		GUILayout* m_playerListLayout = nullptr;
		GUIListBox* m_playerList = nullptr;
		GUIWindow* m_killNumberWindow = nullptr;
		GUIStaticText* m_gameTimeInfo = nullptr;

		GUILayout* m_extraInfo = nullptr;
		GUIStaticText* m_countDownTip = nullptr;
		GUIStaticText* m_monsterCount = nullptr;
		GUIStaticText* m_refreshInfo = nullptr;
		GUIStaticText* m_refreshExtraInfo = nullptr;
		GUILayout* m_gameInfoLayout = nullptr;
		GUILayout* m_ranchDataLayout = nullptr;
		GUIButton* m_btnRanchOperate = nullptr;
		GUIButton* m_btnRanchOrder = nullptr;
		GUILayout* m_broadcastLayout = nullptr;
		GUIStaticText* m_tvBroadcast = nullptr;
		GUIButton* m_btnPlayVideo = nullptr;
		vector<BroadcastMessage>::type m_broadcasts;

		GUILayout* m_birdLayout = nullptr;
		GUIButton* m_birdMiniTip = nullptr;
		GUILayout* m_pixelhall = nullptr;
		GUIStaticText* m_pixelhall_lv_txt = nullptr;
		GUIProgressBar* m_pixelhall_lv_pro = nullptr;
		GUIStaticText* m_pixelhall_rank_txt = nullptr;
		GUIStaticImage* m_pixelhall_rank_img = nullptr;
		GUIStaticText* m_pixelhall_honor_txt = nullptr;
		GUIStaticText* m_pixelhall_yaoshi_txt = nullptr;
		GUIStaticText* m_pixelhall_lv_exp_txt = nullptr;

		GUIStaticText* m_pixel_gungame_person_rank_txt = nullptr;
		GUIStaticText* m_pixel_gungame_person_time_txt = nullptr;
		GUIStaticText* m_pixel_gungame_person_kill_num_txt = nullptr;

		GUIStaticText* m_pixel_gungame_1v1_time_txt = nullptr;
		GUIStaticText* m_pixel_gungame_1v1_red_kill_num_txt = nullptr;
		GUIStaticText* m_pixel_gungame_1v1_blue_kill_num_txt = nullptr;

		GUIStaticText* m_pixel_gungame_team_time_txt = nullptr;
		GUIStaticText* m_pixel_gungame_team_red_kill_num_txt = nullptr;
		GUIStaticText* m_pixel_gungame_team_blue_kill_num_txt = nullptr;
		GUIStaticText* m_pixel_gungame_team_red_player_num_txt = nullptr;
		GUIStaticText* m_pixel_gungame_team_blud_player_num_txt = nullptr;
		GUIStaticImage* m_pixel_gungame_team_red_select_img = nullptr;
		GUIStaticImage* m_pixel_gungame_team_blue_select_img = nullptr;
		GUIStaticImage* m_pixel_gungame_1v1_red_select_img = nullptr;
		GUIStaticImage* m_pixel_gungame_1v1_blue_select_img = nullptr;
	};
}

#endif
