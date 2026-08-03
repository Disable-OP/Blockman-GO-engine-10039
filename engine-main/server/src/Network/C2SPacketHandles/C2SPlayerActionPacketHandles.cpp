#include "C2SPlayerActionPacketHandles.h"
#include "Network/protocol/C2SPackets.h"
#include "Network/protocol/S2CPackets.h"
#include "Network/ClientPeer.h"
#include "Network/GameCmdMgr.h"
#include "Network/RaknetInstance.h"
#include "Server.h"
#include "Network/ServerNetwork.h"
#include "Entity/EntityItem.h"
#include "Util/Random.h"
#include "Util/LanguageKey.h"
#include "Math/MathLib.h"
#include "Block/BlockManager.h"
#include "Block/Blocks.h"
#include "Entity/Enchantment.h"
#include "Item/Item.h"
#include "Item/Items.h"
#include "Entity/EntityArrow.h"
#include "Entity/EntityBullet.h"
#include "Entity/EntityPotion.h"
#include "Entity/EntityVehicle.h"
#include "Entity/EntityActorNpc.h"
#include "Entity/EntitySkillThrowable.h"
#include "Item/ItemCrafter.h"
#include "Entity/EntityMerchant.h"
#include "CommodityManager.h"
#include "Setting/GunSetting.h"
#include "World/Shop.h"
#include "Network/HttpRequest.h"
#include "Script/GameServerEvents.h"
#include "Script/Event/LogicScriptEvents.h"
#include "Inventory/IInventory.h"
#include "Setting/MultiLanTipSetting.h"
#include "Enums/Enums.h"
#include "Entity/EntityPlayer.h"
#include "Network/ManorRequest.h"
#include "Blockman/Entity/EntityCreatureAI.h"
#include "Object/Root.h"
#include "World/Ranch.h"
#include "Blockman/World/BlockCropsManager.h"
#include "Setting/CropsSetting.h"

#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
#define I64FMT "%I64d"
#define U64FMT "%I64u"
#else
#define I64FMT "%lld"
#define U64FMT "%llu"
#endif

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer,
	std::shared_ptr<C2SPacketAnimate> & packet)
{
	LordAssert(clientPeer != NULL);

	/*LordLogInfo("handlePacket_AnimatePacket from %s, type=%d, value=%d",
	clientPeer->getName().c_str(), (int32_t)packet->m_animateType, packet->m_value);*/
	auto player = clientPeer->getEntityPlayer();
	if (player == NULL)
		return;
	
	NETWORK_DEFINE::PacketAnimateType animateType = packet->m_animateType;
	if (animateType == NETWORK_DEFINE::PacketAnimateType::ANIMATE_TYPE_BASE_ACTION)
	{
		player->setBaseAction((ActionState)packet->m_value);
	}
	else
	{
		player->setUpperAction((ActionState)packet->m_value);
	}

	auto sender = Server::Instance()->getNetwork()->getSender();
	sender->sendEntityAnimateToTackingPlayers(animateType, clientPeer->getEntityRuntimeID(), packet->m_value);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer,
	std::shared_ptr<C2SPacketPlayerMovement> & packet)
{
	LordAssert(clientPeer != NULL);

	/* for debug, added by wanglei
	if (packet->m_moving) {
	LordLogDebug("recv C2SPacketPlayerMovement from %d, recv position is (%f, %f, %f)",
	clientPeer->getEntityRuntimeID(), packet->m_x, packet->m_y, packet->m_z);
	}*/

	if (!SCRIPT_EVENT::PlayerMoveEvent::invoke(clientPeer->getEntityPlayer(), packet->m_x, packet->m_y, packet->m_z))
	{
		if (packet->m_moving)
		{
			if (clientPeer->getEntityPlayer()->setToLastPosition(packet->m_x, packet->m_y, packet->m_z))
				return;
		}
	}
	
	clientPeer->movePlayer(packet->m_x, packet->m_y, packet->m_z,
		packet->m_yaw, packet->m_pitch, packet->m_onGround, packet->m_moving, packet->m_rotating);

	/* for debug, added by wanglei
	if (packet->m_moving) {
	LordLogDebug("recv C2SPacketPlayerMovement %d, after moved, now player position is (%f, %f, %f)",
	clientPeer->getEntityRuntimeID(), clientPeer->getEntityPlayer()->position.x,
	clientPeer->getEntityPlayer()->position.y, clientPeer->getEntityPlayer()->position.z);
	}*/
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer,
	std::shared_ptr<C2SPacketVehicleMovement> & packet)
{
	LordAssert(clientPeer != NULL);
	EntityPlayerMP* player = clientPeer->getEntityPlayer();

	EntityVehicle* vehicle = player->getVehicle();
	if (!vehicle)
		return;

	if (packet->m_vehicleId != vehicle->entityId)
		return;

	if (!player->isDriver())
		return;

	vehicle->setPositionAndYaw({ packet->m_x, packet->m_y, packet->m_z}, packet->m_yaw);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer,
	std::shared_ptr<C2SPacketAttack> & packet)
{
	LordAssert(clientPeer != NULL);
	
	auto player = clientPeer->getEntityPlayer();
	if (!player || player->isOnVehicle())
		return;

	clientPeer->doAttack(packet->m_targetEntityRuntimeId, packet->m_targetX, packet->m_targetY, packet->m_targetZ);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer,
	std::shared_ptr<C2SPacketCraftWithCraftingTable> &packet)
{
	ItemCrafter::Instance()->craftItem(clientPeer->getEntityPlayer(), packet->recipeId);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&clientPeer,
	std::shared_ptr<C2SPacketCraftWithHand> &packet)
{
	ItemCrafter::Instance()->craftItem(clientPeer->getEntityPlayer(), packet->recipeId);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer,
	std::shared_ptr<C2SPacketPlayerAction> &packet)
{

	switch (packet->action)
	{
	case NETWORK_DEFINE::PacketPlayerActionType::START_SNEAK:
		clientPeer->getEntityPlayer()->setSneaking(true);
		break;
	case NETWORK_DEFINE::PacketPlayerActionType::STOP_SNEAK:
		clientPeer->getEntityPlayer()->setSneaking(false);
		break;
	case NETWORK_DEFINE::PacketPlayerActionType::START_SPRINTING:
		clientPeer->getEntityPlayer()->setSprinting(true);
		break;
	case NETWORK_DEFINE::PacketPlayerActionType::STOP_SPRINTING:
		clientPeer->getEntityPlayer()->setSprinting(false);
		break;
	}

	//LordLogInfo("handlePacket_C2SPlayerAction_%s_", clientPeer->getName().c_str());
	auto sender = Server::Instance()->getNetwork()->getSender();
	sender->sendPlayerActionToTrackingPlayers(packet->action, clientPeer->getEntityPlayer()->entityId);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketSwapItemPosition> &packet)
{
	LordLogInfo("handlePacket_PacketSwapItemPosition: index1: %d, index2: %d", packet->index1, packet->index2);
	if (packet->index1 < 0 || packet->index2 < 0 || packet->index1 >= InventoryPlayer::MAIN_INVENTORY_COUNT || packet->index2 >= InventoryPlayer::MAIN_INVENTORY_COUNT)
	{
		LordLogError("PacketSwapItemPosition: Invalid index is received, index1: %d, index2: %d", packet->index1, packet->index2);
		return;
	}
	clientPeer->getEntityPlayer()->inventory->swapStacksInSlots(packet->index1, packet->index2);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketDropItemPosition> &packet)
{
	LordLogInfo("handle C2SPacketDropItemPosition: inventoryIndex: %d", packet->inventoryIndex);
	EntityPlayer* player = clientPeer->getEntityPlayer();
	InventoryPlayer* inventory = player->inventory;
	ItemStackPtr stack = clientPeer->getEntityPlayer()->inventory->getMainInventory()->getStackInSlot(packet->inventoryIndex);
	if (stack && stack->stackSize != 0)
	{
		if (!SCRIPT_EVENT::PlayerDropItemEvent::invoke(clientPeer->getRakssid(), stack->itemID, stack->itemDamage))
		{
			inventory->setInventoryDirtyIndex(packet->inventoryIndex);
			Server::Instance()->getNetwork()->getSender()->sendPlayerInventory(clientPeer->getRakssid(), clientPeer->getEntityPlayer()->getInventory());
			return;
		}

		if (!SCRIPT_EVENT::EntityItemSpawnEvent::invoke(stack->itemID, stack->itemDamage, "player.drop"))
		{
			inventory->getMainInventory()->decrStackSize(packet->inventoryIndex, stack->stackSize);
			return;
		}

		float f2 = 0.3F;
		auto position = player->position;
		//position.y += player->getEyeHeight();
		EntityItem* pResult = LordNew EntityItem(player->world, position, stack);

		pResult->position.y = pResult->position.y - 0.3f;
		pResult->delayBeforeCanPickup = 40;
		pResult->motion.x = -sin(player->rotationYaw * 0.017453292F) * cos(player->rotationPitch * 0.017453292F) * f2;
		pResult->motion.z = cos(player->rotationYaw * 0.017453292F) *cos(player->rotationPitch * 0.017453292F) * f2;
		pResult->motion.y = -sin(player->rotationPitch * 0.017453292F) * f2 + 0.1F;

		float f3 = player->world->m_Rand.nextFloat() * ((float)Math::PI * 2.0f);
		f2 = 0.02F * player->world->m_Rand.nextFloat();
		pResult->motion.x += cos(f3) * f2;
		pResult->motion.y += (player->world->m_Rand.nextFloat() - player->world->m_Rand.nextFloat()) * 0.1F;
		pResult->motion.z += sin(f3) * f2;

		if (player->world->spawnEntityInWorld(pResult))
		{
			inventory->getMainInventory()->decrStackSize(packet->inventoryIndex, stack->stackSize);
			LordLogInfo("PacketDropItemPosition:  successful id[%s] x[%s] y[%s] z[%s] motion.x[%s] motion.y[%s] motion.z[%s]",
				StringUtil::ToString(pResult->entityId).c_str(), StringUtil::ToString(pResult->position.x).c_str(), StringUtil::ToString(pResult->position.y).c_str(), StringUtil::ToString(pResult->position.z).c_str(), StringUtil::ToString(pResult->motion.x).c_str(), StringUtil::ToString(pResult->motion.y).c_str(), StringUtil::ToString(pResult->motion.z).c_str());
		}
	}
	else
	{
		LordLogError("PacketDropItemPosition:  failure");
	}
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketEquipArmor> &packet)
{
	auto entityPlayer = clientPeer->getEntityPlayer();
	auto inventory = entityPlayer->inventory;
	auto itemStack = inventory->getStackInSlot(packet->slotIndex);
	if (inventory->equipArmor(itemStack)) {
		Server::Instance()->getNetwork()->getSender()->sendEquipArmorsToTrackingPlayers(entityPlayer->entityId, { itemStack });
	}
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketUnequipArmor> &packet)
{
	auto entityPlayer = clientPeer->getEntityPlayer();
	entityPlayer->inventory->unequipArmor(entityPlayer->inventory->getStackInSlot(packet->slotIndex));
	Server::Instance()->getNetwork()->getSender()->sendUnequipArmorToTrackingPlayers(entityPlayer->entityId, packet->slotIndex);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketChangeItemInHand> &packet)
{
	auto entityPlayer = clientPeer->getEntityPlayer();

	if (!entityPlayer || !entityPlayer->inventory)
	{
		return;
	}

	if (packet->slotIndex < 0)
	{
		entityPlayer->inventory->removeCurrentItemFromHotbar();
	}
	else
	{
		entityPlayer->inventory->putItemToHotbar(packet->slotIndex);
	}

	if (entityPlayer->m_fishEntity)
	{
		ItemStackPtr currentItem = entityPlayer->inventory->getCurrentItem();

		if (!currentItem || currentItem->itemID != Item::fishingRod->itemID)
		{
			entityPlayer->m_fishEntity->setDead();
			entityPlayer->m_fishEntity = nullptr;
		}
	}
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketUseItem> &packet)
{
	auto entityPlayer = clientPeer->getEntityPlayer();
	if (entityPlayer && !entityPlayer->useItem(packet->useType, packet->pos, packet->param0)) {
		// TODO: notify client that use item failed
		LordLogError("C2SPacketUseItem failed");
		return;
	}
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketPlaceBlock> &packet)
{
	auto player = clientPeer->getEntityPlayer();
	auto inventory = player->inventory;

	ItemStackPtr currentItem = inventory->getCurrentItem();
	if (!currentItem) {
		LordLogWarning("Failed to place block, current item stack is null");
		return;
	}

	bool needReset = false;
	int meta = currentItem->itemDamage;
	
	Vector3i vec3;
	vec3.x = (int)packet->blockPosX;
	vec3.y = (int)packet->blockPosY;
	vec3.z = (int)packet->blockPosZ;

	vector<Vector3i>::type positions = currentItem->getItem()->getPlacePositions(Server::Instance()->getWorld(), vec3, packet->face);

	auto block = dynamic_cast<ItemBlock*>(currentItem->getItem());
	if (block)
	{
		//Block* pBlock = BlockManager::sBlocks[currentItem->getItem()->itemID];
		Block* pBlock = BlockManager::sBlocks[block->getBlockID()];
		meta = pBlock->onBlockPlaced(Server::Instance()->getWorld(), vec3, packet->face, packet->hitPos, meta);
	}

	if (!SCRIPT_EVENT::PlayerPlaceBlockEvent::invoke(clientPeer->getRakssid(), currentItem->getItem()->itemID, meta, vec3))
	{
		inventory->setInventoryDirtyIndex(inventory->getInventoryIndexOfCurrentItem());
		auto sendPk = LORD::make_shared<S2CPacketUpdateBlocks>();
		for (Vector3i pos : positions)
		{
			sendPk->updates.emplace_back(pos, Server::Instance()->getWorld()->getBlockId(pos), Server::Instance()->getWorld()->getBlockMeta(pos));
		}
		sendPk->updates.reserve(positions.size());
		Server::Instance()->getNetwork()->sendPacket(sendPk, clientPeer->getRakssid(), true);
		Server::Instance()->getNetwork()->getSender()->sendPlayerInventory(clientPeer->getRakssid(), clientPeer->getEntityPlayer()->getInventory());
		return;
	}

	do {
		if (block)
		{
			player->setPlacingBlock(true);
		}

		if (!currentItem->tryPlaceItemIntoWorld(player, Server::Instance()->getWorld(), BlockPos(packet->blockPosX, packet->blockPosY, packet->blockPosZ), packet->face, packet->hitPos))
		{
			LordLogWarning("Failed to place block");
			// TODO: send back failure
			needReset = true;

		}

		if (currentItem->stackSize == 0)
		{
			inventory->setCurrentItem(nullptr);
		}

		player->setPlacingBlock(false);

		break;
	} while (true);

}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketActivateBlock> &packet)
{
	BlockPos blockPos;
	blockPos.x = (int)packet->blockPosX;
	blockPos.y = (int)packet->blockPosY;
	blockPos.z = (int)packet->blockPosZ;

	auto player = clientPeer->getEntityPlayer();
	auto world = player->getWorldPtr();
	int	blockID = world->getBlockId(blockPos);

	if (blockID > 0 && BlockManager::sBlocks[blockID]->onBlockActivated(world, blockPos, player, packet->face, packet->hitPos))
	{

	}
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketExchangeItemStack> &packet)
{
	BlockPos blockPos;
	blockPos.x = (int)packet->blockPosX;
	blockPos.y = (int)packet->blockPosY;
	blockPos.z = (int)packet->blockPosZ;

	auto player = clientPeer->getEntityPlayer();
	auto world = player->getWorldPtr();
	int	blockID = world->getBlockId(blockPos);

	if (blockID > 0)
	{	
		IInventory* chestInventory = NULL;
		bool isEnderChest = false;

		if (dynamic_cast<BlockChest*>(BlockManager::sBlocks[blockID]))
		{
			chestInventory = dynamic_cast<BlockChest*>(BlockManager::sBlocks[blockID])->getInventory(world, blockPos);
			isEnderChest = false;
		}

		if (dynamic_cast<BlockEnderChest*>(BlockManager::sBlocks[blockID]))
		{
			chestInventory = (IInventory*)player->getInventoryEnderChest();
			isEnderChest = true;
		}
		
		auto playerMainInventory = player->inventory->getMainInventory();
		if (chestInventory && playerMainInventory)
		{
			if (packet->isPlayerInventoryToChest)
			{
				exchangeItemStack(clientPeer, packet, playerMainInventory, chestInventory, isEnderChest);
			}
			else
			{
				exchangeItemStack(clientPeer, packet, chestInventory, playerMainInventory, isEnderChest);
			}
		}
	}
}

void C2SPlayerActionPacketHandles::exchangeItemStack(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketExchangeItemStack> &packet,
	IInventory * initiatorInventory, IInventory * targetInventory, bool isEnderChest)
{
	IInventory* chestInventory = packet->isPlayerInventoryToChest ? targetInventory : initiatorInventory;
	for (int i = 0; i < chestInventory->getSizeInventory(); ++i)
	{
		chestInventory->setPrevInventorySlotContents(i, chestInventory->getStackInSlot(i) == nullptr ? nullptr : chestInventory->getStackInSlot(i)->copy());
	}

	BlockPos blockPos;
	blockPos.x = (int)packet->blockPosX;
	blockPos.y = (int)packet->blockPosY;
	blockPos.z = (int)packet->blockPosZ;

	int index = packet->index;
	int exchangeItemNumber = packet->touchTime == 0 ? 1 : (packet->touchTime / 25);
	ItemStackPtr selectedItemStack = initiatorInventory->getStackInSlot(index);


	bool isChange = false;
	if (selectedItemStack)
	{
		exchangeItemNumber = exchangeItemNumber > selectedItemStack->stackSize ? selectedItemStack->stackSize : exchangeItemNumber;

		if (!SCRIPT_EVENT::PlayerExchangeItemEvent::invoke(clientPeer->getRakssid(), packet->isPlayerInventoryToChest ? 1 : 2,
			selectedItemStack->itemID, selectedItemStack->itemDamage, exchangeItemNumber, selectedItemStack->m_currentClipBulletNum, blockPos))
			return;


		ItemStackPtr  targetStack = targetInventory->findCanSaveItemStackById(selectedItemStack->itemID, selectedItemStack->itemDamage);
		if (targetStack)
		{
			int  canSaveNmu = targetStack->getMaxStackSize() - targetStack->stackSize;
			if (canSaveNmu >= exchangeItemNumber)
			{
				targetStack->stackSize = targetStack->stackSize + exchangeItemNumber;
				initiatorInventory->decrStackSize(index, exchangeItemNumber);
				exchangeItemNumber = 0;
				isChange = true;
			}
			else
			{
				targetStack->stackSize = targetStack->stackSize + canSaveNmu;
				initiatorInventory->decrStackSize(index, canSaveNmu);
				exchangeItemNumber = exchangeItemNumber - canSaveNmu;
				isChange = true;
			}
		}

		if (exchangeItemNumber > 0)
		{
			for (int i = 0; i < targetInventory->getSizeInventory(); i++)
			{
				targetStack = targetInventory->getStackInSlot(i);
				if (!targetStack)
				{
					ItemStackPtr newStack = LORD::make_shared<ItemStack>(selectedItemStack->itemID, exchangeItemNumber, selectedItemStack->itemDamage);
					newStack->setTagCompound(selectedItemStack->getTagCompound() ? (NBTTagCompound*)selectedItemStack->getTagCompound()->copy() : nullptr);
					newStack->m_currentClipBulletNum = selectedItemStack->m_currentClipBulletNum;
					targetInventory->setInventorySlotContents(i, newStack);
					initiatorInventory->decrStackSize(index, exchangeItemNumber);
					isChange = true;
					break;
				}
			}
		}
	}

	if (isChange)
	{
		auto sender = Server::Instance()->getNetwork()->getSender();
		if (isEnderChest)
		{
			sender->sendUpdateEnderChestInventory(clientPeer->getRakssid(), blockPos, packet->face, packet->hitPos, chestInventory);
		}
		else
		{
			sender->sendUpdateChestInventory(blockPos, packet->face, packet->hitPos, chestInventory);
		}
	}
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketChatString> &packet)
{
	ChatComponentString* s = packet->m_chatComponent.get();
	if (s->msg.length() > 100) {
		LordLogError("recv chat msg length > 100 from %s", clientPeer->getName().c_str());
		return;
	}

	if (GameCmdMgr::consumeCmd(clientPeer, s->msg)) {
		return;
	}
	auto sender = Server::Instance()->getNetwork()->getSender();
	sender->sendChat(LORD::make_shared<ChatComponentString>(s->sendId, s->msg, clientPeer->getName()));
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketLogout> &packet)
{
	LordLogInfo("C2SPacketLogout %s logout", clientPeer->getName().c_str());
	ServerNetwork::Instance()->closeSession(clientPeer);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketUseProp> &packet)
{
	//HttpRequest::useProp(clientPeer->getRakssid() ,clientPeer->getPlatformUserId(), packet->m_propId);
	//SCRIPT_EVENT::PlayerUsePropEvent::invoke(clientPeer->getPlatformUserId(), packet->m_propId.c_str());
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketPing> &packet)
{
	ui64 raknetId = clientPeer->getRakssid();
	ui32 currServerTimestamp = Root::Instance()->getCurrentTime();
	ui32 oldServerTimestamp = Server::Instance()->getNetwork()->getServerTimestamp(raknetId);

	/*LordLogInfo("C2SPacketPing %s recved, sessionid " U64FMT ", platform id:" U64FMT
		",oldServerTimestamp: %u, currServerTimestamp: %u, server duration: %u",
		clientPeer->getName().c_str(), raknetId, clientPeer->getPlatformUserId(),
		oldServerTimestamp, currServerTimestamp, (currServerTimestamp - oldServerTimestamp));*/

	if (oldServerTimestamp == 0) {
		// for the delay of first time
		Server::Instance()->getNetwork()->addTimeDelay(raknetId, 5000, 10000);
	} else {
		// calc time delay
		if (currServerTimestamp > oldServerTimestamp && currServerTimestamp - oldServerTimestamp > 5000) {
			ui32 delay = currServerTimestamp - oldServerTimestamp - 5000;
			ui32 totalDelay = Server::Instance()->getNetwork()->addTimeDelay(raknetId, delay, 10000);
			/*LordLogInfo("accum time delay, sessionid " U64FMT ", platform id:" U64FMT
				",add delay %u, total delay:%u",
				raknetId, clientPeer->getPlatformUserId(), delay, totalDelay);*/
		}

		// check whether speed hack
		if (currServerTimestamp > oldServerTimestamp && currServerTimestamp - oldServerTimestamp < 4400) {
			ui32 need = 4400 - (currServerTimestamp - oldServerTimestamp);
			ui32 delayBeforeMinus = 0;
			if (!Server::Instance()->getNetwork()->minusTimeDelay(raknetId, need, delayBeforeMinus)) {
				ServerNetwork::Instance()->getSender()->sendGameover(raknetId, "cheat detected", 7);
				LordLogError("C2SPacketPing %s speed hack, sessionid " U64FMT ", platform id:" U64FMT
					",oldServerTimestamp: %u, currServerTimestamp: %u, server duration: %u, need: %u, delayBeforeMinus:%u",
					clientPeer->getName().c_str(), raknetId, clientPeer->getPlatformUserId(),
					oldServerTimestamp, currServerTimestamp, (currServerTimestamp - oldServerTimestamp), need, delayBeforeMinus);
			} else {
				/*LordLogInfo("consume time delay %u, sessionid " U64FMT ", platform id:" U64FMT, 
					need, raknetId, clientPeer->getPlatformUserId());*/
			}
		}
	}
	
	Server::Instance()->getNetwork()->setServerTimestamp(raknetId, currServerTimestamp);
	Server::Instance()->getNetwork()->getSender()->sendPing(raknetId, packet->m_sendTime);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketLaunchBullet> &packet)
{
	EntityPlayerMP* pPlayer = clientPeer->getEntityPlayer();
	ItemStackPtr pStack = pPlayer->inventory->getCurrentItem();
	if (!pStack)
		return;
	const GunSetting* pSetting = pStack->getGunSetting();
	if (!pSetting)
		return;

	bool bRet = pStack->consumeBullet();
	if (!bRet)
		return;

	EntityBullet* pBullet = LordNew EntityBullet(pPlayer->world, pPlayer, packet->playerPos,
		packet->playerYaw, packet->playerPitch, packet->bulletID, pSetting);

	pPlayer->world->spawnEntityInWorld(pBullet);
	auto sender = Server::Instance()->getNetwork()->getSender();
	sender->broadCastPlaySoundByTypeAtEntity(pSetting->soundType, pPlayer->entityId);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketSpawnVehicle> &packet)
{
	EntityPlayerMP* pPlayer = clientPeer->getEntityPlayer();
	ItemStackPtr pStack = pPlayer->inventory->getCurrentItem();
	if (!pStack || pStack->stackSize < 1)
		return;

	pStack->stackSize--;

	EntityVehicle* pVechile = LordNew EntityVehicle(pPlayer->world, packet->carID, packet->pos, packet->yaw);
	pPlayer->world->spawnEntityInWorld(pVechile);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketGunFireResult> &packet)
{
	EntityPlayerMP* pPlayer = clientPeer->getEntityPlayer();
	if (pPlayer->isOnVehicle())
		return;

	ItemStackPtr pStack = pPlayer->inventory->getCurrentItem();
	if (!pStack)
		return;
	const GunSetting* pSetting = pStack->getGunSetting();
	if (!pSetting)
		return;

	//(zhouyou) need reduce the bullet-num ??????
	if (packet->isNeedConsumeBullet) {
		bool bRet = pStack->consumeBullet();
		if (!bRet)
			return;
	}


	//(zhouyou) if the traceType is 2. do the damage/death/repel logic

	//if (packet->tracyType == ENTITY_PLAYER_TRACE_TYPE)
	//{
	/*
	void ClientPeer::doGunShootTarget(ui32 tracyType, ui32 hitEntityID, const C2SPACKET_DETAIL::Vector3& hitpos,
	bool isHeadshot, const GunSetting* pGunSetting)
	*/
		clientPeer->doGunShootTarget(packet->tracyType, packet->hitEntityID, packet->hitpos, packet->isHeadshot, pSetting);
	//}

	//(zhouyou) broadcast the gun fire message. so every client need render the gun-flame effect and the hit bleeding effect.
	auto sender = Server::Instance()->getNetwork()->getSender();
	/*void broadCastPlayerGunGire(i32 entityId, i32 gunId, const C2SPACKET_DETAIL::Vector3& pos, const C2SPACKET_DETAIL::Vector3& dir,
		ui32 tracyType, const C2SPACKET_DETAIL::Vector3i& blockPos, const C2SPACKET_DETAIL::Vector3& hitpos, ui32 hitEntityID, bool isHeadshot);
	*/
	sender->broadCastPlayerGunGire(pPlayer->entityId, pSetting->gunId, packet->pos, packet->dir,
		packet->tracyType, packet->blockPos, packet->hitpos, packet->hitEntityID, packet->isHeadshot);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketLaunchArrow> &packet)
{
	LordLogInfo("handle C2SPacketLaunchArrow");
	EntityPlayerMP* pPlayer = clientPeer->getEntityPlayer();
	ItemStackPtr pStack = pPlayer->inventory->getCurrentItem();
	if (pStack == nullptr)
		return;

	bool infiniteEnchantedBow = EnchantmentHelper::getEnchantmentLevel(Enchantment::infinity->effectId, pStack) > 0;
	bool infiniteUseArrow = pPlayer->capabilities.isCreativeMode || infiniteEnchantedBow;
	ItemStackPtr arrowItemStack = pPlayer->inventory->getArrowItemStack();
	if (!infiniteUseArrow && !arrowItemStack) {
		return;
	}

	if (!arrowItemStack)
		return;

	float cdTime = packet->cdTime;
	if (cdTime < 0.1f)
		return;

	if (cdTime > 1.0F)
		cdTime = 1.0F;

	EntityArrow* pArrow = LordNew EntityArrow(pPlayer->world, pPlayer, packet->playerPos,
		packet->playerYaw, packet->playerPitch, cdTime, packet->arrowID);

	ItemArrowPotion* potionArrow = dynamic_cast<ItemArrowPotion*>(arrowItemStack->getItem());
	if (potionArrow) {
		pArrow->setPotionEffectAndItem(LordNew PotionEffect(
			potionArrow->getPotionId(),
			potionArrow->getPotionDuration(),
			potionArrow->getPotionAmplifier()), arrowItemStack->getItem());
	}

	if (infiniteEnchantedBow) {
		pArrow->setFromInfiniteEnchantedBow(true);
	}

	if (cdTime == 1.0F)
		pArrow->setIsCritical(true);

	int enchLvl = EnchantmentHelper::getEnchantmentLevel(Enchantment::power->effectId, pStack);

	if (enchLvl > 0)
		pArrow->setDamage(pArrow->getDamage() + enchLvl * 0.5f + 0.5f);

	int var10 = EnchantmentHelper::getEnchantmentLevel(Enchantment::punch->effectId, pStack);

	if (var10 > 0)
		pArrow->setKnockbackStrength(var10);

	if (EnchantmentHelper::getEnchantmentLevel(Enchantment::flame->effectId, pStack) > 0)
	{
		pArrow->setFire(100);
	}

	pStack->damageItem(1, pPlayer);
	// pWorld->playSoundAtEntity(pPlayer, "random.bow", 1.0F, 1.0F / (itemRand->nextFloat() * 0.4F + 1.2F) + cdtime * 0.5F);

	if (infiniteUseArrow)
		pArrow->canBePickedUp = 2;
	else
		pPlayer->inventory->consumeInventoryItem(arrowItemStack->getItem()->itemID);


	pPlayer->world->spawnEntityInWorld(pArrow);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketSplashPotion> &packet)
{
	LordLogInfo("handle C2SPacketSplashPotion");
	EntityPlayerMP* pPlayer = clientPeer->getEntityPlayer();
	ItemStackPtr pStack = pPlayer->inventory->getCurrentItem();

	pPlayer->inventory->setInventorySlotContents(pPlayer->inventory->getInventoryIndexOfCurrentItem(), nullptr);

	auto sender = Server::Instance()->getNetwork()->getSender();
	sender->sendEntityAnimateToTackingPlayers(
		NETWORK_DEFINE::PacketAnimateType::ANIMATE_TYPE_ATTACK,
		clientPeer->getEntityRuntimeID()
	);

	EntityPotion* entityPotion = LordNew EntityPotion(pPlayer->world, pPlayer, pStack);
	entityPotion->setPosition(packet->playerPos);
	entityPotion->setHeadingFromThrower(pPlayer, packet->playerPitch, packet->playerYaw, -20.0, 0.5, 1.0);
	pPlayer->world->spawnEntityInWorld(entityPotion);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketCloseContainer> &packet)
{
	BlockPos containerPos;
	containerPos.x = (int)packet->containerPosX;
	containerPos.y = (int)packet->containerPosY;
	containerPos.z = (int)packet->containerPosZ;

	if (!SCRIPT_EVENT::PlayerCloseContainerEvent::invoke(clientPeer->getRakssid(), containerPos))
		return;
	EntityPlayerMP* player = clientPeer->getEntityPlayer();
	// todo. check the logic is correct. ie. the packet->containerPos is the chest.

	player->closeContainer();
	player->closeOpenChest();
	auto sender = Server::Instance()->getNetwork()->getSender();
	
	sender->sendCloseContainer(player->entityId, true, containerPos);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketUseThrowableItem> &packet)
{
	//LordLogInfo("handlePacket C2SPacketUseThrowableItem");

	auto player = clientPeer->getEntityPlayer();
	if (player)
	{
		ItemStackPtr itemStack = player->inventory->getCurrentItem();
		if (itemStack)
		{
			if (!SCRIPT_EVENT::PlayerUseThrowableItemEvent::invoke(itemStack->itemID))
				return;

			auto sender = Server::Instance()->getNetwork()->getSender();
			sender->sendEntityAnimateToTackingPlayers(NETWORK_DEFINE::PacketAnimateType::ANIMATE_TYPE_ATTACK, clientPeer->getEntityRuntimeID());
			if (itemStack->itemID == Item::snowball->itemID) {
				player->setThrowingSnowball(true);
			}

			itemStack->useItemRightClick(player->getWorldPtr(), player);

			player->setThrowingSnowball(false);
		}
	}
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketSetFlying> &packet)
{
	if (SCRIPT_EVENT::CheckFlyingPermissionEvent::invoke(clientPeer->getRakssid()))
	{
		auto player = clientPeer->getEntityPlayer();
		if (player->capabilities.canBeFlying())
		{
			player->capabilities.isFlying = packet->isFlying;
			auto sender = Server::Instance()->getNetwork()->getSender();
			sender->broadCastPlayerSetFlyingState(player->entityId, packet->isFlying);
		}
	}
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketBuyCommodity> &packet)
{
	auto player = clientPeer->getEntityPlayer();
	if (!player)
	{
		ServerNetwork::Instance()->getSender()->sendBuyCommodityResult(clientPeer->getRakssid(), 2, "failed");
		return;
	}
	// TODO: validate the distance between player and merchant
	auto merchant = dynamic_cast<EntityMerchant*>(Server::Instance()->getWorld()->getEntity(packet->merchantId));
	if (!merchant)
	{
		ServerNetwork::Instance()->getSender()->sendBuyCommodityResult(clientPeer->getRakssid(), 2, "failed");
		return;
	}
	CommodityMapping mapping = CommodityManager::Instance()->getMapping(packet->merchantId, player->getRaknetID());
	auto cList = CommodityManager::Instance()->getCommodityList(mapping.index);
	if (!cList)
	{
		ServerNetwork::Instance()->getSender()->sendBuyCommodityResult(clientPeer->getRakssid(), 2, "failed");
		return;
	}
	if (size_t(packet->groupIndex) >= cList->getCommodities().size())
	{
		ServerNetwork::Instance()->getSender()->sendBuyCommodityResult(clientPeer->getRakssid(), 2, "failed");
		return;
	}
	auto commodityGroup = cList->getCommodities()[packet->groupIndex];
	if (size_t(packet->commodityIndex) >= commodityGroup.size())
	{
		ServerNetwork::Instance()->getSender()->sendBuyCommodityResult(clientPeer->getRakssid(), 2, "failed");
		return;
	}
	auto commodity = commodityGroup[packet->commodityIndex];
	BuyResult result = BuyResult::BuySuccess;
	if (commodity.uniqueId != packet->commodityId || !player->canBuy(commodity, result))
	{
		ServerNetwork::Instance()->getSender()->sendBuyCommodityResult(clientPeer->getRakssid(), 2, "failed");
		return;
	}

	Commodity copy = commodity;

	CommodityInfo commodityInfo;
	commodityInfo.goodsId = commodity.itemId;
	commodityInfo.goodsNum = commodity.itemNum;
	commodityInfo.coinId = commodity.coinId;
	commodityInfo.coinNum = commodity.price;

	ShoppingAttr shoppingAttr;
	shoppingAttr.isConsumeCoin = true;
	shoppingAttr.isAddGoods = true;

	IntProxy msgType;
	msgType.value = 0;

	if (!SCRIPT_EVENT::PlayerBuyCommodityEvent::invoke(clientPeer->getRakssid(), (ui8)commodityGroup.getCategory(), &commodityInfo, &shoppingAttr, &msgType))
	{
		if (msgType.value != 0)
		{
			auto lang = clientPeer->getLanguage();
			std::string msg = MultiLanTipSetting::getMessage(lang, msgType.value, "");
			ServerNetwork::Instance()->getSender()->sendBuyCommodityResult(clientPeer->getRakssid(), 3, msg.c_str());
		}
		else
		{
			ServerNetwork::Instance()->getSender()->sendBuyCommodityResult(clientPeer->getRakssid(), 2, "failed");
		}
		return;
	}
	player->buy(copy, shoppingAttr.isConsumeCoin, shoppingAttr.isAddGoods);
	ServerNetwork::Instance()->getSender()->sendBuyCommodityResult(clientPeer->getRakssid(), 1, "success");
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer,
	std::shared_ptr<C2SPacketTakeVehicle> & packet)
{
	auto player = clientPeer->getEntityPlayer();
	if (!player)
		return;

	auto vehicle = dynamic_cast<EntityVehicle*>(Server::Instance()->getWorld()->getEntity(packet->vehicleId));
	if (!vehicle)
		return;

	bool result = false;
	bool bIsDriver = false;
	if (packet->onoff)
	{
		bIsDriver = vehicle->m_hasDriver == false;
		result = player->takeOnVehicle(vehicle, false, bIsDriver);
	}
	else
	{
		result = player->takeOffVehicle();
		player->setPosition(vehicle->position + Vector3(0, 0.5f, 0));
		Vector3 pos = player->position;
		clientPeer->setPlayerLocation(pos, player->rotationYaw, player->rotationPitch);
	}

	if (result)
	{
		// broadcast the takeon / takeoff the vehicle.
		auto sender = Server::Instance()->getNetwork()->getSender();
		sender->broadCastSyncTakeVehicle(player->entityId, packet->onoff, bIsDriver, vehicle->entityId);
	}
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer,
	std::shared_ptr<C2SReloadBullet> & packet)
{
	LordAssert(clientPeer != NULL);

	clientPeer->reloadBullet();
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer,
	std::shared_ptr<C2SPacketBuyGoods> &packet)
{
	clientPeer->buyGoods(packet->groupIndex, packet->goodsIndex, packet->goodsId);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer,
	std::shared_ptr<C2SPacketResumeOrder> &packet)
{
	String url = clientPeer->getHttpUrl();
	if (url.length() == 0)
	{
		return;
	}
	HttpRequest::resumeOrder(url, packet->orderId);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer,
	std::shared_ptr<C2SPacketSelectRole> &packet)
{
	SCRIPT_EVENT::PlayerSelectRoleEvent::invoke(clientPeer->getRakssid(), packet->role);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer,
	std::shared_ptr<C2SPacketBuyVehicle> & packet)
{
	auto player = clientPeer->getEntityPlayer();
	if (!player)
		return;

	player->tryBuyVehicle(packet->vehicleId);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer,
	std::shared_ptr<C2SPacketLockVehicle> & packet)
{
	auto player = clientPeer->getEntityPlayer();
	if (!player)
		return;

	auto vehicle = dynamic_cast<EntityVehicle*>(Server::Instance()->getWorld()->getEntity(packet->vehicleId));
	if (!vehicle)
		return;

	bool result = player->tryLockVehicle(packet->onoff);
	if (result)
	{
		// broadcast lock / unlock the vehicle by player
		auto sender = Server::Instance()->getNetwork()->getSender();
		sender->broadCastSyncLockVehicle(player->entityId, packet->onoff, vehicle->entityId);
	}
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, 
	std::shared_ptr<C2SPacketClientReady> & packet)
{
	LordLogInfo("C2SPacketClientReady---->UserId: " I64FMT " Ready", clientPeer->getPlatformUserId());
	ServerNetwork::Instance()->getSender()->sendLogicSettingInfo(clientPeer->getRakssid());
	ServerNetwork::Instance()->getSender()->sendEnableAutoShoot(clientPeer->getRakssid(), LogicSetting::Instance()->getAutoShootEnable());
	clientPeer->setClientReady(packet->m_status);
	SCRIPT_EVENT::PlayerReadyEvent::invoke(clientPeer->getRakssid());

	auto player = clientPeer->getEntityPlayer();

	if (!player)
		return;

	player->initUpdateBlocks();

	// Server::Instance()->getNetwork()->getSender()->sendUpdateBlocks(clientPeer->getRakssid());
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer,
	std::shared_ptr<C2SPacketVehicleActionState> & packet)
{
	auto player = clientPeer->getEntityPlayer();
	if (!player)
		return;

	auto vehicle = dynamic_cast<EntityVehicle*>(Server::Instance()->getWorld()->getEntity(packet->vehicleId));
	if (!vehicle)
		return;

	if (player->getVehicle() != vehicle || !player->isDriver())
		return;

	vehicle->setActionState((VehicleActionState)packet->state);
	auto sender = Server::Instance()->getNetwork()->getSender();
	sender->broadCastVehicleActionState(player->entityId, packet->vehicleId, packet->state);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer,
	std::shared_ptr<C2SPacketBuyRespawn> & packet)
{
	clientPeer->buyRespawn(packet->isRespawn, packet->uniqueId, packet->index);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer,
	std::shared_ptr<C2SPacketGoNpcMerchant> & packet)
{
	clientPeer->resetPos(packet->x, packet->y, packet->z, packet->yaw);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer,
	std::shared_ptr<C2SPacketClickTeleport> & packet)
{
	SCRIPT_EVENT::PlayerClickTeleportEvent::invoke(clientPeer->getRakssid());
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer,
std::shared_ptr<C2SPacketLeaveAircraft> & packet)
{
	auto player = clientPeer->getEntityPlayer();
	if (!player)
		return;
	player->takeOffAircraft();
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer,
	std::shared_ptr<C2SPacketChangeClothes> & packet)
{
	auto player = clientPeer->getEntityPlayer();
	if (!player)
		return;

	player->changeClothes(packet->partName, packet->actorId);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer,
	std::shared_ptr<C2SPacketRestClothes> & packet)
{
	auto player = clientPeer->getEntityPlayer();
	if (!player)
		return;
	player->resetClothes(clientPeer->getPlatformUserId());
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer,
	std::shared_ptr<C2SPacketFriendOperation> & packet)
{
	auto s2cPacket = LORD::make_shared<S2CPacketFriendOperation>();
	s2cPacket->operationType = packet->operationType;
	s2cPacket->userId = clientPeer->getPlatformUserId();
	auto player = Server::Instance()->getNetwork()->findPlayerByPlatformUserId(packet->userId);
	if (player)
	{
		Server::Instance()->getNetwork()->sendPacket(s2cPacket, player->getRakssid(), true);
	}
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketSetParachute> &packet)
{
	auto player = clientPeer->getEntityPlayer();
	if (player)
	{
		player->capabilities.isParachute = packet->isParachute;
	}
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketSkillType> &packet)
{
	SCRIPT_EVENT::PlayerSkillTypeEvent::invoke(clientPeer->getRakssid(), packet->skillType, packet->skillStatus);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketClickChangeActor> &packet)
{
	auto player = clientPeer->getEntityPlayer();
	if (!player)
		return;
	SCRIPT_EVENT::PlayerClickChangeActorEvent::invoke(clientPeer->getRakssid());
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketChangePlayerActor> &packet) 
{
	auto player = clientPeer->getEntityPlayer();
	if (!player)
		return;
	SCRIPT_EVENT::PlayerChangeActorEvent::invoke(clientPeer->getRakssid());
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketCameraLockMovement> &packet)
{
	auto player = clientPeer->getEntityPlayer();
	if (player)
	{
		player->m_cameraLockPos = packet->nextPosition;
	}
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketCameraLock> &packet)
{
	auto player = clientPeer->getEntityPlayer();
	player->setCameraLocked(packet->isLocked);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketPickUpItemPay> &packet)
{
	auto  entity = Server::Instance()->getWorld()->getEntity(packet->itemEntityId);
	if (!entity)
	{
		ServerNetwork::Instance()->getSender()->sendRequestResult(clientPeer->getRakssid(), packet->itemId, ResultCode::ITEM_NOT_EXIST, "entity not exit");
		return;
	}
	SCRIPT_EVENT::PlayerBuyPickupItemEvent::invoke(clientPeer->getRakssid(), packet->itemId, packet->itemEntityId);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketAttackActorNpc> &packet)
{
	auto player = clientPeer->getEntityPlayer();
	auto actorNpc = dynamic_cast<EntityActorNpc*>(Server::Instance()->getWorld()->getEntity(packet->entityId));
	if (!actorNpc)
		return;
	player->attackActorNpc(actorNpc, PLAYER_ATTACK_DAMAGE);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketGetManor> &packet)
{
	if (clientPeer)
	{
		ManorRequest::receiveManor(clientPeer->getPlatformUserId(), clientPeer->getName());
	}
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketSellManor> &packet)
{
	ManorRequest::sellManor(clientPeer->getPlatformUserId());
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketUpgradeManor> &packet)
{
	clientPeer->upgradeManor();
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketBuyHouse> &packet)
{
	clientPeer->buyHouse(packet->m_house.id);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketBuyFurniture> &packet)
{
	clientPeer->buyFurniture(packet->m_furniture.id);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketDeleteManorMessage> &packet)
{
	ManorRequest::removeManorMessageById(clientPeer->getPlatformUserId(), packet->m_message.id);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketBuildHouse> &packet)
{
	SCRIPT_EVENT::PlayerBuildHouseEvent::invoke(clientPeer->getRakssid(), packet->m_house.id);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketPutFurniture> &packet)
{
	SCRIPT_EVENT::PlayerPutFurnitureEvent::invoke(clientPeer->getRakssid(), packet->m_furniture.id, packet->m_status);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketRecycleFurniture> &packet)
{
	SCRIPT_EVENT::PlayerRecycleFurnitureEvent::invoke(clientPeer->getRakssid(), packet->m_furniture.id);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketManorTeleport> &packet)
{
	SCRIPT_EVENT::PlayerManorTeleportEvent::invoke(clientPeer->getRakssid());
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketCallOnManor> &packet)
{
	if (!SCRIPT_EVENT::PlayerCallOnTargetManorEvent::invoke(clientPeer->getPlatformUserId(), packet->m_targetUserId))
	{
		ManorRequest::checkHasManor(clientPeer->getPlatformUserId(), packet->m_targetUserId);
	}

}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketGiveRose> &packet)
{
	ManorRequest::praiseManor(clientPeer->getPlatformUserId(), packet->m_targetUserId);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketRequestUpdateRealTimeRank> &packet)
{
	SCRIPT_EVENT::PlayerUpdateRealTimeRankEvent::invoke(clientPeer->getRakssid());
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketBuySwitchableProp> &packet)
{
	IntProxy msgType;
	msgType.value = 0;
	SCRIPT_EVENT::PlayerBuySwitchablePropEvent::invoke(clientPeer->getRakssid(), packet->uniqueId, &msgType);
	if (msgType.value != 0)
	{
		auto lang = clientPeer->getLanguage();
		std::string msg = MultiLanTipSetting::getMessage(lang, msgType.value, "");
		ServerNetwork::Instance()->getSender()->sendPersonalShopTip(clientPeer->getRakssid(), msg.c_str());
	}
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketBuyUpgradeProp> &packet)
{
	IntProxy msgType;
	msgType.value = 0;
	SCRIPT_EVENT::PlayerBuyUpgradePropEvent::invoke(clientPeer->getRakssid(), packet->uniqueId, &msgType);
	if (msgType.value != 0)
	{
		auto lang = clientPeer->getLanguage();
		std::string msg = MultiLanTipSetting::getMessage(lang, msgType.value, "");
		ServerNetwork::Instance()->getSender()->sendPersonalShopTip(clientPeer->getRakssid(), msg.c_str());
	}
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketAttackCreatureEntity> &packet)
{
	auto player = clientPeer->getEntityPlayer();
	auto creatureEntity = dynamic_cast<EntityCreatureAI*>(Server::Instance()->getWorld()->getEntity(packet->entityId));
	if (!creatureEntity)
		return;
	player->attackCreatureEntity(creatureEntity,(int)PLAYER_ATTACK_CREATURE_NONE);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketUpgradeResource> &packet)
{
	auto player = clientPeer->getEntityPlayer();
	player->upgradeResource(packet->resourceId);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketWatchRespawn> &packet)
{
	SCRIPT_EVENT::PlayerWatchRespawnEvent::invoke(clientPeer->getRakssid());
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketMovementInput> &packet)
{
	auto player = clientPeer->getEntityPlayer();
	if (player->m_isCameraLocked || player->isOnAircraft() || (player->isOnVehicle() && !player->isDriver()))
		return;

	auto sender = Server::Instance()->getNetwork()->getSender();
	if (!SCRIPT_EVENT::PlayerMoveEvent::invoke(clientPeer->getEntityPlayer(), player->position.x, player->position.y, player->position.z))
	{
		clientPeer->getEntityPlayer()->setToLastPosition(player->position.x + 1, 0, 0);
		player->setMovementInput(0.0f, 0.0f);
		return;
	}

	player->setMovementInput(packet->forward, packet->strafe);
	if (!player->isWatchMode())
	{
		sender->broadCastPlayerMovementInput(player->entityId);
		if (player->isOnVehicle())
		{
			if (player->isDriver())
			{
				auto vehicle = player->getVehicle();
				if (vehicle != NULL)
					sender->broadCastEntityTeleport(vehicle->entityId);
			}
		}
		else
		{
			sender->broadCastPlayerMovement(player->entityId, false);
		}
	}
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketSendJumpChanged> &packet)
{
	auto player = clientPeer->getEntityPlayer();
	player->setIsJump(packet->isJump);
	auto sender = Server::Instance()->getNetwork()->getSender();

	if (!player->isWatchMode())
	{
		sender->broadCastPlayerJumpChanged(player->entityId);
		sender->broadCastPlayerMovement(player->entityId, false);
	}
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketSendPlayerRotation> &packet)
{
	auto player = clientPeer->getEntityPlayer();
	if (player == NULL || player->isOnAircraft())
		return;

	clientPeer->setPlayerLocation(player->position, packet->yaw, packet->pitch);

	if (!player->isWatchMode())
	{
		auto sender = ServerNetwork::Instance()->getSender();
		if (player->isOnVehicle())
		{
			if (player->isDriver())
			{
				auto vehicle = player->getVehicle();
				if (vehicle != NULL)
				{
					sender->broadCastPlayerRotation(player->entityId);
					sender->broadCastEntityTeleport(vehicle->entityId);
				}
			}
		}
		else
		{
			sender->broadCastPlayerRotation(player->entityId);
			sender->broadCastPlayerMovement(player->entityId, false);
		}
	}
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketSendDownChanged> &packet)
{
	auto player = clientPeer->getEntityPlayer();

	player->setIsDown(packet->isDown);
	auto sender = Server::Instance()->getNetwork()->getSender();

	if (!player->isWatchMode())
	{
		sender->broadCastPlayerDownChanged(player->entityId);
		sender->broadCastPlayerMovement(player->entityId, false);
	}
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketNeedUpdateMerchantCommodities> &packet)
{
	auto player = clientPeer->getEntityPlayer();
	if (!player) { return; }

	auto entityMerchant = dynamic_cast<EntityMerchant*>(Server::Instance()->getWorld()->getEntity(packet->merchantId));
	if (entityMerchant)
	{
		CommodityMapping mapping = CommodityManager::Instance()->getMapping(entityMerchant->entityId, player->getRaknetID());
		Server::Instance()->getNetwork()->getSender()->sendUpdateMerchantCommodities(mapping.rakssId, mapping.entityId, mapping.index);
	}
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketSyncCustomTipResult> &packet)
{
	SCRIPT_EVENT::CustomTipMsgEvent::invoke(clientPeer->getRakssid(), packet->extra, packet->bResult);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketRelaseItemSkill> &packet)
{
	auto pPlayer = clientPeer->getEntityPlayer();
	ItemStackPtr itemStack = pPlayer->inventory->getCurrentItem();
	if (!pPlayer || !itemStack)
		return;
	if (itemStack && itemStack->itemID == packet->skillItemId)
	{
		if (itemStack->isThrowItemSkill())
		{
			SkillItem* pSkillItem = LogicSetting::Instance()->getSkillItem(packet->skillItemId, pPlayer->getOccupation());
			if (pSkillItem)
			{
				SCRIPT_EVENT::ItemSkillConsumptionItemEvent::invoke(clientPeer->getRakssid(), pSkillItem->ItemId);

				EntitySkillThrowable* entitySkillThrowable = LordNew EntitySkillThrowable(Server::Instance()->getWorld(), pPlayer);
				entitySkillThrowable->setSkillId(pSkillItem->SkillId);
				entitySkillThrowable->setReleaseDistance(pSkillItem->ItemSkillDistace);
				entitySkillThrowable->setMoveSpeed(pSkillItem->MoveSpeed);
				entitySkillThrowable->setDropSpeed(pSkillItem->DropSpeed);
				entitySkillThrowable->setIsParabola(pSkillItem->Parabola);
				entitySkillThrowable->setAtlasName(pSkillItem->AtlasName);
				entitySkillThrowable->setItemIconName(pSkillItem->ItemIconName);
				entitySkillThrowable->setItemSkillId(pSkillItem->ItemId);
				entitySkillThrowable->setEffectName(pSkillItem->EffectName);
				entitySkillThrowable->setHeadingFromThrower(pPlayer, pPlayer->rotationPitch, pPlayer->rotationYaw, 0.0, 2.0, 0.0);
				entitySkillThrowable->position.y += pPlayer->getEyeHeight();
				Server::Instance()->getWorld()->spawnEntityInWorld(entitySkillThrowable);
			}
		}
		else
		{
			SCRIPT_EVENT::ItemSkillAttackEvent::invoke(pPlayer->entityId, packet->skillItemId, pPlayer->position);
		}
	}
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketBuildWarOpenShop> &packet)
{
	SCRIPT_EVENT::BuildWarOpenShopEvent::invoke(clientPeer->getRakssid());
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketBuildWarGrade> &packet)
{
	SCRIPT_EVENT::BuildWarGradeEvent::invoke(clientPeer->getRakssid(), packet->grade);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketBuildWarOpenGuessResult> &packet)
{
	SCRIPT_EVENT::BuildWarOpenGuessResultEvent::invoke(clientPeer->getRakssid());
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketBuildWarGuessVisit> &packet)
{
	SCRIPT_EVENT::BuildWarGuessVisitEvent::invoke(clientPeer->getRakssid(), packet->m_platformUserId);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketBuildWarGuessSuc> &packet)
{
	SCRIPT_EVENT::BuildWarGuessSucEvent::invoke(clientPeer->getRakssid(), packet->guessRoomId);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketShowBuildWarResult> &packet)
{
	SCRIPT_EVENT::BuildWarShowResultEvent::invoke(clientPeer->getRakssid());
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer,
	std::shared_ptr<C2SPacketGetCommodity> & packet)
{
	auto player = clientPeer->getEntityPlayer();
	if (!player)
	{
		ServerNetwork::Instance()->getSender()->sendBuyCommodityResult(clientPeer->getRakssid(), 2, "failed");
		return;
	}
	// TODO: validate the distance between player and merchant
	auto merchant = dynamic_cast<EntityMerchant*>(Server::Instance()->getWorld()->getEntity(packet->merchantId));
	if (!merchant)
	{
		ServerNetwork::Instance()->getSender()->sendBuyCommodityResult(clientPeer->getRakssid(), 2, "failed");
		return;
	}
	CommodityMapping mapping = CommodityManager::Instance()->getMapping(packet->merchantId, player->getRaknetID());

	auto commodityList = CommodityManager::Instance()->getCommodityList(mapping.index);
	if (!commodityList)
	{
		ServerNetwork::Instance()->getSender()->sendBuyCommodityResult(clientPeer->getRakssid(), 2, "failed");
		return;
	}
	if (size_t(packet->groupIndex) >= commodityList->getCommodities().size())
	{
		ServerNetwork::Instance()->getSender()->sendBuyCommodityResult(clientPeer->getRakssid(), 2, "failed");
		return;
	}
	auto commodityGroup = commodityList->getCommodities()[packet->groupIndex];
	if (size_t(packet->commodityIndex) >= commodityGroup.size())
	{
		ServerNetwork::Instance()->getSender()->sendBuyCommodityResult(clientPeer->getRakssid(), 2, "failed");
		return;
	}
	auto commodity = commodityGroup[packet->commodityIndex];
	BuyResult result = BuyResult::BuySuccess;
	if (commodity.uniqueId != packet->commodityId || !player->canGet(commodity, result) || (commodity.price && !player->isUnlocked(commodity.itemId, commodity.itemMeta)))
	{
		if (result == BuyResult::HaveGotGood)
		{
			ServerNetwork::Instance()->getSender()->sendBuyCommodityResult(clientPeer->getRakssid(), 4, "failed");
		}
		else
		{
			ServerNetwork::Instance()->getSender()->sendBuyCommodityResult(clientPeer->getRakssid(), 2, "failed");
		}
		return;
	}

	player->buy(commodity, false, true);
	ServerNetwork::Instance()->getSender()->sendBuyCommodityResult(clientPeer->getRakssid(), 1, "success");
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer,
	std::shared_ptr<C2SPacketUnlockCommodity> & packet)
{
	auto player = clientPeer->getEntityPlayer();
	if (!player)
		return;
	// TODO: validate the distance between player and merchant
	auto merchant = dynamic_cast<EntityMerchant*>(Server::Instance()->getWorld()->getEntity(packet->merchantId));
	if (!merchant)
	{
		ServerNetwork::Instance()->getSender()->sendBuyCommodityResult(clientPeer->getRakssid(), 2, "failed");
		return;
	}
	CommodityMapping mapping = CommodityManager::Instance()->getMapping(packet->merchantId, player->getRaknetID());
	auto commodityList = CommodityManager::Instance()->getCommodityList(mapping.index);
	if (!commodityList)
	{
		ServerNetwork::Instance()->getSender()->sendBuyCommodityResult(clientPeer->getRakssid(), 2, "failed");
		return;
	}
	if (size_t(packet->groupIndex) >= commodityList->getCommodities().size())
	{
		ServerNetwork::Instance()->getSender()->sendBuyCommodityResult(clientPeer->getRakssid(), 2, "failed");
		return;
	}
	auto commodityGroup = commodityList->getCommodities()[packet->groupIndex];
	if (size_t(packet->commodityIndex) >= commodityGroup.size())
	{
		ServerNetwork::Instance()->getSender()->sendBuyCommodityResult(clientPeer->getRakssid(), 2, "failed");
		return;
	}
	auto commodity = commodityGroup[packet->commodityIndex];
	BuyResult result = BuyResult::BuySuccess;
	if (commodity.uniqueId != packet->commodityId || !player->canUnlock(commodity, result))
	{
		ServerNetwork::Instance()->getSender()->sendBuyCommodityResult(clientPeer->getRakssid(), 2, "failed");
		return;
	}
	SCRIPT_EVENT::PlayerUnlockCommodityEvent::invoke(player->getRaknetID(), commodity.itemId, commodity.itemMeta);
	player->addUnlockedCommodity(commodity.itemId, commodity.itemMeta);
	player->buy(commodity, true, false);
	ServerNetwork::Instance()->getSender()->sendUnlockedCommodity(clientPeer->getRakssid(), packet->merchantId, commodity.itemId, commodity.itemMeta);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketPlayerInteractionActionMesage> &packet)
{
	auto targetClientPeer =ServerNetwork::Instance()->findPlayerByPlatformUserId(packet->m_targetUserId);
	if (targetClientPeer)
	{
		auto chatComponentTranslation =LORD::make_shared<ChatComponentTranslation>();
		for (auto item : packet->args)
		{
			chatComponentTranslation->formatArgs.push_back(FormatArgs(item));
		}
		chatComponentTranslation->key = packet->m_langKey;
		ServerNetwork::Instance()->getSender()->sendChat(targetClientPeer->getRakssid(), chatComponentTranslation);
	}
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketAttackEntity> &packet)
{
	auto player = clientPeer->getEntityPlayer();
	auto entity = Server::Instance()->getWorld()->getEntity(packet->entityId);
	if (!entity)
	{
		return;
	}
	player->attackEntity(entity);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketConsumeTipResult> &packet)
{
	SCRIPT_EVENT::ConsumeTipMsgEvent::invoke(clientPeer->getRakssid(), packet->extra, packet->bResult);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketBuyEnchantmentProp> &packet)
{
	IntProxy msgType;
	msgType.value = 0;
	SCRIPT_EVENT::PlayerBuyEnchantmentPropEvent::invoke(clientPeer->getRakssid(), packet->uniqueId, &msgType);
	if (msgType.value != 0)
	{
		auto lang = clientPeer->getLanguage();
		std::string msg = MultiLanTipSetting::getMessage(lang, msgType.value, "");
		ServerNetwork::Instance()->getSender()->sendPersonalShopTip(clientPeer->getRakssid(), msg.c_str());
	}
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketBuySuperProp> &packet)
{
	IntProxy msgType;
	msgType.value = 0;
	SCRIPT_EVENT::PlayerBuySuperPropEvent::invoke(clientPeer->getRakssid(), packet->uniqueId, &msgType);
	if (msgType.value != 0)
	{
		auto lang = clientPeer->getLanguage();
		std::string msg = MultiLanTipSetting::getMessage(lang, msgType.value, "");
		ServerNetwork::Instance()->getSender()->sendSuperShopTip(clientPeer->getRakssid(), msg.c_str());
	}
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketBuyCustomProp> &packet)
{
	IntProxy msgType;
	msgType.value = 0;
	SCRIPT_EVENT::PlayerBuyCustomPropEvent::invoke(clientPeer->getRakssid(), packet->uniqueId, &msgType);
	if (msgType.value != 0)
	{
		auto lang = clientPeer->getLanguage();
		std::string msg = MultiLanTipSetting::getMessage(lang, msgType.value, "");
		ServerNetwork::Instance()->getSender()->sendPersonalShopTip(clientPeer->getRakssid(), msg.c_str());
	}
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketClickBackHall> &packet)
{
	SCRIPT_EVENT::PlayerBackHallEvent::invoke(clientPeer->getRakssid());
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketLongHitEntity> &packet)
{
	auto entity = Server::Instance()->getWorld()->getEntity(packet->entityId);
	if (!entity)
	{
		return;
	}
	SCRIPT_EVENT::PlayerLongHitEntityEvent::invoke(clientPeer->getRakssid(), entity->entityId);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketUseFishingRod> &packet)
{
	auto player = clientPeer->getEntityPlayer();
	if (player && player->inventory)
	{
		ItemStackPtr itemStack = player->inventory->getCurrentItem();
		if (itemStack)
		{
			if (itemStack->itemID == Item::fishingRod->itemID)
			{
				itemStack->useItemRightClick(player->getWorldPtr(), player);
			}
		}
	}
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketUseCannon> &packet)
{
	auto player = clientPeer->getEntityPlayer();
	auto entity = Server::Instance()->getWorld()->getEntity(packet->entityId);
	if (!player || !entity)
	{
		return;
	}
	EntityActorCannon* cannon = dynamic_cast<EntityActorCannon*>(entity);
	if (!cannon)
	{
		return;
	}

	if (cannon->position.distanceTo(player->position) > 4.0f)
	{
		return;
	}

	Server::Instance()->getNetwork()->getSender()->sendSyncCannonFireToTrackingPlayers(player->entityId, cannon->entityId);

	player->setPositionAndRotation(cannon->m_launchPoint, player->rotationYaw, player->rotationPitch);
	player->onGround = false;
	player->m_isPulled = true;
	player->motion = cannon->m_motion;
	player->m_remainingFlyTicksByPulled = cannon->m_remainingFlyTicks;
	player->m_flyingByPulled = true;
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketBuyBullet> &packet)
{
	SCRIPT_EVENT::PlayerBuyBulletEvent::invoke(clientPeer->getRakssid());
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketRanchBuyItem> &packet)
{
	clientPeer->buyRanchItem(packet->itemId, packet->num, packet->totalPrice, packet->currencyType);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketRanchBuyBuildItem> &packet)
{
	clientPeer->buyRanchBuildItem(packet->itemId, packet->num, packet->type, packet->currencyType);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketRanchOperation> &packet)
{
	switch (packet->m_type)
	{

	case RanchOperationType::HOUSE_UPGRADE:
		SCRIPT_EVENT::PlayerRanchUpgradeHouseEvent::invoke(clientPeer->getRakssid());
		break;

	case RanchOperationType::STORAGE_UPGRADE:
		SCRIPT_EVENT::PlayerRanchUpgradeStorageEvent::invoke(clientPeer->getRakssid());
		break;

	case RanchOperationType::USE_CUBE_UPGRADE_STORAGE:
		SCRIPT_EVENT::PlayerRanchUseCubeUpgradeStorageEvent::invoke(clientPeer->getRakssid());
		break;
	}
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketRanchReceive> &packet)
{
	SCRIPT_EVENT::PlayerRanchReceiveEvent::invoke(clientPeer->getRakssid(), packet->referrer);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketRanchExpand> &packet)
{
	SCRIPT_EVENT::PlayerRanchExpandEvent::invoke(clientPeer->getRakssid(), packet->landCode);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketRanchShortcut> &packet)
{
	IntProxy money;
	money.value = 0;

	BoolProxy isSuccess;
	isSuccess.value = false;

	SCRIPT_EVENT::GetRanchShortcutCostEvent::invoke(clientPeer->getRakssid(), packet->landCode, &isSuccess, &money);

	if (isSuccess.value == false)
	{
		return;
	}

	if (money.value == 0)
	{
		SCRIPT_EVENT::PlayerRanchShortcutEvent::invoke(clientPeer->getRakssid(), true, packet->landCode);
		return;
	}

	clientPeer->ranchShortcut(packet->landCode, packet->moneyType, money.value);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketRanchDestroyHouse> &packet)
{
	SCRIPT_EVENT::PlayerRanchDestroyHouseEvent::invoke(clientPeer->getRakssid());
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketRanchSellItem> &packet)
{
	SCRIPT_EVENT::PlayerRanchSellItemEvent::invoke(clientPeer->getRakssid(), packet->itemId, packet->num);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketRanchBuyItems> &packet)
{
	clientPeer->buyRanchItems(packet->items);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketRanchBuildTakeProduct> &packet)
{
	//C2SPacketRanchBuildTakeProduct
	SCRIPT_EVENT::PlayerTakeAwayRanchBuildCargoEvent::invoke(clientPeer->getRakssid(), packet->entityId, packet->itemId, packet->index);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketRanchBuildQueueOperation> &packet)
{
	//type: 1 -> production,  2 -> speed up, 3 -> unlockQueue 4->use cube production
	clientPeer->ranchBuildQueueOperation(packet->entityId, packet->type, packet->queueId, packet->productId);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketRanchBuildRemove> &packet)
{
	SCRIPT_EVENT::RemoveItemBuildingEvent::invoke(clientPeer->getRakssid(), packet->actorId);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketRanchOrderOperate> &packet)
{
	//type: 1-> ask for help, 2-> put into, 3-> receive, 4-> refresh
	switch (packet->type)
	{
	case 1:
		SCRIPT_EVENT::PlayerAskForHelpRanchOrderEvent::invoke(clientPeer->getRakssid(), packet->orderId, packet->index);
		break;
	case 2:
		SCRIPT_EVENT::PlayerPutIntoRanchOrderCargoEvent::invoke(clientPeer->getRakssid(), packet->orderId, packet->index);
		break;
	case 3:
		SCRIPT_EVENT::PlayerReceiveRanchOrderCargoEvent::invoke(clientPeer->getRakssid(), packet->orderId, packet->index);
		break;
	case 4:
		SCRIPT_EVENT::PlayerRefreshRanchOrderEvent::invoke(clientPeer->getRakssid(), packet->orderId, packet->index);
		break;
	}
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketRancherExBegin> &packet)
{
	SCRIPT_EVENT::RancherExBeginEvent::invoke(clientPeer->getRakssid());
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketRancherExTaskFinish> &packet)
{
	SCRIPT_EVENT::RancherExTaskFinishEvent::invoke(clientPeer->getRakssid(), packet->itemId, packet->itemNum);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketRanchGiftsOrInvite> &packet)
{
	// type: -> 1 gifts, 2 invite
	switch (packet->type)
	{
	case 1:
		SCRIPT_EVENT::PlayerRanchSendEveryDayGiftEvent::invoke(clientPeer->getRakssid(), packet->targetUserId);
		break;
	case 2:
		SCRIPT_EVENT::PlayerRanchSendInviteJoinGameEvent::invoke(clientPeer->getRakssid(), packet->targetUserId);
		break;
	}
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketRanchHelpFinish> &packet)
{
	SCRIPT_EVENT::PlayerRanchHelpFinishEvent::invoke(clientPeer->getRakssid(), (ui64)packet->helpId, (i32) packet->type);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketReceiveMailReward> &packet)
{
	SCRIPT_EVENT::PlayerRanchReceiveMailRewardEvent::invoke(clientPeer->getRakssid(), (ui64)packet->mailId);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketReceiveRanchAchievementReward> &packet)
{
	SCRIPT_EVENT::PlayerRanchReceiveAchievementRewardEvent::invoke(clientPeer->getRakssid(), packet->achievementId);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketRanchGoExplore> &packet)
{
	SCRIPT_EVENT::PlayerRanchGoExploreEvent::invoke(clientPeer->getRakssid());
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketChangeCurrentItemInfo> &packet)
{
	SCRIPT_EVENT::ChangeCurrentItemInfoEvent::invoke(clientPeer->getRakssid(), packet->itemIndex);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketSpeedUpCrop> &packet)
{
	auto item = CropsSetting::getCropsSetting(packet->blockId);
	if (item)
	{
		if (!Server::Instance()->getBlockCropsManager()->findCrop(packet->blockPos))
		{
			ServerNetwork::Instance()->getSender()->sendRequestResult(clientPeer->getRakssid(), packet->blockId, ResultCode::COMMON, "gui_ranch_speed_up_error_crops_not_exist");
			return;
		}

		if (Server::Instance()->getBlockCropsManager()->isCropsComplete(packet->blockPos))
		{
			ServerNetwork::Instance()->getSender()->sendRequestResult(clientPeer->getRakssid(), packet->blockId, ResultCode::COMMON, "gui_ranch_speed_up_error_crops_complete");
			return;
		}


		if (!clientPeer->getEntityPlayer()->m_ranch->getStorage()->canSave(item->minQuantity))
		{
			ServerNetwork::Instance()->getSender()->sendRequestResult(clientPeer->getRakssid(), packet->blockId, ResultCode::RANCH_STORAGE_FULL, "gui_ranch_storage_full_operation_failure");
			return;
		}

		auto cropInfo = Server::Instance()->getBlockCropsManager()->findCrop(packet->blockPos);
		i32 totalTime = item->getTotalTime();
		i32 curTime = cropInfo->curStageTime;
		for (int i = 1; i < cropInfo->curStage; i++)
		{
			curTime += item->getStateTime(i);
		}
		i32 timeLeft = totalTime - curTime;
		i32 price = clientPeer->getEntityPlayer()->m_ranch->getTimePrice(timeLeft);
		clientPeer->speedUpCrop(packet->blockPos, packet->blockId, price);
	}
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketUseCubeFinshOrder> &packet)
{
	clientPeer->useCubeFinshOrder(packet->orderId, packet->index);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketBuyFlyingPermissionResult> &packet)
{
	SCRIPT_EVENT::BuyFlyingPermissionResultEvent::invoke(clientPeer->getRakssid(), packet->isSuccessful);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketResetCloseup> &packet)
{
	auto player = clientPeer->getEntityPlayer();
	if (!player)
	{
		return;
	}
	player->setEntityInvulnerable(false);
	clientPeer->resetPos(packet->playerPos[0], packet->playerPos[1], packet->playerPos[2]);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer,
	std::shared_ptr<C2SPacketKeepItem> & packet)
{
	SCRIPT_EVENT::PlayerKeepItemEvent::invoke(clientPeer->getRakssid(), packet->isKeepItem);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketOpenEnchantMent> &packet)
{
	SCRIPT_EVENT::OpenEnchantmentEvent::invoke(clientPeer->getRakssid());
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketEnchantmentEffectClick> &packet)
{
	SCRIPT_EVENT::EnchantmentEffectClickEvent::invoke(clientPeer->getRakssid(), packet->equipId, packet->consumeId, packet->index);
}


void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer,
	std::shared_ptr<C2SPacketEnchantmentQuick> & packet)
{
	SCRIPT_EVENT::PlayerEnchantmentQuickEvent::invoke(clientPeer->getRakssid(), packet->equipId, packet->effectIndex);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketSelectLottery> &packet)
{
	SCRIPT_EVENT::PlayerSelectLotteryEvent::invoke(clientPeer->getRakssid(), packet->uniqueId);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketLotteryAgain> &packet)
{
	SCRIPT_EVENT::PlayerLotteryAgainEvent::invoke(clientPeer->getRakssid());
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketHallLotteryAgain> &packet)
{
	SCRIPT_EVENT::PlayerHallLotteryAgainEvent::invoke(clientPeer->getRakssid(), packet->luck_times, packet->entityId);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketSetSpYaw> &packet)
{
	auto player = clientPeer->getEntityPlayer();
	player->spYaw = packet->isSpYaw;
	player->spYawRadian = packet->radian;
	auto sender = Server::Instance()->getNetwork()->getSender();
	sender->broadCastPlayerSetSpYaw(player->entityId, packet->isSpYaw, packet->radian);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketOpenSelectMode> &packet)
{
	SCRIPT_EVENT::OpenSelectModeEvent::invoke(clientPeer->getRakssid());
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer,
	std::shared_ptr<C2SPacketPixelHallUnlockMap> & packet)
{
	SCRIPT_EVENT::PlayerPixelGunHallUnlockMapEvent::invoke(clientPeer->getRakssid(), packet->num, packet->mapId);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketUseStoreGun> &packet)
{
	SCRIPT_EVENT::PlayerUseStoreGunEvent::invoke(clientPeer->getRakssid(), packet->gunId);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketBuyStoreGun> &packet)
{
	SCRIPT_EVENT::PlayerBuyStoreGunEvent::invoke(clientPeer->getRakssid(), packet->gunId);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketUpgradeStoreGun> &packet)
{
	SCRIPT_EVENT::PlayerUpgradeStoreGunEvent::invoke(clientPeer->getRakssid(), packet->gunId);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketQuickUpgradeStoreGun> &packet)
{
	SCRIPT_EVENT::PlayerQuickUpgradeStoreGunEvent::invoke(clientPeer->getRakssid(), packet->gunId);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketUseStoreProp> &packet)
{
	SCRIPT_EVENT::PlayerUseStorePropEvent::invoke(clientPeer->getRakssid(), packet->propId);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketBuyStoreProp> &packet)
{
	SCRIPT_EVENT::PlayerBuyStorePropEvent::invoke(clientPeer->getRakssid(), packet->propId);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketUnloadStoreProp> &packet)
{
	SCRIPT_EVENT::PlayerUnloadStorePropEvent::invoke(clientPeer->getRakssid(), packet->propId);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketUpgradeStoreProp> &packet)
{
	SCRIPT_EVENT::PlayerUpgradeStorePropEvent::invoke(clientPeer->getRakssid(), packet->propId);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketQuickUpgradeStoreProp> &packet)
{
	SCRIPT_EVENT::PlayerQuickUpgradeStorePropEvent::invoke(clientPeer->getRakssid(), packet->propId);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketEffectGunFire> &packet)
{
	EntityPlayerMP* pPlayer = clientPeer->getEntityPlayer();
	if (!pPlayer || pPlayer->isOnVehicle())
		return;

	ItemStackPtr pStack = pPlayer->inventory->getCurrentItem();
	if (!pStack)
		return;
	const GunSetting* pSetting = pStack->getGunSetting();
	if (!pSetting)
		return;

	if (packet->isNeedConsumeBullet && !pStack->consumeBullet())
		return;

	auto sender = Server::Instance()->getNetwork()->getSender();
	sender->broadCastPlayerEffectGunGire(pPlayer->entityId, pSetting->gunId, Vector3(packet->beginPos[0], packet->beginPos[1], packet->beginPos[2]), Vector3(packet->endPos[0], packet->endPos[1], packet->endPos[2]));
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketEffectGunFireResult> &packet)
{
	EntityPlayerMP* pPlayer = clientPeer->getEntityPlayer();
	if (!pPlayer)
		return;

	GunSetting* pSetting = GunSetting::getGunSetting(packet->gunId);
	if (!pSetting)
		return;

	Vector3i blockPos = Vector3i(packet->blockPos[0], packet->blockPos[1], packet->blockPos[2]);
	Vector3 hitPos = Vector3(packet->hitPos[0], packet->hitPos[1], packet->hitPos[2]);
	Vector3 dir = Vector3(packet->dir[0], packet->dir[1], packet->dir[2]);

	clientPeer->doGunShootTarget(packet->traceType, packet->hitEntityId, packet->hitPos, packet->isHeadshot, pSetting);

	auto world = Server::Instance()->getWorld();
	auto blockId = world->getBlockId(blockPos);
	auto meta = world->getBlockMeta(blockPos);
	auto block = BlockManager::getBlockById(blockId);

	auto sender = Server::Instance()->getNetwork()->getSender();

	switch (packet->traceType)
	{
	case BLOCK_TRACE_TYPE:
		if (block)
		{
			SCRIPT_EVENT::BlockBreakWithGunEvent::invoke(pPlayer->getRaknetID(), blockId, blockPos, packet->gunId);
			sender->broadCastPlayerEffectGunGireAtBlockResult(pPlayer->entityId, blockPos, hitPos, dir);
		}
		break;
	case ENTITY_PLAYER_TRACE_TYPE:
	case ENTITY_CREATUREAI_TRACE_TYPE:
		sender->broadCastPlayerEffectGunGireAtEntityResult(pPlayer->entityId, packet->hitEntityId, hitPos, dir, packet->traceType, packet->isHeadshot);
		break;
	default:
		break;
	}
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketEffectGunFireExplosionResult> &packet)
{
	EntityPlayerMP* pPlayer = clientPeer->getEntityPlayer();
	if (!pPlayer)
		return;

	auto sender = Server::Instance()->getNetwork()->getSender();
	sender->broadCastPlayerEffectGunGireExplosionResult(pPlayer->entityId, Vector3(packet->hitPos[0], packet->hitPos[1], packet->hitPos[2]), packet->explosionRange);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketStopLaserGun> &packet)
{
	EntityPlayerMP* pPlayer = clientPeer->getEntityPlayer();
	if (!pPlayer)
		return;

	clientPeer->stopLaserGun();
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketConsumeBullet> &packet)
{
	EntityPlayerMP* pPlayer = clientPeer->getEntityPlayer();
	if (!pPlayer || pPlayer->isOnVehicle())
		return;

	ItemStackPtr pStack = pPlayer->inventory->getCurrentItem();
	if (!pStack)
		return;
	const GunSetting* pSetting = pStack->getGunSetting();
	if (!pSetting)
		return;

	if (!pStack->consumeBullet())
	{
		clientPeer->stopLaserGun();
	}
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketFirstShowStoreGun> &packet)
{
	SCRIPT_EVENT::PlayerFirstShowStoreGunEvent::invoke(clientPeer->getRakssid(), packet->gunId);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketFirstShowStoreProp> &packet)
{
	SCRIPT_EVENT::PlayerFirstShowStorePropEvent::invoke(clientPeer->getRakssid(), packet->propId);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketPixelGunSelectBox> &packet)
{
	SCRIPT_EVENT::PlayerPixelGunSelectBoxEvent::invoke(clientPeer->getRakssid(), packet->index);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketPixelGunExit> &packet)
{
	SCRIPT_EVENT::PlayerPixelGunExitEvent::invoke(clientPeer->getRakssid());
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketPixelGunRematch> &packet)
{
	SCRIPT_EVENT::PlayerPixelGunRematchEvent::invoke(clientPeer->getRakssid());
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketPixelGunRevive> &packet)
{
	SCRIPT_EVENT::PlayerPixelGunReviveEvent::invoke(clientPeer->getRakssid());
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketOpenLotteryChest> &packet)
{
	SCRIPT_EVENT::PlayerOpenLotteryChestEvent::invoke(clientPeer->getRakssid(), packet->chestId);
}


void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketOpenArmorUpgrade> &packet)
{
	SCRIPT_EVENT::OpenArmorUpgradeEvent::invoke(clientPeer->getRakssid());
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketUpgradeArmor> &packet)
{
	SCRIPT_EVENT::PlayerUpgradeArmorEvent::invoke(clientPeer->getRakssid(), packet->state, packet->operateType);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketPixelGunResultBack> &packet)
{
	SCRIPT_EVENT::PlayerPixelGunResultBackEvent::invoke(clientPeer->getRakssid());
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketPixelGunResultNext> &packet)
{
	SCRIPT_EVENT::PlayerPixelGunResultNextEvent::invoke(clientPeer->getRakssid());
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketPixelGunResultRevenge> &packet)
{
	SCRIPT_EVENT::PlayerPixelGunResultRevengeEvent::invoke(clientPeer->getRakssid());
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketPixelGunFight> &packet)
{
	SCRIPT_EVENT::PlayerPixelGunFightEvent::invoke(clientPeer->getRakssid(), packet->gameType, packet->mapName);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketBirdBagOperation> & packet)
{
	SCRIPT_EVENT::BirdSimulatorBagOperationEvent::invoke(clientPeer->getRakssid(), packet->birdId, packet->operationType);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketBirdStoreOperation> & packet)
{
	SCRIPT_EVENT::BirdSimulatorStoreOperationEvent::invoke(clientPeer->getRakssid(), packet->storeId, packet->goodsId, packet->operationType);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketBirdPersonalStoreBuyGoods> & packet)
{
	SCRIPT_EVENT::BirdSimulatorPersonalStoreBuyGoodsEvent::invoke(clientPeer->getRakssid(), packet->tabId, packet->goodsId);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketBirdFuse> & packet)
{
	std::vector<i64> birdIds = std::vector<i64>();
	for (auto item : packet->birdIds)
	{
		birdIds.push_back(item);
	}
	SCRIPT_EVENT::BirdSimulatorFuseEvent::invoke(clientPeer->getRakssid(), birdIds);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketBirdOpenTreasureChest> & packet)
{
	SCRIPT_EVENT::BirdSimulatorOpenTreasureChestEvent::invoke(clientPeer->getRakssid(), packet->entityId);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketBirdLottery> & packet)
{
	SCRIPT_EVENT::BirdSimulatorLotteryEvent::invoke(clientPeer->getRakssid(), packet->entityId);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketBirdTask> & packet)
{
	SCRIPT_EVENT::BirdSimulatorTaskEvent::invoke(clientPeer->getRakssid(), packet->entityId, packet->taskId);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketBirdFeed> & packet)
{
	SCRIPT_EVENT::BirdSimulatorFeedEvent::invoke(clientPeer->getRakssid(), packet->birdId, packet->foodId, packet->num);
}

void C2SPlayerActionPacketHandles::handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketBirdSetDress> & packet)
{
	SCRIPT_EVENT::BirdSimulatorSetDressEvent::invoke(clientPeer->getRakssid(), packet->birdId, packet->dressType, packet->dressId);
}
