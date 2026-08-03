/********************************************************************
filename: 	ItemWorldManager.h
file path:	dev\client\Src\Blockman\Item

version:	1
author:		ajohn
company:	supernano
date:		2017-05-08
*********************************************************************/
#ifndef __ITEM_WORLD_MANAGER_HEADER__
#define __ITEM_WORLD_MANAGER_HEADER__

#include "BM_TypeDef.h"
#include "Core.h"
#include "Item/ItemStack.h"

namespace BLOCKMAN
{

class World;
class EntityPlayer;
class EntityPlayerSP;
class ItemStack;

class PlayerInteractionManager : public ObjectAlloc
{
public:
	/** The world object that this object is connected to. */
	World* theWorld = nullptr;
	/** The EntityPlayerMP object that this object is connected to. */
	EntityPlayerSP* playerSP = nullptr;

protected:
	GAME_TYPE	gameType = GAME_TYPE_INVALID;
	/** True if the player is destroying a block */
	bool		isDestroyingBlock = false;
	int			initialDamage = 0;
	Vector3i	destroyPos;
	int			curblockDamage = 0;
	/** Set to true when the "finished destroying block" packet is received but the block wasn't fully damaged yet. The
	* block will not be destroyed while this is false. */
	bool		receivedFinishDiggingPacket = false;
	Vector3i	delayedDestroyPos;
	int			initialBlockDamage = 0;
	int			durabilityRemainingOnBlock = 0;

protected:
	/** Removes a block and triggers the appropriate events */
	bool removeBlock(const BlockPos& pos);

public:
	PlayerInteractionManager(World* pWorld);
	
	void setGameType(GAME_TYPE type);
	
	GAME_TYPE getGameType() const { return gameType; }

	/** Get if we are in creative game mode.*/
	bool isCreative() const { return gameType == GAME_TYPE_CREATIVE; }

	/** if the gameType is currently NOT_SET then change it to par1 	*/
	void initializeGameType(GAME_TYPE type);
	void updateBlockRemoving();
	/** if not creative, it calls destroyBlockInWorldPartially until the block is broken first. */
	void onBlockClicked(const BlockPos& pos, int side);
	void uncheckedTryHarvestBlock(const BlockPos& pos);
	/** note: this ignores the pars passed in and continues to destroy the onClickedBlock */
	void cancelDestroyingBlock();
	/** Attempts to harvest a block at the given coordinate */
	bool tryHarvestBlock(const BlockPos& pos);
	/** Attempts to right-click use an item by the given EntityPlayer in the given World */
	bool tryUseItem(EntityPlayer* player, World* pWorld, ItemStackPtr pStack);
	/** Activate the clicked on block, otherwise use the held item. */
	bool activateBlockOrUseItem(EntityPlayer* pPlayer, World* pWorld, ItemStackPtr pStack, const BlockPos& pos, int face, const Vector3& hit);
	/** Sets the world instance. */
	void setWorld(World* pWorld) { theWorld = pWorld; }
};

}
#endif