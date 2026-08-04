#include "EntityPlayerMP.h"

#include "EntityTrackerEntry.h"
#include "Entity/Entity.h"
#include "Entity/EntityArrow.h"
#include "Entity/EntityThrowable.h"
#include "Entity/EntityMerchant.h"
#include "Entity/EntityRankNpc.h"
#include "Entity/EntityBlockmanEmpty.h"
#include "Server.h"
#include "Network/ServerPacketSender.h"
#include "Network/ServerNetwork.h"
#include "Network/ClientPeer.h"
#include "RoomManager.h"
#include "CommodityManager.h"
#include "Entity/EntityVehicle.h"
#include "Entity/EntityAircraft.h"
#include "Entity/EntityActorNpc.h"
#include "Script/GameServerEvents.h"
#include "Entity/EntitySessionNpc.h"
#include "Entity/EntityBlockman.h"
#include "Entity/EntityActorCannon.h"
#include "Entity/EntityBuildNpc.h"
#include "Entity/EntityLandNpc.h"
#include "Entity/EntityBulletin.h"
#include "Blockman/Entity/EntityBirdAI.h"

#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
#define I64FMT "%I64d"
#define U64FMT "%I64u"
#else
#define I64FMT "%lld"
#define U64FMT "%llu"
#endif

namespace BLOCKMAN
{
	EntityTrackerEntry::EntityTrackerEntry(Entity* pEntity, int range, int frequency, bool sendVelocity)
	{
		reinit(pEntity, range, frequency, sendVelocity);
	}

	void EntityTrackerEntry::reinit(Entity* pEntity, int range, int frequency, bool sendVelocity)
	{
		myEntity = pEntity;
		blocksDistanceThreshold = range;
		updateFrequency = frequency;
		sendVelocityUpdates = sendVelocity;
		// all the position is scale by 32. server use INT type.
		/*if (dynamic_cast<EntityPlayer*>(myEntity)) {
			LordLogInfo("EntityTrackerEntry construct, position=(%f,%f,%f), id=%d", pEntity->position.x,
				pEntity->position.y, pEntity->position.z, pEntity->entityId);
		}*/
		lastScaledPos.x = int(Math::Floor(pEntity->position.x * 32.0f));
		lastScaledPos.y = int(Math::Floor(pEntity->position.y * 32.0f));
		lastScaledPos.z = int(Math::Floor(pEntity->position.z * 32.0f));
		// lastYaw = int(Math::Floor(pEntity->rotationYaw * 256.0F / 360.0F));
		// lastPitch = int(Math::Floor(pEntity->rotationPitch * 256.0F / 360.0F));
		// lastHeadMotion = int(Math::Floor(pEntity->getRotationYawHead() * 256.0F / 360.0F));

		lastYaw = pEntity->rotationYaw;
		lastPitch = pEntity->rotationPitch;
		lastHeadMotion = pEntity->getRotationYawHead();

		motion = Vector3::ZERO;
		ticks = 0;
		position = Vector3::ZERO;
		isDataInitialized = false;
		playerEntitiesUpdated = false;
		ticksSinceLastForcedTeleport = false;
		passengers = NULL;
		ridingEntity = false;
		initSyncEntityInfoFuncMap();
	}

	EntityTrackerEntry::~EntityTrackerEntry()
	{
		uninitSyncEntityInfoFuncMap();
	}

	bool EntityTrackerEntry::equals(const EntityTrackerEntry& rhs)
	{
		return myEntity->entityId == rhs.myEntity->entityId;
	}

	int EntityTrackerEntry::hashCode()
	{
		return myEntity->entityId;
	}

	void EntityTrackerEntry::informMovementChangeToTrackingPlayers(EntityPlayers& players)
	{
		if (!shouldInformMovement())
			return;

		initializeData(players);
		updatePassengers();

		if (handleItemFrame())
			return;

		sendMovement();

		++ticks;
		sendBeingAttacked();
	}

	void EntityTrackerEntry::informAllAssociatedPlayersOfItemDestruction()
	{
		Server::Instance()->getNetwork()->getSender()->sendRemoveEntityToTrackingPlayers(myEntity->entityId);
	}

	void EntityTrackerEntry::removeFromWatchingList(EntityPlayerMP* pPlayer)
	{
		auto it = trackingPlayers.find(pPlayer->entityId);
		if (it != trackingPlayers.end())
		{
			// pPlayer.destroyedItemsNetCache.add(Integer.valueOf(myEntity.entityId));
			trackingPlayers.erase(it->first);
		}
	}

	void EntityTrackerEntry::spawnOrRemoveSelfEntityAtClientOf(EntityPlayerMP* pPlayer, bool rebirth/* = false*/)
	{
		if (pPlayer == myEntity || pPlayer->isLogout())
			return;

		float dx = Math::Abs(pPlayer->position.x - lastScaledPos.x / 32.f);
		float dz = Math::Abs(pPlayer->position.z - lastScaledPos.z / 32.f);
		if (pPlayer->m_isCameraLocked)
		{
			float cdx = Math::Abs(pPlayer->m_cameraLockPos.x - lastScaledPos.x / 32.f);
			float cdz = Math::Abs(pPlayer->m_cameraLockPos.z - lastScaledPos.z / 32.f);
			dx = Math::Min(dx, cdx);
			dz = Math::Min(dz, cdz);
		}

		bool invisible = false;
		if (auto myPlayer = dynamic_cast<EntityPlayer*>(myEntity))
		{
			invisible = myPlayer->capabilities.isWatchMode && !pPlayer->capabilities.isWatchMode;
		}

		if (dx <= blocksDistanceThreshold
			&& dz <= blocksDistanceThreshold
			&& !invisible)
		{
			if ((trackingPlayers.find(pPlayer->entityId) == trackingPlayers.end() || rebirth)
				&& (isPlayerWatchingThisChunk(pPlayer) || myEntity->forceSpawn))
			{
				trackingPlayers[pPlayer->entityId] = pPlayer;
				//lastHeadMotion = int(Math::Floor(myEntity->getRotationYawHead() * 256.0F / 360.0F));
				lastHeadMotion = myEntity->getRotationYawHead();

				spawnSelfEntityAtClientOf(pPlayer);

				if (myEntity->isClass(ENTITY_CLASS_LIVING_BASE))
				{
					// todo.

				}

				motion = myEntity->motion;

			}
		}
		else if (trackingPlayers.find(pPlayer->entityId) != trackingPlayers.end())
		{
			trackingPlayers.erase(pPlayer->entityId);
			Server::Instance()->getNetwork()->getSender()->sendRemoveEntity(myEntity->entityId, pPlayer);
		}
	}

	void EntityTrackerEntry::spawnOrRemoveSelfEntityFor(EntityPlayers& players, bool rebirth/* = false*/)
	{
		for (auto it : players)
		{
			spawnOrRemoveSelfEntityAtClientOf((EntityPlayerMP*)(it.second), rebirth);
		}
	}

	void EntityTrackerEntry::removePlayerFromTracker(EntityPlayerMP* pPlayer)
	{
		auto it = trackingPlayers.find(pPlayer->entityId);
		if (it != trackingPlayers.end())
		{
			trackingPlayers.erase(it);
			// pPlayer.destroyedItemsNetCache.add(Integer.valueOf(myEntity.entityId));
		}
	}

	void EntityTrackerEntry::forceTelport()
	{
		forceTeleportNow = true;
	}

	bool EntityTrackerEntry::isPlayerWatchingThisChunk(EntityPlayerMP* pPlayer)
	{
		// todo.
		// return pPlayer.getServerForPlayer().getPlayerManager().isPlayerWatchingChunk(pPlayer, myEntity.chunkCoordX, myEntity.chunkCoordZ);
		return true;
	}

	void EntityTrackerEntry::spawnSelfEntityAtClientOf(EntityPlayerMP* targetPlayer)
	{
		auto sender = Server::Instance()->getNetwork()->getSender();
		sender->sendEntitySpawn(targetPlayer->getRaknetID(), myEntity);

		auto iter = m_syncEntityInfoFuncMap.find((int)myEntity->getClassID());
		if (iter != m_syncEntityInfoFuncMap.end())
		{
			auto func = iter->second;
			func(sender, targetPlayer);
		}
	}

	bool EntityTrackerEntry::shouldInformMovement()
	{
		if (myEntity->isClass(ENTITY_CLASS_ARROW) ||
			myEntity->isClass(ENTITY_CLASS_BULLET) ||
			myEntity->isClass(ENTITY_CLASS_CREATURE_BULLET) ||
			myEntity->isClass(ENTITY_CLASS_ITEM_SKILL) ||
			myEntity->isClass(ENTITY_CLASS_FISHHOOK) ||
			dynamic_cast<EntityThrowable*>(myEntity))
		{
			return false;
		}

		return true;
	}

	void EntityTrackerEntry::initializeData(EntityPlayers& players)
	{
		playerEntitiesUpdated = false;

		if (!isDataInitialized || /*true*/myEntity->getDistanceSq(position) > 16.0f)
		{
			position = myEntity->position;
			isDataInitialized = true;
			playerEntitiesUpdated = true;
			spawnOrRemoveSelfEntityFor(players);
		}
	}

	void EntityTrackerEntry::updatePassengers()
	{
		if (passengers != myEntity->ridingEntity || myEntity->ridingEntity && ticks % 60 == 0)
		{
			passengers = myEntity->ridingEntity;
			// todo.
			// sendPacketToAllTrackingPlayers(new Packet39AttachEntity(0, myEntity, myEntity.ridingEntity));
		}
	}

	void EntityTrackerEntry::sendBeingAttacked()
	{
		if (myEntity->beenAttacked)
		{
			bool isEntityPlayer = myEntity->isClass(ENTITY_CLASS_PLAYERMP);
			if (!isEntityPlayer) // entityPlayer do not need velocity sync, comment by oujie
			{
				Server::Instance()->getNetwork()->getSender()->sendEntityVelocityToTrackingPlayers(trackingPlayers, myEntity);
				if (myEntity->isClass(ENTITY_CLASS_PLAYERMP))
				{
					Server::Instance()->getNetwork()->getSender()->sendEntityVelocity(((EntityPlayerMP*)myEntity)->getRaknetID(), myEntity);
				}
			}

			myEntity->beenAttacked = false;
		}
	}

	bool EntityTrackerEntry::handleItemFrame()
	{
		if (myEntity->isClass(ENTITY_CLASS_ITEM_FRAME) && ticks % 10 == 0)
		{
			return true;
		}

		return false;
	}

	void EntityTrackerEntry::sendHeadRotation()
	{
		float headYaw = myEntity->getRotationYawHead();

		if (Math::Abs(headYaw - lastHeadMotion) >= 4.0)
		{
			Server::Instance()->getNetwork()->getSender()->sendEntityHeadRotationToTrackingPlayers(trackingPlayers, myEntity->entityId, headYaw);
			lastHeadMotion = headYaw;
		}
	}

	void EntityTrackerEntry::sendMovement()
	{
		if (ticks % updateFrequency == 0 || myEntity->isAirBorne)
		{
			if (!myEntity->ridingEntity)
			{
				++ticksSinceLastForcedTeleport;
				Vector3i pos;
				pos.x = int(Math::Floor(myEntity->position.x * 32.f));
				pos.y = int(Math::Floor(myEntity->position.y * 32.f));
				pos.z = int(Math::Floor(myEntity->position.z * 32.f));
				float yaw = myEntity->rotationYaw;
				float pitch = myEntity->rotationPitch;
				Vector3i dpos = pos - lastScaledPos;

				bool moving = Math::Abs(dpos.x) >= 4 || Math::Abs(dpos.y) >= 4 || Math::Abs(dpos.z) >= 4;
				bool rotating = Math::Abs(yaw - lastYaw) >= 4.0 || Math::Abs(pitch - lastPitch) >= 4.0;

				if (myEntity->isClass(ENTITY_CLASS_PLAYER) || 
					myEntity->isClass(ENTITY_CLASS_VEHICLE) ||
					myEntity->isClass(ENTITY_CLASS_BLOCKMAN))
				{
					lastScaledPos = pos;
					if (!forceTeleportNow)
						return;
				}

				bool moveImmdi = true;
				if (myEntity->isClass(ENTITY_CLASS_BIRD))
					moveImmdi = false;

				sendVelocityChange();

				if (ticks > 0 && dpos.x >= -128 && dpos.x < 128 && dpos.y >= -128 && dpos.y < 128 && dpos.z >= -128 && dpos.z < 128
					&& ticksSinceLastForcedTeleport <= 400 && !forceTeleportNow && !ridingEntity)
				{
					if (moving && rotating)
					{
						Server::Instance()->getNetwork()->getSender()->sendEntityMovementToTrackingPlayers(trackingPlayers,
							myEntity->entityId, i16(dpos.x), i16(dpos.y), i16(dpos.z), yaw, pitch, moveImmdi);
						/*if (dynamic_cast<EntityPlayer*>(myEntity) && trackingPlayers.size() > 0) {
						LordLogInfo("send entity[%d] movement and rotate to tracking players, dp(%d, %d, %d) moving (%.1f, %.1f, %.1f)",
						myEntity->entityId, dpos.x, dpos.y, dpos.z, myEntity->position.x, myEntity->position.y, myEntity->position.z);
						}*/
					}
					else if (moving)
					{
						Server::Instance()->getNetwork()->getSender()->sendEntityMovementToTrackingPlayers(trackingPlayers,
							myEntity->entityId, i16(dpos.x), i16(dpos.y), i16(dpos.z), moveImmdi);
						/*if (dynamic_cast<EntityPlayer*>(myEntity) && trackingPlayers.size() > 0) {
						LordLogInfo("send entity[%d] movement to tracking players, dp(%d, %d, %d) moving (%.1f, %.1f, %.1f)",
						myEntity->entityId, dpos.x, dpos.y, dpos.z, myEntity->position.x, myEntity->position.y, myEntity->position.z);
						}*/
					}
					else if (rotating)
					{
						Server::Instance()->getNetwork()->getSender()->sendEntityMovementToTrackingPlayers(trackingPlayers,
							myEntity->entityId, yaw, pitch, moveImmdi);
						/*if (dynamic_cast<EntityPlayer*>(myEntity) && trackingPlayers.size() > 0) {
						LordLogInfo("send entity[%d] rotate to tracking players, rotate (%.1f, %.1f)",
						myEntity->entityId, myEntity->rotationYaw, myEntity->rotationPitch);
						}*/
					}
				}
				else
				{
					Server::Instance()->getNetwork()->getSender()->sendEntityTeleportToTrackingPlayers(trackingPlayers,
						myEntity->entityId, pos.x, pos.y, pos.z, yaw, pitch);

					EntityPlayerMP* self = dynamic_cast<EntityPlayerMP*>(myEntity);
					if (forceTeleportNow && self) {
						Server::Instance()->getNetwork()->getSender()->sendEntityTeleport(
							self->getRaknetID(), myEntity->entityId, pos.x, pos.y, pos.z, yaw, pitch);

						forceTeleportNow = false;
					}
					ticksSinceLastForcedTeleport = 0;
					/*if (dynamic_cast<EntityPlayer*>(myEntity)) {
					LordLogInfo("send entity[%d] teleport to tracking players and self, dp(%d, %d, %d) moving (%.1f, %.1f, %.1f)",
					myEntity->entityId, dpos.x, dpos.y, dpos.z, myEntity->position.x, myEntity->position.y, myEntity->position.z);
					}*/
				}

				if (moving)
				{
					/*if (dynamic_cast<EntityPlayer*>(myEntity)) {
					LordLogDebug("update entity[%d] lastScaledPos, ticks=%u, original=(%d,%d,%d), now=(%d,%d,%d), dpos=(%d,%d,%d)/32 = (%f,%f,%f)",
					myEntity->entityId, ticks, int(lastScaledPos.x/32), int(lastScaledPos.y/32), int(lastScaledPos.z/32),
					int(pos.x/32), int(pos.y/32), int(pos.z/32), dpos.x, dpos.y, dpos.z, dpos.x/32.0, dpos.y/32.0, dpos.z/32.0);
					}*/
					lastScaledPos = pos;
				}

				if (rotating)
				{
					lastYaw = yaw;
					lastPitch = pitch;
				}

				ridingEntity = false;
			}

			sendHeadRotation();
			myEntity->isAirBorne = false;
		}
	}

	void EntityTrackerEntry::sendVelocityChange()
	{
		if (sendVelocityUpdates)
		{
			Vector3 dm = myEntity->motion - motion;
			float var19 = 0.02f;
			float dmLenSqr = dm.lenSqr();

			if (dmLenSqr > var19 * var19 || dmLenSqr > 0.0f && myEntity->motion == Vector3::ZERO)
			{
				motion = myEntity->motion;
				Server::Instance()->getNetwork()->getSender()->sendEntityVelocityToTrackingPlayers(trackingPlayers, myEntity->entityId, motion.x, motion.y, motion.z);
			}
		}
	}

	void EntityTrackerEntry::initSyncEntityInfoFuncMap()
	{
		m_syncEntityInfoFuncMap = 
		{
			{ (int)ENTITY_CLASS_PLAYERMP, std::bind(&EntityTrackerEntry::syncEntityPlayerInfo, this, std::placeholders::_1, std::placeholders::_2) },
			{ (int)ENTITY_CLASS_MERCHANT, std::bind(&EntityTrackerEntry::syncEntityMerchantInfo, this, std::placeholders::_1, std::placeholders::_2) },
			{ (int)ENTITY_CLASS_VEHICLE, std::bind(&EntityTrackerEntry::syncEntityVehicileInfo, this, std::placeholders::_1, std::placeholders::_2) },
			{ (int)ENTITY_CLASS_AIRCRAFT, std::bind(&EntityTrackerEntry::syncEntityAircraftInfo, this, std::placeholders::_1, std::placeholders::_2) },
			{ (int)ENTITY_CLASS_RANK_NPC, std::bind(&EntityTrackerEntry::syncEntityRankNpcInfo, this, std::placeholders::_1, std::placeholders::_2) },
			{ (int)ENTITY_CLASS_ACTOR_NPC, std::bind(&EntityTrackerEntry::syncEntityActorNpcInfo, this, std::placeholders::_1, std::placeholders::_2) },
			{ (int)ENTITY_CLASS_SESSION_NPC, std::bind(&EntityTrackerEntry::syncEntitySessionNpcInfo, this, std::placeholders::_1, std::placeholders::_2) },
			{ (int)ENTITY_CLASS_CREATUREAI, std::bind(&EntityTrackerEntry::syncEntityCreatureInfo, this, std::placeholders::_1, std::placeholders::_2) },
			{ (int)ENTITY_CLASS_BLOCKMAN, std::bind(&EntityTrackerEntry::syncEntityBlockmanInfo, this, std::placeholders::_1, std::placeholders::_2) },
			{ (int)ENTITY_CLASS_BLOCKMAN_EMPTY, std::bind(&EntityTrackerEntry::syncEntityBlockmanEmptyInfo, this, std::placeholders::_1, std::placeholders::_2) },
			{ (int)ENTITY_CLASS_ACTOR_CANNON, std::bind(&EntityTrackerEntry::syncEntityActorCannonInfo, this, std::placeholders::_1, std::placeholders::_2) },
			{ (int)ENTITY_CLASS_BUILD_NPC, std::bind(&EntityTrackerEntry::syncEntityBuildNpcInfo, this, std::placeholders::_1, std::placeholders::_2) },
			{ (int)ENTITY_CLASS_LAND_NPC, std::bind(&EntityTrackerEntry::syncEntityLandNpcInfo, this, std::placeholders::_1, std::placeholders::_2) },
			{ (int)ENTITY_CLASS_BULLETIN, std::bind(&EntityTrackerEntry::syncEntityBulletinInfo, this, std::placeholders::_1, std::placeholders::_2) },
			{ (int)ENTITY_CLASS_BIRDAI, std::bind(&EntityTrackerEntry::syncEntityBirdInfo, this, std::placeholders::_1, std::placeholders::_2) },
		};
	}

	void EntityTrackerEntry::uninitSyncEntityInfoFuncMap()
	{
		m_syncEntityInfoFuncMap.clear();
	}

	void EntityTrackerEntry::syncEntityPlayerInfo(const std::shared_ptr<ServerPacketSender>& sender, EntityPlayerMP* targetPlayer)
	{
		EntityPlayerMP* selfPlayer = static_cast<EntityPlayerMP*>(myEntity);
		sender->sendSetWatchMode(targetPlayer->getRaknetID(), selfPlayer->entityId, selfPlayer->capabilities.isWatchMode);
		std::shared_ptr<ClientPeer> selfClientPeer = ServerNetwork::Instance()->findPlayerByRakssid(selfPlayer->getRaknetID());

		if (selfClientPeer) {
			if (selfPlayer->m_isClothesChanged)
			{
				sender->sendUserAttrFromEntity(targetPlayer->getRaknetID(), selfPlayer);
			}
			else
			{
				UserAttrInfo selfAttrInfo;
				if (Server::Instance()->getEnableRoom())
				{
					bool succ = Server::Instance()->getRoomManager()->getUserAttrInfo(selfClientPeer->getPlatformUserId(), selfAttrInfo);
					if (succ) {
						sender->sendUserAttr(targetPlayer->getRaknetID(), selfPlayer->entityId, selfAttrInfo);

						/*LordLogInfo("spawnSelfEntityAtClient, send user attr info, target player name %s,"
							"be sent player name %s,faceId[%d],hairId[%d],topsId[%d],pantsId[%d],shoesId[%d],glassesId[%d],"
							"scarfId=[%d], wingId[%d], hat[%d] , skinColor r[%f] , skinColor g[%f] , skinColor b[%f] , skinColor a[%f]",
							targetPlayer->getShowName().c_str(), selfPlayer->getShowName().c_str(),
							selfAttrInfo.faceId, selfAttrInfo.hairId, selfAttrInfo.topsId,
							selfAttrInfo.pantsId, selfAttrInfo.shoesId, selfAttrInfo.glassesId,
							selfAttrInfo.scarfId, selfAttrInfo.wingId, selfAttrInfo.hatId, selfAttrInfo.skinColor.r, selfAttrInfo.skinColor.g, selfAttrInfo.skinColor.b, selfAttrInfo.skinColor.a);
							*/
					}
					else {
						LordLogError("spawnSelfEntityAtClient but can not get user attr, self platform id " U64FMT
							", target player name %s, be sent player name %s",
							selfClientPeer->getPlatformUserId(), targetPlayer->getShowName().c_str(), selfPlayer->getShowName().c_str());
					}
				}
			}
		}
		else {
			LordLogError("spawnSelfEntityAtClient but can not find self client peer,self session id " U64FMT
				", target player name %s, be sent player name %s",
				selfPlayer->getRaknetID(), targetPlayer->getShowName().c_str(), selfPlayer->getShowName().c_str());
		}

		sender->sendChangePlayerMaxHealth(targetPlayer->getRaknetID(), selfPlayer->hashCode(), selfPlayer->getMaxHealth());
		sender->sendEntityAttribute(targetPlayer, selfPlayer);
		sender->sendChangeItemInHand(targetPlayer, selfPlayer->entityId, selfPlayer->inventory->getCurrentItem());
		sender->sendSetFlying(targetPlayer->getRaknetID(), selfPlayer);
		if (selfPlayer->getTeamId() != 0)
		{
			sender->sendChangePlayerTeam(targetPlayer->getRaknetID(), selfPlayer->getPlatformUserId(), selfPlayer->getTeamId());
		}
		if (selfPlayer->m_isPeopleActor == false)
		{
			sender->sendPlayerChangeActor(targetPlayer->getRaknetID(), selfPlayer);
		}
		auto armorInventory = selfPlayer->inventory->getArmorInventory();
		vector<ItemStackPtr>::type armors;
		for (int i = 0; i < armorInventory->getSizeInventory(); ++i)
		{
			if (armorInventory->getStackInSlot(i))
			{
				armors.push_back(armorInventory->getStackInSlot(i));
			}
		}
		sender->sendEquipArmors(targetPlayer, selfPlayer->entityId, armors);

		auto activePotionMap = selfPlayer->getActivePotionEffects();
		for (auto it : activePotionMap) {
			sender->sendAddEntityEffect(targetPlayer, selfPlayer->entityId, it.second);
		}

		if (selfPlayer->isSneaking()) {
			sender->sendPlayerAction(targetPlayer->getRaknetID(), selfPlayer->entityId, NETWORK_DEFINE::PacketPlayerActionType::START_SNEAK);
		}

		if (selfPlayer->isOnVehicle() && selfPlayer->getVehicle() != NULL)
		{
			sender->syncPlayerTakeOnVehicle(targetPlayer->getRaknetID(), selfPlayer->entityId, selfPlayer->isDriver(), selfPlayer->getVehicle()->entityId);
		}

		if (selfPlayer->isOnAircraft() && selfPlayer->getAircraft() != NULL)
		{
			sender->syncPlayerTakeAircraft(targetPlayer->getRaknetID(), selfPlayer->entityId, selfPlayer->getAircraft()->entityId);
		}

		if (selfPlayer->m_isCameraLocked)
		{
			sender->SyncCameraLockStatus(targetPlayer->getRaknetID(), selfPlayer->entityId, selfPlayer->m_isCameraLocked);
		}

		sender->syncEntityPlayerAnimate(targetPlayer->getRaknetID(), NETWORK_DEFINE::PacketAnimateType::ANIMATE_TYPE_BASE_ACTION, selfPlayer->entityId, selfPlayer->getBaseAction());
		sender->syncEntityPlayerAnimate(targetPlayer->getRaknetID(), NETWORK_DEFINE::PacketAnimateType::ANIMATE_TYPE_UPPER_ACTION, selfPlayer->entityId, selfPlayer->getUpperAction());
		sender->syncEntityPlayerMovementInput(targetPlayer->getRaknetID(), selfPlayer->entityId);
		sender->sendEntityPlayerOccupation(targetPlayer->getRaknetID(), selfPlayer->entityId, selfPlayer->getOccupation());
		sender->syncPlayerCustomEffects(targetPlayer->getRaknetID(), selfPlayer);
		sender->syncPlayerBagInfo(targetPlayer->getRaknetID(), selfPlayer);

		if (selfPlayer->isFrozen())
			sender->syncEntityFrozenTime(targetPlayer->getRaknetID(), selfPlayer->entityId, selfPlayer->getFrozenTime());
		if (selfPlayer->isHurted())
			sender->syncEntityHurtTime(targetPlayer->getRaknetID(), selfPlayer->entityId, selfPlayer->getHurtTime());
		if (selfPlayer->isColorful())
			sender->syncEntityColorfulTime(targetPlayer->getRaknetID(), selfPlayer->entityId, selfPlayer->getColorfulTime());
		if (selfPlayer->isDisableMove())
			sender->syncEntityDisableMoveTime(targetPlayer->getRaknetID(), selfPlayer->entityId, selfPlayer->getDisableMoveTime());

		SCRIPT_EVENT::PlayerReviewPlayerEvent::invoke(selfPlayer->getRaknetID(), targetPlayer->getRaknetID());
	}

	void EntityTrackerEntry::syncEntityMerchantInfo(const std::shared_ptr<ServerPacketSender>& sender, EntityPlayerMP* targetPlayer)
	{
		auto entityMerchant = dynamic_cast<EntityMerchant*>(myEntity);
		CommodityMapping mapping = CommodityManager::Instance()->getMapping(entityMerchant->entityId, targetPlayer->getRaknetID());
		sender->sendUpdateMerchant(mapping.rakssId, mapping.entityId, mapping.index);
	}

	void EntityTrackerEntry::syncEntityVehicileInfo(const std::shared_ptr<ServerPacketSender>& sender, EntityPlayerMP * targetPlayer)
	{
		auto entityVehicle = dynamic_cast<EntityVehicle*>(myEntity);
		sender->sendSyncVehicleState(targetPlayer->getRaknetID(), entityVehicle->entityId);
		//LordLogInfo("S2CEntitySpawnCommonPacket ENTITY_CLASS_VEHICLE EntityId %d CarId %d to %lld", entityVehicle->entityId, entityVehicle->m_vehicleID, targetPlayer->getRaknetID());
	}

	void EntityTrackerEntry::syncEntityAircraftInfo(const std::shared_ptr<ServerPacketSender>& sender, EntityPlayerMP * targetPlayer)
	{
		auto entityAircraft = dynamic_cast<EntityAircraft*>(myEntity);
		sender->syncAircraftState(targetPlayer->getRaknetID(), entityAircraft->entityId);
	}

	void EntityTrackerEntry::syncEntityRankNpcInfo(const std::shared_ptr<ServerPacketSender>& sender, EntityPlayerMP * targetPlayer)
	{
		auto entityRankNpc = dynamic_cast<EntityRankNpc*>(myEntity);
		sender->sendUpdateRankNpc(targetPlayer->getRaknetID(), entityRankNpc->entityId);
	}

	void EntityTrackerEntry::syncEntityActorNpcInfo(const std::shared_ptr<ServerPacketSender>& sender, EntityPlayerMP * targetPlayer)
	{
		auto entityActorNpc = dynamic_cast<EntityActorNpc*>(myEntity);
		sender->sendNpcActorInfo(targetPlayer->getRaknetID(), entityActorNpc);
		SCRIPT_EVENT::PlayerReviewActorNpcEvent::invoke(targetPlayer->getRaknetID(), entityActorNpc->hashCode());
	}
	
	void EntityTrackerEntry::syncEntitySessionNpcInfo(const std::shared_ptr<ServerPacketSender>& sender, EntityPlayerMP * targetPlayer)
	{
		auto entitySessionNpc = dynamic_cast<EntitySessionNpc*>(myEntity);
		sender->syncSessionNpc(targetPlayer->getRaknetID(), entitySessionNpc);
	}
	

	void EntityTrackerEntry::syncEntityCreatureInfo(const std::shared_ptr<ServerPacketSender>& sender, EntityPlayerMP* targetPlayer)
	{
		auto entityCreature = dynamic_cast<EntityCreature*>(myEntity);
		if (entityCreature)
		{
			sender->sendUpdateEntityCreature(targetPlayer->getRaknetID(), entityCreature);
			sender->sendCreatureEntityAction(entityCreature->entityId, (int)entityCreature->getCreatureAction(), (int)entityCreature->getCreatureAction(), (int)entityCreature->getPreCreatureAction());
			sender->syncEntityHealth(targetPlayer->getRaknetID(), entityCreature->entityId, entityCreature->getHealth(), entityCreature->getMaxHealth());
		}
	}

	void EntityTrackerEntry::syncEntityBlockmanInfo(const std::shared_ptr<ServerPacketSender>& sender, EntityPlayerMP * targetPlayer)
	{
		auto entity = dynamic_cast<EntityBlockman*>(myEntity);
		sender->syncEntityBlockman(targetPlayer->getRaknetID(), entity);
	}

	void EntityTrackerEntry::syncEntityBlockmanEmptyInfo(const std::shared_ptr<ServerPacketSender>& sender, EntityPlayerMP * targetPlayer)
	{
		syncEntityBlockmanInfo(sender, targetPlayer);
		auto entity = dynamic_cast<EntityBlockmanEmpty*>(myEntity);
		sender->syncBlockmanMoveAttr(targetPlayer->getRaknetID(), entity);
		sender->syncEntityFrozenTime(targetPlayer->getRaknetID(), entity->entityId, entity->getFrozenTime());
	}

	void EntityTrackerEntry::syncEntityActorCannonInfo(const std::shared_ptr<ServerPacketSender>& sender, EntityPlayerMP * targetPlayer)
	{
		auto entityActorCannon = dynamic_cast<EntityActorCannon*>(myEntity);
		sender->sendCannonActorInfo(targetPlayer->getRaknetID(), entityActorCannon);
	}
	
	void EntityTrackerEntry::syncEntityBuildNpcInfo(const std::shared_ptr<ServerPacketSender>& sender, EntityPlayerMP * targetPlayer)
	{
		auto entity = dynamic_cast<EntityBuildNpc*>(myEntity);
		sender->syncEntityBuildNpc(targetPlayer->getRaknetID(), entity);
	}

	void EntityTrackerEntry::syncEntityLandNpcInfo(const std::shared_ptr<ServerPacketSender>& sender, EntityPlayerMP * targetPlayer)
	{
		auto entity = dynamic_cast<EntityLandNpc*>(myEntity);
		sender->syncEntityLandNpc(targetPlayer->getRaknetID(), entity);
	}

	void EntityTrackerEntry::syncEntityBulletinInfo(const std::shared_ptr<ServerPacketSender>& sender, EntityPlayerMP * targetPlayer)
	{
		auto entity = dynamic_cast<EntityBulletin*>(myEntity);
		sender->syncEntityBulletin(targetPlayer->getRaknetID(), entity);
	}

	void EntityTrackerEntry::syncEntityBirdInfo(const std::shared_ptr<ServerPacketSender>& sender, EntityPlayerMP * targetPlayer)
	{
		auto entity = dynamic_cast<EntityBirdAI*>(myEntity);
		sender->syncEntityBird(targetPlayer->getRaknetID(), entity);
		sender->sendEntityAction(targetPlayer->getRaknetID(), entity->entityId, (int)entity->getClassID(), (int)entity->getBirdAction(), (int)entity->getBirdAction(), (int)entity->getPreBirdAction());
	}
}
