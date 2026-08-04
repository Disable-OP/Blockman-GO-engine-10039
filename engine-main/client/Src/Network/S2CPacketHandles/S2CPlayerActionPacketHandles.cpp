#include "S2CPlayerActionPacketHandles.h"

#include "cEntity/EntityPlayerSPMP.h"
#include "cEntity/EntityOtherPlayerMP.h"
#include "cWorld/Blockman.h"
#include "game.h"
#include "Item/ItemStack.h"
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
#include "TileEntity/TileEntitys.h"
#include "TileEntity/TileEntityInventory.h"
#include "cWorld/SignTextsChangeRecorder.h"
#include "Entity/EntityMerchant.h"
#include "BattleSummary.h"
#include "Blockman/GUI/RootGuiLayout.h"
#include "Util/LanguageKey.h"
#include "Util/LanguageManager.h"
#include "Util/ClientDataReport.h"
#include "Util/SkillEffectManager.h"
#include "cItem/cItem.h"
#include "Item/ItemStack.h"
#include "Setting/GunSetting.h"
#include "Entity/EntityVehicle.h"
#include "Entity/EntityRankNpc.h"
#include "Entity/Enchantment.h"
#include "Entity/Explosion.h"
#include "rapidjson/writer.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "Structs/ChoiceRole.h"
#include "Blockman/cBlock/CustomBlockClient.h"
#include "Entity/EntityAircraft.h"
#include "Trigger/TriggerManagers.h"
#include "Enums/Enums.h"
#include "cWorld/RankManager.h"
#include "Entity/EntityActorNpc.h"
#include "cEntity/EntityActorCannonClient.h"
#include "Blockman/EntityRenders/EntityRenderManager.h"
#include "Block/BlockUtil.h"
#include "Entity/EntitySessionNpc.h"
#include "World/WorldEffectManager.h"
#include "Util/BroadcastManager.h"
#include "Network/ClientHttpRequest.h"
#include "cEntity/PlayerControlller.h"
#include "Render/RenderEntity.h"
#include "cEntity/EntityBulletClient.h"
#include "cEntity/EntityGunFlame.h"

using namespace BLOCKMAN;


void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketSetFlying> &packet)
{
	LordLogInfo("recv S2CPacketSetFlying, isFlying=%s", packet->isFlying ? "true" : "false");
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	EntityPlayer* pPlayer = dataCache->getPlayerByServerId(packet->playerId);
	if (pPlayer)
		pPlayer->capabilities.isFlying = packet->isFlying;
}


void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketSetWatchMode> &packet)
{
	LordLogInfo("recv S2CPacketSetWatchMode, watchMode=%s", packet->isWatchMode ? "true" : "false");
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	EntityPlayer* pPlayer = dataCache->getPlayerByServerId(packet->playerId);
	if (pPlayer)
		pPlayer->setWatchMode(packet->isWatchMode);
}


void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketSetAllowFlying> &packet)
{
	BLOCKMAN::Blockman::Instance()->m_pPlayer->setAllowFlying(packet->allowFlying);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketAnimate>& packet)
{
	//LordLogInfo("_handlePacket_AnimatePacket_, animate type=%d, value=%d", (int)packet->m_animateType, packet->m_value);

	auto dataCache = ClientNetwork::Instance()->getDataCache();

	EntityPlayer* pPlayer = dataCache->getPlayerByServerId(packet->m_entityRuntimeId);

	if (!pPlayer) {
		LordLogError("recv animate packet but can not find entity, entity remote id=%d\n", packet->m_entityRuntimeId);
		return;
	}

	switch (packet->m_animateType)
	{
	case NETWORK_DEFINE::PacketAnimateType::ANIMATE_TYPE_ATTACK:
		pPlayer->swingItem();
		break;

	case NETWORK_DEFINE::PacketAnimateType::ANIMATE_TYPE_BASE_ACTION:
		pPlayer->setBaseAction(static_cast<ActionState>(packet->m_value));
		break;

	case NETWORK_DEFINE::PacketAnimateType::ANIMATE_TYPE_UPPER_ACTION:
		pPlayer->setUpperAction(static_cast<ActionState>(packet->m_value));
		break;

	default:
		LordLogError("recv animate packet with unknown animate type %d\n", static_cast<int>(packet->m_animateType));
		return;
	}
}


void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketRebirth>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();

	dataCache->setSelfServerId(packet->m_playerRuntimeId);
	dataCache->removeClientServerIdPair(BLOCKMAN::Blockman::Instance()->m_pPlayer->entityId);
	BLOCKMAN::Blockman::Instance()->rebirth(Vector3(Real(packet->m_posX), Real(packet->m_posY), Real(packet->m_posZ)),
		packet->m_sex, packet->m_showName);
	dataCache->addClientServerIdPair(BLOCKMAN::Blockman::Instance()->m_pPlayer->entityId, packet->m_playerRuntimeId);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketEquipArmors>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto pEntity = dynamic_cast<EntityOtherPlayerMP*>(dataCache->getEntityByServerId(packet->playerId));
	if (!pEntity) {
		LordLogError("recv S2CPacketEquipArmors but can not find player, id=[%d]", packet->playerId);
		return;
	}

	auto inventory = pEntity->inventory;
	for (auto armorInfo : packet->armors)
	{
		auto itemStack = LORD::make_shared<ItemStack>(armorInfo.itemId, 1, 0);
		if (armorInfo.enchanted)
		{
			// Add any one enchantment to turn on the enchantment rendering on the armor model
			itemStack->addEnchantment(Enchantment::protection, 1);
		}
		inventory->setInventorySlotContents(1, itemStack);
		inventory->equipArmor(itemStack);
	}
	pEntity->m_outLooksChanged = true;
}


void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketUnequipArmor>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto pEntity = dynamic_cast<EntityOtherPlayerMP*>(dataCache->getEntityByServerId(packet->playerId));
	if (!pEntity) {
		LordLogError("recv S2CPacketUnequipArmor but can not find player, id=[%d]", packet->playerId);
		return;
	}
	auto inventory = pEntity->inventory;
	inventory->unequipArmor(inventory->getStackInSlot(packet->slotIndex));
	pEntity->m_outLooksChanged = true;
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketDestroyArmor>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto pEntity = dynamic_cast<EntityOtherPlayerMP*>(dataCache->getEntityByServerId(packet->playerId));
	if (!pEntity) {
		LordLogError("recv S2CPacketDestroyArmor but can not find player, id=[%d]", packet->playerId);
		return;
	}
	auto inventory = pEntity->inventory;
	inventory->destroyArmor(packet->slotIndex);
	pEntity->m_outLooksChanged = true;
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketChangeItemInHand>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto pEntity = dynamic_cast<EntityOtherPlayerMP*>(dataCache->getEntityByServerId(packet->playerId));
	if (!pEntity) {
		LordLogError("recv S2CPacketChangeItemInHand with playId=%d, but can not find player", packet->playerId);
		return;
	}

	pEntity->m_needUpdateUpperAction = true;
	auto inventory = pEntity->inventory;
	if (packet->itemId < 0)
	{
		inventory->removeCurrentItemFromHotbar();
		if (pEntity->m_lastHandMesh != StringUtil::BLANK)
		{
			pEntity->m_handMeshChanged = true;
			pEntity->m_lastHandMesh = StringUtil::BLANK;
		}
	}
	else
	{
		auto itemStack = LORD::make_shared<ItemStack>(packet->itemId, 1, 0);
		if (packet->enchanted)
		{
			// add any enchantment to turn on the rendering of enchantment effect on the item model
			itemStack->addEnchantment(Enchantment::protection, 1);
		}
		inventory->setInventorySlotContents(0, itemStack);
		inventory->putItemToHotbar(0);

		cItem* citem = dynamic_cast<cItem*>(itemStack->getItem());
		if (citem && citem->getRenderType() == ItemRenderType::MESH && citem->getMeshName() != StringUtil::BLANK)
		{
			if (pEntity->m_lastHandMesh != citem->getMeshName())
			{
				pEntity->m_lastHandMesh = citem->getMeshName();
				pEntity->m_handMeshChanged = true;
			}
		}
		else if (pEntity->m_lastHandMesh != StringUtil::BLANK)
		{
			pEntity->m_lastHandMesh = StringUtil::BLANK;
			pEntity->m_handMeshChanged = true;
		}
	}
}


void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketChangeCurrentItem>& packet)
{
	int itemIndex = packet->itemIndex;
	if (itemIndex >= 0 && itemIndex < InventoryPlayer::HOTBAR_COUNT) {
		BLOCKMAN::Blockman::Instance()->m_pPlayer->inventory->currentItemIndex = itemIndex;
		BLOCKMAN::PlayerChangeItemEvent::emit(itemIndex, true);
	}
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketUpdateBlocks>& packet)
{
	int maxHandleBlocks = 10;
	if ((int)packet->updates.size() > maxHandleBlocks)
	{
		BlockModifys block_infos;
		for (const auto& blockInfo : packet->updates)
		{
			BlockModifyInfo block_info;
			block_info.id = blockInfo.getBlockId();
			block_info.meta = (ui16)blockInfo.getBlockMeta();
			Vector3i position(blockInfo.x, blockInfo.y, blockInfo.z);
			block_info.pos = position;

			block_infos.push_back(block_info);
		}
		auto world = BLOCKMAN::Blockman::Instance()->getWorld();
		if (world)
		{
			world->handleLargeBlocks(block_infos);
		}

		block_infos.clear();

		LordLogInfo("S2CPlayerActionPacketHandles::handlePacket S2CPacketUpdateBlocks too big size = %d", (int)packet->updates.size());
		return;
	}

	for (const auto& blockInfo : packet->updates)
	{
		HandleUpdateBlock(blockInfo);
	}
}


void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketPlayerAction>&packet)
{
	LordLogInfo("_handlePacket_S2CPlayerAction_, packet->action=%d", (int)packet->action);
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	EntityPlayer* pPlayer = dataCache->getPlayerByServerId(packet->playerId);
	if (!pPlayer) {
		LordLogError("recv S2CPlayerAction but can not find player, player remote id=%d\n", packet->playerId);
		return;
	}

	ItemStackPtr currentItem = nullptr;
	ItemBow* bow = nullptr;
	switch (packet->action) {
	case NETWORK_DEFINE::PacketPlayerActionType::START_SNEAK:
		pPlayer->setSneaking(true);
		break;

	case NETWORK_DEFINE::PacketPlayerActionType::STOP_SNEAK:
		pPlayer->setSneaking(false);
		break;

	case NETWORK_DEFINE::PacketPlayerActionType::START_PULL_BOW:
		currentItem = pPlayer->inventory->getCurrentItem();
		if (!currentItem) {
			LordLogError("recv S2CPlayerAction::START_PULL_BOW but current item nullptr, player remote id=%d\n", packet->playerId);
			return;
		}
		bow = dynamic_cast<ItemBow*>(currentItem->getItem());
		if (!bow) {
			LordLogError("recv S2CPlayerAction::START_PULL_BOW but current item not bow, player remote id=%d\n", packet->playerId);
			return;
		}
		pPlayer->setItemInUse(currentItem, bow->getMaxItemUseDuration(currentItem));
		break;

	case NETWORK_DEFINE::PacketPlayerActionType::STOP_PULL_BOW:
		pPlayer->clearItemInUse();
		break;

	case NETWORK_DEFINE::PacketPlayerActionType::START_SPRINTING:
		pPlayer->setSprinting(true);
		break;

	case NETWORK_DEFINE::PacketPlayerActionType::STOP_SPRINTING:
		pPlayer->setSprinting(false);
		break;

	case NETWORK_DEFINE::PacketPlayerActionType::START_ON_FIRE:
		pPlayer->setOnFire(true);
		break;

	case NETWORK_DEFINE::PacketPlayerActionType::STOP_ON_FIRE:
		pPlayer->setOnFire(false);
		break;
	default:
		// should not go here
		LordLogError("unknown player action recved, action = %d", static_cast<int>(packet->action));
		//LordAssert(packet->action == NETWORK_DEFINE::PacketPlayerActionType::START_SNEAK);
		break;
	}
}


void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketChestInventory>& packet)
{
	LordLogInfo("=========S2CPacketChestInventory=========");
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto world = dataCache->getWorldClient();
	int	blockID = world->getBlockId(BlockPos((int)packet->blockPosX, (int)packet->blockPosY, (int)packet->blockPosZ));

	if (blockID > 0)
	{
		IInventory* inventory = nullptr;

		if (dynamic_cast<BlockChest*>(BlockManager::sBlocks[blockID]))
		{
			inventory = dynamic_cast<BlockChest*>(BlockManager::sBlocks[blockID])->getInventory(world, BlockPos((int)packet->blockPosX, (int)packet->blockPosY, (int)packet->blockPosZ));
		}

		if (dynamic_cast<BlockEnderChest*>(BlockManager::sBlocks[blockID]))
		{
			inventory = (IInventory*)BLOCKMAN::Blockman::Instance()->m_pPlayer->getInventoryEnderChest();
		}
		
		if (!inventory)
			return;

		for (int i = 0; i < inventory->getSizeInventory(); i++)
		{
			if (inventory->getStackInSlot(i))
			{
				inventory->decrStackSize(i, inventory->getStackInSlot(i)->stackSize);
			}
		}

		for (auto iter = packet->chestInventory.begin(); iter != packet->chestInventory.end(); iter++)
		{
			if (iter->valid)
			{
				ItemStackPtr chestItemStack = LORD::make_shared<ItemStack>(iter->id, iter->size, iter->damage);
				chestItemStack->m_currentClipBulletNum = iter->curClipBullet;
				for (const auto& enchantmentInfo : iter->enchantments)
				{
					chestItemStack->addEnchantment(Enchantment::enchantmentsList[enchantmentInfo.id], enchantmentInfo.level);
				}
				inventory->setInventorySlotContents(iter->index, chestItemStack);
			}

		}
	}
}


void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketUsePropResults>& packet)
{
	GameClient::CGame::Instance()->getShellInterface()->usePropResults(packet->m_propId, packet->m_results);
}


void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketUserIn>& packet)
{
	GameClient::CGame::Instance()->getShellInterface()->userChange(packet->m_nickName, packet->m_teamName, packet->m_platformUserId, packet->m_teamId, true);
	UserInEvent::emit(packet->m_platformUserId, packet->m_nickName, packet->m_teamName, packet->m_teamId, packet->m_curPlayer, packet->m_maxPlayer);
	auto player = BLOCKMAN::Blockman::Instance()->m_pPlayer;
	if (player && GameClient::CGame::Instance()->getPlatformUserId() == packet->m_platformUserId)
	{
		player->setPlatformUserId(packet->m_platformUserId);
		player->setTeamName(packet->m_teamName);
		player->setTeamId(packet->m_teamId);

	}

	auto world = BLOCKMAN::Blockman::Instance()->m_pWorld;
	auto players = world->getPlayers();
	for (auto player : players)
	{
		if (player.second->getPlatformUserId() == packet->m_platformUserId)
		{
			player.second->setTeamId(packet->m_teamId);
			break;
		}
	}
	LordLogInfo("S2CPacketUserIn,UserName[%s],UserId[%llu],teamId[%d],teamName[%s]", packet->m_nickName.c_str(), packet->m_platformUserId, packet->m_teamId, packet->m_teamName.c_str());
}


void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketUserOut>& packet)
{
	GameClient::CGame::Instance()->getShellInterface()->userChange("", "", packet->m_platformUserId, 0, false);
	UserOutEvent::emit(packet->m_platformUserId);
	LordLogInfo("S2CPacketUserOut=====[%lld]", packet->m_platformUserId);
}


void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketPing>& packet)
{
	ui32 ping = Root::Instance()->getCurrentTime() - packet->m_sendTime;
	GameClient::CGame::Instance()->setPing(ping / 2);
	GameClient::CGame::Instance()->setReceivePingTime(Root::Instance()->getCurrentTime());
#if LORD_PLATFORM != LORD_PLATFORM_MAC_IOS
	//LordLogInfo("S2CPacketPing[%s],ReceivePingTime[%s]", StringUtil::ToString(ping).c_str(), StringUtil::ToString(Root::Instance()->getCurrentTime()).c_str());
#endif
}


void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncWorldTime>& packet)
{
	if(BLOCKMAN::Blockman::Instance()->m_accelerateTime == 1)
	BLOCKMAN::Blockman::Instance()->getWorld()->setWorldTime(packet->time);
}


void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketSettlement>& packet)
{
	GameClient::CGame::Instance()->getShellInterface()->gameSettlement(packet->m_gameType, packet->m_gameResult);
}


void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketGameover>& packet)
{
	
	switch (packet->m_code)
	{
	case 1:
		GameClient::CGame::Instance()->dataReport(BLOCKMAN::ClientDataReport::GAME_ERROR_CODE, BLOCKMAN::ClientDataReport::ERROR_CODE_GAME_OVER);
		GameClient::CGame::Instance()->dataReportNotType(BLOCKMAN::ClientDataReport::ENTER_GAME_STATUS, BLOCKMAN::ClientDataReport::ERROR_CODE_GAME_OVER);
		GameoverEvent::emit(packet->m_overMessage);
		GameClient::CGame::Instance()->getNetwork()->logout();
		break;
	case 2:
		GameClient::CGame::Instance()->retryEnter(packet->m_code, packet->m_overMessage);
		break;
	case 3:
		GameClient::CGame::Instance()->retryEnter(packet->m_code, packet->m_overMessage);
		break;
	case 6:
		GameClient::CGame::Instance()->dataReport(BLOCKMAN::ClientDataReport::GAME_ERROR_CODE, BLOCKMAN::ClientDataReport::ERROR_CODE_SERVER_LOAD_DB_ERROR);
		GameClient::CGame::Instance()->dataReportNotType(BLOCKMAN::ClientDataReport::ENTER_GAME_STATUS, BLOCKMAN::ClientDataReport::ERROR_CODE_SERVER_LOAD_DB_ERROR);
		GameoverEvent::emit(packet->m_overMessage);
		GameClient::CGame::Instance()->getNetwork()->logout();
		break;
	case 7:
		GameClient::CGame::Instance()->dataReport(BLOCKMAN::ClientDataReport::GAME_ERROR_CODE, BLOCKMAN::ClientDataReport::ERROR_CODE_CHEAT_DETECTED);
		GameClient::CGame::Instance()->dataReportNotType(BLOCKMAN::ClientDataReport::ENTER_GAME_STATUS, BLOCKMAN::ClientDataReport::ERROR_CODE_CHEAT_DETECTED);
		GameoverEvent::emit(packet->m_overMessage);
		GameClient::CGame::Instance()->getNetwork()->logout();
		break;
	default:
		GameClient::CGame::Instance()->dataReport(BLOCKMAN::ClientDataReport::GAME_ERROR_CODE, BLOCKMAN::ClientDataReport::ERROR_CODE_GAME_STOP);
		GameClient::CGame::Instance()->dataReportNotType(BLOCKMAN::ClientDataReport::ENTER_GAME_STATUS, BLOCKMAN::ClientDataReport::ERROR_CODE_GAME_STOP);
		GameoverEvent::emit(packet->m_overMessage);
		GameClient::CGame::Instance()->getNetwork()->logout();
		break;
	}

	LordLogError("recv S2CPacketGameover");
}


void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketCloseContainer>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	EntityPlayer* pUser = dataCache->getPlayerByServerId(packet->m_playerID);
	BlockPos m_containerPos;
	m_containerPos.x = (int)packet->m_containerPosX;
	m_containerPos.y = (int)packet->m_containerPosY;
	m_containerPos.z = (int)packet->m_containerPosZ;
	if (!pUser)
		return;
	if (!packet->m_close)
	{
		if (pUser)
		{
			auto world = dataCache->getWorldClient();
			int	blockID = world->getBlockId(m_containerPos);
			if (blockID > 0)
			{
				// just for chest only currently
				IInventory* inventory = dynamic_cast<BlockChest*>(BlockManager::sBlocks[blockID])->getInventory(world, m_containerPos);
				if (!inventory)
					return;
				// set the player's container to chest. when the chest GUI is close need to close container. see RootGuiLayout::showChestManage
				pUser->openContainer = LordNew ContainerChest(pUser->inventory, inventory);
			}
		}
	}
	else if (pUser->openContainer != pUser->inventoryContainer)
	{
		pUser->openContainer->onContainerClosed(pUser);
		if (pUser->openContainer == pUser->inventoryContainer)
			return;
		LordSafeDelete(pUser->openContainer);
		pUser->openContainer = pUser->inventoryContainer;
	}
}



void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketPlaySound>& packet)
{
	Entity* pEntity = NULL;
	if (packet->m_entityId != 0)
	{
		auto dataCache = ClientNetwork::Instance()->getDataCache();
		pEntity = dataCache->getEntityByServerId(packet->m_entityId);
	}
	else
	{
		pEntity = BLOCKMAN::Blockman::Instance()->m_pPlayer;
	}

	if (pEntity)
	{
		pEntity->playSoundByType((SoundType)packet->m_soundType);
		if ((SoundType)packet->m_soundType > ST_BgmStart)
		{
			RootGuiLayout::Instance()->checkBackgroundMusic();
		}
	}
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketExplosion>& packet)
{
	auto player = BLOCKMAN::Blockman::Instance()->m_pPlayer;
	if (player)
	{
		BlockPosArr blocks;
		for (auto p : packet->m_poss)
		{
			blocks.push_back(p);
		}
		Explosion* pExplosion = LordNew Explosion(player->world, nullptr, nullptr, packet->m_pos, packet->m_size, blocks);
		pExplosion->isSmoking = packet->m_smoke;
		pExplosion->doExplosionB(true, packet->m_customsize);
		if (!player->capabilities.isWatchMode)
		{
			player->motion += packet->m_motion;
		}
		LordDelete(pExplosion);
	}
}



void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketSetSignTexts>& packet)
{
	for (const auto& signData : packet->signData)
	{
		BlockPos position = signData.position;
		const auto& texts = signData.texts;
		auto world = BLOCKMAN::Blockman::Instance()->getWorld();
		if (world->blockExists(position))
		{
			auto tileEntity = dynamic_cast<TileEntitySign*>(BLOCKMAN::Blockman::Instance()->getWorld()->getBlockTileEntity(position));
			if (!tileEntity)
			{
				LordLogError("Received S2CPacketSetSignTexts, but there is no TileEntitySign at positoin (%d, %d, %d)", position.x, position.y, position.z);
				continue;
			}
			for (size_t i = 0; i < texts.size(); ++i)
			{
				tileEntity->setText(i, texts[i]);
			}
		}
		else
		{
			SignTextsChangeRecorder::Instance()->add(position, texts);
		}
	}
}


void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketChangePlayerInfo>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	EntityPlayer* pUser = dataCache->getPlayerByServerId(packet->m_playerId);
	if (!pUser)
		return;
	pUser->setShowName(packet->m_playerShowName);
}


void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketUserAttr>& packet)
{
	/*LordLogInfo("recv S2CPacketUserAttr entityId=%d, hairId=%d, faceId=%d, topsId=%d, pantsId=%d, "
		"shoesId=%d, glassesId=%d, scarfId=%d, wingId=%d, hatId=%d ,decoratehatId=%d,armId=%d, extrawingId=%d, footHaloId=%d,skinColor r=%f , skinColor g=%f , skinColor b=%f , skinColor a=%f",
		packet->entityId, packet->faceId, packet->hairId, packet->topsId, packet->pantsId,
		packet->shoesId, packet->glassesId, packet->scarfId, packet->wingId, packet->hatId,packet->decoratehatId, packet->armId, packet->extrawingId, packet->footHaloId, packet->skinColor.r, packet->skinColor.g, packet->skinColor.b, packet->skinColor.a);*/

	auto dataCache = ClientNetwork::Instance()->getDataCache();
	EntityPlayer* player = dataCache->getPlayerByServerId(packet->entityId);
	if (!player) {
		LordLogError("Received S2CPacketUserAttr but can not find the entity, entityId=%d", packet->entityId);
		return;
	}

	LordLogInfo("recv S2CPacketUserAttr entityName=%s entityId=%d, hairId=%d, faceId=%d, topsId=%d, pantsId=%d, "
		"shoesId=%d, glassesId=%d, scarfId=%d, wingId=%d, hatId=%d ,decoratehatId=%d,armId=%d, extrawingId=%d, footHaloId=%d,skinColor r=%f , skinColor g=%f , skinColor b=%f , skinColor a=%f",
		player->getEntityName().c_str(), packet->entityId, packet->faceId, packet->hairId, packet->topsId, packet->pantsId,
		packet->shoesId, packet->glassesId, packet->scarfId, packet->wingId, packet->hatId, packet->decoratehatId, packet->armId, packet->extrawingId, packet->footHaloId, packet->skinColor.r, packet->skinColor.g, packet->skinColor.b, packet->skinColor.a);

	player->m_faceID = packet->faceId > 0 ? packet->faceId : 1;
	player->m_hairID = packet->hairId > 0 ? packet->hairId : 1;
	player->m_topsID = packet->topsId > 0 ? packet->topsId : 1;
	player->m_pantsID = packet->pantsId > 0 ? packet->pantsId : 1;
	player->m_shoesID = packet->shoesId > 0 ? packet->shoesId : 1;
	player->m_glassesId = packet->glassesId > 0 ? packet->glassesId : 0;
	player->m_scarfId = packet->scarfId > 0 ? packet->scarfId : 0;
	player->m_wingId = packet->wingId > 0 ? packet->wingId : 0;
	player->m_hatId = packet->hatId > 0 ? packet->hatId : 0;
	player->m_decorate_hatId = packet->decoratehatId > 0 ? packet->decoratehatId : 0;
	player->m_armId = packet->armId > 0 ? packet->armId : 0;
	player->m_tailId = packet->tailId > 0 ? packet->tailId : 0;
	player->m_extra_wingId = packet->extrawingId > 0 ? packet->extrawingId : 0;
	player->m_footHaloId = packet->footHaloId > 0 ? packet->footHaloId : 0;
	player->m_backEffectId = packet->backEffectId > 0 ? packet->backEffectId : 0;
	player->m_crownId = packet->crownId > 0 ? packet->crownId : 0;
	player->m_headEffectId = packet->headEffectId > 0 ? packet->headEffectId : 0;

	player->m_skinColor = Color(packet->skinColor.r, packet->skinColor.g, packet->skinColor.b, packet->skinColor.a);
	//player->m_skinColor = Color(0.482353f  , 0.000000f , 0.666667f , 0.549020f);
	player->m_bagId = packet->bagId > 0 ? packet->bagId : 0;
	player->m_outLooksChanged = true;
}



void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketDebugString>& packet)
{
	BLOCKMAN::Blockman::Instance()->setServerDebugString(packet->debugString);
}


void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketGameStatus>& packet)
{
	GameStatusEvent::emit(packet->m_status);
	LordLogInfo("Received S2CPacketGameStatus m_status %d", packet->m_status);
}


void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketUpdateMerchant>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto entityId = dataCache->getClientId(packet->id);
	auto merchant = dynamic_cast<EntityMerchant*>(BLOCKMAN::Blockman::Instance()->getWorld()->getEntity(entityId));
	if (!merchant)
	{
		LordLogError("EntityMerchant does not exist, id: %d", entityId);
		return;
	}
	String name = packet->name;
	if (name.length() == 0)
	{
		if (packet->nameLang.length() > 0)
		{
			name = LanguageManager::Instance()->getString(packet->nameLang);
		}
		else
		{
			name = LanguageManager::Instance()->getString(LanguageManager::Instance()->getString(LanguageKey::DEFAULT_MERCHANT_NAME));
		}
	}
	merchant->setNameLang(packet->nameLang);
	merchant->setName(name);
	merchant->resetCommodities(packet->commodities);
	merchant->setHasUpdated(true);
	MerchantUpdateEvent::emit(merchant);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketUpdateMerchantInfo>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto entityId = dataCache->getClientId(packet->id);
	auto merchant = dynamic_cast<EntityMerchant*>(BLOCKMAN::Blockman::Instance()->getWorld()->getEntity(entityId));
	if (!merchant)
	{
		LordLogError("EntityMerchant does not exist, id: %d", entityId);
		return;
	}
	String name = packet->name;
	if (name.length() == 0)
	{
		if (packet->nameLang.length() > 0)
		{
			name = LanguageManager::Instance()->getString(packet->nameLang);
		}
		else
		{
			name = LanguageManager::Instance()->getString(LanguageManager::Instance()->getString(LanguageKey::DEFAULT_MERCHANT_NAME));
		}
	}
	merchant->setNameLang(packet->nameLang);
	merchant->setName(name);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketUpdateMerchantCommodities>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto entityId = dataCache->getClientId(packet->id);
	auto merchant = dynamic_cast<EntityMerchant*>(BLOCKMAN::Blockman::Instance()->getWorld()->getEntity(entityId));
	if (!merchant)
	{
		LordLogError("EntityMerchant does not exist, id: %d", entityId);
		return;
	}
	merchant->resetCommodities(packet->commodities);
	merchant->setHasUpdated(true);
	MerchantUpdateEvent::emit(merchant);
}


void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CSyncNpcActorInfo>& packet) 
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto entityId = dataCache->getClientId(packet->entityId);
	auto entityActorNpc = dynamic_cast<EntityActorNpc*>(BLOCKMAN::Blockman::Instance()->getWorld()->getEntity(entityId));

	if (!entityActorNpc)
	{
		LordLogError("EntityActorNpc does not exist, id: %d", entityId);
		return;
	}
	entityActorNpc->setActorName(packet->actorName);
	entityActorNpc->setHeadName(packet->headName);
	entityActorNpc->setSkillName(packet->skillName);
	entityActorNpc->setHaloEffectName(packet->haloEffectName);
	entityActorNpc->setCanObstruct(packet->isCanObstruct);
	entityActorNpc->setContent(packet->content);
	entityActorNpc->setCanCollided(packet->isCanCollided);
	EntityRenderManager::Instance()->refreshEntityActor(BLOCKMAN::Blockman::Instance()->getWorld()->getEntity(entityId), entityActorNpc->getActorName());
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketPlayerSettlement>& packet)
{
	ReceiveDeadSummaryEvent::emit(packet->m_result, packet->m_isNextServer);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketGameSettlement>& packet)
{
	int result = GameClient::ResultEntry::getIsWin(packet->m_result);

	if (result == 0 || result == 1 || result == 2)
	{
		PlayEndingAnimationEvent::emit(packet->m_result, packet->m_isNextServer, result);
	}
	else
	{
		ReceiveFinalSummaryEvent::emit(packet->m_result, packet->m_isNextServer);
	}
}


void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketPlayerLifeStatus>& packet)
{
	PlayerLifeStatusEvent::emit(packet->m_platformUserId, packet->m_isLife);
}


void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncWorldWeather>& packet)
{
	BLOCKMAN::Blockman::Instance()->getWorld()->setWorldWeather(packet->weather, packet->time);
}


void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketBedDestroy>& packet)
{
	TeamBedDestroyedEvent::emit(packet->m_teamId);
}


void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketRespawnCountdown>& packet)
{
	RootGuiLayout::Instance()->showCountdown(packet->m_second);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketAllPlayerTeamInfo>& packet)
{
	GetAllPlayerTeamInfoEvent::emit(packet->m_result);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketChangePlayerTeam>& packet)
{
	PlayerChangeTeamEvent::emit(packet->m_platformUserId, packet->m_teamId);
	if (packet->m_platformUserId == BLOCKMAN::Blockman::Instance()->m_pPlayer->getPlatformUserId())
	{
		BLOCKMAN::Blockman::Instance()->m_pPlayer->setTeamId(packet->m_teamId);
	}
	auto world = BLOCKMAN::Blockman::Instance()->m_pWorld;
	auto players = world->getPlayers();
	for (auto player : players)
	{
		if (player.second->getPlatformUserId() == packet->m_platformUserId)
		{
			player.second->setTeamId(packet->m_teamId);
			LordLogInfo("S2CPacketChangePlayerTeam userId %llu  temaId %d" , packet->m_platformUserId , packet->m_teamId);
			break;
		}
	}
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketPlayerChangeActor>& packet)
{
	LordLogInfo("S2CPacketPlayerChangeActor %s", packet->m_actorName.c_str());
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	EntityPlayer* player = dataCache->getPlayerByServerId(packet->m_entityId);
	if (player) 
	{
		BLOCKMAN::Blockman::Instance()->getWorld()->changePlayerActor(player, packet->m_actorName, packet->m_bodyId);
	}
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketPlayerRestoreActor>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	EntityPlayer* player = dataCache->getPlayerByServerId(packet->m_entityId);
	if (player) {
		BLOCKMAN::Blockman::Instance()->getWorld()->restorePlayerActor(player);
		if (player->entityId == BLOCKMAN::Blockman::Instance()->m_pPlayer->entityId)
		{
			ChangeSkillTypeEvent::emit(SkillType::NONE , 0.f, 0.f);
		}
	}
}


void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketReloadBullet>& packet)
{
	BLOCKMAN::Blockman::Instance()->m_pPlayer->beginReloadBullet(true);
}


void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketUpdateShop>& packet)
{
	Shop* shop = BLOCKMAN::Blockman::Instance()->getWorld()->getShop();
	if (shop)
	{
		shop->resetGoods(packet->m_goods);
		shop->resetRespawnGoods(packet->m_respawnGoods);
	}
}


void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketUpdateShopGoods>& packet)
{
	Shop* shop = BLOCKMAN::Blockman::Instance()->getWorld()->getShop();
	if (shop)
	{
		shop->updateGoods(packet->groupIndex, packet->goodsIndex, packet->limit);
		AppShopUpdateEvent::emit();
	}
}


void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketShoppingResult>& packet)
{
	LordLogInfo("S2CPacketShoppingResult code %d  result %s ", packet->code, packet->result.c_str());
	switch (packet->code)
	{
	case 1: // buy success
		GameClient::CGame::Instance()->getNetwork()->getSender()->sendResumeOrder(packet->result);
		//BuyGoodsResultEvent::emit(LanguageManager::Instance()->getString(LanguageKey::GUI_BUY_GOODS_SUCCESSFUL).c_str());
		break;
	case 2: // inventory shortage
		BuyGoodsResultEvent::emit(LanguageManager::Instance()->getString(LanguageKey::GUI_STR_APP_SHOP_INSUFFICIENT_INVENTORY).c_str());
		break;
	case 3: // not enough money
		BuyGoodsResultEvent::emit(LanguageManager::Instance()->getString(LanguageKey::LACK_OF_MONEY).c_str());
		break;
	case 4: // player inventory full
		BuyGoodsResultEvent::emit(LanguageManager::Instance()->getString(LanguageKey::GUI_STR_APP_SHOP_INVENTORY_IS_FULL).c_str());
		break;
	case 5: // buy goods failed by plugin
		BuyGoodsResultEvent::emit(packet->result);
		break;
	case 6: //other buy success
		//BuyGoodsResultEvent::emit(LanguageManager::Instance()->getString(LanguageKey::GUI_BUY_GOODS_SUCCESSFUL).c_str());
		break;
	default: // buy failed
		BuyGoodsResultEvent::emit(LanguageManager::Instance()->getString(LanguageKey::GUI_BUY_GOODS_FAILURE).c_str());
		break;
	}
}


void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketInitMoney>& packet)
{

	auto player = BLOCKMAN::Blockman::Instance()->m_pPlayer;
	if (!player || !player->m_wallet)
	{
		LordLogError("S2CPacketInitMoney player or m_wallet is null");
		return;
	}
	player->m_wallet->setDiamonds(packet->diamonds);
	player->m_wallet->setGolds(packet->golds);

	LordLogError("S2CPacketInitMoney diamonds %lld", packet->diamonds);
}


void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncGunFireResult>& packet)
{
	const GunSetting* pGunSetting = GunSetting::getGunSetting(packet->gunId);
	if (pGunSetting)
		BLOCKMAN::Blockman::Instance()->m_pPlayer->playSoundByType((SoundType)pGunSetting->soundType);

	// play shoot gunFlame effect
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto pWorld = BLOCKMAN::Blockman::Instance()->getWorld();
	EntityPlayer* player = dataCache->getPlayerByServerId(packet->sourceEntityId);
	if (player && player != BLOCKMAN::Blockman::Instance()->m_pPlayer)
	{
		pWorld->spawnParticle("gunFlame", Vector3::ZERO, Vector3::ZERO, player);
	}
	EntityPlayer* targetPlayer = dataCache->getPlayerByServerId(packet->hitEntityID);
	int blockID = pWorld->getBlockId(packet->blockPos);
	switch (packet->tracyType)
	{
		case BLOCK_TRACE_TYPE:
			if (blockID > 0)
			{
				for (int i = 0; i < 8; ++i)
				{
					Vector3 rv = pWorld->m_Rand.nextVector();
					String name = StringUtil::Format("tilecrack_%d_%d", blockID, pWorld->getBlockMeta(packet->blockPos));
					pWorld->spawnParticle(name, packet->hitpos, Vector3(packet->dir[0] * -2.f + rv.x, 3.f * rv.y, packet->dir[2] * -2.f + rv.z));
				}
				WorldEffectManager::Instance()->addSimpleEffect("effect_gun_be_shot.effect", packet->hitpos, Math::ATan2(-packet->dir[2], -packet->dir[0]) * Math::RAD2DEG - 90.0f, 500);
			}
			break;
		case ENTITY_PLAYER_TRACE_TYPE:
			if (player && targetPlayer)
			{
				if (targetPlayer == BLOCKMAN::Blockman::Instance()->m_pPlayer && (LogicSetting::Instance()->getShowGunEffectWithSingleStatus() || player->getTeamId() != targetPlayer->getTeamId()))
				{
					bool isScreenLeft = false;
					Vector3 targetDir = player->position - targetPlayer->position;
					targetDir.y = 0.0f;
					targetDir.normalize();
					if ((targetDir.x < -0.00001) || (targetDir.x > 0.00001) || (targetDir.z < -0.00001) || (targetDir.z > 0.00001))
					{
						Vector3 viewDir = SceneManager::Instance()->getMainCamera()->getDirection();
						viewDir.y = 0;
						viewDir.normalize();
						float cosValue = targetDir.dot(viewDir);
						Vector3 tangent = targetDir.cross(viewDir);
						if (tangent.y < 0)isScreenLeft = true;
						ShootWarningEvent::emit(Math::ACos(cosValue), isScreenLeft);
					}
				}
				if (packet->isHeadshot && LogicSetting::Instance()->getAllowHeadshotStatus() && (LogicSetting::Instance()->getShowGunEffectWithSingleStatus() || player->getTeamId() != targetPlayer->getTeamId()))
				{
					WorldEffectManager::Instance()->addSimpleEffect("headshot.effect", Vector3(targetPlayer->getPosition().x, targetPlayer->getPosition().y + targetPlayer->getEyeHeight(), targetPlayer->getPosition().z), targetPlayer->getYaw(), 1000);
				}
			}
		case ENTITY_CREATUREAI_TRACE_TYPE:
			for (int i = 0; i < 8; ++i)
			{
				Vector3 rv = pWorld->m_Rand.nextVector();
				String name = StringUtil::Format("tilecrack_%d_%d", BLOCK_ID_BLOCK_REDSTONE, 0); // use redstoneblock as blooding!!! --!
				pWorld->spawnParticle(name, packet->hitpos, Vector3(packet->dir[0] * -2.f + rv.x, 3.f * rv.y, packet->dir[2] * -2.f + rv.z));
			}
			WorldEffectManager::Instance()->addSimpleEffect("effect_gun_be_shot.effect", packet->hitpos, Math::ATan2(-packet->dir[2], -packet->dir[0]) * Math::RAD2DEG - 90.0f, 500);
			break;
		case ENTITY_VEHICLE_TRACE_TYPE:
			break;
		default :
			break;
	}
}


void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketChangeMaxHealth>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	EntityPlayer* player = dataCache->getPlayerByServerId(packet->entityId);
	if (!player) {
		return;
	}
	player->changeMaxHealth(packet->health);
	auto me = BLOCKMAN::Blockman::Instance()->m_pPlayer;
	if (!me)
	{
		return;
	}
	if (player->entityId == me->entityId)
	{
		RootGuiLayout::Instance()->changePlayerMaxHealth(packet->health);
	}
}


void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketUpdateCurrency>& packet)
{
	auto player = BLOCKMAN::Blockman::Instance()->m_pPlayer;
	if (!player || !player->m_wallet)
	{
		LordLogError("S2CPacketUpdateCurrency player or m_wallet is null");
	}
	player->m_wallet->setCurrency(packet->currency);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncTakeVehicle>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto pWorld = BLOCKMAN::Blockman::Instance()->getWorld();
	EntityPlayer* player = dataCache->getPlayerByServerId(packet->playerId);
	if (!player)
		return;

	EntityVehicle* vehicle = dynamic_cast<EntityVehicle*>(dataCache->getEntityByServerId(packet->vehicleId));
	if (!vehicle)
		return;

	bool bisLocalPlayer = player == BLOCKMAN::Blockman::Instance()->m_pPlayer;
	bool isDriver = false;
	if (packet->onoff)
	{
		player->takeOnVehicle(vehicle, true, packet->isDriver);
		if (bisLocalPlayer)
		{
			isDriver = player->isDriver();
			VehicleOnOffEvent::emit(true, isDriver);
		}
	}
	else
	{
		if (bisLocalPlayer)
		{
			isDriver = player->isDriver();
			VehicleOnOffEvent::emit(false, isDriver);
		}

		player->takeOffVehicle();
		if (bisLocalPlayer)
			VehicleOnOffEvent::emit(false, false);
	}
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketSelectRoleData>& packet)
{
	
	if (packet->result.length() <=0)
	{
		return;
	}
	rapidjson::Document doc;
	doc.Parse(packet->result.c_str());
	if (doc.HasParseError()) {
		LordLogError("_parseSelectRoleData fail, parse json fail, [%s]", packet->result.c_str());
		return;
	}

	if (!doc.HasMember("criminal") || !doc.HasMember("police"))
	{
		LordLogError("_parseSelectRoleData fail, parse json fail, [%s]", packet->result.c_str());
		return;
	}

	LordLogInfo("_parseSelectRoleData info: %s", packet->result.c_str());

	BLOCKMAN::ChoiceRole choiceRole;

	choiceRole.type = doc.HasMember("type") ? doc.FindMember("type")->value.GetInt() : 0;
	choiceRole.forceOpen = doc.HasMember("forceOpen") ? doc.FindMember("forceOpen")->value.GetBool() : true;
	rapidjson::Value&  criminal = doc["criminal"];
	rapidjson::Value&  police = doc["police"];

	if (criminal.IsObject())
	{
		choiceRole.criminal.title = criminal.HasMember("title") ? criminal.FindMember("title")->value.GetString() : "";
		choiceRole.criminal.model = criminal.HasMember("model") ? criminal.FindMember("model")->value.GetString() : "";
		choiceRole.criminal.canSelect = criminal.HasMember("canSelect") ? criminal.FindMember("canSelect")->value.GetBool() : true;
		choiceRole.criminal.money = criminal.HasMember("money") ? criminal.FindMember("money")->value.GetInt64() : -1;
		choiceRole.criminal.name = criminal.HasMember("name") ? criminal.FindMember("name")->value.GetString() : "";
		choiceRole.criminal.threat = criminal.HasMember("threat") ? criminal.FindMember("threat")->value.GetInt() : -1;
		choiceRole.criminal.lv = criminal.HasMember("lv") ? criminal.FindMember("lv")->value.GetInt() : -1;
		if (criminal.HasMember("actor"))
		{
			auto actor = criminal.FindMember("actor")->value.GetArray();
			for (int i = 0; i < (int)actor.Size(); i++)
			{
				const rapidjson::Value& object = actor[i];
				ResInfo resInfo;
				resInfo.resName = object["first"].GetString();
				resInfo.resId = object["second"].GetInt();
				choiceRole.criminal.actors.push_back(resInfo);
			}
		}
	}

	if (police.IsObject())
	{
		choiceRole.police.title = police.HasMember("title") ? police.FindMember("title")->value.GetString() : "";
		choiceRole.police.model = police.HasMember("model") ? police.FindMember("model")->value.GetString() : "";
		choiceRole.police.canSelect = police.HasMember("canSelect") ? police.FindMember("canSelect")->value.GetBool() : true;
		choiceRole.police.money = police.HasMember("money") ? police.FindMember("money")->value.GetInt64() : -1;
		choiceRole.police.name = police.HasMember("name") ? police.FindMember("name")->value.GetString() : "";
		choiceRole.police.merit = police.HasMember("merit") ? police.FindMember("merit")->value.GetInt() : -1;
		choiceRole.police.lv = police.HasMember("lv") ? police.FindMember("lv")->value.GetInt() : -1;
		if (police.HasMember("actor"))
		{
			auto actor = police.FindMember("actor")->value.GetArray();
			for (int i = 0; i < (int)actor.Size(); i++)
			{
				const rapidjson::Value& object = actor[i];
				ResInfo resInfo;
				resInfo.resName = object["first"].GetString();
				resInfo.resId = object["second"].GetInt();
				choiceRole.police.actors.push_back(resInfo);
			}
		}
	}
	ShowGuiJailBreakRoleChoiceEvent::emit(choiceRole);
	LordLogInfo("_parseSelectRoleData done");
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncPlayerOwnVehicle>& packet)
{
	auto pPlayer = BLOCKMAN::Blockman::Instance()->m_pPlayer;
	pPlayer->resetOwnvehicle(packet->m_vehicles);

}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncLockVehicle>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto pWorld = BLOCKMAN::Blockman::Instance()->getWorld();
	EntityPlayer* player = dataCache->getPlayerByServerId(packet->playerId);
	if (!player)
		return;

	EntityVehicle* vehicle = dynamic_cast<EntityVehicle*>(dataCache->getEntityByServerId(packet->vehicleId));
	if (!vehicle)
		return;

	player->tryLockVehicle(packet->onoff);
	if (player == BLOCKMAN::Blockman::Instance()->m_pPlayer)
	{
		VehicleLockEvent::emit(packet->onoff);
	}
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncVehicleState>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto entityId = dataCache->getClientId(packet->entityId);
	auto vehicle = dynamic_cast<EntityVehicle*>(BLOCKMAN::Blockman::Instance()->getWorld()->getEntity(entityId));
	if (!vehicle)
	{
		return;
	}

	vehicle->m_bisLocked = packet->isLocked;
	vehicle->setCurrentVelocity(packet->currentVelocity);
	vehicle->m_hasDriver = packet->hasDriver;
	vehicle->setActionState((VehicleActionState) packet->state);

	if (packet->hasDriver)
	{
		auto entityId = dataCache->getClientId(packet->driverId);
		auto pPlayer = BLOCKMAN::Blockman::Instance()->getWorld()->getPlayerEntity(entityId);
		if (pPlayer)
			pPlayer->takeOnVehicle(vehicle, true, true);
	}

	for (auto it = packet->m_passengers.begin(); it != packet->m_passengers.end(); it++)
	{
		auto entityId = dataCache->getClientId(*it);
		auto pPlayer = BLOCKMAN::Blockman::Instance()->getWorld()->getPlayerEntity(entityId);
		if (pPlayer)
			pPlayer->takeOnVehicle(vehicle, true, false);
	}
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncVehicleActionState>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto entityId = dataCache->getClientId(packet->entityId);
	auto vehicle = dynamic_cast<EntityVehicle*>(BLOCKMAN::Blockman::Instance()->getWorld()->getEntity(entityId));
	if (!vehicle)
	{
		return;
	}

	vehicle->setActionState((VehicleActionState) packet->state);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketNotifyGetMoney>& packet)
{
	BLOCKMAN::Blockman::Instance()->m_pPlayer->playSoundByType(ST_GetCoin);
	BLOCKMAN::GetMoneyEvent::emit(packet->count);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketNotifyGetItem>& packet)
{
	BLOCKMAN::Blockman::Instance()->m_pPlayer->playSoundByType(ST_GetItem);
	BLOCKMAN::GetItemEvent::emit(packet->itemId, packet->meta, packet->count);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketBuyCommodityResult>& packet)
{
	LordLogInfo("S2CPacketBuyCommodityResult code %d result %s ", packet->code, packet->result.c_str());
	switch (packet->code)
	{
	case 1: // buy success
		BuyCommodityResultEvent::emit(LanguageManager::Instance()->getString(LanguageKey::GUI_BUY_GOODS_SUCCESSFUL).c_str());
		break;
	case 2: // buy failed
		BuyCommodityResultEvent::emit(LanguageManager::Instance()->getString(LanguageKey::GUI_BUY_GOODS_FAILURE).c_str());
		break;
	case 3: // buy goods failed by plugin
		BuyCommodityResultEvent::emit(packet->result);
		break;
	case 4: // buy failed, have already got it
		BuyCommodityResultEvent::emit(LanguageManager::Instance()->getString(LanguageKey::GUI_HAVE_ALREADY_GOT_BLOCK).c_str());
		break;
	default: // buy failed
		BuyCommodityResultEvent::emit(LanguageManager::Instance()->getString(LanguageKey::GUI_BUY_GOODS_FAILURE).c_str());
		break;
	}
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketShowBuyRespawn>& packet)
{
	Shop* shop = BLOCKMAN::Blockman::Instance()->getWorld()->getShop();
	Wallet *wallet = BLOCKMAN::Blockman::Instance()->m_pPlayer->m_wallet;
	if (shop && wallet)
	{
		RespawnGoods goods = shop->getRespawnGoodsByTime(packet->times);
		if (goods.uniqueId == -1)
		{
			GameClient::CGame::Instance()->getNetwork()->getSender()->sendBuyRespawn(false, -1, -1);
			return;
		}

		int cost = GameClient::CGame::Instance()->isMultiplayer() ? goods.blockmanPrice : goods.blockymodsPrice;
		ShowReviveEvent::emit(goods.coinId, cost, goods.time, goods.uniqueId, goods.index);
	}
	else
	{
		GameClient::CGame::Instance()->getNetwork()->getSender()->sendBuyRespawn(false, -1, -1);
	}
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketBuyRespawnResult>& packet)
{
	if (packet->code == 1)
	{
		GameClient::CGame::Instance()->getNetwork()->getSender()->sendRebirth();
		GameClient::CGame::Instance()->getNetwork()->getSender()->sendResumeOrder(packet->result);
	}
	else
	{
		GameClient::CGame::Instance()->getNetwork()->getSender()->sendBuyRespawn(false, -1, -1);
	}
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketShowGoNpcMerchant>& packet)
{
	ShowGoNpcMerchantTipDialogEvent::emit(packet->entityId, packet->x, packet->y, packet->z, packet->yaw);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketUpdateBackpack>& packet)
{
	auto player = BLOCKMAN::Blockman::Instance()->m_pPlayer;
	if (!player)
		LordLogError("S2CPacketUpdateBackpack error: player is null");
	player->resetBackpack(packet->capacity, packet->maxCapacity);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketRankData>& packet)
{
	GameClient::RankManager::Instance()->setRankInfo(packet->entityId, packet->result.c_str());
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketActivateTrigger> & packet)
{
	int blockId = packet->blockId;
	BlockPos blockPos;
	blockPos.x = (int)packet->blockPosX;
	blockPos.y = (int)packet->blockPosY;
	blockPos.z = (int)packet->blockPosZ;
	World* world = BLOCKMAN::Blockman::Instance()->getWorld();
	if (blockId != world->getBlockId(blockPos) || !BlockUtil::isCustomBlock(blockId))
	{
		LordLogWarning("failed to process S2CPacketActivateTrigger: block ID is invalid: %d", blockId);
		// TODO: do rollback
		return;
	}
	auto block = dynamic_cast<CustomBlockClient*>(BlockManager::sBlocks[blockId]);
	if (!block)
	{
		LordLogWarning("failed to process S2CPacketActivateTrigger: block is not a CustomBlock, block ID: %d", blockId);
		// TODO: do rollback
		return;
	}
	int index = packet->triggerIndex;
	TriggerType type = static_cast<TriggerType>(packet->triggerType);
	auto triggerManager = TRIGGER_MANAGER::getTriggerManager(type);
	const auto& triggerList = triggerManager->getTriggerList(blockId);
	if ((int)triggerList.size() <= index)
	{
		LordLogWarning("failed to process S2CPacketActivateTrigger: "
			"block with id %d does not have trigger with type: %d, index: %d", 
			blockId, packet->triggerType, index);
		return;
	}
	for (auto action : triggerList[index]->getActions())
	{
		action->doAction(blockPos, { BLOCKMAN::Blockman::Instance()->m_pPlayer });
	}
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketUpdateRankNpc>& packet)
{
	//ReceiveRankResultEvent::emit(packet->result);
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto entityId = dataCache->getClientId(packet->id);
	auto entityRank = dynamic_cast<EntityRankNpc*>(BLOCKMAN::Blockman::Instance()->getWorld()->getEntity(entityId));
	if (!entityRank)
	{
		LordLogError("EntityRank does not exist, id: %d", entityId);
		return;
	}
	String name = packet->name;
	if (name.length() == 0)
	{
		if (packet->nameLang.length() > 0)
		{
			name = LanguageManager::Instance()->getString(packet->nameLang);
		}
		else
		{
			name = LanguageManager::Instance()->getString(LanguageManager::Instance()->getString(LanguageKey::DEFAULT_RANK_NAME));
		}
	}
	entityRank->setName(name);

}

void S2CPlayerActionPacketHandles::HandleUpdateBlock(const NETWORK_DEFINE::S2CPacketUpdateBlockInfo & blockInfo)
{
	Vector3i position(blockInfo.x, blockInfo.y, blockInfo.z);
	BLOCKMAN::Blockman::Instance()->getWorld()->setBlock(position, blockInfo.getBlockId());
	BlockChangeRecorderClient::Instance()->record(position, blockInfo.blockMetaAndId);

#if (LORD_PLATFORM == LORD_PLATFORM_MAC_IOS)
	// add some log, it's the key for debug!
	LordLogInfo("Recive update block pos[%d,%d,%d] id:%d meta:%d", position.x, position.y, position.z, blockInfo.getBlockId(), blockInfo.getBlockMeta());
#endif
	auto world = BLOCKMAN::Blockman::Instance()->getWorld();
	if (world->chunkExists(blockInfo.x >> 4, blockInfo.z >> 4))
	{
		// TODO: I'm not sure what should be the flag
		world->setBlockMetadataWithNotify(position, blockInfo.getBlockMeta(), 3);
	}
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketTakeAircraft>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto entityId = dataCache->getClientId(packet->entityId);
	auto playerId = dataCache->getClientId(packet->passengerId);
	auto pPlayer = BLOCKMAN::Blockman::Instance()->getWorld()->getPlayerEntity(playerId);
	if (!pPlayer)
		return;

	if (packet->isOn)
	{
		auto pAircraft = dynamic_cast<EntityAircraft*>(BLOCKMAN::Blockman::Instance()->getWorld()->getEntity(entityId));
		if (!pAircraft)
		{
			return;
		}

		pPlayer->takeOnAircraft(pAircraft);
	}
	else
	{
		if (BLOCKMAN::Blockman::Instance()->m_pPlayer->entityId == pPlayer->entityId) {
			RootGuiLayout::Instance()->showParachuteBtn(false);
		}
		pPlayer->takeOffAircraft();
	}
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketAircraftStartFly>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto entityId = dataCache->getClientId(packet->entityId);

	auto pAircraft = dynamic_cast<EntityAircraft*>(BLOCKMAN::Blockman::Instance()->getWorld()->getEntity(entityId));
	if (!pAircraft)
	{
		return;
	}

	pAircraft->setPosition(packet->startPos);
	pAircraft->RefreshPassengerPosition(packet->startPos);
	pAircraft->setAirLine(packet->startPos, packet->endPos, packet->speed);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncAircraftState>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto entityId = dataCache->getClientId(packet->entityId);
	auto pAircraft = dynamic_cast<EntityAircraft*>(BLOCKMAN::Blockman::Instance()->getWorld()->getEntity(entityId));
	if (!pAircraft)
	{
		return;
	}

	pAircraft->setAirLine(packet->startPos, packet->endPos, packet->speed);
	pAircraft->setFlying(packet->isFlying);
	pAircraft->setFlyingTick(packet->flyingTick);
	for (auto it = packet->m_passengers.begin(); it != packet->m_passengers.end(); it++)
	{
		auto entityId = dataCache->getClientId(*it);
		auto pPlayer = BLOCKMAN::Blockman::Instance()->getWorld()->getPlayerEntity(entityId);
		if (pPlayer)
			pPlayer->takeOnAircraft(pAircraft);
	}
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketFriendOperation>& packet) 
{
	FriendOperationForServerEvent::emit(packet->operationType, packet->userId);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketChangeAircraftUI>& packet)
{
	bool isShowUI = packet->isShowUI;
	if (isShowUI) {
		RootGuiLayout::Instance()->showParachuteBtn(true);
	}
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncGameTimeShowUi>& packet)
{
	SyncGameTimeShowUiEvent::emit(packet->isShowUI, packet->time);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketAirdrop>& packet)
{
	ChickenAirDropPosition::emit(packet->m_airDropPos);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketMemberLeftAndKill>& packet)
{
	MemberLeftAndKillUpdateEvent::emit(packet->m_left, packet->m_kill);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncBlockDestroyeFailure>& packet) 
{
	auto player = BLOCKMAN::Blockman::Instance()->m_pPlayer;
	if (player && !packet->destroyeStatus) {
		BLOCKMAN::Blockman::Instance()->syncBlockDestroyeFailure(packet->destroyeStatus, packet->destroyeFailureType);
	}
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketSkillType>& packet)
{
	auto player = BLOCKMAN::Blockman::Instance()->m_pPlayer;
	if (player ) 
	{
		switch ((SkillType)packet->skillType)
		{
		case SkillType::DEFENSE:
			ChangeSkillTypeEvent::emit(SkillType::DEFENSE , packet->duration, packet->coldDownTime);
			break;
		case SkillType::SPRINT:
			ChangeSkillTypeEvent::emit(SkillType::SPRINT, packet->duration, packet->coldDownTime);
			break;
		case SkillType::RELEASE_TOXIC:
			ChangeSkillTypeEvent::emit(SkillType::RELEASE_TOXIC, packet->duration, packet->coldDownTime);
			break;
		default:
			ChangeSkillTypeEvent::emit(SkillType::NONE , 0.f, 0.f);
			break;
		}
	}
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CSyncChangePlayerActorInfo>& packet)
{
	SyncChangePlayerActortEvent::emit(packet->changeActorCount, packet->needMoneyCount, packet->actorName);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketHideAndSeekBtnStatus>& packet)
{
	RootGuiLayout::Instance()->showHideAndSeekBtn(packet->isShowChangeActorBtn,packet->isShowChangeCameraBtn,packet->isShowOtherBtn);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncCameraLock>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto entityId = dataCache->getClientId(packet->entityId);
	auto pPlayer = BLOCKMAN::Blockman::Instance()->getWorld()->getPlayerEntity(entityId);
	if (!pPlayer)
		return;
	pPlayer->setCameraLocked(packet->isCameraLock);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketEntitySkillEffect>& packet)
{
	EntitySkillEffect* effect = new EntitySkillEffect();
	effect->name = packet->name;
	effect->duration = packet->duration;
	effect->width = packet->width;
	effect->height = packet->height;
	effect->density = packet->density;
	effect->position = packet->position;
	effect->color = packet->color;
	SkillEffectManager::Instance()->addSkillEffect(effect);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketPickUpItemOrder>& packet)
{
	ShowPickUpItemTipDialogEvent::emit(packet->itemEntityId, packet->itemId, packet->pickUpPrice, (CurrencyType) packet->moneyType);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketRequestResult>& packet)
{
	switch ((BLOCKMAN::ResultCode)packet->resultCode)
	{
	case ResultCode::PAY_SUCCESS:
		
		break;
	case ResultCode::SELL_MANOR_SUCCEED:
	{
		auto player = BLOCKMAN::Blockman::Instance()->m_pPlayer;
		player->m_manor->dealloc();
		ShowResultTipDialogEvent::emit(ResultCode::COMMON, packet->resultMessage);
		ManorOnUpdateEvent::emit();
		SetManorBtnVisibleEvent::emit(false);
		break;
	}
	case ResultCode::MANOR_BAN_OPERATION:
		if (packet->resultMessage.length() > 0)
		{
			ShowResultTipDialogEvent::emit(ResultCode::COMMON, packet->resultMessage);
		}
		RefreshManorUiEvent::emit();
		BLOCKMAN::Blockman::Instance()->m_pPlayer->m_isActorChange = false;
		break;
	case ResultCode::RANCH_ORDER_HELP_RESULT:
		ClientHttpRequest::loadMails();
		ClientHttpRequest::refreshFriend();
		ShowResultTipDialogEvent::emit(ResultCode::COMMON, packet->resultMessage);
		break;
	case ResultCode::GIVE_GIFT:
		ClientHttpRequest::loadMails();
		ClientHttpRequest::refreshFriend();
		ShowResultTipDialogEvent::emit(ResultCode::COMMON, packet->resultMessage);
		break;
	case ResultCode::MAIL_RECEIVE_RESULT:
		ClientHttpRequest::loadMails();
		ShowResultTipDialogEvent::emit(ResultCode::COMMON, packet->resultMessage);
		break;
	case ResultCode::RANCH_STORAGE_FULL:
		ShowRanchStorageFullTipEvent::emit(packet->resultMessage);
		break;
	case ResultCode::BIRD_RECEIVE_TASK:
		BirdAcceptTaskEvent::emit();
		break;	
	case ResultCode::BIRD_BUY_PERMISSION:
		BirdBuyPermissionEvent::emit(packet->resultMessage);
		break;
	default:
		ShowResultTipDialogEvent::emit((BLOCKMAN::ResultCode)packet->resultCode, packet->resultMessage);
		break;
	}

	LordLogInfo("S2CPacketPayResult code: %d  message:  %s", packet->resultCode, packet->resultMessage.c_str());
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketDeathUnequipArmor>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto entityId = dataCache->getClientId(packet->entityId);
	auto pPlayer = BLOCKMAN::Blockman::Instance()->getWorld()->getPlayerEntity(entityId);
	if (pPlayer)
	{
		pPlayer->inventory->clear();
		pPlayer->m_outLooksChanged = true;
	}
	LordLogInfo("S2CPacketDeathUnequipArmor entityId: %d", packet->entityId);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncChangeEntityPerspece>& packet)
{
	auto player = BLOCKMAN::Blockman::Instance()->m_pPlayer;
	if (player && (packet->View >=0 && packet->View < 3)){
		BLOCKMAN::Blockman::Instance()->setPersonView(packet->View);
	}
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketPlayerInvisible>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto entityId = dataCache->getClientId(packet->entityId);
	auto player = BLOCKMAN::Blockman::Instance()->getWorld()->getPlayerEntity(entityId);
	if (player)
		player->setActorInvisible(packet->isInvisible);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketChangeHeart>& packet)
{
	ChangeHeartEvent::emit(packet->hp, packet->maxHp);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncSessionNpc>& packet)
{
	LordLogInfo("S2CPacketSyncSessionNpc sessionType  = %d  sessionContent = %s", packet->sessionType, packet->sessionContent.c_str());
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto sessionNpc = dynamic_cast<EntitySessionNpc*>(dataCache->getEntityByServerId(packet->entityId));
	if (sessionNpc)
	{
		String name = packet->name;
		if (name.length() == 0)
		{
			if (packet->nameLang.length() > 0)
			{
				name = LanguageManager::Instance()->getString(packet->nameLang);
			}
		}

		sessionNpc->setName(name.c_str());
		sessionNpc->setActorName(packet->actorName.c_str());
		sessionNpc->setActorBody(packet->actorBody.c_str());
		sessionNpc->setActorBodyId(packet->actorBodyId.c_str());
		sessionNpc->setSessionType(packet->sessionType);
		sessionNpc->setSessionContent(packet->sessionContent.c_str());
		sessionNpc->setEffectName(packet->effectName.c_str());
		sessionNpc->setPerson(packet->isPerson);
		sessionNpc->setTimeLeft(packet->timeLeft);
		sessionNpc->setActorAction(packet->actorAction);
		sessionNpc->setCanCollided(packet->isCanCollided);

		BLOCKMAN::Blockman::Instance()->getWorld()->changeEntityActor(sessionNpc, packet->actorName);
	}
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketUpdateManor>& packet)
{
	auto player = BLOCKMAN::Blockman::Instance()->m_pPlayer;
	if (player)
	{
		player->m_manor->setInfo(&packet->m_manorInfo);
		player->m_manor->setNextInfo(&packet->m_nextManorInfo);

		for (ManorHouse item : packet->m_manorHouses)
		{
			player->m_manor->addHouse(&item);
		}

		for (ManorFurniture item : packet->m_manorFurnitures)
		{
			player->m_manor->addFurniture(&item);
		}

		for (ManorMessage item : packet->m_manorMessages)
		{
			player->m_manor->addMessage(&item);
		}

		ManorOnUpdateEvent::emit();
	}
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketUpdateRealTimeRankInfo>& packet)
{
	RootGuiLayout::Instance()->updateRealTimeRankData(packet->result);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketUpdateGameMonsterInfo>& packet)
{
	UpdateGameMonsterInfoEvent::emit(packet->currCheckPointNum, packet->surplusMonsterNum, packet->surplusCheckPointNum);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketUpdateBossBloodStrip>& packet)
{
	UpdateBossBloodStripEvent::emit(packet->currBloodStrip, packet->maxBloodStrip);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketSwitchablePropsData>& packet)
{
	ChangeSwitchablePropsEvent::emit(packet->data);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketUpgradePropsData>& packet)
{
	ChangeUpgradePropsEvent::emit(packet->data);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketPersonalShopTip>& packet)
{
	PersonalShopTipEvent::emit(packet->tip);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketCreateHouseFromSchematic>& packet)
{
	String path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), BLOCKMAN::Blockman::Instance()->m_worldName, "dynamic", packet->fileName);
	Vector3i start_pos = packet->start_pos;
	bool xImage = packet->xImage;
	bool zImage = packet->zImage;

	bool createOrDestroy = packet->createOrDestroy;

	auto world = BLOCKMAN::Blockman::Instance()->getWorld();
	if (world)
	{
		world->createOrDestroyHouseFromSchematic(path.c_str(), start_pos, xImage, zImage, createOrDestroy);
	}
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketFillAreaByBlockIdAndMate>& packet)
{
	Vector3i start_pos = packet->start_pos;
	Vector3i end_pos = packet->end_pos;
	int block_id = packet->block_id;
	int mate = packet->mate;

	auto world = BLOCKMAN::Blockman::Instance()->getWorld();
	if (world)
	{
		world->fillAreaByBlockIdAndMate(start_pos, end_pos, block_id, mate);
	}
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketShowPlayerOperation>& packet)
{
	ShowPlayerOperationEvent::emit(packet->m_targetUserId, packet->m_targetUserName);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketPlayerSpeedLevel>& packet)
{
	if (BLOCKMAN::Blockman::Instance()->m_pPlayer){
		BLOCKMAN::Blockman::Instance()->m_pPlayer->setSpeedAdditionLevel(packet->speedLevel);
	}
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketShowUpgradeResourceUI>& packet)
{
	ShowUpgradeResourceEvent::emit(packet->itemId, packet->itemNum, packet->resourceId, packet->tipString);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketManorBtnVisible>& packet)
{
	SetManorBtnVisibleEvent::emit(packet->visible);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketCallOnManorResetClient>& packet)
{
	GameClient::CGame::Instance()->resetGame(packet->m_targetUserId);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketUpdateManorOwner>& packet)
{
	auto world = BLOCKMAN::Blockman::Instance()->getWorld();
	if (world)
	{
		std::vector<ManorOwner> owners;
		for (auto item : packet->m_owners)
		{
			owners.push_back(item);
		}

		world->updateManorOwner(0, owners);
	}
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketPlayerExpInfo>& packet)
{
	if (BLOCKMAN::Blockman::Instance()->m_pPlayer){
		BLOCKMAN::Blockman::Instance()->m_pPlayer->setXPStats(packet->exp, packet->maxExp, packet->level);
	}
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketUpdateBasementLife>& packet)
{
	UpdateBasementLifeEvent::emit(packet->curLife, packet->maxLife);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketUpdateChestInventory>& packet)
{
	LordLogInfo("=========S2CPacketUpdateChestInventory=========");
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto world = dataCache->getWorldClient();
	int	blockID = world->getBlockId(BlockPos((int)packet->blockPosX, (int)packet->blockPosY, (int)packet->blockPosZ));

	if (blockID > 0)
	{
		IInventory* inventory = nullptr;

		if (dynamic_cast<BlockChest*>(BlockManager::sBlocks[blockID]))
		{
			inventory = dynamic_cast<BlockChest*>(BlockManager::sBlocks[blockID])->getInventory(world, BlockPos((int)packet->blockPosX, (int)packet->blockPosY, (int)packet->blockPosZ));
		}

		if (dynamic_cast<BlockEnderChest*>(BlockManager::sBlocks[blockID]))
		{
			inventory = (IInventory*)BLOCKMAN::Blockman::Instance()->m_pPlayer->getInventoryEnderChest();
		}

		if (!inventory)
			return;

		for (auto iter = packet->chestInventory.begin(); iter != packet->chestInventory.end(); iter++)
		{
			ItemStackPtr chestItemStack = inventory->getStackInSlot(iter->index);
			if (chestItemStack)
			{
				inventory->decrStackSize(iter->index, chestItemStack->stackSize);
			}
			if (!iter->valid)
			{
				continue;
			}
			chestItemStack = LORD::make_shared<ItemStack>(iter->id, iter->size, iter->damage);
			chestItemStack->m_currentClipBulletNum = iter->curClipBullet;
			for (const auto& enchantmentInfo : iter->enchantments)
			{
				chestItemStack->addEnchantment(Enchantment::enchantmentsList[enchantmentInfo.id], enchantmentInfo.level);
			}
			inventory->setInventorySlotContents(iter->index, chestItemStack);
		}
	}
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketCustomTipMsg>& packet)
{
	ShowCustomTipMsgEvent::emit(packet->messageLang, packet->extra);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketEntityPlayerOccupation>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto player = dynamic_cast<EntityPlayer*>(dataCache->getEntityByServerId(packet->entityId));
	if (player)
	{
		player->setOccupation(packet->occupation);
	}
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncShowMaskTime>& packet)
{
	SyncShowMaskTimeEvent::emit(packet->MilliSecond);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketShowBuildWarBlockBtn>& packet)
{
	RootGuiLayout::Instance()->showBuildWarBlockBtn(packet->isNeedOpen);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketShowBuildWarPlayNum>& packet)
{
	ShowBuildWarPlayerNum::emit(packet->cur_num, packet->max_num);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketShowBuildWarLeftTime>& packet)
{
	ShowBuildWarLeftTime::emit(packet->show, packet->left_time, packet->theme);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketOpenShopByEntityId>& packet)
{
	int entityId = ClientNetwork::Instance()->getDataCache()->getClientId(packet->entityId);
	auto merchant = dynamic_cast<EntityMerchant*>(BLOCKMAN::Blockman::Instance()->getWorld()->getEntity(entityId));
	if (merchant)
	{
		MerchantActivateEvent::emit(merchant);
	}
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketShowGrade>& packet)
{
	if (packet->openOrClose)
	{
		RootGuiLayout::Instance()->showBuildWarGrade(packet->show);
	}
	else
	{
		if (!RootGuiLayout::Instance()->isBuildWarGradeShown())
		{
			RootGuiLayout::Instance()->showBuildWarGrade(packet->show);
		}
		ShowBuildWarGrade::emit(packet->msg, packet->left_time);
	}
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketShowGuessResult>& packet)
{
	RootGuiLayout::Instance()->showMainControl();
	RootGuiLayout::Instance()->showBuildWarGuessResult(true);
	SetBuildWarGuessShow::emit(false);
	ShowBuildWarGuessResult::emit(packet->right, packet->rank);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketShowGuessUi>& packet)
{
	RootGuiLayout::Instance()->showMainControl();
	RootGuiLayout::Instance()->showBuildWarGuessUi(true);
	ShowBuildWarGuessUi::emit(packet->info, packet->guess_room_id);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketGameSettlementExtra>& packet)
{
	ReceiveFinalSummaryExtraEvent::emit(packet->guess_right, packet->guess_name, packet->guess_reward);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketUnlockedCommodity>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto merchantId = dataCache->getClientId(packet->merchantId);
	auto merchant = dynamic_cast<EntityMerchant*>(BLOCKMAN::Blockman::Instance()->getWorld()->getEntity(merchantId));
	if (!merchant)
	{
		LordLogError("EntityMerchant does not exist, id: %d", merchantId);
		return;
	}
	BLOCKMAN::Blockman::Instance()->m_pPlayer->addUnlockedCommodity(packet->itemId, packet->meta);
	UpdateUnlockedCommodityEvent::emit();
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketInitUnlockedCommodity>& packet)
{
	BLOCKMAN::Blockman::Instance()->m_pPlayer->addUnlockedCommodity(packet->itemId, packet->meta);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketTeamResourcesUpdate>& packet)
{
	TeamResourcesUpdateEvent::emit(packet->m_data.c_str());
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketCloseBGM>& packet)
{
	SoundSystem::Instance()->stopBackgroundMusic(false);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketSetArmItem>& packet)
{
	if (packet->itemId <= 0)
		BLOCKMAN::Blockman::Instance()->m_pPlayer->removeArmStack();
	else
		BLOCKMAN::Blockman::Instance()->m_pPlayer->setArmStack(LORD::make_shared<ItemStack>(packet->itemId, 1, 0));
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketBuildProgress>& packet)
{
	UpdateBuildProgressEvent::emit(packet->name, packet->curProgress, packet->maxProgress, packet->iconPath);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketEnchantmentPropsData>& packet)
{
	ChangeEnchantmentPropsEvent::emit(packet->data);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketConsumeCoinTip>& packet)
{
	ShowConsumeCoinTipEvent::emit(packet->message, packet->coinId, packet->price, packet->extra);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketSuperPropsData>& packet)
{
	ChangeSuperPropsEvent::emit(packet->data);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketSuperShopTip>& packet)
{
	SuperShopTipEvent::emit(packet->tip);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketChangeDefense>& packet)
{
	ChangeDefenseEvent::emit(packet->defense, packet->maxDefense);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketChangeAttack>& packet)
{
	ChangeAttackEvent::emit(packet->attack, packet->maxAttack);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketAddSimpleEffect>& packet)
{
	WorldEffectManager::Instance()->addSimpleEffect(packet->effectName, packet->position, packet->yaw, packet->duration, packet->scale, packet->scale, packet->scale);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketShowPersonalShop>& packet)
{
	OpenPersonalShopEvent::emit();
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketPersonalShopArea>& packet)
{
	PersonalShopAreaEvent::emit(packet->startPos, packet->endPos);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketAddGunBulletNum>& packet)
{
	auto player = BLOCKMAN::Blockman::Instance()->m_pPlayer;
	if (player)
	{
		player->addGunBulletNum(packet->gunId, packet->bulletNum);
	}
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketSubGunRecoil>& packet)
{
	auto player = BLOCKMAN::Blockman::Instance()->m_pPlayer;
	if (player)
	{
		player->subGunRecoil(packet->gunId, packet->recoil);
	}
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketCustomPropsData>& packet)
{
	ChangeCustomPropsEvent::emit(packet->data);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketUpdateCustomProps>& packet)
{
	UpdateCustomPropsEvent::emit(packet->data);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CSyncCannonActorInfo>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto entityId = dataCache->getClientId(packet->entityId);
	auto entityActorCannon = dynamic_cast<EntityActorCannonClient*>(BLOCKMAN::Blockman::Instance()->getWorld()->getEntity(entityId));

	if (!entityActorCannon)
	{
		LordLogError("EntityActorCannonClient does not exist, id: %d", entityId);
		return;
	}
	entityActorCannon->setActorName(packet->actorName);
	entityActorCannon->setHeadName(packet->headName);
	entityActorCannon->setSkillName(packet->skillName);
	entityActorCannon->setHaloEffectName(packet->haloEffectName);
	entityActorCannon->setCanObstruct(packet->isCanObstruct);
	entityActorCannon->setContent(packet->content);
	EntityRenderManager::Instance()->refreshEntityActor(BLOCKMAN::Blockman::Instance()->getWorld()->getEntity(entityId), entityActorCannon->getActorName());
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncCannonFire>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto entityId = dataCache->getClientId(packet->cannonEntityId);
	auto entityActorCannon = dynamic_cast<EntityActorCannonClient*>(BLOCKMAN::Blockman::Instance()->getWorld()->getEntity(entityId));

	if (!entityActorCannon)
	{
		return;
	}

	entityActorCannon->m_hasBeenLaunched = true;
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketShowRanchExTip>& packet)
{
	ShowRanchExTip::emit(packet->type, packet->num);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketShowRanchExTask>& packet)
{
	ShowRanchExTask::emit(packet->show, packet->task);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketBroadcastMessage>& packet)
{
	LordLogInfo("S2CPacketBroadcastMessage: type[%d] conten:[%s]", packet->type, packet->content.c_str());
	BroadcastManager::parseBroadcastData(packet->type, packet->content);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketGotoOtherGame>& packet)
{
	LordLogInfo("S2CPacketGotoOtherGame: gameType[%s] mapId[%s]", packet->gameType.c_str(), packet->mapId.c_str());
	GameClient::CGame::Instance()->getShellInterface()->resetGame(packet->targetUserId, packet->gameType, packet->mapId);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketShowRanchExCurrentItemInfo>& packet)
{
	ShowRanchExCurrentItemInfo::emit(packet->show, packet->itemInfo, packet->type, packet->num);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketShowRanchExItem>& packet)
{
	ShowRanchExItem::emit(packet->show, packet->item);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketCropInfo>& packet)
{
	if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::Rancher)
	{
		ShowCropInfoEvent::emit(packet->blockPos, packet->blockId, packet->stage, packet->curStageTime, packet->residueHarvestNum);
	}
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketOccupationUnlock>& packet)
{
	ShowOccupationUnlockTip::emit(packet->occupationInfo);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketBuyFlying>& packet)
{
	GameClient::CGame::Instance()->getShellInterface()->onRecharge(2);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketPlayCloseup>& packet)
{
	BLOCKMAN::Blockman::Instance()->initCloseup(true, packet->playerPos, packet->closeupPos, packet->farDistance, packet->nearDistance, packet->velocity, packet->duration, packet->yaw, packet->pitch);
	BLOCKMAN::Blockman::Instance()->m_entityRender->resetCloseup();
	BLOCKMAN::Blockman::Instance()->m_playerControl->setDisableMovement(true);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketShowBuyKeepItemTip>& packet)
{
	ShowKeepItemTipEvent::emit(packet->coinType, packet->coin, packet->tipTime);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketSendOpenEnchantment>& packet)
{
	RootGuiLayout::Instance()->setEnchantmentShow(true);
	ShowEnchantMentEvent::emit(packet->data);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketSubGunFireCd>& packet)
{
	auto player = Blockman::Instance()->m_pPlayer;
	if (player)
	{
		player->subGunFireCd(packet->gunId, packet->fireCd);
	}
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketLotteryData>& packet)
{
	TigerLotteryDataEvent::emit(packet->data);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketLotteryResult>& packet)
{
	TigerLotteryResultEvent::emit(packet->firstId, packet->secondId, packet->thirdId);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketHideAndSeekHallResult>& packet)
{
	RootGuiLayout::Instance()->showHideAndSeekHallResult();
	HideAndSeekHallResult::emit(packet->itemInfo);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketSendOpenPixelGunHallModeSelect>& packet)
{
	RootGuiLayout::Instance()->setPixelGunHallModeSelectShow(packet->open);
	if (packet->open)
		ShowPixelGunHallModeSelectEvent::emit(packet->data);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketUpdateStoreGunData>& packet)
{
	UpdateStoreGunDataEvent::emit(packet->data);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketUpdateStorePropData>& packet)
{
	UpdateStorePropDataEvent::emit(packet->data);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketShowGunStore>& packet)
{
	OpenGunStoreEvent::emit();
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncEffectGunFire>& packet)
{
	GunSetting* pGunSetting = GunSetting::getGunSetting(packet->gunId);
	EntityPlayer* player = ClientNetwork::Instance()->getDataCache()->getPlayerByServerId(packet->shootingEntityId);
	if (!pGunSetting || !player)
	{
		LordLogError("S2CPacketSyncEffectGunFire shootingPlayer or gunSetting is null");
		return;
	}

	Blockman::Instance()->m_pPlayer->playSoundByType((SoundType)pGunSetting->soundType);

	EntityGunFlame* gunFlame = new EntityGunFlame(Blockman::Instance()->getWorld(), player, pGunSetting);
	Blockman::Instance()->getWorld()->spawnEntityInWorld(gunFlame);

	EntityBulletClient* pBullet = nullptr;
	Vector3 beginPos = Vector3(packet->beginPos[0], packet->beginPos[1], packet->beginPos[2]);
	Vector3 endPos = Vector3(packet->endPos[0], packet->endPos[1], packet->endPos[2]);
	switch (pGunSetting->gunType)
	{
	case RIFLE_EFFECT_GUN:
	case SNIPER_EFFECT_GUN:
		if (!pGunSetting->isPiercing)
		{
			pBullet = new EntityBulletClient(Blockman::Instance()->getWorld(), player, pGunSetting, beginPos, endPos);
		}
		else
		{
			pBullet = new EntityArmorPiercingBulletClient(Blockman::Instance()->getWorld(), player, pGunSetting, beginPos, endPos);
		}
		Blockman::Instance()->getWorld()->spawnEntityInWorld(pBullet);
		break;
	case SHOTGUN_EFFECT_GUN:
		for (int i = 0; i < pGunSetting->bulletPerShoot; i++)
		{
			if (!pGunSetting->isPiercing)
			{
				pBullet = new EntityBulletClient(Blockman::Instance()->getWorld(), player, pGunSetting, beginPos, endPos, Math::UnitRandom() * 6.0f - 3.0f, Math::UnitRandom() * 6.0f - 3.0f);
			}
			else
			{
				pBullet = new EntityArmorPiercingBulletClient(Blockman::Instance()->getWorld(), player, pGunSetting, beginPos, endPos, Math::UnitRandom() * 6.0f - 3.0f, Math::UnitRandom() * 6.0f - 3.0f);
			}
			Blockman::Instance()->getWorld()->spawnEntityInWorld(pBullet);
		}
		break;
	case MORTAR_EFFECT_GUN:
		pBullet = new EntityMortarBulletClient(Blockman::Instance()->getWorld(), player, pGunSetting, beginPos, endPos);
		Blockman::Instance()->getWorld()->spawnEntityInWorld(pBullet);
		break;
	case LASER_EFFECT_GUN:
		pBullet = new EntityLaserBulletClient(Blockman::Instance()->getWorld(), player, pGunSetting, beginPos, endPos);
		Blockman::Instance()->getWorld()->spawnEntityInWorld(pBullet);
		break;
	case ELECTRICITY_EFFECT_GUN:
		pBullet = new EntityLaserBulletClient(Blockman::Instance()->getWorld(), player, pGunSetting, beginPos, endPos, 0.01f);
		Blockman::Instance()->getWorld()->spawnEntityInWorld(pBullet);
		break;
	case MOMENTARY_LASER_EFFECT_GUN:
		pBullet = new EntityMomentaryLaserBulletClient(Blockman::Instance()->getWorld(), player, pGunSetting, beginPos, endPos);
		Blockman::Instance()->getWorld()->spawnEntityInWorld(pBullet);
		break;
	case MOMENTARY_PIERCING_LASER_EFFECT_GUN:
		pBullet = new EntityMomentaryPiercingLaserBulletClient(Blockman::Instance()->getWorld(), player, pGunSetting, beginPos, endPos);
		Blockman::Instance()->getWorld()->spawnEntityInWorld(pBullet);
		break;
	}
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncEffectGunFireAtEntityResult>& packet)
{
	auto pWorld = Blockman::Instance()->getWorld();
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	EntityPlayer* player = dataCache->getPlayerByServerId(packet->shootingEntityId);
	EntityPlayer* targetPlayer = dataCache->getPlayerByServerId(packet->hitEntityId);
	switch (packet->traceType)
	{
	case ENTITY_PLAYER_TRACE_TYPE:
		if (player && targetPlayer)
		{
			if (targetPlayer == Blockman::Instance()->m_pPlayer && (LogicSetting::Instance()->getShowGunEffectWithSingleStatus() || player->getTeamId() != targetPlayer->getTeamId()))
			{
				bool isScreenLeft = false;
				Vector3 targetDir = player->position - targetPlayer->position;
				targetDir.y = 0.0f;
				targetDir.normalize();
				if ((targetDir.x < -0.00001f) || (targetDir.x > 0.00001f) || (targetDir.z < -0.00001f) || (targetDir.z > 0.00001f))
				{
					Vector3 viewDir = SceneManager::Instance()->getMainCamera()->getDirection();
					viewDir.y = 0;
					viewDir.normalize();
					float cosValue = targetDir.dot(viewDir);
					Vector3 tangent = targetDir.cross(viewDir);
					if (tangent.y < 0)isScreenLeft = true;
					ShootWarningEvent::emit(Math::ACos(cosValue), isScreenLeft);
				}

			}
			if (packet->isHeadshot && LogicSetting::Instance()->getAllowHeadshotStatus() && (LogicSetting::Instance()->getShowGunEffectWithSingleStatus() || player->getTeamId() != targetPlayer->getTeamId()))
			{
				WorldEffectManager::Instance()->addSimpleEffect("headshot.effect", Vector3(targetPlayer->getPosition().x, targetPlayer->getPosition().y + targetPlayer->getEyeHeight(), targetPlayer->getPosition().z), targetPlayer->getYaw(), 1000);
			}
		}
	case ENTITY_CREATUREAI_TRACE_TYPE:
		for (int i = 0; i < 8; ++i)
		{
			Vector3 rv = pWorld->m_Rand.nextVector();
			String name = StringUtil::Format("tilecrack_%d_%d", BLOCK_ID_BLOCK_REDSTONE, 0); // use redstoneblock as blooding!!! --!
			pWorld->spawnParticle(name, Vector3(packet->hitPos[0], packet->hitPos[1], packet->hitPos[2]), Vector3(packet->dir[0] * -2.f + rv.x, 3.f * rv.y, packet->dir[2] * -2.f + rv.z));
		}
		WorldEffectManager::Instance()->addSimpleEffect("effect_gun_be_shot.effect", Vector3(packet->hitPos[0], packet->hitPos[1], packet->hitPos[2]), Math::ATan2(-packet->dir[2], -packet->dir[0]) * Math::RAD2DEG - 90.0f, 500);
		break;
	default:
		break;
	}
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncEffectGunFireAtBlockResult>& packet)
{
	Vector3i blockPos = Vector3i(packet->blockPos[0], packet->blockPos[1], packet->blockPos[2]);
	Vector3 hitPos = Vector3(packet->hitPos[0], packet->hitPos[1], packet->hitPos[2]);
	auto pWorld = Blockman::Instance()->getWorld();
	int blockId = pWorld->getBlockId(blockPos);

	if (blockId > 0)
	{
		for (int i = 0; i < 8; ++i)
		{
			Vector3 rv = pWorld->m_Rand.nextVector();
			String name = StringUtil::Format("tilecrack_%d_%d", blockId, pWorld->getBlockMeta(blockPos));
			pWorld->spawnParticle(name, hitPos, Vector3(packet->dir[0] * -2.f + rv.x, 3.f * rv.y, packet->dir[2] * -2.f + rv.z));
		}
		WorldEffectManager::Instance()->addSimpleEffect("effect_gun_be_shot.effect", hitPos, Math::ATan2(-packet->dir[2], -packet->dir[0]) * Math::RAD2DEG - 90.0f, 500);
	}
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncEffectGunFireExplosionResult>& packet)
{
	WorldEffectManager::Instance()->addSimpleEffect("effect_gun_mortar_explosion.effect", Vector3(packet->hitPos[0], packet->hitPos[1], packet->hitPos[2]), 0.0f, 500, packet->explosionRange, packet->explosionRange, packet->explosionRange);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketStopLaserGun>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto entityId = dataCache->getClientId(packet->shootingEntityId);
	auto pWorld = Blockman::Instance()->getWorld();
	EntityArr entities = pWorld->getBulletEntity(entityId);
	for (auto entity : entities)
	{
		EntityLaserBulletClient* laserBulletEntity = dynamic_cast<EntityLaserBulletClient*>(entity);
		if (laserBulletEntity)
		{
			laserBulletEntity->setDead();
		}
	}
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketShowPixelGunHallInfo>& packet)
{
	ShowPixelGunHallInfo::emit(packet->lv, packet->cur_exp, packet->max_exp, packet->yaoshi, packet->is_max);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketSendOpenPixelGun1v1>& packet)
{
	RootGuiLayout::Instance()->setPixelGun1V1Show(packet->open);
	if (packet->open)
		ShowPixelGun1v1Event::emit(packet->data);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketShowChestLottery>& packet)
{
	UpdateChestLotteryIntegralEvent::emit(packet->curIntegral, packet->needIntegral);
	if (packet->open)
		OpenChestLotteryEvent::emit();
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketChestLotteryResult>& packet)
{
	ShowChestRewardResultEvent::emit(packet->rewardId);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketSendOpenPixelGunRevive>& packet)
{
	RootGuiLayout::Instance()->setPixelGunReviveShow(packet->open);
	if (packet->open)
		ShowPixelGunReviveEvent::emit(packet->data);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketUpdateLotteryChestData>& packet)
{
	UpdateLotteryChestDataEvent::emit(packet->data);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketShowPixelGunGamePerson>& packet)
{
	ShowPixelGunGamePerson::emit(packet->time, packet->rank, packet->kill_num);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketShowPixelGunGame1v1>& packet)
{
	ShowPixelGunGame1v1::emit(packet->time, packet->red_kill_num, packet->blue_kill_num, packet->self_team);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketShowPixelGunGameTeam>& packet)
{
	ShowPixelGunGameTeam::emit(packet->time, packet->red_kill_num, packet->red_cur_player_num, packet->red_max_player_num, 
		packet->blue_kill_num, packet->blue_cur_player_num, packet->blue_max_player_num, packet->self_team);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketSendOpenArmorUpgrade>& packet)
{
	RootGuiLayout::Instance()->showPixelGunHallArmorUpgrade(packet->open);
	ShowPixelGunHallArmorUpgradeEvent::emit(packet->data);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketSendOpenPixelGunResult>& packet)
{
	RootGuiLayout::Instance()->setPixelGunResultShow(packet->open);
	if (packet->open)
		ShowPixelGunResultEvent::emit(packet->data, packet->result_type);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketPlayerCurrentSeasonInfo>& packet)
{
	UpdateCurrentSeasonInfoEvent::emit(packet->honorId, packet->rank, packet->honor, packet->endTime);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketPlayerLastSeasonInfo>& packet)
{
	UpdateLastSeasonInfoEvent::emit(packet->honorId, packet->rank, packet->honor);
	OpenSeasonRewardEvent::emit();
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketDisarmament>& packet)
{
	UpdateDisarmamentStatusEvent::emit(packet->isDisarmament);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketSetPlayerOpacity>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto entityId = dataCache->getClientId(packet->entityId);
	EntityPlayer* pPlayer = BLOCKMAN::Blockman::Instance()->getWorld()->getPlayerEntity(entityId);
	if (pPlayer)
	{
		pPlayer->m_opacity = packet->opacityValue;
	}
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketSendOpenPixelGunLvUp>& packet)
{
	RootGuiLayout::Instance()->showPixelGunGameLvUp(packet->open);
	if (packet->open)
		ShowPixelGunLvupEvent::emit(packet->data);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketSendOpenPixelGunResultSpecialInfo>& packet)
{
	if (RootGuiLayout::Instance()->isPixelGunResultOpen())
	{
		ShowPixelGunResultSpecialInfoEvent::emit(packet->btnRevengeEnable);
	}
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketNotifyGetGoods>& packet)
{
	BLOCKMAN::Blockman::Instance()->m_pPlayer->playSoundByType(ST_GetItem);
	BLOCKMAN::GetGoodsEvent::emit(packet->icon, packet->count);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketShowSeasonRank>& packet)
{
	OpenSeasonRankEvent::emit();
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketPlayerNamePerspective>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto entityId = dataCache->getClientId(packet->entityId);
	auto player = BLOCKMAN::Blockman::Instance()->getWorld()->getPlayerEntity(entityId);
	if (player)
		player->setNamePerspective(packet->isPerspective);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketAddCustomEffect>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto entityId = dataCache->getClientId(packet->entityId);
	auto player = BLOCKMAN::Blockman::Instance()->getWorld()->getPlayerEntity(entityId);
	if (player)
		player->addCustomEffect(packet->name, packet->effectName, packet->duration);
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncCustomEffect>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto entityId = dataCache->getClientId(packet->entityId);
	auto player = BLOCKMAN::Blockman::Instance()->getWorld()->getPlayerEntity(entityId);
	if (player)
	{
		vector<CustomEffect>::type effects = packet->customEffects;
		for (auto effect : effects)
		{
			player->addCustomEffect(effect.m_name, effect.m_effect_name, effect.m_duration);
		}
	}
}

void S2CPlayerActionPacketHandles::handlePacket(std::shared_ptr<S2CPacketSyncPlayerBagInfo>& packet)
{
	auto dataCache = ClientNetwork::Instance()->getDataCache();
	auto player = dynamic_cast<EntityPlayer*>(dataCache->getEntityByServerId(packet->entityId));
	if (player)
	{
		player->setBirdConvert(packet->isConvert);
		player->setBagInfo(packet->curCapacity, packet->maxCapacity);
	}
}
