#include "EntityTracker.h"
#include "EntityPlayerMP.h"

#include "World/World.h"
#include "Chunk/Chunk.h"


namespace BLOCKMAN
{

EntityTracker::EntityTracker(World* pWorld)
{
	m_world = pWorld;
	m_entityViewDistance = 64;
}

EntityTracker::~EntityTracker()
{
	for (auto i : m_entitySet) {
		EntityTrackerEntry* pEntry = i;
		LordDelete(pEntry);
	}
	m_entitySet.clear();
	m_idEntityMap.clear();
}

void EntityTracker::addEntityToTracker(Entity* toAddEntity)
{
	if (toAddEntity->isClass(ENTITY_CLASS_PLAYERMP))
	{
		EntityPlayerMP* pPlayer = (EntityPlayerMP*)toAddEntity;
		if (pPlayer->isLogout())
			return;
		addEntityToTracker(toAddEntity, 512, 2);
		for (auto it : m_entitySet)
		{
			EntityTrackerEntry* pEntry = it;
			if (pEntry->myEntity != pPlayer)
				pEntry->spawnOrRemoveSelfEntityAtClientOf(pPlayer);
		}
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_FISHHOOK))
	{
		addEntityToTracker(toAddEntity, 64, 5, false);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_ARROW))
	{
		addEntityToTracker(toAddEntity, 64, 20, false);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_BULLET))
	{
		addEntityToTracker(toAddEntity, 128, 20, false);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_SMALL_FIRE_BALL))
	{
		addEntityToTracker(toAddEntity, 64, 10, false);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_FIRE_BALL))
	{
		addEntityToTracker(toAddEntity, 64, 10, false);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_ENDER_PEARL))
	{
		addEntityToTracker(toAddEntity, 64, 10, true);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_ENDER_EYE))
	{
		addEntityToTracker(toAddEntity, 64, 4, true);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_EGG))
	{
		addEntityToTracker(toAddEntity, 64, 10, true);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_POTION))
	{
		addEntityToTracker(toAddEntity, 64, 10, true);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_EXP_BOTTLE))
	{
		addEntityToTracker(toAddEntity, 64, 10, true);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_FIREWORK_ROCKET))
	{
		addEntityToTracker(toAddEntity, 64, 10, true);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_ITEM))
	{
		addEntityToTracker(toAddEntity, 64, 20, false);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_BLOCKMAN))
	{
		addEntityToTracker(toAddEntity, 64, 20, true);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_BOAT))
	{
		addEntityToTracker(toAddEntity, 80, 3, true);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_SQUID))
	{
		addEntityToTracker(toAddEntity, 64, 3, true);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_WITHER))
	{
		addEntityToTracker(toAddEntity, 80, 3, false);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_BAT))
	{
		addEntityToTracker(toAddEntity, 80, 3, false);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_ANIMALS))
	{
		addEntityToTracker(toAddEntity, 80, 3, true);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_DRAGON))
	{
		addEntityToTracker(toAddEntity, 160, 3, true);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_TNT_PRIMED))
	{
		addEntityToTracker(toAddEntity, 160, 10, true);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_FALLING_SAND))
	{
		addEntityToTracker(toAddEntity, 160, 20, true);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_HANGING))
	{
		addEntityToTracker(toAddEntity, 160, Math::MAX_I32, false);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_XPORB))
	{
		addEntityToTracker(toAddEntity, 160, 20, true);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_ENDER_CRYSTAL))
	{
		addEntityToTracker(toAddEntity, 256, Math::MAX_I32, false);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_SNOWBALL))
	{
		addEntityToTracker(toAddEntity, 64, 20, false);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_TNT_THROWABLE))
	{
		addEntityToTracker(toAddEntity, 64, 20, false);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_MERCHANT))
	{
		addEntityToTracker(toAddEntity, 64, 20, false);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_GRENADE))
	{
		addEntityToTracker(toAddEntity, 64, 20, false);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_VEHICLE))
	{
		addEntityToTracker(toAddEntity, 128, 10, false);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_RANK_NPC))
	{
		addEntityToTracker(toAddEntity, 64, 20, false);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_AIRCRAFT))
	{
		addEntityToTracker(toAddEntity, 128, 20, false);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_ACTOR_NPC)) {
		addEntityToTracker(toAddEntity, 64, 20, false);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_SESSION_NPC)) {
		addEntityToTracker(toAddEntity, 64, 20, false);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_CREATUREAI)) {
		addEntityToTracker(toAddEntity, 256, 6, false);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_CREATURE_BULLET)) {
		addEntityToTracker(toAddEntity, 128, 20, false);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_ITEM_SKILL))
	{
		addEntityToTracker(toAddEntity, 64, 20, false);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_BLOCKMAN_EMPTY)) {
		addEntityToTracker(toAddEntity, 64, 20, false);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_BUILD_NPC)) {
		addEntityToTracker(toAddEntity, 64, 20, false);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_LAND_NPC)) {
		addEntityToTracker(toAddEntity, 64, 20, false);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_ACTOR_CANNON)) {
		addEntityToTracker(toAddEntity, 64, 20, false);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_BULLETIN)) {
		addEntityToTracker(toAddEntity, 64, 20, false);
	}
	else if (toAddEntity->isClass(ENTITY_CLASS_BIRDAI)) {
		addEntityToTracker(toAddEntity, 64, 6, false);
	}
	else {
		LordLogError("EntityTracker::addEntityToTracker, entity is belong to unknown class");
	}
}

void EntityTracker::addEntityToTracker(Entity* toAddEntity, int viewDis, int frequency, bool sendVelocity)
{
	if (viewDis > m_entityViewDistance)
	{
		viewDis = m_entityViewDistance;
	}

	EntityTrackerEntry* pEntry = nullptr;
	bool rebirth = false;
	if (m_idEntityMap.find(toAddEntity->entityId) == m_idEntityMap.end()) {
		pEntry = LordNew EntityTrackerEntry(toAddEntity, viewDis, frequency, sendVelocity);
		m_entitySet.insert(pEntry);
		m_idEntityMap.insert(std::make_pair(toAddEntity->entityId, pEntry));
	}
	else {
		pEntry = m_idEntityMap.find(toAddEntity->entityId)->second;
		pEntry->reinit(toAddEntity, viewDis, frequency, sendVelocity);
		rebirth = true;
		LordLogInfo("Entity is already tracked,id=%d, name=%s", toAddEntity->entityId, toAddEntity->getEntityName().c_str());
	}

	//LordLogInfo("Add entity [%s] to EntityTracker list", toAddEntity->getEntityName().c_str());
	pEntry->spawnOrRemoveSelfEntityFor(m_world->getPlayers(), rebirth);
}

void EntityTracker::removeEntityFromAllTrackingPlayers(Entity* pEntity)
{
	if (pEntity->isClass(ENTITY_CLASS_PLAYERMP))
	{
		EntityPlayerMP* pPlayer = (EntityPlayerMP*)pEntity;

		for (auto it : m_entitySet)
		{
			EntityTrackerEntry* pEntry = it;
			//LordLogInfo("Del player [%s] to EntityTracker list", pEntity->getEntityName().c_str());
			it->removeFromWatchingList(pPlayer);
		}
	}

	auto it = m_idEntityMap.find(pEntity->entityId);
	if (it != m_idEntityMap.end())
	{
		EntityTrackerEntry* pEntry = it->second;
		if (pEntry)
		{
			m_entitySet.erase(pEntry);
			pEntry->informAllAssociatedPlayersOfItemDestruction();
			m_idEntityMap.erase(it);

			LordDelete(pEntry);
		}
	}
}

void EntityTracker::updateTrackedEntities()
{
	list<EntityPlayerMP*>::type players;

	for(auto it : m_entitySet)
	{
		EntityTrackerEntry* pEntry = it;
		pEntry->informMovementChangeToTrackingPlayers(m_world->getPlayers());

		if (pEntry->playerEntitiesUpdated && pEntry->myEntity->isClass(ENTITY_CLASS_PLAYERMP))
		{
			players.push_back((EntityPlayerMP*)pEntry->myEntity);
		}
	}

	for (auto it : players)
	{
		EntityPlayerMP* pPlayer = it;
		
		for (auto jt : m_entitySet)
		{
			EntityTrackerEntry* pEntry = jt;

			if (pEntry->myEntity != pPlayer)
			{
				pEntry->spawnOrRemoveSelfEntityAtClientOf(pPlayer);
			}
		}
	}
}

void EntityTracker::removePlayerFromTrackers(EntityPlayerMP* pPlayer)
{
	for (auto it : m_entitySet)
	{
		EntityTrackerEntry* pEntry = it;
		it->removePlayerFromTracker(pPlayer);
	}
}

void EntityTracker::sendLeashedEntitiesInChunk(EntityPlayerMP* pPlayer, const ChunkPtr& pChunk)
{
	for (auto it : m_entitySet)
	{
		EntityTrackerEntry* pEntry = it;

		if (pEntry->myEntity != pPlayer && 
			pEntry->myEntity->chunkCoord.x == pChunk->m_posX &&
			pEntry->myEntity->chunkCoord.z == pChunk->m_posZ)
		{
			pEntry->spawnOrRemoveSelfEntityAtClientOf(pPlayer);
		}
	}
}

const EntityPlayers EntityTracker::getTrackingPlayersOf(int entityId) const
{
	auto iter = m_idEntityMap.find(entityId);
	if (iter == m_idEntityMap.end())
	{
		return {};
	}
	return iter->second->trackingPlayers;
}

bool EntityTracker::forceTeleport(int entityId)
{
	auto iter = m_idEntityMap.find(entityId);
	if (iter == m_idEntityMap.end()) {
		return false;
	}

	iter->second->forceTelport();

	return true;
}

}
