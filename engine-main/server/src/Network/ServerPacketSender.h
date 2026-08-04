/********************************************************************
filename: 	PacketSender
file path:	H:\sandboxol\client\blockmango-client\dev\server\src\Network\PacketSender.h

version:	1
author:		qutianxiang
company:    sandboxol
date:		2017/06/27
*********************************************************************/
#ifndef __PACKETSENDER_H__
#define __PACKETSENDER_H__
#include "Global.h"
#include "Network/CommonPacketSender.h"
#include "Entity/Entity.h"
#include "BM_Container_def.h"
#include "Inventory/InventoryPlayer.h"
#include "Inventory/IInventory.h"
#include "Chat/ChatComponent.h"
#include "Enums/Enums.h"
#include "World/Manor.h"
#include "Entity/EntityActorNpc.h"
#include "Entity/EntitySessionNpc.h"
#include "Entity/EntityCreature.h"
#include "Entity/EntityActorCannon.h"

namespace BLOCKMAN
{
	class EntityPlayerMP;
	class TileEntitySign;
	class EntityMerchant;
	class EntityBlockman;
	class EntityBlockmanEmpty;
	class EntityBlockman; 
	class EntityBlockmanEmpty;
	class EntityBuildNpc;
	class EntityLandNpc;
	class RanchCommon;
	class EntityBulletin;
	class EntityBird;
	class BirdGain;
	class BirdInfo;
	class BirdTask;
	class BirdStore;
	class BirdPersonalStoreTab;
}
struct UserAttrInfo;

class ServerPacketSender : public CommonPacketSender
{
public:
	ServerPacketSender();
	~ServerPacketSender();

	void sendPacketToTrackingPlayers(int trackedEntityId, std::shared_ptr<DataPacket> packet, bool includeSelf = false);

	void sendPlayerActionToTrackingPlayers(NETWORK_DEFINE::PacketPlayerActionType action, int entityId, bool includeSelf = false);
	virtual void sendEntityAnimateToTackingPlayers(NETWORK_DEFINE::PacketAnimateType animateType, int entityId, int32_t value = 0);
	void broadCastUpdateBlock(int x, int y, int z, int entityId = -1)
	{
		broadCastUpdateBlock(Vector3i(x, y, z), entityId);
	}
	void broadCastUpdateBlock(Vector3i position, int entityId = -1);

	void sendAddEntityEffectToTrackingPlayers(int entityId, BLOCKMAN::PotionEffect * effect);
	void sendAddEntityEffect(BLOCKMAN::EntityPlayerMP* targetEntity, int entityId, BLOCKMAN::PotionEffect * effect);

	void sendRemoveEntityEffectToTrackingPlayers(int entityId, int potionId);

	// runtimeID 
	void sendGameInfo(ui64 rakssid, int runtimeID, const Vector3i& spawnPos, const vector<String>::type& recipes, bool isTimeStopped, int32_t sex, String gameType, int32_t defaultIdle);
	void sendUserAttr(ui64 rakssid, int runtimeID, const UserAttrInfo& attr);
	void sendUserAttrToTrackingPlayers(int entityId, const UserAttrInfo& attr);

	void sendSyncWorldTime(ui64 rakssid, int64_t time);
	void broadcastSyncWorldTime(int64_t time);

	void sendSyncWorldWeather(ui64 rakssid, String weather, int time);
	void broadcastSyncWorldWeather(String weather, int time);

	void sendUpdateBlocks(ui64 rakssid);
	void sendUpdateBlocksWithDistance(ui64 rakssid, const std::vector<NETWORK_DEFINE::S2CPacketUpdateBlockInfo>& block_info);

	void resetUpdateBlocks();
	void sendPlayerInventory(ui64 rakssid, BLOCKMAN::InventoryPlayer * inventory);

	void sendRebirth(ui64 rakssid, int runtimeID, const Vector3i& spawnPos, int sex, const String& showName);

	void sendEntityAttribute(BLOCKMAN::EntityPlayerMP * targetEntity, BLOCKMAN::EntityPlayerMP * changedEntity);

	void sendEntityAttributeToTrackingPlayers(BLOCKMAN::EntityPlayerMP * pEntity);

	void sendEntityHeadRotationToTrackingPlayers(const BLOCKMAN::EntityPlayers & trackingPlayers, int entityId, real32 yaw);

	void sendEntityVelocity(ui64 rakssid, BLOCKMAN::Entity * pEntity);

	void sendEntityVelocityToTrackingPlayers(const BLOCKMAN::EntityPlayers & trackingPlayers, BLOCKMAN::Entity * pEntity);

	void sendEntityVelocityToTrackingPlayers(const BLOCKMAN::EntityPlayers& trackingPlayers, int entityId, float motionX, float motionY, float motionZ);

	void sendEntityTeleport(ui64 rakssid, int entityId, int x, int y, int z, real32 yaw, real32 pitch);

	void sendEntityTeleportToTrackingPlayers(const BLOCKMAN::EntityPlayers & trackingPlayers, int entityId, int x, int y, int z, real32 yaw, real32 pitch);

	void sendEntityMovementToTrackingPlayers(const BLOCKMAN::EntityPlayers & trackingPlayers, int entityId, i16 x, i16 y, i16 z, real32 yaw, real32 pitch, bool immdi = true);

	void sendEntityMovementToTrackingPlayers(const BLOCKMAN::EntityPlayers & trackingPlayers, int entityId, real32 yaw, real32 pitch, bool immdi = true);

	void sendEntityMovementToTrackingPlayers(const BLOCKMAN::EntityPlayers & trackingPlayers, int entityId, i16 x, i16 y, i16 z, bool immdi = true);

	void sendPlayerChangeItem(ui64 rakssid, int itemIndex);

	void sendEntitySpawn(ui64 rakssid, BLOCKMAN::Entity * myEntity);
	void sendEquipArmors(BLOCKMAN::EntityPlayerMP* targetEntity, int entityId, const vector<BLOCKMAN::ItemStackPtr>::type & armors);
	void sendEquipArmorsToTrackingPlayers(int entityId, const vector<BLOCKMAN::ItemStackPtr>::type & armors);
	void sendUnequipArmorToTrackingPlayers(int entityId, int slotIndex);
	void sendDestroyArmorToTrackingPlayers(int entityId, int slotIndex) override;
	void sendChangeItemInHand(BLOCKMAN::EntityPlayerMP* targetEntity, int entityId, BLOCKMAN::ItemStackPtr itemStack);
	void sendChangeItemInHandToTrackingPlayers(int entityId, BLOCKMAN::ItemStackPtr itemStack);

	void sendRemoveEntity(int removedEntityId, BLOCKMAN::EntityPlayerMP* targetEntity);
	void sendRemoveEntityToTrackingPlayers(int entityID);
	void sendSystemsChat(String key, String playerName, int chatType = 0);
	void sendChat(std::shared_ptr<ChatComponentString> chatString);
	void sendChat(ui64 targetPlayerId, std::shared_ptr<ChatComponentString> chatString);
	void sendChat(std::shared_ptr<ChatComponentTranslation> chatTranslation);
	void sendChat(ui64 targetPlayerId, std::shared_ptr<ChatComponentTranslation> chatTranslation);
	void sendNotification(std::shared_ptr<ChatComponentNotification> notification);
	void sendNotification(ui64 rakssid, std::shared_ptr<ChatComponentNotification> notification);
	void sendChestInventory(BLOCKMAN::EntityPlayer* player, Vector3i blockPos, int face, Vector3 hitPos, BLOCKMAN::IInventory* inventory) override;
	void sendUpdateChestInventory(Vector3i blockPos, int face, Vector3 hitPos, BLOCKMAN::IInventory* inventory);
	void sendUpdateEnderChestInventory(ui64 rakssid, Vector3i blockPos, int face, Vector3 hitPos, BLOCKMAN::IInventory* inventory);
	void sendUsePropResults(ui64 rakssId, String propId, int results);
	void sendUserIn(ui64 rakssId, ui64 platformUserId, String nickName, int teamId, String teamName, int curPlayer, int maxPlayer);
	void sendUserOut(ui64 platformUserId);
	void sendLoginResult(ui64 rakssId, int32_t resultCode);
	void sendPing(ui64 rakssId, ui32 sendTime);
	void sendSettlement(ui64 rakssId, String gameType, String result);
	void sendGameover(ui64 rakssId, String overMessage, int code);
	void sendCloseContainer(int playerID, bool close, const Vector3i& containerPos);
	void sendPlayerAction(ui64 rakssId, int playerId, NETWORK_DEFINE::PacketPlayerActionType action);
	void sendPlaySound(BLOCKMAN::EntityPlayer* player, uint soundType);
	void sendExplosion(BLOCKMAN::EntityPlayer* player, const Vector3& pos, float size, const BLOCKMAN::BlockPosArr& poss, const Vector3& motion, bool isSmoke, float custom_size = 1.0f) override;
	//void sendPlaySound(ui64 rakssId, const String& soundName, float volumn, float pitch);
	void sendPlaySoundByType(ui64 rakssId, uint soundType);
	//void broadCastPlaySound(const String& soundName, float volumn, float pitch);
	void broadCastPlaySoundByType(uint soundType);
	void broadCastPlaySoundByTypeAtEntity(uint soundType, i32 entityId);
	void sendAllSetSignTexts(ui64 rakssId);
	void broadcastSetSignTexts(BLOCKMAN::TileEntitySign* tileEntitySign);
	void sendSetSignTexts(BLOCKMAN::TileEntitySign* tileEntitySign, ui64 rakssId);
	void sendSetAllowFlying(ui64 rakssId, bool allowFlying);
	void sendSetWatchMode(ui64 rakssId, int entityId, bool isWatchMode);
	void sendSetWatchModeToTrackingPlayers(int entityId, bool isWatchMode);
	void sendChangePlayerInfo(ui64 rakssId, BLOCKMAN::EntityPlayer* player);
	void broadCastChangePlayerInfo(BLOCKMAN::EntityPlayer* player);
	void sendGameStatus(ui64 rakssId, int status);
	void sendUpdateMerchant(ui64 rakssid, int entityId, int index);
	void sendUpdateMerchantInfo(ui64 rakssid, int entityId);
	void sendUpdateMerchantCommodities(ui64 rakssid, int entityId, int index);
	void sendSetFlying(ui64 rakssId, BLOCKMAN::EntityPlayer* player);
	void sendSetFlyingToTrackingPlayers(BLOCKMAN::EntityPlayer* player);
	void sendDebugString(ui64 rakssId, const String& str);
	void sendPlayerSettlement(ui64 rakssId, String result, bool8 isNextServer);
	void sendGameSettlement(ui64 rakssId, String result, bool8 isNextServer);
	void broadCastPlayerLifeStatus(ui64 platformUserId, bool isLife);
	void broadCastBedDestroy(int teamId);
	void sendRespawnCountdown(ui64 rakssId, ui32 second);
	void broadAllPlayerTeamInfo(String result);
	void sendChangePlayerTeam(ui64 rakssId, ui64 platformUserId, int teamId);
	void broadCastChangePlayerTeam(ui64 platformUserId, int teamId);
	void sendReloadBullet(ui64 rakssId);
	void sendPlayerChangeActor(ui64 rakssId, const BLOCKMAN::EntityPlayerMP* pPlayer);
	void broadCastPlayerChangeActor(BLOCKMAN::EntityPlayer* pPlayer);
	void broadCastPlayerRestoreActor(int entityId);
	void sendUpdateShop(ui64 rakssId);
	void broadCastUpdateShop();
	void broadCastUpdateShopGoods(int groupIndex, int goodsIndex, int limit);
	void sendShoppingResult(ui64 rakssId, int code, String result);
	void sendInitMoney(ui64 rakssId, ui64 diamonds, ui64 golds);
	void broadCastPlayerGunGire(i32 entityId, i32 gunId, const C2SPACKET_DETAIL::Vector3& pos, const C2SPACKET_DETAIL::Vector3& dir,
		ui32 tracyType, const C2SPACKET_DETAIL::Vector3i& blockPos, const C2SPACKET_DETAIL::Vector3& hitpos, ui32 hitEntityID, bool isHeadshot);
	void broadCastChangePlayerMaxHealth(i32 entityId, float health);
	void sendChangePlayerMaxHealth(ui64 rakssId, i32 entityId, float health);
	void sendPlayerUpdateCurrency(ui64 rakssId, ui64 currency);
	void broadCastSyncTakeVehicle(int playerId, bool onoff, bool isDriver, int vehicleId);
	void sendUserAttrFromEntityToTrackingPlayers(BLOCKMAN::EntityPlayer* player);
	void sendUserAttrFromEntity(ui64 rakssId, BLOCKMAN::EntityPlayer* player);
	void sendSelectRoleData(ui64 rakssId, String result);
	void sendPlayerOwnVehicle(ui64 rakssId, const std::set<int> & ownVehicle);
	void broadCastSyncLockVehicle(int playerId, bool onoff, int vehicleId);
	void sendSyncVehicleState(ui64 rakssId, int entityId);
	void syncPlayerTakeOnVehicle(ui64 rakssId, int playerId, bool isDriver, int vehicleId);
	void broadCastVehicleActionState(int playerId, int vehicleId, int state);
	void notifyGetMoney(ui64 rakssId, int count);
	void notifyGetItem(ui64 rakssId, int itemId, int meta, int count);
	void sendBuyCommodityResult(ui64 rakssId, int code, String result);
	void sendShowBuyRespawn(ui64 rakssId, i32 times);
	void sendBuyRespawnResult(ui64 rakssId, i32 code, String result);
	void sendShowGoNpcMerchant(ui64 rakssId, i32 entityId, float x, float y, float z, float yaw);
	void sendUpdateBackpack(ui64 rakssId, int capacity, int maxCapacity);
	void sendRankData(ui64 rakssId, i32 entityId, String result);
	void sendUpdateRankNpc(ui64 rakssId, int entityId);
	void sendActivateTrigger(ui64 rakssId, const Vector3i& blockPos, int blockId, int triggerIndex);
	void broadCastSyncBlock();
	void broadcastTakeAircraft(int playerId, bool onoff, int aircraftId);
	void syncPlayerTakeAircraft(ui64 rakssId, int playerId, int aircraftId);
	void broadCastAircraftStartFly(int entityId);
	void syncAircraftState(ui64 rakssId, int entityId);
	void sendChangeAircraftUI(ui64 rakssId, bool isShowUI);
	void broadCastChangeAircraftUI(bool isShowUI);
	void sendAirDropPosition(Vector3 position);
	void broadCastPoisonCircleRange(vector<Vector2>::type range, float speed);
	void sendMemberLeftAndKill(ui64 rakssId, int left, int kill);
	void sendBlockDestroyeFailure(ui64 rakssId, bool destroyeStatus, int destroyeFailureType);
	void sendSkillType(ui64 rakssId, int skillType, float duration, float coldDownTime);
	void syncChangePlayerActorInfo(ui64 rakssId, int changeActorCount, int needMoneyCount);
	void sendNpcActorInfo(i64 rakssId, BLOCKMAN::EntityActorNpc* pNpc);
	void sendPickUpItemOrder(ui64 rakssId, i32 itemEntityId, i32 itemId, i32 pickUpPrice, i32 moneyType);
	void sendRequestResult(ui64 rakssId, i32 itemId, BLOCKMAN::ResultCode resultCode, String resultMessage);
	void sendShowHideAndSeekBtnStatus(i64 rakssId, bool isActoBtnStatus, bool isCameraBtnStatus, bool isOtherBtnStatus);
	void sendSyncGameTimeShowUi(ui64 rakssId, bool isShowUI,int time);
	void broadCastGameTimeShowUi(bool isShowUI, int time);
	void createArrowTipMark(ui64 rakssId, Vector3 sourcePostion, Vector3 targetPostion, int  destoryTime, float distance);
	void broadCastCameraLockToClient(bool isCameraLock, int entityId);
	void SyncCameraLockStatus(ui64 rakssId, int playerId, bool isCameraLock);
	void sendDeathUnequipArmor(i32 entityId);
	void sendChangeEntityPerspece(ui64 rakssId, int View);
	void sendPlayerInvisible(ui64 rakssId, i32 entityId, bool isInvisible);
	void broadCastPlayerInvisible(i32 entityId, bool isInvisible);
	void sendPlayerChangeHeart(ui64 rakssId, int hp, int maxHp);
	void sendPlayerBossStripWithIcon(ui64 rakssId, String name, int curProgress, int maxProgress, String iconPath);

	void syncSessionNpc(ui64 rakssId, BLOCKMAN::EntitySessionNpc* npc);
	void sendPlayerChangeDefense(ui64 rakssId, int defense, int maxDefense);
	void sendPlayerChangeAttack(ui64 rakssId, int attack, int maxAttack);

	void sendUpdateManor(BLOCKMAN::EntityPlayerMP* player);
	void sendCreateHouseFromSchematic(const String& fileName, Vector3i start_pos, bool xImage = false, bool zImage = false, bool createOrDestroy = true);
	void sendShowPlayerOperation(ui64 rakssId, ui64 targetUserId);
	void sendManorBtnVisible(ui64 rakssId, bool visible);
	void sendPlayerInventory(int entityId);
	void sendCallOnManorResetClient(ui64 rakssId, ui64 targetUserId);
	void sendUpdateManorOwner(ui64 rakssid);
	void sendUpdateRealTimeRankInfo(ui64 rakssId, String result);
	void broadCastGameMonsterInfo(int currCheckPointNum, int surplusMonsterNum, int surplusCheckPointNum);
	void broadCastBossBloodStrip(int currBloodStrip, int maxBloodStrip);
	void sendSwitchablePropsData(ui64 rakssId, String data);
	void sendUpgradePropsData(ui64 rakssId, String data);
	void sendPersonalShopTip(ui64 rakssId, String tip);
	void sendUpdateEntityCreature(i64 rakssId, BLOCKMAN::EntityCreature* creature);
	void sendCreatureEntityAction(int entityId, int actionStatus, int baseAction, int uperBaseAction);
	void sendPlayerSpeedAdditionLevel(i64 rakssId, int speed);
	void sendPlayerExpInfo(i64 rakssId, int level, float exp, int maxExp);
	void sendfillAreaByBlockIdAndMate(Vector3i start_pos, Vector3i end_pos, int block_id, int mate = 0);
	void sendEntitySkillEffect(Vector3 position, String name, float duration, int width, int height, Vector3 color, int density);
	void showUpgradeResourceUI(ui64 rakssId, int costItemId, int costItemNum, int resourceId, String tipStr);
	void broadCastBasementLife(float curLife, float maxLife);

	void broadCastPlayerMovementInput(int playerEntityId);
	void broadCastPlayerRotation(int playerEntityId, bool includeSelf = false);
	void broadCastPlayerMovement(int playerEntityId, bool includeSelf);
	void broadCastPlayerJumpChanged(int playerEntityId);
	void broadCastPlayerSpecialJump(int playerEntityId, float yFactor, float xFactor, float zFactor);
	void syncEntityPlayerAnimate(ui64 rakssId, NETWORK_DEFINE::PacketAnimateType animateType, int entityId, int32_t value);
	void syncEntityPlayerMovementInput(ui64 rakssId, int playerEntityId);
	void broadCastPlayerDownChanged(int playerEntityId);
	void broadCastPlayerSetFlyingState(int playerEntityId, bool isFlying);
	void broadCastPlayerSetSpYaw(int playerEntityId, bool isSpYaw, float radian);
	void sendGameTipNotification(ui64 rakssid, int msgType, ChatComponentNotification* notification);
	void broadCastEntityTeleport(int entityId);
	void syncPlayerMovement(ui64 rakssId, int playerEntityId);
	void syncPlayerDownChanged(ui64 rakssId, int playerEntityId);
	void syncPlayerJumpChanged(ui64 rakssId, int playerEntityId);

	void sendCustomTipMsg(ui64 rakssId, String messageLang, String extra);
	void sendEntityPlayerOccupation(ui64 rakssId, int entityId, int occupation);
	void sendTeamResourcesUpdate(String data);
	void syncShowMaskTime(ui64 rakssId, int MilliSecond);

	void sendBuildWarBlockBtn(ui64 rakssId, bool isNeedOpen);
	void sendbroadcastBuildWarPlayNum(ui8 cur_num, ui8 max_num);
	void sendbroadcastBuildWarLeftTime(ui64 rakssId, bool show, ui16 left_time, ui32 theme);
	void sendbroadcastRanchExTip(ui16 type, ui16 num);
	void sendbroadcastRanchExTask(bool show, String task);
	void sendShowRanchExCurrentItemInfo(ui64 rakssId, bool show, String itemInfo, int type, int num);
	void sendShowRanchExItem(ui64 rakssId, bool show, String item);
	void sendOpenChest(bool show, int entityId, Vector3i pos);
	void sendOpenShopByEntityId(ui64 rakssId, int entityId);
	void sendShowBuildGrade(ui64 rakssId, bool openOrClose, bool show, ui32 msg, ui32 grade_msg, ui8 left_time);
	void sendShowBuildGuessResult(ui64 rakssId, bool right, int rank);
	void showBuildGuessUi(ui64 rakssId, int guess_room_id, String info);
	void sendGameSettlementExtra(ui64 rakssId, bool guess_right, String guess_name, int guess_reward);
	void sendUnlockedCommodity(ui64 rakssid, int merchantId, int itemId, int meta);
	void sendInitUnlockedCommodity(ui64 rakssId, std::vector<int> unlockedCommodity);
	void sendCloseBGM(ui64 rakssId);
	void sendSetArmItem(ui64 rakssId, int itemId);
	void broadCastActorNpcContent(int entityId, String content);
	void broadCastBuildProgress(String content, int curProgress, int maxProgress);
	void sendEnchantmentPropsData(ui64 rakssId, String data);
	void sendPlayerEnableMovement(int entityId);

	void sendConsumeCoinTip(ui64 rakssId, String message, int coinId, int price, String extra);
	void sendSuperPropsData(ui64 rakssId, String data);
	void sendSuperShopTip(ui64 rakssId, String tip);
	void broadCastAddSimpleEffect(String effectName, Vector3 position, float yaw, int duration, float scale);
	void sendAddSimpleEffect(String effectName, Vector3 position, float yaw, int duration, int targetId, float scale);
	void sendEnterOtherGame(ui64 rakssId, String gameType, ui64 userId, String mapId = "");
	void sendKillMsg(ui64 rakssId, String data);
	void sendShowPersonalShop(ui64 rakssId);
	void sendSetPersonalShopArea(ui64 rakssId, Vector3 startPos, Vector3 endPos);
	void sendAddGunBulletNum(ui64 rakssId, int gunId, int bulletNum);
	void sendSubGunRecoil(ui64 rakssId, int gunId, float recoil);
	void sendCustomPropsData(ui64 rakssId, String data);
	void updateCustomProps(ui64 rakssId, String data);

	void syncEntityBlockman(ui64 rakssId, BLOCKMAN::EntityBlockman* blockMan);
	void syncBlockmanMoveAttr(ui64 rakssId, BLOCKMAN::EntityBlockman* blockMan);
	void broadCastCarDirection(int type, int direction);
	void broadCastCarProgress(int type, float progress);
	void syncEntityFrozenTime(ui64 rakssId, int entityId, float time);
	void syncEntityDisableMoveTime(ui64 rakssId, int entityId, float time);
	void broadCastPlayerMovementWithMotion(int playerEntityId, bool includeSelf);
	void sendCannonActorInfo(i64 rakssId, BLOCKMAN::EntityActorCannon* pCannon);
	void sendSyncCannonFireToTrackingPlayers(int playerEntityId, int cannonEntityId);
	void sendSubGunFireCd(ui64 rakssId, int gunId, float fireCd);
	void syncEntityHurtTime(ui64 rakssId, int entityId, float time);
	void sendLotteryData(ui64 rakssId, String data);
	void sendLotteryResult(ui64 rakssId, String firstId, String secondId, String thirdId);
	void sendHideAndSeekHallResult(ui64 rakssId, String itemInfo);
	void syncEntityColorfulTime(ui64 rakssId, int entityId, float time);
	void sendEntitySelected(ui64 rakssId, int entityId, int type);

	void syncRanch(BLOCKMAN::EntityPlayer* player, int syncType) override;
	void syncRanchRank(BLOCKMAN::EntityPlayer* player, int rankType) override;
	void syncRanchBuild(BLOCKMAN::EntityPlayer* player, int buildType) override;
	void syncEntityBuildNpc(ui64 rakssId, BLOCKMAN::EntityBuildNpc* buildNpc);
	void syncEntityLandNpc(ui64 rakssId, BLOCKMAN::EntityLandNpc* landNpc);
	void sendRanchGain(ui64 rakssId, vector<BLOCKMAN::RanchCommon>::type items);
	void sendBroadcastMessage(ui64 rakssId, i32 type, const String& content);
	void sendGotoOtherGame(ui64 rakssId, ui64 targetUserId ,String gameType, String mapId = "");
	void sendRanchUnlockItem(ui64 rakssId, vector<i32>::type items);
	void sendCropInfo(i64 userId, Vector3i blockPos, i32 blockId, i32 stage, i32 curStageTime, i32 residueHarvestNum);
	void sendAppExpResult(i64 userId, int curLv, int toLv, int addExp, int curExp, int toExp, int upExp, int status);
	void sendOccupationUnlock(ui64 rakssId, String occupationInfo);
	void sendBuyFlying(ui64 rakssId);
	void sendPlayCloseup(ui64 rakssId, const Vector3& playerPos, const Vector3& closeupPos, float farDistance, float nearDistance, float velocity, float duration, float yaw, float pitch);
	void sendKeepItemTip(i64 rakssId, int coinType, int coin, int time);
	void sendOpenEnchantment(i64 rakssId, String data);
	void sendOpenPixelGunHallModeSelect(i64 rakssId, bool open, String data);
	void sendOpenArmorUpgrade(i64 rakssId, bool open, String data);
	void sendUpdateStoreGunData(i64 rakssId, String data);
	void sendUpdateStorePropData(i64 rakssId, String data);
	void sendShowGunStore(i64 rakssId);
	void sendShowPixelGunHallInfo(i64 rakssId, int lv, int cur_exp, int max_exp, int yaoshi, bool is_max);
	void sendOpenPixel1v1(i64 rakssId, bool open, String data);
	void sendShowChestLottery(i64 rakssId, bool open, int curIntegral, int needIntegral);
	void sendOpenPixelRevive(i64 rakssId, bool open, String data);
	void sendChestLotteryResult(i64 rakssId, int rewardId, bool hasGet);
	void sendUpdateLotteryChestData(i64 rakssId, String data);
	void sendShowPixelGunGamePerson(i64 rakssId, int time, int rank, int kill_num);
	void sendShowPixelGunGame1v1(i64 rakssId, int time, int red_kill_num, int blue_kill_num, int self_team);
	void sendShowPixelGunGameTeam(i64 rakssId, int time, int red_kill_num, int red_cur_player_num, int red_max_player_num, int blue_kill_num, int blue_cur_player_num, int blue_max_player_num, int self_team);
	void sendOpenPixelResult(i64 rakssId, bool open, String data, int result_type);
	void sendPlayerCurrentSeasonInfo(i64 rakssId, int honorId, int rank, int honor, int endTime);
	void sendPlayerLastSeasonInfo(i64 rakssId, int honorId, int rank, int honor);
	void sendEnableAutoShoot(i64 rakssId, bool enable);
	void sendLogicSettingInfo(ui64 rakssid);
	void sendDisarmament(i64 rakssId, bool isDisarmament);
	void sendOpenPixelLvUp(i64 rakssId, bool open, String data);
	void sendOpenPixelResultSpecialInfo(i64 rakssId, bool btnRevengeEnable);
	void notifyGetGoods(ui64 rakssId, String icon, int count);
	void sendShowSeasonRank(ui64 rakssId);
	void sendPlayerNamePerspective(ui64 rakssId, i32 entityId, bool isPerspective);
	void sendAddPlayerCustomEffect(i32 entityId, String name, String effectName, float duration);
	void syncPlayerCustomEffects(ui64 rakssId, BLOCKMAN::EntityPlayer* player);

	void broadCastPlayerEffectGunGire(int shootingEntityId, int gunId, const Vector3& beginPos, const Vector3& endPos);
	void broadCastPlayerEffectGunGireAtEntityResult(int shootingEntityId, int hitEntityId, const Vector3& hitPos, const Vector3& dir, int traceType, bool isHeadshot);
	void broadCastPlayerEffectGunGireAtBlockResult(int shootingEntityId, const Vector3i& blockPos, const Vector3& hitPos, const Vector3& dir);
	void broadCastPlayerEffectGunGireExplosionResult(int shootingEntityId, const Vector3& hitPos,float explosionRange);
	void broadCastStopLaserGun(i32 shootingEntityId);
	void broadCastPlayerOpacity(int entityId, float opacityValue);

	void syncEntityBulletin(ui64 rakssId, BLOCKMAN::EntityBulletin* entityBulletin);
	void syncEntityBird(ui64 rakssId, BLOCKMAN::EntityBird* entityBird);
	void syncBirdSimulator(BLOCKMAN::EntityPlayer* player, int syncType) override;
	void syncPlayerBagInfo(ui64 rakssId, BLOCKMAN::EntityPlayer* player);
	void sendBirdGain(ui64 rakssId,const vector<BLOCKMAN::BirdGain>::type& items);
	void sendBirdNestOperation(ui64 rakssId, bool isUnlock, i64 birdId);
	void sendBirdLotteryResult(ui64 rakssId, String eggBodyId, const BLOCKMAN::BirdInfo& info);
	void sendEntityAction(ui64 rakssId, int entityId, int entityClass, int actionStatus, int baseAction, int uperBaseAction);
	void sendBirdAddScore(ui64 rakssId, i32 score, i32 scoreType);
	void onBirdSimulatorGather(ui64 userId, i64 birdId, const  BLOCKMAN::BlockPos& pos);
	void syncEntityHealth(ui64 rakssId, i32 entityId, float health, float maxHealth);
	void syncBirdSimulatorStoreItem(ui64 rakssId, const  BLOCKMAN::BirdStore& store);
	void syncBirdSimulatorTaskItem(ui64 rakssId, bool isRemove, const  BLOCKMAN::BirdTask& taskItem);
	void syncBirdSimulatorPersonStoreTab(ui64 rakssId, const  BLOCKMAN::BirdPersonalStoreTab& storeTab);
	void changeGuideArrowStatus(ui64 rakssId, Vector3 arrowPos, bool flag);
	void broadCastAddWallText(String text, Vector3 textPos, float scale, float yaw, float pitch, float r, float g, float b, float a);
	void broadCastDeleteWallText(Vector3 textPos);

private:
	std::shared_ptr<DataPacket> constructEntitySpawnPacket(BLOCKMAN::Entity* myEntity);
	std::shared_ptr<DataPacket> constructPlaySoundPacket(uint soundType, i32 entityId = 0);
	std::shared_ptr<DataPacket> constructAddPotionEffectPacket(int entityId, BLOCKMAN::PotionEffect* effect);
};

#endif
