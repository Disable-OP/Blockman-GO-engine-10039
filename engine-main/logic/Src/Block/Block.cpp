#include "Block.h"
#include "BlockManager.h"

#include "World/IBlockAccess.h"
#include "World/World.h"
#include "World/RayTracyResult.h"
#include "Item/ItemStack.h"
#include "Item/Item.h"
#include "Stats/StatList.h"
#include "Entity/Enchantment.h"
#include "Entity/EntityItem.h"
#include "Entity/EntityPlayer.h"
#include "Util/Random.h"
#include "Script/Event/LogicScriptEvents.h"
#include "Block/BlockUtil.h"

namespace BLOCKMAN
{

Block::Block(int blockID, const BM_Material& mat, const String& name, bool isOpaqueCube)
	: m_blockID(blockID)
	, m_blockHardness(0)
	, m_blockResistance(0)
	, m_blockConstructorCalled(0)
	, m_enableStats(false)
	, m_needsRandomTick(false)
	, m_isBlockContainer(false)
	, m_blockParticleGravity(1.0f)
	, m_blockMaterial(mat)
	, m_slipperiness(0.6f)
	, m_blockName(name)
	, m_isOpaqueCube(isOpaqueCube)
	, m_speed_attenuation(1.0f)
	, m_immune_fall_damage(false)
{
	setBlockBounds(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
	BlockManager::onBlockCreated(this);
}


Block::~Block()
{}

void Block::initializeBlock() {}

/*
Block setStepSound(StepSound par1StepSound)
{
	stepSound = par1StepSound;
	return this;
}
*/

void Block::setLightOpacity(int lightOpacity)
{
	BlockManager::setLightOpacity(m_blockID, lightOpacity);
}

void Block::setLightValue(float lightValue)
{
	BlockManager::setLightValue(m_blockID, lightValue);
}

void Block::setResistance(float resistance)
{
	m_blockResistance = resistance * 3.0F;
}

bool Block::isBlockSolid(IBlockAccess* pBlockAccess, const BlockPos& pos, int face) 
{
	return pBlockAccess->getBlockMaterial(pos).isSolid(); 
}

int Block::getItemBlockId() const
{
	if (BlockUtil::isBasicBlock(getBlockID()))
	{
		return m_blockID;
	}
	else
	{
		return m_blockID + Item::EC_BLOCK_TO_ITEM_OFFSET;
	}
}

void Block::setHardness(float hardness)
{
	m_blockHardness = hardness;

	if (m_blockResistance < hardness * 5.0f)
	{
		m_blockResistance = hardness * 5.0f;
	}
}

void Block::setBlockBounds(float minx, float miny, float minz, float maxx, float maxy, float maxz)
{
	m_min.x = minx;
	m_min.y = miny;
	m_min.z = minz;
	m_max.x = maxx;
	m_max.y = maxy;
	m_max.z = maxz;
}

void Block::setBlockBounds(const Vector3& min, const Vector3& max)
{
	m_min = min;
	m_max = max;
}

void Block::addCollisionBoxesToList(World* pWorld, const BlockPos& pos, const Box& aabb, AABBList& lst, Entity* pEntity)
{
	Box box = getCollisionBoundingBox(pWorld, pos);

	if (!box.isNull() && aabb.isIntersected(box))
	{
		lst.push_back(box);
	}
}


Box Block::getCollisionBoundingBox(World* pWorld, const BlockPos& pos)
{
	return Box(Vector3(pos) + m_min, Vector3(pos) + m_max);
}

float Block::getPlayerRelativeBlockHardness(EntityPlayer* pEntityPlayer, World* pWorld, const BlockPos& pos)
{
	float hardness = getBlockHardness(pWorld, pos);
	if (hardness == 0.0f)
	{
		// Return a number that big enough
		return 100000;
	}
	if (hardness < 0.0f) {
		return 0.0f;
	}

	float strength = 0.0f;
	if (pEntityPlayer->canHarvestBlock(this)) {
		strength = pEntityPlayer->getCurrentPlayerStrVsBlock(this, true);
		return strength / hardness / 30.0F;
	}
	else {
		strength = pEntityPlayer->getCurrentPlayerStrVsBlock(this, false);
		return strength / hardness / 100.0F;
	}
}

void Block::dropBlockAsItem(World* pWorld, const BlockPos& pos, int damage, int fortune)
{
	dropBlockAsItemWithChance(pWorld, pos, damage, 1.0f, fortune);
}

void Block::dropBlockAsItemWithChance(World* pWorld, const BlockPos& pos, int meta, float chance, int fortune)
{
	if (!pWorld->m_isClient)
	{
		int chance = quantityDroppedWithBonus(fortune, pWorld->m_Rand);

		for (int i = 0; i < chance; ++i)
		{
			if (pWorld->m_Rand.nextFloat() <= chance)
			{
				int dropID = idDropped(meta, pWorld->m_Rand, fortune);

				if (dropID > 0)
				{
					dropBlockAsItem_do(pWorld, pos, LORD::make_shared<ItemStack>(dropID, 1, damageDropped(meta)));
				}
			}
		}
	}
}

void Block::dropBlockAsItem_do(World* pWorld, const BlockPos& pos, ItemStackPtr pStack)
{
	if (!SCRIPT_EVENT::EntityItemSpawnEvent::invoke(pStack->itemID, pStack->itemDamage, "break.block"))
		return;
	if (!pWorld->m_isClient /*&& pWorld->getGameRules().getGameRuleBooleanValue("doTileDrops")*/)
	{
		float randPos = 0.7F;
		float posX = (pWorld->m_Rand.nextFloat() * randPos) + (1.0F - randPos) * 0.5f;
		float posY = (pWorld->m_Rand.nextFloat() * randPos) + (1.0F - randPos) * 0.5f;
		float posZ = (pWorld->m_Rand.nextFloat() * randPos) + (1.0F - randPos) * 0.5f;
		
		Vector3 spawn = Vector3(pos) + Vector3(posX, posY, posZ);
		EntityItem* pEntity = LordNew EntityItem(pWorld, spawn, pStack);
		pEntity->delayBeforeCanPickup = 10;
		pWorld->spawnEntityInWorld(pEntity);
	}
}

void Block::dropXpOnBlockBreak(World* pWorld, const BlockPos& pos, int exp)
{
	/*
	if (!par1World.isRemote)
	{
		while (par5 > 0)
		{
			int var6 = EntityXPOrb.getXPSplit(par5);
			par5 -= var6;
			par1World.spawnEntityInWorld(new EntityXPOrb(par1World, (double)par2 + 0.5D, (double)par3 + 0.5D, (double)par4 + 0.5D, var6));
		}
	}
	*/
}

/*
public float getExplosionResistance(Entity par1Entity)
{
	return blockResistance / 5.0F;
}
*/

RayTraceResult Block::collisionRayTrace(World* pWorld, const BlockPos& pos, const Vector3& start, const Vector3& end)
{
	RayTraceResult result;
	setBlockBoundsBasedOnState(pWorld, pos);
	Vector3 s = start - Vector3(pos);
	Vector3 e = end - Vector3(pos);
	Vector3 vminX = s.getIntermediateWithXValue(e, m_min.x);
	Vector3 vmaxX = s.getIntermediateWithXValue(e, m_max.x);
	Vector3 vminY = s.getIntermediateWithYValue(e, m_min.y);
	Vector3 vmaxY = s.getIntermediateWithYValue(e, m_max.y);
	Vector3 vminZ = s.getIntermediateWithZValue(e, m_min.z);
	Vector3 vmaxZ = s.getIntermediateWithZValue(e, m_max.z);

	if (!isVecInsideYZBounds(vminX))
		vminX = Vector3::INVALID;

	if (!isVecInsideYZBounds(vmaxX))
		vmaxX = Vector3::INVALID;

	if (!isVecInsideXZBounds(vminY))
		vminY = Vector3::INVALID;

	if (!isVecInsideXZBounds(vmaxY))
		vmaxY = Vector3::INVALID;

	if (!isVecInsideXYBounds(vminZ))
		vminZ = Vector3::INVALID;

	if (!isVecInsideXYBounds(vmaxZ))
		vmaxZ = Vector3::INVALID;

	Vector3 vIntersector = Vector3::INVALID;

	if (!vminX.isInvalid() && (vIntersector.isInvalid() || s.squareDistanceTo(vminX) < s.squareDistanceTo(vIntersector)))
		vIntersector = vminX;

	if (!vmaxX.isInvalid() && (vIntersector.isInvalid() || s.squareDistanceTo(vmaxX) < s.squareDistanceTo(vIntersector)))
		vIntersector = vmaxX;

	if (!vminY.isInvalid() && (vIntersector.isInvalid() || s.squareDistanceTo(vminY) < s.squareDistanceTo(vIntersector)))
		vIntersector = vminY;

	if (!vmaxY.isInvalid() && (vIntersector.isInvalid() || s.squareDistanceTo(vmaxY) < s.squareDistanceTo(vIntersector)))
		vIntersector = vmaxY;

	if (!vminZ.isInvalid() && (vIntersector.isInvalid() || s.squareDistanceTo(vminZ) < s.squareDistanceTo(vIntersector)))
		vIntersector = vminZ;

	if (!vmaxZ.isInvalid() && (vIntersector.isInvalid() || s.squareDistanceTo(vmaxZ) < s.squareDistanceTo(vIntersector)))
		vIntersector = vmaxZ;

	if (!vIntersector.isInvalid())
	{
		Facing* facing = NULL;

		if (vIntersector == vminX)
			facing = Facing::WEST;
		
		if (vIntersector == vmaxX)
			facing = Facing::EAST;

		if (vIntersector == vminY)
			facing = Facing::DOWN;

		if (vIntersector == vmaxY)
			facing = Facing::UP;

		if (vIntersector == vminZ)
			facing = Facing::NORTH;

		if (vIntersector == vmaxZ)
			facing = Facing::SOUTH;

		result.set(vIntersector + Vector3(pos), facing, pos);
	}
	return result;
}

bool Block::isVecInsideYZBounds(const Vector3& vec)
{
	return 
		vec.y >= m_min.y && vec.y <= m_max.y &&
		vec.z >= m_min.z && vec.z <= m_max.z;
}

bool Block::isVecInsideXZBounds(const Vector3& vec)
{
	return 
		vec.x >= m_min.x && vec.x <= m_max.x &&
		vec.z >= m_min.z && vec.z <= m_max.z;
}

bool Block::isVecInsideXYBounds(const Vector3& vec)
{
	return
		vec.x >= m_min.x && vec.x <= m_max.x &&
		vec.y >= m_min.y && vec.y <= m_max.y;
}
 
/*
bool Block::canPlaceBlockOnSide(World* pWorld, const BlockPos& pos, int side, ItemStack par6ItemStack)
{
	return canPlaceBlockOnSide(pWorld, x, y, z, side);
}
*/

bool Block::canPlaceBlockOnSide(World* pWorld, const BlockPos& pos, int side)
{
	return canPlaceBlockAt(pWorld, pos);
}

bool Block::canPlaceBlockAt(World* pWorld, const BlockPos& pos)
{
	int blockID = pWorld->getBlockId(pos);
	return blockID == 0 || BlockManager::sBlocks[blockID]->m_blockMaterial.isReplaceable();
}

bool Block::onBlockActivated(World* pWorld, const BlockPos& pos, BLOCKMAN::EntityPlayer* pPlayer, int face, const Vector3& hit)
{
	pPlayer->setActivatedBlock(pos);
	return false;
}

void Block::harvestBlock(World* pWorld, EntityPlayer* pPlayer, const BlockPos& pos, int damage)
{
	pPlayer->addStat(StatList::mineBlockStatArray[m_blockID], 1);
	pPlayer->addExhaustion(0.025F);

	if (canSilkHarvest() && EnchantmentHelper::getSilkTouchModifier(pPlayer))
	{
		ItemStackPtr pStack = createStackedBlock(damage);

		if (pStack)
		{
			dropBlockAsItem_do(pWorld, pos, pStack);
		}
	}
	else
	{
		int fortune = EnchantmentHelper::getFortuneModifier(pPlayer);
		dropBlockAsItem(pWorld, pos, damage, fortune);
	}
}

ItemStackPtr Block::createStackedBlock(int damage)
{
	int subtype = 0;

	if (m_blockID >= 0 && getItemBlockId() < Item::MAX_ITEM_COUNT && Item::itemsList[getItemBlockId()]->getHasSubtypes())
	{
		subtype = damage;
	}

	return LORD::make_shared<ItemStack>(getItemBlockId(), 1, subtype);
}

int Block::quantityDroppedWithBonus(int par1, Random& rand)
{
	return quantityDropped(rand);
}

String Block::getLocalizedName()
{
	return "temp";
}

String Block::getUnlocalizedName()
{
	return "tile." + m_blockName;
}

int Block::getDamageValue(World* pWorld, const BlockPos& pos)
{
	return damageDropped(pWorld->getBlockMeta(pos));
}

bool Block::isAssociatedBlockID(int id1, int id2)
{
	if (id1 == id2)
		return true;

	if (id1 != 0 && id2 != 0 &&
		BlockManager::sBlocks[id1] &&
		BlockManager::sBlocks[id2])
		return BlockManager::sBlocks[id1]->isAssociatedBlockID(id2);

	return false;
}

Box Block::getSelectedBoundingBox(World* par1World, const BlockPos& pos)
{
	return Box(Vector3(pos) + m_min, Vector3(pos) + m_max);
}

bool Block::shouldSideBeRendered(IBlockAccess* piBlockAccess, const BlockPos& pos, int side)
{
	LordAssert(piBlockAccess);
	return
		(pos.y < 0) ? false :						// y < 0 is the baserock.
		((side == BM_FACE_DOWN && m_min.y > 0.0) ? true :
		((side == BM_FACE_UP && m_max.y < 1.0) ? true :
		((side == BM_FACE_NORTH && m_min.z > 0.0) ? true :
		((side == BM_FACE_SOUTH && m_max.z < 1.0) ? true :
		((side == BM_FACE_WEST && m_min.x > 0.0) ? true :
		((side == BM_FACE_EAST && m_max.x < 1.0) ? true :
		!piBlockAccess->isBlockOpaqueCube(pos)))))));
}

}