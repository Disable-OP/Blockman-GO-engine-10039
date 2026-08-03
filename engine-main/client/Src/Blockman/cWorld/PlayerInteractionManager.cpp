#include "PlayerInteractionManager.h"

#include "cEntity/EntityPlayerSP.h"

#include "World/World.h"
#include "Block/Block.h"
#include "Block/BlockManager.h"
#include "Item/ItemStack.h"
#include "Inventory/InventoryPlayer.h"

namespace BLOCKMAN
{

PlayerInteractionManager::PlayerInteractionManager(World* pWorld)
	: theWorld(pWorld)
	, playerSP(NULL)
	, gameType(GAME_TYPE_INVALID)
	, isDestroyingBlock(false)
	, durabilityRemainingOnBlock(-1)
	, initialDamage(0)
	, destroyPos(Vector3i::ZERO)
	, curblockDamage(0)
	, receivedFinishDiggingPacket(false)
	, delayedDestroyPos(Vector3i::ZERO)
	, initialBlockDamage(0)
{
}

void PlayerInteractionManager::setGameType(GAME_TYPE type)
{
	gameType = type;
	if (type == GAME_TYPE_CREATIVE)
	{
		playerSP->capabilities.allowFlying = true;
		playerSP->capabilities.isCreativeMode = true;
		playerSP->capabilities.disableDamage = true;
		playerSP->capabilities.isWatchMode = false;
	}
	else
	{
		playerSP->capabilities.allowFlying = false;
		playerSP->capabilities.isCreativeMode = false;
		playerSP->capabilities.disableDamage = false;
		playerSP->capabilities.isFlying = false;
		playerSP->capabilities.isWatchMode = false;
	}

	playerSP->capabilities.allowEdit = gameType != GAME_TYPE_ADVENTURE;
}

void PlayerInteractionManager::initializeGameType(GAME_TYPE type)
{
	if (gameType == GAME_TYPE_INVALID)
	{
		gameType = type;
	}

	setGameType(gameType);
}

void PlayerInteractionManager::updateBlockRemoving()
{
	++curblockDamage;
	
	if (receivedFinishDiggingPacket)
	{
		int damage = curblockDamage - initialBlockDamage;
		int blockID = theWorld->getBlockId(delayedDestroyPos);

		if (blockID == 0)
		{
			receivedFinishDiggingPacket = false;
		}
		else
		{
			Block* pBlock = BlockManager::sBlocks[blockID];
			float damgeRatio = pBlock->getPlayerRelativeBlockHardness(playerSP, playerSP->world, delayedDestroyPos) * (float)(damage + 1);
			int damages = (int)(damgeRatio * 10.0F);

			if (damages != durabilityRemainingOnBlock)
			{
				// todo.
				// theWorld->destroyBlockInWorldPartially(playerSP->entityId, delayedDestroyPos.x, delayedDestroyPos.y, delayedDestroyPos.z, damages);
				durabilityRemainingOnBlock = damages;
			}

			if (damgeRatio >= 1.0F)
			{
				receivedFinishDiggingPacket = false;
				tryHarvestBlock(delayedDestroyPos);
			}
		}
	}
	else if (isDestroyingBlock)
	{
		int blockID = theWorld->getBlockId(destroyPos);
		Block* pBlock = BlockManager::sBlocks[blockID];

		if (!pBlock)
		{
			// todo.
			// theWorld->destroyBlockInWorldPartially(playerSP->entityId, destroyPos.x, destroyPos.y, destroyPos.z, -1);
			durabilityRemainingOnBlock = -1;
			isDestroyingBlock = false;
		}
		else
		{
			int damage = curblockDamage - initialDamage;
			float damageRatio = pBlock->getPlayerRelativeBlockHardness(playerSP, playerSP->world, destroyPos) * (float)(damage + 1);
			int damages = (int)(damageRatio * 10.0F);

			if (damages != durabilityRemainingOnBlock)
			{
				// todo.
				// theWorld->destroyBlockInWorldPartially(playerSP->entityId, destroyPos.x, destroyPos.y, destroyPos.z, damages);
				durabilityRemainingOnBlock = damages;
			}
		}
	}
}

void PlayerInteractionManager::onBlockClicked(const BlockPos& pos, int side)
{
	if (gameType != GAME_TYPE_ADVENTURE || playerSP->isCurrentToolAdventureModeExempt(pos))
	{
		if (isCreative())
		{
			if (!theWorld->extinguishFire((EntityPlayer*)NULL, pos, side))
			{
				tryHarvestBlock(pos);
			}
		}
		else
		{
			theWorld->extinguishFire((EntityPlayer*)NULL, pos, side);
			initialDamage = curblockDamage;
			float damageRatio = 1.0F;
			int blockID = theWorld->getBlockId(pos);

			if (blockID > 0)
			{
				BlockManager::sBlocks[blockID]->onBlockClicked(theWorld, pos, playerSP);
				damageRatio = BlockManager::sBlocks[blockID]->getPlayerRelativeBlockHardness(playerSP, playerSP->world, pos);
			}

			if (blockID > 0 && damageRatio >= 1.0F)
			{
				tryHarvestBlock(pos);
			}
			else
			{
				isDestroyingBlock = true;
				destroyPos = pos;
				int damages = (int)(damageRatio * 10.0F);
				// todo.
				// theWorld->destroyBlockInWorldPartially(playerSP->entityId, x, y, z, damages);
				durabilityRemainingOnBlock = damages;
			}
		}
	}
}

void PlayerInteractionManager::uncheckedTryHarvestBlock(const BlockPos& pos)
{
	if (destroyPos == pos)
	{
		int damage = curblockDamage - initialDamage;
		int blockID = theWorld->getBlockId(pos);

		if (blockID != 0)
		{
			Block* pBlock = BlockManager::sBlocks[blockID];
			float damageRatio = pBlock->getPlayerRelativeBlockHardness(playerSP, playerSP->world, pos) * (float)(damage + 1);

			if (damageRatio >= 0.7F)
			{
				isDestroyingBlock = false;
				// todo.
				// theWorld->destroyBlockInWorldPartially(playerSP->entityId, x, y, z, -1);
				tryHarvestBlock(pos);
			}
			else if (!receivedFinishDiggingPacket)
			{
				isDestroyingBlock = false;
				receivedFinishDiggingPacket = true;
				delayedDestroyPos = pos;
				initialBlockDamage = initialDamage;
			}
		}
	}
}

void PlayerInteractionManager::cancelDestroyingBlock()
{
	isDestroyingBlock = false;
	// todo.
	// theWorld->destroyBlockInWorldPartially(playerSP->entityId, destroyPos.x, destroyPos.y, destroyPos.z, -1);
}

bool PlayerInteractionManager::removeBlock(const BlockPos& pos)
{
	Block* pBlock = BlockManager::sBlocks[theWorld->getBlockId(pos)];
	int meta = theWorld->getBlockMeta(pos);

	if (pBlock)
	{
		pBlock->onBlockHarvested(theWorld, pos, meta, playerSP);
	}

	bool result = theWorld->setBlockToAir(pos);

	if (pBlock && result)
	{
		pBlock->onBlockDestroyedByPlayer(theWorld, pos, meta);
	}

	return result;
}

bool PlayerInteractionManager::tryHarvestBlock(const BlockPos& pos)
{
	if (gameType == GAME_TYPE_ADVENTURE && !playerSP->isCurrentToolAdventureModeExempt(pos))
	{
		return false;
	}
	else if (gameType == GAME_TYPE_CREATIVE && playerSP->getHeldItem() /*&& thisPlayerMP->getHeldItem()->getItem() instanceof ItemSword*/) // todo.
	{
		return false;
	}
	else
	{
		int blockID = theWorld->getBlockId(pos);
		int meta = theWorld->getBlockMeta(pos);
		// theWorld->playAuxSFXAtEntity(thisPlayerMP, 2001, x, y, z, blockID + (theWorld.getBlockMetadata(x, y, z) << 12));
		bool removeResult = removeBlock(pos);

		if (isCreative())
		{
			// thisPlayerMP.playerNetServerHandler.sendPacketToPlayer(new Packet53BlockChange(x, y, z, theWorld));
		}
		else
		{
			ItemStackPtr pStack = playerSP->getCurrentEquippedItem();
			bool canHarvest = playerSP->canHarvestBlock(BlockManager::sBlocks[blockID]);

			if (pStack)
			{
				pStack->onBlockDestroyed(theWorld, blockID, pos, playerSP);

				if (pStack->stackSize == 0)
				{
					playerSP->destroyCurrentEquippedItem();
				}
			}

			if (removeResult && canHarvest)
			{
				BlockManager::sBlocks[blockID]->harvestBlock(theWorld, playerSP, pos, meta);
			}
		}

		return removeResult;
	}
}

bool PlayerInteractionManager::tryUseItem(EntityPlayer* player, World* pWorld, ItemStackPtr pStack)
{
	int size = pStack->stackSize;
	int damage = pStack->getItemDamage();
	ItemStackPtr usedStack = pStack->useItemRightClick(pWorld, player);

	if (usedStack == pStack && 
		( (!usedStack) ||
			usedStack->stackSize == size &&
			usedStack->getMaxItemUseDuration() <= 0 &&
			usedStack->getItemDamage() == damage))
	{
		return false;
	}
	else
	{
		player->inventory->setCurrentItem(usedStack);

		if (isCreative())
		{
			usedStack->stackSize = size;

			if (usedStack->isItemStackDamageable())
			{
				usedStack->setItemDamage(damage);
			}
		}

		if (usedStack->stackSize == 0)
		{
			player->inventory->setCurrentItem(nullptr);
		}

		if (!player->isUsingItem())
		{
			// ((EntityPlayerMP)player).sendContainerToPlayer(player.inventoryContainer);
		}

		return true;
	}
}

bool PlayerInteractionManager::activateBlockOrUseItem(EntityPlayer* pPlayer, World* pWorld, ItemStackPtr pStack, const BlockPos& pos, int face, const Vector3& hit)
{
	if (!pPlayer->isSneaking() || (!pPlayer->getHeldItem()))
	{
		int blockID = pWorld->getBlockId(pos);

		if (blockID > 0 && BlockManager::sBlocks[blockID]->onBlockActivated(pWorld, pos, pPlayer, face, hit))
		{
			return true;
		}
	}

	if (!pStack)
	{
		return false;
	}
	else if (isCreative())
	{
		int itemDamage = pStack->getItemDamage();
		int size = pStack->stackSize;
		bool placeRet = pStack->tryPlaceItemIntoWorld(pPlayer, pWorld, pos, face, hit);
		pStack->setItemDamage(itemDamage);
		pStack->stackSize = size;
		return placeRet;
	}
	else
	{
		return pStack->tryPlaceItemIntoWorld(pPlayer, pWorld, pos, face, hit);
	}
}


}
