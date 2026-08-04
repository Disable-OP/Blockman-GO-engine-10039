
#ifndef __S2CENTITYPACKETHANDLES_H__
#define __S2CENTITYPACKETHANDLES_H__

#include "Network/protocol/S2CPackets.h"
#include <memory>

class S2CEntityPacketHandles
{
public:
	static void handlePacket(std::shared_ptr<S2CPacketRemoveEntity>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketEntityMovement>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketEntityTeleport>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketEntityVelocity>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketEntityHeadRotation>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSpawnExpOrb>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSpawnMob>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSpawnPlayer>& packet);
	static void handlePacket(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketAddEntityEffect>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketRemoveEntityEffect>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncCreateArrowTipMark>& packet);	
	static void handlePacket(std::shared_ptr<S2CPacketEntityPlayerTeleport>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSpawnCreatureBullet>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncCreatureEntityAction>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncEntityCreature>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSpwanSkillItemThrowable>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketActorNpcContent>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketEnableMovement>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncEntityBlockmanInfo>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketBlockmanMoveAttr>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketEntityFrozenTime>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketEntityPlayerTeleportWithMotion>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSpawnCannon>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncEntityBuildNpc>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncEntityLandNpc>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketEntityHurtTime>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketEntityColorfulTime>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketEntitySelected>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketEntityDisableMoveTime>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncEntityBulletin>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncEntityBird>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncEntityAction>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncEntityHealth>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketChangeGuideArrowStatus>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketAddWallText>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketDeleteWallText>& packet);

private:
	static void createEntityItem(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet);
	static void createEntityArrow(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet);
	static void createEntityBullet(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet);
	static void createEntityVehicle(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet);
	static void createEntitySnowball(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet);
	static void createEntityTNTPrimed(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet);
	static void createEntityPotion(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet);
	static void createEntityGrenade(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet);
	static void createEntityTNTThrowable(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet);
	static void createEntityMerchant(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet);
	static void createEntityRank(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet);
	static void createEntityAircraft(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet);
	static void createEntityActortNpc(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet);
	static void createEntitySessionNpc(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet);
	static void createEntityCreature(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet);
	static void createEntityFireball(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet);
	static void createEntityBlockman(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet);
	static void createEntityFishHook(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet);
	static void createEntityBuildNpc(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet);
	static void createEntityLandNpc(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet);
	static void createEntityBulletin(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet);
	static void createEntityBird(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet);
	static void updateEntityBirdAction(std::shared_ptr<S2CPacketSyncEntityAction>& packet);
};

#endif
