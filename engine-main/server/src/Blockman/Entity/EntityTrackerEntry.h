/********************************************************************
filename: 	EntityTrackerEntry.h
file path:	h:\sandboxol\client\blockmango-client\dev\server\src\Entity
version:	1
author:		zhouyou
company:    sandboxol
date:		2017/07/11
*********************************************************************/
#ifndef __ENTITY_TRACKER_ENTRY_H__
#define __ENTITY_TRACKER_ENTRY_H__

#include "BM_Container_def.h"

#include <memory>

class DataPacket;
class ServerPacketSender;

namespace BLOCKMAN
{

class Entity;
class EntityPlayerMP;

class EntityTrackerEntry : public ObjectAlloc
{
public:
	Entity* myEntity = nullptr;
	int blocksDistanceThreshold = 0;

	/** check for sync when ticks % updateFrequency==0 */
	int updateFrequency = 0;
	Vector3i lastScaledPos;
	float lastYaw = 0.0f;
	float lastPitch = 0.0f;
	float lastHeadMotion = 0.0f;
	Vector3 motion;
	unsigned ticks = 0;
	bool playerEntitiesUpdated = false;

	/** Holds references to all the players that are currently receiving position updates for this entity. */
	EntityPlayers trackingPlayers;

protected:
	Vector3 position;

	/** set to true on first sendLocationToClients */
	bool isDataInitialized = false;
	bool sendVelocityUpdates = false;

	/** every 400 ticks a  full teleport packet is sent, rather than just a "move me +x" command, so that position remains fully synced. */
	int ticksSinceLastForcedTeleport = 0;
	bool forceTeleportNow = false;
	Entity* passengers = nullptr;
	bool ridingEntity = false;
	
public:
	EntityTrackerEntry(Entity* pEntity, int range, int frequency, bool sendVelocity);
	void reinit(Entity* pEntity, int range, int frequency, bool sendVelocity);
	~EntityTrackerEntry();

	bool equals(const EntityTrackerEntry& rhs);
	int hashCode();
	
	/** also sends velocity, rotation, and riding info. */
	void informMovementChangeToTrackingPlayers(EntityPlayers& players);

	void informAllAssociatedPlayersOfItemDestruction();

	void removeFromWatchingList(EntityPlayerMP* pPlayer);

	/** if the player is more than the distance threshold (typically 64) then the player is removed instead */
	void spawnOrRemoveSelfEntityAtClientOf(EntityPlayerMP* pPlayer, bool rebirth = false);

	void spawnOrRemoveSelfEntityFor(EntityPlayers& players, bool rebirth = false);

	void removePlayerFromTracker(EntityPlayerMP* pPlayer);
	
	void forceTelport();

protected:
	
	bool isPlayerWatchingThisChunk(EntityPlayerMP* pPlayer);
	void spawnSelfEntityAtClientOf(EntityPlayerMP* pPlayer);

private:
	bool shouldInformMovement();
	void initializeData(EntityPlayers& players);
	void updatePassengers();
	void sendBeingAttacked();
	bool handleItemFrame();
	void sendHeadRotation();
	void sendMovement();
	void sendVelocityChange();

	typedef std::function<void(const std::shared_ptr<ServerPacketSender>& sender, EntityPlayerMP* targetPlayer)> SyncEntityInfoFunc;
	std::map<int, SyncEntityInfoFunc> m_syncEntityInfoFuncMap;
	void initSyncEntityInfoFuncMap();
	void uninitSyncEntityInfoFuncMap();

	void syncEntityPlayerInfo(const std::shared_ptr<ServerPacketSender>& sender, EntityPlayerMP* targetPlayer);
	void syncEntityMerchantInfo(const std::shared_ptr<ServerPacketSender>& sender, EntityPlayerMP* targetPlayer);
	void syncEntityVehicileInfo(const std::shared_ptr<ServerPacketSender>& sender, EntityPlayerMP* targetPlayer);
	void syncEntityAircraftInfo(const std::shared_ptr<ServerPacketSender>& sender, EntityPlayerMP* targetPlayer);
	void syncEntityRankNpcInfo(const std::shared_ptr<ServerPacketSender>& sender, EntityPlayerMP* targetPlayer);
	void syncEntityActorNpcInfo(const std::shared_ptr<ServerPacketSender>& sender, EntityPlayerMP* targetPlayer);
	void syncEntitySessionNpcInfo(const std::shared_ptr<ServerPacketSender>& sender, EntityPlayerMP* targetPlayer);
	void syncEntityCreatureInfo(const std::shared_ptr<ServerPacketSender>& sender, EntityPlayerMP* targetPlayer);
	void syncEntityBlockmanInfo(const std::shared_ptr<ServerPacketSender>& sender, EntityPlayerMP* targetPlayer);
	void syncEntityBlockmanEmptyInfo(const std::shared_ptr<ServerPacketSender>& sender, EntityPlayerMP* targetPlayer);
	void syncEntityActorCannonInfo(const std::shared_ptr<ServerPacketSender>& sender, EntityPlayerMP* targetPlayer);
	void syncEntityBuildNpcInfo(const std::shared_ptr<ServerPacketSender>& sender, EntityPlayerMP* targetPlayer);
	void syncEntityLandNpcInfo(const std::shared_ptr<ServerPacketSender>& sender, EntityPlayerMP* targetPlayer);
	void syncEntityBulletinInfo(const std::shared_ptr<ServerPacketSender>& sender, EntityPlayerMP* targetPlayer);
	void syncEntityBirdInfo(const std::shared_ptr<ServerPacketSender>& sender, EntityPlayerMP* targetPlayer);
};


}

#endif