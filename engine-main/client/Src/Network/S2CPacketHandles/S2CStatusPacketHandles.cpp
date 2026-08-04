#include "S2CStatusPacketHandles.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "cEntity/EntityOtherPlayerMP.h"
#include "cWorld/Blockman.h"
#include "game.h"

#include "Item/ItemStack.h"
#include "Entity/Entity.h"
#include "Entity/EntityPlayer.h"
#include "Entity/DamageSource.h"
#include "Entity/EntityItem.h"
#include "Inventory/InventoryPlayer.h"
#include "World/World.h"
#include "Util/ClientEvents.h"
#include "Entity/Enchantment.h"

using namespace BLOCKMAN;

void S2CStatusPacketHandles::handlePacket(std::shared_ptr<S2CPacketUpdateAttribute>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	EntityPlayer* pPlayer = dataCache->getPlayerByServerId(packet->m_entityRuntimeId);
	Entity* entity = dataCache->getEntityByServerId(packet->m_entityRuntimeId);
	if (pPlayer)
	{
		pPlayer->setAir(packet->m_air);
		pPlayer->getFoodStats()->setFoodLevel(packet->m_foodLevel);
		pPlayer->getFoodStats()->setFoodSaturationLevel(packet->m_foodSaturationLevel);
		if (packet->m_health > 0.0f) {
			pPlayer->setEntityHealth(packet->m_health);
		}
		else {
			PlayerDeathEvent::emit(pPlayer->entityId);
			pPlayer->playSound(pPlayer->getDeathSoundType());
			if (packet->m_entityRuntimeId == dataCache->getSelfServerId()) {
				// change to watch mode automatically
				pPlayer->setEntityHealth(0.1f);
			}
			else {
				pPlayer->setEntityHealth(0.0f);
			}
		}
	}
	else if (entity)
	{
		// TODO: NPC attribute changed
	}
	else
	{
		LordLogError("_handlePacket_UpdatePlayerAttributePacket_, can not find player");
	}
}


void S2CStatusPacketHandles::handlePacket(std::shared_ptr<S2CPacketPlayerInventory>& packet)
{
	auto inventory = BLOCKMAN::Blockman::Instance()->m_pPlayer->inventory;
	if (!inventory)
		return;

	inventory->setInventorySync(true);
	for (auto iter = packet->playerInventory.begin(); iter != packet->playerInventory.end(); iter++)
	{
		int correspondingHotbarIndex = -1;
		for (size_t index = 0; index < InventoryPlayer::HOTBAR_COUNT; ++index)
		{
			bool isAddHotbar = inventory->hotbar[index] == static_cast<ItemStackPtr>(inventory->mainInventory[iter->index]);
			if (isAddHotbar && inventory->hotbar[index] == nullptr && iter->index >= InventoryPlayer::HOTBAR_COUNT)
			{
				isAddHotbar = false;
			}
			if (isAddHotbar)
			{
				correspondingHotbarIndex = index;
				break;
			}
		}

		if (iter->index >= 0 && iter->index < InventoryPlayer::MAIN_INVENTORY_COUNT)
		{
			if (iter->valid)
			{
				int id = iter->id;
				int size = iter->size;
				int damage = iter->damage;
				int maxDamage = iter->maxDamage;
				auto newItemStack = LORD::make_shared<ItemStack>(id, size, damage);

				if (maxDamage > 0)
				{
					Item* pItem = Item::getItemById(id);
					if (pItem)
						pItem->setMaxDamage(maxDamage);
				}

				if (newItemStack && newItemStack->getGunSetting())
				{
					int MaxBulletNum = BLOCKMAN::Blockman::Instance()->m_pPlayer->getGunBulletNum(newItemStack->getGunSetting());
					newItemStack->setCurrentClipBulletNum(iter->curClipBullet);
					newItemStack->setTotalBulletNum(MaxBulletNum);
				}
				for (const auto& enchantmentInfo : iter->enchantments)
				{
					newItemStack->addEnchantment(Enchantment::enchantmentsList[enchantmentInfo.id], enchantmentInfo.level);
				}
				inventory->mainInventory[iter->index] = newItemStack;
			}
			else
			{
				inventory->mainInventory[iter->index] = nullptr;
			}
			if (iter->needAdjustStackIndex)
			{
				if (correspondingHotbarIndex >= 0)
				{
					inventory->hotbar[correspondingHotbarIndex] = inventory->mainInventory[iter->index];
				}
			}
			else
			{
				if (iter->index >= 0 && iter->index < InventoryPlayer::HOTBAR_COUNT)
					inventory->hotbar[iter->index] = inventory->mainInventory[iter->index];
			}
			
		}
		else if (iter->index >= InventoryPlayer::MAIN_INVENTORY_COUNT && iter->index < InventoryPlayer::MAIN_INVENTORY_COUNT + InventoryPlayer::ARMOR_INVENTORY_COUNT)
		{
			if (iter->valid)
			{
				int id = iter->id;
				int size = iter->size;
				int damage = iter->damage;
				int maxDamage = iter->maxDamage;
				auto newItemStack = LORD::make_shared<ItemStack>(id, size, damage);

				if (maxDamage > 0)
				{
					Item* pItem = Item::getItemById(id);
					if (pItem)
						pItem->setMaxDamage(maxDamage);
				}

				for (const auto& enchantmentInfo : iter->enchantments)
				{
					newItemStack->addEnchantment(Enchantment::enchantmentsList[enchantmentInfo.id], enchantmentInfo.level);
				}
				inventory->armorInventory[iter->index - InventoryPlayer::MAIN_INVENTORY_COUNT] = newItemStack;
			}
			else
			{
				inventory->armorInventory[iter->index - InventoryPlayer::MAIN_INVENTORY_COUNT] = nullptr;
			}
		}
	}

	if (!BLOCKMAN::Blockman::Instance()->m_pPlayer->isInventoryInited()) {
		BLOCKMAN::Blockman::Instance()->m_pPlayer->setInventoryInited(true);
		for (size_t i = 0; i < InventoryPlayer::HOTBAR_COUNT; ++i)
		{
			inventory->hotbar[i] = inventory->mainInventory[i];
		}
	}
	PlayerChangeItemEvent::emit(inventory->currentItemIndex, false);
}

void S2CStatusPacketHandles::handlePacket(std::shared_ptr<S2CPacketPoisonCircleRange>& packet)
{
	auto world = BLOCKMAN::Blockman::Instance()->getWorld();
	if (world)
	{
		BLOCKMAN::Blockman::Instance()->getWorld()->setPoisonCircleRange(
			packet->minSafeRange,
			packet->maxSafeRange,
			packet->minPoisonRange,
			packet->maxPoisonRange,
			packet->speed);
	}
}

void S2CStatusPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncPlayerMovementInput>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto entityId = dataCache->getClientId(packet->playerEntityId);
	auto pPlayer = BLOCKMAN::Blockman::Instance()->getWorld()->getPlayerEntity(entityId);
	if (!pPlayer)
		return;

	pPlayer->setMovementInput(packet->forward, packet->strafe);
}

void S2CStatusPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncPlayerRotation>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto entityId = dataCache->getClientId(packet->playerEntityId);
	auto pPlayer = BLOCKMAN::Blockman::Instance()->getWorld()->getPlayerEntity(entityId);
	if (!pPlayer)
		return;

	pPlayer->setPositionAndRotation2(pPlayer->position, packet->rotationYaw, packet->rotationPitch, 3);
}

void S2CStatusPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncPlayerJumpChanged>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto entityId = dataCache->getClientId(packet->playerEntityId);
	auto pPlayer = BLOCKMAN::Blockman::Instance()->getWorld()->getPlayerEntity(entityId);
	if (!pPlayer)
		return;

	pPlayer->setIsJump(packet->isJump);
}

void S2CStatusPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncPlayerDownChanged>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto entityId = dataCache->getClientId(packet->playerEntityId);
	auto pPlayer = BLOCKMAN::Blockman::Instance()->getWorld()->getPlayerEntity(entityId);
	if (!pPlayer)
		return;

	pPlayer->setIsDown(packet->isDown);
}

void S2CStatusPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncPlayerSetFlyingState>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto entityId = dataCache->getClientId(packet->playerEntityId);
	auto pPlayer = BLOCKMAN::Blockman::Instance()->getWorld()->getPlayerEntity(entityId);
	if (!pPlayer)
		return;

	pPlayer->capabilities.isFlying = packet->isFlying;
}

void S2CStatusPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncPlayerSpecialJump>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto entityId = dataCache->getClientId(packet->playerEntityId);
	auto pPlayer = BLOCKMAN::Blockman::Instance()->getWorld()->getPlayerEntity(entityId);
	if (!pPlayer)
		return;

	pPlayer->motion.y += packet->yFactor;

	float f1 = Math::Sin(pPlayer->rotationYaw * Math::PI / 180.0F);
	float f2 = Math::Cos(pPlayer->rotationYaw * Math::PI / 180.0F);
	float dx = pPlayer->moveStrafing * f2 - pPlayer->moveForward * f1;
	float dz = pPlayer->moveForward * f2 + pPlayer->moveStrafing * f1;
	pPlayer->motion.x += dx * packet->xFactor;
	pPlayer->motion.z += dz * packet->zFactor;
}

void S2CStatusPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncPlayerSetSpYaw>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto entityId = dataCache->getClientId(packet->playerEntityId);
	auto pPlayer = Blockman::Instance()->getWorld()->getPlayerEntity(entityId);
	if (!pPlayer)
		return;

	pPlayer->spYaw = packet->isSpYaw;
	pPlayer->spYawRadian = packet->radian;
}

