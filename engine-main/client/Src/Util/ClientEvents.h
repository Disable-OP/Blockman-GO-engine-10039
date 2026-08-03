#pragma once
#include <initializer_list>
#include "Util/Event.h"
#include "Inventory/IInventory.h"
#include "Inventory/Wallet.h"
#include "Structs/ChoiceRole.h"
#include "Enums/Enums.h"
#include "World/Manor.h"
#include "UI/GUIUDim.h"

namespace BLOCKMAN
{
	class EntityMerchant;
	class RanchCommon;
	class ProductQueue;
	class ProductRecipe;
	class RanchOrderInfo;
	class BroadcastMessage;
	class BirdGiftBag;
	class BirdGain;
	class BirdInfo;

	inline namespace NETWORK
	{
		struct NetworkConnectionFailureEvent : Event<NetworkConnectionFailureEvent> {};
		struct NetworkConnectionDisconnectEvent : Event<NetworkConnectionDisconnectEvent> {};
		struct NetworkConnectionTimeoutEvent : Event<NetworkConnectionTimeoutEvent> {};
		struct NetworkConnectionKickOutEvent : Event<NetworkConnectionKickOutEvent> {};
		struct NetworkConnectionSuccessfulEvent : Event<NetworkConnectionSuccessfulEvent> {};
		struct NetworkRetryConnectionEvent : Event<NetworkRetryConnectionEvent> {};
	}

	inline namespace APPLICATION
	{
		struct LoadWorldEndEvent : Event<LoadWorldEndEvent> {};
		struct LoadWorldStartEvent : Event<LoadWorldStartEvent> {};
		struct LoginFailureEvent :Event<LoginFailureEvent> {};
		struct LoginTokenErrorEvent :Event<LoginTokenErrorEvent> {};
		struct LoginGetUserAttrFailEvent :Event<LoginGetUserAttrFailEvent> {};
		struct LoginWaitRoleAttrFailEvent :Event<LoginWaitRoleAttrFailEvent> {};
		struct LoginResponseTimeoutEvent :Event<LoginResponseTimeoutEvent> {};
		struct LoginSuccEvent :Event<LoginSuccEvent> {};
		struct UserInEvent:Event<UserInEvent, const ui64, const String&, const String&, const int , const int, const int> {};
		struct UserOutEvent:Event<UserOutEvent, const ui64> {};
		struct GameStatusEvent :Event<GameStatusEvent, const int> {};
		struct CheckCSVersionFailEvent :Event<CheckCSVersionFailEvent> {};
		struct CheckCSVersionSuccEvent :Event<CheckCSVersionSuccEvent> {};
		struct BeKickedEvent :Event<BeKickedEvent> {};
		struct FriendOperationForAppHttpResultEvent:Event<FriendOperationForAppHttpResultEvent, int, ui64> {};
		struct FriendOperationForServerEvent :Event<FriendOperationForServerEvent, int, ui64> {};
	}

	inline namespace GUI
	{
		struct ShowItemNameEvent : Event<ShowItemNameEvent, ItemStackPtr> {};
		struct DownloadMapFailureEvent :Event<DownloadMapFailureEvent> {};
		struct DownloadMapSuccessEvent :Event<DownloadMapSuccessEvent> {};
		struct DownloadMapProgressEvent :Event<DownloadMapProgressEvent, const float, const float> {};
		struct GameoverEvent :Event<GameoverEvent, const String&> {};
		struct BreakBlockProgressEvent :Event<BreakBlockProgressEvent, float, RAYTRACE_TYPE> {};
		struct BreakBlockUiManageEvent :Event <BreakBlockUiManageEvent, bool, float, float > {};
		struct PlayerLifeStatusEvent : Event<PlayerLifeStatusEvent, ui64, bool> {};
		struct PlayerChangeItemEvent : Event<PlayerChangeItemEvent, int, bool> {};
		struct ReceiveDeadSummaryEvent : Event<ReceiveDeadSummaryEvent, const String&, bool> {};
		struct ReceiveFinalSummaryEvent : Event<ReceiveFinalSummaryEvent, const String&, bool> {};
		struct TeamBedDestroyedEvent : Event<TeamBedDestroyedEvent, int> {};
		struct GetAllPlayerTeamInfoEvent : Event<GetAllPlayerTeamInfoEvent, const String&> {};
		struct PlayerChangeTeamEvent : Event<PlayerChangeTeamEvent, ui64, int> {};
		struct AppShopUpdateEvent : Event<AppShopUpdateEvent> {};
		struct BuyGoodsResultEvent : Event<BuyGoodsResultEvent, const String&> {};
		struct ShowGuiJailBreakRoleChoiceEvent : Event<ShowGuiJailBreakRoleChoiceEvent, ChoiceRole> {};
		struct ShowVehicleTipDialogEvent : Event<ShowVehicleTipDialogEvent, int> {};
		struct showUnlockCommodityTipDialogEvent : Event<showUnlockCommodityTipDialogEvent, int, int, int, int, int, const String&> {};
		struct ShowGoAppShopTipDialogEvent : Event<ShowGoAppShopTipDialogEvent> {};
		struct ShowGoNpcMerchantTipDialogEvent : Event<ShowGoNpcMerchantTipDialogEvent, int, float, float, float, float> {};
		struct VehicleLockEvent : Event<VehicleLockEvent, bool> {};
		struct VehicleOnOffEvent : Event<VehicleOnOffEvent, bool, bool> {};
		struct GetMoneyEvent : Event<GetMoneyEvent, int> {};
		struct GetItemEvent : Event<GetItemEvent, int, int, int> {};
		struct GetGoodsEvent : Event<GetGoodsEvent, const String&, int> {};
		struct BuyCommodityResultEvent : Event<BuyCommodityResultEvent, const String&> {};
		struct ShowReviveEvent : Event<ShowReviveEvent, int, int, int, int, int> {};
		struct UpdateBlockProgressEvent : Event<UpdateBlockProgressEvent, float, bool> {};
		struct ShowMinimapEvent : Event<ShowMinimapEvent, float> {};
		struct MemberLeftAndKillUpdateEvent : Event<MemberLeftAndKillUpdateEvent, int, int> {};
		struct ChangeSkillTypeEvent : Event<ChangeSkillTypeEvent, SkillType, float, float> {};
		struct ShowPickUpItemTipDialogEvent : Event<ShowPickUpItemTipDialogEvent, i32, i32, i32, CurrencyType> {};
		struct ShowResultTipDialogEvent : Event<ShowResultTipDialogEvent, ResultCode, const String&> {};
		struct ReceiveRankResultEvent : Event<ReceiveRankResultEvent, const String&> {};
		struct SyncChangePlayerActortEvent : Event<SyncChangePlayerActortEvent, int, int, const String&> {};
		struct SyncGameTimeShowUiEvent : Event<SyncGameTimeShowUiEvent, bool, int> {};
		struct ChangeHeartEvent : Event<ChangeHeartEvent, int, int> {};
		struct ShowGetSellManorTipDialogEvent : Event<ShowGetSellManorTipDialogEvent> {};
		struct ManorOnUpdateEvent : Event<ManorOnUpdateEvent> {};
		struct ManorHouseSelectEvent : Event<ManorHouseSelectEvent, ManorHouse*> {};
		struct ShowLackOfMoneyTipDialogEvent : Event<ShowLackOfMoneyTipDialogEvent, i32, CurrencyType> {};
		struct SetPutFurnitureIdEvent : Event<SetPutFurnitureIdEvent, i32> {};
		struct SetManorBtnVisibleEvent : Event<SetManorBtnVisibleEvent, bool> {};
		struct ShowRedPointEvent : Event<ShowRedPointEvent> {};
		struct ShowPlayerOperationEvent : Event<ShowPlayerOperationEvent, ui64, const String&> {};
		struct ShowManorBuildTipDialogEvent : Event<ShowManorBuildTipDialogEvent> {};
		struct SureBuildManorHouseEvent : Event<SureBuildManorHouseEvent> {};
		struct RefreshManorUiEvent : Event<RefreshManorUiEvent> {};
		struct ManorMessageRemoveEvent : Event<ManorMessageRemoveEvent, const String&, const String&> {};
		struct UpdateGameMonsterInfoEvent : Event<UpdateGameMonsterInfoEvent, int, int, int> {};
		struct UpdateBossBloodStripEvent : Event<UpdateBossBloodStripEvent, int, int> {};
		struct ChangeSwitchablePropsEvent : Event<ChangeSwitchablePropsEvent, const String&> {};
		struct ChangeUpgradePropsEvent : Event<ChangeUpgradePropsEvent, const String&> {};
		struct PersonalShopTipEvent : Event<PersonalShopTipEvent, const String&> {};
		struct ShowSwitchablePropEvent : Event<ShowSwitchablePropEvent, const String&, const String&, const String&, int> {};
		struct ShowUpgradeResourceEvent : Event<ShowUpgradeResourceEvent, int, int, int, const String&> {};
		struct UpdateBasementLifeEvent : Event<UpdateBasementLifeEvent, float, float> {};
		struct ShowCustomTipMsgEvent : Event<ShowCustomTipMsgEvent, const String&, const String&> {};
		struct ServerSyncGameTypeEvent : Event<ServerSyncGameTypeEvent> {};
		struct ShowGetManorTipDialogEvent : Event<ShowGetManorTipDialogEvent> {};
		struct ShowSellManorTipDialogEvent : Event<ShowSellManorTipDialogEvent> {};
		struct ShowShopItemTipEvent : Event<ShowShopItemTipEvent, int, int, const String&, int, const String&, const String&, int > {};
		struct SyncShowMaskTimeEvent : Event<SyncShowMaskTimeEvent, int> {};
		struct ShowBuildWarPlayerNum : Event<ShowBuildWarPlayerNum, ui8, ui8> {};
		struct ShowBuildWarLeftTime : Event<ShowBuildWarLeftTime, bool, ui16, const String&> {};
		struct ShowBuildWarGrade : Event<ShowBuildWarGrade, const String&, ui8> {};
		struct ShowBuildWarGuessResult : Event<ShowBuildWarGuessResult, bool, int> {};
		struct ShowBuildWarGuessUi : Event<ShowBuildWarGuessUi, const String&, int> {};
		struct SetBuildWarGuessShow : Event<SetBuildWarGuessShow, bool> {};
		struct ShowBuildWarGuessTipDialogEvent : Event<ShowBuildWarGuessTipDialogEvent, int, const String&> {};
		struct ReceiveFinalSummaryExtraEvent : Event<ReceiveFinalSummaryExtraEvent, bool, const String&, int> {};
		struct ShowMulitTipDialogEvent : Event<ShowMulitTipDialogEvent, const String&> {};
		struct TeamResourcesUpdateEvent : Event<TeamResourcesUpdateEvent, const String&> {};
		struct OpenPersonalShopEvent : Event<OpenPersonalShopEvent> {};
		struct UpdateBuildProgressEvent : Event<UpdateBuildProgressEvent, const String&, int, int, const String&> {};
		struct ChangeEnchantmentPropsEvent : Event<ChangeEnchantmentPropsEvent, const String&> {};
		struct ChangePersonalSuperPropsEvent :Event<ChangePersonalSuperPropsEvent, const String&> {};
		struct ShowConsumeCoinTipEvent : Event<ShowConsumeCoinTipEvent, const String&, int, int, const String&> {};
		struct OpenSuperShopEvent : Event<OpenSuperShopEvent> {};
		struct ChangeSuperPropsEvent : Event<ChangeSuperPropsEvent, const String&> {};
		struct SuperShopTipEvent : Event<SuperShopTipEvent, const String&> {};
		struct ShowSuperPropEvent : Event<ShowSuperPropEvent, const String&, const String&, const String&> {};
		struct ChangeDefenseEvent : Event<ChangeDefenseEvent, int, int> {};
		struct ChangeAttackEvent : Event<ChangeAttackEvent, int, int> {};
		struct CastSkillEvent :Event<CastSkillEvent> {};
		struct ShootWarningEvent : Event<ShootWarningEvent, float, bool> {};
		struct PlayEndingAnimationEvent : Event<PlayEndingAnimationEvent, const String&, bool, int> {};
		struct ReceiveKillMsgEvent : Event<ReceiveKillMsgEvent, const String&> {};
		struct PersonalShopAreaEvent : Event<PersonalShopAreaEvent, const Vector3 &, const Vector3 &> {};
		struct ChangeCustomPropsEvent : Event<ChangeCustomPropsEvent, const String&> {};
		struct UpdateCustomPropsEvent : Event<UpdateCustomPropsEvent, const String&> {};
		struct UpdateCarDirectionEvent : Event<UpdateCarDirectionEvent, int, int> {};
		struct UpdateCarProgressEvent : Event<UpdateCarProgressEvent, int, float> {};
		struct ShowCannonBtnEvent : Event<ShowCannonBtnEvent, bool, int> {};
		struct HideRechargeBtnEvent : Event<HideRechargeBtnEvent> {};

		struct ShowMulitTipInteractionDialogEvent : Event<ShowMulitTipInteractionDialogEvent, const String&> {};
		struct RanchDataChangeEvent : Event<RanchDataChangeEvent> {};
		struct RanchHouseChangeEvent : Event<RanchHouseChangeEvent> {};
		struct RanchStorageChangeEvent : Event<RanchStorageChangeEvent> {};
		struct RanchRankChangeEvent : Event<RanchRankChangeEvent> {};
		struct RanchBuildChangeEvent : Event<RanchBuildChangeEvent> {};
		struct RanchFactoryBuildChangeEvent : Event<RanchFactoryBuildChangeEvent> {};
		struct RanchFarmingBuildChangeEvent : Event<RanchFarmingBuildChangeEvent,i32> {};
		struct ShowRanchExpandTipEvent : Event<ShowRanchExpandTipEvent, i32, i32, const vector<RanchCommon>::type&, const vector<RanchCommon>::type& > {};
		struct ShowRanchStorageUpgradeTipEvent : Event<ShowRanchStorageUpgradeTipEvent> {};
		struct ShowRanchTimeTipEvent : Event<ShowRanchTimeTipEvent, i32, i32, i32, const String &> {};
		struct ShowRanchUnlockItemsTipEvent : Event<ShowRanchUnlockItemsTipEvent, const vector<i32>::type&> {};
		struct ShowRanchCommonTipEvent : Event<ShowRanchCommonTipEvent, const String&> {};
		struct ShowRanchBuildFarmEvent : Event<ShowRanchBuildFarmEvent, i32> {};
		struct ShowRanchBuildFactoryEvent : Event<ShowRanchBuildFactoryEvent, i32> {};
		
		struct ShowRanchQueueUnlockEvent : Event<ShowRanchQueueUnlockEvent, i32, i32, i32, i32, const String & > {};
		struct ShowRanchLockOfMoneyEvent : Event<ShowRanchLockOfMoneyEvent, i32, i32> {};
		struct ShowRanchLockItemsEvent : Event<ShowRanchLockItemsEvent, const vector<RanchCommon>::type&> {};
		struct ShowRanchSellItemEvent : Event<ShowRanchSellItemEvent, const UVector2&, const RanchCommon&> {};
		struct ShowRanchGainTipEvent : Event<ShowRanchGainTipEvent, const vector<RanchCommon>::type&> {};
		struct RanchBuildFarmUpdateEvent : Event<RanchBuildFarmUpdateEvent, i32> {};
		struct RanchBuildFactoryUpdateEvent : Event<RanchBuildFactoryUpdateEvent, i32> {};
		struct RanchItemClickEvent : Event<RanchItemClickEvent, i32, const String &> {};
		struct RanchStorageOpenEvent : Event<RanchStorageOpenEvent> {};
		struct RanchOrderChangeEvent : Event<RanchOrderChangeEvent> {};
		struct RanchOrderItemClickEvent : Event<RanchOrderItemClickEvent, const UVector2&, const RanchOrderInfo&> {};
		struct RanchOrderItemHideEvent : Event<RanchOrderItemHideEvent> {};
		struct ShowRanchRankEvent : Event<ShowRanchRankEvent, i32> {};
		struct ShowRanchTakeEvent : Event<ShowRanchTakeEvent, i32, const UVector2&> {};
		struct FrendDataChangeEvent : Event<FrendDataChangeEvent> {};
		struct ClanMemberDataChangeEvent : Event<ClanMemberDataChangeEvent> {};
		
		struct ShowRanchExBeginTipDialogEvent : Event<ShowRanchExBeginTipDialogEvent> {};
		struct ShowRanchExTaskFinishTipDialogEvent : Event<ShowRanchExTaskFinishTipDialogEvent> {};
		struct ShowRanchExTip : Event<ShowRanchExTip, ui16, ui16> {};
		struct ShowRanchExCurrentItemInfo : Event<ShowRanchExCurrentItemInfo, bool, const String &, int, int> {};
		struct ShowRanchExTask : Event<ShowRanchExTask, bool, const String &> {};
		struct ShowRanchExItem : Event<ShowRanchExItem, bool, const String &> {};
		struct RanchFriendOperateHideEvent : Event<RanchFriendOperateHideEvent> {};
		struct RanchFriendClickEvent : Event<RanchFriendClickEvent, i64, const String &> {};
		struct ShowRanchFriendHelpListEvent : Event<ShowRanchFriendHelpListEvent, i64> {};
		struct MailUpdateEvent : Event<MailUpdateEvent> {};
		struct FriendUpdateEvent : Event<FriendUpdateEvent> {};
		struct RanchAchievementChangeEvent : Event<RanchAchievementChangeEvent> {};
		struct BroadcastReceiveEvent : Event<BroadcastReceiveEvent, const BroadcastMessage&> {};
		struct UserActorChangeEvent : Event<UserActorChangeEvent, i64> {};
		struct ShowCropInfoEvent : Event<ShowCropInfoEvent, Vector3i, i32 , i32, i32, i32> {};
		struct ShowSpeedUpQueueEvent : Event<ShowSpeedUpQueueEvent, i32, i32, i32, i32, i32> {};
		struct ShowRanchOrderOperateLockItemsEvent : Event<ShowRanchOrderOperateLockItemsEvent, const vector<RanchCommon>::type&, i32, i32> {};
		struct RanchRecipeItemClickEvent : Event<RanchRecipeItemClickEvent,  const ProductRecipe&> {};
		struct ShowRanchLockItemsByBuildOperationEvent : Event<ShowRanchLockItemsByBuildOperationEvent, const vector<RanchCommon>::type&, i32, i32> {};
		struct ShowRanchStorageFullTipEvent : Event<ShowRanchStorageFullTipEvent, const String&> {};
		struct ShowRanchLockItemsByUpgradeStorageEvent : Event<ShowRanchLockItemsByUpgradeStorageEvent, const vector<RanchCommon>::type&> {};
		struct ShowRanchLockItemsByOrderHelpEvent : Event<ShowRanchLockItemsByOrderHelpEvent, const vector<RanchCommon>::type&, i64> {};
		struct GameVideoChangeEvent : Event<GameVideoChangeEvent, i32> {};
		struct ShowOccupationUnlockTip : Event<ShowOccupationUnlockTip, const String&> {};
		struct ShowKeepItemTipEvent : Event<ShowKeepItemTipEvent, int, int, int> {};
		struct ShowEnchantMentEvent : Event<ShowEnchantMentEvent, const String &> {};
		struct ShowEnchantMentQuickEvent : Event<ShowEnchantMentQuickEvent, int, int, int> {};

		struct ShowPixelGunHallNotOpenEvent : Event<ShowPixelGunHallNotOpenEvent, int> {};
		struct ShowPixelGunHallLvNotEnoughEvent : Event<ShowPixelGunHallLvNotEnoughEvent, int, int> {};
		struct ShowPixelGunHallLvMapLockDiamondEvent : Event<ShowPixelGunHallLvMapLockDiamondEvent, int, int> {};

		struct ShowPixelGunHallModeSelectEvent : Event<ShowPixelGunHallModeSelectEvent, const String &> {};
		struct ShowPixelGun1v1Event : Event<ShowPixelGun1v1Event, const String &> {};
		struct ShowPixelGunReviveEvent : Event<ShowPixelGunReviveEvent, const String &> {};
		struct ShowPixelGunHallModeSelectReWardInfoEvent : Event<ShowPixelGunHallModeSelectReWardInfoEvent, int> {};
		struct ShowPixelGunHallMapSelectChangeEvent : Event<ShowPixelGunHallMapSelectChangeEvent, int> {};
		struct ShowPixelGunResultEvent : Event<ShowPixelGunResultEvent, const String &, int> {};
		struct ShowPixelGunLvupEvent : Event<ShowPixelGunLvupEvent, const String &> {};
		struct ShowPixelGunResultSpecialInfoEvent : Event<ShowPixelGunResultSpecialInfoEvent, bool> {};

		struct OpenGunStoreEvent : Event<OpenGunStoreEvent> {};
		struct GunStoreTabChangeEvent : Event<GunStoreTabChangeEvent, int> {};
		struct GunStoreGunItemClickEvent : Event<GunStoreGunItemClickEvent, int> {};
		struct GunStorePropItemClickEvent : Event<GunStorePropItemClickEvent, int> {};
		struct ShowStoreGunDetailEvent : Event<ShowStoreGunDetailEvent, int> {};
		struct ShowStorePropDetailEvent : Event<ShowStorePropDetailEvent, int> {};
		struct UpdateStoreGunDataEvent : Event<UpdateStoreGunDataEvent, const String&> {};
		struct UpdateStorePropDataEvent : Event<UpdateStorePropDataEvent, const String&> {};

		struct ShowPixelGunHallInfo : Event<ShowPixelGunHallInfo, int, int, int, int, bool> {};
		struct ShowPixelGunGamePerson : Event<ShowPixelGunGamePerson, int, int, int> {};
		struct ShowPixelGunGame1v1 : Event<ShowPixelGunGame1v1, int, int, int, int> {};
		struct ShowPixelGunGameTeam : Event<ShowPixelGunGameTeam, int, int, int, int, int, int, int, int> {};

		struct OpenChestLotteryEvent : Event<OpenChestLotteryEvent> {};
		struct UpdateChestLotteryIntegralEvent :Event<UpdateChestLotteryIntegralEvent, int, int> {};
		struct ShowLotteryChestDetailEvent : Event<ShowLotteryChestDetailEvent, int> {};
		struct ShowChestRewardDetailEvent : Event<ShowChestRewardDetailEvent, int> {};
		struct ShowChestRewardResultEvent : Event<ShowChestRewardResultEvent, int> {};
		struct OpenLotteryChestEvent : Event<OpenLotteryChestEvent, int> {};
		struct UpdateLotteryChestDataEvent : Event<UpdateLotteryChestDataEvent, const String&> {};

		struct ShowPixelGunHallArmorUpgradeEvent : Event<ShowPixelGunHallArmorUpgradeEvent, const String &> {};

		struct OpenSeasonRankEvent : Event<OpenSeasonRankEvent> {};
		struct OpenSeasonRewardEvent : Event<OpenSeasonRewardEvent> {};
		struct UpdateSeasonRuleEvent : Event<UpdateSeasonRuleEvent, const String&> {};
		struct UpdateSeasonRankDataEvent : Event<UpdateSeasonRankDataEvent, int, const String&> {};
		struct UpdateCurrentSeasonInfoEvent : Event<UpdateCurrentSeasonInfoEvent, int, int, int, int> {};
		struct UpdateLastSeasonInfoEvent : Event<UpdateLastSeasonInfoEvent, int, int, int> {};

		struct UpdateDisarmamentStatusEvent : Event<UpdateDisarmamentStatusEvent, bool> {};
		struct TigerLotteryDataEvent : Event<TigerLotteryDataEvent, const String&> {};
		struct TigerLotteryResultEvent : Event<TigerLotteryResultEvent, const String&, const String&, const String&> {};
		struct HideAndSeekHallResult : Event<HideAndSeekHallResult, const String&> {};
		struct UpdateBirdPackEvent : Event<UpdateBirdPackEvent> {};
		struct ShowBirdInfoClickEvent : Event<ShowBirdInfoClickEvent, i64> {};
		struct BirdFoodItemClickEvent : Event<BirdFoodItemClickEvent, i32, i32> {};
		struct BirdFuseRemoveItemEvent : Event<BirdFuseRemoveItemEvent, i64> {};
		struct BirdRemoveFuseItemEvent : Event<BirdRemoveFuseItemEvent, i64> {};
		struct BirdFusePutItemEvent : Event<BirdFusePutItemEvent, i64, const String&> {};	
		struct UpdateBirdStoreEvent : Event<UpdateBirdStoreEvent> {};
		struct UpdateBirdPersonalShopInfoEvent : Event<UpdateBirdPersonalShopInfoEvent, const BirdGiftBag&, i32> {};
		struct UpdateBirdPersonalShopEvent : Event<UpdateBirdPersonalShopEvent> {};
		struct UpdateBirdAtlasEvent : Event<UpdateBirdAtlasEvent> {};	
		struct UpdateBirdActivityEvent : Event<UpdateBirdActivityEvent> {};		
		struct ShowBirdStoreEvent : Event<ShowBirdStoreEvent, i32, i32> {};
		struct ShowBirdStoreByNpcEvent : Event<ShowBirdStoreByNpcEvent, const String&> {};
		struct ShowBirdCommonTipEvent : Event<ShowBirdCommonTipEvent, const String&> {};
		struct ShowBirdLockOfMoneyEvent : Event<ShowBirdLockOfMoneyEvent, i32, i32> {};
		struct ShowBirdUnlockNestTipEvent : Event<ShowBirdUnlockNestTipEvent, i32, i32> {};
		struct ShowBirdExpandCarryTipEvent : Event<ShowBirdExpandCarryTipEvent, i32, i32> {};
		struct ShowBirdCarryFullTipEvent : Event<ShowBirdCarryFullTipEvent, i32, i32> {};
		struct ShowBirdCapacityFullTipEvent : Event<ShowBirdCapacityFullTipEvent, i32, i32> {};
		struct ShowBirdExpandCapacityTipEvent : Event<ShowBirdExpandCapacityTipEvent, i32, i32> {};
		struct ShowBirdLotteryTipEvent : Event<ShowBirdLotteryTipEvent, i32, i32> {};
		struct ShowBirdOpenChestTipEvent : Event<ShowBirdOpenChestTipEvent, i32, i32, i32> {};
		struct ShowBirdLotteryByNpcEvent : Event<ShowBirdLotteryByNpcEvent, i32> {};
		struct UpdateBirdLotteryEvent : Event<UpdateBirdLotteryEvent> {};
		struct ShowBirdTaskTipEvent : Event<ShowBirdTaskTipEvent, const String&> {};
		struct BirdTaskChangeEvent : Event<BirdTaskChangeEvent> {};
		struct ShowBirdGainTipEvent : Event<ShowBirdGainTipEvent, const vector<BirdGain>::type&> {};
		struct BirdPackOpenEvent : Event<BirdPackOpenEvent> {};
		struct BirdNestExpandEvent : Event<BirdNestExpandEvent> {};
		struct HideBirdActivityEvent : Event<HideBirdActivityEvent> {};
		struct ShowBirdLotteryResultTipEvent : Event<ShowBirdLotteryResultTipEvent, const String &, const String &, const BirdInfo &> {};
		struct BirdAddScoreEvent : Event<BirdAddScoreEvent, i32, i32> {};
		struct BirdBuffChangeEvent : Event<BirdBuffChangeEvent> {};
		struct BirdAcceptTaskEvent : Event<BirdAcceptTaskEvent> {};
		struct BirdBuyPermissionEvent : Event<BirdBuyPermissionEvent, const String &> {};
		struct ShowBirdPersonalShopEvent : Event<ShowBirdPersonalShopEvent, i32> {};

		struct ShowBirdDeleteTipEvent : Event<ShowBirdDeleteTipEvent, i64,const String&> {};
	}

	inline namespace KEY
	{
		struct BackKeyDownEvent : Event<BackKeyDownEvent> {};
	}

	inline namespace WORLD
	{
		struct PlayerDeathEvent : Event<PlayerDeathEvent, ui64> {};
		struct CraftingTableActivateEvent : Event<CraftingTableActivateEvent> {};
		struct ChestActivateEvent : Event<ChestActivateEvent, IInventory*, const Vector3i &, int, const Vector3 &> {};
		struct MerchantActivateEvent : Event<MerchantActivateEvent, EntityMerchant*> {};
		struct MerchantUpdateEvent : Event<MerchantUpdateEvent, EntityMerchant*> {};
		struct RankActivateEvent : Event<RankActivateEvent, const String&,int> {};
		struct ChickenAirDropPosition : Event<ChickenAirDropPosition, const Vector3 &> {};
		struct UpdateUnlockedCommodityEvent : Event<UpdateUnlockedCommodityEvent> {};
	}

	inline namespace CHAT
	{
		struct ChatMessageReceiveEvent :Event<ChatMessageReceiveEvent, const String&> {};
		struct TopSystemNotificationEvent :Event<TopSystemNotificationEvent, const String&, i32> {};
		struct BottomSystemNotificationEvent :Event<BottomSystemNotificationEvent, const String&, i32> {};
		struct CenterSystemNotificationEvent :Event<CenterSystemNotificationEvent, const String&, i32> {};
		struct TeamResourcesShowNotificationEvent : Event<TeamResourcesShowNotificationEvent, const String&, i32> {};
	}

	inline namespace MISC
	{
		struct ClientBlockChangeRecordClearEvent : Event<ClientBlockChangeRecordClearEvent, int, int> {};
	}
}
