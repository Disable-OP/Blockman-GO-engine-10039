/********************************************************************
filename: 	Blocks.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-11-22
*********************************************************************/
#ifndef __BLOCKS_HEADER__
#define __BLOCKS_HEADER__

#include "Block.h"
#include "BM_TypeDef.h"
#include "Setting/DoorSetting.h"
#include "Setting/CropsSetting.h"

using namespace LORD;

namespace BLOCKMAN
{

class BlockFlower : public Block
{
public:
	BlockFlower(int id, const BM_Material& mat, const String& name);
	BlockFlower(int id);

	/** implement override function from Block. */
	virtual bool canPlaceBlockAt(World* pWorld, const BlockPos& pos);
	virtual void onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborblockID);
	virtual void updateTick(World* pWorld, const BlockPos& pos, Random& rand);
	virtual bool canBlockStay(World* pWorld, const BlockPos& pos);
	virtual Box getCollisionBoundingBox(World* pWorld, const BlockPos& pos) { return Box(); }
	virtual bool renderAsNormalBlock() const { return false; }
	virtual int idDropped(int meta, Random& rand, int fortune) { return m_blockID; }

protected:
	virtual bool canThisPlantGrowOnThisBlockID(int id);
	virtual bool checkFlowerChange(World* pWorld, const BlockPos& pos);
};

class BlockTallGrass : public BlockFlower
{
public:
	BlockTallGrass(int id, const BM_Material& mat, const String& name);
	
	/** implement override function from Block. */
	virtual int idDropped(int meta, Random& rand, int fortune);
	virtual int quantityDroppedWithBonus(int fortune, Random& rand);
	virtual void harvestBlock(World* pWorld, EntityPlayer* pPlayer, const BlockPos& pos, int damage);
	virtual int getDamageValue(World* pWorld, const BlockPos& pos);
};

class BlockDeadBush : public BlockFlower
{
public:
	BlockDeadBush(int id, const BM_Material& mat, const String& name);

	/** implement override function from Block. */
	virtual int idDropped(int meta, Random& rand, int fortune) { return -1; }
	virtual void harvestBlock(World* pWorld, EntityPlayer* pPlayer, const BlockPos& pos, int damage);

protected:
	/** implement override function from BlockFlower. */
	virtual bool canThisPlantGrowOnThisBlockID(int id);
};

class BlockFluid : public Block
{
public:
	BlockFluid(int id, const BM_Material& mat, const String& name);

	/** implement override function from Block. */
	virtual bool getBlocksMovement(IBlockAccess* pBlockAccess, const BlockPos& pos) const;
	virtual bool renderAsNormalBlock() const { return false; }
	virtual bool canCollideCheck(int meta, bool onboat) const { return onboat && meta == 0; }
	virtual Box getCollisionBoundingBox(World* pWorld, const BlockPos& pos) { return Box(); }
	virtual int idDropped(int meta, Random& rand, int fortune) { return 0; }
	virtual int quantityDropped(Random& rand) { return 0; }
	virtual void updateTick(World* pWorld, const BlockPos& pos, Random& rand) {}
	virtual int tickRate(World* pWorld) const;
	virtual void onBlockAdded(World* pWorld, const BlockPos& pos) { checkForHarden(pWorld, pos); }
	virtual void onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborblockID) { checkForHarden(pWorld, pos); }
	virtual bool isUpdateScheduled() { return false; }
	virtual Vector3 velocityToAddToEntity(World* pWorld, const BlockPos& pos, Entity* pEntity, const Vector3& vec);
	virtual bool isBlockSolid(IBlockAccess* pIBlockAccess, const BlockPos& pos, int side);

public:
	static float getFluidHeightPercent(int meta);
	static double getFlowDirection(IBlockAccess* pIBlockAccess, const BlockPos& pos, const BM_Material& mat);
	

protected:
	int getFlowDecay(World* pWorld, const BlockPos& pos);
	int getEffectiveFlowDecay(IBlockAccess* pBlockAccess, const BlockPos& pos);
	Vector3 getFlowVector(IBlockAccess* pBlockAccess, const BlockPos& pos);
	void checkForHarden(World* pWorld, const BlockPos& pos);
	void triggerLavaMixEffects(World* pWorld, const BlockPos& pos);
};

class BlockStationary : public BlockFluid
{
public:
	BlockStationary(int id, const BM_Material& mat, const String& name);

	/** implement override function from Block. */
	virtual bool getBlocksMovement(IBlockAccess* pBlockAccess, const BlockPos& pos) const;
	virtual void onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborblockID);
	virtual void updateTick(World* pWorld, const BlockPos& pos, Random& rand);

protected:
	/** Changes the block ID to that of an updating fluid. */
	void setNotStationary(World* pWorld, const BlockPos& pos);
	bool isFlammable(World* pWorld, const BlockPos& pos);
};

class BlockFlowing : public BlockFluid
{
public:
	BlockFlowing(int id, const BM_Material& mat, const String& name);

	/** implement override function from Block. */
	virtual bool getBlocksMovement(IBlockAccess* pBlockAccess, const BlockPos& pos) const;
	virtual void updateTick(World* pWorld, const BlockPos& pos, Random& rand);
	virtual void onBlockAdded(World* pWorld, const BlockPos& pos);
	virtual bool isUpdateScheduled() const { return true; }

protected:
	void updateFlow(World* pWorld, const BlockPos& pos);
	void flowIntoBlock(World* pWorld, const BlockPos& pos, int meta);
	int calculateFlowCost(World* pWorld, const BlockPos& pos, int accumulate, int preDir);
	bool* getOptimalFlowDirections(World* pWorld, const BlockPos& pos);
	bool blockBlocksFlow(World* pWorld, const BlockPos& pos);
	int getSmallestFlowDecay(World* pWorld, const BlockPos& pos, int currentDecay);
	bool liquidCanDisplaceBlock(World* pWorld, const BlockPos& pos);

protected:
	int numAdjacentSources = 0;
	bool isOptimalFlowDirection[4];
	int flowCost[4];
};

class BlockMushroom : public BlockFlower
{
public:
	BlockMushroom(int id, const BM_Material& mat, const String& name);

	/** implement override function from Block. */
	virtual void updateTick(World* pWorld, const BlockPos& pos, Random& rand);
	virtual bool canPlaceBlockAt(World* pWorld, const BlockPos& pos);
	virtual bool canBlockStay(World* pWorld, const BlockPos& pos);

	/** Fertilize the mushroom. */
	bool fertilizeMushroom(World* pWorld, const BlockPos& pos, Random& rand);

protected:
	/** implement override function from BlockFlower. */
	virtual bool canThisPlantGrowOnThisBlockID(int id);

};

class BlockCactus : public Block
{
public:
	BlockCactus(int id, const BM_Material& mat, const String& name);

	/** implement override function from Block. */
	virtual void updateTick(World* pWorld, const BlockPos& pos, Random& rand);
	virtual Box getCollisionBoundingBox(World* pWorld, const BlockPos& pos);
	virtual bool renderAsNormalBlock() const { return false; }
	virtual bool canPlaceBlockAt(World* pWorld, const BlockPos& pos);
	virtual void onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborID);
	virtual bool canBlockStay(World* pWorld, const BlockPos& pos);
	virtual void onEntityCollidedWithBlock(World* pWorld, const BlockPos& pos, Entity* pEntity);
};

class BlockRotatedPillar : public Block
{
public:
	BlockRotatedPillar(int id, const BM_Material& mat, const String& name);

	/** implement override function from Block. */
	virtual int onBlockPlaced(World* pWorld, const BlockPos& pos, int side, const Vector3& hit, int meta);
	virtual int damageDropped(int meta) { return meta & 3; }
	virtual ItemStackPtr createStackedBlock(int damage);
};

class BlockLogBase : public BlockRotatedPillar
{
public:
	BlockLogBase(int id, const BM_Material& mat, const String& name);

	/** implement override function from Block. */
	virtual int quantityDropped(Random& rand) {	return 1; }
	virtual int idDropped(int meta, Random& rand, int fortune) = 0;
	virtual void breakBlock(World* pWorld, const BlockPos& pos, int blockID, int meta);

	/** returns a number between 0 and 3 */
	static int limitToValidMetadata(int meta) { return meta & 3; }
	static bool isWood(int blockId) { return blockId == BLOCK_ID_WOOD || blockId == BLOCK_ID_WOOD2; }
};

class BlockLog1 : public BlockLogBase
{
public:
	static StringArray woodType;

public:
	BlockLog1(int id, const BM_Material& mat, const String& name);

	/** implement override function from Block. */
	virtual int idDropped(int meta, Random& rand, int fortune) { return BLOCK_ID_WOOD; }
};

class BlockLog2 : public BlockLogBase
{
public:
	static StringArray woodType;

public:
	BlockLog2(int id, const BM_Material& mat, const String& name);

	/** implement override function from Block. */
	virtual int idDropped(int meta, Random& rand, int fortune) { return BLOCK_ID_WOOD2; }
};

class BlockHay : public BlockRotatedPillar
{
public:
	BlockHay(int id, const BM_Material& mat, const String& name);
};

class BlockLeavesBase : public Block
{
public:
	BlockLeavesBase(int id, const BM_Material& mat, const String& name, bool isOpaqueCube);
};

class BlockLeaves : public BlockLeavesBase
{
public:
	BlockLeaves(int id, const BM_Material& mat, const String& name);

	/** implement override function from Block. */
	virtual void breakBlock(World* pWorld, const BlockPos& pos, int blockID, int meta);
	virtual void updateTick(World* pWorld, const BlockPos& pos, Random& rand);
	virtual int quantityDropped(Random& rand) { return rand.nextInt(20) == 0 ? 1 : 0; }
	virtual int idDropped(int meta, Random& rand, int fortune);
	virtual void dropBlockAsItemWithChance(World* pWorld, const BlockPos& pos, int meta, float chance, int fortune);
	virtual void harvestBlock(World* pWorld, EntityPlayer* pPlayer, const BlockPos& pos, int damage);
	virtual int damageDropped(int meta) { return meta & 3; }

protected:
	void removeLeaves(World* pWorld, const BlockPos& pos);

protected:
	int* adjacentTreeBlocks = nullptr;
};

class BlockSnow : public Block
{
public:
	BlockSnow(int id, const BM_Material& mat, const String& name);

	/** implement override function from Block. */
	virtual Box getCollisionBoundingBox(World* pWorld, const BlockPos& pos);
	virtual bool renderAsNormalBlock() const { return false; }
	virtual void setBlockBoundsForItemRender() { setBlockBoundsForSnowDepth(0); }
	virtual void setBlockBoundsBasedOnState(IBlockAccess* pBlockAccess, const BlockPos& pos);
	virtual bool canPlaceBlockAt(World* pWorld, const BlockPos& pos);
	virtual void onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborID);
	virtual void harvestBlock(World* pWorld, EntityPlayer* pPlayer, const BlockPos& pos, int damage);
	virtual int idDropped(int meta, Random& rand, int fortune) { return ITEM_ID_SNOW_BALL; }
	virtual int quantityDropped(Random& rand) { return 0; }
	virtual void updateTick(World* pWorld, const BlockPos& pos, Random& rand);
	virtual void addCollisionBoxesToList(World* pWorld, const BlockPos& pos, const Box& aabb, AABBList& lst, Entity* pEntity);

protected:
	void setBlockBoundsForSnowDepth(int meta);
	bool canSnowStay(World* pWorld, const BlockPos& pos);	
};

class BlockSand : public Block
{
public:
	/** Do blocks fall instantly to where they stop or do they fall over time */
	static bool fallInstantly;

public:
	BlockSand(int id, const BM_Material& mat, const String& name);

	/** implement override function from Block. */
	virtual void onBlockAdded(World* pWorld, const BlockPos& pos);
	virtual void onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborID);
	virtual void updateTick(World* pWorld, const BlockPos& pos, Random& rand);
	virtual int tickRate(World* pWorld) const { return 2; }
	
	/** Called when the falling block entity for this block hits the ground and turns back into a block. */
	virtual void onFinishFalling(World* pWorld, const BlockPos& pos, int par5) {}
	// virtual void onStartFalling(EntityFallingSand par1EntityFallingSand) {}

	static bool canFallBelow(World* pWorld, const BlockPos& pos);

protected:
	void tryToFall(World* pWorld, const BlockPos& pos);
	
};

class BlockVine : public Block
{
public:
	BlockVine(int id, const BM_Material& mat, const String& name);

	/** implement override function from Block. */
	virtual Box getCollisionBoundingBox(World* pWorld, const BlockPos& pos) override;
	virtual void setBlockBoundsForItemRender() { setBlockBounds(0.0F, 0.0F, 0.0F, 1.0F, 1.0F, 1.0F); }
	virtual bool renderAsNormalBlock() const { return false; }
	virtual void setBlockBoundsBasedOnState(IBlockAccess* pBlockAccess, const BlockPos& pos);
	virtual bool canPlaceBlockOnSide(World* pWorld, const BlockPos& pos, int side);
	virtual void onNeighborBlockChange(World* pWorld, const BlockPos& pos, int blockID);
	virtual void updateTick(World* pWorld, const BlockPos& pos, Random& rand);
	virtual int onBlockPlaced(World* pWorld, const BlockPos& pos, int side, const Vector3& hit, int meta);
	

protected:
	bool canBePlacedOn(int blockID);
	bool canVineStay(World* pWorld, const BlockPos& pos);
};

class BlockCocoa : public Block
{
public:
	BlockCocoa(int id, const BM_Material& mat, const String& name);

	static int getDirection(int dir) { return dir & 3; }
	static int getAgeFromMeta(int meta) { return (meta & 12) >> 2; }

	/** implement override function from Block. */
	virtual void updateTick(World* pWorld, const BlockPos& pos, Random& rand);
	virtual bool canBlockStay(World* pWorld, const BlockPos& pos);
	virtual int onBlockPlaced(World* pWorld, const BlockPos& pos, int side, const Vector3& hit, int meta);
	virtual void onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neightborID);
};

class BlockGrass : public Block
{
public:
	BlockGrass(int id, const BM_Material& mat, const String& name);

	/** implement override function from Block. */
	virtual void updateTick(World* pWorld, const BlockPos& pos, Random& rand);
	virtual int idDropped(int meta, Random& rand, int fortune);
};

class BlockWood : public Block
{
public:
	static StringArray woodType;

public:
	BlockWood(int id, const BM_Material& mat, const String& name);

	/** implement override function from Block. */
	virtual int damageDropped(int meta) { return meta; }
};

class BlockLilyPad : public BlockFlower
{
public:
	BlockLilyPad(int id, const BM_Material& mat, const String& name);

	/** implement override function from Block. */
	virtual void addCollisionBoxesToList(World* pWorld, const BlockPos& pos, const Box& aabb, AABBList& lst, Entity* pEntity);
	virtual Box getCollisionBoundingBox(World* pWorld, const BlockPos& pos);
	virtual bool canBlockStay(World* pWorld, const BlockPos& pos);

protected:
	/** implement override function from BlockFlower.  */
	virtual bool canThisPlantGrowOnThisBlockID(int blockID);
};

class BlockReed : public Block
{
public:
	BlockReed(int id, const BM_Material& mat, const String& name);

	/** implement override function from Block. */
	virtual void updateTick(World* pWorld, const BlockPos& pos, Random& random);
	virtual bool canPlaceBlockAt(World* pWorld, const BlockPos& pos);
	virtual void onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborId);
	virtual bool canBlockStay(World* pWorld, const BlockPos& pos);
	virtual int idDropped(int meta, Random& rand, int fortune) { return ITEM_ID_SEED; }
	virtual bool renderAsNormalBlock() const { return false; }
	virtual int isPowered(World* pWorld, const BlockPos& pos) { return ITEM_ID_REED; }

protected:
	void checkBlockCoordValid(World* pWorld, const BlockPos& pos);
};

class BlockSandStone : public Block
{
public:
	static StringArray SAND_STONE_TYPES;

public:
	BlockSandStone(int id, const BM_Material& mat, const String& name);

	/** implement override function from Block. */
	virtual int damageDropped(int meta) { return meta; }
};

class BlockMycelium : public Block
{
public:
	BlockMycelium(int id, const BM_Material& mat, const String& name);

	/** implement override function from Block. */
	virtual void updateTick(World* pWorld, const BlockPos& pos, Random& rand);
	virtual int idDropped(int meta, Random& rand, int fortune);
};

class BlockContainer : public Block , public ITileEntityProvider
{
protected:
	BlockContainer(int id, const BM_Material& mat, const String& name, bool isOpaque);
	
	/** implement override funtions from Block */
	virtual void onBlockAdded(World* pWorld, const BlockPos& pos) { Block::onBlockAdded(pWorld, pos); }
	virtual void breakBlock(World* pWorld, const BlockPos& pos, int blockID, int meta);
	virtual bool onBlockEventReceived(World* pWorld, const BlockPos& pos, int id, int param);
};

class IInventory;

class BlockChest : public BlockContainer
{
public:
	/** Determines whether of not the chest is trapped. */
	// int isTrapped = false;

	/* enum block trapped type */
	enum BLockTrappedType
	{
		BTT_BLOCK_NOT_TRAPPED = 0,
		BTT_BLOCK_TRAPPED = 1,
		BTT_BLOCK_NOT_TRAPPED_NEW = 2,
		BTT_BLOCK_TRAPPED_NEW = 3,
	};

protected:
	int m_block_trapped_type = BTT_BLOCK_NOT_TRAPPED;

	bool isThereANeighborChest(World* pWorld, const BlockPos& pos);
	/** Looks for a sitting ocelot within certain bounds. Such an ocelot is considered to be blocking access to the chest. */
	static bool isOcelotBlockingChest(World* pWorld, const BlockPos& pos);

public:
	BlockChest(int id, const BM_Material& mat, const String& name, int block_trapped_type);
	/** Turns the adjacent chests to a double chest. */
	void unifyAdjacentChests(World* pWorld, const BlockPos& pos);
	IInventory* getInventory(World* pWorld, const BlockPos& pos);
	int getChestType() { return m_block_trapped_type; }

	/** implement override functions from ITileEntityProvider */
	virtual TileEntity* createNewTileEntity(World* pWorld);

	/** implement override functions from Block */
	virtual bool renderAsNormalBlock() const { return false; }
	virtual void setBlockBoundsBasedOnState(IBlockAccess* pBlockAccess, const BlockPos& pos);
	virtual void onBlockAdded(World* pWorld, const BlockPos& pos);
	virtual void onBlockPlacedBy(World* pWorld, const BlockPos& pos, EntityLivingBase* pLiving, ItemStackPtr pStack);
	virtual bool canPlaceBlockAt(World* pWorld, const BlockPos& pos);
	virtual void onNeighborBlockChange(World* pWorld, const BlockPos& pos, int blockID);
	virtual void breakBlock(World* pWorld, const BlockPos& pos, int blockID, int meta);
	virtual bool onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit);
	virtual bool canProvidePower() { /*return isTrapped == 1; */ return m_block_trapped_type == BTT_BLOCK_TRAPPED || m_block_trapped_type == BTT_BLOCK_TRAPPED_NEW; }
	virtual int isProvidingWeakPower(IBlockAccess* pBlockAccess, const BlockPos& pos, int side);
	virtual int isProvidingStrongPower(IBlockAccess* pBlockAccess, const BlockPos& pos, int side);
	virtual bool hasComparatorInputOverride() { return true; }
	virtual int getComparatorInputOverride(World* pWorld, const BlockPos& pos, int side);
};

class BlockDirectional : public Block
{
public:
	BlockDirectional(int id, const BM_Material& mat, const String& name, bool bOpaque);
	
	/** Returns the orentation value from the specified metadata */
	static int getDirection(int meta) { return meta & 3; }
};

class BlockBed : public BlockDirectional
{
public:
	/** Maps the foot-of-bed block to the head-of-bed block. */
	static BlockPos footBlockToHeadBlockMap[4];

protected:
	/** Set the bounds of the bed block. */
	void setBounds() { setBlockBounds(0.0F, 0.0F, 0.0F, 1.0F, 0.5625F, 1.0F); }

public:
	BlockBed(int id, const String& name);
	
	/** implement override functions from Block */
	virtual bool onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit);
	virtual bool renderAsNormalBlock() const { return false; }
	virtual void setBlockBoundsBasedOnState(IBlockAccess* pBlockAccess, const BlockPos& pos) { setBounds(); }
	virtual void onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborID);
	virtual int idDropped(int meta, Random& rand, int fortune);
	virtual int idPicked(World* pWorld, const BlockPos& pos);
	virtual void dropBlockAsItemWithChance(World* pWorld, const BlockPos& pos, int meta, float chance, int fortune);
	virtual int getMobilityFlag() { return 1; }
	virtual void onBlockHarvested(World* pWorld, const BlockPos& pos, int meta, EntityPlayer* pPlayer);

	/** Returns whether or not this bed block is the head of the bed. */
	static bool isBlockHeadOfBed(int par0) { return (par0 & 8) != 0; }
	/** Return whether or not the bed is occupied. */
	static bool isBedOccupied(int par0) { return (par0 & 4) != 0; }
	/** Sets whether or not the bed is occupied. */
	static void setBedOccupied(World* pWorld, const BlockPos& pos, bool par4);
	/** Gets the nearest empty chunk coordinates for the player to wake up from a bed into. */
	static Vector3i getNearestEmptyChunkCoordinates(World* pWorld, const BlockPos& pos, int tries);

	Vector3i getOtherSidePos(const BlockPos & pos, int meta);
};


class BlockSilverfish : public Block
{
public:
	static StringArray silverfishStoneTypes;

public:
	BlockSilverfish(int id, const BM_Material& mat, const String& name);

	/** implement override functions from Block */
	virtual int quantityDropped(Random& rand) { return 0; }
	virtual int getDamageValue(World* pWorld, const BlockPos& pos);
	virtual void onBlockDestroyedByPlayer(World* pWorld, const BlockPos& pos, int meta);
	virtual ItemStackPtr createStackedBlock(int damage);
};

class BlockFarmland : public Block
{
public:
	BlockFarmland(int id, const BM_Material& mat, const String& name);
	
	/** implement override functions from Block */
	virtual void updateTick(World* pWorld, const BlockPos& pos, Random& rand);
	virtual void onFallenUpon(World* pWorld, const BlockPos& pos, Entity* pEntity, float distance);
	virtual void onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborBlockID);
	virtual int idDropped(int meta, Random& rand, int fortune) { return BLOCK_ID_DIRT; }
	virtual int isPowered(World* pWorld, const BlockPos& pos) { return BLOCK_ID_DIRT; };
	
protected:
	/* returns true if there is at least one cropblock nearby (x-1 to x+1, y+1, z-1 to z+1) */
	bool isCropsNearby(World* pWorld, const BlockPos& pos);
	/* returns true if there's water nearby (x-4 to x+4, y to y+1, k-4 to k+4) */
	bool isWaterNearby(World* pWorld, const BlockPos& pos);	
};

class BlockDoor : public Block
{
public:
	BlockDoor(int id, const BM_Material& mat, const String& name);

	/** implement override functions from Block */
	virtual bool getBlocksMovement(IBlockAccess* pBlockAccess, const BlockPos& pos) const;
	virtual bool renderAsNormalBlock() const { return false; }
	virtual Box getCollisionBoundingBox(World* pWorld, const BlockPos& pos);
	virtual void setBlockBoundsBasedOnState(IBlockAccess* pBlockAccess, const BlockPos& pos);
	virtual bool onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit);
	virtual void onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborID);
	virtual int idDropped(int meta, Random& rand, int fortune);
	virtual int idPicked(World* pWorld, const BlockPos& pos);
	virtual RayTraceResult collisionRayTrace(World* pWorld, const BlockPos& pos, const Vector3& start, const Vector3& end);
	virtual bool canPlaceBlockAt(World* pWorld, const BlockPos& pos);
	virtual int getMobilityFlag() { return 1; }
	virtual void onBlockHarvested(World* pWorld, const BlockPos& pos, int meta, EntityPlayer* pPlayer);
	virtual void addCollisionBoxesToList(World* pWorld, const BlockPos& pos, const Box& aabb, AABBList& lst, Entity* pEntity);

	/** is the door opening. */
	bool isDoorOpen(IBlockAccess* pBlockAccess, const BlockPos& pos);
	/** Returns 0, 1, 2 or 3 depending on where the hinge is. */
	int getDoorOrientation(IBlockAccess* pBlockAccess, const BlockPos& pos);
	const DoorSetting* getDoorSetting() { return DoorSetting::getDoorSetting(m_blockID); };
	void switchDoorState(World* pWorld, const BlockPos& pos, bool isAutoClose);

protected:
	int getFullMetadata(IBlockAccess* pBlockAccess, const BlockPos& pos) const;
	void setDoorRotation(int meta);
	void onPoweredBlockChange(World* pWorld, const BlockPos& pos, bool powered);
	virtual bool checkDoor(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer);
};

class BlockSnowBlock : public Block
{
public:
	BlockSnowBlock(int id, const BM_Material& mat, const String& name);

	/** implement override functions from Block */
	virtual int idDropped(int meta, Random& rand, int fortune) { return ITEM_ID_SNOW_BALL; }
	virtual int quantityDropped(Random& rand) { return 4; }
	virtual void updateTick(World* pWorld, const BlockPos& pos, Random& rand);
};

class BlockCake : public Block
{
public:
	BlockCake(int id, const BM_Material& mat, const String& name);

	/** implement override functions from Block */
	virtual void setBlockBoundsBasedOnState(IBlockAccess* pBlockAccess, const BlockPos& pos);
	virtual void setBlockBoundsForItemRender();
	virtual Box getCollisionBoundingBox(World* pWorld, const BlockPos& pos);
	virtual bool renderAsNormalBlock() const { return false; }
	virtual bool onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit);
	virtual void onBlockClicked(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer);
	virtual bool canPlaceBlockAt(World* pWorld, const BlockPos& pos);
	virtual void onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborBlockID);
	virtual bool canBlockStay(World* pWorld, const BlockPos& pos);
	virtual int quantityDropped(Random& rand) { return 0; }
	virtual int idDropped(int meta, Random& rand, int fortune) { return 0; }
	virtual int isPowered(World* pWorld, const BlockPos& pos) { return ITEM_ID_CAKE; }

	void eatCakeSlice(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer);
};

class BlockTrapDoor : public Block
{
public:
	BlockTrapDoor(int id, const BM_Material& mat, const String& name);

	/** implement override functions from Block */
	virtual bool renderAsNormalBlock() const { return false; }
	virtual bool getBlocksMovement(IBlockAccess* pAccess, const BlockPos& pos) const;
	virtual Box getCollisionBoundingBox(World* pWorld, const BlockPos& pos);
	virtual void setBlockBoundsBasedOnState(IBlockAccess* pBlockAccess, const BlockPos& pos);
	virtual void setBlockBoundsForItemRender();
	virtual bool onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit);
	virtual void onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborID);
	virtual RayTraceResult collisionRayTrace(World* pWorld, const BlockPos& pos, const Vector3& start, const Vector3& end);
	virtual int onBlockPlaced(World* pWorld, const BlockPos& pos, int side, const Vector3& hit, int meta);
	virtual bool canPlaceBlockOnSide(World* pWorld, const BlockPos& pos, int side);
	virtual void addCollisionBoxesToList(World* pWorld, const BlockPos& pos, const Box& aabb, AABBList& lst, Entity* pEntity);

	/** Return true if the trap door is open. */
	static bool isTrapdoorOpen(int meta) { return (meta & 4) != 0; }

protected:
	static bool isValidSupportBlock(int blockID);
	void onPoweredBlockChange(World* pWorld, const BlockPos& pos, bool powered);
};


class BlockStoneBrick : public Block
{
public:
	static StringArray STONE_BRICK_TYPES;

public:
	BlockStoneBrick(int id, const BM_Material& mat, const String& name);

	/** implement override functions from Block */
	virtual int damageDropped(int meta) { return meta; }
};


class BlockMushroomCap : public Block
{
public:
	BlockMushroomCap(int id, const BM_Material& mat, const String& name, int mushroomType);

	/** implement override functions from Block */
	virtual int quantityDropped(Random& rand);
	virtual int idDropped(int meta, Random& rand, int fortune) { return BLOCK_ID_MUSHROOM_BROWN + m_mushroomType; }
	virtual int isPowered(World* pWorld, const BlockPos& pos) { return BLOCK_ID_MUSHROOM_BROWN + m_mushroomType; }

protected:
	int m_mushroomType = 0;
};

class BlockMelon : public Block
{
public:
	BlockMelon(int id, const BM_Material& mat, const String& name);

	/** implement override functions from Block */
	virtual int idDropped(int meta, Random& rand, int fortune) { return ITEM_ID_MELON; }
	virtual int quantityDropped(Random& rand) { return 3 + rand.nextInt(5); }
	virtual int quantityDroppedWithBonus(int bonus, Random& rand);
};

class BlockOre : public Block
{
public:
	BlockOre(int id, const BM_Material& mat, const String& name);

	/** implement override functions from Block */
	virtual int idDropped(int meta, Random& rand, int fortune);
	virtual int quantityDropped(Random& rand);
	virtual int quantityDroppedWithBonus(int bonus, Random& rand);
	virtual void dropBlockAsItemWithChance(World* pWorld, const BlockPos& pos, int meta, float chance, int fortune);
	virtual int damageDropped(int meta) { return m_blockID == BLOCK_ID_ORE_LAPIS ? 4 : 0; }
};

class BlockOreStorage : public Block
{
public:
	BlockOreStorage(int id, const BM_Material& mat, const String& name);
};

class BlockPumpkin : public BlockDirectional
{
public:
	BlockPumpkin(int id, const BM_Material& mat, const String& name, bool type);

	/** implement override functions from Block */
	virtual void onBlockAdded(World* pWorld, const BlockPos& pos);
	virtual bool canPlaceBlockAt(World* pWorld, const BlockPos& pos);
	virtual void onBlockPlacedBy(World* pWorld, const BlockPos& pos, EntityLivingBase* pLiving, ItemStackPtr pStack);

protected:
	bool m_blockType = 0;
};

class BlockCommandBlock : public BlockContainer
{
public:
	BlockCommandBlock(int id, const BM_Material& mat, const String& name);

	/** implement override functions from ITileEntityProvider */
	virtual TileEntity* createNewTileEntity(World* pWorld);

	/** implement override functions from Block */
	virtual void onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborBlockID);
	virtual void updateTick(World* pWorld, const BlockPos& pos, Random& rand);
	virtual int tickRate(World* pWorld) const { return 1; }
	virtual bool onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit);
	virtual bool hasComparatorInputOverride() { return true; }
	virtual int getComparatorInputOverride(World* pWorld, const BlockPos& pos, int side);
	virtual void onBlockPlacedBy(World* pWorld, const BlockPos& pos, EntityLivingBase* pLiving, ItemStackPtr pStack);
	virtual int quantityDropped(Random& rand) { return 0; }
};

class BlockDaylightDetector : public BlockContainer
{
public:
	BlockDaylightDetector(int id, const BM_Material& mat, const String& name);
	
	/** implement override functions from ITileEntityProvider */
	virtual TileEntity* createNewTileEntity(World* pWorld);
	
	/** implement override functions from Block */
	virtual void setBlockBoundsBasedOnState(IBlockAccess* pBlockAccess, const BlockPos& pos) { setBlockBounds(0.0F, 0.0F, 0.0F, 1.0F, 0.375F, 1.0F); }
	virtual int isProvidingWeakPower(IBlockAccess* piBlockAccess, const BlockPos& pos, int side);
	virtual bool renderAsNormalBlock() const  { return false; }	
	virtual bool canProvidePower() { return true; }
};

class Facing;

class BlockDispenser : public BlockContainer
{
public:
	BlockDispenser(int id, const BM_Material& mat, const String& name);
	virtual ~BlockDispenser();

	/** implement override functions from ITileEntityProvider */
	TileEntity* createNewTileEntity(World* pWorld);

	/** implement override functions from Block */
	virtual int tickRate(World* pWorld) const { return 4; }
	virtual void onBlockAdded(World* pWorld, const BlockPos& pos);
	virtual bool onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit);
	virtual void onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborBlockID);
	virtual void updateTick(World* pWorld, const BlockPos& pos, Random& rand);
	virtual void onBlockPlacedBy(World* pWorld, const BlockPos& pos, EntityLivingBase* pLiving, ItemStackPtr pStack);
	virtual void breakBlock(World* pWorld, const BlockPos& pos, int blockID, int meta);
	virtual bool hasComparatorInputOverride() { return true; }
	virtual int getComparatorInputOverride(World* pWorld, const BlockPos& pos, int side);

	static Vector3 getIPositionFromBlockSource(const BlockPos& pos);
	static Facing* getFacing(int meta);

protected:
	Random* m_pRand = nullptr;
};

class BlockEnchantmentTable : public BlockContainer
{
public:
	BlockEnchantmentTable(int id, const BM_Material& mat, const String& name);

	/** implement override functions from ITileEntityProvider */
	virtual TileEntity* createNewTileEntity(World* pWorld);

	/** implement override functions from Block */
	virtual bool renderAsNormalBlock() const { return false; }
	virtual bool onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit);
	virtual void onBlockPlacedBy(World* pWorld, const BlockPos& pos, EntityLivingBase* pLiving, ItemStackPtr pStack);
};

class BlockFurnace : public BlockContainer
{
public:
	BlockFurnace(int id, const BM_Material& mat, const String& name, bool isActive);

	/** implement override functions from ITileEntityProvider */
	virtual TileEntity* createNewTileEntity(World* pWorld);

	/** implement override functions from Block */
	virtual int idDropped(int meta, Random& rand, int fortune) { return BLOCK_ID_FURNACE_IDLE; }
	virtual void onBlockAdded(World* pWorld, const BlockPos& pos);
	virtual bool onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit);
	virtual void onBlockPlacedBy(World* pWorld, const BlockPos& pos, EntityLivingBase* pLiving, ItemStackPtr pStack);
	virtual void breakBlock(World* pWorld, const BlockPos& pos, int blockID, int meta);
	virtual bool hasComparatorInputOverride() { return true; }
	virtual int getComparatorInputOverride(World* pWorld, const BlockPos& pos, int side);
	virtual int isPowered(World* pWorld, const BlockPos& pos) { return BLOCK_ID_FURNACE_IDLE; }

	/** Update which block ID the furnace is using depending on whether or not it is burning */
	static void updateFurnaceBlockState(bool burning, World* pWorld, const BlockPos& pos);

protected:
	void setDefaultDirection(World* pWorld, const BlockPos& pos);

protected:
	Random m_furnaceRand;
	bool m_isActive = false;	
	
	static bool keepFurnaceInventory;
};

class BlockJukeBox : public BlockContainer
{
public:
	BlockJukeBox(int id, const BM_Material& mat, const String& name);

	/** implement override functions from ITileEntityProvider */
	virtual TileEntity* createNewTileEntity(World* pWorld);

	/** implement override functions from Block */
	virtual bool onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit);
	virtual void breakBlock(World* pWorld, const BlockPos& pos, int blockID, int meta);
	virtual void dropBlockAsItemWithChance(World* pWorld, const BlockPos& pos, int meta, float chance, int fortune);
	virtual bool hasComparatorInputOverride() { return true; }
	virtual int getComparatorInputOverride(World* pWorld, const BlockPos& pos, int side);

	void insertRecord(World* pWorld, const BlockPos& pos, ItemStackPtr pStack);
	void ejectRecord(World* pWorld, const BlockPos& pos);
};

class BlockMobSpawner : public BlockContainer
{
public:
	BlockMobSpawner(int id, const BM_Material& mat, const String& name);

	/** implement override functions from ITileEntityProvider */
	virtual TileEntity* createNewTileEntity(World* pWorld);

	/** implement override functions from Block */
	virtual int idDropped(int meta, Random& rand, int fortune) { return 0; }
	virtual int quantityDropped(Random& rand) { return 0; }
	virtual void dropBlockAsItemWithChance(World* pWorld, const BlockPos& pos, int meta, float chance, int fortune);	
	virtual int isPowered(World* pWorld, const BlockPos& pos) { return 0; }
};


class BlockNote : public BlockContainer
{
public:
	BlockNote(int id, const BM_Material& mat, const String& name);

	/** implement override functions from ITileEntityProvider */
	virtual TileEntity* createNewTileEntity(World* pWorld);

	/** implement override functions from Block */
	virtual void onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborID);
	virtual bool onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit);
	virtual void onBlockClicked(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer);
	virtual bool onBlockEventReceived(World* pWorld, const BlockPos& pos, int id, int param);
};

class BlockStairs : public Block
{
public:
	BlockStairs(int id, const BM_Material& mat, const String& name, Block* pBlock, int meta);

	/** implement override functions from Block */
	virtual void setBlockBoundsBasedOnState(IBlockAccess* pBlockAccess, const BlockPos& pos);
	virtual bool renderAsNormalBlock() const { return false; }
	virtual void addCollisionBoxesToList(World* pWorld, const BlockPos& pos, const Box& aabb, AABBList& lst, Entity* pEntity);
	virtual void onBlockClicked(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer);
	virtual void onBlockDestroyedByPlayer(World* pWorld, const BlockPos& pos, int meta);
	virtual float getExplosionResistance(Entity* pEntity);
	virtual int tickRate(World* pWorld) const { return m_modelBlock->tickRate(pWorld); }
	virtual Vector3 velocityToAddToEntity(World* pWorld, const BlockPos& pos, Entity* pEntity, const Vector3& vec);
	virtual bool isCollidable() const { return m_modelBlock->isCollidable(); }
	virtual bool canCollideCheck(int meta, bool onboat) const { return m_modelBlock->canCollideCheck(meta, onboat); }
	virtual bool canPlaceBlockAt(World* pWorld, const BlockPos& pos) { return m_modelBlock->canPlaceBlockAt(pWorld, pos); }
	virtual void onBlockAdded(World* pWorld, const BlockPos& pos);
	virtual void breakBlock(World* pWorld, const BlockPos& pos, int blockID, int meta) { m_modelBlock->breakBlock(pWorld, pos, blockID, meta); }
	virtual void onEntityWalking(World* pWorld, const BlockPos& pos, Entity* pEntity) { m_modelBlock->onEntityWalking(pWorld, pos, pEntity); }
	virtual void updateTick(World* pWorld, const BlockPos& pos, Random& rand) { m_modelBlock->updateTick(pWorld, pos, rand); }
	virtual bool onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pEntityPlayer, int face, const Vector3& hit);
	virtual void onBlockDestroyedByExplosion(World* pWorld, const BlockPos& pos, Explosion* pExplosion) { m_modelBlock->onBlockDestroyedByExplosion(pWorld, pos, pExplosion); }
	virtual void onBlockPlacedBy(World* pWorld, const BlockPos& pos, EntityLivingBase* pLiving, ItemStackPtr pStack);
	virtual int onBlockPlaced(World* pWorld, const BlockPos& pos, int side, const Vector3& hit, int meta) { return side != 0 && (side == 1 || hit.y <= 0.5F) ? meta : meta | 4; }
	virtual RayTraceResult collisionRayTrace(World* pWorld, const BlockPos& pos, const Vector3& start, const Vector3& end);

	void getCollSlabBouding(IBlockAccess* pBlockAccess, const BlockPos& pos);
	bool getCollQuarterBouding(IBlockAccess* pBlockAccess, const BlockPos& pos);
	bool getCollEighthBouding(IBlockAccess* pBlockAccess, const BlockPos& pos);
	
	static bool isBlockStairsID(int blockID);
	
protected:
	bool isTheSameStairs(IBlockAccess* pBlockAccess, const BlockPos& pos, int meta);

protected:
	Block* m_modelBlock = nullptr;
	int m_modelBlockMeta = 0;
	bool m_rayTracyed = false;
	int m_rayTracyCount = 0;	
};

class BlockHalfSlab : public Block
{
public:
	BlockHalfSlab(int id, const BM_Material& mat, const String& name, bool isDoubleSlab);

	/** implement override functions from Block */
	virtual void setBlockBoundsBasedOnState(IBlockAccess* pBlockAccess, const BlockPos& pos);
	virtual void setBlockBoundsForItemRender();
	virtual void addCollisionBoxesToList(World* pWorld, const BlockPos& pos, const Box& aabb, AABBList& lst, Entity* pEntity);
	virtual int onBlockPlaced(World* pWorld, const BlockPos& pos, int side, const Vector3& hit, int meta);
	virtual int idPicked(World* pWorld, const BlockPos& pos);
	virtual int quantityDropped(Random& rand) { return m_isDoubleSlab ? 2 : 1; }
	virtual int damageDropped(int meta) { return meta & 7; }
	virtual bool renderAsNormalBlock() const { return m_isDoubleSlab; }	
	virtual int getDamageValue(World* pWorld, const BlockPos& pos) { return Block::getDamageValue(pWorld, pos) & 7; }
	
protected:
	static bool isBlockSingleSlab(int blockID) { return blockID == BLOCK_ID_STONE_SINGLE_SLAB || blockID == BLOCK_ID_WOOD_SINGLE_SLAB || blockID == BLOCK_ID_REDSAND_STONE_SINGLESLAB; }

protected:
	bool m_isDoubleSlab = false;
};

class BlockStep : public BlockHalfSlab
{
public:
	BlockStep(int id, const BM_Material& mat, const String& name, bool isDoubleSlab);

	/** implement override functions from Block */
	virtual int idDropped(int meta, Random& rand, int fortune) { return BLOCK_ID_STONE_SINGLE_SLAB; }
	virtual ItemStackPtr createStackedBlock(int damage);
};

class BlockWoodSlab : public BlockHalfSlab
{
public:
	BlockWoodSlab(int id, const BM_Material& mat, const String& name, bool isDoubleSlab);

	/** implement override functions from Block */
	virtual int idDropped(int meta, Random& rand, int fortune) { return BLOCK_ID_WOOD_SINGLE_SLAB; }
	virtual ItemStackPtr createStackedBlock(int damage);
};

class BlockRedStoneSlab : public BlockHalfSlab
{
public:
	BlockRedStoneSlab(int id, const BM_Material& mat, const String& name, bool isDoubleSlab);

	/** implement override functions from Block */
	virtual int idDropped(int meta, Random& rand, int fortune) { return BLOCK_ID_REDSAND_STONE_SINGLESLAB; }
	virtual ItemStackPtr createStackedBlock(int damage);
};

class BlockCrops : public BlockFlower
{
public:
	BlockCrops(int id, const BM_Material& mat, const String& name);

	/** implement override functions from Block */
	virtual void updateTick(World* pWorld, const BlockPos& pos, Random& rand);
	virtual int idDropped(int meta, Random& rand, int fortune);
	virtual int quantityDropped(Random& rand) { return 1; }
	virtual void dropBlockAsItemWithChance(World* pWorld, const BlockPos& pos, int meta, float chance, int fortune);
	virtual int isPowered(World* pWorld, const BlockPos& pos) { return getSeedItem(); }

	/* Apply bone meal to the crops. */
	void fertilize(World* pWorld, const BlockPos& pos);

	virtual int getSeedItem() { return ITEM_ID_SEED; }
	virtual int getCropItem() { return ITEM_ID_WHEAT; }

protected:
	/** implement override functions from BlockFlower */
	virtual bool canThisPlantGrowOnThisBlockID(int blockID) { return blockID == BLOCK_ID_TILLED_FIELD; };

	float getGrowthRate(World* pWorld, const BlockPos& pos);
};

class BlockCarrot : public BlockCrops
{
public:
	BlockCarrot(int id, const BM_Material& mat, const String& name);

	/** implement override functions from BlockCrops */
	virtual int getSeedItem() { return ITEM_ID_CARROT; }
	virtual int getCropItem() { return ITEM_ID_CARROT; }
};

class BlockPotato : public BlockCrops
{
public:
	BlockPotato(int id, const BM_Material& mat, const String& name);

	/** implement override functions from Block */
	virtual void dropBlockAsItemWithChance(World* pWorld, const BlockPos& pos, int meta, float chance, int fortune);
	
	/** implement override functions from BlockCrops */
	virtual int getSeedItem() { return ITEM_ID_POTATO; }
	virtual int getCropItem() { return ITEM_ID_POTATO; }
};

class BlockStem : public BlockFlower
{
public:
	BlockStem(int id, const BM_Material& mat, const String& name, Block* fruit);
	
	/** implement override functions from Block */
	virtual void updateTick(World* pWorld, const BlockPos& pos, Random& rand);	
	virtual void setBlockBoundsForItemRender();
	virtual void setBlockBoundsBasedOnState(IBlockAccess* pBlockAccess, const BlockPos& pos);
	virtual void dropBlockAsItemWithChance(World* pWorld, const BlockPos& pos, int meta, float chance, int fortune);
	virtual int idDropped(int meta, Random& rand, int fortune) { return -1; }
	virtual int idPicked(World* pWorld, const BlockPos& pos);
	virtual int quantityDropped(Random& rand) { return 1; }

	/** Returns the current state of the stem. Returns -1 if the stem is not fully grown, or a value between 0 and 3
	* based on the direction the stem is facing. */
	int getState(IBlockAccess* pBlockAccess, const BlockPos& pos);
	void fertilizeStem(World* pWorld, const BlockPos& pos);
		
protected:
	/** implement override functions from BlockFlower */
	virtual bool canThisPlantGrowOnThisBlockID(int blockID) { return blockID == BLOCK_ID_TILLED_FIELD; }
	float getGrowthModifier(World* pWorld, const BlockPos& pos);

protected:
	Block* m_fruitBlock = nullptr;
};

class BlockButton : public Block
{
protected:
	BlockButton(int id, const BM_Material& mat, const String& name, bool sensible);

public:

	/** implement override functions from Block */
	virtual int tickRate(World *pWorld) const { return m_sensible ? 30 : 20; }
	virtual bool renderAsNormalBlock() const { return false; }
	virtual bool canPlaceBlockOnSide(World* pWorld, const BlockPos& pos, int side);
	virtual bool canPlaceBlockAt(World* pWorld, const BlockPos& pos);
	virtual int onBlockPlaced(World* pWorld, const BlockPos& pos, int side, const Vector3& hit, int meta);
	virtual void onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neightborBlockID);
	virtual void setBlockBoundsBasedOnState(IBlockAccess* pBlockAccess, const BlockPos& pos);
	virtual void setBlockBoundsForItemRender();
	virtual bool onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit);
	virtual void breakBlock(World* pWorld, const BlockPos& pos, int blockID, int meta);
	virtual int isProvidingWeakPower(IBlockAccess* piBlockAccess, const BlockPos& pos, int side);
	virtual int isProvidingStrongPower(IBlockAccess* iBlockAccess, const BlockPos& pos, int side);
	virtual bool canProvidePower() { return true; }
	virtual void updateTick(World* pWorld, const BlockPos& pos, Random& rand);
	virtual void onEntityCollidedWithBlock(World* pWorld, const BlockPos& pos, Entity* pEntity);
	virtual Box  getCollisionBoundingBox(World* pWorld, const BlockPos& pos) { return Box(); }


protected:
	int getOrientation(World* pWorld, const BlockPos& pos);
	bool redundantCanPlaceBlockAt(World* pWorld, const BlockPos& pos);
	void setButtonBounding(int meta);
	void notifyNeighbors(World* pWorld, const BlockPos& pos, bool upOrDown, int entityId);
	void notifyNeighborsFace(World* pWorld, const BlockPos& pos, int face);

protected:
	bool m_sensible = false;
};

class BlockButtonWood : public BlockButton
{
public:
	BlockButtonWood(int id, const BM_Material& mat, const String& name);
};

class BlockButtonStone : public BlockButton
{
public:
	BlockButtonStone(int id, const BM_Material& mat, const String& name);
};

class BlockLadder : public Block
{
public:
	BlockLadder(int id, const BM_Material& mat, const String& name);

	/** implement override functions from Block */
	virtual Box getCollisionBoundingBox(World* pWorld, const BlockPos& pos);
	virtual void setBlockBoundsBasedOnState(IBlockAccess* pBlockAccess, const BlockPos& pos);
	void updateLadderBounds(int meta);
	virtual bool renderAsNormalBlock() const { return false; }
	virtual bool canPlaceBlockAt(World* pWorld, const BlockPos& pos);
	virtual int onBlockPlaced(World* pWorld, const BlockPos& pos, int side, const Vector3& hit, int meta);
	virtual void onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborBlockID);
	virtual int quantityDropped(Random& rand) { return 1; }
};

class BlockFence : public Block
{
public:
	BlockFence(int id, const BM_Material& mat, const String& name);
	
	/** implement override functions from Block */
	virtual void addCollisionBoxesToList(World* pWorld, const BlockPos& pos, const Box& aabb, AABBList& lst, Entity* pEntity);
	virtual void setBlockBoundsBasedOnState(IBlockAccess* pBlockAccess, const BlockPos& pos);	
	virtual bool renderAsNormalBlock() const { return false; }
	virtual bool getBlocksMovement(IBlockAccess* pAccess, const BlockPos& pos) const { return false; }
	virtual bool onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit);
	
	/** Returns true if the specified block can be connected by a fence */
	bool canConnectFenceTo(IBlockAccess* pBlockAccess, const BlockPos& pos);
	static bool isIdAFence(int blockID);
};

class BlockPane : public Block
{
public:
	BlockPane(int id, const BM_Material& mat, const String& name, bool canDropItSelf);

	/** implement override functions from Block */
	virtual bool renderAsNormalBlock() const { return false; }
	virtual void setBlockBoundsBasedOnState(IBlockAccess* pBlockAccess, const BlockPos& pos);
	virtual int idDropped(int meta, Random& rand, int fortune);
	virtual bool canSilkHarvest() { return true; }
	virtual void addCollisionBoxesToList(World* pWorld, const BlockPos& pos, const Box& aabb, AABBList& lst, Entity* pEntity);
	virtual void setBlockBoundsForItemRender() { setBlockBounds(0.0F, 0.0F, 0.0F, 1.0F, 1.0F, 1.0F); }
	virtual ItemStackPtr createStackedBlock(int damage);

	/** Gets passed in the blockID of the block adjacent and supposed to return true if its allowed to connect to the
	* type of blockID passed in. Args: blockID */
	bool canThisPaneConnectToThisBlockID(int blockID);	
		
protected:
	bool m_canDropItSelf = false;
};

class BlockFenceGate : public BlockDirectional
{
public:
	BlockFenceGate(int id, const BM_Material& mat, const String& name);

	/** implement override functions from Block */
	virtual bool renderAsNormalBlock() const { return false; }
	virtual void setBlockBoundsBasedOnState(IBlockAccess* pBlockAccess, const BlockPos& pos);
	virtual bool canPlaceBlockAt(World* pWorld, const BlockPos& pos);
	virtual void onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neightborBlockID);
	virtual Box getCollisionBoundingBox(World* pWorld, const BlockPos& pos);
	virtual bool getBlocksMovement(IBlockAccess* pAccess, const BlockPos& pos) const;
	virtual void onBlockPlacedBy(World* pWorld, const BlockPos& pos, EntityLivingBase* pLiving, ItemStackPtr pStack);
	virtual bool onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit);
	virtual void addCollisionBoxesToList(World* pWorld, const BlockPos& pos, const Box& aabb, AABBList& lst, Entity* pEntity);

	/** Returns if the fence gate is open according to its metadata. */
	static bool isFenceGateOpen(int meta) { return (meta & 4) != 0; }
};

class BlockWall : public Block
{
public:
	static StringArray types;

public:
	BlockWall(int id, const BM_Material& mat, const String& name);
	
	/** implement override functions from Block */
	virtual bool renderAsNormalBlock() const { return false; }	
	virtual bool getBlocksMovement(IBlockAccess* pBlockAccess, const BlockPos& pos) const { return false; }
	virtual void setBlockBoundsBasedOnState(IBlockAccess* pBlockAccess, const BlockPos& pos);
	virtual int damageDropped(int meta) { return meta; }
	virtual Box getCollisionBoundingBox(World* pWorld, const BlockPos& pos);
	
	/** Return whether an adjacent block can connect to a wall. */
	bool canConnectWallTo(IBlockAccess* pBlockAccess, const BlockPos& pos);
};

class BlockBreakable : public Block
{
public:
	BlockBreakable(int id, const BM_Material& mat, const String& name, bool flag);

protected:
	bool m_flag = false;
};

class BlockGlass : public BlockBreakable
{
public:
	BlockGlass(int id, const BM_Material& mat, const String& name );

	/** implement override functions from Block */
	virtual bool renderAsNormalBlock() const { return false; }
	virtual int quantityDropped(Random& rand) { return 0; }
	virtual bool canSilkHarvest() { return true; }
};

class BlockAirWall : public BlockGlass
{
public:
	BlockAirWall(int id, const BM_Material& mat, const String& name);
};

class BlockIce : public BlockBreakable
{
public:
	BlockIce(int id, const BM_Material& mat, const String& name);

	/** implement override functions from Block */
	virtual void harvestBlock(World* pWorld, EntityPlayer* pPlayer, const BlockPos& pos, int damage);
	virtual int quantityDropped(Random& rand) { return 0; }
	virtual void updateTick(World* pWorld, const BlockPos& pos, Random& rand);
	virtual int getMobilityFlag() { return 0; }
};

class BlockPackedIce : public Block
{
public:
	BlockPackedIce(int id, const BM_Material& mat, const String& name);
	virtual int quantityDropped(Random& rand) { return 0; }
};

class BlockPortal : public BlockBreakable
{
public:
	BlockPortal(int id, const BM_Material& mat, const String& name);

	/** implement override functions from Block */
	virtual bool renderAsNormalBlock() const { return false; }
	virtual void setBlockBoundsBasedOnState(IBlockAccess* pBlockAccess, const BlockPos& pos);
	virtual void updateTick(World* pWorld, const BlockPos& pos, Random& rand);
	virtual int quantityDropped(Random& rand) { return 0; }
	virtual int isPowered(World* pWorld, const BlockPos& pos) { return 0; }
	virtual void onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborBlockID);
	virtual Box getCollisionBoundingBox(World* pWorld, const BlockPos& pos) { return Box(); }		
	virtual void onEntityCollidedWithBlock(World* pWorld, const BlockPos& pos, Entity* pEntity);

	/** Checks to see if this location is valid to create a portal and will return True if it does. */
	bool tryToCreatePortal(World* pWorld, const BlockPos& pos);
};

class BlockQuartz : public Block
{
public:
	static StringArray quartzBlockTypes;

public:
	BlockQuartz(int id, const BM_Material& mat, const String& name);

	/** implement override functions from Block */
	virtual int onBlockPlaced(World* pWorld, const BlockPos& pos, int side, const Vector3& hit, int meta);
	virtual int damageDropped(int meta) { return meta != 3 && meta != 4 ? meta : 2; }
	virtual ItemStackPtr createStackedBlock(int damage);
};

class BlockBeacon : public BlockContainer
{
public:
	BlockBeacon(int id, const BM_Material& mat, const String& name);
	
	/** implement override functions from ITileEntityProvider */
	virtual TileEntity* createNewTileEntity(World* pWorld);

	/** implement override functions from Block */
	virtual bool renderAsNormalBlock() const { return false; }
	virtual bool onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit);
	virtual void onBlockPlacedBy(World* pWorld, const BlockPos& pos, EntityLivingBase* pLiving, ItemStackPtr pStack);
};

class BlockAnvil : public BlockSand
{
public:
	static StringArray statuses;
	int field_82521_b = 0;
	
	BlockAnvil(int id, const BM_Material& mat, const String& name);

	/** implement override functions from Block */
	virtual bool renderAsNormalBlock() const { return false; }	
	virtual void setBlockBoundsBasedOnState(IBlockAccess* pBlockAccess, const BlockPos& pos);
	virtual int damageDropped(int meta) { return meta >> 2; }
	virtual void onBlockPlacedBy(World* pWorld, const BlockPos& pos, EntityLivingBase* pLiving, ItemStackPtr pStack);
	virtual bool onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit);

	/** implement override functions from BlockSand */
	virtual void onFinishFalling(World* pWorld, const BlockPos& pos, int par5);
	//virtual void onStartFalling(EntityFallingSand par1EntityFallingSand)
};

class BlockBanner : public Block
{
public:

	BlockBanner(int id, const BM_Material& mat, const String& name);

	/** implement override functions from Block */
	virtual Box getCollisionBoundingBox(World* pWorld, const BlockPos& pos) { return Box(); }
	virtual bool renderAsNormalBlock() const { return false; }
	virtual int onBlockPlaced(World* pWorld, const BlockPos& pos, int side, const Vector3& hit, int meta);
	virtual void onBlockHarvested(World* pWorld, const BlockPos& pos, int meta, EntityPlayer* pPlayer);
	virtual void onBlockDestroyedByPlayer(World* pWorld, const BlockPos& pos, int meta);
};

class BlockPoweredOre : public BlockOreStorage
{
public:
	BlockPoweredOre(int id, const BM_Material& mat, const String& name);

	/** implement override functions from Block */
	virtual bool canProvidePower() { return true; }
	virtual int isProvidingWeakPower(IBlockAccess* pBlockAccess, const BlockPos& pos, int side) { return 15; }
};

class BlockFlowerPot : public Block
{
public:
	BlockFlowerPot(int id, const BM_Material& mat, const String& name);

	/** implement override functions from Block */
	virtual bool renderAsNormalBlock() const { return false; }
	virtual void setBlockBoundsForItemRender();
	virtual bool onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit);
	virtual int getDamageValue(World* pWorld, const BlockPos& pos);
	virtual bool isFlowerPot() { return true; }
	virtual bool canPlaceBlockAt(World* pWorld, const BlockPos& pos);
	virtual void onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborBlockID);
	virtual void dropBlockAsItemWithChance(World* pWorld, const BlockPos& pos, int meta, float chance, int fortune);
	virtual int idDropped(int meta, Random& rand, int fortune) { return ITEM_ID_FLOWERPOT; }
	virtual int idPicked(World* pWorld, const BlockPos& pos);
	
	/** Return the item associated with the specified flower pot metadata value. */
	static ItemStackPtr getPlantForMeta(int meta);
	/** Return the flower pot metadata value associated with the specified item. */
	static int getMetaForPlant(ItemStackPtr pItemStack);
};

class BlockSapling : public BlockFlower
{
public:
	static StringArray WOOD_TYPES;

public:
	BlockSapling(int id, const BM_Material& mat, const String& name);

	/** implement override functions from Block */
	virtual void updateTick(World* pWorld, const BlockPos& pos, Random& rand);
	virtual int damageDropped(int meta) { return meta & 3; }
	
	void markOrGrowMarked(World* pWorld, const BlockPos& pos, Random& rand);
	/* Attempts to grow a sapling into a tree. */
	void growTree(World* pWorld, const BlockPos& pos, Random& rand);
	/* Determines if the same sapling is present at the given location. */
	bool isSameSapling(World* pWorld, const BlockPos& pos, int par5);
};

class BlockRedstoneLight : public Block
{
public:
	BlockRedstoneLight(int id, const BM_Material& mat, const String& name, bool powered);

	/** implement override functions from Block */
	virtual int idDropped(int meta, Random& rand, int fortune) { return BLOCK_ID_REDSTONE_LAMP_IDLE; }
	virtual void onBlockAdded(World* pWorld, const BlockPos& pos);
	virtual void onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborBlockID);
	virtual void updateTick(World* pWorld, const BlockPos& pos, Random& rand);
	virtual int isPowered(World* pWorld, const BlockPos& pos) { return BLOCK_ID_REDSTONE_LAMP_IDLE; }

protected:
	bool m_powered = false;
};

class BlockColored : public Block
{
public:
	static const int colorType = 16;

	BlockColored(int id, const BM_Material& mat, const String name);

	/** implement override functions from Block */
	virtual int damageDropped(int meta) { return meta; }
	
	/** Takes a dye damage value and returns the block damage value to match. */
	static int getBlockFromDye(int val) { return ~val & 15; }
	/** Takes a block damage value and returns the dye damage value to match. */
	static int getDyeFromBlock(int val) { return ~val & 15; }
};

class BlockCarpet : public Block
{
public:
	BlockCarpet(int id, const BM_Material& mat, const String& name);

	/** implement override functions from Block */
	virtual bool renderAsNormalBlock() const { return false; }
	virtual void setBlockBoundsBasedOnState(IBlockAccess* pBlockAccess, const BlockPos& pos);
	virtual void setBlockBoundsForItemRender() { setCarpetBounding(); }
	virtual Box getCollisionBoundingBox(World* pWorld, const BlockPos& pos);
	virtual bool canPlaceBlockAt(World* pWorld, const BlockPos& pos);
	virtual void onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborID);
	virtual bool canBlockStay(World* pWorld, const BlockPos& pos);
	virtual int damageDropped(int meta) { return meta; }

protected:
	void setCarpetBounding();
	bool checkForDrop(World* pWorld, const BlockPos& pos);
};

class BlockRoad : public BlockCarpet
{
public:
	BlockRoad(int id, const BM_Material& mat, const String& name);
	virtual void setBlockBoundsBasedOnState(IBlockAccess* pBlockAccess, const BlockPos& pos) { setBounding(); }
	virtual void setBlockBoundsForItemRender() { setBounding(); }
	virtual Box getCollisionBoundingBox(World* pWorld, const BlockPos& pos);

protected:
	void setBounding();
};



class BlockWeb : public Block
{
public:
	BlockWeb(int id, const BM_Material& mat, const String& name);

	/** implement override functions from Block */
	virtual bool renderAsNormalBlock() const { return false; }
	virtual bool canSilkHarvest() { return true; }
	virtual int idDropped(int meta, Random& rand, int fortune) { return ITEM_ID_SILK; }
	virtual void onEntityCollidedWithBlock(World* pWorld, const BlockPos& pos, Entity* pEntity);
	virtual Box getCollisionBoundingBox(World* pWorld, const BlockPos& pos);
};

class BlockTNT : public Block
{
public:
	BlockTNT(int id, const BM_Material& mat, const String& name);

	/** implement override functions from Block */
	virtual void onBlockAdded(World* pWorld, const BlockPos& pos);
	virtual void onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborID);
	virtual void onBlockDestroyedByExplosion(World* pWorld, const BlockPos& pos, Explosion* pExplosion);
	virtual void onBlockDestroyedByPlayer(World* pWorld, const BlockPos& pos, int meta);	
	virtual bool onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit);
	virtual void onEntityCollidedWithBlock(World* pWorld, const BlockPos& pos, Entity* pEntity);
	virtual bool canDropFromExplosion(Explosion* pExplosion) { return false; }

	void explode(World* pWorld, const BlockPos& pos, int meta, EntityLivingBase* caster);
};

class BlockBookshelf : public Block
{
public:
	BlockBookshelf(int id, const BM_Material& mat, const String& name);

	/** implement override functions from Block */
	virtual int quantityDropped(Random& rand) { return 3; }
	virtual int idDropped(int meta, Random& rand, int fortune) { return ITEM_ID_BOOK; }
};

class BlockWorkbench : public Block
{
public:
	BlockWorkbench(int id, const BM_Material& mat, const String& name);
};

class BlockClay : public Block
{
public:
	BlockClay(int id, const BM_Material& mat, const String& name);

	/** implement override functions from Block */
	virtual int idDropped(int meta, Random& rand, int fortune) { return ITEM_ID_CLAY; }
	virtual int quantityDropped(Random& rand) { return 4; }
};

class BlockRedstoneOre : public Block
{
public:
	BlockRedstoneOre(int id, const BM_Material& mat, const String& name, bool glowing);

	/** implement override functions from Block */
	virtual int tickRate(World* pWorld) const { return 30; }
	virtual void onBlockClicked(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer);
	virtual void onEntityWalking(World* pWorld, const BlockPos& pos, Entity* pEntity);
	virtual bool onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit);
	virtual void updateTick(World* pWorld, const BlockPos& pos, Random& rand);
	virtual int idDropped(int meta, Random& rand, int fortune) { return ITEM_ID_REDSTONE; }
	virtual int quantityDropped(Random& rand);
	virtual void dropBlockAsItemWithChance(World* pWorld, const BlockPos& pos, int meta, float chance, int fortune);
	virtual ItemStackPtr createStackedBlock(int damage);

protected:
	bool m_glowing = false;

	void glow(World* pWorld, const BlockPos& pos);
	virtual void sparkle(World* pWorld, const BlockPos& pos) {}
};

class BlockRailBase : public Block
{
	friend class BlockRailLogic;
protected:
	const bool m_isPowered = false;
	BlockRailBase(int id, const BM_Material& mat, const String& name, bool isPowered);
	void refreshTrackShape(World* pWorld, const BlockPos& pos, bool par5);

public:	
	/** implement override functions from Block */
	virtual bool renderAsNormalBlock() const { return false; }
	virtual Box getCollisionBoundingBox(World* pWorld, const BlockPos& pos) { return Box(); }
	virtual RayTraceResult collisionRayTrace(World* pWorld, const BlockPos& pos, const Vector3& start, const Vector3& end);
	virtual void setBlockBoundsBasedOnState(IBlockAccess* pBlockAccess, const BlockPos& pos);
	virtual int quantityDropped(Random& rand) { return 1; }
	virtual bool canPlaceBlockAt(World* pWorld, const BlockPos& pos);
	virtual void onBlockAdded(World* pWorld, const BlockPos& pos);
	virtual void onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborBlockID);
	virtual int getMobilityFlag() { return 0; }
	virtual void breakBlock(World* pWorld, const BlockPos& pos, int blockID, int meta);
	
	/** update the real way state. */
	virtual void updateRailState(World* pWorld, const BlockPos& pos, int meta, int dir, int neightborID) {}
	bool isPowered() { return m_isPowered; }

	static bool isRailBlockAt(World* pWorld, const BlockPos& pos);
	static bool isRailBlock(int blockID);

};

class BlockRailPowered : public BlockRailBase
{
public:
	BlockRailPowered(int id, const BM_Material& mat, const String& name);

	/** implement override functions from BlockRailBase */
	virtual void updateRailState(World* pWorld, const BlockPos& pos, int meta, int dir, int neightborID);
	bool findPoweredRailSignal(World* pWorld, const BlockPos& pos, int meta, bool polarity, int dir);
	bool isSameRailWithPower(World* pWorld, const BlockPos& pos, bool polarity, int dis, int dir);
};

class BlockRail : public BlockRailBase
{
public:
	BlockRail(int id, const BM_Material& mat, const String& name);

	/** implement override functions from BlockRailBase */
	virtual void updateRailState(World* pWorld, const BlockPos& pos, int meta, int dir, int neightborID);
};

class BlockDetectorRail : public BlockRailBase
{
public:
	BlockDetectorRail(int id, const BM_Material& mat, const String& name);

	/** implement override functions from Block */
	virtual int tickRate(World* pWorld) const { return 20; }
	virtual bool canProvidePower() { return true; }
	virtual void onEntityCollidedWithBlock(World* pWorld, const BlockPos& pos, Entity* pEntity);
	virtual void updateTick(World* pWorld, const BlockPos& pos, Random& rand);
	virtual int isProvidingWeakPower(IBlockAccess* piBlockAccess, const BlockPos& pos, int side);
	virtual int isProvidingStrongPower(IBlockAccess* piBlockAccess, const BlockPos& pos, int side);
	virtual void onBlockAdded(World* pWorld, const BlockPos& pos);
	virtual bool hasComparatorInputOverride() { return true; }
	virtual int getComparatorInputOverride(World* pWorld, const BlockPos& pos, int side);

protected:
	void setStateIfMinecartInteractsWithRail(World* pWorld, const BlockPos& pos, int meta);
};

class BlockPistonBase : public Block
{
public:
	BlockPistonBase(int id, const BM_Material& mat, const String& name, bool isSticky);

	/** implement override functions from Block */
	virtual bool renderAsNormalBlock() const { return false; }
	virtual void setBlockBoundsBasedOnState(IBlockAccess* pBlockAccess, const BlockPos& pos);
	virtual void setBlockBoundsForItemRender() { setBlockBounds(0.0F, 0.0F, 0.0F, 1.0F, 1.0F, 1.0F); }
	virtual bool onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit) { return false; }
	virtual void onBlockPlacedBy(World* pWorld, const BlockPos& pos, EntityLivingBase* pLiving, ItemStackPtr pStack);
	virtual void onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborBlockID);
	virtual void onBlockAdded(World* pWorld, const BlockPos& pos);
	virtual bool onBlockEventReceived(World* pWorld, const BlockPos& pos, int id, int param);
	virtual void addCollisionBoxesToList(World* pWorld, const BlockPos& pos, const Box& aabb, AABBList& lst, Entity* pEntity);
	virtual Box getCollisionBoundingBox(World* pWorld, const BlockPos& pos);
	
	/** returns an int which describes the direction the piston faces	*/
	static int getOrientation(int meta) { return meta & 7; }
	/** Determine if the metadata is related to something powered. */
	static bool isExtended(int meta) { return (meta & 8) != 0; }
	/** gets the way this piston should face for that entity that placed it. */
	static int determineOrientation(World* pWorld, const BlockPos& pos, EntityLivingBase* pLiving);
	/** returns true if the piston can push the specified block */
	static bool canPushBlock(int blockID, World* pWorld, const BlockPos& pos, bool par5);
	/** checks to see if this piston could push the blocks in front of it. */
	static bool canExtend(World* pWorld, const BlockPos& pos, int side);

protected:
	bool m_isSticky;
	/** handles attempts to extend or retract the piston. */
	void updatePistonState(World* pWorld, const BlockPos& pos);
	/** checks the block to that side to see if it is indirectly powered. */
	bool isIndirectlyPowered(World* pWorld, const BlockPos& pos, int side);
	/** attempts to extend the piston. returns false if impossible. */
	bool tryExtend(World* pWorld, const BlockPos& pos, int side);
};

class BlockPistonExtension : public Block
{
public:
	BlockPistonExtension(int id, const BM_Material& mat, const String& name);

	/** implement override functions from Block */
	virtual bool renderAsNormalBlock() const { return false; }
	virtual void setBlockBoundsBasedOnState(IBlockAccess* pBlockAccess, const BlockPos& pos);
	virtual void onBlockHarvested(World* pWorld, const BlockPos& pos, int meta, EntityPlayer* pPlayer);
	virtual void breakBlock(World* pWorld, const BlockPos& pos, int blockID, int meta);
	virtual int quantityDropped(Random& rand) { return 0; }
	virtual bool canPlaceBlockAt(World* pWorld, const BlockPos& pos) { return false; }
	virtual bool canPlaceBlockOnSide(World* pWorld, const BlockPos& pos, int side) { return false; }
	virtual void addCollisionBoxesToList(World* pWorld, const BlockPos& pos, const Box& aabb, AABBList& lst, Entity* pEntity);
	virtual void onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborBlockID);
	virtual int idPicked(World* pWorld, const BlockPos& pos);
	
	static int getDirectionMeta(int meta) { return meta & 7; }
};

class TileEntityPiston;

class BlockPistonMoving : public BlockContainer
{
public:
	BlockPistonMoving(int id, const BM_Material& mat, const String& name);
	
	/** implement override functions from ITileEntityProvider */
	virtual TileEntity* createNewTileEntity(World* pWorld) { return NULL; }

	/** implement override functions from Block */
	virtual void onBlockAdded(World* pWorld, const BlockPos& pos) {}
	virtual void breakBlock(World* pWorld, const BlockPos& pos, int blockID, int meta);
	virtual bool canPlaceBlockAt(World* pWorld, const BlockPos& pos) { return false; }
	virtual bool canPlaceBlockOnSide(World* pWorld, const BlockPos& pos, int side) { return false; }
	virtual bool renderAsNormalBlock() const { return false; }
	virtual bool onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit);
	virtual int idDropped(int meta, Random& rand, int fortune) { return 0; }
	virtual void dropBlockAsItemWithChance(World* pWorld, const BlockPos& pos, int meta, float chance, int fortune);
	virtual void onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborID);
	virtual Box getCollisionBoundingBox(World* pWorld, const BlockPos& pos);
	virtual void setBlockBoundsBasedOnState(IBlockAccess* piBlockAccess, const BlockPos& pos);
	virtual int isPowered(World* pWorld, const BlockPos& pos) { return 0; }

	/** gets a new TileEntityPiston created with the arguments provided. */
	static TileEntity* getTileEntity(int id, int meta, int side, bool extending, bool renderhead);
	/** get aabb from pos */
	Box getAxisAlignedBB(World* pWorld, const BlockPos& pos, int blockID, float progress, int side);
	
protected:
	/** gets the piston tile entity at the specified location */
	TileEntityPiston* getTileEntityAtLocation(IBlockAccess* pAccess, const BlockPos& pos);
};

class BlockTorch : public Block
{
public:
	BlockTorch(int id, const BM_Material& mat, const String& name);

	/** implement override functions from Block */
	virtual bool renderAsNormalBlock() const { return false; }
	virtual Box getCollisionBoundingBox(World* pWorld, const BlockPos& pos) { return Box(); }
	virtual bool canPlaceBlockAt(World* pWorld, const BlockPos& pos);
	virtual int onBlockPlaced(World* pWorld, const BlockPos& pos, int side, const Vector3& hit, int meta);
	virtual void updateTick(World* pWorld, const BlockPos& pos, Random& rand);
	virtual void onBlockAdded(World* pWorld, const BlockPos& pos);
	virtual void onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborBlockID);
	virtual RayTraceResult collisionRayTrace(World* pWorld, const BlockPos& pos, const Vector3& start, const Vector3& end);

protected:
	bool onNeighborChange_impl(World* pWorld, const BlockPos& pos);
	bool dropTorchIfCantStay(World* pWorld, const BlockPos& pos);
	bool canPlaceTorchOn(World* pWorld, const BlockPos& pos);
};

class BlockRedstoneTorch : public BlockTorch
{
public:
	BlockRedstoneTorch(int id, const BM_Material& mat, const String& name, bool isActive);

	/** implement override functions from Block */
	virtual int tickRate(World* pWorld) const { return 2; }
	virtual void onBlockAdded(World* pWorld, const BlockPos& pos);
	virtual void breakBlock(World* pWorld, const BlockPos& pos, int blockID, int meta);
	virtual int isProvidingWeakPower(IBlockAccess* piBlockAccess, const BlockPos& pos, int side);
	virtual void updateTick(World* pWorld, const BlockPos& pos, Random& rand);
	virtual void onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborBlockID);
	virtual int isProvidingStrongPower(IBlockAccess* iBlockAccess, const BlockPos& pos, int side);
	virtual bool canProvidePower() { return true; }
	virtual int idDropped(int meta, Random& rand, int fortune) { return BLOCK_ID_TORCH_REDSTONE_ACTIVE; }
	virtual int idPicked(World* pWorld, const BlockPos& pos);
	virtual bool isAssociatedBlockID(int blockID) { return blockID == BLOCK_ID_TORCH_REDSTONE_IDLE || blockID == BLOCK_ID_TORCH_REDSTONE_ACTIVE; }

protected:
	bool m_isActive = false;

	typedef list<std::pair<BlockPos, i64>>::type RedstoneUpdateInfoArr;
	static RedstoneUpdateInfoArr redstoneUpdateInfo;
	bool checkForBurnout(World* pWorld, const BlockPos& pos, bool par5);
	/* Returns true or false based on whether the block the torch is attached to is providing indirect power. */
	bool isIndirectlyPowered(World* pWorld, const BlockPos& pos);
};

class BlockFire : public Block
{
public:
	BlockFire(int id, const BM_Material& mat, const String& name);

	/** implement override functions from Block */
	virtual void initializeBlock();
	virtual bool renderAsNormalBlock() const { return false; }	
	virtual int quantityDropped(Random& rand) { return 0; }
	virtual int tickRate(World* pWorld) const { return 30; }
	virtual void updateTick(World* pWorld, const BlockPos& pos, Random& rand);
	virtual bool isUpdateScheduled() const  { return false; }
	virtual bool isCollidable() const { return false; }
	virtual bool canPlaceBlockAt(World* pWorld, const BlockPos& pos);
	virtual void onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborBlockID);
	virtual void onBlockAdded(World* pWorld, const BlockPos& pos);
	virtual Box getCollisionBoundingBox(World* pWorld, const BlockPos& pos) { return Box(); }

	/** Retrieves a specified block's chance to encourage their neighbors to burn and if the number is greater than the
	* current number passed in it will return its number instead of the passed in one. */
	int getChanceToEncourageFire(World* pWorld, const BlockPos& pos, int chance);
	/** Checks the specified block coordinate to see if it can catch fire.  Args: blockAccess, pos */
	bool canBlockCatchFire(IBlockAccess* pBlockAccess, const BlockPos& pos);

protected:
	int m_chanceToEncourageFire[256] = { 0 };
	int m_abilityToCatchFire[256] = { 0 };

	/** Sets the burn rate for a block. The larger abilityToCatchFire the more easily it will catch. */
	void setBurnRate(int blockID, int chanceToEncourageFire, int abilityToCatchFire);
	void tryToCatchBlockOnFire(World* pWorld, const BlockPos& pos, int chance, Random* pRand, int par7);
	/** Returns true if at least one block next to this one can burn. */
	bool canNeighborBurn(World* pWorld, const BlockPos& pos);
	/** Gets the highest chance of a neighbor block encouraging this block to catch fire */
	int getChanceOfNeighborsEncouragingFire(World* pWorld, const BlockPos& pos);
};

class BlockLever : public Block
{
public:
	BlockLever(int id, const BM_Material& mat, const String& name);

	/** implement override functions from Block */
	virtual bool renderAsNormalBlock() const { return false; }
	virtual void setBlockBoundsBasedOnState(IBlockAccess* pBlockAccess, const BlockPos& pos);
	virtual Box getCollisionBoundingBox(World* pWorld, const BlockPos& pos) { return Box(); }
	virtual bool canPlaceBlockOnSide(World* pWorld, const BlockPos& pos, int side);
	virtual bool canPlaceBlockAt(World* pWorld, const BlockPos& pos);
	virtual int onBlockPlaced(World* pWorld, const BlockPos& pos, int side, const Vector3& hit, int meta);
	virtual void onBlockPlacedBy(World* pWorld, const BlockPos& pos, EntityLivingBase* pLiving, ItemStackPtr pStack);
	virtual void onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborBlockID);
	virtual bool onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit);
	virtual void breakBlock(World* pWorld, const BlockPos& pos, int blockID, int meta);
	virtual int isProvidingWeakPower(IBlockAccess* piBlockAccess, const BlockPos& pos, int side);
	virtual int isProvidingStrongPower(IBlockAccess* piBlockAccess, const BlockPos& pos, int side);
	virtual bool canProvidePower() { return true; }

	static int invertMetadata(int meta);

protected:

	/** Checks if the block is attached to another block. If it is not, it returns false and drops the block as an item. If it is it returns true. */
	bool checkIfAttachedToBlock(World* pWorld, const BlockPos& pos);
};

class BlockNetherStalk : public BlockFlower
{
public:
	BlockNetherStalk(int id, const BM_Material& mat, const String& name);
	
	/** implement override functions from Block */
	virtual bool canBlockStay(World* pWorld, const BlockPos& pos);
	virtual void updateTick(World* pWorld, const BlockPos& pos, Random& rand);
	virtual void dropBlockAsItemWithChance(World* pWorld, const BlockPos& pos, int meta, float chance, int fortune);
	virtual int idDropped(int meta, Random& rand, int fortune) { return 0; }
	virtual int quantityDropped(Random& rand) { return 0; }
	virtual int isPowered(World* pWorld, const BlockPos& pos) { return ITEM_ID_NETHER_STALK_SEEDS; }

protected:
	virtual bool canThisPlantGrowOnThisBlockID(int blockID) { return blockID == BLOCK_ID_SLOW_SAND; }
};

class BlockBrewingStand : public BlockContainer
{
public:
	BlockBrewingStand(int id, const BM_Material& mat, const String& name);
	
	/** implement override functions from ITileEntityProvider */
	TileEntity* createNewTileEntity(World* pWorld);

	/** implement override functions from Block */
	virtual bool renderAsNormalBlock() const { return false; }
	virtual void setBlockBoundsForItemRender();
	virtual void addCollisionBoxesToList(World* pWorld, const BlockPos& pos, const Box& aabb, AABBList& lst, Entity* pEntity);
	virtual bool onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit);
	virtual void onBlockPlacedBy(World* pWorld, const BlockPos& pos, EntityLivingBase* pLiving, ItemStackPtr pStack);
	virtual void breakBlock(World* pWorld, const BlockPos& pos, int blockID, int meta);
	virtual int idDropped(int meta, Random& rand, int fortune) { return ITEM_ID_BREWING_STAND; }
	virtual int isPowered(World* pWorld, const BlockPos& pos) { return ITEM_ID_BREWING_STAND; }
	virtual bool hasComparatorInputOverride() { return true; }
	virtual int getComparatorInputOverride(World* pWorld, const BlockPos& pos, int side);
};

class BlockCauldron : public Block
{
public:
	BlockCauldron(int id, const BM_Material& mat, const String& name);

	/** implement override functions from Block */
	virtual bool renderAsNormalBlock() const { return false; }
	virtual void setBlockBoundsForItemRender();
	virtual void addCollisionBoxesToList(World* pWorld, const BlockPos& pos, const Box& aabb, AABBList& lst, Entity* pEntity);
	virtual bool onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit);
	virtual void fillWithRain(World* pWorld, const BlockPos& pos);
	virtual int idDropped(int meta, Random& rand, int fortune) { return ITEM_ID_CAULDRON; }
	virtual int isPowered(World* pWorld, const BlockPos& pos) { return ITEM_ID_CAULDRON; }
	virtual bool hasComparatorInputOverride() { return true; }
	virtual int getComparatorInputOverride(World* pWorld, const BlockPos& pos, int side);
};

class BlockDragonEgg : public Block
{
public:
	BlockDragonEgg(int id, const BM_Material& mat, const String& name);

	/** implement override functions from Block */
	virtual bool renderAsNormalBlock() const { return false; }
	virtual void onBlockAdded(World* pWorld, const BlockPos& pos);
	virtual void onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborBlockID);
	virtual void updateTick(World* pWorld, const BlockPos& pos, Random& rand);
	virtual bool onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit);
	virtual void onBlockClicked(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer);
	virtual int tickRate(World* pWorld) const { return 5; }
	virtual int isPowered(World* pWorld, const BlockPos& pos) { return 0; }

protected:
	void fallIfPossible(World* pWorld, const BlockPos& pos);
	void teleportNearby(World* pWorld, const BlockPos& pos);

};

class BlockTripWire : public Block
{
public:
	BlockTripWire(int id, const BM_Material& mat, const String& name);

	/** implement override functions from Block */
	virtual bool renderAsNormalBlock() const { return false; }
	virtual int tickRate(World* pWorld) const { return 10; }
	virtual void setBlockBoundsBasedOnState(IBlockAccess* pBlockAccess, const BlockPos& pos);
	virtual Box getCollisionBoundingBox(World* pWorld, const BlockPos& pos) { return Box(); }
	virtual int idDropped(int meta, Random& rand, int fortune) { return ITEM_ID_SILK; }
	virtual int isPowered(World* pWorld, const BlockPos& pos) { return ITEM_ID_SILK; }
	virtual void onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborBlockID);
	virtual void onBlockAdded(World* pWorld, const BlockPos& pos);
	virtual void breakBlock(World* pWorld, const BlockPos& pos, int blockID, int meta);
	virtual void onBlockHarvested(World* pWorld, const BlockPos& pos, int meta, EntityPlayer* pPlayer);
	virtual void onEntityCollidedWithBlock(World* pWorld, const BlockPos& pos, Entity* pEntity);
	virtual void updateTick(World* pWorld, const BlockPos& pos, Random& rand);

	/** Called serverside after this block is replaced with another in Chunk, but before the Tile Entity is updated */
	void notifyHook(World* pWorld, const BlockPos& pos, int par5);
	static bool isConnectedTo(IBlockAccess* pBlockAccess, const BlockPos& pos, int meta, int side);

protected:
	void updateTripWireState(World* pWorld, const BlockPos& pos);
};

class BlockTripWireSource : public Block
{
public:
	BlockTripWireSource(int id, const BM_Material& mat, const String& name);

	/** implement override functions from Block */
	virtual bool renderAsNormalBlock() const { return false; }
	virtual int tickRate(World* pWorld) const { return 10; }
	virtual void setBlockBoundsBasedOnState(IBlockAccess* pBlockAccess, const BlockPos& pos);
	virtual Box getCollisionBoundingBox(World* pWorld, const BlockPos& pos) { return Box(); }
	virtual bool canPlaceBlockOnSide(World* pWorld, const BlockPos& pos, int side);
	virtual bool canPlaceBlockAt(World* pWorld, const BlockPos& pos);
	virtual int onBlockPlaced(World* pWorld, const BlockPos& pos, int side, const Vector3& hit, int meta);
	virtual void onPostBlockPlaced(World* pWorld, const BlockPos& pos, int side);
	virtual void onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborBlockID);
	virtual void updateTick(World* pWorld, const BlockPos& pos, Random& rand);
	virtual void breakBlock(World* pWorld, const BlockPos& pos, int blockID, int meta);
	virtual int isProvidingWeakPower(IBlockAccess* piBlockAccess, const BlockPos& pos, int side);
	virtual int isProvidingStrongPower(IBlockAccess* piBlockAccess, const BlockPos& pos, int side);
	virtual bool canProvidePower() { return true; }

	void calculateState(World* pWorld, const BlockPos& pos, int blockID, int meta, bool par7, int par8, int par9);

protected:
	virtual void playSoundEffect(World* pWorld, const BlockPos& pos, bool par5, bool par6, bool par7, bool par8) {}
	void notifyNeighborOfChange(World* pWorld, const BlockPos& pos, int side);
	bool checkForDrop(World* pWorld, const BlockPos& pos);
};

class BlockHopper : public BlockContainer
{
public:
	BlockHopper(int id, const BM_Material& mat, const String& name);
	
	/** implement override functions from ITileEntityProvider */
	virtual TileEntity* createNewTileEntity(World* pWorld);

	/** implement override functions from Block */
	virtual bool renderAsNormalBlock() const { return false; }
	virtual void setBlockBoundsBasedOnState(IBlockAccess* pBlockAccess, const BlockPos& pos) { setBlockBounds(0.0F, 0.0F, 0.0F, 1.0F, 1.0F, 1.0F); }
	virtual void addCollisionBoxesToList(World* pWorld, const BlockPos& pos, const Box& aabb, AABBList& lst, Entity* pEntity);
	virtual int onBlockPlaced(World* pWorld, const BlockPos& pos, int side, const Vector3& hit, int meta);
	virtual void onBlockPlacedBy(World* pWorld, const BlockPos& pos, EntityLivingBase* pLiving, ItemStackPtr pStack);
	virtual void onBlockAdded(World* pWorld, const BlockPos& pos);
	virtual bool onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit);
	virtual void onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborBlockID);
	virtual void breakBlock(World* pWorld, const BlockPos& pos, int blockID, int meta);
	virtual bool hasComparatorInputOverride() { return true; }
	virtual int getComparatorInputOverride(World* pWorld, const BlockPos& pos, int side);
	
	static int getDirectionFromMetadata(int meta) { return meta & 7; }
	static bool getIsBlockNotPoweredFromMetadata(int meta) { return (meta & 8) != 8; }
	//static TileEntityHopper getHopperTile(IBlockAccess par0IBlockAccess, int par1, int par2, int par3)

protected:
	//private final Random field_94457_a = new Random();
	void updateMetadata(World* pWorld, const BlockPos& pos);
};

class BlockDropper : public BlockDispenser
{
public:
	BlockDropper(int id, const BM_Material& mat, const String& name);

	/** implement override functions from ITileEntityProvider */
	virtual TileEntity* createNewTileEntity(World* pWorld) { return NULL; }//return new TileEntityDropper(); }

protected:
	//IBehaviorDispenseItem getBehaviorForItemStack(ItemStack par1ItemStack)

	//void dispense(World par1World, const BlockPos& pos)
};

class BlockBasePressurePlate : public Block
{
public:
	BlockBasePressurePlate(int id, const BM_Material& mat, const String& name);

	/** implement override functions from Block */
	virtual bool renderAsNormalBlock() const { return false; }
	virtual void setBlockBoundsBasedOnState(IBlockAccess* pBlockAccess, const BlockPos& pos);
	virtual void setBlockBoundsForItemRender();
	virtual int tickRate(World* pWorld) const { return 5; }
	virtual Box getCollisionBoundingBox(World* pWorld, const BlockPos& pos) { return Box(); }
	virtual bool getBlocksMovement(IBlockAccess* pAccess, const BlockPos& pos) const { return true; }
	virtual bool canPlaceBlockAt(World* pWorld, const BlockPos& pos);
	virtual void onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neightborID);
	virtual void updateTick(World* pWorld, const BlockPos& pos, Random& rand);
	virtual void onEntityCollidedWithBlock(World* pWorld, const BlockPos& pos, Entity* pEntity);
	virtual void breakBlock(World* pWorld, const BlockPos& pos, int blockID, int meta);
	virtual int isProvidingWeakPower(IBlockAccess* piBlockAccess, const BlockPos& pos, int side);
	virtual int isProvidingStrongPower(IBlockAccess* piBlockAccess, const BlockPos& pos, int side);
	virtual bool canProvidePower() { return true; }
	virtual int getMobilityFlag() { return 1; }

	/** Returns the current state of the pressure plate. Returns a value between 0 and 15 based on the number of items on it. */
	virtual int getPlateState(World* pWorld, const BlockPos& pos) = 0;
	/** Argument is metadata. Returns power level (0-15) */
	virtual int getPowerSupply(int meta) = 0;
	/** Argument is weight (0-15). Return the metadata to be set because of it. */
	virtual int getMetaFromWeight(int weight) = 0;

protected:
	/** set the block bounding box by if the block is powered. */
	void SetBlockBoundByPower(bool isPowered);
	/** Checks if there are mobs on the plate. If a mob is on the plate and it is off, it turns it on, and vice versa. */
	void setStateIfMobInteractsWithPlate(World* pWorld, const BlockPos& pos, int power, int entityId = -1);
	Box getSensitiveAABB(const BlockPos& pos);
	void updateNeighbors(World* pWorld, const BlockPos& pos);

};

class BlockPressurePlate : public BlockBasePressurePlate
{
public:
	BlockPressurePlate(int id, const BM_Material& mat, const String& name, MOB_TYPE mobType);

	/** implement override functions from BlockBasePressurePlate */
	virtual int getPlateState(World* pWorld, const BlockPos& pos);
	virtual int getPowerSupply(int meta) { return meta == 1 ? 15 : 0; }
	virtual int getMetaFromWeight(int weight) { return weight > 0 ? 1 : 0; }

protected:
	MOB_TYPE m_triggerMobType = MOB_TYPE_EVERYTHING;
};

class BlockPressurePlateWeighted : public BlockBasePressurePlate
{
public:
	BlockPressurePlateWeighted(int id, const BM_Material& mat, const String& name, int maxItemsWeighted, MOB_TYPE mobType);

	/** implement override functions from BlockBasePressurePlate */
	virtual int getPlateState(World* pWorld, const BlockPos& pos);
	virtual int getPowerSupply(int meta) { return meta; }
	virtual int getMetaFromWeight(int weight) { return weight; }

	/** implement override functions from Block */
	virtual int tickRate(World* pWorld) const { return 10; }

protected:
	int m_maxItemsWeighted = 0;
	MOB_TYPE m_triggerMobType = MOB_TYPE_EVERYTHING;
};

class BlockSoulSand : public Block
{
public:
	BlockSoulSand(int id, const BM_Material& mat, const String& name);

	/** implement override functions from Block */
	virtual Box getCollisionBoundingBox(World* pWorld, const BlockPos& pos);
	virtual void onEntityCollidedWithBlock(World* pWorld, const BlockPos& pos, Entity* pEntity);
};

class BlockGlowStone : public Block
{
public:
	BlockGlowStone(int id, const BM_Material& mat, const String& name);

	/** implement override functions from Block */
	virtual int quantityDroppedWithBonus(int bonus, Random& rand);
	virtual int quantityDropped(Random& rand);
	virtual int idDropped(int meta, Random& rand, int fortune) { return ITEM_ID_GLOWSTONE; }
};

class BlockEndPortalFrame : public Block
{
public:
	BlockEndPortalFrame(int id, const BM_Material& mat, const String& name);

	/** implement override functions from Block */
	virtual void setBlockBoundsForItemRender() { setBlockBounds(0.0F, 0.0F, 0.0F, 1.0F, 0.8125F, 1.0F); }
	virtual void addCollisionBoxesToList(World* pWorld, const BlockPos& pos, const Box& aabb, AABBList& lst, Entity* pEntity);
	virtual int idDropped(int meta, Random& rand, int fortune) { return 0; }
	virtual void onBlockPlacedBy(World* pWorld, const BlockPos& pos, EntityLivingBase* pEntityLivingBase, ItemStackPtr pItemStack);
	virtual bool hasComparatorInputOverride() { return true; }
	virtual int getComparatorInputOverride(World* pWorld, const BlockPos& pos, int side);

	/* checks if an ender eye has been inserted into the frame block. parameters: metadata */
	static bool isEnderEyeInserted(int meta) { return (meta & 4) != 0; }
};

class BlockSign : public BlockContainer
{
protected:
	/** Whether this is a freestanding sign or a wall-mounted sign */
	bool isFreestanding = false;
	int  signClassID = 0;

public:
	BlockSign(int id, const BM_Material& mat, const String& name, int signClass, bool freestanding);
	
	/** implement override functions from ITileEntityProvider */
	virtual TileEntity* createNewTileEntity(World* pWorld);

	/** implement override functions from Block */
	virtual Box getCollisionBoundingBox(World* pWorld, const BlockPos& pos) { return Box(); }
	virtual void setBlockBoundsBasedOnState(IBlockAccess* pBlockAccess, const BlockPos& pos);
	virtual bool renderAsNormalBlock() const { return false; }
	virtual bool getBlocksMovement(IBlockAccess* pBlockAccess, const BlockPos& pos) const { return true; }
	virtual int idDropped(int meta, Random& rand, int fortune);
	virtual int idPicked(World* pWorld, const BlockPos& pos);
	virtual void onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborID);
	virtual bool onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit);
};

class BlockRedstoneWire : public Block
{
public:
	BlockRedstoneWire(int id, const BM_Material& mat, const String& name);

	/** implement override functions from Block */
	virtual Box getCollisionBoundingBox(World* pWorld, const BlockPos& pos) override { return Box(); }
	virtual bool renderAsNormalBlock() const override { return false; }
	virtual bool canPlaceBlockAt(World* pWorld, const BlockPos& pos) override;
	virtual void onBlockAdded(World* pWorld, const BlockPos& pos) override;
	virtual void breakBlock(World* pWorld, const BlockPos& pos, int blockID, int meta) override;
	virtual void onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborID) override;
	virtual int idDropped(int meta, Random& rand, int fortune) override;
	virtual int isProvidingStrongPower(IBlockAccess* pBlockAccess, const BlockPos& pos, int side) override;
	virtual int isProvidingWeakPower(IBlockAccess* pBlockAccess, const BlockPos& pos, int side) override;
	virtual bool canProvidePower() override { return wiresProvidePower; }
	virtual int idPicked(World* pWorld, const BlockPos& pos) override;

	/** Returns true if redstone wire can connect to the specified block. */
	static bool isPowerProviderOrWire(IBlockAccess* pBlockAccess, const BlockPos& pos, int dir);
	/** Returns true if the block coordinate passed can provide power, or is a redstone wire, or if its a repeater that is powered. */
	static bool isPoweredOrRepeater(IBlockAccess* pBlockAccess, const BlockPos& pos, int dir);
	
protected:
	/** Sets the strength of the wire current (0-15) for this block based on neighboring blocks and propagates to neighboring redstone wires */
	void updateAndPropagateCurrentStrength(World* world, const BlockPos& pos);

	void calculateCurrentChanges(World* world, const BlockPos& pos1, const BlockPos& pos2);

	/** Calls World.notifyBlocksOfNeighborChange() for all neighboring blocks, but only if the given block is a redstone wire. */
	void notifyWireNeighborsOfNeighborChange(World* world, const BlockPos& pos);
	
	/** Returns the current strength at the specified block if it is greater than the passed value, or the passed value otherwise. Signature: (world, x, y, z, strength) */
	int getMaxCurrentStrength(World* pWorld, const BlockPos& pos, int minStrength);
	
protected:
	bool wiresProvidePower = false;
	BlockPosArr blocksNeedingUpdate;
};

class BlockRedstoneLogic : public BlockDirectional
{
public:
	BlockRedstoneLogic(int id, const BM_Material& mat, const String& name, bool powered);

	/** implement override functions from Block */
	virtual bool renderAsNormalBlock() const { return false; }
	virtual bool canPlaceBlockAt(World* pWorld, const BlockPos& pos);
	virtual bool canBlockStay(World* pWorld, const BlockPos& pos);
	virtual void updateTick(World* pWorld, const BlockPos& pos, Random& rand);
	virtual int isProvidingStrongPower(IBlockAccess* pBlockAccess, const BlockPos& pos, int side);
	virtual int isProvidingWeakPower(IBlockAccess* pBlockAccess, const BlockPos& pos, int side);
	virtual void onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborID);
	virtual bool canProvidePower() { return true; }
	virtual void onBlockPlacedBy(World* pWorld, const BlockPos& pos, EntityLivingBase* pLiving, ItemStackPtr pStack);
	virtual void onBlockAdded(World* pWorld, const BlockPos& pos) { notifyNeighbors(pWorld, pos); }
	virtual void onBlockDestroyedByPlayer(World* pWorld, const BlockPos& pos, int meta);
	virtual bool isAssociatedBlockID(int blockID) { return isSameDiode(blockID); }

	virtual bool isLocked(IBlockAccess* pBlockAccess, const BlockPos& pos, int meta) { return false; }

	bool isSameDiode(int blockID);
	bool isFacingTowardsRepeater(World* pWorld, const BlockPos& pos, int meta);

	static bool isRedstoneRepeaterBlockID(int blockID);

public:
	/** Tells whether the repeater is powered or not */
	bool isRepeaterPowered = false;

protected:
	int calculateInputStrength(IBlockAccess* pBlockAccess, const BlockPos& pos, int meta);
	int getInputStrength(IBlockAccess* pBlockAccess, const BlockPos& pos, int meta);
	void notifyNeighbors(World* pWorld, const BlockPos& pos);
	int getTickDelay(int blockID) { return getDelay(blockID); }

	virtual bool isBlockProvidePower(int blockID);
	virtual void updateState(World* pWorld, const BlockPos& pos, int blockID);
	virtual int getInputStrength(World* pWorld, const BlockPos& pos, int meta);
	virtual bool isGettingInput(World* pWorld, const BlockPos& pos, int meta);
	virtual int getActiveSignal(IBlockAccess* pBlockAccess, const BlockPos& pos, int meta) { return 15; }
	virtual bool isPowered(int meta) { return isRepeaterPowered; }
	virtual int getDelay(int meta) = 0;
	virtual int getPoweredState() = 0;
	virtual int getUperState() = 0;
};

class TileEntityComparator;
class BlockComparator : public BlockRedstoneLogic, public ITileEntityProvider
{
public:
	BlockComparator(int id, const BM_Material& mat, const String& name, bool powered); 

	/** implement override functions from ITileEntityProvider */
	virtual TileEntity* createNewTileEntity(World* pWorld);

	/** implement override functions from Block */
	virtual int idDropped(int meta, Random& rand, int fortune);
	virtual int idPicked(World* pWorld, const BlockPos& pos);
	virtual bool onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit);
	virtual void updateTick(World* pWorld, const BlockPos& pos, Random& rand);
	virtual void onBlockAdded(World* pWorld, const BlockPos& pos);
	virtual void breakBlock(World* pWorld, const BlockPos& pos, int blockID, int meta);
	virtual bool onBlockEventReceived(World* pWorld, const BlockPos& pos, int id, int param);

	bool isComparatorActived(int meta) { return (meta & 4) == 4; }
	/** Returns the blockTileEntity at given coordinates. */
	TileEntityComparator* getTileEntityComparator(IBlockAccess* pBlockAccess, const BlockPos& pos);
	
protected:
	/** implement override functions from BlockRedstoneLogic */
	virtual int getDelay(int meta) { return 2; }
	virtual int getPoweredState() { return BLOCK_ID_REDSTONE_COMPARATOR_ACTIVE; }
	virtual int getUperState() { return BLOCK_ID_REDSTONE_COMPARATOR_IDLE; }
	virtual bool isPowered(int meta) { return isRepeaterPowered || (meta & 8) != 0; }
	virtual int getActiveSignal(IBlockAccess* pBlockAccess, const BlockPos& pos, int meta);
	virtual bool isGettingInput(World* pWorld, const BlockPos& pos, int meta);
	virtual int getInputStrength(World* pWorld, const BlockPos& pos, int meta);
	virtual void updateState(World* pWorld, const BlockPos& pos, int blockID);

	int getOutputStrength(World* pWorld, const BlockPos& pos, int meta);
	void onStateChange(World* pWorld, const BlockPos& pos, Random& rand);
};


class BlockRedstoneRepeater : public BlockRedstoneLogic
{
protected:
	/** The states in which the redstone repeater blocks can be. */
	static int repeaterState[4]; // = new int[] {1, 2, 3, 4};

public:
	BlockRedstoneRepeater(int id, const BM_Material& mat, const String& name, bool powered);

	/** implement override functions from Block */
	virtual bool onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit);
	virtual int idDropped(int meta, Random& rand, int fortune);
	virtual int idPicked(World* pWorld, const BlockPos& pos);
	virtual void breakBlock(World* pWorld, const BlockPos& pos, int blockID, int meta);

	/** implement override functions from BlockRedstoneLogic */
	virtual bool isLocked(IBlockAccess* pBlockAccess, const BlockPos& pos, int meta);

protected:
	/** implement override functions from BlockRedstoneLogic */
	virtual int getDelay(int meta) { return repeaterState[(meta & 12) >> 2] * 2; }
	virtual int getPoweredState() { return BLOCK_ID_REDSTONE_REPEATER_ACTIVE; }
	virtual int getUperState() { return BLOCK_ID_REDSTONE_REPEATER_IDLE; }
	virtual bool isBlockProvidePower(int block) { return isRedstoneRepeaterBlockID(block); }
};

class BlockEndPortal : public BlockContainer
{
public:
	/** true if the enderdragon has been killed - allows end portal blocks to be created in the end */
	static bool bossDefeated;

public:
	BlockEndPortal(int id, const BM_Material& mat, const String& name);

	/** implement override functions from ITileEntityProvider */
	virtual TileEntity* createNewTileEntity(World* pWorld);
	
	/** implement override functions from Block */
	virtual void setBlockBoundsBasedOnState(IBlockAccess* piBlockAccess, const BlockPos& pos);
	virtual void addCollisionBoxesToList(World* pWorld, const BlockPos& pos, const Box& aabb, AABBList& lst, Entity* pEntity) {}
	virtual bool renderAsNormalBlock() const { return false; }
	virtual int quantityDropped(Random& rand) {	return 0; }
	virtual void onEntityCollidedWithBlock(World* pWorld, const BlockPos& pos, Entity* pEntity);
	virtual void onBlockAdded(World* pWorld, const BlockPos& pos);
	virtual int idPicked(World* pWorld, const BlockPos& pos) { return 0; }
};

class BlockEnderChest : public BlockContainer
{
public:
	enum BlockEnderChestType
	{
		BECT_OLD = 0,
		BECT_NEW = 1,
	};

	BlockEnderChest(int id, const BM_Material& mat, const String& name, int block_ender_chest_type);
	int getChestType() { return m_block_ender_chest_type; }

	/** implement override functions from ITileEntityProvider */
	virtual TileEntity* createNewTileEntity(World* pWorld);

	/** implement override functions from Block */
	virtual bool renderAsNormalBlock() const { return false; }
	virtual int idDropped(int meta, Random& rand, int fortune);
	virtual int quantityDropped(Random& rand) {	return 8; }
	virtual bool canSilkHarvest() { return true; }
	virtual void onBlockPlacedBy(World* pWorld, const BlockPos& pos, EntityLivingBase* pLiving, ItemStackPtr pStack);
	virtual bool onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit);
protected:
	int m_block_ender_chest_type = BECT_OLD;
};

class TileEntitySkull;
class BlockSkull : public BlockContainer
{
public:
	BlockSkull(int id, const BM_Material& mat, const String& name);

	/** implement override functions from ITileEntityProvider */
	virtual TileEntity* createNewTileEntity(World* pWorld);

	/** implement override functions from Block */
	virtual bool renderAsNormalBlock() const { return false; }
	virtual void setBlockBoundsBasedOnState(IBlockAccess* piBlockAccess, const BlockPos& pos);
	virtual Box getCollisionBoundingBox(World* pWorld, const BlockPos& pos);
	virtual void onBlockPlacedBy(World* pWorld, const BlockPos& pos, EntityLivingBase* pLiving, ItemStackPtr pStack);
	virtual int idPicked(World* pWorld, const BlockPos& pos);
	virtual int getDamageValue(World* pWorld, const BlockPos& pos);
	virtual int damageDropped(int meta) { return meta; }
	virtual void dropBlockAsItemWithChance(World* pWorld, const BlockPos& pos, int meta, float chance, int fortune) {}
	virtual void onBlockHarvested(World* pWorld, const BlockPos& pos, int meta, EntityPlayer* pPlayer);
	virtual void breakBlock(World* pWorld, const BlockPos& pos, int blockID, int meta);
	virtual int idDropped(int meta, Random& rand, int fortune);

	/** This method attempts to create a wither at the given location and skull	*/
	void makeWither(World* pWorld, const BlockPos& pos, TileEntitySkull* pSkull);

protected:
	bool isSkullType(World* pWorld, const BlockPos& pos, int skullType);
	
};

class BlockWithEffect : public Block
{
public:
	BlockWithEffect(int id, const BM_Material& mat, const String& name);
};

class BlockDirt : public Block
{
public:
	BlockDirt(int id, const BM_Material& mat, const String& name);
};

class BlockNewCrops : public BlockFlower
{
public:
	BlockNewCrops(int id, const BM_Material& mat, const String& name, int seedsId);

	int seedsIdOfCrops = 0;

	/** implement override functions from Block */
	virtual int idDropped(int meta, Random& rand, int fortune);
	virtual void dropBlockAsItemWithChance(World* pWorld, const BlockPos& pos, int meta, float chance, int fortune);

	virtual int getSeedItem() { return seedsIdOfCrops; }
	virtual int getCropItem() { return this->getItemBlockId(); }

protected:
	/** implement override functions from BlockFlower */
	virtual bool canThisPlantGrowOnThisBlockID(int blockID) { return blockID == BLOCK_ID_TILLED_FIELD; }
	bool checkFlowerChange(World* pWorld, const BlockPos& pos);
};

class BlockSlime : public Block
{
public:
	BlockSlime(int id, const BM_Material& mat, const String& name);

	virtual void onFallenUpon(World* pWorld, const BlockPos& pos, Entity* pEntity, float distance);
};

class BlockFruits : public BlockFlower
{
public:
	BlockFruits(int id, const BM_Material& mat, const String& name);

	/** implement override functions from Block */
	virtual int idDropped(int meta, Random& rand, int fortune);
	virtual void dropBlockAsItemWithChance(World* pWorld, const BlockPos& pos, int meta, float chance, int fortune);
	// Called upon block activation (right click on the block.)
	virtual bool onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit);
	virtual bool canThisPlantGrowOnThisBlockID(int blockID);
	virtual void onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborblockID);
	static bool isFruits(int id) { return id >= 1301 && id <= 1312; }
};

class BlockNest : public BlockCarpet
{
public:
	BlockNest(int id, const BM_Material& mat, const String& name);
	virtual void setBlockBoundsBasedOnState(IBlockAccess* pBlockAccess, const BlockPos& pos) { setBounding(); }
	virtual void setBlockBoundsForItemRender() { setBounding(); }
	virtual Box getCollisionBoundingBox(World* pWorld, const BlockPos& pos);
	virtual bool onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit);

protected:
	void setBounding();
};

class BlockStainedGlass : public BlockBreakable
{
public:
	static const int colorType = 16;

	BlockStainedGlass(int id, const BM_Material& mat, const String& name);

	/** implement override functions from Block */
	virtual bool renderAsNormalBlock() const { return false; }
	virtual int quantityDropped(Random& rand) { return 0; }
	virtual bool canSilkHarvest() { return true; }

	/** Takes a dye damage value and returns the block damage value to match. */
	static int getBlockFromDye(int val) { return ~val & 15; }
	/** Takes a block damage value and returns the dye damage value to match. */
	static int getDyeFromBlock(int val) { return ~val & 15; }
};

class BlockTransparency : public BlockGlass
{
public:
	BlockTransparency(int id, const BM_Material& mat, const String& name);

	/** implement override function from Block. */
	virtual void addCollisionBoxesToList(World* pWorld, const BlockPos& pos, const Box& aabb, AABBList& lst, Entity* pEntity);
	virtual bool isCollidable() const { return false; }
};

}

#endif
