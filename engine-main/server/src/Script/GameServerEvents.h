#pragma once
#include "Script/Event/ScriptEvent.h"
#include "Blockman/Entity/EntityPlayerMP.h"
#include "Network/ClientPeer.h"
#include "Entity/DamageSource.h"
#include "Inventory/IInventory.h"
#include "Blockman/World/ServerWorld.h"
#include "TileEntity/TileEntityInventory.h"
#include "RoomManager.h"
#include "Network/RoomClient.h"
#include "Script/VarProxy/BaseValueProxy.h"
#include "World/Manor.h"
#include "Enums/Enums.h"

using namespace BLOCKMAN;

namespace SCRIPT_EVENT
{
	// respath  webVer, RankAddr,  serverworld roomgameconfig
	struct GameInitEvent :public ScriptEvent<GameInitEvent, const char *, const char *, ServerWorld*,
		RoomGameConfig&, Vector3i*> {};

	//ClientPeer
	struct PlayerLoginEvent :public ScriptEvent<PlayerLoginEvent, ClientPeer*> {};

	// platformUid
	struct PlayerLogoutEvent :public ScriptEvent<PlayerLogoutEvent, ui64> {};

	// diePlayer iskillbyplayer entityPlayer
	struct PlayerDieEvent :public ScriptEvent<PlayerDieEvent, EntityPlayerMP*, bool, EntityPlayerMP*> {};

	// hurt player hurt from hurtvalue
	struct PlayerAttackedEvent :public ScriptEvent<PlayerAttackedEvent,
		EntityPlayerMP*, EntityPlayerMP *, const char * , FloatProxy*> {};

	//move player x , y , z
	struct PlayerMoveEvent :public ScriptEvent<PlayerMoveEvent, EntityPlayerMP*, float, float, float > {};

	//dynamicAttr
	struct PlayerDynamicAttrEvent :public ScriptEvent<PlayerDynamicAttrEvent, ui64, int, int, int, bool, int> {};

	//dynamicAttrAndManorInfo
	struct PlayerDynamicAttrAndManorInfoEvent :public ScriptEvent<PlayerDynamicAttrAndManorInfoEvent, ui64, ui64, int , int, int, int, bool, int> {};

	//Inventory
	struct ChestOpenEvent :public ScriptEvent<ChestOpenEvent, Vector3i, TileEntityChest*, ui64> {};

	// block break blockid, x, y, z
	struct BlockBreakEvent :public ScriptEvent<BlockBreakEvent, ui64, int, Vector3i> {};

	// block break blokid, meta, x, y, z
	struct BlockBreakWithMetaEvent:public ScriptEvent<BlockBreakWithMetaEvent, ui64, int, int, Vector3i, int, int> {};

	// block break blockid, x, y, z, gunId
	struct BlockBreakWithGunEvent :public ScriptEvent<BlockBreakWithGunEvent, ui64, int, Vector3i, int> {};

	// block start break blockid, x, y, z
	struct BlockStartBreakEvent :public ScriptEvent<BlockStartBreakEvent, ui64, int, Vector3i> {};

	// block abort break blockid, x, y, z
	struct BlockAbortBreakEvent :public ScriptEvent<BlockAbortBreakEvent, ui64, int, Vector3i> {};

	// use prop 
	struct PlayerUsePropEvent :public ScriptEvent<PlayerUsePropEvent, ui64, const char *> {};

	// player place block event
	struct PlayerPlaceBlockEvent : public ScriptEvent<PlayerPlaceBlockEvent, ui64, int, int, Vector3i> {};

	// player spawn event
	struct PlayerFirstSpawnEvent :public ScriptEvent<PlayerFirstSpawnEvent, ClientPeer*> {};
	// player Rebirth event
	struct PlayerRespawnEvent : public ScriptEvent<PlayerRespawnEvent, ClientPeer*> {};

	// exchange item event
	// rakssid
	// exchange type
	//				1, playerinv to chestinv
	//				2, chestinv to playerinv
	// itemid
	// itemMeta
	// itemnum
	// bullets
	// chest position
	struct PlayerExchangeItemEvent :public ScriptEvent<PlayerExchangeItemEvent, ui64, ui16, int, int, int, int, Vector3i> {};

	// change item in hand

	// item id
	// item meta
	struct PlayerChangeItemInHandEvent :public ScriptEvent<PlayerChangeItemInHandEvent, ui64, int , int > {};
	
	// use throwable item event
	struct PlayerUseThrowableItemEvent :public ScriptEvent<PlayerUseThrowableItemEvent, int> {};

	// player shopping 
	// rakssId type goodsId
	struct PlayerBuyCommodityEvent : public ScriptEvent<PlayerBuyCommodityEvent, ui64, ui8, CommodityInfo*, ShoppingAttr*, IntProxy*> {};

	// pickup item
	struct PlayerPickupItemEvent : public ScriptEvent<PlayerPickupItemEvent, ui64, int, int, i32> {};

	// pickup item end
	struct PlayerPickupItemEndEvent : public ScriptEvent<PlayerPickupItemEndEvent, ui64, int, int> {};

	// player buy goods 
	// rakssId type goodsId
	struct PlayerBuyGoodsEvent : public ScriptEvent<PlayerBuyGoodsEvent, ui64, i32, i32, IntProxy*, BoolProxy*> {};

	// bug goods add item handle
	struct PlayerBuyGoodsAddItemEvent : public ScriptEvent<PlayerBuyGoodsAddItemEvent, ui64, i32, i32, i32> {};

	// player buy goods 
	// rakssId type goodsId
	struct PlayerBuyGoodsSuccessEvent : public ScriptEvent<PlayerBuyGoodsSuccessEvent, ui64, i32, i32> {};

	// currency change
	struct PlayerCurrencyChangeEvent : public ScriptEvent<PlayerCurrencyChangeEvent, ui64, ui64> {};
	
	// player unlock commodity
	struct PlayerUnlockCommodityEvent : public ScriptEvent<PlayerUnlockCommodityEvent, ui64, int, int> {};

	// select role
	struct PlayerSelectRoleEvent : public ScriptEvent<PlayerSelectRoleEvent, ui64, ui32> {};

	// bug vehicle
	struct PlayerBuyVehicleSuccessEvent : public ScriptEvent<PlayerBuyVehicleSuccessEvent, ui64, ui32> {};

	// player ready
	struct PlayerReadyEvent : public ScriptEvent<PlayerReadyEvent, ui64> {};

	// get data from db back
	struct GetDataFromDBEvent : public ScriptEvent<GetDataFromDBEvent, ui64, ui32, String> {};

	// get data from db error back
	struct GetDataFromDBErrorEvent : public ScriptEvent<GetDataFromDBErrorEvent, ui64, ui32> {};

	// player use item
	struct PlayerUseCommonItemEvent : public ScriptEvent<PlayerUseCommonItemEvent, ui64, ui32> {};

	// player buy Respawn result
	struct PlayerBuyRespawnResultEvent : public ScriptEvent<PlayerBuyRespawnResultEvent, ui64, ui32> {};

	// zscore from redis back
	struct ZScoreFromRedisDBEvent : public ScriptEvent<ZScoreFromRedisDBEvent, String, String, ui32, ui32> {};

	// zrange from redis back
	struct ZRangeFromRedisDBEvent : public ScriptEvent<ZRangeFromRedisDBEvent, String, String> {};

	// player drop item
	struct PlayerDropItemEvent : public ScriptEvent<PlayerDropItemEvent, ui64, ui32, ui32> {};

	// player click teleport
	struct PlayerClickTeleportEvent : public ScriptEvent<PlayerClickTeleportEvent, ui64> {};

	// player close container
	struct PlayerCloseContainerEvent : public ScriptEvent<PlayerCloseContainerEvent, ui64, Vector3i> {};

	// player skill event
	struct PlayerSkillTypeEvent : public ScriptEvent<PlayerSkillTypeEvent, ui64, int, bool> {};
	
	//change player actor
	struct PlayerClickChangeActorEvent : public  ScriptEvent<PlayerClickChangeActorEvent, ui64> {};

	//change player actor
	struct PlayerChangeActorEvent : public  ScriptEvent<PlayerChangeActorEvent, ui64> {};

	//player buy pick up item
	struct PlayerBuyPickupItemEvent : public  ScriptEvent <PlayerBuyPickupItemEvent, ui64, i32, i32> {};

	//player buy pick up item result
	struct PlayerBuyPickupItemResultEvent : public  ScriptEvent <PlayerBuyPickupItemResultEvent, ui64, bool, i32, i32, i32, i32, i32> {};

	// attack actor npc
	struct PlayerAttackActorNpcEvent : public  ScriptEvent<PlayerAttackActorNpcEvent, ui64, ui32> {};

	// review player
	struct PlayerReviewPlayerEvent : public  ScriptEvent<PlayerReviewPlayerEvent, ui64, ui64> {};

	// buy actor after
	struct PlayerBuyActorResultEvent : public ScriptEvent<PlayerBuyActorResultEvent, ui64, bool> {};
	
	// get manor 
	struct PlayerGetManorEvent : public ScriptEvent<PlayerGetManorEvent, ui64, String> {};

	// sell manor
	struct PlayerSellManorEvent : public ScriptEvent <PlayerSellManorEvent, ui64, bool, String > {};

	// update manor result
	struct PlayerUpdateManorResultEvent : public ScriptEvent <PlayerUpdateManorResultEvent, ui64, bool, String> {};

	// buy house result 
	struct PlayerBuyHouseResultEvent : public ScriptEvent <PlayerBuyHouseResultEvent, ui64, bool, i32, i32, i32> {};

	// buy furinture result
	struct PlayerBuyFurnitureResultEvent : public ScriptEvent <PlayerBuyFurnitureResultEvent, ui64, bool, i32, i32> {};

	// build house
	struct PlayerBuildHouseEvent : public ScriptEvent <PlayerBuildHouseEvent, ui64, i32> {};

	// put furniture
	struct PlayerPutFurnitureEvent : public ScriptEvent <PlayerPutFurnitureEvent, ui64, i32, PutFurnitureStatus> {};

	// recycle furniture
	struct PlayerRecycleFurnitureEvent : public ScriptEvent <PlayerRecycleFurnitureEvent, ui64, i32> {};

	// player click manor transform button
	struct PlayerManorTeleportEvent : public ScriptEvent <PlayerManorTeleportEvent, ui64> {};

	// update real time rank
	struct PlayerUpdateRealTimeRankEvent : public ScriptEvent<PlayerUpdateRealTimeRankEvent, ui64> {};

	// buy switchable prop
	struct PlayerBuySwitchablePropEvent : public ScriptEvent<PlayerBuySwitchablePropEvent, ui64, String, IntProxy*> {};

	// buy upgrade prop
	struct PlayerBuyUpgradePropEvent : public ScriptEvent<PlayerBuyUpgradePropEvent, ui64, String, IntProxy*> {};

	// attack creature
	struct PlayerAttackCreatureEvent : public  ScriptEvent<PlayerAttackCreatureEvent, ui64, ui32, ui32> {};
	
	// upgrade resource 
	struct PlayerUpgradeResourceEvent : public ScriptEvent<PlayerUpgradeResourceEvent, ui64, int> {};

	//crops update 
	struct BlockCropsUpdateEvent : public ScriptEvent<BlockCropsUpdateEvent, ui64, int, int, int, int, Vector3i, int> {};

	// crops pick
	struct BlockCropsPickEvent : public ScriptEvent<BlockCropsPickEvent, ui64, int, int, int, Vector3i, int, int> {};

	// crops steal
	struct BlockCropsStealEvent : public ScriptEvent<BlockCropsStealEvent, ui64, int, int, int, Vector3i, i32> {};

	// get exchange rate from platform money to game money
	struct GetMoneyExchangeRateEvent : public ScriptEvent<GetMoneyExchangeRateEvent, IntProxy*> {};

	//load user's manor infomation 
	struct PlayerLoadManorInfoEvent : public ScriptEvent<PlayerLoadManorInfoEvent, ui64, String> {};

	// load manor master infomation 
	struct PlayerLoadManorMasterInfoEvent : public ScriptEvent<PlayerLoadManorMasterInfoEvent, ui64, String> {};

	// load  manor config 
	struct LoadManorConfigEvent : public ScriptEvent<LoadManorConfigEvent, String> {};

	// load  manor inventory 
	struct LoadManorInventoryEvent : public ScriptEvent<LoadManorInventoryEvent, ui64, String, String> {};

	// load manor block
	struct LoadManorBlockEvent : public ScriptEvent<LoadManorBlockEvent, ui64, String, String> {};

	// block crops place
	struct BlockCropsPlaceEvent : public ScriptEvent<BlockCropsPlaceEvent, ui64, int, Vector3i , int> {};

	//player transfer to target monor, return false if target not in the same server 
	struct PlayerCallOnTargetManorEvent : public ScriptEvent<PlayerCallOnTargetManorEvent, ui64, ui64> {};

	// watch respawn
	struct PlayerWatchRespawnEvent : public ScriptEvent<PlayerWatchRespawnEvent, ui64> {};
	
	// player praise result
	struct PlayerPraiseResultEvent : public ScriptEvent<PlayerPraiseResultEvent, ui64, int> {};

	// http response
	struct HttpResponseEvent : public ScriptEvent<HttpResponseEvent, String, String> {};

	//custom tip result
	struct CustomTipMsgEvent : public ScriptEvent<CustomTipMsgEvent, ui64, String, bool> {};

	// manor charm rank
	struct LoadManorCharmRankEvent : public ScriptEvent<LoadManorCharmRankEvent, bool, String> {};

	// manor potential rank
	struct LoadManorPotentialRankEvent : public ScriptEvent<LoadManorPotentialRankEvent, bool, String> {};

	// my manor rank
	struct LoadMyManorRankEvent : public ScriptEvent<LoadMyManorRankEvent, bool, ui64, String> {};

	//user manor release
	struct UserManorReleaseEvent : public ScriptEvent<UserManorReleaseEvent, ui64> {};

	// open shop
	struct BuildWarOpenShopEvent : public ScriptEvent<BuildWarOpenShopEvent, ui64> {};

	// build war grade
	struct BuildWarGradeEvent : public ScriptEvent<BuildWarGradeEvent, ui64, ui8> {};

	// build war open guess result
	struct BuildWarOpenGuessResultEvent : public ScriptEvent<BuildWarOpenGuessResultEvent, ui64> {};

	// build war guess visit
	struct BuildWarGuessVisitEvent : public ScriptEvent<BuildWarGuessVisitEvent, ui64, ui64> {};

	// build war guess suc
	struct BuildWarGuessSucEvent : public ScriptEvent<BuildWarGuessSucEvent, ui64, int> {};

	// build war guess suc
	struct BuildWarShowResultEvent : public ScriptEvent<BuildWarShowResultEvent, ui64> {};
	
	// attack sessionNpc
	struct PlayerAttackSessionNpcEvent : public ScriptEvent<PlayerAttackSessionNpcEvent, ui64, ui32> {};

	// consume tip result
	struct ConsumeTipMsgEvent : public ScriptEvent<ConsumeTipMsgEvent, ui64, String, bool> {};

	// consume diamonds result
	struct ConsumeDiamondsEvent : public ScriptEvent<ConsumeDiamondsEvent, ui64, bool, String, String, String> {};

	// buy enchantment prop
	struct PlayerBuyEnchantmentPropEvent : public ScriptEvent<PlayerBuyEnchantmentPropEvent, ui64, String, IntProxy*> {};

	// buy super prop
	struct PlayerBuySuperPropEvent : public ScriptEvent<PlayerBuySuperPropEvent, ui64, String, IntProxy*> {};

	// buy custom prop
	struct PlayerBuyCustomPropEvent : public ScriptEvent<PlayerBuyCustomPropEvent, ui64, String, IntProxy*> {};

	// back hall
	struct PlayerBackHallEvent : public ScriptEvent<PlayerBackHallEvent, ui64> {};

	// long hit entity
	struct PlayerLongHitEntityEvent : public ScriptEvent<PlayerLongHitEntityEvent, ui64, ui32> {};

	// buy bullet
	struct PlayerBuyBulletEvent : public ScriptEvent<PlayerBuyBulletEvent, ui64> {};

	// player buy ranch item result
	struct PlayerBuyRanchItemResultEvent : public  ScriptEvent <PlayerBuyRanchItemResultEvent, ui64, bool, std::vector<RanchCommon> > {};

	// player ranch receive event
	struct PlayerRanchReceiveEvent : public  ScriptEvent <PlayerRanchReceiveEvent, ui64, ui64> {};

	// player ranch expand event
	struct PlayerRanchExpandEvent : public  ScriptEvent <PlayerRanchExpandEvent, ui64, i32> {};

	// player ranch upgrade house event
	struct PlayerRanchUpgradeHouseEvent : public  ScriptEvent <PlayerRanchUpgradeHouseEvent, ui64> {};

	// player ranch upgrade storage event
	struct PlayerRanchUpgradeStorageEvent : public  ScriptEvent <PlayerRanchUpgradeStorageEvent, ui64> {};

	// player ranch use cube upgrade storage event
	struct PlayerRanchUseCubeUpgradeStorageEvent : public  ScriptEvent <PlayerRanchUseCubeUpgradeStorageEvent, ui64> {};

	//player ranch sell item
	struct PlayerRanchSellItemEvent : public ScriptEvent <PlayerRanchSellItemEvent, ui64, i32, i32> {};

	// player ranch shortcut event
	struct PlayerRanchShortcutEvent : public ScriptEvent <PlayerRanchShortcutEvent, ui64, bool, i32> {};

	// player ranch destroy house
	struct PlayerRanchDestroyHouseEvent : public ScriptEvent <PlayerRanchDestroyHouseEvent, ui64> {};

	// get ranch shortcut cost
	struct GetRanchShortcutCostEvent : public ScriptEvent <GetRanchShortcutCostEvent, ui64, i32, BoolProxy*, IntProxy*> {};

	// can ranch storage save items. return true if enough capacity to save items
	struct CanRanchStorageSaveItemsEvent : public ScriptEvent <CanRanchStorageSaveItemsEvent, ui64, std::vector<RanchCommon>, i32, IntProxy*, BoolProxy*> {};

	// get ranch building item cost
	struct GetRanchBuildItemCostEvent : public ScriptEvent <GetRanchBuildItemCostEvent, ui64, i32, i32, i32, BoolProxy*, IntProxy*, IntProxy*> {};

	// player buy ranch building item result
	struct PlayerBuyRanchBuildItemResultEvent : public ScriptEvent <PlayerBuyRanchBuildItemResultEvent, ui64, i32, bool, i32, i32> {};

	// player take away items from ranch building cargo
	struct PlayerTakeAwayRanchBuildCargoEvent : public ScriptEvent <PlayerTakeAwayRanchBuildCargoEvent, ui64, i32, i32, i32> {};

	// player ranch building queue product
	struct PlayerRanchBuildingQueueProductEvent : public ScriptEvent <PlayerRanchBuildingQueueProductEvent, ui64, i32, i32, i32> {};

	// player ranch building queue speed up
	struct PlayerRanchBuildingQueueSpeedUpEvent : public ScriptEvent <PlayerRanchBuildingQueueSpeedUpEvent, ui64, i32, i32, i32, bool> {};

	//player ranch building queue unlock
	struct PlayerRanchBuildingQueueUnlockEvent : public ScriptEvent <PlayerRanchBuildingQueueUnlockEvent, ui64, i32, i32, i32, bool> {};

	// get ranch building speedup cost
	struct GetRanchBuildingSpeedUpCostEvent : public ScriptEvent <GetRanchBuildingSpeedUpCostEvent, ui64, i32, i32, i32, BoolProxy*, IntProxy*> {};

	// get ranch building queue unlock cost
	struct GetRanchBuildQueueUnlockCostEvent : public ScriptEvent <GetRanchBuildQueueUnlockCostEvent, ui64, i32, i32, BoolProxy*, IntProxy*, IntProxy*> {};

	// create explore map
	struct CreateExploreMapEvent : public ScriptEvent<CreateExploreMapEvent, ui64> {};

	// rancher explore begin
	struct RancherExBeginEvent : public ScriptEvent <RancherExBeginEvent, ui64> {};

	// rancher explore task finish
	struct RancherExTaskFinishEvent : public ScriptEvent <RancherExTaskFinishEvent, ui64, int, int> {};

	// rancher player help finish event
	struct PlayerRanchHelpFinishEvent : public ScriptEvent <PlayerRanchHelpFinishEvent, ui64, ui64, i32> {};

	// ranch player receive mail reward event
	struct PlayerRanchReceiveMailRewardEvent : public ScriptEvent <PlayerRanchReceiveMailRewardEvent, ui64, i64> {};

	// ranch player send every day gift event
	struct PlayerRanchSendEveryDayGiftEvent : public ScriptEvent <PlayerRanchSendEveryDayGiftEvent, ui64, i64> {};

	// ranch player broadcast refresh order
	struct RefreshPlayerRanchOrderEvent : public ScriptEvent <RefreshPlayerRanchOrderEvent, ui64> {};

	// ranch player receive achievement reward
	struct PlayerRanchReceiveAchievementRewardEvent : public ScriptEvent <PlayerRanchReceiveAchievementRewardEvent, ui64, i64> {};

	// ranch player go explore event
	struct PlayerRanchGoExploreEvent : public ScriptEvent <PlayerRanchGoExploreEvent, ui64> {};

	// ranch player invite other player join game
	struct PlayerRanchSendInviteJoinGameEvent : public ScriptEvent <PlayerRanchSendInviteJoinGameEvent, ui64, ui64> {};

	// change current item info event
	struct ChangeCurrentItemInfoEvent : public ScriptEvent <ChangeCurrentItemInfoEvent, ui64, int> {};

	// change current item info event
	struct BlockCropsInfoEvent : public ScriptEvent <BlockCropsInfoEvent, ui64, int, int, int, int> {};

	// get ranch task items cost
	struct GetRanchTaskItemsCostEvent : public ScriptEvent <GetRanchTaskItemsCostEvent, ui64, i32, i32, BoolProxy*, IntProxy*> {};

	// buy ranch task items result
	struct PlayerBuyRanchTaskItemsResultEvent : public ScriptEvent <PlayerBuyRanchTaskItemsResultEvent, ui64, i32, i32, String> {};

	// building production speed up
	struct PlayerRanchBuildingProductionSpeedUpEvent : public ScriptEvent <PlayerRanchBuildingProductionSpeedUpEvent, ui64, i32, i32, i32> {};

	// shot actor npc
	struct PlayerShotActorNpcEvent : public  ScriptEvent<PlayerShotActorNpcEvent, ui64, ui32> {};

	// select lottery
	struct PlayerSelectLotteryEvent : public  ScriptEvent<PlayerSelectLotteryEvent, ui64, String> {};

	// lottery again
	struct PlayerLotteryAgainEvent : public  ScriptEvent<PlayerLotteryAgainEvent, ui64> {};

	// hall lottery again
	struct PlayerHallLotteryAgainEvent : public  ScriptEvent<PlayerHallLotteryAgainEvent, ui64, int, int> {};

	// review actor npc
	struct PlayerReviewActorNpcEvent : public  ScriptEvent<PlayerReviewActorNpcEvent, ui64, int> {};

	struct BuyFlyingPermissionResultEvent : public ScriptEvent <BuyFlyingPermissionResultEvent, ui64, bool> {};

	struct CheckFlyingPermissionEvent : public ScriptEvent <CheckFlyingPermissionEvent, ui64> {};

	struct PlayerPotionEffectFinishedEvent : public ScriptEvent <PlayerPotionEffectFinishedEvent, ui64, i32> {};

	struct PlayerKeepItemEvent : public ScriptEvent<PlayerKeepItemEvent, ui64, bool> {};

	struct ItemSkillConsumptionItemEvent : public ScriptEvent<ItemSkillConsumptionItemEvent, ui64, int> {};

	struct OpenEnchantmentEvent : public ScriptEvent<OpenEnchantmentEvent, ui64> {};

	struct EnchantmentEffectClickEvent : public ScriptEvent<EnchantmentEffectClickEvent, ui64, int, int, int> {};

	struct PlayerEnchantmentQuickEvent : public ScriptEvent<PlayerEnchantmentQuickEvent, ui64, int, int> {};

	struct OpenSelectModeEvent : public ScriptEvent<OpenSelectModeEvent, ui64> {};

	struct PlayerPixelGunHallUnlockMapEvent : public ScriptEvent<PlayerPixelGunHallUnlockMapEvent, ui64, int, int> {};

	struct PlayerUseStoreGunEvent : public ScriptEvent<PlayerUseStoreGunEvent, ui64, int> {};

	struct PlayerBuyStoreGunEvent : public ScriptEvent<PlayerBuyStoreGunEvent, ui64, int> {};

	struct PlayerUpgradeStoreGunEvent : public ScriptEvent<PlayerUpgradeStoreGunEvent, ui64, int> {};

	struct PlayerQuickUpgradeStoreGunEvent : public ScriptEvent<PlayerQuickUpgradeStoreGunEvent, ui64, int> {};

	struct PlayerUseStorePropEvent : public ScriptEvent<PlayerUseStorePropEvent, ui64, int> {};

	struct PlayerUnloadStorePropEvent : public ScriptEvent<PlayerUnloadStorePropEvent, ui64, int> {};

	struct PlayerBuyStorePropEvent : public ScriptEvent<PlayerBuyStorePropEvent, ui64, int> {};

	struct PlayerUpgradeStorePropEvent : public ScriptEvent<PlayerUpgradeStorePropEvent, ui64, int> {};

	struct PlayerQuickUpgradeStorePropEvent : public ScriptEvent<PlayerQuickUpgradeStorePropEvent, ui64, int> {};

	struct PlayerFirstShowStoreGunEvent : public ScriptEvent<PlayerFirstShowStoreGunEvent, ui64, int> {};

	struct PlayerFirstShowStorePropEvent : public ScriptEvent<PlayerFirstShowStorePropEvent, ui64, int> {};

	struct PlayerPixelGunSelectBoxEvent : public ScriptEvent<PlayerPixelGunSelectBoxEvent, ui64, int> {};

	struct PlayerPixelGunExitEvent : public ScriptEvent<PlayerPixelGunExitEvent, ui64> {};

	struct PlayerPixelGunRematchEvent : public ScriptEvent<PlayerPixelGunRematchEvent, ui64> {};

	struct PlayerPixelGunReviveEvent : public ScriptEvent<PlayerPixelGunReviveEvent, ui64> {};

	struct PlayerOpenLotteryChestEvent : public ScriptEvent<PlayerOpenLotteryChestEvent, ui64, int> {};

	struct OpenArmorUpgradeEvent : public ScriptEvent<OpenArmorUpgradeEvent, ui64> {};

	struct PlayerUpgradeArmorEvent : public ScriptEvent<PlayerUpgradeArmorEvent, ui64, bool,int> {};

	struct PlayerPixelGunResultBackEvent : public ScriptEvent<PlayerPixelGunResultBackEvent, ui64> {};

	struct PlayerPixelGunResultNextEvent : public ScriptEvent<PlayerPixelGunResultNextEvent, ui64> {};

	struct PlayerPixelGunResultRevengeEvent : public ScriptEvent<PlayerPixelGunResultRevengeEvent, ui64> {};

	struct PlayerPixelGunFightEvent : public ScriptEvent<PlayerPixelGunFightEvent, ui64, String, String> {};

	struct BirdSimulatorBagOperationEvent : public ScriptEvent<BirdSimulatorBagOperationEvent, ui64, i64, int> {};

	struct BirdSimulatorStoreOperationEvent : public ScriptEvent<BirdSimulatorStoreOperationEvent, ui64, int, int, int> {};

	struct BirdSimulatorPersonalStoreBuyGoodsEvent : public ScriptEvent<BirdSimulatorPersonalStoreBuyGoodsEvent, ui64, int, int> {};

	struct BirdSimulatorFuseEvent : public ScriptEvent<BirdSimulatorFuseEvent, ui64, std::vector<i64>> {};

	struct BirdSimulatorOpenTreasureChestEvent : public ScriptEvent<BirdSimulatorOpenTreasureChestEvent, ui64, i32> {};

	struct BirdSimulatorLotteryEvent : public ScriptEvent<BirdSimulatorLotteryEvent, ui64, i32> {};

	struct BirdSimulatorTaskEvent : public ScriptEvent<BirdSimulatorTaskEvent, ui64, i32, i32> {};

	struct BirdSimulatorFeedEvent : public ScriptEvent<BirdSimulatorFeedEvent, ui64, i64, i32, i32> {};

	struct BirdSimulatorSetDressEvent : public ScriptEvent<BirdSimulatorSetDressEvent, ui64, i64, i32, i32> {};

	struct BirdSimulator_MonsterDieEvent: public ScriptEvent<BirdSimulator_MonsterDieEvent, i32> {};

	struct BirdSimulator_MonsterFindPlayerEvent: public ScriptEvent<BirdSimulator_MonsterFindPlayerEvent, i32, ui64> {};
	
}


