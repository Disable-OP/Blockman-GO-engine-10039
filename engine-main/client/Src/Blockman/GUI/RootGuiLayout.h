#ifndef __ROOT_GUI_LAYOUT_HEADER__
#define __ROOT_GUI_LAYOUT_HEADER__

#include <functional>
#include "Core.h"
#include "GUI/gui_layout.h"
#include "Util/ClientEvents.h"
#include "UI/GUIStaticText.h"
#include "Inventory/IInventory.h"
#include "Util/SubscriptionGuard.h"
#include "BattleSummary.h"
#include "Structs/ChoiceRole.h"

namespace GameClient
{
	struct ResultEntry;
}

namespace BLOCKMAN
{
	class RootGuiLayout : public gui_layout, public Singleton<RootGuiLayout>
	{
	private:
		struct OpenChestArgs {
			int delayTickToOpenChest;
			IInventory* inventory;
			Vector3i blockPos;
			int face;
			Vector3 hisPos;
		};
	public:
		enum class LayoutId
		{
			MAIN,
			PLAYER_INVENTORY,
			LOADING_PAGE,
			PLAYER_INFO,
			GAME_TIP_DIALOG,
			CHEST,
			CHAT,
			MENU,
			TOOL_BAR,
			DEAD_SUMMARY,
			FINAL_SUMMARY,
			SHOP,
			SUMMARY_POINT,
			COUNTDOWN,
			APP_SHOP,
			SUMMARY_TIP,
			JAIL_BREAK_ROLE_CHOICE,
			TIP_DIALOG,
			BUY_GOODS_TIP,
			RANK,
			BLOCK_LOADING,
			MINI_MAP,
			MANOR,
			UPGRADE_MANOR,
			PERSONAL_SHOP,
			REAL_TIME_RANK,
			BUILD_WAR_GRADE,
			BUILD_WAR_GUESS,
			BUILD_WAR_GUESS_RESULT,
			MULTI_TIP_DIALOG,
			OUTPUT_CONFIG,
			SUPER_SHOP,
			KILL_PANEL,
			CAR_PROGRESS_TABLE,
			SUPERPRO_PSPANEL,
			MULTI_TIP_INTERACTION_DIALOG,
			RANCH,
			RANCH_TIP,
			RANCH_TIME_TIP,
			RANCH_ORDER,
			RANCH_UPGRAGE,
			RANCH_FACTORY_BUILD,
			RANCH_FARM_BUILD,
			RANCH_GAIN_TIP,
			RANCH_RANK,
			RANCH_PROSPERITY_RANK,
			OCCUPATION_UNLOCK,
			ENCHANTMENT_PANEL,
			TIGER_LOTTERY,
			HIDEANDSEEKHALL_RESULT,
			PIXEL_GUNHALL_MODE_SELECT,
			GUN_STORE,
			PIXEL_GUN_1V1,
			CHEST_LOTTERY,
			PIXEL_GUN_REVIVE,
			PIXEL_GUNHALL_ARMOR_UPGRADE,
			PIXEL_GUN_RESULT,
			SEASON_RANK,
			SEASON_REWARD,
			LOTTERY_CHEST_DETAIL,
			PIXEL_GUN_GAME_LVUP,
			BIRD_PACKANDFUSE,
			BIRD_PERSONAL_SHOP,
			BIRD_STORE,
			BIRD_LOTTERY,
			BIRD_TIP,
			BIRD_TASK_TIP,
			BIRD_ATLAS,
			BIRD_GAIN_TIP,
			BIRD_ACTIVITY,
		};
	private:
		SubscriptionGuard m_subscriptionGuard;

		static const LayoutId ALL_LAYOUT_ID[];
		map<LayoutId, gui_layout*>::type m_layouts;
		map<LayoutId, bool>::type m_layoutShown;
		void hideAllChildren();
		bool openCraftingTable();
		bool handlePlayerDeath(ui64 playerRuntimeId);
		bool onBackKeyDown();
		bool openChest(IInventory* inventory, const Vector3i & blockPos, int face, const Vector3 & hisPos);

	public:
		~RootGuiLayout();
		
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		void addLayout(gui_layout* layout)
		{
			gui_layout::addLayout(m_rootWindow, layout);
		}
		void showGameGui();
		void showPlayerInventory();
		void showMainControl();
		void showGameTipDialogManage(bool isShow);
		void showChestManage(bool idShow);
		void showChatGui();
		void createItemNameUi();
		bool showItemNameUi(ItemStackPtr itemStack);
		void showMenu();
		void showLoadPage();
		void showDeadSummaryGui();
		void showFinalSummaryGui();
		bool showShop(EntityMerchant* merchant);
		bool shopUpdate(EntityMerchant* merchant);
		void showSummarPoint(const LORD::UVector2& pos, const std::map<int, int>& pointMap);
		void showSummaryRewardTip(const LORD::UVector2& pos, int total);
		void showCountdown(i8 second);
		void hideCountdown();
		void showGameCloseTipDialog();
		void showAppShop();
		bool showPersonalShop();
		bool showSuperShop();
		int  getSelfTeamId();
		void changePlayerMaxHealth(float health);
		bool showJailBreakRoleChoice(ChoiceRole  role);
		bool showTipDialog();
		bool hideTipDialog();
		bool showBuyGoodsTip(bool bShow);
		bool showRank(const String& rankInfo, int EntityId);
		bool hideRank();
		bool showBlockLoading(bool bShow);
		void switchSerpece();
		void updateSwitchSerpece();
		int getViewNum();
		void showMiniMap(bool isShow) { m_isShowMiniMap = isShow; }
		void showParachuteBtn(bool isNeedOpen);
		void showBuildWarBlockBtn(bool isNeedOpen);
		void showDeadSummaryGui(const String& gameResult, bool isNextServer);
		bool isPlayerInventoryShown() { return m_layoutShown[LayoutId::PLAYER_INVENTORY]; }
		void showHideAndSeekBtn(bool isShowChangeActoBtn, bool isShowLockCameraBtn, bool isShowOthenBtn);
		bool isChestInventoryShown() { return m_layoutShown[LayoutId::CHEST]; }
		bool isLoading() { return m_layoutShown[LayoutId::LOADING_PAGE]; }
		bool showManor();
		void showUpgradeManor();
		void hidePlayerOperation();
		bool updateRealTimeRankData(String result);
		bool canPersonalShop();
		bool isShowRealTimeRankStatus();
		void isColseRealTimeRank(bool status);
		void showRealTimeRank();
		void hideRealTimeRank();
		void showBuildWarGrade(bool show);
		void showBuildWarGuessResult(bool show);
		void showBuildWarGuessUi(bool show);
		bool isBuildWarGradeShown() { return m_layoutShown[LayoutId::BUILD_WAR_GRADE]; }
		bool isMainControlShown() { return m_layoutShown[LayoutId::MAIN]; }
		bool isAppShopShown() { return m_layoutShown[LayoutId::APP_SHOP]; }
		bool isMenuShown() { return m_layoutShown[LayoutId::MENU]; }
		bool isChatShown() { return m_layoutShown[LayoutId::CHAT]; }
		bool isFinalSummaryShown() { return m_layoutShown[LayoutId::FINAL_SUMMARY]; }
		bool updateShopUnlockedState();
		void showMultiTipDialog();
		void hideMultiTipDialog();
		void showMultiTipInteractionDialog();
		void hideMultiTipInteractionDialog();
		void showRanch();
		void showRanchTip();
		void hideRanchTip();
		void showRanchOrder();
		void showRanchUpgrade();
		void hideRanchUpgrade();
		void showRanchBuildFactory();
		void showRanchBuildFarm();
		void setRanchTimeTipVisible(bool isShow);
		void setRanchGainTipVisible(bool isShow);
		void setRanchRankVisible(bool isShow);
		void setRanchProsperityRankVisible(bool isShow);
		void setOccupationUnlockVisible(bool isShow);
		void checkBackgroundMusic();
		bool setEnchantmentShow(bool isShow);
		void showTigerLottery();
		void hideTigerLottery();
		void showHideAndSeekHallResult();
		void showLackOfMoney(i32 difference, CurrencyType type);
		bool showBirdPackAndFuse(bool isPack);
		bool isShowPack();
		void showBirdPersonShop();
		void showBirdLottery();
		bool isShowBirdLottery();
		void showBirdStore();
		void setBirdTipVisible(bool isShow);
		bool isShowBirdStore();
		bool isBirdViewShow();
		
		void setBirdTaskTipVisible(bool isShow);
		bool isShowBirdTaskTip();
		void showBirdAtlas();
		void setBirdGainTipVisible(bool isShow);
		void showBirdActivity();
		void setPixelGunHallModeSelectShow(bool isShow);
		void setPixelGun1V1Show(bool isShow);
		void setPixelGunReviveShow(bool isShow);
		void setPixelGunResultShow(bool isShow);
		bool showGunStore();
		bool showChestLottery();
		void showPixelGunHallArmorUpgrade(bool isShow);
		bool showSeasonRank();
		bool showSeasonReward();
		bool showLotteryChestDetail(int type);
		void hideLotteryChestDetail();
		void showPixelGunGameLvUp(bool show);
		bool isPixelGunResultOpen();

	private:
		bool m_isLoad = false;
		bool m_isChoiceRole = false;
		bool m_isShowItemName = false;
		bool m_isShowMiniMap = false;
		ui32 m_showItemNameTime = 0;
		OpenChestArgs m_openChestArgs;
		GUIStaticText* m_stItemName = nullptr;
		ChoiceRole m_choiceRole;
	};
}

#endif // !__ROOT_GUI_LAYOUT_HEADER__

