#include "gui_mainControl.h"

#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "cEntity/PlayerControlller.h"
#include "GUI/GuiSlotTable.h"
#include "Network/ClientNetwork.h"
#include "Network/ClientPacketSender.h"

#include "World/GameSettings.h"
#include "Entity/Entity.h"
#include "Inventory/InventoryPlayer.h"
#include "GUI/GuiSlotTable.h"
#include "UI/GUIButton.h"
#include "UI/GUIListBox.h"
#include "UI/GUIStaticText.h"
#include "cEntity/PlayerControlller.h"
#include "Entity/EntityItem.h"
#include "Inventory/InventoryPlayer.h"
#include "World/World.h"
#include "Util/Random.h"
#include "UI/GUIStaticImage.h"
#include "Math/MathLib.h"
#include <functional>
#include "game.h"
#include "Util/CommonEvents.h"
#include "Util/LanguageKey.h"
#include "Setting/TeamInfoSetting.h"
#include "Item/Items.h"
#include "Setting/GunSetting.h"
#include "Setting/BulletClipSetting.h"
#include "Setting/UIDisplaySetting.h"
#include "Util/LanguageManager.h"
#include "UI/GUICheckBox.h"
#include "Entity/EntityVehicle.h"
#include "Enums/Enums.h"
#include "Object/Root.h"
#include "Setting/MultiLanTipSetting.h"
#include "Render/RenderEntity.h"
#include "cEntity/EntityActorCannonClient.h"
#include "Util/UICommon.h"
#include "GUI/gui_birdPackAndFuse.h"
#include "Setting/LogicSetting.h"
#include "EntityRenders/EntityRenderManager.h"
#include "Actor/ActorObject.h"
#include "World/BirdSimulator.h"
#include "GuiBirdTaskItem.h"
#include "GuiDef.h"
#include "UI/GUIFontManager.h"
#include "Util/RedDotManager.h"

namespace BLOCKMAN
{

	const gui_mainControl::ButtonId gui_mainControl::ALL_BUTTON_ID[] = {
		ButtonId::FORWARD,
		ButtonId::BACK,
		ButtonId::LEFT,
		ButtonId::RIGHT,
		ButtonId::JUMP,
		ButtonId::SNEAK,
		ButtonId::UP,
		ButtonId::DOWN,
		ButtonId::TOP_LEFT,
		ButtonId::TOP_LEFT_BG,
		ButtonId::TOP_RIGHT,
		ButtonId::TOP_RIGHT_BG
	};
	const Vector3 gui_mainControl::HOLD_SCALE = Vector3(1.05f, 1.05f, 1.05f);
	const Vector3 gui_mainControl::RELEASE_SCALE = Vector3(1.f, 1.f, 1.f);
	const map<gui_mainControl::ButtonId, std::reference_wrapper<const String>>::type gui_mainControl::BUTTON_TO_KEY_BINDINGS = {
		{ ButtonId::FORWARD,       KeyBinding::KEY_FORWORD },
		{ ButtonId::BACK,          KeyBinding::KEY_BACK },
		{ ButtonId::LEFT,          KeyBinding::KEY_LEFT },
		{ ButtonId::RIGHT,         KeyBinding::KEY_RIGHT },
		{ ButtonId::JUMP,          KeyBinding::KEY_JUMP },
		{ ButtonId::UP,            KeyBinding::KEY_JUMP },
		{ ButtonId::DOWN,          KeyBinding::KEY_SNEAK },
		{ ButtonId::TOP_LEFT,      KeyBinding::KEY_TOP_LEFT },
		{ ButtonId::TOP_LEFT_BG,   KeyBinding::KEY_TOP_LEFT },
		{ ButtonId::TOP_RIGHT,     KeyBinding::KEY_TOP_RIGHT },
		{ ButtonId::TOP_RIGHT_BG,  KeyBinding::KEY_TOP_RIGHT }
	};

	gui_mainControl::gui_mainControl()
		: gui_layout("Main.json")
	{
	}

	gui_mainControl::~gui_mainControl()
	{
		for (auto& pair : m_buttons)
		{
			pair.second->removeAllEvents();
		}

		LordDelete(m_guiPlayerOperation);
		m_guiPlayerOperation = nullptr;

		if (m_guiTeamResources)
		{
			LordDelete(m_guiTeamResources);
			m_guiTeamResources = nullptr;
		}

		removeAllEventsOfWindow("MainControl-sneak");
		removeAllEventsOfWindow("MainControl-jump");
		removeAllEventsOfWindow("Main-Sneak");
		removeAllEventsOfWindow("Main-Jump");
		removeAllEventsOfWindow("Main-ToggleInventoryButton");
		removeAllEventsOfWindow("Main-VisibleBar-SlotTable");
		removeAllEventsOfWindow("Main-VisibleBar-SlotTable");
		removeAllEventsOfWindow("Main-VisibleBar-SlotTable");
		removeAllEventsOfWindow("Main-BtnChat");
		removeAllEventsOfWindow("Main-VisibleBar-SlotTable");

		for (auto& item : m_birdScoreViews)
		{
			if (item.itemView)
			{
				m_rootWindow->RemoveChildWindow(item.itemView);
				GUIWindowManager::Instance()->DestroyGUIWindow(item.itemView);
				item.itemView = nullptr;
			}
		}
		m_birdScoreViews.clear();

		m_subscriptionGuard.unsubscribeAll();
		m_ItemSkillCdTime.clear();
	}

	void gui_mainControl::onLoad()
	{
		//LordAssertX(!m_rootWindow, "gui_mainControl has initEvents already!");	
		m_rootWindow->SetTouchable(false);
		m_crossHairLayout = getWindow<GUILayout>("Main-Gun-CrossHairs");
		m_crossHairLayout->SetVisible(false);
		m_commonCrossHair = getWindow<GUIStaticImage>("Main-Common-CrossHair");
		m_moveTopCrossHair = getWindow<GUIStaticImage>("Main-MoveTop-CrossHair");
		m_moveLeftCrossHair = getWindow<GUIStaticImage>("Main-MoveLeft-CrossHair");
		m_moveBottomCrossHair = getWindow<GUIStaticImage>("Main-MoveBottom-CrossHair");
		m_moveRightCrossHair = getWindow<GUIStaticImage>("Main-MoveRight-CrossHair");
		m_moveTopCrossHairBeginYPos = m_moveTopCrossHair->GetYPosition().d_offset;
		m_moveLeftCrossHairBeginXPos = m_moveLeftCrossHair->GetXPosition().d_offset;
		m_moveBottomCrossHairBeginYPos = m_moveBottomCrossHair->GetYPosition().d_offset;
		m_moveRightCrossHairBeginXPos = m_moveRightCrossHair->GetXPosition().d_offset;
		m_shotgunMoveTopCrossHair = getWindow<GUIStaticImage>("Main-Shotgun-MoveTop-CrossHair");
		m_shotgunMoveLeftCrossHair = getWindow<GUIStaticImage>("Main-Shotgun-MoveLeft-CrossHair");
		m_shotgunMoveBottomCrossHair = getWindow<GUIStaticImage>("Main-Shotgun-MoveBottom-CrossHair");
		m_shotgunMoveRightCrossHair = getWindow<GUIStaticImage>("Main-Shotgun-MoveRight-CrossHair");
		m_shotgunMoveTopCrossHairBeginYPos = m_shotgunMoveTopCrossHair->GetYPosition().d_offset;
		m_shotgunMoveLeftCrossHairBeginXPos = m_shotgunMoveLeftCrossHair->GetXPosition().d_offset;
		m_shotgunMoveBottomCrossHairBeginYPos = m_shotgunMoveBottomCrossHair->GetYPosition().d_offset;
		m_shotgunMoveRightCrossHairBeginXPos = m_shotgunMoveRightCrossHair->GetXPosition().d_offset;

		m_warningLayout = getWindow<GUILayout>("Main-Warning");
		m_warningLayout->SetVisible(false);
		m_shootWarning = getWindow<GUIStaticImage>("Main-Shoot-Warning");
		m_shootWarning->SetVisible(false);
		m_subscriptionGuard.add(ShootWarningEvent::subscribe([this](float rotateAngle, bool isScreenLeft) -> bool {
			m_warningLayout->SetVisible(true);
			m_showShootWarning = true;
			m_shootWarningAlpha = 1.0f;
			if (isScreenLeft)
			{
				m_shootWarningRotateAngle = 360.0f - rotateAngle * Math::RAD2DEG;
			}
			else
			{
				m_shootWarningRotateAngle = rotateAngle * Math::RAD2DEG;
			}
			return true;
		}));

		m_endingAnimationLayout = getWindow<GUILayout>("Main-Ending-Animation");
		m_endingAnimationLayout->SetVisible(false);
		m_endingStatus = getWindow<GUIStaticImage>("Main-Ending-Status");
		m_endingStatus->SetVisible(false);
		m_subscriptionGuard.add(PlayEndingAnimationEvent::subscribe([this](String resultStr, bool isNextServer, int result) -> bool {
			m_gameResult = resultStr;
			m_isNextServer = isNextServer;
			if (result == 0)
			{
				playSoundByType(ST_GameCompleteLose);
				m_endingStatus->SetImage("set:ending_status.json image:ending_fail");
			}
			else if(result == 1)
			{
				playSoundByType(ST_GameCompleteWin);
				m_endingStatus->SetImage("set:ending_status.json image:ending_victory");
			}
			else if (result == 2)
			{
				playSoundByType(ST_GameCompleteDraw);
				m_endingStatus->SetImage("set:ending_status.json image:ending_draw");
			}
			m_endingAnimationLayout->SetVisible(true);
			m_playEndingAnimation = true;
			m_isZoomInEndingImg = true;
			m_beginRotating = false;
			m_beginPlayEndingAnimation = false;
			m_endingAnimationScale = 0.0f;
			m_endingAnimationRotatingTime = 0.0f;
			m_endingImgDurationTime = 0.0f;
			Blockman::Instance()->m_pPlayer->rotationPitch = 0.0f;
			Blockman::Instance()->setPersonView(1);
			Blockman::Instance()->m_playerControl->setDisableMovement(true);
			Blockman::Instance()->m_gameSettings->setMoveState((int)(MoveState::RunState));
			refreshMoveState();
			return true;
		}));

		m_gunOperationWindow = getWindow("Main-Gun-Operate");
		m_gunOperationWindow->SetVisible(true);
		m_gunSniperSight = getWindow("Main-Gun-Operate-SniperSight");
		m_subscriptionGuard.add(AimingStateChangeEvent::subscribe([this](bool aiming,int crossHairType) -> bool {
			m_crossHairLayout->SetVisible(aiming);
			checkHideGunUI();
			checkHideReleaseSkillBtnUI();
			if (aiming)
				switchCrossHair(crossHairType);
			return true;
		}));

		m_arrowControlWindow = getWindow("Main-Control");
		m_buttons[ButtonId::FORWARD] = getWindow("MainControl-forward");
		m_buttons[ButtonId::BACK] = getWindow("MainControl-back");
		m_buttons[ButtonId::LEFT] = getWindow("MainControl-left");
		m_buttons[ButtonId::RIGHT] = getWindow("MainControl-right");
		m_buttons[ButtonId::JUMP] = getWindow("Main-Jump");
		m_buttons[ButtonId::DROP] = getWindow("Main-Drop");
		m_buttons[ButtonId::UP] = getWindow("Main-Up");
		m_buttons[ButtonId::DOWN] = getWindow("Main-Down");
		m_buttons[ButtonId::TOP_LEFT] = getWindow("MainControl-top-left");
		m_buttons[ButtonId::TOP_RIGHT] = getWindow("MainControl-top-right");
		m_buttons[ButtonId::TOP_LEFT_BG] = getWindow("MainControl-top-left-bg");
		m_buttons[ButtonId::TOP_RIGHT_BG] = getWindow("MainControl-top-right-bg");
		m_buttons[ButtonId::SNEAK] = getWindow("MainControl-sneak");

		m_buttonPressed[ButtonId::SNEAK] = false;

		m_breakBlockProgress = getWindow<GUIStaticImage>("Main-Break-Block-Progress-Nor");
		m_hitEntityProgress = getWindow<GUIProgressBar>("Main-Hit-Entity-Progress");
		m_guiSlotTable = getWindow<GuiSlotTable>("Main-VisibleBar-SlotTable");
		m_flyingControls = getWindow("Main-FlyingControls");
		m_btnPutFurnitureSure = getWindow<GUIButton>("Main-Put-Furniture-Sure");
		m_btnPutFurnitureCancel = getWindow<GUIButton>("Main-Put-Furniture-Cancel");
		m_btnManorPraise = getWindow<GUIButton>("Main-Manor-Praise");
		m_btnOpenFlying = getWindow<GUIButton>("Main-OpenFlying");
		setText(m_btnOpenFlying, "gui_flying");

		for (ButtonId id : ALL_BUTTON_ID)
		{
			subscribeTouchEvent(m_buttons[id], id);
		}

		subscribeTouchEvent(getWindow("MainControl-jump"), ButtonId::JUMP);
		m_buttons[ButtonId::SNEAK]->subscribeEvent("WindowDoubleClick", std::bind(&gui_mainControl::onSneakDoubleClick, this, std::placeholders::_1));
		getWindow("Main-Sneak")->subscribeEvent("WindowDoubleClick", std::bind(&gui_mainControl::onSneakDoubleClick, this, std::placeholders::_1));
		m_buttons[ButtonId::JUMP]->subscribeEvent("WindowDoubleClick", std::bind(&gui_mainControl::onJumpDoubleClick, this, std::placeholders::_1));
		m_buttons[ButtonId::DROP]->subscribeEvent("WindowDoubleClick", std::bind(&gui_mainControl::onDropDoubleClick, this, std::placeholders::_1));
		getWindow("Main-ToggleInventoryButton")->subscribeEvent(EventWindowTouchUp, SubscriberSlot(&gui_mainControl::showPlayerInventory, this));
		getWindow("Main-GunPreviewCheckBox")->subscribeEvent(EventCheckStateChanged, SubscriberSlot(&gui_mainControl::onCheckChange, this));
		getWindow("Main-VisibleBar-SlotTable")->subscribeEvent("ItemStackTouchUp", SubscriberSlot(&gui_mainControl::onItemStackTouchUp, this));
		getWindow("Main-VisibleBar-SlotTable")->subscribeEvent("ItemStackLongTouchStart", SubscriberSlot(&gui_mainControl::onItemStackLongTouchStart, this));
		getWindow("Main-VisibleBar-SlotTable")->subscribeEvent("ItemStackLongTouchEnd", SubscriberSlot(&gui_mainControl::onItemStackLongTouchEnd, this));
		getWindow("Main-VisibleBar-SlotTable")->setEnableLongTouch(true);
		m_actorWindow = getWindow<GuiActorWindow>("Main-Gun-Actor");

		m_chatMessageListBox = getWindow<GUIListBox>("Main-Chat-Message");
		m_subscriptionGuard.add(ChatMessageReceiveEvent::subscribe(std::bind(&gui_mainControl::addChatMessage, this, std::placeholders::_1)));
		m_subscriptionGuard.add(TopSystemNotificationEvent::subscribe(std::bind(&gui_mainControl::showTopSystemNotification, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(BottomSystemNotificationEvent::subscribe(std::bind(&gui_mainControl::showBottomSystemNotification, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(CenterSystemNotificationEvent::subscribe(std::bind(&gui_mainControl::showCenterSystemNotification, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(BreakBlockProgressEvent::subscribe(std::bind(&gui_mainControl::onBreakBlockProgress, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(BreakBlockUiManageEvent::subscribe(std::bind(&gui_mainControl::breakBlockUiManage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		m_subscriptionGuard.add(PlayerChangeItemEvent::subscribe(std::bind(&gui_mainControl::onChangeCurrentItem, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(VehicleLockEvent::subscribe(std::bind(&gui_mainControl::onLockStateChanged, this, std::placeholders::_1)));
		m_subscriptionGuard.add(VehicleOnOffEvent::subscribe(std::bind(&gui_mainControl::onVehicleStateChanged, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(ChangeSkillTypeEvent::subscribe(std::bind(&gui_mainControl::setSkillType, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		m_subscriptionGuard.add(SetPutFurnitureIdEvent::subscribe(std::bind(&gui_mainControl::setPutFurnitureId, this, std::placeholders::_1)));
		m_subscriptionGuard.add(ShowPlayerOperationEvent::subscribe(std::bind(&gui_mainControl::showPlayerOperation, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(SetManorBtnVisibleEvent::subscribe(std::bind(&gui_mainControl::setManorBtnVisible, this, std::placeholders::_1)));
		m_subscriptionGuard.add(RefreshManorUiEvent::subscribe(std::bind(&gui_mainControl::onPutFurnitureCancel, this)));
		m_subscriptionGuard.add(GameStatusEvent::subscribe(std::bind(&gui_mainControl::setGameStatus, this, std::placeholders::_1)));
		m_subscriptionGuard.add(ServerSyncGameTypeEvent::subscribe(std::bind(&gui_mainControl::onGameTypeUpdate, this)));
		m_subscriptionGuard.add(BirdAddScoreEvent::subscribe(std::bind(&gui_mainControl::onBirdAddScore, this, std::placeholders::_1, std::placeholders::_2)));

		m_chatMessageListBox->SetAllowScroll(true);

		m_watchBattleWindow = getWindow("Main-Watch-Battle-Tip");
		checkWatchRespawnUI();

		getWindow("Main-Gun-Preview")->SetVisible(false);

		getWindow("Main-Gun-Harm-Title")->SetText(getString(LanguageKey::GUI_STR_GUN_HARM));
		getWindow("Main-Gun-Bullet-Title")->SetText(getString(LanguageKey::GUI_STR_GUN_BULLET));
		getWindow("Main-Gun-Range-Title")->SetText(getString(LanguageKey::GUI_STR_GUN_RANGE));
		getWindow("Main-Gun-FiringRate-Title")->SetText(getString(LanguageKey::GUI_STR_GUN_FIRING_RATE));
		getWindow("Main-Gun-Deviation-Title")->SetText(getString(LanguageKey::GUI_STR_GUN_DEVIATION));

		m_topSystemNotification = getWindow<GUIStaticText>("Main-Top-System-Notification");
		m_bottomSystemNotification = getWindow<GUIStaticText>("Main-Bottom-System-Notification");
		m_centerSystemNotification = getWindow<GUIStaticText>("Main-Center-System-Notification");
		m_playerOperationLayout = getWindow<GUILayout>("Main-PlayerOperation");
		m_playerOperationLayout->subscribeEvent(EventWindowTouchUp, std::bind(&gui_mainControl::hidePlayerOperationView, this, std::placeholders::_1));
		if (!m_guiPlayerOperation)
		{
			m_guiPlayerOperation = LordNew gui_playerOperation;
			m_guiPlayerOperation->attachTo(this, m_playerOperationLayout);
		}

		if (!m_guiTeamResources)
		{
			m_guiTeamResources = LordNew gui_teamResources;
			m_guiTeamResources->attachTo(this, getWindow("Main"));
		}

		m_leftShootBtn = getWindow<GUIButton>("Main-Gun-Operate-LeftShootBtn");
		m_leftShootBtn->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onLeftShootBtnTouchUp, this, std::placeholders::_1));
		m_leftShootBtn->subscribeEvent(EventWindowTouchDown, std::bind(&gui_mainControl::onLeftShootBtnTouchDown, this, std::placeholders::_1));
		m_leftShootBtn->subscribeEvent(EventMotionRelease, std::bind(&gui_mainControl::onLeftShootBtnTouchUp, this, std::placeholders::_1));
		m_rightShootBtn = getWindow<GUIButton>("Main-Gun-Operate-RightShootBtn");
		m_rightShootBtn->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onRightShootBtnTouchUp, this, std::placeholders::_1));
		m_rightShootBtn->subscribeEvent(EventWindowTouchDown, std::bind(&gui_mainControl::onRightShootBtnTouchDown, this, std::placeholders::_1));
		m_rightShootBtn->subscribeEvent(EventWindowTouchMove, std::bind(&gui_mainControl::onRightShootBtnMoved, this, std::placeholders::_1));
		m_rightShootBtn->subscribeEvent(EventMotionRelease, std::bind(&gui_mainControl::onRightShootBtnTouchUp, this, std::placeholders::_1));
		m_originButtonPos = m_rightShootBtn->GetPosition();
		m_reloadBulletBtn = getWindow<GUIButton>("Main-Gun-Operate-ReloadBtn");
		m_reloadBulletBtn->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickReloadBulletBtn, this, std::placeholders::_1));
		m_aimBtn = getWindow<GUIButton>("Main-Gun-Operate-AimBtn");
		m_aimBtn->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickAimBtn, this, std::placeholders::_1));
		m_cancelAimBtn = getWindow<GUIButton>("Main-Gun-Operate-CancelAimBtn");
		m_cancelAimBtn->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickCancelAimBtn, this, std::placeholders::_1));
		m_throwGunBtn = getWindow<GUIButton>("Main-Gun-Operate-ThrowBtn");
		m_throwGunBtn->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickThrowGunBtn, this, std::placeholders::_1));
		m_throwGunBtn->SetVisible(UIDisplaySetting::getUIDisplay()->isShowThrowGun);
		m_prevFovSetting = Blockman::Instance()->m_gameSettings->getFovSetting();
		m_prevMouseSensitive = Blockman::Instance()->m_gameSettings->getMouseSensitivity();
		m_prevCameraView = Blockman::Instance()->m_gameSettings->getPersonView();
		m_carControlWindow = getWindow("Main-Car-Control");
		m_carControlWindow->SetVisible(false);
		getWindow<GUIButton>("Main-Car-Control-Lock")->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickLockBtn, this, std::placeholders::_1));
		getWindow<GUIButton>("Main-Car-Control-Unlock")->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickUnlockBtn, this, std::placeholders::_1));
		m_showManorBtn = getWindow<GUIButton>("MainControl-Show-Manor");
		m_teleportBtn = getWindow<GUIButton>("MainControl-Teleport");
		m_btnPlaceBuilding = getWindow<GUIButton>("Main-Place-Building");
		m_teleportBtn->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickTeleportBtn, this, std::placeholders::_1));
		m_showManorBtn->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickShowManorBtn, this, std::placeholders::_1));
		m_btnPlaceBuilding->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onPlaceBuildingClick, this, std::placeholders::_1));
		m_teleportBtn->SetVisible(false);
		m_showManorBtn->SetVisible(false);

		m_parachuteBtn = getWindow<GUIButton>("Main-Parachute");
		if (m_parachuteBtn)
		{
			m_parachuteBtn->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickParachuteBtn, this, std::placeholders::_1));
			m_parachuteBtn->SetVisible(false);
		}

		m_flyBtn = getWindow<GUIButton>("Main-Fly");
		if (m_flyBtn)
		{
			m_flyBtn->subscribeEvent(EventWindowTouchDown, std::bind(&gui_mainControl::onClickFlyBtnDown, this, std::placeholders::_1));
			m_flyBtn->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickFlyBtnUp, this, std::placeholders::_1));
			m_flyBtn->subscribeEvent(EventMotionRelease, std::bind(&gui_mainControl::onClickFlyBtnUp, this, std::placeholders::_1));
		}

		m_buildWarBlockBtn = getWindow<GUIButton>("Main-BuildWar-Block");
		if (m_buildWarBlockBtn)
		{
			m_buildWarBlockBtn->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickBuildWarBlockBtn, this, std::placeholders::_1));
			m_buildWarBlockBtn->SetVisible(GameClient::CGame::Instance()->GetGameType() == ClientGameType::Build_War);
		}

		if (isHideSneakBtn())
		{	
			m_buttons[ButtonId::SNEAK]->SetVisible(false);
			getWindow("Main-Sneak")->SetVisible(false);
		}

		if (isHideStateBtn())
		{
			getWindow("Main-MoveState")->SetVisible(false);
		}

		m_hideAndSeekControlWindow = getWindow("Main-HideAndSeek-Operate");
		if (m_hideAndSeekControlWindow)
		{
			m_hideAndSeekControlWindow->SetVisible(false);
		}

		m_changeActorBtn = getWindow<GUIButton>("Main-Change-Actor");
		if (m_changeActorBtn)
		{
			m_changeActorBtn->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickChangeActorBtn, this, std::placeholders::_1));
			m_changeActorBtn->SetVisible(true);
		}

		m_hideandseekChangeYaw = getWindow<GUILayout>("Main-HideAndSeek-ChangeYaw");
		m_hideandseekChangeYawLeftBtn = getWindow<GUIButton>("Main-HideAndSeek-ChangeYawLeftBtn");
		m_hideandseekChangeYawRightBtn = getWindow<GUIButton>("Main-HideAndSeek-ChangeYawRightBtn");
		if (m_hideandseekChangeYawLeftBtn)
		{
			m_hideandseekChangeYawLeftBtn->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickChangeActorYawBtn, this, std::placeholders::_1, true));
			m_hideandseekChangeYawLeftBtn->subscribeEvent("WindowLongTouchStart", std::bind(&gui_mainControl::onClickChangeActorYawLongTouchStartBtn, this, std::placeholders::_1, true));
			m_hideandseekChangeYawLeftBtn->subscribeEvent("WindowLongTouchEnd", std::bind(&gui_mainControl::onClickChangeActorYawLongTouchEndBtn, this, std::placeholders::_1, true));
			m_hideandseekChangeYawLeftBtn->subscribeEvent("MotionRelease", std::bind(&gui_mainControl::onClickChangeActorYawLongTouchEndBtn, this, std::placeholders::_1, true));
			m_hideandseekChangeYawLeftBtn->SetVisible(true);
		}
		if (m_hideandseekChangeYawRightBtn)
		{
			m_hideandseekChangeYawRightBtn->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickChangeActorYawBtn, this, std::placeholders::_1, false));
			m_hideandseekChangeYawRightBtn->subscribeEvent("WindowLongTouchStart", std::bind(&gui_mainControl::onClickChangeActorYawLongTouchStartBtn, this, std::placeholders::_1, false));
			m_hideandseekChangeYawRightBtn->subscribeEvent("WindowLongTouchEnd", std::bind(&gui_mainControl::onClickChangeActorYawLongTouchEndBtn, this, std::placeholders::_1, false));
			m_hideandseekChangeYawRightBtn->subscribeEvent("MotionRelease", std::bind(&gui_mainControl::onClickChangeActorYawLongTouchEndBtn, this, std::placeholders::_1, false));
			m_hideandseekChangeYawRightBtn->SetVisible(true);
		}


		m_cameraChangeStatusBtn = getWindow<GUIButton>("Main-Camera-Change-Status");
														
		if (m_cameraChangeStatusBtn)
		{
			m_cameraChangeStatusBtn->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickChangeCameraStatusBtn, this, std::placeholders::_1));
			m_cameraChangeStatusBtn->SetVisible(true);
		}

		m_personalShopBtn = getWindow<GUIButton>("Main-Personal-Shop");
		if (m_personalShopBtn)
		{
			m_personalShopBtn->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickPersonalShopBtn, this, std::placeholders::_1));
			m_personalShopBtn->SetVisible(false);
		}
		m_buyButtleBtn = getWindow<GUIButton>("Main-Buy-Bullet");
		{
			m_buyButtleBtn->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickBuyBulletBtn, this, std::placeholders::_1));
			m_buyButtleBtn->SetVisible(false);
		}

		m_subscriptionGuard.add(GameStatusEvent::subscribe(std::bind(&gui_mainControl::refreshHideAndSeekBtn, this, std::placeholders::_1)));

		m_cameraLockImage = getWindow<GUIStaticImage>("Main-Camera-Lock");
		m_cameraUnLockImage = getWindow<GUIStaticImage>("Main-Camera-Unlock");
		if (m_cameraUnLockImage && m_cameraLockImage)
		{
			m_cameraUnLockImage->SetVisible(true);
			m_cameraLockImage->SetVisible(false);
		}

		m_btnBirdFuse = getWindow<GUIButton>("Main-Bird-Fuse");
		m_btnBirdPack = getWindow<GUIButton>("Main-Bird-Pack");
		m_btnBirdAtlas = getWindow<GUIButton>("Main-Bird-Atlas");
		m_btnBirdPersonalShop = getWindow<GUIButton>("Main-Bird-Personal-Shop");
		m_birdActivity = getWindow<GUIButton>("Main-Bird-Activity");

		m_btnBirdPack->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickBirdPackBtn, this, std::placeholders::_1));
		m_btnBirdFuse->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickBirdFuseBtn, this, std::placeholders::_1));
		m_btnBirdAtlas->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickBirdAtlasBtn, this, std::placeholders::_1));
		m_btnBirdPersonalShop->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickBirdPersonalShopBtn, this, std::placeholders::_1));
		m_birdActivity->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickBirdActivityBtn, this, std::placeholders::_1));
		
		m_poleControlWindow = getWindow("Main-PoleControl");
		m_poleBgWindow = getWindow("Main-PoleControl-bg");
		m_poleCenterWindow = getWindow("Main-PoleControl-Center");
		m_poleMoveWindow = getWindow("Main-PoleControl-Move");
		m_originPolePos = m_poleCenterWindow->GetPosition();
		m_poleMoveWindow->subscribeEvent(EventWindowTouchMove, std::bind(&gui_mainControl::onPoleTouchMove, this, std::placeholders::_1));
		m_poleMoveWindow->subscribeEvent(EventMotionRelease, std::bind(&gui_mainControl::onPoleTouchUp, this, std::placeholders::_1));
		m_poleMoveWindow->subscribeEvent(EventWindowTouchDown, std::bind(&gui_mainControl::onPoleTouchDown, this, std::placeholders::_1));
		m_poleMoveWindow->subscribeEvent(EventWindowTouchUp, std::bind(&gui_mainControl::onPoleTouchUp, this, std::placeholders::_1));
		m_poleBgWindow->SetAlpha(0.75);
		auto rect = m_poleCenterWindow->GetUnclippedOuterRect();
		m_originPoleAbsPos = UVector2(UDim(0.0f, rect.left + m_poleCenterWindow->GetPixelSize().x / 2),
			UDim(0.0f, rect.top + m_poleCenterWindow->GetPixelSize().y / 2));

		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::BirdSimulator)
		{
			getWindow("Main-MoveState")->SetVisible(false);
		}		
		m_sneakStateWindow = getWindow("Main-MoveState-Sneak");
		m_runStateWindow = getWindow("Main-MoveState-Run");
		m_sprintStateWindow = getWindow("Main-MoveState-Sprint");
		m_skillBtn = getWindow<GUIButton>("Main-Skill");
		m_sneakStateWindow->subscribeEvent(EventWindowClick, std::bind(&gui_mainControl::onClickSwitchMoveState, this, std::placeholders::_1));
		m_runStateWindow->subscribeEvent(EventWindowClick, std::bind(&gui_mainControl::onClickSwitchMoveState, this, std::placeholders::_1));
		m_sprintStateWindow->subscribeEvent(EventWindowClick, std::bind(&gui_mainControl::onClickSwitchMoveState, this, std::placeholders::_1));
		m_btnPutFurnitureSure->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickPutFurnitureSureBtn, this, std::placeholders::_1));
		m_btnPutFurnitureCancel->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickPutFurnitureCancelBtn, this, std::placeholders::_1, true));
		m_btnManorPraise->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickManorPraise, this, std::placeholders::_1));
		m_skillBtn->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickSkillBtn, this, std::placeholders::_1));
		m_subscriptionGuard.add(UpdateBossBloodStripEvent::subscribe(std::bind(&gui_mainControl::refreshBossLife, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(UpdateBasementLifeEvent::subscribe(std::bind(&gui_mainControl::refreshBasementLife, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(UpdateBuildProgressEvent::subscribe(std::bind(&gui_mainControl::refreshBuildProgress, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)));
		m_subscriptionGuard.add(ShowRanchExTask::subscribe(std::bind(&gui_mainControl::showRanchExTask, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(ShowRanchExCurrentItemInfo::subscribe(std::bind(&gui_mainControl::showRanchExCurrentItemTip, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)));
		m_subscriptionGuard.add(ShowRanchExItem::subscribe(std::bind(&gui_mainControl::showRanchExItem, this, std::placeholders::_1, std::placeholders::_2)));

		refreshMoveState();
		
		//CreepyAliens boss life
		m_bossLifeProgress = getWindow<GUIProgressBar>("Main-Boss-Life");
		m_bossLifeProgress->SetVisible(false);
		m_bossLifeIcon = getWindow<GUIStaticImage>("Main-Boss-Life-Icon");

		// exp
		m_playerExpProgress = getWindow<GUIProgressBar>("Main-Player-Exp");

		//CreepyAliens basement life
		m_basementLifeLayout = getWindow<GUILayout>("Main-Basement-Life");
		m_basementLifeLayout->SetVisible(false);
		m_basementLifeInfo = getWindow<GUIStaticText>("Main-Basement-Life-Info");
		m_basementLifeProgress = getWindow<GUIProgressBar>("Main-Basement-Life-Value");

		m_subscriptionGuard.add(SyncShowMaskTimeEvent::subscribe(std::bind(&gui_mainControl::syncShowMaskTime, this, std::placeholders::_1)));
		m_subscriptionGuard.add(CastSkillEvent::subscribe(std::bind(&gui_mainControl::onAutoCastSkill, this)));

		CheckGunUI();
		checkAimUI();
		LoadSkillReleaseBtn();
		LoadThrowPotUi();
		LoadRanchExUI();
		
		m_darkMask = getWindow<GUIStaticImage>("Main-Dark-Mask");
		m_hurtMask = getWindow<GUIStaticImage>("Main-Hurt-Mask");
		m_frozenMask = getWindow<GUIStaticImage>("Main-Frozen-Mask");

		m_btnFishing = getWindow<GUIButton>("Main-Fishing");
		m_btnFishing->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickFishingBtn, this, std::placeholders::_1));

		CheckFishingBtn();

		m_btnCannon = getWindow<GUIButton>("Main-Cannon");
		m_btnCannon->SetVisible(false);
		m_btnCannon->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickCannonBtn, this, std::placeholders::_1));
		m_btnOpenFlying->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickOpenFlying, this, std::placeholders::_1));
		m_subscriptionGuard.add(ShowCannonBtnEvent::subscribe(std::bind(&gui_mainControl::showCannonBtn, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(HideBirdActivityEvent::subscribe(std::bind(&gui_mainControl::hideBirdActivity, this)));
		
		
		
		m_birdTaskLayout = getWindow<GUILayout>("Main-Bird-Task");
		m_btnHideTask = getWindow<GUIButton>("Main-Bird-Task-Hide");
		m_btnShowTask = getWindow<GUIButton>("Main-Bird-BtnTask");
		m_birdTaskList = getWindow<GUIListBox>("Main-Bird-Task-List");
		m_btnHideTask->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onHideBirdTaskClick, this, std::placeholders::_1));
		m_btnShowTask->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onShowBirdTaskClick, this, std::placeholders::_1));
		m_subscriptionGuard.add(BirdTaskChangeEvent::subscribe(std::bind(&gui_mainControl::onBirdTaskChange, this)));
		m_subscriptionGuard.add(BirdAcceptTaskEvent::subscribe(std::bind(&gui_mainControl::onShowBirdTaskClick, this, EventArgs())));
		
		//m_btnUseItem = getWindow<GUIButton>("Main-Use-Item");
		//m_btnUseItem->SetText(getString(LanguageKey::GUI_USEITEM));
		//m_btnUseItem->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickUseItemBtn, this, std::placeholders::_1));
		//CheckUseItemBtn();
		onGameTypeUpdate();
		m_subscriptionGuard.add(UpdateDisarmamentStatusEvent::subscribe([this](bool isDisarmament) -> bool {
			m_leftShootBtn->SetEnabled(!isDisarmament);
			m_rightShootBtn->SetEnabled(!isDisarmament);
			return true;
		}));
	}

	void gui_mainControl::onShow()
	{
		checkAimUI();
	}

	bool gui_mainControl::onGameTypeUpdate()
	{
		m_btnBirdPack->SetVisible(GameClient::CGame::Instance()->GetGameType() == ClientGameType::BirdSimulator);
		m_btnBirdFuse->SetVisible(GameClient::CGame::Instance()->GetGameType() == ClientGameType::BirdSimulator);
		m_btnBirdPersonalShop->SetVisible(GameClient::CGame::Instance()->GetGameType() == ClientGameType::BirdSimulator);
		m_btnShowTask->SetVisible(GameClient::CGame::Instance()->GetGameType() == ClientGameType::BirdSimulator);
		m_btnBirdAtlas->SetVisible(GameClient::CGame::Instance()->GetGameType() == ClientGameType::BirdSimulator);
		m_birdActivity->SetVisible(GameClient::CGame::Instance()->GetGameType() == ClientGameType::BirdSimulator);
		return true;
	}

	bool gui_mainControl::onSneakDoubleClick(const EventArgs &)
	{
		auto pClientMovementInput = Blockman::Instance()->m_gameSettings->m_clientMovementInput;
		pClientMovementInput->sneakPressed = !pClientMovementInput->sneakPressed;
		if (pClientMovementInput->sneakPressed)
		{
			Blockman::Instance()->m_gameSettings->setMoveState((int)(MoveState::SneakState));
		}
		else
		{
			Blockman::Instance()->m_gameSettings->switchMoveState();
		}
		refreshMoveState();
		return true;
	}

	bool gui_mainControl::onClickSkillBtn(const EventArgs &)
	{
		if (m_skillType == SkillType::NONE)
		{
			return false;
		}
		switch (m_skillType)
		{
		case SkillType::DEFENSE:
			Blockman::Instance()->m_pPlayer->playDefendProgress(m_skillDuration);
			break;
		case SkillType::SPRINT:
			break;
		case SkillType::RELEASE_TOXIC:
			Blockman::Instance()->m_pPlayer->setSkillAnimation(ActionState::AS_SKILL_POISION);
			break;
		}
		m_skillResidueCDTime = (int) m_skillCDTime * 1000;
		ClientNetwork::Instance()->getSender()->sendSkillType(m_skillType, true);
		return true;
	}

	bool gui_mainControl::onClickPutFurnitureSureBtn(const EventArgs &)
	{
		ManorFurniture* furniture = Blockman::Instance()->m_pPlayer->m_manor->findFurnitureById(m_putFurnitureId);
		Blockman::Instance()->setPersonView(m_currentCameraView);
		if (!furniture)
		{			
			m_putFurnitureId = -1;
			return false;
		}
		if (furniture->operationType == OperationType::RECYCLE &&  Blockman::Instance()->m_gameSettings->m_isEditFurniture)
		{
			ClientNetwork::Instance()->getSender()->sendPutFurniture(m_putFurnitureId, PutFurnitureStatus::EDIT_SURE);
		}
		else
		{
			ClientNetwork::Instance()->getSender()->sendPutFurniture(m_putFurnitureId, PutFurnitureStatus::SURE);
		}
		m_putFurnitureId = -1;
		return true;
	}

	bool gui_mainControl::onClickPutFurnitureCancelBtn(const EventArgs&, bool isBtnClick)
	{
		ManorFurniture* furniture = Blockman::Instance()->m_pPlayer->m_manor->findFurnitureById(m_putFurnitureId);
		Blockman::Instance()->setPersonView(m_currentCameraView);
		if (!furniture || !isBtnClick)
		{
			m_putFurnitureId = -1;
			return false;
		}
		if (furniture->operationType == OperationType::RECYCLE &&  Blockman::Instance()->m_gameSettings->m_isEditFurniture)
		{
			ClientNetwork::Instance()->getSender()->sendPutFurniture(m_putFurnitureId, PutFurnitureStatus::EDIT_CANCEL);
		}
		else
		{
			ClientNetwork::Instance()->getSender()->sendPutFurniture(m_putFurnitureId, PutFurnitureStatus::CANCEL);
		}
		m_putFurnitureId = -1;
		return true;
	}

	bool gui_mainControl::onPutFurnitureCancel()
	{
		onClickPutFurnitureCancelBtn(EventArgs(), false);
		return true;
	}

	bool gui_mainControl::onClickManorPraise(const EventArgs &)
	{
		ClientNetwork::Instance()->getSender()->sendGiveRose(manorCheckPlayerLocation());
		return true;
	}

	bool gui_mainControl::setSkillType(SkillType skillType, float duration, float cdTime)
	{
		this->m_skillType = skillType;
		this->m_skillDuration = duration;
		this->m_skillCDTime = cdTime;
		return true;
	}

	void gui_mainControl::subscribeTouchEvent(GUIWindow* window, ButtonId id)
	{
		window->subscribeEvent("WindowTouchDown", std::bind(&gui_mainControl::onButtonPressed, this, std::placeholders::_1, id));
		window->subscribeEvent("WindowTouchUp", std::bind(&gui_mainControl::onButtonReleased, this, std::placeholders::_1, id));
		window->subscribeEvent("MotionRelease", std::bind(&gui_mainControl::onButtonReleased, this, std::placeholders::_1, id));
	}

	bool gui_mainControl::onJumpDoubleClick(const EventArgs &)
	{
		// already add a new "Main-Fly" button to replace this function by single click
		return true;
// 		if (Blockman::Instance()->m_pPlayer->capabilities.canBeFlying())
// 		{
// 			Blockman::Instance()->m_pPlayer->capabilities.isFlying = true;
// 			ClientNetwork::Instance()->getSender()->sendSetFlying(true);
// 			Blockman::Instance()->m_pPlayer->movementInput->down = false;
// 			Blockman::Instance()->m_pPlayer->movementInput->jump = false;
// 			m_buttonPressed[ButtonId::DROP] = false;
// 			m_buttonPressed[ButtonId::UP] = false;
// 			m_buttonPressed[ButtonId::DOWN] = false;
// 		}
// 		return true;
	}

	bool gui_mainControl::onDropDoubleClick(const EventArgs &)
	{
		auto pClientMovementInput = Blockman::Instance()->m_gameSettings->m_clientMovementInput;
		Blockman::Instance()->m_pPlayer->capabilities.isFlying = false;
		ClientNetwork::Instance()->getSender()->sendSetFlying(false);
		pClientMovementInput->down = false;
		pClientMovementInput->jump = false;
		m_buttonPressed[ButtonId::JUMP] = false;
		KeyBinding::setKeyBindState(BUTTON_TO_KEY_BINDINGS.at(ButtonId::JUMP), false);
		return true;
	}

	bool gui_mainControl::onButtonPressed(const EventArgs &, ButtonId buttonId)
	{
		if (buttonId == ButtonId::SNEAK)
			return true;

		m_buttonPressed[buttonId] = true;
		//LordLogInfo("onButtonPressed %d", (int)(buttonId));
		KeyBinding::setKeyBindState(BUTTON_TO_KEY_BINDINGS.at(buttonId), true);
		if (buttonId == ButtonId::FORWARD || buttonId == ButtonId::TOP_LEFT || buttonId == ButtonId::TOP_RIGHT ||
			buttonId == ButtonId::TOP_LEFT_BG || buttonId == ButtonId::TOP_RIGHT_BG)
		{
			m_isShowTopLeftAndTopRight = false;
			m_buttons[ButtonId::TOP_LEFT]->SetVisible(true);
			m_buttons[ButtonId::TOP_RIGHT]->SetVisible(true);
			m_buttons[ButtonId::TOP_LEFT_BG]->SetVisible(true);
			m_buttons[ButtonId::TOP_RIGHT_BG]->SetVisible(true);
		}
		
		return true;
	}

	bool gui_mainControl::onButtonReleased(const EventArgs &, ButtonId buttonId)
	{
		if (buttonId == ButtonId::SNEAK)
			return true;
		m_buttonPressed[buttonId] = false;
		//LordLogInfo("onButtonReleased %d", (int)(buttonId));
		KeyBinding::setKeyBindState(BUTTON_TO_KEY_BINDINGS.at(buttonId), false);
		if (buttonId == ButtonId::FORWARD)
		{
			m_isShowTopLeftAndTopRight = true;
		}
		else if (buttonId != ButtonId::JUMP && buttonId != ButtonId::DROP && buttonId != ButtonId::UP && buttonId != ButtonId::DOWN)
		{
			m_buttons[ButtonId::TOP_LEFT]->SetVisible(false);
			m_buttons[ButtonId::TOP_RIGHT]->SetVisible(false);
			m_buttons[ButtonId::TOP_LEFT_BG]->SetVisible(false);
			m_buttons[ButtonId::TOP_RIGHT_BG]->SetVisible(false);
		}
		return true;
	}

	bool gui_mainControl::onCheckChange(const EventArgs &)
	{
		refreshGunPreview();
		getWindow("Main-GunDetailsIcon")->SetVisible(!getWindow<GUICheckBox>("Main-GunPreviewCheckBox")->GetChecked());
		return true;
	}

	bool gui_mainControl::onItemStackTouchUp(const EventArgs& events)
	{
		auto& touchEvent = dynamic_cast<const GuiSlotTable::ItemStackTouchEventArgs&>(events);
		LordAssert(touchEvent.row == 0);
		onChangeCurrentItem(touchEvent.column, true);
		return true;
	}

	bool gui_mainControl::onChangeCurrentItem(int itemIndex, bool isShowItemName)
	{
		if (itemIndex >= 0 && itemIndex < InventoryPlayer::HOTBAR_COUNT) {
			Blockman::Instance()->m_pPlayer->inventory->currentItemIndex = itemIndex;
			ItemStackPtr stack = Blockman::Instance()->m_pPlayer->inventory->getCurrentItem();
			CheckGunUI();
			CheckFishingBtn();
			//CheckUseItemBtn();
			checkAimUI();
			refreshItemSkillShowUi();
			if (stack && stack->stackSize > 0)
			{
				if (isShowItemName)
				{
					getParent()->showItemNameUi(stack);
				}
				if (getWindow<GUICheckBox>("Main-GunPreviewCheckBox")->GetChecked())
				{
					refreshGunPreview();
				}
				if (UIDisplaySetting::getUIDisplay()->isShowGunDetail)
				{
					getWindow("Main-GunDetailsIcon")->SetVisible(!getWindow<GUICheckBox>("Main-GunPreviewCheckBox")->GetChecked());
				}
			}
			ClientNetwork::Instance()->getSender()->sendChangeCurrentItemInfo(itemIndex);
			return true;
		}
		return false;
	}

	bool gui_mainControl::onItemStackLongTouchStart(const EventArgs & events)
	{
		auto& touchEvent = dynamic_cast<const GuiSlotTable::ItemStackTouchEventArgs&>(events);
		m_dropItemIndex = touchEvent.column;
		Blockman::Instance()->m_pPlayer->inventory->currentItemIndex = touchEvent.column;
		ItemStackPtr stack = Blockman::Instance()->m_pPlayer->inventory->hotbar[touchEvent.column];
		CheckGunUI();
		if (stack && stack->stackSize > 0)
		{
			m_isLongTouch = true;
			m_touchTime = 300;
			getParent()->showItemNameUi(stack);
		}
		return true;
	}

	bool gui_mainControl::onItemStackLongTouchEnd(const EventArgs & events)
	{
		m_isLongTouch = false;
		m_touchTime = 0;
		updateDropItemProgress();
		m_dropItemIndex = -1;
		getWindow("Main-DropItemProgress")->SetVisible(false);
		return true;
	}

	void gui_mainControl::onUpdate(ui32 nTimeElapse)
	{

		if (!Blockman::Instance()->m_pPlayer)
		{
			return;
		}

		if (!m_isCanChangeCameraStatus)
		{
			m_ChangeCameraStatusCd = m_ChangeCameraStatusCd + nTimeElapse;
			if (m_ChangeCameraStatusCd > 1000)
				m_isCanChangeCameraStatus = true;
		}

		ClientGameType gameType = GameClient::CGame::Instance()->GetGameType();
		auto pDisplay = UIDisplaySetting::getUIDisplay();
		getWindow("Main-ToggleInventoryButton")->SetVisible(pDisplay->isShowBag);
		getWindow("Main-GunPreviewCheckBox")->SetVisible(pDisplay->isShowGunDetail);
		getWindow("Main-GunDetailsIcon")->SetVisible(pDisplay->isShowGunDetail);

		for (ButtonId id : ALL_BUTTON_ID)
		{
			m_buttons[id]->SetAlpha(m_buttonPressed[id] ? HOLD_ALPHA : RELEASE_ALPHA);
			m_buttons[id]->SetActionMatrixScale(m_buttonPressed[id] ? HOLD_SCALE : RELEASE_SCALE);
		}

		bool isSneak = Blockman::Instance()->m_gameSettings->m_clientMovementInput->sneakPressed;
		auto sneak = dynamic_cast<GUIStaticImage*>(m_buttons[ButtonId::SNEAK]);
		auto mainSneak = getWindow<GUIStaticImage>("Main-Sneak");
		if (sneak->IsVisible() || mainSneak->IsVisible())
		{
			sneak->SetAlpha(isSneak ? 0.9f : RELEASE_ALPHA);
			mainSneak->SetAlpha(isSneak ? 0.9f : RELEASE_ALPHA);
		}

		if (Blockman::Instance()->m_pPlayer->capabilities.isFlying)
		{
			m_buttons[ButtonId::JUMP]->SetVisible(false);
			m_flyingControls->SetVisible(true);
		}
		else
		{
			m_buttons[ButtonId::JUMP]->SetVisible(true);
			m_flyingControls->SetVisible(false);
		}
		useWatchModeUI(Blockman::Instance()->m_pPlayer->capabilities.isWatchMode);

		EntityPlayerSP* player = Blockman::Instance()->m_pPlayer;
		LordAssert(player);
		InventoryPlayer* inventory = player->inventory;
		LordAssert(inventory);
		inventory->updateInventoryCd((float)nTimeElapse / 1000);

		for (int i = 0; i < MAX_ITEM_COUNT; ++i)
		{
			m_guiSlotTable->setItemStack(0, i, inventory->hotbar[i]);
		}
		m_guiSlotTable->selectItemSlot(inventory->currentItemIndex);
		m_touchTime = m_isLongTouch ? m_touchTime + nTimeElapse : 0;
		m_removeMessageKeepTime = m_removeMessageKeepTime + nTimeElapse;
		if (m_removeMessageKeepTime >= 4000)
		{
			removeChatMessage();
			m_removeMessageKeepTime = 0;
		}
		if (m_isShowTopLeftAndTopRight)
		{
			m_keepShowTopLeftAndTopRightTime += nTimeElapse;
			if (m_keepShowTopLeftAndTopRightTime > 150)
			{
				m_buttons[ButtonId::TOP_LEFT]->SetVisible(false);
				m_buttons[ButtonId::TOP_RIGHT]->SetVisible(false);
				m_buttons[ButtonId::TOP_LEFT_BG]->SetVisible(false);
				m_buttons[ButtonId::TOP_RIGHT_BG]->SetVisible(false);
			}
		}
		else
		{
			m_keepShowTopLeftAndTopRightTime = 0;
		}

		m_keepShowTopSystemNotificationTime = m_keepShowTopSystemNotificationTime > 0 ? m_keepShowTopSystemNotificationTime - nTimeElapse : 0;
		m_keepShowBottomSystemNotificationTime = m_keepShowBottomSystemNotificationTime > 0 ? m_keepShowBottomSystemNotificationTime - nTimeElapse : 0;
		m_keepShowCenterSystemNotificationTime = m_keepShowCenterSystemNotificationTime > 0 ? m_keepShowCenterSystemNotificationTime - nTimeElapse : 0;
		
		m_topSystemNotification->SetVisible(m_keepShowTopSystemNotificationTime > 0);
		m_bottomSystemNotification->SetVisible(m_keepShowBottomSystemNotificationTime > 0);
		m_centerSystemNotification->SetVisible(m_keepShowCenterSystemNotificationTime > 0);

		URect  mainControlArea = getWindow("Main-Control")->GetArea();
		mainControlArea.setWidth(UDim(Blockman::Instance()->m_gameSettings->getMainGuiWidth(), 0.0f));
		mainControlArea.setHeight(UDim(Blockman::Instance()->m_gameSettings->getMainGuiHeight(), 0.0f));
		getWindow("Main-Control")->SetArea(mainControlArea);

		URect  mainJumpControlsArea = getWindow("Main-Jump-Controls")->GetArea();
		mainJumpControlsArea.setWidth(UDim(Blockman::Instance()->m_gameSettings->getMainGuiWidth() * 0.382f, 0.0f));
		mainJumpControlsArea.setHeight(UDim(Blockman::Instance()->m_gameSettings->getMainGuiHeight(), 0.0f));
		getWindow("Main-Jump-Controls")->SetArea(mainJumpControlsArea);

		URect  mainFlyingControlsArea = getWindow("Main-FlyingControls")->GetArea();
		mainFlyingControlsArea.setWidth(UDim(Blockman::Instance()->m_gameSettings->getMainGuiWidth() * 0.382f, 0.0f));
		mainFlyingControlsArea.setHeight(UDim(Blockman::Instance()->m_gameSettings->getMainGuiHeight(), 0.0f));
		getWindow("Main-FlyingControls")->SetArea(mainFlyingControlsArea);

		switchMoveControl(Blockman::Instance()->m_gameSettings->usePole > 0);

		if (m_isLongTouch)
		{
			updateDropItemProgress();
		}

		checkHideGunUI();

		ItemStackPtr stack = inventory->getCurrentItem();
		if (!(stack && stack->getGunSetting() && (stack->getGunSetting()->gunType == LASER_EFFECT_GUN || stack->getGunSetting()->gunType == ELECTRICITY_EFFECT_GUN)))
		{
			updateShootProgress();
		}

		if (m_teleportBtnCountdown > 0)
		{
			m_teleportBtnCountdown = m_teleportBtnCountdown - nTimeElapse; 
		}
		m_teleportBtn->SetEnabled(m_teleportBtnCountdown <= 0);
		m_teleportBtn->SetText(m_teleportBtnCountdown > 0 ? StringUtil::ToString(m_teleportBtnCountdown / 1000 + 1).c_str() : "");
		m_teleportBtn->SetTextScale(1.6f);
		m_teleportBtn->SetVisible(GameClient::CGame::Instance()->GetGameType() == ClientGameType::Sand_Digger);
		m_skillBtn->SetVisible(GameClient::CGame::Instance()->GetGameType() == ClientGameType::Zombie_Infecting && m_skillType != SkillType::NONE);
		m_showManorBtn->SetVisible(GameClient::CGame::Instance()->GetGameType() == ClientGameType::Tiny_Town && m_manorBtnVisible);
		m_btnPutFurnitureSure->SetVisible(GameClient::CGame::Instance()->GetGameType() == ClientGameType::Tiny_Town && m_putFurnitureId > 0);
		m_btnPutFurnitureCancel->SetVisible(GameClient::CGame::Instance()->GetGameType() == ClientGameType::Tiny_Town && m_putFurnitureId > 0);
		m_btnManorPraise->SetVisible(GameClient::CGame::Instance()->GetGameType() == ClientGameType::Tiny_Town && m_putFurnitureId<=0 && manorCheckPlayerLocation() != 0);
		updateSkillBtnInfo(nTimeElapse);
		updateExpBar();
		checkPersonalShopUI();
		checkCrossHairIsCanAttack();
		updateItemSkillBtnInfo(nTimeElapse);
		checkSkillIsHaveAttackEntity();
		updateMaskInfo(nTimeElapse);
		checkFlyBtnShow();
		checkOpenFlyingShow();
		birdTaskUpdate(nTimeElapse);

		if (m_showShootWarning)
		{
			if (m_shootWarningAlpha <= 0.0f)
			{
				m_showShootWarning = false;
				m_warningLayout->SetVisible(false);
				m_shootWarning->SetVisible(false);
			}
			else
			{
				m_shootWarningAlpha -= 0.2f * Root::Instance()->getFrameTime() / 100;
				m_shootWarning->SetAlpha(Math::Clamp(m_shootWarningAlpha, 0.0f, 1.0f));
				m_shootWarning->SetRotate(m_shootWarningRotateAngle);
				m_shootWarning->SetVisible(true);
			}
		}

		if (m_playEndingAnimation)
		{
			updateEndingAnimation();
		}

		m_btnPlaceBuilding->SetVisible( checkPlaceBuildingViewVisible());
		updateHideAndSeekYawChange(nTimeElapse);
		onBirdDataUpdate(nTimeElapse);
	}

	void gui_mainControl::checkPersonalShopUI()
	{
		bool canShowPersonalShop = getParent()->canPersonalShop();
		m_personalShopBtn->SetVisible(canShowPersonalShop);
		m_buyButtleBtn->SetVisible(canShowPersonalShop && UIDisplaySetting::getUIDisplay()->isShowBuyBullet);
	}

	bool gui_mainControl::canUseByUseItemBtn(Item* item)
	{
		if (item->itemID == BLOCK_ID_TNT 
			|| item->itemID == Item::snowball->itemID
			|| dynamic_cast<ItemFood*>(item)
			|| dynamic_cast<ItemTeleportScroll*>(item)
			|| dynamic_cast<ItemCreateBridge*>(item)
			|| dynamic_cast<ItemPotion*>(item)
			|| dynamic_cast<ItemGrenade*>(item)
			|| dynamic_cast<ItemFireball*>(item)
			|| dynamic_cast<ItemMedichine*>(item))
		{
			return true;
		}
		return false;
	}

	void gui_mainControl::CheckUseItemBtn()
	{
		if (!Blockman::Instance() || !Blockman::Instance()->m_pPlayer || !Blockman::Instance()->m_pPlayer->inventory)
		{
			return;
		}

		if (Blockman::Instance()->m_pPlayer->isOnVehicle())
		{
			m_btnUseItem->SetVisible(false);
			return;
		}

		ItemStackPtr pCurrentItem = Blockman::Instance()->m_pPlayer->inventory->getCurrentItem();
		if (!pCurrentItem)
		{
			m_btnUseItem->SetVisible(false);
			return;
		}

		if (canUseByUseItemBtn(pCurrentItem->getItem()))
		{
			m_btnUseItem->SetVisible(true);
			return;
		}

		m_btnUseItem->SetVisible(false);
	}

	bool gui_mainControl::onClickUseItemBtn(const EventArgs &)
	{
		if (!Blockman::Instance()->m_pPlayer->inventory)
			return false;

		ItemStackPtr pCurrentItem = Blockman::Instance()->m_pPlayer->inventory->getCurrentItem();

		if (pCurrentItem)
		{
			if (canUseByUseItemBtn(pCurrentItem->getItem()))
			{
				pCurrentItem->useItemRightClick(Blockman::Instance()->m_pWorld, Blockman::Instance()->m_pPlayer);
				Blockman::Instance()->m_pPlayer->m_isUsingItemByBtn = true;
				ItemPotion* potion = dynamic_cast<ItemPotion*>(pCurrentItem->getItem());
				if (pCurrentItem->itemID == Item::snowball->itemID || (potion && potion->isSplash()))
				{
					Blockman::Instance()->m_pPlayer->m_isUsingItemByBtn = false;
				}
				return true;
			}
		}

		return false;
	}

	bool gui_mainControl::showPlayerInventory(const EventArgs & events)
	{
		getParent()->showPlayerInventory();
		playSoundByType(ST_Click);
		return true;
	}

	bool gui_mainControl::addChatMessage(const String&  message)
	{
		static unsigned count = 0;
		String strTextName = StringUtil::Format("Main-Chat-Message-List-item-%d", count++).c_str();
		GUIStaticText* pStaticText = (GUIStaticText*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_TEXT, strTextName.c_str());
		pStaticText->SetText(message.c_str());
		pStaticText->SetTouchable(false);
		pStaticText->SetTextVertAlign(VA_CENTRE);
		pStaticText->SetHorizontalAlignment(HA_CENTRE);
		pStaticText->SetVerticalAlignment(VA_CENTRE);
		pStaticText->SetTextScale(1.2f);
		pStaticText->SetWordWrap(true);
		float h = 30.f;
		if (message.length() <30.0f)
		{
			h = 30.f;
		}
		else if (message.length() < 60.0f)
		{
			h = 50.f;
		} 
		else if (message.length() < 90.0f) 
		{
			h = 70.f;
		}
		else
		{
			h = 90.f;
		}

		pStaticText->SetArea(UDim(0, 0), UDim(0, 0), UDim(1, 0), UDim(0, h));
		m_chatMessageListBox->AddItem(pStaticText, false);

		if (m_chatMessageListBox->getContainerWindow()->GetChildCount() == 1)
		{
			m_removeMessageKeepTime = 0;
		}
		else
		{
			scrollBottom();
		}

		return true;
	}

	bool gui_mainControl::showTopSystemNotification(const String & message, i32 keepTime)
	{
		m_topSystemNotification->SetText(message.c_str());
		m_topSystemNotification->SetVisible(true);
		m_topSystemNotification->SetTouchable(false);
		getWindow<GUIStaticText>("Main-Top-System-Notification")->SetTextScale(1.2f);
		m_keepShowTopSystemNotificationTime = keepTime * 1000;
		return true;
	}

	bool gui_mainControl::showBottomSystemNotification(const String & message, i32 keepTime)
	{
		m_bottomSystemNotification->SetText(message.c_str());
		m_bottomSystemNotification->SetVisible(true);
		m_bottomSystemNotification->SetTouchable(false);
		getWindow<GUIStaticText>("Main-Bottom-System-Notification")->SetTextScale(1.2f);
		m_keepShowBottomSystemNotificationTime = keepTime * 1000;
		return true;
	}

	bool gui_mainControl::showCenterSystemNotification(const String & message, i32 keepTime)
	{
		m_centerSystemNotification->SetText(message.c_str());
		m_centerSystemNotification->SetVisible(true);
		m_centerSystemNotification->SetTouchable(false);
		getWindow<GUIStaticText>("Main-Center-System-Notification")->SetTextScale(3.0f);
		m_keepShowCenterSystemNotificationTime = keepTime * 1000;
		return true;
	}

	bool gui_mainControl::onBreakBlockProgress(float progress, RAYTRACE_TYPE type)
	{
		if (m_breakBlockProgress && m_breakBlockProgress->GetChildByIndex(0))
		{
			m_breakBlockProgress->GetChildByIndex(0)->SetArea(UDim(0, 0), UDim(0, 0), UDim(progress, 0), UDim(progress, 0));
		}
		if (progress > 0 && type == RAYTRACE_TYPE_ENTITY)
		{
			m_hitEntityProgress->SetVisible(true);
			m_hitEntityProgress->SetProgress(progress);
		}
		else
		{
			m_hitEntityProgress->SetVisible(false);
			m_hitEntityProgress->SetProgress(0.0f);
		}
		return true;
	}

	bool gui_mainControl::breakBlockUiManage(bool isShow, float x, float y)
	{
		if (m_breakBlockProgress)
		{
			m_breakBlockProgress->SetVisible(isShow);
			m_breakBlockProgress->SetArea(UDim(0, x - 90.0f), UDim(0, y - 90.0f), UDim(0, 180.f), UDim(0, 180.f));
		}
		return true;
	}

	bool gui_mainControl::switchMoveControl(bool isDefault)
	{
		m_arrowControlWindow->SetVisible(!isDefault);
		m_poleControlWindow->SetVisible(isDefault);
		if (isDefault)
		{
			m_buttons[ButtonId::JUMP]->SetVisible(true);
			getWindow("Main-Sneak")->SetVisible(false);
		}
		else
		{
			bool isJumpDefault = Blockman::Instance()->m_gameSettings->isJumpSneakDefault > 0;
			switchJumpSneak(isJumpDefault);
		}
		return true;
	}

	bool gui_mainControl::switchJumpSneak(bool isDefault)
	{
		if (!isHideSneakBtn())
		{
			m_buttons[ButtonId::SNEAK]->SetVisible(isDefault);
			getWindow("Main-Sneak")->SetVisible(!isDefault);
		}
		m_buttons[ButtonId::JUMP]->SetVisible(isDefault);
		getWindow("MainControl-jump")->SetVisible(!isDefault);
		return true;
	}

	void gui_mainControl::useWatchModeUI(bool isWatchMode)
	{
		getWindow("Main-VisibleBar")->SetVisible(!isWatchMode);
		getWindow("Main-Player-Exp")->SetVisible(!isWatchMode && GameClient::CGame::Instance()->GetGameType() != ClientGameType::RanchersExplore);
		getWindow("Main-ItemBarBg")->SetVisible(!isWatchMode);
		m_watchBattleWindow->SetVisible(isWatchMode);

		if (isWatchMode && Blockman::Instance()->m_gameSettings->getAutoRunStatus())
		{
			ForceAutoRun();
		}
	}

	void gui_mainControl::removeChatMessage()
	{
		if (m_chatMessageListBox && m_chatMessageListBox->getContainerWindow())
		{
			if (m_chatMessageListBox->getContainerWindow()->GetChildCount() > 0)
			{
				m_chatMessageListBox->DeleteItem(0);
				scrollBottom();
				if (m_chatMessageListBox->GetPixelSize().y - m_chatMessageListBox->getContainerWindow()->GetPixelSize().y < 0)
				{
					removeChatMessage();
				}
			}
		}
	}

	void gui_mainControl::scrollBottom()
	{
		if (m_chatMessageListBox->getContainerWindow() && m_chatMessageListBox->GetPixelSize().y - m_chatMessageListBox->getContainerWindow()->GetPixelSize().y < 0)
		{
			m_chatMessageListBox->SetScrollOffset(m_chatMessageListBox->GetPixelSize().y - m_chatMessageListBox->getContainerWindow()->GetPixelSize().y);
		}
	}
	void gui_mainControl::updateDropItemProgress()
	{
		GuiSlotTable* slotTable = dynamic_cast<GuiSlotTable*>(getWindow("Main-VisibleBar-SlotTable"));
		GUIProgressBar* dropItemProgress = dynamic_cast<GUIProgressBar*>(getWindow("Main-DropItemProgress"));
		if (slotTable && m_dropItemIndex >= 0)
		{
			float progress = (float)m_touchTime / 1200.f;
			slotTable->setDropItemProgress(m_dropItemIndex, progress >= 1.0f ? 0 : progress);
			dropItemProgress->SetProgress(progress > 1 ? 0 : progress);
			dropItemProgress->SetVisible(progress <= 1);
			if (progress >= 1)
			{
				EntityPlayerSP* player = Blockman::Instance()->m_pPlayer;
				LordAssert(player);
				InventoryPlayer* inventory = player->inventory;
				ItemStackPtr stack = inventory->hotbar[m_dropItemIndex];
				if (stack && stack->stackSize != 0)
				{
					ClientNetwork::Instance()->getSender()->sendDropItemPosition(inventory->getMainInventory()->findItemStack(stack));
					inventory->getMainInventory()->decrStackSize(inventory->getMainInventory()->findItemStack(stack), stack->stackSize);
					HideGunUI();
				}
				m_isLongTouch = false;
				m_touchTime = 0;
				m_dropItemIndex = -1;
				dropItemProgress->SetVisible(false);
			}
		}
		else
		{
			dropItemProgress->SetProgress(0.f);
			dropItemProgress->SetVisible(false);
		}
	}

	void gui_mainControl::HideGunUI()
	{
		m_gunOperationWindow->SetVisible(false);
		m_isTouchLeftShootBtn = false;
		m_isTouchRightShootBtn = false;
		setAimState(false);
	}

	void gui_mainControl::CheckGunUI()
	{
		if (!Blockman::Instance() || !Blockman::Instance()->m_pPlayer || !Blockman::Instance()->m_pPlayer->inventory)
		{
			return;
		}

		if (Blockman::Instance()->m_pPlayer->isOnVehicle())
		{
			HideGunUI();
			return;
		}

		ItemStackPtr pCurrentItem = Blockman::Instance()->m_pPlayer->inventory->getCurrentItem();
		if (!pCurrentItem)
		{
			HideGunUI();
			return;
		}

		ItemGun* pGun = dynamic_cast<ItemGun*>(pCurrentItem->getItem());
		if (!pGun)
		{
			HideGunUI();
			return;
		}
		m_gunOperationWindow->SetVisible(true);
		setAimState(false);
		const GunSetting* pSetting = pGun->getGunSetting();
		if (pSetting)
		{
			m_aimBtn->SetVisible(pSetting->sniperDistance > 0.0f);
		}
	}

	bool gui_mainControl::tryFireGun(bool bSingleClick)
	{
		if (!CanShoot(bSingleClick))
			return false;

		Blockman::Instance()->m_pPlayer->swingItem();
		Blockman::Instance()->m_pPlayer->gunFire();

		auto pPlayer = Blockman::Instance()->m_pPlayer;
		ItemStackPtr stack = pPlayer->inventory->getCurrentItem();
		if (stack)
		{
			ItemGun* pGun = dynamic_cast<ItemGun*>(stack->getItem());
			if (pGun)
			{
				const GunSetting* pGunSetting = pGun->getGunSetting();
				if (pGunSetting)
				{
					m_crossHairMoveStepLength = pGunSetting->crossHairMoveSpeed;
					m_crossHairMoveRange = pGunSetting->crossHairMoveDistance;
					if (pGunSetting->gunType == SNIPER_GUN || pGunSetting->gunType == SNIPER_EFFECT_GUN) {
							HideGunUI();
							//Blockman::Instance()->m_pPlayer->playReloadProgress();
							CheckGunUI();
					}
				}
			}
		}
		return true;
	}

	bool gui_mainControl::CanShoot(bool bSingleClick)
	{
		auto pPlayer = Blockman::Instance()->m_pPlayer;
		if (pPlayer->isOnVehicle())
			return false;

		if (pPlayer->m_isReloadBullet)
			return false;

		ItemStackPtr stack = pPlayer->inventory->getCurrentItem();
		if (!stack || stack->stackSize < 1)
			return false;

		ItemGun* pGun = dynamic_cast<ItemGun*>(stack->getItem());
		if (!pGun)
		{
			LordLogError("CurrentStack is not type of ItemGun");
			return false;
		}

		const GunSetting* pGunSetting = pGun->getGunSetting();
		if (!pGunSetting)
		{
			LordLogError("The GunSetting of currentStack not found");
			return false;
		}

		if (stack->isOnCd())
			return false;

		int bulletNum = stack->getCurrentClipBulletNum();
		int totalNum = stack->getTotalBulletNum();
		if (bulletNum < 1)
		{
			int nCountInInventory = pPlayer->inventory->getItemNum(stack->itemID);
			if (nCountInInventory > 0)
			{
				Blockman::Instance()->m_pPlayer->beginReloadBullet();
			}
			else if (bSingleClick)
			{
				playSoundByType((SoundType)pGunSetting->emptySoundType);
			}

			return false;
		}

		stack->consumeBullet();
		stack->setCd(pPlayer->getGunFireCd(pGunSetting));
		return true;
	}

	bool gui_mainControl::onClickReloadBulletBtn(const EventArgs &)
	{
		HideGunUI();
		bool bResult = Blockman::Instance()->m_pPlayer->beginReloadBullet();
		CheckGunUI();
		return bResult;
	}

	bool gui_mainControl::onClickAimBtn(const EventArgs & events)
	{
		m_aimBtn->SetVisible(false);
		setAimState(true);
		AimingStateChangeEvent::emit(false, NONE_TYPE);
		return true;
	}

	bool gui_mainControl::onClickCancelAimBtn(const EventArgs & events)
	{
		m_aimBtn->SetVisible(true);
		setAimState(false);
		AimingStateChangeEvent::emit(true, RIFLE_CROSS_HAIR);
		return true;
	}

	bool gui_mainControl::onClickThrowGunBtn(const EventArgs &)
	{
		int index = Blockman::Instance()->m_pPlayer->inventory->currentItemIndex;
		GameClient::CGame::Instance()->getNetwork()->getSender()->sendDropItemPosition(index);
		return true;
	}

	void gui_mainControl::checkWatchRespawnUI()
	{
		auto btnWatchRespawn = getWindow<GUIButton>("Main-Watch-Battle-Respawn");
		auto pDisplay = UIDisplaySetting::getUIDisplay();
		getWindow<GUIStaticText>("Main-Watch-Battle-Text")->SetText(getString(LanguageKey::GUI_MAIN_WATCHING_BATTLE).c_str());
		if (pDisplay->isShowWatchRespawn)
		{
			btnWatchRespawn->SetText(getString(LanguageKey::GUI_REBIRTH));
			btnWatchRespawn->SetVisible(true);
			btnWatchRespawn->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onWatchRepawnClick, this, std::placeholders::_1));
		}
		else
		{
			btnWatchRespawn->SetVisible(false);
		}
	}

	void gui_mainControl::refreshGunPreview()
	{
		ItemStackPtr itemStack = Blockman::Instance()->m_pPlayer->inventory->getCurrentItem();
		if (!itemStack || itemStack->stackSize <= 0)
		{
			getWindow("Main-Gun-Preview")->SetVisible(false);
			getWindow<GUICheckBox>("Main-GunPreviewCheckBox")->SetCheckedNoEvent(false);
			return;
		}

		ItemGun* pGun = dynamic_cast<ItemGun*>(itemStack->getItem());
		if (!pGun)
		{
			getWindow("Main-Gun-Preview")->SetVisible(false);
			getWindow<GUICheckBox>("Main-GunPreviewCheckBox")->SetCheckedNoEvent(false);
			return;
		}

		getWindow("Main-Gun-Preview")->SetVisible(getWindow<GUICheckBox>("Main-GunPreviewCheckBox")->GetChecked());

		const GunSetting* gunSetting = pGun->getGunSetting();
		if (!gunSetting)
		{
			return;
		}
		getWindow("Main-Gun-Harm-Value")->SetText(StringUtil::ToString(gunSetting->damage).c_str());
		getWindow("Main-Gun-Range-Value")->SetText(StringUtil::Format(getString(LanguageKey::GUI_STR_GUN_RANGE_VALUE).c_str(), gunSetting->shootRange).c_str());
		getWindow("Main-Gun-FiringRate-Value")->SetText(StringUtil::Format(getString(LanguageKey::GUI_STR_GUN_FIRING_RATE_VALUE).c_str(), gunSetting->cdTime).c_str());
		getWindow("Main-Gun-Deviation-Value")->SetText(StringUtil::Format(getString(LanguageKey::GUI_STR_GUN_DEVIATION_VALUE).c_str(), gunSetting->bulletOffset).c_str());
		getWindow("Main-Gun-Name")->SetText(LanguageManager::Instance()->getItemName(itemStack->getItemName()).c_str());
		const BulletClipSetting*  bulletClipSetting = itemStack->getBulletClipSetting();
		if (bulletClipSetting)
		{
			getWindow<GUIStaticImage>("Main-Gun-Bullet-Icon")->SetImage(bulletClipSetting->bulletIcon.c_str());
		}
		m_actorWindow->SetActor("gun.actor", "idle");
		m_actorWindow->SetRotateY(90);
		StringArray array = StringUtil::Split(gunSetting->previewMesh, ".", 1);
		if (array.size() == 2)
		{
			m_actorWindow->UseBodyPart(array[0], array[1]);
		}
	}

	bool gui_mainControl::onLeftShootBtnTouchDown(const EventArgs& events)
	{
		if (!m_isTouchLeftShootBtn)
		{
			m_isTouchLeftShootBtn = true;
			bool result = tryFireGun(true);
			m_bChangeCrosshair = result ? true : m_bChangeCrosshair;
			return result;
		}

		return true;
	}

	bool gui_mainControl::onLeftShootBtnTouchUp(const EventArgs& events)
	{
		m_isTouchLeftShootBtn = false;

		InventoryPlayer* inventory = Blockman::Instance()->m_pPlayer->inventory;
		ItemStackPtr stack = inventory ? inventory->getCurrentItem() : nullptr;
		if (stack && stack->getGunSetting() && (stack->getGunSetting()->gunType == LASER_EFFECT_GUN || stack->getGunSetting()->gunType == ELECTRICITY_EFFECT_GUN))
		{
			ClientNetwork::Instance()->getSender()->sendStopLaserGun();
		}

		return true;
	}

	bool gui_mainControl::onRightShootBtnTouchDown(const EventArgs& events)
	{
		if (!m_isTouchRightShootBtn)
		{
			m_isTouchRightShootBtn = true;
			bool result = tryFireGun(true);
			m_bChangeCrosshair = result ? true : m_bChangeCrosshair;
			return result;
		}

		return true;
	}

	bool gui_mainControl::onRightShootBtnTouchUp(const EventArgs& events)
	{
		m_isTouchRightShootBtn = false;
		m_rightShootBtn->SetPosition(m_originButtonPos);
		//Blockman::Instance()->m_gameSettings->mouseSensitivity = m_prevMouseSensitive;
		Blockman::Instance()->m_gameSettings->setMouseMoveState(false);

		InventoryPlayer* inventory = Blockman::Instance()->m_pPlayer->inventory;
		ItemStackPtr stack = inventory ? inventory->getCurrentItem() : nullptr;
		if (stack && stack->getGunSetting() && (stack->getGunSetting()->gunType == LASER_EFFECT_GUN || stack->getGunSetting()->gunType == ELECTRICITY_EFFECT_GUN))
		{
			ClientNetwork::Instance()->getSender()->sendStopLaserGun();
		}

		return true;
	}

	bool gui_mainControl::onRightShootBtnMoved(const EventArgs & events)
	{
		auto mouseEventArgs = dynamic_cast<const MouseEventArgs&>(events);
		Blockman::Instance()->m_gameSettings->beginMouseMove(mouseEventArgs.position.x, mouseEventArgs.position.y);
		float offset = m_rightShootBtn->GetWidth().d_offset;
		m_rightShootBtn->SetPosition(UVector2({ 0, mouseEventArgs.position.x - offset / 2 }, {0, mouseEventArgs.position.y - offset / 2}));
		return false;
	}

	void gui_mainControl::updateShootProgress()
	{
		if (!m_isTouchLeftShootBtn && !m_isTouchRightShootBtn)
			return;

		m_bChangeCrosshair = tryFireGun(false) ? true : m_bChangeCrosshair;
	}

	void gui_mainControl::setAimState(bool bState)
	{
		if (!Blockman::Instance() || !Blockman::Instance()->m_pPlayer || !Blockman::Instance()->m_pPlayer->inventory)
		{
			return;
		}

		m_gunSniperSight->SetVisible(bState);
		m_cancelAimBtn->SetVisible(bState);
		if (!bState)
		{
			if (Blockman::Instance()->m_pPlayer->m_bIsInAimSight)
			{
				Blockman::Instance()->m_gameSettings->setFovSetting(m_prevFovSetting);
				Blockman::Instance()->m_gameSettings->setMouseSensitivity(m_prevMouseSensitive);
				Blockman::Instance()->m_gameSettings->setPersonView(m_prevCameraView);
			}
		}
		else
		{
			ItemStackPtr stack = Blockman::Instance()->m_pPlayer->inventory->getCurrentItem();
			if (stack)
			{
				const GunSetting* pSetting = stack->getGunSetting();
				if (pSetting)
				{
					m_prevFovSetting = Blockman::Instance()->m_gameSettings->getFovSetting();
					m_prevMouseSensitive = Blockman::Instance()->m_gameSettings->getMouseSensitivity();
					m_prevCameraView = Blockman::Instance()->m_gameSettings->getPersonView();
					float fov = -(float)pSetting->sniperDistance / 10;
					fov = Math::Max(-1.4f, fov); // must > -1.5
					Blockman::Instance()->m_gameSettings->setFovSetting(fov);
					Blockman::Instance()->m_gameSettings->setMouseSensitivity(pSetting->mouseSensitivity);
					Blockman::Instance()->m_gameSettings->setPersonView(GameSettings::SPVT_FIRST);
				}
			}
		}

		Blockman::Instance()->m_pPlayer->m_bIsInAimSight = bState;
	}

	void gui_mainControl::checkHideGunUI()
	{
		/*if (!m_gunOperationWindow->IsVisible())
			return;*/
		m_gunOperationWindow->SetVisible(true);

		if (Blockman::Instance()->m_pPlayer->isOnVehicle())
		{
			HideGunUI();
			return;
		}

		ItemStackPtr pCurrentItem = Blockman::Instance()->m_pPlayer->inventory->getCurrentItem();
		if (!pCurrentItem)
		{
			HideGunUI();
			return;
		}

		ItemGun* pGun = dynamic_cast<ItemGun*>(pCurrentItem->getItem());
		if (!pGun)
		{
			HideGunUI();
			return;
		}
	}

	void gui_mainControl::checkAimUI()
	{
		if (!Blockman::Instance() || !Blockman::Instance()->m_pPlayer || !Blockman::Instance()->m_pPlayer->inventory)
		{
			return;
		}

		ItemStackPtr ptr = Blockman::Instance()->m_pPlayer->inventory->getCurrentItem();
		if (ptr && !Blockman::Instance()->m_pPlayer->isOnVehicle())
		{
			if (ptr->itemID == BLOCKMAN::Item::snowball->itemID || ptr->isGunItem() || ptr->isThrowItemSkill() || ptr->itemID == BLOCKMAN::Item::fishingRod->itemID)
			{
				int crossHairType = COMMON_CROSS_HAIR;
				if (ptr->getGunSetting())
				{
					crossHairType = Blockman::Instance()->m_pPlayer->getCrossHairByGunType(ptr->getGunSetting()->gunType);
				}
				if (ptr->isThrowItemSkill())
				{
					crossHairType = RIFLE_CROSS_HAIR;
				}
				AimingStateChangeEvent::emit(true, crossHairType);
			}
			else
			{
				AimingStateChangeEvent::emit(false, NONE_TYPE);
			}
		}
		else
		{
			AimingStateChangeEvent::emit(false, NONE_TYPE);
		}
	}

	void gui_mainControl::updateCameraBtnUI()
	{
		if (Blockman::Instance()->m_pPlayer->getCurrCameraStatus())
		{
			m_cameraLockImage->SetVisible(true);
			m_cameraUnLockImage->SetVisible(false);
		}
		else
		{
			m_cameraLockImage->SetVisible(false);
			m_cameraUnLockImage->SetVisible(true);
		}
	}

	bool gui_mainControl::onClickLockBtn(const EventArgs & events)
	{
		auto pPlayer = Blockman::Instance()->m_pPlayer;
		if (!pPlayer->isDriver() || pPlayer->getVehicle() == NULL)
			return false;

		GameClient::CGame::Instance()->getNetwork()->getSender()->sendLockVehicle(true, pPlayer->getVehicle()->entityId);
		return true;
	}

	bool gui_mainControl::onClickUnlockBtn(const EventArgs & events)
	{
		auto pPlayer = Blockman::Instance()->m_pPlayer;
		if (!pPlayer->isDriver() || pPlayer->getVehicle() == NULL)
			return false;

		GameClient::CGame::Instance()->getNetwork()->getSender()->sendLockVehicle(false, pPlayer->getVehicle()->entityId);
		return true;
	}

	bool gui_mainControl::onClickTeleportBtn(const EventArgs & events)
	{
		GameClient::CGame::Instance()->getNetwork()->getSender()->sendClickTeleport();
		m_teleportBtnCountdown = 9 * 1000;
		return true;
	}

	bool gui_mainControl::showParachuteBtn(bool isNeedOpen)
	{
		if (m_parachuteBtn) {
			if (isNeedOpen) {
				m_parachuteBtn->SetVisible(true);
			}
			else {
				m_parachuteBtn->SetVisible(false);
			}
			
		}
		return true;
	}

	void gui_mainControl::showFlyBtn(bool isNeedOpen)
	{
		if (m_flyBtn) {
			if (isNeedOpen) {
				m_flyBtn->SetVisible(true);
			}
			else {
				m_flyBtn->SetVisible(false);
			}
		}
	}

	void gui_mainControl::showBuildWarBlockBtn(bool isNeedOpen)
	{
		if (m_buildWarBlockBtn) {
			if (isNeedOpen) {
				m_buildWarBlockBtn->SetVisible(true);
			}
			else {
				m_buildWarBlockBtn->SetVisible(false);
			}
		}
	}

	bool gui_mainControl::onPoleTouchDown(const EventArgs & events)
	{
		m_poleMoveWindow->SetArea(UDim(0.0f, -50.0f), UDim(-1.25f, 70.0f), UDim(2.0f, 0.0f), UDim(2.25f, 0.0f));
		m_poleBgWindow->SetAlpha(0.5);
		return true;
	}

	bool gui_mainControl::onPoleTouchUp(const EventArgs & events)
	{
		m_poleMoveWindow->SetArea(UDim(0.0f, 0.0f), UDim(0.0f, 0.0f), UDim(1.0f, 0.0f), UDim(1.0f, 0.0f));
		m_poleCenterWindow->SetPosition(m_originPolePos);
		m_poleBgWindow->SetAlpha(0.75);
		Blockman::Instance()->m_gameSettings->m_poleForward = 0.0f;
		Blockman::Instance()->m_gameSettings->m_poleStrafe = 0.0f;
		return true;
	}

	bool gui_mainControl::onPoleTouchMove(const EventArgs & events)
	{
		setPoleCenterPos(events);
		return true;
	}

	bool gui_mainControl::setPoleCenterPos(const EventArgs & events)
	{
		auto mouseEventArgs = dynamic_cast<const MouseEventArgs&>(events);
		float fMaxDis = 25.0f;
		float offX = mouseEventArgs.position.x - m_originPoleAbsPos.d_x.d_offset;
		float offY = mouseEventArgs.position.y - m_originPoleAbsPos.d_y.d_offset;
		float disSqr = offX * offX + offY * offY;
		if (disSqr > fMaxDis * fMaxDis)
		{
			float rate = Math::Sqrt(fMaxDis * fMaxDis / disSqr);
			offX = offX * rate;
			offY = offY * rate;
			disSqr = fMaxDis * fMaxDis;
		}

		auto newPos = UVector2({ 0, m_originPolePos.d_x.d_offset + offX }, { 0, m_originPolePos.d_y.d_offset + offY });
		m_poleCenterWindow->SetPosition(newPos);
		Blockman::Instance()->m_gameSettings->m_poleForward = -offY / Math::Sqrt(disSqr);
		Blockman::Instance()->m_gameSettings->m_poleStrafe = -offX / Math::Sqrt(disSqr);
		return true;
	}

	bool gui_mainControl::onClickSwitchMoveState(const EventArgs & events)
	{
		Blockman::Instance()->m_gameSettings->switchMoveState();
		refreshMoveState();
		return true;
	}

	void gui_mainControl::refreshMoveState()
	{
		auto pGameSetting = Blockman::Instance()->m_gameSettings;
		MoveState curState = pGameSetting->m_curMoveState;
		m_sneakStateWindow->SetVisible(curState == MoveState::SneakState);
		m_runStateWindow->SetVisible(curState == MoveState::RunState);
		m_sprintStateWindow->SetVisible(curState == MoveState::SprintState);
		pGameSetting->m_clientMovementInput->sneakPressed = (curState == MoveState::SneakState);
	}

	void gui_mainControl::updateSkillBtnInfo(ui32 timeElapse)
	{

		if (m_skillResidueCDTime > 0)
		{
			m_skillResidueCDTime = m_skillResidueCDTime - timeElapse;
		}
		m_skillBtn->SetEnabled(m_skillResidueCDTime <= 0);
		m_skillBtn->SetText(m_skillResidueCDTime > 0 ? StringUtil::ToString(m_skillResidueCDTime / 1000 + 1).c_str() : "");
		m_skillBtn->SetTextScale(1.6f);

		switch (m_skillType)
		{
		case SkillType::DEFENSE:
			m_skillBtn->SetNormalImage("set:chicken_icon.json image:btn_defense");
			m_skillBtn->SetPushedImage("set:chicken_icon.json image:btn_defense");
			break;
		case SkillType::SPRINT:
			m_skillBtn->SetNormalImage("set:chicken_icon.json image:btn_sprint");
			m_skillBtn->SetPushedImage("set:chicken_icon.json image:btn_sprint");
			break;
		case SkillType::RELEASE_TOXIC:
			m_skillBtn->SetNormalImage("set:chicken_icon.json image:btn_poison");
			m_skillBtn->SetPushedImage("set:chicken_icon.json image:btn_poison");
			break;
		default:
			m_skillBtn->SetNormalImage("");
			m_skillBtn->SetPushedImage("");
			break;
		}
	}

	bool gui_mainControl::onClickParachuteBtn(const EventArgs & events)
	{
		//m_parachuteBtn->SetVisible(false);
		auto pPlayer = Blockman::Instance()->m_pPlayer;
		if (!pPlayer)
			return false;

		//if (!pPlayer->onGround) {
			//pPlayer->startParachute();
			GameClient::CGame::Instance()->getNetwork()->getSender()->sendLeaveAircraft();
		//}
		return true;
	}

	bool gui_mainControl::onClickFlyBtnDown(const EventArgs & events)
	{
		if (Blockman::Instance()->m_pPlayer->capabilities.canBeFlying())
		{
			KeyBinding::setKeyBindState(BUTTON_TO_KEY_BINDINGS.at(ButtonId::JUMP), true);
			Blockman::Instance()->m_pPlayer->capabilities.isFlying = true;
			ClientNetwork::Instance()->getSender()->sendSetFlying(true);
			Blockman::Instance()->m_pPlayer->movementInput->down = false;
			Blockman::Instance()->m_pPlayer->movementInput->jump = false;
			m_buttonPressed[ButtonId::DROP] = false;
			m_buttonPressed[ButtonId::UP] = false;
			m_buttonPressed[ButtonId::DOWN] = false;
		}
		return true;
	}

	bool gui_mainControl::onClickFlyBtnUp(const EventArgs & events)
	{
		KeyBinding::setKeyBindState(BUTTON_TO_KEY_BINDINGS.at(ButtonId::JUMP), false);
		return true;
	}

	bool gui_mainControl::onClickBuildWarBlockBtn(const EventArgs & events)
	{
		GameClient::CGame::Instance()->getNetwork()->getSender()->sendBuildWarOpenShop();
		return true;
	}

	bool gui_mainControl::onClickShowManorBtn(const EventArgs & events)
	{
		RootGuiLayout::Instance()->showManor();
		return true;
	}

	bool gui_mainControl::onClickShowExItemBtn(const EventArgs & events)
	{
		getWindow<GUIStaticImage>("Main-Show-RanchItem-RedPoint")->SetVisible(false);
		if (!m_ranchExItemShow)
		{
			getWindow<GUILayout>("Main-RanchExItemLayout")->SetVisible(true);
			m_ranchExItemShow = true;
		}
		else
		{
			getWindow<GUILayout>("Main-RanchExItemLayout")->SetVisible(false);
			m_ranchExItemShow = false;
		}
		return true;
	}

	bool gui_mainControl::onClickChangeActorBtn(const EventArgs & events)
	{
		GameClient::CGame::Instance()->getNetwork()->getSender()->sendClickChangeActor();
		return true;
	}

	bool gui_mainControl::onClickChangeActorYawBtn(const EventArgs & events, bool left)
	{
		if (Blockman::Instance()->m_pPlayer->getCurrCameraStatus())
		{
			return true;
		}

		ActorObject* actor = EntityRenderManager::Instance()->getEntityActor(Blockman::Instance()->renderViewEntity);
		if (!actor)
			return true;

		float offset = left ? 5.0f : -5.0f;
		Quaternion curQ = actor->GetOrientation();
		Real radian = curQ.getRadian() + offset * Math::DEG2RAD;

		// 0 or 360
		if (radian > 6.2)
		{
			radian = 0.0f;
		}
		if (radian < 0.0)
		{
			radian = 6.2f;
		}
		Blockman::Instance()->m_pPlayer->spYawRadian = radian;
		ClientNetwork::Instance()->getSender()->sendSetSpYaw(Blockman::Instance()->m_pPlayer->spYaw, Blockman::Instance()->m_pPlayer->spYawRadian);

		return true;
	}

	bool gui_mainControl::onClickChangeActorYawLongTouchStartBtn(const EventArgs & events, bool left)
	{
		if (Blockman::Instance()->m_pPlayer->getCurrCameraStatus())
		{
			return true;
		}

		m_hideandseekYawChange = true;
		m_hideandseekYawLeft = left;
		return true;
	}

	bool gui_mainControl::onClickChangeActorYawLongTouchEndBtn(const EventArgs & events, bool left)
	{
		if (Blockman::Instance()->m_pPlayer->getCurrCameraStatus())
		{
			return true;
		}

		m_hideandseekYawChange = false;
		m_hideandseekYawLeft = left;

		return true;
	}

	bool gui_mainControl::showHideAndSeekBtn(bool isShowChangeActoBtn, bool isShowLockCameraBtn, bool isShowOthenBtn)
	{
		m_hideAndSeekControlWindow->SetVisible(isShowChangeActoBtn || isShowLockCameraBtn || isShowOthenBtn);
		m_changeActorBtn->SetVisible(isShowChangeActoBtn);
		m_cameraChangeStatusBtn->SetVisible(isShowLockCameraBtn);

		m_hideandseekChangeYaw->SetVisible(isShowOthenBtn);
		Blockman::Instance()->m_pPlayer->spYaw = isShowOthenBtn;
		ClientNetwork::Instance()->getSender()->sendSetSpYaw(Blockman::Instance()->m_pPlayer->spYaw, Blockman::Instance()->m_pPlayer->spYawRadian);

		updateCameraBtnUI();
		return true;
	}

	bool gui_mainControl::refreshHideAndSeekBtn(const int status)
	{
		m_gameStatus = status == 1 ? GRME_ROUND_STATUS_START : GRME_ROUND_STATUS_WAIT;
		if (m_gameStatus == GRME_ROUND_STATUS_WAIT)
		{
			if (Blockman::Instance()->m_pPlayer->m_isCameraLocked)
				Blockman::Instance()->m_pPlayer->setCameraLocked(false);
			if (m_hideAndSeekControlWindow->IsVisible())
			{
				m_hideAndSeekControlWindow->SetVisible(false);
				m_changeActorBtn->SetVisible(false);
				m_cameraChangeStatusBtn->SetVisible(false);
			}
		}
		return true;
	}

	bool gui_mainControl::setPutFurnitureId(i32 furnitureId)
	{
		this->m_putFurnitureId = furnitureId;
		RootGuiLayout::Instance()->showMainControl();
		m_currentCameraView = Blockman::Instance()->m_gameSettings->getPersonView();
		Blockman::Instance()->setPersonView(1);
		return true;
	}

	bool gui_mainControl::showPlayerOperation(ui64 targetUserId, const String& name)
	{
		if (m_playerOperationLayout && m_guiPlayerOperation)
		{
			m_guiPlayerOperation->setTargetUserId(targetUserId);
			m_guiPlayerOperation->setTargetUserName(name);
			m_playerOperationLayout->SetVisible(true);
		}
		return true;
	}

	bool gui_mainControl::setManorBtnVisible(bool visible)
	{
		m_manorBtnVisible = visible;
		return true;
	}

	bool gui_mainControl::onClickChangeCameraStatusBtn(const EventArgs & events)
	{
		if (!m_cameraLockImage || !m_cameraUnLockImage)
			return false;

		if (Blockman::Instance()->m_pPlayer->getBaseAction() != AS_IDLE)
			return false;

		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::HASHidden || GameClient::CGame::Instance()->GetGameType() == ClientGameType::HASChase)
		{
			if (!m_isCanChangeCameraStatus)
				return false;
			m_isCanChangeCameraStatus = false;
			m_ChangeCameraStatusCd = 0;
		}
		Blockman::Instance()->m_pPlayer->setCameraLocked(!Blockman::Instance()->m_pPlayer->getCurrCameraStatus());
		updateCameraBtnUI();

		return true;
	}

	bool gui_mainControl::onLockStateChanged(bool bState)
	{
		getWindow("Main-Car-Control-Lock")->SetVisible(!bState);
		getWindow("Main-Car-Control-Unlock")->SetVisible(bState);
		return true;
	}

	bool gui_mainControl::onVehicleStateChanged(bool bOnoff, bool bIsDriver)
	{
		CheckGunUI();
		checkAimUI();
		CheckFishingBtn();
		//CheckUseItemBtn();

		if (bIsDriver)
		{
			m_carControlWindow->SetVisible(bOnoff);
			if (bOnoff)
			{
				onLockStateChanged(false);
			}
		}

		return true;
	}

	bool gui_mainControl::isHideSneakBtn() 
	{
		auto pDisplay = UIDisplaySetting::getUIDisplay();
		return !pDisplay->isShowSneak;
	}

	bool gui_mainControl::isHideStateBtn()
	{
		auto pDisplay = UIDisplaySetting::getUIDisplay();
		return !pDisplay->isShowState;
	}

	bool gui_mainControl::hidePlayerOperationView(const EventArgs & events)
	{
		if (m_playerOperationLayout)
		{
			m_playerOperationLayout->SetVisible(false);
		}
		return true;
	}

	ui64 gui_mainControl::manorCheckPlayerLocation()
	{
		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::Tiny_Town)
		{
			return dynamic_cast<WorldClient*>(Blockman::Instance()->m_pWorld)->findManorByPlayerPosition(Blockman::Instance()->m_pPlayer);
		}
		return 0;
	}

	bool gui_mainControl::checkPlaceBuildingViewVisible()
	{
		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::Rancher && Blockman::Instance()->m_pPlayer && Blockman::Instance()->objectMouseOver.typeOfHit == RAYTRACE_TYPE_BLOCK)
		{
			ItemStackPtr pCurrentItem = Blockman::Instance()->m_pPlayer->inventory->getCurrentItem();
			return pCurrentItem && (dynamic_cast<ItemBuild*>(pCurrentItem->getItem()) || dynamic_cast<ItemHouse*>(pCurrentItem->getItem()));
		}
		return false;
	}

	void gui_mainControl::updateExpBar()
	{
		EntityPlayerSPMP* player = Blockman::Instance()->m_pPlayer;
		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::Rancher || GameClient::CGame::Instance()->GetGameType() == ClientGameType::RanchersExplore)
		{
			m_playerExpProgress->SetVisible(false);
		}

		if (player)
		{
			if (player->experienceTotal != 0)
			{
				m_playerExpProgress->SetProgress(player->experience / player->experienceTotal);
			}
		}
	}

	bool gui_mainControl::onWatchRepawnClick(const EventArgs & events)
	{
		GameClient::CGame::Instance()->getNetwork()->getSender()->sendWatchRespawn();
		return true;
	}

	bool gui_mainControl::refreshBossLife(int currentBossLife, int maxBossLife)
	{
		m_bossLifeProgress->SetVisible(currentBossLife > 0);
		m_bossLifeProgress->SetProgress((float)currentBossLife / (float)maxBossLife);
		String txt = StringUtil::Format(getString(LanguageKey::GUI_BOSS_LIFE).c_str(), currentBossLife, maxBossLife);
		getWindow<GUIStaticText>("Main-Boss-Life-Info")->SetText(txt.c_str());
		return true;
	}

	bool gui_mainControl::refreshBuildProgress(const String & name, int curProgress, int maxProgress, const String & iconPath)
	{
		if (iconPath != "")
		{
			m_bossLifeIcon->SetImage(iconPath.c_str());
		}
		m_bossLifeProgress->SetVisible(curProgress > 0);
		m_bossLifeProgress->SetProgress((float)curProgress / (float)maxProgress);
		String content = getString(name).c_str();
		if (content.empty())
		{
			String txt = StringUtil::Format(getString(LanguageKey::GUI_BUILD_PROGRESS).c_str(), curProgress, maxProgress, name.c_str());
			getWindow<GUIStaticText>("Main-Boss-Life-Info")->SetText(txt.c_str());
		}
		else
		{
			String txt = StringUtil::Format(content.c_str(), curProgress);
			getWindow<GUIStaticText>("Main-Boss-Life-Info")->SetText(txt.c_str());
		}
		return true;
	}

	bool gui_mainControl::getTaskInfo(const String & info, std::vector<TaskInfo>& task_list)
	{
		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(info.c_str());

		if (doc->HasParseError())
		{
			LordLogError("gui_mainControl::getTaskInfo HasParseError.");
			LordDelete(doc);
			return false;
		}

		if (!doc->HasMember("groupId") || !doc->HasMember("task_list"))
		{
			LordLogError("gui_mainControl::getTaskInfo The info result content missed some field.");
			LordDelete(doc);
			return false;
		}

		rapidjson::Value arr = doc->FindMember("task_list")->value.GetArray();
		for (size_t i = 0; i < arr.Size(); ++i)
		{
			if (!arr[i].HasMember("task_id")
				|| !arr[i].HasMember("task_status")
				|| !arr[i].HasMember("task_num")
				|| !arr[i].HasMember("task_type")
				|| !arr[i].HasMember("task_des")
				|| !arr[i].HasMember("task_param")
				|| !arr[i].HasMember("task_icon"))
			{
				LordLogError("gui_toolBar::getTaskInfo The game result content player_info missed some field. %d", i);
				LordDelete(doc);
				return false;
			}

			int task_id = arr[i].FindMember("task_id")->value.GetInt();
			int task_status = arr[i].FindMember("task_status")->value.GetInt();
			int task_num = arr[i].FindMember("task_num")->value.GetInt();
			int task_type = arr[i].FindMember("task_type")->value.GetInt();
			int task_des = arr[i].FindMember("task_des")->value.GetInt();
			int task_param = arr[i].FindMember("task_param")->value.GetInt();
			String task_icon = arr[i].FindMember("task_icon")->value.GetString();

			TaskInfo taskInfo;
			taskInfo.task_id = task_id;
			taskInfo.task_status = task_status;
			taskInfo.task_num = task_num;
			taskInfo.task_type = task_type;
			taskInfo.task_des = task_des;
			taskInfo.task_param = task_param;
			taskInfo.task_icon = task_icon;

			task_list.push_back(taskInfo);
		}

		LordDelete(doc);
		return true;
	}

	bool gui_mainControl::getRanchExItemInfo(const String & info, std::vector<RanchExItemInfo>& item_list)
	{
		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(info.c_str());

		if (doc->HasParseError())
		{
			LordLogError("gui_mainControl::getTaskInfo HasParseError.");
			LordDelete(doc);
			return false;
		}

		if (!doc->HasMember("item_list"))
		{
			LordLogError("gui_mainControl::getTaskInfo The info result content missed some field.");
			LordDelete(doc);
			return false;
		}

		rapidjson::Value arr = doc->FindMember("item_list")->value.GetArray();
		for (size_t i = 0; i < arr.Size(); ++i)
		{
			if (!arr[i].HasMember("itemId")
				|| !arr[i].HasMember("itemNum")
				|| !arr[i].HasMember("itemImg"))
			{
				LordLogError("gui_toolBar::getRanchExItemInfo The game result content player_info missed some field. %d", i);
				LordDelete(doc);
				return false;
			}

			int itemId = arr[i].FindMember("itemId")->value.GetInt();
			int itemNum = arr[i].FindMember("itemNum")->value.GetInt();
			String itemImg = arr[i].FindMember("itemImg")->value.GetString();

			RanchExItemInfo itemInfo;
			itemInfo.item_id = itemId;
			itemInfo.item_num = itemNum;
			itemInfo.item_img = itemImg;

			item_list.push_back(itemInfo);
		}

		LordDelete(doc);
		return true;
	}

	bool gui_mainControl::showRanchExTask(bool show, String task)
	{
		if (GameClient::CGame::Instance()->GetGameType() != ClientGameType::RanchersExplore) return true;

		if (show)
		{
			getWindow<GUILayout>("Main-RanchExTask")->SetVisible(true);
			getWindow<GUIStaticText>("Main-RanchExTitle")->SetText(getString(LanguageKey::TOOL_BAR_RANCHEX_TASK_TITLE).c_str());

			std::vector<TaskInfo> task_list;

			if (getTaskInfo(task, task_list))
			{

				for (int i = 0; i < (int)task_list.size() && i < TashInfoMax; ++i)
				{
					String lanType = LanguageManager::Instance()->getCurrentLanguage();
					String task_txt = "";

					task_txt += StringUtil::Format(getString(LanguageKey::TOOL_BAR_RANCHEX_TASK_INFO).c_str(),
						MultiLanTipSetting::getMessage(lanType, task_list[i].task_des, StringUtil::ToString(task_list[i].task_num).c_str()).c_str());

					getWindow<GUIStaticText>(StringUtil::Format("Main-RanchExTaskTxt%d", i + 1).c_str())->SetText(task_txt.c_str());

					if (task_list[i].task_icon.length() > 0)
					{
						getWindow<GUIStaticImage>(StringUtil::Format("Main-RanchExTaskIcon%d", i + 1).c_str())->SetVisible(true);
						getWindow<GUIStaticImage>(StringUtil::Format("Main-RanchExTaskIcon%d", i + 1).c_str())->SetImage(task_list[i].task_icon.c_str());

						if (task_list[i].task_status > 0)
						{
							getWindow<GUIStaticImage>(StringUtil::Format("Main-RanchExTaskIcon%d", i + 1).c_str())->SetAlpha(0.2f);
						}
						else
						{
							getWindow<GUIStaticImage>(StringUtil::Format("Main-RanchExTaskIcon%d", i + 1).c_str())->SetAlpha(1.0f);
						}
					}
					else
					{
						getWindow<GUIStaticImage>(StringUtil::Format("Main-RanchExTaskIcon%d", i + 1).c_str())->SetVisible(false);
					}
				}
			}
		}
		else
		{
			getWindow<GUILayout>("Main-RanchExTask")->SetVisible(false);
		}

		return true;
	}

	bool gui_mainControl::showRanchExCurrentItemTip(bool show, String itemInfo, int type, int num)
	{
		if (GameClient::CGame::Instance()->GetGameType() != ClientGameType::RanchersExplore) return true;

		if (show)
		{
			getWindow<GUILayout>("Main-RanchExCurrentItemLayout")->SetVisible(true);
			if (type == 0)
			{
				getWindow<GUIStaticText>("Main-RanchExCurrentItemTxt")->SetText(StringUtil::Format(getString(itemInfo.c_str()).c_str(), num).c_str());
				if (num <= 0)
				{
					getWindow<GUILayout>("Main-RanchExCurrentItemLayout")->SetVisible(false);
				}
			}
			else
			{
				getWindow<GUIStaticText>("Main-RanchExCurrentItemTxt")->SetText(getString(itemInfo.c_str()).c_str());
			}
		}
		else
		{
			getWindow<GUILayout>("Main-RanchExCurrentItemLayout")->SetVisible(false);
		}

		return true;
	}

	bool gui_mainControl::showRanchExItem(bool show, String item)
	{
		if (GameClient::CGame::Instance()->GetGameType() != ClientGameType::RanchersExplore) return true;

		if (show)
		{
			// getWindow<GUILayout>("Main-RanchExItemLayout")->SetVisible(true);

			std::vector<RanchExItemInfo> item_list;

			if (getRanchExItemInfo(item, item_list))
			{
				getWindow<GUIStaticImage>("Main-Show-RanchItem-RedPoint")->SetVisible(true);
				for (int i = 0; i < (int)item_list.size() && i < MAX_RANCHEX_ITEM_COUNT; ++i)
				{
					m_RanchExItemImg[i]->SetVisible(true);
					m_RanchExItemImg[i]->SetImage(item_list[i].item_img.c_str());
					m_RanchExItemTxt[i]->SetText(StringUtil::ToString(item_list[i].item_num).c_str());
				}
			}
		}
		else
		{
			getWindow<GUILayout>("Main-RanchExItemLayout")->SetVisible(false);
		}

		return true;
	}

	bool gui_mainControl::refreshBasementLife(float curLife, float maxLife)
	{
		m_basementLifeLayout->SetVisible(curLife > 0);
		float progress = curLife / maxLife;
		m_basementLifeProgress->SetProgress(progress);
		String value = StringUtil::Format("[%.0f", Math::Ceil(curLife * 100 / maxLife));
		String txt = StringUtil::Format(getString(LanguageKey::GUI_BASEMENT_LIFE).c_str(), value.c_str());
		txt += "%]";
		m_basementLifeInfo->SetText(txt.c_str());
		if (progress > 0.8f)
			m_basementLifeProgress->SetProgressImage("set:creepy_aliens.json image:basement_life_green");
		else if (progress > 0.3f)
			m_basementLifeProgress->SetProgressImage("set:creepy_aliens.json image:basement_life_yellow");
		else
			m_basementLifeProgress->SetProgressImage("set:creepy_aliens.json image:basement_life_red");
		return true;
	}

	bool gui_mainControl::onClickPersonalShopBtn(const EventArgs & events)
	{
		getParent()->showPersonalShop();
		return true;
	}

	bool gui_mainControl::onClickBuyBulletBtn(const EventArgs & events)
	{
		ClientNetwork::Instance()->getSender()->sendBuyBullet();
		return true;
	}

	void gui_mainControl::gunFireNotBulletHideOtherUi()
	{
		m_reloadBulletBtn->SetVisible(false);
	}

	void gui_mainControl::switchCrossHair(int crossHairType)
	{
		if (m_bChangeCrosshair && m_crossHairMoveSum < m_crossHairMoveRange)
		{
			m_crossHairMoveSum += m_crossHairMoveStepLength * Root::Instance()->getFrameTime() / 10;
			m_crossHairMoveSum = Math::Min(m_crossHairMoveSum, m_crossHairMoveRange);
		}
		else if (!m_bChangeCrosshair && m_crossHairMoveSum > 0.0f)
		{
			m_crossHairMoveSum -= m_crossHairMoveStepLength * Root::Instance()->getFrameTime() / 10;
			m_crossHairMoveSum = Math::Max(m_crossHairMoveSum, 0.0f);
		}
		if (m_crossHairMoveSum >= m_crossHairMoveRange)
		{
			m_bChangeCrosshair = false;
		}

		switch (crossHairType)
		{
		case RIFLE_CROSS_HAIR:
		case MACHINE_CROSS_HAIR:
			m_commonCrossHair->SetVisible(false);
			m_moveTopCrossHair->SetVisible(true);
			m_moveLeftCrossHair->SetVisible(true);
			m_moveBottomCrossHair->SetVisible(true);
			m_moveRightCrossHair->SetVisible(true);
			m_shotgunMoveTopCrossHair->SetVisible(false);
			m_shotgunMoveLeftCrossHair->SetVisible(false);
			m_shotgunMoveBottomCrossHair->SetVisible(false);
			m_shotgunMoveRightCrossHair->SetVisible(false);
			m_moveTopCrossHair->SetYPosition(LORD::UDim(0.0f, m_moveTopCrossHairBeginYPos - m_crossHairMoveSum));
			m_moveLeftCrossHair->SetXPosition(LORD::UDim(0.0f, m_moveLeftCrossHairBeginXPos - m_crossHairMoveSum));
			m_moveBottomCrossHair->SetYPosition(LORD::UDim(0.0f, m_moveBottomCrossHairBeginYPos + m_crossHairMoveSum));
			m_moveRightCrossHair->SetXPosition(LORD::UDim(0.0f, m_moveRightCrossHairBeginXPos + m_crossHairMoveSum));
			break;
		case COMMON_CROSS_HAIR:
			m_commonCrossHair->SetVisible(true);
			m_moveTopCrossHair->SetVisible(false);
			m_moveLeftCrossHair->SetVisible(false);
			m_moveBottomCrossHair->SetVisible(false);
			m_moveRightCrossHair->SetVisible(false);
			m_shotgunMoveTopCrossHair->SetVisible(false);
			m_shotgunMoveLeftCrossHair->SetVisible(false);
			m_shotgunMoveBottomCrossHair->SetVisible(false);
			m_shotgunMoveRightCrossHair->SetVisible(false);
			break;
		case SHOTGUN_CROSS_HAIR:
			m_commonCrossHair->SetVisible(false);
			m_moveTopCrossHair->SetVisible(false);
			m_moveLeftCrossHair->SetVisible(false);
			m_moveBottomCrossHair->SetVisible(false);
			m_moveRightCrossHair->SetVisible(false);
			m_shotgunMoveTopCrossHair->SetVisible(true);
			m_shotgunMoveLeftCrossHair->SetVisible(true);
			m_shotgunMoveBottomCrossHair->SetVisible(true);
			m_shotgunMoveRightCrossHair->SetVisible(true);
			m_shotgunMoveTopCrossHair->SetYPosition(LORD::UDim(0.0f, m_shotgunMoveTopCrossHairBeginYPos - m_crossHairMoveSum));
			m_shotgunMoveLeftCrossHair->SetXPosition(LORD::UDim(0.0f, m_shotgunMoveLeftCrossHairBeginXPos - m_crossHairMoveSum));
			m_shotgunMoveBottomCrossHair->SetYPosition(LORD::UDim(0.0f, m_shotgunMoveBottomCrossHairBeginYPos + m_crossHairMoveSum));
			m_shotgunMoveRightCrossHair->SetXPosition(LORD::UDim(0.0f, m_shotgunMoveRightCrossHairBeginXPos + m_crossHairMoveSum));
			break;
		case NONE_TYPE:
			m_commonCrossHair->SetVisible(false);
			m_moveTopCrossHair->SetVisible(false);
			m_moveLeftCrossHair->SetVisible(false);
			m_moveBottomCrossHair->SetVisible(false);
			m_moveRightCrossHair->SetVisible(false);
			m_shotgunMoveTopCrossHair->SetVisible(false);
			m_shotgunMoveLeftCrossHair->SetVisible(false);
			m_shotgunMoveBottomCrossHair->SetVisible(false);
			m_shotgunMoveRightCrossHair->SetVisible(false);
			break;
		}
	}

	void  gui_mainControl::checkCrossHairIsCanAttack()
	{
		auto pPlayer = Blockman::Instance()->m_pPlayer;
		ItemStackPtr stack = Blockman::Instance()->m_pPlayer->inventory->getCurrentItem();
		if (stack && pPlayer)
		{
			ItemGun* pGun = dynamic_cast<ItemGun*>(stack->getItem());
			if (pGun)
			{
				const GunSetting* pGunSetting = pGun->getGunSetting();
				if (pGunSetting)
				{
					int crossHairType = Blockman::Instance()->m_pPlayer->getCrossHairByGunType(pGunSetting->gunType);
					const float range = pGunSetting->shootRange;
					checkDistance(range, crossHairType, pGunSetting->isAutoShoot);
				}
			}
		}
	}

	void gui_mainControl::checkFlyBtnShow()
	{
		bool sign = Blockman::Instance()->m_pPlayer->capabilities.isFlying;
		if (Blockman::Instance()->m_pPlayer->capabilities.canBeFlying() && !sign)
		{
			showFlyBtn(true);
		}
		else
		{
			showFlyBtn(false);
		}
	}

	void gui_mainControl::switchCrossHairColor(int crossHairType)
	{
		bool isRedAim = UIDisplaySetting::getUIDisplay()->isRedAim;
		if (!isRedAim)
			return;

		switch (crossHairType)
		{
		case RIFLE_CROSS_HAIR:
		case MACHINE_CROSS_HAIR:
			m_moveTopCrossHair->SetDrawColor(Color(1.0f, 0.0f, 0.0f, 1.0f));
			m_moveLeftCrossHair->SetDrawColor(Color(1.0f, 0.0f, 0.0f, 1.0f));
			m_moveBottomCrossHair->SetDrawColor(Color(1.0f, 0.0f, 0.0f, 1.0f));
			m_moveRightCrossHair->SetDrawColor(Color(1.0f, 0.0f, 0.0f, 1.0f));
			break;
		case COMMON_CROSS_HAIR:
			m_commonCrossHair->SetDrawColor(Color(1.0f, 0.0f, 0.0f, 1.0f));
			break;
		case SHOTGUN_CROSS_HAIR:
			m_shotgunMoveTopCrossHair->SetDrawColor(Color(1.0f, 0.0f, 0.0f, 1.0f));
			m_shotgunMoveLeftCrossHair->SetDrawColor(Color(1.0f, 0.0f, 0.0f, 1.0f));
			m_shotgunMoveBottomCrossHair->SetDrawColor(Color(1.0f, 0.0f, 0.0f, 1.0f));
			m_shotgunMoveRightCrossHair->SetDrawColor(Color(1.0f, 0.0f, 0.0f, 1.0f));
			break;
		}
	}
	void gui_mainControl::clearCrossHairColor()
	{
		m_crossHairLayout->SetVisible(true);
		m_commonCrossHair->SetDrawColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
		m_moveTopCrossHair->SetDrawColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
		m_moveLeftCrossHair->SetDrawColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
		m_moveBottomCrossHair->SetDrawColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
		m_moveRightCrossHair->SetDrawColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
		m_shotgunMoveTopCrossHair->SetDrawColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
		m_shotgunMoveLeftCrossHair->SetDrawColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
		m_shotgunMoveBottomCrossHair->SetDrawColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
		m_shotgunMoveRightCrossHair->SetDrawColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
	}

	float gui_mainControl::CheckCollisionCreature(Vector3 beginPos, Vector3 dir, float range, float rangeRatio)
	{
		float min_dis = range;
		auto pPlayer = Blockman::Instance()->m_pPlayer;
		//ENTITY_CLASS_CREATURE
		Entity* tracyEntity = nullptr;
		EntityArr tracyEntityCreature = Blockman::Instance()->getWorld()->getEntitiesWithinRayTracy(pPlayer, ENTITY_CLASS_CREATURE, beginPos, dir, range * rangeRatio);
		for (int i = 0; i < int(tracyEntityCreature.size()); ++i)
		{
			Entity* pEntity = tracyEntityCreature[i];
			float distance = beginPos.distanceTo(pEntity->position);
			if (pEntity->canBePushed() && distance <= min_dis)
			{
				tracyEntity = pEntity;
				min_dis = distance;
			}
		}
		auto pCreature = dynamic_cast<EntityCreature*>(tracyEntity);
		if (pCreature)
		{
			return pCreature->isBase() ? 1000000.0f : min_dis;
		}
		return 1000000.0f;
	}

	float gui_mainControl::CheckCollisionEntityPlayer(Vector3 beginPos, Vector3 dir, float range, float rangeRatio)
	{
		float min_dis = range;
		auto pPlayer = Blockman::Instance()->m_pPlayer;
		//ENTITY_CLASS_PLAYER
		Entity* tracyEntity = nullptr;
		EntityArr tracyEntityPlayers = Blockman::Instance()->getWorld()->getEntitiesWithinRayTracy(pPlayer, ENTITY_CLASS_PLAYER, beginPos, dir, range * rangeRatio);
		for (int i = 0; i < int(tracyEntityPlayers.size()); ++i)
		{
			Entity* pEntity = tracyEntityPlayers[i];
			float distance = beginPos.distanceTo(pEntity->position);
			if (pEntity->canBePushed() && distance <= min_dis && pEntity != pPlayer)
			{
				tracyEntity = pEntity;
				min_dis = distance;
			}
		}
		bool CollisionEntityPlayer = tracyEntity != NULL;
		if (CollisionEntityPlayer)
		{
			bool singleGame = LogicSetting::Instance()->getShowGunEffectWithSingleStatus();
			if (!singleGame && dynamic_cast<EntityPlayer*>(tracyEntity)->getTeamId() == pPlayer->getTeamId())
			{
				return 1000000.0f;
			}
			return min_dis;
		}
		return 1000000.0f;
	}

	float gui_mainControl::CheckCollisionActorNpc(Vector3 beginPos, Vector3 dir, float range, float rangeRatio)
	{
		float min_dis = range;
		auto pPlayer = Blockman::Instance()->m_pPlayer;
		//ENTITY_CLASS_ACTOR_NPC
		Entity* tracyEntity = nullptr;
		EntityArr tracyEntityActorNpc = Blockman::Instance()->getWorld()->getEntitiesWithinRayTracy(pPlayer, ENTITY_CLASS_ACTOR_NPC, beginPos, dir, range * rangeRatio);
		for (int i = 0; i < int(tracyEntityActorNpc.size()); ++i)
		{
			Entity* pEntity = tracyEntityActorNpc[i];
			float distance = beginPos.distanceTo(pEntity->position);
			if (pEntity->canBePushed() && distance <= min_dis)
			{
				tracyEntity = pEntity;
				min_dis = distance;
			}
		}
		if (tracyEntity)
		{
			return min_dis;
		}
		return 1000000.0f;
	}

	float gui_mainControl::CheckCollisionBlock(Vector3 beginPos, Vector3 dir, float range, float rangeRatio)
	{
		//trace the world's block.
		Vector3 endPos = beginPos + dir * range;
		RayTraceResult trace = Blockman::Instance()->getWorld()->rayTraceBlocks(beginPos, endPos, false, true);
		if (trace.result)
		{
			return beginPos.distanceTo(trace.hitVec);
		}
		return 1000000.0f;
	}

	void gui_mainControl::LoadSkillReleaseBtn()
	{
		m_skillOperationWindow = getWindow("Main-Main-Skill-Operate");
		m_skillOperationWindow->SetVisible(false);
		m_skillReleaseBtn = getWindow<GUIButton>("Main-Skill-Release-btn");
		m_skillReleaseBtn->SetText("");
		m_skillReleaseBtn->SetVisible(false);
		m_skillReleaseBtn->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onSkillReleaseBtn, this, std::placeholders::_1));
	}

	bool gui_mainControl::onSkillReleaseBtn(const EventArgs & events)
	{
		castSkill();
		return true;
	}

	bool gui_mainControl::onAutoCastSkill() 
	{
		ItemStackPtr pCurrentItem = Blockman::Instance()->m_pPlayer->inventory->getCurrentItem();
		if (pCurrentItem && pCurrentItem->isAutoCastSkill())
		{
			castSkill();
		}
		return true;
	}

	bool gui_mainControl::castSkill()
	{
		ItemStackPtr pCurrentItem = Blockman::Instance()->m_pPlayer->inventory->getCurrentItem();


		if (pCurrentItem && pCurrentItem->isItemSkill())
		{
			SkillItem* pSkillItem = LogicSetting::Instance()->getSkillItem(pCurrentItem->itemID, Blockman::Instance()->m_pPlayer->getOccupation());
			if (m_ItemSkillCdTime[pCurrentItem->itemID].x <= 0)
			{
				if (pSkillItem)
				{
					if (pCurrentItem->isThrowItemSkill())
					{
						Blockman::Instance()->m_pPlayer->swingItem();
					}
					else {
						UseEatSkill();
					}
					int soltIndex = Blockman::Instance()->m_pPlayer->inventory->currentItemIndex;
					int skillCd = (int)(pSkillItem->ItemSkillCd * 1000);
					m_ItemSkillCdTime[pCurrentItem->itemID] = { skillCd , skillCd };
					GameClient::CGame::Instance()->getNetwork()->getSender()->sendRelaseItemSkill(pCurrentItem->itemID);
					
				}
			}
		}
		return true;
	}

	void gui_mainControl::UseEatSkill()
	{
		ItemStackPtr pCurrentItem = Blockman::Instance()->m_pPlayer->inventory->getCurrentItem();
		if (pCurrentItem)
		{
			float		rotationPitch = Blockman::Instance()->m_pPlayer->rotationPitch;
			float		rotationYaw = Blockman::Instance()->m_pPlayer->prevRotationYaw;
			Vector3		position = Blockman::Instance()->m_pPlayer->position;
			Random& rand = Blockman::Instance()->m_pPlayer->world->m_Rand;
			for (int i = 0; i < 5; ++i)
			{
				String paritcleName = StringUtil::Format("iconcrack_%d", pCurrentItem->getItem()->itemID);
			
				for (int i = 0; i < 5; ++i)
				{
					Vector3 var4((rand.nextFloat() - 0.5f) * 0.1f, Math::UnitRandom() * 0.1f + 0.1f, 0.0f);
					var4.rotateAroundX(-rotationPitch * Math::DEG2RAD);
					var4.rotateAroundY(-rotationYaw * Math::DEG2RAD);
					var4.y += 0.05f;
					Vector3 var5((rand.nextFloat() - 0.5f) * 0.3f, (-rand.nextFloat()) * 0.6f - 0.3f, 0.6f);
					var5.rotateAroundX(-rotationPitch * Math::DEG2RAD);
					var5.rotateAroundY(-rotationYaw * Math::DEG2RAD);
					var5 = var5 + position;
					var5.y += Blockman::Instance()->m_pPlayer->getEyeHeight();
					Blockman::Instance()->m_pPlayer->world->spawnParticle(paritcleName, var5, var4);
				}
			}
		}
	}

	void gui_mainControl::HideSkillUI()
	{
		m_skillOperationWindow->SetVisible(false);
		m_skillReleaseBtn->SetVisible(false);
	}

	void  gui_mainControl::checkHideReleaseSkillBtnUI()
	{
		if (!m_skillOperationWindow || !m_skillReleaseBtn)
		{
			return;
		}
		m_skillOperationWindow->SetVisible(true);
		m_skillReleaseBtn->SetVisible(true);
		if (Blockman::Instance()->m_pPlayer->isOnVehicle())
		{
			HideSkillUI();
			return;
		}

		ItemStackPtr pCurrentItem = Blockman::Instance()->m_pPlayer->inventory->getCurrentItem();
		if (!pCurrentItem)
		{
			HideSkillUI();
			return;
		}
		
		if (!pCurrentItem->isItemSkill() || pCurrentItem->isItemSkillBook() || pCurrentItem->isAutoCastSkill())
		{
			HideSkillUI();
			return;
		}
	}

	void gui_mainControl::updateItemSkillBtnInfo(ui32 timeElapse)
	{
		map<int, Vector2i>::iterator m_it;
		for (int i = 0; i < 9; ++i)
		{
			ItemStackPtr ItemStack = Blockman::Instance()->m_pPlayer->inventory->getItemIdBySlot(i);
			if (ItemStack && ItemStack->isItemSkill())
			{
				if (m_ItemSkillCdTime.find(ItemStack->itemID) == m_ItemSkillCdTime.end())
				{
					m_ItemSkillCdTime[ItemStack->itemID] = { 0, 0 };
				}
			}
		}
		GuiSlotTable* slotTable = dynamic_cast<GuiSlotTable*>(getWindow("Main-VisibleBar-SlotTable"));
		for (m_it = m_ItemSkillCdTime.begin(); m_it != m_ItemSkillCdTime.end(); m_it++)
		{
			if (m_it->first > 0 && m_it->second.x > 0 && m_it->second.y > 0)
			{
				m_it->second.x = m_it->second.x - timeElapse;
			}
			int nSlot = Blockman::Instance()->m_pPlayer->inventory->getItemSlotByHotbat(m_it->first);
			if (m_it->second.x <= 0)
			{
				m_it->second.x = 0;
				if (slotTable)
				{
					if (nSlot >= 0)
						slotTable->setItemSkillCdProgress(nSlot, 0, 0);
				}
			}
			else
			{
				if (slotTable)
				{
					if (nSlot >= 0)
						slotTable->setItemSkillCdProgress(nSlot, m_it->second.y - m_it->second.x, m_it->second.y);
				}
			}
		}		
	}

	void gui_mainControl::refreshItemSkillShowUi()
	{
		int soltIndex = Blockman::Instance()->m_pPlayer->inventory->currentItemIndex;
		GuiSlotTable* slotTable = dynamic_cast<GuiSlotTable*>(getWindow("Main-VisibleBar-SlotTable"));
		if (slotTable)
			slotTable->setItemSkillCdProgress(soltIndex, 0, 0);
	}

	void gui_mainControl::checkSkillIsHaveAttackEntity()
	{
		auto pPlayer = Blockman::Instance()->m_pPlayer;
		if (pPlayer)
		{
			ItemStackPtr stack = Blockman::Instance()->m_pPlayer->inventory->getCurrentItem();
			if (stack && stack->isThrowItemSkill() && stack->isShowCrossHair())
			{
				SkillItem* pSkillItem = LogicSetting::Instance()->getSkillItem(stack->itemID, Blockman::Instance()->m_pPlayer->getOccupation());
				if (pSkillItem)
				{
					int crossHairType = RIFLE_CROSS_HAIR;
					const float range = pSkillItem->ItemSkillDistace;
					checkDistance(range, crossHairType, false);
				}
			}
		}
	}

	void gui_mainControl::updateMaskInfo(ui32 nTimeElapse)
	{
		if (m_showMaskTime > 0)
		{
			m_showMaskTime = m_showMaskTime - nTimeElapse;
		}

		if (m_darkMask)
		{
			m_darkMask->SetVisible(m_showMaskTime > 0);
		}

		auto player = Blockman::Instance()->m_pPlayer;

		if (m_frozenMask && player)
		{
			m_frozenMask->SetVisible(player->isFrozen());
		}

		if (m_hurtMask && player)
		{
			m_hurtMask->SetVisible(player->isHurted());
		}
	}

	void gui_mainControl::checkDistance(float Distace, int crossHairType, bool isAutoShoot)
	{
		auto pPlayer = Blockman::Instance()->m_pPlayer;
		if (pPlayer)
		{
			Vector3 beginPos = pPlayer->position;
			beginPos.y += pPlayer->getEyeHeight();
			beginPos += pPlayer->m_thirdPersonCameraOffset;
			float yaw = pPlayer->rotationYaw;
			float pitch = pPlayer->rotationPitch;
			Vector3 dir;
			dir.x = -float(Math::Sin(double(yaw) * Math::DEG2RAD) * Math::Cos(double(pitch) * Math::DEG2RAD));
			dir.z = float(Math::Cos(double(yaw) * Math::DEG2RAD) * Math::Cos(double(pitch) * Math::DEG2RAD));
			dir.y = -float(Math::Sin(double(pitch) * Math::DEG2RAD));
			Vector3 endPos = beginPos + dir * Distace;
			float rangeRatio = 1.0f;
			
			float BlockDistance = CheckCollisionBlock(beginPos, dir, Distace, rangeRatio);
			float ActorNpcDistance = CheckCollisionActorNpc(beginPos, dir, Distace, rangeRatio);
			float CreatureDistance = CheckCollisionCreature(beginPos, dir, Distace, rangeRatio);
			float PlayerDistance = CheckCollisionEntityPlayer(beginPos, dir, Distace, rangeRatio);
			
			bool CollisionBlock = BlockDistance < 999999.0f;
			bool CollisionActorNpc = ActorNpcDistance < 999999.0f;
			bool CollisionCreature = CreatureDistance < 999999.0f;
			bool CollisionEntityPlayer = PlayerDistance < 999999.0f;

			float MinDistance = Math::Min(BlockDistance, ActorNpcDistance);
			MinDistance = Math::Min(MinDistance, CreatureDistance);
			MinDistance = Math::Min(MinDistance, PlayerDistance);

			if (CollisionBlock)
			{
				bool isRedAimBlock = UIDisplaySetting::getUIDisplay()->isRedAimBlock;
				if (isRedAimBlock)
				{
					switchCrossHairColor(crossHairType);
					switchCrossHair(crossHairType);
				}
				else
				{
					clearCrossHairColor();
					switchCrossHair(crossHairType);
				}
				if (MinDistance == BlockDistance)
				{
					return;
				}
			}
			
			if (CollisionActorNpc || CollisionCreature || CollisionEntityPlayer)
			{
				switchCrossHairColor(crossHairType);
				switchCrossHair(crossHairType);
			}
			else
			{
				clearCrossHairColor();
				switchCrossHair(crossHairType);
			}

			bool enableAutoShoot = LogicSetting::Instance()->getAutoShootEnable();
			if (enableAutoShoot && isAutoShoot && CollisionEntityPlayer && MinDistance == PlayerDistance)
			{
				tryFireGun(true);
			}
		}
	}

	bool gui_mainControl::setGameStatus(const int status)
	{
		m_gameStatus = status == 1 ? GRME_ROUND_STATUS_START : GRME_ROUND_STATUS_WAIT;
		if (m_gameStatus == GRME_ROUND_STATUS_WAIT)
		{
			ForceAutoRun();
			m_ItemSkillCdTime.clear();
			updateItemSkillBtnInfo(0);
		}
		return true;
	}

	bool gui_mainControl::LoadThrowPotUi()
	{
		m_throwPotLayout = getWindow<GUILayout>("Main-throwpot-Controls");
		m_autoRunBtnFalseStatus = getWindow("Main-AutoRun-False-Btn");
		m_autoRunBtnTrueStatus = getWindow("Main-AutoRun-True-Btn");
		m_attackBtn = getWindow<GUIButton>("Main-Attack-Btn");

		m_throwPotLayout->SetVisible(GameClient::CGame::Instance()->GetGameType() == ClientGameType::Throw_Pot);
		m_autoRunBtnFalseStatus->subscribeEvent(EventWindowClick, std::bind(&gui_mainControl::onAutoRunBtn, this, std::placeholders::_1));
		m_autoRunBtnTrueStatus->subscribeEvent(EventWindowClick, std::bind(&gui_mainControl::onCloseAutoRunBtn, this, std::placeholders::_1));
		m_attackBtn->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onAttackBtn, this, std::placeholders::_1));
		m_autoRunBtnTrueStatus->SetVisible(false);
		m_autoRunBtnFalseStatus->SetVisible(true);
		return true;
	}

	bool gui_mainControl::onAutoRunBtn(const EventArgs & events)
	{
		//LordLogInfo("onAutoRunClick");
		m_autoRunBtnTrueStatus->SetVisible(true);
		m_autoRunBtnFalseStatus->SetVisible(false);
		Blockman::Instance()->m_pPlayer->setSprinting(true);
		Blockman::Instance()->m_gameSettings->setAutoRunStatus(true);
		return true;
	}

	bool gui_mainControl::onCloseAutoRunBtn(const EventArgs & events)
	{
		//LordLogInfo("onCloseAutoRunClick");
		m_autoRunBtnTrueStatus->SetVisible(false);
		m_autoRunBtnFalseStatus->SetVisible(true);
		Blockman::Instance()->m_pPlayer->setSprinting(false);
		Blockman::Instance()->m_gameSettings->setAutoRunStatus(false);
		return true;
	}

	bool gui_mainControl::onAttackBtn(const EventArgs & events)
	{
		if (!Blockman::Instance()->m_pPlayer)
			return false;

		ItemStackPtr pCurrentItem = Blockman::Instance()->m_pPlayer->inventory->getCurrentItem();
		if (pCurrentItem && pCurrentItem->isItemSkill())
		{
			GameClient::CGame::Instance()->getNetwork()->getSender()->sendRelaseItemSkill(pCurrentItem->itemID);
		}
		else
		{
			Blockman::Instance()->ManualAttack();
		}
		return true;
	}

	bool gui_mainControl::syncShowMaskTime(int MilliSecond)
	{
		m_showMaskTime = MilliSecond;
		return true;
	}

	void gui_mainControl::ForceAutoRun()
	{
		if (Blockman::Instance()->m_gameSettings->getAutoRunStatus())
		{
			m_autoRunBtnTrueStatus->SetVisible(false);
			m_autoRunBtnFalseStatus->SetVisible(true);
			Blockman::Instance()->m_pPlayer->setSprinting(false);
			Blockman::Instance()->m_gameSettings->setAutoRunStatus(false);
		}
	}

	void gui_mainControl::updateEndingAnimation()
	{
		float frameTime = (float)Root::Instance()->getFrameTime();

		if (m_endingAnimationRotatingTime < 1000.0f)
		{
			m_endingAnimationRotatingTime += frameTime;
		}
		else if (m_endingAnimationRotatingTime < 2000.0f)// wait for rotating
		{
			if (!m_beginRotating)
			{
				m_beginRotating = true;
				Blockman::Instance()->m_entityRender->setEndingAnimationStatus(true);
				Blockman::Instance()->m_entityRender->setEndingAnimationRotationAngle(0.0f);
			}
			m_endingAnimationRotatingTime += frameTime;
		}
		else
		{
			if (!m_beginPlayEndingAnimation)
			{
				m_beginPlayEndingAnimation = true;
				m_endingStatus->SetVisible(true);
			}

			if (m_isZoomInEndingImg && m_endingAnimationScale < 1.2f)
			{
				m_endingAnimationScale += frameTime * 0.005f;
			}
			else if (!m_isZoomInEndingImg && m_endingAnimationScale > 1.0f)
			{
				m_endingAnimationScale -= frameTime * 0.005f;
			}

			if (m_isZoomInEndingImg && m_endingAnimationScale >= 1.2f)
			{
				m_isZoomInEndingImg = false;
				m_endingAnimationScale = 1.2f;
			}
			else if (!m_isZoomInEndingImg && m_endingAnimationScale < 1.0f)
			{
				m_endingAnimationScale = 1.0f;
			}
			else if (!m_isZoomInEndingImg && m_endingAnimationScale == 1.0f)
			{
				m_endingImgDurationTime += frameTime;
			}

			if (m_endingImgDurationTime > 3000.0f)
			{
				m_playEndingAnimation = false;
				m_endingStatus->SetVisible(false);
				Blockman::Instance()->setPersonView(0);
				getParent()->updateSwitchSerpece();
				Blockman::Instance()->m_entityRender->setEndingAnimationStatus(false);
				Blockman::Instance()->m_playerControl->setDisableMovement(false);
				ReceiveFinalSummaryEvent::emit(m_gameResult, m_isNextServer);
			}
			else
			{
				m_endingStatus->SetScale(Vector3(m_endingAnimationScale, m_endingAnimationScale, m_endingAnimationScale));
			}
		}
	}

	bool gui_mainControl::onClickFishingBtn(const EventArgs &)
	{
		if (!Blockman::Instance()->m_pPlayer->inventory)
			return false;

		ItemStackPtr pCurrentItem = Blockman::Instance()->m_pPlayer->inventory->getCurrentItem();

		if (pCurrentItem)
		{
			if (pCurrentItem->itemID == Item::fishingRod->itemID)
			{
				pCurrentItem->useItemRightClick(Blockman::Instance()->m_pWorld, Blockman::Instance()->m_pPlayer);
				return true;
			}
		}

		return false;
	}

	void gui_mainControl::CheckFishingBtn()
	{
		if (!Blockman::Instance() || !Blockman::Instance()->m_pPlayer || !Blockman::Instance()->m_pPlayer->inventory)
		{
			return;
		}

		if (Blockman::Instance()->m_pPlayer->isOnVehicle())
		{
			m_btnFishing->SetVisible(false);
			return;
		}

		ItemStackPtr pCurrentItem = Blockman::Instance()->m_pPlayer->inventory->getCurrentItem();
		if (!pCurrentItem)
		{
			m_btnFishing->SetVisible(false);
			return;
		}

		ItemFishingRod* pFishingRod = dynamic_cast<ItemFishingRod*>(pCurrentItem->getItem());
		if (!pFishingRod)
		{
			m_btnFishing->SetVisible(false);
			return;
		}
		m_btnFishing->SetVisible(true);
	}

	bool gui_mainControl::onClickCannonBtn(const EventArgs &)
	{
		EntityActorCannonClient* pCannon = dynamic_cast<EntityActorCannonClient*>(Blockman::Instance()->getWorld()->getEntity(m_cannonEntityId));
		if (!pCannon)
		{
			return false;
		}

		pCannon->m_hasBeenLaunched = true;

		ClientNetwork::Instance()->getSender()->sendUseCannon(m_cannonEntityId);
		return true;
	}

	bool gui_mainControl::showCannonBtn(bool isShow, int cannonEntityId)
	{
		m_btnCannon->SetVisible(isShow);
		m_cannonEntityId = cannonEntityId;
		return true;
	}

	bool gui_mainControl::hideBirdActivity()
	{
		m_birdActivity->SetVisible(false);
		return true;
	}

	void gui_mainControl::LoadRanchExUI()
	{
		getWindow<GUILayout>("Main-RanchExItemLayout")->SetVisible(false);
		getWindow<GUIButton>("MainControl-Show-RanchItem")->SetVisible(GameClient::CGame::Instance()->GetGameType() == ClientGameType::RanchersExplore);
		getWindow<GUIStaticImage>("Main-Show-RanchItem-RedPoint")->SetVisible(false);
		getWindow<GUIButton>("MainControl-Show-RanchItem")->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickShowExItemBtn, this, std::placeholders::_1));

		for (int i = 0; i < MAX_RANCHEX_ITEM_COUNT; ++i)
		{
			m_RanchExItemImg[i] = getWindow<GUIStaticImage>(StringUtil::Format("Main-RanchExItemImg%d", i + 1).c_str());
			m_RanchExItemTxt[i] = getWindow<GUIStaticText>(StringUtil::Format("Main-RanchExItemTxt%d", i + 1).c_str());
			m_RanchExItemImg[i]->SetVisible(false);
		}
	}

	void gui_mainControl::checkOpenFlyingShow()
	{
		m_btnOpenFlying->SetVisible(UICommon::checkOpenFlying());
	}

	bool gui_mainControl::onClickOpenFlying(const EventArgs & args)
	{
		if (Blockman::Instance()->m_pPlayer)
		{
			bool sign = Blockman::Instance()->m_pPlayer->capabilities.isFlying;
			ClientNetwork::Instance()->getSender()->sendSetFlying(!sign);
		}
		return true;
	}

	bool gui_mainControl::onPlaceBuildingClick(const EventArgs & args)
	{
		if (Blockman::Instance())
		{
			Blockman::Instance()->ManualAttack();
		}
		return true;
	}
	
	void gui_mainControl::updateHideAndSeekYawChange(ui32 nTimeElapse)
	{
		if (!m_hideandseekYawChange) return;

		ActorObject* actor = EntityRenderManager::Instance()->getEntityActor(Blockman::Instance()->renderViewEntity);
		if (!actor)
			return;

		float offset = m_hideandseekYawLeft ? 1.5f : -1.5f;
		Quaternion curQ = actor->GetOrientation();
		Real radian = curQ.getRadian() + offset * Math::DEG2RAD;
		// LordLogInfo("updateHideAndSeekYawChange offset: %f", offset);

		// 0 or 360
		if (radian > 6.2)
		{
			radian = 0.0f;
		}
		if (radian < 0.0)
		{
			radian = 6.2f;
		}
		Blockman::Instance()->m_pPlayer->spYawRadian = radian;
		ClientNetwork::Instance()->getSender()->sendSetSpYaw(Blockman::Instance()->m_pPlayer->spYaw, Blockman::Instance()->m_pPlayer->spYawRadian);
	}

	bool gui_mainControl::onBirdAddScore(i32 score, i32 scoreType)
	{
		if (isShown())
		{
			static int viewCount = 0;
			String viewName = StringUtil::Format("Main-BirdAddScore-%d", viewCount++);
			GUIStaticText* itemView = (GUIStaticText*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_TEXT, viewName.c_str());
			itemView->SetText(StringUtil::Format(scoreType > 0 ? "+%d" : "-%d", score).c_str());
			itemView->SetSize(UVector2(UDim(0, 50.f), UDim(0, 25.f)));
			itemView->SetTextHorzAlign(HA_CENTRE);
			itemView->SetTextVertAlign(VA_CENTRE);
			itemView->SetTextBoader(Color::BLACK);
			switch (scoreType)
			{
			case -1:
				itemView->SetTextColor(Color::RED);
				break;
			case 1:
				itemView->SetTextColor(Color::RED);
				break;
			case 2:
				itemView->SetTextColor(Color::YELLOW);
				break;
			case 3:
				itemView->SetTextColor(Color::BLUE);
				break;
			case 4:
				itemView->SetTextColor(Color::GREEN);
				break;
			}
			itemView->SetFont(GUIFontManager::Instance()->GetFont("HT20"));
			itemView->SetHorizontalAlignment(HA_CENTRE);
			m_birdScoreViews.push_back(BirdScoreView(itemView, scoreType));
			m_rootWindow->AddChildWindow(itemView);
		}
		return true;
	}

	void gui_mainControl::onBirdDataUpdate(ui32 nTimeElapse)
	{

		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::BirdSimulator)
		{
			bool hasTip = false;
			int index = 0;
			float widthOffset = 0;

			for (auto& item : m_birdScoreViews)
			{
				if (item.timeLeft <= 0 && item.itemView)
				{
					m_rootWindow->RemoveChildWindow(item.itemView);
					GUIWindowManager::Instance()->DestroyGUIWindow(item.itemView);
					item.itemView = nullptr;
				}
				else
				{
					if (widthOffset < 200.f)
					{
						widthOffset += 15.f;
					}
				}
			}

			for (auto& item : m_birdScoreViews)
			{
				if (item.itemView)
				{
					item.timeLeft -= nTimeElapse;
					if (item.timeLeft > 0)
					{
						float scale = (1000 - item.timeLeft) / 1000.f;
						float h = 0.5f - 0.25f * scale;
						item.itemView->SetTextScale(0.5f + scale);
						if (item.type == -1)
						{
							item.itemView->SetPosition(UVector2(UDim(0, 0), UDim(h, 0)));
						}
						else
						{
							item.itemView->SetPosition(UVector2(UDim(0, 30.f *  (index % 13)- widthOffset), UDim(h, index % 2 * 50.f)));
						}
					
						index++;
						hasTip = true;
					}
					else
					{
						item.itemView->SetVisible(false);
					}
				}
			}

			if (!hasTip)
			{
				for (auto& item : m_birdScoreViews)
				{
					if (item.itemView)
					{
						m_rootWindow->RemoveChildWindow(item.itemView);
						GUIWindowManager::Instance()->DestroyGUIWindow(item.itemView);
						item.itemView = nullptr;
					}
				}
				m_birdScoreViews.clear();
			}

			getWindow("Main-BirdTask-RedDot")->SetVisible(RedDotManager::checkBirdTask());
		}
	}
	
	bool gui_mainControl::onClickBirdPackBtn(const EventArgs & events)
	{
		RootGuiLayout::Instance()->showBirdPackAndFuse(true);
		return true;
	}

	bool gui_mainControl::onClickBirdFuseBtn(const EventArgs & events)
	{
		RootGuiLayout::Instance()->showBirdPackAndFuse(false);
		return true;
	}

	bool gui_mainControl::onClickBirdPersonalShopBtn(const EventArgs & events)
	{
		RootGuiLayout::Instance()->showBirdPersonShop();
		return true;
	}

	bool gui_mainControl::onClickBirdAtlasBtn(const EventArgs & events)
	{
		RootGuiLayout::Instance()->showBirdAtlas();
		return true;
	}

	bool gui_mainControl::onClickBirdActivityBtn(const EventArgs & events)
	{
		RootGuiLayout::Instance()->showBirdActivity();
		return true;
	}

	bool gui_mainControl::onHideBirdTaskClick(const EventArgs & events)
	{
		m_isShowBirdTask = false;
		m_birdTaskViewLoadTime = 300;
		return true;
	}

	bool gui_mainControl::onShowBirdTaskClick(const EventArgs & events)
	{
		m_isShowBirdTask = true;
		m_birdTaskViewLoadTime = 300;
		onBirdTaskChange();
		return true;
	}

	bool gui_mainControl::onBirdTaskChange()
	{
		if (!UICommon::checkBirdParam() || !m_isShowBirdTask)
		{
			return false;
		}

		const auto& tasks = Blockman::Instance()->m_pPlayer->m_birdSimulator->getTasks();
		float  oldOffset = tasks.size() > 0 ? m_birdTaskList->GetScrollOffset() : 0;
		m_birdTaskList->ClearAllItem();
		static int taskCount = 0;
		for (auto& task : tasks)
		{
			String iterLayout = StringUtil::Format("Main-Bird-Task-List-Item-%d", taskCount++).c_str();
			GuiBirdTaskItem* titleItem = (GuiBirdTaskItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_BIRD_TASK_ITEM, iterLayout.c_str());
			titleItem->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(1.f, 0), UDim(0.0f, 28.f));
			titleItem->setTitle(getString(task.taskName).c_str());
			m_birdTaskList->AddItem(titleItem, false);
			for (auto& item : task.contents)
			{
				String itemLayout = StringUtil::Format("Main-Bird-Task-List-Item-%d", taskCount++).c_str();
				GuiBirdTaskItem* infoItem = (GuiBirdTaskItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_BIRD_TASK_ITEM, itemLayout.c_str());
				infoItem->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(1.f, 0), UDim(0.0f, 46.f));
				infoItem->setTask(task.taskId, item.id);
				m_birdTaskList->AddItem(infoItem, false);
			}
		}
		m_birdTaskList->SetScrollOffset(oldOffset);
		return true;
	}

	void gui_mainControl::birdTaskUpdate(i32 nTimeElapse)
	{
		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::BirdSimulator)
		{
			if (m_birdTaskViewLoadTime > 0)
			{
				m_birdTaskViewLoadTime -= nTimeElapse;
				m_birdTaskViewLoadTime = m_birdTaskViewLoadTime > 0 ? m_birdTaskViewLoadTime : 0;
			}

			if (m_isShowBirdTask)
			{
				float with = 258.f * (1.f - m_birdTaskViewLoadTime / 300.f);
				m_birdTaskLayout->SetWidth(UDim(0, with));
				m_birdTaskLayout->SetVisible(true);
				m_btnHideTask->SetVisible(true);
				m_btnShowTask->SetVisible(false);
			}
			else
			{
				float with = 258.f * m_birdTaskViewLoadTime / 300.f;
				m_birdTaskLayout->SetWidth(UDim(0, with));
				m_birdTaskLayout->SetVisible(m_birdTaskViewLoadTime > 0);
				m_btnHideTask->SetVisible(m_birdTaskViewLoadTime > 0);
				m_btnShowTask->SetVisible(true);
			}
		}
		else
		{
			m_birdTaskLayout->SetVisible(false);
			m_btnHideTask->SetVisible(false);
			m_btnShowTask->SetVisible(false);
		}
	}

}
