#include "ServerPacketSender.h"
#include "Server.h"
#include "Network/ServerNetwork.h"
#include "Network/ClientPeer.h"
#include "Network/NetworkDefine.h"
#include "Entity/EntityArrow.h"
#include "Entity/EntityBullet.h"
#include "Entity/EntityPotion.h"
#include "Entity/EntityXPOrb.h"
#include "Entity/EntityVehicle.h"
#include "Entity/EntityRankNpc.h"
#include "Blockman/Entity/EntityPlayerMP.h"
#include "Inventory/IInventory.h"
#include "Blockman/World/BlockChangeRecorderServer.h"
#include "Chat/ChatComponent.h"
#include "Item/Potion.h"
#include "Blockman/Entity/EntityTracker.h"
#include "TileEntity/TileEntityInventory.h"
#include "Script/GameServerEvents.h"
#include "Entity/EntitySnowball.h"
#include "Entity/EntityTNTPrimed.h"
#include "Entity/EntityTNTThrowable.h"
#include "Entity/EntityCreatureBullet.h"
#include "Entity/EntityGrenade.h"
#include "Entity/EntityFireball.h"
#include "Inventory/CoinManager.h"
#include "CommodityManager.h"
#include "Setting/LogicSetting.h"
#include "Entity/EntityAircraft.h"
#include "Blockman/Entity/EntityCreatureAI.h"
#include "Setting/MultiLanTipSetting.h"
#include "Entity/EntitySkillThrowable.h"
#include "Entity/EntityBlockman.h"
#include "Entity/EntityBlockmanEmpty.h"
#include "World/Ranch.h"
#include "Entity/EntityLandNpc.h"
#include "Entity/EntityBulletin.h"
#include "Entity/EntityBird.h"
#include "World/BirdSimulator.h"
#include "Network/protocol/S2CPackets.h"

using namespace BLOCKMAN;

ServerPacketSender::ServerPacketSender()
{

}

ServerPacketSender::~ServerPacketSender()
{

}

void ServerPacketSender::sendPacketToTrackingPlayers(int trackedEntityId, std::shared_ptr<DataPacket> packet, bool includeSelf/* = false*/)
{
	auto network = Server::Instance()->getNetwork();
	if (includeSelf) {
		EntityPlayerMP* self = dynamic_cast<EntityPlayerMP*>(Server::Instance()->getWorld()->getPlayerEntity(trackedEntityId));
		if (self) {
			network->sendPacket(packet, self->getRaknetID(), true);
		}
	}

	const auto& targetPlayers = EntityTracker::Instance()->getTrackingPlayersOf(trackedEntityId);
	for (const auto& pair : targetPlayers)
	{
		network->sendPacket(packet, dynamic_cast<EntityPlayerMP*>(pair.second)->getRaknetID(), true);
	}
}


void ServerPacketSender::sendEntityAnimateToTackingPlayers(NETWORK_DEFINE::PacketAnimateType animateType, int entityId, int32_t value/* = 0*/)
{
	auto animatePacket = LORD::make_shared<S2CPacketAnimate>();
	animatePacket->m_animateType = animateType;
	animatePacket->m_entityRuntimeId = entityId;
	animatePacket->m_value = value;
	sendPacketToTrackingPlayers(entityId, animatePacket);
}

void ServerPacketSender::broadCastUpdateBlock(Vector3i position, int entityId)
{
	auto packet = LORD::make_shared<S2CPacketUpdateBlocks>();
	auto world = Server::Instance()->getWorld();
	packet->updates.emplace_back(position, world->getBlockId(position), world->getBlockMeta(position));
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet, entityId);
}

std::shared_ptr<DataPacket> ServerPacketSender::constructAddPotionEffectPacket(int entityId, PotionEffect* effect)
{
	auto packet = LORD::make_shared<S2CPacketAddEntityEffect>();
	packet->entityId = entityId;
	packet->potionId = effect->getPotionID();
	packet->amplifier = effect->getAmplifier();
	packet->duration = effect->getDuration();
	packet->isAmbient = effect->getIsAmbient();
	packet->isSplashPotion = effect->getIsSplashPotion();
	packet->isPotionDurationMax = effect->getIsPotionDurationMax();

	return packet;
}

void ServerPacketSender::sendAddEntityEffectToTrackingPlayers(int entityId, PotionEffect* effect)
{
	auto packet = constructAddPotionEffectPacket(entityId, effect);

	//LordLogInfo("sendAddEntityEffectToTrackingPlayers: entityId: %d, potionId: %d, amplifier: %d, duration: %d",
	//	packet->entityId, packet->potionId, packet->amplifier, packet->duration);

	sendPacketToTrackingPlayers(entityId, packet, true);
}

void ServerPacketSender::sendAddEntityEffect(BLOCKMAN::EntityPlayerMP* targetEntity, int entityId, BLOCKMAN::PotionEffect * effect)
{
	auto packet = constructAddPotionEffectPacket(entityId, effect);

	//LordLogInfo("sendAddEntityEffect: entityId: %d, potionId: %d, amplifier: %d, duration: %d",
	//	packet->entityId, packet->potionId, packet->amplifier, packet->duration);

	Server::Instance()->getNetwork()->sendPacket(packet, targetEntity->getRaknetID(), true);
}

void ServerPacketSender::sendRemoveEntityEffectToTrackingPlayers(int entityId, int potionId)
{
	auto packet = LORD::make_shared<S2CPacketRemoveEntityEffect>();
	packet->entityId = entityId;
	packet->potionId = potionId;

	//LordLogInfo("sendRemoveEntityEffectToTrackingPlayers: entityId: %d, potionId: %d", packet->entityId, packet->potionId);

	sendPacketToTrackingPlayers(entityId, packet, true);
}

void ServerPacketSender::sendGameInfo(ui64 rakssid, int runtimeID, const Vector3i& spawnPos, 
	const vector<String>::type& recipes, bool isTimeStopped, int32_t sex, String gameType, int32_t defaultIdle)
{
	auto packet = LORD::make_shared<S2CPacketGameInfo>();
	packet->m_playerRuntimeId = runtimeID;
	packet->spawnPos = spawnPos;
	packet->recipes = recipes;
	packet->isTimeStopped = isTimeStopped;
	packet->sex = sex;
	packet->m_gameType = gameType;
	packet->m_defaultIdle = defaultIdle;
	LORD::map<int, int>::type coinMapping = CoinManager::Instance()->m_coinMapping;
	for (LORD::map<int, int>::type::iterator it = coinMapping.begin(); it != coinMapping.end(); ++it)
	{
		packet->m_coinMapping.push_back({ it->first, it->second });
	}

	Server::Instance()->getNetwork()->sendPacket(packet, rakssid, true);
}

void ServerPacketSender::sendUserAttrToTrackingPlayers(int entityId, const UserAttrInfo& attr)
{
	auto packet = LORD::make_shared<S2CPacketUserAttr>();
	packet->entityId = entityId;
	packet->faceId = attr.faceId;
	packet->hairId = attr.hairId;
	packet->topsId = attr.topsId;
	packet->pantsId = attr.pantsId;
	packet->shoesId = attr.shoesId;
	packet->glassesId = attr.glassesId;
	packet->scarfId = attr.scarfId;
	packet->wingId = attr.wingId;
	packet->hatId = attr.hatId;
	packet->decoratehatId = attr.decoratehatId;
	packet->armId = attr.armId;
	packet->tailId = attr.tailId;
	packet->footHaloId = attr.footHaloId;
	packet->backEffectId = attr.backEffectId;
	packet->headEffectId = attr.headEffectId;
	packet->extrawingId = attr.extrawingId;
	packet->crownId = attr.crownId;
	packet->bagId = attr.bagId;
	packet->skinColor.r = attr.skinColor.r; 
	packet->skinColor.g = attr.skinColor.g;
	packet->skinColor.b = attr.skinColor.b;
	packet->skinColor.a = attr.skinColor.a;
	sendPacketToTrackingPlayers(entityId, packet, true);
}

void ServerPacketSender::sendUserAttr(ui64 rakssid, int runtimeID, const UserAttrInfo& attr)
{
	auto packet = LORD::make_shared<S2CPacketUserAttr>();
	packet->entityId = runtimeID;
	packet->faceId = attr.faceId;
	packet->hairId = attr.hairId;
	packet->topsId = attr.topsId;
	packet->pantsId = attr.pantsId;
	packet->shoesId = attr.shoesId;
	packet->glassesId = attr.glassesId;
	packet->scarfId = attr.scarfId;
	packet->wingId = attr.wingId;
	packet->hatId = attr.hatId;
	packet->decoratehatId = attr.decoratehatId;
	packet->armId = attr.armId;
	packet->tailId = attr.tailId;
	packet->footHaloId = attr.footHaloId;
	packet->backEffectId = attr.backEffectId;
	packet->headEffectId = attr.headEffectId;
	packet->extrawingId = attr.extrawingId;
	packet->crownId = attr.crownId;
	packet->bagId = attr.bagId;
	packet->skinColor.r = attr.skinColor.r;
	packet->skinColor.g = attr.skinColor.g;
	packet->skinColor.b = attr.skinColor.b;
	packet->skinColor.a = attr.skinColor.a;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssid, true);
}

void ServerPacketSender::sendUserAttrFromEntityToTrackingPlayers(BLOCKMAN::EntityPlayer* player)
{
	auto packet = LORD::make_shared<S2CPacketUserAttr>();
	packet->entityId = player->entityId;
	packet->faceId = player->m_faceID;
	packet->hairId = player->m_hairID;
	packet->topsId = player->m_topsID;
	packet->pantsId = player->m_pantsID;
	packet->shoesId = player->m_shoesID;
	packet->glassesId = player->m_glassesId;
	packet->scarfId = player->m_scarfId;
	packet->wingId = player->m_wingId;
	packet->hatId = player->m_hatId;
	packet->decoratehatId = player->m_decorate_hatId;
	packet->armId = player->m_armId;
	packet->footHaloId = player->m_footHaloId;
	packet->backEffectId = player->m_backEffectId;
	packet->extrawingId = player->m_extra_wingId;
	packet->crownId = player->m_crownId;
	packet->bagId = player->m_bagId;
	packet->skinColor.r = player->m_skinColor.r;
	packet->skinColor.g = player->m_skinColor.g;
	packet->skinColor.b = player->m_skinColor.b;
	packet->skinColor.a = player->m_skinColor.a;
	packet->headEffectId = player->m_headEffectId;
	sendPacketToTrackingPlayers(player->entityId, packet, true);
}

void ServerPacketSender::sendUserAttrFromEntity(ui64 rakssId, BLOCKMAN::EntityPlayer* player)
{
	auto packet = LORD::make_shared<S2CPacketUserAttr>();
	packet->entityId = player->entityId;
	packet->faceId = player->m_faceID;
	packet->hairId = player->m_hairID;
	packet->topsId = player->m_topsID;
	packet->pantsId = player->m_pantsID;
	packet->shoesId = player->m_shoesID;
	packet->glassesId = player->m_glassesId;
	packet->scarfId = player->m_scarfId;
	packet->wingId = player->m_wingId;
	packet->hatId = player->m_hatId;
	packet->decoratehatId = player->m_decorate_hatId;
	packet->armId = player->m_armId;
	packet->extrawingId = player->m_extra_wingId;
	packet->footHaloId = player->m_footHaloId;
	packet->backEffectId = player->m_backEffectId;
	packet->crownId = player->m_crownId;
	packet->headEffectId = player->m_headEffectId;
	packet->bagId = player->m_bagId;
	packet->skinColor.r = player->m_skinColor.r;
	packet->skinColor.g = player->m_skinColor.g;
	packet->skinColor.b = player->m_skinColor.b;
	packet->skinColor.a = player->m_skinColor.a;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendSyncWorldTime(ui64 rakssid, int64_t time)
{
	auto packet = LORD::make_shared<S2CPacketSyncWorldTime>();
	packet->time = time;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssid, true);
}

void ServerPacketSender::broadcastSyncWorldTime(int64_t time)
{
	auto packet = LORD::make_shared<S2CPacketSyncWorldTime>();
	packet->time = time;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendSyncWorldWeather(ui64 rakssid, String weather, int time)
{
	auto packet = LORD::make_shared<S2CPacketSyncWorldWeather>();
	packet->weather = weather;
	packet->time = time;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssid, true);
}

void ServerPacketSender::broadcastSyncWorldWeather(String weather, int time)
{
	auto packet = LORD::make_shared<S2CPacketSyncWorldWeather>();
	packet->weather = weather;
	packet->time = time;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::resetUpdateBlocks()
{
	std::vector<BlockPos> blocks;
	const auto& positions = BlockChangeRecorderServer::Instance()->getChangedPositions();

	for (const auto& position : positions)
	{
		BlockPos block(position.pos.x, position.pos.y, position.pos.z);
		blocks.push_back(block);
	}

	EntityPlayers& players = Server::Instance()->getWorld()->getPlayers();

	for (EntityPlayers::iterator it = players.begin(); it != players.end(); ++it)
	{
		EntityPlayerMP* pPlayer = dynamic_cast<EntityPlayerMP*>(it->second);
		if (!pPlayer)
			continue;

		pPlayer->addUpdateBlocks(blocks);
	}
}

void ServerPacketSender::sendUpdateBlocks(ui64 rakssid)
{
	auto packet = LORD::make_shared<S2CPacketUpdateBlocks>();
	const auto& positions = BlockChangeRecorderServer::Instance()->getChangedPositions();
	packet->updates.reserve(positions.size());
	auto world = Server::Instance()->getWorld();
	for (const auto& position : positions)
	{
		packet->updates.emplace_back(position.pos, world->getBlockId(position.pos), world->getBlockMeta(position.pos));
	}

	Server::Instance()->getNetwork()->sendPacket(packet, rakssid, true);
}

void ServerPacketSender::sendUpdateBlocksWithDistance(ui64 rakssid, const std::vector<NETWORK_DEFINE::S2CPacketUpdateBlockInfo>& block_info)
{
	if (block_info.size() <= 0) return;

	auto packet = LORD::make_shared<S2CPacketUpdateBlocks>();

	packet->updates.reserve(block_info.size());
	auto world = Server::Instance()->getWorld();

	for (auto block : block_info)
	{
		packet->updates.emplace_back(Vector3i(block.x, block.y, block.z), (int)block.getBlockId(), (int)block.getBlockMeta());
	}

	Server::Instance()->getNetwork()->sendPacket(packet, rakssid, true);
}

void ServerPacketSender::sendPlayerInventory(ui64 rakssid, InventoryPlayer* inventory)
{
	auto packet = LORD::make_shared<S2CPacketPlayerInventory>();

	int dirtyIndex = inventory->getInventoryDirtyIndex();
	if (dirtyIndex >= 0)
	{
		packet->playerInventory.push_back({ inventory->mainInventory[dirtyIndex], dirtyIndex });
		inventory->setInventoryDirtyIndex(-1);
	}
	else
	{
		for (int i = 0; i < InventoryPlayer::MAIN_INVENTORY_COUNT; ++i)
		{
			if (!ItemStack::areItemStacksEqual(inventory->mainInventory[i], inventory->prevMainInventory[i]))
			{
				packet->playerInventory.push_back({ inventory->mainInventory[i], i });
				inventory->prevMainInventory[i] = inventory->mainInventory[i] == nullptr ? nullptr : inventory->mainInventory[i]->copy();
			}
		}
		for (int i = 0; i < InventoryPlayer::ARMOR_INVENTORY_COUNT; ++i)
		{
			if (!ItemStack::areItemStacksEqual(inventory->armorInventory[i], inventory->prevArmorInventory[i]))
			{
				packet->playerInventory.push_back({ inventory->armorInventory[i], i + InventoryPlayer::MAIN_INVENTORY_COUNT });
				inventory->prevArmorInventory[i] = inventory->armorInventory[i] == nullptr ? nullptr : inventory->armorInventory[i]->copy();
			}
		}
	}

	/*
	LordLogInfo("sendPlayerInventory");
	for (size_t i = 0; i < packet->playerInventory.size(); ++i) {
		LordLogInfo("inventory[%u] = {%d, %d, %d, %d}", i, packet->playerInventory[i].valid,
			packet->playerInventory[i].id, packet->playerInventory[i].size, packet->playerInventory[i].damage);
		for (size_t j = 0; j < packet->playerInventory[i].enchantments.size(); ++j) {
			LordLogInfo("inventory[%u][%u] = {%d, %d}", i, j, (int32_t)packet->playerInventory[i].enchantments[j].id,
				(int32_t)packet->playerInventory[i].enchantments[j].level);
		}
	}
    */
	Server::Instance()->getNetwork()->sendPacket(packet, rakssid, true);
}

void ServerPacketSender::sendRebirth(ui64 rakssid, int runtimeID, const Vector3i& spawnPos, int sex, const String& showName)
{
	auto packet = LORD::make_shared<S2CPacketRebirth>();
	packet->m_playerRuntimeId = runtimeID;
	packet->m_posX = (i16)spawnPos.x;
	packet->m_posY = (i16)spawnPos.y;
	packet->m_posZ = (i16)spawnPos.z;
	packet->m_sex = sex;
	packet->m_showName = showName;

	Server::Instance()->getNetwork()->sendPacket(packet, rakssid, true);
}

void ServerPacketSender::sendEntityAttribute(EntityPlayerMP* targetEntity, EntityPlayerMP* changedEntity)
{
	auto packet = std::make_shared<S2CPacketUpdateAttribute>(changedEntity);
	Server::Instance()->getNetwork()->sendPacket(packet, targetEntity->getRaknetID(), true);
}

void ServerPacketSender::sendEntityAttributeToTrackingPlayers(EntityPlayerMP* changedEntity)
{
	auto packet = std::make_shared<S2CPacketUpdateAttribute>(changedEntity);
	sendPacketToTrackingPlayers(changedEntity->entityId, packet);
}

void ServerPacketSender::sendRemoveEntity(int removedEntityId, EntityPlayerMP* targetEntity)
{
	auto packet = LORD::make_shared<S2CPacketRemoveEntity>();
	packet->m_entityID = removedEntityId;
	Server::Instance()->getNetwork()->sendPacket(packet, targetEntity->getRaknetID(), true);
}

void ServerPacketSender::sendRemoveEntityToTrackingPlayers(int entityID)
{
	auto packet = LORD::make_shared<S2CPacketRemoveEntity>();
	packet->m_entityID = entityID;
	sendPacketToTrackingPlayers(entityID, packet);
}

void ServerPacketSender::sendEntityHeadRotationToTrackingPlayers(const EntityPlayers& trackingPlayers,
	int entityId, real32 headYaw)
{
	auto packet = LORD::make_shared<S2CPacketEntityHeadRotation>();
	packet->m_entityID = entityId;
	packet->m_headYaw = headYaw;

	for (auto it : trackingPlayers)
	{
		EntityPlayerMP* pPlayer = (EntityPlayerMP*)(it.second);
		Server::Instance()->getNetwork()->sendPacket(packet, pPlayer->getRaknetID(), true);
	}
}

void ServerPacketSender::sendEntityVelocity(ui64 rakssid, BLOCKMAN::Entity* pEntity)
{
	auto packet = LORD::make_shared<S2CPacketEntityVelocity>(pEntity);
	Server::Instance()->getNetwork()->sendPacket(packet, rakssid, true);

}

void ServerPacketSender::sendEntityVelocityToTrackingPlayers(const EntityPlayers& trackingPlayers,
	BLOCKMAN::Entity* pEntity)
{
	auto packet = LORD::make_shared<S2CPacketEntityVelocity>(pEntity);
	for (auto it : trackingPlayers)
	{
		EntityPlayerMP* pPlayer = (EntityPlayerMP*)(it.second);
		Server::Instance()->getNetwork()->sendPacket(packet, pPlayer->getRaknetID(), true);
	}
}

void ServerPacketSender::sendEntityVelocityToTrackingPlayers(const EntityPlayers& trackingPlayers,
	int entityId, float motionX, float motionY, float motionZ)
{
	auto packet = LORD::make_shared<S2CPacketEntityVelocity>(entityId, motionX, motionY, motionZ);
	for (auto it : trackingPlayers)
	{
		EntityPlayerMP* pPlayer = (EntityPlayerMP*)(it.second);
		Server::Instance()->getNetwork()->sendPacket(packet, pPlayer->getRaknetID(), true);
	}
}

void ServerPacketSender::sendEntityTeleport(ui64 rakssid, int entityId, int x, int y, int z, real32 yaw, real32 pitch)
{
	auto packet = LORD::make_shared<S2CPacketEntityTeleport>(entityId, x, y, z, yaw, pitch);
	//LordLogInfo("sendEntityTeleport, entityId=%d, (%d,%d,%d)", entityId, int(x / 32), int(y / 32), int(z / 32));
	Server::Instance()->getNetwork()->sendPacket(packet, rakssid, true);
}

void ServerPacketSender::sendEntityTeleportToTrackingPlayers(const EntityPlayers& trackingPlayers,
	int entityId, int x, int y, int z, real32 yaw, real32 pitch)
{
	auto packet = LORD::make_shared<S2CPacketEntityTeleport>(entityId, x, y, z, yaw, pitch);
	for (auto it : trackingPlayers)
	{
		EntityPlayerMP* pPlayer = (EntityPlayerMP*)(it.second);
		//LordLogInfo("sendEntityTeleportToTrackingPlayers, entityId=%d, (%d,%d,%d)", entityId, int(x/32), int(y/32), int(z/32));
		Server::Instance()->getNetwork()->sendPacket(packet, pPlayer->getRaknetID(), true);
	}
}

void ServerPacketSender::sendEntityMovementToTrackingPlayers(const EntityPlayers& trackingPlayers,
	int entityId, i16 x, i16 y, i16 z, real32 yaw, real32 pitch, bool immdi)
{
	auto packet = LORD::make_shared<S2CPacketEntityMovement>(entityId, x, y, z, yaw, pitch);
	for (auto it : trackingPlayers)
	{
		EntityPlayerMP* pPlayer = (EntityPlayerMP*)(it.second);
		Server::Instance()->getNetwork()->sendPacket(packet, pPlayer->getRaknetID(), immdi);
	}
}

void ServerPacketSender::sendEntityMovementToTrackingPlayers(const EntityPlayers& trackingPlayers,
	int entityId, real32 yaw, real32 pitch, bool immdi)
{
	auto packet = LORD::make_shared<S2CPacketEntityMovement>(entityId, yaw, pitch);
	for (auto it : trackingPlayers)
	{
		EntityPlayerMP* pPlayer = (EntityPlayerMP*)(it.second);
		Server::Instance()->getNetwork()->sendPacket(packet, pPlayer->getRaknetID(), immdi);
	}
}

void ServerPacketSender::sendEntityMovementToTrackingPlayers(const EntityPlayers& trackingPlayers,
	int entityId, i16 x, i16 y, i16 z,  bool immdi)
{
	auto packet = LORD::make_shared<S2CPacketEntityMovement>(entityId, x, y, z);
	for (auto it : trackingPlayers)
	{
		EntityPlayerMP* pPlayer = (EntityPlayerMP*)(it.second);
		Server::Instance()->getNetwork()->sendPacket(packet, pPlayer->getRaknetID(), immdi);
	}
}

void ServerPacketSender::sendPlayerChangeItem(ui64 rakssid, int itemIndex)
{
	auto packet = LORD::make_shared<S2CPacketChangeCurrentItem>();
	packet->itemIndex = itemIndex;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssid, true);

}

void ServerPacketSender::sendEntitySpawn(ui64 rakssid, Entity* myEntity)
{
	std::shared_ptr<DataPacket> packet = constructEntitySpawnPacket(myEntity);
	if (packet != nullptr)
	{
		Server::Instance()->getNetwork()->sendPacket(packet, rakssid, true);
	}
}

void ServerPacketSender::sendEquipArmors(BLOCKMAN::EntityPlayerMP * targetEntity, int entityId, const vector<ItemStackPtr>::type & armors)
{
	auto packet = LORD::make_shared<S2CPacketEquipArmors>();
	packet->playerId = entityId;
	for (const auto& armor : armors)
	{
		packet->armors.push_back({ armor });
	}
	Server::Instance()->getNetwork()->sendPacket(packet, targetEntity->getRaknetID(), true);
}

void ServerPacketSender::sendEquipArmorsToTrackingPlayers(int entityId, const vector<ItemStackPtr>::type & armors)
{
	auto packet = LORD::make_shared<S2CPacketEquipArmors>();
	packet->playerId = entityId;
	for (const auto& armor : armors)
	{
		packet->armors.push_back({ armor });
	}
	sendPacketToTrackingPlayers(entityId, packet);
}

void ServerPacketSender::sendUnequipArmorToTrackingPlayers(int entityId, int slotIndex)
{
	auto packet = LORD::make_shared<S2CPacketUnequipArmor>();
	packet->playerId = entityId;
	packet->slotIndex = slotIndex;
	sendPacketToTrackingPlayers(entityId, packet);
}

void ServerPacketSender::sendDestroyArmorToTrackingPlayers(int entityId, int slotIndex)
{
	auto packet = LORD::make_shared<S2CPacketDestroyArmor>();
	packet->playerId = entityId;
	packet->slotIndex = slotIndex;
	sendPacketToTrackingPlayers(entityId, packet);
}

void ServerPacketSender::sendChangeItemInHand(BLOCKMAN::EntityPlayerMP * targetEntity, int entityId, BLOCKMAN::ItemStackPtr itemStack)
{
	auto packet = LORD::make_shared<S2CPacketChangeItemInHand>();
	packet->playerId = entityId;
	if (!itemStack)
	{
		packet->itemId = -1;
		packet->enchanted = false;
	}
	else
	{
		packet->itemId = itemStack->itemID;
		auto tagList = itemStack->getEnchantmentTagList();
		packet->enchanted = tagList && tagList->tagCount() > 0;
	}
	Server::Instance()->getNetwork()->sendPacket(packet, targetEntity->getRaknetID(), true);
}

void ServerPacketSender::sendChangeItemInHandToTrackingPlayers(int entityId, BLOCKMAN::ItemStackPtr itemStack)
{
	auto packet = LORD::make_shared<S2CPacketChangeItemInHand>();
	packet->playerId = entityId;
	if (!itemStack)
	{
		packet->itemId = -1;
		packet->enchanted = false;
	}
	else
	{
		packet->itemId = itemStack->itemID;
		auto tagList = itemStack->getEnchantmentTagList();
		packet->enchanted = tagList && tagList->tagCount() > 0;
	}
	sendPacketToTrackingPlayers(entityId, packet);
}

std::shared_ptr<DataPacket> ServerPacketSender::constructEntitySpawnPacket(Entity* myEntity)
{
	if (myEntity->isDead)
	{
		// myEntity.worldObj.getWorldLogAgent().logWarning("Fetching addPacket for removed entity");
	}

	if (myEntity->isClass(ENTITY_CLASS_ITEM))
	{
		EntityItem* itemEntity = dynamic_cast<EntityItem*>(myEntity);
		//LordLogInfo("SpawnEntityItem, name=%s", itemEntity->getEntityName().c_str());
		return LORD::make_shared<S2CEntitySpawnCommonPacket>(itemEntity, ENTITY_CREATE_TYPE_ITEM, 1);
	}
	else if (myEntity->isClass(ENTITY_CLASS_PLAYERMP))
	{
		//EntityPlayer* pPlayer = (EntityPlayer*)myEntity;
		EntityPlayer *pPlayer = dynamic_cast<EntityPlayer*>(myEntity);
		std::shared_ptr<DataPacket> packet = LORD::make_shared<S2CPacketSpawnPlayer>(pPlayer);
		//LordLogInfo("SpawnPlayer \"%s\" id[%d] pos (%.1f, %.1f, %.1f)", pPlayer->getCommandSenderName().c_str(), pPlayer->entityId, pPlayer->position.x, pPlayer->position.y, pPlayer->position.z);
		return packet;
	}
	else if (myEntity->isClass(ENTITY_CLASS_BLOCKMAN))
	{
		auto var9 =  dynamic_cast<EntityBlockman*>(myEntity);
		return LORD::make_shared<S2CEntitySpawnCommonPacket>(myEntity, ENTITY_CREATE_TYPE_BLOCKMAN, (int) var9->getBlockmanType());
	}
	else if (myEntity->isClass(ENTITY_CLASS_BOAT))
	{
		return LORD::make_shared<S2CEntitySpawnCommonPacket>(myEntity, ENTITY_CREATE_TYPE_BOAT, 0);
	}
	else if (myEntity->isClass(ENTITY_CLASS_ANIMALS) || myEntity->isClass(ENTITY_CLASS_DRAGON))
	{
		// do nothing special
	}
	else if (myEntity->isClass(ENTITY_CLASS_FISHHOOK))
	{
		Entity* anglerEntity = ((EntityFishHook*)myEntity)->getAngler();
		if (anglerEntity)
		{
			int anglerEntityId = anglerEntity->entityId;
			return LORD::make_shared<S2CEntitySpawnCommonPacket>(myEntity, ENTITY_CREATE_TYPE_FISHHOOK, anglerEntityId);
		}
	}
	else if (myEntity->isClass(ENTITY_CLASS_ARROW))
	{
		int shootingEntityId = ((EntityArrow*)myEntity)->m_shootingEntityId;
		//LordLogInfo("SpawnArrow, entityId[%d],x[%f] y[%f] z[%f] motion.x[%f] motion.y[%f] motion.z[%f] yaw[%f] pitch[%f] onFire[%s]",
		//	myEntity->entityId, myEntity->position.x, myEntity->position.y, myEntity->position.z,
		//	myEntity->motion.x, myEntity->motion.y, myEntity->motion.z, myEntity->rotationYaw, myEntity->rotationPitch, myEntity->isBurning() ? "true" : "false");

		return LORD::make_shared<S2CEntitySpawnCommonPacket>(myEntity, ENTITY_CREATE_TYPE_ARROE, shootingEntityId, ((EntityArrow*)myEntity)->m_potionEffect);
	}
	else if (myEntity->isClass(ENTITY_CLASS_BULLET))
	{
		int shootingEntityId = ((EntityBullet*)myEntity)->m_shootingEntityId;
		return LORD::make_shared<S2CEntitySpawnCommonPacket>(myEntity, ENTITY_CREATE_TYPE_BULLET, shootingEntityId);
	}
	else if (myEntity->isClass(ENTITY_CLASS_VEHICLE))
	{
		EntityVehicle* pVehicle = (EntityVehicle*)myEntity;
		return LORD::make_shared<S2CEntitySpawnCommonPacket>(myEntity, ENTITY_CREATE_TYPE_VEHICLE, pVehicle->m_vehicleID);
	}
	else if (myEntity->isClass(ENTITY_CLASS_POTION))
	{
		Entity* thrower = ((EntityPotion*)myEntity)->getThrower();
		//LordLogInfo("SpawnEntityPotion, entityId[%d],x[%f] y[%f] z[%f] motion.x[%f] motion.y[%f] motion.z[%f] yaw[%f] pitch[%f] onFire[%s]",
		//	myEntity->entityId, myEntity->position.x, myEntity->position.y, myEntity->position.z,
		//	myEntity->motion.x, myEntity->motion.y, myEntity->motion.z, myEntity->rotationYaw, myEntity->rotationPitch, myEntity->isBurning() ? "true" : "false");

		return LORD::make_shared<S2CEntitySpawnCommonPacket>(myEntity, ENTITY_CREATE_TYPE_POTION, thrower ? thrower->entityId : myEntity->entityId);
	}
	else if (myEntity->isClass(ENTITY_CLASS_EXP_BOTTLE))
	{
		// return new Packet23VehicleSpawn(myEntity, 75);
		return nullptr;
	}
	else if (myEntity->isClass(ENTITY_CLASS_ENDER_PEARL))
	{
		// return new Packet23VehicleSpawn(myEntity, 65);
		return nullptr;
	}
	else if (myEntity->isClass(ENTITY_CLASS_ENDER_EYE))
	{
		// return new Packet23VehicleSpawn(myEntity, 72);
		return nullptr;
	}
	else if (myEntity->isClass(ENTITY_CLASS_FIREWORK_ROCKET))
	{
		// return new Packet23VehicleSpawn(myEntity, 76);
		return nullptr;
	}
	else if (myEntity->isClass(ENTITY_CLASS_EGG))
	{
		// return new Packet23VehicleSpawn(myEntity, 62);
		return nullptr;
	}
	else if (myEntity->isClass(ENTITY_CLASS_TNT_PRIMED))
	{
		Entity* shootingEntity = ((EntityTNTPrimed*)myEntity)->getTntPlacedBy();
		return LORD::make_shared<S2CEntitySpawnCommonPacket>(myEntity, ENTITY_CREATE_TYPE_TNT_PRIMED, shootingEntity ? shootingEntity->entityId : myEntity->entityId);
	}
	else if (myEntity->isClass(ENTITY_CLASS_ENDER_CRYSTAL))
	{
		// return new Packet23VehicleSpawn(myEntity, 51);
		return nullptr;
	}
	else if (myEntity->isClass(ENTITY_CLASS_FALLING_SAND))
	{
		// EntityFallingSand var5 = (EntityFallingSand)myEntity;
		// return new Packet23VehicleSpawn(myEntity, 70, var5.blockID | var5.metadata << 16);
		return nullptr;
	}
	else if (myEntity->isClass(ENTITY_CLASS_PAINTING))
	{
		// return new Packet25EntityPainting((EntityPainting)myEntity);
		return nullptr;
	}
	else if (myEntity->isClass(ENTITY_CLASS_ITEM_FRAME))
	{
		/*EntityItemFrame var4 = (EntityItemFrame)myEntity;
		var2 = new Packet23VehicleSpawn(myEntity, 71, var4.hangingDirection);
		var2.xPosition = MathHelper.floor_float((float)(var4.xPosition * 32));
		var2.yPosition = MathHelper.floor_float((float)(var4.yPosition * 32));
		var2.zPosition = MathHelper.floor_float((float)(var4.zPosition * 32));
		return var2;*/
		return nullptr;
	}
	else if (myEntity->isClass(ENTITY_CLASS_LEASH_KNOT))
	{
		/*EntityLeashKnot var1 = (EntityLeashKnot)myEntity;
		var2 = new Packet23VehicleSpawn(myEntity, 77);
		var2.xPosition = MathHelper.floor_float((float)(var1.xPosition * 32));
		var2.yPosition = MathHelper.floor_float((float)(var1.yPosition * 32));
		var2.zPosition = MathHelper.floor_float((float)(var1.zPosition * 32));
		return var2;*/
		return nullptr;
	}
	else if (myEntity->isClass(ENTITY_CLASS_XPORB))
	{
		return LORD::make_shared<S2CPacketSpawnExpOrb>((EntityXPOrb*)myEntity);
	}
	else if (myEntity->isClass(ENTITY_CLASS_SNOWBALL))
	{
		//return LORD::make_shared<S2CPacketSpawnExpOrb>((EntityXPOrb*)myEntity);

		Entity* shootingEntity = ((EntitySnowball*)myEntity)->getThrower();
		int meta = ((EntitySnowball*)myEntity)->getMeta();
		//LordLogInfo("SpawnSnowBall, entityId[%d],x[%f] y[%f] z[%f] motion.x[%f] motion.y[%f] motion.z[%f] yaw[%f] pitch[%f] onFire[%s]",
        //			myEntity->entityId, myEntity->position.x, myEntity->position.y, myEntity->position.z,
        //			myEntity->motion.x, myEntity->motion.y, myEntity->motion.z, myEntity->rotationYaw, myEntity->rotationPitch, myEntity->isBurning() ? "true" : "false");

		return LORD::make_shared<S2CEntitySpawnCommonPacket>(
				myEntity,
				ENTITY_CREATE_TYPE_SNOWBALL,
				shootingEntity ? shootingEntity->entityId : myEntity->entityId,
				nullptr,
				meta);
	}
	else if (myEntity->isClass(ENTITY_CLASS_GRENADE))
	{
		Entity* shootingEntity = ((EntityGrenade*)myEntity)->getThrower();
		auto packet = LORD::make_shared<S2CEntitySpawnCommonPacket>(myEntity, ENTITY_CREATE_TYPE_GRENADE, shootingEntity ? shootingEntity->entityId : myEntity->entityId);
		packet->m_itemID = ((EntityGrenade*)myEntity)->getItemId();
		return packet;
	}
	else if (myEntity->isClass(ENTITY_CLASS_TNT_THROWABLE))
	{
		//return LORD::make_shared<S2CPacketSpawnExpOrb>((EntityXPOrb*)myEntity);

		Entity* shootingEntity = ((EntityTNTThrowable*)myEntity)->getThrower();
		//LordLogInfo("SpawnTNTThrowable, entityId[%d],x[%f] y[%f] z[%f] motion.x[%f] motion.y[%f] motion.z[%f] yaw[%f] pitch[%f] onFire[%s]",
        //			myEntity->entityId, myEntity->position.x, myEntity->position.y, myEntity->position.z,
        //			myEntity->motion.x, myEntity->motion.y, myEntity->motion.z, myEntity->rotationYaw, myEntity->rotationPitch, myEntity->isBurning() ? "true" : "false");

		return LORD::make_shared<S2CEntitySpawnCommonPacket>(myEntity, ENTITY_CREATE_TYPE_TNT_THROWABLE, shootingEntity ? shootingEntity->entityId : myEntity->entityId);
	}
	else if (myEntity->isClass(ENTITY_CLASS_FIRE_BALL))
	{
		Entity* shootingEntity = ((EntityFireball*)myEntity)->getThrower();
		return LORD::make_shared<S2CEntitySpawnCommonPacket>(myEntity, ENTITY_CREATE_TYPE_FIREBALL, shootingEntity ? shootingEntity->entityId : myEntity->entityId);
	}
	else if (myEntity->isClass(ENTITY_CLASS_MERCHANT))
	{
		return LORD::make_shared<S2CEntitySpawnCommonPacket>(myEntity, ENTITY_CREATE_TYPE_MERCHANT);
	}
	else if (myEntity->isClass(ENTITY_CLASS_RANK_NPC))
	{
		return LORD::make_shared<S2CEntitySpawnCommonPacket>(myEntity, ENTITY_CREATE_TYPE_RANK_NPC);
	}
	else if (myEntity->isClass(ENTITY_CLASS_AIRCRAFT))
	{
		EntityAircraft* pAircraft = (EntityAircraft*)myEntity;
		return LORD::make_shared<S2CEntitySpawnCommonPacket>(myEntity, ENTITY_CREATE_TYPE_AIRCRAFT, pAircraft->getAircraftId());
	}
	else if (myEntity->isClass(ENTITY_CLASS_ACTOR_NPC)) 
	{
		return LORD::make_shared<S2CEntitySpawnCommonPacket>(myEntity, ENTITY_CREATE_TYPE_ACTOR_NPC);
	}
	else if (myEntity->isClass(ENTITY_CLASS_SESSION_NPC))
	{
		return LORD::make_shared<S2CEntitySpawnCommonPacket>(myEntity, ENTITY_CREATE_TYPE_SESSION_NPC);
	}
	else if (myEntity->isClass(ENTITY_CLASS_CREATUREAI))
	{
		EntityCreatureAI* pAi = (EntityCreatureAI*)myEntity;
		return LORD::make_shared<S2CEntitySpawnCommonPacket>(myEntity, ENTITY_CREATE_TYPE_CREATURE, pAi->m_MonsterId);
	}
	else if (myEntity->isClass(ENTITY_CLASS_CREATURE_BULLET))
	{
		EntityCreatureBullet* pBullet = (EntityCreatureBullet*)myEntity;
		auto packet = LORD::make_shared<S2CPacketSpawnCreatureBullet>();
		packet->entityId = pBullet->hashCode();
		packet->type = (int)pBullet->getType();
		packet->throwerId = pBullet->getThrowerId();
		packet->targeterId = pBullet->getTargeterId();
		return packet;
	}
	else if (myEntity->isClass(ENTITY_CLASS_ITEM_SKILL))
	{
		EntitySkillThrowable* pSkillThrowable = (EntitySkillThrowable*)myEntity;
		if (pSkillThrowable)
		{
			auto packet = LORD::make_shared<S2CPacketSpwanSkillItemThrowable>();
			packet->throwerId = pSkillThrowable->getThrowerId();
			packet->skillId = pSkillThrowable->getSkillId();
			packet->motion = pSkillThrowable->motion;
			return packet;
		}
	}
	else if (myEntity->isClass(ENTITY_CLASS_BLOCKMAN_EMPTY))
	{
		auto var9 = dynamic_cast<EntityBlockmanEmpty*>(myEntity);
		return LORD::make_shared<S2CEntitySpawnCommonPacket>(myEntity, ENTITY_CREATE_TYPE_BLOCKMAN_EMPTY, (int) var9->getBlockmanType());
	}
	else if (myEntity->isClass(ENTITY_CLASS_ACTOR_CANNON))
	{
		EntityActorCannon* pCannon = (EntityActorCannon*)myEntity;
		if (pCannon)
		{
			return LORD::make_shared<S2CPacketSpawnCannon>(pCannon);
		}
	}
	else if (myEntity->isClass(ENTITY_CLASS_BUILD_NPC))
	{
		return LORD::make_shared<S2CEntitySpawnCommonPacket>(myEntity, ENTITY_CREATE_TYPE_BUILD_NPC);
	}
	else if (myEntity->isClass(ENTITY_CLASS_LAND_NPC))
	{
		return LORD::make_shared<S2CEntitySpawnCommonPacket>(myEntity, ENTITY_CREATE_TYPE_LAND_NPC);
	}
	else if (myEntity->isClass(ENTITY_CLASS_BULLETIN))
	{
		return LORD::make_shared<S2CEntitySpawnCommonPacket>(myEntity, ENTITY_CREATE_TYPE_BULLETIN);
	}
	else if (myEntity->isClass(ENTITY_CLASS_BIRDAI))
	{
		return LORD::make_shared<S2CEntitySpawnCommonPacket>(myEntity, ENTITY_CREATE_TYPE_BIRD);
	}
	else
	{
		// throw new IllegalArgumentException("Don\'t know how to add " + myEntity.getClass() + "!");
	}

	return LORD::make_shared<S2CPacketSpawnMob>((EntityLivingBase*)myEntity);
}

void ServerPacketSender::sendPlayerAction(ui64 rakssId, int playerId, NETWORK_DEFINE::PacketPlayerActionType action)
{
	auto packet = LORD::make_shared<S2CPacketPlayerAction>();
	packet->playerId = playerId;
	packet->action = action;

	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}


std::shared_ptr<DataPacket> ServerPacketSender::constructPlaySoundPacket(uint soundType, i32 entityId)
{
	auto packet = LORD::make_shared<S2CPacketPlaySound>();
	packet->m_soundType = soundType;
	packet->m_entityId = entityId;

	return packet;
}

void ServerPacketSender::sendPlaySound(BLOCKMAN::EntityPlayer* player,uint soundType)
{
	ui64   rakssId =  dynamic_cast<EntityPlayerMP*>(player)->getRaknetID();
	auto packet = constructPlaySoundPacket(soundType);
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendPlaySoundByType(ui64 rakssId, uint soundType)
{
	auto packet = constructPlaySoundPacket(soundType);
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::broadCastPlaySoundByType(uint soundType)
{
	auto packet = constructPlaySoundPacket(soundType);
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::broadCastPlaySoundByTypeAtEntity(uint soundType, i32 entityId)
{
	auto packet = constructPlaySoundPacket(soundType, entityId);
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendAllSetSignTexts(ui64 rakssId)
{
	auto packet = LORD::make_shared<S2CPacketSetSignTexts>();
	for (auto tileEntity : Server::Instance()->getWorld()->getLoadedTileEntities())
	{
		if (auto tileEntitySign = dynamic_cast<TileEntitySign*>(tileEntity))
		{
			S2CPacketSetSignTexts::SignData data;
			data.position = tileEntitySign->m_pos;
			for (size_t i = 0; i < BLOCKMAN::TileEntitySign::SIGN_COUNT; ++i)
			{
				data.texts[i] = tileEntitySign->getText(i);
			}
			packet->signData.push_back(data);
		}
	}
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::broadcastSetSignTexts(BLOCKMAN::TileEntitySign* tileEntitySign)
{
	auto packet = LORD::make_shared<S2CPacketSetSignTexts>();
	S2CPacketSetSignTexts::SignData data;
	data.position = tileEntitySign->m_pos;
	for (size_t i = 0; i < BLOCKMAN::TileEntitySign::SIGN_COUNT; ++i)
	{
		data.texts[i] = tileEntitySign->getText(i);
	}
	packet->signData.push_back(data);
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendSetSignTexts(BLOCKMAN::TileEntitySign * tileEntitySign, ui64 rakssId)
{
	if (!tileEntitySign) return;

	auto packet = LORD::make_shared<S2CPacketSetSignTexts>();
	S2CPacketSetSignTexts::SignData data;
	data.position = tileEntitySign->m_pos;
	for (size_t i = 0; i < BLOCKMAN::TileEntitySign::SIGN_COUNT; ++i)
	{
		data.texts[i] = tileEntitySign->getText(i);
	}
	packet->signData.push_back(data);
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendChangePlayerInfo(ui64 rakssId, BLOCKMAN::EntityPlayer* player)
{
	auto packet = LORD::make_shared<S2CPacketChangePlayerInfo>();
	packet->m_playerId = player->entityId;
	packet->m_playerShowName = player->getShowName();
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::broadCastChangePlayerInfo(BLOCKMAN::EntityPlayer * player)
{
	auto packet = LORD::make_shared<S2CPacketChangePlayerInfo>();
	packet->m_playerId = player->entityId;
	packet->m_playerShowName = player->getShowName();
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendUpdateMerchant(ui64 rakssid, int entityId, int index)
{
	auto merchant = dynamic_cast<EntityMerchant*>(Server::Instance()->getWorld()->getEntity(entityId));
	if (merchant)
	{
		auto cList = CommodityManager::Instance()->getCommodityList(index);
		if (!cList)
			return;
		auto packet = LORD::make_shared<S2CPacketUpdateMerchant>();
		packet->id = merchant->entityId;
		packet->nameLang = merchant->getNameLang();
		packet->name = merchant->getName();
		packet->commodities = cList->getCommodities();
		Server::Instance()->getNetwork()->sendPacket(packet, rakssid);
	}
}

void ServerPacketSender::sendUpdateMerchantInfo(ui64 rakssid, int entityId)
{
	auto merchant = dynamic_cast<EntityMerchant*>(Server::Instance()->getWorld()->getEntity(entityId));
	if (merchant)
	{
		auto packet = LORD::make_shared<S2CPacketUpdateMerchantInfo>();
		packet->id = merchant->entityId;
		packet->nameLang = merchant->getNameLang();
		packet->name = merchant->getName();
		Server::Instance()->getNetwork()->sendPacket(packet, rakssid);
	}
}

void ServerPacketSender::sendUpdateMerchantCommodities(ui64 rakssid, int entityId, int index)
{
	auto merchant = dynamic_cast<EntityMerchant*>(Server::Instance()->getWorld()->getEntity(entityId));
	if (merchant)
	{
		auto packet = LORD::make_shared<S2CPacketUpdateMerchantCommodities>();
		packet->id = merchant->entityId;
		auto cList = CommodityManager::Instance()->getCommodityList(index);
		if (!cList)
			return;
		packet->commodities = cList->getCommodities();
		Server::Instance()->getNetwork()->sendPacket(packet, rakssid);
	}
}

void ServerPacketSender::sendSetFlying(ui64 rakssId, BLOCKMAN::EntityPlayer * player)
{
	auto packet = LORD::make_shared<S2CPacketSetFlying>();
	packet->playerId = player->entityId;
	packet->isFlying = player->capabilities.isFlying;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendSetFlyingToTrackingPlayers(BLOCKMAN::EntityPlayer * player)
{
	auto packet = LORD::make_shared<S2CPacketSetFlying>();
	packet->playerId = player->entityId;
	packet->isFlying = player->capabilities.isFlying;
	sendPacketToTrackingPlayers(player->entityId, packet);
}

void ServerPacketSender::sendExplosion(BLOCKMAN::EntityPlayer* player, const Vector3& pos, float size, const BLOCKMAN::BlockPosArr& poss, const Vector3& motion, bool isSmoke, float custom_size)
{
	ui64   rakssId = dynamic_cast<EntityPlayerMP*>(player)->getRaknetID();
	auto packet = LORD::make_shared<S2CPacketExplosion>();
	packet->m_size = size;
	packet->m_pos = pos;
	packet->m_motion = motion;
	packet->m_customsize = custom_size;
	packet->m_smoke = isSmoke;
	for (auto p : poss)
	{
		packet->m_poss.push_back(p);
	}
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendPlayerActionToTrackingPlayers(NETWORK_DEFINE::PacketPlayerActionType action, int playerId, bool includeSelf/* = false*/)
{
	auto packet = LORD::make_shared<S2CPacketPlayerAction>();
	packet->playerId = playerId;
	packet->action = action;

	sendPacketToTrackingPlayers(playerId, packet, includeSelf);
}

void ServerPacketSender::sendSystemsChat(String key, String playerName, int chatType)
{
	auto packet = LORD::make_shared<S2CPacketChatTranslation>();
	auto chatComponentTranslation = LORD::make_shared<ChatComponentTranslation>();
	FormatArgs args = FormatArgs(playerName);
	chatComponentTranslation->key = key;
	chatComponentTranslation->formatArgs.push_back(args);
	packet->m_chatType = chatType;
	packet->m_chatComponent = chatComponentTranslation;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendChat(std::shared_ptr<ChatComponentString> chatString)
{
	auto packet = LORD::make_shared<S2CPacketChatString>();
	packet->m_chatType = 1;
	packet->m_chatComponent = chatString;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendChat(ui64 targetRaknetID, std::shared_ptr<ChatComponentString> chatString)
{
	auto packet = LORD::make_shared<S2CPacketChatString>();
	packet->m_chatType = 1;
	packet->m_chatComponent = chatString;
	Server::Instance()->getNetwork()->sendPacket(packet, targetRaknetID, true);
}

void ServerPacketSender::sendChat(std::shared_ptr<ChatComponentTranslation> chatTranslation)
{
	auto packet = LORD::make_shared<S2CPacketChatTranslation>();
	packet->m_chatType = 0;
	packet->m_chatComponent = chatTranslation;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendChat(ui64 targetRaknetID, std::shared_ptr<ChatComponentTranslation> chatTranslation)
{
	auto packet = LORD::make_shared<S2CPacketChatTranslation>();
	packet->m_chatType = 0;
	packet->m_chatComponent = chatTranslation;
	Server::Instance()->getNetwork()->sendPacket(packet, targetRaknetID, true);
}

void ServerPacketSender::sendDebugString(ui64 rakssId, const String& str)
{
	auto packet = LORD::make_shared<S2CPacketDebugString>();
	packet->debugString = str;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendNotification(ui64 rakssid, std::shared_ptr<ChatComponentNotification> chatNotification)
{
	auto packet = LORD::make_shared<S2CPacketChatNotification>();
	packet->m_chatType = 0;
	packet->m_chatComponent = chatNotification;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssid,  true);
}

void ServerPacketSender::sendNotification(std::shared_ptr<ChatComponentNotification> chatNotification)
{
	auto packet = LORD::make_shared<S2CPacketChatNotification>();
	packet->m_chatType = 0;
	packet->m_chatComponent = chatNotification;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendChestInventory(BLOCKMAN::EntityPlayer* player, Vector3i blockPos, int face, Vector3 hitPos, BLOCKMAN::IInventory* inventory)
{
	auto mp = dynamic_cast<EntityPlayerMP*> (player);
	BLOCKMAN::TileEntityChest* tileChest = dynamic_cast<BLOCKMAN::TileEntityChest*>(inventory);
	if (tileChest != nullptr)
	{
		tileChest->openChestPlayers.push_back(player->entityId);
		SCRIPT_EVENT::ChestOpenEvent::invoke(blockPos, tileChest, mp->getRaknetID());
		mp->setOpenChest(tileChest);
	}

	auto packet = LORD::make_shared<S2CPacketChestInventory>();

	for (int i = 0; i < inventory->getSizeInventory(); ++i)
	{
		if(inventory->getStackInSlot(i))
		{
			packet->chestInventory.push_back({ inventory->getStackInSlot(i), i});
		}
	}
	packet->face = face;
	packet->hitPos = hitPos;
	packet->blockPosX = (i16)blockPos.x;
	packet->blockPosY = (i16)blockPos.y;
	packet->blockPosZ = (i16)blockPos.z;
	packet->entityId = player->entityId;
	LordLogInfo("=========sendChestInventory=========");

	/*int logkk = 0;
	for (auto inv : packet->chestInventory)
	{
		LordLogInfo("-----Inv-slot %d ---Id %d  ---num %d", logkk, inv.id, inv.size);
		logkk++;
	}*/
	Server::Instance()->getNetwork()->sendPacket(packet, mp->getRaknetID(), true);

	sendCloseContainer(player->entityId, false, blockPos);
}

void ServerPacketSender::sendUpdateChestInventory(Vector3i blockPos, int face, Vector3 hitPos, BLOCKMAN::IInventory * inventory)
{
	auto packet = LORD::make_shared<S2CPacketUpdateChestInventory>();
	LordLogInfo("=========sendUpdateChestInventory=========");
	for (int i = 0; i < inventory->getSizeInventory(); ++i)
	{
		if (!ItemStack::areItemStacksEqual(inventory->getStackInSlot(i), inventory->getPrevStackInSlot(i)))
		{
			packet->chestInventory.push_back({ inventory->getStackInSlot(i), i});
		}
	}
	packet->face = face;
	packet->hitPos = hitPos;
	packet->blockPosX = (i16)blockPos.x;
	packet->blockPosY = (i16)blockPos.y;
	packet->blockPosZ = (i16)blockPos.z;
	packet->entityId = 0;

	TileEntityChest* chestInventory = dynamic_cast<BLOCKMAN::TileEntityChest*>(inventory);
	if (!chestInventory)
	{
		Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
		return;
	}
	for (auto iter = chestInventory->openChestPlayers.begin(); iter != chestInventory->openChestPlayers.end();)
	{
		EntityPlayerMP* player = dynamic_cast<EntityPlayerMP*>(Server::Instance()->getWorld()->getPlayerEntity((*iter)));
		if (!player)
		{
			iter = chestInventory->openChestPlayers.erase(iter);
			continue;
		}
		Server::Instance()->getNetwork()->sendPacket(packet, player->getRaknetID(), true);
		iter++;
	}
}

void ServerPacketSender::sendUpdateEnderChestInventory(ui64 rakssid, Vector3i blockPos, int face, Vector3 hitPos, BLOCKMAN::IInventory * inventory)
{
	auto packet = LORD::make_shared<S2CPacketUpdateChestInventory>();
	for (int i = 0; i < inventory->getSizeInventory(); ++i)
	{
		if (!ItemStack::areItemStacksEqual(inventory->getStackInSlot(i), inventory->getPrevStackInSlot(i)))
		{
			packet->chestInventory.push_back({ inventory->getStackInSlot(i), i });
		}
	}
	packet->face = face;
	packet->hitPos = hitPos;
	packet->blockPosX = (i16)blockPos.x;
	packet->blockPosY = (i16)blockPos.y;
	packet->blockPosZ = (i16)blockPos.z;
	packet->entityId = 0;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssid, true);
}

void ServerPacketSender::sendUsePropResults(ui64 rakssId, String propId, int results)
{
	auto packet = LORD::make_shared<S2CPacketUsePropResults>();
	packet->m_propId = propId;
	packet->m_results = results;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);

}

void ServerPacketSender::sendUserIn(ui64 rakssId, ui64 platformUserId, String nickName, int teamId, String teamName, int curPlayer, int maxPlayer)
{
	auto packet = LORD::make_shared<S2CPacketUserIn>();
	packet->m_nickName = nickName;
	packet->m_platformUserId = platformUserId;
	packet->m_teamId = teamId;
	packet->m_teamName = teamName;
	packet->m_curPlayer = curPlayer;
	packet->m_maxPlayer = maxPlayer;

	if (rakssId == 0)
	{
		Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
	}
	else
	{
		Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
	}

}

void ServerPacketSender::sendUserOut(ui64 platformUserId)
{
	auto packet = LORD::make_shared<S2CPacketUserOut>();
	packet->m_platformUserId = platformUserId;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendLoginResult(ui64 rakssId, int32_t resultCode)
{
	auto packet = LORD::make_shared<S2CPacketLoginResult>();
	packet->m_resultCode = resultCode;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendPing(ui64 rakssId, ui32 sendTime)
{
	auto packet = LORD::make_shared<S2CPacketPing>();
	packet->m_sendTime = sendTime;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendSettlement(ui64 rakssId, String gameType, String gameResult)
{
	auto packet = LORD::make_shared<S2CPacketSettlement>();
	packet->m_gameType = gameType;
	packet->m_gameResult = gameResult;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendGameover(ui64 rakssId , String overMessage, int code)
{
	auto packet = LORD::make_shared<S2CPacketGameover>();
	packet->m_overMessage = overMessage;
	packet->m_code = code;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendCloseContainer(int playerID, bool close, const Vector3i& containerPos)
{
	auto packet = LORD::make_shared<S2CPacketCloseContainer>();
	packet->m_close = close;
	packet->m_playerID = playerID;
	packet->m_containerPosX = (i16)containerPos.x;
	packet->m_containerPosY = (i16)containerPos.y;
	packet->m_containerPosZ = (i16)containerPos.z;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendSetAllowFlying(ui64 rakssId, bool allowFlying)
{
	auto packet = LORD::make_shared<S2CPacketSetAllowFlying>();
	packet->allowFlying = allowFlying;

	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendGameStatus(ui64 rakssId, int status)
{
	auto packet = LORD::make_shared<S2CPacketGameStatus>();
	packet->m_status = status;
	if (rakssId == 0)
	{
		Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
	}
	else
	{
		Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
	}
}


void ServerPacketSender::sendSetWatchMode(ui64 rakssId, int entityId, bool isWatchMode)
{
	auto packet = LORD::make_shared<S2CPacketSetWatchMode>();
	packet->playerId = entityId;
	packet->isWatchMode = isWatchMode;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendSetWatchModeToTrackingPlayers(int entityId, bool isWatchMode)
{
	auto packet = LORD::make_shared<S2CPacketSetWatchMode>();
	packet->playerId = entityId;
	packet->isWatchMode = isWatchMode;
	sendPacketToTrackingPlayers(entityId, packet);
}

void ServerPacketSender::sendPlayerSettlement(ui64 rakssId, String result, bool8 isNextServer)
{
	auto packet = LORD::make_shared<S2CPacketPlayerSettlement>();
	packet->m_result = result;
	packet->m_isNextServer = isNextServer;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendGameSettlement(ui64 rakssId, String result, bool8 isNextServer)
{
	auto packet = LORD::make_shared<S2CPacketGameSettlement>();
	packet->m_result = result;
	packet->m_isNextServer = isNextServer;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::broadCastPlayerLifeStatus(ui64 platformUserId, bool isLife)
{
	auto packet = LORD::make_shared<S2CPacketPlayerLifeStatus>();
	packet->m_platformUserId = platformUserId;
	packet->m_isLife = isLife;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::broadCastBedDestroy(int teamId)
{
	auto packet = LORD::make_shared<S2CPacketBedDestroy>();
	packet->m_teamId = teamId;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendRespawnCountdown(ui64 rakssId, ui32 second)
{
	auto packet = LORD::make_shared<S2CPacketRespawnCountdown>();
	packet->m_second = second;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::broadAllPlayerTeamInfo(String result)
{
	auto packet = LORD::make_shared<S2CPacketAllPlayerTeamInfo>();
	packet->m_result = result;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendChangePlayerTeam(ui64 rakssId, ui64 platformUserId, int teamId)
{
	auto packet = LORD::make_shared<S2CPacketChangePlayerTeam>();
	packet->m_platformUserId = platformUserId;
	packet->m_teamId = teamId;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::broadCastChangePlayerTeam(ui64 platformUserId, int teamId)
{
	auto packet = LORD::make_shared<S2CPacketChangePlayerTeam>();
	packet->m_platformUserId = platformUserId;
	packet->m_teamId = teamId;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendPlayerChangeActor(ui64 rakssId, const BLOCKMAN::EntityPlayerMP* pPlayer)
{
	auto packet = LORD::make_shared<S2CPacketPlayerChangeActor>();
	packet->m_entityId = pPlayer->entityId;
	packet->m_actorName = pPlayer->m_actorName;
	packet->m_bodyId = pPlayer->m_actorBodyId;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::syncChangePlayerActorInfo(ui64 rakssId, int changeActortCount, int needMoneyCount)
{
	auto packet = LORD::make_shared<S2CSyncChangePlayerActorInfo>();
	packet->changeActorCount = changeActortCount;
	packet->needMoneyCount = needMoneyCount;
	packet->actorName = "";
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendNpcActorInfo(i64 rakssId, EntityActorNpc* pNpc)
{
	auto packet = LORD::make_shared<S2CSyncNpcActorInfo>();
	packet->entityId = pNpc->entityId;
	packet->actorName = pNpc->getActorName();
	packet->headName = pNpc->getHeadName();
	packet->skillName = pNpc->getSkillName();
	packet->haloEffectName = pNpc->getHaloEffectName();
	packet->isCanObstruct = pNpc->isCanObstruct();
	packet->content = pNpc->getContent();
	packet->isCanCollided = pNpc->canBeCollidedWith();
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::broadCastPlayerChangeActor(EntityPlayer* pPlayer)
{
	auto packet = LORD::make_shared<S2CPacketPlayerChangeActor>();
	packet->m_entityId = pPlayer->entityId;
	packet->m_actorName = pPlayer->m_actorName;
	packet->m_bodyId = pPlayer->m_actorBodyId;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::broadCastPlayerRestoreActor(int entityId)
{
	auto packet = LORD::make_shared<S2CPacketPlayerRestoreActor>();
	packet->m_entityId = entityId;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendUpdateShop(ui64 rakssId)
{
	Shop* shop = Server::Instance()->getWorld()->getShop();
	if (shop)
	{
		auto packet = LORD::make_shared<S2CPacketUpdateShop>();
		packet->m_goods = shop->getGoods();
		packet->m_respawnGoods = shop->getRespawnGoods();
		Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
	}
}

void ServerPacketSender::broadCastUpdateShop()
{
	Shop* shop = Server::Instance()->getWorld()->getShop();
	if (shop)
	{
		auto packet = LORD::make_shared<S2CPacketUpdateShop>();
		packet->m_goods = shop->getGoods();
		packet->m_respawnGoods = shop->getRespawnGoods();
		Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
	}
}

void ServerPacketSender::broadCastUpdateShopGoods(int groupIndex, int goodsIndex, int limit)
{
	auto packet = LORD::make_shared<S2CPacketUpdateShopGoods>();
	packet->groupIndex = groupIndex;
	packet->goodsIndex = goodsIndex;
	packet->limit = limit;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendShoppingResult(ui64 rakssId, int code, String result)
{
	auto packet = LORD::make_shared<S2CPacketShoppingResult>();
	packet->code = code;
	packet->result = result;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendInitMoney(ui64 rakssId, ui64 diamonds, ui64 golds)
{
	auto packet = LORD::make_shared<S2CPacketInitMoney>();
	packet->diamonds = diamonds;
	packet->golds = golds;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendReloadBullet(ui64 rakssId)
{
	auto packet = LORD::make_shared<S2CPacketReloadBullet>();
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::broadCastPlayerGunGire(i32 entityId, i32 gunId, const C2SPACKET_DETAIL::Vector3& pos, const C2SPACKET_DETAIL::Vector3& dir,
	ui32 tracyType, const C2SPACKET_DETAIL::Vector3i& blockPos, const C2SPACKET_DETAIL::Vector3& hitpos, ui32 hitEntityID, bool isHeadshot)
{
	auto s2cPacket = LORD::make_shared<S2CPacketSyncGunFireResult>();
	s2cPacket->sourceEntityId = entityId;
	s2cPacket->gunId = gunId;
	s2cPacket->pos = pos;
	s2cPacket->dir = dir;
	s2cPacket->tracyType = tracyType;
	s2cPacket->blockPos = blockPos;
	s2cPacket->hitpos = hitpos;
	s2cPacket->hitEntityID = hitEntityID;
	s2cPacket->isHeadshot = isHeadshot;
	sendPacketToTrackingPlayers(entityId, s2cPacket, true);
}

void ServerPacketSender::broadCastSyncTakeVehicle(int playerId, bool onoff, bool isDriver, int vehicleId)
{
	auto packet = LORD::make_shared<S2CPacketSyncTakeVehicle>();

	packet->playerId = playerId;
	packet->onoff = onoff;
	packet->vehicleId = vehicleId;
	packet->isDriver = isDriver;
	sendPacketToTrackingPlayers(playerId, packet, true);
}

void ServerPacketSender::broadCastChangePlayerMaxHealth(i32 entityId, float health)
{
	auto packet = LORD::make_shared<S2CPacketChangeMaxHealth>();
	packet->entityId = entityId;
	packet->health = health;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendChangePlayerMaxHealth(ui64 rakssId, i32 entityId, float health)
{
	auto packet = LORD::make_shared<S2CPacketChangeMaxHealth>();
	packet->entityId = entityId;
	packet->health = health;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendPlayerUpdateCurrency(ui64 rakssId, ui64 currency)
{
	auto packet = LORD::make_shared<S2CPacketUpdateCurrency>();
	packet->currency = currency;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendSelectRoleData(ui64 rakssId, String result)
{
	auto packet = LORD::make_shared<S2CPacketSelectRoleData>();
	packet->result = result;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendPlayerOwnVehicle(ui64 rakssId, const std::set<int>& ownVehicle)
{
	auto packet = LORD::make_shared<S2CPacketSyncPlayerOwnVehicle>();
	for (auto iter = ownVehicle.cbegin(); iter != ownVehicle.cend(); iter++)
	{
		packet->m_vehicles.push_back(*iter);
	}

	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::broadCastSyncLockVehicle(int playerId, bool onoff, int vehicleId)
{
	auto packet = LORD::make_shared<S2CPacketSyncLockVehicle>();

	packet->playerId = playerId;
	packet->onoff = onoff;
	packet->vehicleId = vehicleId;
	sendPacketToTrackingPlayers(playerId, packet, true);
}

void ServerPacketSender::sendSyncVehicleState(ui64 rakssId, int entityId)
{
	auto vehicle = dynamic_cast<EntityVehicle*>(Server::Instance()->getWorld()->getEntity(entityId));
	if (!vehicle)
		return;

	auto packet = LORD::make_shared<S2CPacketSyncVehicleState>();
	packet->entityId = entityId;
	packet->currentVelocity = vehicle->m_currentVelocity;
	packet->isLocked = vehicle->m_bisLocked;
	packet->hasDriver = vehicle->m_hasDriver;
	packet->state = (int)vehicle->m_curState;
	packet->driverId = vehicle->m_driverEntityId;

	for (int passengerId : vehicle->m_passengerEntityIdList)
	{
		packet->m_passengers.push_back(passengerId);
	}

	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::syncPlayerTakeOnVehicle(ui64 rakssId, int playerId, bool isDriver, int vehicleId)
{
	auto packet = LORD::make_shared<S2CPacketSyncTakeVehicle>();

	packet->playerId = playerId;
	packet->onoff = true;
	packet->vehicleId = vehicleId;
	packet->isDriver = isDriver;

	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::broadCastVehicleActionState(int playerId, int vehicleId, int state)
{
	auto packet = LORD::make_shared<S2CPacketSyncVehicleActionState>();

	packet->entityId = vehicleId;
	packet->state = state;
	sendPacketToTrackingPlayers(playerId, packet, true);
}

void ServerPacketSender::notifyGetMoney(ui64 rakssId, int count)
{
	auto packet = LORD::make_shared<S2CPacketNotifyGetMoney>();
	packet->count = count;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::notifyGetItem(ui64 rakssId, int itemId, int meta, int count)
{
	auto packet = LORD::make_shared<S2CPacketNotifyGetItem>();
	packet->count = count;
	packet->itemId = itemId;
	packet->meta = meta;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendBuyCommodityResult(ui64 rakssId, int code, String result)
{
	auto packet = LORD::make_shared<S2CPacketBuyCommodityResult>();
	packet->code = code;
	packet->result = result;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendShowBuyRespawn(ui64 rakssId, i32 times)
{
	auto packet = LORD::make_shared<S2CPacketShowBuyRespawn>();
	packet->times = times;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendBuyRespawnResult(ui64 rakssId, i32 code, String result)
{
	auto packet = LORD::make_shared<S2CPacketBuyRespawnResult>();
	packet->code = code;
	packet->result = result;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendShowGoNpcMerchant(ui64 rakssId, i32 entityId, float x, float y, float z, float yaw)
{
	auto packet = LORD::make_shared<S2CPacketShowGoNpcMerchant>();
	packet->x = x;
	packet->y = y;
	packet->z = z;
	packet->entityId = entityId;
	packet->yaw = yaw;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendUpdateBackpack(ui64 rakssId, int capacity, int maxCapacity)
{
	auto packet = LORD::make_shared<S2CPacketUpdateBackpack>();
	packet->capacity = capacity;
	packet->maxCapacity = maxCapacity;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendRankData(ui64 rakssId, i32 entityId, String result)
{
	auto packet = LORD::make_shared<S2CPacketRankData>();
	packet->entityId = entityId;
	packet->result = result;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendUpdateRankNpc(ui64 rakssId, int entityId)
{
	auto entityRank = dynamic_cast<EntityRankNpc*>(Server::Instance()->getWorld()->getEntity(entityId));
	if (entityRank)
	{
		auto packet = LORD::make_shared<S2CPacketUpdateRankNpc>();
		packet->id = entityRank->entityId;
		packet->nameLang = entityRank->getNameLang();
		packet->name = entityRank->getName();
		packet->rankInfo = entityRank->getRankInfo();
		Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
	}

}

void ServerPacketSender::sendActivateTrigger(ui64 rakssId, const Vector3i & blockPos, int blockId, int triggerIndex)
{
	auto packet = LORD::make_shared<S2CPacketActivateTrigger>();
	packet->blockPosX = (i16)blockPos.x;
	packet->blockPosY = (i16)blockPos.y;
	packet->blockPosZ = (i16)blockPos.z;
	packet->blockId = blockId;
	packet->triggerIndex = triggerIndex;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::broadCastSyncBlock()
{
	// FIX [SYMPTOM-4]: getBlockSyncList() returns a reference; `auto positions`
	// (by value) copied the whole list each call and positions.clear() below
	// cleared the COPY — the real list grew without bound and every broadcast
	// resent the full edit history to every player.
	auto& positions = BlockChangeRecorderServer::Instance()->getBlockSyncList();

	EntityPlayers& players = Server::Instance()->getWorld()->getPlayers();

	for (EntityPlayers::iterator it = players.begin(); it != players.end(); ++it)
	{
		EntityPlayerMP* pPlayer = dynamic_cast<EntityPlayerMP*>(it->second);
		if (!pPlayer)
			continue;

		pPlayer->addUpdateBlocks(positions);
	}

	positions.clear();
}

void ServerPacketSender::broadcastTakeAircraft(int playerId, bool onoff, int aircraftId)
{
	auto packet = LORD::make_shared<S2CPacketTakeAircraft>();
	packet->passengerId = playerId;
	packet->entityId = aircraftId;
	packet->isOn = onoff;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::syncPlayerTakeAircraft(ui64 rakssId, int playerId, int aircraftId)
{
	auto packet = LORD::make_shared<S2CPacketTakeAircraft>();
	packet->passengerId = playerId;
	packet->entityId = aircraftId;
	packet->isOn = true;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::broadCastAircraftStartFly(int entityId)
{
	auto aircraft = dynamic_cast<EntityAircraft*>(Server::Instance()->getWorld()->getEntity(entityId));
	if (!aircraft)
		return;

	auto packet = LORD::make_shared<S2CPacketAircraftStartFly>();
	packet->entityId = entityId;
	packet->speed = aircraft->getSpeed();
	packet->startPos = aircraft->getAirLineStartPosition();
	packet->endPos = aircraft->getAirLineEndPosition();
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::syncAircraftState(ui64 rakssId, int entityId)
{
	auto aircraft = dynamic_cast<EntityAircraft*>(Server::Instance()->getWorld()->getEntity(entityId));
	if (!aircraft)
		return;

	auto packet = LORD::make_shared<S2CPacketSyncAircraftState>();
	packet->entityId = entityId;
	packet->isFlying = aircraft->isAircraftFlying();
	packet->speed = aircraft->getSpeed();
	packet->flyingTick = aircraft->getFlyingTick();
	packet->startPos = aircraft->getAirLineStartPosition();
	packet->endPos = aircraft->getAirLineEndPosition();
	auto passengers = aircraft->getPassengers();
	for (auto it = passengers.cbegin(); it != passengers.cend(); it++)
	{
		packet->m_passengers.push_back(*it);
	}

	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::sendChangeAircraftUI(ui64 rakssId, bool isShowUI)
{
	auto packet = LORD::make_shared<S2CPacketChangeAircraftUI>();
	packet->isShowUI = isShowUI;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::broadCastChangeAircraftUI(bool isShowUI)
{
	auto packet = LORD::make_shared<S2CPacketChangeAircraftUI>();
	packet->isShowUI = isShowUI;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendAirDropPosition(Vector3 position)
{
	auto packet = LORD::make_shared<S2CPacketAirdrop>();
	packet->m_airDropPos = position;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::broadCastPoisonCircleRange(vector<Vector2>::type range, float speed)
{
	auto packet = LORD::make_shared<S2CPacketPoisonCircleRange>();
	packet->minSafeRange = Vector3(range[0].x, 0, range[0].y);
	packet->maxSafeRange = Vector3(range[1].x, 0, range[1].y);
	packet->minPoisonRange = Vector3(range[2].x, 0, range[2].y);
	packet->maxPoisonRange = Vector3(range[3].x, 0, range[3].y);
	packet->speed = speed;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendMemberLeftAndKill(ui64 rakssId, int left, int kill)
{
	auto packet = LORD::make_shared<S2CPacketMemberLeftAndKill>();
	packet->m_left = left;
	packet->m_kill = kill;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendBlockDestroyeFailure(ui64 rakssId, bool destroyeStatus, int destroyeFailureType)
{
	auto packet = LORD::make_shared<S2CPacketSyncBlockDestroyeFailure>();
	packet->destroyeStatus = destroyeStatus;
	packet->destroyeFailureType = destroyeFailureType;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendSkillType(ui64 rakssId, int skillType, float duration, float coldDownTime)
{
	auto packet = LORD::make_shared<S2CPacketSkillType>();
	packet->skillType = skillType;
	packet->duration = duration;
	packet->coldDownTime = coldDownTime;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendShowHideAndSeekBtnStatus(i64 rakssId, bool isActoBtnStatus, bool isCameraBtnStatus, bool isOtherBtnStatus)
{
	auto packet = LORD::make_shared<S2CPacketHideAndSeekBtnStatus>();
	packet->isShowChangeActorBtn = isActoBtnStatus;
	packet->isShowChangeCameraBtn = isCameraBtnStatus;
	packet->isShowOtherBtn = isOtherBtnStatus;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendPickUpItemOrder(ui64 rakssId, i32 itemEntityId, i32 itemId, i32 pickUpPrice, i32 moneyType)
{
	auto packet = LORD::make_shared<S2CPacketPickUpItemOrder>();
	packet->itemId = itemId;
	packet->pickUpPrice = pickUpPrice;
	packet->moneyType = moneyType;
	packet->itemEntityId = itemEntityId;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendRequestResult(ui64 rakssId, i32 itemId, BLOCKMAN::ResultCode resultCode, String resultMessage)
{
	auto packet = LORD::make_shared<S2CPacketRequestResult>();
	packet->itemId = itemId;
	packet->resultCode = (int)resultCode;
	packet->resultMessage = resultMessage;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendSyncGameTimeShowUi(ui64 rakssId, bool isShowUI,int time) 
{
	auto packet = LORD::make_shared<S2CPacketSyncGameTimeShowUi>();
	packet->isShowUI = isShowUI;
	packet->time = time;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::broadCastGameTimeShowUi(bool isShowUI, int time)
{
	auto packet = LORD::make_shared<S2CPacketSyncGameTimeShowUi>();
	packet->isShowUI = isShowUI;
	packet->time = time;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);

}

void ServerPacketSender::createArrowTipMark(ui64 rakssId, Vector3 sourcePostion, Vector3 targetPostion, int destoryTime, float distances)
{
	auto packet = LORD::make_shared<S2CPacketSyncCreateArrowTipMark>();
	packet->destoryTime = destoryTime;
	packet->sourcePostion = sourcePostion;
	packet->targetPostion = targetPostion;
	packet->distances = distances;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::broadCastCameraLockToClient(bool isCameraLock, int entityId)
{
	auto packet = LORD::make_shared<S2CPacketSyncCameraLock>();
	packet->entityId = entityId;
	packet->isCameraLock = isCameraLock;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::SyncCameraLockStatus(ui64 rakssId, int playerId, bool isCameraLock)
{
	auto packet = LORD::make_shared<S2CPacketSyncCameraLock>();
	packet->entityId = playerId;
	packet->isCameraLock = isCameraLock;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendDeathUnequipArmor(i32 entityId)
{
	auto packet = LORD::make_shared<S2CPacketDeathUnequipArmor>();
	packet->entityId = entityId;
	EntityPlayerMP* player = dynamic_cast<EntityPlayerMP*>(Server::Instance()->getWorld()->getPlayerEntity(entityId));
	if (player)
	{
		player->inventory->clearPrev();
	}
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendChangeEntityPerspece(ui64 rakssId, int View)
{
	auto packet = LORD::make_shared<S2CPacketSyncChangeEntityPerspece>();
	packet->View = View;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendPlayerInvisible(ui64 rakssId, i32 entityId, bool isInvisible)
{
	auto packet = LORD::make_shared<S2CPacketPlayerInvisible>();
	packet->isInvisible = isInvisible;
	packet->entityId = entityId;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::broadCastPlayerInvisible(i32 entityId, bool isInvisible)
{
	auto packet = LORD::make_shared<S2CPacketPlayerInvisible>();
	packet->isInvisible = isInvisible;
	packet->entityId = entityId;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendPlayerChangeHeart(ui64 rakssId, int hp, int maxHp)
{
	auto packet = LORD::make_shared<S2CPacketChangeHeart>();
	packet->hp = hp;
	packet->maxHp = maxHp;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendPlayerBossStripWithIcon(ui64 rakssId, String name, int curProgress, int maxProgress, String iconPath)
{
	auto packet = LORD::make_shared<S2CPacketBuildProgress>();
	packet->name = name;
	packet->curProgress = curProgress;
	packet->maxProgress = maxProgress;
	packet->iconPath = iconPath;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendPlayerChangeDefense(ui64 rakssId, int defense, int maxDefense)
{
	auto packet = LORD::make_shared<S2CPacketChangeDefense>();
	packet->defense = defense;
	packet->maxDefense = maxDefense;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendPlayerChangeAttack(ui64 rakssId, int attack, int maxAttack)
{
	auto packet = LORD::make_shared<S2CPacketChangeAttack>();
	packet->attack = attack;
	packet->maxAttack = maxAttack;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::syncSessionNpc(ui64 rakssId, BLOCKMAN::EntitySessionNpc * npc)
{
	auto packet = LORD::make_shared<S2CPacketSyncSessionNpc>();
	auto player = ServerNetwork::Instance()->findPlayerByRakssid(rakssId);
	ui64 userId = npc->isPerson() && player ? player->getPlatformUserId() : 0;
	packet->entityId = npc->entityId;
	packet->isPerson = npc->isPerson();
	packet->sessionType = npc->getSessionType();
	packet->name = npc->getName();
	packet->timeLeft = npc->getTimeLeftByUser(userId);
	packet->nameLang = npc->getNameLangByUser(userId);
	packet->actorName = npc->getActorNameByUser(userId);
	packet->actorBody = npc->getActorBodyByUser(userId);
	packet->actorAction = npc->getActorActionByUser(userId);
	packet->actorBodyId = npc->getActorBodyIdByUser(userId);
	packet->sessionContent = npc->getSessionContentByUser(userId);
	packet->effectName = npc->getEffectNameByUser(userId);
	packet->isCanCollided = npc->isCanCollidedByUser(userId);

	if (rakssId != 0 )
	{
		Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
	}
	else
	{
		sendPacketToTrackingPlayers(npc->entityId, packet);
	}

}

void ServerPacketSender::sendUpdateManor(BLOCKMAN::EntityPlayerMP* player)
{
	Manor* manor = player->m_manor;
	if (manor && manor->getInfo() && manor->getNextInfo())
	{
		auto packet = LORD::make_shared<S2CPacketUpdateManor>();
		packet->m_manorInfo = *manor->getInfo();
		packet->m_nextManorInfo = *manor->getNextInfo();

		for (ManorHouse* item : manor->getHouses())
		{
			packet->m_manorHouses.push_back(*item);
		}

		for (ManorFurniture* item : manor->getFurnitures())
		{
			packet->m_manorFurnitures.push_back(*item);
		}

		for (ManorMessage* item : manor->getMessages())
		{
			packet->m_manorMessages.push_back(*item);
		}

		Server::Instance()->getNetwork()->sendPacket(packet, player->getRaknetID(), true);
	}

}

void ServerPacketSender::sendUpdateRealTimeRankInfo(ui64 rakssId, String result)
{
	auto packet = LORD::make_shared<S2CPacketUpdateRealTimeRankInfo>();
	packet->result = result;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::broadCastGameMonsterInfo(int currCheckPointNum, int surplusMonsterNum, int surplusCheckPointNum)
{
	auto packet = LORD::make_shared<S2CPacketUpdateGameMonsterInfo>();
	packet->currCheckPointNum = currCheckPointNum;
	packet->surplusMonsterNum = surplusMonsterNum;
	packet->surplusCheckPointNum = surplusCheckPointNum;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::broadCastBossBloodStrip(int currBloodStrip, int maxBloodStrip)
{
	auto packet = LORD::make_shared<S2CPacketUpdateBossBloodStrip>();
	packet->currBloodStrip = currBloodStrip;
	packet->maxBloodStrip = maxBloodStrip;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendSwitchablePropsData(ui64 rakssId, String data)
{
	auto packet = LORD::make_shared<S2CPacketSwitchablePropsData>();
	packet->data = data;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendUpgradePropsData(ui64 rakssId, String data)
{
	auto packet = LORD::make_shared<S2CPacketUpgradePropsData>();
	packet->data = data;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendPersonalShopTip(ui64 rakssId, String tip)
{
	auto packet = LORD::make_shared<S2CPacketPersonalShopTip>();
	packet->tip = tip;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendCreateHouseFromSchematic(const String& fileName, Vector3i start_pos, bool xImage, bool zImage, bool createOrDestroy)
{
	auto packet = LORD::make_shared<S2CPacketCreateHouseFromSchematic>();
	packet->start_pos = start_pos;
	packet->fileName = fileName;
	packet->xImage = xImage;
	packet->zImage = zImage;
	packet->createOrDestroy = createOrDestroy;

	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendfillAreaByBlockIdAndMate(Vector3i start_pos, Vector3i end_pos, int block_id, int mate)
{
	auto packet = LORD::make_shared<S2CPacketFillAreaByBlockIdAndMate>();
	packet->start_pos = start_pos;
	packet->end_pos = end_pos;
	packet->block_id = block_id;
	packet->mate = mate;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendShowPlayerOperation(ui64 rakssId, ui64 targetUserId)
{
	auto pree = Server::Instance()->getNetwork()->findPlayerByPlatformUserId(targetUserId);
	auto packet = LORD::make_shared<S2CPacketShowPlayerOperation>();
	packet->m_targetUserId = targetUserId;
	packet->m_targetUserName = pree ? pree->getName() : "";
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendUpdateEntityCreature(i64 rakssId, BLOCKMAN::EntityCreature* creature)
{
	auto packet = LORD::make_shared<S2CPacketSyncEntityCreature>();
	packet->entityId = creature->entityId;
	packet->actorName = creature->getActorName();
	packet->creatureEntityType = creature->getMonsterType();
	packet->nameLang = creature->getNameLang();
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendCreatureEntityAction(int entityId, int actionStatus,int baseAction,int PreBaseAction)
{
	auto packet = LORD::make_shared<S2CPacketSyncCreatureEntityAction>();
	packet->entityId = entityId;
	packet->actionStatus= actionStatus;
	packet->baseAction = baseAction;
	packet->preBaseAction = PreBaseAction;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendPlayerSpeedAdditionLevel(i64 rakssId, int speedLevel)
{
	auto packet = LORD::make_shared<S2CPacketPlayerSpeedLevel>();
	packet->speedLevel = speedLevel;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendPlayerExpInfo(i64 rakssId, int level, float exp, int maxExp)
{
	auto packet = LORD::make_shared<S2CPacketPlayerExpInfo>();
	packet->level = level;
	packet->exp = exp;
	packet->maxExp = maxExp;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendEntitySkillEffect(Vector3 position, String name, float duration, int width, int height, Vector3 color, int density)
{
	auto packet = LORD::make_shared<S2CPacketEntitySkillEffect>();
	packet->position = position;
	packet->name = name;
	packet->duration = duration;
	packet->width = width;
	packet->height = height;
	packet->color = color;
	packet->density = density;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::showUpgradeResourceUI(ui64 rakssId, int costItemId, int costItemNum, int resourceId, String tipStr)
{
	auto packet = LORD::make_shared<S2CPacketShowUpgradeResourceUI>();
	packet->itemId = costItemId;
	packet->itemNum = costItemNum;
	packet->resourceId = resourceId;
	packet->tipString = tipStr;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendManorBtnVisible(ui64 rakssId, bool visible)
{
	auto packet = LORD::make_shared<S2CPacketManorBtnVisible>();
	packet->visible = visible;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendPlayerInventory(int entityId)
{
	EntityPlayerMP* player = dynamic_cast<EntityPlayerMP*>(Server::Instance()->getWorld()->getPlayerEntity(entityId));
	if (player)
	{
		sendPlayerInventory(player->getRaknetID(), player->getInventory());
	}
}

void ServerPacketSender::sendCallOnManorResetClient(ui64 rakssId, ui64 targetUserId)
{
	auto packet = LORD::make_shared<S2CPacketCallOnManorResetClient>();
	packet->m_targetUserId = targetUserId;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendUpdateManorOwner(ui64 rakssid)
{
	auto packet = LORD::make_shared<S2CPacketUpdateManorOwner>();
	auto world = Server::Instance()->getWorld();
	for (auto owner : world->m_manorControl->getOwners())
	{
		packet->m_owners.push_back(owner);
	}
	if (rakssid > 0)
	{
		Server::Instance()->getNetwork()->sendPacket(packet, rakssid, true);
		return;
	}
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::broadCastBasementLife(float curLife, float maxLife)
{
	auto packet = LORD::make_shared<S2CPacketUpdateBasementLife>();
	packet->curLife = curLife;
	packet->maxLife = maxLife;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::broadCastPlayerMovementInput(int playerEntityId)
{
	EntityPlayerMP* pPlayer = dynamic_cast<EntityPlayerMP*>(Server::Instance()->getWorld()->getPlayerEntity(playerEntityId));
	if (pPlayer == nullptr || pPlayer->movementInput == nullptr)
		return;

	auto packet = LORD::make_shared<S2CPacketSyncPlayerMovementInput>();
	packet->playerEntityId = playerEntityId;
	packet->forward = pPlayer->movementInput->moveForward;
	packet->strafe = pPlayer->movementInput->moveStrafe;
	sendPacketToTrackingPlayers(playerEntityId, packet, false);
}

void ServerPacketSender::broadCastPlayerRotation(int playerEntityId, bool includeSelf)
{
	EntityPlayerMP* pPlayer = dynamic_cast<EntityPlayerMP*>(Server::Instance()->getWorld()->getPlayerEntity(playerEntityId));
	if (pPlayer == nullptr)
		return;

	auto packet = LORD::make_shared<S2CPacketSyncPlayerRotation>();
	packet->playerEntityId = playerEntityId;
	packet->rotationPitch = pPlayer->rotationPitch;
	packet->rotationYaw = pPlayer->rotationYaw;
	sendPacketToTrackingPlayers(playerEntityId, packet, includeSelf);
}

void ServerPacketSender::broadCastPlayerMovement(int playerEntityId, bool includeSelf)
{
	EntityPlayerMP* pPlayer = dynamic_cast<EntityPlayerMP*>(Server::Instance()->getWorld()->getPlayerEntity(playerEntityId));
	if (pPlayer == nullptr)
		return;

	auto packet = LORD::make_shared<S2CPacketEntityPlayerTeleport>(pPlayer);
	sendPacketToTrackingPlayers(playerEntityId, packet, includeSelf);
}

void ServerPacketSender::broadCastPlayerJumpChanged(int playerEntityId)
{
	EntityPlayerMP* pPlayer = dynamic_cast<EntityPlayerMP*>(Server::Instance()->getWorld()->getPlayerEntity(playerEntityId));
	if (pPlayer == nullptr || pPlayer->movementInput == nullptr)
		return;

	auto packet = LORD::make_shared<S2CPacketSyncPlayerJumpChanged>();
	packet->playerEntityId = playerEntityId;
	packet->isJump = pPlayer->movementInput->jump;
	sendPacketToTrackingPlayers(playerEntityId, packet, false);
}

void ServerPacketSender::broadCastPlayerSpecialJump(int playerEntityId, float yFactor, float xFactor, float zFactor)
{
	EntityPlayerMP* pPlayer = dynamic_cast<EntityPlayerMP*>(Server::Instance()->getWorld()->getPlayerEntity(playerEntityId));
	if (pPlayer == nullptr)
		return;

	auto packet = LORD::make_shared<S2CPacketSyncPlayerSpecialJump>();
	packet->playerEntityId = playerEntityId;
	packet->yFactor = yFactor;
	packet->xFactor = xFactor;
	packet->zFactor = zFactor;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::syncEntityPlayerAnimate(ui64 rakssId, NETWORK_DEFINE::PacketAnimateType animateType, int entityId, int32_t value)
{
	auto animatePacket = LORD::make_shared<S2CPacketAnimate>();
	animatePacket->m_animateType = animateType;
	animatePacket->m_entityRuntimeId = entityId;
	animatePacket->m_value = value;
	Server::Instance()->getNetwork()->sendPacket(animatePacket, rakssId, true);
}

void ServerPacketSender::syncEntityPlayerMovementInput(ui64 rakssId, int playerEntityId)
{
	EntityPlayerMP* pPlayer = dynamic_cast<EntityPlayerMP*>(Server::Instance()->getWorld()->getPlayerEntity(playerEntityId));
	if (pPlayer == nullptr || pPlayer->movementInput == nullptr)
		return;

	auto packet = LORD::make_shared<S2CPacketSyncPlayerMovementInput>();
	packet->playerEntityId = playerEntityId;
	packet->forward = pPlayer->movementInput->moveForward;
	packet->strafe = pPlayer->movementInput->moveStrafe;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::broadCastPlayerDownChanged(int playerEntityId)
{
	EntityPlayerMP* pPlayer = dynamic_cast<EntityPlayerMP*>(Server::Instance()->getWorld()->getPlayerEntity(playerEntityId));
	if (pPlayer == nullptr || pPlayer->movementInput == nullptr)
		return;

	auto packet = LORD::make_shared<S2CPacketSyncPlayerDownChanged>();
	packet->playerEntityId = playerEntityId;
	packet->isDown = pPlayer->movementInput->down;
	sendPacketToTrackingPlayers(playerEntityId, packet, false);
}

void ServerPacketSender::broadCastPlayerSetFlyingState(int playerEntityId, bool isFlying)
{
	EntityPlayerMP* pPlayer = dynamic_cast<EntityPlayerMP*>(Server::Instance()->getWorld()->getPlayerEntity(playerEntityId));
	if (pPlayer == nullptr)
		return;

	auto packet = LORD::make_shared<S2CPacketSyncPlayerSetFlyingState>();
	packet->playerEntityId = playerEntityId;
	packet->isFlying = isFlying;
	sendPacketToTrackingPlayers(playerEntityId, packet);
}

void ServerPacketSender::broadCastPlayerSetSpYaw(int playerEntityId, bool isSpYaw, float radian)
{
	EntityPlayerMP* pPlayer = dynamic_cast<EntityPlayerMP*>(Server::Instance()->getWorld()->getPlayerEntity(playerEntityId));
	if (pPlayer == nullptr)
		return;

	auto packet = LORD::make_shared<S2CPacketSyncPlayerSetSpYaw>();
	packet->playerEntityId = playerEntityId;
	packet->isSpYaw = isSpYaw;
	packet->radian = radian;
	sendPacketToTrackingPlayers(playerEntityId, packet);
}

void ServerPacketSender::sendGameTipNotification(ui64 rakssid, int msgType, ChatComponentNotification * notification)
{
	auto packet = LORD::make_shared<S2CPacketGameTipNotification>();
	packet->m_tipType = msgType;
	packet->m_chatComponent.reset(notification);

	if (rakssid == 0)
	{
		Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
	}
	else
	{
		Server::Instance()->getNetwork()->sendPacket(packet, rakssid, true);
	}
}

void ServerPacketSender::broadCastEntityTeleport(int entityId)
{
	Entity* myEntity = Server::Instance()->getWorld()->getEntity(entityId);
	if (myEntity == nullptr)
		return;

	int x = int(Math::Floor(myEntity->position.x * 32.f));
	int y = int(Math::Floor(myEntity->position.y * 32.f));
	int z = int(Math::Floor(myEntity->position.z * 32.f));
	float yaw = myEntity->rotationYaw;
	float pitch = myEntity->rotationPitch;

	auto packet = LORD::make_shared<S2CPacketEntityTeleport>(entityId, x, y, z, yaw, pitch);
	sendPacketToTrackingPlayers(entityId, packet, true);
}

void ServerPacketSender::syncPlayerMovement(ui64 rakssId, int playerEntityId)
{
	EntityPlayerMP* pPlayer = dynamic_cast<EntityPlayerMP*>(Server::Instance()->getWorld()->getPlayerEntity(playerEntityId));
	if (pPlayer == nullptr)
		return;

	auto packet = LORD::make_shared<S2CPacketEntityPlayerTeleport>(pPlayer);
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::syncPlayerDownChanged(ui64 rakssId, int playerEntityId)
{
	EntityPlayerMP* pPlayer = dynamic_cast<EntityPlayerMP*>(Server::Instance()->getWorld()->getPlayerEntity(playerEntityId));
	if (pPlayer == nullptr || pPlayer->movementInput == nullptr)
		return;

	auto packet = LORD::make_shared<S2CPacketSyncPlayerDownChanged>();
	packet->playerEntityId = playerEntityId;
	packet->isDown = pPlayer->movementInput->down;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::syncPlayerJumpChanged(ui64 rakssId, int playerEntityId)
{
	EntityPlayerMP* pPlayer = dynamic_cast<EntityPlayerMP*>(Server::Instance()->getWorld()->getPlayerEntity(playerEntityId));
	if (pPlayer == nullptr || pPlayer->movementInput == nullptr)
		return;

	auto packet = LORD::make_shared<S2CPacketSyncPlayerJumpChanged>();
	packet->playerEntityId = playerEntityId;
	packet->isJump = pPlayer->movementInput->jump;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendCustomTipMsg(ui64 rakssId, String messageLang, String extra)
{
	auto packet = LORD::make_shared<S2CPacketCustomTipMsg>();
	packet->messageLang = messageLang;
	packet->extra = extra;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendEntityPlayerOccupation(ui64 rakssId, int entityId, int occupation)
{
	auto packet = LORD::make_shared<S2CPacketEntityPlayerOccupation>();
	packet->occupation = occupation;
	packet->entityId = entityId;
	if (rakssId != 0)
	{
		Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
	}
	else
	{
		sendPacketToTrackingPlayers(entityId, packet, true);
	}
}

void ServerPacketSender::syncShowMaskTime(ui64 rakssId, int MilliSecond)
{
	auto packet = LORD::make_shared<S2CPacketSyncShowMaskTime>();
	packet->MilliSecond = MilliSecond;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendBuildWarBlockBtn(ui64 rakssId, bool isNeedOpen)
{
	auto packet = LORD::make_shared<S2CPacketShowBuildWarBlockBtn>();
	packet->isNeedOpen = isNeedOpen;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendbroadcastBuildWarPlayNum(ui8 cur_num, ui8 max_num)
{
	auto packet = LORD::make_shared<S2CPacketShowBuildWarPlayNum>();
	packet->cur_num = cur_num;
	packet->max_num = max_num;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendbroadcastBuildWarLeftTime(ui64 rakssId, bool show, ui16 left_time, ui32 theme)
{
	auto packet = LORD::make_shared<S2CPacketShowBuildWarLeftTime>();
	packet->show = show;
	packet->left_time = left_time;

	auto clientPeer = Server::Instance()->getNetwork()->findPlayerByRakssid(rakssId);
	if (clientPeer)
	{
		String lan = clientPeer->getLanguage();
		std::string msg = MultiLanTipSetting::getMessage(lan, theme, "");
		packet->theme = msg.c_str();
		Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
	}
}

void ServerPacketSender::sendbroadcastRanchExTip(ui16 type, ui16 num)
{
	auto packet = LORD::make_shared<S2CPacketShowRanchExTip>();
	packet->type = type;
	packet->num = num;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet, true);
}

void ServerPacketSender::sendbroadcastRanchExTask(bool show, String task)
{
	auto packet = LORD::make_shared<S2CPacketShowRanchExTask>();
	packet->show = show;
	packet->task = task;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet, true);
}

void ServerPacketSender::sendShowRanchExCurrentItemInfo(ui64 rakssId, bool show, String itemInfo, int type, int num)
{
	auto packet = LORD::make_shared<S2CPacketShowRanchExCurrentItemInfo>();
	packet->show = show;
	packet->itemInfo = itemInfo;
	packet->num = num;
	packet->type = type;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendShowRanchExItem(ui64 rakssId, bool show, String item)
{
	auto packet = LORD::make_shared<S2CPacketShowRanchExItem>();
	packet->show = show;
	packet->item = item;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendOpenChest(bool show, int entityId, Vector3i pos)
{
	sendCloseContainer(entityId, show, pos);
}

void ServerPacketSender::sendOpenShopByEntityId(ui64 rakssId, int entityId)
{
	auto packet = LORD::make_shared<S2CPacketOpenShopByEntityId>();
	packet->entityId = entityId;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendShowBuildGrade(ui64 rakssId, bool openOrClose, bool show, ui32 msg, ui32 grade_msg, ui8 left_time)
{
	auto packet = LORD::make_shared<S2CPacketShowGrade>();
	packet->openOrClose = openOrClose;
	packet->show = show;
	auto clientPeer = Server::Instance()->getNetwork()->findPlayerByRakssid(rakssId);
	if (clientPeer)
	{
		String lan = clientPeer->getLanguage();
		std::string show_msg = MultiLanTipSetting::getMessage(lan, msg, "");

		if (!openOrClose && grade_msg > 0)
		{
			show_msg = MultiLanTipSetting::getMessage(lan, grade_msg, "");
		}

		packet->msg = show_msg.c_str();

		packet->left_time = left_time;
		Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
	}
}

void ServerPacketSender::sendShowBuildGuessResult(ui64 rakssId, bool right, int rank)
{
	auto packet = LORD::make_shared<S2CPacketShowGuessResult>();
	packet->right = right;
	packet->rank = rank;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::showBuildGuessUi(ui64 rakssId, int guess_room_id, String info)
{
	auto packet = LORD::make_shared<S2CPacketShowGuessUi>();
	packet->info = info;
	packet->guess_room_id = guess_room_id;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}


void ServerPacketSender::sendGameSettlementExtra(ui64 rakssId, bool guess_right, String guess_name, int guess_reward)
{
	auto packet = LORD::make_shared<S2CPacketGameSettlementExtra>();
	packet->guess_right = guess_right;
	packet->guess_name = guess_name;
	packet->guess_reward = guess_reward;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendUnlockedCommodity(ui64 rakssid, int merchantId, int itemId, int meta)
{
	auto packet = LORD::make_shared<S2CPacketUnlockedCommodity>();
	packet->merchantId = merchantId;
	packet->itemId = itemId;
	packet->meta = meta;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssid);
}

void ServerPacketSender::sendInitUnlockedCommodity(ui64 rakssId, std::vector<int> unlockedCommodity)
{
	if (unlockedCommodity.size() == 2)
	{
		auto packet = LORD::make_shared<S2CPacketInitUnlockedCommodity>();
		packet->itemId = unlockedCommodity[0];
		packet->meta = unlockedCommodity[1];
		Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
	}
}

void ServerPacketSender::sendTeamResourcesUpdate(String data)
{
	auto packet = LORD::make_shared<S2CPacketTeamResourcesUpdate>();
	packet->m_data = data;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendCloseBGM(ui64 rakssId)
{
	auto packet = LORD::make_shared<S2CPacketCloseBGM>();
	if (rakssId == 0)
	{
		Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
	}
	else
	{
		Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
	}
}

void ServerPacketSender::sendSetArmItem(ui64 rakssId, int itemId)
{
	auto packet = LORD::make_shared<S2CPacketSetArmItem>();
	packet->itemId = itemId;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::broadCastActorNpcContent(int entityId, String content)
{
	auto packet = LORD::make_shared<S2CPacketActorNpcContent>();
	packet->entityId = entityId;
	packet->content = content;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::broadCastBuildProgress(String name, int curProgress, int maxProgress)
{
	auto packet = LORD::make_shared<S2CPacketBuildProgress>();
	packet->name = name;
	packet->curProgress = curProgress;
	packet->maxProgress = maxProgress;
	packet->iconPath = "";
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendEnchantmentPropsData(ui64 rakssId, String data)
{
	auto packet = LORD::make_shared<S2CPacketEnchantmentPropsData>();
	packet->data = data;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendPlayerEnableMovement(int entityId)
{
	auto packet = LORD::make_shared<S2CPacketEnableMovement>();
	packet->entityId = entityId;
	sendPacketToTrackingPlayers(entityId, packet, false);
}

void ServerPacketSender::sendConsumeCoinTip(ui64 rakssId, String message, int coinId, int price, String extra)
{
	auto packet = LORD::make_shared<S2CPacketConsumeCoinTip>();
	packet->message = message;
	packet->coinId = coinId;
	packet->price = price;
	packet->extra = extra;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendSuperPropsData(ui64 rakssId, String data)
{
	auto packet = LORD::make_shared<S2CPacketSuperPropsData>();
	packet->data = data;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendSuperShopTip(ui64 rakssId, String tip)
{
	auto packet = LORD::make_shared<S2CPacketSuperShopTip>();
	packet->tip = tip;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::broadCastAddSimpleEffect(String effectName, Vector3 position, float yaw, int duration, float scale)
{
	auto packet = LORD::make_shared<S2CPacketAddSimpleEffect>();
	packet->effectName = effectName;
	packet->position = position;
	packet->yaw = yaw;
	packet->duration = duration;
	packet->scale = scale;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendAddSimpleEffect(String effectName, Vector3 position, float yaw, int duration, int targetId, float scale)
{
	auto packet = LORD::make_shared<S2CPacketAddSimpleEffect>();
	packet->effectName = effectName;
	packet->position = position;
	packet->yaw = yaw;
	packet->duration = duration;
	packet->scale = scale;
	Server::Instance()->getNetwork()->sendPacket(packet, targetId, true);
}

void ServerPacketSender::sendEnterOtherGame(ui64 rakssId, String gameType, ui64 targetId, String mapId)
{
	auto packet = LORD::make_shared<S2CPacketEnterOtherGame>();
	packet->gameType = gameType;
	packet->targetId = targetId;
	packet->mapId = mapId;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendKillMsg(ui64 rakssId, String data)
{
	auto packet = LORD::make_shared<S2CPacketKillMsg>();
	packet->data = data;
	if (rakssId == 0)
		Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
	else
		Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendShowPersonalShop(ui64 rakssId)
{
	auto packet = LORD::make_shared<S2CPacketShowPersonalShop>();
	if (rakssId == 0)
		Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
	else
		Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendSetPersonalShopArea(ui64 rakssId, Vector3 startPos, Vector3 endPos)
{
	auto packet = LORD::make_shared<S2CPacketPersonalShopArea>();
	packet->startPos = startPos;
	packet->endPos = endPos;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendAddGunBulletNum(ui64 rakssId, int gunId, int bulletNum)
{
	auto packet = LORD::make_shared<S2CPacketAddGunBulletNum>();
	packet->gunId = gunId;
	packet->bulletNum = bulletNum;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendSubGunRecoil(ui64 rakssId, int gunId, float recoil)
{
	auto packet = LORD::make_shared<S2CPacketSubGunRecoil>();
	packet->gunId = gunId;
	packet->recoil = recoil;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendCustomPropsData(ui64 rakssId, String data)
{
	auto packet = LORD::make_shared<S2CPacketCustomPropsData>();
	packet->data = data;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::updateCustomProps(ui64 rakssId, String data)
{
	auto packet = LORD::make_shared<S2CPacketUpdateCustomProps>();
	packet->data = data;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::syncEntityBlockman(ui64 rakssId, BLOCKMAN::EntityBlockman * blockman)
{
	auto packet = LORD::make_shared<S2CPacketSyncEntityBlockmanInfo>();
	packet->entityId = blockman->entityId;
	packet->blockmanType = (int)blockman->getBlockmanType();
	packet->nameLang = blockman->getNameLang();
	packet->actorName = blockman->getActorName();
	packet->actorBody = blockman->getActorBody();
	packet->actorBodyId = blockman->getActorBodyId();
	packet->maxSpeed = blockman->getCurrentMaxSpeed();
	switch (blockman->getBlockmanType()) {
		case EntityBlockmanType::BLOCKMAN_EMPTY:
		{
			auto entity = dynamic_cast<EntityBlockmanEmpty*>(blockman);
			if (entity)
			{
				packet->curSpeed = entity->getCurSpeed();
				packet->addSpeed = entity->getSpeedAddtion();
				packet->isRecordRoute = entity->isNeedRecordRoute();
				packet->longHitTimes = entity->getLongHitTimes();
			}
		}
		break;
	}
	if (rakssId == 0)
		sendPacketToTrackingPlayers(blockman->entityId, packet, true);
	else
		Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
	
}

void ServerPacketSender::syncRanch(BLOCKMAN::EntityPlayer * player, int syncType)
{
	auto p = dynamic_cast<EntityPlayerMP*>(player);
	if (p&& p->m_ranch)
	{
		switch ((BLOCKMAN::SyncRanchType)syncType)
		{
		case SyncRanchType::HOUSE: {
			auto packet = LORD::make_shared<S2CPacketSyncRanchHouse>();
			packet->m_house = *p->m_ranch->getHouse();
			Server::Instance()->getNetwork()->sendPacket(packet, p->getRaknetID(), true);
			break;
			}
		case SyncRanchType::INFO: {
			auto packet = LORD::make_shared<S2CPacketSyncRanchInfo>();
			packet->m_info = *p->m_ranch->getInfo();
			Server::Instance()->getNetwork()->sendPacket(packet, p->getRaknetID(), true);
			break;
			}

		case SyncRanchType::STORAGE: {
			auto  packet = LORD::make_shared<S2CPacketSyncRanchStorage>();
			packet->m_storage = *p->m_ranch->getStorage();
			Server::Instance()->getNetwork()->sendPacket(packet, p->getRaknetID(), true);
			break;
			}
		case SyncRanchType::ORDER: {
			auto  packet = LORD::make_shared<S2CPacketSyncRanchOrder>();
			packet->m_orders = p->m_ranch->getOrders();
			Server::Instance()->getNetwork()->sendPacket(packet, p->getRaknetID(), true);
			break;
			}
		case SyncRanchType::ACHIEVEMENT: {
			auto  packet = LORD::make_shared<S2CPacketSyncRanchAchievement>();
			packet->m_achievements = p->m_ranch->getAchievements();
			Server::Instance()->getNetwork()->sendPacket(packet, p->getRaknetID(), true);
			break;
		}
		case SyncRanchType::TIME_PRICE: {
			auto  packet = LORD::make_shared<S2CPacketSyncRanchTimePrice>();
			packet->m_timePrices = p->m_ranch->getTimePrices();
			Server::Instance()->getNetwork()->sendPacket(packet, p->getRaknetID(), true);
			break;
		}
		case SyncRanchType::SHORTCUT_FREE_TIMES: {
			auto  packet = LORD::make_shared<S2CPacketSyncRanchShortcutFreeTimes>();
			const auto& items = p->m_ranch->getShortcutFreeTimes();
			for (auto item : items )
			{
				vector<i32>::type newItems = vector<i32>::type();
				newItems.push_back(item.first);
				newItems.push_back(item.second);
				packet->m_shortcutFreeTimes.push_back(newItems);
			}

			Server::Instance()->getNetwork()->sendPacket(packet, p->getRaknetID(), true);
			break;
		}
		}

	}
}

void ServerPacketSender::syncRanchRank(BLOCKMAN::EntityPlayer * player, int rankType)
{
	auto p = dynamic_cast<EntityPlayerMP*>(player);
	if (p && p->m_ranch)
	{
		auto packet = LORD::make_shared<S2CPacketSyncRanchRank>();
		packet->m_rankType = rankType;
		auto ranks = p->m_ranch->getRankByType(rankType);
		for (auto rank : ranks)
		{
			packet->m_ranks.push_back(rank);
		}
		Server::Instance()->getNetwork()->sendPacket(packet, p->getRaknetID(), true);
	}
}

void ServerPacketSender::syncRanchBuild(BLOCKMAN::EntityPlayer * player, int buildType)
{
	auto p = dynamic_cast<EntityPlayerMP*>(player);
	if (p && p->m_ranch)
	{
		auto packet = LORD::make_shared<S2CPacketSyncRanchBuild>();
		packet->m_buildType = buildType;
		auto builds = p->m_ranch->getBuildByType(buildType);
		for (auto build : builds)
		{
			packet->m_builds.push_back(build);
		}
		Server::Instance()->getNetwork()->sendPacket(packet, p->getRaknetID(), true);
	}
}

void ServerPacketSender::syncEntityBuildNpc(ui64 rakssId, BLOCKMAN::EntityBuildNpc * buildNpc)
{
	auto packet = LORD::make_shared<S2CPacketSyncEntityBuildNpc>();
	packet->entityId = buildNpc->entityId;
	packet->m_actorId = buildNpc->m_actorId;
	packet->m_userId = buildNpc->m_userId;
	packet->m_maxQueueNum = buildNpc->m_maxQueueNum;
	packet->m_productCapacity = buildNpc->m_productCapacity;
	packet->m_queueUnlockPrice = buildNpc->m_queueUnlockPrice;
	packet->m_queueUnlockCurrencyType = buildNpc->m_queueUnlockCurrencyType;

	for (auto product : buildNpc->m_products)
	{
		packet->m_products.push_back(product);
	}

	for (auto queue : buildNpc->m_unlockQueues)
	{
		packet->m_unlockQueues.push_back(queue);
	}

	for (auto recipe : buildNpc->m_recipes)
	{
		packet->m_recipes.push_back(recipe);
	}

	if (rakssId == 0)
	{
		sendPacketToTrackingPlayers(buildNpc->entityId, packet);
	}
	else
	{
		Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
	}
}

void ServerPacketSender::syncEntityLandNpc(ui64 rakssId, BLOCKMAN::EntityLandNpc * landNpc)
{
	auto packet = LORD::make_shared<S2CPacketSyncEntityLandNpc>();
	packet->entityId = landNpc->entityId;
	packet->name = landNpc->getName();
	packet->nameLang = landNpc->getNameLang();
	packet->actorName = landNpc->getActorName();
	packet->actorBody = landNpc->getActorBody();
	packet->actorBodyId = landNpc->getActorBodyId();
	packet->status = landNpc->getStatus();
	packet->timeLeft = landNpc->getTimeLeft();
	packet->recipe = landNpc->getRecipe();
	packet->reward = landNpc->getReward();
	packet->userId = landNpc->getUserId();
	packet->landCode = landNpc->getLandCode();
	packet->price = landNpc->getPrice();
	packet->totalTime = landNpc->getTotalTime();

	if (rakssId != 0)
	{
		Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
	}
	else
	{
		sendPacketToTrackingPlayers(landNpc->entityId, packet);
	}
}

void ServerPacketSender::sendRanchGain(ui64 rakssId, vector<BLOCKMAN::RanchCommon>::type items)
{
	auto packet = LORD::make_shared<S2CPacketRanchGain>();
	packet->items = items;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendBroadcastMessage(ui64 rakssId, i32 type, const String& content)
{
	auto packet = LORD::make_shared<S2CPacketBroadcastMessage>();
	packet->type = type;
	packet->content = content;
	if (rakssId == 0)
	{
		Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
	}
	else
	{
		Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
	}
}

void ServerPacketSender::sendGotoOtherGame(ui64 rakssId, ui64 targetUserId, String gameType, String mapId)
{
	auto packet = LORD::make_shared<S2CPacketGotoOtherGame>();
	packet->gameType = gameType;
	packet->targetUserId = targetUserId;
	packet->mapId = mapId;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendRanchUnlockItem(ui64 rakssId, vector<i32>::type items)
{
	auto packet = LORD::make_shared<S2CPacketRanchUnlockItem>();
	packet->items = items;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::syncBlockmanMoveAttr(ui64 rakssId, BLOCKMAN::EntityBlockman * blockman)
{
	auto entity = dynamic_cast<EntityBlockmanEmpty*>(blockman);
	if (entity)
	{
		auto packet = LORD::make_shared<S2CPacketBlockmanMoveAttr>();
		packet->entityId = entity->entityId;
		packet->pushX = entity->pushX;
		packet->pushZ = entity->pushZ;
		packet->position = entity->position;
		packet->motion = entity->motion;
		packet->yaw = entity->rotationYaw;
		if (rakssId == 0)
			sendPacketToTrackingPlayers(blockman->entityId, packet, true);
		else
			Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
	}
}

void ServerPacketSender::broadCastCarDirection(int type, int direction)
{
	auto packet = LORD::make_shared<S2CPacketCarDirection>();
	packet->type = type;
	packet->direction = direction;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::broadCastCarProgress(int type, float progress)
{
	auto packet = LORD::make_shared<S2CPacketCarProgress>();
	packet->type = type;
	packet->progress = progress;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::syncEntityFrozenTime(ui64 rakssId, int entityId, float time)
{
	auto packet = LORD::make_shared<S2CPacketEntityFrozenTime>();
	packet->entityId = entityId;
	packet->time = time;
	if (rakssId == 0)
		sendPacketToTrackingPlayers(entityId, packet, true);
	else
		Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::broadCastPlayerMovementWithMotion(int playerEntityId, bool includeSelf)
{
	EntityPlayerMP* pPlayer = dynamic_cast<EntityPlayerMP*>(Server::Instance()->getWorld()->getPlayerEntity(playerEntityId));
	if (pPlayer == nullptr)
		return;

	auto packet = LORD::make_shared<S2CPacketEntityPlayerTeleportWithMotion>(pPlayer);

	sendPacketToTrackingPlayers(playerEntityId, packet, includeSelf);
}

void ServerPacketSender::sendCannonActorInfo(i64 rakssId, BLOCKMAN::EntityActorCannon* pCannon)
{
	auto packet = LORD::make_shared<S2CSyncCannonActorInfo>();
	packet->entityId = pCannon->entityId;
	packet->actorName = pCannon->getActorName();
	packet->headName = pCannon->getHeadName();
	packet->skillName = pCannon->getSkillName();
	packet->haloEffectName = pCannon->getHaloEffectName();
	packet->isCanObstruct = pCannon->isCanObstruct();
	packet->content = pCannon->getContent();
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendSyncCannonFireToTrackingPlayers(int playerEntityId, int cannonEntityId)
{
	auto packet = LORD::make_shared<S2CPacketSyncCannonFire>();
	packet->cannonEntityId = cannonEntityId;
	sendPacketToTrackingPlayers(playerEntityId, packet);
}

void ServerPacketSender::sendCropInfo(i64 userId, Vector3i blockPos, i32 blockId, i32 stage, i32 curStageTime, i32 residueHarvestNum)
{
	auto pree = Server::Instance()->getNetwork()->findPlayerByPlatformUserId(userId);
	if (pree)
	{
		auto packet = LORD::make_shared<S2CPacketCropInfo>();
		packet->blockPos = blockPos;
		packet->blockId = blockId;
		packet->stage = stage;
		packet->curStageTime = curStageTime;
		packet->residueHarvestNum = residueHarvestNum;
		Server::Instance()->getNetwork()->sendPacket(packet, pree->getRakssid(), true);
	}

}

void ServerPacketSender::sendAppExpResult(i64 userId, int curLv, int toLv, int addExp, int curExp, int toExp, int upExp, int status)
{
	auto pree = Server::Instance()->getNetwork()->findPlayerByPlatformUserId(userId);
	if (pree)
	{
		auto packet = LORD::make_shared<S2CPacketAppExpResult>();
		packet->curLv = curLv;
		packet->toLv = toLv;
		packet->addExp = addExp;
		packet->curExp = curExp;
		packet->toExp = toExp;
		packet->upExp = upExp;
		packet->status = status;
		Server::Instance()->getNetwork()->sendPacket(packet, pree->getRakssid(), true);
	}
}

void ServerPacketSender::sendOccupationUnlock(ui64 rakssId, String occupationInfo)
{
	auto packet = LORD::make_shared<S2CPacketOccupationUnlock>();
	packet->occupationInfo = occupationInfo;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendBuyFlying(ui64 rakssId)
{
	auto packet = LORD::make_shared<S2CPacketBuyFlying>();
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendPlayCloseup(ui64 rakssId, const Vector3& playerPos, const Vector3& closeupPos, float farDistance, float nearDistance, float velocity, float duration, float yaw, float pitch)
{
	auto packet = LORD::make_shared<S2CPacketPlayCloseup>();
	packet->playerPos = playerPos;
	packet->closeupPos = closeupPos;
	packet->farDistance = farDistance;
	packet->nearDistance = nearDistance;
	packet->velocity = velocity;
	packet->duration = duration;
	packet->yaw = yaw;
	packet->pitch = pitch;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendKeepItemTip(i64 rakssId, int coinType, int coin, int tipTime)
{
	auto packet = LORD::make_shared<S2CPacketShowBuyKeepItemTip>();
	packet->coinType = coinType;
	packet->coin = coin;
	packet->tipTime = tipTime;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendOpenEnchantment(i64 rakssId, String data)
{
	auto packet = LORD::make_shared<S2CPacketSendOpenEnchantment>();
	packet->data = data;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendLogicSettingInfo(ui64 rakssid)
{
	auto packet = LORD::make_shared<S2CPacketLogicSettingInfo>();
	packet->m_mainInventorySize = LogicSetting::Instance()->getMaxInventorySize();
	packet->m_sneakShowName = LogicSetting::Instance()->isSneakShowName();
	packet->m_breakBlockSoon = LogicSetting::Instance()->isBreakBlockSoon();
	packet->m_isCanDamageItem = LogicSetting::Instance()->isCanDamageItem();
	packet->m_gunSetting = LogicSetting::Instance()->getGunPluginSetting();
	packet->m_bulletClipSetting = LogicSetting::Instance()->getBulletClipPluginSetting();
	packet->m_blockDynamicAttr = LogicSetting::Instance()->getBlockDynamicAttr();
	packet->m_toolItemDynamicAttr = LogicSetting::Instance()->getToolItemDynamicAttr();
	packet->m_disableSelectEntity = LogicSetting::Instance()->isDisableSelectEntity();
	packet->m_gunIsNeedBullet = LogicSetting::Instance()->getGunIsNeedBulletStatus();
	packet->m_skillItemSetting = LogicSetting::Instance()->getSkillItemSetting();
	packet->m_hideClouds = LogicSetting::Instance()->isHideClouds();
	packet->m_canCloseChest = LogicSetting::Instance()->isCanCloseChest();
	packet->m_showGunEffectWithSingle = LogicSetting::Instance()->getShowGunEffectWithSingleStatus();
	packet->m_allowHeadshot = LogicSetting::Instance()->getAllowHeadshotStatus();
	packet->m_thirdPersonDistance = LogicSetting::Instance()->getThirdPersonDistance();
	Server::Instance()->getNetwork()->sendPacket(packet, rakssid, true);
}

void ServerPacketSender::sendSubGunFireCd(ui64 rakssId, int gunId, float fireCd)
{
	auto packet = LORD::make_shared<S2CPacketSubGunFireCd>();
	packet->gunId = gunId;
	packet->fireCd = fireCd;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::syncEntityHurtTime(ui64 rakssId, int entityId, float time)
{
	auto packet = LORD::make_shared<S2CPacketEntityHurtTime>();
	packet->entityId = entityId;
	packet->time = time;
	if (rakssId == 0)
		sendPacketToTrackingPlayers(entityId, packet, true);
	else
		Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendLotteryData(ui64 rakssId, String data)
{
	auto packet = LORD::make_shared<S2CPacketLotteryData>();
	packet->data = data;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendLotteryResult(ui64 rakssId, String firstId, String secondId, String thirdId)
{
	auto packet = LORD::make_shared<S2CPacketLotteryResult>();
	packet->firstId = firstId;
	packet->secondId = secondId;
	packet->thirdId = thirdId;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendHideAndSeekHallResult(ui64 rakssId, String itemInfo)
{
	auto packet = LORD::make_shared<S2CPacketHideAndSeekHallResult>();
	packet->itemInfo = itemInfo;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::syncEntityColorfulTime(ui64 rakssId, int entityId, float time)
{
	auto packet = LORD::make_shared<S2CPacketEntityColorfulTime>();
	packet->entityId = entityId;
	packet->time = time;
	if (rakssId == 0)
		sendPacketToTrackingPlayers(entityId, packet, true);
	else
		Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendEntitySelected(ui64 rakssId, int entityId, int type)
{
	auto packet = LORD::make_shared<S2CPacketEntitySelected>();
	packet->entityId = entityId;
	packet->type = type;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::syncEntityDisableMoveTime(ui64 rakssId, int entityId, float time)
{
	auto packet = LORD::make_shared<S2CPacketEntityDisableMoveTime>();
	packet->entityId = entityId;
	packet->time = time;
	if (rakssId == 0)
		sendPacketToTrackingPlayers(entityId, packet, true);
	else
		Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendOpenPixelGunHallModeSelect(i64 rakssId, bool open, String data)
{
	auto packet = LORD::make_shared<S2CPacketSendOpenPixelGunHallModeSelect>();
	packet->open = open;
	packet->data = data;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendUpdateStoreGunData(i64 rakssId, String data)
{
	auto packet = LORD::make_shared<S2CPacketUpdateStoreGunData>();
	packet->data = data;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendUpdateStorePropData(i64 rakssId, String data)
{
	auto packet = LORD::make_shared<S2CPacketUpdateStorePropData>();
	packet->data = data;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendShowGunStore(i64 rakssId)
{
	auto packet = LORD::make_shared<S2CPacketShowGunStore>();
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::broadCastPlayerEffectGunGire(int shootingEntityId, int gunId, const Vector3& beginPos, const Vector3& endPos)
{
	auto s2cPacket = LORD::make_shared<S2CPacketSyncEffectGunFire>();
	s2cPacket->shootingEntityId = shootingEntityId;
	s2cPacket->gunId = gunId;
	s2cPacket->endPos = endPos;
	s2cPacket->beginPos = beginPos;
	sendPacketToTrackingPlayers(shootingEntityId, s2cPacket, true);
}

void ServerPacketSender::broadCastPlayerEffectGunGireAtEntityResult(int shootingEntityId, int hitEntityId, const Vector3& hitPos, const Vector3& dir, int traceType, bool isHeadshot)
{
	auto s2cPacket = LORD::make_shared<S2CPacketSyncEffectGunFireAtEntityResult>();
	s2cPacket->shootingEntityId = shootingEntityId;
	s2cPacket->hitEntityId = hitEntityId;
	s2cPacket->hitPos = hitPos;
	s2cPacket->dir = dir;
	s2cPacket->traceType = traceType;
	s2cPacket->isHeadshot = isHeadshot;
	sendPacketToTrackingPlayers(shootingEntityId, s2cPacket, true);
}

void ServerPacketSender::broadCastPlayerEffectGunGireAtBlockResult(int shootingEntityId, const Vector3i& blockPos, const Vector3& hitPos, const Vector3& dir)
{
	auto s2cPacket = LORD::make_shared<S2CPacketSyncEffectGunFireAtBlockResult>();
	s2cPacket->blockPos = blockPos;
	s2cPacket->hitPos = hitPos;
	s2cPacket->dir = dir;
	sendPacketToTrackingPlayers(shootingEntityId, s2cPacket, true);
}

void ServerPacketSender::broadCastPlayerEffectGunGireExplosionResult(int shootingEntityId, const Vector3& hitPos, float explosionRange)
{
	auto s2cPacket = LORD::make_shared<S2CPacketSyncEffectGunFireExplosionResult>();
	s2cPacket->hitPos = hitPos;
	s2cPacket->explosionRange = explosionRange;
	sendPacketToTrackingPlayers(shootingEntityId, s2cPacket, true);
}

void ServerPacketSender::broadCastStopLaserGun(i32 shootingEntityId)
{
	auto packet = LORD::make_shared<S2CPacketStopLaserGun>();
	packet->shootingEntityId = shootingEntityId;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::broadCastPlayerOpacity(int entityId, float opacityValue)
{
	auto packet = LORD::make_shared<S2CPacketSetPlayerOpacity>();
	packet->entityId = entityId;
	packet->opacityValue = opacityValue;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::sendShowChestLottery(i64 rakssId, bool open, int curIntegral, int needIntegral)
{
	auto packet = LORD::make_shared<S2CPacketShowChestLottery>();
	packet->open = open;
	packet->curIntegral = curIntegral;
	packet->needIntegral = needIntegral;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendChestLotteryResult(i64 rakssId, int rewardId, bool hasGet)
{
	auto packet = LORD::make_shared<S2CPacketChestLotteryResult>();
	packet->rewardId = rewardId;
	packet->hasGet = hasGet;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendShowPixelGunHallInfo(i64 rakssId, int lv, int cur_exp, int max_exp, int yaoshi, bool is_max)
{
	auto packet = LORD::make_shared<S2CPacketShowPixelGunHallInfo>();
	packet->lv = lv;
	packet->cur_exp = cur_exp;
	packet->max_exp = max_exp;
	packet->yaoshi = yaoshi;
	packet->is_max = is_max;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendOpenPixel1v1(i64 rakssId, bool open, String data)
{
	auto packet = LORD::make_shared<S2CPacketSendOpenPixelGun1v1>();
	packet->open = open;
	packet->data = data;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendOpenPixelRevive(i64 rakssId, bool open, String data)
{
	auto packet = LORD::make_shared<S2CPacketSendOpenPixelGunRevive>();
	packet->open = open;
	packet->data = data;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendUpdateLotteryChestData(i64 rakssId, String data)
{
	auto packet = LORD::make_shared<S2CPacketUpdateLotteryChestData>();
	packet->data = data;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendShowPixelGunGamePerson(i64 rakssId, int time, int rank, int kill_num)
{
	auto packet = LORD::make_shared<S2CPacketShowPixelGunGamePerson>();
	packet->time = time;
	packet->rank = rank;
	packet->kill_num = kill_num;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendShowPixelGunGame1v1(i64 rakssId, int time, int red_kill_num, int blue_kill_num, int self_team)
{
	auto packet = LORD::make_shared<S2CPacketShowPixelGunGame1v1>();
	packet->time = time;
	packet->red_kill_num = red_kill_num;
	packet->blue_kill_num = blue_kill_num;
	packet->self_team = self_team;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendShowPixelGunGameTeam(i64 rakssId, int time, int red_kill_num, int red_cur_player_num, int red_max_player_num, int blue_kill_num, int blue_cur_player_num, int blue_max_player_num, int self_team)
{
	auto packet = LORD::make_shared<S2CPacketShowPixelGunGameTeam>();
	packet->time = time;
	packet->red_kill_num = red_kill_num;
	packet->red_cur_player_num = red_cur_player_num;
	packet->red_max_player_num = red_max_player_num;
	packet->blue_kill_num = blue_kill_num;
	packet->blue_cur_player_num = blue_cur_player_num;
	packet->blue_max_player_num = blue_max_player_num;
	packet->self_team = self_team;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}


void ServerPacketSender::sendOpenArmorUpgrade(i64 rakssId, bool open, String data)
{
	auto packet = LORD::make_shared<S2CPacketSendOpenArmorUpgrade>();
	packet->open = open;
	packet->data = data;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendOpenPixelResult(i64 rakssId, bool open, String data, int result_type)
{
	auto packet = LORD::make_shared<S2CPacketSendOpenPixelGunResult>();
	packet->open = open;
	packet->data = data;
	packet->result_type = result_type;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendPlayerCurrentSeasonInfo(i64 rakssId, int honorId, int rank, int honor, int endTime)
{
	auto packet = LORD::make_shared<S2CPacketPlayerCurrentSeasonInfo>();
	packet->honorId = honorId;
	packet->rank = rank;
	packet->honor = honor;
	packet->endTime = endTime;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendPlayerLastSeasonInfo(i64 rakssId, int honorId, int rank, int honor)
{
	auto packet = LORD::make_shared<S2CPacketPlayerLastSeasonInfo>();
	packet->honorId = honorId;
	packet->rank = rank;
	packet->honor = honor;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendEnableAutoShoot(i64 rakssId, bool enable)
{
	auto packet = LORD::make_shared<S2CPacketEnableAutoShoot>();
	packet->enable = enable;
	if (rakssId == 0)
		Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
	else
		Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendDisarmament(i64 rakssId, bool isDisarmament)
{
	auto packet = LORD::make_shared<S2CPacketDisarmament>();
	packet->isDisarmament = isDisarmament;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendOpenPixelLvUp(i64 rakssId, bool open, String data)
{
	auto packet = LORD::make_shared<S2CPacketSendOpenPixelGunLvUp>();
	packet->open = open;
	packet->data = data;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendOpenPixelResultSpecialInfo(i64 rakssId, bool btnRevengeEnable)
{
	auto packet = LORD::make_shared<S2CPacketSendOpenPixelGunResultSpecialInfo>();
	packet->btnRevengeEnable = btnRevengeEnable;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::notifyGetGoods(ui64 rakssId, String icon, int count)
{
	auto packet = LORD::make_shared<S2CPacketNotifyGetGoods>();
	packet->icon = icon;
	packet->count = count;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendShowSeasonRank(ui64 rakssId)
{
	auto packet = LORD::make_shared<S2CPacketShowSeasonRank>();
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendPlayerNamePerspective(ui64 rakssId, i32 entityId, bool isPerspective)
{
	auto packet = LORD::make_shared<S2CPacketPlayerNamePerspective>();
	packet->entityId = entityId;
	packet->isPerspective = isPerspective;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendAddPlayerCustomEffect(i32 entityId, String name, String effectName, float duration)
{
	auto packet = LORD::make_shared<S2CPacketAddCustomEffect>();
	packet->entityId = entityId;
	packet->name = name;
	packet->effectName = effectName;
	packet->duration = duration;
	sendPacketToTrackingPlayers(entityId, packet, true);
}

void ServerPacketSender::syncPlayerCustomEffects(ui64 rakssId, BLOCKMAN::EntityPlayer * player)
{
	auto packet = LORD::make_shared<S2CPacketSyncCustomEffect>();
	packet->entityId = player->entityId;
	packet->customEffects = player->m_custom_effects;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::syncEntityBulletin(ui64 rakssId, BLOCKMAN::EntityBulletin * entityBulletin)
{
	auto packet = LORD::make_shared<S2CPacketSyncEntityBulletin>();
	packet->entityId = entityBulletin->entityId;
	packet->bulletinId = entityBulletin->getBulletinId();

	if (rakssId != 0)
	{
		Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
	}
	else
	{
		sendPacketToTrackingPlayers(entityBulletin->entityId, packet);
	}
}

void ServerPacketSender::syncEntityBird(ui64 rakssId, BLOCKMAN::EntityBird * entityBird)
{
	auto packet = LORD::make_shared<S2CPacketSyncEntityBird>();
	packet->entityId = entityBird->entityId;
	packet->userId = entityBird->m_userId;
	packet->birdId = entityBird->m_birdId;
	packet->nameLang = entityBird->getNameLang();
	packet->actorName = entityBird->getActorName();
	packet->actorBody = entityBird->getActorBody();
	packet->actorBodyId = entityBird->getActorBodyId();
	packet->dressGlasses = entityBird->getDressGlasses();
	packet->dressHat = entityBird->getDressHat();
	packet->dressBeak = entityBird->getDressBeak();
	packet->dressWing = entityBird->getDressWing();
	packet->dressTail = entityBird->getDressTail();
	packet->dressEffect = entityBird->getDressEffect();

	if (rakssId != 0)
	{
		Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
	}
	else
	{
		sendPacketToTrackingPlayers(entityBird->entityId, packet);
	}
}

void ServerPacketSender::syncBirdSimulator(BLOCKMAN::EntityPlayer * player, int syncType)
{
	auto p = dynamic_cast<EntityPlayerMP*>(player);
	if (p&& p->m_birdSimulator)
	{
		switch ((SyncBirdSimulatorType) syncType)
		{
		case SyncBirdSimulatorType::DEFAULT:
			break;
		case SyncBirdSimulatorType::BAG:
		{
			auto packet = LORD::make_shared<S2CPacketSyncBirdBag>();
			packet->birdBag = *player->m_birdSimulator->getBag();
			Server::Instance()->getNetwork()->sendPacket(packet, p->getRaknetID(), true);
		}
		break; 
		case SyncBirdSimulatorType::ACTIVITY:
		{
			auto packet = LORD::make_shared<S2CPacketSyncBirdActivity>();
			packet->activity = *player->m_birdSimulator->getActivity();
			Server::Instance()->getNetwork()->sendPacket(packet, p->getRaknetID(), true);
		}
		break;
		case SyncBirdSimulatorType::PLAYER_INFO:
		{
			auto packet = LORD::make_shared<S2CPacketSyncBirdPlayerInfo>();
			packet->playerInfo = *player->m_birdSimulator->getPlayerInfo();
			Server::Instance()->getNetwork()->sendPacket(packet, p->getRaknetID(), true);
		}
		break;
		case SyncBirdSimulatorType::ATLAS:
		{
			auto packet = LORD::make_shared<S2CPacketSyncBirdAtlas>();
			packet->atlas = player->m_birdSimulator->getAtlas();
			Server::Instance()->getNetwork()->sendPacket(packet, p->getRaknetID(), true);
		}
		break;
		case SyncBirdSimulatorType::TAKS:
		{
			auto packet = LORD::make_shared<S2CPacketSyncBirdTask>();
			packet->tasks = player->m_birdSimulator->getTasks();
			Server::Instance()->getNetwork()->sendPacket(packet, p->getRaknetID(), true);
		}
		break;
		case SyncBirdSimulatorType::FOOD:
		{
			auto packet = LORD::make_shared<S2CPacketSyncBirdFood>();
			packet->foods = player->m_birdSimulator->getFoods();
			Server::Instance()->getNetwork()->sendPacket(packet, p->getRaknetID(), true);
		}
		break;
		case SyncBirdSimulatorType::DRESS:
		{
			auto packet = LORD::make_shared<S2CPacketSyncBirdDress>();
			packet->dress = player->m_birdSimulator->getDress();
			Server::Instance()->getNetwork()->sendPacket(packet, p->getRaknetID(), true);
		}
		break;
		case SyncBirdSimulatorType::STORE:
		{
			auto packet = LORD::make_shared<S2CPacketSyncBirdStore>();
			packet->stores = player->m_birdSimulator->getStores();
			Server::Instance()->getNetwork()->sendPacket(packet, p->getRaknetID(), true);
		}
			break;
		case SyncBirdSimulatorType::PERSONAL_STORE:
		{
			auto packet = LORD::make_shared<S2CPacketSyncBirdPersonalStore>();
			packet->personalStore = player->m_birdSimulator->getPersonalStore();
			Server::Instance()->getNetwork()->sendPacket(packet, p->getRaknetID(), true);
		}
		break;
		case SyncBirdSimulatorType::SCOPE:
		{
			auto packet = LORD::make_shared<S2CPacketSyncBirdScope>();
			packet->scopes = player->m_birdSimulator->getScopes();
			Server::Instance()->getNetwork()->sendPacket(packet, p->getRaknetID(), true);
		}
		break;
		case SyncBirdSimulatorType::TIME_PRICE:
		{
			auto packet = LORD::make_shared<S2CPacketSyncBirdTimePrice>();
			packet->timePrices = player->m_birdSimulator->getTimePrices();
			Server::Instance()->getNetwork()->sendPacket(packet, p->getRaknetID(), true);
		}
		break;
		case SyncBirdSimulatorType::BUFF:
		{
			auto packet = LORD::make_shared<S2CPacketSyncBirdBuff>();
			packet->buffs = player->m_birdSimulator->getBuffs();
			Server::Instance()->getNetwork()->sendPacket(packet, p->getRaknetID(), true);
		}
		break;
		}
	}
}

void ServerPacketSender::syncPlayerBagInfo(ui64 rakssId, BLOCKMAN::EntityPlayer* player)
{
	auto packet = LORD::make_shared<S2CPacketSyncPlayerBagInfo>();
	packet->entityId = player->entityId;
	packet->isConvert = player->m_isBirdConvert;
	packet->curCapacity = player->m_bagCapacity;
	packet->maxCapacity = player->m_bagMaxCapacity;
	if (rakssId != 0)
	{
		Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
	}
	else
	{
		sendPacketToTrackingPlayers(player->entityId, packet, true);
	}
	
}

void ServerPacketSender::sendBirdGain(ui64 rakssId, const vector<BLOCKMAN::BirdGain>::type& items)
{
	auto packet = LORD::make_shared<S2CPacketBirdGain>();
	packet->items = items;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendBirdNestOperation(ui64 rakssId, bool isUnlock, i64 birdId)
{
	auto packet = LORD::make_shared<S2CPacketBirdNestOperation>();
	packet->isUnlock = isUnlock;
	packet->birdId = birdId;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendBirdLotteryResult(ui64 rakssId, String eggBodyId, const BLOCKMAN::BirdInfo & info)
{
	auto packet = LORD::make_shared<S2CPacketBirdLotteryResult>();
	packet->eggBodyId = eggBodyId;
	packet->birdInfo = info;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::sendEntityAction(ui64 rakssId, int entityId, int entityClass, int actionStatus, int baseAction, int uperBaseAction)
{
	auto packet = LORD::make_shared<S2CPacketSyncEntityAction>();
	packet->entityId = entityId;
	packet->entityClass = entityClass;
	packet->actionStatus = actionStatus;
	packet->baseAction = baseAction;
	packet->preBaseAction = uperBaseAction;

	if (rakssId != 0)
	{
		Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
	}
	else
	{
		sendPacketToTrackingPlayers(entityId, packet);
	}
}

void ServerPacketSender::sendBirdAddScore(ui64 rakssId, i32 score, i32 scoreType)
{
	auto packet = LORD::make_shared<S2CPacketBirdAddScore>();
	packet->score = score;
	packet->scoreType = scoreType;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::onBirdSimulatorGather(ui64 userId, i64 birdId, const  BLOCKMAN::BlockPos & pos)
{
	if (Server::Instance()->getBlockFruitsManager())
	{
		Server::Instance()->getBlockFruitsManager()->onGather(userId, birdId, pos);
	}
}

void ServerPacketSender::syncEntityHealth(ui64 rakssId, i32 entityId, float health, float maxHealth)
{
	auto packet = LORD::make_shared<S2CPacketSyncEntityHealth>();
	packet->entityId = entityId;
	packet->health = health;
	packet->maxHealth = maxHealth;

	if (rakssId != 0)
	{
		Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
	}
	else
	{
		sendPacketToTrackingPlayers(entityId, packet);
	}
}

void ServerPacketSender::syncBirdSimulatorStoreItem(ui64 rakssId, const BLOCKMAN::BirdStore & store)
{
	auto packet = LORD::make_shared<S2CPacketSyncBirdSimulatorStoreItem>();
	packet->store = store;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::syncBirdSimulatorTaskItem(ui64 rakssId, bool isRemove, const BLOCKMAN::BirdTask & taskItem)
{
	auto packet = LORD::make_shared<S2CPacketSyncBirdSimulatorTaskItem>();
	packet->taskItem = taskItem;
	packet->isRemove = isRemove;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::syncBirdSimulatorPersonStoreTab(ui64 rakssId, const BLOCKMAN::BirdPersonalStoreTab & storeTab)
{
	auto packet = LORD::make_shared<S2CPacketSyncBirdSimulatorPersonStoreTab>();
	packet->storeTab = storeTab;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId, true);
}

void ServerPacketSender::changeGuideArrowStatus(ui64 rakssId, Vector3 arrowPos, bool flag)
{
	auto packet = LORD::make_shared<S2CPacketChangeGuideArrowStatus>();
	packet->arrowPos = arrowPos;
	packet->flag = flag;
	Server::Instance()->getNetwork()->sendPacket(packet, rakssId);
}

void ServerPacketSender::broadCastAddWallText(String text, Vector3 textPos, float scale, float yaw, float pitch, float r, float g, float b, float a)
{
	auto packet = LORD::make_shared<S2CPacketAddWallText>();
	packet->text = text;
	packet->textPos = textPos;
	packet->scale = scale;
	packet->yaw = yaw;
	packet->pitch = pitch;
	packet->r = r;
	packet->g = g;
	packet->b = b;
	packet->a = a;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}

void ServerPacketSender::broadCastDeleteWallText(Vector3 textPos)
{
	auto packet = LORD::make_shared<S2CPacketDeleteWallText>();
	packet->textPos = textPos;
	Server::Instance()->getNetwork()->broadCastEntityPacket(packet);
}
