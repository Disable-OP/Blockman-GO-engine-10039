#include "RootGuiLayout.h"

#include "cEntity/EntityPlayerSPMP.h"
#include "cWorld/Blockman.h"
#include "GUI/gui_mainControl.h"
#include "GUI/gui_playerInventoryControl.h"
#include "GUI/gui_loadingPage.h"
#include "GUI/gui_playerInfo.h"
#include "GUI/gui_gameTipDialog.h"
#include "GUI/gui_chest.h" 
#include "GUI/gui_chat.h"
#include "GUI/gui_menu.h"
#include "GUI/gui_toolBar.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Inventory/IInventory.h"
#include "Inventory/Container.h"
#include "Util/ClientEvents.h"
#include "Util/LanguageManager.h"
#include "Network/ClientNetwork.h"
#include "Entity/Enchantment.h"
#include "GUI/gui_deadSummary.h"
#include "GUI/gui_finalSummary.h"
#include "GUI/gui_shop.h"
#include "GUI/gui_summaryPoint.h"
#include "GUI/gui_countdown.h"
#include "GUI/gui_appShop.h"
#include "GUI/gui_summaryTip.h"
#include "GUI/gui_jailBreakRoleChoice.h"
#include "GUI/gui_tipDialog.h"
#include "GUI/gui_buyGoodsTip.h"
#include "GUI/gui_rank.h"
#include "GUI/gui_blockLoading.h"
#include "GUI/gui_miniMap.h"
#include "GUI/gui_manor.h"
#include "GUI/gui_manorUpgrade.h"
#include "GUI/gui_personalShop.h"
#include "GUI/gui_realTimeRank.h"
#include "GUI/gui_multiTipDialog.h"
#include "GUI/gui_buildwarGrade.h"
#include "GUI/gui_buildwarGuess.h"
#include "GUI/gui_buildwarGuessResult.h"
#include "GUI/gui_outputConfig.h"
#include "GUI/gui_superShop.h"
#include "GUI/gui_killPanel.h"
#include "GUI/gui_carProgressTable.h"
#include "GUI/gui_multiTipInteractionDialog.h"
#include "GUI/gui_ranch.h"
#include "GUI/gui_ranchUpgrade.h"
#include "GUI/gui_ranchOrder.h"
#include "GUI/gui_ranchTip.h"
#include "GUI/gui_ranchTimeTip.h"
#include "GUI/gui_ranchGainTip.h"
#include "GUI/gui_ranchRank.h"
#include "GUI/gui_ranchProsperityRank.h"
#include "GUI/gui_ranchBuildFactory.h"
#include "GUI/gui_ranchBuildFarm.h"
#include "GUI/gui_occupationUnlock.h"
#include "GUI/gui_enchantmentPanel.h"
#include "GUI/gui_tigerLottery.h"
#include "GUI/gui_HideAndSeekHallResult.h"
#include "GUI/gui_pixelGunHallModelSelect.h"
#include "GUI/gui_gunStore.h"
#include "GUI/gui_pixelgun1v1.h"
#include "GUI/gui_chestLottery.h"
#include "GUI/gui_lotteryChestDetail.h"
#include "GUI/gui_pixelgunrevive.h"
#include "GUI/gui_pixelGunHallArmorUpgrade.h"
#include "GUI/gui_pixelgungameresult.h"
#include "GUI/gui_seasonRank.h"
#include "GUI/gui_seasonReward.h"
#include "GUI/gui_pixelgungamelvup.h"
#include "GUI/gui_birdPackAndFuse.h"
#include "GUI/gui_birdPersonalShop.h"
#include "GUI/gui_birdStore.h"
#include "GUI/gui_birdLottery.h"
#include "GUI/gui_birdTip.h"
#include "GUI/gui_birdTaskTip.h"
#include "GUI/gui_birdAtlas.h"
#include "GUI/gui_birdGainTip.h"
#include "GUI/gui_birdActivity.h"
#include "game.h"

namespace BLOCKMAN
{
	const RootGuiLayout::LayoutId RootGuiLayout::ALL_LAYOUT_ID[] = {
		LayoutId::MAIN,
		LayoutId::PLAYER_INVENTORY,
		LayoutId::LOADING_PAGE,
		LayoutId::PLAYER_INFO,
		LayoutId::GAME_TIP_DIALOG,
		LayoutId::CHEST,
		LayoutId::CHAT,
		LayoutId::MENU,
		LayoutId::TOOL_BAR,
		LayoutId::DEAD_SUMMARY,
		LayoutId::FINAL_SUMMARY,
		LayoutId::SHOP,
		LayoutId::SUMMARY_POINT,
		LayoutId::COUNTDOWN,
		LayoutId::APP_SHOP,
		LayoutId::SUMMARY_TIP,
		LayoutId::JAIL_BREAK_ROLE_CHOICE,
		LayoutId::TIP_DIALOG,
		LayoutId::BUY_GOODS_TIP,
		LayoutId::RANK,
		LayoutId::BLOCK_LOADING,
		LayoutId::MINI_MAP,
		LayoutId::MANOR,
		LayoutId::UPGRADE_MANOR,
		LayoutId::PERSONAL_SHOP,
		LayoutId::REAL_TIME_RANK,
		LayoutId::BUILD_WAR_GRADE,
		LayoutId::BUILD_WAR_GUESS,
		LayoutId::BUILD_WAR_GUESS_RESULT,
		LayoutId::MULTI_TIP_DIALOG,
		LayoutId::REAL_TIME_RANK,
		LayoutId::OUTPUT_CONFIG,
		LayoutId::SUPER_SHOP,
		LayoutId::KILL_PANEL,
		LayoutId::CAR_PROGRESS_TABLE,
		LayoutId::MULTI_TIP_INTERACTION_DIALOG,
		LayoutId::RANCH,
		LayoutId::RANCH_UPGRAGE,
		LayoutId::RANCH_ORDER,
		LayoutId::RANCH_TIP,
		LayoutId::RANCH_FACTORY_BUILD,
		LayoutId::RANCH_FARM_BUILD,
		LayoutId::RANCH_TIME_TIP,
		LayoutId::RANCH_GAIN_TIP,
		LayoutId::RANCH_RANK,
		LayoutId::RANCH_PROSPERITY_RANK,
		LayoutId::OCCUPATION_UNLOCK,
		LayoutId::ENCHANTMENT_PANEL,
		LayoutId::TIGER_LOTTERY,
		LayoutId::HIDEANDSEEKHALL_RESULT,
		LayoutId::PIXEL_GUNHALL_MODE_SELECT,
		LayoutId::GUN_STORE,
		LayoutId::PIXEL_GUN_1V1,
		LayoutId::CHEST_LOTTERY,
		LayoutId::PIXEL_GUN_REVIVE,
		LayoutId::PIXEL_GUNHALL_ARMOR_UPGRADE,
		LayoutId::PIXEL_GUN_RESULT,
		LayoutId::SEASON_RANK,
		LayoutId::SEASON_REWARD,
		LayoutId::LOTTERY_CHEST_DETAIL,
		LayoutId::PIXEL_GUN_GAME_LVUP,
		LayoutId::BIRD_PACKANDFUSE,
		LayoutId::BIRD_PERSONAL_SHOP,
		LayoutId::BIRD_STORE,
		LayoutId::BIRD_LOTTERY,
		LayoutId::BIRD_TIP,
		LayoutId::BIRD_TASK_TIP,
		LayoutId::BIRD_ATLAS,
		LayoutId::BIRD_GAIN_TIP,
		LayoutId::BIRD_ACTIVITY,
	};

	RootGuiLayout::~RootGuiLayout()
	{
		LordSafeDelete(m_stItemName);
        
		m_subscriptionGuard.unsubscribeAll();
		for (auto it : m_layouts) {
		    LordSafeDelete(it.second);
		}
		m_layouts.clear();
	}

	void RootGuiLayout::hideAllChildren()
	{
		for (auto id : ALL_LAYOUT_ID)
		{
			m_layoutShown[id] = false;
		}
	}

	bool RootGuiLayout::openCraftingTable()
	{
		dynamic_cast<gui_playerInventoryControl*>(m_layouts[LayoutId::PLAYER_INVENTORY])->setInCraftingTable(true);
		showPlayerInventory();
		return true;
	}

	void RootGuiLayout::onLoad()
	{
		hideAllChildren();
		m_layouts[LayoutId::MAIN] = LordNew gui_mainControl;
		m_layouts[LayoutId::PLAYER_INVENTORY] = LordNew gui_playerInventoryControl;
		m_layouts[LayoutId::LOADING_PAGE] = LordNew gui_loadingPage;
		m_layouts[LayoutId::PLAYER_INFO] = LordNew gui_playerInfo;
		m_layouts[LayoutId::GAME_TIP_DIALOG] = LordNew gui_gameTipDialog;
		m_layouts[LayoutId::CHEST] = LordNew gui_chest;
		m_layouts[LayoutId::CHAT] = LordNew gui_chat;
		m_layouts[LayoutId::MENU] = LordNew gui_menu;
		m_layouts[LayoutId::TOOL_BAR] = LordNew gui_toolBar;
		m_layouts[LayoutId::DEAD_SUMMARY] = LordNew gui_deadSummary;
		m_layouts[LayoutId::FINAL_SUMMARY] = LordNew gui_finalSummary;
		m_layouts[LayoutId::SHOP] = LordNew gui_shop;
		m_layouts[LayoutId::SUMMARY_POINT] = LordNew gui_summaryPoint;
		m_layouts[LayoutId::COUNTDOWN] = LordNew gui_countdown;
		m_layouts[LayoutId::APP_SHOP] = LordNew gui_appShop;
		m_layouts[LayoutId::SUMMARY_TIP] = LordNew gui_summaryTip;
		m_layouts[LayoutId::JAIL_BREAK_ROLE_CHOICE] = LordNew gui_jailBreakRoleChoice;
		m_layouts[LayoutId::RANK] = LordNew gui_rank;
		m_layouts[LayoutId::MINI_MAP] = LordNew gui_miniMap;
		m_layouts[LayoutId::TIP_DIALOG] = LordNew gui_tipDialog;
		m_layouts[LayoutId::BUY_GOODS_TIP] = LordNew gui_buyGoodsTip;
		m_layouts[LayoutId::BLOCK_LOADING] = LordNew gui_blockLoading;
		m_layouts[LayoutId::MANOR] = LordNew gui_manor;
		m_layouts[LayoutId::UPGRADE_MANOR] = LordNew gui_manorUpgrade;
		m_layouts[LayoutId::PERSONAL_SHOP] = LordNew gui_personalShop;
		m_layouts[LayoutId::REAL_TIME_RANK] = LordNew gui_realTimeRank;
		m_layouts[LayoutId::BUILD_WAR_GRADE] = LordNew gui_buildwarGrade;
		m_layouts[LayoutId::BUILD_WAR_GUESS] = LordNew gui_buildwarGuess;
		m_layouts[LayoutId::BUILD_WAR_GUESS_RESULT] = LordNew gui_buildwarGuessResult;
		m_layouts[LayoutId::MULTI_TIP_DIALOG] = LordNew gui_multiTipDialog;
		m_layouts[LayoutId::MULTI_TIP_INTERACTION_DIALOG] = LordNew gui_multiTipInteractionDialog;
		m_layouts[LayoutId::OUTPUT_CONFIG] = LordNew gui_outputConfig;
		m_layouts[LayoutId::SUPER_SHOP] = LordNew gui_superShop;
		m_layouts[LayoutId::KILL_PANEL] = LordNew gui_killPanel;
		m_layouts[LayoutId::CAR_PROGRESS_TABLE] = LordNew gui_carProgressTable;
		m_layouts[LayoutId::RANCH] = LordNew gui_ranch;
		m_layouts[LayoutId::RANCH_ORDER] = LordNew gui_ranchOrder;
		m_layouts[LayoutId::RANCH_UPGRAGE] = LordNew gui_ranchUpgrade;
		m_layouts[LayoutId::RANCH_TIP] = LordNew gui_ranchTip;
		m_layouts[LayoutId::RANCH_FACTORY_BUILD] = LordNew gui_ranchBuildFactory;
		m_layouts[LayoutId::RANCH_FARM_BUILD] = LordNew gui_ranchBuildFarm;
		m_layouts[LayoutId::RANCH_TIME_TIP] = LordNew gui_ranchTimeTip;
		m_layouts[LayoutId::RANCH_GAIN_TIP] = LordNew gui_ranchGainTip;
		m_layouts[LayoutId::RANCH_RANK] = LordNew gui_ranchRank;
		m_layouts[LayoutId::RANCH_PROSPERITY_RANK] = LordNew gui_ranchProsperityRank;
		m_layouts[LayoutId::OCCUPATION_UNLOCK] = LordNew gui_occupationUnlock;
		m_layouts[LayoutId::ENCHANTMENT_PANEL] = LordNew gui_enchantmentPanel;
		m_layouts[LayoutId::TIGER_LOTTERY] = LordNew gui_tigerLottery;
		m_layouts[LayoutId::HIDEANDSEEKHALL_RESULT] = LordNew gui_hideAndSeekHallResult;
		m_layouts[LayoutId::PIXEL_GUNHALL_MODE_SELECT] = LordNew gui_pixelGunHallModelSelect;
		m_layouts[LayoutId::GUN_STORE] = LordNew gui_gunStore;
		m_layouts[LayoutId::PIXEL_GUN_1V1] = LordNew gui_pixelgun1v1;
		m_layouts[LayoutId::CHEST_LOTTERY] = LordNew gui_chestLottery;
		m_layouts[LayoutId::PIXEL_GUN_REVIVE] = LordNew gui_pixelgunrevive;
		m_layouts[LayoutId::PIXEL_GUNHALL_ARMOR_UPGRADE] = LordNew gui_pixelGunHallArmorUpgrade;
		m_layouts[LayoutId::PIXEL_GUN_RESULT] = LordNew gui_pixelgungameresult;
		m_layouts[LayoutId::SEASON_RANK] = LordNew gui_seasonRank;
		m_layouts[LayoutId::SEASON_REWARD] = LordNew gui_seasonReward;
		m_layouts[LayoutId::LOTTERY_CHEST_DETAIL] = LordNew gui_lotteryChestDetail;
		m_layouts[LayoutId::PIXEL_GUN_GAME_LVUP] = LordNew gui_pixelgungamelvup;
		m_layouts[LayoutId::BIRD_PACKANDFUSE] = LordNew gui_birdPackAndFuse;
		m_layouts[LayoutId::BIRD_PERSONAL_SHOP] = LordNew gui_birdPersonalShop;
		m_layouts[LayoutId::BIRD_STORE] = LordNew gui_birdStore;
		m_layouts[LayoutId::BIRD_LOTTERY] = LordNew gui_birdLottery;
		m_layouts[LayoutId::BIRD_TIP] = LordNew gui_birdTip;
		m_layouts[LayoutId::BIRD_TASK_TIP] = LordNew gui_birdTaskTip;
		m_layouts[LayoutId::BIRD_ATLAS] = LordNew gui_birdAtlas;
		m_layouts[LayoutId::BIRD_GAIN_TIP] = LordNew gui_birdGainTip;
		m_layouts[LayoutId::BIRD_ACTIVITY] = LordNew gui_birdActivity;

		addLayout(m_layouts[LayoutId::LOADING_PAGE]);
		addLayout(m_layouts[LayoutId::TOOL_BAR]);
		m_layoutShown[LayoutId::TOOL_BAR] = false;
		m_layoutShown[LayoutId::LOADING_PAGE] = true;

		m_subscriptionGuard.add(PlayerDeathEvent::subscribe(std::bind(&RootGuiLayout::handlePlayerDeath, this, std::placeholders::_1)));
		m_subscriptionGuard.add(BackKeyDownEvent::subscribe(std::bind(&RootGuiLayout::onBackKeyDown, this)));
		m_subscriptionGuard.add(ShowItemNameEvent::subscribe(std::bind(&RootGuiLayout::showItemNameUi, this, std::placeholders::_1)));
		m_subscriptionGuard.add(ShowGuiJailBreakRoleChoiceEvent::subscribe(std::bind(&RootGuiLayout::showJailBreakRoleChoice, this, std::placeholders::_1)));
		m_subscriptionGuard.add(RankActivateEvent::subscribe(std::bind(&RootGuiLayout::showRank, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(OpenPersonalShopEvent::subscribe(std::bind(&RootGuiLayout::showPersonalShop, this)));
		m_subscriptionGuard.add(OpenSuperShopEvent::subscribe(std::bind(&RootGuiLayout::showSuperShop, this)));
		m_subscriptionGuard.add(OpenGunStoreEvent::subscribe(std::bind(&RootGuiLayout::showGunStore, this)));
		m_subscriptionGuard.add(OpenChestLotteryEvent::subscribe(std::bind(&RootGuiLayout::showChestLottery, this)));
		m_subscriptionGuard.add(OpenSeasonRankEvent::subscribe(std::bind(&RootGuiLayout::showSeasonRank, this)));
		m_subscriptionGuard.add(OpenSeasonRewardEvent::subscribe(std::bind(&RootGuiLayout::showSeasonReward, this)));
		m_subscriptionGuard.add(ShowLotteryChestDetailEvent::subscribe(std::bind(&RootGuiLayout::showLotteryChestDetail, this, std::placeholders::_1)));
		m_subscriptionGuard.add(BirdPackOpenEvent::subscribe(std::bind(&RootGuiLayout::showBirdPackAndFuse, this, true)));

		m_openChestArgs.delayTickToOpenChest = -1;
		m_openChestArgs.inventory = nullptr;
	}

	void RootGuiLayout::showGameGui()
	{
		addLayout(m_layouts[LayoutId::OUTPUT_CONFIG]);
		addLayout(m_layouts[LayoutId::COUNTDOWN]);
		addLayout(m_layouts[LayoutId::PLAYER_INFO]);
		addLayout(m_layouts[LayoutId::MAIN]);
		addLayout(m_layouts[LayoutId::PLAYER_INVENTORY]);
		addLayout(m_layouts[LayoutId::GAME_TIP_DIALOG]);
		addLayout(m_layouts[LayoutId::CHEST]);
		addLayout(m_layouts[LayoutId::TIGER_LOTTERY]);
		addLayout(m_layouts[LayoutId::CHAT]);
		addLayout(m_layouts[LayoutId::MENU]);
		addLayout(m_layouts[LayoutId::DEAD_SUMMARY]);
		addLayout(m_layouts[LayoutId::FINAL_SUMMARY]);
		addLayout(m_layouts[LayoutId::SHOP]);
		addLayout(m_layouts[LayoutId::APP_SHOP]);
		addLayout(m_layouts[LayoutId::PERSONAL_SHOP]);
		addLayout(m_layouts[LayoutId::SUPER_SHOP]);
		addLayout(m_layouts[LayoutId::SUMMARY_POINT]);
		addLayout(m_layouts[LayoutId::SUMMARY_TIP]);
		addLayout(m_layouts[LayoutId::JAIL_BREAK_ROLE_CHOICE]);
		addLayout(m_layouts[LayoutId::RANK]);
		addLayout(m_layouts[LayoutId::BLOCK_LOADING]);
		addLayout(m_layouts[LayoutId::MINI_MAP]);
		addLayout(m_layouts[LayoutId::MANOR]);
		addLayout(m_layouts[LayoutId::UPGRADE_MANOR]);
		addLayout(m_layouts[LayoutId::REAL_TIME_RANK]);
		addLayout(m_layouts[LayoutId::MULTI_TIP_DIALOG]);
		addLayout(m_layouts[LayoutId::BUILD_WAR_GRADE]);
		addLayout(m_layouts[LayoutId::BUILD_WAR_GUESS]);
		addLayout(m_layouts[LayoutId::BUILD_WAR_GUESS_RESULT]);
		addLayout(m_layouts[LayoutId::RANCH]);
		addLayout(m_layouts[LayoutId::RANCH_ORDER]);
		addLayout(m_layouts[LayoutId::MULTI_TIP_INTERACTION_DIALOG]);
		addLayout(m_layouts[LayoutId::RANCH_FACTORY_BUILD]);
		addLayout(m_layouts[LayoutId::RANCH_FARM_BUILD]);
		addLayout(m_layouts[LayoutId::RANCH_UPGRAGE]);
		addLayout(m_layouts[LayoutId::RANCH_TIME_TIP]);
		addLayout(m_layouts[LayoutId::RANCH_RANK]);
		addLayout(m_layouts[LayoutId::RANCH_PROSPERITY_RANK]);
		addLayout(m_layouts[LayoutId::OCCUPATION_UNLOCK]);
		addLayout(m_layouts[LayoutId::ENCHANTMENT_PANEL]);
        addLayout(m_layouts[LayoutId::HIDEANDSEEKHALL_RESULT]);
		addLayout(m_layouts[LayoutId::PIXEL_GUNHALL_MODE_SELECT]);
		addLayout(m_layouts[LayoutId::GUN_STORE]);
		addLayout(m_layouts[LayoutId::PIXEL_GUN_1V1]);
		addLayout(m_layouts[LayoutId::CHEST_LOTTERY]);
		addLayout(m_layouts[LayoutId::PIXEL_GUN_REVIVE]);
		addLayout(m_layouts[LayoutId::PIXEL_GUNHALL_ARMOR_UPGRADE]);
		addLayout(m_layouts[LayoutId::PIXEL_GUN_RESULT]);
		addLayout(m_layouts[LayoutId::SEASON_RANK]);
		addLayout(m_layouts[LayoutId::SEASON_REWARD]);
		addLayout(m_layouts[LayoutId::LOTTERY_CHEST_DETAIL]);
		addLayout(m_layouts[LayoutId::PIXEL_GUN_GAME_LVUP]);
		addLayout(m_layouts[LayoutId::HIDEANDSEEKHALL_RESULT]);
		addLayout(m_layouts[LayoutId::BIRD_PACKANDFUSE]);
		addLayout(m_layouts[LayoutId::BIRD_PERSONAL_SHOP]);
		addLayout(m_layouts[LayoutId::BIRD_STORE]);
		addLayout(m_layouts[LayoutId::BIRD_LOTTERY]);
		addLayout(m_layouts[LayoutId::BIRD_TASK_TIP]);
		addLayout(m_layouts[LayoutId::BIRD_ATLAS]);
		addLayout(m_layouts[LayoutId::BIRD_ACTIVITY]);

		//--------------------------Top view divider start----------------------------
		addLayout(m_layouts[LayoutId::KILL_PANEL]);
		addLayout(m_layouts[LayoutId::CAR_PROGRESS_TABLE]);
		addLayout(m_layouts[LayoutId::RANCH_TIP]);
		addLayout(m_layouts[LayoutId::RANCH_GAIN_TIP]);
		addLayout(m_layouts[LayoutId::BIRD_TIP]);
		addLayout(m_layouts[LayoutId::BIRD_GAIN_TIP]);
		addLayout(m_layouts[LayoutId::TIP_DIALOG]);
		addLayout(m_layouts[LayoutId::BUY_GOODS_TIP]);

		//--------------------------Top view divider end----------------------------


		m_layouts[LayoutId::COUNTDOWN]->hide();
		m_layouts[LayoutId::PLAYER_INVENTORY]->hide();
		m_layouts[LayoutId::GAME_TIP_DIALOG]->hide();
		m_layouts[LayoutId::CHEST]->hide();
		m_layouts[LayoutId::CHAT]->hide();
		m_layouts[LayoutId::MENU]->hide();
		m_layouts[LayoutId::MAIN]->hide();
		m_layouts[LayoutId::TOOL_BAR]->hide();
		m_layouts[LayoutId::DEAD_SUMMARY]->hide();
		m_layouts[LayoutId::FINAL_SUMMARY]->hide();
		m_layouts[LayoutId::SHOP]->hide();
		m_layouts[LayoutId::APP_SHOP]->hide();
		m_layouts[LayoutId::SUMMARY_POINT]->hide();
		m_layouts[LayoutId::SUMMARY_TIP]->hide();
		m_layouts[LayoutId::JAIL_BREAK_ROLE_CHOICE]->hide();
		m_layouts[LayoutId::TIP_DIALOG]->hide();
		m_layouts[LayoutId::BUY_GOODS_TIP]->hide();
		m_layouts[LayoutId::RANK]->hide();
		m_layouts[LayoutId::BLOCK_LOADING]->hide();
		m_layouts[LayoutId::MANOR]->hide();
		m_layouts[LayoutId::UPGRADE_MANOR]->hide();
		m_layouts[LayoutId::PERSONAL_SHOP]->hide();
		m_layouts[LayoutId::REAL_TIME_RANK]->hide();
		m_layouts[LayoutId::BUILD_WAR_GRADE]->hide();
		m_layouts[LayoutId::BUILD_WAR_GUESS]->hide();
		m_layouts[LayoutId::BUILD_WAR_GUESS_RESULT]->hide();
		m_layouts[LayoutId::MULTI_TIP_DIALOG]->hide();
		m_layouts[LayoutId::OUTPUT_CONFIG]->hide();
		m_layouts[LayoutId::SUPER_SHOP]->hide();
		m_layouts[LayoutId::KILL_PANEL]->hide();
		m_layouts[LayoutId::CAR_PROGRESS_TABLE]->hide();
		m_layouts[LayoutId::RANCH]->hide();
		m_layouts[LayoutId::MULTI_TIP_INTERACTION_DIALOG]->hide();
		m_layouts[LayoutId::RANCH_UPGRAGE]->hide();
		m_layouts[LayoutId::RANCH_ORDER]->hide();
		m_layouts[LayoutId::RANCH_TIME_TIP]->hide();
		m_layouts[LayoutId::RANCH_TIP]->hide();
		m_layouts[LayoutId::RANCH_FACTORY_BUILD]->hide();
		m_layouts[LayoutId::RANCH_FARM_BUILD]->hide();
		m_layouts[LayoutId::RANCH_GAIN_TIP]->hide();
		m_layouts[LayoutId::RANCH_RANK]->hide();
		m_layouts[LayoutId::RANCH_PROSPERITY_RANK]->hide();
		m_layouts[LayoutId::OCCUPATION_UNLOCK]->hide();
		m_layouts[LayoutId::ENCHANTMENT_PANEL]->hide();
		m_layouts[LayoutId::TIGER_LOTTERY]->hide();
		m_layouts[LayoutId::HIDEANDSEEKHALL_RESULT]->hide();
		m_layouts[LayoutId::PIXEL_GUNHALL_MODE_SELECT]->hide();
		m_layouts[LayoutId::GUN_STORE]->hide();
		m_layouts[LayoutId::PIXEL_GUN_1V1]->hide();
		m_layouts[LayoutId::CHEST_LOTTERY]->hide();
		m_layouts[LayoutId::PIXEL_GUN_REVIVE]->hide();
		m_layouts[LayoutId::PIXEL_GUNHALL_ARMOR_UPGRADE]->hide();
		m_layouts[LayoutId::PIXEL_GUN_RESULT]->hide();
		m_layouts[LayoutId::SEASON_RANK]->hide();
		m_layouts[LayoutId::SEASON_REWARD]->hide();
		m_layouts[LayoutId::LOTTERY_CHEST_DETAIL]->hide();
		m_layouts[LayoutId::PIXEL_GUN_GAME_LVUP]->hide();
		m_layouts[LayoutId::BIRD_PACKANDFUSE]->hide();
		m_layouts[LayoutId::BIRD_PERSONAL_SHOP]->hide();
		m_layouts[LayoutId::BIRD_STORE]->hide();
		m_layouts[LayoutId::BIRD_LOTTERY]->hide();
		m_layouts[LayoutId::BIRD_TIP]->hide();
		m_layouts[LayoutId::BIRD_TASK_TIP]->hide();
		m_layouts[LayoutId::BIRD_ATLAS]->hide();
		m_layouts[LayoutId::BIRD_GAIN_TIP]->hide();
		m_layouts[LayoutId::BIRD_ACTIVITY]->hide();

		if (m_isChoiceRole)
		{
			auto roleChoice = dynamic_cast<gui_jailBreakRoleChoice*>(m_layouts[LayoutId::JAIL_BREAK_ROLE_CHOICE]);
			if (roleChoice)
			{
				roleChoice->setChoiceRole(m_choiceRole);
			}
			m_layoutShown[LayoutId::JAIL_BREAK_ROLE_CHOICE] = true;
		} 
		else
		{
			m_layoutShown[LayoutId::LOADING_PAGE] = false;
			m_layoutShown[LayoutId::MAIN] = true;
			m_layoutShown[LayoutId::PLAYER_INFO] = true;
			m_layoutShown[LayoutId::TOOL_BAR] = true;
			m_layoutShown[LayoutId::MINI_MAP] = m_isShowMiniMap;
			m_layoutShown[LayoutId::REAL_TIME_RANK] = false;
			m_layoutShown[LayoutId::KILL_PANEL] = true;
			m_layoutShown[LayoutId::CAR_PROGRESS_TABLE] = true;
		}

		m_subscriptionGuard.add(CraftingTableActivateEvent::subscribe(std::bind(&RootGuiLayout::openCraftingTable, this)));
		m_subscriptionGuard.add(ChestActivateEvent::subscribe(std::bind(&RootGuiLayout::openChest, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)));
		m_subscriptionGuard.add(MerchantActivateEvent::subscribe(std::bind(&RootGuiLayout::showShop, this, std::placeholders::_1)));
		m_subscriptionGuard.add(MerchantUpdateEvent::subscribe(std::bind(&RootGuiLayout::shopUpdate, this, std::placeholders::_1)));
		m_subscriptionGuard.add(UpdateUnlockedCommodityEvent::subscribe(std::bind(&RootGuiLayout::updateShopUnlockedState, this)));

		m_isLoad = true;
	}

	void RootGuiLayout::onUpdate(ui32 nTimeElapse)
	{
		m_layoutShown[LayoutId::OUTPUT_CONFIG] = GameClient::CGame::Instance()->isDebugMessageShown() && !m_layoutShown[LayoutId::LOADING_PAGE];
		auto player = Blockman::Instance()->m_pPlayer;
		if (player && m_layoutShown[LayoutId::MAIN] )
		{
			m_layoutShown[LayoutId::PLAYER_INFO] = !player->capabilities.isWatchMode;
		}
		for (auto id : ALL_LAYOUT_ID)
		{
			if (m_layoutShown[id])
			{
				m_layouts[id]->show();
			}
			else
			{
				m_layouts[id]->hide();
			}
		}

		if (m_openChestArgs.delayTickToOpenChest > 0) {
			--m_openChestArgs.delayTickToOpenChest;
			if (m_openChestArgs.delayTickToOpenChest == 0) {
				showChestManage(true);
				dynamic_cast<gui_chest*>(m_layouts[LayoutId::CHEST])->setChestInventory(
					m_openChestArgs.inventory,
					m_openChestArgs.blockPos, m_openChestArgs.face, m_openChestArgs.hisPos
				);

				m_openChestArgs.delayTickToOpenChest = -1;
				m_openChestArgs.inventory = nullptr;
			}
		}

		if (m_stItemName)
		{
			m_stItemName->SetVisible(m_isShowItemName);
			m_stItemName->SetTouchable(false);
			m_showItemNameTime = m_isShowItemName ? m_showItemNameTime + nTimeElapse : 0;
			m_isShowItemName = m_isShowItemName && m_showItemNameTime > 3000 ? false : m_isShowItemName;
		}
	}

	void RootGuiLayout::showPlayerInventory()
	{
		hideAllChildren();
		m_layoutShown[LayoutId::PLAYER_INVENTORY] = true;
	}

	void RootGuiLayout::showMainControl()
	{
		bool showCountdown = m_layoutShown[LayoutId::COUNTDOWN];
		bool showTigerLottery = m_layoutShown[LayoutId::TIGER_LOTTERY];
		bool showPixelGun1v1 = m_layoutShown[LayoutId::PIXEL_GUN_1V1];
		hideAllChildren();
		if (showCountdown)
		{
			m_layoutShown[LayoutId::MAIN] = true;
			m_layoutShown[LayoutId::COUNTDOWN] = true;
		}
		else
		{
			m_layoutShown[LayoutId::MAIN] = true;
			m_layoutShown[LayoutId::PLAYER_INFO] = true;
			m_layoutShown[LayoutId::TOOL_BAR] = true;
			m_layoutShown[LayoutId::MINI_MAP] = m_isShowMiniMap;
			m_layoutShown[LayoutId::KILL_PANEL] = true;
			m_layoutShown[LayoutId::CAR_PROGRESS_TABLE] = true;
			dynamic_cast<gui_toolBar*>(m_layouts[LayoutId::TOOL_BAR])->resetCheckBox();
		}
		m_layoutShown[LayoutId::TIGER_LOTTERY] = showTigerLottery;
		m_layoutShown[LayoutId::PIXEL_GUN_1V1] = showPixelGun1v1;
	}

	void RootGuiLayout::showGameTipDialogManage(bool isShow)
	{

		if (m_layoutShown[LayoutId::LOADING_PAGE])
		{
			return;
		}

		m_layoutShown[LayoutId::GAME_TIP_DIALOG] = isShow;
		if (isShow)
		{
			hideAllChildren();
			m_layoutShown[LayoutId::GAME_TIP_DIALOG] = true;
			m_layoutShown[LayoutId::TOOL_BAR] = false;
		}
		else
		{
			showMainControl();
		}
	}

	void RootGuiLayout::showGameCloseTipDialog()
	{
		dynamic_cast<gui_gameTipDialog*>(m_layouts[LayoutId::GAME_TIP_DIALOG])->setShowType(gui_gameTipDialog::ShowType::CLOSE_GAME);
		showGameTipDialogManage(true);
	}

	void RootGuiLayout::showAppShop()
	{
		bool showTigerLottery = m_layoutShown[LayoutId::TIGER_LOTTERY];
		hideAllChildren();
		m_layoutShown[LayoutId::APP_SHOP] = true;
		m_layoutShown[LayoutId::TOOL_BAR] = true;
		m_layoutShown[LayoutId::TIGER_LOTTERY] = showTigerLottery;
	}

	bool RootGuiLayout::showPersonalShop()
	{
		bool showCountdown = m_layoutShown[LayoutId::COUNTDOWN];
		hideAllChildren();
		m_layoutShown[LayoutId::COUNTDOWN] = showCountdown;
		m_layoutShown[LayoutId::PERSONAL_SHOP] = true;
		m_layoutShown[LayoutId::TOOL_BAR] = true;
		return true;
	}

	bool RootGuiLayout::showSuperShop()
	{
		hideAllChildren();
		m_layoutShown[LayoutId::SUPER_SHOP] = true;
		m_layoutShown[LayoutId::TOOL_BAR] = true;
		return true;
	}

	bool RootGuiLayout::handlePlayerDeath(ui64 playerRuntimeId)
	{
		return true; //in case rebirth
		if (m_layoutShown[LayoutId::DEAD_SUMMARY] || m_layoutShown[LayoutId::FINAL_SUMMARY])
		{
			return true;
		}

		if (m_layoutShown[LayoutId::COUNTDOWN])
		{
			return true;
		}

		if (m_layoutShown[LayoutId::TIP_DIALOG])
		{
			gui_tipDialog* pLayout = dynamic_cast<gui_tipDialog*>(m_layouts[LayoutId::TIP_DIALOG]);
			if (pLayout && pLayout->getShowTipType() == TipType::REVIVE)
			{
				return true;
			}
		}

		if (m_layoutShown[LayoutId::LOADING_PAGE])
		{
			return true;
		}

		EntityPlayerSPMP* me = Blockman::Instance()->m_pPlayer;
		if (!m_layouts[LayoutId::GAME_TIP_DIALOG]->isShown() && me->entityId == playerRuntimeId)
		{
			LordLogInfo("RootGuiLayout::handlePlayerDeath user die");
			dynamic_cast<gui_gameTipDialog*>(m_layouts[LayoutId::GAME_TIP_DIALOG])->setShowType(gui_gameTipDialog::ShowType::DEATH);
			showGameTipDialogManage(true);
		}
		return true;
	}

	bool RootGuiLayout::onBackKeyDown()
	{

		if (m_layoutShown[LayoutId::LOADING_PAGE] || m_layoutShown[LayoutId::JAIL_BREAK_ROLE_CHOICE])
		{
			return true;
		}

		for (auto id : ALL_LAYOUT_ID)
		{
			if (m_layoutShown[id] && 
				id != LayoutId::GAME_TIP_DIALOG && 
				id != LayoutId::MAIN && 
				id != LayoutId::PLAYER_INFO && 
				id != LayoutId::TOOL_BAR && 
				id != LayoutId::MINI_MAP && 
				id != LayoutId::BUILD_WAR_GUESS_RESULT &&
				id != LayoutId::OUTPUT_CONFIG && 
				id != LayoutId::KILL_PANEL &&
				id != LayoutId::CAR_PROGRESS_TABLE &&
				id != LayoutId::PIXEL_GUN_1V1)
			{
				if (m_layoutShown[id] && id == LayoutId::CHEST)
				{
					showChestManage(false);
				}
				showMainControl();
				return true;
			}
		}

		if (!m_layouts[LayoutId::GAME_TIP_DIALOG]->isShown())
		{
			showMenu();
		}
		else
		{
			if (dynamic_cast<gui_gameTipDialog*>(m_layouts[LayoutId::GAME_TIP_DIALOG])->getShowType() != gui_gameTipDialog::ShowType::DEATH  &&
				dynamic_cast<gui_gameTipDialog*>(m_layouts[LayoutId::GAME_TIP_DIALOG])->getShowType() != gui_gameTipDialog::ShowType::NETWORK_DISCONNECT)
			{
				showGameTipDialogManage(false);
			}
		}
		return true;
	}

	bool RootGuiLayout::openChest(IInventory* inventory, const Vector3i & blockPos, int face, const Vector3 & hisPos)
	{
		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::RanchersExplore) return true;

		if (m_openChestArgs.delayTickToOpenChest < 0) {
			m_openChestArgs.delayTickToOpenChest = 20;
			m_openChestArgs.inventory = inventory;
			m_openChestArgs.blockPos = blockPos;
			m_openChestArgs.face = face;
			m_openChestArgs.hisPos = hisPos;
		}
		return true;
	}

	void RootGuiLayout::showChestManage(bool isShow)
	{
		if (!isShow)
		{
			showMainControl();
			Container* pContainer = Blockman::Instance()->m_pPlayer->openContainer;
			ClientNetwork::Instance()->getSender()->sendCloseContainer(BlockPos::ONE);
		}
		else
		{
			hideAllChildren();
			m_layoutShown[LayoutId::CHEST] = isShow;
		}
	}

	void RootGuiLayout::showChatGui()
	{
		bool showTigerLottery = m_layoutShown[LayoutId::TIGER_LOTTERY];
		bool showPixelGun1v1 = m_layoutShown[LayoutId::PIXEL_GUN_1V1];
		hideAllChildren();
		m_layoutShown[LayoutId::CHAT] = true;
		m_layoutShown[LayoutId::TOOL_BAR] = true;
		m_layoutShown[LayoutId::TIGER_LOTTERY] = showTigerLottery;
		m_layoutShown[LayoutId::PIXEL_GUN_1V1] = showPixelGun1v1;
	}

	void RootGuiLayout::showDeadSummaryGui()
	{
		hideAllChildren();
		m_layoutShown[LayoutId::DEAD_SUMMARY] = true;
	}

	void RootGuiLayout::showFinalSummaryGui()
	{
		hideAllChildren();
		m_layoutShown[LayoutId::FINAL_SUMMARY] = true;
	}

	bool RootGuiLayout::showShop(EntityMerchant * merchant)
	{
		hideAllChildren();
		m_layoutShown[LayoutId::SHOP] = true;
		auto shop = dynamic_cast<gui_shop*>(m_layouts[LayoutId::SHOP]);
		if (shop)
		{
			shop->onShopOpen(merchant);
		}
		return true;
	}

	bool RootGuiLayout::shopUpdate(EntityMerchant * merchant) {
		if (m_layoutShown[LayoutId::SHOP]) {
			auto shop = dynamic_cast<gui_shop*>(m_layouts[LayoutId::SHOP]);
			if (shop)
			{
				shop->onShopOpen(merchant);
			}
		}
		return true;
	}

	void RootGuiLayout::createItemNameUi()
	{
		if (!m_stItemName)
		{
			m_stItemName = LordNew GUIStaticText(GWT_STATIC_TEXT, "RootGuiLayout-Item-Name");
			m_stItemName->SetArea(UDim(0, 0), UDim(1, -160), UDim(1, 0), UDim(0, 30.0f));
			m_stItemName->SetTextHorzAlign(HA_CENTRE);
			m_stItemName->SetTextVertAlign(VA_CENTRE);
			m_stItemName->SetText("");
			m_stItemName->SetWordWrap(true);
			m_rootWindow->AddChildWindow(m_stItemName);
		}
	}

	bool RootGuiLayout::showItemNameUi(ItemStackPtr itemStack)
	{
		LordAssert(itemStack && itemStack->stackSize > 0);
		StringStream ss;
		ss << LanguageManager::Instance()->getItemName(itemStack->getItemName());
		auto enchantmentList = itemStack->getEnchantmentTagList();
		if (enchantmentList && enchantmentList->tagCount() > 0)
		{
			for (int i = 0; i < enchantmentList->tagCount(); ++i)
			{
				auto id = static_cast<NBTTagCompound*>(enchantmentList->tagAt(i))->getShort("id");
				auto level = static_cast<NBTTagCompound*>(enchantmentList->tagAt(i))->getShort("lvl");
				auto enchantment = Enchantment::enchantmentsList[id];
				ss << "#n"
					<< LanguageManager::Instance()->getString(enchantment->getName())
					<< ' '
					<< LanguageManager::Instance()->getString("enchantment.level." + StringUtil::ToString(level));
			}
		}
		if (m_stItemName)
		{
			m_isShowItemName = true;
			auto displayString = ss.str();
			StringUtil::Trim(displayString);
			m_stItemName->SetText(displayString.c_str());
			m_showItemNameTime = 0;
		}
		return true;
	}

	void RootGuiLayout::showMenu()
	{
		bool showTigerLottery = m_layoutShown[LayoutId::TIGER_LOTTERY];
		bool showPixelGun1v1 = m_layoutShown[LayoutId::PIXEL_GUN_1V1];
		hideAllChildren();
		m_layoutShown[LayoutId::MENU] = true;
		m_layoutShown[LayoutId::TOOL_BAR] = true;
		m_layoutShown[LayoutId::TIGER_LOTTERY] = showTigerLottery;
		m_layoutShown[LayoutId::PIXEL_GUN_1V1] = showPixelGun1v1;
		dynamic_cast<gui_toolBar*>(m_layouts[LayoutId::TOOL_BAR])->openMenu();
	}

	void RootGuiLayout::showLoadPage()
	{
		hideAllChildren();
		m_layoutShown[LayoutId::LOADING_PAGE] = true;
	}

	void RootGuiLayout::showSummarPoint(const LORD::UVector2& pos, const std::map<int, int>& pointMap)
	{
		gui_summaryPoint* pLayout = dynamic_cast<gui_summaryPoint*>(m_layouts[LayoutId::SUMMARY_POINT]);
		if (!pLayout)
			return;

		m_layoutShown[LayoutId::SUMMARY_POINT] = !m_layoutShown[LayoutId::SUMMARY_POINT];
		if (m_layoutShown[LayoutId::SUMMARY_POINT])
		{
			pLayout->showContent(pos, pointMap);
		}
	}

	void RootGuiLayout::showSummaryRewardTip(const LORD::UVector2& pos, int total)
	{
		gui_summaryTip* pLayout = dynamic_cast<gui_summaryTip*>(m_layouts[LayoutId::SUMMARY_TIP]);
		if (!pLayout)
			return;

		m_layoutShown[LayoutId::SUMMARY_TIP] = !m_layoutShown[LayoutId::SUMMARY_TIP];
		if (m_layoutShown[LayoutId::SUMMARY_TIP])
		{
			pLayout->showContent(pos, total);
		}
	}

	void RootGuiLayout::showCountdown(i8 second)
	{
		hideAllChildren();
		gui_countdown* pLayout = dynamic_cast<gui_countdown*>(m_layouts[LayoutId::COUNTDOWN]);
		if (!pLayout)
		{
			return;
		}
		pLayout->setCountdown(second);
		m_layoutShown[LayoutId::MAIN] = true;
		m_layoutShown[LayoutId::COUNTDOWN] = true;
	}

	void RootGuiLayout::hideCountdown()
	{
		m_layoutShown[LayoutId::COUNTDOWN] = false;
		showMainControl();
	}

	void RootGuiLayout::changePlayerMaxHealth(float health)
	{
		gui_playerInfo * playerInfo = dynamic_cast<gui_playerInfo*>(m_layouts[LayoutId::PLAYER_INFO]);
		if (playerInfo)
		{
			playerInfo->resetMaxHealth(health);
		}
	}

	bool RootGuiLayout::showJailBreakRoleChoice(ChoiceRole role)
	{
		m_isChoiceRole = true;
		m_choiceRole = role;
		if (m_isLoad)
		{
			hideAllChildren();
			auto roleChoice = dynamic_cast<gui_jailBreakRoleChoice*>(m_layouts[LayoutId::JAIL_BREAK_ROLE_CHOICE]);
			if (roleChoice)
			{
				roleChoice->setChoiceRole(role);
			}
			m_layoutShown[LayoutId::JAIL_BREAK_ROLE_CHOICE] = true;
		}
		return true;
	}

	bool RootGuiLayout::showTipDialog()
	{
		m_layoutShown[LayoutId::TIP_DIALOG] = true;
		return true;
	}

	bool RootGuiLayout::hideTipDialog()
	{
		m_layoutShown[LayoutId::TIP_DIALOG] = false;
		return true;
	}

	int RootGuiLayout::getSelfTeamId()
	{
		gui_toolBar* pLayout = dynamic_cast<gui_toolBar*>(m_layouts[LayoutId::TOOL_BAR]);
		if (!pLayout)
		{
			return 0;
		}

		return pLayout->getSelfTeamId();
	}

	bool RootGuiLayout::showBuyGoodsTip(bool bShow)
	{
		m_layoutShown[LayoutId::BUY_GOODS_TIP] = bShow;
		return true;
	}

	bool RootGuiLayout::showRank(const String& rankInfo,int EntityId)
	{
		hideAllChildren();
		m_layoutShown[LayoutId::RANK] = true;
		return true;
	}

	bool RootGuiLayout::hideRank() {
		hideAllChildren();
		showMainControl();
		return true;
	}

	bool RootGuiLayout::showBlockLoading(bool bShow)
	{
		m_layoutShown[LayoutId::BLOCK_LOADING] = bShow;
		return true;
	}

	void RootGuiLayout::switchSerpece() {
		Blockman::Instance()->switchPerspece();
		updateSwitchSerpece();
	}

	void RootGuiLayout::updateSwitchSerpece() {
		gui_toolBar* pLayout = dynamic_cast<gui_toolBar*>(m_layouts[LayoutId::TOOL_BAR]);
		pLayout->updatePerspeceIcon(Blockman::Instance()->getCurrPersonView());
	}

	int RootGuiLayout::getViewNum() {
		return Blockman::Instance()->getCurrPersonView();
	}

	void RootGuiLayout::showParachuteBtn(bool isNeedOpen) {
		gui_mainControl* pLayout = dynamic_cast<gui_mainControl*>(m_layouts[LayoutId::MAIN]);
		pLayout->showParachuteBtn(isNeedOpen);
	}

	void RootGuiLayout::showBuildWarBlockBtn(bool isNeedOpen)
	{
		gui_mainControl* pLayout = dynamic_cast<gui_mainControl*>(m_layouts[LayoutId::MAIN]);
		pLayout->showBuildWarBlockBtn(isNeedOpen);
	}

	void RootGuiLayout::showHideAndSeekBtn(bool isShowChangeActoBtn, bool isShowLockCameraBtn, bool isShowOthenBtn)
	{
		gui_mainControl* pLayout = dynamic_cast<gui_mainControl*>(m_layouts[LayoutId::MAIN]);
		pLayout->showHideAndSeekBtn(isShowChangeActoBtn, isShowLockCameraBtn,isShowOthenBtn);
	}

	bool RootGuiLayout::showManor()
	{
		hideAllChildren();
		m_layoutShown[LayoutId::TOOL_BAR] = true;
		m_layoutShown[LayoutId::MANOR] = true;
		return true;
	}

	void RootGuiLayout::showUpgradeManor()
	{
		hideAllChildren();
		m_layoutShown[LayoutId::TOOL_BAR] = true;
		m_layoutShown[LayoutId::MANOR] = true;
		m_layoutShown[LayoutId::UPGRADE_MANOR] = true;
	}

	void RootGuiLayout::hidePlayerOperation()
	{
		auto pLayout = dynamic_cast<gui_mainControl*>(m_layouts[LayoutId::MAIN]);
		if (pLayout)
		{
			pLayout->hidePlayerOperationView(EventArgs());
		}
	}

	void RootGuiLayout::showDeadSummaryGui(const String& gameResult, bool isNextServer) {
		gui_deadSummary* pLayout = dynamic_cast<gui_deadSummary*>(m_layouts[LayoutId::DEAD_SUMMARY]);
		pLayout->showDeadSummary(gameResult, isNextServer);
	}

	bool RootGuiLayout::updateRealTimeRankData(String result) 
	{
		gui_realTimeRank* pLayout = dynamic_cast<gui_realTimeRank*>(m_layouts[LayoutId::REAL_TIME_RANK]);
		if (pLayout)
			pLayout->updateRealTimeRankData(result);
		return true;
	}

	bool RootGuiLayout::canPersonalShop()
	{
		gui_personalShop* pShop = dynamic_cast<gui_personalShop*>(m_layouts[LayoutId::PERSONAL_SHOP]);
		if (pShop)
			return pShop->canShow();
		return false;
	}

	bool RootGuiLayout::isShowRealTimeRankStatus()
	{
		gui_realTimeRank* pRank = dynamic_cast<gui_realTimeRank*>(m_layouts[LayoutId::REAL_TIME_RANK]);
		if (pRank)
			return pRank->isShow();

		return false;
	}

	void RootGuiLayout::isColseRealTimeRank(bool status)
	{
		gui_realTimeRank* pLayout = dynamic_cast<gui_realTimeRank*>(m_layouts[LayoutId::REAL_TIME_RANK]);
		if (pLayout)
			pLayout->isColseRealTimeRankOp(status);
	}

	void RootGuiLayout::showRealTimeRank()
	{
		hideAllChildren();
		showMainControl();
		m_layoutShown[LayoutId::REAL_TIME_RANK] = true;
		m_layoutShown[LayoutId::TOOL_BAR] = true;
	}

	void RootGuiLayout::hideRealTimeRank()
	{
		hideAllChildren();
		showMainControl();
	}

	void RootGuiLayout::showBuildWarGrade(bool show)
	{
		if (show)
		{
			if (!isMainControlShown())
			{
				return;
			}

			m_layoutShown[LayoutId::BUILD_WAR_GRADE] = true;
		}
		else
		{
			m_layoutShown[LayoutId::BUILD_WAR_GRADE] = false;
		}

	}

	void RootGuiLayout::showBuildWarGuessResult(bool show)
	{

		m_layoutShown[LayoutId::BUILD_WAR_GUESS_RESULT] = show;
	}

	void RootGuiLayout::showBuildWarGuessUi(bool show)
	{
		m_layoutShown[LayoutId::BUILD_WAR_GUESS] = show;
	}

	bool RootGuiLayout::updateShopUnlockedState()
	{
		if (m_layoutShown[LayoutId::SHOP]) {
			auto shop = dynamic_cast<gui_shop*>(m_layouts[LayoutId::SHOP]);
			if (shop)
			{
				shop->onUpdateUnlockedState();
			}
		}
		return true;
	}

	void RootGuiLayout::showMultiTipDialog()
	{
		m_layoutShown[LayoutId::MULTI_TIP_DIALOG] = true;
	}

	void RootGuiLayout::hideMultiTipDialog()
	{
		m_layoutShown[LayoutId::MULTI_TIP_DIALOG] = false;
	}

	void RootGuiLayout::showMultiTipInteractionDialog()
	{
		m_layoutShown[LayoutId::MULTI_TIP_INTERACTION_DIALOG] = true;
	}

	void RootGuiLayout::hideMultiTipInteractionDialog()
	{
		m_layoutShown[LayoutId::MULTI_TIP_INTERACTION_DIALOG] = false;
	}

	void RootGuiLayout::showRanch()
	{
		hideAllChildren();
		m_layoutShown[LayoutId::TOOL_BAR] = true;
		m_layoutShown[LayoutId::RANCH] = true;
		dynamic_cast<gui_ranch*>(m_layouts[LayoutId::RANCH])->openRanch();
	}

	void RootGuiLayout::showRanchTip()
	{
		/*hideAllChildren();
		m_layoutShown[LayoutId::TOOL_BAR] = true;*/
		m_layoutShown[LayoutId::RANCH_TIP] = true;
	}

	void RootGuiLayout::hideRanchTip()
	{
		m_layoutShown[LayoutId::RANCH_TIP] = false;
	}

	void RootGuiLayout::showRanchOrder()
	{
		hideAllChildren();
		m_layoutShown[LayoutId::TOOL_BAR] = true;
		m_layoutShown[LayoutId::RANCH_ORDER] = true;
	}

	void RootGuiLayout::showRanchUpgrade()
	{
		m_layoutShown[LayoutId::RANCH_UPGRAGE] = true;
	}

	void RootGuiLayout::hideRanchUpgrade()
	{
		m_layoutShown[LayoutId::RANCH_UPGRAGE] = false;
	}

	void RootGuiLayout::showRanchBuildFactory()
	{
		hideAllChildren();
		m_layoutShown[LayoutId::TOOL_BAR] = true;
		m_layoutShown[LayoutId::RANCH_FACTORY_BUILD] = true;
	}

	void RootGuiLayout::showRanchBuildFarm()
	{
		hideAllChildren();
		m_layoutShown[LayoutId::TOOL_BAR] = true;
		m_layoutShown[LayoutId::RANCH_FARM_BUILD] = true;
	}

	void RootGuiLayout::setRanchTimeTipVisible(bool isShow)
	{
		m_layoutShown[LayoutId::RANCH_TIME_TIP] = isShow;
	}

	void RootGuiLayout::setRanchGainTipVisible(bool isShow)
	{
		m_layoutShown[LayoutId::RANCH_GAIN_TIP] = isShow;
	}

	void RootGuiLayout::setRanchRankVisible(bool isShow)
	{
		m_layoutShown[LayoutId::RANCH_RANK] = isShow;
	}

	void RootGuiLayout::setRanchProsperityRankVisible(bool isShow)
	{
		m_layoutShown[LayoutId::RANCH_PROSPERITY_RANK] = isShow;
	}

	void RootGuiLayout::setOccupationUnlockVisible(bool isShow)
	{
		m_layoutShown[LayoutId::OCCUPATION_UNLOCK] = isShow;
	}

	void RootGuiLayout::checkBackgroundMusic()
	{
		m_layouts[LayoutId::MENU]->checkBackgroundMusic();
	}

	bool RootGuiLayout::setEnchantmentShow(bool isShow)
	{
		if (isShow)
		{
			hideAllChildren();
			m_layoutShown[LayoutId::MAIN] = true;
		}

		m_layoutShown[LayoutId::ENCHANTMENT_PANEL] = isShow;
		return true;
	}

	void RootGuiLayout::showTigerLottery()
	{
		showMainControl();
		m_layoutShown[LayoutId::TIGER_LOTTERY] = true;
	}

	void RootGuiLayout::hideTigerLottery()
	{
		m_layoutShown[LayoutId::TIGER_LOTTERY] = false;
		showMainControl();
	}

	void RootGuiLayout::showHideAndSeekHallResult()
	{
		showMainControl();
		m_layoutShown[LayoutId::HIDEANDSEEKHALL_RESULT] = true;
	}

	void RootGuiLayout::showLackOfMoney(i32 difference, CurrencyType type)
	{
		hideAllChildren();
		showMainControl();
		m_layoutShown[LayoutId::TIP_DIALOG] = true;
		auto tip = dynamic_cast<gui_tipDialog*>(m_layouts[LayoutId::TIP_DIALOG]);
		if (tip)
		{
			tip->showLackOfMoney(difference, type);
		}
	}
	
	bool RootGuiLayout::showBirdPackAndFuse(bool isPack)
	{
		hideAllChildren();
		m_layoutShown[LayoutId::TOOL_BAR] = true;
		m_layoutShown[LayoutId::BIRD_PACKANDFUSE] = true;
		if (m_layoutShown[LayoutId::BIRD_PACKANDFUSE]) {
			auto view = dynamic_cast<gui_birdPackAndFuse*>(m_layouts[LayoutId::BIRD_PACKANDFUSE]);
			if (view)
			{
				view->setShowPackOrFuse(isPack);
			}
		}
		return true;
	}

	bool RootGuiLayout::isShowPack()
	{
		if (m_layoutShown[LayoutId::BIRD_PACKANDFUSE]) 
		{
			auto view = dynamic_cast<gui_birdPackAndFuse*>(m_layouts[LayoutId::BIRD_PACKANDFUSE]);
			if (view->isShowPack())
			{
				return true;
			}
		}
		return false;
	}

	void RootGuiLayout::showBirdPersonShop()
	{
		hideAllChildren();
		m_layoutShown[LayoutId::TOOL_BAR] = true;
		m_layoutShown[LayoutId::MAIN] = true;
		m_layoutShown[LayoutId::BIRD_PERSONAL_SHOP] = true;
	}

	void RootGuiLayout::showBirdStore()
	{
		hideAllChildren();
		m_layoutShown[LayoutId::TOOL_BAR] = true;
		m_layoutShown[LayoutId::BIRD_STORE] = true;
	}
	
	void RootGuiLayout::showBirdLottery()
	{
		m_layoutShown[LayoutId::BIRD_LOTTERY] = true;
	}

	bool RootGuiLayout::isShowBirdLottery()
	{
		return m_layoutShown[LayoutId::BIRD_LOTTERY];
	}
	
	void RootGuiLayout::setBirdTipVisible(bool isShow)
	{
		m_layoutShown[LayoutId::BIRD_TIP] = isShow;
	}
	
	bool RootGuiLayout::isShowBirdStore()
	{
		return m_layoutShown[LayoutId::BIRD_STORE];
	}

	bool RootGuiLayout::isBirdViewShow()
	{
		return isShowBirdStore()  || isShowBirdTaskTip()  || isShowBirdLottery() 
			|| m_layoutShown[LayoutId::BIRD_PERSONAL_SHOP] || m_layoutShown[LayoutId::BIRD_ACTIVITY]  || m_layoutShown[LayoutId::BIRD_ATLAS] 
			|| m_layoutShown[LayoutId::MENU] || m_layoutShown[LayoutId::CHAT] || m_layoutShown[LayoutId::BIRD_PACKANDFUSE] ;
	}

	void RootGuiLayout::setBirdTaskTipVisible(bool isShow)
	{
		m_layoutShown[LayoutId::BIRD_TASK_TIP] = isShow;
	}

	bool RootGuiLayout::isShowBirdTaskTip()
	{
		return m_layoutShown[LayoutId::BIRD_TASK_TIP];
	}

	void RootGuiLayout::showBirdAtlas()
	{
		hideAllChildren();
		m_layoutShown[LayoutId::TOOL_BAR] = true;
		m_layoutShown[LayoutId::BIRD_ATLAS] = true;
	}

	void RootGuiLayout::setBirdGainTipVisible(bool isShow)
	{
		m_layoutShown[LayoutId::BIRD_GAIN_TIP] = isShow;
	}

	void RootGuiLayout::showBirdActivity()
	{
		hideAllChildren();
		m_layoutShown[LayoutId::TOOL_BAR] = true;
		m_layoutShown[LayoutId::BIRD_ACTIVITY] = true;
	}

	void RootGuiLayout::setPixelGunHallModeSelectShow(bool isShow)
	{
		if (isShow)
		{
			showMainControl();
		}
		m_layoutShown[LayoutId::PIXEL_GUNHALL_MODE_SELECT] = isShow;
	}

	void RootGuiLayout::setPixelGun1V1Show(bool isShow)
	{
		if (isShow)
		{
			showMainControl();
		}
		m_layoutShown[LayoutId::PIXEL_GUN_1V1] = isShow;
	}

	void RootGuiLayout::setPixelGunReviveShow(bool isShow)
	{
		showMainControl();
		m_layoutShown[LayoutId::PIXEL_GUN_REVIVE] = isShow;
		if (isShow)
		{
			m_layoutShown[LayoutId::MAIN] = false;
			m_layoutShown[LayoutId::PLAYER_INFO] = false;
		}
		else
		{
			m_layoutShown[LayoutId::MAIN] = true;
			m_layoutShown[LayoutId::PLAYER_INFO] = true;
		}
	}

	void RootGuiLayout::setPixelGunResultShow(bool isShow)
	{
		showMainControl();
		m_layoutShown[LayoutId::PIXEL_GUN_RESULT] = isShow;
		if (isShow)
		{
			m_layoutShown[LayoutId::MAIN] = false;
			m_layoutShown[LayoutId::PLAYER_INFO] = false;
		}
		else 
		{
			m_layoutShown[LayoutId::MAIN] = true;
			m_layoutShown[LayoutId::PLAYER_INFO] = true;
		}
	}

	bool RootGuiLayout::showGunStore()
	{
		showMainControl();
		m_layoutShown[LayoutId::GUN_STORE] = true;
		return true;
	}

	bool RootGuiLayout::showChestLottery()
	{
		showMainControl();
		m_layoutShown[LayoutId::CHEST_LOTTERY] = true;
		return true;
	}

	void RootGuiLayout::showPixelGunHallArmorUpgrade(bool isShow)
	{
		if (isShow)
		{
			showMainControl();
			m_layoutShown[LayoutId::PIXEL_GUNHALL_ARMOR_UPGRADE] = isShow;
		}
	}

	bool RootGuiLayout::showSeasonRank()
	{
		showMainControl();
		m_layoutShown[LayoutId::SEASON_RANK] = true;
		return true;
	}

	bool RootGuiLayout::showSeasonReward()
	{
		showMainControl();
		m_layoutShown[LayoutId::SEASON_REWARD] = true;
		return true;
	}

	bool RootGuiLayout::showLotteryChestDetail(int type)
	{
		m_layoutShown[LayoutId::LOTTERY_CHEST_DETAIL] = true;
		auto lottery_chest_detail = dynamic_cast<gui_lotteryChestDetail*>(m_layouts[LayoutId::LOTTERY_CHEST_DETAIL]);
		if (lottery_chest_detail)
		{
			lottery_chest_detail->ShowChestDetail((LotteryChestType)type);
		}
		return true;
	}

	void RootGuiLayout::hideLotteryChestDetail()
	{
		m_layoutShown[LayoutId::LOTTERY_CHEST_DETAIL] = false;
	}

	void RootGuiLayout::showPixelGunGameLvUp(bool show)
	{
		m_layoutShown[LayoutId::PIXEL_GUN_GAME_LVUP] = show;
	}

	bool RootGuiLayout::isPixelGunResultOpen()
	{
		return m_layoutShown[LayoutId::PIXEL_GUN_RESULT];
	}

}
