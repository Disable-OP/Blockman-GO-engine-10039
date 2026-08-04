/********************************************************************
filename: 	CommonPacketSender
file path:	H:\sandboxol\client\blockmango-client\dev\common\Src\Network\CommonPacketSender.h

version:	1
author:		qutianxiang
company:    sandboxol
date:		2017/06/28
*********************************************************************/
#ifndef __COMMONPACKETSENDER_H__
#define __COMMONPACKETSENDER_H__
#include "TypeDef.h"
#include "Network/NetworkDefine.h"
#include "BM_Container_def.h"

namespace BLOCKMAN {
	class EntityPlayer;
	class IInventory;
	class EntityBlockman;
}

class CommonPacketSender
{
public:
	CommonPacketSender(){};
	~CommonPacketSender(){};
	virtual void sendSplashPotion(const Vector3& playerPos, float playerYaw, float playerPitch) {};
	virtual void sendLaunchArrow(float cdTime, const Vector3& playerPos, float playerYaw, float playerPitch,int arrowID) {};
	virtual void sendLaunchBullet(int bulletID, const Vector3& playerPos, float playerYaw, float playerPitch) {};
	virtual void sendSpawnVehicle(int carID, const Vector3& pos, float yaw) {};
	virtual void sendGunFireResult(const Vector3& pos, const Vector3& dir, int tracyType, const Vector3i& blockPos, const Vector3& hitpos, int hitEnttityID, bool isNeedConsumeBullet, bool isHeadshot) {};
	virtual void sendUseItem(NETWORK_DEFINE::C2SPacketUseItemUseType useType, const Vector3i& pos = Vector3i(0, 0, 0), int param0 = 0) {};
	virtual void sendEffectGunFire(bool isNeedConsumeBullet, const Vector3& beginPos, const Vector3& endPos) {};
	virtual void sendChestInventory(BLOCKMAN::EntityPlayer* player, Vector3i blockPos, int face, Vector3 hitPos, BLOCKMAN::IInventory* inventory) {};
	virtual void sendPlayerAction(NETWORK_DEFINE::PacketPlayerActionType action) {};
	virtual void sendPlayerActionToTrackingPlayers(NETWORK_DEFINE::PacketPlayerActionType action, int playerId, bool includeSelf = false) {};
	virtual void sendUseThrowableItem(){};
	virtual void sendPlaySound(BLOCKMAN::EntityPlayer* player, const String& soundName, float volumn, float pitch) {};
	virtual void sendExplosion(BLOCKMAN::EntityPlayer* player, const Vector3& pos, float size, const BLOCKMAN::BlockPosArr& poss, const Vector3& motion, bool isSmoke, float custom_size = 1.0f) {};
	virtual void sendVehicleMovement(int vehicleId, bool onGround, bool moved, bool rotated, float x, float y, float z, float yaw) {};
	virtual void sendVehicleActionState(int vehicleId, int state) {};
	virtual void sendDestroyArmorToTrackingPlayers(int entityId, int slotIndex) {};
	virtual void sendPlayerInventory(int entityId) {};
	virtual void syncEntityBlockman(ui64 rakssId, BLOCKMAN::EntityBlockman* blockMan) {};
	virtual void syncBlockmanMoveAttr(ui64 rakssId, BLOCKMAN::EntityBlockman* blockMan) {};
	virtual void sendUseFishingRod() {};
	virtual void syncRanch(BLOCKMAN::EntityPlayer* player, int syncType) {}
	virtual void syncRanchRank(BLOCKMAN::EntityPlayer* player, int rankType) {}
	virtual void syncRanchBuild(BLOCKMAN::EntityPlayer* player, int buildType) {}
	virtual void syncMail(BLOCKMAN::EntityPlayer* player) {}
	virtual void buildNotification(i64 userId, const String& message){}
	virtual void syncBirdSimulator(BLOCKMAN::EntityPlayer* player, int syncType) {}
	virtual void onBirdSimulatorGather(ui64 userId, i64 birdId, const BLOCKMAN::BlockPos& pos) {}
};
#endif