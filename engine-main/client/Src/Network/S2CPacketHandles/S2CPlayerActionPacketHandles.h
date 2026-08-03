/********************************************************************
filename: 	S2CPlayerActionPacketHandles
file path:	H:\sandboxol\client\blockmango-client\dev\client\Src\Network\S2CPacketHandles\S2CPlayerActionPacketHandles.h

version:	1
author:		qutianxiang
company:    sandboxol
date:		2017/07/25
*********************************************************************/
#ifndef __S2CPLAYERACTIONPACKETHANDLES_H__
#define __S2CPLAYERACTIONPACKETHANDLES_H__

#include "Network/protocol/S2CPackets.h"
#include <memory>

class S2CPlayerActionPacketHandles
{
public:
	static void handlePacket(std::shared_ptr<S2CPacketRebirth>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketAnimate>&packet);
	static void handlePacket(std::shared_ptr<S2CPacketPlayerAction>&packet);
	static void handlePacket(std::shared_ptr<S2CPacketEquipArmors>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketUnequipArmor>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketDestroyArmor>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketChangeItemInHand>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketChangeCurrentItem>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketUpdateBlocks>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketChestInventory>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketUsePropResults>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketUserIn>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketUserOut>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketPing>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncWorldTime>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSettlement>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketGameover>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketCloseContainer>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketPlaySound>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketExplosion>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSetSignTexts>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSetAllowFlying> &packet); 
	static void handlePacket(std::shared_ptr<S2CPacketChangePlayerInfo> &packet);
	static void handlePacket(std::shared_ptr<S2CPacketGameStatus> &packet);
	static void handlePacket(std::shared_ptr<S2CPacketSetWatchMode> &packet);
	static void handlePacket(std::shared_ptr<S2CPacketUpdateMerchant> &packet);
	static void handlePacket(std::shared_ptr<S2CPacketUpdateMerchantInfo> &packet);
	static void handlePacket(std::shared_ptr<S2CPacketUpdateMerchantCommodities> &packet);
	static void handlePacket(std::shared_ptr<S2CPacketSetFlying> &packet);
	static void handlePacket(std::shared_ptr<S2CPacketPlayerSettlement>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketGameSettlement>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketUserAttr>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketDebugString>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketPlayerLifeStatus>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncWorldWeather>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketBedDestroy>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketRespawnCountdown>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketAllPlayerTeamInfo>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketChangePlayerTeam>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketReloadBullet>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketPlayerChangeActor>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketPlayerRestoreActor>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketUpdateShop>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketUpdateShopGoods>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketShoppingResult>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketInitMoney>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncGunFireResult>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketChangeMaxHealth>& packet); 
	static void handlePacket(std::shared_ptr<S2CPacketUpdateCurrency>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncTakeVehicle>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSelectRoleData>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncPlayerOwnVehicle>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncLockVehicle>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncVehicleState>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncVehicleActionState>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketNotifyGetMoney>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketNotifyGetItem>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketBuyCommodityResult>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketShowBuyRespawn>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketBuyRespawnResult>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketShowGoNpcMerchant>& packet); 
	static void handlePacket(std::shared_ptr<S2CPacketUpdateBackpack>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketRankData>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketActivateTrigger>& packet); 
	static void handlePacket(std::shared_ptr<S2CPacketUpdateRankNpc>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketTakeAircraft>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketAircraftStartFly>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncAircraftState>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketFriendOperation>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketChangeAircraftUI>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketAirdrop>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketMemberLeftAndKill>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncBlockDestroyeFailure>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSkillType>& packet);
	static void handlePacket(std::shared_ptr<S2CSyncChangePlayerActorInfo>& packet);
	static void handlePacket(std::shared_ptr<S2CSyncNpcActorInfo>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketHideAndSeekBtnStatus>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketEntitySkillEffect>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketPickUpItemOrder>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketRequestResult>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncGameTimeShowUi>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketDeathUnequipArmor>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncCameraLock>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncChangeEntityPerspece>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketPlayerInvisible>& packet); 
	static void handlePacket(std::shared_ptr<S2CPacketPlayerNamePerspective>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketChangeHeart>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncSessionNpc>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketUpdateManor>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketUpdateRealTimeRankInfo>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketUpdateGameMonsterInfo>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketUpdateBossBloodStrip>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSwitchablePropsData>& packet); 
	static void handlePacket(std::shared_ptr<S2CPacketUpgradePropsData>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketPersonalShopTip>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketCreateHouseFromSchematic>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketPlayerSpeedLevel>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketPlayerExpInfo>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketFillAreaByBlockIdAndMate>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketShowPlayerOperation>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketShowUpgradeResourceUI>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketManorBtnVisible>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketCallOnManorResetClient>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketUpdateManorOwner>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketUpdateBasementLife>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketUpdateChestInventory>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketCustomTipMsg>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketEntityPlayerOccupation>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketShowBuildWarBlockBtn>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketShowBuildWarPlayNum>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketShowBuildWarLeftTime>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketOpenShopByEntityId>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketShowGrade>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketShowGuessResult>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketShowGuessUi>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketGameSettlementExtra>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketUnlockedCommodity>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketInitUnlockedCommodity>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketTeamResourcesUpdate>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketBuildProgress>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketCloseBGM>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSetArmItem>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncShowMaskTime>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketEnchantmentPropsData>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketConsumeCoinTip>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSuperPropsData>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSuperShopTip>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketChangeDefense>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketChangeAttack>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketAddSimpleEffect>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncEffectGunFire>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncEffectGunFireAtEntityResult>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncEffectGunFireAtBlockResult>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncEffectGunFireExplosionResult>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketStopLaserGun>& packet);

	static void handlePacket(std::shared_ptr<S2CPacketShowPersonalShop>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketPersonalShopArea>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketAddGunBulletNum>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSubGunRecoil>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketCustomPropsData>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketUpdateCustomProps>& packet);
	static void handlePacket(std::shared_ptr<S2CSyncCannonActorInfo>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncCannonFire>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketShowRanchExTip>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketShowRanchExTask>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketBroadcastMessage>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketGotoOtherGame>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketShowRanchExCurrentItemInfo>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketShowRanchExItem>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketCropInfo>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketOccupationUnlock>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketBuyFlying>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketPlayCloseup>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketShowBuyKeepItemTip>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSendOpenEnchantment>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSubGunFireCd>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketLotteryData>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketLotteryResult>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketHideAndSeekHallResult>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSendOpenPixelGunHallModeSelect>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketUpdateStoreGunData>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketUpdateStorePropData>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketShowGunStore>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketShowPixelGunHallInfo>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSendOpenPixelGun1v1>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketShowChestLottery>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSendOpenPixelGunRevive>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketChestLotteryResult>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketUpdateLotteryChestData>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketShowPixelGunGamePerson>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketShowPixelGunGame1v1>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketShowPixelGunGameTeam>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSendOpenPixelGunResult>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketPlayerCurrentSeasonInfo>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketPlayerLastSeasonInfo>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketDisarmament>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSetPlayerOpacity>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSendOpenPixelGunLvUp>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSendOpenPixelGunResultSpecialInfo>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketNotifyGetGoods>& packet);	
	static void handlePacket(std::shared_ptr<S2CPacketShowSeasonRank>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketAddCustomEffect>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncCustomEffect>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncPlayerBagInfo>& packet);

	static void HandleUpdateBlock(const NETWORK_DEFINE::S2CPacketUpdateBlockInfo& blockInfo);
	static void handlePacket(std::shared_ptr<S2CPacketSendOpenArmorUpgrade>& packet);
};
#endif
