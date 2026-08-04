#include "S2CEntityPacketHandles.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "cEntity/EntityOtherPlayerMP.h"
#include "cWorld/Blockman.h"
#include "game.h"
#include "Item/ItemStack.h"
#include "Entity/Entity.h"
#include "Entity/EntityPlayer.h"
#include "Entity/DamageSource.h"
#include "Entity/EntityItem.h"
#include "Entity/EntityArrow.h"
#include "Entity/EntityBullet.h"
#include "Entity/EntityPotion.h"
#include "World/World.h"
#include "Inventory/InventoryPlayer.h"
#include "Inventory/IInventory.h"
#include "Inventory/Container.h"
#include "Util/ClientEvents.h"
#include "Item/Potion.h"
#include "Item/Items.h"
#include "Block/BlockManager.h"
#include "Block/Blocks.h"
#include "cWorld/BlockChangeRecorderClient.h"
#include "ShellInterface.h"
#include "Object/Root.h"
#include "Entity/Enchantment.h"
#include "Entity/EntitySnowball.h"
#include "Entity/EntityTNTPrimed.h"
#include "Entity/EntityGrenade.h"
#include "Entity/EntityTNTThrowable.h"
#include "Entity/Explosion.h"
#include "Entity/EntityVehicle.h"
#include "Entity/EntityRankNpc.h"
#include "TileEntity/TileEntitys.h"
#include "cWorld/SignTextsChangeRecorder.h"
#include "Entity/EntityMerchant.h"
#include "BattleSummary.h"
#include "Blockman/GUI/RootGuiLayout.h"
#include "Util/LanguageKey.h"
#include "Util/LanguageManager.h"
#include "Util/ClientDataReport.h"
#include "cItem/cItem.h"
#include "Item/ItemStack.h"
#include "Setting/GunSetting.h"
#include "Setting/UIDisplaySetting.h"
#include "Entity/EntityAircraft.h"
#include "Entity/EntityActorNpc.h"
#include "Blockman/EntityRenders/EntityRenderManager.h"
#include "Entity/EntitySessionNpc.h"
#include "Entity/EntityCreature.h"
#include "Entity/EntityCreatureBullet.h"
#include "Entity/EntitySkillThrowable.h"
#include "Entity/EntityFireball.h"
#include "Entity/EntityBlockmanEmpty.h"
#include "Entity/EntityBlockman.h"
#include "cEntity/EntityActorCannonClient.h"
#include "cEntity/PlayerControlller.h"
#include "cEntity/EntityFishHookClient.h"
#include "Entity/EntityBuildNpc.h"
#include "Entity/EntityLandNpc.h"
#include "Entity/EntityBulletin.h"
#include "Entity/EntityBird.h"
#include "Render/RenderGlobal.h"

using namespace BLOCKMAN;


void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketAddEntityEffect>& packet) 
{

	LordLogInfo("_handlePacket_S2CPacketAddEntityEffect_, packet->entityId=%d, packet->potionId=%d",
		(int)packet->entityId, (int)packet->potionId);

	auto dataCache = ClientNetwork::Instance()->getDataCache();
	EntityPlayer* pPlayer = dataCache->getPlayerByServerId(packet->entityId);
	if (!pPlayer) {
		LordLogError("recv S2CPacketAddEntityEffect but can not find player, player remote id=%d\n", packet->entityId);
		return;
	}

	PotionEffect* effect = LordNew PotionEffect(packet->potionId, packet->duration, packet->amplifier, packet->isAmbient);
	effect->setSplashPotion(packet->isSplashPotion);
	effect->setPotionDurationMax(packet->isPotionDurationMax);
	pPlayer->addPotionEffect(effect);

	if (packet->potionId == Potion::invisibility->getId()) {
		if (dataCache->getSelfServerId() != packet->entityId) {
			pPlayer->setInvisible(true);
		}
	}
}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketRemoveEntityEffect>& packet)
{

	LordLogInfo("_handlePacket_S2CPacketRemoveEntityEffect_, packet->entityId=%d", (int)packet->entityId);

	auto dataCache = ClientNetwork::Instance()->getDataCache();
	EntityPlayer* pPlayer = dataCache->getPlayerByServerId(packet->entityId);
	if (!pPlayer) {
		LordLogError("recv S2CPacketRemoveEntityEffect but can not find player, player remote id=%d\n", packet->entityId);
		return;
	}
	pPlayer->removePotionEffect(packet->potionId);

	if (packet->potionId == Potion::invisibility->getId()) {
		if (dataCache->getSelfServerId() != packet->entityId) {
			pPlayer->setInvisible(false);
		}
	}
}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketRemoveEntity>&packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	if (packet->m_entityID == dataCache->getSelfServerId())
	{
		LordLogError("It is impossible to receive a S2CPacketRemoveEntity to delete self Player.");
		return;
	}

	// if the entity is snowBall just remove the relesionship of the 
	int clientID = dataCache->getClientId(packet->m_entityID);
	dataCache->removeEntityByServerId(packet->m_entityID);
	dataCache->removeClientServerIdPair(clientID);
}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketEntityMovement>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();

	Entity* pEntity = dataCache->getEntityByServerId(packet->m_entityId);
	if (!pEntity)
	{
		return;
	}

	auto pBlockman = dataCache->getBlockman();
	if (pEntity->isClass(ENTITY_CLASS_VEHICLE) && pBlockman->m_pPlayer->getVehicle() == pEntity && pBlockman->m_pPlayer->isDriver())
		return;

	if (pEntity)
	{
		int dx = packet->m_x;
		int dy = packet->m_y;
		int dz = packet->m_z;
		pEntity->serverPos.x += dx;
		pEntity->serverPos.y += dy;
		pEntity->serverPos.z += dz;
		float x = pEntity->serverPos.x / 32.f;
		float y = pEntity->serverPos.y / 32.f;
		float z = pEntity->serverPos.z / 32.f;
		float yaw = packet->m_rotating ? (packet->m_yaw) : pEntity->rotationYaw;
		float pitch = packet->m_rotating ? (packet->m_pitch) : pEntity->rotationPitch;

		/*
		if (packet->m_moving || (dx != 0 || dy != 0 || dz != 0)) {
		if (dynamic_cast<EntityPlayer*>(pEntity)) {
		LordLogInfo("recv S2CPacketEntityMovement, Entity id[%d] moving dp(%d, %d, %d) pos(%f, %f, %f)", packet->m_entityId, dx, dy, dz, x, y, z);
		}
		}
		if (packet->m_rotating) {
		if (dynamic_cast<EntityPlayer*>(pEntity)) {
		LordLogInfo("recv S2CPacketEntityMovement,Entity id[%d] Rotation yaw_pitch(%f, %f)", packet->m_entityId, yaw, pitch);
		}
		}*/

		int increment = 3;
		if (pEntity->isClass(ENTITY_CLASS_CREATURE)
			|| pEntity->isClass(ENTITY_CLASS_BIRD))
			increment = 12;

		pEntity->setPositionAndRotation2(Vector3(x, y, z), yaw, pitch, increment);
	}
}


void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketEntityTeleport>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();

	Entity* pEntity = dataCache->getEntityByServerId(packet->m_entityId);
	if (!pEntity) {
		/************************************************************************/
		// when player died, all entity and clientId serverId map cleared
		/************************************************************************/
		return;
	}

	pEntity->serverPos.x = packet->m_x;
	pEntity->serverPos.y = packet->m_y;
	pEntity->serverPos.z = packet->m_z;
	float x = pEntity->serverPos.x / 32.f;
	float z = pEntity->serverPos.z / 32.f;
	float yaw = packet->m_yaw;
	float pitch = packet->m_pitch;
	if (dynamic_cast<EntityPlayerSPMP*>(pEntity))
	{
		float y = pEntity->serverPos.y / 32.f + pEntity->yOffset;
		//LordLogInfo("PacketEntityTeleport id[%d] pos(%f, %f, %f)", packet->m_entityId, x, y, z);	
		auto player = dynamic_cast<EntityPlayerSPMP*>(pEntity);
		player->setPositionAndRotation({ x, y, z }, yaw, pitch);
		player->setVelocity(Vector3(0, 0, 0));
		player->recordOldPosSPMP();
	}
	else
	{
		auto pBlockman = dataCache->getBlockman();
		if (pEntity->isClass(ENTITY_CLASS_VEHICLE) && pBlockman->m_pPlayer->getVehicle() == pEntity && pBlockman->m_pPlayer->isDriver())
			return;

		float y = pEntity->serverPos.y / 32.f + 0.015625f;
		//LordLogInfo("PacketEntityTeleport id[%d] pos(%f, %f, %f)", packet->m_entityId, x, y, z);
		pEntity->setPositionAndRotation2(Vector3(x, y, z), yaw, pitch, 1);
		pEntity->setVelocity(Vector3(0, 0, 0));
	}
}


void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketEntityVelocity>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();

	Entity* pEntity = dataCache->getEntityByServerId(packet->m_entityID);

	if (pEntity)
	{
		float velx = packet->m_motionX / 8000.f;
		float vely = packet->m_motionY / 8000.f;
		float velz = packet->m_motionZ / 8000.f;
		/*if (dynamic_cast<EntityPlayer*>(pEntity)) {
		LordLogInfo("PacketEntityVelocity id[%d] Velocity(%f, %f, %f)", packet->m_entityID, velx, vely, velz);
		}*/
		pEntity->setVelocity(Vector3(velx, vely, velz));
	}
}


void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketSpawnExpOrb>& packet)
{
	// nothing todo. 
	// this protocol is client to server.
}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketSpawnMob>& packet)
{
	// nothing todo. 
	// this protocol is client to server.
	LordLogInfo("handlePacket>>S2CPacketSpawnMob");
}


void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketSpawnPlayer>& packet)
{
	auto pWorld = BLOCKMAN::Blockman::Instance()->getWorld();

	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto pBlockman = dataCache->getBlockman();
	auto pWorldClient = dataCache->getWorldClient();

	EntityOtherPlayerMP* pPlayer = dynamic_cast<EntityOtherPlayerMP*>(dataCache->getPlayerByServerId(packet->m_entityID));
	if (pPlayer)
	{
		int clientID = dataCache->getClientId(packet->m_entityID);
		dataCache->removeEntityByServerId(packet->m_entityID);
		dataCache->removeClientServerIdPair(clientID);
		pPlayer = nullptr;
	}

	float x = packet->m_posX / 32.f;
	float y = packet->m_posY / 32.f;
	float z = packet->m_posZ / 32.f;
	// float yaw = packet->m_yaw * 360.f / 256.0F;
	// float pitch = packet->m_pitch * 360.f / 256.0F;
	float yaw = packet->m_yaw;
	float pitch = packet->m_pitch;
	pPlayer = LordNew EntityOtherPlayerMP(pBlockman->m_pWorld, packet->m_name);
	pPlayer->prevPos.x = pPlayer->lastTickPos.x = float(pPlayer->serverPos.x = packet->m_posX);
	pPlayer->prevPos.y = pPlayer->lastTickPos.y = float(pPlayer->serverPos.y = packet->m_posY);
	pPlayer->prevPos.z = pPlayer->lastTickPos.z = float(pPlayer->serverPos.z = packet->m_posZ);

	pPlayer->setPositionAndRotation(Vector3(x, y, z), yaw, pitch);
	pPlayer->setRotationYawHead(yaw);
	pPlayer->setInvisible(packet->m_invisible);
	pPlayer->setShowName(packet->m_showName);
	pPlayer->m_sex = packet->m_sex;
	pPlayer->m_outLooksChanged = true;
	pPlayer->setTeamId(packet->m_teamId);
	pPlayer->setTeamName(packet->m_teamName);
	pPlayer->setPlatformUserId(packet->m_platformUserId);
	pPlayer->m_defaultIdle = packet->m_defaultIdle;
	pPlayer->spYaw = packet->m_isSpYaw;
	pPlayer->spYawRadian = packet->m_SpYawRadian;
	LordLogInfo("spawnPlayer get name %s getshowName %s", packet->m_name.c_str(), packet->m_showName.c_str());

	// add entity to world
	pWorldClient->spawnEntityInWorld(pPlayer);
	dataCache->addClientServerIdPair(pPlayer->entityId, packet->m_entityID);
	LordLogInfo("recv S2CPacketSpawnPlayer server id[%d] sex[%d] name[%s] pos(%f, %f, %f)",
		packet->m_entityID, packet->m_sex, packet->m_name.c_str(), x, y, z);
}

void S2CEntityPacketHandles::createEntityItem(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet)
{
	int itemId = packet->m_itemID;
	int count = packet->m_count;

	int throwerId = packet->m_throwerID;

	auto world = BLOCKMAN::Blockman::Instance()->getWorld();
	EntityItem* entityItem = LordNew EntityItem(world, packet->m_pos, LORD::make_shared<ItemStack>(itemId, count, packet->m_itemDamage));
	if (packet->m_enchanted)
	{
		// add any enchantment to turn on the rendering of enchantment effect on the item model
		entityItem->getEntityItem()->addEnchantment(Enchantment::protection, 1);
	}
	entityItem->delayBeforeCanPickup = 40;
	entityItem->motion = packet->m_motion;
	entityItem->rotationPitch = packet->m_pitch;
	entityItem->rotationYaw = packet->m_yaw;
	entityItem->serverPos.x = int(Math::Floor(packet->m_pos[0] * 32.f));
	entityItem->serverPos.y = int(Math::Floor(packet->m_pos[1] * 32.f));
	entityItem->serverPos.z = int(Math::Floor(packet->m_pos[2] * 32.f));
	entityItem->forceSpawn = true;
	world->spawnEntityInWorld(entityItem);

	auto dataCache = ClientNetwork::Instance()->getDataCache();
	dataCache->addClientServerIdPair(entityItem->entityId, packet->m_entityID);
	/*LordLogInfo("S2CEntitySpawnCommonPacket(EntityItem): clientId[%d],serverId[%d] x[%f] y[%f] z[%f] motion.x[%f] motion.y[%f] motion.z[%f]",
	entityItem->entityId, packet->m_entityID, entityItem->position.x, entityItem->position.y, entityItem->position.z,
	entityItem->motion.x, entityItem->motion.y, entityItem->motion.z);*/
}

void S2CEntityPacketHandles::createEntityVehicle(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet)
{
	auto world = BLOCKMAN::Blockman::Instance()->getWorld();
	auto dataCache = ClientNetwork::Instance()->getDataCache();

	int carID = packet->m_throwerID; //use throwerID as carID

	EntityVehicle* pVehicle = LordNew EntityVehicle(world, carID, packet->m_pos, packet->m_yaw);
	world->spawnEntityInWorld(pVehicle);

	dataCache->addClientServerIdPair(pVehicle->entityId, packet->m_entityID);
	//LordLogInfo("createEntityVehicle carId %d, clientId %d, serverId %d", carID, pVehicle->entityId, packet->m_entityID);
}

void S2CEntityPacketHandles::createEntityBullet(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet)
{
	auto world = BLOCKMAN::Blockman::Instance()->getWorld();
	auto dataCache = ClientNetwork::Instance()->getDataCache();

	EntityPlayer* shooter = dataCache->getPlayerByServerId(packet->m_throwerID);
	if (!shooter) {
		LordLogError("S2CEntitySpawnCommonPacket(EntityBullet) can not find shooter, throwerID[%d]", packet->m_throwerID);
		return;
	}

	Vector3 originalPos = shooter->position;
	originalPos.x += Math::Cos(packet->m_yaw * Math::DEG2RAD) * 0.16F;
	originalPos.y += 0.1f;
	originalPos.z += Math::Sin(packet->m_yaw * Math::DEG2RAD) * 0.16F;

	EntityBullet* pBullet = LordNew EntityBullet(world);
	pBullet->setLocationAndAngles(originalPos, packet->m_yaw, packet->m_pitch);
	pBullet->setPosition(packet->m_pos);
	pBullet->yOffset = 0.0f;
	pBullet->setVelocity(packet->m_motion);
	pBullet->m_shootingEntityId = shooter->entityId;

	world->spawnEntityInWorld(pBullet);

	dataCache->addClientServerIdPair(pBullet->entityId, packet->m_entityID);
	/*LordLogInfo("S2CEntitySpawnCommonPacket(EntityArrow): clientId[%d],serverId[%d] x[%f] y[%f] z[%f] motion.x[%f] motion.y[%f] motion.z[%f] yaw[%f] pitch[%f]",
	entityArrow->entityId, packet->m_entityID, entityArrow->position.x, entityArrow->position.y,entityArrow->position.z,
	entityArrow->motion.x, entityArrow->motion.y, entityArrow->motion.z, entityArrow->rotationYaw, entityArrow->rotationPitch);*/
}

void S2CEntityPacketHandles::createEntityArrow(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet)
{
	auto world = BLOCKMAN::Blockman::Instance()->getWorld();
	auto dataCache = ClientNetwork::Instance()->getDataCache();

	EntityPlayer* shooter = dataCache->getPlayerByServerId(packet->m_throwerID);
	if (!shooter) {
		LordLogError("S2CEntitySpawnCommonPacket(EntityArrow) can not find shooter, throwerID[%d]", packet->m_throwerID);
		return;
	}

	Vector3 originalPos = shooter->position;
	originalPos.x += Math::Cos(packet->m_yaw * Math::DEG2RAD) * 0.16F;
	originalPos.y += 0.1f;
	originalPos.z += Math::Sin(packet->m_yaw * Math::DEG2RAD) * 0.16F;

	EntityArrow* entityArrow = LordNew EntityArrow(world);
	entityArrow->setLocationAndAngles(originalPos, packet->m_yaw, packet->m_pitch);
	entityArrow->setPosition(packet->m_pos);
	entityArrow->yOffset = 0.0f;
	entityArrow->setVelocity(packet->m_motion);
	entityArrow->m_shootingEntityId = shooter->entityId;
	entityArrow->setOnFire(packet->m_onFire);
	if (packet->potionId != 0)
	{
		entityArrow->m_potionEffect = LordNew PotionEffect(packet->potionId, packet->duration, packet->amplifier, packet->amplifier);
	}

	world->spawnEntityInWorld(entityArrow);

	dataCache->addClientServerIdPair(entityArrow->entityId, packet->m_entityID);
	/*LordLogInfo("S2CEntitySpawnCommonPacket(EntityArrow): clientId[%d],serverId[%d] x[%f] y[%f] z[%f] motion.x[%f] motion.y[%f] motion.z[%f] yaw[%f] pitch[%f]",
	entityArrow->entityId, packet->m_entityID, entityArrow->position.x, entityArrow->position.y,entityArrow->position.z,
	entityArrow->motion.x, entityArrow->motion.y, entityArrow->motion.z, entityArrow->rotationYaw, entityArrow->rotationPitch);*/
}


void S2CEntityPacketHandles::createEntitySnowball(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet)
{
	auto world = BLOCKMAN::Blockman::Instance()->getWorld();
	auto dataCache = ClientNetwork::Instance()->getDataCache();

	EntityPlayer* shooter = dataCache->getPlayerByServerId(packet->m_throwerID);
	if (!shooter) {
		LordLogError("S2CEntitySpawnCommonPacket(EntitySnowball) can not find shooter, throwerID[%d]", packet->m_throwerID);
		return;
	}

	Vector3 originalPos = shooter->position;
	originalPos.x += Math::Cos(packet->m_yaw * Math::DEG2RAD) * 0.16F;
	originalPos.y += 0.1f;
	originalPos.z += Math::Sin(packet->m_yaw * Math::DEG2RAD) * 0.16F;

	EntitySnowball* entitySnowball = LordNew EntitySnowball(world, shooter);
	entitySnowball->setLocationAndAngles(originalPos, packet->m_yaw, packet->m_pitch);
	entitySnowball->setPosition(packet->m_pos);
	entitySnowball->setHeadingFromThrower(shooter, shooter->rotationPitch, shooter->rotationYaw, 0.0F, 1.5F, 1.0F);
	entitySnowball->setVelocity(packet->m_motion);
	entitySnowball->setMeta(packet->m_itemDamage);
	world->spawnEntityInWorld(entitySnowball);

	dataCache->addClientServerIdPair(entitySnowball->entityId, packet->m_entityID);
}

void S2CEntityPacketHandles::createEntityTNTThrowable(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet)
{
	auto world = BLOCKMAN::Blockman::Instance()->getWorld();
	auto dataCache = ClientNetwork::Instance()->getDataCache();

	EntityPlayer* shooter = dataCache->getPlayerByServerId(packet->m_throwerID);
	if (!shooter) {
		LordLogError("S2CEntitySpawnCommonPacket(EntityTNTThrowable) can not find shooter, throwerID[%d]", packet->m_throwerID);
		return;
	}

	Vector3 originalPos = shooter->position;
	originalPos.x += Math::Cos(packet->m_yaw * Math::DEG2RAD) * 0.16F;
	originalPos.y += 0.1f;
	originalPos.z += Math::Sin(packet->m_yaw * Math::DEG2RAD) * 0.16F;

	EntityTNTThrowable* entityTNTThrowable = LordNew EntityTNTThrowable(world, shooter);
	entityTNTThrowable->setLocationAndAngles(originalPos, packet->m_yaw, packet->m_pitch);
	entityTNTThrowable->setHeadingFromThrower(shooter, shooter->rotationPitch, shooter->rotationYaw, 0.0F, 1.5F, 1.0F);
	entityTNTThrowable->setVelocity(packet->m_motion);
	world->spawnEntityInWorld(entityTNTThrowable);

	dataCache->addClientServerIdPair(entityTNTThrowable->entityId, packet->m_entityID);
}

void S2CEntityPacketHandles::createEntityGrenade(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet)
{
	auto world = BLOCKMAN::Blockman::Instance()->getWorld();
	auto dataCache = ClientNetwork::Instance()->getDataCache();

	EntityPlayer* shooter = dataCache->getPlayerByServerId(packet->m_throwerID);
	if (!shooter) {
		LordLogError("S2CEntitySpawnCommonPacket(EntityGrenade) can not find shooter, throwerID[%d]", packet->m_throwerID);
		return;
	}

	Vector3 originalPos = shooter->position;
	originalPos.x += Math::Cos(packet->m_yaw * Math::DEG2RAD) * 0.16F;
	originalPos.y += 0.1f;
	originalPos.z += Math::Sin(packet->m_yaw * Math::DEG2RAD) * 0.16F;

	EntityGrenade* entityGrenade = LordNew EntityGrenade(world, shooter, packet->m_itemID);
	entityGrenade->setLocationAndAngles(originalPos, packet->m_yaw, packet->m_pitch);
	entityGrenade->setHeadingFromThrower(shooter, shooter->rotationPitch, shooter->rotationYaw, 0.0F, 1.5F, 1.0F);
	entityGrenade->setVelocity(packet->m_motion);
	world->spawnEntityInWorld(entityGrenade);

	dataCache->addClientServerIdPair(entityGrenade->entityId, packet->m_entityID);
}

void S2CEntityPacketHandles::createEntityMerchant(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet)
{
	auto world = BLOCKMAN::Blockman::Instance()->getWorld();
	auto entityMerchant = LordNew EntityMerchant(world);

	float x = packet->m_pos[0] / 32.f;
	float y = packet->m_pos[1] / 32.f;
	float z = packet->m_pos[2] / 32.f;
	// float yaw = packet->m_yaw * 360.f / 256.0F;
	// float pitch = packet->m_pitch * 360.f / 256.0F;
	float yaw = packet->m_yaw;
	float pitch = packet->m_pitch;
	entityMerchant->prevPos = entityMerchant->lastTickPos = packet->m_pos;
	entityMerchant->serverPos.x = static_cast<int>(Math::Floor(packet->m_pos[0] * 32));
	entityMerchant->serverPos.y = static_cast<int>(Math::Floor(packet->m_pos[1] * 32));
	entityMerchant->serverPos.z = static_cast<int>(Math::Floor(packet->m_pos[2] * 32));
	entityMerchant->setPositionAndRotation(packet->m_pos, packet->m_yaw, packet->m_pitch);
	// TODO: add display name
	entityMerchant->forceSpawn = true;
	world->spawnEntityInWorld(entityMerchant);
	ClientNetwork::Instance()->getDataCache()->addClientServerIdPair(entityMerchant->entityId, packet->m_entityID);
}

void S2CEntityPacketHandles::createEntityRank(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet)
{
	auto world = BLOCKMAN::Blockman::Instance()->getWorld();
	auto entityRank = LordNew EntityRankNpc(world);

	float x = packet->m_pos[0] / 32.f;
	float y = packet->m_pos[1] / 32.f;
	float z = packet->m_pos[2] / 32.f;
	// float yaw = packet->m_yaw * 360.f / 256.0F;
	// float pitch = packet->m_pitch * 360.f / 256.0F;
	float yaw = packet->m_yaw;
	float pitch = packet->m_pitch;
	entityRank->prevPos = entityRank->lastTickPos = packet->m_pos;
	entityRank->serverPos.x = static_cast<int>(Math::Floor(packet->m_pos[0] * 32));
	entityRank->serverPos.y = static_cast<int>(Math::Floor(packet->m_pos[1] * 32));
	entityRank->serverPos.z = static_cast<int>(Math::Floor(packet->m_pos[2] * 32));
	entityRank->setPositionAndRotation(packet->m_pos, packet->m_yaw, packet->m_pitch);
	// TODO: add display name
	entityRank->forceSpawn = true;
	world->spawnEntityInWorld(entityRank);
	ClientNetwork::Instance()->getDataCache()->addClientServerIdPair(entityRank->entityId, packet->m_entityID);
}


void S2CEntityPacketHandles::createEntityAircraft(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet)
{
	auto world = BLOCKMAN::Blockman::Instance()->getWorld();
	auto dataCache = ClientNetwork::Instance()->getDataCache();

	int aircraftID = packet->m_throwerID;

	EntityAircraft* pAircraft = LordNew EntityAircraft(world, aircraftID, packet->m_pos, packet->m_yaw);
	world->spawnEntityInWorld(pAircraft);

	dataCache->addClientServerIdPair(pAircraft->entityId, packet->m_entityID);
	//LordLogInfo("createEntityVehicle carId %d, clientId %d, serverId %d", carID, pVehicle->entityId, packet->m_entityID);
}

void S2CEntityPacketHandles::createEntityTNTPrimed(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet)
{
	auto world = BLOCKMAN::Blockman::Instance()->getWorld();
	auto dataCache = ClientNetwork::Instance()->getDataCache();

	EntityPlayer* shooter = dataCache->getPlayerByServerId(packet->m_throwerID);
	if (!shooter) {
		LordLogError("S2CEntitySpawnCommonPacket(EntityTNTPrimed) can not find shooter, throwerID[%d]", packet->m_throwerID);
		return;
	}

	Vector3 originalPos = shooter->position;
	originalPos.x += Math::Cos(packet->m_yaw * Math::DEG2RAD) * 0.16F;
	originalPos.y += 0.1f;
	originalPos.z += Math::Sin(packet->m_yaw * Math::DEG2RAD) * 0.16F;

	EntityTNTPrimed* tntPrimed = LordNew EntityTNTPrimed(world, originalPos, shooter);
	tntPrimed->setLocationAndAngles(originalPos, packet->m_yaw, packet->m_pitch);
	tntPrimed->setVelocity(packet->m_motion);
	world->spawnEntityInWorld(tntPrimed);
	dataCache->addClientServerIdPair(tntPrimed->entityId, packet->m_entityID);
}

void S2CEntityPacketHandles::createEntityPotion(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet)
{
	auto world = BLOCKMAN::Blockman::Instance()->getWorld();
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	EntityPlayer* thrower = dataCache->getPlayerByServerId(packet->m_throwerID);
	if (!thrower) {
		LordLogError("S2CEntitySpawnCommonPacket(EntityPotion) can not find thrower, throwerID[%d]", packet->m_throwerID);
		return;
	}

	EntityPotion* entityPotion = LordNew EntityPotion(world, thrower, thrower->inventory->getCurrentItem());
	entityPotion->setPosition(packet->m_pos);
	entityPotion->setVelocity(packet->m_motion);
	world->spawnEntityInWorld(entityPotion);

	dataCache->addClientServerIdPair(entityPotion->entityId, packet->m_entityID);
	/*LordLogInfo("S2CEntitySpawnCommonPacket(EntityPotion): clientId[%d],serverId[%d] x[%f] y[%f] z[%f] motion.x[%f] motion.y[%f] motion.z[%f] yaw[%f] pitch[%f]",
	entityPotion->entityId, packet->m_entityID, entityPotion->position.x, entityPotion->position.y, entityPotion->position.z,
	entityPotion->motion.x, entityPotion->motion.y, entityPotion->motion.z, entityPotion->rotationYaw, entityPotion->rotationPitch);*/
}

void S2CEntityPacketHandles::createEntityActortNpc(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet)
{
	auto world = BLOCKMAN::Blockman::Instance()->getWorld();
	auto entityActorNpc = LordNew EntityActorNpc(world);

	float x = packet->m_pos[0] / 32.f;
	float y = packet->m_pos[1] / 32.f;
	float z = packet->m_pos[2] / 32.f;
	float yaw = packet->m_yaw * 360.f / 256.0F;
	float pitch = packet->m_pitch * 360.f / 256.0F;
	entityActorNpc->prevPos = entityActorNpc->lastTickPos = packet->m_pos;
	entityActorNpc->serverPos.x = static_cast<int>(Math::Floor(packet->m_pos[0] * 32));
	entityActorNpc->serverPos.y = static_cast<int>(Math::Floor(packet->m_pos[1] * 32));
	entityActorNpc->serverPos.z = static_cast<int>(Math::Floor(packet->m_pos[2] * 32));
	entityActorNpc->setPositionAndRotation(packet->m_pos, packet->m_yaw, packet->m_pitch);
	// TODO: add display name
	entityActorNpc->forceSpawn = true;
	world->spawnEntityInWorld(entityActorNpc);
	ClientNetwork::Instance()->getDataCache()->addClientServerIdPair(entityActorNpc->entityId, packet->m_entityID);
}

void S2CEntityPacketHandles::createEntityCreature(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet)
{
	auto world = BLOCKMAN::Blockman::Instance()->getWorld();
	auto dataCache = ClientNetwork::Instance()->getDataCache();

	int monsterID = packet->m_throwerID;

	Vector3 pos = packet->m_pos;
	EntityCreature* pCreature = LordNew EntityCreature(world, monsterID, pos, packet->m_yaw, "");
	pCreature->serverPos.x = static_cast<int>(Math::Floor(packet->m_pos[0] * 32));
	pCreature->serverPos.y = static_cast<int>(Math::Floor(packet->m_pos[1] * 32));
	pCreature->serverPos.z = static_cast<int>(Math::Floor(packet->m_pos[2] * 32));
	pCreature->prevPos = pCreature->lastTickPos = packet->m_pos;
	pCreature->setPositionAndRotation(packet->m_pos, packet->m_yaw, packet->m_pitch);
	world->spawnEntityInWorld(pCreature);

	dataCache->addClientServerIdPair(pCreature->entityId, packet->m_entityID);
}

void S2CEntityPacketHandles::createEntityBuildNpc(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet)
{
	auto world = BLOCKMAN::Blockman::Instance()->getWorld();
	auto entityBuildNpc = LordNew EntityBuildNpc(world, packet->m_pos);

	float x = packet->m_pos[0] / 32.f;
	float y = packet->m_pos[1] / 32.f;
	float z = packet->m_pos[2] / 32.f;
	float yaw = packet->m_yaw * 360.f / 256.0F;
	float pitch = packet->m_pitch * 360.f / 256.0F;
	entityBuildNpc->prevPos = entityBuildNpc->lastTickPos = packet->m_pos;
	entityBuildNpc->serverPos.x = static_cast<int>(Math::Floor(packet->m_pos[0] * 32));
	entityBuildNpc->serverPos.y = static_cast<int>(Math::Floor(packet->m_pos[1] * 32));
	entityBuildNpc->serverPos.z = static_cast<int>(Math::Floor(packet->m_pos[2] * 32));
	entityBuildNpc->setPositionAndRotation(packet->m_pos, packet->m_yaw, packet->m_pitch);
	// TODO: add display name
	entityBuildNpc->forceSpawn = true;
	world->spawnEntityInWorld(entityBuildNpc);
	ClientNetwork::Instance()->getDataCache()->addClientServerIdPair(entityBuildNpc->entityId, packet->m_entityID);
}

void S2CEntityPacketHandles::createEntityLandNpc(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet)
{
	auto world = BLOCKMAN::Blockman::Instance()->getWorld();
	auto entityLandNpc = LordNew EntityLandNpc(world, packet->m_pos);

	float x = packet->m_pos[0] / 32.f;
	float y = packet->m_pos[1] / 32.f;
	float z = packet->m_pos[2] / 32.f;
	float yaw = packet->m_yaw * 360.f / 256.0F;
	float pitch = packet->m_pitch * 360.f / 256.0F;
	entityLandNpc->prevPos = entityLandNpc->lastTickPos = packet->m_pos;
	entityLandNpc->serverPos.x = static_cast<int>(Math::Floor(packet->m_pos[0] * 32));
	entityLandNpc->serverPos.y = static_cast<int>(Math::Floor(packet->m_pos[1] * 32));
	entityLandNpc->serverPos.z = static_cast<int>(Math::Floor(packet->m_pos[2] * 32));
	entityLandNpc->setPositionAndRotation(packet->m_pos, packet->m_yaw, packet->m_pitch);
	// TODO: add display name
	entityLandNpc->forceSpawn = true;
	world->spawnEntityInWorld(entityLandNpc);
	ClientNetwork::Instance()->getDataCache()->addClientServerIdPair(entityLandNpc->entityId, packet->m_entityID);
}

void S2CEntityPacketHandles::createEntityBulletin(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet)
{
	auto world = Blockman::Instance()->getWorld();
	auto entityBulletin = LordNew EntityBulletin(world, packet->m_pos);

	float x = packet->m_pos[0] / 32.f;
	float y = packet->m_pos[1] / 32.f;
	float z = packet->m_pos[2] / 32.f;
	float yaw = packet->m_yaw * 360.f / 256.0F;
	float pitch = packet->m_pitch * 360.f / 256.0F;
	entityBulletin->prevPos = entityBulletin->lastTickPos = packet->m_pos;
	entityBulletin->serverPos.x = static_cast<int>(Math::Floor(packet->m_pos[0] * 32));
	entityBulletin->serverPos.y = static_cast<int>(Math::Floor(packet->m_pos[1] * 32));
	entityBulletin->serverPos.z = static_cast<int>(Math::Floor(packet->m_pos[2] * 32));
	entityBulletin->setPositionAndRotation(packet->m_pos, packet->m_yaw, packet->m_pitch);
	// TODO: add display name
	entityBulletin->forceSpawn = true;
	world->spawnEntityInWorld(entityBulletin);
	ClientNetwork::Instance()->getDataCache()->addClientServerIdPair(entityBulletin->entityId, packet->m_entityID);
}

void S2CEntityPacketHandles::createEntityBird(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet)
{
	auto world = Blockman::Instance()->getWorld();
	auto entity = LordNew EntityBird(world);

	float x = packet->m_pos[0] / 32.f;
	float y = packet->m_pos[1] / 32.f;
	float z = packet->m_pos[2] / 32.f;
	float yaw = packet->m_yaw * 360.f / 256.0F;
	float pitch = packet->m_pitch * 360.f / 256.0F;
	entity->prevPos = entity->lastTickPos = packet->m_pos;
	entity->serverPos.x = static_cast<int>(Math::Floor(packet->m_pos[0] * 32));
	entity->serverPos.y = static_cast<int>(Math::Floor(packet->m_pos[1] * 32));
	entity->serverPos.z = static_cast<int>(Math::Floor(packet->m_pos[2] * 32));
	entity->setPositionAndRotation(packet->m_pos, packet->m_yaw, packet->m_pitch);
	// TODO: add display name
	entity->forceSpawn = true;
	world->spawnEntityInWorld(entity);
	ClientNetwork::Instance()->getDataCache()->addClientServerIdPair(entity->entityId, packet->m_entityID);
}

void S2CEntityPacketHandles::updateEntityBirdAction(std::shared_ptr<S2CPacketSyncEntityAction>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto entity = dynamic_cast<EntityBird*>(dataCache->getEntityByServerId(packet->entityId));
	if (!entity)
	{
		LordLogError("entityBird does not exist, id: %d", packet->entityId);
		return;
	}
	if (entity->getBaseAction() == BIRD_AS_ATTACK)
		entity->setBaseAction((BirdActionState)packet->preBaseAction);
	entity->setBaseAction((BirdActionState)packet->actionStatus);
	entity->changeBirdAction((BirdActionState)packet->actionStatus);
}

void S2CEntityPacketHandles::createEntityFireball(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet)
{
	auto world = BLOCKMAN::Blockman::Instance()->getWorld();
	auto dataCache = ClientNetwork::Instance()->getDataCache();

	EntityPlayer* shooter = dataCache->getPlayerByServerId(packet->m_throwerID);
	if (!shooter) {
		LordLogError("S2CEntitySpawnCommonPacket(EntityFireball) can not find shooter, throwerID[%d]", packet->m_throwerID);
		return;
	}

	Vector3 originalPos = shooter->position;
	originalPos.x += Math::Cos(packet->m_yaw * Math::DEG2RAD) * 0.16F;
	originalPos.y += 0.1f;
	originalPos.z += Math::Sin(packet->m_yaw * Math::DEG2RAD) * 0.16F;

	EntityFireball* entityFireball = LordNew EntityFireball(world, shooter);
	entityFireball->setLocationAndAngles(originalPos, packet->m_yaw, packet->m_pitch);
	entityFireball->setHeadingFromThrower(shooter, shooter->rotationPitch, shooter->rotationYaw, 0.0F, 1.5F, 1.0F);
	entityFireball->setVelocity(packet->m_motion);
	world->spawnEntityInWorld(entityFireball);

	dataCache->addClientServerIdPair(entityFireball->entityId, packet->m_entityID);

}

void S2CEntityPacketHandles::createEntityBlockman(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet)
{
	auto world = BLOCKMAN::Blockman::Instance()->getWorld();
	EntityBlockmanEmpty* blockman = LordNew EntityBlockmanEmpty(world, packet->m_pos, packet->m_yaw, true);
	if (blockman)
	{
		float x = packet->m_pos[0] / 32.f;
		float y = packet->m_pos[1] / 32.f;
		float z = packet->m_pos[2] / 32.f;
		float yaw = packet->m_yaw * 360.f / 256.0F;
		float pitch = packet->m_pitch * 360.f / 256.0F;
		blockman->prevPos = blockman->lastTickPos = packet->m_pos;
		blockman->serverPos.x = static_cast<int>(Math::Floor(packet->m_pos[0] * 32));
		blockman->serverPos.y = static_cast<int>(Math::Floor(packet->m_pos[1] * 32));
		blockman->serverPos.z = static_cast<int>(Math::Floor(packet->m_pos[2] * 32));
		blockman->setPositionAndRotation(packet->m_pos, packet->m_yaw, packet->m_pitch);
		// TODO: add display name
		blockman->forceSpawn = true;
		world->spawnEntityInWorld(blockman);
		ClientNetwork::Instance()->getDataCache()->addClientServerIdPair(blockman->entityId, packet->m_entityID);
	}
}

void S2CEntityPacketHandles::createEntityFishHook(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet)
{
	auto world = BLOCKMAN::Blockman::Instance()->getWorld();
	auto dataCache = ClientNetwork::Instance()->getDataCache();

	EntityPlayer* angler = dataCache->getPlayerByServerId(packet->m_throwerID);
	if (!angler) {
		LordLogError("S2CEntitySpawnCommonPacket(EntityFishHookClient) can not find angler, anglerID[%d]", packet->m_throwerID);
		return;
	}

	Vector3 originalPos = angler->position;
	originalPos.x += Math::Cos(packet->m_yaw * Math::DEG2RAD) * 0.16F;
	originalPos.y += 0.1f;
	originalPos.z += Math::Sin(packet->m_yaw * Math::DEG2RAD) * 0.16F;

	EntityFishHookClient* entityFishHook = LordNew EntityFishHookClient(world, angler);
	entityFishHook->setLocationAndAngles(originalPos, packet->m_yaw, packet->m_pitch);
	entityFishHook->setPosition(packet->m_pos);
	entityFishHook->setHeadingFromThrower(angler, angler->rotationPitch, angler->rotationYaw, 0.0f, 2.0f, 1.0f);
	entityFishHook->setVelocity(packet->m_motion);

	angler->m_fishEntity = entityFishHook;
	world->spawnEntityInWorld(entityFishHook);

	dataCache->addClientServerIdPair(entityFishHook->entityId, packet->m_entityID);
}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet)
{
	/*LordLogInfo("recv S2CEntitySpawnCommonPacket, entityId[%d], type[%d], throwId[%d]",
	packet->m_entityID, packet->m_type, packet->m_throwerID);*/

	switch (packet->m_type)
	{
	case ENTITY_CREATE_TYPE_BOAT:
		LordLogError("recv S2CEntitySpawnCommonPacket with unknown type[%d]", packet->m_type);
		//TODO
		break;
	case ENTITY_CREATE_TYPE_ITEM:
		S2CEntityPacketHandles::createEntityItem(packet);
		break;
	case ENTITY_CREATE_TYPE_ARROE:
		S2CEntityPacketHandles::createEntityArrow(packet);
		break;
	case ENTITY_CREATE_TYPE_BULLET:
		S2CEntityPacketHandles::createEntityBullet(packet);
		break;
	case ENTITY_CREATE_TYPE_VEHICLE:
		S2CEntityPacketHandles::createEntityVehicle(packet);
		break;
	case ENTITY_CREATE_TYPE_POTION:
		S2CEntityPacketHandles::createEntityPotion(packet);
		break;
	case ENTITY_CREATE_TYPE_TNT_PRIMED:
		S2CEntityPacketHandles::createEntityTNTPrimed(packet);
		break;
	case ENTITY_CREATE_TYPE_SNOWBALL:
		S2CEntityPacketHandles::createEntitySnowball(packet);
		break;
	case ENTITY_CREATE_TYPE_GRENADE:
		S2CEntityPacketHandles::createEntityGrenade(packet);
		break;
	case ENTITY_CREATE_TYPE_TNT_THROWABLE:
		S2CEntityPacketHandles::createEntityTNTThrowable(packet);
		break;
	case ENTITY_CREATE_TYPE_MERCHANT:
		S2CEntityPacketHandles::createEntityMerchant(packet);
		break;
	case ENTITY_CREATE_TYPE_RANK_NPC:
		S2CEntityPacketHandles::createEntityRank(packet);
		break;
	case ENTITY_CREATE_TYPE_AIRCRAFT:
		S2CEntityPacketHandles::createEntityAircraft(packet);
		break;
	case ENTITY_CREATE_TYPE_ACTOR_NPC:
		S2CEntityPacketHandles::createEntityActortNpc(packet);
		break;
	case ENTITY_CREATE_TYPE_SESSION_NPC:
		S2CEntityPacketHandles::createEntitySessionNpc(packet);
		break;
	case ENTITY_CREATE_TYPE_CREATURE:
		S2CEntityPacketHandles::createEntityCreature(packet);
		break;
	case ENTITY_CREATE_TYPE_FIREBALL:
		S2CEntityPacketHandles::createEntityFireball(packet);
		break;
	case ENTITY_CREATE_TYPE_BLOCKMAN:
	case ENTITY_CREATE_TYPE_BLOCKMAN_EMPTY:
		S2CEntityPacketHandles::createEntityBlockman(packet);
		break;
	case ENTITY_CREATE_TYPE_FISHHOOK:
		S2CEntityPacketHandles::createEntityFishHook(packet);
		break;
	case ENTITY_CREATE_TYPE_BUILD_NPC:
		S2CEntityPacketHandles::createEntityBuildNpc(packet);
		break;
	case ENTITY_CREATE_TYPE_LAND_NPC:
		S2CEntityPacketHandles::createEntityLandNpc(packet);
		break; 
	case ENTITY_CREATE_TYPE_BULLETIN:
		S2CEntityPacketHandles::createEntityBulletin(packet);
		break; 
	case ENTITY_CREATE_TYPE_BIRD:
		S2CEntityPacketHandles::createEntityBird(packet);
		break;
	default:
		LordLogError("recv S2CEntitySpawnCommonPacket with unknown type[%d]", packet->m_type);
		return;
	}
}


void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketEntityHeadRotation>& packet)
{

	auto dataCache = ClientNetwork::Instance()->getDataCache();

	Entity* pEntity = dataCache->getEntityByServerId(packet->m_entityID);

	if (pEntity)
	{
		//float headYaw = packet->m_headYaw * 360.f / 256.0F;
		float headYaw = packet->m_headYaw;
		//LordLogInfo("PacketEntityHeadRotation id[%d] headYaw(%f)", packet->m_entityID, headYaw);
		pEntity->setRotationYawHead(headYaw);
	}
}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncCreateArrowTipMark>& packet)
{

	auto world = BLOCKMAN::Blockman::Instance()->getWorld();
	auto entityArrowTipMark = LordNew EntityActorNpc(world);

	float posDx = Math::Abs(packet->targetPostion[0] - packet->sourcePostion[0]);
	float posDz = Math::Abs(packet->targetPostion[2] - packet->targetPostion[2]);
	float distances = Math::Sqrt(posDx*posDx + posDz * posDz);

	float pitch = Math::ATan(Math::Abs(packet->targetPostion[1] - packet->sourcePostion[1]) / distances) * Math::RAD2DEG;
	float Yaw = Math::ATan((packet->targetPostion[0] - packet->sourcePostion[0]) / (packet->targetPostion[2] - packet->targetPostion[2])) * Math::RAD2DEG;
	Yaw = 180.0f - Yaw;


	Vector3 EntityPos = Vector3(packet->sourcePostion[0] + packet->distances, packet->sourcePostion[1], packet->sourcePostion[2]);
	entityArrowTipMark->prevPos = entityArrowTipMark->lastTickPos = EntityPos;
	entityArrowTipMark->serverPos.x = static_cast<int>(Math::Floor(EntityPos.x * 32));
	entityArrowTipMark->serverPos.y = static_cast<int>(Math::Floor(EntityPos.y * 32));
	entityArrowTipMark->serverPos.z = static_cast<int>(Math::Floor(EntityPos.z * 32));
	entityArrowTipMark->setPositionAndRotation(EntityPos, Yaw, pitch);

	entityArrowTipMark->forceSpawn = true;
	world->spawnEntityInWorld(entityArrowTipMark);
	ClientNetwork::Instance()->getDataCache()->addClientServerIdPair(entityArrowTipMark->entityId, 0);
	EntityRenderManager::Instance()->renderEntity(entityArrowTipMark, (float)packet->destoryTime * 1000);
	entityArrowTipMark->setActorName("arrow02.actor");
	EntityRenderManager::Instance()->refreshEntityActor(BLOCKMAN::Blockman::Instance()->getWorld()->getEntity(entityArrowTipMark->entityId), "arrow02.actor");
	BLOCKMAN::Blockman::Instance()->m_pPlayer->setArrowTipMarkEntityId(entityArrowTipMark->entityId);
	BLOCKMAN::Blockman::Instance()->m_pPlayer->setArrowTipMarkDestoryTimes(packet->destoryTime);
}

void S2CEntityPacketHandles::createEntitySessionNpc(std::shared_ptr<S2CEntitySpawnCommonPacket>& packet)
{
	auto world = BLOCKMAN::Blockman::Instance()->getWorld();
	auto entitySessionNpc = LordNew EntitySessionNpc(world);

	float x = packet->m_pos[0] / 32.f;
	float y = packet->m_pos[1] / 32.f;
	float z = packet->m_pos[2] / 32.f;
	// float yaw = packet->m_yaw * 360.f / 256.0F;
	// float pitch = packet->m_pitch * 360.f / 256.0F;
	float yaw = packet->m_yaw;
	float pitch = packet->m_pitch;
	entitySessionNpc->prevPos = entitySessionNpc->lastTickPos = packet->m_pos;
	entitySessionNpc->serverPos.x = static_cast<int>(Math::Floor(packet->m_pos[0] * 32));
	entitySessionNpc->serverPos.y = static_cast<int>(Math::Floor(packet->m_pos[1] * 32));
	entitySessionNpc->serverPos.z = static_cast<int>(Math::Floor(packet->m_pos[2] * 32));
	entitySessionNpc->setPositionAndRotation(packet->m_pos, packet->m_yaw, packet->m_pitch);
	// TODO: add display name
	entitySessionNpc->forceSpawn = true;
	world->spawnEntityInWorld(entitySessionNpc);
	ClientNetwork::Instance()->getDataCache()->addClientServerIdPair(entitySessionNpc->entityId, packet->m_entityID);
}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketEntityPlayerTeleport>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();

	Entity* pEntity = dataCache->getEntityByServerId(packet->m_entityId);
	if (!pEntity) {
		return;
	}

	pEntity->serverPos.x = (int)(packet->m_x * 32);
	pEntity->serverPos.y = (int)(packet->m_y * 32);
	pEntity->serverPos.z = (int)(packet->m_z * 32);
	float x = packet->m_x;
	float z = packet->m_z;
	float yaw = pEntity->rotationYaw;
	float pitch = pEntity->rotationPitch;
	if (dynamic_cast<EntityPlayerSPMP*>(pEntity))
	{
		float y = packet->m_y + pEntity->yOffset + 1 / 32.0f; // 1 / 32.0f is for the deviation of float, prevent the player dropping down the block  
		pEntity->setPositionAndRotation({ x, y, z }, yaw, pitch);
	}
	else if (auto otherPlayer = dynamic_cast<EntityOtherPlayerMP*>(pEntity))
	{
		float y = packet->m_y + 1 / 32.0f;
		yaw = otherPlayer->getNextYaw();
		pitch = otherPlayer->getNextPitch();
		pEntity->setPositionAndRotation2(Vector3(x, y, z), yaw, pitch, 3);
	}
}


void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketSpawnCreatureBullet>& packet)
{
	auto world = BLOCKMAN::Blockman::Instance()->getWorld();
	auto dataCache = ClientNetwork::Instance()->getDataCache();

	EntityLivingBase* thrower = dynamic_cast<EntityLivingBase*>(dataCache->getEntityByServerId(packet->throwerId));
	EntityLivingBase* targeter = dynamic_cast<EntityLivingBase*>(dataCache->getEntityByServerId(packet->targeterId));
	if (thrower == nullptr || targeter == nullptr)
		return;

	EntityCreatureBullet* pBullet = LordNew EntityCreatureBullet(world, thrower, targeter, CreatureBulletType(packet->type));
	world->spawnEntityInWorld(pBullet);

	dataCache->addClientServerIdPair(pBullet->entityId, packet->entityId);
}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncEntityCreature>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto entityId = dataCache->getClientId(packet->entityId);
	auto entityCreature = dynamic_cast<EntityCreature*>(BLOCKMAN::Blockman::Instance()->getWorld()->getEntity(entityId));
	if (!entityCreature)
	{
		LordLogError("entityCreature does not exist, id: %d", entityId);
		return;
	}
	entityCreature->setActorName(packet->actorName);
	entityCreature->setMonsterType((CRETURE_AI_TYPE)packet->creatureEntityType);
	entityCreature->setNameLang(packet->nameLang);
	EntityRenderManager::Instance()->refreshEntityActor(BLOCKMAN::Blockman::Instance()->getWorld()->getEntity(entityId), entityCreature->getActorName());
}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncCreatureEntityAction>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto entityId = dataCache->getClientId(packet->entityId);
	auto entityCreature = dynamic_cast<EntityCreature*>(BLOCKMAN::Blockman::Instance()->getWorld()->getEntity(entityId));
	if (!entityCreature)
	{
		LordLogError("entityCreature does not exist, id: %d", packet->entityId);
		return;
	}

	if (entityCreature->getBaseAction() == CREATURE_AS_ATTACK)
		entityCreature->setBaseAction((CreatureActionState)packet->preBaseAction);

	entityCreature->setBaseAction((CreatureActionState)packet->actionStatus);

	entityCreature->changeCreatureAction((CreatureActionState)packet->actionStatus);
	if (entityCreature->getCreatureAction() == CREATURE_AS_BE_PLAYER_ATTACK)
		entityCreature->setHurtTime(5);
}

void  S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketSpwanSkillItemThrowable>& packet)
{
	auto world = BLOCKMAN::Blockman::Instance()->getWorld();
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto entityId = dataCache->getClientId(packet->throwerId);
	EntityLivingBase* thrower = dynamic_cast<EntityLivingBase*>(dataCache->getEntityByServerId(packet->throwerId));
	if (!thrower)
		return;
	SkillItem* pSkillItem = LogicSetting::Instance()->getSkillItem(packet->skillId);
	if (pSkillItem)
	{
		EntitySkillThrowable* entitySkillThrowable = LordNew EntitySkillThrowable(world, thrower);
		entitySkillThrowable->setReleaseDistance(pSkillItem->ItemSkillDistace);
		entitySkillThrowable->setMoveSpeed(pSkillItem->MoveSpeed);
		entitySkillThrowable->setDropSpeed(pSkillItem->DropSpeed);
		entitySkillThrowable->setIsParabola(pSkillItem->Parabola);
		entitySkillThrowable->setAtlasName(pSkillItem->AtlasName);
		entitySkillThrowable->setItemIconName(pSkillItem->ItemIconName);
		entitySkillThrowable->setItemSkillId(pSkillItem->ItemId);
		entitySkillThrowable->setEffectName(pSkillItem->EffectName);
		entitySkillThrowable->setHeadingFromThrower(thrower, thrower->rotationPitch, thrower->rotationYaw, 0.0, 2.0, 0.0);
		entitySkillThrowable->setVelocity(packet->motion);
		world->spawnEntityInWorld(entitySkillThrowable);
		dataCache->addClientServerIdPair(entitySkillThrowable->entityId, packet->throwerId);
	}
}

void  S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketActorNpcContent>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto entity = dynamic_cast<EntityActorNpc*>(dataCache->getEntityByServerId(packet->entityId));
	if (!entity)
		return;
	entity->setContent(packet->content);
}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketEnableMovement>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto player = dynamic_cast<EntityPlayer*>(dataCache->getPlayerByServerId(packet->entityId));
	if (!player)
		return;
	player->enableMovement();
}


void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncEntityBlockmanInfo>& packet)
{
	LordLogInfo("S2CPacketSyncEntityBlockmanInfo blockmanType  = %d ", packet->blockmanType);
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto blockman = dynamic_cast<EntityBlockman*>(dataCache->getEntityByServerId(packet->entityId));
	if (blockman)
	{
		blockman->setNameLang(packet->nameLang);
		blockman->setActorName(packet->actorName);
		blockman->setActorBody(packet->actorBody);
		blockman->setActorBodyId(packet->actorBodyId);
		blockman->setCurrentMaxSpeed(packet->maxSpeed);
		BLOCKMAN::Blockman::Instance()->getWorld()->changeEntityActor(blockman, packet->actorName);
		if (auto entity = dynamic_cast<EntityBlockmanEmpty*>(blockman))
		{
			entity->setCurSpeed(packet->curSpeed);
			entity->setSpeedAddtion(packet->addSpeed);
			entity->setNeedRecordRoute(packet->isRecordRoute);
			entity->setLongHitTimes(packet->longHitTimes);
		}
	}
}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketBlockmanMoveAttr>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto blockman = dynamic_cast<EntityBlockmanEmpty*>(dataCache->getEntityByServerId(packet->entityId));
	if (blockman)
	{
		blockman->pushX = packet->pushX;
		blockman->pushZ = packet->pushZ;
		blockman->motion = packet->motion;
		blockman->setPositionAndRotation2(packet->position, packet->yaw, blockman->rotationPitch, 3);
	}
}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketEntityFrozenTime>& packet)
{
	auto entity = ClientNetwork::Instance()->getDataCache()->getEntityByServerId(packet->entityId);
	auto player = dynamic_cast<EntityPlayer*>(entity);
	if (player)
	{
		if (player == BLOCKMAN::Blockman::Instance()->m_pPlayer)
		{
			BLOCKMAN::Blockman::Instance()->m_playerControl->setDisableMovement(packet->time > 0);
		}
		player->setOnFrozen(packet->time);
		return;
	}
	auto blockman = dynamic_cast<EntityBlockmanEmpty*>(entity);
	if (blockman)
	{
		blockman->setOnFrozen(packet->time);
		return;
	}
}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketEntityPlayerTeleportWithMotion>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();

	Entity* pEntity = dataCache->getEntityByServerId(packet->m_entityId);
	if (!pEntity)
	{
		return;
	}

	pEntity->serverPos.x = (int)(packet->m_x * 32);
	pEntity->serverPos.y = (int)(packet->m_y * 32);
	pEntity->serverPos.z = (int)(packet->m_z * 32);

	pEntity->motion.x = packet->m_mx;
	pEntity->motion.y = packet->m_my;
	pEntity->motion.z = packet->m_mz;
	pEntity->onGround = packet->m_isOnGround;

	float x = packet->m_x;
	float z = packet->m_z;
	float yaw = pEntity->rotationYaw;
	float pitch = pEntity->rotationPitch;
	if (dynamic_cast<EntityPlayerSPMP*>(pEntity))
	{
		float y = packet->m_y + pEntity->yOffset + 1 / 32.0f; // 1 / 32.0f is for the deviation of float, prevent the player dropping down the block  
		pEntity->setPositionAndRotation({ x, y, z }, yaw, pitch);
	}
	else if (auto otherPlayer = dynamic_cast<EntityOtherPlayerMP*>(pEntity))
	{
		float y = packet->m_y + 1 / 32.0f;
		yaw = otherPlayer->getNextYaw();
		pitch = otherPlayer->getNextPitch();
		pEntity->setPositionAndRotation2(Vector3(x, y, z), yaw, pitch, 3);
	}
}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketSpawnCannon>& packet)
{
	auto world = BLOCKMAN::Blockman::Instance()->getWorld();
	auto entityActorCannon = LordNew EntityActorCannonClient(world, packet->m_pos, packet->m_fallOnPos);

	entityActorCannon->prevPos = entityActorCannon->lastTickPos = packet->m_pos;
	entityActorCannon->serverPos.x = static_cast<int>(Math::Floor(packet->m_pos[0] * 32));
	entityActorCannon->serverPos.y = static_cast<int>(Math::Floor(packet->m_pos[1] * 32));
	entityActorCannon->serverPos.z = static_cast<int>(Math::Floor(packet->m_pos[2] * 32));
	entityActorCannon->setPositionAndRotation(packet->m_pos, packet->m_yaw, packet->m_pitch);
	entityActorCannon->forceSpawn = true;
	world->spawnEntityInWorld(entityActorCannon);
	ClientNetwork::Instance()->getDataCache()->addClientServerIdPair(entityActorCannon->entityId, packet->m_entityID);
}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncEntityBuildNpc>& packet)
{
	LordLogInfo("S2CPacketSyncEntityBuildNpc actorId  = %d ", packet->m_actorId);
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto buildNpc = dynamic_cast<EntityBuildNpc*>(dataCache->getEntityByServerId(packet->entityId));
	if (buildNpc)
	{
		buildNpc->m_userId = packet->m_userId;
		buildNpc->setActorId(packet->m_actorId);
		buildNpc->m_maxQueueNum = packet->m_maxQueueNum;
		buildNpc->m_productCapacity = packet->m_productCapacity;
		buildNpc->m_queueUnlockPrice = packet->m_queueUnlockPrice;
		buildNpc->m_queueUnlockCurrencyType = packet->m_queueUnlockCurrencyType;

		buildNpc->setRecipes(packet->m_recipes);
		buildNpc->setProducts(packet->m_products);
		buildNpc->setUnlockQueues(packet->m_unlockQueues);
		buildNpc->m_isDataChange = true;

		BLOCKMAN::Blockman::Instance()->getWorld()->changeEntityActor(buildNpc, buildNpc->getActorName());

		if (buildNpc->getBuildType() == BuildType::BUILD_FARM)
		{
			RanchBuildFarmUpdateEvent::emit(buildNpc->entityId);
		}

		if (buildNpc->getBuildType() == BuildType::BUILD_PLANT)
		{
			RanchBuildFactoryUpdateEvent::emit(buildNpc->entityId);
		}
	}
}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncEntityLandNpc>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto landNpc = dynamic_cast<EntityLandNpc*>(dataCache->getEntityByServerId(packet->entityId));
	if (landNpc)
	{
		LordLogInfo("S2CPacketSyncEntityLandNpc entityId  = %d ", packet->entityId);
		landNpc->setName(packet->name);
		landNpc->setNameLang(packet->nameLang);
		landNpc->setActorName(packet->actorName);
		landNpc->setActorBody(packet->actorBody);
		landNpc->setActorBodyId(packet->actorBodyId);
		landNpc->setStatus(packet->status);
		landNpc->setTimeLeft(packet->timeLeft);
		landNpc->setRecipe(packet->recipe);
		landNpc->setReward(packet->reward);
		landNpc->setUserId(packet->userId);
		landNpc->setLandCode(packet->landCode);
		landNpc->setPrice(packet->price);
		landNpc->setTotalTime(packet->totalTime);
		BLOCKMAN::Blockman::Instance()->getWorld()->changeEntityActor(landNpc, landNpc->getActorName());
	}

}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncEntityBulletin>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto entityBulletin = dynamic_cast<EntityBulletin*>(dataCache->getEntityByServerId(packet->entityId));
	if (entityBulletin)
	{
		LordLogInfo("S2CPacketSyncEntityBulletin entityId  = %d ", packet->entityId);
		entityBulletin->setBulletinId( packet->bulletinId);
		Blockman::Instance()->getWorld()->changeEntityActor(entityBulletin, entityBulletin->getActorName());
	}

}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketEntityHurtTime>& packet)
{
	auto entity = ClientNetwork::Instance()->getDataCache()->getEntityByServerId(packet->entityId);
	auto player = dynamic_cast<EntityPlayer*>(entity);
	if (player)
	{
		player->setOnHurt(packet->time);
	}
}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketEntityColorfulTime>& packet)
{
	auto entity = ClientNetwork::Instance()->getDataCache()->getEntityByServerId(packet->entityId);
	auto player = dynamic_cast<EntityPlayer*>(entity);
	if (player)
	{
		player->setOnColorful(packet->time);
	}
}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketEntitySelected>& packet)
{
	auto entity = ClientNetwork::Instance()->getDataCache()->getEntityByServerId(packet->entityId);
	if (entity)
		entity->setOnSelected(packet->type);
}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketEntityDisableMoveTime>& packet)
{
	auto entity = ClientNetwork::Instance()->getDataCache()->getEntityByServerId(packet->entityId);
	auto player = dynamic_cast<EntityPlayer*>(entity);
	if (player)
	{
		if (player == Blockman::Instance()->m_pPlayer)
		{
			Blockman::Instance()->m_playerControl->setDisableMovement(true);
		}
		player->setDisableMove(packet->time);
		return;
	}
}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncEntityBird>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto entity = dynamic_cast<EntityBird*>(dataCache->getEntityByServerId(packet->entityId));
	if (entity)
	{
		LordLogInfo("S2CPacketSyncEntityBird entityId  = %d ", packet->entityId);
		entity->m_userId = packet->userId;
		entity->m_birdId = packet->birdId;
		entity->setNameLang(packet->nameLang);
		entity->setActorName(packet->actorName);
		entity->setActorBody(packet->actorBody);
		entity->setActorBodyId(packet->actorBodyId);
		entity->setDressGlasses(packet->dressGlasses);
		entity->setDressHat(packet->dressHat);
		entity->setDressBeak(packet->dressBeak);
		entity->setDressWing(packet->dressWing);
		entity->setDressTail(packet->dressTail);
		entity->setDressEffect(packet->dressEffect);
		Blockman::Instance()->getWorld()->changeEntityActor(entity, entity->getActorName());
	}

}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncEntityAction>& packet)
{
	switch (packet->entityClass)
	{
	case ENTITY_CLASS_BIRD:
	case ENTITY_CLASS_BIRDAI:
		updateEntityBirdAction(packet);
		break;
	}
}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncEntityHealth>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto entity = dynamic_cast<EntityLivingBase*>(dataCache->getEntityByServerId(packet->entityId));
	if (!entity)
	{
		LordLogError("S2CPacketSyncEntityHealth entity does not exist, id: %d", packet->entityId);
		return;
	}
	entity->setEntityMaxHealth(packet->maxHealth);
	entity->setEntityHealth(packet->health);
}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketChangeGuideArrowStatus>& packet)
{
	if (packet->flag)
	{
		Blockman::Instance()->m_pPlayer->addGuideArrow(Vector3(packet->arrowPos[0], packet->arrowPos[1], packet->arrowPos[2]));
	}
	else
	{
		Blockman::Instance()->m_pPlayer->deleteGuideArrow(Vector3(packet->arrowPos[0], packet->arrowPos[1], packet->arrowPos[2]));
	}
}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketAddWallText>& packet)
{
	Blockman::Instance()->m_globalRender->addWallText(packet->text, Vector3(packet->textPos[0], packet->textPos[1], packet->textPos[2]), packet->scale, packet->yaw, packet->pitch, packet->r, packet->g, packet->b, packet->a);
}

void S2CEntityPacketHandles::handlePacket(std::shared_ptr<S2CPacketDeleteWallText>& packet)
{
	Blockman::Instance()->m_globalRender->deleteWallText(Vector3(packet->textPos[0], packet->textPos[1], packet->textPos[2]));
}
