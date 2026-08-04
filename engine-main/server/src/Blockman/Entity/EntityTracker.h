/********************************************************************
filename: 	EntityTracker.h
file path:	h:\sandboxol\client\blockmango-client\dev\server\src\Entity
version:	1
author:		zhouyou
company:    sandboxol
date:		2017/07/11
*********************************************************************/
#ifndef __ENTITY_TRACKER_H__
#define __ENTITY_TRACKER_H__

#include "EntityTrackerEntry.h"

namespace BLOCKMAN
{
class World;
class Entity;
class EntityPlayerMP;

typedef set<EntityTrackerEntry*>::type EntityTrackerSet;
typedef map<int, EntityTrackerEntry*>::type IDEntityTrackerMap;

class EntityTracker : public Singleton<EntityTracker>, public ObjectAlloc
{
protected:
	World* m_world;

	/** List of tracked entities, used for iteration operations on tracked entities. */
	EntityTrackerSet m_entitySet;
	IDEntityTrackerMap m_idEntityMap;
	int m_entityViewDistance;

public:
	EntityTracker(World* pWorld);
	virtual ~EntityTracker();

	/** if entity is a player sends all tracked events to the player, otherwise, adds with a visibility and update arate based on the class type */
	void addEntityToTracker(Entity* toAddEntity);

	void addEntityToTracker(Entity* toAddEntity, int viewDis, int frequency, bool sendVelocity = false);

	void removeEntityFromAllTrackingPlayers(Entity* pEntity);

	void updateTrackedEntities();
	
	void removePlayerFromTrackers(EntityPlayerMP* pPlayer);
	
	void sendLeashedEntitiesInChunk(EntityPlayerMP* pPlayer, const ChunkPtr& pChunk);

	const EntityPlayers getTrackingPlayersOf(int entityId) const;
	
	bool forceTeleport(int entityId);
};

}

#endif