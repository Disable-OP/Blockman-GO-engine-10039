/********************************************************************
filename: 	ClientPacketSender
file path:	H:\sandboxol\client\blockmango-client\dev\client\Src\Network\ClientPacketSender.h

version:	1
author:		qutianxiang
company:    sandboxol
date:		2017/06/29
*********************************************************************/
#ifndef __CLIENTPACKETSENDER_H__
#define __CLIENTPACKETSENDER_H__

#include "Entity/Entity.h"
#include "Network/CommonPacketSender.h"
#include "Chat/ChatComponent.h"
#include "Enums/Enums.h"
#include "World/Manor.h"
#include "World/Ranch.h"

using namespace BLOCKMAN;

class ClientPacketSender :public CommonPacketSender
{
public:
	ClientPacketSender();
	~ClientPacketSender();

	void sendLogin();
	void sendRebirth();
	void sendAnimate(int32_t entityRuntimeId, NETWORK_DEFINE::PacketAnimateType animateType, int32_t value = 0);
	void sendAttack(int targetEntityRuntimeId, real32 targetX, real32 targetY, real32 targetZ);
	void sendPlayerMovement(bool onGround);
	void sendPlayerMovement(bool onGround, float yaw, float pitch);
	void sendPlayerMovement(bool onGround, float x, float y, float z);
	void sendPlayerMovement(bool onGround, float x, float y, float z, float yaw, float pitch);
	void sendVehicleMovement(int vehicleId, bool onGround, bool moved, bool rotated, float x, float y, float z, float yaw);
	void sendCraftWithCraftingTable(int recipeId, Vector3i craftingTablePositoin);
	void sendCraftWithHand(int recipeId);
	void sendPlayerAction(NETWORK_DEFINE::PacketPlayerActionType action);
	void sendUseItem(NETWORK_DEFINE::C2SPacketUseItemUseType useType, const Vector3i& pos = Vector3i(0,0,0), int param0 = 0);
	void sendSwapItemPosition(int index1, int index2);
	void sendDropItemPosition(int inventoryIndex);
	void sendEquipArmor(int slotIndex);
	void sendUnequipArmor(int slotIndex);
	void sendChangeItemInHand(int slotIndex);
	void sendStartBreakBlock(int x, int y, int z);
	void sendStartBreakBlock(const Vector3i& position)
	{
		sendStartBreakBlock(position.x, position.y, position.z);
	}
	void sendAbortBreakBlock();
	void sendFinishBreakBlock();
	void sendPlaceBlock(const Vector3i& blockPos, int face, const Vector3 & hisPos);
	void sendActivateBlock(const Vector3i& blockPos, int face, const Vector3 & hisPos);
	void sendExchangeItemStack(bool  isPlayerInventoryToChest, i32 touchTime, int index, Vector3i blockPos, int face, Vector3  hisPos);
	void sendChatString(String msg);
	void sendUseProp(String propId);
	void sendPing();
	void sendLaunchArrow(float cdTime, const Vector3& playerPos, float playerYaw, float playerPitch,int arrowID);
	void sendLaunchBullet(int bulletID, const Vector3& playerPos, float playerYaw, float playerPitch);
	void sendSpawnVehicle(int carID, const Vector3& pos, float yaw);
	void sendGunFireResult(const Vector3& pos, const Vector3& dir, int tracyType, const Vector3i& blockPos, const Vector3& hitpos, int hitEnttityID, bool isNeedConsumeBullet, bool isHeadshot);
	void sendSplashPotion(const Vector3& playerPos, float playerYaw, float playerPitch);
	void sendCloseContainer(const Vector3i& containerPos);
	void sendUseThrowableItem();
	void sendSetFlying(bool isFlying);
	void sendSetSpYaw(bool isSpYaw, float radian);
	void sendBuyCommodity(int merchantId, int groupIndex, int commodityIndex, int commodityId);
	void sendTakeVehicle(bool onoff, int carID);
	void sendCheckCSVersion();
	void sendReloadBullet();
	void sendBuyGoods(int groupIndex, int goodsIndex, int goodsId);
	void sendResumeOrder(String orderId);
	void sendSelectRole(int role);
	void sendBuyVehicle(int carId);
	void sendLockVehicle(bool onoff, int vehicleId);
	void sendClientReady();
	void sendVehicleActionState(int vehicleId, int state);
	void sendBuyRespawn(bool isRespawn, i32 uniqueId, i32 index);
	void sendKeepItem(bool isKeepItem);
	void sendEnchantmentQuick(int equipId, int effectIndex);
	void sendPixelHallUnlockMap(int num, int mapId);
	void sendGoNpcMerchant(float x, float y, float z, float yaw);
	void sendClickTeleport();
	void sendLeaveAircraft();
	void sendChangeClothes(int actorId, String partName);
	void sendRestClothes();
	void sendFriendOperation(int operationType, ui64 userId);
	void sendSetParachute(bool isParachute);
	void sendSkillType(BLOCKMAN::SkillType skill , bool isRelease);
	void sendChangePlayerActor();
	void sendClickChangeActor();
	void sendCameraLockMovement(Vector3& pos);
	void sendCameraLock(bool bLock);
	void sendPickUpItemPay(i32 itemEntityId, i32 itemId);
	void sendAttackActorNpc(i32 entityId);

	void sendGetManor();
	void sendSellManor();
	void sendUpgradeManor();

	void sendBuyHouse(BLOCKMAN::ManorHouse* house);
	void sendBuyFurniture(BLOCKMAN::ManorFurniture* furniture);
	void sendDeleteManorMessage(BLOCKMAN::ManorMessage* message);

	void sendBuildHouse(BLOCKMAN::ManorHouse* house);
	void sendPutFurniture(i32 furnitureId, BLOCKMAN::PutFurnitureStatus status);
	void sendRecycleFurniture(BLOCKMAN::ManorFurniture* furniture);
	void sendManorTeleport();
	void sendCallOnManor(ui64 targetUserId);
	void sendGiveRose(ui64 targetUserId);

	void sendRequestUpdateRealTimeRank();
	void sendBuySwitchableProp(String uniqueId);
	void sendBuyUpgradeProp(String uniqueId);
	void sendAttackCreatureEntity(i32 entityId);

	void sendCustomTipResult(String extra, bool bResult);
	void sendRelaseItemSkill(int skillItemId);

	void sendUpgradeResource(i32 resourceId);
	void sendWatchRespawn();
	void sendMovementInputChanged(float forward, float strafe);
	void sendJumpChanged(bool isJump);
	void sendPlayerRotationChanged(float yaw, float pitch);
	void sendDownChanged(bool isDown);
	void sendNeedUpdateMerchantCommodities(int merchantId);

	void sendBuildWarOpenShop();
	void sendGradeScore(int grade);
	void sendBuildWarOpenGuessResult();
	void sendBuildWarResult();
	void sendBuildWarGuessVisit(ui64 platformUserId);
	void sendBuildWarGuessSuc(int guessRoomId);
	void sendGetCommodity(int merchantId, int groupIndex, int commodityIndex, int commodityId);
	void sendUnlockCommodity(int merchantId, int groupIndex, int commodityIndex, int commodityId);
	
	void sendPlayerInteractionActionMesage(ui64 targetUserId, String langKey, LORD::vector<String>::type args);
	void sendAttackEntity(i32 entityId);
	void sendConsumeTipResult(String extra, bool bResult);
	void sendBuyEnchantmentProp(String uniqueId);
	void sendBuySuperProp(String uniqueId);
	void sendBuyCustomProp(String uniqueId);
	void sendClickBackHall();
	void sendLongHitEntity(i32 entityId);
	void sendUseFishingRod();
	void sendUseCannon(i32 entityId);
	void sendUpdateUserMoney();
	void sendBuyBullet();
	void sendBuyRanchItem(i32 itemId, i32 num, i32 totalPrice, i32 currencyType);
	void sendBuyRanchBuildItem(i32 itemId, i32 num, i32 type, i32 currencyType);
	void sendRanchOperation(BLOCKMAN::RanchOperationType type);
	void sendRanchReceive(ui64 referrer);
	void sendRanchExpand(i32 landCode);
	void sendRanchShortcut(i32 landCode, i32 moneyType);
	void sendRanchDestroyHouse();
	void sendRanchSellItem(i32 itemId, i32 num);
	void sendRanchBuyItems(const vector<RanchCommon>::type items);
	void sendRanchBuildTakeProduct(i32 entityId, i32 itemId, i32 index);
	void sendRanchBuildQueueOperation(i32 type, i32 entityId, i32 queueId, i32 productId);
	void sendRanchBuildRemove(i32 actorId);
	void sendRanchOrderOperate(i32 type, i32 orderId, i32 index);
	void sendRanchExBegin();
	void sendRanchExTaskFinish(int itemId, int itemNum);
	void sendRanchGiftsOrInvite(int type, i64 targetUserId);
	void sendRanchHelpFinish(i64 helpId, i32 type);
	void sendReceiveMailReward(i64 mailId);
	void sendReceiveRanchAchievementReward(i64 achievementId);
	void sendRanchGoExplore();
	void sendChangeCurrentItemInfo(int itemIndex);
	void buildNotification(i64 userId, const String& message) override;
	void sendSpeedUpCrop(Vector3i blockPos, i32 blockId);
	void sendUseCubeFinshOrder(i32 orderId, i32  index);
	void sendBuyFlyingPermissionResult();
	void sendResetCloseup(const Vector3& playerPos);
	void sendOpenEnchantMent();
	void sendEnchantmentEffectClick(int equipId, int consumeId, int effectIndex);
	void sendSelectLotteryItem(String uniqueId);
	void sendLotteryAgain();
	void sendHallLotteryAgain(int luck_times, int entityId);

	void sendOpenModeSelect();
	
	void sendUseStoreGun(int gunId);
	void sendBuyStoreGun(int gunId);
	void sendUpgradeStoreGun(int gunId);
	void sendQuickUpgradeStoreGun(int gunId);
	void sendUseStoreProp(int propId);
	void sendUnloadStoreProp(int propId);
	void sendBuyStoreProp(int propId);
	void sendUpgradeStoreProp(int propId);
	void sendQuickUpgradeStoreProp(int propId);
	void sendFirstShowStoreGun(int gunId);
	void sendFirstShowStoreProp(int propId);

	void sendEffectGunFire(bool isNeedConsumeBullet, const Vector3& beginPos, const Vector3& endPos);
	void sendEffectGunFireResult(int hitEntityId, const Vector3& hitPos, const Vector3& dir, int gunId, int traceType, const Vector3i& blockPos, bool isHeadshot = false);
	void sendEffectGunFireExplosionResult(const Vector3& hitPos, float explosionRange);
	void sendStopLaserGun();
	void sendConsumeBullet();
	void sendPixelGunSelectBox(int index);
	void sendPixelGunExit();
	void sendPixelGunRematch();
	void sendPixelGunRevive();
	void sendOpenLotteryChest(int chestId);
	void sendOpenArmorUpgrade();
	void sendUpgradeArmor(bool state,int operateType);
	void sendPixelGunResultBack();
	void sendPixelGunResultNext();
	void sendPixelGunResultRevenge();
	void sendPixelGunFight(String gameType, String mapName);

	void sendBirdBagOperation(i64 birdId, i32 operationType);
	void sendBirdStoreOperation(i32 storeId, i32 goodsId, i32 operationType);
	void sendBirdPersonalStoreBuyGoods(i32 tabId, i32 goodsId);
	void sendBirdFuse(const vector<i64>::type& birdIds);
	void sendBirdOpenTreasureChest(i32 entityId);
	void sendBirdLottery(i32 entityId);
	void sendBirdTask(i32 entityId, i32 taskId);
	void sendBirdFeed(i64 birdId, i32 foodId, i32 num);
	void sendBirdSetDress(i64 birdId, i32 dressType, i32 dressId);

	// Server-authoritative world generation: ask the server for one chunk.
	// Called by ChunkServiceClient when a cache miss would otherwise return
	// NonexistentChunk (i.e., the local Anvil dir doesn't have the chunk).
	// The server replies with S2CPacketChunkData, which the client-side
	// S2CChunkPacketHandles decodes and injects into the chunk cache.
	void sendRequestChunk(int32_t chunkX, int32_t chunkZ);
};

#endif
