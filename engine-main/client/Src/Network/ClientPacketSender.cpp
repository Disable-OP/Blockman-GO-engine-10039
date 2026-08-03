#include "ClientPacketSender.h"
#include "Network/protocol/C2SPackets.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "cWorld/Blockman.h"
#include "game.h"
#include "Object/Root.h"
#include "Entity/EntityPlayer.h"
#include "Util/LanguageManager.h"
#include "GameVersionDefine.h"
#include "Entity/EntityAircraft.h"
#include "Util/ClientEvents.h"
#include "Enums/Enums.h"
#include "Util/LanguageKey.h"

using namespace BLOCKMAN;

ClientPacketSender::ClientPacketSender()
{

}

ClientPacketSender::~ClientPacketSender()
{

}

void ClientPacketSender::sendLogin()
{
	auto packet = LORD::make_shared<C2SPacketLogin>();
	packet->m_playerName = GameClient::CGame::Instance()->getPlayerName();
	packet->m_gameTimestamp = GameClient::CGame::Instance()->getGameTimestamp();
	packet->m_platformUserId = GameClient::CGame::Instance()->getPlatformUserId();
	packet->m_gameToken = GameClient::CGame::Instance()->getGameToken();
	packet->m_netVersion = 1;
	packet->m_language = LanguageManager::Instance()->getCurrentLanguage();
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
	GameClient::CGame::Instance()->setSendLoginTime(Root::Instance()->getCurrentTime());
}

void ClientPacketSender::sendRebirth()
{
	auto packet = LORD::make_shared<C2SPacketRebirth>();
	packet->m_playerName = GameClient::CGame::Instance()->getPlayerName();
	packet->m_netVersion = 1; //todo 

	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendAnimate(int32_t clientEntityId, NETWORK_DEFINE::PacketAnimateType animateType, int32_t value/* = 0*/)
{
	auto packet = LORD::make_shared<C2SPacketAnimate>();
	int remoteEntityId = ClientNetwork::Instance()->getDataCache()->getServerId(clientEntityId);
	if (remoteEntityId <= 0) {
		LordLogError("try to send animate but can not find remote entity id with client id=%d", clientEntityId);
		return;
	}
	packet->m_entityRuntimeId = remoteEntityId;
	packet->m_animateType = animateType;
	packet->m_value = value;

	auto pData = std::dynamic_pointer_cast<DataPacket>(packet);
	GameClient::CGame::Instance()->getNetwork()->sendPacket(pData, true);
}

void ClientPacketSender::sendAttack(int targetEntityRuntimeId, real32 targetX, real32 targetY, real32 targetZ)
{
	auto packet = LORD::make_shared<C2SPacketAttack>();
	packet->m_targetEntityRuntimeId = targetEntityRuntimeId;
	packet->m_targetX = targetX;
	packet->m_targetY = targetY;
	packet->m_targetZ = targetZ;

	auto pData = std::dynamic_pointer_cast<DataPacket>(packet);
	GameClient::CGame::Instance()->getNetwork()->sendPacket(pData, true);
}

void ClientPacketSender::sendPlayerMovement(bool onGround)
{
	auto packet = LORD::make_shared<C2SPacketPlayerMovement>();
	packet->m_onGround = onGround;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendPlayerMovement(bool onGround, float yaw, float pitch)
{
	auto packet = LORD::make_shared<C2SPacketPlayerMovement>();
	packet->m_onGround = onGround;
	packet->m_yaw = yaw;
	packet->m_pitch = pitch;
	packet->m_rotating = true;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendPlayerMovement(bool onGround, float x, float y, float z)
{
	auto packet = LORD::make_shared<C2SPacketPlayerMovement>();
	packet->m_onGround = onGround;
	packet->m_x = x;
	packet->m_y = y;
	packet->m_z = z;
	packet->m_moving = true;

	/* for debug, added by wanglei
	int localId = Blockman::Instance()->m_pPlayer->entityId;
	int remoteId = ClientNetwork::Instance()->getDataCache()->getRemoteId(localId);
	LordLogDebug("%d sendPlayerMovement, only moving, new postion=(%f, %f, %f)",
		remoteId, x, y, z);
	*/

	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendPlayerMovement(bool onGround, float x, float y, float z, float yaw, float pitch)
{
	auto packet = LORD::make_shared<C2SPacketPlayerMovement>();
	packet->m_onGround = onGround;
	packet->m_x = x;
	packet->m_y = y;
	packet->m_z = z;
	packet->m_yaw = yaw;
	packet->m_pitch = pitch;
	packet->m_moving = true;
	packet->m_rotating = true;

	/* for debug, added by wanglei
	int localId = Blockman::Instance()->m_pPlayer->entityId;
	int remoteId = ClientNetwork::Instance()->getDataCache()->getRemoteId(localId);
	LordLogDebug("%d sendPlayerMovement, moving and rotating, new postion=(%f, %f, %f), new rotate=(%f, %f)",
		remoteId, x, y, z, yaw, pitch);
	*/

	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendVehicleMovement(int vehicleId, bool onGround, bool moved, bool rotated, float x, float y, float z, float yaw)
{
	auto packet = LORD::make_shared<C2SPacketVehicleMovement>();
	packet->m_vehicleId = ClientNetwork::Instance()->getDataCache()->getServerId(vehicleId);
	packet->m_onGround = onGround;
	packet->m_x = x;
	packet->m_y = y;
	packet->m_z = z;
	packet->m_yaw = yaw;
	packet->m_moving = moved;
	packet->m_onGround = onGround;
	packet->m_rotating = rotated;

	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendVehicleActionState(int vehicleId, int state)
{
	auto packet = LORD::make_shared<C2SPacketVehicleActionState>();
	packet->vehicleId = ClientNetwork::Instance()->getDataCache()->getServerId(vehicleId);
	packet->state = state;

	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendCraftWithCraftingTable(int recipeId, Vector3i craftingTablePositoin)
{
	auto packet = LORD::make_shared<C2SPacketCraftWithCraftingTable>();
	packet->recipeId = recipeId;
	packet->craftingTablePosition = craftingTablePositoin;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendCraftWithHand(int recipeId)
{
	auto packet = LORD::make_shared<C2SPacketCraftWithHand>();
	packet->recipeId = recipeId;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendSwapItemPosition(int index1, int index2)
{
	auto packet = LORD::make_shared<C2SPacketSwapItemPosition>();
	packet->index1 = index1;
	packet->index2 = index2;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendDropItemPosition(int inventoryIndex)
{
	auto packet = LORD::make_shared<C2SPacketDropItemPosition>();
	packet->inventoryIndex = inventoryIndex;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendEquipArmor(int slotIndex)
{
	auto packet = LORD::make_shared<C2SPacketEquipArmor>();
	packet->slotIndex = slotIndex;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendUnequipArmor(int slotIndex)
{
	auto packet = LORD::make_shared<C2SPacketUnequipArmor>();
	packet->slotIndex = slotIndex;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendChangeItemInHand(int slotIndex)
{
	auto packet = LORD::make_shared<C2SPacketChangeItemInHand>();
	packet->slotIndex = slotIndex;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
	LordLogInfo("sendChangeItemInHand, slotIndex=%d", slotIndex);
}

void ClientPacketSender::sendStartBreakBlock(int x, int y, int z)
{
	auto packet = LORD::make_shared<C2SPacketStartBreakBlock>();
	packet->x = (i16)x;
	packet->y = (i16)y;
	packet->z = (i16)z;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendAbortBreakBlock()
{
	GameClient::CGame::Instance()->getNetwork()->sendPacket(LORD::make_shared<C2SPacketAbortBreakBlock>(), true);
}

void ClientPacketSender::sendFinishBreakBlock()
{
	GameClient::CGame::Instance()->getNetwork()->sendPacket(LORD::make_shared<C2SPacketFinishBreakBlock>(), true);
}

void ClientPacketSender::sendPlaceBlock(const Vector3i & blockPos, int face, const Vector3 & hisPos)
{
	auto packet = LORD::make_shared<C2SPacketPlaceBlock>();
	packet->blockPosX = (i16)blockPos.x;
	packet->blockPosY = (i16)blockPos.y;
	packet->blockPosZ = (i16)blockPos.z;
	packet->face = face;
	packet->hitPos = hisPos;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendActivateBlock(const Vector3i & blockPos, int face, const Vector3 & hisPos)
{
	auto packet = LORD::make_shared<C2SPacketActivateBlock>();
	packet->blockPosX = (i16)blockPos.x;
	packet->blockPosY = (i16)blockPos.y;
	packet->blockPosZ = (i16)blockPos.z;
	packet->face = face;
	packet->hitPos = hisPos;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendExchangeItemStack(bool isPlayerInventoryToChest, i32 touchTime, int index, Vector3i blockPos, int face, Vector3 hisPos)
{
	auto packet = LORD::make_shared<C2SPacketExchangeItemStack>();
	packet->isPlayerInventoryToChest = isPlayerInventoryToChest;
	packet->touchTime = touchTime;
	packet->index = index;
	packet->blockPosX = (i16)blockPos.x;
	packet->blockPosY = (i16)blockPos.y;
	packet->blockPosZ = (i16)blockPos.z;
	packet->face = face;
	packet->hitPos = hisPos;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendChatString(String msg)
{
	auto packet = LORD::make_shared<C2SPacketChatString>();
	packet->m_chatType = 1;
	packet->m_chatComponent = LORD::make_shared<ChatComponentString>(ClientNetwork::Instance()->getDataCache()->getSelfServerId(), msg);
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendUseProp(String propId)
{
	LordLogDebug("sendUseProp");
	auto packet = LORD::make_shared<C2SPacketUseProp>();
	packet->m_propId = propId;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendPing()
{
	auto packet = LORD::make_shared<C2SPacketPing>();
	packet->m_sendTime = Root::Instance()->getCurrentTime();
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendPlayerAction(NETWORK_DEFINE::PacketPlayerActionType action)
{
	LordLogDebug("sendPlayerAction, action=%d", (int)action);
	auto packet = LORD::make_shared<C2SPacketPlayerAction>();
	packet->action = action;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendUseItem(NETWORK_DEFINE::C2SPacketUseItemUseType useType, const Vector3i& pos, int param0)
{
	LordLogDebug("sendUseItem,useType=%d", (int)useType);
	auto packet = LORD::make_shared<C2SPacketUseItem>();
	packet->useType = useType;
	packet->pos = pos;
	packet->param0 = param0;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendLaunchArrow(float cdTime, const Vector3& playerPos, float playerYaw, float playerPitch,int arrowID)
{
	LordLogDebug("sendLaunchArrow,cdTime=%f", cdTime);
	auto packet = LORD::make_shared<C2SPacketLaunchArrow>();
	packet->cdTime = cdTime;
	packet->playerPos = playerPos;
	packet->playerYaw = playerYaw;
	packet->playerPitch = playerPitch;
	packet->arrowID = arrowID;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendLaunchBullet(int bulletID, const Vector3& playerPos, float playerYaw, float playerPitch)
{
	auto packet = LORD::make_shared<C2SPacketLaunchBullet>();
	packet->bulletID = bulletID;
	packet->playerPos = playerPos;
	packet->playerYaw = playerYaw;
	packet->playerPitch = playerPitch;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendSpawnVehicle(int carID, const Vector3& pos, float yaw)
{
	auto packet = LORD::make_shared<C2SPacketSpawnVehicle>();
	packet->carID = carID;
	packet->pos = pos;
	packet->yaw = yaw;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendGunFireResult(const Vector3& pos, const Vector3& dir, int tracyType, const Vector3i& blockPos, const Vector3& hitpos, int hitEnttityID, bool isNeedConsumeBullet, bool isHeadshot)
{
	auto packet = LORD::make_shared<C2SPacketGunFireResult>();
	packet->pos = pos;
	packet->dir = dir;
	packet->tracyType = tracyType;
	packet->blockPos = blockPos;
	packet->hitpos = hitpos;
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	int serverID = dataCache->getServerId(hitEnttityID);
	packet->hitEntityID = serverID;  // need transform clinetID to serverID !!!!!!!!!!!
	packet->isNeedConsumeBullet = isNeedConsumeBullet;
	packet->isHeadshot = isHeadshot;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendSplashPotion(const Vector3& playerPos, float playerYaw, float playerPitch)
{
	LordLogDebug("sendSplashPotion,playerPos=(%f,%f,%f)", playerPos.x, playerPos.y, playerPos.z);
	auto packet = LORD::make_shared<C2SPacketSplashPotion>();
	packet->playerPos = playerPos;
	packet->playerYaw = playerYaw;
	packet->playerPitch = playerPitch;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendCloseContainer(const Vector3i& containerPos)
{
	auto packet = LORD::make_shared<C2SPacketCloseContainer>();
	// packet->containerPos = containerPos;  // useless
	packet->containerPosX = (i16)containerPos.x;
	packet->containerPosY = (i16)containerPos.y;
	packet->containerPosZ = (i16)containerPos.z;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendUseThrowableItem()
{
	auto packet = LORD::make_shared<C2SPacketUseThrowableItem>();
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendSetFlying(bool isFlying)
{
	auto packet = LORD::make_shared<C2SPacketSetFlying>();
	packet->isFlying = isFlying;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendSetSpYaw(bool isSpYaw, float radian)
{
	auto packet = LORD::make_shared<C2SPacketSetSpYaw>();
	packet->isSpYaw = isSpYaw;
	packet->radian = radian;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendBuyCommodity(int merchantId, int groupIndex, int commodityIndex, int commodityId)
{
	auto packet = LORD::make_shared<C2SPacketBuyCommodity>();
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	packet->merchantId = dataCache->getServerId(merchantId);
	packet->groupIndex = groupIndex;
	packet->commodityIndex = commodityIndex;
	packet->commodityId = commodityId;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendTakeVehicle(bool onoff, int carID)
{
	auto packet = LORD::make_shared<C2SPacketTakeVehicle>();
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	packet->onoff = onoff;
	packet->vehicleId = dataCache->getServerId(carID);
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendCheckCSVersion()
{
	auto packet = LORD::make_shared<C2SPacketCheckCSVersion>();
	packet->m_version = EngineVersionSetting::getEngineVersion();
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
	GameClient::CGame::Instance()->setSendCheckCSVersionTime(Root::Instance()->getCurrentTime());
}

void ClientPacketSender::sendReloadBullet()
{
	auto packet = LORD::make_shared<C2SReloadBullet>();
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendBuyGoods(int groupIndex, int goodsIndex, int goodsId)
{
	auto packet = LORD::make_shared<C2SPacketBuyGoods>();
	packet->groupIndex = groupIndex;
	packet->goodsIndex = goodsIndex;
	packet->goodsId = goodsId;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendResumeOrder(String orderId)
{
	auto packet = LORD::make_shared<C2SPacketResumeOrder>();
	packet->orderId = orderId;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendSelectRole(int role)
{
	auto packet = LORD::make_shared<C2SPacketSelectRole>();
	packet->role = role;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendBuyVehicle(int carId)
{
	auto packet = LORD::make_shared<C2SPacketBuyVehicle>();
	packet->vehicleId = carId;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendLockVehicle(bool onoff, int vehicleId)
{
	auto packet = LORD::make_shared<C2SPacketLockVehicle>();
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	packet->vehicleId = dataCache->getServerId(vehicleId);
	packet->onoff = onoff;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendClientReady()
{
	auto packet = LORD::make_shared<C2SPacketClientReady>();
	packet->m_status = 1;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendBuyRespawn(bool isRespawn, i32 uniqueId, i32 index)
{
	auto packet = LORD::make_shared<C2SPacketBuyRespawn>();
	packet->isRespawn = isRespawn;
	packet->uniqueId = uniqueId;
	packet->index = index;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendKeepItem(bool isKeepItem)
{
	auto packet = LORD::make_shared<C2SPacketKeepItem>();
	packet->isKeepItem = isKeepItem;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendEnchantmentQuick(int equipId, int effectIndex)
{
	auto packet = LORD::make_shared<C2SPacketEnchantmentQuick>();
	packet->equipId = equipId;
	packet->effectIndex = effectIndex;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendPixelHallUnlockMap(int num, int mapId)
{
	auto packet = LORD::make_shared<C2SPacketPixelHallUnlockMap>();
	packet->num = num;
	packet->mapId = mapId;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}


void ClientPacketSender::sendGoNpcMerchant(float x, float y, float z, float yaw)
{
	auto packet = LORD::make_shared<C2SPacketGoNpcMerchant>();
	packet->x = x;
	packet->y = y;
	packet->z = z;
	packet->yaw = yaw;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendClickTeleport()
{
	auto packet = LORD::make_shared<C2SPacketClickTeleport>();
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendLeaveAircraft()
{
	if (!BLOCKMAN::Blockman::Instance()->m_pPlayer->isOnAircraft())
		return;
	
	auto packet = LORD::make_shared<C2SPacketLeaveAircraft>();
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendChangeClothes(int actorId, String partName)
{
	auto packet = LORD::make_shared<C2SPacketChangeClothes>();
	packet->actorId = actorId;
	packet->partName = partName;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendRestClothes()
{
	auto packet = LORD::make_shared<C2SPacketRestClothes>();
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendFriendOperation(int operationType, ui64 userId)
{
	auto packet = LORD::make_shared<C2SPacketFriendOperation>();
	packet->operationType = operationType;
	packet->userId = userId;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void  ClientPacketSender::sendSetParachute(bool isParachute)
{
	auto packet = LORD::make_shared<C2SPacketSetParachute>();
	packet->isParachute = isParachute;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendSkillType(BLOCKMAN::SkillType skill, bool isRelease)
{
	auto packet = LORD::make_shared<C2SPacketSkillType>();
	packet->skillType = (int)skill;
	packet->skillStatus = isRelease;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}	
void ClientPacketSender::sendChangePlayerActor() 
{
	auto packet = LORD::make_shared<C2SPacketChangePlayerActor>();
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendClickChangeActor()
{
	auto packet = LORD::make_shared<C2SPacketClickChangeActor>();
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendCameraLockMovement(Vector3 & pos)
{
	auto packet = LORD::make_shared<C2SPacketCameraLockMovement>();
	packet->nextPosition = pos;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendCameraLock(bool bLock)
{
	auto packet = LORD::make_shared<C2SPacketCameraLock>();
	packet->isLocked = bLock;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}


void ClientPacketSender::sendPickUpItemPay(i32 itemEntityId, i32 itemId)
{
	auto packet = LORD::make_shared<C2SPacketPickUpItemPay>();
	packet->itemId = itemId;
	packet->itemEntityId = itemEntityId;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendAttackActorNpc(i32 entityId)
{
	auto packet = LORD::make_shared<C2SPacketAttackActorNpc>();
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	packet->entityId = dataCache->getServerId(entityId);
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}


void ClientPacketSender::sendGetManor()
{
	GameClient::CGame::Instance()->getNetwork()->sendPacket(LORD::make_shared<C2SPacketGetManor>(), true);
}

void ClientPacketSender::sendSellManor()
{
	GameClient::CGame::Instance()->getNetwork()->sendPacket(LORD::make_shared<C2SPacketSellManor>(), true);
}

void ClientPacketSender::sendUpgradeManor()
{
	
	GameClient::CGame::Instance()->getNetwork()->sendPacket(LORD::make_shared<C2SPacketUpgradeManor>(), true);
}

void ClientPacketSender::sendBuyHouse(BLOCKMAN::ManorHouse* house)
{
	auto packet = LORD::make_shared<C2SPacketBuyHouse>();
	packet->m_house = *house;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendBuyFurniture(BLOCKMAN::ManorFurniture* furniture)
{
	auto packet = LORD::make_shared<C2SPacketBuyFurniture>();
	packet->m_furniture = *furniture;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendDeleteManorMessage(BLOCKMAN::ManorMessage* message)
{
	auto packet = LORD::make_shared<C2SPacketDeleteManorMessage>();
	packet->m_message = *message;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}


void ClientPacketSender::sendBuildHouse(BLOCKMAN::ManorHouse* house)
{
	auto packet = LORD::make_shared <C2SPacketBuildHouse> ();
	packet->m_house = *house;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendPutFurniture(i32 furnitureId, BLOCKMAN::PutFurnitureStatus status)
{
	auto item = BLOCKMAN::Blockman::Instance()->m_pPlayer->m_manor->findFurnitureById(furnitureId);
	if (!item)
	{
		return;
	}
	switch (status)
	{
	case BLOCKMAN::PutFurnitureStatus::START:
	case BLOCKMAN::PutFurnitureStatus::EDIT_START:
		BLOCKMAN::Blockman::Instance()->m_pPlayer->m_isActorChange = true;
		break;
	default:
		BLOCKMAN::Blockman::Instance()->m_pPlayer->m_isActorChange = false;
		break;
	}

	auto packet = LORD::make_shared<C2SPacketPutFurniture>();
	packet->m_status = status;
	packet->m_furniture = *item;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendRecycleFurniture(BLOCKMAN::ManorFurniture* furniture)
{
	auto packet = LORD::make_shared<C2SPacketRecycleFurniture>();
	packet->m_furniture = *furniture;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendManorTeleport()
{
	GameClient::CGame::Instance()->getNetwork()->sendPacket(LORD::make_shared<C2SPacketManorTeleport>(), true);
}

void ClientPacketSender::sendCallOnManor(ui64 targetUserId)
{
	if (targetUserId == 0)
	{
		LordLogError("send call on manor targetUserId  = 0");
		return;
	}

	if (BLOCKMAN::Blockman::Instance()->m_pPlayer->isOnVehicle())
	{
		ShowResultTipDialogEvent::emit(ResultCode::COMMON, LanguageKey::GUI_MANOR_CALL_ON_IN_VEHICLE);
		return;
	}
	auto packet = LORD::make_shared<C2SPacketCallOnManor>();
	packet->m_targetUserId = targetUserId;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendGiveRose(ui64 targetUserId)
{
	if (targetUserId == 0)
	{
		LordLogError("send give rose targetUserId  = 0");
		return;
	}
	auto packet = LORD::make_shared<C2SPacketGiveRose>();
	packet->m_targetUserId = targetUserId;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendRequestUpdateRealTimeRank()
{
	auto packet = LORD::make_shared<C2SPacketRequestUpdateRealTimeRank>();
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendBuySwitchableProp(String uniqueId)
{
	auto packet = LORD::make_shared<C2SPacketBuySwitchableProp>();
	packet->uniqueId = uniqueId;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendBuyUpgradeProp(String uniqueId)
{
	auto packet = LORD::make_shared<C2SPacketBuyUpgradeProp>();
	packet->uniqueId = uniqueId;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendAttackCreatureEntity(i32 entityId)
{
	auto packet = LORD::make_shared<C2SPacketAttackCreatureEntity>();
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	packet->entityId = dataCache->getServerId(entityId);
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendUpgradeResource(i32 resourceId)
{
	auto packet = LORD::make_shared<C2SPacketUpgradeResource>();
	packet->resourceId = resourceId;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendWatchRespawn()
{
	auto packet = LORD::make_shared<C2SPacketWatchRespawn>();
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendMovementInputChanged(float forward, float strafe)
{
	auto packet = LORD::make_shared<C2SPacketMovementInput>();
	packet->forward = forward;
	packet->strafe = strafe;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendJumpChanged(bool isJump)
{
	auto packet = LORD::make_shared<C2SPacketSendJumpChanged>();
	packet->isJump = isJump;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendPlayerRotationChanged(float yaw, float pitch)
{
	auto packet = LORD::make_shared<C2SPacketSendPlayerRotation>();
	packet->yaw = yaw;
	packet->pitch = pitch;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendDownChanged(bool isDown)
{
	auto packet = LORD::make_shared<C2SPacketSendDownChanged>();
	packet->isDown = isDown;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendNeedUpdateMerchantCommodities(int merchantId)
{
	auto packet = LORD::make_shared<C2SPacketNeedUpdateMerchantCommodities>();
	packet->merchantId = merchantId;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendCustomTipResult(String extra, bool bResult)
{
	auto packet = LORD::make_shared<C2SPacketSyncCustomTipResult>();
	packet->extra = extra;
	packet->bResult = bResult;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void  ClientPacketSender::sendRelaseItemSkill(int skillItemId)
{
	auto packet = LORD::make_shared<C2SPacketRelaseItemSkill>();
	packet->skillItemId = skillItemId;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendBuildWarOpenShop()
{
	GameClient::CGame::Instance()->getNetwork()->sendPacket(LORD::make_shared<C2SPacketBuildWarOpenShop>(), true);
}

void ClientPacketSender::sendGradeScore(int grade)
{
	auto packet = LORD::make_shared<C2SPacketBuildWarGrade>();
	packet->grade = (ui8)grade;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendBuildWarOpenGuessResult()
{
	auto packet = LORD::make_shared<C2SPacketBuildWarOpenGuessResult>();
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendBuildWarResult()
{
	auto packet = LORD::make_shared<C2SPacketShowBuildWarResult>();
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendBuildWarGuessVisit(ui64 platformUserId)
{
	auto packet = LORD::make_shared<C2SPacketBuildWarGuessVisit>();
	packet->m_platformUserId = platformUserId;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendBuildWarGuessSuc(int guessRoomId)
{
	auto packet = LORD::make_shared<C2SPacketBuildWarGuessSuc>();
	packet->guessRoomId = guessRoomId;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendGetCommodity(int merchantId, int groupIndex, int commodityIndex, int commodityId)
{
	auto packet = LORD::make_shared<C2SPacketGetCommodity>();
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	packet->merchantId = dataCache->getServerId(merchantId);
	packet->groupIndex = groupIndex;
	packet->commodityIndex = commodityIndex;
	packet->commodityId = commodityId;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendUnlockCommodity(int merchantId, int groupIndex, int commodityIndex, int commodityId)
{
	auto packet = LORD::make_shared<C2SPacketUnlockCommodity>();
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	packet->merchantId = dataCache->getServerId(merchantId);
	packet->groupIndex = groupIndex;
	packet->commodityIndex = commodityIndex;
	packet->commodityId = commodityId;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendPlayerInteractionActionMesage(ui64 targetUserId, String langKey, LORD::vector<String>::type args)
{
	auto packet = LORD::make_shared<C2SPacketPlayerInteractionActionMesage>();
	packet->m_targetUserId = targetUserId;
	packet->m_langKey = langKey;
	packet->args = args;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendAttackEntity(i32 entityId)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto packet = LORD::make_shared<C2SPacketAttackEntity>();
	packet->entityId = dataCache->getServerId(entityId);
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendBuyEnchantmentProp(String uniqueId)
{
	auto packet = LORD::make_shared<C2SPacketBuyEnchantmentProp>();
	packet->uniqueId = uniqueId;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendConsumeTipResult(String extra, bool bResult)
{
	auto packet = LORD::make_shared<C2SPacketConsumeTipResult>();
	packet->extra = extra;
	packet->bResult = bResult;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendBuySuperProp(String uniqueId)
{
	auto packet = LORD::make_shared<C2SPacketBuySuperProp>();
	packet->uniqueId = uniqueId;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendBuyCustomProp(String uniqueId)
{
	auto packet = LORD::make_shared<C2SPacketBuyCustomProp>();
	packet->uniqueId = uniqueId;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendClickBackHall()
{
	auto packet = LORD::make_shared<C2SPacketClickBackHall>();
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendLongHitEntity(i32 entityId)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto packet = LORD::make_shared<C2SPacketLongHitEntity>();
	packet->entityId = dataCache->getServerId(entityId);
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendUseFishingRod()
{
	auto packet = LORD::make_shared<C2SPacketUseFishingRod>();
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendUseCannon(i32 entityId)
{
	auto packet = LORD::make_shared<C2SPacketUseCannon>();
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	packet->entityId = dataCache->getServerId(entityId);
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendUpdateUserMoney()
{
	auto packet = LORD::make_shared<C2SPacketUpdateUserMoney>();
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendBuyBullet()
{
	auto packet = LORD::make_shared<C2SPacketBuyBullet>();
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendBuyRanchItem(i32 itemId, i32 num, i32 totalPrice, i32 currencyType)
{
	auto packet = LORD::make_shared<C2SPacketRanchBuyItem>();
	packet->itemId = itemId;
	packet->num = num;
	packet->totalPrice = totalPrice;
	packet->currencyType = currencyType;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendBuyRanchBuildItem(i32 itemId, i32 num, i32 type, i32 currencyType)
{
	auto packet = LORD::make_shared<C2SPacketRanchBuyBuildItem>();
	packet->itemId = itemId;
	packet->num = num;
	packet->type = type;
	packet->currencyType = currencyType;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendRanchOperation(RanchOperationType type)
{
	auto packet = LORD::make_shared<C2SPacketRanchOperation>();
	packet->m_type = type;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendRanchReceive(ui64 referrer)
{
	auto packet = LORD::make_shared<C2SPacketRanchReceive>();
	packet->referrer = referrer;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendRanchExpand(i32 landCode)
{
	auto packet = LORD::make_shared<C2SPacketRanchExpand>();
	packet->landCode = landCode;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendRanchShortcut(i32 landCode, i32 moneyType)
{
	auto packet = LORD::make_shared<C2SPacketRanchShortcut>();
	packet->landCode = landCode;
	packet->moneyType = moneyType;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendRanchDestroyHouse()
{
	auto packet = LORD::make_shared<C2SPacketRanchDestroyHouse>();
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendRanchSellItem(i32 itemId, i32 num)
{
	auto packet = LORD::make_shared<C2SPacketRanchSellItem>();
	packet->itemId = itemId;
	packet->num = num;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendRanchBuyItems(const vector<RanchCommon>::type items)
{
	auto packet = LORD::make_shared<C2SPacketRanchBuyItems>();
	packet->items = items;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendRanchBuildTakeProduct(i32 entityId, i32 itemId, i32 index)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	i32 serverId = dataCache->getServerId(entityId);

	auto packet = LORD::make_shared<C2SPacketRanchBuildTakeProduct>();
	packet->entityId = serverId;
	packet->itemId = itemId;
	packet->index = index;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendRanchBuildQueueOperation(i32 type, i32 entityId, i32 queueId, i32 productId)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	i32 serverId = dataCache->getServerId(entityId);

	auto packet = LORD::make_shared<C2SPacketRanchBuildQueueOperation>();
	packet->type = type;
	packet->entityId = serverId;
	packet->queueId = queueId;
	packet->productId = productId;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendRanchBuildRemove(i32 actorId)
{
	auto packet = LORD::make_shared<C2SPacketRanchBuildRemove>();
	packet->actorId = actorId;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendRanchOrderOperate(i32 type, i32 orderId, i32 index)
{
	auto packet = LORD::make_shared<C2SPacketRanchOrderOperate>();
	packet->type = type;
	packet->orderId = orderId;
	packet->index = index;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendRanchExBegin()
{
	GameClient::CGame::Instance()->getNetwork()->sendPacket(LORD::make_shared<C2SPacketRancherExBegin>(), true);
}

void ClientPacketSender::sendRanchExTaskFinish(int itemId, int itemNum)
{
	auto packet = LORD::make_shared<C2SPacketRancherExTaskFinish>();
	packet->itemId = itemId;
	packet->itemNum = itemNum;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

// 1 gifts 2 invite
void ClientPacketSender::sendRanchGiftsOrInvite(int type, i64 targetUserId)
{
	auto packet = LORD::make_shared<C2SPacketRanchGiftsOrInvite>();
	packet->type = type;
	packet->targetUserId = targetUserId;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendRanchHelpFinish(i64 helpId, i32 type)
{
	auto packet = LORD::make_shared<C2SPacketRanchHelpFinish>();
	packet->type = type;
	packet->helpId = helpId;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendReceiveMailReward(i64 mailId)
{
	auto packet = LORD::make_shared<C2SPacketReceiveMailReward>();
	packet->mailId = mailId;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendReceiveRanchAchievementReward(i64 achievementId)
{
	auto packet = LORD::make_shared<C2SPacketReceiveRanchAchievementReward>();
	packet->achievementId = achievementId;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendRanchGoExplore()
{
	auto packet = LORD::make_shared<C2SPacketRanchGoExplore>();
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendChangeCurrentItemInfo(int itemIndex)
{
	auto packet = LORD::make_shared<C2SPacketChangeCurrentItemInfo>();
	packet->itemIndex = itemIndex;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::buildNotification(i64 userId, const String & message)
{
	if (userId == GameClient::CGame::Instance()->getPlatformUserId())
	{
		String buildName = LanguageManager::Instance()->getString(message);
		String msg = LanguageManager::Instance()->getString("gui_ranch_build_road_not_exist");
		BottomSystemNotificationEvent::emit(StringUtil::Format(msg.c_str(), buildName.c_str()), 2);
	}
}

void ClientPacketSender::sendSpeedUpCrop(Vector3i blockPos, i32 blockId)
{
	auto packet = LORD::make_shared<C2SPacketSpeedUpCrop>();
	packet->blockPos = blockPos;
	packet->blockId = blockId;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendUseCubeFinshOrder(i32 orderId, i32 index)
{
	auto packet = LORD::make_shared<C2SPacketUseCubeFinshOrder>();
	packet->orderId = orderId;
	packet->index = index;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendSelectLotteryItem(String uniqueId)
{
	auto packet = LORD::make_shared<C2SPacketSelectLottery>();
	packet->uniqueId = uniqueId;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendLotteryAgain()
{
	auto packet = LORD::make_shared<C2SPacketLotteryAgain>();
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendHallLotteryAgain(int luck_times, int entityId)
{
	auto packet = LORD::make_shared<C2SPacketHallLotteryAgain>();
	packet->luck_times = luck_times;
	packet->entityId = entityId;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendBuyFlyingPermissionResult()
{
	auto packet = LORD::make_shared<C2SPacketBuyFlyingPermissionResult>();
	packet->isSuccessful = true;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendResetCloseup(const Vector3& playerPos)
{
	auto packet = LORD::make_shared<C2SPacketResetCloseup>();
	packet->playerPos = playerPos;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendOpenEnchantMent()
{
	auto packet = LORD::make_shared<C2SPacketOpenEnchantMent>();
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendEnchantmentEffectClick(int equipId, int consumeId, int effectIndex)
{
	auto packet = LORD::make_shared<C2SPacketEnchantmentEffectClick>();
	packet->equipId = equipId;
	packet->consumeId = consumeId;
	packet->index = effectIndex;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendOpenModeSelect()
{
	auto packet = LORD::make_shared<C2SPacketOpenSelectMode>();
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendUseStoreGun(int gunId)
{
	auto packet = LORD::make_shared<C2SPacketUseStoreGun>();
	packet->gunId = gunId;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendBuyStoreGun(int gunId)
{
	auto packet = LORD::make_shared<C2SPacketBuyStoreGun>();
	packet->gunId = gunId;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendUpgradeStoreGun(int gunId)
{
	auto packet = LORD::make_shared<C2SPacketUpgradeStoreGun>();
	packet->gunId = gunId;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendQuickUpgradeStoreGun(int gunId)
{
	auto packet = LORD::make_shared<C2SPacketQuickUpgradeStoreGun>();
	packet->gunId = gunId;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendUseStoreProp(int propId)
{
	auto packet = LORD::make_shared<C2SPacketUseStoreProp>();
	packet->propId = propId;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendUnloadStoreProp(int propId)
{
	auto packet = LORD::make_shared<C2SPacketUnloadStoreProp>();
	packet->propId = propId;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendBuyStoreProp(int propId)
{
	auto packet = LORD::make_shared<C2SPacketBuyStoreProp>();
	packet->propId = propId;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendUpgradeStoreProp(int propId)
{
	auto packet = LORD::make_shared<C2SPacketUpgradeStoreProp>();
	packet->propId = propId;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendQuickUpgradeStoreProp(int propId)
{
	auto packet = LORD::make_shared<C2SPacketQuickUpgradeStoreProp>();
	packet->propId = propId;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendFirstShowStoreGun(int gunId)
{
	auto packet = LORD::make_shared<C2SPacketFirstShowStoreGun>();
	packet->gunId = gunId;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendFirstShowStoreProp(int propId)
{
	auto packet = LORD::make_shared<C2SPacketFirstShowStoreProp>();
	packet->propId = propId;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendEffectGunFire(bool isNeedConsumeBullet, const Vector3& beginPos, const Vector3& endPos)
{
	auto packet = LORD::make_shared<C2SPacketEffectGunFire>();
	packet->isNeedConsumeBullet = isNeedConsumeBullet;
	packet->beginPos = beginPos;
	packet->endPos = endPos;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendEffectGunFireResult(int hitEntityId, const Vector3& hitPos, const Vector3& dir, int gunId, int traceType, const Vector3i& blockPos, bool isHeadshot)
{
	auto packet = LORD::make_shared<C2SPacketEffectGunFireResult>();
	packet->hitEntityId = hitEntityId;
	packet->gunId = gunId;
	packet->blockPos = blockPos;
	packet->hitPos = hitPos;
	packet->dir = dir;
	packet->traceType = traceType;
	packet->isHeadshot = isHeadshot;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendEffectGunFireExplosionResult(const Vector3& hitPos, float explosionRange)
{
	auto packet = LORD::make_shared<C2SPacketEffectGunFireExplosionResult>();
	packet->hitPos = hitPos;
	packet->explosionRange = explosionRange;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendStopLaserGun()
{
	auto packet = LORD::make_shared<C2SPacketStopLaserGun>();
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendConsumeBullet()
{
	auto packet = LORD::make_shared<C2SPacketConsumeBullet>();
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendPixelGunSelectBox(int index)
{
	auto packet = LORD::make_shared<C2SPacketPixelGunSelectBox>();
	packet->index = index;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendPixelGunExit()
{
	auto packet = LORD::make_shared<C2SPacketPixelGunExit>();
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendPixelGunRematch()
{
	auto packet = LORD::make_shared<C2SPacketPixelGunRematch>();
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendPixelGunRevive()
{
	auto packet = LORD::make_shared<C2SPacketPixelGunRevive>();
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendOpenLotteryChest(int chestId)
{
	auto packet = LORD::make_shared<C2SPacketOpenLotteryChest>();
	packet->chestId = chestId;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendOpenArmorUpgrade()
{
	auto packet = LORD::make_shared<C2SPacketOpenArmorUpgrade>();
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendUpgradeArmor(bool state, int operateType)
{
	auto packet = LORD::make_shared<C2SPacketUpgradeArmor>();
	packet->state = state;
	packet->operateType = operateType;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendPixelGunResultBack()
{
	auto packet = LORD::make_shared<C2SPacketPixelGunResultBack>();
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendPixelGunResultNext()
{
	auto packet = LORD::make_shared<C2SPacketPixelGunResultNext>();
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendPixelGunResultRevenge()
{
	auto packet = LORD::make_shared<C2SPacketPixelGunResultRevenge>();
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendPixelGunFight(String gameType, String mapName)
{
	auto packet = LORD::make_shared<C2SPacketPixelGunFight>();
	packet->gameType = gameType;
	packet->mapName = mapName;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendBirdBagOperation(i64 birdId, i32 operationType)
{
	//operationType 1 carry - 2 cancel carry - 3 delete - 4 expand Carry - 5 expand Capacity 6 convert
	auto packet = LORD::make_shared<C2SPacketBirdBagOperation>();
	packet->birdId = birdId;
	packet->operationType = operationType;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendBirdStoreOperation(i32 storeId, i32 goodsId, i32 operationType)
{
	auto packet = LORD::make_shared<C2SPacketBirdStoreOperation>();
	packet->storeId = storeId;
	packet->goodsId = goodsId;
	packet->operationType = operationType;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendBirdPersonalStoreBuyGoods(i32 tabId, i32 goodsId)
{
	auto packet = LORD::make_shared<C2SPacketBirdPersonalStoreBuyGoods>();
	packet->tabId = tabId;
	packet->goodsId = goodsId;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendBirdFuse(const vector<i64>::type& birdIds)
{
	auto packet = LORD::make_shared<C2SPacketBirdFuse>();
	packet->birdIds = birdIds;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendBirdOpenTreasureChest(i32 entityId)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto packet = LORD::make_shared<C2SPacketBirdOpenTreasureChest>();
	packet->entityId = dataCache->getServerId(entityId);
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendBirdLottery(i32 entityId)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto packet = LORD::make_shared<C2SPacketBirdLottery>();
	packet->entityId = dataCache->getServerId(entityId);
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendBirdTask(i32 entityId, i32 taskId)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto packet = LORD::make_shared<C2SPacketBirdTask>();
	packet->entityId = entityId;
	packet->taskId = taskId;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendBirdFeed(i64 birdId, i32 foodId, i32 num)
{
	auto packet = LORD::make_shared<C2SPacketBirdFeed>();
	packet->birdId = birdId;
	packet->foodId = foodId;
	packet->num = num;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}

void ClientPacketSender::sendBirdSetDress(i64 birdId, i32 dressType, i32 dressId)
{
	auto packet = LORD::make_shared<C2SPacketBirdSetDress>();
	packet->birdId = birdId;
	packet->dressType = dressType;
	packet->dressId = dressId;
	GameClient::CGame::Instance()->getNetwork()->sendPacket(packet, true);
}