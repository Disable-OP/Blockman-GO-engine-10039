/********************************************************************
filename: 	C2SPlayerActionPacketHandles
file path:	H:\sandboxol\client\blockmango-client\dev\server\src\Network\C2SPacketHandles\C2SPlayerActionPacketHandles.h

version:	1
author:		qutianxiang
company:    sandboxol
date:		2017/07/25
*********************************************************************/
#ifndef __PLAYERACTIONPACKETHANDLES_H__
#define __PLAYERACTIONPACKETHANDLES_H__
#include <memory>
#include "Network/protocol/C2SPackets.h"
#include "Network/protocol/S2CPackets.h"
#include "Inventory/IInventory.h"

class ClientPeer;
class C2SPlayerActionPacketHandles
{
public:
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketPlayerMovement>& packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketVehicleMovement>& packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketAnimate> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketAttack> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketCraftWithCraftingTable> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketCraftWithHand> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketPlayerAction> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketSwapItemPosition> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketDropItemPosition> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketEquipArmor> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketUnequipArmor> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketChangeItemInHand> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketUseItem> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketPlaceBlock> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketActivateBlock> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketExchangeItemStack> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketChatString> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketLogout> &packet);
	static void exchangeItemStack(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketExchangeItemStack> &packet, 
		BLOCKMAN::IInventory * initiatorInventory, BLOCKMAN::IInventory * targetInventory, bool isEnderChest);

	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketUseProp> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketPing>& packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketLaunchArrow> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketLaunchBullet>& packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketSpawnVehicle>& packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketGunFireResult>& packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketSplashPotion> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketCloseContainer> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketUseThrowableItem> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketSetFlying> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketBuyCommodity> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketTakeVehicle> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SReloadBullet> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketBuyGoods> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketResumeOrder> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketSelectRole> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketBuyVehicle> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketLockVehicle> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketClientReady> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketVehicleActionState> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketBuyRespawn> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketGoNpcMerchant> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketClickTeleport> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketChangeClothes> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketRestClothes> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketLeaveAircraft> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketFriendOperation> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketSetParachute> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketSkillType> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketClickChangeActor> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketChangePlayerActor> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketCameraLockMovement> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketCameraLock> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketPickUpItemPay> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketAttackActorNpc> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketGetManor> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketSellManor> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketUpgradeManor> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketBuyHouse> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketBuyFurniture> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketDeleteManorMessage> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketBuildHouse> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketPutFurniture> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketRecycleFurniture> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketManorTeleport> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketCallOnManor> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketGiveRose> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketRequestUpdateRealTimeRank> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketBuySwitchableProp> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketBuyUpgradeProp> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketAttackCreatureEntity> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketUpgradeResource> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketWatchRespawn> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketMovementInput> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketSendJumpChanged> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketSendPlayerRotation>& packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketSendDownChanged>& packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketNeedUpdateMerchantCommodities>& packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketSyncCustomTipResult> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketRelaseItemSkill> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketBuildWarOpenShop> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketBuildWarGrade> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketBuildWarOpenGuessResult> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketBuildWarGuessVisit> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketBuildWarGuessSuc> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketShowBuildWarResult> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketGetCommodity> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketUnlockCommodity> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketPlayerInteractionActionMesage> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketAttackEntity> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketConsumeTipResult> &packet); 
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketBuyEnchantmentProp> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketBuySuperProp> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketBuyCustomProp> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketClickBackHall> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketLongHitEntity> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketUseFishingRod> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketUseCannon> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketBuyBullet> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketRanchBuyItem> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketRanchBuyBuildItem> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketRanchOperation> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketRanchReceive> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketRanchExpand> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketRanchShortcut> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketRanchSellItem> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketRanchDestroyHouse> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketRanchBuyItems> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketRanchBuildTakeProduct> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketRanchBuildQueueOperation> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketRanchBuildRemove> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketRanchOrderOperate> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketRancherExBegin> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketRancherExTaskFinish> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketRanchGiftsOrInvite> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketRanchHelpFinish> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketReceiveMailReward> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketReceiveRanchAchievementReward> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketRanchGoExplore> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketChangeCurrentItemInfo> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketSpeedUpCrop> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketUseCubeFinshOrder> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketBuyFlyingPermissionResult> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketResetCloseup> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketKeepItem> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketOpenEnchantMent> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketEnchantmentEffectClick> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketEnchantmentQuick> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketSelectLottery> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketLotteryAgain> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketHallLotteryAgain> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketSetSpYaw> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketOpenSelectMode> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketPixelHallUnlockMap> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketUseStoreGun> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketBuyStoreGun> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketUpgradeStoreGun> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketQuickUpgradeStoreGun> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketUseStoreProp> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketBuyStoreProp> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketUnloadStoreProp> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketUpgradeStoreProp> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketQuickUpgradeStoreProp> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketEffectGunFire>& packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketEffectGunFireResult>& packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketEffectGunFireExplosionResult>& packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketStopLaserGun>& packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketConsumeBullet>& packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketFirstShowStoreGun> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketFirstShowStoreProp> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketPixelGunSelectBox> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketPixelGunExit> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketPixelGunRematch> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketPixelGunRevive> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketOpenLotteryChest> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketOpenArmorUpgrade> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketUpgradeArmor> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketPixelGunResultBack> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketPixelGunResultNext> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketPixelGunResultRevenge> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketPixelGunFight> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketBirdBagOperation> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketBirdStoreOperation> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketBirdPersonalStoreBuyGoods> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketBirdFuse> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketBirdOpenTreasureChest> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketBirdLottery> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketBirdTask> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketBirdFeed> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketBirdSetDress> &packet);
};
#endif
