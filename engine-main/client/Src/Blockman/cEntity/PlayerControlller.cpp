#include "PlayerControlller.h"

#include "cEntity/EntityPlayerSPMP.h"
#include "cWorld/Blockman.h"
#include "cWorld/PlayerInteractionManager.h"
#include "cWorld/Blockman.h"
#include "Network/ClientNetwork.h"

/** header files from common. */
#include "cBlock/cBlock.h"
#include "cBlock/cBlockManager.h"
#include "Item/Item.h"
#include "Item/Items.h"
#include "Item/ItemStack.h"
#include "Entity/EntityPlayer.h"
#include "World/World.h"
#include "Inventory/InventoryPlayer.h"
#include "Inventory/Container.h"
#include "Audio/SoundSystem.h"
#include "Util/ClientEvents.h"
#include "Setting/LogicSetting.h"
#include "game.h"

namespace BLOCKMAN
{

PlayerController::PlayerController(Blockman* pbm/*, NetClientHandler par2NetClientHandler*/)
{
	currentGameType = GAME_TYPE_SURVIVAL;
	bm = pbm;
	currentBlockPos = Vector3i(-1, -1, -1);
	currentItemHittingBlock = NULL;
	curBlockDamageMP = 0.f;
	stepSoundTickCounter = 0.f;
	blockHitDelay = 0;
	isHittingBlock = false;
	/** Current game type for the player */
	currentPlayerItem = 0;
}

void PlayerController::clickBlockCreative(Blockman* pmc, PlayerController* ppc, const BlockPos& pos, int face)
{
	if (!pmc->m_pWorld->extinguishFire(pmc->m_pPlayer, pos, face))
	{
		ppc->onPlayerDestroyBlock(pos, face);
	}
}

void PlayerController::setPlayerCapabilities(EntityPlayer* pPlayer)
{
	// currentGameType.configurePlayerCapabilities(par1EntityPlayer->capabilities);
}

void PlayerController::setGameType(GAME_TYPE type)
{
	currentGameType = type;
	// currentGameType.configurePlayerCapabilities(bm.thePlayer.capabilities);
}

void PlayerController::flipPlayer(EntityPlayer* pPlayer)
{
	pPlayer->rotationYaw = -180.0F;
}

bool PlayerController::shouldDrawHUD()
{
	return currentGameType == GAME_TYPE_SURVIVAL || currentGameType == GAME_TYPE_ADVENTURE;
}

bool PlayerController::onPlayerDestroyBlock(const BlockPos& pos, int face)
{
	if (currentGameType == GAME_TYPE_ADVENTURE && !bm->m_pPlayer->isCurrentToolAdventureModeExempt(pos))
		return false;
	
	World* pWorld = bm->m_pWorld;
	Block* pBlock = BlockManager::sBlocks[pWorld->getBlockId(pos)];

	if (!pBlock)
		return false;

	bool bResult = false;

	if (!dynamic_cast<BlockFruits*>(pBlock))
	{
		pWorld->playAuxSFX(2001, pos, pBlock->getBlockID() + (pWorld->getBlockMeta(pos) << 12));
		bResult = LogicSetting::Instance()->isBreakBlockSoon() ? pWorld->setBlockToAir(pos) : true;
	}
	else 
	{ 
		bResult = true; 
	}

	int meta = pWorld->getBlockMeta(pos);
	if (bResult)
	{
		ClientNetwork::Instance()->getSender()->sendFinishBreakBlock();
	}

	currentBlockPos.y = -1;

	if (currentGameType != GAME_TYPE_CREATIVE)
	{
		ItemStackPtr pStack = bm->m_pPlayer->getCurrentEquippedItem();

		if (pStack)
		{
			pStack->onBlockDestroyed(pWorld, pBlock->getBlockID(), pos, bm->m_pPlayer);

			if (pStack->stackSize == 0)
			{
				bm->m_pPlayer->destroyCurrentEquippedItem();
			}
		}
	}

	return bResult;
}

void PlayerController::clickBlock(const BlockPos& pos, int face)
{
	if (currentGameType != GAME_TYPE_ADVENTURE || bm->m_pPlayer->isCurrentToolAdventureModeExempt(pos))
	{
		if (currentGameType == GAME_TYPE_CREATIVE)
		{
			// netClientHandler.addToSendQueue(new Packet14BlockDig(0, x, y, z, face));
			// todo.
			// currently client mode. do the blockDig things.
			bm->m_interactionMgr->onBlockClicked(pos, face);

			clickBlockCreative(bm, this, pos, face);
			blockHitDelay = 5;
		}
		else if (!isHittingBlock || !sameToolAndBlock(pos))
		{
			if (currentGameType == GAME_TYPE_SURVIVAL && bm->m_pPlayer->getHeldItem())
			{
				ItemSword* pSword = dynamic_cast<ItemSword*>(bm->m_pPlayer->getHeldItem()->getItem());
				if (pSword)
					return;
			}

			int blockID = bm->m_pWorld->getBlockId(pos);
			if (blockID > 0 && dynamic_cast<BlockFruits*>(BlockManager::sBlocks[blockID]))
			{
				return;
			}

			if (isHittingBlock)
			{
				// netClientHandler.addToSendQueue(new Packet14BlockDig(1, currentBlockX, currentBlockY, currentblockZ, face));
				// todo.
				// currently client mode. do the blockDig things.
				bm->m_interactionMgr->cancelDestroyingBlock();
				ClientNetwork::Instance()->getSender()->sendAbortBreakBlock();
			}

			ClientNetwork::Instance()->getSender()->sendStartBreakBlock(pos);
			// netClientHandler.addToSendQueue(new Packet14BlockDig(0, x, y, z, face));
			// todo.
			// currently client mode. do the blockDig things.
			//bm->m_interactionMgr->onBlockClicked(pos, face);

			//int blockID = bm->m_pWorld->getBlockId(pos);

			if (blockID > 0 && curBlockDamageMP == 0.0F)
			{
				BlockManager::sBlocks[blockID]->onBlockClicked(bm->m_pWorld, pos, bm->m_pPlayer);
			}

			if (blockID > 0 && BlockManager::sBlocks[blockID]->getPlayerRelativeBlockHardness(bm->m_pPlayer, bm->m_pPlayer->world, pos) >= 1.0F)
			{
				onPlayerDestroyBlock(pos, face);
			}
			else
			{
				isHittingBlock = true;
				currentBlockPos = pos;
				currentItemHittingBlock = bm->m_pPlayer->getHeldItem();
				setCurBlockDamageMP(0.0f, RAYTRACE_TYPE_BLOCK);
				stepSoundTickCounter = 0.0F;
				bm->m_pWorld->destroyBlockInWorldPartially(bm->m_pPlayer->entityId, currentBlockPos, (int)(curBlockDamageMP * 10.0F) - 1);
			}
		}
	}
}

void PlayerController::resetBlockRemoving()
{
	if (isHittingBlock)
	{
		// netClientHandler.addToSendQueue(new Packet14BlockDig(1, currentBlockX, currentBlockY, currentblockZ, -1));
		ClientNetwork::Instance()->getSender()->sendAbortBreakBlock();
	}

	isHittingBlock = false;
	setCurBlockDamageMP(0.0f, RAYTRACE_TYPE_BLOCK);
	bm->m_pWorld->destroyBlockInWorldPartially(bm->m_pPlayer->entityId, currentBlockPos, -1);
}

void PlayerController::onPlayerDamageBlock(const BlockPos& pos, int face)
{
	syncCurrentPlayItem();
	if (blockHitDelay > 0)
	{
		--blockHitDelay;
	}
	else if (currentGameType == GAME_TYPE_CREATIVE)
	{
		blockHitDelay = 5;
		// netClientHandler.addToSendQueue(new Packet14BlockDig(0, x, y, z, face));
		// todo.
		// currently client mode. do the blockDig things.
		bm->m_interactionMgr->onBlockClicked(pos, face);

		clickBlockCreative(bm, this, pos, face);
	}
	else
	{
		if (sameToolAndBlock(pos))
		{
			int blockID = bm->m_pWorld->getBlockId(pos);

			if (blockID == 0)
			{
				isHittingBlock = false;
				return;
			}

			Block* pBlock = BlockManager::sBlocks[blockID];
			curBlockDamageMP += pBlock->getPlayerRelativeBlockHardness(bm->m_pPlayer, bm->m_pPlayer->world, pos);
			setCurBlockDamageMP(curBlockDamageMP, RAYTRACE_TYPE_BLOCK);
			if (Math::Mod(stepSoundTickCounter, 4.0f) == 0.0F && pBlock)
			{
				cBlock* pcBlock = cBlockManager::scBlocks[blockID];
				SoundSystem::Instance()->playEffectByType(pcBlock->getStepSound().stepSound); 
				//bm->getWorld()->playSound(Vector3(pos) + 0.5f, pcBlock->getStepSound().getStepSound(), (pcBlock->getStepSound().volume + 1.0F) / 8.0F, pcBlock->getStepSound().pitch * 0.5F, false);
				bm->getWorld()->playSoundByType(Vector3(pos) + 0.5f, pcBlock->getStepSound().stepSound);
			}

			++stepSoundTickCounter;

			if (curBlockDamageMP >= 1.0F)
			{
				isHittingBlock = false;
				// netClientHandler.addToSendQueue(new Packet14BlockDig(2, x, y, z, face));
				// todo.
				// currently client mode. do the blockDig things.
				//bm->m_interactionMgr->uncheckedTryHarvestBlock(pos);

				onPlayerDestroyBlock(pos, face);
				setCurBlockDamageMP(0.0F, RAYTRACE_TYPE_BLOCK);
				stepSoundTickCounter = 0.0F;
				blockHitDelay = 5;
			}

			bm->m_pWorld->destroyBlockInWorldPartially(bm->m_pPlayer->entityId, currentBlockPos, (int)(curBlockDamageMP * 10.0F) - 1);
		}
		else
		{
			clickBlock(pos, face);
		}
	}
}

void PlayerController::updateController()
{
	syncCurrentPlayItem();
	// bm.sndManager.playRandomMusicIfReady();
}

bool PlayerController::sameToolAndBlock(const BlockPos& pos)
{
	ItemStackPtr pStack = bm->m_pPlayer->getHeldItem();
	bool flag = (!currentItemHittingBlock) && (!pStack);

	if (currentItemHittingBlock && pStack )
	{
		flag = pStack->itemID == currentItemHittingBlock->itemID &&
			ItemStack::areItemStackTagsEqual(pStack, currentItemHittingBlock) &&
			(pStack->isItemStackDamageable() || pStack->getItemDamage() == currentItemHittingBlock->getItemDamage());
	}

	return pos == currentBlockPos && flag;
}

void PlayerController::syncCurrentPlayItem()
{
	//int slot = bm->m_pPlayer->inventory->currentItemIndex;

	//if (slot != currentPlayerItem)
	//{
	//	currentPlayerItem = slot;
	//}
}

bool PlayerController::onPlayerClickToBlock(EntityPlayer* pPlayer, World* pWorld, ItemStackPtr pStack, const BlockPos& pos, int face, const Vector3& ray)
{
	syncCurrentPlayItem();
	Vector3 hit = ray - Vector3(pos);
	bool activated = false;
	int blockID;

	if (!pPlayer->isSneaking() || !pPlayer->getHeldItem())
	{
		blockID = pWorld->getBlockId(pos);

		if (blockID > 0 && BlockManager::sBlocks[blockID]->onBlockActivated(pWorld, pos, pPlayer, face, hit))
		{
			activated = true;
		}
	}


	if (!activated && pStack )
	{
		ItemBlock* pItemBlock = dynamic_cast<ItemBlock*>(pStack->getItem());
		if(pItemBlock && !pItemBlock->canPlaceItemBlockOnSide(pWorld, pos, face, pPlayer, pStack))
		{
			return false;
		}
	}
	// netClientHandler.addToSendQueue(new Packet15Place(x, y, z, face, pPlayer.inventory.getCurrentItem(), hitx, hity, hitz));

	if (activated)
	{
		ClientNetwork::Instance()->getSender()->sendActivateBlock(pos, face, hit);
		return true;
	}
	else if (!pStack)
	{
		return false;
	}
	else if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::Build_War)
	{
		int size = pStack->stackSize;
		bool placed = pStack->tryPlaceItemIntoWorld(pPlayer, pWorld, pos, face, hit);
		pStack->stackSize = size;
		if (placed)
		{
			ClientNetwork::Instance()->getSender()->sendPlaceBlock(pos, face, hit);
		}
		return placed;
	}
	else if (currentGameType == GAME_TYPE_CREATIVE)
	{
		int damage = pStack->getItemDamage();
		int size = pStack->stackSize;
		bool placed = pStack->tryPlaceItemIntoWorld(pPlayer, pWorld, pos, face, hit);
		pStack->setItemDamage(damage);
		pStack->stackSize = size;
		if (placed)
		{
			ClientNetwork::Instance()->getSender()->sendPlaceBlock(pos, face, hit);
		}
		return placed;
	}
	else
	{
		bool placed = pStack->tryPlaceItemIntoWorld(pPlayer, pWorld, pos, face, hit);
		if (placed)
		{
			ClientNetwork::Instance()->getSender()->sendPlaceBlock(pos, face, hit);
		}
		return placed;
	}
}

bool PlayerController::useItem(EntityPlayer* pPlayer, World* pWorld, ItemStackPtr pStack)
{
	syncCurrentPlayItem();
	// netClientHandler.addToSendQueue(new Packet15Place(-1, -1, -1, 255, pPlayer.inventory.getCurrentItem(), 0.0F, 0.0F, 0.0F));
	int size = pStack->stackSize;
	ItemStackPtr useStack = pStack->useItemRightClick(pWorld, pPlayer);

	if (!useStack) {
		return false;
	}

	if (useStack == pStack && (!useStack || useStack->stackSize == size))
	{
		return false;
	}
	else
	{
		if (useStack->stackSize == 0) {
			pPlayer->inventory->setCurrentItem(nullptr);
		}
		else if (pStack != useStack) {
			pPlayer->inventory->setCurrentItem(useStack);
		}

		return true;
	}
}

EntityPlayerSPMP* PlayerController::createClientPlayer(World* pWorld, PlayerInteractionManager* pInteractionMgr)
{
	return LordNew EntityPlayerSPMP(bm, pWorld, pInteractionMgr);
}

void PlayerController::attackEntity(EntityPlayer* pPlayer, Entity* pEntity)
{
	syncCurrentPlayItem();
	// netClientHandler.addToSendQueue(new Packet7UseEntity(pPlayer.entityId, pEntity.entityId, 1));
	pPlayer->attackTargetEntityWithCurrentItem(pEntity);
}

bool PlayerController::interactWithEntity(EntityPlayer* pPlayer, Entity* pEntity)
{
	syncCurrentPlayItem();
	// netClientHandler.addToSendQueue(new Packet7UseEntity(pPlayer.entityId, pEntity.entityId, 0));
	return pPlayer->interactWith(pEntity);
}

ItemStackPtr PlayerController::windowClick(int windowID, int slotId, int mouseButton, int clickType, EntityPlayer* pPlayer)
{
	short action = pPlayer->openContainer->getNextTransactionID(pPlayer->inventory);
	ItemStackPtr pStack = pPlayer->openContainer->slotClick(slotId, mouseButton, clickType, pPlayer);
	// netClientHandler.addToSendQueue(new Packet102WindowClick(windowID, slotId, mouseButton, clickType, pStack, action));
	return pStack;
}

void PlayerController::sendEnchantPacket(int windowID, int button)
{
	// netClientHandler.addToSendQueue(new Packet108EnchantItem(windowID, button));
}

void PlayerController::sendSlotPacket(ItemStackPtr pStack, int slot)
{
	if (currentGameType != GAME_TYPE_CREATIVE)
		return;
	//netClientHandler.addToSendQueue(new Packet107CreativeSetSlot(slot, pStack));
}

void PlayerController::sendPacketDropItem(ItemStackPtr pStack)
{
	if (currentGameType != GAME_TYPE_CREATIVE)
		return;

	if (!pStack)
		return;
	
	// netClientHandler.addToSendQueue(new Packet107CreativeSetSlot(-1, pDropStack));
}

void PlayerController::onStoppedUsingItem(EntityPlayer* pPlayer)
{
	syncCurrentPlayItem();
	// netClientHandler.addToSendQueue(new Packet14BlockDig(5, 0, 0, 0, 255));
	pPlayer->stopUsingItem();
}

bool PlayerController::isRidingHorse()
{
	return false;
	// todo.
	// return bm->m_pPlayer->isRiding() && bm.thePlayer.ridingEntity instanceof EntityHorse;
}

void PlayerController::onPlayerLongHitEntity(Entity * pEntity)
{
	if (!pEntity->canLongHit() || pEntity->getLongHitTimes() <= 0)
	{
		onPlayerCancelLongHitEntity();
		return;
	}
	if (curLongHitEntity == nullptr || curLongHitEntity != pEntity)
	{
		curLongHitEntityTimes = 0;
	}
	curLongHitEntity = pEntity;
	curLongHitEntityTimes++;
	float progress = (float)curLongHitEntityTimes / pEntity->getLongHitTimes();
	setCurBlockDamageMP(progress, RAYTRACE_TYPE_ENTITY);
	if (progress >= 1.0f)
	{
		onPlayerFinishLongHitEntity();
	}
	if (curLongHitEntityTimes % 5 == 0)
	{
		SoundSystem::Instance()->playEffectByType(ST_BreakIronBlock);
	}
}

void PlayerController::onPlayerCancelLongHitEntity()
{
	if (curLongHitEntity != nullptr)
	{
		curLongHitEntity = nullptr;
		curLongHitEntityTimes = 0;
		setCurBlockDamageMP(0.0f, RAYTRACE_TYPE_ENTITY);
	}
}

void PlayerController::onPlayerFinishLongHitEntity()
{
	curLongHitEntityTimes = 0;
	setCurBlockDamageMP(0.0f, RAYTRACE_TYPE_ENTITY);
	if (curLongHitEntity)
	{
		ClientNetwork::Instance()->getSender()->sendLongHitEntity(curLongHitEntity->entityId);
	}
}

}
