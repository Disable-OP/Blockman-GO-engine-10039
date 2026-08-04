/********************************************************************
filename: 	Block.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-11-3
*********************************************************************/
#ifndef __BLOCK_HEADER__
#define __BLOCK_HEADER__

#include "BM_Material.h"
#include "BM_TypeDef.h"
#include "BM_Container_def.h"
#include "Util/Random.h"

#include "Core.h"
using namespace LORD;

namespace BLOCKMAN
{

class IBlockAccess;
class World;
class Random;
class Entity;
class RayTraceResult;
class Explosion;
class EntityLivingBase;
class ItemStack;
class EntityPlayer;

class Block : public ObjectAlloc
{
	friend class BlockManager;
	friend class cBlockManager;
public:
	Block(int blockID, const BM_Material& mat, const String& name, bool isOpaqueCube = true);
	virtual ~Block();

	// This method is called on a block after all other blocks gets already created. You can use it to reference and
	// configure something on the block that needs the others ones.
	virtual void initializeBlock();

	// Sets the footstep sound for the block. Returns the object for convenience in constructing.
	//Block setStepSound(StepSound par1StepSound);
	bool checkType(i64 typeMask) const { return (m_type & typeMask) != 0; }

	// Sets how much light is blocked going through this block. Returns the object for convenience in constructing.
	void setLightOpacity(int lightOpacity);

	// Sets the amount of light emitted by a block from 0.0f to 1.0f (converts internally to 0-15). Returns the object for convenience in constructing.
	void setLightValue(float lightValue);

	// Sets the the blocks resistance to explosions. Returns the object for convenience in constructing.
	void setResistance(float resistance);

	// If this block doesn't render as an ordinary block it will return False (examples: signs, buttons, stairs, etc)
	// comment by Zhao Zongsheng: It seems that blocks that is 1m*1m*1m and is not a special block like fluid should be normal blocks
	virtual bool renderAsNormalBlock() const { return true; }

	virtual bool getBlocksMovement(IBlockAccess* pAccess, const BlockPos& pos) const { return !m_blockMaterial.blocksMovement(); }

	int getBlockID() const { return m_blockID;  }
	int getMetaData() const { return 0; }
	int getItemBlockId() const;

	// Sets how many hits it takes to break a block.
	void setHardness(float hardness);
	
	// This method will make the hardness of the block equals to -1, and the block is indestructible.
	void setBlockUnbreakable() { setHardness(-1.0f); }

	// Returns the block hardness at a location. Args: world, pos
	virtual float getBlockHardness(World* pWorld, const BlockPos& pos) const { return m_blockHardness; }

	// Sets whether this block type will receive random update ticks
	void setTickRandomly(bool tickRandomly) { m_needsRandomTick = tickRandomly; }

	// Returns whether or not this block is of a type that needs random ticking. Called for ref-counting purposes by
	// ExtendedBlockStorage in order to broadly cull a chunk from the random chunk update list for efficiency's sake.
	bool getTickRandomly() const { return m_needsRandomTick; }

	bool hasTileEntity() const { return m_isBlockContainer; }

	// if the block is solid or not. solid block can block the water flow
	virtual bool isBlockSolid(IBlockAccess* pBlockAccess, const BlockPos& pos, int face);

	// Sets the bounds of the block.  minX, minY, minZ, maxX, maxY, maxZ
	void setBlockBounds(float minx, float miny, float minz, float maxx, float maxy, float maxz);
	void setBlockBounds(const Vector3& min, const Vector3& max);
	
	// Adds all intersecting collision boxes to a list. (Be sure to only add boxes to the list if they intersect the
	// mask.) Parameters: World, X, Y, Z, mask, list, colliding entity
	virtual void addCollisionBoxesToList(World* pWorld, const BlockPos& pos, const Box& aabb, AABBList& lst, Entity* pEntity);

	// Returns a bounding box from the pool of bounding boxes (this means this box can change after the pool has been cleared to be reused)
	virtual Box getCollisionBoundingBox(World* pWorld, const BlockPos& pos);

	// Is this block (a) opaque and (b) a full 1m cube?  This determines whether or not to render the shared face of two
	// adjacent blocks and also whether the player can attach torches, redstone wire, etc to this block.
	virtual bool isOpaqueCube() const { return m_isOpaqueCube; }

	// Returns whether this block is collideable based on the arguments passed in \n@param par1 block metaData \n@param
	// par2 whether the player right-clicked while holding a boat
	virtual bool canCollideCheck(int metadata, bool onboat) const { return isCollidable(); }

	// Returns if this block is collidable (only used by Fire). Args: x, y, z
	virtual bool isCollidable() const {	return true; }

	// Ticks the block if it's been scheduled
	virtual void updateTick(World* pWorld, const BlockPos& pos, Random& rand) {}

	// Called right before the block is destroyed by a player.  Args: world, x, y, z, metaData
	virtual void onBlockDestroyedByPlayer(World* pWorld, const BlockPos& pos, int meta) {}

	// Lets the block know when one of its neighbor changes. Doesn't know which neighbor changed (coordinates passed are
	// their own) Args: x, y, z, neighbor blockID
	virtual void onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborID) {}

	// How many world ticks before ticking
	virtual int tickRate(World* pWorld) const { return 10; }

	// Called whenever the block is added into the world. Args: world, x, y, z
	virtual void onBlockAdded(World* pWorld, const BlockPos& pos) {}

	// ejects contained items into the world, and notifies neighbours of an update, as appropriate
	virtual void breakBlock(World* pWorld, const BlockPos& pos, int blockID, int meta) {}

	// Returns the quantity of items to drop on block destruction.
	virtual int quantityDropped(Random& rand) { return 1; }

	// Returns the ID of the items to drop on destruction.
	virtual int idDropped(int meta, Random& rand, int fortune) { return getItemBlockId(); }

	// Gets the hardness of block at the given coordinates in the given world, relative to the ability of the given EntityPlayer.
	float getPlayerRelativeBlockHardness(EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos);

	// Drops the specified block items
	void dropBlockAsItem(World* pWorld, const BlockPos& pos, int par5, int par6);

	// Drops the block items with a specified chance of dropping the specified items
	virtual void dropBlockAsItemWithChance(World* pWorld, const BlockPos& pos, int meta, float chance, int fortune);

	// Spawns EntityItem in the world for the given ItemStack if the world is not remote.
	void dropBlockAsItem_do(World* pWorld, const BlockPos& pos, ItemStackPtr pStack);

	// called by spawner, ore, redstoneOre blocks
	void dropXpOnBlockBreak(World* pWorld, const BlockPos& pos, int exp);

	// Determines the damage on the item the block drops. Used in cloth and wood.
	virtual int damageDropped(int meta) { return 0; }

	// Returns how much this block can resist explosions from the passed in entity.
	virtual float getExplosionResistance(Entity* pEntity) { return m_blockResistance / 5.f; }

	// Ray traces through the blocks collision from start vector to end vector returning a ray trace hit. 
	// Args: world, x, y, z, startVec, endVec
	virtual RayTraceResult collisionRayTrace(World* pWorld, const BlockPos& pos, const Vector3& start, const Vector3& end);

	// Checks if a vector is within the Y and Z bounds of the block.
	bool isVecInsideYZBounds(const Vector3& vec);

	// Checks if a vector is within the X and Z bounds of the block.
	bool isVecInsideXZBounds(const Vector3& vec);

	// Checks if a vector is within the X and Y bounds of the block.
	bool isVecInsideXYBounds(const Vector3& vec);

	// Called upon the block being destroyed by an explosion
	virtual void onBlockDestroyedByExplosion(World* pWorld, const BlockPos& pos, Explosion* pExplosion) {}

	bool canPlaceBlockOnSide(World* pWorld, const BlockPos& pos, int side, ItemStackPtr pStack) { return canPlaceBlockOnSide(pWorld, pos, side); }

	// checks to see if you can place this block can be placed on that side of a block: BlockLever overrides
	bool canPlaceBlockOnSide(World* pWorld, const BlockPos& pos, int side);

	// Checks to see if its valid to put this block at the specified coordinates. Args: world, x, y, z
	virtual bool canPlaceBlockAt(World* pWorld, const BlockPos& pos);

	// Called upon block activation (right click on the block.)
	virtual bool onBlockActivated(World* pWorld, const BlockPos& pos, BLOCKMAN::EntityPlayer* pPlayer, int face, const Vector3& hit);

	// Called whenever an entity is walking on top of this block. Args: world, x, y, z, entity
	virtual void onEntityWalking(World* pWorld, const BlockPos& pos, Entity* pEntity) {}

	// Called when a block is placed using its ItemBlock. Args: World, X, Y, Z, side, hitX, hitY, hitZ, block metadata
	virtual int onBlockPlaced(World* pWorld, const BlockPos& pos, int side, const Vector3& hit, int meta)	{ return meta; }

	// Called when the block is clicked by a player. Args: x, y, z, entityPlayer
	virtual void onBlockClicked(World* pWorld, const BlockPos& pos, BLOCKMAN::EntityPlayer* pPlayer) {}

	// Can add to the passed in vector for a movement vector to be applied to the entity. Args: x, y, z, entity, vec3d
	virtual Vector3 velocityToAddToEntity(World* pWorld, const BlockPos& pos, Entity* pEntity, const Vector3& vec) { return vec; }

	// Updates the blocks bounds based on its current state. Args: world, x, y, z
	virtual void setBlockBoundsBasedOnState(IBlockAccess* piBlockAccess, const BlockPos& pos) {}

	// returns the block bounderies minX value
	Vector3 getBlockBoundsMin() const { return m_min; }
	Vector3 getBlockBoundsMax() const { return m_max; }
	float getBlockBoundsMinX() const { return m_min.x; }
	float getBlockBoundsMaxX() const { return m_max.x; }
	float getBlockBoundsMinY() const { return m_min.y; }
	float getBlockBoundsMaxY() const { return m_max.y; }
	float getBlockBoundsMinZ() const { return m_min.z; }
	float getBlockBoundsMaxZ() const { return m_max.z; }
	
	// Returns true if the block is emitting indirect/weak redstone power on the specified side. If isBlockNormalCube
	// returns true, standard redstone propagation rules will apply instead and this will not be called. Args: World, X,
	// Y, Z, side. Note that the side is reversed - eg it is 1 (up) when checking the bottom of the block.
	virtual int isProvidingWeakPower(IBlockAccess* piBlockAccess, const BlockPos& pos, int side)  { return 0; }

	// Can this block provide power. Only wire currently seems to have this change based on its state.
	virtual bool canProvidePower() { return false; }

	// Triggered whenever an entity collides with this block (enters into the block). Args: world, x, y, z, entity
	virtual void onEntityCollidedWithBlock(World* pWorld, const BlockPos& pos, Entity* pEntity) {}

	// Returns true if the block is emitting direct/strong redstone power on the specified side. Args: World, X, Y, Z,
	// side. Note that the side is reversed - eg it is 1 (up) when checking the bottom of the block.
	virtual int isProvidingStrongPower(IBlockAccess* iBlockAccess, const BlockPos& pos, int side) { return 0; }

	// Sets the block's bounds for rendering it as an item
	virtual void setBlockBoundsForItemRender() {}

	// Called when the player destroys a block with an item that can harvest it. (i, j, k) are the coordinates of the
	// block and l is the block's subtype/damage.
	virtual void harvestBlock(World* pWorld, BLOCKMAN::EntityPlayer* pPlayer, const BlockPos& pos, int damage);

	// Return true if a player with Silk Touch can harvest this block directly, and not its normal drops.
	virtual bool canSilkHarvest() {	return renderAsNormalBlock() && !m_isBlockContainer; }

	// Returns an item stack containing a single instance of the current block type. 'damage' is the block's subtype/damage
	// and is ignored for blocks which do not support subtypes. Blocks which cannot be harvested should return null.
	virtual ItemStackPtr createStackedBlock(int damage);
	
	// Returns the usual quantity dropped by the block plus a bonus of 1 to 'i' (inclusive).
	virtual int quantityDroppedWithBonus(int bonus, Random& rand);

	// Can this block stay at this position.  Similar to canPlaceBlockAt except gets checked often with plants.
	virtual bool canBlockStay(World* pWorld, const BlockPos& pos)  { return true; }

	// Called when the block is placed in the world.
	virtual void onBlockPlacedBy(World* pWorld, const BlockPos& pos, EntityLivingBase* pLiving, ItemStackPtr pStack) {}

	// Called after a block is placed
	virtual void onPostBlockPlaced(World* pWorld, const BlockPos& pos, int side) {}

	// Gets the localized name of this block. Used for the statistics page.
	String getLocalizedName();

	// Returns the unlocalized name of this block.
	String getUnlocalizedName();

	// Called when the block receives a BlockEvent - see World.addBlockEvent. By default, passes it on to the tile
	// entity at this location. Args: world, x, y, z, blockID, EventID, event parameter
	virtual bool onBlockEventReceived(World* pWorld, const BlockPos& pos, int id, int param) { return false; }

	// Return the state of blocks statistics flags - if the block is counted for mined and placed.
	bool getEnableStats() {	return m_enableStats; }

	// Disable statistics for the block, the block will no count for mined or placed.
	void disableStats() { m_enableStats = false; }

	// Returns the mobility information of the block, 0 = free, 1 = can't push but can move over, 2 = total immobility and stop pistons
	virtual int getMobilityFlag() {	return m_blockMaterial.getMaterialMobility(); }

	// Block's chance to react to an entity falling on it.
	virtual void onFallenUpon(World* pWorld, const BlockPos& pos, Entity* pEntity, float distance) {}

	// only called by clickMiddleMouseButton , and passed to inventory.setCurrentItem (along with isCreative)
	virtual int idPicked(World* pWorld, const BlockPos& pos) { return m_blockID; }

	// Get the block's damage value (for use with pick block).
	virtual int getDamageValue(World* pWorld, const BlockPos& pos);

	// Called when the block is attempted to be harvested
	virtual void onBlockHarvested(World* pWorld, const BlockPos& pos, int meta, BLOCKMAN::EntityPlayer* pPlayer) {}

	// Called when this block is set (with meta data).
	virtual void onSetBlockIDWithMetaData(World* pWorld, const BlockPos& pos, int par5) {}

	// currently only used by BlockCauldron to incrament meta-data during rain
	virtual void fillWithRain(World* pWorld, const BlockPos& pos) {}

	// Returns true only if block is flowerPot
	virtual bool isFlowerPot() const { return false; }

	virtual bool isUpdateScheduled() const { return true; }

	// Return whether this block can drop from an explosion.
	virtual bool canDropFromExplosion(Explosion* pExplosion) { return true; }

	// Returns true if the given block ID is equivalent to this one. Example: redstoneTorchOn matches itself and
	// redstoneTorchOff, and vice versa. Most blocks only match themselves.
	virtual bool isAssociatedBlockID(int blockID) { return m_blockID == blockID; }

	static bool isAssociatedBlockID(int id1, int id2);

	// If this returns true, then comparators facing away from this block will use the value from
	// getComparatorInputOverride instead of the actual redstone signal strength.
	virtual bool hasComparatorInputOverride() { return false; }

	// If hasComparatorInputOverride returns true, the return value from this is used instead of the redstone signal
	// strength when this block inputs to a comparator.
	virtual int getComparatorInputOverride(World* pWorld, const BlockPos& pos, int side) { return 0; }

	// Returns the bounding box of the wired rectangular prism to render.
	virtual Box getSelectedBoundingBox(World* pWorld, const BlockPos& pos);

	// Returns true if the given side of this block type should be rendered, if the adjacent block is at the given coordinates. 
	virtual bool shouldSideBeRendered(IBlockAccess* piBlockAccess, const BlockPos& pos, int side);

	const BM_Material& getMaterial() const { return m_blockMaterial;  }
	float slipperiness() { return m_slipperiness; }
	float getHardness() const { return m_blockHardness; }
	float getResistance() const { return m_blockResistance; }
	float getParticleGravity() const { return m_blockParticleGravity; }
	float speed_attenuation() const { return m_speed_attenuation; }
	float immune_fall_damage() const { return m_immune_fall_damage; }

protected:

	//unlocalized block name, just for debug
    String m_blockName;

	// no usage now
	i64 m_type = 0;

    // ID of the block.
    int m_blockID = 0;

    // Indicates how many hits it takes to break a block.
    float m_blockHardness = 0;

    // Indicates the blocks resistance to explosions. 
    float m_blockResistance = 0;

    // Never used, set to true when Block's constructor is called through the chain of super()'s.
    bool m_blockConstructorCalled = 0;

    // If this field is true, the block is counted for statistics (mined or placed)
    bool m_enableStats = false;

	// water, fire, snow is true for this field;  stone, dirt e.g. is false
	bool m_isOpaqueCube = true;
	
	// no usage now
    // Flags whether or not this block is of a type that needs random ticking. Ref-counted by BlockStorage in
    // order to broadly cull a chunk from the random chunk update list for efficiency's sake.
    bool m_needsRandomTick = false;

    // true if the Block can contains a Tile Entity, like chest block
    bool m_isBlockContainer = false;

    // minimum and maximum X Y Z for the block bounds (local coordinates)
	Vector3 m_min;
	Vector3 m_max;
	
	// Sound of stepping on the block
    // StepSound stepSound;

	// gravity for particle when digging or breaking
    float m_blockParticleGravity = 1.f;

    // Block material definition.
    BM_Material m_blockMaterial;

    // Determines how much velocity is maintained while moving on top of this block
    float m_slipperiness = 0.6f;

	// 
	float m_speed_attenuation = 1.0f;

	// 
	bool m_immune_fall_damage = false;
};

class ITileEntityProvider
{
public:
	virtual TileEntity* createNewTileEntity(World* pWorld) = 0;
};

}

#endif
