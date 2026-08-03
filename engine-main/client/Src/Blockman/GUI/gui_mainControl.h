/********************************************************************
filename: 	gui_mainControl.h
file path:	dev\client\Src\GUI

version:	1
author:		ajohn
company:	supernano
date:		2016-12-26
*********************************************************************/
#ifndef __GUI_MAIN_CONTROL_HEADER__
#define __GUI_MAIN_CONTROL_HEADER__

#include "Core.h"
#include "gui_layout.h"
#include "GuiSlotTable.h"
#include "UI/GUIWindow.h"
#include "UI/GUIListBox.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticImage.h"
#include "GUI/RootGuiLayout.h"
#include "UI/GUIStaticText.h"
#include "Util/SubscriptionGuard.h"
#include "GUI/GuiActorWindow.h"
#include "UI/GUIButton.h"
#include "GUI/GuiDef.h"
#include "gui_playerOperation.h"
#include "UI/GUILayout.h"
#include "GUI/gui_teamResources.h"

using namespace LORD;

namespace BLOCKMAN
{

	class gui_mainControl : public gui_layout
	{
	private:
		class BirdScoreView
		{
		public:
			GUIStaticText* itemView = nullptr;
			i32 timeLeft = 1000;
			i32 type = 1;
			BirdScoreView(GUIStaticText* view, i32 ty) : itemView(view), type(ty) {}
		};
	private:
		enum class ButtonId
		{
			FORWARD,
			BACK,
			LEFT,
			RIGHT,
			JUMP,
			SNEAK,
			DROP,
			UP,
			DOWN,
			TOP_LEFT,
			TOP_LEFT_BG,
			TOP_RIGHT_BG,
			TOP_RIGHT
		};
		static const ButtonId ALL_BUTTON_ID[];

		SubscriptionGuard m_subscriptionGuard;

	public:
		gui_mainControl();
		virtual ~gui_mainControl();

		const int TashInfoMax = 5;
		struct TaskInfo
		{
			int task_id = 0;
			int task_num = 0;
			int task_type = 0;
			int task_status = 0;
			int task_des = 0;
			int task_param = 0;
			String task_icon = "";
		};

		struct RanchExItemInfo
		{
			String item_img;
			int item_id;
			int item_num;
		};

		bool onItemStackTouchUp(const EventArgs& events);
		bool onItemStackLongTouchStart(const EventArgs& events);
		bool onItemStackLongTouchEnd(const EventArgs& events);
		bool onChangeCurrentItem(int itemIndex, bool isShowItemName);

		void onUpdate(ui32 nTimeElapse) override;
		void onLoad() override;
		void onShow() override;
		bool onGameTypeUpdate();
		bool onJumpDoubleClick(const EventArgs &);
		bool onDropDoubleClick(const EventArgs &);
		bool onButtonPressed(const EventArgs&, ButtonId buttonId);
		bool onButtonReleased(const EventArgs&, ButtonId buttonId);
		bool onCheckChange(const EventArgs &);

		bool showPlayerInventory(const EventArgs& events);
		bool addChatMessage(const String& message);
		bool showTopSystemNotification(const String& message, i32 keepTime);
		bool showBottomSystemNotification(const String& message, i32 keepTime); 
		bool showCenterSystemNotification(const String& message, i32 keepTime);
		bool onBreakBlockProgress(float progress, RAYTRACE_TYPE type);
		bool breakBlockUiManage(bool isShow, float x, float y);
		bool switchMoveControl(bool isDefault);
		bool switchJumpSneak(bool isDefault);
		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}
		bool showParachuteBtn(bool isNeedOpen);
		void showFlyBtn(bool isNeedOpen);
		void showBuildWarBlockBtn(bool isNeedOpen);
		bool onClickChangeActorBtn(const EventArgs & events);
		bool onClickChangeActorYawBtn(const EventArgs & events, bool left);
		bool onClickChangeActorYawLongTouchStartBtn(const EventArgs & events, bool left);
		bool onClickChangeActorYawLongTouchEndBtn(const EventArgs & events, bool left);
		bool onClickChangeCameraStatusBtn(const EventArgs & events);
		bool showHideAndSeekBtn(bool isShowChangeActoBtn, bool isShowLockCameraBtn, bool isShowOthenBtn);
		bool refreshHideAndSeekBtn(const int status);
		bool setPutFurnitureId(i32 furnitureId);
		bool showPlayerOperation(ui64 targetUserId, const String& name);
		bool setManorBtnVisible(bool visible);
		bool hidePlayerOperationView(const EventArgs& events);
		bool refreshBossLife(int currentBossLife, int maxBossLife);
		bool refreshBasementLife(float curLife, float maxLife);
		bool onClickPersonalShopBtn(const EventArgs & events);
		bool onClickBuyBulletBtn(const EventArgs & events);
		void gunFireNotBulletHideOtherUi();
		void LoadSkillReleaseBtn();
		bool onSkillReleaseBtn(const EventArgs & events);
		bool onAutoCastSkill();
		bool castSkill();
		void checkHideReleaseSkillBtnUI();
		void HideSkillUI();
		void updateItemSkillBtnInfo(ui32 timeElapse);
		void refreshItemSkillShowUi();
		void UseEatSkill();
		bool setGameStatus(const int status);
		bool refreshBuildProgress(const String& name, int curProgress, int maxProgress, const String& iconPath);
		bool getTaskInfo(const String& info, std::vector<TaskInfo>& task_list);
		bool getRanchExItemInfo(const String& info, std::vector<RanchExItemInfo>& item_list);
		bool showRanchExTask(bool show, String task);
		bool showRanchExCurrentItemTip(bool show, String itemInfo, int type, int num);
		bool showRanchExItem(bool show, String item);

		bool LoadThrowPotUi();
		bool onAutoRunBtn(const EventArgs & events);
		bool onCloseAutoRunBtn(const EventArgs & events);
		bool onAttackBtn(const EventArgs & events);
		bool onPlaceBuildingClick(const EventArgs& args);

		bool onClickBirdPackBtn(const EventArgs & events);
		bool onClickBirdFuseBtn(const EventArgs & events);
		bool onClickBirdPersonalShopBtn(const EventArgs & events);
		bool onClickBirdAtlasBtn(const EventArgs & events);
		bool onClickBirdActivityBtn(const EventArgs & events);
		bool onHideBirdTaskClick(const EventArgs & events);
		bool onShowBirdTaskClick(const EventArgs & events);
		bool onBirdTaskChange();
		void birdTaskUpdate(i32 nTimeElapse);
		bool hideBirdActivity();

	private:
		void subscribeTouchEvent(GUIWindow* window, ButtonId id);
		void removeChatMessage();
		void scrollBottom();
		void updateDropItemProgress();
		void useWatchModeUI(bool isWatchMode);
		void CheckGunUI();
		void HideGunUI();
		bool tryFireGun(bool bSingleClick);
		bool CanShoot(bool bSingleClick);
		bool onClickReloadBulletBtn(const EventArgs &);
		bool onClickAimBtn(const EventArgs &);
		bool onClickCancelAimBtn(const EventArgs &);
		bool onClickThrowGunBtn(const EventArgs &);
		void refreshGunPreview();
		void checkWatchRespawnUI();
		bool onLeftShootBtnTouchDown(const EventArgs& events);
		bool onLeftShootBtnTouchUp(const EventArgs& events);
		bool onRightShootBtnTouchDown(const EventArgs& events);
		bool onRightShootBtnTouchUp(const EventArgs& events);
		bool onRightShootBtnMoved(const EventArgs& events);
		void updateShootProgress();
		void setAimState(bool bState);
		void checkHideGunUI();
		void checkAimUI();
		bool onClickLockBtn(const EventArgs& events);
		bool onClickUnlockBtn(const EventArgs& events);
		bool onClickTeleportBtn(const EventArgs& events);
		bool onClickParachuteBtn(const EventArgs& events);
		bool onClickFlyBtnDown(const EventArgs& events);
		bool onClickFlyBtnUp(const EventArgs& events);
		bool onClickBuildWarBlockBtn(const EventArgs& events);
		bool onClickShowManorBtn(const EventArgs& events);
		bool onClickShowExItemBtn(const EventArgs& events);
		bool onLockStateChanged(bool bState);
		bool onVehicleStateChanged(bool bOnoff, bool bIsDriver);
		bool onPoleTouchDown(const EventArgs& events);
		bool onPoleTouchUp(const EventArgs& events);
		bool onPoleTouchMove(const EventArgs& events);
		bool setPoleCenterPos(const EventArgs& events);
		bool onClickSwitchMoveState(const EventArgs& events);
		bool onSneakDoubleClick(const EventArgs &);
		bool onClickSkillBtn(const EventArgs&);
		bool onClickPutFurnitureSureBtn(const EventArgs&);
		bool onClickPutFurnitureCancelBtn(const EventArgs&, bool isBtnClick);
		bool onPutFurnitureCancel();
		bool onClickManorPraise(const EventArgs&);
		bool setSkillType(SkillType skillType, float duration, float cdTime);
		void refreshMoveState();
		void updateSkillBtnInfo(ui32 timeElapse);
		void updateCameraBtnUI();
		bool isHideSneakBtn();
		bool isHideStateBtn();
		void updateExpBar();
		bool onWatchRepawnClick(const EventArgs& events);
		void switchCrossHair(int crossHairType);
		void checkCrossHairIsCanAttack();
		void checkFlyBtnShow();
		void switchCrossHairColor(int crossHairType);
		float CheckCollisionCreature(Vector3 beginPos, Vector3 dir, float range, float rangeRatio);
		float CheckCollisionBlock(Vector3 beginPos, Vector3 dir, float range, float rangeRatio);
		float CheckCollisionEntityPlayer(Vector3 beginPos, Vector3 dir, float range, float rangeRatio);
		float CheckCollisionActorNpc(Vector3 beginPos, Vector3 dir, float range, float rangeRatio);
		void checkDistance(float Distace, int  crossHairType, bool isAutoShoot);
		void clearCrossHairColor();
		void checkSkillIsHaveAttackEntity();
		void updateMaskInfo(ui32 nTimeElapse);
		bool syncShowMaskTime(int MilliSecond);
		void ForceAutoRun();
		void updateEndingAnimation();
		bool onClickFishingBtn(const EventArgs&);
		void CheckFishingBtn();
		bool onClickCannonBtn(const EventArgs&);
		bool showCannonBtn(bool isShow, int cannonEntityId);
		void checkPersonalShopUI();
		void LoadRanchExUI();
		bool canUseByUseItemBtn(Item* item);
		void CheckUseItemBtn();
		bool onClickUseItemBtn(const EventArgs&);
		void checkOpenFlyingShow();
		bool onClickOpenFlying(const EventArgs & args);
		void updateHideAndSeekYawChange(ui32 nTimeElapse);
		bool onBirdAddScore(i32 score, i32 scoreType);
		void onBirdDataUpdate(ui32 nTimeElapse);

	private:
		ui64 manorCheckPlayerLocation();
		bool checkPlaceBuildingViewVisible();

	protected:
		static constexpr int MAX_ITEM_COUNT = 9;
		static constexpr float HOLD_ALPHA = 0.5f;
		static constexpr float RELEASE_ALPHA = 0.6f;
		static const int MAX_RANCHEX_ITEM_COUNT = 24;
		static const Vector3 HOLD_SCALE;
		static const Vector3 RELEASE_SCALE;
		static const map<ButtonId, std::reference_wrapper<const String>>::type BUTTON_TO_KEY_BINDINGS;
		map<ButtonId, GUIWindow*>::type m_buttons;
		map<ButtonId, bool>::type m_buttonPressed;
		map<int, Vector2i>::type m_ItemSkillCdTime;
		int m_showMaskTime = -1;
	private:
		i32 m_touchTime = 0;
		i32 m_removeMessageKeepTime = 0;
		i32 m_keepShowTopLeftAndTopRightTime = 0;
		i32 m_keepShowTopSystemNotificationTime = 0;
		i32 m_keepShowCenterSystemNotificationTime = 0;
		i32 m_keepShowBottomSystemNotificationTime = 0;
		i32 m_teleportBtnCountdown = -1;
		int m_dropItemIndex = 0;
		bool m_isLongTouch = false;
		bool m_isShowTopLeftAndTopRight = false;
		bool m_isTouchLeftShootBtn = false;
		bool m_isTouchRightShootBtn = false;
		GuiSlotTable* m_guiSlotTable = nullptr;
		GUIListBox* m_chatMessageListBox = nullptr;
		GUIWindow* m_flyingControls = nullptr;
		GUIStaticImage* m_breakBlockProgress = nullptr;
		GUIWindow* m_watchBattleWindow = nullptr;
		GUIWindow* m_gunOperationWindow = nullptr;
		GUIWindow* m_gunSniperSight = nullptr;
		GUIButton* m_leftShootBtn = nullptr;
		GUIButton* m_rightShootBtn = nullptr;
		GUIButton* m_reloadBulletBtn = nullptr;
		GUIButton* m_aimBtn = nullptr;
		GUIButton* m_cancelAimBtn = nullptr;
		GUIButton* m_throwGunBtn = nullptr;
		GUIButton* m_teleportBtn = nullptr;
		GUIButton* m_showManorBtn = nullptr;
		GUIButton* m_parachuteBtn = nullptr;
		GUIButton* m_flyBtn = nullptr;
		GUIButton* m_buildWarBlockBtn = nullptr;
		GUIButton* m_skillBtn = nullptr;
		GUIWindow* m_hideAndSeekControlWindow = nullptr;
		GUIButton* m_cameraChangeStatusBtn = nullptr;
		GUIButton* m_changeActorBtn = nullptr;
		GUILayout* m_hideandseekChangeYaw = nullptr;
		GUIButton* m_hideandseekChangeYawLeftBtn = nullptr;
		GUIButton* m_hideandseekChangeYawRightBtn = nullptr;
		GUIButton* m_btnPutFurnitureSure = nullptr;
		GUIButton* m_btnPutFurnitureCancel = nullptr;
		GUIButton* m_btnManorPraise = nullptr;
		GUIButton* m_personalShopBtn = nullptr;
		GUIButton* m_buyButtleBtn = nullptr;
		GUIStaticImage* m_cameraLockImage = nullptr;
		GUIStaticImage* m_cameraUnLockImage = nullptr;
		GuiActorWindow* m_actorWindow = nullptr;
		float		m_prevFovSetting = 0.0f;
		float		m_prevMouseSensitive = 0.0f;
		int         m_prevCameraView = 0;
		UVector2    m_originButtonPos;
		GUIWindow*	m_carControlWindow = nullptr;
		GUIStaticText* m_topSystemNotification = nullptr;
		GUIStaticText* m_bottomSystemNotification = nullptr;
		GUIStaticText* m_centerSystemNotification = nullptr;
		UVector2    m_originPolePos;
		GUIWindow*  m_poleCenterWindow = nullptr;
		GUIWindow*  m_poleBgWindow = nullptr;
		UVector2    m_originPoleAbsPos;
		GUIWindow*  m_poleControlWindow = nullptr;
		GUIWindow*  m_arrowControlWindow = nullptr;
		GUIWindow*  m_poleMoveWindow = nullptr;

		GUIWindow* m_sneakStateWindow = nullptr;
		GUIWindow* m_runStateWindow = nullptr;
		GUIWindow* m_sprintStateWindow = nullptr;
		GUILayout* m_playerOperationLayout = nullptr;
		GUIProgressBar*	m_bossLifeProgress = nullptr;
		GUIProgressBar* m_playerExpProgress = nullptr;
		GUIProgressBar* m_basementLifeProgress = nullptr;
		GUIProgressBar* m_hitEntityProgress = nullptr;

		float       m_poleMoveForward = 0.0f;
		float       m_poleMoveStrafe = 0.0f;

		SkillType m_skillType = SkillType::NONE;
		float m_skillDuration = 0.f;
		float m_skillCDTime = 0.f;
		int m_skillResidueCDTime = 0;
		int m_gameStatus = GRME_ROUND_STATUS_WAIT;
		
		i32 m_putFurnitureId = -1;
		i32 m_currentCameraView = 0;
		bool m_manorBtnVisible = false;
		bool m_ranchExItemShow = false;

		GUILayout* m_crossHairLayout = nullptr;
		GUILayout* m_basementLifeLayout = nullptr;
		GUIStaticText* m_basementLifeInfo = nullptr;
		GUIStaticImage* m_moveTopCrossHair = nullptr;
		GUIStaticImage* m_moveLeftCrossHair = nullptr;
		GUIStaticImage* m_moveBottomCrossHair = nullptr;
		GUIStaticImage* m_moveRightCrossHair = nullptr;
		GUIStaticImage* m_shotgunMoveTopCrossHair = nullptr;
		GUIStaticImage* m_shotgunMoveLeftCrossHair = nullptr;
		GUIStaticImage* m_shotgunMoveBottomCrossHair = nullptr;
		GUIStaticImage* m_shotgunMoveRightCrossHair = nullptr;
		GUIStaticImage* m_commonCrossHair = nullptr;
		GUIWindow* m_skillOperationWindow = nullptr;
		GUIButton* m_skillReleaseBtn = nullptr;
		GUILayout* m_throwPotLayout = nullptr;
		GUIWindow* m_autoRunBtnFalseStatus= nullptr;
		GUIWindow* m_autoRunBtnTrueStatus = nullptr;
		GUIButton* m_attackBtn = nullptr;
		GUILayout* m_warningLayout = nullptr;
		GUIStaticImage* m_shootWarning = nullptr;
		GUIStaticImage* m_darkMask = nullptr;
		GUIStaticImage* m_frozenMask = nullptr;
		GUIStaticImage* m_hurtMask = nullptr;
		GUILayout* m_endingAnimationLayout = nullptr;
		GUIStaticImage* m_endingStatus = nullptr;
		GUIStaticImage* m_RanchExItemImg[MAX_RANCHEX_ITEM_COUNT];
		GUIStaticText* m_RanchExItemTxt[MAX_RANCHEX_ITEM_COUNT];
		GUIButton* m_btnPlaceBuilding = nullptr;
		GUIStaticImage * m_bossLifeIcon = nullptr;

		bool m_bChangeCrosshair = false;
		float m_moveTopCrossHairBeginYPos = 0.0f;
		float m_moveLeftCrossHairBeginXPos = 0.0f;
		float m_moveBottomCrossHairBeginYPos = 0.0f;
		float m_moveRightCrossHairBeginXPos = 0.0f;
		float m_shotgunMoveTopCrossHairBeginYPos = 0.0f;
		float m_shotgunMoveLeftCrossHairBeginXPos = 0.0f;
		float m_shotgunMoveBottomCrossHairBeginYPos = 0.0f;
		float m_shotgunMoveRightCrossHairBeginXPos = 0.0f;
		float m_crossHairMoveRange = 20.0f;
		float m_crossHairMoveStepLength = 2.0f;
		float m_crossHairMoveSum = 0.0f;

		bool m_showShootWarning = false;
		float m_shootWarningRotateAngle = 0.0f;
		float m_shootWarningAlpha = 1.0f;

		bool m_playEndingAnimation = false;
		bool m_isZoomInEndingImg = true;
		bool m_beginRotating = false;
		bool m_beginPlayEndingAnimation = false;
		float m_endingAnimationScale = 0.0f;
		float m_endingAnimationRotatingTime = 0.0f;
		float m_endingImgDurationTime = 0.0f;
		String m_gameResult = "";
		bool m_isNextServer = false;

		GUIButton* m_btnFishing = nullptr;
		GUIButton* m_btnCannon = nullptr;
		GUIButton* m_btnOpenFlying = nullptr;
		int m_cannonEntityId = 0;
		GUIButton* m_btnUseItem = nullptr;
		bool m_hideandseekYawChange = false;
		bool m_hideandseekYawLeft = false;
		bool m_isCanChangeCameraStatus = true;
		ui32 m_ChangeCameraStatusCd = 0;


		bool m_isShowBirdTask = false;

		i32 m_birdTaskViewLoadTime = 0;

		GUIButton* m_btnBirdPack = nullptr;
		GUIButton* m_btnBirdFuse = nullptr;
		GUIButton* m_btnBirdPersonalShop = nullptr;
		GUIButton* m_btnBirdAtlas = nullptr;
		GUIButton* m_btnHideTask = nullptr;
		GUIButton* m_btnShowTask = nullptr;
		GUILayout* m_birdTaskLayout = nullptr;
		GUIListBox* m_birdTaskList = nullptr;
		GUIButton* m_birdActivity = nullptr;
		vector<BirdScoreView>::type m_birdScoreViews;

	private:
		gui_playerOperation* m_guiPlayerOperation = nullptr;
		gui_teamResources* m_guiTeamResources = nullptr;

	
	};
}

#endif
