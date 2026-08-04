#include "Blocks.h"
#include "BlockRailLogic.h"

#include "BlockManager.h"

#include "World/World.h"
#include "World/IBlockAccess.h"
#include "World/WorldProvider.h"
#include "WorldGenerator/BiomeGen.h"
#include "WorldGenerator/WorldGenerator.h"
#include "TileEntity/TileEntity.h"
#include "TileEntity/TileEntitys.h"
#include "TileEntity/TileEntityInventory.h"
#include "Item/ItemStack.h"
#include "Item/Item.h"
#include "Item/Items.h"
#include "Inventory/InventoryLargeChest.h"
#include "Inventory/InventoryEnderChest.h"
#include "Entity/EntityLivingBase.h"
#include "Entity/EntityPlayer.h"
#include "Entity/EntityArrow.h"
#include "Entity/EntityItem.h"
#include "Entity/DamageSource.h"
#include "Entity/Enchantment.h"
#include "Entity/EntityTNTPrimed.h"
#include "Entity/Explosion.h"
#include "Stats/StatList.h"
#include "Util/Random.h"
#include "Inventory/Container.h"
#include "Inventory/InventoryPlayer.h"
#include "Common.h"
#include "Script/Event/LogicScriptEvents.h"
#include "Setting/LogicSetting.h"

namespace BLOCKMAN
{

BlockFlower::BlockFlower(int id, const BM_Material& mat, const String& name)
	: Block(id, mat, name, false)
{
	setTickRandomly(true);
	float var3 = 0.2F;
	setBlockBounds(0.5F - var3, 0.0F, 0.5F - var3, 0.5F + var3, var3 * 3.0F, 0.5F + var3);
}

BlockFlower::BlockFlower(int id)
	: Block(id, BM_Material::BM_MAT_plants, "flower", "flower")
{
}

bool BlockFlower::canPlaceBlockAt(World* pWorld, const BlockPos& pos)
{
	return Block::canPlaceBlockAt(pWorld, pos) &&
		canThisPlantGrowOnThisBlockID(pWorld->getBlockId(pos.getNegY()));
}

bool BlockFlower::canThisPlantGrowOnThisBlockID(int blockID)
{
	return 
		blockID == BLOCK_ID_GRASS ||
		blockID == BLOCK_ID_DIRT || 
		blockID == BLOCK_ID_TILLED_FIELD;
}

void BlockFlower::onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborblockID)
{
	Block::onNeighborBlockChange(pWorld, pos, neighborblockID);
	checkFlowerChange(pWorld, pos);
}

void BlockFlower::updateTick(World* pWorld, const BlockPos& pos, Random& rand)
{
	checkFlowerChange(pWorld, pos);
}

bool BlockFlower::checkFlowerChange(World* pWorld, const BlockPos& pos)
{
	bool bret = false;
	if (!canBlockStay(pWorld, pos))
	{
		bret = true;
		dropBlockAsItem(pWorld, pos, pWorld->getBlockMeta(pos), 0);
		pWorld->setBlock(pos, 0, 0, 2);
	}

	return bret;
}

bool BlockFlower::canBlockStay(World* pWorld, const BlockPos& pos)
{
	return 
		// todo.
		//(pWorld->getFullBlockLightValue(pos) >= 8 ||
		pWorld->canBlockSeeTheSky(pos) &&
		canThisPlantGrowOnThisBlockID(pWorld->getBlockId(pos.getNegY()));
}

BlockFluid::BlockFluid(int id, const BM_Material& mat, const String& name)
	: Block(id, mat, name, false)
{
	float var3 = 0.0F;
	float var4 = 0.0F;
	setBlockBounds(0.0F + var4, 0.0F + var3, 0.0F + var4, 1.0F + var4, 1.0F + var3, 1.0F + var4);
	setTickRandomly(true);
}

bool BlockFluid::getBlocksMovement(IBlockAccess* pBlockAccess, const BlockPos& pos) const
{
	return m_blockMaterial != BM_Material::BM_MAT_lava;
}

float BlockFluid::getFluidHeightPercent(int meta)
{
	if (meta >= 8)
	{
		meta = 0;
	}

	return (float)(meta + 1) / 9.0F;
}

int BlockFluid::getFlowDecay(World* pWorld, const BlockPos& pos)
{
	return pWorld->getBlockMaterial(pos) == m_blockMaterial ? pWorld->getBlockMeta(pos) : -1;
}

int BlockFluid::getEffectiveFlowDecay(IBlockAccess* pBlockAccess, const BlockPos& pos)
{
	if (pBlockAccess->getBlockMaterial(pos) != m_blockMaterial)
	{
		return -1;
	}
	else
	{
		int meta = pBlockAccess->getBlockMeta(pos);

		if (meta >= 8)
		{
			meta = 0;
		}

		return meta;
	}
}

Vector3 BlockFluid::getFlowVector(IBlockAccess* pBlockAccess, const BlockPos& pos)
{
	Vector3 retVec = Vector3::ZERO;
	int decay = getEffectiveFlowDecay(pBlockAccess, pos);

	for (int dir = 0; dir < 4; ++dir)
	{
		int x = pos.x;
		int z = pos.z;

		if (dir == 0)
			--x;
		if (dir == 1)
			--z;
		if (dir == 2)
			++x;
		if (dir == 3)
			++z;

		BlockPos pos1(x, pos.y, z);
		int decay1 = getEffectiveFlowDecay(pBlockAccess, pos1);
		int delDecay = 0;

		if (decay1 < 0)
		{
			if (!pBlockAccess->getBlockMaterial(pos1).blocksMovement())
			{
				decay1 = getEffectiveFlowDecay(pBlockAccess, pos.getNegY());

				if (decay1 >= 0)
				{
					delDecay = decay1 - (decay - 8);
					retVec += Vector3(float(x - pos.x) * delDecay, 0.f, float(z - pos.z) * delDecay);
				}
			}
		}
		else if (decay1 >= 0)
		{
			delDecay = decay1 - decay;
			retVec += Vector3(float(x - pos.x) * delDecay, 0.f, float(z - pos.z) * delDecay);
		}
	}

	if (pBlockAccess->getBlockMeta(pos) >= 8)
	{
		bool var13 = false;

		if (var13 || isBlockSolid(pBlockAccess, pos.getNegZ(), 2))
		{
			var13 = true;
		}

		if (var13 || isBlockSolid(pBlockAccess, pos.getPosZ(), 3))
		{
			var13 = true;
		}

		if (var13 || isBlockSolid(pBlockAccess, pos.getNegX(), 4))
		{
			var13 = true;
		}

		if (var13 || isBlockSolid(pBlockAccess, pos.getPosX(), 5))
		{
			var13 = true;
		}

		if (var13 || isBlockSolid(pBlockAccess, pos.getUN(), 2))
		{
			var13 = true;
		}

		if (var13 || isBlockSolid(pBlockAccess, pos.getUS(), 3))
		{
			var13 = true;
		}

		if (var13 || isBlockSolid(pBlockAccess, pos.getUW(), 4))
		{
			var13 = true;
		}

		if (var13 || isBlockSolid(pBlockAccess, pos.getUE(), 5))
		{
			var13 = true;
		}

		if (var13)
		{
			retVec = retVec.normalizedCopy() + Vector3(0.0f, -6.0f, 0.0f);
		}
	}

	retVec.normalize();
	return retVec;
}

int BlockFluid::tickRate(World* pWorld) const
{
	return m_blockMaterial == BM_Material::BM_MAT_water ? 5 : (m_blockMaterial == BM_Material::BM_MAT_lava ? (pWorld->m_provider->hasNoSky ? 10 : 30) : 0);
}

Vector3 BlockFluid::velocityToAddToEntity(World* pWorld, const BlockPos& pos, Entity* pEntity, const Vector3& vec)
{
	Vector3 flow  = getFlowVector(pWorld, pos);
	return flow + vec;
}

bool BlockFluid::isBlockSolid(IBlockAccess* pIBlockAccess, const BlockPos& pos, int side)
{
	const BM_Material& mat = pIBlockAccess->getBlockMaterial(pos);
	return mat == m_blockMaterial ? false :
		(side == 1 ? true :
		(mat == BM_Material::BM_MAT_ice ? false :
		Block::isBlockSolid(pIBlockAccess, pos, side)));
}

double BlockFluid::getFlowDirection(IBlockAccess* pIBlockAccess, const BlockPos& pos, const BM_Material& mat)
{
	Vector3 flowVec = Vector3::ZERO;

	if (mat == BM_Material::BM_MAT_water)
	{
		BlockFluid* water = (BlockFluid*)(BlockManager::waterMoving);
		flowVec = water->getFlowVector(pIBlockAccess, pos);
	}

	if (mat == BM_Material::BM_MAT_lava)
	{
		BlockFluid* lava = (BlockFluid*)(BlockManager::lavaMoving);
		flowVec = lava->getFlowVector(pIBlockAccess, pos);
	}

	return flowVec.x == 0.0f && flowVec.z == 0.0 ? -1000.0 : Math::ATan2(flowVec.z, flowVec.x) - (Math::PI_DIV2);
}

void BlockFluid::checkForHarden(World* pWorld, const BlockPos& pos)
{
	if (pWorld->getBlockId(pos) == m_blockID)
	{
		if (m_blockMaterial == BM_Material::BM_MAT_lava)
		{
			bool flag = false;

			if (flag || pWorld->getBlockMaterial(pos.getNegZ()) == BM_Material::BM_MAT_water)
				flag = true;
			if (flag || pWorld->getBlockMaterial(pos.getPosZ()) == BM_Material::BM_MAT_water)
				flag = true;
			if (flag || pWorld->getBlockMaterial(pos.getNegX()) == BM_Material::BM_MAT_water)
				flag = true;
			if (flag || pWorld->getBlockMaterial(pos.getPosX()) == BM_Material::BM_MAT_water)
				flag = true;
			if (flag || pWorld->getBlockMaterial(pos.getPosY()) == BM_Material::BM_MAT_water)
				flag = true;

			if (flag)
			{
				int meta = pWorld->getBlockMeta(pos);

				if (meta == 0)
				{
					pWorld->setBlock(pos, BLOCK_ID_OBSIDIAN);
				}
				else if (meta <= 4)
				{
					pWorld->setBlock(pos, BLOCK_ID_COBBLE_STONE);
				}

				triggerLavaMixEffects(pWorld, pos);
			}
		}
	}
}

void BlockFluid::triggerLavaMixEffects(World* pWorld, const BlockPos& pos)
{
	float test1 = pWorld->m_Rand.nextFloat() - pWorld->m_Rand.nextFloat();
	if (test1 > 1.0f)
	{
		int a = 5;
		a++; // just silince the complier optimize.
	}
	//pWorld->playSoundEffect(Vector3(pos) + 0.5f, "random.fizz", 0.5F, 2.6F + (pWorld->m_Rand.nextFloat() - pWorld->m_Rand.nextFloat()) * 0.8F);
	pWorld->playSoundEffectByType(Vector3(pos) + 0.5f, ST_Fizz);

	for (int var5 = 0; var5 < 8; ++var5)
	{
		pWorld->spawnParticle("largesmoke", { pos.x + Math::UnitRandom(), pos.y + 1.2f, pos.z + Math::UnitRandom() }, Vector3::ZERO);
	}
}

BlockStationary::BlockStationary(int id, const BM_Material& mat, const String& name)
	: BlockFluid(id, mat, name)
{
	setTickRandomly(false);

	if (mat == BM_Material::BM_MAT_lava)
	{
		setTickRandomly(true);
	}
}

bool BlockStationary::getBlocksMovement(IBlockAccess* pBlockAccess, const BlockPos& pos) const
{
	return m_blockMaterial != BM_Material::BM_MAT_lava;
}

void BlockStationary::onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborblockID)
{
	BlockFluid::onNeighborBlockChange(pWorld, pos, neighborblockID);

	if (pWorld->getBlockId(pos) == m_blockID)
	{
		if (!SCRIPT_EVENT::BlockStationaryNotStationaryEvent::invoke(m_blockID))
		{
			setNotStationary(pWorld, pos);
		}	
	}
}

void BlockStationary::setNotStationary(World* pWorld, const BlockPos& pos)
{
	int var5 = pWorld->getBlockMeta(pos);
	pWorld->setBlock(pos, m_blockID - 1, var5, 2);
	pWorld->scheduleBlockUpdate(pos, m_blockID - 1, tickRate(pWorld));
}

void BlockStationary::updateTick(World* pWorld, const BlockPos& pos, Random& rand)
{
	if (m_blockMaterial == BM_Material::BM_MAT_lava)
	{
		int count = rand.nextInt(3);
		BlockPos pos1 = pos;

		for (int i = 0; i < count; ++i)
		{
			pos1.x += rand.nextInt(3) - 1;
			++pos1.y;
			pos1.z += rand.nextInt(3) - 1;
			int blockID = pWorld->getBlockId(pos1);

			if (blockID == 0)
			{
				if (isFlammable(pWorld, pos1.getNegX()) ||
					isFlammable(pWorld, pos1.getPosX()) ||
					isFlammable(pWorld, pos1.getNegZ()) ||
					isFlammable(pWorld, pos1.getPosZ()) ||
					isFlammable(pWorld, pos1.getNegY()) ||
					isFlammable(pWorld, pos1.getPosY()))
				{
					pWorld->setBlock(pos1, BLOCK_ID_FIRE);
					return;
				}
			}
			else if (BlockManager::sBlocks[blockID]->getMaterial().blocksMovement())
			{
				return;
			}
		}

		if (count == 0)
		{
			for (int i = 0; i < 3; ++i)
			{
				pos1.x = pos.x + rand.nextInt(3) - 1;
				pos1.z = pos.z + rand.nextInt(3) - 1;

				if (pWorld->isAirBlock(pos1.getPosY()) && isFlammable(pWorld, pos1))
				{
					pWorld->setBlock(pos1.getPosY(), BLOCK_ID_FIRE);
				}
			}
		}
	}
}

bool BlockStationary::isFlammable(World* pWorld, const BlockPos& pos)
{
	return pWorld->getBlockMaterial(pos).getCanBurn();
}

BlockFlowing::BlockFlowing(int id, const BM_Material& mat, const String& name)
	: BlockFluid(id, mat, name)
{
	isOptimalFlowDirection[0] = false;
	isOptimalFlowDirection[1] = false;
	isOptimalFlowDirection[2] = false;
	isOptimalFlowDirection[3] = false;
	flowCost[0] = 0;
	flowCost[1] = 0;
	flowCost[2] = 0;
	flowCost[3] = 0;
}

bool BlockFlowing::getBlocksMovement(IBlockAccess* pBlockAccess, const BlockPos& pos) const
{
	return m_blockMaterial != BM_Material::BM_MAT_lava;
}

void BlockFlowing::updateTick(World* pWorld, const BlockPos& pos, Random& rand)
{	
	int decay = getFlowDecay(pWorld, pos);
	i8 var7 = 1;

	if (m_blockMaterial == BM_Material::BM_MAT_lava && !pWorld->m_provider->isHellWorld)
	{
		var7 = 2;
	}

	bool var8 = true;
	int var9 = tickRate(pWorld);
	int var11;

	if (decay > 0)
	{
		i8 var10 = -100;
		numAdjacentSources = 0;
		int var13 = getSmallestFlowDecay(pWorld, pos.getNegX(), var10);
		var13 = getSmallestFlowDecay(pWorld, pos.getPosX(), var13);
		var13 = getSmallestFlowDecay(pWorld, pos.getNegZ(), var13);
		var13 = getSmallestFlowDecay(pWorld, pos.getPosZ(), var13);
		var11 = var13 + var7;

		if (var11 >= 8 || var13 < 0)
		{
			var11 = -1;
		}

		if (getFlowDecay(pWorld, pos.getPosY()) >= 0)
		{
			int var12 = getFlowDecay(pWorld, pos.getPosY());

			if (var12 >= 8)
			{
				var11 = var12;
			}
			else
			{
				var11 = var12 + 8;
			}
		}

		if (numAdjacentSources >= 2 && m_blockMaterial == BM_Material::BM_MAT_water)
		{
			BlockPos pos_down = pos.getNegY();
			if (pWorld->getBlockMaterial(pos_down).isSolid())
			{
				var11 = 0;
			}
			else if (pWorld->getBlockMaterial(pos_down) == m_blockMaterial && pWorld->getBlockMeta(pos_down) == 0)
			{
				var11 = 0;
			}
		}

		if (m_blockMaterial == BM_Material::BM_MAT_lava && decay < 8 && var11 < 8 && var11 > decay && rand.nextInt(4) != 0)
		{
			var9 *= 4;
		}

		if (var11 == decay)
		{
			if (var8)
			{
				updateFlow(pWorld, pos);
			}
		}
		else
		{
			decay = var11;

			if (var11 < 0)
			{
				pWorld->setBlockToAir(pos);
			}
			else
			{
				pWorld->setBlockMetadataWithNotify(pos, var11, 2);
				pWorld->scheduleBlockUpdate(pos, m_blockID, var9);
				pWorld->notifyBlocksOfNeighborChange(pos, m_blockID);
			}
		}
	}
	else
	{
		updateFlow(pWorld, pos);
	}

	BlockPos pos_down = pos.getNegY();
	if (liquidCanDisplaceBlock(pWorld, pos_down))
	{
		if (m_blockMaterial == BM_Material::BM_MAT_lava &&
			pWorld->getBlockMaterial(pos_down) == BM_Material::BM_MAT_water)
		{
			pWorld->setBlock(pos_down, BLOCK_ID_STONE);
			triggerLavaMixEffects(pWorld, pos_down);
			return;
		}

		if (decay >= 8)
		{
			flowIntoBlock(pWorld, pos_down, decay);
		}
		else
		{
			flowIntoBlock(pWorld, pos_down, decay + 8);
		}
	}
	else if (decay >= 0 && (decay == 0 || blockBlocksFlow(pWorld, pos_down)))
	{
		bool* var14 = getOptimalFlowDirections(pWorld, pos);
		var11 = decay + var7;

		if (decay >= 8)
		{
			var11 = 1;
		}

		if (var11 >= 8)
		{
			return;
		}

		if (var14[0])
		{
			flowIntoBlock(pWorld, pos.getNegX(), var11);
		}

		if (var14[1])
		{
			flowIntoBlock(pWorld, pos.getPosX(), var11);
		}

		if (var14[2])
		{
			flowIntoBlock(pWorld, pos.getNegZ(), var11);
		}

		if (var14[3])
		{
			flowIntoBlock(pWorld, pos.getPosZ(), var11);
		}
	}
}

void BlockFlowing::onBlockAdded(World* pWorld, const BlockPos& pos)
{
	BlockFluid::onBlockAdded(pWorld, pos);

	if (pWorld->getBlockId(pos) == m_blockID)
	{
		pWorld->scheduleBlockUpdate(pos, m_blockID, tickRate(pWorld));
	}
}

void BlockFlowing::updateFlow(World* pWorld, const BlockPos& pos)
{
	int var5 = pWorld->getBlockMeta(pos);
	pWorld->setBlock(pos, m_blockID + 1, var5, 2);
}

void BlockFlowing::flowIntoBlock(World* pWorld, const BlockPos& pos, int meta)
{
	if (liquidCanDisplaceBlock(pWorld, pos))
	{
		int var6 = pWorld->getBlockId(pos);

		if (var6 > 0)
		{
			if (m_blockMaterial == BM_Material::BM_MAT_lava)
			{
				triggerLavaMixEffects(pWorld, pos);
			}
			else
			{
				BlockManager::sBlocks[var6]->dropBlockAsItem(pWorld, pos, pWorld->getBlockMeta(pos), 0);
			}
		}

		pWorld->setBlock(pos, m_blockID, meta, 3);
	}
}

int BlockFlowing::calculateFlowCost(World* pWorld, const BlockPos& pos, int accumulate, int preDir)
{
	int var7 = 1000;

	for (int i = 0; i < 4; ++i)
	{
		if ((i != 0 || preDir != 1) &&
			(i != 1 || preDir != 0) &&
			(i != 2 || preDir != 3) && 
			(i != 3 || preDir != 2))
		{
			BlockPos pos1 = pos;

			if (i == 0)
				pos1.x--;
			if (i == 1)
				pos1.x++;
			if (i == 2)
				pos1.z--;
			if (i == 3)
				pos1.z++;

			if (!blockBlocksFlow(pWorld, pos1) &&
				(pWorld->getBlockMaterial(pos1) != m_blockMaterial ||
					pWorld->getBlockMeta(pos1) != 0))
			{
				if (!blockBlocksFlow(pWorld, pos1.getNegY()))
				{
					return accumulate;
				}

				if (accumulate < 4)
				{
					int var12 = calculateFlowCost(pWorld, pos1, accumulate + 1, i);

					if (var12 < var7)
					{
						var7 = var12;
					}
				}
			}
		}
	}

	return var7;
}

bool* BlockFlowing::getOptimalFlowDirections(World* pWorld, const BlockPos& pos)
{
	for (int i = 0; i < 4; ++i)
	{
		flowCost[i] = 1000;
		BlockPos pos1 = pos;

		if (i == 0)
			--pos1.x;
		else if (i == 1)
			++pos1.x;
		else if (i == 2)
			--pos1.z;
		else if (i == 3)
			++pos1.z;

		if (!blockBlocksFlow(pWorld, pos1) &&
			(pWorld->getBlockMaterial(pos1) != m_blockMaterial ||
				pWorld->getBlockMeta(pos1) != 0))
		{
			if (blockBlocksFlow(pWorld, pos1.getNegY()))
			{
				flowCost[i] = calculateFlowCost(pWorld, pos1, 1, i);
			}
			else
			{
				flowCost[i] = 0;
			}
		}
	}

	int cost = flowCost[0];

	for (int i = 1; i < 4; ++i)
	{
		if (flowCost[i] < cost)
		{
			cost = flowCost[i];
		}
	}

	for (int i = 0; i < 4; ++i)
	{
		isOptimalFlowDirection[i] = flowCost[i] == cost;
	}

	return isOptimalFlowDirection;
}

bool BlockFlowing::blockBlocksFlow(World* pWorld, const BlockPos& pos)
{
	int blockID = pWorld->getBlockId(pos);

	if (blockID != BLOCK_ID_DOOR_WOOD &&
		blockID != BLOCK_ID_DOOR_IRON &&
		blockID != BLOCK_ID_SIGN_POST &&
		blockID != BLOCK_ID_LADDER &&
		blockID != BLOCK_ID_REED)
	{
		if (blockID == 0)
		{
			return false;
		}
		else
		{
			const BM_Material& var6 = BlockManager::sBlocks[blockID]->getMaterial();
			return var6 == BM_Material::BM_MAT_portal ? true : var6.blocksMovement();
		}
	}
	else
	{
		return true;
	}
}

int BlockFlowing::getSmallestFlowDecay(World* pWorld, const BlockPos& pos, int currentDecay)
{
	int decay = getFlowDecay(pWorld, pos);

	if (decay < 0)
	{
		return currentDecay;
	}
	else
	{
		if (decay == 0)
		{
			++numAdjacentSources;
		}

		if (decay >= 8)
		{
			decay = 0;
		}

		return currentDecay >= 0 && decay >= currentDecay ? currentDecay : decay;
	}
}

bool BlockFlowing::liquidCanDisplaceBlock(World* pWorld, const BlockPos& pos)
{
	const BM_Material& mat = pWorld->getBlockMaterial(pos);
	return mat == m_blockMaterial ? false : (mat == BM_Material::BM_MAT_lava ? false : !blockBlocksFlow(pWorld, pos));
}

BlockMushroom::BlockMushroom(int id, const BM_Material& mat, const String& name)
	: BlockFlower(id, mat, name)
{
	float var2 = 0.2F;
	setBlockBounds(0.5F - var2, 0.0F, 0.5F - var2, 0.5F + var2, var2 * 2.0F, 0.5F + var2);
	setTickRandomly(true);
}

void BlockMushroom::updateTick(World* pWorld, const BlockPos& pos, Random& rand)
{
	if (rand.nextInt(25) == 0)
	{
		int extend = 4;
		int var7 = 5;
	
		for (int x = pos.x - extend; x <= pos.x + extend; ++x)
		{
			for (int z = pos.z - extend; z <= pos.z + extend; ++z)
			{
				for (int y = pos.y - 1; y <= pos.y + 1; ++y)
				{
					if (pWorld->getBlockId(BlockPos(x, y, z)) == m_blockID)
					{
						--var7;

						if (var7 <= 0)
						{
							return;
						}
					}
				}
			}
		}

		BlockPos pos1;
		BlockPos pos2 = pos;
		pos1.x = pos.x + rand.nextInt(3) - 1;
		pos1.y = pos.y + rand.nextInt(2) - rand.nextInt(2);
		pos1.z = pos.z + rand.nextInt(3) - 1;

		for (int i = 0; i < 4; ++i)
		{
			if (pWorld->isAirBlock(pos1) && canBlockStay(pWorld, pos1))
			{
				pos2 = pos1;
			}

			pos1.x = pos2.x + rand.nextInt(3) - 1;
			pos1.y = pos2.y + rand.nextInt(2) - rand.nextInt(2);
			pos1.z = pos2.z + rand.nextInt(3) - 1;
		}

		if (pWorld->isAirBlock(pos1) && canBlockStay(pWorld, pos1))
		{
			pWorld->setBlock(pos1, m_blockID, 0, 2);
		}
	}
}

bool BlockMushroom::canPlaceBlockAt(World* pWorld, const BlockPos& pos)
{
	return BlockFlower::canPlaceBlockAt(pWorld, pos) && canBlockStay(pWorld, pos);
}

bool BlockMushroom::canBlockStay(World* pWorld, const BlockPos& pos)
{
	if (pos.y >= 0 && pos.y < 256)
	{
		int blockID = pWorld->getBlockId(pos.getNegY());
		return blockID == BLOCK_ID_MYCELIUM ||
			// todo.
			//pWorld->getFullBlockLightValue(pos) < 13 &&
			canThisPlantGrowOnThisBlockID(blockID);
	}
	else
	{
		return false;
	}
}

bool BlockMushroom::fertilizeMushroom(World* pWorld, const BlockPos& pos, Random& rand)
{
	int meta = pWorld->getBlockMeta(pos);
	pWorld->setBlockToAir(pos);
	WorldGenBigMushroom* big_mushroom_gen = NULL;

	if (m_blockID == BLOCK_ID_MUSHROOM_BROWN)
	{
		big_mushroom_gen = LordNew WorldGenBigMushroom(0);
	}
	else if (m_blockID == BLOCK_ID_MUSHROOM_RED)
	{
		big_mushroom_gen = LordNew WorldGenBigMushroom(1);
	}

	if (big_mushroom_gen != NULL && big_mushroom_gen->generate(pWorld, rand, pos.x, pos.y, pos.z))
	{
		LordSafeDelete(big_mushroom_gen);
		return true;
	}
	else
	{
		pWorld->setBlock(pos, m_blockID, meta, 3);
		return false;
	}
}

bool BlockMushroom::canThisPlantGrowOnThisBlockID(int blockID)
{
	return BlockManager::sOpaqueCubeLookup[blockID] != 0;
}

BlockCactus::BlockCactus(int id, const BM_Material& mat, const String& name)
	: Block(id, mat, name, false)
{
	setTickRandomly(true);
}

void BlockCactus::updateTick(World* pWorld, const BlockPos& pos, Random& rand)
{
	if (pWorld->isAirBlock(pos.getPosY()))
	{
		int i = 1;
		for (; pWorld->getBlockId(BlockPos(pos.x, pos.y - i, pos.z)) == m_blockID; ++i)
		{
			;
		}

		if (i < 3)
		{
			int meta = pWorld->getBlockMeta(pos);

			if (meta == 15)
			{
				pWorld->setBlock(pos.getPosY(), m_blockID);
				pWorld->setBlockMetadataWithNotify(pos, 0, 4);
				onNeighborBlockChange(pWorld, pos.getPosY(), m_blockID);
			}
			else
			{
				pWorld->setBlockMetadataWithNotify(pos, meta + 1, 4);
			}
		}
	}
}

Box BlockCactus::getCollisionBoundingBox(World* pWorld, const BlockPos& pos)
{
	float bound = 0.0625F;
	return Box(pos.x + bound, (float)pos.y, pos.z + bound, pos.x + 1 - bound, pos.y + 1 - bound, pos.z + 1 - bound);
}

bool BlockCactus::canPlaceBlockAt(World* pWorld, const BlockPos& pos)
{
	return !Block::canPlaceBlockAt(pWorld, pos) ? false : canBlockStay(pWorld, pos);
}

void BlockCactus::onNeighborBlockChange(World* pWorld, const BlockPos& pos, int blockID)
{
	if (!canBlockStay(pWorld, pos))
	{
		pWorld->destroyBlock(pos, true);
	}
}

bool BlockCactus::canBlockStay(World* pWorld, const BlockPos& pos)
{
	if (pWorld->getBlockMaterial(pos.getNegX()).isSolid())
	{
		return false;
	}
	else if (pWorld->getBlockMaterial(pos.getPosX()).isSolid())
	{
		return false;
	}
	else if (pWorld->getBlockMaterial(pos.getNegZ()).isSolid())
	{
		return false;
	}
	else if (pWorld->getBlockMaterial(pos.getPosZ()).isSolid())
	{
		return false;
	}
	else
	{
		int var5 = pWorld->getBlockId(pos.getNegY());
		return var5 == BLOCK_ID_CACTUS || var5 == BLOCK_ID_SAND;
	}
}

void BlockCactus::onEntityCollidedWithBlock(World* pWorld, const BlockPos& pos, Entity* pEntity)
{
	LordAssert(pEntity);
	pEntity->attackEntityFrom(DamageSource::cactus, 1.0F);
}

BlockDeadBush::BlockDeadBush(int id, const BM_Material& mat, const String& name)
	: BlockFlower(id, mat, name)
{
	float bound = 0.4F;
	setBlockBounds(0.5F - bound, 0.0F, 0.5F - bound, 0.5F + bound, 0.8F, 0.5F + bound);
}

void BlockDeadBush::harvestBlock(World* pWorld, EntityPlayer* pPlayer, const BlockPos& pos, int damage)
{
	if (!pWorld->m_isClient && 
		pPlayer->getCurrentEquippedItem() &&
		pPlayer->getCurrentEquippedItem()->itemID == Item::shears->itemID)
	{
		pPlayer->addStat(StatList::mineBlockStatArray[m_blockID], 1);
		dropBlockAsItem_do(pWorld, pos, LORD::make_shared<ItemStack>(m_blockID, 1, damage));
	}
	else
	{
		Block::harvestBlock(pWorld, pPlayer, pos, damage);
	}
}

bool BlockDeadBush::canThisPlantGrowOnThisBlockID(int blockID)
{
	return blockID == BLOCK_ID_SAND;
}

BlockTallGrass::BlockTallGrass(int id, const BM_Material& mat, const String& name)
	: BlockFlower(id, mat, name)
{
	float bound = 0.4F;
	setBlockBounds(0.5F - bound, 0.0F, 0.5F - bound, 0.5F + bound, 0.8F, 0.5F + bound);
}

int BlockTallGrass::idDropped(int meta, Random& rand, int fortune)
{
	return rand.nextInt(8) == 0 ? ITEM_ID_SEED : -1;
}

int BlockTallGrass::quantityDroppedWithBonus(int fortune, Random& rand)
{
	return 1 + rand.nextInt(fortune * 2 + 1);
}

void BlockTallGrass::harvestBlock(World* pWorld, EntityPlayer* pPlayer, const BlockPos& pos, int damage)
{
	if (!pWorld->m_isClient &&
		pPlayer->getCurrentEquippedItem() && 
		pPlayer->getCurrentEquippedItem()->itemID == Item::shears->itemID)
	{
		pPlayer->addStat(StatList::mineBlockStatArray[m_blockID], 1);
		dropBlockAsItem_do(pWorld, pos, LORD::make_shared<ItemStack>(m_blockID, 1, damage));
	}
	else
	{
		Block::harvestBlock(pWorld, pPlayer, pos, damage);
	}
}

int BlockTallGrass::getDamageValue(World* pWorld, const BlockPos& pos)
{
	return pWorld->getBlockMeta(pos);
}

BlockRotatedPillar::BlockRotatedPillar(int id, const BM_Material& mat, const String& name)
	: Block(id, mat, name)
{
}

int BlockRotatedPillar::onBlockPlaced(World* pWorld, const BlockPos& pos, int side, const Vector3& hit, int meta)
{
	int var10 = meta & 3;
	i8 var11 = 0;

	switch (side)
	{
	case 0:
	case 1:
		var11 = 0;
		break;

	case 2:
	case 3:
		var11 = 8;
		break;

	case 4:
	case 5:
		var11 = 4;
	}

	return var10 | var11;
}

ItemStackPtr BlockRotatedPillar::createStackedBlock(int damage)
{
	return LORD::make_shared<ItemStack>(m_blockID, 1, damageDropped(damage));
}

BlockLogBase::BlockLogBase(int id, const BM_Material& mat, const String& name)
	: BlockRotatedPillar(id, mat, name)
{
}

void BlockLogBase::breakBlock(World* pWorld, const BlockPos& pos, int blockID, int meta)
{
	if (SCRIPT_EVENT::BlockLeavesBreakEvent::invoke(m_blockID))
	{
		int extend = 4;
		int range = extend + 1;

		if (pWorld->checkChunksExist(pos - range, pos + range))
		{
			for (int x = -extend; x <= extend; ++x)
			{
				for (int y = -extend; y <= extend; ++y)
				{
					for (int z = -extend; z <= extend; ++z)
					{
						BlockPos pos1 = pos + BlockPos(x, y, z);
						int blockID = pWorld->getBlockId(pos1);

						if (blockID == BLOCK_ID_LEAVES)
						{
							int meta = pWorld->getBlockMeta(pos1);

							if ((meta & 8) == 0)
							{
								pWorld->setBlockMetadataWithNotify(pos1, meta | 8, 4);
							}
						}
					}
				}
			}
		}
	}
}

StringArray BlockLog1::woodType = { "oak", "spruce", "birch", "jungle" };

BlockLog1::BlockLog1(int id, const BM_Material& mat, const String& name)
	: BlockLogBase(id, mat, name)
{
}

StringArray BlockLog2::woodType = { "acacia", "big_oak" };

BlockLog2::BlockLog2(int id, const BM_Material& mat, const String& name)
	: BlockLogBase(id, mat, name)
{
}

BlockLeavesBase::BlockLeavesBase(int id, const BM_Material& mat, const String& name, bool isOpaqueCube)
	: Block(id, mat, name, isOpaqueCube)
{
}

BlockLeaves::BlockLeaves(int id, const BM_Material& mat, const String& name)
	: BlockLeavesBase(id, mat, name, true)
{
	setTickRandomly(true);
	adjacentTreeBlocks = NULL;
}

void BlockLeaves::breakBlock(World* pWorld, const BlockPos& pos, int blockID, int meta)
{
	if (SCRIPT_EVENT::BlockLeavesBreakEvent::invoke(m_blockID))
	{
		int extend = 1;
		int range = extend + 1;

		if (pWorld->checkChunksExist(pos - range, pos + range))
		{
			for (int x = -extend; x <= extend; ++x)
			{
				for (int y = -extend; y <= extend; ++y)
				{
					for (int z = -extend; z <= extend; ++z)
					{
						BlockPos pos1 = pos + BlockPos(x, y, z);
						int blockID1 = pWorld->getBlockId(pos1);

						if (blockID1 == BLOCK_ID_LEAVES)
						{
							int meta1 = pWorld->getBlockMeta(pos1);
							pWorld->setBlockMetadataWithNotify(pos1, meta1 | 8, 4);
						}
					}
				}
			}
		}
	}
}

void BlockLeaves::updateTick(World* pWorld, const BlockPos& pos, Random& rand)
{
	if (!pWorld->m_isClient)
	{
		int meta = pWorld->getBlockMeta(pos);

		if ((meta & 8) != 0 && (meta & 4) == 0)
		{
			int extend = 4;
			int range = extend + 1;
			int width = 32;
			int quad = width * width;
			int offset = width / 2;

			if (adjacentTreeBlocks == NULL)
			{
				adjacentTreeBlocks = (int*)LordMalloc(sizeof(int)*width * width * width);
			}

			if (pWorld->checkChunksExist(pos - range, pos + range))
			{
				for (int x = -extend; x <= extend; ++x)
				{
					for (int y = -extend; y <= extend; ++y)
					{
						for (int z = -extend; z <= extend; ++z)
						{
							BlockPos pos1 = pos + BlockPos(x, y, z);
							int blockID = pWorld->getBlockId(pos1);

							if (BlockLogBase::isWood(blockID))
							{
								adjacentTreeBlocks[(x + offset) * quad + (y + offset) * width + z + offset] = 0;
							}
							else if (blockID == BLOCK_ID_LEAVES)
							{
								adjacentTreeBlocks[(x + offset) * quad + (y + offset) * width + z + offset] = -2;
							}
							else
							{
								adjacentTreeBlocks[(x + offset) * quad + (y + offset) * width + z + offset] = -1;
							}
						}
					}
				}

				for (int side = 1; side <= 4; ++side)
				{
					for (int x = -extend; x <= extend; ++x)
					{
						for (int y = -extend; y <= extend; ++y)
						{
							for (int z = -extend; z <= extend; ++z)
							{
								if (adjacentTreeBlocks[(x + offset) * quad + (y + offset) * width + z + offset] == side - 1)
								{
									if (adjacentTreeBlocks[(x + offset - 1) * quad + (y + offset) * width + z + offset] == -2)
									{
										adjacentTreeBlocks[(x + offset - 1) * quad + (y + offset) * width + z + offset] = side;
									}

									if (adjacentTreeBlocks[(x + offset + 1) * quad + (y + offset) * width + z + offset] == -2)
									{
										adjacentTreeBlocks[(x + offset + 1) * quad + (y + offset) * width + z + offset] = side;
									}

									if (adjacentTreeBlocks[(x + offset) * quad + (y + offset - 1) * width + z + offset] == -2)
									{
										adjacentTreeBlocks[(x + offset) * quad + (y + offset - 1) * width + z + offset] = side;
									}

									if (adjacentTreeBlocks[(x + offset) * quad + (y + offset + 1) * width + z + offset] == -2)
									{
										adjacentTreeBlocks[(x + offset) * quad + (y + offset + 1) * width + z + offset] = side;
									}

									if (adjacentTreeBlocks[(x + offset) * quad + (y + offset) * width + (z + offset - 1)] == -2)
									{
										adjacentTreeBlocks[(x + offset) * quad + (y + offset) * width + (z + offset - 1)] = side;
									}

									if (adjacentTreeBlocks[(x + offset) * quad + (y + offset) * width + z + offset + 1] == -2)
									{
										adjacentTreeBlocks[(x + offset) * quad + (y + offset) * width + z + offset + 1] = side;
									}
								}
							}
						}
					}
				}
			}

			int adjacent = adjacentTreeBlocks[offset * quad + offset * width + offset];

			if (adjacent >= 0)
			{
				pWorld->setBlockMetadataWithNotify(pos, meta & -9, 4);
			}
			else
			{
				removeLeaves(pWorld, pos);
			}
		}
	}
}

void BlockLeaves::removeLeaves(World* pWorld, const BlockPos& pos)
{
	dropBlockAsItem(pWorld, pos, pWorld->getBlockMeta(pos), 0);
	pWorld->setBlockToAir(pos);
}

int BlockLeaves::idDropped(int meta, Random& pRandom, int fortune)
{
	return BLOCK_ID_SAPLING;
}

void BlockLeaves::dropBlockAsItemWithChance(World* pWorld, const BlockPos& pos, int meta, float chance, int fortune)
{
	if (!pWorld->m_isClient)
	{
		int var8 = 20;

		if ((meta & 3) == 3)
		{
			var8 = 40;
		}

		if (fortune > 0)
		{
			var8 -= 2 << fortune;

			if (var8 < 10)
			{
				var8 = 10;
			}
		}

		if (pWorld->m_Rand.nextInt(var8) == 0)
		{
			int id = idDropped(meta, pWorld->m_Rand, fortune);
			dropBlockAsItem_do(pWorld, pos, LORD::make_shared<ItemStack>(id, 1, damageDropped(meta)));
		}

		var8 = 200;

		if (fortune > 0)
		{
			var8 -= 10 << fortune;

			if (var8 < 40)
			{
				var8 = 40;
			}
		}

		if ((meta & 3) == 0 && pWorld->m_Rand.nextInt(var8) == 0)
		{
			dropBlockAsItem_do(pWorld, pos, LORD::make_shared<ItemStack>(Item::appleRed, 1, 0));
		}
	}
}

void BlockLeaves::harvestBlock(World* pWorld, EntityPlayer* pPlayer, const BlockPos& pos, int damage)
{
	if (!pWorld->m_isClient &&
		pPlayer->getCurrentEquippedItem() &&
		pPlayer->getCurrentEquippedItem()->itemID == Item::shears->itemID)
	{
		pPlayer->addStat(StatList::mineBlockStatArray[m_blockID], 1);
		dropBlockAsItem_do(pWorld, pos, LORD::make_shared<ItemStack>(m_blockID, 1, damage & 3));
	}

	Block::harvestBlock(pWorld, pPlayer, pos, damage);
}

BlockSnow::BlockSnow(int id, const BM_Material& mat, const String& name)
	: Block(id, mat, name, false)
{
	setBlockBounds(0.0F, 0.0F, 0.0F, 1.0F, 0.125F, 1.0F);
	setTickRandomly(true);
	setBlockBoundsForSnowDepth(0);
}

Box BlockSnow::getCollisionBoundingBox(World* pWorld, const BlockPos& pos)
{
	return Box();
/*
	int meta = pWorld->getBlockMeta(pos) & 7;
	float thickness = 0.125F;
	return Box(Vector3(pos) + m_min, Vector3(pos) + Vector3(m_max.x, meta*thickness, m_max.z));*/
}

void BlockSnow::setBlockBoundsBasedOnState(IBlockAccess* pBlockAccess, const BlockPos& pos)
{
	setBlockBoundsForSnowDepth(pBlockAccess->getBlockMeta(pos));
}

bool BlockSnow::canPlaceBlockAt(World* pWorld, const BlockPos& pos)
{
	BlockPos pos1 = pos.getNegY();
	int blockID_down = pWorld->getBlockId(pos1);
	return blockID_down == 0 ? false : 
		(blockID_down == m_blockID &&
		(pWorld->getBlockMeta(pos1) & 7) == 7 ? true :
			(blockID_down != BLOCK_ID_LEAVES &&
			!BlockManager::sBlocks[blockID_down]->isOpaqueCube() ? false :
				pWorld->getBlockMaterial(pos1).blocksMovement()));
}

void BlockSnow::onNeighborBlockChange(World* pWorld, const BlockPos& pos, int blockID)
{
	canSnowStay(pWorld, pos);
}

void BlockSnow::harvestBlock(World* pWorld, EntityPlayer* pPlayer, const BlockPos& pos, int damage)
{
	int itemID = Item::snowball->itemID;
	int var8 = damage & 7;
	dropBlockAsItem_do(pWorld, pos, LORD::make_shared<ItemStack>(itemID, var8 + 1, 0));
	pWorld->setBlockToAir(pos);
	pPlayer->addStat(StatList::mineBlockStatArray[m_blockID], 1);
}

void BlockSnow::updateTick(World* pWorld, const BlockPos& pos, Random& rand)
{
	// todo.
	//if (pWorld->getSavedLightValue(BLOCK_LIGHT_VALUE, pos) > 11)
	if(false)
	{
		dropBlockAsItem(pWorld, pos, pWorld->getBlockMeta(pos), 0);
		pWorld->setBlockToAir(pos);
	}
}

void BlockSnow::addCollisionBoxesToList(World * pWorld, const BlockPos & pos, const Box & aabb, AABBList & lst, Entity * pEntity)
{
	return;
}

void BlockSnow::setBlockBoundsForSnowDepth(int meta)
{
	int meta_mod = meta & 7;
	float thickness = (float)(2 * (1 + meta_mod)) / 16.0F;
	setBlockBounds(0.0F, 0.0F, 0.0F, 1.0F, thickness, 1.0F);
}

bool BlockSnow::canSnowStay(World* pWorld, const BlockPos& pos)
{
	if (!canPlaceBlockAt(pWorld, pos))
	{
		dropBlockAsItem(pWorld, pos, pWorld->getBlockMeta(pos), 0);
		pWorld->setBlockToAir(pos);
		return false;
	}
	else
	{
		return true;
	}
}

bool BlockSand::fallInstantly = false;

BlockSand::BlockSand(int id, const BM_Material& mat, const String& name)
	: Block(id, mat, name)
{
}

void BlockSand::onBlockAdded(World* pWorld, const BlockPos& pos)
{
	pWorld->scheduleBlockUpdate(pos, m_blockID, tickRate(pWorld));
}

void BlockSand::onNeighborBlockChange(World* pWorld, const BlockPos& pos, int blockID)
{
	pWorld->scheduleBlockUpdate(pos, m_blockID, tickRate(pWorld));
}

void BlockSand::updateTick(World* pWorld, const BlockPos& pos, Random& rand)
{
	if (!pWorld->m_isClient)
	{
		tryToFall(pWorld, pos);
	}
}

void BlockSand::tryToFall(World* pWorld, const BlockPos& pos)
{
	BlockPos pos1 = pos.getNegY();
	if (canFallBelow(pWorld, pos1) && pos.y >= 0)
	{
		int range = 32;

		if (!fallInstantly && pWorld->checkChunksExist(pos - range, pos + range))
		{
			if (!pWorld->m_isClient)
			{
				// modify by zhouyou.
				// EntityFallingSand var9 = new EntityFallingSand(pWorld, (double)((float)x + 0.5F), (double)((float)y + 0.5F), (double)((float)z + 0.5F), blockID, pWorld->getBlockMetadata(x, y, z));
				// onStartFalling(var9);
				// pWorld->spawnEntityInWorld(var9);
			}
		}
		else
		{
			pWorld->setBlockToAir(pos);

			while (canFallBelow(pWorld, pos1) && pos1.y > 0)
			{
				--pos1.y;
			}

			if (pos1.y > 0)
			{
				pWorld->setBlock(pos1, m_blockID);
			}
		}
	}
}

bool BlockSand::canFallBelow(World* pWorld, const BlockPos& pos)
{
	int blockID = pWorld->getBlockId(pos);

	if (blockID == 0)
	{
		return true;
	}
	else if (blockID == BLOCK_ID_FIRE)
	{
		return true;
	}
	else
	{
		const BM_Material& mat = BlockManager::sBlocks[blockID]->getMaterial();
		return mat == BM_Material::BM_MAT_water ? true : mat == BM_Material::BM_MAT_lava;
	}
}


BlockVine::BlockVine(int id, const BM_Material& mat, const String& name)
	: Block(id, mat, name, false)
{
	setTickRandomly(true);
}

Box BlockVine::getCollisionBoundingBox(World* pWorld, const BlockPos& pos)
{
	return {};
}

bool BlockVine::canPlaceBlockOnSide(World* pWorld, const BlockPos& pos, int side)
{
	switch (side)
	{
	case BM_FACE_UP:
		return canBePlacedOn(pWorld->getBlockId(pos + offsetForSide[side]));

	case BM_FACE_NORTH:
	case BM_FACE_SOUTH:
	case BM_FACE_WEST:
	case BM_FACE_EAST:
		return canBePlacedOn(pWorld->getBlockId(pos + offsetForSide[oppositeSide[side]]));

	default:
		return false;
	}
}

void BlockVine::setBlockBoundsBasedOnState(IBlockAccess* pBlockAccess, const BlockPos& pos)
{
	float bound = 0.0625F;
	int meta = pBlockAccess->getBlockMeta(pos);
	float minx = 1.0F;
	float miny = 1.0F;
	float minz = 1.0F;
	float maxx = 0.0F;
	float maxy = 0.0F;
	float maxz = 0.0F;
	bool flag = meta > 0;

	if ((meta & 2) != 0)
	{
		maxx = Math::Max(maxx, bound);
		minx = 0.0F;
		miny = 0.0F;
		maxy = 1.0F;
		minz = 0.0F;
		maxz = 1.0F;
		flag = true;
	}

	if ((meta & 8) != 0)
	{
		minx = Math::Min(minx, 1.f - bound);
		maxx = 1.0F;
		miny = 0.0F;
		maxy = 1.0F;
		minz = 0.0F;
		maxz = 1.0F;
		flag = true;
	}

	if ((meta & 4) != 0)
	{
		maxz = Math::Max(maxz, bound);
		minz = 0.0F;
		minx = 0.0F;
		maxx = 1.0F;
		miny = 0.0F;
		maxy = 1.0F;
		flag = true;
	}

	if ((meta & 1) != 0)
	{
		minz = Math::Min(minz, 1.f - bound);
		maxz = 1.0F;
		minx = 0.0F;
		maxx = 1.0F;
		miny = 0.0F;
		maxy = 1.0F;
		flag = true;
	}

	if (!flag && canBePlacedOn(pBlockAccess->getBlockId(pos.getPosY())))
	{
		miny = Math::Min(miny, 0.9375F);
		maxy = 1.0F;
		minx = 0.0F;
		maxx = 1.0F;
		minz = 0.0F;
		maxz = 1.0F;
	}

	setBlockBounds(minx, miny, minz, maxx, maxy, maxz);
}

bool BlockVine::canBePlacedOn(int blockID)
{
	if (blockID == 0)
	{
		return false;
	}
	else
	{
		Block* pBlock = BlockManager::sBlocks[blockID];
		return pBlock->renderAsNormalBlock() && pBlock->getMaterial().blocksMovement();
	}
}

bool BlockVine::canVineStay(World* pWorld, const BlockPos& pos)
{
	int meta = pWorld->getBlockMeta(pos);
	int meta2 = meta;
	BlockPos pos1 = pos.getPosY();

	if (meta > 0)
	{
		for (int i = 0; i <= 3; ++i)
		{
			int meta_dir = 1 << i;

			if ((meta & meta_dir) != 0 && !canBePlacedOn(pWorld->getBlockId(pos + dir_offset[i])) &&
				(pWorld->getBlockId(pos1) != m_blockID || (pWorld->getBlockMeta(pos1) & meta_dir) == 0))
			{
				meta2 &= ~meta_dir;
			}
		}
	}

	if (meta2 == 0 && !canBePlacedOn(pWorld->getBlockId(pos1)))
	{
		return false;
	}
	else
	{
		if (meta2 != meta)
		{
			pWorld->setBlockMetadataWithNotify(pos, meta2, 2);
		}

		return true;
	}
}

void BlockVine::onNeighborBlockChange(World* pWorld, const BlockPos& pos, int blockID)
{
	if (!pWorld->m_isClient && !canVineStay(pWorld, pos))
	{
		dropBlockAsItem(pWorld, pos, pWorld->getBlockMeta(pos), 0);
		pWorld->setBlockToAir(pos);
	}
}

void BlockVine::updateTick(World* pWorld, const BlockPos& pos, Random& rand)
{
	if (!pWorld->m_isClient && pWorld->m_Rand.nextInt(4) == 0)
	{
		int extend = 4;
		int range = 5;
		bool flag = false;

		for (int x = pos.x - extend; x <= pos.x + extend; ++x)
		{
			for (int z = pos.z - extend; z <= pos.z + extend; ++z)
			{
				for (int y = pos.y - 1; y <= pos.y + 1; ++y)
				{
					if (pWorld->getBlockId(BlockPos(x, y, z)) == m_blockID)
					{
						--range;

						if (range <= 0)
						{
							flag = true;
							goto LOOPEND;
						}
					}
				}
			}
		}
LOOPEND:
		int meta = pWorld->getBlockMeta(pos);
		int face = pWorld->m_Rand.nextInt(6);
		int dir = dir_facingToDirection[face];
	
		BlockPos pos1 = pos.getPosY();
		if (face == 1 && pos.y < 255 && pWorld->isAirBlock(pos1))
		{
			if (flag)
			{
				return;
			}

			int meta2 = pWorld->m_Rand.nextInt(16) & meta;

			if (meta2 > 0)
			{
				for (int dir1 = 0; dir1 <= 3; ++dir1)
				{
					if (!canBePlacedOn(pWorld->getBlockId(pos1 + dir_offset[dir1])))
					{
						meta2 &= ~(1 << dir1);
					}
				}

				if (meta2 > 0)
				{
					pWorld->setBlock(pos1, m_blockID, meta2, 2);
				}
			}
		}
		else
		{
			if (face >= 2 && face <= 5 && (meta & 1 << dir) == 0)
			{
				if (flag)
				{
					return;
				}

				int blockID = pWorld->getBlockId(pos + dir_offset[dir]);

				Block* pBlock = BlockManager::sBlocks[blockID];
				if (blockID != 0 && pBlock != NULL)
				{
					if (pBlock->getMaterial().isOpaque() && pBlock->renderAsNormalBlock())
					{
						pWorld->setBlockMetadataWithNotify(pos, meta | (1 << dir), 2);
					}
				}
				else
				{
					int dir2 = dir + 1 & 3;
					int dir3 = dir + 3 & 3;

					if ((meta & (1 << dir2)) != 0 && 
						canBePlacedOn(pWorld->getBlockId(pos + dir_offset[dir] + dir_offset[dir2])))
					{
						pWorld->setBlock(pos + dir_offset[dir], m_blockID, 1 << dir2, 2);
					}
					else if ((meta & (1 << dir3)) != 0 &&
						canBePlacedOn(pWorld->getBlockId(pos + dir_offset[dir] + dir_offset[dir3])))
					{
						pWorld->setBlock(pos + dir_offset[dir], m_blockID, 1 << dir3, 2);
					}
					else if ((meta & (1 << dir2)) != 0 && 
						pWorld->isAirBlock(pos + dir_offset[dir] + dir_offset[dir2]) &&
						canBePlacedOn(pWorld->getBlockId(pos + dir_offset[dir2])))
					{
						pWorld->setBlock(pos + dir_offset[dir] + dir_offset[dir2], m_blockID, 1 << (dir + 2 & 3), 2);
					}
					else if ((meta & (1 << dir3)) != 0 &&
						pWorld->isAirBlock(pos + dir_offset[dir] + dir_offset[dir3]) && 
						canBePlacedOn(pWorld->getBlockId(pos + dir_offset[dir3])))
					{
						pWorld->setBlock(pos + dir_offset[dir] + dir_offset[dir3], m_blockID, 1 << (dir + 2 & 3), 2);
					}
					else if (canBePlacedOn(pWorld->getBlockId(pos1 + dir_offset[dir])))
					{
						pWorld->setBlock(pos + dir_offset[dir], m_blockID, 0, 2);
					}
				}
			}
			else if (pos.y > 1)
			{
				BlockPos pos2 = pos.getNegY();
				int blockID = pWorld->getBlockId(pos2);

				if (blockID == 0)
				{
					int meta2 = pWorld->m_Rand.nextInt(16) & meta;

					if (meta2 > 0)
					{
						pWorld->setBlock(pos2, m_blockID, meta2, 2);
					}
				}
				else if (blockID == m_blockID)
				{
					int meta2 = pWorld->m_Rand.nextInt(16) & meta;
					int meta3 = pWorld->getBlockMeta(pos2);

					if (meta3 != (meta3 | meta2))
					{
						pWorld->setBlockMetadataWithNotify(pos2, meta3 | meta2, 2);
					}
				}
			}
		}
	}
}

int BlockVine::onBlockPlaced(World* pWorld, const BlockPos& pos, int side, const Vector3& hit, int meta)
{
	i8 var10 = 0;

	switch (side)
	{
	case 2:
		var10 = 1;
		break;

	case 3:
		var10 = 4;
		break;

	case 4:
		var10 = 8;
		break;

	case 5:
		var10 = 2;
	}

	return var10 != 0 ? var10 : meta;
}

BlockCocoa::BlockCocoa(int id, const BM_Material& mat, const String& name)
	: Block(id, mat, name, false)
{
	setTickRandomly(true);
}

void BlockCocoa::updateTick(World* pWorld, const BlockPos& pos, Random& rand)
{
	if (!canBlockStay(pWorld, pos))
	{
		dropBlockAsItem(pWorld, pos, pWorld->getBlockMeta(pos), 0);
		pWorld->setBlock(pos, 0, 0, 2);
	}
	else if (pWorld->m_Rand.nextInt(5) == 0)
	{
		int meta = pWorld->getBlockMeta(pos);
		int age = getAgeFromMeta(meta);

		if (age < 2)
		{
			++age;
			pWorld->setBlockMetadataWithNotify(pos, age << 2 | getDirection(meta), 2);
		}
	}
}

bool BlockCocoa::canBlockStay(World* pWorld, const BlockPos& pos)
{
	int dir = getDirection(pWorld->getBlockMeta(pos));
	BlockPos pos1 = pos + dir_offset[dir];
	int blockID = pWorld->getBlockId(pos1);
	return BlockLogBase::isWood(blockID) && BlockLogBase::limitToValidMetadata(pWorld->getBlockMeta(pos1)) == 3;
}

int BlockCocoa::onBlockPlaced(World* pWorld, const BlockPos& pos, int side, const Vector3& hit, int meta)
{
	if (side == 1 || side == 0)
	{
		side = 2;
	}

	return dir_rotateOpposite[dir_facingToDirection[side]];
}

void BlockCocoa::onNeighborBlockChange(World* pWorld, const BlockPos& pos, int blockID)
{
	if (!canBlockStay(pWorld, pos))
	{
		dropBlockAsItem(pWorld, pos, pWorld->getBlockMeta(pos), 0);
		pWorld->setBlock(pos, 0, 0, 2);
	}
}

BlockGrass::BlockGrass(int id, const BM_Material& mat, const String& name)
	: Block(id, mat, name)
{
	setTickRandomly(true);
}

void BlockGrass::updateTick(World* pWorld, const BlockPos& pos, Random& rand)
{
	// todo.
	//if (!pWorld->m_isRemote)
	if(false)
	{
		BlockPos pos1 = pos.getPosY();
		
		if (pWorld->getBlockLightValue(pos1) < 4 &&
			BlockManager::sLightOpacity[pWorld->getBlockId(pos1)] > 2)
		{
			pWorld->setBlock(pos, BLOCK_ID_DIRT);
		}
		else if (pWorld->getBlockLightValue(pos1) >= 9)
		{
			for (int dir = 0; dir < 4; ++dir)
			{
				BlockPos pos2;
				pos2.x = pos.x + rand.nextInt(3) - 1;
				pos2.y = pos.y + rand.nextInt(5) - 3;
				pos2.z = pos.z + rand.nextInt(3) - 1;

				BlockPos pos3 = pos2.getPosY();
				int blockID = pWorld->getBlockId(pos3);

				if (pWorld->getBlockId(pos2) == BLOCK_ID_DIRT &&
					pWorld->getBlockLightValue(pos3) >= 4 &&
					BlockManager::sLightOpacity[blockID] <= 2)
				{
					pWorld->setBlock(pos2, BLOCK_ID_GRASS);
				}
			}
		}
	}
}

int BlockGrass::idDropped(int meta, Random& rand, int fortune)
{
	Block* pblock = BlockManager::dirt;
	return pblock->idDropped(0, rand, fortune);
}

//add by maxicheng
BlockReed::BlockReed(int id, const BM_Material& mat, const String& name)
	:Block(id, mat, name, false)
{
	float bound = 0.375F;
	setBlockBounds(0.5F - bound, 0.0F, 0.5F - bound, 0.5F + bound, 1.0F, 0.5F + bound);
}

void BlockReed::updateTick(World* pWorld, const BlockPos& pos, Random& random)
{
	BlockPos pos1 = pos.getPosY();
	if (pWorld->isAirBlock(pos1))
	{
		int len = 1;
		for (; pWorld->getBlockId(BlockPos(pos.x, pos.y - len, pos.z)) == m_blockID; ++len)
		{
			;
		}

		if (len < 3)
		{
			int meta = pWorld->getBlockMeta(pos);

			if (meta == 15)
			{
				pWorld->setBlock(pos1, m_blockID);
				pWorld->setBlockMetadataWithNotify(pos, 0, 4);
			}
			else
			{
				pWorld->setBlockMetadataWithNotify(pos, meta + 1, 4);
			}

		}

	}
}

bool BlockReed::canPlaceBlockAt(World* pWorld, const BlockPos& pos)
{
	BlockPos pos1 = pos.getPosY();
	int blockID = pWorld->getBlockId(pos1);
	return blockID == m_blockID ? true :
		(blockID != BLOCK_ID_GRASS && blockID != BLOCK_ID_DIRT && blockID != BLOCK_ID_SAND) ? false :
		(pWorld->getBlockMaterial(pos1.getNegX()) == BM_Material::BM_MAT_water ? true :
			(pWorld->getBlockMaterial(pos1.getPosX()) == BM_Material::BM_MAT_water ? true :
				(pWorld->getBlockMaterial(pos1.getNegZ()) == BM_Material::BM_MAT_water ? true :
					(pWorld->getBlockMaterial(pos1.getPosZ()) == BM_Material::BM_MAT_water))));
}

void BlockReed::onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborId)
{
	this->checkBlockCoordValid(pWorld, pos);
}

void BlockReed::checkBlockCoordValid(World* pWorld, const BlockPos& pos)
{
	if (!this->canBlockStay(pWorld, pos))
	{
		this->dropBlockAsItem(pWorld, pos, pWorld->getBlockMeta(pos), 0);
		pWorld->setBlockToAir(pos);
	}
}

bool BlockReed::canBlockStay(World* pWorld, const BlockPos& pos)
{
	return this->canPlaceBlockAt(pWorld, pos);
}

StringArray BlockWood::woodType = { "oak", "spruce", "birch", "jungle", "acacia", "big_oak" };

BlockWood::BlockWood(int id, const BM_Material& mat, const String& name)
	: Block(id, mat, name)
{
}

BlockLilyPad::BlockLilyPad(int id, const BM_Material& mat, const String& name)
	: BlockFlower(id, mat, name)
{
	float bound = 0.5F;
	float thickness = 0.015625F;
	setBlockBounds(0.5F - bound, 0.0F, 0.5F - bound, 0.5F + bound, thickness, 0.5F + bound);
}

Box BlockLilyPad::getCollisionBoundingBox(World* pWorld, const BlockPos& pos)
{
	return Box(Vector3(pos) + m_min, Vector3(pos) + m_max);
}

void BlockLilyPad::addCollisionBoxesToList(World* pWorld, const BlockPos& pos, const Box& aabb, AABBList& lst, Entity* pEntity)
{
	if (pEntity == NULL || !(pEntity->isClass(ENTITY_CLASS_BOAT)))
	{
		Block::addCollisionBoxesToList(pWorld, pos, aabb, lst, pEntity);
	}
}

bool BlockLilyPad::canBlockStay(World* pWorld, const BlockPos& pos)
{
	BlockPos pos1 = pos.getPosY();
	return pos.y >= 0 &&
		pos.y < 256 ? pWorld->getBlockMaterial(pos1) == BM_Material::BM_MAT_water &&
		pWorld->getBlockMeta(pos1) == 0 : false;
}

bool BlockLilyPad::canThisPlantGrowOnThisBlockID(int blockID) 
{ 
	return blockID == BLOCK_ID_WATERSTILL; 
}

StringArray BlockSandStone::SAND_STONE_TYPES = { "default", "chiseled", "smooth" };

BlockSandStone::BlockSandStone(int id, const BM_Material& mat, const String& name)
	: Block(id, mat, name)
{
}

BlockMycelium::BlockMycelium(int id, const BM_Material& mat, const String& name)
	: Block(id, mat, name)
{ }

void BlockMycelium::updateTick(World* pWorld, const BlockPos& pos, Random& rand)
{
	// todo.
	//if (!pWorld->m_isRemote)
	if(false)
	{
		const int* pLightOpacity = BlockManager::sLightOpacity;

		BlockPos pos1 = pos.getPosY();
		if (pWorld->getBlockLightValue(pos1) < 4 && 
			pLightOpacity[pWorld->getBlockId(pos1)] > 2)
		{
			pWorld->setBlock(pos, BLOCK_ID_DIRT);
		}
		else if (pWorld->getBlockLightValue(pos1) >= 9)
		{
			for (int dir = 0; dir < 4; ++dir)
			{
				BlockPos pos2 = pos;
				pos2.x += rand.nextInt(3) - 1;
				pos2.y += rand.nextInt(5) - 3;
				pos2.z += rand.nextInt(3) - 1;
				int blockID = pWorld->getBlockId(pos2.getPosY());

				if (pWorld->getBlockId(pos2) == BLOCK_ID_DIRT &&
					pWorld->getBlockLightValue(pos2.getPosY()) >= 4 && 
					pLightOpacity[blockID] <= 2)
				{
					pWorld->setBlock(pos2, m_blockID);
				}
			}
		}
	}
}

int BlockMycelium::idDropped(int meta, Random& rand, int fortune)
{
	return BlockManager::dirt->idDropped(0, rand, fortune);
}

BlockContainer::BlockContainer(int id, const BM_Material& mat, const String& name, bool isOpaque)
	: Block(id, mat, name, isOpaque)
{
	m_isBlockContainer = true;
}

void BlockContainer::breakBlock(World* pWorld, const BlockPos& pos, int blockID, int meta)
{
	Block::breakBlock(pWorld, pos, blockID, meta);
	pWorld->removeBlockTileEntity(pos);
}

bool BlockContainer::onBlockEventReceived(World* pWorld, const BlockPos& pos, int id, int param)
{
	Block::onBlockEventReceived(pWorld, pos, id, param);
	TileEntity* pTileEntity = pWorld->getBlockTileEntity(pos);
	return pTileEntity != NULL ? pTileEntity->receiveClientEvent(id, param) : false;
}

BlockChest::BlockChest(int id, const BM_Material& mat, const String& name, int block_trapped_type)
	: BlockContainer(id, mat, name, false)
{
	//isTrapped = trapped;
	m_block_trapped_type = block_trapped_type;
	setBlockBounds(0.0625F, 0.0F, 0.0625F, 0.9375F, 0.875F, 0.9375F);
}

void BlockChest::setBlockBoundsBasedOnState(IBlockAccess* pBlockAccess, const BlockPos& pos)
{
	if (pBlockAccess->getBlockId(pos.getNegZ()) == m_blockID)
	{
		setBlockBounds(0.0625F, 0.0F, 0.0F, 0.9375F, 0.875F, 0.9375F);
	}
	else if (pBlockAccess->getBlockId(pos.getPosZ()) == m_blockID)
	{
		setBlockBounds(0.0625F, 0.0F, 0.0625F, 0.9375F, 0.875F, 1.0F);
	}
	else if (pBlockAccess->getBlockId(pos.getNegX()) == m_blockID)
	{
		setBlockBounds(0.0F, 0.0F, 0.0625F, 0.9375F, 0.875F, 0.9375F);
	}
	else if (pBlockAccess->getBlockId(pos.getPosX()) == m_blockID)
	{
		setBlockBounds(0.0625F, 0.0F, 0.0625F, 1.0F, 0.875F, 0.9375F);
	}
	else
	{
		setBlockBounds(0.0625F, 0.0F, 0.0625F, 0.9375F, 0.875F, 0.9375F);
	}
}

void BlockChest::onBlockAdded(World* pWorld, const BlockPos& pos)
{
	BlockContainer::onBlockAdded(pWorld, pos);
	unifyAdjacentChests(pWorld, pos);
	int block_north = pWorld->getBlockId(pos.getNegZ());
	int block_south = pWorld->getBlockId(pos.getPosZ());
	int block_west = pWorld->getBlockId(pos.getNegX());
	int block_east = pWorld->getBlockId(pos.getPosX());

	if (block_north == m_blockID)
		unifyAdjacentChests(pWorld, pos.getNegZ());
	if (block_south == m_blockID)
		unifyAdjacentChests(pWorld, pos.getPosZ());
	if (block_west == m_blockID)
		unifyAdjacentChests(pWorld, pos.getNegX());
	if (block_east == m_blockID)
		unifyAdjacentChests(pWorld, pos.getPosX());
}

void BlockChest::onBlockPlacedBy(World* pWorld, const BlockPos& pos, EntityLivingBase* pLiving, ItemStackPtr pStack)
{
	BlockPos negZ = pos.getNegZ();
	BlockPos posZ = pos.getPosZ();
	BlockPos negX = pos.getNegX();
	BlockPos posX = pos.getPosX();
	int block_north = pWorld->getBlockId(negZ);
	int block_south = pWorld->getBlockId(posZ);
	int block_west = pWorld->getBlockId(negX);
	int block_east = pWorld->getBlockId(posX);
	
	int meta = 0;
	int dir = int(Math::Floor((pLiving->rotationYaw * 4.0F / 360.0F) + 0.5f)) & 3;

	if (dir == 0)
		meta = 2;
	if (dir == 1)
		meta = 5;
	if (dir == 2)
		meta = 3;
	if (dir == 3)
		meta = 4;

	if (block_north != m_blockID && block_south != m_blockID && block_west != m_blockID && block_east != m_blockID)
	{
		pWorld->setBlockMetadataWithNotify(pos, meta, 3);
	}
	else
	{
		if ((block_north == m_blockID || block_south == m_blockID) && (meta == 4 || meta == 5))
		{
			if (block_north == m_blockID)
				pWorld->setBlockMetadataWithNotify(negZ, meta, 3);
			else
				pWorld->setBlockMetadataWithNotify(posZ, meta, 3);

			pWorld->setBlockMetadataWithNotify(pos, meta, 3);
		}

		if ((block_west == m_blockID || block_east == m_blockID) && (meta == 2 || meta == 3))
		{
			if (block_west == m_blockID)
				pWorld->setBlockMetadataWithNotify(negX, meta, 3);
			else
				pWorld->setBlockMetadataWithNotify(posX, meta, 3);

			pWorld->setBlockMetadataWithNotify(pos, meta, 3);
		}
	}

	if (pStack->hasDisplayName())
	{
		TileEntityChest* pChest = dynamic_cast<TileEntityChest*>(pWorld->getBlockTileEntity(pos));
		if (pChest)
			pChest->setChestGuiName(pStack->getDisplayName());
	}
}

void BlockChest::unifyAdjacentChests(World* pWorld, const BlockPos& pos)
{
	if (!pWorld->m_isClient)
	{
		int block_north = pWorld->getBlockId(pos.getNegZ());
		int block_south = pWorld->getBlockId(pos.getPosZ());
		int block_west = pWorld->getBlockId(pos.getNegX());
		int block_east = pWorld->getBlockId(pos.getPosX());
		int meta;

		const i8* opaque = BlockManager::sOpaqueCubeLookup;
		if (block_north != m_blockID && block_south != m_blockID)
		{
			if (block_west != m_blockID && block_east != m_blockID)
			{
				meta = 3;

				if (opaque[block_north] && !opaque[block_south])
					meta = 3;
				if (opaque[block_south] && !opaque[block_north])
					meta = 2;
				if (opaque[block_west] && !opaque[block_east])
					meta = 5;
				if (opaque[block_east] && !opaque[block_west])
					meta = 4;
			}
			else
			{
				BlockPos pos1 = pos.getPosY();
				if (block_west == m_blockID)
					pos1.x--;
				else
					pos1.x++;

				int bID1 = pWorld->getBlockId(pos1.getNegZ());
				int bID2 = pWorld->getBlockId(pos1.getPosZ());
				meta = 3;
				int meta2 = 0;

				if (block_west == m_blockID)
					meta2 = pWorld->getBlockMeta(pos.getNegX());
				else
					meta2 = pWorld->getBlockMeta(pos.getPosX());

				if (meta2 == 2)
					meta = 2;

				if ((opaque[block_north] || opaque[bID1]) && !opaque[block_south] && !opaque[bID2])
					meta = 3;

				if ((opaque[block_south] || opaque[bID2]) && !opaque[block_north] && !opaque[bID1])
					meta = 2;
			}
		}
		else
		{
			BlockPos pos1 = pos;
			if (block_north == m_blockID)
				pos1.z--;
			else
				pos1.z++;

			int bID1 = pWorld->getBlockId(pos1.getNegX());
			int bID2 = pWorld->getBlockId(pos1.getPosX());
			meta = 5;
			int meta2 = 0;

			if (block_north == m_blockID)
				meta2 = pWorld->getBlockMeta(pos.getNegZ());
			else
				meta2 = pWorld->getBlockMeta(pos.getPosZ());

			if (meta2 == 4)
				meta = 4;

			if ((opaque[block_west] || opaque[bID1]) && !opaque[block_east] && !opaque[bID2])
				meta = 5;

			if ((opaque[block_east] || opaque[bID2]) && !opaque[block_west] && !opaque[bID1])
				meta = 4;
		}

		pWorld->setBlockMetadataWithNotify(pos, meta, 3);
	}
}

bool BlockChest::canPlaceBlockAt(World* pWorld, const BlockPos& pos)
{
	int chest_neighbor = 0;
	BlockPos negZ = pos.getNegZ();
	BlockPos posZ = pos.getPosZ();
	BlockPos negX = pos.getNegX();
	BlockPos posX = pos.getPosX();

	if (pWorld->getBlockId(negX) == m_blockID)
	{
		++chest_neighbor;
	}

	if (pWorld->getBlockId(posX) == m_blockID)
	{
		++chest_neighbor;
	}

	if (pWorld->getBlockId(negZ) == m_blockID)
	{
		++chest_neighbor;
	}

	if (pWorld->getBlockId(posZ) == m_blockID)
	{
		++chest_neighbor;
	}

	return chest_neighbor > 1 ? false : 
		(isThereANeighborChest(pWorld, negX) ? false : 
			(isThereANeighborChest(pWorld, posX) ? false : 
				(isThereANeighborChest(pWorld, negZ) ? false :
					!isThereANeighborChest(pWorld, posZ))));
}

bool BlockChest::isThereANeighborChest(World* pWorld, const BlockPos& pos)
{
	BlockPos negZ = pos.getNegZ();
	BlockPos posZ = pos.getPosZ();
	BlockPos negX = pos.getNegX();
	BlockPos posX = pos.getPosX();

	return pWorld->getBlockId(pos) != m_blockID ? false :
		(pWorld->getBlockId(negX) == m_blockID ? true :
			(pWorld->getBlockId(posX) == m_blockID ? true :
				(pWorld->getBlockId(negZ) == m_blockID ? true :
					pWorld->getBlockId(posZ) == m_blockID)));
}

void BlockChest::onNeighborBlockChange(World* pWorld, const BlockPos& pos, int blockID)
{
	Block::onNeighborBlockChange(pWorld, pos, blockID);
	TileEntityChest* pChest = dynamic_cast<TileEntityChest*>(pWorld->getBlockTileEntity(pos));

	if (pChest != NULL)
		pChest->updateContainingBlockInfo();
}

void BlockChest::breakBlock(World* pWorld, const BlockPos& pos, int blockID, int meta)
{
	
	TileEntityChest* pChest = dynamic_cast<TileEntityChest*>(pWorld->getBlockTileEntity(pos));

	if (pChest && !pWorld->m_isClient)
	{
		for (int i = 0; i < pChest->getSizeInventory(); ++i)
		{
			ItemStackPtr pStack = pChest->getStackInSlot(i);

			if (!pStack)
				continue; 

			if (!SCRIPT_EVENT::EntityItemSpawnEvent::invoke(pStack->itemID, pStack->itemDamage, "break.chest.drop"))
				continue;

			Vector3 spawnPos = pWorld->m_Rand.nextVector() * 0.8f + 0.1f;
			EntityItem* pItem = NULL;

			for (; pStack->stackSize > 0; pWorld->spawnEntityInWorld(pItem))
			{
				int randsize = pWorld->m_Rand.nextInt(21) + 10;

				if (randsize > pStack->stackSize)
				{
					randsize = pStack->stackSize;
				}

				pStack->stackSize -= randsize;
				pItem = LordNew EntityItem(pWorld, Vector3(pos) + spawnPos, LORD::make_shared<ItemStack>(pStack->itemID, randsize, pStack->getItemDamage()));
				float motion = 0.05F;
				pItem->motion.x = float(pWorld->m_Rand.nextGaussian() * motion);
				pItem->motion.y = float(pWorld->m_Rand.nextGaussian() * motion + 0.2F);
				pItem->motion.z = float(pWorld->m_Rand.nextGaussian() * motion);

				if (pStack->hasTagCompound())
				{
					pItem->getEntityItem()->setTagCompound((NBTTagCompound*)pStack->getTagCompound()->copy());
				}
			}
		}

		pWorld->updateComparatorOutputLevel(pos, blockID);
	}

	BlockContainer::breakBlock(pWorld, pos, blockID, meta);
}

bool BlockChest::onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit)
{
	if (pWorld->m_isClient)
	{
		return true;
	}
	else
	{
		if (SCRIPT_EVENT::ChestOpenSpHandleEvent::invoke(pos, pPlayer->hashCode()))
		{
			pPlayer->setActivatedBlock(pos);
			IInventory* pInventory = getInventory(pWorld, pos);
			if (pInventory)
			{
				BLOCKMAN::GameCommon::Instance()->getCommonPacketSender()->sendChestInventory(pPlayer, pos, face, hit, pInventory);
			}
		}
		
		return true;
	}
}

IInventory* BlockChest::getInventory(World* pWorld, const BlockPos& pos)
{
	TileEntityChest* pChest = dynamic_cast<TileEntityChest*>(pWorld->getBlockTileEntity(pos));
	if (!pChest)
		return NULL;

	BlockPos negZ = pos.getNegZ();
	BlockPos posZ = pos.getPosZ();
	BlockPos negX = pos.getNegX();
	BlockPos posX = pos.getPosX();
	BlockPos negY = pos.getNegY();
	BlockPos posY = pos.getPosY();

	if (pWorld->isBlockNormalCube(posY))
		return NULL;
	else if (isOcelotBlockingChest(pWorld, pos))
		return NULL;
	else if (pWorld->getBlockId(negX) == m_blockID && (pWorld->isBlockNormalCube(negX.getPosY()) || isOcelotBlockingChest(pWorld, negX)))
		return NULL;
	else if (pWorld->getBlockId(posX) == m_blockID && (pWorld->isBlockNormalCube(posX.getPosY()) || isOcelotBlockingChest(pWorld, posX)))
		return NULL;
	else if (pWorld->getBlockId(negZ) == m_blockID && (pWorld->isBlockNormalCube(negZ.getPosY()) || isOcelotBlockingChest(pWorld, negZ)))
		return NULL;
	else if (pWorld->getBlockId(posZ) == m_blockID && (pWorld->isBlockNormalCube(posZ.getPosY()) || isOcelotBlockingChest(pWorld, posZ)))
		return NULL;
	else
	{
		IInventory* pResult = NULL;
		if (pWorld->getBlockId(negX) == m_blockID)
			pResult = LordNew InventoryLargeChest("container.chestDouble", dynamic_cast<TileEntityChest*>(pWorld->getBlockTileEntity(negX)), (IInventory*)pChest);
		
		if (pWorld->getBlockId(posX) == m_blockID)
			pResult = LordNew InventoryLargeChest("container.chestDouble", (IInventory*)pChest, dynamic_cast<TileEntityChest*>(pWorld->getBlockTileEntity(posX)));
		
		if (pWorld->getBlockId(negZ) == m_blockID)
			pResult = LordNew InventoryLargeChest("container.chestDouble", dynamic_cast<TileEntityChest*>(pWorld->getBlockTileEntity(negZ)), (IInventory*)pChest);
		
		if (pWorld->getBlockId(posZ) == m_blockID)
			pResult = LordNew InventoryLargeChest("container.chestDouble", (IInventory*)pChest, dynamic_cast<TileEntityChest*>(pWorld->getBlockTileEntity(posZ)));

		return pChest;
	}
	return NULL;
}

TileEntity* BlockChest::createNewTileEntity(World* pWorld)
{
	TileEntityChest* pChest = LordNew TileEntityChest();
	pChest->setWorldObj(pWorld);
	return pChest;
}

int BlockChest::isProvidingWeakPower(IBlockAccess* pBlockAccess, const BlockPos& pos, int side)
{
	if (!canProvidePower())
	{
		return 0;
	}
	else
	{
		TileEntityChest* pChest = dynamic_cast<TileEntityChest*>(pBlockAccess->getBlockTileEntity(pos));
		if (pChest)
		{
			int playerNum = pChest->numUsingPlayers;
			return Math::Clamp(playerNum, 0, 15);
		}
	}
	return 0;
}

int BlockChest::isProvidingStrongPower(IBlockAccess* pBlockAccess, const BlockPos& pos, int side)
{
	return side == 1 ? isProvidingWeakPower(pBlockAccess, pos, side) : 0;
}

bool BlockChest::isOcelotBlockingChest(World* pWorld, const BlockPos& pos)
{
	// todo.
	/*Iterator var4 = pWorld->getEntitiesWithinAABB(EntityOcelot.class, AxisAlignedBB.getAABBPool().getAABB((double)x, (double)(y + 1), (double)z, (double)(x + 1), (double)(y + 2), (double)(z + 1))).iterator();
	EntityOcelot var6;

	do
	{
		if (!var4.hasNext())
		{
			return false;
		}

		EntityOcelot var5 = (EntityOcelot)var4.next();
		var6 = (EntityOcelot)var5;
	} while (!var6.isSitting());

	return true;*/
	return false;
}

int BlockChest::getComparatorInputOverride(World* pWorld, const BlockPos& pos, int side)
{
	// todo.
	// return Container::calcRedstoneFromInventory(getInventory(pWorld, x, y, z));
	return 0;
}

BlockDirectional::BlockDirectional(int id, const BM_Material& mat, const String& name, bool bOpaque)
	: Block(id, mat, name, bOpaque)
{
}

BlockPos BlockBed::footBlockToHeadBlockMap[4] = { BlockPos(0, 0, 1), BlockPos(-1, 0, 0), BlockPos(0, 0, -1), BlockPos(1, 0, 0) };

BlockBed::BlockBed(int id, const String& name)
	: BlockDirectional(id, BM_Material::BM_MAT_cloth, name, true)
{
	m_isOpaqueCube = false;
	setLightOpacity(5);
	setBounds();
}

bool BlockBed::onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit)
{
	if (pWorld->m_isClient)
		return true;

	pPlayer->setActivatedBlock(pos);
	int meta = pWorld->getBlockMeta(pos);
	BlockPos pos2 = pos;

	if (!isBlockHeadOfBed(meta))
	{
		int dir = getDirection(meta);
		pos2 += footBlockToHeadBlockMap[dir];

		if (pWorld->getBlockId(pos2) != m_blockID)
		{
			return true;
		}

		meta = pWorld->getBlockMeta(pos2);
	}

	if (pWorld->m_provider->canRespawnHere() && pWorld->getBiomeGenForCoords(pos2.x, pos2.z) != BiomeGenBase::hell)
	{
		if (isBedOccupied(meta))
		{
			EntityPlayer* pPlayer2 = NULL;
			EntityPlayers& players = pWorld->getPlayers();

			for (EntityPlayers::iterator it = players.begin(); it != players.end(); ++it)
			{
				EntityPlayer* pPlayer3 = it->second;
				if (!pPlayer3)
					continue;
				if (pPlayer3->isPlayerSleeping())
				{
					Vector3i bedPos = pPlayer3->bedLocation;
					if (bedPos == pos2)
					{
						pPlayer2 = pPlayer3;
					}
				}

			}
			
			if (pPlayer2)
			{
				pPlayer->addChatMessage("tile.bed.occupied");
				return true;
			}

			setBedOccupied(pWorld, pos2, false);
		}

		SLEEP_RESULT sleepResult = pPlayer->sleepInBedAt(pos2);

		if (sleepResult == SLEEP_RESULT_OK)
		{
			setBedOccupied(pWorld, pos2, true);
			return true;
		}
		else
		{
			if (sleepResult == SLEEP_RESULT_NOT_POSSIBLE_NOW)
			{
				pPlayer->addChatMessage("tile.bed.noSleep");
			}
			else if (sleepResult == SLEEP_RESULT_NOT_SAFE)
			{
				pPlayer->addChatMessage("tile.bed.notSafe");
			}

			return true;
		}
	}
	else
	{
		pWorld->setBlockToAir(pos2);
		int dir = getDirection(meta);
		pos2 += footBlockToHeadBlockMap[dir];

		if (pWorld->getBlockId(pos2) == m_blockID)
		{
			pWorld->setBlockToAir(pos2);
		}

		pWorld->newExplosion(nullptr, nullptr, Vector3(pos2) + Vector3::HALF, 5.f, true, true);
		return true;
	}
}

Vector3i BlockBed::getOtherSidePos(const BlockPos & pos, int meta)
{
	int dir = getDirection(meta);
	if (isBlockHeadOfBed(meta))
	{
		return pos - footBlockToHeadBlockMap[dir];
	}
	else
	{
		return pos + footBlockToHeadBlockMap[dir];
	}
}

void BlockBed::onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborID)
{
	int meta = pWorld->getBlockMeta(pos);
	int dir = getDirection(meta);

	if (isBlockHeadOfBed(meta))
	{
		if (pWorld->getBlockId(pos - footBlockToHeadBlockMap[dir]) != m_blockID)
		{
			pWorld->setBlockToAir(pos);
		}
	}
	else if (pWorld->getBlockId(pos + footBlockToHeadBlockMap[dir]) != m_blockID)
	{
		pWorld->setBlockToAir(pos);

		if (!pWorld->m_isClient)
		{
			dropBlockAsItem(pWorld, pos, meta, 0);
		}
	}
}

int BlockBed::idDropped(int meta, Random& rand, int fortune)
{
	return isBlockHeadOfBed(meta) ? 0 : Item::bed->itemID;
}

void BlockBed::setBedOccupied(World* pWorld, const BlockPos& pos, bool occupy)
{
	int meta = pWorld->getBlockMeta(pos);

	if (occupy)
	{
		meta |= 4;
	}
	else
	{
		meta &= -5;
	}

	pWorld->setBlockMetadataWithNotify(pos, meta, 4);
}


Vector3i BlockBed::getNearestEmptyChunkCoordinates(World* pWorld, const BlockPos& pos, int tries)
{
	int meta = pWorld->getBlockMeta(pos);
	int dir = BlockDirectional::getDirection(meta);

	for (int i = 0; i <= 1; ++i)
	{
		int xstart = pos.x - footBlockToHeadBlockMap[dir].x * i - 1;
		int zstart = pos.z - footBlockToHeadBlockMap[dir].z * i - 1;
		int xend = xstart + 2;
		int zend = zstart + 2;

		for (int ix = xstart; ix <= xend; ++ix)
		{
			for (int iz = zstart; iz <= zend; ++iz)
			{
				BlockPos pos1(ix, pos.y, iz);
				if (pWorld->doesBlockHaveSolidTopSurface(pos1.getNegY()) && 
					!pWorld->getBlockMaterial(pos1).isOpaque() && 
					!pWorld->getBlockMaterial(pos1.getPosY()).isOpaque())
				{
					if (tries <= 0)
					{
						return pos1;
					}

					--tries;
				}
			}
		}
	}

	return Vector3i::INVALID;
}

void BlockBed::dropBlockAsItemWithChance(World* pWorld, const BlockPos& pos, int meta, float chance, int fortune)
{
	// delete it for bedwar temporary

	//if (!isBlockHeadOfBed(meta))
	//{
	//	Block::dropBlockAsItemWithChance(pWorld, pos, meta, chance, 0);
	//}
}

int BlockBed::idPicked(World* pWorld, const BlockPos& pos)
{
	return Item::bed->itemID;
}

void BlockBed::onBlockHarvested(World* pWorld, const BlockPos& pos, int meta, EntityPlayer* pPlayer)
{
	if (pPlayer->capabilities.isCreativeMode && isBlockHeadOfBed(meta))
	{
		int dir = getDirection(meta);
		BlockPos pos1 = pos - footBlockToHeadBlockMap[dir];

		if (pWorld->getBlockId(pos1) == m_blockID)
		{
			pWorld->setBlockToAir(pos1);
		}
	}
}

StringArray BlockSilverfish::silverfishStoneTypes = { "stone", "cobble", "brick" };

BlockSilverfish::BlockSilverfish(int id, const BM_Material& mat, const String& name)
	:Block(id, mat, name)
{ }

int BlockSilverfish::getDamageValue(World* pWorld, const BlockPos& pos)
{
	return pWorld->getBlockMeta(pos);
}

void BlockSilverfish::onBlockDestroyedByPlayer(World* pWorld, const BlockPos& pos, int meta)
{
	if (!pWorld->m_isClient)
	{
		// todo.
		/*
		EntitySilverfish var6 = new EntitySilverfish(par1World);
		var6.setLocationAndAngles((double)par2 + 0.5D, (double)par3, (double)par4 + 0.5D, 0.0F, 0.0F);
		pWorld->spawnEntityInWorld(var6);
		var6.spawnExplosionParticle();
		*/
	}

	Block::onBlockDestroyedByPlayer(pWorld, pos, meta);
}

ItemStackPtr BlockSilverfish::createStackedBlock(int damage)
{
	int blockID = BLOCK_ID_STONE;

	if (damage == 1)
		blockID = BLOCK_ID_COBBLE_STONE;
	if (damage == 2)
		blockID = BLOCK_ID_STONE_BRICK;

	return LORD::make_shared<ItemStack>(blockID, 1, 0);
}

BlockFarmland::BlockFarmland(int id, const BM_Material& mat, const String& name)
	:Block(id, mat, name, false)
{ 
}

void BlockFarmland::updateTick(World* pWorld, const BlockPos& pos, Random& rand)
{
	if (!isWaterNearby(pWorld, pos) && !pWorld->canLightningStrikeAt(pos.getPosY()))
	{
		int meta = pWorld->getBlockMeta(pos);
		
		if (meta > 0)
		{
			pWorld->setBlockMetadataWithNotify(pos, meta - 1, 2);
		}
		else if (!isCropsNearby(pWorld, pos))
		{
			pWorld->setBlock(pos, BLOCK_ID_DIRT);
		}
	}
	else
	{
		pWorld->setBlockMetadataWithNotify(pos, 7, 2);
	}
}

void BlockFarmland::onFallenUpon(World* pWorld, const BlockPos& pos, Entity* pEntity, float distance)
{
	
	if (!pWorld->m_isClient && pWorld->m_Rand.nextFloat() < distance - 0.5F)
	{
		if (!(pEntity->isClass(ENTITY_CLASS_PLAYER))) //&& !pWorld->getGameRules().getGameRuleBooleanValue("mobGriefing"))
		{
			return;
		}

		// pWorld->setBlock(pos, BLOCK_ID_DIRT); /* in case of it turn into BLOCK_ID_DIRT when stepped by player in Game:Tiny Town, add by oujie */
	}
	
}

void BlockFarmland::onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborBlockID)
{
	Block::onNeighborBlockChange(pWorld, pos, neighborBlockID);
	const BM_Material& mat = pWorld->getBlockMaterial(pos.getPosY());

	if (mat.isSolid())
	{
		//pWorld->setBlock(pos, BLOCK_ID_DIRT); /* in case of it turn into BLOCK_ID_DIRT when Neighbor Block Changed in Game:Tiny Town, add by oujie */
	}
}

bool BlockFarmland::isCropsNearby(World* pWorld, const BlockPos& pos)
{
	int size = 0;
	
	for (int ix = pos.x - size; ix <= pos.x + size; ++ix)
	{
		for (int iz = pos.z - size; iz <= pos.z + size; ++iz)
		{
			BlockPos pos1(ix, pos.y + 1, iz);
			int blockID = pWorld->getBlockId(pos1);

			if (blockID == BLOCK_ID_CROPS || 
				blockID == BLOCK_ID_MELON_STEM || 
				blockID == BLOCK_ID_PUMPKIN_STEM || 
				blockID == BLOCK_ID_POTATO || 
				blockID == BLOCK_ID_CARROT)
			{
				return true;
			}
		}
	}

	return false;
}

bool BlockFarmland::isWaterNearby(World* pWorld, const BlockPos& pos)
{
	for (int ix = pos.x - 4; ix <= pos.x + 4; ++ix)
	{
		for (int iy = pos.y; iy <= pos.y + 1; ++iy)
		{
			for (int iz = pos.z - 4; iz <= pos.z + 4; ++iz)
			{
				BlockPos pos1(ix, iy, iz);
				if (pWorld->getBlockMaterial(pos1) == BM_Material::BM_MAT_water)
				{
					return true;
				}
			}
		}
	}
	
	return false;
}

BlockDoor::BlockDoor(int id, const BM_Material& mat, const String& name)
	:Block(id, mat, name, false)
{
	float bound = 0.5F;
	float height = 1.0F;
	setBlockBounds(0.5F - bound, 0.0F, 0.5F - bound, 0.5F + bound, height, 0.5F + bound);
}

bool BlockDoor::getBlocksMovement(IBlockAccess* pBlockAccess, const BlockPos& pos) const
{
	int meta = getFullMetadata(pBlockAccess, pos);
	return (meta & 4) != 0;
}

Box BlockDoor::getCollisionBoundingBox(World* pWorld, const BlockPos& pos)
{
	setBlockBoundsBasedOnState(pWorld, pos);
	return Block::getCollisionBoundingBox(pWorld, pos);
}

void BlockDoor::setBlockBoundsBasedOnState(IBlockAccess* pBlockAccess, const BlockPos& pos)
{
	setDoorRotation(getFullMetadata(pBlockAccess, pos));
}

bool BlockDoor::onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit)
{
	if (!checkDoor(pWorld, pos, pPlayer))
	{
		return false;
	}

	if (m_blockMaterial == BM_Material::BM_MAT_iron)
	{
		return true;
	}
	pPlayer->setActivatedBlock(pos);
	switchDoorState(pWorld, pos, false);
	if (pWorld->m_isClient)
	{
		return true;
	}
	bool bIsDoorOpen = isDoorOpen(pWorld, pos);
	if (bIsDoorOpen)
	{
		SCRIPT_EVENT::PlayerOpenDoorEvent::invoke(pPlayer->hashCode(), pos);
	}
	// pWorld->playAuxSFXAtEntity(par5EntityPlayer, 1003, par2, par3, par4, 0);
	return true;
}

int BlockDoor::getDoorOrientation(IBlockAccess* pBlockAccess, const BlockPos& pos)
{
	return getFullMetadata(pBlockAccess, pos) & 3;
}

void BlockDoor::onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborID)
{
	int meta = pWorld->getBlockMeta(pos);

	if ((meta & 8) == 0)
	{
		bool flag = false;

		BlockPos negY = pos.getNegY();
		BlockPos posY = pos.getPosY();
		if (pWorld->getBlockId(posY) != m_blockID)
		{
			pWorld->setBlockToAir(pos);
			flag = true;
		}

		if (!pWorld->doesBlockHaveSolidTopSurface(negY))
		{
			pWorld->setBlockToAir(pos);
			flag = true;

			if (pWorld->getBlockId(posY) == m_blockID)
			{
				pWorld->setBlockToAir(posY);
			}
		}

		if (flag)
		{
			if (!pWorld->m_isClient)
			{
				dropBlockAsItem(pWorld, pos, meta, 0);
			}
		}
		else
		{
			bool powered = pWorld->isBlockIndirectlyGettingPowered(pos) || pWorld->isBlockIndirectlyGettingPowered(posY);

			if ((powered || neighborID > 0 && 
				BlockManager::sBlocks[neighborID]->canProvidePower()) && 
				neighborID != m_blockID)
			{
				onPoweredBlockChange(pWorld, pos, powered);
			}
		}
	}
	else
	{
		int blockId = pWorld->getBlockId(pos.getNegY());
		if (blockId != m_blockID)
		{
			pWorld->setBlockToAir(pos);
			LordLogInfo("onNeighborBlockChange by set block failure : blockId(%d) != m_blockID(%d) pos (%d, %d, %d)", blockId, m_blockID, pos.x, pos.y, pos.z);
		}

		if (neighborID > 0 && neighborID != m_blockID)
		{
			onNeighborBlockChange(pWorld, pos.getNegY(), neighborID);
		}
	}
}

int BlockDoor::idDropped(int meta, Random& rand, int fortune)
{	
	if ((meta & 8) != 0)
	{
		return 0;
	}
	else if (m_blockMaterial == BM_Material::BM_MAT_iron)
	{		
		return ITEM_ID_DOOR_IRON;	
	}
	else
	{
		return ITEM_ID_DOOR_WOOD;		
	}
}

RayTraceResult BlockDoor::collisionRayTrace(World* pWorld, const BlockPos& pos, const Vector3& start, const Vector3& end)
{
	setBlockBoundsBasedOnState(pWorld, pos);
	return Block::collisionRayTrace(pWorld, pos, start, end);
}

bool BlockDoor::canPlaceBlockAt(World* pWorld, const BlockPos& pos)
{
	return pos.y >= 255 ? false :
		pWorld->doesBlockHaveSolidTopSurface(pos.getNegY()) &&
		Block::canPlaceBlockAt(pWorld, pos) &&
		Block::canPlaceBlockAt(pWorld, pos.getPosY());
}

int BlockDoor::getFullMetadata(IBlockAccess* pBlockAccess, const BlockPos& pos) const
{
	int meta = pBlockAccess->getBlockMeta(pos);
	bool isUpperPart = (meta & 8) != 0;
	int leastSignificant = 0;
	int mostSignificant = 0;

	if (isUpperPart)
	{
		leastSignificant = pBlockAccess->getBlockMeta(pos.getNegY());
		mostSignificant = meta;
	}
	else
	{
		leastSignificant = meta;
		mostSignificant = pBlockAccess->getBlockMeta(pos.getPosY());
	}

	bool hingeOnRight = (mostSignificant & 1) != 0;
	return leastSignificant & 7 | (isUpperPart ? 8 : 0) | (hingeOnRight ? 16 : 0);
}

int BlockDoor::idPicked(World* pWorld, const BlockPos& pos)
{
	return m_blockMaterial == BM_Material::BM_MAT_iron ? ITEM_ID_DOOR_IRON : ITEM_ID_DOOR_WOOD;	
}

void BlockDoor::onBlockHarvested(World* pWorld, const BlockPos& pos, int meta, EntityPlayer* pPlayer)
{
	if (pPlayer->capabilities.isCreativeMode && 
		(meta & 8) != 0 && 
		pWorld->getBlockId(pos.getNegY()) == m_blockID)
	{
		pWorld->setBlockToAir(pos.getNegY());
	}
}

bool BlockDoor::isDoorOpen(IBlockAccess* pBlockAccess, const BlockPos& pos)
{
	return (getFullMetadata(pBlockAccess, pos) & 4) != 0;
}

void BlockDoor::setDoorRotation(int meta)
{
	float bound = 0.1875F;
	setBlockBounds(0.0F, 0.0F, 0.0F, 1.0F, 2.0F, 1.0F);
	int d = meta & 3;
	bool var4 = (meta & 4) != 0;
	bool var5 = (meta & 16) != 0;

	if (d == 0)
	{
		if (var4)
		{
			if (!var5)
				setBlockBounds(0.0F, 0.0F, 0.0F, 1.0F, 1.0F, bound);
			else
				setBlockBounds(0.0F, 0.0F, 1.0F - bound, 1.0F, 1.0F, 1.0F);
		}
		else
			setBlockBounds(0.0F, 0.0F, 0.0F, bound, 1.0F, 1.0F);
	}
	else if (d == 1)
	{
		if (var4)
		{
			if (!var5)
				setBlockBounds(1.0F - bound, 0.0F, 0.0F, 1.0F, 1.0F, 1.0F);
			else
				setBlockBounds(0.0F, 0.0F, 0.0F, bound, 1.0F, 1.0F);
		}
		else
			setBlockBounds(0.0F, 0.0F, 0.0F, 1.0F, 1.0F, bound);
	}
	else if (d == 2)
	{
		if (var4)
		{
			if (!var5)
				setBlockBounds(0.0F, 0.0F, 1.0F - bound, 1.0F, 1.0F, 1.0F);
			else
				setBlockBounds(0.0F, 0.0F, 0.0F, 1.0F, 1.0F, bound);
		}
		else
			setBlockBounds(1.0F - bound, 0.0F, 0.0F, 1.0F, 1.0F, 1.0F);
	}
	else if (d == 3)
	{
		if (var4)
		{
			if (!var5)
				setBlockBounds(0.0F, 0.0F, 0.0F, bound, 1.0F, 1.0F);
			else
				setBlockBounds(1.0F - bound, 0.0F, 0.0F, 1.0F, 1.0F, 1.0F);
		}
		else
			setBlockBounds(0.0F, 0.0F, 1.0F - bound, 1.0F, 1.0F, 1.0F);
	}
}

void BlockDoor::onPoweredBlockChange(World* pWorld, const BlockPos& pos, bool powered)
{
	int meta = getFullMetadata(pWorld, pos);
	bool opened = (meta & 4) != 0;

	if (opened != powered)
	{
		int var8 = meta & 7;
		var8 ^= 4;

		if ((meta & 8) == 0)
		{
			pWorld->setBlockMetadataWithNotify(pos, var8, 2);
			pWorld->markBlockRangeForRenderUpdate(pos, pos);
		}
		else
		{
			pWorld->setBlockMetadataWithNotify(pos.getNegY(), var8, 2);
			pWorld->markBlockRangeForRenderUpdate(pos.getNegY(), pos);
		}
		// par1World.playAuxSFXAtEntity((EntityPlayer)null, 1003, par2, par3, par4, 0);
	}
}

bool BlockDoor::checkDoor(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer)
{
	const DoorSetting* pSetting = getDoorSetting();
	if (!pSetting)
		return true;

	if (isDoorOpen(pWorld, pos))
		return true;

	if (pSetting->teamId == 0 || pSetting->teamId == pPlayer->getTeamId())
		return true;

	auto inventory = pPlayer->inventory;
	if (!inventory)
		return false;

	auto keyPtr = inventory->getKeyById(pSetting->keyId);
	if (!keyPtr || keyPtr->stackSize < 1)
		return false;
	if (!pWorld->m_isClient)
	{
		if (keyPtr->stackSize <= 1)
		{
			int slot = inventory->findItemStack(keyPtr);
			if (slot == -1) {
				LordLogError("BlockDoor::checkDoor, findItemStack return -1");
				return false;
			}
			inventory->setInventorySlotContents(slot, nullptr);
			keyPtr = nullptr;
		}
		else
		{
			keyPtr->stackSize = keyPtr->stackSize - 1;
		}
	}
	return true;
}

void BlockDoor::switchDoorState(World* pWorld, const BlockPos & pos, bool isAutoClose)
{
	bool bIsDoorOpen = isDoorOpen(pWorld, pos);
	if (isAutoClose && !bIsDoorOpen)
		return;

	int meta = getFullMetadata(pWorld, pos);
	int var11 = meta & 7;
	var11 ^= 4;

	if ((meta & 8) == 0)
	{
		pWorld->setBlockMetadataWithNotify(pos, var11, 2);
		pWorld->markBlockRangeForRenderUpdate(pos, pos);
	}
	else
	{
		pWorld->setBlockMetadataWithNotify(pos.getNegY(), var11, 2);
		pWorld->markBlockRangeForRenderUpdate(pos.getNegY(), pos);
	}
	
	if (!isAutoClose && !pWorld->m_isClient)
	{
		const DoorSetting* pSetting = getDoorSetting();
		if (!pSetting || pSetting->closeTime < 0)
			return;

		if (bIsDoorOpen)
			pWorld->removeBlockDoor(pos);
		else
			pWorld->addBlockDoor(pos, pSetting->closeTime);
	}
}

void BlockDoor::addCollisionBoxesToList(World* pWorld, const BlockPos& pos, const Box& aabb, AABBList& lst, Entity* pEntity)
{
	if (auto entityPlayer = dynamic_cast<EntityPlayer*>(pEntity))
	{
		if (entityPlayer->capabilities.isWatchMode)
		{
			return;
		}
	}
	Block::addCollisionBoxesToList(pWorld, pos, aabb, lst, pEntity);
}

BlockSnowBlock::BlockSnowBlock(int id, const BM_Material& mat, const String& name)
	:Block(id, mat, name)
{
	setTickRandomly(true);
}

void BlockSnowBlock::updateTick(World* pWorld, const BlockPos& pos, Random& rand)
{	
	// todo.
	//if (par1World->getSavedLightValue(EnumSkyBlock.Block, par2, par3, par4) > 11)
	if(false)
	{
		dropBlockAsItem(pWorld, pos, pWorld->getBlockMeta(pos), 0);
		pWorld->setBlockToAir(pos);
	}
}

BlockCake::BlockCake(int id, const BM_Material& mat, const String& name)
	:Block(id, mat, name, false)
{
	setTickRandomly(true);
}

void BlockCake::setBlockBoundsBasedOnState(IBlockAccess* pBlockAccess, const BlockPos& pos)
{
	int meta = pBlockAccess->getBlockMeta(pos);
	float bound = 0.0625F;
	float beginX = (float)(1 + meta * 2) / 16.0F;
	float thickness = 0.5F;
	setBlockBounds(beginX, 0.0F, bound, 1.0F - bound, thickness, 1.0F - bound);
}

void BlockCake::setBlockBoundsForItemRender()
{
	float bound = 0.0625F;
	float thickness = 0.5F;
	setBlockBounds(bound, 0.0F, bound, 1.0F - bound, thickness, 1.0F - bound);
}

Box BlockCake::getCollisionBoundingBox(World* pWorld, const BlockPos& pos)
{
	int meta = pWorld->getBlockMeta(pos);
	float f1 = 0.0625F;
	float f2 = (float)(1 + meta * 2) / 16.0F;
	float f3 = 0.5F;
	return Box(pos.x + f2, (float)pos.y, pos.z + f1, pos.x + 1.f - f1, pos.y + f3 - f1, pos.z + 1.f - f1);
}

bool BlockCake::onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit)
{
	pPlayer->setActivatedBlock(pos);
	eatCakeSlice(pWorld, pos, pPlayer);
	return true;
}

void BlockCake::onBlockClicked(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer)
{
	eatCakeSlice(pWorld, pos, pPlayer);
}

bool BlockCake::canPlaceBlockAt(World* pWorld, const BlockPos& pos)
{
	return !Block::canPlaceBlockAt(pWorld, pos) ? false : canBlockStay(pWorld, pos);
}

void BlockCake::onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborBlockID)
{
	if (!canBlockStay(pWorld, pos))
	{
		pWorld->setBlockToAir(pos);
	}
}

bool BlockCake::canBlockStay(World* pWorld, const BlockPos& pos)
{
	return pWorld->getBlockMaterial(pos.getNegY()).isSolid();
}

void BlockCake::eatCakeSlice(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer)
{
	if (pPlayer->canEat(false))
	{
		pPlayer->getFoodStats()->addStats(2, 0.1F);
		int meta = pWorld->getBlockMeta(pos) + 1;

		if (meta >= 6)
		{
			pWorld->setBlockToAir(pos);
		}
		else
		{
			pWorld->setBlockMetadataWithNotify(pos, meta, 2);
		}
	}
}

BlockTrapDoor::BlockTrapDoor(int id, const BM_Material& mat, const String& name)
	:Block(id, mat, name, false)
{
	float bound = 0.5F;
	float thickness = 1.0F;
	setBlockBounds(0.5F - bound, 0.0F, 0.5F - bound, 0.5F + bound, thickness, 0.5F + bound);
}

bool BlockTrapDoor::getBlocksMovement(IBlockAccess* pAccess, const BlockPos& pos) const
{
	return !isTrapdoorOpen(pAccess->getBlockMeta(pos));
}

Box BlockTrapDoor::getCollisionBoundingBox(World* pWorld, const BlockPos& pos)
{
	setBlockBoundsBasedOnState(pWorld, pos);
	return Block::getCollisionBoundingBox(pWorld, pos);
}

void BlockTrapDoor::setBlockBoundsBasedOnState(IBlockAccess* pBlockAccess, const BlockPos& pos)
{
	float bound = 0.1875F;
	int meta = pBlockAccess->getBlockMeta(pos);
	if ((meta & 8) != 0)
	{
		setBlockBounds(0.0F, 1.0F - bound, 0.0F, 1.0F, 1.0F, 1.0F);
	}
	else
	{
		setBlockBounds(0.0F, 0.0F, 0.0F, 1.0F, bound, 1.0F);
	}

	if (isTrapdoorOpen(meta))
	{
		if ((meta & 3) == 0)
		{
			setBlockBounds(0.0F, 0.0F, 1.0F - bound, 1.0F, 1.0F, 1.0F);
		}

		if ((meta & 3) == 1)
		{
			setBlockBounds(0.0F, 0.0F, 0.0F, 1.0F, 1.0F, bound);
		}

		if ((meta & 3) == 2)
		{
			setBlockBounds(1.0F - bound, 0.0F, 0.0F, 1.0F, 1.0F, 1.0F);
		}

		if ((meta & 3) == 3)
		{
			setBlockBounds(0.0F, 0.0F, 0.0F, bound, 1.0F, 1.0F);
		}
	}
}

void BlockTrapDoor::setBlockBoundsForItemRender()
{
	float bound = 0.1875F;
	setBlockBounds(0.0F, 0.5F - bound / 2.0F, 0.0F, 1.0F, 0.5F + bound / 2.0F, 1.0F);
}

bool BlockTrapDoor::onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit)
{

	if (LogicSetting::Instance()->getGameType() == ClientGameType::BirdSimulator)
	{
		//int meta = pWorld->getBlockMeta(pos);
		//if (meta == 6)
		//{
		//	BirdNestExpandEvent::emit();
		//}
		//else
		//{
		//	BirdPackOpenEvent::emit();
		//}
		SCRIPT_EVENT::BirdSimulatorNestOperationEvent::invoke(pPlayer->getPlatformUserId(), pWorld->getBlockMeta(pos), pos);
		return true;
	}

	if (m_blockMaterial == BM_Material::BM_MAT_iron)
	{
		return true;
	}

	pPlayer->setActivatedBlock(pos);
	int meta = pWorld->getBlockMeta(pos);
	pWorld->setBlockMetadataWithNotify(pos, meta ^ 4, 2);
	// par1World.playAuxSFXAtEntity(par5EntityPlayer, 1003, par2, par3, par4, 0);
	return true;
}

void BlockTrapDoor::onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborBlockID)
{
	if (!pWorld->m_isClient)
	{
		int meta = pWorld->getBlockMeta(pos);
		BlockPos pos1 = pos;

		if ((meta & 3) == 0)
			pos1.z++;
		else if ((meta & 3) == 1)
			pos1.z--;
		else if ((meta & 3) == 2)
			pos1.x++;
		else if ((meta & 3) == 3)
			pos1.x--;

		if (!isValidSupportBlock(pWorld->getBlockId(pos1)))
		{
			pWorld->setBlockToAir(pos);
			dropBlockAsItem(pWorld, pos, meta, 0);
		}

		bool powered = pWorld->isBlockIndirectlyGettingPowered(pos);

		if (powered || neighborBlockID > 0 && BlockManager::sBlocks[neighborBlockID]->canProvidePower())
		{
			onPoweredBlockChange(pWorld, pos, powered);
		}
	}
}

RayTraceResult BlockTrapDoor::collisionRayTrace(World* pWorld, const BlockPos& pos, const Vector3& start, const Vector3& end)
{
	setBlockBoundsBasedOnState(pWorld, pos);

	Vector3 ls = start - Vector3(pos);
	Vector3 le = end - Vector3(pos);

	Vector3 vminx = ls.getIntermediateWithXValue(le, m_min.x);
	Vector3 vmaxx = ls.getIntermediateWithXValue(le, m_max.x);
	Vector3 vminy = ls.getIntermediateWithYValue(le, m_min.y);
	Vector3 vmaxy = ls.getIntermediateWithYValue(le, m_max.y);
	Vector3 vminz = ls.getIntermediateWithZValue(le, m_min.z);
	Vector3 vmaxz = ls.getIntermediateWithZValue(le, m_max.z);

	if (!isVecInsideYZBounds(vminx))
	{
		vminx = Vector3::INVALID;
	}

	if (!isVecInsideYZBounds(vmaxx))
	{
		vmaxx = Vector3::INVALID;
	}

	if (!isVecInsideXZBounds(vminy))
	{
		vminy = Vector3::INVALID;
	}

	if (!isVecInsideXZBounds(vmaxy))
	{
		vmaxy = Vector3::INVALID;
	}

	if (!isVecInsideXYBounds(vminz))
	{
		vminz = Vector3::INVALID;
	}

	if (!isVecInsideXYBounds(vmaxz))
	{
		vmaxz = Vector3::INVALID;
	}

	Vector3 var13 = Vector3::INVALID;

	if (vminx != Vector3::INVALID && (var13 == Vector3::INVALID || ls.squareDistanceTo(vminx) < ls.squareDistanceTo(var13)))
	{
		var13 = vminx;
	}

	if (vmaxx != Vector3::INVALID && (var13 == Vector3::INVALID || ls.squareDistanceTo(vmaxx) < ls.squareDistanceTo(var13)))
	{
		var13 = vmaxx;
	}

	if (vminy != Vector3::INVALID && (var13 == Vector3::INVALID || ls.squareDistanceTo(vminy) < ls.squareDistanceTo(var13)))
	{
		var13 = vminy;
	}

	if (vmaxy != Vector3::INVALID && (var13 == Vector3::INVALID || ls.squareDistanceTo(vmaxy) < ls.squareDistanceTo(var13)))
	{
		var13 = vmaxy;
	}

	if (vminz != Vector3::INVALID && (var13 == Vector3::INVALID || ls.squareDistanceTo(vminz) < ls.squareDistanceTo(var13)))
	{
		var13 = vminz;
	}

	if (vmaxz != Vector3::INVALID && (var13 == Vector3::INVALID || ls.squareDistanceTo(vmaxz) < ls.squareDistanceTo(var13)))
	{
		var13 = vmaxz;
	}

	RayTraceResult result;
	if (var13 == Vector3::INVALID)
	{
		return result;
	}

	Facing* facing = NULL; // D - U - N - S - W - E

	if (var13 == vminx)
	{
		facing = Facing::WEST;
	}

	if (var13 == vmaxx)
	{
		facing = Facing::EAST;
	}

	if (var13 == vminy)
	{
		facing = Facing::DOWN;
	}

	if (var13 == vmaxy)
	{
		facing = Facing::UP;
	}

	if (var13 == vminz)
	{
		facing = Facing::NORTH;
	}

	if (var13 == vmaxz)
	{
		facing = Facing::SOUTH;
	}

	result.set(var13 + Vector3(pos), facing, pos);
	return result;
}

int BlockTrapDoor::onBlockPlaced(World* pWorld, const BlockPos& pos, int side, const Vector3& hit, int meta)
{
	int ret = 0;

	if (side == 2)
		ret = 0;

	if (side == 3)
		ret = 1;

	if (side == 4)
		ret = 2;

	if (side == 5)
		ret = 3;

	if (side != 1 && side != 0 && meta > 0.5F)
		ret |= 8;

	return ret;
}

bool BlockTrapDoor::canPlaceBlockOnSide(World* pWorld, const BlockPos& pos, int side)
{
	if (side == 0 || side == 1)
	{
		return false;
	}
	BlockPos pos1 = pos;
	if (side == 2)
		++pos1.z;
	else if (side == 3)
		--pos1.z;
	else if (side == 4)
		++pos1.x;
	else if (side == 5)
		--pos1.x;

	return isValidSupportBlock(pWorld->getBlockId(pos1));
}

bool BlockTrapDoor::isValidSupportBlock(int blockID)
{
	if (blockID <= 0)
	{
		return false;
	}
	else
	{
		Block* pBlock = BlockManager::sBlocks[blockID];
		return pBlock != NULL && pBlock->getMaterial().isOpaque() && pBlock->renderAsNormalBlock() || pBlock->getBlockID() == BLOCK_ID_GLOW_STONE; //|| pBlock instanceof BlockHalfSlab || pBlock instanceof BlockStairs;		
	}
}

void BlockTrapDoor::onPoweredBlockChange(World* pWorld, const BlockPos& pos, bool powered)
{
	int meta = pWorld->getBlockMeta(pos);
	bool opend = (meta & 4) > 0;

	if (opend != powered)
	{
		pWorld->setBlockMetadataWithNotify(pos, meta ^ 4, 2);
		// par1World.playAuxSFXAtEntity((EntityPlayer)null, 1003, par2, par3, par4, 0);
	}
}

void BlockTrapDoor::addCollisionBoxesToList(World* pWorld, const BlockPos& pos, const Box& aabb, AABBList& lst, Entity* pEntity)
{
	if (auto entityPlayer = dynamic_cast<EntityPlayer*>(pEntity))
	{
		if (entityPlayer->capabilities.isWatchMode)
		{
			return;
		}
	}
	Block::addCollisionBoxesToList(pWorld, pos, aabb, lst, pEntity);
}

StringArray BlockStoneBrick::STONE_BRICK_TYPES = { "default", "mossy", "cracked", "chiseled" };

BlockStoneBrick::BlockStoneBrick(int id, const BM_Material& mat, const String& name)
	:Block(id, mat, name)
{
}

BlockMushroomCap::BlockMushroomCap(int id, const BM_Material& mat, const String& name, int mushroomType)
	:Block(id, mat, name)
{
	m_mushroomType = mushroomType;
}

int BlockMushroomCap::quantityDropped(Random& rand)
{
	int num = rand.nextInt(10) - 7;	

	if (num < 0)
	{
		num = 0;
	}

	return num;
}

BlockMelon::BlockMelon(int id, const BM_Material& mat, const String& name)
	:Block(id, mat, name)
{
}

int BlockMelon::quantityDroppedWithBonus(int fortune, Random& rand)
{
	int num = quantityDropped(rand) + rand.nextInt(1 + fortune);

	if (num > 9)
	{
		num = 9;
	}

	return num;
}

BlockOre::BlockOre(int id, const BM_Material& mat, const String& name)
	:Block(id, mat, name)
{
}

int BlockOre::idDropped(int meta, Random& rand, int fortune)
{
	if (SCRIPT_EVENT::BlockOreDroppedEvent::invoke(m_blockID))
	{
		switch (m_blockID)
		{
		case BLOCK_ID_ORE_COAL:
			return ITEM_ID_COAL;
		case BLOCK_ID_ORE_DIAMOND:
			return ITEM_ID_DIAMOND;
		case BLOCK_ID_ORE_LAPIS:
			return ITEM_ID_DYE_POWER;
		case BLOCK_ID_ORE_EMERALD:
			return ITEM_ID_EMERALA;
		case BLOCK_ID_ORE_NETHER_QUARTZ:
			return ITEM_ID_NETHER_QUARTZ;
		default:
			return m_blockID;
		}
	}
	else
	{
		return m_blockID;
	}
}

int BlockOre::quantityDropped(Random& rand)
{
	if (SCRIPT_EVENT::BlockOreDroppedEvent::invoke(m_blockID))
	{
		return m_blockID == BLOCK_ID_ORE_LAPIS ? 4 + rand.nextInt(5) : 1;
	}
	else
	{
		return 1;
	}
}

int BlockOre::quantityDroppedWithBonus(int fortune, Random& rand)
{
	if (fortune > 0 && m_blockID != idDropped(0, rand, fortune))
	{
		int num = rand.nextInt(fortune + 2) - 1;

		if (num < 0)
		{
			num = 0;
		}

		return quantityDropped(rand) * (num + 1);
	}
	else
	{
		return quantityDropped(rand);
	}
}

void BlockOre::dropBlockAsItemWithChance(World* pWorld, const BlockPos& pos, int meta, float chance, int fortune)
{
	Block::dropBlockAsItemWithChance(pWorld, pos, meta, chance, fortune);

	if (idDropped(meta, pWorld->m_Rand, fortune) != m_blockID)
	{
		int exp = 0;

		if (m_blockID == BLOCK_ID_ORE_COAL)
		{
			exp = pWorld->m_Rand.nextIntRange(0, 2);
		}
		else if (m_blockID == BLOCK_ID_ORE_DIAMOND)
		{
			exp = pWorld->m_Rand.nextIntRange(3, 7);
		}
		else if (m_blockID == BLOCK_ID_ORE_EMERALD)
		{
			exp = pWorld->m_Rand.nextIntRange(3, 7);
		}
		else if (m_blockID == BLOCK_ID_ORE_LAPIS)
		{
			exp = pWorld->m_Rand.nextIntRange(2, 5);
		}
		else if (m_blockID == BLOCK_ID_ORE_NETHER_QUARTZ)
		{
			exp = pWorld->m_Rand.nextIntRange(2, 5);
		}

		dropXpOnBlockBreak(pWorld, pos, exp);
	}
}

BlockOreStorage::BlockOreStorage(int id, const BM_Material& mat, const String& name)
	:Block(id, mat, name)
{
}

BlockPumpkin::BlockPumpkin(int id, const BM_Material& mat, const String& name, bool type)
	:BlockDirectional(id, mat, name, true)
{
	setTickRandomly(true);
	m_blockType = type;
}

void BlockPumpkin::onBlockAdded(World* pWorld, const BlockPos& pos)
{
	BlockDirectional::onBlockAdded(pWorld, pos);

	BlockPos negY = pos.getNegY();
	BlockPos negY2 = negY.getNegY();
	if (pWorld->getBlockId(negY) == BLOCK_ID_BLOCK_SNOW && pWorld->getBlockId(negY2) == BLOCK_ID_BLOCK_SNOW)
	{
		if (!pWorld->m_isClient)
		{
			pWorld->setBlock(pos, 0, 0, 2);
			pWorld->setBlock(negY, 0, 0, 2);
			pWorld->setBlock(negY2, 0, 0, 2);
			//EntitySnowman var9 = new EntitySnowman(pBlock);
			//var9.setLocationAndAngles((double)x + 0.5D, (double)y - 1.95D, (double)z + 0.5D, 0.0F, 0.0F);
			//pBlock.spawnEntityInWorld(var9);
			pWorld->notifyBlockChange(pos, 0);
			pWorld->notifyBlockChange(negY, 0);
			pWorld->notifyBlockChange(negY2, 0);
		}

		for (int i = 0; i < 120; ++i)
		{
			Vector3 spawn;
			spawn.x = pos.x + pWorld->m_Rand.nextFloat();
			spawn.y = pos.y - 2.f + pWorld->m_Rand.nextFloat() * 2.5F;
			spawn.z = pos.z + pWorld->m_Rand.nextFloat();
			pWorld->spawnParticle("snowshovel", spawn, Vector3::ZERO);
		}
	}
	else if (pWorld->getBlockId(negY) == BLOCK_ID_BLOCK_IRON && pWorld->getBlockId(negY2) == BLOCK_ID_BLOCK_IRON)
	{
		BlockPos negX = negY.getNegX();
		BlockPos negZ = negY.getNegZ();
		BlockPos posX = negY.getPosX();
		BlockPos posZ = negY.getPosZ();
		bool flag1 = pWorld->getBlockId(negX) == BLOCK_ID_BLOCK_IRON && pWorld->getBlockId(posX) == BLOCK_ID_BLOCK_IRON;
		bool flag2 = pWorld->getBlockId(negZ) == BLOCK_ID_BLOCK_IRON && pWorld->getBlockId(posZ) == BLOCK_ID_BLOCK_IRON;

		if (flag1 || flag2)
		{
			pWorld->setBlock(pos, 0, 0, 2);
			pWorld->setBlock(negY, 0, 0, 2);
			pWorld->setBlock(negY2, 0, 0, 2);

			if (flag1)
			{
				pWorld->setBlock(negX, 0, 0, 2);
				pWorld->setBlock(posX, 0, 0, 2);
			}
			else
			{
				pWorld->setBlock(negZ, 0, 0, 2);
				pWorld->setBlock(posZ, 0, 0, 2);
			}

			/*	EntityIronGolem var7 = new EntityIronGolem(pBlock);
				var7.setPlayerCreated(true);
				var7.setLocationAndAngles((double)x + 0.5D, (double)y - 1.95D, (double)z + 0.5D, 0.0F, 0.0F);
				pBlock->spawnEntityInWorld(var7);*/

			for (int i = 0; i < 120; ++i)
			{
				Vector3 spawn;
				spawn.x = pos.x + pWorld->m_Rand.nextFloat();
				spawn.y = pos.y - 2.f + pWorld->m_Rand.nextFloat() * 3.9F;
				spawn.z = pos.z + pWorld->m_Rand.nextFloat();
				pWorld->spawnParticle("snowballpoof", spawn, Vector3::ZERO);
			}

			pWorld->notifyBlockChange(pos, 0);
			pWorld->notifyBlockChange(negY, 0);
			pWorld->notifyBlockChange(negY2, 0);

			if (flag1)
			{
				pWorld->notifyBlockChange(negX, 0);
				pWorld->notifyBlockChange(posX, 0);
			}
			else
			{
				pWorld->notifyBlockChange(negZ, 0);
				pWorld->notifyBlockChange(posZ, 0);
			}
		}
	}
}

bool BlockPumpkin::canPlaceBlockAt(World* pWorld, const BlockPos& pos)
{
	int blockID = pWorld->getBlockId(pos);
	
	if (blockID == 0 || BlockManager::sBlocks[blockID]->getMaterial().isReplaceable())
	{
		return pWorld->doesBlockHaveSolidTopSurface(pos.getNegY());
	}	
	else
	{
		return false;
	}	
}

void BlockPumpkin::onBlockPlacedBy(World* pWorld, const BlockPos& pos, EntityLivingBase* pLiving, ItemStackPtr pStack)
{
	int meta = int(Math::Floor((double)(pLiving->rotationYaw * 4.0F / 360.0F) + 2.5f)) & 3;
	pWorld->setBlockMetadataWithNotify(pos, meta, 2);
}



BlockCommandBlock::BlockCommandBlock(int id, const BM_Material& mat, const String& name)
	:BlockContainer(id, mat, name, true)
{}

TileEntity* BlockCommandBlock::createNewTileEntity(World* pWorld)
{
	return LordNew TileEntityCommandBlock();
}

void BlockCommandBlock::onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborBlockID)
{
	if (!pWorld->m_isClient)
	{
		bool bPower = pWorld->isBlockIndirectlyGettingPowered(pos);
		int meta = pWorld->getBlockMeta(pos);
		bool bOpen = (meta & 1) != 0;

		if (bPower && !bOpen)
		{
			pWorld->setBlockMetadataWithNotify(pos, meta | 1, 4);
			pWorld->scheduleBlockUpdate(pos, m_blockID, tickRate(pWorld));
		}
		else if (!bPower && bOpen)
		{
			pWorld->setBlockMetadataWithNotify(pos, meta & -2, 4);
		}
	}
}

void BlockCommandBlock::updateTick(World* pWorld, const BlockPos& pos, Random& rand)
{
	TileEntity* pTileEntity = pWorld->getBlockTileEntity(pos);

	// todo.
	/*
	if (pTileEntity && pTileEntity instanceof TileEntityCommandBlock)
	{
		TileEntityCommandBlock var7 = (TileEntityCommandBlock)pTileEntity;
		var7.func_96102_a(var7.executeCommandOnPowered(par1World));
		par1World.func_96440_m(par2, par3, par4, blockID);
	}
	*/
}

bool BlockCommandBlock::onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit)
{
	// todo.
	// nothing todo in logic side.
	// will call the client side to show the GUI.
	pPlayer->setActivatedBlock(pos);
	return true;
}

int BlockCommandBlock::getComparatorInputOverride(World* pWorld, const BlockPos& pos, int side)
{
	TileEntity* ptileEntity = pWorld->getBlockTileEntity(pos);

	// todo.
	// return ptileEntity != null && ptileEntity instanceof TileEntityCommandBlock ? ((TileEntityCommandBlock)ptileEntity).func_96103_d() : 0;
	return 0;
}

void BlockCommandBlock::onBlockPlacedBy(World* pWorld, const BlockPos& pos, EntityLivingBase* pLiving, ItemStackPtr pStack)
{
	// todo.
	/*
	TileEntityCommandBlock var7 = (TileEntityCommandBlock)par1World.getBlockTileEntity(par2, par3, par4);

	if (par6ItemStack.hasDisplayName())
	{
		var7.setCommandSenderName(par6ItemStack.getDisplayName());
	}
	*/
}

BlockDaylightDetector::BlockDaylightDetector(int id, const BM_Material& mat, const String& name)
	:BlockContainer(id, mat, name, false)
{
	m_isOpaqueCube = false;
	setBlockBounds(0.0F, 0.0F, 0.0F, 1.0F, 0.375F, 1.0F);

	//add by maxicheng
	setLightOpacity(0);
}

int BlockDaylightDetector::isProvidingWeakPower(IBlockAccess* piBlockAccess, const BlockPos& pos, int side)
{
	return piBlockAccess->getBlockMeta(pos);
}

TileEntity* BlockDaylightDetector::createNewTileEntity(World* pWorld)
{
	return LordNew TileEntityDaylightDetector();
}

BlockDispenser::BlockDispenser(int id, const BM_Material& mat, const String& name)
	:BlockContainer(id, mat, name, true)
{
	m_pRand = LordNew Random();
}

BlockDispenser::~BlockDispenser() {
	LordSafeDelete(m_pRand);
}

void BlockDispenser::onBlockAdded(World* pWorld, const BlockPos& pos)
{
	BlockContainer::onBlockAdded(pWorld, pos);
	//setDispenserDefaultDirection(pWorld, x, y, z);
}

bool BlockDispenser::onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit)
{
	if (pWorld->m_isClient)
		return true;

	pPlayer->setActivatedBlock(pos);
	TileEntityDispenser* dispenser = (TileEntityDispenser*)pWorld->getBlockTileEntity(pos);
	if (dispenser)
	{
		// todo.
		// dispenser->displayGUIDispenser(dispenser);
	}

	return true;
}

void BlockDispenser::onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborID)
{
	bool var6 = pWorld->isBlockIndirectlyGettingPowered(pos) || pWorld->isBlockIndirectlyGettingPowered(pos.getPosY());
	int meta = pWorld->getBlockMeta(pos);
	bool var8 = (meta & 8) != 0;

	if (var6 && !var8)
	{
		pWorld->scheduleBlockUpdate(pos, m_blockID, tickRate(pWorld));
		pWorld->setBlockMetadataWithNotify(pos, meta | 8, 4);
	}
	else if (!var6 && var8)
	{
		pWorld->setBlockMetadataWithNotify(pos, meta & -9, 4);
	}
}

void BlockDispenser::updateTick(World* pWorld, const BlockPos& pos, Random& rand)
{
	if (!pWorld->m_isClient)
	{
		//dispense(pWorld, x, y, z);
	}
}

void BlockDispenser::onBlockPlacedBy(World* pWorld, const BlockPos& pos, EntityLivingBase* pLiving, ItemStackPtr pStack)
{
	int var7 = BlockPistonBase::determineOrientation(pWorld, pos, pLiving);
	pWorld->setBlockMetadataWithNotify(pos, var7, 2);

	if (pStack->hasDisplayName())
	{
		TileEntityDispenser* despenser = (TileEntityDispenser*)pWorld->getBlockTileEntity(pos);
		despenser->setCustomName(pStack->getDisplayName());
	}
}

void BlockDispenser::breakBlock(World* pWorld, const BlockPos& pos, int blockID, int meta)
{
	TileEntityDispenser* despenser = (TileEntityDispenser*)pWorld->getBlockTileEntity(pos);

	if (despenser)
	{
		for (int i = 0; i < despenser->getSizeInventory(); ++i)
		{
			ItemStackPtr pStack = despenser->getStackInSlot(i);

			if (!pStack)
				continue;

			if (!SCRIPT_EVENT::EntityItemSpawnEvent::invoke(pStack->itemID, pStack->itemDamage, "break.despenser.drop"))
				continue;

			Vector3 fvel = m_pRand->nextVector() * 0.8f + 0.1f;

			while (pStack->stackSize > 0)
			{
				int rs = m_pRand->nextInt(21) + 10;

				if (rs > pStack->stackSize)
				{
					rs = pStack->stackSize;
				}

				pStack->stackSize -= rs;

				EntityItem* pItem = LordNew EntityItem(pWorld, Vector3(pos) + fvel,
					LORD::make_shared<ItemStack>(pStack->itemID, rs, pStack->getItemDamage()));

				if (pStack->hasTagCompound())
				{
					pItem->getEntityItem()->setTagCompound((NBTTagCompound*)pStack->getTagCompound()->copy());
				}

				float scale = 0.05F;
				pItem->motion = m_pRand->nextGaussianVec() * scale + Vector3(0.f, 0.2f, 0.f);
				pWorld->spawnEntityInWorld(pItem);
			}
		}

		pWorld->updateComparatorOutputLevel(pos, blockID);
	}

	Block::breakBlock(pWorld, pos, blockID, meta);
}

int BlockDispenser::getComparatorInputOverride(World* pWorld, const BlockPos& pos, int side)
{
	IInventory* pInventory = (IInventory*)pWorld->getBlockTileEntity(pos);
	return Container::calcRedstoneFromInventory(pInventory);
}

Vector3 BlockDispenser::getIPositionFromBlockSource(const BlockPos& pos)
{
	// todo.
	/*Facing* var1 = getFacing(pos.getBlockMeta());
	double var2 = par0IBlockSource.getX() + 0.7D * (double)var1.getFrontOffsetX();
	double var4 = par0IBlockSource.getY() + 0.7D * (double)var1.getFrontOffsetY();
	double var6 = par0IBlockSource.getZ() + 0.7D * (double)var1.getFrontOffsetZ();
	return new PositionImpl(var2, var4, var6);*/
	return Vector3::ZERO;
}

Facing* BlockDispenser::getFacing(int meta)
{
	return Facing::VALUES[(meta & 7) % 6];
}

TileEntity* BlockDispenser::createNewTileEntity(World* pWorld)
{
	return LordNew TileEntityDispenser();
}

BlockEnchantmentTable::BlockEnchantmentTable(int id, const BM_Material& mat, const String& name)
	:BlockContainer(id, mat, name, false)
{
	m_isOpaqueCube = false;
	setBlockBounds(0.0F, 0.0F, 0.0F, 1.0F, 0.75F, 1.0F);
	setLightOpacity(0);
}

bool BlockEnchantmentTable::onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit)
{
	if (pWorld->m_isClient)
		return true;

	pPlayer->setActivatedBlock(pos);
	// todo.
	// in logic side, no need to do more things. in client side show the GUI.
	/*
	TileEntityEnchantmentTable var10 = (TileEntityEnchantmentTable)par1World.getBlockTileEntity(par2, par3, par4);
	par5EntityPlayer.displayGUIEnchantment(par2, par3, par4, var10.func_94135_b() ? var10.func_94133_a() : null);
	return true;
	*/
	return true;
}

void BlockEnchantmentTable::onBlockPlacedBy(World* pWorld, const BlockPos& pos, EntityLivingBase* pLiving, ItemStackPtr pStack)
{
	Block::onBlockPlacedBy(pWorld, pos, pLiving, pStack);

	if (pStack->hasDisplayName())
	{
		// todo.
		// ((TileEntityEnchantmentTable)par1World.getBlockTileEntity(par2, par3, par4)).func_94134_a(par6ItemStack.getDisplayName());
	}
}

TileEntity* BlockEnchantmentTable::createNewTileEntity(World* pWorld)
{
	return new TileEntityEnchantmentTable();
}

bool BlockFurnace::keepFurnaceInventory = false;

BlockFurnace::BlockFurnace(int id, const BM_Material& mat, const String& name, bool isActive)
	:BlockContainer(id, mat, name, true)
{
	m_isActive = isActive;
}

void BlockFurnace::onBlockAdded(World* pWorld, const BlockPos& pos)
{
	BlockContainer::onBlockAdded(pWorld, pos);
	
	//error
	//setDefaultDirection(pWorld, x, y, z);
}

bool BlockFurnace::onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit)
{
	if (pWorld->m_isClient)
		return true;

	pPlayer->setActivatedBlock(pos);
	TileEntityFurnace* pFurnace = (TileEntityFurnace*)pWorld->getBlockTileEntity(pos);

	if (pFurnace)
	{
		// todo.
		// on the client side show the GUI for furnace.
		// par5EntityPlayer.displayGUIFurnace(pFurnace);
	}

	return true;
}

void BlockFurnace::onBlockPlacedBy(World* pWorld, const BlockPos& pos, EntityLivingBase* pLiving, ItemStackPtr pStack)
{
	int dir = int(Math::Floor((pLiving->rotationYaw * 4.0F / 360.0F) + 0.5f)) & 3;

	if (dir == 0)
		pWorld->setBlockMetadataWithNotify(pos, 2, 2);

	if (dir == 1)
		pWorld->setBlockMetadataWithNotify(pos, 5, 2);

	if (dir == 2)
		pWorld->setBlockMetadataWithNotify(pos, 3, 2);

	if (dir == 3)
		pWorld->setBlockMetadataWithNotify(pos, 4, 2);

	if (pStack->hasDisplayName())
	{
		TileEntityFurnace* pFurnace = (TileEntityFurnace*)pWorld->getBlockTileEntity(pos);
		// todo.
		// setGuiDisplayName(par6ItemStack.getDisplayName());
	}
}

TileEntity* BlockFurnace::createNewTileEntity(World* pWorld)
{
	return LordNew TileEntityFurnace();
}

void BlockFurnace::breakBlock(World* pWorld, const BlockPos& pos, int blockID, int meta)
{
	
	if (!keepFurnaceInventory)
	{
		TileEntityFurnace* pFurnace = (TileEntityFurnace*)pWorld->getBlockTileEntity(pos);

		if (pFurnace != NULL)
		{
			for (int i = 0; i < pFurnace->getSizeInventory(); ++i)
			{
				ItemStackPtr pStack = pFurnace->getStackInSlot(i);

				if (pStack != NULL)
				{

					if (!SCRIPT_EVENT::EntityItemSpawnEvent::invoke(pStack->itemID, pStack->itemDamage, "break.furnace.drop"))
						continue;

					Vector3 offset = m_furnaceRand.nextVector() * 0.8F + 0.1F;

					while (pStack->stackSize > 0)
					{
						int randsize = m_furnaceRand.nextInt(21) + 10;

						if (randsize > pStack->stackSize)
						{
							randsize = pStack->stackSize;
						}

						pStack->stackSize -= randsize;					
						Vector3 spawnpos = Vector3(pos) + offset;
						EntityItem* pItem = LordNew EntityItem(pWorld, spawnpos, LORD::make_shared<ItemStack>(pStack->itemID, randsize, pStack->getItemDamage()));

						if (pStack->hasTagCompound())
						{							
							pItem->getEntityItem()->setTagCompound((NBTTagCompound*)pStack->getTagCompound()->copy());
						}

						float var15 = 0.05F;
						pItem->motion = m_furnaceRand.nextGaussianVec() * var15 + Vector3(0.f, 0.2f, 0.f);
						pWorld->spawnEntityInWorld(pItem);						
					}
				}
			}

			pWorld->updateComparatorOutputLevel(pos, blockID);
		}
	}

	BlockContainer::breakBlock(pWorld, pos, blockID, meta);
}

int BlockFurnace::getComparatorInputOverride(World* pWorld, const BlockPos& pos, int side)
{
	IInventory* pInventory = (IInventory*)pWorld->getBlockTileEntity(pos);
	return Container::calcRedstoneFromInventory(pInventory);
}

void BlockFurnace::updateFurnaceBlockState(bool burning, World* pWorld, const BlockPos& pos)
{
	int meta = pWorld->getBlockMeta(pos);
	TileEntity* pTileEntity = pWorld->getBlockTileEntity(pos);
	keepFurnaceInventory = true;

	if (burning)
	{
		pWorld->setBlock(pos, BLOCK_ID_FURNACE_IDLE);
	}
	else
	{
		pWorld->setBlock(pos, BLOCK_ID_FURNACE_IDLE);
	}

	keepFurnaceInventory = false;
	pWorld->setBlockMetadataWithNotify(pos, meta, 2);

	if (pTileEntity)
	{
		pTileEntity->validate();
		pWorld->setBlockTileEntity(pos, pTileEntity);
	}
}

void BlockFurnace::setDefaultDirection(World* pWorld, const BlockPos& pos)
{
	if (!pWorld->m_isClient)
	{
		int northID = pWorld->getBlockId(pos.getNegZ());
		int southID = pWorld->getBlockId(pos.getPosZ());
		int westID = pWorld->getBlockId(pos.getNegX());
		int eastID = pWorld->getBlockId(pos.getPosX());
		int meta = 3;

		const i8* opaque = BlockManager::sOpaqueCubeLookup;
		if (opaque[northID] && !opaque[southID])
			meta = 3;

		if (opaque[southID] && !opaque[northID])
			meta = 2;

		if (opaque[westID] && !opaque[eastID])
			meta = 5;

		if (opaque[eastID] && !opaque[westID])
			meta = 4;

		pWorld->setBlockMetadataWithNotify(pos, meta, 2);
	}
}

BlockJukeBox::BlockJukeBox(int id, const BM_Material& mat, const String& name)
	:BlockContainer(id, mat, name, true)
{
}

TileEntity* BlockJukeBox::createNewTileEntity(World* pWorld)
{
	return LordNew TileEntityRecordPlayer();
}

bool BlockJukeBox::onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit)
{
	if (pWorld->getBlockMeta(pos) == 0)
		return false;
	pPlayer->setActivatedBlock(pos);
	ejectRecord(pWorld, pos);
	return true;
}

void BlockJukeBox::breakBlock(World* pWorld, const BlockPos& pos, int blockID, int meta)
{
	ejectRecord(pWorld, pos);
	Block::breakBlock(pWorld, pos, blockID, meta);
}

void BlockJukeBox::dropBlockAsItemWithChance(World* pWorld, const BlockPos& pos, int meta, float chance, int fortune)
{
	if (!pWorld->m_isClient)
	{
		Block::dropBlockAsItemWithChance(pWorld, pos, meta, chance, 0);
	}
}

int BlockJukeBox::getComparatorInputOverride(World* pWorld, const BlockPos& pos, int side)
{
	// todo.
	// ItemStackPtr pStack = ((TileEntityRecordPlayer)par1World.getBlockTileEntity(par2, par3, par4)).func_96097_a();
	// return pStack == null ? 0 : pStack.itemID + 1 - Item.record13.itemID;
	return 0;
}

void BlockJukeBox::insertRecord(World* pWorld, const BlockPos& pos, ItemStackPtr pStack)
{
	if (pWorld->m_isClient)
		return;
	
	// todo.
	/*TileEntityRecordPlayer pRecordPlayer = (TileEntityRecordPlayer)par1World.getBlockTileEntity(par2, par3, par4);

	if (pRecordPlayer != null)
	{
		pRecordPlayer.func_96098_a(par5ItemStack.copy());
		par1World.setBlockMetadataWithNotify(par2, par3, par4, 1, 2);
	}*/
}

void BlockJukeBox::ejectRecord(World* pWorld, const BlockPos& pos)
{
	if (pWorld->m_isClient)
		return;
	
	// todo.
	/*
	TileEntityRecordPlayer pRecordPlayer = (TileEntityRecordPlayer)par1World.getBlockTileEntity(par2, par3, par4);

	if (!pRecordPlayer)
		return;

	ItemStack var6 = pRecordPlayer.func_96097_a();

	if (var6 != null)
	{
		par1World.playAuxSFX(1005, par2, par3, par4, 0);
		par1World.playRecord((String)null, par2, par3, par4);
		pRecordPlayer.func_96098_a((ItemStack)null);
		par1World.setBlockMetadataWithNotify(par2, par3, par4, 0, 2);
		float var7 = 0.7F;
		double var8 = (double)(par1World.rand.nextFloat() * var7) + (double)(1.0F - var7) * 0.5D;
		double var10 = (double)(par1World.rand.nextFloat() * var7) + (double)(1.0F - var7) * 0.2D + 0.6D;
		double var12 = (double)(par1World.rand.nextFloat() * var7) + (double)(1.0F - var7) * 0.5D;
		ItemStack var14 = var6.copy();
		EntityItem var15 = new EntityItem(par1World, (double)par2 + var8, (double)par3 + var10, (double)par4 + var12, var14);
		var15.delayBeforeCanPickup = 10;
		par1World.spawnEntityInWorld(var15);
	}
	*/
}

BlockMobSpawner::BlockMobSpawner(int id, const BM_Material& mat, const String& name)
	:BlockContainer(id, mat, name, false)
{
	m_isOpaqueCube = false;	
}

TileEntity* BlockMobSpawner::createNewTileEntity(World* pWorld)
{
	return LordNew TileEntityMobSpawner();
}

void BlockMobSpawner::dropBlockAsItemWithChance(World* pWorld, const BlockPos& pos, int meta, float chance, int fortune)
{
	Block::dropBlockAsItemWithChance(pWorld, pos, meta, chance, fortune);
	int exp = 15 + pWorld->m_Rand.nextInt(15) + pWorld->m_Rand.nextInt(15);
	dropXpOnBlockBreak(pWorld, pos, exp);
}

BlockNote::BlockNote(int id, const BM_Material& mat, const String& name)
	:BlockContainer(id, mat, name, true)
{
}

TileEntity* BlockNote::createNewTileEntity(World* pWorld)
{
	return LordNew TileEntityNote();
}

void BlockNote::onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborBlockID)
{
	// todo.
	/*
	bool var6 = pWorld->isBlockIndirectlyGettingPowered(x, y, z);
	TileEntityNote* var7 = (TileEntityNote*)pWorld->getBlockTileEntity(x, y, z);

	if (var7 != NULL && var7.previousRedstoneState != var6)
	{
		if (var6)
		{
			var7->triggerNote(pWorld, x, y, z);
		}

		var7->previousRedstoneState = var6;
	}
	*/
}

bool BlockNote::onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit)
{
	if (pWorld->m_isClient)
		return true;

	pPlayer->setActivatedBlock(pos);
	// todo.
	/*TileEntityNote pNote = (TileEntityNote)par1World.getBlockTileEntity(par2, par3, par4);

	if (pNote != null)
	{
		pNote->changePitch();
		pNote->triggerNote(par1World, par2, par3, par4);
	}*/

	return true;
}

void BlockNote::onBlockClicked(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer)
{
	if (!pWorld->m_isClient)
	{
		// todo.
		/*TileEntityNote var6 = (TileEntityNote*)pWorld->getBlockTileEntity(pos);

		if (var6 != null)
		{
			var6.triggerNote(par1World, par2, par3, par4);
		}*/
	}
}

bool BlockNote::onBlockEventReceived(World* pWorld, const BlockPos& pos, int id, int param)
{
	// in client side, play sound and spawn particles.
	return true;
}

BlockStairs::BlockStairs(int id, const BM_Material& mat, const String& name, Block* pBlock, int meta)
	:Block(id, mat, name, false)
{
	LordAssert(pBlock);
	m_modelBlock = pBlock;
	m_modelBlockMeta = meta;
	//setHardness(par2Block.blockHardness);
	//setResistance(par2Block.blockResistance / 3.0F);
	//setStepSound(par2Block.stepSound);
	setLightOpacity(0);
}

void BlockStairs::addCollisionBoxesToList(World* pWorld, const BlockPos& pos, const Box& aabb, AABBList& lst, Entity* pEntity)
{
	getCollSlabBouding(pWorld, pos);
	Block::addCollisionBoxesToList(pWorld, pos, aabb, lst, pEntity);
	bool var8 = getCollQuarterBouding(pWorld, pos);
	Block::addCollisionBoxesToList(pWorld, pos, aabb, lst, pEntity);

	if (var8 && getCollEighthBouding(pWorld, pos))
	{
		Block::addCollisionBoxesToList(pWorld, pos, aabb, lst, pEntity);
	}

	setBlockBounds(0.0F, 0.0F, 0.0F, 1.0F, 1.0F, 1.0F);
}

void BlockStairs::onBlockClicked(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer)
{
	m_modelBlock->onBlockClicked(pWorld, pos, pPlayer);
}

void BlockStairs::onBlockDestroyedByPlayer(World* pWorld, const BlockPos& pos, int meta)
{
	m_modelBlock->onBlockDestroyedByPlayer(pWorld, pos, meta);
}

float BlockStairs::getExplosionResistance(Entity* pEntity)
{
	return m_modelBlock->getExplosionResistance(pEntity);
}

Vector3 BlockStairs::velocityToAddToEntity(World* pWorld, const BlockPos& pos, Entity* pEntity, const Vector3& vec)
{
	return m_modelBlock->velocityToAddToEntity(pWorld, pos, pEntity, vec);
}

void BlockStairs::setBlockBoundsBasedOnState(IBlockAccess* pBlockAccess, const BlockPos& pos)
{
	if (m_rayTracyed)
	{
		setBlockBounds(0.5F * (float)(m_rayTracyCount % 2), 
			0.5F * (float)(m_rayTracyCount / 2 % 2), 
			0.5F * (float)(m_rayTracyCount / 4 % 2), 
			0.5F + 0.5F * (float)(m_rayTracyCount % 2), 
			0.5F + 0.5F * (float)(m_rayTracyCount / 2 % 2), 
			0.5F + 0.5F * (float)(m_rayTracyCount / 4 % 2));
	}
	else
	{
		setBlockBounds(0.0F, 0.0F, 0.0F, 1.0F, 1.0F, 1.0F);
	}
}

void BlockStairs::getCollSlabBouding(IBlockAccess* pBlockAccess, const BlockPos& pos)
{
	int meta = pBlockAccess->getBlockMeta(pos);

	if ((meta & 4) != 0)
	{
		setBlockBounds(0.0F, 0.5F, 0.0F, 1.0F, 1.0F, 1.0F);
	}
	else
	{
		setBlockBounds(0.0F, 0.0F, 0.0F, 1.0F, 0.5F, 1.0F);
	}
}

bool BlockStairs::isBlockStairsID(int blockID)
{
	Block* pBlock = BlockManager::sBlocks[blockID];	

	return blockID > 0 && typeid(BlockStairs) == typeid(pBlock);//Block.blocksList[blockID] instanceof BlockStairs;
}

bool BlockStairs::isTheSameStairs(IBlockAccess* pBlockAccess, const BlockPos& pos, int meta)
{
	int blockID = pBlockAccess->getBlockId(pos);
	return isBlockStairsID(blockID) && pBlockAccess->getBlockMeta(pos) == meta;
}

bool BlockStairs::getCollQuarterBouding(IBlockAccess* pBlockAccess, const BlockPos& pos)
{
	int meta = pBlockAccess->getBlockMeta(pos);
	int side = meta & 3;
	float minY = 0.5F;
	float maxY = 1.0F;

	if ((meta & 4) != 0)
	{
		minY = 0.0F;
		maxY = 0.5F;
	}

	BlockPos eastPos = pos.getPosX();
	BlockPos westPos = pos.getNegX();
	BlockPos northPos = pos.getNegZ();
	BlockPos southPos = pos.getPosZ();
	float minX = 0.0F;
	float maxX = 1.0F;
	float minZ = 0.0F;
	float maxZ = 0.5F;
	bool result = true;

	if (side == 0)
	{
		minX = 0.5F;
		maxZ = 1.0F;
		int eastID = pBlockAccess->getBlockId(eastPos);
		int eastMeta = pBlockAccess->getBlockMeta(eastPos);

		if (isBlockStairsID(eastID) && (meta & 4) == (eastMeta & 4))
		{
			int meta2 = eastMeta & 3;

			if (meta2 == 3 && !isTheSameStairs(pBlockAccess, southPos, meta))
			{
				maxZ = 0.5F;
				result = false;
			}
			else if (meta2 == 2 && !isTheSameStairs(pBlockAccess, northPos, meta))
			{
				minZ = 0.5F;
				result = false;
			}
		}
	}
	else if (side == 1)
	{
		maxX = 0.5F;
		maxZ = 1.0F;
		int westID = pBlockAccess->getBlockId(westPos);
		int westMeta = pBlockAccess->getBlockMeta(westPos);

		if (isBlockStairsID(westID) && (meta & 4) == (westMeta & 4))
		{
			int meta2 = westMeta & 3;

			if (meta2 == 3 && !isTheSameStairs(pBlockAccess, southPos, meta))
			{
				maxZ = 0.5F;
				result = false;
			}
			else if (meta2 == 2 && !isTheSameStairs(pBlockAccess, northPos, meta))
			{
				minZ = 0.5F;
				result = false;
			}
		}
	}
	else if (side == 2)
	{
		minZ = 0.5F;
		maxZ = 1.0F;
		int southID = pBlockAccess->getBlockId(southPos);
		int southMeta = pBlockAccess->getBlockMeta(southPos);

		if (isBlockStairsID(southID) && (meta & 4) == (southMeta & 4))
		{
			int meta2 = southMeta & 3;

			if (meta2 == 1 && !isTheSameStairs(pBlockAccess, eastPos, meta))
			{
				maxX = 0.5F;
				result = false;
			}
			else if (meta2 == 0 && !isTheSameStairs(pBlockAccess, westPos, meta))
			{
				minX = 0.5F;
				result = false;
			}
		}
	}
	else if (side == 3)
	{
		int northID = pBlockAccess->getBlockId(northPos);
		int northMeta = pBlockAccess->getBlockMeta(northPos);

		if (isBlockStairsID(northID) && (meta & 4) == (northMeta & 4))
		{
			int meta2 = northMeta & 3;

			if (meta2 == 1 && !isTheSameStairs(pBlockAccess, eastPos, meta))
			{
				maxX = 0.5F;
				result = false;
			}
			else if (meta2 == 0 && !isTheSameStairs(pBlockAccess, westPos, meta))
			{
				minX = 0.5F;
				result = false;
			}
		}
	}

	setBlockBounds(minX, minY, minZ, maxX, maxY, maxZ);
	return result;
}

bool BlockStairs::getCollEighthBouding(IBlockAccess* pBlockAccess, const BlockPos& pos)
{
	int meta = pBlockAccess->getBlockMeta(pos);
	int side = meta & 3;
	float minY = 0.5F;
	float maxY = 1.0F;

	if ((meta & 4) != 0)
	{
		minY = 0.0F;
		maxY = 0.5F;
	}

	BlockPos eastPos = pos.getPosX();
	BlockPos westPos = pos.getNegX();
	BlockPos northPos = pos.getNegZ();
	BlockPos southPos = pos.getPosZ();
	float minX = 0.0F;
	float maxX = 0.5F;
	float minZ = 0.5F;
	float maxZ = 1.0F;
	bool var13 = false;
		
	if (side == 0)
	{
		int westID = pBlockAccess->getBlockId(westPos);
		int westMeta = pBlockAccess->getBlockMeta(westPos);

		if (isBlockStairsID(westID) && (meta & 4) == (westMeta & 4))
		{
			int dir = westMeta & 3;

			if (dir == 3 && !isTheSameStairs(pBlockAccess, northPos, meta))
			{
				minZ = 0.0F;
				maxZ = 0.5F;
				var13 = true;
			}
			else if (dir == 2 && !isTheSameStairs(pBlockAccess, southPos, meta))
			{
				minZ = 0.5F;
				maxZ = 1.0F;
				var13 = true;
			}
		}
	}
	else if (side == 1)
	{
		int eastID = pBlockAccess->getBlockId(eastPos);
		int eastMeta = pBlockAccess->getBlockMeta(eastPos);

		if (isBlockStairsID(eastID) && (meta & 4) == (eastMeta & 4))
		{
			minX = 0.5F;
			maxX = 1.0F;
			int dir = eastMeta & 3;

			if (dir == 3 && !isTheSameStairs(pBlockAccess, northPos, meta))
			{
				minZ = 0.0F;
				maxZ = 0.5F;
				var13 = true;
			}
			else if (dir == 2 && !isTheSameStairs(pBlockAccess, southPos, meta))
			{
				minZ = 0.5F;
				maxZ = 1.0F;
				var13 = true;
			}
		}
	}
	else if (side == 2)
	{
		int westID = pBlockAccess->getBlockId(westPos);
		int westMeta = pBlockAccess->getBlockMeta(westPos);

		if (isBlockStairsID(westID) && (meta & 4) == (westMeta & 4))
		{
			minZ = 0.0F;
			maxZ = 0.5F;
			int dir = westMeta & 3;

			if (dir == 1 && !isTheSameStairs(pBlockAccess, westPos, meta))
			{
				var13 = true;
			}
			else if (dir == 0 && !isTheSameStairs(pBlockAccess, eastPos, meta))
			{
				minX = 0.5F;
				maxX = 1.0F;
				var13 = true;
			}
		}
	}
	else if (side == 3)
	{
		int southID = pBlockAccess->getBlockId(southPos);
		int southMeta = pBlockAccess->getBlockMeta(southPos);

		if (isBlockStairsID(southID) && (meta & 4) == (southMeta & 4))
		{
			int dir = southMeta & 3;

			if (dir == 1 && !isTheSameStairs(pBlockAccess, westPos, meta))
			{
				var13 = true;
			}
			else if (dir == 0 && !isTheSameStairs(pBlockAccess, eastPos, meta))
			{
				minX = 0.5F;
				maxX = 1.0F;
				var13 = true;
			}
		}
	}

	if (var13)
	{
		setBlockBounds(minX, minY, minZ, maxX, maxY, maxZ);
	}

	return var13;
}

void BlockStairs::onBlockAdded(World* pWorld, const BlockPos& pos)
{
	onNeighborBlockChange(pWorld, pos, 0);
	m_modelBlock->onBlockAdded(pWorld, pos);
}

bool BlockStairs::onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pEntityPlayer, int face, const Vector3& hit)
{
	return m_modelBlock->onBlockActivated(pWorld, pos, pEntityPlayer, face, Vector3::ZERO);
}

void BlockStairs::onBlockPlacedBy(World* pWorld, const BlockPos& pos, EntityLivingBase* pLiving, ItemStackPtr pStack)
{
	
	int dir = (int)Math::Floor((pLiving->rotationYaw * 4.0F / 360.0F) + 0.5F) & 3;
	int meta = pWorld->getBlockMeta(pos) & 4;

	if (dir == 0)
	{
		pWorld->setBlockMetadataWithNotify(pos, 2 | meta, 2);
	}

	if (dir == 1)
	{
		pWorld->setBlockMetadataWithNotify(pos, 1 | meta, 2);
	}

	if (dir == 2)
	{
		pWorld->setBlockMetadataWithNotify(pos, 3 | meta, 2);
	}

	if (dir == 3)
	{
		pWorld->setBlockMetadataWithNotify(pos, 0 | meta, 2);
	}
}

RayTraceResult BlockStairs::collisionRayTrace(World* pWorld, const BlockPos& pos, const Vector3& start, const Vector3& end)
{
	int rayIdx[8][2] = { { 2, 6 }, { 3, 7 }, { 2, 3 }, { 6, 7 }, { 0, 4 }, { 1, 5 }, { 0, 1 }, { 4, 5 } };
	RayTraceResult rayTraces[8];
	int meta = pWorld->getBlockMeta(pos);
	int dir = meta & 3;
	bool bHeadStand = (meta & 4) == 4;
	int* subRayIdx = rayIdx[dir + (bHeadStand ? 4 : 0)];
	m_rayTracyed = true;
	
	for (int i = 0; i < 8; ++i)
	{
		m_rayTracyCount = i;
		rayTraces[i] = Block::collisionRayTrace(pWorld, pos, start, end);
	}

	for (int i = 0; i < 2; ++i)
	{
		int idx = subRayIdx[i];
		rayTraces[idx].reset();
	}

	RayTraceResult result;
	float minDis = 0.0f;

	for (int i = 0; i < 8; ++i)
	{
		RayTraceResult& raytrace = rayTraces[i];

		if (raytrace.result)
		{
			float dis = raytrace.hitVec.squareDistanceTo(end);

			if (dis > minDis)
			{
				result = raytrace;
				minDis = dis;
			}
		}
	}

	return result;
}

BlockHalfSlab::BlockHalfSlab(int id, const BM_Material& mat, const String& name, bool isDoubleSlab)
	:Block(id, mat, name, isDoubleSlab)
{	
	m_isDoubleSlab = isDoubleSlab;
	if (isDoubleSlab)
	{
		BlockManager::sOpaqueCubeLookup[id] = true; 
	}
	else
	{
		setBlockBounds(0.0F, 0.0F, 0.0F, 1.0F, 0.5F, 1.0F);
	}
	setLightOpacity(0);
}

void BlockHalfSlab::setBlockBoundsBasedOnState(IBlockAccess* pBlockAccess, const BlockPos& pos)
{
	if (m_isDoubleSlab)
	{
		setBlockBounds(0.0F, 0.0F, 0.0F, 1.0F, 1.0F, 1.0F);
	}
	else
	{
		bool var5 = (pBlockAccess->getBlockMeta(pos) & 8) != 0;

		if (var5)
		{
			setBlockBounds(0.0F, 0.5F, 0.0F, 1.0F, 1.0F, 1.0F);
		}
		else
		{
			setBlockBounds(0.0F, 0.0F, 0.0F, 1.0F, 0.5F, 1.0F);
		}
	}
}

void BlockHalfSlab::addCollisionBoxesToList(World* pWorld, const BlockPos& pos, const Box& aabb, AABBList& lst, Entity* pEntity)
{
	setBlockBoundsBasedOnState(pWorld, pos);
	Block::addCollisionBoxesToList(pWorld, pos, aabb, lst, pEntity);
}

void BlockHalfSlab::setBlockBoundsForItemRender()
{
	if (m_isDoubleSlab)
	{
		setBlockBounds(0.0F, 0.0F, 0.0F, 1.0F, 1.0F, 1.0F);
	}
	else
	{
		setBlockBounds(0.0F, 0.0F, 0.0F, 1.0F, 0.5F, 1.0F);
	}
}

int BlockHalfSlab::onBlockPlaced(World* pWorld, const BlockPos& pos, int side, const Vector3& hit, int meta)
{
	return m_isDoubleSlab ? meta : (side != 0 && (side == 1 || hit.y <= 0.5F) ? meta : meta | 8);
}

int BlockHalfSlab::idPicked(World* pWorld, const BlockPos& pos)
{
	return isBlockSingleSlab(m_blockID) ? m_blockID : 
	(m_blockID == BLOCK_ID_STONE_DOUBLE_SLAB ? BLOCK_ID_STONE_SINGLE_SLAB : 
	(m_blockID == BLOCK_ID_WOOD_DOUBLE_SLAB ? BLOCK_ID_WOOD_SINGLE_SLAB : BLOCK_ID_STONE_SINGLE_SLAB));
}

BlockStep::BlockStep(int id, const BM_Material& mat, const String& name, bool isDoubleSlab)
	:BlockHalfSlab(id, mat, name, isDoubleSlab)
{
}

ItemStackPtr BlockStep::createStackedBlock(int damage)
{
	return LORD::make_shared<ItemStack>(BLOCK_ID_STONE_SINGLE_SLAB, 2, damage & 7);
}

BlockWoodSlab::BlockWoodSlab(int id, const BM_Material& mat, const String& name, bool isDoubleSlab)
	:BlockHalfSlab(id, mat, name, isDoubleSlab)
{
}

ItemStackPtr BlockWoodSlab::createStackedBlock(int damage)
{
	return LORD::make_shared<ItemStack>(BLOCK_ID_WOOD_SINGLE_SLAB, 2, damage & 7);
}

BlockRedStoneSlab::BlockRedStoneSlab(int id, const BM_Material& mat, const String& name, bool isDoubleSlab)
	:BlockHalfSlab(id, mat, name, isDoubleSlab)
{
}

ItemStackPtr BlockRedStoneSlab::createStackedBlock(int damage)
{
	return LORD::make_shared<ItemStack>(BLOCK_ID_REDSAND_STONE_SINGLESLAB, 2, damage & 7);
}

BlockCrops::BlockCrops(int id, const BM_Material& mat, const String& name)
	:BlockFlower(id, mat, name)
{
	setTickRandomly(true);
	float offset = 0.5F;
	setBlockBounds(0.5F - offset, 0.0F, 0.5F - offset, 0.5F + offset, 0.25F, 0.5F + offset);
	setHardness(0.0F);
	disableStats();
}

void BlockCrops::updateTick(World* pWorld, const BlockPos& pos, Random& rand)
{
	BlockFlower::updateTick(pWorld, pos, rand);

	// todo.
	//if (pWorld->getBlockLightValue(pos.getPosY()) >= 9)
	if(false)
	{
		int meta = pWorld->getBlockMeta(pos);

		if (meta < 7)
		{
			float growthRate = getGrowthRate(pWorld, pos);

			if (rand.nextInt((int)(25.0F / growthRate) + 1) == 0)
			{
				++meta;
				pWorld->setBlockMetadataWithNotify(pos, meta, 2);
			}
		}
	}
}

int BlockCrops::idDropped(int meta, Random& rand, int fortune)
{	
	return meta == 7 ? getCropItem() : getSeedItem();
}

void BlockCrops::dropBlockAsItemWithChance(World* pWorld, const BlockPos& pos, int meta, float chance, int fortune)
{
	BlockFlower::dropBlockAsItemWithChance(pWorld, pos, meta, chance, 0);

	if (!pWorld->m_isClient)
	{
		if (meta >= 7)
		{
			int var8 = 3 + fortune;

			for (int i = 0; i < var8; ++i)
			{
				if (pWorld->m_Rand.nextInt(15) <= meta)
				{
					dropBlockAsItem_do(pWorld, pos, LORD::make_shared<ItemStack>(getSeedItem(), 1, 0));
				}
			}
		}
	}
}

void BlockCrops::fertilize(World* pWorld, const BlockPos& pos)
{	
	//int var5 = pWorld->getBlockMetadata(x, y, z) + MathHelper.getRandomIntegerInRange(pWorld->rand, 2, 5);
	int meta = pWorld->getBlockMeta(pos) + pWorld->m_Rand.nextInt(3) + 2;

	if (meta > 7)
	{
		meta = 7;
	}

	pWorld->setBlockMetadataWithNotify(pos, meta, 2);
}

float BlockCrops::getGrowthRate(World* pWorld, const BlockPos& pos)
{
	float rate = 1.0F;
	int northID = pWorld->getBlockId(pos.getNegZ());
	int southID = pWorld->getBlockId(pos.getPosZ());
	int westID = pWorld->getBlockId(pos.getNegX());
	int eastID = pWorld->getBlockId(pos.getPosX());
	int WNID = pWorld->getBlockId(pos.getNegZ().getNegX());
	int ENID = pWorld->getBlockId(pos.getNegZ().getPosX());
	int ESID = pWorld->getBlockId(pos.getPosZ().getPosX());
	int WSID = pWorld->getBlockId(pos.getPosZ().getNegX());
	bool bWEsame = westID == m_blockID || eastID == m_blockID;
	bool bSNsame = northID == m_blockID || southID == m_blockID;
	bool bCornerSame = WNID == m_blockID || ENID == m_blockID || ESID == m_blockID || WSID == m_blockID;

	for (int ix = pos.x - 1; ix <= pos.x + 1; ++ix)
	{
		for (int iz = pos.z - 1; iz <= pos.z + 1; ++iz)
		{
			BlockPos pos1(ix, pos.y - 1, iz);
			int blockID = pWorld->getBlockId(pos1);
			float contribute = 0.0F;

			if (blockID == BLOCK_ID_TILLED_FIELD)
			{
				contribute = 1.0F;

				if (pWorld->getBlockMeta(pos1) > 0)
				{
					contribute = 3.0F;
				}
			}

			if (ix != pos.x || iz != pos.z)
			{
				contribute /= 4.0F;
			}

			rate += contribute;
		}
	}

	if (bCornerSame || bWEsame && bSNsame)
	{
		rate /= 2.0F;
	}

	return rate;
}

BlockCarrot::BlockCarrot(int id, const BM_Material& mat, const String& name)
	:BlockCrops(id, mat, name)
{}

BlockPotato::BlockPotato(int id, const BM_Material& mat, const String& name)
	:BlockCrops(id, mat, name)
{}

void BlockPotato::dropBlockAsItemWithChance(World* pWorld, const BlockPos& pos, int meta, float chance, int fortune)
{
	BlockFlower::dropBlockAsItemWithChance(pWorld, pos, meta, chance, fortune);

	if (!pWorld->m_isClient)
	{
		if (meta >= 7 && pWorld->m_Rand.nextInt(50) == 0)
		{
			dropBlockAsItem_do(pWorld, pos, LORD::make_shared<ItemStack>(Item::poisonousPotato));
		}
	}
}

BlockStem::BlockStem(int id, const BM_Material& mat, const String& name, Block* fruit)
	:BlockFlower(id, mat, name)
{
	m_fruitBlock = fruit;
	setTickRandomly(true);
	float offset = 0.125F;
	setBlockBounds(0.5F - offset, 0.0F, 0.5F - offset, 0.5F + offset, 0.25F, 0.5F + offset);
}

void BlockStem::updateTick(World* pWorld, const BlockPos& pos, Random& rand)
{
	BlockFlower::updateTick(pWorld, pos, rand);

	// todo.
	//if (pWorld->getBlockLightValue(pos.getPosY()) >= 9)
	if(false)
	{
		float growthmodifier = getGrowthModifier(pWorld, pos);

		if (rand.nextInt((int)(25.0F / growthmodifier) + 1) == 0)
		{
			int meta = pWorld->getBlockMeta(pos);

			if (meta < 7)
			{
				++meta;
				pWorld->setBlockMetadataWithNotify(pos, meta, 2);
			}
			else
			{
				if (pWorld->getBlockId(pos.getNegX()) == m_fruitBlock->getBlockID())
					return;

				if (pWorld->getBlockId(pos.getPosX()) == m_fruitBlock->getBlockID())
					return;

				if (pWorld->getBlockId(pos.getNegZ()) == m_fruitBlock->getBlockID())
					return;

				if (pWorld->getBlockId(pos.getPosZ()) == m_fruitBlock->getBlockID())
					return;

				int dir = rand.nextInt(4);
				BlockPos pos1 = pos;

				if (dir == 0)
					--pos1.x;
				else if (dir == 1)
					++pos1.x;
				else if (dir == 2)
					--pos1.z;
				else if (dir == 3)
					++pos1.z;

				int blockID = pWorld->getBlockId(pos1.getNegY());

				if (pWorld->getBlockId(pos1) == 0 && (blockID == BLOCK_ID_TILLED_FIELD || blockID == BLOCK_ID_DIRT || blockID == BLOCK_ID_GRASS))
				{
					pWorld->setBlock(pos1, m_fruitBlock->getBlockID());
				}
			}
		}
	}
}

void BlockStem::setBlockBoundsForItemRender()
{
	float bound = 0.125F;
	setBlockBounds(0.5F - bound, 0.0F, 0.5F - bound, 0.5F + bound, 0.25F, 0.5F + bound);
}

void BlockStem::setBlockBoundsBasedOnState(IBlockAccess* pBlockAccess, const BlockPos& pos)
{
	m_max.y = (pBlockAccess->getBlockMeta(pos) * 2 + 2) / 16.0F;
	float bound = 0.125F;
	setBlockBounds(0.5F - bound, 0.0F, 0.5F - bound, 0.5F + bound, m_max.y, 0.5F + bound);
}

void BlockStem::dropBlockAsItemWithChance(World* pWorld, const BlockPos& pos, int meta, float chance, int fortune)
{
	BlockFlower::dropBlockAsItemWithChance(pWorld, pos, meta, chance, fortune);

	if (!pWorld->m_isClient)
	{
		Item* pItem = NULL;

		if (m_fruitBlock == BlockManager::pumpkin)
		{
			pItem = Item::pumpkinSeeds;
		}

		if (m_fruitBlock == BlockManager::melon)
		{
			pItem = Item::melonSeeds;
		}

		for (int i = 0; i < 3; ++i)
		{
			if (pWorld->m_Rand.nextInt(15) <= meta)
			{
				dropBlockAsItem_do(pWorld, pos, LORD::make_shared<ItemStack>(pItem));
			}
		}
	}
}

int BlockStem::idPicked(World* pWorld, const BlockPos& pos)
{
	return m_fruitBlock == BlockManager::pumpkin ? Item::pumpkinSeeds->itemID : 
		  (m_fruitBlock == BlockManager::melon ? Item::melonSeeds->itemID : 0);
}

int BlockStem::getState(IBlockAccess* pBlockAccess, const BlockPos& pos)
{
	int meta = pBlockAccess->getBlockMeta(pos);
	return meta < 7 ? -1 : 
		(pBlockAccess->getBlockId(pos.getNegX()) == m_fruitBlock->getBlockID() ? 0 : 
		(pBlockAccess->getBlockId(pos.getPosX()) == m_fruitBlock->getBlockID() ? 1 : 
		(pBlockAccess->getBlockId(pos.getNegZ()) == m_fruitBlock->getBlockID() ? 2 : 
		(pBlockAccess->getBlockId(pos.getPosZ()) == m_fruitBlock->getBlockID() ? 3 : -1))));
}

void BlockStem::fertilizeStem(World* pWorld, const BlockPos& pos)
{
	//int meta = pWorld->getBlockMetadata(x, y, z) + MathHelper.getRandomIntegerInRange(pWorld->rand, 2, 5);
	int meta = pWorld->getBlockMeta(pos) + pWorld->m_Rand.nextInt(3) + 2;

	if (meta > 7)
	{
		meta = 7;
	}

	pWorld->setBlockMetadataWithNotify(pos, meta, 2);
}

float BlockStem::getGrowthModifier(World* pWorld, const BlockPos& pos)
{
	float var5 = 1.0F;
	int Nid = pWorld->getBlockId(pos.getNegZ());
	int Sid = pWorld->getBlockId(pos.getPosZ());
	int Wid = pWorld->getBlockId(pos.getNegX());
	int Eid = pWorld->getBlockId(pos.getPosX());
	int WNid = pWorld->getBlockId(pos.getNegZ().getNegX());
	int ENid = pWorld->getBlockId(pos.getNegZ().getPosX());
	int ESid = pWorld->getBlockId(pos.getPosZ().getPosX());
	int WSid = pWorld->getBlockId(pos.getPosZ().getNegX());
	bool bEWsame = Wid == m_blockID || Eid == m_blockID;
	bool bNSsame = Nid == m_blockID || Sid == m_blockID;
	bool bCornerSame = WNid == m_blockID || ENid == m_blockID || ESid == m_blockID || WSid == m_blockID;

	for (int x = pos.x - 1; x <= pos.x + 1; ++x)
	{
		for (int z = pos.z - 1; z <= pos.z + 1; ++z)
		{
			BlockPos pos1(x, pos.y - 1, z);
			int blockID = pWorld->getBlockId(pos1);
			float contribute = 0.0F;

			if (blockID == BLOCK_ID_TILLED_FIELD)
			{
				contribute = 1.0F;

				if (pWorld->getBlockMeta(pos1) > 0)
				{
					contribute = 3.0F;
				}
			}

			if (x != pos.x || z != pos.z)
			{
				contribute /= 4.0F;
			}

			var5 += contribute;
		}
	}

	if (bCornerSame || bEWsame && bNSsame)
	{
		var5 /= 2.0F;
	}

	return var5;
}

BlockButton::BlockButton(int id, const BM_Material& mat, const String& name, bool sensible)
	: Block(id, mat, name, false)
{
	setTickRandomly(true);
	m_sensible = sensible;
}

bool BlockButton::canPlaceBlockOnSide(World* pWorld, const BlockPos& pos, int side)
{
	return side == 2 && pWorld->isBlockNormalCube(pos.getPosZ()) ? true : 
		  (side == 3 && pWorld->isBlockNormalCube(pos.getNegZ()) ? true : 
		  (side == 4 && pWorld->isBlockNormalCube(pos.getPosX()) ? true : 
		   side == 5 && pWorld->isBlockNormalCube(pos.getNegX())));
}

bool BlockButton::canPlaceBlockAt(World* pWorld, const BlockPos& pos)
{
	return pWorld->isBlockNormalCube(pos.getNegX()) ? true :
		(pWorld->isBlockNormalCube(pos.getPosX()) ? true :
		(pWorld->isBlockNormalCube(pos.getNegZ()) ? true :
		pWorld->isBlockNormalCube(pos.getPosZ())));
}

int BlockButton::onBlockPlaced(World* pWorld, const BlockPos& pos, int side, const Vector3& hit, int meta1)
{
	int meta = pWorld->getBlockMeta(pos);
	int var11 = meta & 8;
	meta &= 7;

	if (side == 2 && pWorld->isBlockNormalCube(pos.getPosZ()))
	{
		meta = 4;
	}
	else if (side == 3 && pWorld->isBlockNormalCube(pos.getNegZ()))
	{
		meta = 3;
	}
	else if (side == 4 && pWorld->isBlockNormalCube(pos.getPosX()))
	{
		meta = 2;
	}
	else if (side == 5 && pWorld->isBlockNormalCube(pos.getNegX()))
	{
		meta = 1;
	}
	else
	{
		meta = getOrientation(pWorld, pos);
	}

	return meta + var11;
}

void BlockButton::onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborID)
{
	if (redundantCanPlaceBlockAt(pWorld, pos))
	{
		int meta = pWorld->getBlockMeta(pos) & 7;
		bool flag = false;

		if (!pWorld->isBlockNormalCube(pos.getNegX()) && meta == 1)
		{
			flag = true;
		}

		if (!pWorld->isBlockNormalCube(pos.getPosX()) && meta == 2)
		{
			flag = true;
		}

		if (!pWorld->isBlockNormalCube(pos.getNegZ()) && meta == 3)
		{
			flag = true;
		}

		if (!pWorld->isBlockNormalCube(pos.getPosZ()) && meta == 4)
		{
			flag = true;
		}

		if (flag)
		{
			dropBlockAsItem(pWorld, pos, pWorld->getBlockMeta(pos), 0);
			pWorld->setBlockToAir(pos);
		}
	}
}

void BlockButton::setBlockBoundsBasedOnState(IBlockAccess* pBlockAccess, const BlockPos& pos)
{
	int meta = pBlockAccess->getBlockMeta(pos);
	setButtonBounding(meta);
}

void BlockButton::setBlockBoundsForItemRender()
{
	float boundx = 0.1875F;
	float boundy = 0.125F;
	float boundz = 0.125F;
	setBlockBounds(0.5F - boundx, 0.5F - boundy, 0.5F - boundz, 0.5F + boundx, 0.5F + boundy, 0.5F + boundz);
}

bool BlockButton::onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face1, const Vector3& hit)
{
	pPlayer->setActivatedBlock(pos);
	int meta = pWorld->getBlockMeta(pos);
	int face = meta & 7;
	int var12 = 8 - (meta & 8);

	if (var12 == 0)
		return true;

	pWorld->setBlockMetadataWithNotify(pos, face + var12, 3);
	pWorld->markBlockRangeForRenderUpdate(pos, pos);
	//pWorld->playSoundEffect(Vector3(pos) + 0.5f, "random.click", 0.3F, 0.6F);
	pWorld->playSoundEffectByType(Vector3(pos) + 0.5f, ST_Click);
	if (SCRIPT_EVENT::BlockSwitchEvent::invoke(pPlayer->entityId, true, m_blockID, pos))
	{
		notifyNeighborsFace(pWorld, pos, face);
	}
	pWorld->scheduleBlockUpdate(pos, m_blockID, tickRate(pWorld));
	return true;
}

void BlockButton::breakBlock(World* pWorld, const BlockPos& pos, int blockID, int meta)
{
	if ((meta & 8) > 0)
	{
		int face = meta & 7;
		notifyNeighborsFace(pWorld, pos, face);
	}

	Block::breakBlock(pWorld, pos, blockID, meta);
}

int BlockButton::isProvidingWeakPower(IBlockAccess* piBlockAccess, const BlockPos& pos, int side)
{
	return (piBlockAccess->getBlockMeta(pos) & 8) > 0 ? 15 : 0;
}

int BlockButton::isProvidingStrongPower(IBlockAccess* iBlockAccess, const BlockPos& pos, int side)
{
	int meta = iBlockAccess->getBlockMeta(pos);

	if ((meta & 8) == 0)
		return 0;

	int face = meta & 7;

	if ((face == 5 && side == 1) ||
		(face == 4 && side == 2) ||
		(face == 3 && side == 3) ||
		(face == 2 && side == 4) ||
		(face == 1 && side == 5))
		return 15;

	return 0;
}

void BlockButton::updateTick(World* pWorld, const BlockPos& pos, Random& rand)
{
	if (!pWorld->m_isClient)
	{
		int meta = pWorld->getBlockMeta(pos);

		if ((meta & 8) != 0)
		{
			if (m_sensible)
			{
				notifyNeighbors(pWorld, pos, false, -1);
			}
			else
			{
				pWorld->setBlockMetadataWithNotify(pos, meta & 7, 3);
				int face = meta & 7;
				if (SCRIPT_EVENT::BlockSwitchEvent::invoke(-1, false, m_blockID, pos))
				{
					notifyNeighborsFace(pWorld, pos, face);
				}
				//pWorld->playSoundEffect(Vector3(pos) + 0.5f, "random.click", 0.3F, 0.5F);
				pWorld->playSoundEffectByType(Vector3(pos) + 0.5f, ST_Click);
				pWorld->markBlockRangeForRenderUpdate(pos, pos);
			}
		}
	}
}

void BlockButton::onEntityCollidedWithBlock(World* pWorld, const BlockPos& pos, Entity* pEntity)
{
	if (pWorld->m_isClient)
		return;
	
	if (!m_sensible)
		return;
		
	if ((pWorld->getBlockMeta(pos) & 8) == 0 && pEntity)
	{
		notifyNeighbors(pWorld, pos, true, pEntity->entityId);
	}
}

int BlockButton::getOrientation(World* pWorld, const BlockPos& pos)
{
	return pWorld->isBlockNormalCube(pos.getNegX()) ? 1 : 
		  (pWorld->isBlockNormalCube(pos.getPosX()) ? 2 : 
		  (pWorld->isBlockNormalCube(pos.getNegZ()) ? 3 : 
		  (pWorld->isBlockNormalCube(pos.getPosZ()) ? 4 : 1)));
}

bool BlockButton::redundantCanPlaceBlockAt(World* pWorld, const BlockPos& pos)
{
	if (!canPlaceBlockAt(pWorld, pos))
	{
		dropBlockAsItem(pWorld, pos, pWorld->getBlockMeta(pos), 0);
		pWorld->setBlockToAir(pos);
		return false;
	}

	return true;
}

void BlockButton::setButtonBounding(int meta)
{
	int state = meta & 7;
	float f1 = 0.375F;
	float f2 = 0.625F;
	float f3 = 0.1875F;
	float f4 = 0.125F;

	if ((meta & 8) > 0)
	{
		f4 = 0.0625F;
	}

	if (state == 1)
	{
		setBlockBounds(0.0F, f1, 0.5F - f3, f4, f2, 0.5F + f3);
	}
	else if (state == 2)
	{
		setBlockBounds(1.0F - f4, f1, 0.5F - f3, 1.0F, f2, 0.5F + f3);
	}
	else if (state == 3)
	{
		setBlockBounds(0.5F - f3, f1, 0.0F, 0.5F + f3, f2, f4);
	}
	else if (state == 4)
	{
		setBlockBounds(0.5F - f3, f1, 1.0F - f4, 0.5F + f3, f2, 1.0F);
	}
}

void BlockButton::notifyNeighbors(World* pWorld, const BlockPos& pos, bool upOrDown, int entityId)
{
	int meta = pWorld->getBlockMeta(pos);
	int face = meta & 7;
	bool var7 = (meta & 8) != 0;
	setButtonBounding(meta);
	EntityArr entities = pWorld->getEntitiesWithinAABB(ENTITY_CLASS_ARROW, Box(Vector3(pos) + m_min, Vector3(pos) + m_max));
	bool hasEntity = !entities.empty();

	if (hasEntity && !var7)
	{
		pWorld->setBlockMetadataWithNotify(pos, face | 8, 3);

		if (SCRIPT_EVENT::BlockSwitchEvent::invoke(entityId, upOrDown, m_blockID, pos))
		{
			notifyNeighborsFace(pWorld, pos, face);
		}
		pWorld->markBlockRangeForRenderUpdate(pos, pos);
		//pWorld->playSoundEffect(Vector3(pos) + 0.5f, "random.click", 0.3F, 0.6F);
		pWorld->playSoundEffectByType(Vector3(pos) + 0.5f, ST_Click);
	}

	if (!hasEntity && var7)
	{
		pWorld->setBlockMetadataWithNotify(pos, face, 3);
		if (SCRIPT_EVENT::BlockSwitchEvent::invoke(entityId, upOrDown, m_blockID, pos))
		{
			notifyNeighborsFace(pWorld, pos, face);
		}
		pWorld->markBlockRangeForRenderUpdate(pos, pos);
		//pWorld->playSoundEffect(Vector3(pos) + 0.5f, "random.click", 0.3F, 0.5F);
		pWorld->playSoundEffectByType(Vector3(pos) + 0.5f, ST_Click);
	}

	if (hasEntity)
	{
		pWorld->scheduleBlockUpdate(pos, m_blockID, tickRate(pWorld));
	}
}

void BlockButton::notifyNeighborsFace(World* pWorld, const BlockPos& pos, int face)
{
	pWorld->notifyBlocksOfNeighborChange(pos, m_blockID);

	if (face == 1)
	{
		pWorld->notifyBlocksOfNeighborChange(pos.getNegX(), m_blockID);
	}
	else if (face == 2)
	{
		pWorld->notifyBlocksOfNeighborChange(pos.getPosX(), m_blockID);
	}
	else if (face == 3)
	{
		pWorld->notifyBlocksOfNeighborChange(pos.getNegZ(), m_blockID);
	}
	else if (face == 4)
	{
		pWorld->notifyBlocksOfNeighborChange(pos.getPosZ(), m_blockID);
	}
	else
	{
		pWorld->notifyBlocksOfNeighborChange(pos.getNegY(), m_blockID);
	}
}

BlockButtonWood::BlockButtonWood(int id, const BM_Material& mat, const String& name)
	: BlockButton(id, mat, name, false)
{}


BlockButtonStone::BlockButtonStone(int id, const BM_Material& mat, const String& name)
	: BlockButton(id, mat, name, true)
{}

BlockLadder::BlockLadder(int id, const BM_Material& mat, const String& name)
	: Block(id, mat, name, false)
{
}

Box BlockLadder::getCollisionBoundingBox(World* pWorld, const BlockPos& pos)
{
	setBlockBoundsBasedOnState(pWorld, pos);
	return Block::getCollisionBoundingBox(pWorld, pos);
}

void BlockLadder::setBlockBoundsBasedOnState(IBlockAccess* pBlockAccess, const BlockPos& pos)
{
	updateLadderBounds(pBlockAccess->getBlockMeta(pos));
}

void BlockLadder::updateLadderBounds(int meta)
{
	float bound = 0.125F;

	if (meta == 0)
	{
		setBlockBounds(0.0F, 0.0F, 1.0F - bound, 1.0F, 1.0F, 1.0F);
	}

	if (meta == 3)
	{
		setBlockBounds(0.0F, 0.0F, 0.0F, 1.0F, 1.0F, bound);
	}

	if (meta == 4)
	{
		setBlockBounds(1.0F - bound, 0.0F, 0.0F, 1.0F, 1.0F, 1.0F);
	}

	if (meta == 5)
	{
		setBlockBounds(0.0F, 0.0F, 0.0F, bound, 1.0F, 1.0F);
	}
}

bool BlockLadder::canPlaceBlockAt(World* pWorld, const BlockPos& pos)
{
	return pWorld->isBlockNormalCube(pos.getNegX()) ? true : 
		  (pWorld->isBlockNormalCube(pos.getPosX()) ? true : 
		  (pWorld->isBlockNormalCube(pos.getNegZ()) ? true : 
		   pWorld->isBlockNormalCube(pos.getPosZ())));
}

int BlockLadder::onBlockPlaced(World* pWorld, const BlockPos& pos, int side, const Vector3& hit, int meta)
{
	int ret = meta;

	if ((meta == 0 || side == 2) && pWorld->isBlockNormalCube(pos.getPosZ()))
	{
		ret = 0;
	}

	if ((ret == 0 || side == 3) && pWorld->isBlockNormalCube(pos.getNegZ()))
	{
		ret = 3;
	}

	if ((ret == 0 || side == 4) && pWorld->isBlockNormalCube(pos.getPosX()))
	{
		ret = 4;
	}

	if ((ret == 0 || side == 5) && pWorld->isBlockNormalCube(pos.getNegX()))
	{
		ret = 5;
	}

	return ret;
}

void BlockLadder::onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborID)
{
	int meta = pWorld->getBlockMeta(pos);
	bool flag = false;

	if (meta == 0 && pWorld->isBlockNormalCube(pos.getPosZ()))
	{
		flag = true;
	}

	if (meta == 3 && pWorld->isBlockNormalCube(pos.getNegZ()))
	{
		flag = true;
	}

	if (meta == 4 && pWorld->isBlockNormalCube(pos.getPosX()))
	{
		flag = true;
	}

	if (meta == 5 && pWorld->isBlockNormalCube(pos.getNegX()))
	{
		flag = true;
	}

	if (!flag)
	{
		dropBlockAsItem(pWorld, pos, meta, 0);
		pWorld->setBlockToAir(pos);
	}

	Block::onNeighborBlockChange(pWorld, pos, neighborID);
}

BlockFence::BlockFence(int id, const BM_Material& mat, const String& name)
	: Block(id, mat, name, false)
{	
}	

void BlockFence::setBlockBoundsBasedOnState(IBlockAccess* pBlockAccess, const BlockPos& pos)
{
	bool bNorth = canConnectFenceTo(pBlockAccess, pos.getNegZ());
	bool bSouth = canConnectFenceTo(pBlockAccess, pos.getPosZ());
	bool bWest = canConnectFenceTo(pBlockAccess, pos.getNegX());
	bool bEast = canConnectFenceTo(pBlockAccess, pos.getPosX());
	float minx = 0.375F;
	float maxx = 0.625F;
	float minz = 0.375F;
	float maxz = 0.625F;

	if (bNorth)
	{
		minz = 0.0F;
	}

	if (bSouth)
	{
		maxz = 1.0F;
	}

	if (bWest)
	{
		minx = 0.0F;
	}

	if (bEast)
	{
		maxx = 1.0F;
	}

	setBlockBounds(minx, 0.0F, minz, maxx, 1.0F, maxz);
}

bool BlockFence::onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit)
{
	if (pWorld->m_isClient)
		return true;

	pPlayer->setActivatedBlock(pos);
	// todo.
	// return ItemLeash::func_135066_a(par5EntityPlayer, par1World, par2, par3, par4);
	return false;
}

void BlockFence::addCollisionBoxesToList(World* pWorld, const BlockPos& pos, const Box& aabb, AABBList& lst, Entity* pEntity)
{
	bool bNorth = canConnectFenceTo(pWorld, pos.getNegZ());
	bool bSouth = canConnectFenceTo(pWorld, pos.getPosZ());
	bool bWest = canConnectFenceTo(pWorld, pos.getNegX());
	bool bEast = canConnectFenceTo(pWorld, pos.getPosX());
	float minx = 0.375F;
	float maxx = 0.625F;
	float minz = 0.375F;
	float maxz = 0.625F;

	if (bNorth)
	{
		minz = 0.0F;
	}

	if (bSouth)
	{
		maxz = 1.0F;
	}

	if (bNorth || bSouth)
	{
		setBlockBounds(minx, 0.0F, minz, maxx, 1.5F, maxz);
		Block::addCollisionBoxesToList(pWorld, pos, aabb, lst, pEntity);
	}

	minz = 0.375F;
	maxz = 0.625F;

	if (bWest)
	{
		minx = 0.0F;
	}

	if (bEast)
	{
		maxx = 1.0F;
	}

	if (bWest || bEast || !bNorth && !bSouth)
	{
		setBlockBounds(minx, 0.0F, minz, maxx, 1.5F, maxz);
		Block::addCollisionBoxesToList(pWorld, pos, aabb, lst, pEntity);
	}

	if (bNorth)
	{
		minz = 0.0F;
	}

	if (bSouth)
	{
		maxz = 1.0F;
	}

	setBlockBounds(minx, 0.0F, minz, maxx, 1.0F, maxz);
}

bool BlockFence::canConnectFenceTo(IBlockAccess* pBlockAccess, const BlockPos& pos)
{
	int blockID = pBlockAccess->getBlockId(pos);

	
	if (blockID != m_blockID && blockID != BLOCK_ID_FENCE_GATE)
	{
		Block* pBlock = BlockManager::sBlocks[blockID];
		return pBlock != NULL && pBlock->getMaterial().isOpaque() && 
			pBlock->renderAsNormalBlock() ? pBlock->getMaterial() != BM_Material::BM_MAT_pumpkin : false;
	}
	else
	{
		return true;
	}
}

bool BlockFence::isIdAFence(int blockID) 
{ 
	return blockID == BLOCK_ID_FENCE || blockID == BLOCK_ID_NETHER_FENCE; 
}

BlockPane::BlockPane(int id, const BM_Material& mat, const String& name, bool canDropItSelf)
	: Block(id, mat, name, false)
	, m_canDropItSelf(canDropItSelf)
{	
}

void BlockPane::addCollisionBoxesToList(World* pWorld, const BlockPos& pos, const Box& aabb, AABBList& lst, Entity* pEntity)
{
	bool bNorth = canThisPaneConnectToThisBlockID(pWorld->getBlockId(pos.getNegZ()));
	bool bSouth = canThisPaneConnectToThisBlockID(pWorld->getBlockId(pos.getPosZ()));
	bool bWest = canThisPaneConnectToThisBlockID(pWorld->getBlockId(pos.getNegX()));
	bool bEast = canThisPaneConnectToThisBlockID(pWorld->getBlockId(pos.getPosX()));

	if ((!bWest || !bEast) && (bWest || bEast || bNorth || bSouth))
	{
		if (bWest && !bEast)
		{
			setBlockBounds(0.0F, 0.0F, 0.4375F, 0.5F, 1.5F, 0.5625F);
			Block::addCollisionBoxesToList(pWorld, pos, aabb, lst, pEntity);
		}
		else if (!bWest && bEast)
		{
			setBlockBounds(0.5F, 0.0F, 0.4375F, 1.0F, 1.5F, 0.5625F);
			Block::addCollisionBoxesToList(pWorld, pos, aabb, lst, pEntity);
		}
	}
	else
	{
		setBlockBounds(0.0F, 0.0F, 0.4375F, 1.0F, 1.5F, 0.5625F);
		Block::addCollisionBoxesToList(pWorld, pos, aabb, lst, pEntity);
	}

	if ((!bNorth || !bSouth) && (bWest || bEast || bNorth || bSouth))
	{
		if (bNorth && !bSouth)
		{
			setBlockBounds(0.4375F, 0.0F, 0.0F, 0.5625F, 1.5F, 0.5F);
			Block::addCollisionBoxesToList(pWorld, pos, aabb, lst, pEntity);
		}
		else if (!bNorth && bSouth)
		{
			setBlockBounds(0.4375F, 0.0F, 0.5F, 0.5625F, 1.5F, 1.0F);
			Block::addCollisionBoxesToList(pWorld, pos, aabb, lst, pEntity);
		}
	}
	else
	{
		setBlockBounds(0.4375F, 0.0F, 0.0F, 0.5625F, 1.5F, 1.0F);
		Block::addCollisionBoxesToList(pWorld, pos, aabb, lst, pEntity);
	}
}

ItemStackPtr BlockPane::createStackedBlock(int damage)
{
	return LORD::make_shared<ItemStack>(m_blockID, 1, damage);
}

void BlockPane::setBlockBoundsBasedOnState(IBlockAccess* pBlockAccess, const BlockPos& pos)
{
	bool bNorth = canThisPaneConnectToThisBlockID(pBlockAccess->getBlockId(pos.getNegZ()));
	bool bSouth = canThisPaneConnectToThisBlockID(pBlockAccess->getBlockId(pos.getPosZ()));
	bool bWest = canThisPaneConnectToThisBlockID(pBlockAccess->getBlockId(pos.getNegX()));
	bool bEast = canThisPaneConnectToThisBlockID(pBlockAccess->getBlockId(pos.getPosX()));
	float minx = 0.4375F;
	float maxx = 0.5625F;
	float minz = 0.4375F;
	float maxz = 0.5625F;

	if ((!bWest || !bEast) && (bWest || bEast || bNorth || bSouth))
	{
		if (bWest && !bEast)
		{
			minx = 0.0F;
		}
		else if (!bWest && bEast)
		{
			maxx = 1.0F;
		}
	}
	else
	{
		minx = 0.0F;
		maxx = 1.0F;
	}

	if ((!bNorth || !bSouth) && (bWest || bEast || bNorth || bSouth))
	{
		if (bNorth && !bSouth)
		{
			minz = 0.0F;
		}
		else if (!bNorth && bSouth)
		{
			maxz = 1.0F;
		}
	}
	else
	{
		minz = 0.0F;
		maxz = 1.0F;
	}

	setBlockBounds(minx, 0.0F, minz, maxx, 1.0F, maxz);
}

int BlockPane::idDropped(int meta, Random& rand, int fortune)
{
	return !m_canDropItSelf ? 0 : Block::idDropped(meta, rand, fortune);
}

bool BlockPane::canThisPaneConnectToThisBlockID(int blockID)
{
	return BlockManager::sOpaqueCubeLookup[blockID] || blockID == m_blockID || blockID == BLOCK_ID_GRASS;
}

BlockFenceGate::BlockFenceGate(int id, const BM_Material& mat, const String& name)
	: BlockDirectional(id, mat, name, false)
{
}

void BlockFenceGate::setBlockBoundsBasedOnState(IBlockAccess* pBlockAccess, const BlockPos& pos)
{
	int dir = getDirection(pBlockAccess->getBlockMeta(pos));

	if (dir != 2 && dir != 0)
	{
		setBlockBounds(0.375F, 0.0F, 0.0F, 0.625F, 1.0F, 1.0F);
	}
	else
	{
		setBlockBounds(0.0F, 0.0F, 0.375F, 1.0F, 1.0F, 0.625F);
	}
}

bool BlockFenceGate::canPlaceBlockAt(World* pWorld, const BlockPos& pos)
{
	return !pWorld->getBlockMaterial(pos.getNegY()).isSolid() ? false : Block::canPlaceBlockAt(pWorld, pos);
}

void BlockFenceGate::onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neightborBlockID)
{
	if (!pWorld->m_isClient)
	{
		int meta = pWorld->getBlockMeta(pos);
		bool bPower = pWorld->isBlockIndirectlyGettingPowered(pos);

		if (bPower || neightborBlockID > 0 && BlockManager::sBlocks[neightborBlockID]->canProvidePower())
		{
			if (bPower && !isFenceGateOpen(meta))
			{
				pWorld->setBlockMetadataWithNotify(pos, meta | 4, 2);
				//pWorld->playAuxSFXAtEntity((EntityPlayer)null, 1003, x, y, z, 0);
			}
			else if (!bPower && isFenceGateOpen(meta))
			{
				pWorld->setBlockMetadataWithNotify(pos, meta & -5, 2);
				//pWorld->playAuxSFXAtEntity((EntityPlayer)null, 1003, x, y, z, 0);
			}
		}
	}
}

Box BlockFenceGate::getCollisionBoundingBox(World* pWorld, const BlockPos& pos)
{
	int meta = pWorld->getBlockMeta(pos);

	if (isFenceGateOpen(meta))
		return Box();

	if (meta != 2 && meta != 0)
	{
		return Box(pos.x + 0.375f, (float)pos.y, (float)pos.z, pos.x + 0.625f, pos.y + 1.5f, pos.z + 1.f);
	}
	else
	{
		return Box((float)pos.x, (float)pos.y, pos.z + 0.375f, pos.x + 1.f, pos.y + 1.5f, pos.z + 0.625f);
	}
}

bool BlockFenceGate::getBlocksMovement(IBlockAccess* pAccess, const BlockPos& pos) const
{
	return isFenceGateOpen(pAccess->getBlockMeta(pos));
}

void BlockFenceGate::onBlockPlacedBy(World* pWorld, const BlockPos& pos, EntityLivingBase* pLiving, ItemStackPtr pStack)
{
	int meta = ((int)(Math::Floor((pLiving->rotationYaw * 4.0F / 360.0F) + 0.5f)) & 3) % 4;
	pWorld->setBlockMetadataWithNotify(pos, meta, 2);
}

bool BlockFenceGate::onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit)
{
	pPlayer->setActivatedBlock(pos);
	int meta = pWorld->getBlockMeta(pos);

	if (isFenceGateOpen(meta))
	{
		pWorld->setBlockMetadataWithNotify(pos, meta & -5, 2);
	}
	else
	{
		int dirMeta = ((int)(Math::Floor((pPlayer->rotationYaw * 4.0F / 360.0F) + 0.5f)) & 3) % 4;
		int dir = getDirection(meta);

		if (dir == (dirMeta + 2) % 4)
		{
			meta = dirMeta;
		}

		pWorld->setBlockMetadataWithNotify(pos, meta | 4, 2);
	}

	// todo.
	// par1World.playAuxSFXAtEntity(par5EntityPlayer, 1003, par2, par3, par4, 0);
	return true;
}

void BlockFenceGate::addCollisionBoxesToList(World * pWorld, const BlockPos & pos, const Box & aabb, AABBList & lst, Entity * pEntity)
{
	if (auto entityPlayer = dynamic_cast<EntityPlayer*>(pEntity))
	{
		if (entityPlayer->capabilities.isWatchMode)
		{
			return;
		}
	}
	Block::addCollisionBoxesToList(pWorld, pos, aabb, lst, pEntity);
}

StringArray BlockWall::types = { "normal", "mossy" };

BlockWall::BlockWall(int id, const BM_Material& mat, const String& name)
	: Block(id, mat, name, false)
{
}

void BlockWall::setBlockBoundsBasedOnState(IBlockAccess* pBlockAccess, const BlockPos& pos)
{
	bool canConnectZNeg = canConnectWallTo(pBlockAccess, pos.getNegZ());
	bool canConnectZPos = canConnectWallTo(pBlockAccess, pos.getPosZ());
	bool canConnectXNeg = canConnectWallTo(pBlockAccess, pos.getNegX());
	bool canConnectXPos = canConnectWallTo(pBlockAccess, pos.getPosX());
	float minX = 0.25F;
	float maxX = 0.75F;
	float minZ = 0.25F;
	float maxZ = 0.75F;
	float maxY = 1.0F;

	if (canConnectZNeg)
	{
		minZ = 0.0F;
	}

	if (canConnectZPos)
	{
		maxZ = 1.0F;
	}

	if (canConnectXNeg)
	{
		minX = 0.0F;
	}

	if (canConnectXPos)
	{
		maxX = 1.0F;
	}

	if (canConnectZNeg && canConnectZPos && !canConnectXNeg && !canConnectXPos)
	{
		maxY = 0.8125F;
		minX = 0.3125F;
		maxX = 0.6875F;
	}
	else if (!canConnectZNeg && !canConnectZPos && canConnectXNeg && canConnectXPos)
	{
		maxY = 0.8125F;
		minZ = 0.3125F;
		maxZ = 0.6875F;
	}

	setBlockBounds(minX, 0.0F, minZ, maxX, maxY, maxZ);
}

Box BlockWall::getCollisionBoundingBox(World* pWorld, const BlockPos& pos)
{
	setBlockBoundsBasedOnState(pWorld, pos);
	m_max.y = 1.5f;
	return Block::getCollisionBoundingBox(pWorld, pos);
}

bool BlockWall::canConnectWallTo(IBlockAccess* pBlockAccess, const BlockPos& pos)
{
	int blockID = pBlockAccess->getBlockId(pos);

	if (blockID != m_blockID && blockID != BLOCK_ID_FENCE_GATE)
	{
		Block* pBlock = BlockManager::sBlocks[blockID];
		return pBlock != NULL && pBlock->getMaterial().isOpaque() 
		&& pBlock->renderAsNormalBlock() ? pBlock->getMaterial() != BM_Material::BM_MAT_pumpkin : false;
	}
	else
	{
		return true;
	}
}

BlockBreakable::BlockBreakable(int id, const BM_Material& mat, const String& name, bool flag)
	: Block(id, mat, name, false)
	, m_flag(flag)
{
}

BlockGlass::BlockGlass(int id, const BM_Material& mat, const String& name)
	: BlockBreakable(id, mat, name, false)
{}

BlockAirWall::BlockAirWall(int id, const BM_Material& mat, const String& name)
	: BlockGlass(id, mat, name)
{}

BlockIce::BlockIce(int id, const BM_Material& mat, const String& name)
	: BlockBreakable(id, mat, name, false)
{	
}

void BlockIce::harvestBlock(World* pWorld, EntityPlayer* pPlayer, const BlockPos& pos, int damage)
{
	pPlayer->addStat(StatList::mineBlockStatArray[m_blockID], 1);
	pPlayer->addExhaustion(0.025F);

	if (canSilkHarvest() && EnchantmentHelper::getSilkTouchModifier(pPlayer))
	{
		ItemStackPtr pStack = createStackedBlock(damage);

		if (pStack)
			dropBlockAsItem_do(pWorld, pos, pStack);
	}
	else
	{
		if (pWorld->m_provider->isHellWorld)
		{
			pWorld->setBlockToAir(pos);
			return;
		}

		int fortune = EnchantmentHelper::getFortuneModifier(pPlayer);
		dropBlockAsItem(pWorld, pos, damage, fortune);
		const BM_Material& mat = pWorld->getBlockMaterial(pos.getNegY());

		if (mat.blocksMovement() || mat.isLiquid())
		{
			pWorld->setBlock(pos, BLOCK_ID_WATERMOVING);
		}
	}
}

void BlockIce::updateTick(World* pWorld, const BlockPos& pos, Random& rand)
{
	// todo.
	//if (pWorld->getSavedLightValue(0, pos) > 11 - BlockManager::sLightOpacity[m_blockID])
	if(false)
	{
		//if (pWorld->provider.isHellWorld)
		if(false)
		{
			pWorld->setBlockToAir(pos);
			return;
		}

		dropBlockAsItem(pWorld, pos, pWorld->getBlockMeta(pos), 0);
		pWorld->setBlock(pos, BLOCK_ID_WATERSTILL);
	}
}

BlockPackedIce::BlockPackedIce(int id, const BM_Material & mat, const String & name)
	: Block(id, mat, name)
{
	m_slipperiness = 0.98F;
}

BlockPortal::BlockPortal(int id, const BM_Material& mat, const String& name)
	: BlockBreakable(id, mat, name, false)
{
	setTickRandomly(true);
}

void BlockPortal::setBlockBoundsBasedOnState(IBlockAccess* pBlockAccess, const BlockPos& pos)
{
	float var5;
	float var6;

	if (pBlockAccess->getBlockId(pos.getNegX()) != m_blockID && pBlockAccess->getBlockId(pos.getPosX()) != m_blockID)
	{
		var5 = 0.125F;
		var6 = 0.5F;
		setBlockBounds(0.5F - var5, 0.0F, 0.5F - var6, 0.5F + var5, 1.0F, 0.5F + var6);
	}
	else
	{
		var5 = 0.5F;
		var6 = 0.125F;
		setBlockBounds(0.5F - var5, 0.0F, 0.5F - var6, 0.5F + var5, 1.0F, 0.5F + var6);
	}
}

void BlockPortal::updateTick(World* pWorld, const BlockPos& pos, Random& rand)
{
	BlockBreakable::updateTick(pWorld, pos, rand);
	
	if (pWorld->m_provider->isSurfaceWorld() && rand.nextInt(2000) < pWorld->m_difficultySetting)
	{
		int y;

		for (y = pos.y; !pWorld->doesBlockHaveSolidTopSurface(BlockPos(pos.x, y, pos.z)) && y > 0; --y)
		{
			;
		}

		if (y > 0 && !pWorld->isBlockNormalCube(BlockPos(pos.x, y + 1, pos.z)))
		{
			Vector3 spawn;
			spawn.x = pos.x + 0.5f;
			spawn.y = y + 1.1f;
			spawn.z = pos.z + 0.5f;
			Entity* pEntity = ItemMonsterPlacer::spawnCreature(pWorld, 57, spawn);

			if (pEntity != NULL)
			{
				pEntity->timeUntilPortal = pEntity->getPortalCooldown();
			}
		}
	}
}

void BlockPortal::onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborBlockID)
{
	int offx = 0;
	int offz = 1;

	if (pWorld->getBlockId(pos.getNegX()) == m_blockID || pWorld->getBlockId(pos.getPosX()) == m_blockID)
	{
		offx = 1;
		offz = 0;
	}

	int x = pos.x;
	int z = pos.z;
	int y = pos.y;

	for (; pWorld->getBlockId(BlockPos(x, y - 1, z)) == m_blockID; --y)
	{
		;
	}

	if (pWorld->getBlockId(BlockPos(x, y - 1, z)) != BLOCK_ID_OBSIDIAN)
	{
		pWorld->setBlockToAir(pos);
	}
	else
	{
		int iy = pos.y;

		for (; iy < 4 && pWorld->getBlockId(BlockPos(x, y + iy, z)) == m_blockID; ++iy)
		{
			;
		}

		if (iy == 3 && pWorld->getBlockId(BlockPos(x, y + iy, z)) == BLOCK_ID_OBSIDIAN)
		{
			bool var10 = pWorld->getBlockId(pos.getNegX()) == m_blockID || pWorld->getBlockId(pos.getPosX()) == m_blockID;
			bool var11 = pWorld->getBlockId(pos.getNegZ()) == m_blockID || pWorld->getBlockId(pos.getPosZ()) == m_blockID;

			if (var10 && var11)
			{
				pWorld->setBlockToAir(pos);
			}
			else
			{
				if ((pWorld->getBlockId(BlockPos(x + offx, y, z + offz)) != BLOCK_ID_OBSIDIAN || pWorld->getBlockId(BlockPos(x - offx, y, z - offz)) != m_blockID) &&
					(pWorld->getBlockId(BlockPos(x - offx, y, z - offz)) != BLOCK_ID_OBSIDIAN || pWorld->getBlockId(BlockPos(x + offx, y, z + offz)) != m_blockID))
				{
					pWorld->setBlockToAir(pos);
				}
			}
		}
		else
		{
			pWorld->setBlockToAir(pos);
		}
	}
}

void BlockPortal::onEntityCollidedWithBlock(World* pWorld, const BlockPos& pos, Entity* pEntity)
{
	if (pEntity->ridingEntity == NULL && pEntity->riddenByEntity == NULL)
	{
		pEntity->setInPortal();
	}
}

bool BlockPortal::tryToCreatePortal(World* pWorld, const BlockPos& pos)
{
	BlockPos pos1 = pos;
	int ex = 0;
	int ez = 0;

	if (pWorld->getBlockId(pos.getNegX()) == BLOCK_ID_OBSIDIAN ||
		pWorld->getBlockId(pos.getPosX()) == BLOCK_ID_OBSIDIAN)
	{
		ex = 1;
	}

	if (pWorld->getBlockId(pos.getNegZ()) == BLOCK_ID_OBSIDIAN ||
		pWorld->getBlockId(pos.getPosZ()) == BLOCK_ID_OBSIDIAN)
	{
		ez = 1;
	}

	if (ex == ez)
	{
		return false;
	}

	if (pWorld->getBlockId(pos - BlockPos(ex, 0, ez)) == 0)
	{
		pos1.x -= ex;
		pos1.z -= ez;
	}

	int ix;
	int iz;

	for (ix = -1; ix <= 2; ++ix)
	{
		for (iz = -1; iz <= 3; ++iz)
		{
			bool isBorder = ix == -1 || ix == 2 || iz == -1 || iz == 3;

			if (ix != -1 && ix != 2 || iz != -1 && iz != 3)
			{
				int blockID = pWorld->getBlockId(pos1 + BlockPos(ex * ix, iz, ez * ix));

				if (isBorder)
				{
					if (blockID != BLOCK_ID_OBSIDIAN)
					{
						return false;
					}
				}
				else if (blockID != 0 && blockID != BLOCK_ID_FIRE)
				{
					return false;
				}
			}
		}
	}

	for (ix = 0; ix < 2; ++ix)
	{
		for (iz = 0; iz < 3; ++iz)
		{
			pWorld->setBlock(pos1 + BlockPos(ex * ix, iz, ez * ix), BLOCK_ID_PORTAL, 0, 2);
		}
	}

	return true;
}

StringArray BlockQuartz::quartzBlockTypes = { "default", "chiseled", "lines" };

BlockQuartz::BlockQuartz(int id, const BM_Material& mat, const String& name)
	: Block(id, mat, name)
{
}

int BlockQuartz::onBlockPlaced(World* pWorld, const BlockPos& pos, int side, const Vector3& hit, int meta)
{
	if (meta == 2)
	{
		switch (side)
		{
		case 0:
		case 1:
			meta = 2;
			break;

		case 2:
		case 3:
			meta = 4;
			break;

		case 4:
		case 5:
			meta = 3;
		}
	}

	return meta;
}

ItemStackPtr BlockQuartz::createStackedBlock(int damage)
{
	if (damage != 3 && damage != 4)
		return Block::createStackedBlock(damage);

	return LORD::make_shared<ItemStack>(m_blockID, 1, 2);
}


BlockBeacon::BlockBeacon(int id, const BM_Material& mat, const String& name)
	: BlockContainer(id, mat, name, false)
{
	m_isOpaqueCube = false;
	setHardness(3.0F);
}

TileEntity* BlockBeacon::createNewTileEntity(World* pWorld)
{	
	return LordNew TileEntityBeacon();
}

bool BlockBeacon::onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit)
{
	if (pWorld->m_isClient)
		return true;

	pPlayer->setActivatedBlock(pos);
	// todo.
	// in client side need to open GUI
	/*
	TileEntityBeacon pBeacon = (TileEntityBeacon)pWorld->getBlockTileEntity(pos);

	if (pBeacon)
	{
	par5EntityPlayer.displayGUIBeacon(pBeacon);
	}
	*/

	return true;
}

void BlockBeacon::onBlockPlacedBy(World* pWorld, const BlockPos& pos, EntityLivingBase* pLiving, ItemStackPtr pStack)
{
	Block::onBlockPlacedBy(pWorld, pos, pLiving, pStack);

	if (pStack->hasDisplayName())
	{
		// todo.
		// ((TileEntityBeacon)par1World.getBlockTileEntity(par2, par3, par4)).func_94047_a(par6ItemStack.getDisplayName());
	}
}

StringArray BlockAnvil::statuses = { "intact", "slightlyDamaged", "veryDamaged" };

BlockAnvil::BlockAnvil(int id, const BM_Material& mat, const String& name)
	: BlockSand(id, mat, name)
{
	m_isOpaqueCube = false;
	setLightOpacity(0);
}

void BlockAnvil::setBlockBoundsBasedOnState(IBlockAccess* pBlockAccess, const BlockPos& pos)
{
	int state = pBlockAccess->getBlockMeta(pos) & 3;
	
	if (state != 3 && state != 1)
	{
		setBlockBounds(0.125F, 0.0F, 0.0F, 0.875F, 1.0F, 1.0F);
	}
	else
	{
		setBlockBounds(0.0F, 0.0F, 0.125F, 1.0F, 1.0F, 0.875F);
	}
}

void BlockAnvil::onBlockPlacedBy(World* pWorld, const BlockPos& pos, EntityLivingBase* pLiving, ItemStackPtr pStack)
{
	int dir = int(Math::Floor((double)(pLiving->rotationYaw * 4.0F / 360.0F) + 0.5f)) & 3;
	int meta = pWorld->getBlockMeta(pos) >> 2;
	++dir;
	dir %= 4;

	if (dir == 0)
	{
		pWorld->setBlockMetadataWithNotify(pos, 2 | meta << 2, 2);
	}

	if (dir == 1)
	{
		pWorld->setBlockMetadataWithNotify(pos, 3 | meta << 2, 2);
	}

	if (dir == 2)
	{
		pWorld->setBlockMetadataWithNotify(pos, 0 | meta << 2, 2);
	}

	if (dir == 3)
	{
		pWorld->setBlockMetadataWithNotify(pos, 1 | meta << 2, 2);
	}
}

bool BlockAnvil::onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit)
{
	if (pWorld->m_isClient)
		return true;

	pPlayer->setActivatedBlock(pos);
	// todo.
	// in client side need to display GUI
	// par5EntityPlayer.displayGUIAnvil(par2, par3, par4);
	return true;
}

void BlockAnvil::onFinishFalling(World* pWorld, const BlockPos& pos, int par5)
{
	// todo.
	// client side.
	// pWorld->playAuxSFX(1022, par2, par3, par4, 0);
}

BlockBanner::BlockBanner(int id, const BM_Material& mat, const String& name)
	: Block(id, mat, name)
{
	m_isOpaqueCube = false;
}

int BlockBanner::onBlockPlaced(World * pWorld, const BlockPos & pos, int side, const Vector3 & hit, int meta)
{
	//rad = (int)Math::Abs(Blockman::Instance()->m_pPlayer->getYaw()) % 360 / 22.5;
	return meta;
}

void BlockBanner::onBlockHarvested(World * pWorld, const BlockPos & pos, int meta, EntityPlayer * pPlayer)
{

}

void BlockBanner::onBlockDestroyedByPlayer(World * pWorld, const BlockPos & pos, int meta)
{

}

BlockPoweredOre::BlockPoweredOre(int id, const BM_Material& mat, const String& name)
	: BlockOreStorage(id, mat, name)
{
}

BlockFlowerPot::BlockFlowerPot(int id, const BM_Material& mat, const String& name)
	: Block(id, mat, name, false)
{
	setBlockBoundsForItemRender();
}
void BlockFlowerPot::setBlockBoundsForItemRender()
{
	float thickness = 0.375F;
	float bound = thickness / 2.0F;
	setBlockBounds(0.5F - bound, 0.0F, 0.5F - bound, 0.5F + bound, thickness, 0.5F + bound);
}

bool BlockFlowerPot::onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit)
{
	pPlayer->setActivatedBlock(pos);
	ItemStackPtr pStack = pPlayer->inventory->getCurrentItem();

	if (!pStack)
		return false;
	if (pWorld->getBlockMeta(pos) != 0)
		return false;
	
	int meta = getMetaForPlant(pStack);

	if (meta > 0)
	{
		pWorld->setBlockMetadataWithNotify(pos, meta, 2);

		if (!pPlayer->capabilities.isCreativeMode &&
			--pStack->stackSize <= 0)
		{
			pPlayer->inventory->setInventorySlotContents(pPlayer->inventory->getInventoryIndexOfCurrentItem(), NULL);
		}

		return true;
	}
	return false;
}

int BlockFlowerPot::idPicked(World* pWorld, const BlockPos& pos)
{
	ItemStackPtr pItemStack = getPlantForMeta(pWorld->getBlockMeta(pos));
	return pItemStack == NULL ? Item::flowerPot->itemID : pItemStack->itemID;
}

int BlockFlowerPot::getDamageValue(World* pWorld, const BlockPos& pos)
{
	ItemStackPtr pItemStack = getPlantForMeta(pWorld->getBlockMeta(pos));
	return pItemStack == NULL ? Item::flowerPot->itemID : pItemStack->getItemDamage();
}

bool BlockFlowerPot::canPlaceBlockAt(World* pWorld, const BlockPos& pos)
{
	return Block::canPlaceBlockAt(pWorld, pos) && pWorld->doesBlockHaveSolidTopSurface(pos.getNegY());
}

void BlockFlowerPot::onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborBlockID)
{
	if (!pWorld->doesBlockHaveSolidTopSurface(pos.getNegY()))
	{
		dropBlockAsItem(pWorld, pos, pWorld->getBlockMeta(pos), 0);
		pWorld->setBlockToAir(pos);
	}
}

void BlockFlowerPot::dropBlockAsItemWithChance(World* pWorld, const BlockPos& pos, int meta, float chance, int fortune)
{
	Block::dropBlockAsItemWithChance(pWorld, pos, meta, chance, fortune);

	if (meta > 0)
	{
		ItemStackPtr pItemStack = getPlantForMeta(meta);

		if (pItemStack != NULL)
		{
			dropBlockAsItem_do(pWorld, pos, pItemStack);
		}
	}
}

ItemStackPtr BlockFlowerPot::getPlantForMeta(int meta)
{
	switch (meta)
	{
	case 1:
		return LORD::make_shared<ItemStack>(BlockManager::plantRed);

	case 2:
		return LORD::make_shared<ItemStack>(BlockManager::plantYellow);

	case 3:
		return LORD::make_shared<ItemStack>(BlockManager::sapling, 1, 0);

	case 4:
		return LORD::make_shared<ItemStack>(BlockManager::sapling, 1, 1);

	case 5:
		return LORD::make_shared<ItemStack>(BlockManager::sapling, 1, 2);

	case 6:
		return LORD::make_shared<ItemStack>(BlockManager::sapling, 1, 3);

	case 7:
		return LORD::make_shared<ItemStack>(BlockManager::mushroomRed);

	case 8:
		return LORD::make_shared<ItemStack>(BlockManager::mushroomBrown);

	case 9:
		return LORD::make_shared<ItemStack>(BlockManager::cactus);

	case 10:
		return LORD::make_shared<ItemStack>(BlockManager::deadBush);

	case 11:
		return LORD::make_shared<ItemStack>(BlockManager::tallGrass, 1, 2);

	default:
		return NULL;
	}
}

int BlockFlowerPot::getMetaForPlant(ItemStackPtr pItemStack)
{
	int itemID = pItemStack->getItem()->itemID;

	if (itemID == BLOCK_ID_PLANT_RED)
	{
		return 1;
	}
	else if (itemID == BLOCK_ID_PLANT_YELLOW)
	{
		return 2;
	}
	else if (itemID == BLOCK_ID_CACTUS)
	{
		return 9;
	}
	else if (itemID == BLOCK_ID_MUSHROOM_BROWN)
	{
		return 8;
	}
	else if (itemID == BLOCK_ID_MUSHROOM_RED)
	{
		return 7;
	}
	else if (itemID == BLOCK_ID_DEADBUSH)
	{
		return 10;
	}
	else
	{
		if (itemID == BLOCK_ID_SAPLING)
		{
			switch (pItemStack->getItemDamage())
			{
			case 0:
				return 3;

			case 1:
				return 4;

			case 2:
				return 5;

			case 3:
				return 6;
			}
		}

		if (itemID == BLOCK_ID_TALL_GRASS)
		{
			switch (pItemStack->getItemDamage())
			{
			case 2:
				return 11;
			}
		}

		return 0;
	}
}

StringArray BlockSapling::WOOD_TYPES = { "oak", "spruce", "birch", "jungle", "acacia", "roofed_oak" };

BlockSapling::BlockSapling(int id, const BM_Material& mat, const String& name)
	: BlockFlower(id, mat, name)
{
	float bound = 0.4F;
	setBlockBounds(0.5F - bound, 0.0F, 0.5F - bound, 0.5F + bound, bound * 2.0F, 0.5F + bound);
}

void BlockSapling::updateTick(World* pWorld, const BlockPos& pos, Random& rand)
{
	if (!pWorld->m_isClient)
	{
		BlockFlower::updateTick(pWorld, pos, rand);

		// todo.
		//if (pWorld->getBlockLightValue(pos.getPosY()) >= 9 && rand.nextInt(7) == 0)
		if(false)
		{
			markOrGrowMarked(pWorld, pos, rand);
		}
	}
}

void BlockSapling::markOrGrowMarked(World* pWorld, const BlockPos& pos, Random& rand)
{
	int meta = pWorld->getBlockMeta(pos);

	if ((meta & 8) == 0)
	{
		pWorld->setBlockMetadataWithNotify(pos, meta | 8, 4);
	}
	else
	{
		growTree(pWorld, pos, rand);
	}
}

void BlockSapling::growTree(World* pWorld, const BlockPos& pos, Random& rand)
{
	/*
	int var6 = pWorld->getBlockMetadata(x, y, z) & 3;
	Object var7 = null;
	int var8 = 0;
	int var9 = 0;
	bool var10 = false;

	if (var6 == 1)
	{
		var7 = new WorldGenTaiga2(true);
	}
	else if (var6 == 2)
	{
		var7 = new WorldGenForest(true);
	}
	else if (var6 == 3)
	{
		for (var8 = 0; var8 >= -1; --var8)
		{
			for (var9 = 0; var9 >= -1; --var9)
			{
				if (isSameSapling(pWorld, x + var8, y, z + var9, 3) && 
					isSameSapling(pWorld, x + var8 + 1, y, z + var9, 3) && 
					isSameSapling(pWorld, x + var8, y, z + var9 + 1, 3) && 
					isSameSapling(pWorld, x + var8 + 1, y, z + var9 + 1, 3))
				{
					var7 = new WorldGenHugeTrees(true, 10 + rand.nextInt(20), 3, 3);
					var10 = true;
					break;
				}
			}

			if (var7 != null)
			{
				break;
			}
		}

		if (var7 == null)
		{
			var9 = 0;
			var8 = 0;
			var7 = new WorldGenTrees(true, 4 + rand.nextInt(7), 3, 3, false);
		}
	}
	else
	{
		var7 = new WorldGenTrees(true);

		if (rand.nextInt(10) == 0)
		{
			var7 = new WorldGenBigTree(true);
		}
	}

	if (var10)
	{
		pWorld->setBlock(x + var8, y, z + var9, 0, 0, 4);
		pWorld->setBlock(x + var8 + 1, y, z + var9, 0, 0, 4);
		pWorld->setBlock(x + var8, y, z + var9 + 1, 0, 0, 4);
		pWorld->setBlock(x + var8 + 1, y, z + var9 + 1, 0, 0, 4);
	}
	else
	{
		pWorld->setBlock(x, y, z, 0, 0, 4);
	}

	if (!((WorldGenerator)var7).generate(pWorld, rand, x + var8, y, z + var9))
	{
		if (var10)
		{
			pWorld->setBlock(x + var8, y, z + var9, blockID, var6, 4);
			pWorld->setBlock(x + var8 + 1, y, z + var9, blockID, var6, 4);
			pWorld->setBlock(x + var8, y, z + var9 + 1, blockID, var6, 4);
			pWorld->setBlock(x + var8 + 1, y, z + var9 + 1, blockID, var6, 4);
		}
		else
		{
			pWorld->setBlock(x, y, z, blockID, var6, 4);
		}
	}
	*/
}

bool BlockSapling::isSameSapling(World* pWorld, const BlockPos& pos, int meta)
{
	return pWorld->getBlockId(pos) == m_blockID && (pWorld->getBlockMeta(pos) & 3) == meta;
}

BlockRedstoneLight::BlockRedstoneLight(int id, const BM_Material& mat, const String& name, bool powered)
	: Block(id, mat, name)
	, m_powered(powered)
{
	if (powered)
	{
		//setLightValue(1.0F);
	}
}

void BlockRedstoneLight::onBlockAdded(World* pWorld, const BlockPos& pos)
{	
	if (!pWorld->m_isClient)
	{
		// error
		if (m_powered) //&& !pWorld->isBlockIndirectlyGettingPowered(x, y, z)) 
		{
			pWorld->scheduleBlockUpdate(pos, m_blockID, 4);
		}
		else if (!m_powered) //&& pWorld->isBlockIndirectlyGettingPowered(x, y, z))
		{
			pWorld->setBlock(pos, BLOCK_ID_REDSTONE_LAMP_IDLE, 0, 2);
		}
	}
}

void BlockRedstoneLight::onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborBlockID)
{
	if (!pWorld->m_isClient)
	{
		if (m_powered && !pWorld->isBlockIndirectlyGettingPowered(pos))
		{
			pWorld->scheduleBlockUpdate(pos, m_blockID, 4);
		}
		else if (!m_powered && pWorld->isBlockIndirectlyGettingPowered(pos))
		{
			pWorld->setBlock(pos, BLOCK_ID_REDSTONE_LAMP_ACTIVE, 0, 2);
		}
	}
}

void BlockRedstoneLight::updateTick(World* pWorld, const BlockPos& pos, Random& rand)
{

	if (!pWorld->m_isClient && m_powered && !pWorld->isBlockIndirectlyGettingPowered(pos))
	{
		pWorld->setBlock(pos, BLOCK_ID_REDSTONE_LAMP_IDLE, 0, 2);
	}
}

BlockColored::BlockColored(int id, const BM_Material& mat, const String name)
	: Block(id, mat, name)
{
}

BlockHay::BlockHay(int id, const BM_Material& mat, const String& name)
	: BlockRotatedPillar(id, mat, name)
{
}

BlockCarpet::BlockCarpet(int id, const BM_Material& mat, const String& name)
	: Block(id, mat, name, false)
{
	setBlockBounds(0.0F, 0.0F, 0.0F, 1.0F, 0.0625F, 1.0F);
	setTickRandomly(true);
	setCarpetBounding();
}

void BlockCarpet::setBlockBoundsBasedOnState(IBlockAccess* pBlockAccess, const BlockPos& pos)
{
	setCarpetBounding();
}

Box BlockCarpet::getCollisionBoundingBox(World* pWorld, const BlockPos& pos)
{
	int var5 = 1;
	float bound = 0;//0.0625F;
	return Box(pos.x + m_min.x, pos.y + m_min.y, pos.z + m_min.z, pos.x + m_max.x, pos.y + var5 * bound, pos.z + m_max.z);
}

bool BlockCarpet::canPlaceBlockAt(World* pWorld, const BlockPos& pos)
{
	return Block::canPlaceBlockAt(pWorld, pos) && canBlockStay(pWorld, pos);
}

void BlockCarpet::onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborBlockID)
{
	checkForDrop(pWorld, pos);
}

bool BlockCarpet::canBlockStay(World* pWorld, const BlockPos& pos)
{
	return !pWorld->isAirBlock(pos.getNegY());
}

void BlockCarpet::setCarpetBounding()
{
	setBlockBounds(0.0F, 0.0F, 0.0F, 1.0F, 0.0625f, 1.0F);
}

bool BlockCarpet::checkForDrop(World* pWorld, const BlockPos& pos)
{
	if (!canBlockStay(pWorld, pos))
	{
		dropBlockAsItem(pWorld, pos, pWorld->getBlockMeta(pos), 0);
		pWorld->setBlockToAir(pos);
		return false;
	}
	else
	{
		return true;
	}
}

BlockWeb::BlockWeb(int id, const BM_Material& mat, const String& name)
	: Block(id, mat, name, false)
{
}

void BlockWeb::onEntityCollidedWithBlock(World* pWorld, const BlockPos& pos, Entity* pEntity)
{
	pEntity->setInWeb();
}

Box BlockWeb::getCollisionBoundingBox(World* pWorld, const BlockPos& pos)
{
	return Box();
}

BlockTNT::BlockTNT(int id, const BM_Material& mat, const String& name)
	: Block(id, mat, name)
{
}

void BlockTNT::onBlockAdded(World* pWorld, const BlockPos& pos)
{
	Block::onBlockAdded(pWorld, pos);

	if (pWorld->isBlockIndirectlyGettingPowered(pos))
	{
		onBlockDestroyedByPlayer(pWorld, pos, 1);
		pWorld->setBlockToAir(pos);
	}
}

void BlockTNT::onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborBlockID)
{
	if (pWorld->isBlockIndirectlyGettingPowered(pos))
	{
		onBlockDestroyedByPlayer(pWorld, pos, 1);
		pWorld->setBlockToAir(pos);
	}
}

void BlockTNT::onBlockDestroyedByExplosion(World* pWorld, const BlockPos& pos, Explosion* pExplosion)
{
	if (pWorld->m_isClient) {
		LordLogInfo("onBlockDestroyedByExplosion");
		return;
	}

	Vector3 p = Vector3(pos.x + 0.5f, (float)pos.y, pos.z + 0.5f);
	EntityTNTPrimed* tnt = LordNew EntityTNTPrimed(pWorld, p, pExplosion->getExplosivePlacedBy());
	// int fuse = pWorld->m_Rand.nextInt(tnt->getFuse() / 4) + tnt->getFuse() / 8;
	int fuse = 80; // add by zhouyou. currently not sync the fuse param.
	tnt->setFuse(fuse);
	pWorld->spawnEntityInWorld(tnt);
	
	// todo.
	// in client side.
	/*
	EntityTNTPrimed var6 = new EntityTNTPrimed(par1World, (double)((float)par2 + 0.5F), (double)((float)par3 + 0.5F), (double)((float)par4 + 0.5F), par5Explosion.func_94613_c());
	var6.fuse = par1World.rand.nextInt(var6.fuse / 4) + var6.fuse / 8;
	par1World.spawnEntityInWorld(var6);
	*/
}

void BlockTNT::onBlockDestroyedByPlayer(World* pWorld, const BlockPos& pos, int meta)
{
	explode(pWorld, pos, meta, NULL);
}

bool BlockTNT::onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit)
{
	if (pPlayer->getCurrentEquippedItem() &&
		pPlayer->getCurrentEquippedItem()->itemID == Item::flintAndSteel->itemID)
	{
		explode(pWorld, pos, 1, pPlayer);
		pWorld->setBlockToAir(pos);
		pPlayer->getCurrentEquippedItem()->damageItem(1, pPlayer);
		return true;
	}
	else
	{
		return Block::onBlockActivated(pWorld, pos, pPlayer, face, hit);
	}
}

void BlockTNT::onEntityCollidedWithBlock(World* pWorld, const BlockPos& pos, Entity* pEntity)
{
	if (pWorld->m_isClient)
		return;


	if (!pEntity->isClass(ENTITY_CLASS_ARROW))
		return;

	EntityArrow* pArrow = (EntityArrow*)(pEntity);
	if (pArrow->isBurning())
	{
		EntityLivingBase* caster = NULL;
		Entity* pShootingEntity = pArrow->getShootingEntity();
		if (pShootingEntity != NULL && pShootingEntity->isClass(ENTITY_CLASS_LIVING_BASE))
			caster = (EntityLivingBase*)(pShootingEntity);
		explode(pWorld, pos, 1, caster);
		pWorld->setBlockToAir(pos);
	}
}

void BlockTNT::explode(World* pWorld, const BlockPos& pos, int meta, EntityLivingBase* caster)
{
	if (pWorld->m_isClient)
		return;

	if ((meta & 1) != 1)
		return;
	Vector3 p = Vector3(pos.x+0.5f, (float)pos.y,pos.z+0.5f);
	EntityTNTPrimed* tnt = LordNew EntityTNTPrimed(pWorld, p, caster);
	pWorld->spawnEntityInWorld(tnt);
	
	// todo.
	/*
	EntityTNTPrimed var7 = new EntityTNTPrimed(par1World, (double)((float)par2 + 0.5F), (double)((float)par3 + 0.5F), (double)((float)par4 + 0.5F), par6EntityLivingBase);
	par1World.spawnEntityInWorld(var7);
	// par1World.playSoundAtEntity(var7, "random.fuse", 1.0F, 1.0F);
	*/
}

BlockBookshelf::BlockBookshelf(int id, const BM_Material& mat, const String& name)
	: Block(id, mat, name)
{
}

BlockWorkbench::BlockWorkbench(int id, const BM_Material& mat, const String& name)
	: Block(id, mat, name)
{
}

BlockClay::BlockClay(int id, const BM_Material& mat, const String& name)
	: Block(id, mat, name)
{
}

BlockRedstoneOre::BlockRedstoneOre(int id, const BM_Material& mat, const String& name, bool glowing)
	: Block(id, mat, name)
	, m_glowing(glowing)
{
	if (glowing)
	{
		setTickRandomly(true);
	}
}

void BlockRedstoneOre::onBlockClicked(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer)
{
	glow(pWorld, pos);
	Block::onBlockClicked(pWorld, pos, pPlayer);
}

void BlockRedstoneOre::onEntityWalking(World* pWorld, const BlockPos& pos, Entity* pEntity)
{
	glow(pWorld, pos);
	Block::onEntityWalking(pWorld, pos, pEntity);
}

bool BlockRedstoneOre::onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit)
{
	glow(pWorld, pos);
	return Block::onBlockActivated(pWorld, pos, pPlayer, face, hit);
}

void BlockRedstoneOre::dropBlockAsItemWithChance(World* pWorld, const BlockPos& pos, int meta, float chance, int fortune)
{
	Block::dropBlockAsItemWithChance(pWorld, pos, meta, chance, fortune);

	if (idDropped(meta, pWorld->m_Rand, fortune) != m_blockID)
	{
		int exp = 1 + pWorld->m_Rand.nextInt(5);
		dropXpOnBlockBreak(pWorld, pos, exp);
	}
}

ItemStackPtr BlockRedstoneOre::createStackedBlock(int damage)
{
	return LORD::make_shared<ItemStack>(BlockManager::oreRedstone);
}

void BlockRedstoneOre::updateTick(World* pWorld, const BlockPos& pos, Random& rand)
{
	if (m_blockID == BLOCK_ID_ORE_REDSTONE_GLOWING)
	{
		pWorld->setBlock(pos, BLOCK_ID_ORE_REDSTONE);
	}
}

int BlockRedstoneOre::quantityDropped(Random& rand)
{
	return 4 + rand.nextInt(2);	
}

void BlockRedstoneOre::glow(World* pWorld, const BlockPos& pos)
{
	sparkle(pWorld, pos);

	if (m_blockID == BLOCK_ID_ORE_REDSTONE)
	{
		pWorld->setBlock(pos, BLOCK_ID_ORE_REDSTONE_GLOWING);
	}
}

BlockRailBase::BlockRailBase(int id, const BM_Material& mat, const String& name, bool isPowered)
	: Block(id, mat, name, false)
	, m_isPowered(isPowered)
{
	setBlockBounds(0.0F, 0.0F, 0.0F, 1.0F, 0.125F, 1.0F);
}

RayTraceResult BlockRailBase::collisionRayTrace(World* pWorld, const BlockPos& pos, const Vector3& start, const Vector3& end)
{
	setBlockBoundsBasedOnState(pWorld, pos);
	return Block::collisionRayTrace(pWorld, pos, start, end);
}

void BlockRailBase::setBlockBoundsBasedOnState(IBlockAccess* pBlockAccess, const BlockPos& pos)
{
	int meta = pBlockAccess->getBlockMeta(pos);

	if (meta >= 2 && meta <= 5)
	{
		setBlockBounds(0.0F, 0.0F, 0.0F, 1.0F, 0.625F, 1.0F);
	}
	else
	{
		setBlockBounds(0.0F, 0.0F, 0.0F, 1.0F, 0.125F, 1.0F);
	}
}

bool BlockRailBase::canPlaceBlockAt(World* pWorld, const BlockPos& pos)
{
	return pWorld->doesBlockHaveSolidTopSurface(pos.getNegY());
}

void BlockRailBase::onBlockAdded(World* pWorld, const BlockPos& pos)
{
	if (!pWorld->m_isClient)
	{		
		refreshTrackShape(pWorld, pos, true);

		if (m_isPowered)
		{
			onNeighborBlockChange(pWorld, pos, m_blockID);
		}
	}
}

void BlockRailBase::breakBlock(World* pWorld, const BlockPos& pos, int blockID, int meta)
{
	int meta2 = meta;

	if (isPowered())
	{
		meta2 = meta & 7;
	}

	Block::breakBlock(pWorld, pos, blockID, meta);

	if (meta2 == 2 || meta2 == 3 || meta2 == 4 || meta2 == 5)
	{
		pWorld->notifyBlocksOfNeighborChange(pos.getPosY(), blockID);
	}

	if (isPowered())
	{
		pWorld->notifyBlocksOfNeighborChange(pos, blockID);
		pWorld->notifyBlocksOfNeighborChange(pos.getNegY(), blockID);
	}
}

void BlockRailBase::refreshTrackShape(World* pWorld, const BlockPos& pos, bool par5)
{
	if (pWorld->m_isClient)
		return;
	
	BlockRailLogic* pLogic = LordNew BlockRailLogic(this, pWorld, pos);
	pLogic->checkConnectionToNeighbor(pWorld->isBlockIndirectlyGettingPowered(pos), par5);
	LordDelete pLogic;
}

void BlockRailBase::onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborBlockID)
{
	if (!pWorld->m_isClient)
	{
		int meta = pWorld->getBlockMeta(pos);
		int dir = meta;

		if (m_isPowered)
		{
			dir = meta & 7;
		}

		bool var8 = false;
		
		if (!pWorld->doesBlockHaveSolidTopSurface(pos.getNegY()))
		{
			var8 = true;
		}

		if (dir == 2 && !pWorld->doesBlockHaveSolidTopSurface(pos.getPosX()))
		{
			var8 = true;
		}

		if (dir == 3 && !pWorld->doesBlockHaveSolidTopSurface(pos.getNegX()))
		{
			var8 = true;
		}

		if (dir == 4 && !pWorld->doesBlockHaveSolidTopSurface(pos.getNegZ()))
		{
			var8 = true;
		}

		if (dir == 5 && !pWorld->doesBlockHaveSolidTopSurface(pos.getPosZ()))
		{
			var8 = true;
		}
		

		if (var8)
		{
			dropBlockAsItem(pWorld, pos, pWorld->getBlockMeta(pos), 0);
			pWorld->setBlockToAir(pos);
		}
		else
		{
			updateRailState(pWorld, pos, meta, dir, neighborBlockID);
		}
	}
}

bool BlockRailBase::isRailBlockAt(World* pWrold, const BlockPos& pos)
{
	return isRailBlock(pWrold->getBlockId(pos));
}

bool BlockRailBase::isRailBlock(int blockID)
{
	return blockID == BLOCK_ID_RAIL ||
		blockID == BLOCK_ID_RAIL_POWERED ||
		blockID == BLOCK_ID_RAIL_DETECTOR ||
		blockID == BLOCK_ID_RAIL_ACTIVATOR ||
		blockID == BLOCK_ID_RAIL_RECEIVE;
}

BlockRailPowered::BlockRailPowered(int id, const BM_Material& mat, const String& name)
	: BlockRailBase(id, mat, name, true)
{}

void BlockRailPowered::updateRailState(World* pWorld, const BlockPos& pos, int meta, int dir, int blockID)
{
	bool bPower = pWorld->isBlockIndirectlyGettingPowered(pos);
	bPower = bPower || findPoweredRailSignal(pWorld, pos, meta, true, 0) || findPoweredRailSignal(pWorld, pos, meta, false, 0);
	bool flag = false;

	if (bPower && (meta & 8) == 0)
	{
		pWorld->setBlockMetadataWithNotify(pos, dir | 8, 3);
		flag = true;
	}
	else if (!bPower && (meta & 8) != 0)
	{
		pWorld->setBlockMetadataWithNotify(pos, dir, 3);
		flag = true;
	}

	if (flag)
	{
		pWorld->notifyBlocksOfNeighborChange(pos.getNegY(), m_blockID);

		if (dir == 2 || dir == 3 || dir == 4 || dir == 5)
		{
			pWorld->notifyBlocksOfNeighborChange(pos.getPosY(), m_blockID);
		}
	}
}

bool BlockRailPowered::findPoweredRailSignal(World* pWorld, const BlockPos& pos, int meta, bool polarity, int dis)
{
	if (dis >= 8)
	{
		return false;
	}
	else
	{
		int dir = meta & 7;
		bool flag = true;
		BlockPos pos1 = pos;

		switch (dir)
		{
		case 0:
			if (polarity)
			{
				++pos1.z;
			}
			else
			{
				--pos1.z;
			}

			break;

		case 1:
			if (polarity)
			{
				--pos1.x;
			}
			else
			{
				++pos1.x;
			}

			break;

		case 2:
			if (polarity)
			{
				--pos1.x;
			}
			else
			{
				++pos1.x;
				++pos1.y;
				flag = false;
			}

			dir = 1;
			break;

		case 3:
			if (polarity)
			{
				--pos1.x;
				++pos1.y;
				flag = false;
			}
			else
			{
				++pos1.x;
			}

			dir = 1;
			break;

		case 4:
			if (polarity)
			{
				++pos1.z;
			}
			else
			{
				--pos1.z;
				++pos1.y;
				flag = false;
			}

			dir = 0;
			break;

		case 5:
			if (polarity)
			{
				++pos1.z;
				++pos1.y;
				flag = false;
			}
			else
			{
				--pos1.z;
			}

			dir = 0;
		}

		return isSameRailWithPower(pWorld, pos1, polarity, dis, dir) ? true : flag && isSameRailWithPower(pWorld, pos1.getNegY(), polarity, dis, dir);
	}
}

bool BlockRailPowered::isSameRailWithPower(World* pWorld, const BlockPos& pos, bool polarity, int dis, int dir)
{
	int blockID = pWorld->getBlockId(pos);

	if (blockID == m_blockID)
	{
		int meta = pWorld->getBlockMeta(pos);
		int meta2 = meta & 7;

		if (dir == 1 && (meta2 == 0 || meta2 == 4 || meta2 == 5))
		{
			return false;
		}

		if (dir == 0 && (meta2 == 1 || meta2 == 2 || meta2 == 3))
		{
			return false;
		}

		if ((meta & 8) != 0)
		{
			if (pWorld->isBlockIndirectlyGettingPowered(pos))
			{
				return true;
			}

			return findPoweredRailSignal(pWorld, pos, meta, polarity, dis + 1);
		}
	}

	return false;
}

BlockRail::BlockRail(int id, const BM_Material& mat, const String& name)
	: BlockRailBase(id, mat, name, false)
{}

void BlockRail::updateRailState(World* pWorld, const BlockPos& pos, int meta, int dir, int blockID)
{
	if (blockID > 0 && BlockManager::sBlocks[blockID]->canProvidePower()) //&& (new BlockBaseRailLogic(this, pWorld, x, y, z)).getNumberOfAdjacentTracks() == 3)
	{
		refreshTrackShape(pWorld, pos, false);
	}
}

BlockDetectorRail::BlockDetectorRail(int id, const BM_Material& mat, const String& name)
	: BlockRailBase(id, mat, name, true)
{
	setTickRandomly(true);
}

void BlockDetectorRail::updateTick(World* pWorld, const BlockPos& pos, Random& rand)
{
	if (!pWorld->m_isClient)
	{
		int meta = pWorld->getBlockMeta(pos);

		if ((meta & 8) != 0)
		{
			setStateIfMinecartInteractsWithRail(pWorld, pos, meta);
		}
	}
}

void BlockDetectorRail::onEntityCollidedWithBlock(World* pWorld, const BlockPos& pos, Entity* pEntity)
{
	if (pWorld->m_isClient)
		return;
	
	int meta = pWorld->getBlockMeta(pos);

	if ((meta & 8) == 0)
	{
		setStateIfMinecartInteractsWithRail(pWorld, pos, meta);
	}
}

int BlockDetectorRail::isProvidingWeakPower(IBlockAccess* piBlockAccess, const BlockPos& pos, int side)
{
	return (piBlockAccess->getBlockMeta(pos) & 8) != 0 ? 15 : 0;
}

int BlockDetectorRail::isProvidingStrongPower(IBlockAccess* piBlockAccess, const BlockPos& pos, int side)
{
	return (piBlockAccess->getBlockMeta(pos) & 8) == 0 ? 0 : (side == 1 ? 15 : 0);
}

void BlockDetectorRail::onBlockAdded(World* pWorld, const BlockPos& pos)
{
	BlockRailBase::onBlockAdded(pWorld, pos);
	setStateIfMinecartInteractsWithRail(pWorld, pos, pWorld->getBlockMeta(pos));
}

int BlockDetectorRail::getComparatorInputOverride(World* pWorld, const BlockPos& pos, int side)
{
	if ((pWorld->getBlockMeta(pos) & 8) > 0)
	{
		float bound = 0.125F;
		// todo.
		/*List var7 = pWorld->selectEntitiesWithinAABB(EntityMinecart.class, AxisAlignedBB.getAABBPool().getAABB((double)((float)par2 + bound), (double)par3, (double)((float)par4 + bound), (double)((float)(par2 + 1) - bound), (double)((float)(par3 + 1) - bound), (double)((float)(par4 + 1) - bound)), IEntitySelector.selectInventories);

		if (var7.size() > 0)
		{
			return Container.calcRedstoneFromInventory((IInventory)var7.get(0));
		}*/
	}

	return 0;
}

void BlockDetectorRail::setStateIfMinecartInteractsWithRail(World* pWorld, const BlockPos& pos, int meta)
{
	bool var6 = (meta & 8) != 0;
	bool hasEntity = false;
	float bound = 0.125F;
	
	EntityArr entities = pWorld->getEntitiesWithinAABB(
		ENTITY_CLASS_BLOCKMAN, 
		Box(pos.x + bound, (float)pos.y, pos.z + bound, 
		pos.x + 1.f - bound, pos.y + 1.f - bound, pos.z + 1.f - bound));

	if (!entities.empty())
	{
		hasEntity = true;
	}

	if (hasEntity && !var6)
	{
		pWorld->setBlockMetadataWithNotify(pos, meta | 8, 3);
		pWorld->notifyBlocksOfNeighborChange(pos, m_blockID);
		pWorld->notifyBlocksOfNeighborChange(pos.getNegY(), m_blockID);
		pWorld->markBlockRangeForRenderUpdate(pos, pos);
	}

	if (!hasEntity && var6)
	{
		pWorld->setBlockMetadataWithNotify(pos, meta & 7, 3);
		pWorld->notifyBlocksOfNeighborChange(pos, m_blockID);
		pWorld->notifyBlocksOfNeighborChange(pos.getNegY(), m_blockID);
		pWorld->markBlockRangeForRenderUpdate(pos, pos);
	}

	if (hasEntity)
	{
		pWorld->scheduleBlockUpdate(pos, m_blockID, tickRate(pWorld));
	}

	pWorld->updateComparatorOutputLevel(pos, m_blockID);
}

BlockPistonBase::BlockPistonBase(int id, const BM_Material& mat, const String& name, bool isSticky)
	: Block(id, mat, name, false)
	, m_isSticky(isSticky)
{
	setHardness(0.5F);
}

void BlockPistonBase::addCollisionBoxesToList(World* pWorld, const BlockPos& pos, const Box& aabb, AABBList& lst, Entity* pEntity)
{
	setBlockBounds(0.0F, 0.0F, 0.0F, 1.0F, 1.0F, 1.0F);
	Block::addCollisionBoxesToList(pWorld, pos, aabb, lst, pEntity);
}

Box BlockPistonBase::getCollisionBoundingBox(World* pWorld, const BlockPos& pos)
{
	setBlockBoundsBasedOnState(pWorld, pos);
	return Block::getCollisionBoundingBox(pWorld, pos);
}

void BlockPistonBase::setBlockBoundsBasedOnState(IBlockAccess* pBlockAccess, const BlockPos& pos)
{
	int meta = pBlockAccess->getBlockMeta(pos);

	if (isExtended(meta))
	{		

		switch (getOrientation(meta))
		{
		case 0:
			setBlockBounds(0.0F, 0.25F, 0.0F, 1.0F, 1.0F, 1.0F);
			break;

		case 1:
			setBlockBounds(0.0F, 0.0F, 0.0F, 1.0F, 0.75F, 1.0F);
			break;

		case 2:
			setBlockBounds(0.0F, 0.0F, 0.25F, 1.0F, 1.0F, 1.0F);
			break;

		case 3:
			setBlockBounds(0.0F, 0.0F, 0.0F, 1.0F, 1.0F, 0.75F);
			break;

		case 4:
			setBlockBounds(0.25F, 0.0F, 0.0F, 1.0F, 1.0F, 1.0F);
			break;

		case 5:
			setBlockBounds(0.0F, 0.0F, 0.0F, 0.75F, 1.0F, 1.0F);
		}
	}
	else
	{
		setBlockBounds(0.0F, 0.0F, 0.0F, 1.0F, 1.0F, 1.0F);
	}
}

void BlockPistonBase::onBlockPlacedBy(World* pWorld, const BlockPos& pos, EntityLivingBase* pLiving, ItemStackPtr pStack)
{
	int dir = determineOrientation(pWorld, pos, pLiving);
	pWorld->setBlockMetadataWithNotify(pos, dir, 2);

	if (!pWorld->m_isClient)
	{
		updatePistonState(pWorld, pos);
	}
}

void BlockPistonBase::onNeighborBlockChange(World* pWorld, const BlockPos& pos, int beighborBlockID)
{
	if (!pWorld->m_isClient)
	{
		updatePistonState(pWorld, pos);
	}
}

void BlockPistonBase::onBlockAdded(World* pWorld, const BlockPos& pos)
{
	if (!pWorld->m_isClient && 
		!pWorld->getBlockTileEntity(pos))
	{
		updatePistonState(pWorld, pos);
	}
}

bool BlockPistonBase::onBlockEventReceived(World* pWorld, const BlockPos& pos, int id, int param)
{
	if (!pWorld->m_isClient)
	{
		bool bPowered = isIndirectlyPowered(pWorld, pos, param);

		if (bPowered && id == 1)
		{
			pWorld->setBlockMetadataWithNotify(pos, param | 8, 2);
			return false;
		}

		if (!bPowered && id == 0)
		{
			return false;
		}
	}

	if (id == 0)
	{
		if (!tryExtend(pWorld, pos, param))
		{
			return false;
		}

		pWorld->setBlockMetadataWithNotify(pos, param | 8, 2);
		//pWorld->playSoundEffect(Vector3(pos) + 0.5f, "tile.piston.out", 0.5F, pWorld->m_Rand.nextFloat() * 0.25F + 0.6F);
		pWorld->playSoundEffectByType(Vector3(pos) + 0.5f, ST_PistonOut);
	}
	else if (id == 1)
	{
		// todo.
		TileEntity* pTileEntity = pWorld->getBlockTileEntity(pos + offsetForSide[param]);

		if (pTileEntity && pTileEntity->isClass(TILE_ENTITY_PISTON))
		{
			((TileEntityPiston*)pTileEntity)->clearPistonTileEntity();
		}

		pWorld->setBlock(pos, BLOCK_ID_PISTON_MOVING, param, 3);
		pWorld->setBlockTileEntity(pos, BlockPistonMoving::getTileEntity(m_blockID, param, param, false, true));

		// pull back.
		if (m_isSticky)
		{
			BlockPos pos1 = pos + offsetForSide[param] * 2;
			int blockID = pWorld->getBlockId(pos1);
			int meta = pWorld->getBlockMeta(pos1);
			bool var13 = false;

			if (blockID == BLOCK_ID_PISTON_MOVING)
			{
				TileEntity* pEntity = pWorld->getBlockTileEntity(pos1);

				if (pEntity->isClass(TILE_ENTITY_PISTON))
				{
					TileEntityPiston* piston = (TileEntityPiston*)pEntity;

					if (piston->getPistonOrientation() == param && piston->isExtending())
					{
						piston->clearPistonTileEntity();
						blockID = piston->getStoredBlockID();
						meta = piston->getBlockMetadata();
						var13 = true;
					}
				}
			}

			if (!var13 && blockID > 0 && canPushBlock(blockID, pWorld, pos1, false) &&
				(BlockManager::sBlocks[blockID]->getMobilityFlag() == 0 || blockID == BLOCK_ID_PISTON_BASE || blockID == BLOCK_ID_PISTON_STICKY_BASE))
			{
				BlockPos pos2 = pos + offsetForSide[param];
				pWorld->setBlock(pos2, BLOCK_ID_PISTON_MOVING, meta, 3);
				pWorld->setBlockTileEntity(pos2, BlockPistonMoving::getTileEntity(blockID, meta, param, false, false));
				pWorld->setBlockToAir(pos1);
			}
			else if (!var13)
			{
				pWorld->setBlockToAir(pos + offsetForSide[param]);
			}
		}
		else
		{
			pWorld->setBlockToAir(pos + offsetForSide[param]);
		}
		//pWorld->playSoundEffect(Vector3(pos) + 0.5f, "tile.piston.in", 0.5F, pWorld->m_Rand.nextFloat() * 0.15F + 0.6F);
		pWorld->playSoundEffectByType(Vector3(pos) + 0.5f, ST_PistonIn);
	}

	return true;
}

int BlockPistonBase::determineOrientation(World* pWorld, const BlockPos& pos, EntityLivingBase* pLiving)
{
	if (Math::Abs(pLiving->position.x - pos.x) < 2.0f && 
		Math::Abs(pLiving->position.z - pos.z) < 2.0f)
	{
		float var5 = pLiving->position.y + 1.82f - pLiving->yOffset;

		if (var5 - pos.y > 2.0f)
		{
			return 1;
		}

		if (pos.y - var5 > 0.0f)
		{
			return 0;
		}
	}

	int dir = int(Math::Floor((double)(pLiving->rotationYaw * 4.0F / 360.0F) + 0.5f)) & 3;
	return dir == 0 ? 2 : (dir == 1 ? 5 : (dir == 2 ? 3 : (dir == 3 ? 4 : 0)));
}

bool BlockPistonBase::canPushBlock(int blockID, World* pWorld, const BlockPos& pos, bool par5)
{
	if (blockID == BLOCK_ID_OBSIDIAN)
		return false;
	
	if (blockID != BLOCK_ID_PISTON_BASE &&
		blockID != BLOCK_ID_PISTON_STICKY_BASE)
	{
		if (BlockManager::sBlocks[blockID]->getBlockHardness(pWorld, pos) == -1.0F)
		{
			return false;
		}

		if (BlockManager::sBlocks[blockID]->getMobilityFlag() == 2)
		{
			return false;
		}

		if (BlockManager::sBlocks[blockID]->getMobilityFlag() == 1)
		{
			if (!par5)
				return false;

			return true;
		}
	}
	else if (isExtended(pWorld->getBlockMeta(pos)))
	{
		return false;
	}

	ITileEntityProvider* pTileEntityProvider = dynamic_cast<ITileEntityProvider*>(BlockManager::sBlocks[blockID]);
	return !pTileEntityProvider;
}

bool BlockPistonBase::canExtend(World* pWorld, const BlockPos& pos, int side)
{
	BlockPos pos1 = pos + offsetForSide[side];
	int forceRange = 0;

	while (true)
	{
		if (forceRange < 13)
		{
			if (pos1.y <= 0 || pos1.y >= 255)
			{
				return false;
			}

			int blockID = pWorld->getBlockId(pos1);

			if (blockID != 0)
			{
				if (!canPushBlock(blockID, pWorld, pos1, true))
				{
					return false;
				}

				if (BlockManager::sBlocks[blockID]->getMobilityFlag() != 1)
				{
					if (forceRange == 12)
					{
						return false;
					}

					pos1 += offsetForSide[side];
					++forceRange;
					continue;
				}
			}
		}

		return true;
	}
}

void BlockPistonBase::updatePistonState(World* pWorld, const BlockPos& pos)
{
	int meta = pWorld->getBlockMeta(pos);
	int dir = getOrientation(meta);

	if (dir != 7)
	{
		bool powered = isIndirectlyPowered(pWorld, pos, dir);

		if (powered && !isExtended(meta))
		{
			if (canExtend(pWorld, pos, dir))
			{
				pWorld->addBlockEvent(pos, m_blockID, 0, dir);
			}
		}
		else if (!powered && isExtended(meta))
		{
			pWorld->setBlockMetadataWithNotify(pos, dir, 2);
			pWorld->addBlockEvent(pos, m_blockID, 1, dir);
		}
	}
}

bool BlockPistonBase::isIndirectlyPowered(World* pWorld, const BlockPos& pos, int side)
{
	if (side != 0 && pWorld->getIndirectPowerOutput(pos.getNegY(), 0))
		return true;
	if (side != 1 && pWorld->getIndirectPowerOutput(pos.getPosY(), 1))
		return true;
	if (side != 2 && pWorld->getIndirectPowerOutput(pos.getNegZ(), 2))
		return true;
	if (side != 3 && pWorld->getIndirectPowerOutput(pos.getPosZ(), 3))
		return true;
	if (side != 4 && pWorld->getIndirectPowerOutput(pos.getNegX(), 4))
		return true;
	if (side != 5 && pWorld->getIndirectPowerOutput(pos.getPosX(), 5))
		return true;
	if (pWorld->getIndirectPowerOutput(pos, 0))
		return true;
	if (pWorld->getIndirectPowerOutput(pos + BlockPos(0, 2, 0), 1))
		return true;
	if (pWorld->getIndirectPowerOutput(pos.getUN(), 2))
		return true;
	if (pWorld->getIndirectPowerOutput(pos.getUS(), 3))
		return true;
	if (pWorld->getIndirectPowerOutput(pos.getUW(), 4))
		return true;
	if (pWorld->getIndirectPowerOutput(pos.getUE(), 5))
		return true;

	return false;
}

bool BlockPistonBase::tryExtend(World* pWorld, const BlockPos& pos, int side)
{
	BlockPos pos1 = pos + offsetForSide[side];
	int extendForce = 0;

	while (true)
	{
		int blockID;

		if (extendForce < 13)
		{
			if (pos1.y <= 0 || pos1.y >= 255)
			{
				return false;
			}

			blockID = pWorld->getBlockId(pos1);

			if (blockID != 0)
			{
				if (!canPushBlock(blockID, pWorld, pos1, true))
				{
					return false;
				}

				if (BlockManager::sBlocks[blockID]->getMobilityFlag() != 1)
				{
					if (extendForce == 12)
					{
						return false;
					}

					pos1 += offsetForSide[side];
					++extendForce;
					continue;
				}

				BlockManager::sBlocks[blockID]->dropBlockAsItem(pWorld, pos1, pWorld->getBlockMeta(pos1), 0);
				pWorld->setBlockToAir(pos1);
			}
		}

		BlockPos pos2 = pos1;
		int i = 0;
		int forceIDs[13];
		

		for (; pos2 != pos ;)
		{
			BlockPos pos3 = pos2 - offsetForSide[side];
			int blockID2 = pWorld->getBlockId(pos3);
			int meta = pWorld->getBlockMeta(pos3);

			if (blockID2 == m_blockID && pos3 == pos)
			{
				pWorld->setBlock(pos2, BLOCK_ID_PISTON_MOVING, side | (m_isSticky ? 8 : 0), 4);
				pWorld->setBlockTileEntity(pos2, BlockPistonMoving::getTileEntity(BLOCK_ID_PISTON_EXTENSION, side | (m_isSticky ? 8 : 0), side, true, false));
			}
			else
			{
				pWorld->setBlock(pos2, BLOCK_ID_PISTON_MOVING, meta, 4);
				pWorld->setBlockTileEntity(pos2, BlockPistonMoving::getTileEntity(blockID2, meta, side, true, false));
			}

			forceIDs[i++] = blockID2;
			pos2 = pos3;
		}


		for (i = 0; pos1 != pos; )
		{
			pos2 = pos1 - offsetForSide[side];
			pWorld->notifyBlocksOfNeighborChange(pos2, forceIDs[i++]);
			pos1 = pos2;
		}

		return true;
	}
}

BlockPistonExtension::BlockPistonExtension(int id, const BM_Material& mat, const String& name)
	: Block(id, mat, name, false)
{
	
	setHardness(0.5F);
}

void BlockPistonExtension::addCollisionBoxesToList(World* pWorld, const BlockPos& pos, const Box& aabb, AABBList& lst, Entity* pEntity)
{
	int meta = pWorld->getBlockMeta(pos);
	float var9 = 0.25F;
	float var10 = 0.375F;
	float var11 = 0.625F;
	float var12 = 0.25F;
	float var13 = 0.75F;

	switch (getDirectionMeta(meta))
	{
	case 0:
		setBlockBounds(0.0F, 0.0F, 0.0F, 1.0F, 0.25F, 1.0F);
		Block::addCollisionBoxesToList(pWorld, pos, aabb, lst, pEntity);
		setBlockBounds(0.375F, 0.25F, 0.375F, 0.625F, 1.0F, 0.625F);
		Block::addCollisionBoxesToList(pWorld, pos, aabb, lst, pEntity);
		break;

	case 1:
		setBlockBounds(0.0F, 0.75F, 0.0F, 1.0F, 1.0F, 1.0F);
		Block::addCollisionBoxesToList(pWorld, pos, aabb, lst, pEntity);
		setBlockBounds(0.375F, 0.0F, 0.375F, 0.625F, 0.75F, 0.625F);
		Block::addCollisionBoxesToList(pWorld, pos, aabb, lst, pEntity);
		break;

	case 2:
		setBlockBounds(0.0F, 0.0F, 0.0F, 1.0F, 1.0F, 0.25F);
		Block::addCollisionBoxesToList(pWorld, pos, aabb, lst, pEntity);
		setBlockBounds(0.25F, 0.375F, 0.25F, 0.75F, 0.625F, 1.0F);
		Block::addCollisionBoxesToList(pWorld, pos, aabb, lst, pEntity);
		break;

	case 3:
		setBlockBounds(0.0F, 0.0F, 0.75F, 1.0F, 1.0F, 1.0F);
		Block::addCollisionBoxesToList(pWorld, pos, aabb, lst, pEntity);
		setBlockBounds(0.25F, 0.375F, 0.0F, 0.75F, 0.625F, 0.75F);
		Block::addCollisionBoxesToList(pWorld, pos, aabb, lst, pEntity);
		break;

	case 4:
		setBlockBounds(0.0F, 0.0F, 0.0F, 0.25F, 1.0F, 1.0F);
		Block::addCollisionBoxesToList(pWorld, pos, aabb, lst, pEntity);
		setBlockBounds(0.375F, 0.25F, 0.25F, 0.625F, 0.75F, 1.0F);
		Block::addCollisionBoxesToList(pWorld, pos, aabb, lst, pEntity);
		break;

	case 5:
		setBlockBounds(0.75F, 0.0F, 0.0F, 1.0F, 1.0F, 1.0F);
		Block::addCollisionBoxesToList(pWorld, pos, aabb, lst, pEntity);
		setBlockBounds(0.0F, 0.375F, 0.25F, 0.75F, 0.625F, 0.75F);
		Block::addCollisionBoxesToList(pWorld, pos, aabb, lst, pEntity);
	}

	setBlockBounds(0.0F, 0.0F, 0.0F, 1.0F, 1.0F, 1.0F);
}

void BlockPistonExtension::setBlockBoundsBasedOnState(IBlockAccess* pBlockAccess, const BlockPos& pos)
{
	int meta = pBlockAccess->getBlockMeta(pos);
	float bound = 0.25F;

	switch (getDirectionMeta(meta))
	{
	case 0:
		setBlockBounds(0.0F, 0.0F, 0.0F, 1.0F, bound, 1.0F);
		break;

	case 1:
		setBlockBounds(0.0F, 1 - bound, 0.0F, 1.0F, 1.0F, 1.0F);
		break;

	case 2:
		setBlockBounds(0.0F, 0.0F, 0.0F, 1.0F, 1.0F, bound);
		break;

	case 3:
		setBlockBounds(0.0F, 0.0F, 1 - bound, 1.0F, 1.0F, 1.0F);
		break;

	case 4:
		setBlockBounds(0.0F, 0.0F, 0.0F, bound, 1.0F, 1.0F);
		break;

	case 5:
		setBlockBounds(1 - bound, 0.0F, 0.0F, 1.0F, 1.0F, 1.0F);
	}
}

void BlockPistonExtension::onBlockHarvested(World* pWorld, const BlockPos& pos, int meta, EntityPlayer* pPlayer)
{
	if (pPlayer->capabilities.isCreativeMode)
	{
		int dir = getDirectionMeta(meta);
		int blockID = pWorld->getBlockId(pos - offsetForSide[dir]);

		if (blockID == BLOCK_ID_PISTON_BASE || blockID == BLOCK_ID_PISTON_STICKY_BASE)
		{
			pWorld->setBlockToAir(pos - offsetForSide[dir]);
		}
	}

	Block::onBlockHarvested(pWorld, pos, meta, pPlayer);
}

void BlockPistonExtension::breakBlock(World* pWorld, const BlockPos& pos, int blockID, int meta)
{
	Block::breakBlock(pWorld, pos, blockID, meta);
	int opposite = oppositeSide[getDirectionMeta(meta)];
	BlockPos pos1 = pos + offsetForSide[opposite];
	int blockID2 = pWorld->getBlockId(pos1);

	if (blockID2 == BLOCK_ID_PISTON_BASE || blockID2 == BLOCK_ID_PISTON_STICKY_BASE)
	{
		meta = pWorld->getBlockMeta(pos1);

		if (BlockPistonBase::isExtended(meta))
		{
			BlockManager::sBlocks[blockID2]->dropBlockAsItem(pWorld, pos1, meta, 0);
			pWorld->setBlockToAir(pos1);
		}
	}
}

void BlockPistonExtension::onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborBlockID)
{
	int dir = getDirectionMeta(pWorld->getBlockMeta(pos));
	int blockID = pWorld->getBlockId(pos - offsetForSide[dir]);

	if (blockID != BLOCK_ID_PISTON_BASE && 
		blockID != BLOCK_ID_PISTON_STICKY_BASE)
	{
		pWorld->setBlockToAir(pos);
	}
	else
	{
		BlockManager::sBlocks[blockID]->onNeighborBlockChange(pWorld, pos - offsetForSide[dir], neighborBlockID);
	}
}

int BlockPistonExtension::idPicked(World* pWorld, const BlockPos& pos)
{
	int meta = pWorld->getBlockMeta(pos);
	return (meta & 8) != 0 ? BLOCK_ID_PISTON_STICKY_BASE : BLOCK_ID_PISTON_BASE;
}

BlockPistonMoving::BlockPistonMoving(int id, const BM_Material& mat, const String& name)
: BlockContainer(id, mat, name, false)
{
	setHardness(-1.0f);
}

void BlockPistonMoving::breakBlock(World* pWorld, const BlockPos& pos, int blockID, int meta)
{
	TileEntity* pTileEntity = pWorld->getBlockTileEntity(pos);

	if (pTileEntity && pTileEntity->isClass(TILE_ENTITY_PISTON))
	{
		((TileEntityPiston*)pTileEntity)->clearPistonTileEntity();
	}
	else
	{
		BlockContainer::breakBlock(pWorld, pos, blockID, meta);
	}
}

bool BlockPistonMoving::onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit)
{
	if (!pWorld->m_isClient && !pWorld->getBlockTileEntity(pos))
	{
		pPlayer->setActivatedBlock(pos);
		pWorld->setBlockToAir(pos);
		return true;
	}
	else
	{
		return false;
	}
}

void BlockPistonMoving::dropBlockAsItemWithChance(World* pWorld, const BlockPos& pos, int meta, float chance, int fortune)
{
	if (!pWorld->m_isClient)
	{
		// todo.
		/*TileEntityPiston pPiston = getTileEntityAtLocation(pWorld, pos);

		if (pPiston != null)
		{
			Block.blocksList[pPiston.getStoredBlockID()].dropBlockAsItem(par1World, par2, par3, par4, pPiston.getBlockMetadata(), 0);
		}*/
	}
}

void BlockPistonMoving::onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborID)
{
	if (!pWorld->m_isClient)
	{
		pWorld->getBlockTileEntity(pos);
		// then?
	}
}

TileEntity* BlockPistonMoving::getTileEntity(int id, int meta, int side, bool extending, bool renderhead)
{
	return LordNew TileEntityPiston(id, meta, side, extending, renderhead);
}

Box BlockPistonMoving::getCollisionBoundingBox(World* pWorld, const BlockPos& pos)
{
	TileEntityPiston* pPiston = getTileEntityAtLocation(pWorld, pos);

	if (!pPiston)
	{
		return Box();
	}
	else
	{
		float progress = pPiston->getProgress(0.0F);

		if (pPiston->isExtending())
		{
			progress = 1.0F - progress;
		}

		return getAxisAlignedBB(pWorld, pos, pPiston->getStoredBlockID(), progress, pPiston->getPistonOrientation());
	}
}

void BlockPistonMoving::setBlockBoundsBasedOnState(IBlockAccess* piBlockAccess, const BlockPos& pos)
{
	TileEntityPiston* pPiston = getTileEntityAtLocation(piBlockAccess, pos);

	if (pPiston)
	{
		Block* pBlock = BlockManager::sBlocks[pPiston->getStoredBlockID()];

		if (!pBlock || pBlock == this)
		{
			return;
		}

		pBlock->setBlockBoundsBasedOnState(piBlockAccess, pos);
		float progress = pPiston->getProgress(0.0F);

		if (pPiston->isExtending())
		{
			progress = 1.0F - progress;
		}

		int dir = pPiston->getPistonOrientation();
		m_min = pBlock->getBlockBoundsMin() - Vector3(offsetForSide[dir]) * progress;
		m_max = pBlock->getBlockBoundsMax() - Vector3(offsetForSide[dir]) * progress;
	}
}

Box BlockPistonMoving::getAxisAlignedBB(World* pWorld, const BlockPos& pos, int blockID, float progress, int side)
{
	if (blockID != 0 && blockID != m_blockID)
	{
		Box aabb = BlockManager::sBlocks[blockID]->getCollisionBoundingBox(pWorld, pos);

		if (aabb.isNull())
		{
			return aabb;
		}
		else
		{
			aabb.vMin -= Vector3(offsetForSide[side]) * progress;
			aabb.vMax -= Vector3(offsetForSide[side]) * progress;
			return aabb;
		}
	}
	return Box();
}

TileEntityPiston* BlockPistonMoving::getTileEntityAtLocation(IBlockAccess* pAccess, const BlockPos& pos)
{
	TileEntity* pTileEntity = pAccess->getBlockTileEntity(pos);
	TileEntityPiston* pPiston = dynamic_cast<TileEntityPiston*>(pTileEntity);
	return pPiston;
}

BlockTorch::BlockTorch(int id, const BM_Material& mat, const String& name)
	: Block(id, mat, name, false)
{
	setTickRandomly(true);
}

bool BlockTorch::canPlaceTorchOn(World* pWorld, const BlockPos& pos)
{
	if (pWorld->doesBlockHaveSolidTopSurface(pos))
	{
		return true;
	}
	else
	{
		int blockID = pWorld->getBlockId(pos);

		return 
			blockID == BLOCK_ID_FENCE || 
			blockID == BLOCK_ID_NETHER_FENCE ||
			blockID == BLOCK_ID_GLASS ||
			blockID == BLOCK_ID_COBBLE_STONE_WALL;
	}
}

bool BlockTorch::canPlaceBlockAt(World* pWorld, const BlockPos& pos)
{
	return pWorld->isBlockNormalCubeDefault(pos.getNegX(), true) ? true : 
		  (pWorld->isBlockNormalCubeDefault(pos.getPosX(), true) ? true : 
		  (pWorld->isBlockNormalCubeDefault(pos.getNegZ(), true) ? true : 
		  (pWorld->isBlockNormalCubeDefault(pos.getPosZ(), true) ? true : canPlaceTorchOn(pWorld, pos.getNegY()))));
}

int BlockTorch::onBlockPlaced(World* pWorld, const BlockPos& pos, int side, const Vector3& hit, int meta)
{
	int meta2 = meta;

	if (side == 1 && canPlaceTorchOn(pWorld, pos.getNegY()))
	{
		meta2 = 5;
	}

	if (side == 2 && pWorld->isBlockNormalCubeDefault(pos.getPosZ(), true))
	{
		meta2 = 4;
	}

	if (side == 3 && pWorld->isBlockNormalCubeDefault(pos.getNegZ(), true))
	{
		meta2 = 3;
	}

	if (side == 4 && pWorld->isBlockNormalCubeDefault(pos.getPosX(), true))
	{
		meta2 = 2;
	}

	if (side == 5 && pWorld->isBlockNormalCubeDefault(pos.getNegX(), true))
	{
		meta2 = 1;
	}

	return meta2;
}

void BlockTorch::updateTick(World* pWorld, const BlockPos& pos, Random& rand)
{
	Block::updateTick(pWorld, pos, rand);

	if (pWorld->getBlockMeta(pos) == 0)
	{
		onBlockAdded(pWorld, pos);
	}
}

void BlockTorch::onBlockAdded(World* pWorld, const BlockPos& pos)
{
	if (pWorld->getBlockMeta(pos) == 0)
	{
		if (pWorld->isBlockNormalCubeDefault(pos.getNegX(), true))
		{
			pWorld->setBlockMetadataWithNotify(pos, 1, 2);
		}
		else if (pWorld->isBlockNormalCubeDefault(pos.getPosX(), true))
		{
			pWorld->setBlockMetadataWithNotify(pos, 2, 2);
		}
		else if (pWorld->isBlockNormalCubeDefault(pos.getNegZ(), true))
		{
			pWorld->setBlockMetadataWithNotify(pos, 3, 2);
		}
		else if (pWorld->isBlockNormalCubeDefault(pos.getPosZ(), true))
		{
			pWorld->setBlockMetadataWithNotify(pos, 4, 2);
		}
		else if (canPlaceTorchOn(pWorld, pos.getPosY()))
		{
			pWorld->setBlockMetadataWithNotify(pos, 5, 2);
		}
	}
	
	dropTorchIfCantStay(pWorld, pos);
}

void BlockTorch::onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborBlockID)
{
	onNeighborChange_impl(pWorld, pos);
}

RayTraceResult BlockTorch::collisionRayTrace(World* pWorld, const BlockPos& pos, const Vector3& start, const Vector3& end)
{
	int meta = pWorld->getBlockMeta(pos) & 7;
	float bound = 0.15F;

	if (meta == 1)
	{
		setBlockBounds(0.0F, 0.2F, 0.5F - bound, bound * 2.0F, 0.8F, 0.5F + bound);
	}
	else if (meta == 2)
	{
		setBlockBounds(1.0F - bound * 2.0F, 0.2F, 0.5F - bound, 1.0F, 0.8F, 0.5F + bound);
	}
	else if (meta == 3)
	{
		setBlockBounds(0.5F - bound, 0.2F, 0.0F, 0.5F + bound, 0.8F, bound * 2.0F);
	}
	else if (meta == 4)
	{
		setBlockBounds(0.5F - bound, 0.2F, 1.0F - bound * 2.0F, 0.5F + bound, 0.8F, 1.0F);
	}
	else
	{
		bound = 0.1F;
		setBlockBounds(0.5F - bound, 0.0F, 0.5F - bound, 0.5F + bound, 0.6F, 0.5F + bound);
	}

	return Block::collisionRayTrace(pWorld, pos, start, end);
}

bool BlockTorch::onNeighborChange_impl(World* pWorld, const BlockPos& pos)
{
	if (dropTorchIfCantStay(pWorld, pos))
	{
		int meta = pWorld->getBlockMeta(pos);
		bool flag = false;

		if (!pWorld->isBlockNormalCubeDefault(pos.getNegX(), true) && meta == 1)
		{
			flag = true;
		}

		if (!pWorld->isBlockNormalCubeDefault(pos.getPosX(), true) && meta == 2)
		{
			flag = true;
		}

		if (!pWorld->isBlockNormalCubeDefault(pos.getNegZ(), true) && meta == 3)
		{
			flag = true;
		}

		if (!pWorld->isBlockNormalCubeDefault(pos.getPosZ(), true) && meta == 4)
		{
			flag = true;
		}

		if (!canPlaceTorchOn(pWorld, pos.getNegY()) && meta == 5)
		{
			flag = true;
		}

		if (flag)
		{
			dropBlockAsItem(pWorld, pos, pWorld->getBlockMeta(pos), 0);
			pWorld->setBlockToAir(pos);
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return true;
	}
}

bool BlockTorch::dropTorchIfCantStay(World* pWorld, const BlockPos& pos)
{
	if (!canPlaceBlockAt(pWorld, pos))
	{
		if (pWorld->getBlockId(pos) == m_blockID)
		{
			dropBlockAsItem(pWorld, pos, pWorld->getBlockMeta(pos), 0);
			pWorld->setBlockToAir(pos);
		}

		return false;
	}
	else
	{
		return true;
	}
}

BlockRedstoneTorch::RedstoneUpdateInfoArr BlockRedstoneTorch::redstoneUpdateInfo;

BlockRedstoneTorch::BlockRedstoneTorch(int id, const BM_Material& mat, const String& name, bool isActive)
	: BlockTorch(id, mat, name)
	, m_isActive(isActive)
{
	setTickRandomly(true);
}

void BlockRedstoneTorch::onBlockAdded(World* pWorld, const BlockPos& pos)
{
	
	if (pWorld->getBlockMeta(pos) == 0)
	{
		BlockTorch::onBlockAdded(pWorld, pos);
	}

	if (m_isActive)
	{
		pWorld->notifyBlocksOfNeighborChange(pos.getNegY(), m_blockID);
		pWorld->notifyBlocksOfNeighborChange(pos.getPosY(), m_blockID);
		pWorld->notifyBlocksOfNeighborChange(pos.getNegX(), m_blockID);
		pWorld->notifyBlocksOfNeighborChange(pos.getPosX(), m_blockID);
		pWorld->notifyBlocksOfNeighborChange(pos.getNegZ(), m_blockID);
		pWorld->notifyBlocksOfNeighborChange(pos.getPosZ(), m_blockID);
	}
	
}

void BlockRedstoneTorch::breakBlock(World* pWorld, const BlockPos& pos, int blockID, int meta)
{
	if (m_isActive)
	{
		pWorld->notifyBlocksOfNeighborChange(pos.getNegY(), m_blockID);
		pWorld->notifyBlocksOfNeighborChange(pos.getPosY(), m_blockID);
		pWorld->notifyBlocksOfNeighborChange(pos.getNegX(), m_blockID);
		pWorld->notifyBlocksOfNeighborChange(pos.getPosX(), m_blockID);
		pWorld->notifyBlocksOfNeighborChange(pos.getNegZ(), m_blockID);
		pWorld->notifyBlocksOfNeighborChange(pos.getPosZ(), m_blockID);
	}
}

int BlockRedstoneTorch::isProvidingWeakPower(IBlockAccess* piBlockAccess, const BlockPos& pos, int side)
{
	if (!m_isActive)
	{
		return 0;
	}
	else
	{
		int meta = piBlockAccess->getBlockMeta(pos);
		return meta == 5 && side == 1 ? 0 : (meta == 3 && side == 3 ? 0 : (meta == 4 && side == 2 ? 0 : (meta == 1 && side == 5 ? 0 : (meta == 2 && side == 4 ? 0 : 15))));
	}
}

void BlockRedstoneTorch::updateTick(World* pWorld, const BlockPos& pos, Random& rand)
{
	bool powered = isIndirectlyPowered(pWorld, pos);

	for (auto it = redstoneUpdateInfo.begin(); it != redstoneUpdateInfo.end(); )
	{
		if (pWorld->getTotalWorldTime() - it->second > 60L)
			it = redstoneUpdateInfo.erase(it);
		else
			++it;
	}

	if (m_isActive)
	{
		if (powered)
		{
			pWorld->setBlock(pos, BLOCK_ID_TORCH_REDSTONE_IDLE, pWorld->getBlockMeta(pos), 3);

			if (checkForBurnout(pWorld, pos, true))
			{
				//pWorld->playSoundEffect(Vector3(pos) + Vector3::HALF, "random.fizz", 0.5f, 2.6f + (rand.nextFloat() - rand.nextFloat()) * 0.8f);
				pWorld->playSoundEffectByType(Vector3(pos) + Vector3::HALF, ST_Fizz);
				for (int I = 0; I < 5; ++I)
				{
					pWorld->spawnParticle("smoke", Vector3(pos) + rand.nextVector() * 0.6f + 0.2f, Vector3::ZERO);
				}
			}
		}
	}
	else if (!powered && !checkForBurnout(pWorld, pos, false))
	{
		pWorld->setBlock(pos, BLOCK_ID_TORCH_REDSTONE_ACTIVE, pWorld->getBlockMeta(pos), 3);
	}
}

void BlockRedstoneTorch::onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborBlockID)
{
	if (!onNeighborChange_impl(pWorld, pos))
	{
		bool powered = isIndirectlyPowered(pWorld, pos);

		if (m_isActive && powered || !m_isActive && !powered)
		{
			pWorld->scheduleBlockUpdate(pos, m_blockID, tickRate(pWorld));
		}
	}
}

int BlockRedstoneTorch::isProvidingStrongPower(IBlockAccess* iBlockAccess, const BlockPos& pos, int side)
{
	return side == 0 ? isProvidingWeakPower(iBlockAccess, pos, side) : 0;
}

int BlockRedstoneTorch::idPicked(World* pWorld, const BlockPos& pos)
{
	return BLOCK_ID_TORCH_REDSTONE_ACTIVE;
}

bool BlockRedstoneTorch::checkForBurnout(World* pWorld, const BlockPos& pos, bool par5)
{
	if (par5)
	{
		redstoneUpdateInfo.push_back(std::make_pair(pos, pWorld->getTotalWorldTime()));
	}

	int count = 0;
	for (auto it = redstoneUpdateInfo.begin(); it != redstoneUpdateInfo.end(); ++it)
	{
		if (it->first == pos)
		{
			++count;
			if (count >= 8)
				return true;
		}
	}
	return false;
}

bool BlockRedstoneTorch::isIndirectlyPowered(World* pWorld, const BlockPos& pos)
{
	int meta = pWorld->getBlockMeta(pos);
	return meta == 5 && pWorld->getIndirectPowerOutput(pos.getNegY(), 0) ? true : 
		  (meta == 3 && pWorld->getIndirectPowerOutput(pos.getNegZ(), 2) ? true : 
		  (meta == 4 && pWorld->getIndirectPowerOutput(pos.getPosZ(), 3) ? true : 
		  (meta == 1 && pWorld->getIndirectPowerOutput(pos.getNegX(), 4) ? true : 
		   meta == 2 && pWorld->getIndirectPowerOutput(pos.getPosX(), 5))));
}

BlockFire::BlockFire(int id, const BM_Material& mat, const String& name)
	: Block(id, mat, name, false)
{
	setTickRandomly(true);
}

void BlockFire::initializeBlock()
{
	setBurnRate(BLOCK_ID_PLANKS, 5, 20);
	setBurnRate(BLOCK_ID_WOOD_DOUBLE_SLAB, 5, 20);
	setBurnRate(BLOCK_ID_WOOD_SINGLE_SLAB, 5, 20);
	setBurnRate(BLOCK_ID_FENCE, 5, 20);
	setBurnRate(BLOCK_ID_STAIRS_WOOD_OAK, 5, 20);
	setBurnRate(BLOCK_ID_STAIRS_WOOD_BIRCH, 5, 20);
	setBurnRate(BLOCK_ID_STAIRS_WOOD_SPURCE, 5, 20);
	setBurnRate(BLOCK_ID_STAIRS_WOOD_JUNGLE, 5, 20);
	setBurnRate(BLOCK_ID_WOOD, 5, 5);
	setBurnRate(BLOCK_ID_WOOD2, 5, 5);
	setBurnRate(BLOCK_ID_LEAVES, 30, 60);
	setBurnRate(BLOCK_ID_BOOKSHELF, 30, 20);
	setBurnRate(BLOCK_ID_TNT, 15, 100);
	setBurnRate(BLOCK_ID_TALL_GRASS, 60, 100);
	setBurnRate(BLOCK_ID_CLOTH, 30, 60);
	setBurnRate(BLOCK_ID_VINE, 15, 100);
	//setBurnRate(Block.field_111034_cE.blockID, 5, 5);
	//setBurnRate(Block.field_111038_cB.blockID, 60, 20);
}

void BlockFire::updateTick(World* pWorld, const BlockPos& pos, Random& rand)
{
	/*
	if (pWorld->getGameRules().getGameRuleBooleanValue("doFireTick"))
	{
		bool var6 = pWorld->getBlockId(x, y - 1, z) == BLOCK_ID_NETHERRACK;

		if (pWorld->provider instanceof WorldProviderEnd && par1World.getBlockId(par2, par3 - 1, par4) == Block.bedrock.blockID)
		{
			var6 = true;
		}

		if (!canPlaceBlockAt(pWorld, x, y, z))
		{
			pWorld->setBlockToAir(x, y, z);
		}

		if (!var6 && pWorld->isRaining() && (
			pWorld->canLightningStrikeAt(x, y, z) || 
			pWorld->canLightningStrikeAt(pos.getNeighborNegX()) || 
			pWorld->canLightningStrikeAt(pos.getNeighborPosX()) || 
			pWorld->canLightningStrikeAt(pos.getNeighborNegZ()) || 
			pWorld->canLightningStrikeAt(pos.getNeighborPosZ())))
		{
			pWorld->setBlockToAir(x, y, z);
		}
		else
		{
			int var7 = pWorld->getBlockMetadata(x, y, z);

			if (var7 < 15)
			{
				pWorld->setBlockMetadataWithNotify(x, y, z, var7 + rand.nextInt(3) / 2, 4);
			}

			pWorld->scheduleBlockUpdate(x, y, z, m_blockID, tickRate(pWorld) + rand.nextInt(10));

			if (!var6 && !canNeighborBurn(pWorld, x, y, z))
			{
				if (!pWorld->doesBlockHaveSolidTopSurface(x, y - 1, z) || var7 > 3)
				{
					pWorld->setBlockToAir(x, y, z);
				}
			}
			else if (!var6 && !canBlockCatchFire(pWorld, x, y - 1, z) && var7 == 15 && rand.nextInt(4) == 0)
			{
				pWorld->setBlockToAir(x, y, z);
			}
			else
			{
				bool var8 = pWorld->isBlockHighHumidity(x, y, z);
				int var9 = 0;

				if (var8)
				{
					var9 = -50;
				}

				tryToCatchBlockOnFire(pWorld, pos.getNeighborPosX(), 300 + var9, rand, var7);
				tryToCatchBlockOnFire(pWorld, pos.getNeighborNegX(), 300 + var9, rand, var7);
				tryToCatchBlockOnFire(pWorld, x, y - 1, z, 250 + var9, rand, var7);
				tryToCatchBlockOnFire(pWorld, x, y + 1, z, 250 + var9, rand, var7);
				tryToCatchBlockOnFire(pWorld, pos.getNeighborNegZ(), 300 + var9, rand, var7);
				tryToCatchBlockOnFire(pWorld, pos.getNeighborPosZ(), 300 + var9, rand, var7);

				for (int var10 = x - 1; var10 <= x + 1; ++var10)
				{
					for (int var11 = z - 1; var11 <= z + 1; ++var11)
					{
						for (int var12 = y - 1; var12 <= y + 4; ++var12)
						{
							if (var10 != x || var12 != y || var11 != z)
							{
								int var13 = 100;

								if (var12 > y + 1)
								{
									var13 += (var12 - (y + 1)) * 100;
								}

								int var14 = getChanceOfNeighborsEncouragingFire(pWorld, var10, var12, var11);

								if (var14 > 0)
								{
									int var15 = (var14 + 40 + pWorld->difficultySetting * 7) / (var7 + 30);

									if (var8)
									{
										var15 /= 2;
									}

									if (var15 > 0 && rand.nextInt(var13) <= var15 && (!pWorld->isRaining() || !pWorld->canLightningStrikeAt(var10, var12, var11)) && !pWorld->canLightningStrikeAt(var10 - 1, var12, z) && !pWorld->canLightningStrikeAt(var10 + 1, var12, var11) && !pWorld->canLightningStrikeAt(var10, var12, var11 - 1) && !pWorld->canLightningStrikeAt(var10, var12, var11 + 1))
									{
										int var16 = var7 + rand.nextInt(5) / 4;

										if (var16 > 15)
										{
											var16 = 15;
										}

										pWorld->setBlock(var10, var12, var11, m_blockID, var16, 3);
									}
								}
							}
						}
					}
				}
			}
		}
	}
	*/
}

bool BlockFire::canPlaceBlockAt(World* pWorld, const BlockPos& pos)
{
	return pWorld->doesBlockHaveSolidTopSurface(pos.getNegY()) || canNeighborBurn(pWorld, pos);
}

void BlockFire::onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborBlockID)
{
	if (!pWorld->doesBlockHaveSolidTopSurface(pos.getNegY()) && !canNeighborBurn(pWorld, pos))
	{
		pWorld->setBlockToAir(pos);
	}
}

void BlockFire::onBlockAdded(World* pWorld, const BlockPos& pos)
{
	if (pWorld->m_provider->dimensionId > 0 ||
		pWorld->getBlockId(pos.getNegY()) != BLOCK_ID_OBSIDIAN ||
		!((BlockPortal*)(BlockManager::portal))->tryToCreatePortal(pWorld, pos))
	{
		if (!pWorld->doesBlockHaveSolidTopSurface(pos.getNegY()) && !canNeighborBurn(pWorld, pos))
		{
			pWorld->setBlockToAir(pos);
		}
		else
		{
			pWorld->scheduleBlockUpdate(pos, m_blockID, tickRate(pWorld) + pWorld->m_Rand.nextInt(10));
		}
	}
}

int BlockFire::getChanceToEncourageFire(World* pWorld, const BlockPos& pos, int chance)
{
	int change2 = m_chanceToEncourageFire[pWorld->getBlockId(pos)];
	return change2 > chance ? change2 : chance;
}

bool BlockFire::canBlockCatchFire(IBlockAccess* pBlockAccess, const BlockPos& pos)
{
	return m_chanceToEncourageFire[pBlockAccess->getBlockId(pos)] > 0;
}

void BlockFire::tryToCatchBlockOnFire(World* pWorld, const BlockPos& pos, int chance, Random* pRand, int par7)
{
	int c = m_abilityToCatchFire[pWorld->getBlockId(pos)];

	if (pRand->nextInt(chance) < c)
	{
		bool isTnt = pWorld->getBlockId(pos) == BLOCK_ID_TNT;

		if (pRand->nextInt(par7 + 10) < 5 && !pWorld->canLightningStrikeAt(pos))
		{
			int var10 = par7 + pRand->nextInt(5) / 4;

			if (var10 > 15)
			{
				var10 = 15;
			}

			pWorld->setBlock(pos, m_blockID, var10, 3);
		}
		else
		{
			pWorld->setBlockToAir(pos);
		}

		if (isTnt)
		{
			BlockManager::tnt->onBlockDestroyedByPlayer(pWorld, pos, 1);
		}
	}
}

void BlockFire::setBurnRate(int blockID, int chanceToEncourageFire, int abilityToCatchFire)
{
	m_chanceToEncourageFire[blockID] = chanceToEncourageFire;
	m_abilityToCatchFire[blockID] = abilityToCatchFire;
}

bool BlockFire::canNeighborBurn(World* pWorld, const BlockPos& pos)
{
	return canBlockCatchFire(pWorld, pos.getPosX()) ? true : 
		  (canBlockCatchFire(pWorld, pos.getNegX()) ? true : 
		  (canBlockCatchFire(pWorld, pos.getNegY()) ? true : 
		  (canBlockCatchFire(pWorld, pos.getPosY()) ? true : 
		  (canBlockCatchFire(pWorld, pos.getNegZ()) ? true : 
		   canBlockCatchFire(pWorld, pos.getPosZ())))));
}

int BlockFire::getChanceOfNeighborsEncouragingFire(World* pWorld, const BlockPos& pos)
{
	if (!pWorld->isAirBlock(pos))
		return 0;
	
	int ret = getChanceToEncourageFire(pWorld, pos.getPosX(), 0);
	ret = getChanceToEncourageFire(pWorld, pos.getNegX(), ret);
	ret = getChanceToEncourageFire(pWorld, pos.getNegY(), ret);
	ret = getChanceToEncourageFire(pWorld, pos.getPosY(), ret);
	ret = getChanceToEncourageFire(pWorld, pos.getNegZ(), ret);
	ret = getChanceToEncourageFire(pWorld, pos.getPosZ(), ret);
	return ret;
}

BlockLever::BlockLever(int id, const BM_Material& mat, const String& name)
	: Block(id, mat, name, false)
{
}

void BlockLever::setBlockBoundsBasedOnState(IBlockAccess* pBlockAccess, const BlockPos& pos)
{
	int meta = pBlockAccess->getBlockMeta(pos) & 7;
	float bound = 0.1875F;

	if (meta == 1)
	{
		setBlockBounds(0.0F, 0.2F, 0.5F - bound, bound * 2.0F, 0.8F, 0.5F + bound);
	}
	else if (meta == 2)
	{
		setBlockBounds(1.0F - bound * 2.0F, 0.2F, 0.5F - bound, 1.0F, 0.8F, 0.5F + bound);
	}
	else if (meta == 3)
	{
		setBlockBounds(0.5F - bound, 0.2F, 0.0F, 0.5F + bound, 0.8F, bound * 2.0F);
	}
	else if (meta == 4)
	{
		setBlockBounds(0.5F - bound, 0.2F, 1.0F - bound * 2.0F, 0.5F + bound, 0.8F, 1.0F);
	}
	else if (meta != 5 && meta != 6)
	{
		if (meta == 0 || meta == 7)
		{
			bound = 0.25F;
			setBlockBounds(0.5F - bound, 0.4F, 0.5F - bound, 0.5F + bound, 1.0F, 0.5F + bound);
		}
	}
	else
	{
		bound = 0.25F;
		setBlockBounds(0.5F - bound, 0.0F, 0.5F - bound, 0.5F + bound, 0.6F, 0.5F + bound);
	}
}

bool BlockLever::canPlaceBlockOnSide(World* pWorld, const BlockPos& pos, int side)
{
	return side == 0 && pWorld->isBlockNormalCube(pos.getPosY()) ? true : 
		  (side == 1 && pWorld->doesBlockHaveSolidTopSurface(pos.getNegY()) ? true : 
		  (side == 2 && pWorld->isBlockNormalCube(pos.getPosZ()) ? true : 
		  (side == 3 && pWorld->isBlockNormalCube(pos.getNegZ()) ? true : 
		  (side == 4 && pWorld->isBlockNormalCube(pos.getPosX()) ? true : 
		   side == 5 && pWorld->isBlockNormalCube(pos.getNegX())))));
}

bool BlockLever::canPlaceBlockAt(World* pWorld, const BlockPos& pos)
{
	return pWorld->isBlockNormalCube(pos.getNegX()) ? true : 
		  (pWorld->isBlockNormalCube(pos.getPosX()) ? true : 
		  (pWorld->isBlockNormalCube(pos.getNegZ()) ? true : 
		  (pWorld->isBlockNormalCube(pos.getPosZ()) ? true : 
		  (pWorld->doesBlockHaveSolidTopSurface(pos.getNegY()) ? true :
		  pWorld->isBlockNormalCube(pos.getPosY())))));
}

int BlockLever::onBlockPlaced(World* pWorld, const BlockPos& pos, int side, const Vector3& hit, int meta)
{
	int var11 = meta & 8;
	int var10 = meta & 7;
	int var12 = -1;

	if (side == 0 && pWorld->isBlockNormalCube(pos.getPosY()))
	{
		var12 = 0;
	}

	if (side == 1 && pWorld->doesBlockHaveSolidTopSurface(pos.getNegY()))
	{
		var12 = 5;
	}

	if (side == 2 && pWorld->isBlockNormalCube(pos.getPosZ()))
	{
		var12 = 4;
	}

	if (side == 3 && pWorld->isBlockNormalCube(pos.getNegZ()))
	{
		var12 = 3;
	}

	if (side == 4 && pWorld->isBlockNormalCube(pos.getPosX()))
	{
		var12 = 2;
	}

	if (side == 5 && pWorld->isBlockNormalCube(pos.getNegX()))
	{
		var12 = 1;
	}

	return var12 + var11;
}

void BlockLever::onBlockPlacedBy(World* pWorld, const BlockPos& pos, EntityLivingBase* pLiving, ItemStackPtr pStack)
{
	int meta = pWorld->getBlockMeta(pos);
	int var8 = meta & 7;
	int var9 = meta & 8;

	if (var8 == invertMetadata(1))
	{
		if ((int(Math::Floor(pLiving->rotationYaw * 4.0F / 360.0F + 0.5f)) & 1) == 0)
		{
			pWorld->setBlockMetadataWithNotify(pos, 5 | var9, 2);
		}
		else
		{
			pWorld->setBlockMetadataWithNotify(pos, 6 | var9, 2);
		}
	}
	else if (var8 == invertMetadata(0))
	{
		if ((int(Math::Floor(pLiving->rotationYaw * 4.0F / 360.0F + 0.5f)) & 1) == 0)
		{
			pWorld->setBlockMetadataWithNotify(pos, 7 | var9, 2);
		}
		else
		{
			pWorld->setBlockMetadataWithNotify(pos, 0 | var9, 2);
		}
	}
}

void BlockLever::onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborBlockID)
{
	if (checkIfAttachedToBlock(pWorld, pos))
	{
		int meta = pWorld->getBlockMeta(pos) & 7;
		bool flag = false;

		if (!pWorld->isBlockNormalCube(pos.getNegX()) && meta == 1)
		{
			flag = true;
		}

		if (!pWorld->isBlockNormalCube(pos.getPosX()) && meta == 2)
		{
			flag = true;
		}

		if (!pWorld->isBlockNormalCube(pos.getNegZ()) && meta == 3)
		{
			flag = true;
		}

		if (!pWorld->isBlockNormalCube(pos.getPosZ()) && meta == 4)
		{
			flag = true;
		}

		if (!pWorld->doesBlockHaveSolidTopSurface(pos.getNegY()) && meta == 5)
		{
			flag = true;
		}

		if (!pWorld->doesBlockHaveSolidTopSurface(pos.getNegY()) && meta == 6)
		{
			flag = true;
		}

		if (!pWorld->isBlockNormalCube(pos.getPosY()) && meta == 0)
		{
			flag = true;
		}

		if (!pWorld->isBlockNormalCube(pos.getPosY()) && meta == 7)
		{
			flag = true;
		}

		if (flag)
		{
			dropBlockAsItem(pWorld, pos, pWorld->getBlockMeta(pos), 0);
			pWorld->setBlockToAir(pos);
		}
	}
}

bool BlockLever::onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit)
{
	if (pWorld->m_isClient)
		return true;

	pPlayer->setActivatedBlock(pos);
	int meta = pWorld->getBlockMeta(pos);
	int var11 = meta & 7;
	int var12 = 8 - (meta & 8);
	pWorld->setBlockMetadataWithNotify(pos, var11 + var12, 3);
	//pWorld->playSoundEffect(Vector3(pos) + 0.5f, "random.click", 0.3F, var12 > 0 ? 0.6F : 0.5F);
	pWorld->playSoundEffectByType(Vector3(pos) + 0.5f, ST_Click);
	pWorld->notifyBlocksOfNeighborChange(pos, m_blockID);

	if (meta >= 0 && meta <= 7)
	{
		SCRIPT_EVENT::BlockSwitchEvent::invoke(pPlayer->entityId, true, m_blockID, pos);
	}
	else
	{
		SCRIPT_EVENT::BlockSwitchEvent::invoke(pPlayer->entityId, false, m_blockID, pos);
	}

	if (var11 == 1)
	{
		pWorld->notifyBlocksOfNeighborChange(pos.getNegX(), m_blockID);
	}
	else if (var11 == 2)
	{
		pWorld->notifyBlocksOfNeighborChange(pos.getPosX(), m_blockID);
	}
	else if (var11 == 3)
	{
		pWorld->notifyBlocksOfNeighborChange(pos.getNegZ(), m_blockID);
	}
	else if (var11 == 4)
	{
		pWorld->notifyBlocksOfNeighborChange(pos.getPosZ(), m_blockID);
	}
	else if (var11 != 5 && var11 != 6)
	{
		if (var11 == 0 || var11 == 7)
		{
			pWorld->notifyBlocksOfNeighborChange(pos.getPosY(), m_blockID);
		}
	}
	else
	{
		pWorld->notifyBlocksOfNeighborChange(pos.getNegY(), m_blockID);
	}

	return true;
}

void BlockLever::breakBlock(World* pWorld, const BlockPos& pos, int blockID, int meta)
{
	if ((meta & 8) > 0)
	{
		pWorld->notifyBlocksOfNeighborChange(pos, m_blockID);
		int dir = meta & 7;

		if (dir == 1)
		{
			pWorld->notifyBlocksOfNeighborChange(pos.getNegX(), m_blockID);
		}
		else if (dir == 2)
		{
			pWorld->notifyBlocksOfNeighborChange(pos.getPosX(), m_blockID);
		}
		else if (dir == 3)
		{
			pWorld->notifyBlocksOfNeighborChange(pos.getNegZ(), m_blockID);
		}
		else if (dir == 4)
		{
			pWorld->notifyBlocksOfNeighborChange(pos.getPosZ(), m_blockID);
		}
		else if (dir != 5 && dir != 6)
		{
			if (dir == 0 || dir == 7)
			{
				pWorld->notifyBlocksOfNeighborChange(pos.getPosY(), m_blockID);
			}
		}
		else
		{
			pWorld->notifyBlocksOfNeighborChange(pos.getNegY(), m_blockID);
		}
	}

	Block::breakBlock(pWorld, pos, blockID, meta);
}

int BlockLever::isProvidingWeakPower(IBlockAccess* piBlockAccess, const BlockPos& pos, int side)
{
	return (piBlockAccess->getBlockMeta(pos) & 8) > 0 ? 15 : 0;
}

int BlockLever::isProvidingStrongPower(IBlockAccess* piBlockAccess, const BlockPos& pos, int side)
{
	int meta = piBlockAccess->getBlockMeta(pos);

	if ((meta & 8) == 0)
	{
		return 0;
	}

	int dir = meta & 7;
	return
		dir == 0 && side == 0 ? 15 :
		(dir == 7 && side == 0 ? 15 :
		(dir == 6 && side == 1 ? 15 :
		(dir == 5 && side == 1 ? 15 :
		(dir == 4 && side == 2 ? 15 :
		(dir == 3 && side == 3 ? 15 :
		(dir == 2 && side == 4 ? 15 :
		(dir == 1 && side == 5 ? 15 : 0)))))));
}

int BlockLever::invertMetadata(int meta)
{
	switch (meta)
	{
	case 0:
		return 0;

	case 1:
		return 5;

	case 2:
		return 4;

	case 3:
		return 3;

	case 4:
		return 2;

	case 5:
		return 1;

	default:
		return -1;
	}
}

bool BlockLever::checkIfAttachedToBlock(World* pWorld, const BlockPos& pos)
{
	if (!canPlaceBlockAt(pWorld, pos))
	{
		dropBlockAsItem(pWorld, pos, pWorld->getBlockMeta(pos), 0);
		pWorld->setBlockToAir(pos);
		return false;
	}
	else
	{
		return true;
	}
}

BlockNetherStalk::BlockNetherStalk(int id, const BM_Material& mat, const String& name)
	: BlockFlower(id, mat, name)
{
	setTickRandomly(true);
	float bound = 0.5F;
	setBlockBounds(0.5F - bound, 0.0F, 0.5F - bound, 0.5F + bound, 0.25F, 0.5F + bound);
}

bool BlockNetherStalk::canBlockStay(World* pWorld, const BlockPos& pos)
{
	return canThisPlantGrowOnThisBlockID(pWorld->getBlockId(pos.getNegY()));
}

void BlockNetherStalk::updateTick(World* pWorld, const BlockPos& pos, Random& rand)
{
	int meta = pWorld->getBlockMeta(pos);

	if (meta < 3 && rand.nextInt(10) == 0)
	{
		++meta;
		pWorld->setBlockMetadataWithNotify(pos, meta, 2);
	}

	BlockFlower::updateTick(pWorld, pos, rand);
}

void BlockNetherStalk::dropBlockAsItemWithChance(World* pWorld, const BlockPos& pos, int meta, float chance, int fortune)
{
	if (pWorld->m_isClient)
		return;
	
	int var8 = 1;

	if (meta >= 3)
	{
		var8 = 2 + pWorld->m_Rand.nextInt(3);

		if (fortune > 0)
		{
			var8 += pWorld->m_Rand.nextInt(fortune + 1);
		}
	}

	for (int i = 0; i < var8; ++i)
	{
		dropBlockAsItem_do(pWorld, pos, LORD::make_shared<ItemStack>(Item::netherStalkSeeds));
	}
}


BlockBrewingStand::BlockBrewingStand(int id, const BM_Material& mat, const String& name)
	: BlockContainer(id, mat, name, false)
{
	m_isOpaqueCube = false;
}

void BlockBrewingStand::setBlockBoundsForItemRender()
{
	setBlockBounds(0.f, 0.f, 0.f, 1.0F, 0.125F, 1.0F);
}

void BlockBrewingStand::addCollisionBoxesToList(World* pWorld, const BlockPos& pos, const Box& aabb, AABBList& lst, Entity* pEntity)
{
	setBlockBounds(0.4375F, 0.0F, 0.4375F, 0.5625F, 0.875F, 0.5625F);
	Block::addCollisionBoxesToList(pWorld, pos, aabb, lst, pEntity);
	setBlockBoundsForItemRender();
	Block::addCollisionBoxesToList(pWorld, pos, aabb, lst, pEntity);
}

TileEntity* BlockBrewingStand::createNewTileEntity(World* pWorld)
{
	return LordNew TileEntityBrewingStand();
}

bool BlockBrewingStand::onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit)
{
	if (pWorld->m_isClient)
		return true;

	pPlayer->setActivatedBlock(pos);
	// todo.
	/*TileEntityBrewingStand var10 = (TileEntityBrewingStand)par1World.getBlockTileEntity(par2, par3, par4);

	if (var10 != null)
	{
		par5EntityPlayer.displayGUIBrewingStand(var10);
	}*/

	return true;
}

void BlockBrewingStand::onBlockPlacedBy(World* pWorld, const BlockPos& pos, EntityLivingBase* pLiving, ItemStackPtr pStack)
{
	if (pStack->hasDisplayName())
	{
		// todo.
		// ((TileEntityBrewingStand)par1World.getBlockTileEntity(par2, par3, par4)).func_94131_a(par6ItemStack.getDisplayName());
	}
}

void BlockBrewingStand::breakBlock(World* pWorld, const BlockPos& pos, int blockID, int meta)
{
	TileEntity* pTileEntity = pWorld->getBlockTileEntity(pos);

	// todo.
	/*
	if (pTileEntity instanceof TileEntityBrewingStand)
	{
		TileEntityBrewingStand var8 = (TileEntityBrewingStand)pTileEntity;

		for (int var9 = 0; var9 < var8.getSizeInventory(); ++var9)
		{
			ItemStack var10 = var8.getStackInSlot(var9);

			if (var10 != null)
			{
				float var11 = rand.nextFloat() * 0.8F + 0.1F;
				float var12 = rand.nextFloat() * 0.8F + 0.1F;
				float var13 = rand.nextFloat() * 0.8F + 0.1F;

				while (var10.stackSize > 0)
				{
					int var14 = rand.nextInt(21) + 10;

					if (var14 > var10.stackSize)
					{
						var14 = var10.stackSize;
					}

					var10.stackSize -= var14;
					EntityItem var15 = new EntityItem(par1World, (double)((float)par2 + var11), (double)((float)par3 + var12), (double)((float)par4 + var13), LORD::make_shared<ItemStack>(var10.itemID, var14, var10.getItemDamage()));
					float var16 = 0.05F;
					var15.motionX = (double)((float)rand.nextGaussian() * var16);
					var15.motionY = (double)((float)rand.nextGaussian() * var16 + 0.2F);
					var15.motionZ = (double)((float)rand.nextGaussian() * var16);
					par1World.spawnEntityInWorld(var15);
				}
			}
		}
	}
	*/

	Block::breakBlock(pWorld, pos, blockID, meta);
}

int BlockBrewingStand::getComparatorInputOverride(World* pWorld, const BlockPos& pos, int side)
{
	return Container::calcRedstoneFromInventory((IInventory*)pWorld->getBlockTileEntity(pos));
}

BlockCauldron::BlockCauldron(int id, const BM_Material& mat, const String& name)
	: Block(id, mat, name, false)
{
}

void BlockCauldron::addCollisionBoxesToList(World* pWorld, const BlockPos& pos, const Box& aabb, AABBList& lst, Entity* pEntity)
{
	setBlockBounds(0.0F, 0.0F, 0.0F, 1.0F, 0.3125F, 1.0F);
	Block::addCollisionBoxesToList(pWorld, pos, aabb, lst, pEntity);
	float var8 = 0.125F;
	setBlockBounds(0.0F, 0.0F, 0.0F, var8, 1.0F, 1.0F);
	Block::addCollisionBoxesToList(pWorld, pos, aabb, lst, pEntity);
	setBlockBounds(0.0F, 0.0F, 0.0F, 1.0F, 1.0F, var8);
	Block::addCollisionBoxesToList(pWorld, pos, aabb, lst, pEntity);
	setBlockBounds(1.0F - var8, 0.0F, 0.0F, 1.0F, 1.0F, 1.0F);
	Block::addCollisionBoxesToList(pWorld, pos, aabb, lst, pEntity);
	setBlockBounds(0.0F, 0.0F, 1.0F - var8, 1.0F, 1.0F, 1.0F);
	Block::addCollisionBoxesToList(pWorld, pos, aabb, lst, pEntity);
	setBlockBoundsForItemRender();
}

bool BlockCauldron::onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit)
{
	if (pWorld->m_isClient)
		return true;

	pPlayer->setActivatedBlock(pos);
	ItemStackPtr pStack = pPlayer->inventory->getCurrentItem();

	if (!pStack)
		return true;
	
	int meta = pWorld->getBlockMeta(pos);

	if (pStack->itemID == Item::bucketWater->itemID)
	{
		if (meta < 3)
		{
			if (!pPlayer->capabilities.isCreativeMode)
			{
				pPlayer->inventory->setInventorySlotContents(pPlayer->inventory->getInventoryIndexOfCurrentItem(), LORD::make_shared<ItemStack>(Item::bucketEmpty));
			}

			pWorld->setBlockMetadataWithNotify(pos, 3, 2);
			pWorld->updateComparatorOutputLevel(pos, m_blockID);
		}

		return true;
	}
	else
	{
		if (pStack->itemID == Item::glassBottle->itemID)
		{
			if (meta > 0)
			{
				ItemStackPtr pPotion = LORD::make_shared<ItemStack>(Item::potion, 1, 0);

				if (!SCRIPT_EVENT::EntityItemSpawnEvent::invoke(pPotion->itemID, pPotion->itemDamage, "potion.drop"))
					return true;

				if (!pPlayer->inventory->addItemStackToInventory(pPotion))
				{
					Vector3 spawnPos = Vector3(pos) + Vector3(0.5f, 1.5f, 0.5f);
					pWorld->spawnEntityInWorld(LordNew EntityItem(pWorld, spawnPos, pPotion));
				}
				// todo.
				/*else if (par5EntityPlayer instanceof EntityPlayerMP)
				{
					((EntityPlayerMP)par5EntityPlayer).sendContainerToPlayer(par5EntityPlayer.inventoryContainer);
				}*/

				--pStack->stackSize;

				if (pStack->stackSize <= 0)
				{
					pPlayer->inventory->setInventorySlotContents(pPlayer->inventory->getInventoryIndexOfCurrentItem(), NULL);
				}

				pWorld->setBlockMetadataWithNotify(pos, meta - 1, 2);
				pWorld->updateComparatorOutputLevel(pos, m_blockID);
			}
		}
		else if (meta > 0 && 
			dynamic_cast<ItemArmor*>(pStack->getItem()) &&
			((ItemArmor*)pStack->getItem())->getArmorMaterial() == ArmorMaterial::CLOTH)
		{
			ItemArmor* pArmor = (ItemArmor*)pStack->getItem();
			pArmor->removeColor(pStack);
			pWorld->setBlockMetadataWithNotify(pos, meta - 1, 2);
			pWorld->updateComparatorOutputLevel(pos, m_blockID);
			return true;
		}

		return true;
	}
}

void BlockCauldron::fillWithRain(World* pWorld, const BlockPos& pos)
{
	if (pWorld->m_Rand.nextInt(20) == 1)
	{
		int meta = pWorld->getBlockMeta(pos);

		if (meta < 3)
		{
			pWorld->setBlockMetadataWithNotify(pos, meta + 1, 2);
		}
	}
}

int BlockCauldron::getComparatorInputOverride(World* pWorld, const BlockPos& pos, int side)
{
	int meta = pWorld->getBlockMeta(pos);
	return meta;
}

void BlockCauldron::setBlockBoundsForItemRender()
{
	setBlockBounds(0.0F, 0.0F, 0.0F, 1.0F, 1.0F, 1.0F);
}

BlockDragonEgg::BlockDragonEgg(int id, const BM_Material& mat, const String& name)
	: Block(id, mat, name, false)
{
	setBlockBounds(0.0625F, 0.0F, 0.0625F, 0.9375F, 1.0F, 0.9375F);
}

void BlockDragonEgg::onBlockAdded(World* pWorld, const BlockPos& pos)
{
	pWorld->scheduleBlockUpdate(pos, m_blockID, tickRate(pWorld));
}

void BlockDragonEgg::onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborBlockID)
{
	pWorld->scheduleBlockUpdate(pos, m_blockID, tickRate(pWorld));
}

void BlockDragonEgg::updateTick(World* pWorld, const BlockPos& pos, Random& rand)
{
	fallIfPossible(pWorld, pos);
}

bool BlockDragonEgg::onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit)
{
	teleportNearby(pWorld, pos);
	pPlayer->setActivatedBlock(pos);
	return true;
}

void BlockDragonEgg::onBlockClicked(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer)
{
	teleportNearby(pWorld, pos);
}

void BlockDragonEgg::fallIfPossible(World* pWorld, const BlockPos& pos)
{
	if (BlockSand::canFallBelow(pWorld, pos.getNegY()) && pos.y >= 0)
	{
		int range = 32;

		if (!BlockSand::fallInstantly && pWorld->checkChunksExist(pos - range, pos + range))
		{
			// todo.
			/*
			EntityFallingSand var6 = new EntityFallingSand(par1World, (double)((float)par2 + 0.5F), (double)((float)par3 + 0.5F), (double)((float)par4 + 0.5F), blockID);
			pWorld->spawnEntityInWorld(var6);
			*/
		}
		else
		{
			pWorld->setBlockToAir(pos);
			BlockPos pos1 = pos;

			while (BlockSand::canFallBelow(pWorld, pos1.getNegY()) && pos1.y > 0)
			{
				--pos1.y;
			}

			if (pos1.y > 0)
			{
				pWorld->setBlock(pos1, m_blockID, 0, 2);
			}
		}
	}
}

void BlockDragonEgg::teleportNearby(World* pWorld, const BlockPos& pos)
{
	if (pWorld->getBlockId(pos) != m_blockID)
		return;
	
	for (int i = 0; i < 1000; ++i)
	{
		BlockPos pos1 = pos;
		pos1.x += pWorld->m_Rand.nextInt(16) - pWorld->m_Rand.nextInt(16);
		pos1.y += pWorld->m_Rand.nextInt(8) - pWorld->m_Rand.nextInt(8);
		pos1.z += pWorld->m_Rand.nextInt(16) - pWorld->m_Rand.nextInt(16);

		if (pWorld->getBlockId(pos1) != 0)
			continue;
		
		if (!pWorld->m_isClient)
		{
			pWorld->setBlock(pos1, m_blockID, pWorld->getBlockMeta(pos), 2);
			pWorld->setBlockToAir(pos);
		}
		else
		{
			for (int j = 0; j < 128; ++j)
			{
				float var11 = float(pWorld->m_Rand.nextDouble());
				Vector3 vel = pWorld->m_Rand.nextVector() - 0.5f * 0.2f;
				Vector3 lol = Vector3(pos1) + Vector3(pos - pos1) * var11 + pWorld->m_Rand.nextVector() - Vector3(0.5f, 0.f, 0.5f) + 0.5f;
				pWorld->spawnParticle("portal", lol, vel);
			}
		}

		return;
	}
}

BlockTripWire::BlockTripWire(int id, const BM_Material& mat, const String& name)
	: Block(id, mat, name, false)
{
	setBlockBounds(0.0F, 0.0F, 0.0F, 1.0F, 0.15625F, 1.0F);
	setTickRandomly(true);
}

void BlockTripWire::setBlockBoundsBasedOnState(IBlockAccess* pBlockAccess, const BlockPos& pos)
{
	int meta = pBlockAccess->getBlockMeta(pos);
	bool var6 = (meta & 4) == 4;
	bool var7 = (meta & 2) == 2;

	if (!var7)
	{
		setBlockBounds(0.0F, 0.0F, 0.0F, 1.0F, 0.09375F, 1.0F);
	}
	else if (!var6)
	{
		setBlockBounds(0.0F, 0.0F, 0.0F, 1.0F, 0.5F, 1.0F);
	}
	else
	{
		setBlockBounds(0.0F, 0.0625F, 0.0F, 1.0F, 0.15625F, 1.0F);
	}
}

void BlockTripWire::onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborBlockID)
{
	int meta = pWorld->getBlockMeta(pos);
	bool var7 = (meta & 2) == 2;
	bool var8 = !pWorld->doesBlockHaveSolidTopSurface(pos.getNegY());

	if (var7 != var8)
	{
		dropBlockAsItem(pWorld, pos, meta, 0);
		pWorld->setBlockToAir(pos);
	}
}

void BlockTripWire::onBlockAdded(World* pWorld, const BlockPos& pos)
{
	int var5 = pWorld->doesBlockHaveSolidTopSurface(pos.getNegY()) ? 0 : 2;
	pWorld->setBlockMetadataWithNotify(pos, var5, 3);
	notifyHook(pWorld, pos, var5);
}

void BlockTripWire::breakBlock(World* pWorld, const BlockPos& pos, int blockID, int meta)
{
	notifyHook(pWorld, pos, meta | 1);
}

void BlockTripWire::notifyHook(World* pWorld, const BlockPos& pos, int par5)
{
	int dir = 0;

	while (dir < 2)
	{
		int var7 = 1;

		while (true)
		{
			if (var7 < 42)
			{
				int x = pos.x + dir_offsetX[dir] * var7;
				int z = pos.z + dir_offsetZ[dir] * var7;
				BlockPos pos1(x, pos.y, z);
				int blockID = pWorld->getBlockId(pos1);

				if (blockID == BLOCK_ID_TRIP_WIRE_SOURCE)
				{
					int srcdir = pWorld->getBlockMeta(pos1) & 3;

					if (srcdir == dir_rotateOpposite[dir])
					{
						BlockManager::tripWireSource->calculateState(pWorld, pos1, blockID, pWorld->getBlockMeta(pos1), true, var7, par5);
					}
				}
				else if (blockID == BLOCK_ID_TRIP_WIRE)
				{
					++var7;
					continue;
				}
			}

			++dir;
			break;
		}
	}
}

void BlockTripWire::onBlockHarvested(World* pWorld, const BlockPos& pos, int meta, EntityPlayer* pPlayer)
{
	if (pWorld->m_isClient)
		return;
	
	if (pPlayer->getCurrentEquippedItem() &&
		pPlayer->getCurrentEquippedItem()->itemID == Item::shears->itemID)
	{
		pWorld->setBlockMetadataWithNotify(pos, meta | 8, 4);
	}
}

void BlockTripWire::onEntityCollidedWithBlock(World* pWorld, const BlockPos& pos, Entity* pEntity)
{
	if (pWorld->m_isClient)
		return;
	
	if ((pWorld->getBlockMeta(pos) & 1) != 1)
	{
		updateTripWireState(pWorld, pos);
	}
}

void BlockTripWire::updateTick(World* pWorld, const BlockPos& pos, Random& rand)
{
	if (!pWorld->m_isClient)
	{
		if ((pWorld->getBlockMeta(pos) & 1) == 1)
		{
			updateTripWireState(pWorld, pos);
		}
	}
}

bool BlockTripWire::isConnectedTo(IBlockAccess* pBlockAccess, const BlockPos& pos, int meta, int side)
{
	BlockPos pos1 = pos + dir_offset[side];
	int blockID = pBlockAccess->getBlockId(pos1);
	bool var10 = (meta & 2) == 2;
	int meta2;

	if (blockID == BLOCK_ID_TRIP_WIRE_SOURCE)
	{
		meta2 = pBlockAccess->getBlockMeta(pos1);
		int var13 = meta2 & 3;
		return var13 == dir_rotateOpposite[side];
	}
	else if (blockID == BLOCK_ID_TRIP_WIRE)
	{
		meta2 = pBlockAccess->getBlockMeta(pos1);
		bool var12 = (meta2 & 2) == 2;
		return var10 == var12;
	}
	else
	{
		return false;
	}
}

void BlockTripWire::updateTripWireState(World* pWorld, const BlockPos& pos)
{
	/*
	int var5 = pWorld->getBlockMetadata(x, y, z);
	bool var6 = (var5 & 1) == 1;
	bool var7 = false;
	List var8 = pWorld->getEntitiesWithinAABBExcludingEntity((Entity)null, AxisAlignedBB.getAABBPool().getAABB((double)par2 + minX, (double)par3 + minY, (double)par4 + minZ, (double)par2 + maxX, (double)par3 + maxY, (double)par4 + maxZ));

	if (!var8.isEmpty())
	{
		Iterator var9 = var8.iterator();

		while (var9.hasNext())
		{
			Entity var10 = (Entity)var9.next();

			if (!var10.doesEntityNotTriggerPressurePlate())
			{
				var7 = true;
				break;
			}
		}
	}

	if (var7 && !var6)
	{
		var5 |= 1;
	}

	if (!var7 && var6)
	{
		var5 &= -2;
	}

	if (var7 != var6)
	{
		pWorld->setBlockMetadataWithNotify(x, y, z, var5, 3);
		func_72149_e(pWorld, x, y, z, var5);
	}

	if (var7)
	{
		pWorld->scheduleBlockUpdate(x, y, z, blockID, tickRate(pWorld));
	}
	*/
}

BlockTripWireSource::BlockTripWireSource(int id, const BM_Material& mat, const String& name)
	: Block(id, mat, name, false)
{
	setTickRandomly(true);
}

void BlockTripWireSource::setBlockBoundsBasedOnState(IBlockAccess* pBlockAccess, const BlockPos& pos)
{
	int meta = pBlockAccess->getBlockMeta(pos) & 3;
	float bound = 0.1875F;

	if (meta == 3)
	{
		setBlockBounds(0.0F, 0.2F, 0.5F - bound, bound * 2.0F, 0.8F, 0.5F + bound);
	}
	else if (meta == 1)
	{
		setBlockBounds(1.0F - bound * 2.0F, 0.2F, 0.5F - bound, 1.0F, 0.8F, 0.5F + bound);
	}
	else if (meta == 0)
	{
		setBlockBounds(0.5F - bound, 0.2F, 0.0F, 0.5F + bound, 0.8F, bound * 2.0F);
	}
	else if (meta == 2)
	{
		setBlockBounds(0.5F - bound, 0.2F, 1.0F - bound * 2.0F, 0.5F + bound, 0.8F, 1.0F);
	}
}

bool BlockTripWireSource::canPlaceBlockOnSide(World* pWorld, const BlockPos& pos, int side)
{
	return side == 2 && pWorld->isBlockNormalCube(pos.getPosZ()) ? true : 
		  (side == 3 && pWorld->isBlockNormalCube(pos.getNegZ()) ? true : 
		  (side == 4 && pWorld->isBlockNormalCube(pos.getPosX()) ? true : 
		  side == 5 && pWorld->isBlockNormalCube(pos.getNegX())));
}

bool BlockTripWireSource::canPlaceBlockAt(World* pWorld, const BlockPos& pos)
{
	return pWorld->isBlockNormalCube(pos.getNegX()) ? true : 
		  (pWorld->isBlockNormalCube(pos.getPosX()) ? true : 
		  (pWorld->isBlockNormalCube(pos.getNegZ()) ? true :
		  pWorld->isBlockNormalCube(pos.getPosZ())));
}

int BlockTripWireSource::onBlockPlaced(World* pWorld, const BlockPos& pos, int side, const Vector3& hit, int meta)
{
	int ret = 0;

	if (side == 2 && pWorld->isBlockNormalCubeDefault(pos.getPosZ(), true))
	{
		ret = 2;
	}

	if (side == 3 && pWorld->isBlockNormalCubeDefault(pos.getNegZ(), true))
	{
		ret = 0;
	}

	if (side == 4 && pWorld->isBlockNormalCubeDefault(pos.getPosX(), true))
	{
		ret = 1;
	}

	if (side == 5 && pWorld->isBlockNormalCubeDefault(pos.getNegX(), true))
	{
		ret = 3;
	}

	return ret;
}

void BlockTripWireSource::onPostBlockPlaced(World* pWorld, const BlockPos& pos, int side)
{
	calculateState(pWorld, pos, m_blockID, side, false, -1, 0);
}

void BlockTripWireSource::onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborBlockID)
{
	if (neighborBlockID != m_blockID)
	{
		if (checkForDrop(pWorld, pos))
		{
			int meta = pWorld->getBlockMeta(pos);
			int meta2 = meta & 3;
			bool flag = false;

			if (!pWorld->isBlockNormalCube(pos.getNegX()) && meta2 == 3)
			{
				flag = true;
			}

			if (!pWorld->isBlockNormalCube(pos.getPosX()) && meta2 == 1)
			{
				flag = true;
			}

			if (!pWorld->isBlockNormalCube(pos.getNegZ()) && meta2 == 0)
			{
				flag = true;
			}

			if (!pWorld->isBlockNormalCube(pos.getPosZ()) && meta2 == 2)
			{
				flag = true;
			}

			if (flag)
			{
				dropBlockAsItem(pWorld, pos, meta, 0);
				pWorld->setBlockToAir(pos);
			}
		}
	}
}

void BlockTripWireSource::updateTick(World* pWorld, const BlockPos& pos, Random& rand)
{
	calculateState(pWorld, pos, m_blockID, pWorld->getBlockMeta(pos), true, -1, 0);
}

void BlockTripWireSource::breakBlock(World* pWorld, const BlockPos& pos, int blockID, int meta)
{
	bool var7 = (meta & 4) == 4;
	bool var8 = (meta & 8) == 8;

	if (var7 || var8)
	{
		//calculateState(pWorld, x, y, z, 0, progress, false, -1, 0);
	}

	if (var8)
	{
		pWorld->notifyBlocksOfNeighborChange(pos, m_blockID);
		int var9 = meta & 3;

		if (var9 == 3)
		{
			pWorld->notifyBlocksOfNeighborChange(pos.getNegX(), m_blockID);
		}
		else if (var9 == 1)
		{
			pWorld->notifyBlocksOfNeighborChange(pos.getPosX(), m_blockID);
		}
		else if (var9 == 0)
		{
			pWorld->notifyBlocksOfNeighborChange(pos.getNegZ(), m_blockID);
		}
		else if (var9 == 2)
		{
			pWorld->notifyBlocksOfNeighborChange(pos.getPosZ(), m_blockID);
		}
	}

	Block::breakBlock(pWorld, pos, blockID, meta);
}

int BlockTripWireSource::isProvidingWeakPower(IBlockAccess* piBlockAccess, const BlockPos& pos, int side)
{
	return (piBlockAccess->getBlockMeta(pos) & 8) == 8 ? 15 : 0;
}

int BlockTripWireSource::isProvidingStrongPower(IBlockAccess* piBlockAccess, const BlockPos& pos, int side)
{
	int meta = piBlockAccess->getBlockMeta(pos);

	if ((meta & 8) != 8)
	{
		return 0;
	}
	else
	{
		int dir = meta & 3;
		return dir == 2 && side == 2 ? 15 :
			(dir == 0 && side == 3 ? 15 :
			(dir == 1 && side == 4 ? 15 :
			(dir == 3 && side == 5 ? 15 : 0)));
	}
}

void BlockTripWireSource::calculateState(World* pWorld, const BlockPos& pos, int blockID, int meta, bool par7, int par8, int par9)
{
	int dir = meta & 3;
	bool var11 = (meta & 4) == 4;
	bool var12 = (meta & 8) == 8;
	bool isSource = blockID == BLOCK_ID_TRIP_WIRE_SOURCE;
	bool var14 = false;
	bool var15 = !pWorld->doesBlockHaveSolidTopSurface(pos.getNegY());
	int x = dir_offsetX[dir];
	int z = dir_offsetZ[dir];
	int extLen = 0;
	int var19[42];
	
	for (int i = 1; i < 42; ++i)
	{
		int ix = pos.x + x * i;
		int iz = pos.z + z * i;
		BlockPos pos1(ix, pos.y, iz);
		int offsetID = pWorld->getBlockId(pos1);
		int offsetMeta = 0;

		if (offsetID == BLOCK_ID_TRIP_WIRE_SOURCE)
		{
			offsetMeta = pWorld->getBlockMeta(pos1);

			if ((offsetMeta & 3) == dir_rotateOpposite[dir])
			{
				extLen = i;
			}

			break;
		}

		if (offsetID != BLOCK_ID_TRIP_WIRE && i != par8)
		{
			var19[i] = -1;
			isSource = false;
		}
		else
		{
			offsetMeta = i == par8 ? par9 : pWorld->getBlockMeta(pos1);
			bool var25 = (offsetMeta & 8) != 8;
			bool var26 = (offsetMeta & 1) == 1;
			bool var27 = (offsetMeta & 2) == 2;
			isSource &= var27 == var15;
			var14 |= var25 && var26;
			var19[i] = offsetMeta;

			if (i == par8)
			{
				pWorld->scheduleBlockUpdate(pos, blockID, tickRate(pWorld));
				isSource &= var25;
			}
		}
	}

	isSource &= extLen > 1;
	var14 &= isSource;
	int im = (isSource ? 4 : 0) | (var14 ? 8 : 0);
	meta = dir | im;

	if (extLen > 0)
	{
		int ix = pos.x + x * extLen;
		int iz = pos.z + z * extLen;
		int opposite = dir_rotateOpposite[dir];
		BlockPos pos1(ix, pos.y, iz);
		pWorld->setBlockMetadataWithNotify(pos1, opposite | im, 3);
		notifyNeighborOfChange(pWorld, pos1, opposite);
		playSoundEffect(pWorld, pos1, isSource, var14, var11, var12);
	}

	playSoundEffect(pWorld, pos, isSource, var14, var11, var12);

	if (blockID > 0)
	{
		pWorld->setBlockMetadataWithNotify(pos, meta, 3);

		if (par7)
		{
			notifyNeighborOfChange(pWorld, pos, dir);
		}
	}

	if (var11 != isSource)
	{
		for (int i = 1; i < extLen; ++i)
		{
			int ix = pos.x + x * i;
			int iz = pos.z + z * i;
			BlockPos pos1(ix, pos.y, iz);
			int savedmeta = var19[ix];

			if (savedmeta >= 0)
			{
				if (isSource)
				{
					savedmeta |= 4;
				}
				else
				{
					savedmeta &= -5;
				}

				pWorld->setBlockMetadataWithNotify(pos1, savedmeta, 3);
			}
		}
	}
}

void BlockTripWireSource::notifyNeighborOfChange(World* pWorld, const BlockPos& pos, int side)
{
	pWorld->notifyBlocksOfNeighborChange(pos, m_blockID);

	if (side == 3)
	{
		pWorld->notifyBlocksOfNeighborChange(pos.getNegX(), m_blockID);
	}
	else if (side == 1)
	{
		pWorld->notifyBlocksOfNeighborChange(pos.getPosX(), m_blockID);
	}
	else if (side == 0)
	{
		pWorld->notifyBlocksOfNeighborChange(pos.getNegZ(), m_blockID);
	}
	else if (side == 2)
	{
		pWorld->notifyBlocksOfNeighborChange(pos.getPosZ(), m_blockID);
	}
}

bool BlockTripWireSource::checkForDrop(World* pWorld, const BlockPos& pos)
{
	if (!canPlaceBlockAt(pWorld, pos))
	{
		dropBlockAsItem(pWorld, pos, pWorld->getBlockMeta(pos), 0);
		pWorld->setBlockToAir(pos);
		return false;
	}
	return true;
}

BlockHopper::BlockHopper(int id, const BM_Material& mat, const String& name)
	: BlockContainer(id, mat, name, false)
{
	m_isOpaqueCube = false;
	setBlockBounds(0.0F, 0.0F, 0.0F, 1.0F, 1.0F, 1.0F);
}

TileEntity* BlockHopper::createNewTileEntity(World* pWorld)
{
	return LordNew TileEntityHopper();
}

void BlockHopper::addCollisionBoxesToList(World* pWorld, const BlockPos& pos, const Box& aabb, AABBList& lst, Entity* pEntity)
{
	float bound = 0.125F;
	setBlockBounds(0.0F, 0.0F, 0.0F, 1.0F, 0.625F, 1.0F);
	Block::addCollisionBoxesToList(pWorld, pos, aabb, lst, pEntity);
	setBlockBounds(0.0F, 0.0F, 0.0F, bound, 1.0F, 1.0F);
	Block::addCollisionBoxesToList(pWorld, pos, aabb, lst, pEntity);
	setBlockBounds(0.0F, 0.0F, 0.0F, 1.0F, 1.0F, bound);
	Block::addCollisionBoxesToList(pWorld, pos, aabb, lst, pEntity);
	setBlockBounds(1.0F - bound, 0.0F, 0.0F, 1.0F, 1.0F, 1.0F);
	Block::addCollisionBoxesToList(pWorld, pos, aabb, lst, pEntity);
	setBlockBounds(0.0F, 0.0F, 1.0F - bound, 1.0F, 1.0F, 1.0F);
	Block::addCollisionBoxesToList(pWorld, pos, aabb, lst, pEntity);
	setBlockBounds(0.0F, 0.0F, 0.0F, 1.0F, 1.0F, 1.0F);
}

int BlockHopper::onBlockPlaced(World* pWorld, const BlockPos& pos, int side, const Vector3& hit, int meta)
{
	int opposite = oppositeSide[side];

	if (opposite == 1)
	{
		opposite = 0;
	}

	return opposite;
}

void BlockHopper::onBlockPlacedBy(World* pWorld, const BlockPos& pos, EntityLivingBase* pLiving, ItemStackPtr pStack)
{
	Block::onBlockPlacedBy(pWorld, pos, pLiving, pStack);

	if (pStack->hasDisplayName())
	{
		// todo.
		/* TileEntityHopper var7 = getHopperTile(par1World, par2, par3, par4);
		var7.setInventoryName(par6ItemStack.getDisplayName());*/
	}
}

void BlockHopper::onBlockAdded(World* pWorld, const BlockPos& pos)
{
	BlockContainer::onBlockAdded(pWorld, pos);
	//updateMetadata(pWorld, x, y, z);
}

bool BlockHopper::onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit)
{
	if (pWorld->m_isClient)
		return true;

	pPlayer->setActivatedBlock(pos);
	// todo.
	/*TileEntityHopper var10 = getHopperTile(par1World, par2, par3, par4);
	if (var10 != null)
	{
		par5EntityPlayer.displayGUIHopper(var10);
	}*/

	return true;
}

void BlockHopper::onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborBlockID)
{
	updateMetadata(pWorld, pos);
}

void BlockHopper::breakBlock(World* pWorld, const BlockPos& pos, int blockID, int meta)
{
	// todo.
	/*TileEntityHopper var7 = (TileEntityHopper)par1World.getBlockTileEntity(par2, par3, par4);

	if (var7 != null)
	{
	for (int var8 = 0; var8 < var7.getSizeInventory(); ++var8)
	{
	ItemStack var9 = var7.getStackInSlot(var8);

	if (var9 != null)
	{
	float var10 = field_94457_a.nextFloat() * 0.8F + 0.1F;
	float var11 = field_94457_a.nextFloat() * 0.8F + 0.1F;
	float var12 = field_94457_a.nextFloat() * 0.8F + 0.1F;

	while (var9.stackSize > 0)
	{
	int var13 = field_94457_a.nextInt(21) + 10;

	if (var13 > var9.stackSize)
	{
	var13 = var9.stackSize;
	}

	var9.stackSize -= var13;
	EntityItem var14 = new EntityItem(par1World, (double)((float)par2 + var10), (double)((float)par3 + var11), (double)((float)par4 + var12), LORD::make_shared<ItemStack>(var9.itemID, var13, var9.getItemDamage()));

	if (var9.hasTagCompound())
	{
	var14.getEntityItem().setTagCompound((NBTTagCompound)var9.getTagCompound().copy());
	}

	float var15 = 0.05F;
	var14.motionX = (double)((float)field_94457_a.nextGaussian() * var15);
	var14.motionY = (double)((float)field_94457_a.nextGaussian() * var15 + 0.2F);
	var14.motionZ = (double)((float)field_94457_a.nextGaussian() * var15);
	par1World.spawnEntityInWorld(var14);
	}
	}
	}

	par1World.func_96440_m(par2, par3, par4, par5);
	}*/

	Block::breakBlock(pWorld, pos, blockID, meta);
}

int BlockHopper::getComparatorInputOverride(World* pWorld, const BlockPos& pos, int side)
{
	// todo.
	// return Container::calcRedstoneFromInventory(getHopperTile(par1World, par2, par3, par4));
	return 0;
}

void BlockHopper::updateMetadata(World* pWorld, const BlockPos& pos)
{
	int meta = pWorld->getBlockMeta(pos);
	int dir = getDirectionFromMetadata(meta);
	bool bPower = !pWorld->isBlockIndirectlyGettingPowered(pos);
	bool bPower2 = getIsBlockNotPoweredFromMetadata(meta);

	if (bPower != bPower2)
	{
		pWorld->setBlockMetadataWithNotify(pos, dir | (bPower ? 0 : 8), 4);
	}
}

BlockDropper::BlockDropper(int id, const BM_Material& mat, const String& name)
	: BlockDispenser(id, mat, name)
{}

BlockBasePressurePlate::BlockBasePressurePlate(int id, const BM_Material& mat, const String& name)
	: Block(id, mat, name)
{
	m_isOpaqueCube = false;
	setTickRandomly(true);
	SetBlockBoundByPower(true);	
	setLightOpacity(0);
}

void BlockBasePressurePlate::setBlockBoundsBasedOnState(IBlockAccess* pBlockAccess, const BlockPos& pos)
{
	int meta = pBlockAccess->getBlockMeta(pos);
	bool isPowered = getPowerSupply(meta) > 0;
	SetBlockBoundByPower(isPowered);
}

void BlockBasePressurePlate::setBlockBoundsForItemRender()
{
	float boundx = 0.5F;
	float boundy = 0.125F;
	float boundz = 0.5F;
	setBlockBounds(0.5F - boundx, 0, 0.5F - boundz, 0.5F + boundx, boundy, 0.5F + boundz);
}

bool BlockBasePressurePlate::canPlaceBlockAt(World* pWorld, const BlockPos& pos)
{
	return pWorld->doesBlockHaveSolidTopSurface(pos.getNegY()) ||
		BlockFence::isIdAFence(pWorld->getBlockId(pos.getNegY()));
}

void BlockBasePressurePlate::onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborBlockID)
{
	bool var6 = false;

	if (!pWorld->doesBlockHaveSolidTopSurface(pos.getNegY()) &&
		!BlockFence::isIdAFence(pWorld->getBlockId(pos.getNegY())))
	{
		var6 = true;
	}

	if (var6)
	{
		dropBlockAsItem(pWorld, pos, pWorld->getBlockMeta(pos), 0);
		pWorld->setBlockToAir(pos);
	}
}

void BlockBasePressurePlate::updateTick(World* pWorld, const BlockPos& pos, Random& rand)
{
	if (!pWorld->m_isClient)
	{
		int power = getPowerSupply(pWorld->getBlockMeta(pos));

		if (power > 0)
		{
			setStateIfMobInteractsWithPlate(pWorld, pos, power);
		}
	}
}

void BlockBasePressurePlate::onEntityCollidedWithBlock(World* pWorld, const BlockPos& pos, Entity* pEntity)
{
	if (pWorld->m_isClient)
		return;
	
	int power = getPowerSupply(pWorld->getBlockMeta(pos));

	if (power == 0 && pEntity)
	{
		setStateIfMobInteractsWithPlate(pWorld, pos, power, pEntity->entityId);
	}
}

void BlockBasePressurePlate::setStateIfMobInteractsWithPlate(World* pWorld, const BlockPos& pos, int power, int entityId)
{
	int state = getPlateState(pWorld, pos);
	bool powered = power > 0;
	bool stated = state > 0;

	if (power != state)
	{
		pWorld->setBlockMetadataWithNotify(pos, getMetaFromWeight(state), 2);

		if (SCRIPT_EVENT::BlockSwitchEvent::invoke(entityId, power == 0, m_blockID, pos))
		{
			updateNeighbors(pWorld, pos);
		}
		SCRIPT_EVENT::BlockPressurePlateWeightedEvent::invoke(entityId, state, m_blockID, pos);
		pWorld->markBlockRangeForRenderUpdate(pos, pos);
	}

	if (!stated && powered)
	{
		//pWorld->playSoundEffect({pos.x + 0.5f, pos.y + 0.1f, pos.z + 0.5f}, "random.click", 0.3F, 0.5F);
		pWorld->playSoundEffectByType({ pos.x + 0.5f, pos.y + 0.1f, pos.z + 0.5f }, ST_Click);
	}
	else if (stated && !powered)
	{
		//pWorld->playSoundEffect({ pos.x + 0.5f, pos.y + 0.1f, pos.z + 0.5f }, "random.click", 0.3F, 0.6F);
		pWorld->playSoundEffectByType({ pos.x + 0.5f, pos.y + 0.1f, pos.z + 0.5f }, ST_Click);
	}

	if (stated)
	{
		pWorld->scheduleBlockUpdate(pos, m_blockID, tickRate(pWorld));
	}
}

Box BlockBasePressurePlate::getSensitiveAABB(const BlockPos& pos)
{
	float f = 0.125F;
	return Box(pos.x + f, (float)pos.y, pos.z + f, pos.x + 1.f - f, pos.y + 0.25f, pos.z + 1.f - f);
}

void BlockBasePressurePlate::breakBlock(World* pWorld, const BlockPos& pos, int blockID, int meta)
{
	if (getPowerSupply(meta) > 0)
	{
		updateNeighbors(pWorld, pos);
	}

	Block::breakBlock(pWorld, pos, blockID, meta);
}

void BlockBasePressurePlate::updateNeighbors(World* pWorld, const BlockPos& pos)
{
	pWorld->notifyBlocksOfNeighborChange(pos, m_blockID);
	pWorld->notifyBlocksOfNeighborChange(pos.getNegY(), m_blockID);
}

int BlockBasePressurePlate::isProvidingWeakPower(IBlockAccess* piBlockAccess, const BlockPos& pos, int side)
{
	return getPowerSupply(piBlockAccess->getBlockMeta(pos));
}

int BlockBasePressurePlate::isProvidingStrongPower(IBlockAccess* piBlockAccess, const BlockPos& pos, int side)
{
	return side == 1 ? getPowerSupply(piBlockAccess->getBlockMeta(pos)) : 0;
}

void BlockBasePressurePlate::SetBlockBoundByPower(bool isPowered)
{	
	float bound = 0.0625F;

	if (isPowered)
	{
		setBlockBounds(bound, 0.1F, bound, 1.0F - bound, 0.0425F, 1.0F - bound);
	}
	else
	{
		setBlockBounds(bound, 0.01F, bound, 1.0F - bound, 0.0625F, 1.0F - bound);
	}
}

void setStateIfMobInteractsWithPlate(World* pWorld, const BlockPos& pos, int par5)
{

}

BlockPressurePlate::BlockPressurePlate(int id, const BM_Material& mat, const String& name, MOB_TYPE mobType)
	: BlockBasePressurePlate(id, mat, name)
	, m_triggerMobType(mobType)
{}

int BlockPressurePlate::getPlateState(World* pWorld, const BlockPos& pos)
{
	EntityArr entities;

	if (m_triggerMobType == MOB_TYPE_EVERYTHING)
	{
		entities = pWorld->getEntitiesWithinAABBExcludingEntity(NULL, getSensitiveAABB(pos));
	}

	if (m_triggerMobType == MOB_TYPE_MOBS)
	{
		entities = pWorld->getEntitiesWithinAABB(ENTITY_CLASS_LIVING_BASE, getSensitiveAABB(pos));
	}

	if (m_triggerMobType == MOB_TYPE_PLAYERS)
	{
		entities = pWorld->getEntitiesWithinAABB(ENTITY_CLASS_PLAYER, getSensitiveAABB(pos));
	}

	if (!entities.empty())
	{
		for (auto it : entities)
		{
			Entity* pEntity = it;
			if (!pEntity->doesEntityNotTriggerPressurePlate())
				return 15;
		}
	}

	return 0;
}

BlockPressurePlateWeighted::BlockPressurePlateWeighted(int id, const BM_Material& mat, const String& name, int maxItemsWeighted, MOB_TYPE mobType)
	: BlockBasePressurePlate(id, mat, name)
	, m_maxItemsWeighted(maxItemsWeighted)
	, m_triggerMobType(mobType)
{}

int BlockPressurePlateWeighted::getPlateState(World* pWorld, const BlockPos& pos)
{
	int size = 0;
	EntityArr entities;
	
	if (m_triggerMobType == MOB_TYPE_ITEM)
	{
		entities = pWorld->getEntitiesWithinAABB(ENTITY_CLASS_ITEM, getSensitiveAABB(pos));

		for (auto it : entities)
		{
			EntityItem* pItem = (EntityItem*)it;
			size += pItem->getEntityItem()->stackSize;

			if (size >= m_maxItemsWeighted)
				break;
		}

		if (size <= 0)
			return 0;

		float ratio = (float)Math::Min(m_maxItemsWeighted, size) / (float)m_maxItemsWeighted;
		return int(Math::Ceil(ratio * 15.0F));
	}

	if (m_triggerMobType == MOB_TYPE_PLAYERS)
	{
		entities = pWorld->getEntitiesWithinAABB(ENTITY_CLASS_PLAYER, getSensitiveAABB(pos));

		for (auto it : entities)
		{
			Entity* pEntity = it;
			if (!pEntity->doesEntityNotTriggerPressurePlate())
				size += 1;

			if (size >= m_maxItemsWeighted)
				break;
		}

		return size;
	}
	
	return size;
}

BlockSoulSand::BlockSoulSand(int id, const BM_Material& mat, const String& name)
	: Block(id, mat, name)
{
}

Box BlockSoulSand::getCollisionBoundingBox(World* pWorld, const BlockPos& pos)
{	
	return Box(float(pos.x), float(pos.y), float(pos.z), float(pos.x + 1), pos.y + 1 - 0.125F, float(pos.z + 1));
}

void BlockSoulSand::onEntityCollidedWithBlock(World* pWorld, const BlockPos& pos, Entity* pEntity)
{
	pEntity->motion.x *= 0.4F;
	pEntity->motion.z *= 0.4F;
}

BlockGlowStone::BlockGlowStone(int id, const BM_Material& mat, const String& name)
	: Block(id, mat, name)
{
}

int BlockGlowStone::quantityDroppedWithBonus(int bonus, Random& rand)
{
	return Math::Clamp(quantityDropped(rand) + rand.nextInt(bonus + 1), 1, 4);
}

int BlockGlowStone::quantityDropped(Random& rand)
{
	return 2 + rand.nextInt(3);
}

BlockEndPortalFrame::BlockEndPortalFrame(int id, const BM_Material& mat, const String& name)
	: Block(id, mat, name, false)
{}

void BlockEndPortalFrame::addCollisionBoxesToList(World* pWorld, const BlockPos& pos, const Box& aabb, AABBList& lst, Entity* pEntity)
{
	setBlockBounds(0.0F, 0.0F, 0.0F, 1.0F, 0.8125F, 1.0F);
	Block::addCollisionBoxesToList(pWorld, pos, aabb, lst, pEntity);
	int var8 = pWorld->getBlockMeta(pos);

	if (isEnderEyeInserted(var8))
	{
		setBlockBounds(0.3125F, 0.8125F, 0.3125F, 0.6875F, 1.0F, 0.6875F);
		Block::addCollisionBoxesToList(pWorld, pos, aabb, lst, pEntity);
	}

	setBlockBoundsForItemRender();
}

void BlockEndPortalFrame::onBlockPlacedBy(World* pWorld, const BlockPos& pos, EntityLivingBase* pEntityLivingBase, ItemStackPtr pItemStack)
{	
	int dir = (((int)Math::Floor((pEntityLivingBase->rotationYaw * 4.0F / 360.0F) + 0.5F) & 3) + 2) % 4;
	pWorld->setBlockMetadataWithNotify(pos, dir, 2);
}

int BlockEndPortalFrame::getComparatorInputOverride(World* pWorld, const BlockPos& pos, int side)
{
	int meta = pWorld->getBlockMeta(pos);
	return isEnderEyeInserted(meta) ? 15 : 0;
}

BlockSign::BlockSign(int id, const BM_Material& mat, const String& name, int signClass, bool freestanding)
: BlockContainer(id, mat, name, false)
, signClassID(signClass)
, isFreestanding(freestanding)
{
	float bound = 0.25F;
	float height = 1.0F;
	setBlockBounds(0.5F - bound, 0.0F, 0.5F - bound, 0.5F + bound, height, 0.5F + bound);
}

void BlockSign::setBlockBoundsBasedOnState(IBlockAccess* pBlockAccess, const BlockPos& pos)
{
	if (isFreestanding)
		return;
	
	int meta = pBlockAccess->getBlockMeta(pos);
	float fy = 0.28125F;
	float fy2 = 0.78125F;
	float fx = 0.0F;
	float fx2 = 1.0F;
	float bound = 0.125F;
	setBlockBounds(0.0F, 0.0F, 0.0F, 1.0F, 1.0F, 1.0F);

	if (meta == 2)
		setBlockBounds(fx, fy, 1.0F - bound, fx2, fy2, 1.0F);

	if (meta == 3)
		setBlockBounds(fx, fy, 0.0F, fx2, fy2, bound);

	if (meta == 4)
		setBlockBounds(1.0F - bound, fy, fx, 1.0F, fy2, fx2);

	if (meta == 5)
		setBlockBounds(0.0F, fy, fx, bound, fy2, fx2);
}

TileEntity* BlockSign::createNewTileEntity(World* pWorld)
{
	return LordNew TileEntitySign();
}

int BlockSign::idDropped(int meta, Random& rand, int fortune)
{
	return Item::sign->itemID;
}

void BlockSign::onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborID)
{
	bool isDrop = false;

	if (isFreestanding)
	{
		if (!pWorld->getBlockMaterial(pos.getNegY()).isSolid())
		{
			isDrop = true;
		}
	}
	else
	{
		int meta = pWorld->getBlockMeta(pos);
		isDrop = true;

		if (meta == 2 && pWorld->getBlockMaterial(pos.getPosZ()).isSolid())
		{
			isDrop = false;
		}

		if (meta == 3 && pWorld->getBlockMaterial(pos.getNegZ()).isSolid())
		{
			isDrop = false;
		}

		if (meta == 4 && pWorld->getBlockMaterial(pos.getPosX()).isSolid())
		{
			isDrop = false;
		}

		if (meta == 5 && pWorld->getBlockMaterial(pos.getNegX()).isSolid())
		{
			isDrop = false;
		}
	}

	if (isDrop)
	{
		dropBlockAsItem(pWorld, pos, pWorld->getBlockMeta(pos), 0);
		pWorld->setBlockToAir(pos);
	}

	Block::onNeighborBlockChange(pWorld, pos, neighborID);
}

bool BlockSign::onBlockActivated(World * pWorld, const BlockPos & pos, EntityPlayer * pPlayer, int face, const Vector3 & hit)
{
	pPlayer->setActivatedBlock(pos);
	if (!pWorld->m_isClient)
	{
		SCRIPT_EVENT::OnClickBlockSignEvent::invoke(pPlayer->hashCode(), pos);
	}

	return true;
}

int BlockSign::idPicked(World* pWorld, const BlockPos& pos)
{
	return Item::sign->itemID;
}

BlockRedstoneWire::BlockRedstoneWire(int id, const BM_Material& mat, const String& name)
	: Block(id, mat, name, false)
	, wiresProvidePower(true)
{}

bool BlockRedstoneWire::canPlaceBlockAt(World* pWorld, const BlockPos& pos)
{
	return
		pWorld->doesBlockHaveSolidTopSurface(pos.getNegY()) ||
		pWorld->getBlockId(pos.getNegY()) == BLOCK_ID_GLOW_STONE;
}

void BlockRedstoneWire::updateAndPropagateCurrentStrength(World* pWorld, const BlockPos& pos)
{
	calculateCurrentChanges(pWorld, pos, pos);
	BlockPosArr temp(blocksNeedingUpdate);
	blocksNeedingUpdate.clear();

	for (auto it : temp)
	{
		const BlockPos& pos = it;
		pWorld->notifyBlocksOfNeighborChange(pos, m_blockID);
	}
}

void BlockRedstoneWire::calculateCurrentChanges(World* pWorld, const BlockPos& pos1, const BlockPos& pos2)
{
	int meta1 = pWorld->getBlockMeta(pos1);
	int var9 = 0;
	int strength2 = getMaxCurrentStrength(pWorld, pos2, var9);
	wiresProvidePower = false;
	int strength1 = pWorld->getStrongestIndirectPower(pos1);
	wiresProvidePower = true;

	if (strength1 > 0 && strength1 > strength2 - 1)
	{
		strength2 = strength1;
	}

	int strength = 0;

	for (int dir = 0; dir < 4; ++dir)
	{
		BlockPos pos = pos1;
		switch (dir)
		{
		case 0: --pos.x; break;
		case 1: ++pos.x; break;
		case 2: --pos.z; break;
		case 3: ++pos.z; break;
		default: break;
		}
		
		if (pos.x != pos2.x || pos.z != pos2.z)
		{
			strength = getMaxCurrentStrength(pWorld, pos, strength);
		}

		if (pWorld->isBlockNormalCube(pos) && !pWorld->isBlockNormalCube(pos1.getPosY()))
		{
			if ((pos.x != pos2.x || pos.z != pos2.z) && pos1.y >= pos2.y)
			{
				strength = getMaxCurrentStrength(pWorld, pos.getPosY(), strength);
			}
		}
		else if (!pWorld->isBlockNormalCube(pos) && (pos.x != pos2.x || pos.z != pos2.z) && pos1.y <= pos2.y)
		{
			strength = getMaxCurrentStrength(pWorld, pos.getNegY(), strength);
		}
	}

	if (strength > strength2)
	{
		strength2 = strength - 1;
	}
	else if (strength2 > 0)
	{
		--strength2;
	}
	else
	{
		strength2 = 0;
	}

	if (strength1 > strength2 - 1)
	{
		strength2 = strength1;
	}

	if (meta1 != strength2)
	{
		pWorld->setBlockMetadataWithNotify(pos1, strength2, 2);
		blocksNeedingUpdate.push_back(pos1);
		blocksNeedingUpdate.push_back(pos1.getNegX());
		blocksNeedingUpdate.push_back(pos1.getPosX());
		blocksNeedingUpdate.push_back(pos1.getNegY());
		blocksNeedingUpdate.push_back(pos1.getPosY());
		blocksNeedingUpdate.push_back(pos1.getNegZ());
		blocksNeedingUpdate.push_back(pos1.getPosZ());
	}
}

void BlockRedstoneWire::notifyWireNeighborsOfNeighborChange(World* world, const BlockPos& pos)
{
	if (world->getBlockId(pos) == m_blockID)
	{
		world->notifyBlocksOfNeighborChange(pos, m_blockID);
		world->notifyBlocksOfNeighborChange(pos.getNegX(), m_blockID);
		world->notifyBlocksOfNeighborChange(pos.getPosX(), m_blockID);
		world->notifyBlocksOfNeighborChange(pos.getNegZ(), m_blockID);
		world->notifyBlocksOfNeighborChange(pos.getPosZ(), m_blockID);
		world->notifyBlocksOfNeighborChange(pos.getNegY(), m_blockID);
		world->notifyBlocksOfNeighborChange(pos.getPosY(), m_blockID);
	}
}

void BlockRedstoneWire::onBlockAdded(World* pWorld, const BlockPos& pos)
{
	Block::onBlockAdded(pWorld, pos);

	if (!pWorld->m_isClient)
	{
		updateAndPropagateCurrentStrength(pWorld, pos);
		pWorld->notifyBlocksOfNeighborChange(pos.getPosY(), m_blockID);
		pWorld->notifyBlocksOfNeighborChange(pos.getNegY(), m_blockID);
		notifyWireNeighborsOfNeighborChange(pWorld, pos.getNegX());
		notifyWireNeighborsOfNeighborChange(pWorld, pos.getPosX());
		notifyWireNeighborsOfNeighborChange(pWorld, pos.getNegZ());
		notifyWireNeighborsOfNeighborChange(pWorld, pos.getPosZ());

		if (pWorld->isBlockNormalCube(pos.getNegX()))
		{
			notifyWireNeighborsOfNeighborChange(pWorld, pos.getUW());
		}
		else
		{
			notifyWireNeighborsOfNeighborChange(pWorld, pos.getDW());
		}

		if (pWorld->isBlockNormalCube(pos.getPosX()))
		{
			notifyWireNeighborsOfNeighborChange(pWorld, pos.getUE());
		}
		else
		{
			notifyWireNeighborsOfNeighborChange(pWorld, pos.getDE());
		}

		if (pWorld->isBlockNormalCube(pos.getNegZ()))
		{
			notifyWireNeighborsOfNeighborChange(pWorld, pos.getUN());
		}
		else
		{
			notifyWireNeighborsOfNeighborChange(pWorld, pos.getDN());
		}

		if (pWorld->isBlockNormalCube(pos.getPosZ()))
		{
			notifyWireNeighborsOfNeighborChange(pWorld, pos.getUS());
		}
		else
		{
			notifyWireNeighborsOfNeighborChange(pWorld, pos.getDS());
		}
	}
}

void BlockRedstoneWire::breakBlock(World* pWorld, const BlockPos& pos, int blockID, int meta)
{
	Block::breakBlock(pWorld, pos, blockID, meta);

	if (!pWorld->m_isClient)
	{
		pWorld->notifyBlocksOfNeighborChange(pos.getPosY(), blockID);
		pWorld->notifyBlocksOfNeighborChange(pos.getNegY(), blockID);
		pWorld->notifyBlocksOfNeighborChange(pos.getPosX(), blockID);
		pWorld->notifyBlocksOfNeighborChange(pos.getNegX(), blockID);
		pWorld->notifyBlocksOfNeighborChange(pos.getPosZ(), blockID);
		pWorld->notifyBlocksOfNeighborChange(pos.getNegZ(), blockID);
		updateAndPropagateCurrentStrength(pWorld, pos);
		notifyWireNeighborsOfNeighborChange(pWorld, pos.getNegX());
		notifyWireNeighborsOfNeighborChange(pWorld, pos.getPosX());
		notifyWireNeighborsOfNeighborChange(pWorld, pos.getNegZ());
		notifyWireNeighborsOfNeighborChange(pWorld, pos.getPosZ());

		if (pWorld->isBlockNormalCube(pos.getNegX()))
		{
			notifyWireNeighborsOfNeighborChange(pWorld, pos.getUW());
		}
		else
		{
			notifyWireNeighborsOfNeighborChange(pWorld, pos.getDW());
		}

		if (pWorld->isBlockNormalCube(pos.getPosX()))
		{
			notifyWireNeighborsOfNeighborChange(pWorld, pos.getUE());
		}
		else
		{
			notifyWireNeighborsOfNeighborChange(pWorld, pos.getDE());
		}

		if (pWorld->isBlockNormalCube(pos.getNegZ()))
		{
			notifyWireNeighborsOfNeighborChange(pWorld, pos.getUN());
		}
		else
		{
			notifyWireNeighborsOfNeighborChange(pWorld, pos.getDN());
		}

		if (pWorld->isBlockNormalCube(pos.getPosZ()))
		{
			notifyWireNeighborsOfNeighborChange(pWorld, pos.getUS());
		}
		else
		{
			notifyWireNeighborsOfNeighborChange(pWorld, pos.getDS());
		}
	}
}

void BlockRedstoneWire::onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborID)
{
	if (!pWorld->m_isClient)
	{
		bool place = canPlaceBlockAt(pWorld, pos);

		if (place)
		{
			updateAndPropagateCurrentStrength(pWorld, pos);
		}
		else
		{
			dropBlockAsItem(pWorld, pos, 0, 0);
			pWorld->setBlockToAir(pos);
		}

		Block::onNeighborBlockChange(pWorld, pos, neighborID);
	}
}

int BlockRedstoneWire::idDropped(int meta, Random& rand, int fortune)
{
	return Item::redstone->itemID;
}

int BlockRedstoneWire::isProvidingStrongPower(IBlockAccess* pBlockAccess, const BlockPos& pos, int side)
{
	return !wiresProvidePower ? 0 : isProvidingWeakPower(pBlockAccess, pos, side);
}

int BlockRedstoneWire::isProvidingWeakPower(IBlockAccess* pBlockAccess, const BlockPos& pos, int side)
{
	if (!wiresProvidePower)
	{
		return 0;
	}
	else
	{
		int strength = pBlockAccess->getBlockMeta(pos);

		if (strength == 0)
		{
			return 0;
		}
		else if (side == 1)
		{
			return strength;
		}
		else
		{
			bool pw = isPoweredOrRepeater(pBlockAccess, pos.getNegX(), 1) || !pBlockAccess->isBlockNormalCube(pos.getNegX()) && isPoweredOrRepeater(pBlockAccess, pos.getDW(), -1);
			bool pe = isPoweredOrRepeater(pBlockAccess, pos.getPosX(), 3) || !pBlockAccess->isBlockNormalCube(pos.getPosX()) && isPoweredOrRepeater(pBlockAccess, pos.getDE(), -1);
			bool pn = isPoweredOrRepeater(pBlockAccess, pos.getNegZ(), 2) || !pBlockAccess->isBlockNormalCube(pos.getNegZ()) && isPoweredOrRepeater(pBlockAccess, pos.getDN(), -1);
			bool ps = isPoweredOrRepeater(pBlockAccess, pos.getPosZ(), 0) || !pBlockAccess->isBlockNormalCube(pos.getPosZ()) && isPoweredOrRepeater(pBlockAccess, pos.getDS(), -1);

			if (!pBlockAccess->isBlockNormalCube(pos.getPosY()))
			{
				if (pBlockAccess->isBlockNormalCube(pos.getNegX()) && isPoweredOrRepeater(pBlockAccess, pos.getUW(), -1))
				{
					pw = true;
				}

				if (pBlockAccess->isBlockNormalCube(pos.getPosX()) && isPoweredOrRepeater(pBlockAccess, pos.getUE(), -1))
				{
					pe = true;
				}

				if (pBlockAccess->isBlockNormalCube(pos.getNegZ()) && isPoweredOrRepeater(pBlockAccess, pos.getUN(), -1))
				{
					pn = true;
				}

				if (pBlockAccess->isBlockNormalCube(pos.getPosZ()) && isPoweredOrRepeater(pBlockAccess, pos.getUS(), -1))
				{
					ps = true;
				}
			}

			return !pn && !pe && !pw && !ps && side >= 2 && side <= 5 ? strength : (side == 2 && pn && !pw && !pe ? strength : (side == 3 && ps && !pw && !pe ? strength : (side == 4 && pw && !pn && !ps ? strength : (side == 5 && pe && !pn && !ps ? strength : 0))));
		}
	}
}

bool BlockRedstoneWire::isPowerProviderOrWire(IBlockAccess* pBlockAccess, const BlockPos& pos, int dir)
{
	int id = pBlockAccess->getBlockId(pos);

	if (id == BLOCK_ID_REDSTONE_WIRE)
	{
		return true;
	}
	else if (id == 0)
	{
		return false;
	}
	else if (!BlockManager::redstoneRepeaterIdle->isSameDiode(id))
	{
		return BlockManager::sBlocks[id]->canProvidePower() && dir != -1;
	}
	int meta = pBlockAccess->getBlockMeta(pos);
	return dir == (meta & 3) || dir == dir_rotateOpposite[meta & 3];
}

bool BlockRedstoneWire::isPoweredOrRepeater(IBlockAccess* pBlockAccess, const BlockPos& pos, int dir)
{
	if (isPowerProviderOrWire(pBlockAccess, pos, dir))
		return true;
	
	int id = pBlockAccess->getBlockId(pos);

	if (id == BLOCK_ID_REDSTONE_REPEATER_ACTIVE)
	{
		int meta = pBlockAccess->getBlockMeta(pos);
		return dir == (meta & 3);
	}

	return false;
}

int BlockRedstoneWire::idPicked(World* pWorld, const BlockPos& pos)
{
	return Item::redstone->itemID;
}

int BlockRedstoneWire::getMaxCurrentStrength(World* pWorld, const BlockPos& pos, int minStrength)
{
	if (pWorld->getBlockId(pos) != m_blockID)
	{
		return minStrength;
	}
	else
	{
		int strength = pWorld->getBlockMeta(pos);
		return strength > minStrength ? strength : minStrength;
	}
}

BlockRedstoneLogic::BlockRedstoneLogic(int id, const BM_Material& mat, const String& name, bool powered)
: BlockDirectional(id, mat, name, false)
, isRepeaterPowered(powered)
{
	// super(par1, Material.circuits);
	setBlockBounds(0.0F, 0.0F, 0.0F, 1.0F, 0.125F, 1.0F);
}

bool BlockRedstoneLogic::canPlaceBlockAt(World* pWorld, const BlockPos& pos)
{
	return !pWorld->doesBlockHaveSolidTopSurface(pos.getNegY()) ? false : Block::canPlaceBlockAt(pWorld, pos);
}

bool BlockRedstoneLogic::canBlockStay(World* pWorld, const BlockPos& pos)
{
	return !pWorld->doesBlockHaveSolidTopSurface(pos.getNegY()) ? false : Block::canBlockStay(pWorld, pos);
}

void BlockRedstoneLogic::updateTick(World* pWorld, const BlockPos& pos, Random& rand)
{
	int meta = pWorld->getBlockMeta(pos);

	if (!isLocked(pWorld, pos, meta))
	{
		bool var7 = isGettingInput(pWorld, pos, meta);

		if (isRepeaterPowered && !var7)
		{
			pWorld->setBlock(pos, getUperState(), meta, 2);
		}
		else if (!isRepeaterPowered)
		{
			pWorld->setBlock(pos, getPoweredState(), meta, 2);

			if (!var7)
			{
				pWorld->scheduleBlockUpdateWithPriority(pos, getPoweredState(), getTickDelay(meta), -1);
			}
		}
	}
}

int BlockRedstoneLogic::isProvidingStrongPower(IBlockAccess* pBlockAccess, const BlockPos& pos, int side)
{
	return isProvidingWeakPower(pBlockAccess, pos, side);
}

int BlockRedstoneLogic::isProvidingWeakPower(IBlockAccess* pBlockAccess, const BlockPos& pos, int side)
{
	int meta = pBlockAccess->getBlockMeta(pos);

	if (!isPowered(meta))
	{
		return 0;
	}
	else
	{
		int var7 = getDirection(meta);
		return
			var7 == 0 && side == 3 ? getActiveSignal(pBlockAccess, pos, meta) :
			(var7 == 1 && side == 4 ? getActiveSignal(pBlockAccess, pos, meta) :
			(var7 == 2 && side == 2 ? getActiveSignal(pBlockAccess, pos, meta) :
			(var7 == 3 && side == 5 ? getActiveSignal(pBlockAccess, pos, meta) : 0)));
	}
}

void BlockRedstoneLogic::onNeighborBlockChange(World* pWorld, const BlockPos& pos, int neighborID)
{
	if (!canBlockStay(pWorld, pos))
	{
		dropBlockAsItem(pWorld, pos, pWorld->getBlockMeta(pos), 0);
		pWorld->setBlockToAir(pos);
		pWorld->notifyBlocksOfNeighborChange(pos.getPosX(), m_blockID);
		pWorld->notifyBlocksOfNeighborChange(pos.getNegX(), m_blockID);
		pWorld->notifyBlocksOfNeighborChange(pos.getPosZ(), m_blockID);
		pWorld->notifyBlocksOfNeighborChange(pos.getNegZ(), m_blockID);
		pWorld->notifyBlocksOfNeighborChange(pos.getPosY(), m_blockID);
		pWorld->notifyBlocksOfNeighborChange(pos.getNegY(), m_blockID);
	}
	else
	{
		updateState(pWorld, pos, neighborID);
	}
}

void BlockRedstoneLogic::updateState(World* pWorld, const BlockPos& pos, int blockID)
{
	int meta = pWorld->getBlockMeta(pos);

	if (!isLocked(pWorld, pos, meta))
	{
		bool hasInput = isGettingInput(pWorld, pos, meta);

		if ((isRepeaterPowered && !hasInput || !isRepeaterPowered && hasInput) && !pWorld->isBlockTickScheduledThisTick(pos, blockID))
		{
			int Priority = -1;

			if (isFacingTowardsRepeater(pWorld, pos, meta))
			{
				Priority = -3;
			}
			else if (isRepeaterPowered)
			{
				Priority = -2;
			}

			pWorld->scheduleBlockUpdateWithPriority(pos, blockID, getDelay(meta), Priority);
		}
	}
}

bool BlockRedstoneLogic::isGettingInput(World* pWorld, const BlockPos& pos, int meta)
{
	return getInputStrength(pWorld, pos, meta) > 0;
}

int BlockRedstoneLogic::getInputStrength(World* pWorld, const BlockPos& pos, int meta)
{
	int dir = getDirection(meta);
	BlockPos pos1 = pos + dir_offset[dir];
	int power = pWorld->getIndirectPowerLevelTo(pos, dir_directionToFacing[dir]);
	return power >= 15 ? power :
		Math::Max(power, pWorld->getBlockId(pos1) == BLOCK_ID_REDSTONE_WIRE ? pWorld->getBlockMeta(pos1) : 0);
}

int BlockRedstoneLogic::calculateInputStrength(IBlockAccess* pBlockAccess, const BlockPos& pos, int meta)
{
	int dir = getDirection(meta);

	switch (dir)
	{
	case 0:
	case 2:
		return Math::Max(getInputStrength(pBlockAccess, pos.getNegX(), 4), getInputStrength(pBlockAccess, pos.getPosX(), 5));

	case 1:
	case 3:
		return Math::Max(getInputStrength(pBlockAccess, pos.getPosZ(), 3), getInputStrength(pBlockAccess, pos.getNegZ(), 2));

	default:
		return 0;
	}
}

int BlockRedstoneLogic::getInputStrength(IBlockAccess* pBlockAccess, const BlockPos& pos, int meta)
{
	int blockID = pBlockAccess->getBlockId(pos);
	return isBlockProvidePower(blockID) ? (blockID == BLOCK_ID_REDSTONE_WIRE ? pBlockAccess->getBlockMeta(pos) : pBlockAccess->isBlockProvidingPowerTo(pos, meta)) : 0;
}

void BlockRedstoneLogic::onBlockPlacedBy(World* pWorld, const BlockPos& pos, EntityLivingBase* pLiving, ItemStackPtr pStack)
{
	int dir = ((int(Math::Floor((pLiving->rotationYaw * 4.0F / 360.0F) + 0.5f)) & 3) + 2) % 4;
	pWorld->setBlockMetadataWithNotify(pos, dir, 3);
	bool hasInput = isGettingInput(pWorld, pos, dir);

	if (hasInput)
	{
		pWorld->scheduleBlockUpdate(pos, m_blockID, 1);
	}
}

void BlockRedstoneLogic::notifyNeighbors(World* pWorld, const BlockPos& pos)
{
	int dir = getDirection(pWorld->getBlockMeta(pos));

	if (dir == 1)
	{
		pWorld->notifyBlockOfNeighborChange(pos.getPosX(), m_blockID);
		pWorld->notifyBlocksOfNeighborChange(pos.getPosX(), m_blockID, 4);
	}

	if (dir == 3)
	{
		pWorld->notifyBlockOfNeighborChange(pos.getNegX(), m_blockID);
		pWorld->notifyBlocksOfNeighborChange(pos.getNegX(), m_blockID, 5);
	}

	if (dir == 2)
	{
		pWorld->notifyBlockOfNeighborChange(pos.getPosZ(), m_blockID);
		pWorld->notifyBlocksOfNeighborChange(pos.getPosZ(), m_blockID, 2);
	}

	if (dir == 0)
	{
		pWorld->notifyBlockOfNeighborChange(pos.getNegZ(), m_blockID);
		pWorld->notifyBlocksOfNeighborChange(pos.getNegZ(), m_blockID, 3);
	}
}

void BlockRedstoneLogic::onBlockDestroyedByPlayer(World* pWorld, const BlockPos& pos, int meta)
{
	if (isRepeaterPowered)
	{
		pWorld->notifyBlocksOfNeighborChange(pos.getPosX(), m_blockID);
		pWorld->notifyBlocksOfNeighborChange(pos.getNegX(), m_blockID);
		pWorld->notifyBlocksOfNeighborChange(pos.getPosZ(), m_blockID);
		pWorld->notifyBlocksOfNeighborChange(pos.getNegZ(), m_blockID);
		pWorld->notifyBlocksOfNeighborChange(pos.getPosY(), m_blockID);
		pWorld->notifyBlocksOfNeighborChange(pos.getNegY(), m_blockID);
	}

	Block::onBlockDestroyedByPlayer(pWorld, pos, meta);
}

bool BlockRedstoneLogic::isBlockProvidePower(int blockID)
{
	Block* pBlock = BlockManager::sBlocks[blockID];
	return pBlock != NULL && pBlock->canProvidePower();
}

bool BlockRedstoneLogic::isRedstoneRepeaterBlockID(int blockID)
{
	return BlockManager::redstoneRepeaterIdle->isSameDiode(blockID) || BlockManager::redstoneComparatorIdle->isSameDiode(blockID);
}

bool BlockRedstoneLogic::isSameDiode(int blockID)
{
	return blockID == getPoweredState() || blockID == getUperState();
}

bool BlockRedstoneLogic::isFacingTowardsRepeater(World* pWorld, const BlockPos& pos, int meta)
{
	int dir = getDirection(meta);

	if (isRedstoneRepeaterBlockID(pWorld->getBlockId(pos - dir_offset[dir])))
	{
		int meta2 = pWorld->getBlockMeta(pos - dir_offset[dir]);
		int dir2 = getDirection(meta2);
		return dir2 != dir;
	}
	else
	{
		return false;
	}
}

BlockComparator::BlockComparator(int id, const BM_Material& mat, const String& name, bool powered)
: BlockRedstoneLogic(id, mat, name, powered)
{
	m_isBlockContainer = true;
}

int BlockComparator::idDropped(int meta, Random& rand, int fortune)
{
	return Item::comparator->itemID;
}

int BlockComparator::idPicked(World* pWorld, const BlockPos& pos)
{
	return Item::comparator->itemID;
}

int BlockComparator::getActiveSignal(IBlockAccess* pBlockAccess, const BlockPos& pos, int meta)
{
	return getTileEntityComparator(pBlockAccess, pos)->getOutputSignal();
}

int BlockComparator::getOutputStrength(World* pWorld, const BlockPos& pos, int meta)
{
	return !isComparatorActived(meta) ? getInputStrength(pWorld, pos, meta) :
		Math::Max(getInputStrength(pWorld, pos, meta) - calculateInputStrength(pWorld, pos, meta), 0);
}

bool BlockComparator::isGettingInput(World* pWorld, const BlockPos& pos, int meta)
{
	int strength = getInputStrength(pWorld, pos, meta);

	if (strength >= 15)
	{
		return true;
	}
	else if (strength == 0)
	{
		return false;
	}
	else
	{
		int var7 = calculateInputStrength(pWorld, pos, meta);
		return var7 == 0 ? true : strength >= var7;
	}
}

int BlockComparator::getInputStrength(World* pWorld, const BlockPos& pos, int meta)
{
	int strength = BlockRedstoneLogic::getInputStrength(pWorld, pos, meta);
	int dir = getDirection(meta);
	BlockPos pos1 = pos + dir_offset[dir];
	int blockID = pWorld->getBlockId(pos1);

	if (blockID > 0)
	{
		Block* pBlock = BlockManager::sBlocks[blockID];
		if (pBlock->hasComparatorInputOverride())
		{
			strength = pBlock->getComparatorInputOverride(pWorld, pos1, dir_rotateOpposite[dir]);
		}
		else if (strength < 15 && BlockManager::isNormalCube(blockID))
		{
			pos1 += dir_offset[dir];
			blockID = pWorld->getBlockId(pos1);

			if (blockID > 0 && BlockManager::sBlocks[blockID]->hasComparatorInputOverride())
			{
				strength = BlockManager::sBlocks[blockID]->getComparatorInputOverride(pWorld, pos1, dir_rotateOpposite[dir]);
			}
		}
	}

	return strength;
}

TileEntityComparator* BlockComparator::getTileEntityComparator(IBlockAccess* pBlockAccess, const BlockPos& pos)
{
	return (TileEntityComparator*)pBlockAccess->getBlockTileEntity(pos);
}

bool BlockComparator::onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit)
{
	pPlayer->setActivatedBlock(pos);
	int meta = pWorld->getBlockMeta(pos);
	bool ispowered = isRepeaterPowered | ((meta & 8) != 0);
	bool var12 = !isComparatorActived(meta);
	int var13 = var12 ? 4 : 0;
	var13 |= ispowered ? 8 : 0;
	//pWorld->playSoundEffect(Vector3(pos) + 0.5f, "random.click", 0.3F, var12 ? 0.55F : 0.5F);
	pWorld->playSoundEffectByType(Vector3(pos) + 0.5f, ST_Click);
	pWorld->setBlockMetadataWithNotify(pos, var13 | meta & 3, 2);
	onStateChange(pWorld, pos, pWorld->m_Rand);
	return true;
}

void BlockComparator::updateState(World* pWorld, const BlockPos& pos, int blockID)
{
	if (pWorld->isBlockTickScheduledThisTick(pos, m_blockID))
		return;

	int meta = pWorld->getBlockMeta(pos);
	int outStrength = getOutputStrength(pWorld, pos, meta);
	int outSignal = getTileEntityComparator(pWorld, pos)->getOutputSignal();

	if (outStrength != outSignal || isPowered(meta) != isGettingInput(pWorld, pos, meta))
	{
		if (isFacingTowardsRepeater(pWorld, pos, meta))
		{
			pWorld->scheduleBlockUpdateWithPriority(pos, m_blockID, getTickDelay(0), -1);
		}
		else
		{
			pWorld->scheduleBlockUpdateWithPriority(pos, m_blockID, getTickDelay(0), 0);
		}
	}
}

void BlockComparator::onStateChange(World* pWorld, const BlockPos& pos, Random& rand)
{
	int meta = pWorld->getBlockMeta(pos);
	int outStrength = getOutputStrength(pWorld, pos, meta);
	int outSignal = getTileEntityComparator(pWorld, pos)->getOutputSignal();
	getTileEntityComparator(pWorld, pos)->setOutputSignal(outStrength);

	if (outSignal != outStrength || !isComparatorActived(meta))
	{
		bool hasInput = isGettingInput(pWorld, pos, meta);
		bool hasPower = isRepeaterPowered || (meta & 8) != 0;

		if (hasPower && !hasInput)
		{
			pWorld->setBlockMetadataWithNotify(pos, meta & -9, 2);
		}
		else if (!hasPower && hasInput)
		{
			pWorld->setBlockMetadataWithNotify(pos, meta | 8, 2);
		}

		notifyNeighbors(pWorld, pos);
	}
}

void BlockComparator::updateTick(World* pWorld, const BlockPos& pos, Random& rand)
{
	if (isRepeaterPowered)
	{
		int meta = pWorld->getBlockMeta(pos);
		pWorld->setBlock(pos, getUperState(), meta | 8, 4);
	}

	onStateChange(pWorld, pos, rand);
}

void BlockComparator::onBlockAdded(World* pWorld, const BlockPos& pos)
{
	BlockRedstoneLogic::onBlockAdded(pWorld, pos);
	pWorld->setBlockTileEntity(pos, createNewTileEntity(pWorld));
}

void BlockComparator::breakBlock(World* pWorld, const BlockPos& pos, int blockID, int meta)
{
	Block::breakBlock(pWorld, pos, blockID, meta);
	pWorld->removeBlockTileEntity(pos);
	notifyNeighbors(pWorld, pos);
}

bool BlockComparator::onBlockEventReceived(World* pWorld, const BlockPos& pos, int id, int param)
{
	Block::onBlockEventReceived(pWorld, pos, id, param);
	TileEntity* pTileEntity = pWorld->getBlockTileEntity(pos);
	return pTileEntity != NULL ? pTileEntity->receiveClientEvent(id, param) : false;
}

TileEntity* BlockComparator::createNewTileEntity(World* pWorld)
{
	return LordNew TileEntityComparator();
}

int BlockRedstoneRepeater::repeaterState[4] = {1, 2, 3, 4};

BlockRedstoneRepeater::BlockRedstoneRepeater(int id, const BM_Material& mat, const String& name, bool powered)
: BlockRedstoneLogic(id, mat, name, powered)
{}

bool BlockRedstoneRepeater::onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit)
{
	pPlayer->setActivatedBlock(pos);
	int meta = pWorld->getBlockMeta(pos);
	int var11 = (meta & 12) >> 2;
	var11 = ((var11 + 1) << 2) & 12;
	pWorld->setBlockMetadataWithNotify(pos, var11 | meta & 3, 3);
	return true;
}

int BlockRedstoneRepeater::idDropped(int meta, Random& rand, int fortune)
{
	return Item::redstoneRepeater->itemID;
}

int BlockRedstoneRepeater::idPicked(World* pWorld, const BlockPos& pos)
{
	return Item::redstoneRepeater->itemID;
}

void BlockRedstoneRepeater::breakBlock(World* pWorld, const BlockPos& pos, int blockID, int meta)
{
	Block::breakBlock(pWorld, pos, blockID, meta);
	notifyNeighbors(pWorld, pos);
}

bool BlockRedstoneRepeater::isLocked(IBlockAccess* pBlockAccess, const BlockPos& pos, int meta)
{
	return calculateInputStrength(pBlockAccess, pos, meta) > 0;
}

bool BlockEndPortal::bossDefeated = false;

BlockEndPortal::BlockEndPortal(int id, const BM_Material& mat, const String& name)
: BlockContainer(id, mat, name, false)
{
	setLightValue(1.0F);
}

TileEntity* BlockEndPortal::createNewTileEntity(World* pWorld)
{
	return new TileEntityEndPortal();
}

void BlockEndPortal::setBlockBoundsBasedOnState(IBlockAccess* piBlockAccess, const BlockPos& pos)
{
	float h = 0.0625F;
	setBlockBounds(0.0F, 0.0F, 0.0F, 1.0F, h, 1.0F);
}

void BlockEndPortal::onEntityCollidedWithBlock(World* pWorld, const BlockPos& pos, Entity* pEntity)
{
	//todo.
	/*if (par5Entity.ridingEntity == null && par5Entity.riddenByEntity == null && !par1World.isRemote)
	{
		par5Entity.travelToDimension(1);
	}*/
}

void BlockEndPortal::onBlockAdded(World* pWorld, const BlockPos& pos)
{
	if (!bossDefeated)
	{
		if (pWorld->m_provider->dimensionId != 0)
		{
			pWorld->setBlockToAir(pos);
		}
	}
}

BlockEnderChest::BlockEnderChest(int id, const BM_Material& mat, const String& name, int block_ender_chest_type)
: BlockContainer(id, mat, name, false)
{
	m_block_ender_chest_type = block_ender_chest_type;
	setBlockBounds(0.0625F, 0.0F, 0.0625F, 0.9375F, 0.875F, 0.9375F);
}

int BlockEnderChest::idDropped(int meta, Random& rand, int fortune)
{
	return BLOCK_ID_OBSIDIAN;
}

void BlockEnderChest::onBlockPlacedBy(World* pWorld, const BlockPos& pos, EntityLivingBase* pLiving, ItemStackPtr pStack)
{
	int id = 0;
	int dir = int(Math::Floor((pLiving->rotationYaw * 4.0F / 360.0F) + 0.5f)) & 3;

	if (dir == 0)
		id = 2;

	if (dir == 1)
		id = 5;

	if (dir == 2)
		id = 3;

	if (dir == 3)
		id = 4;

	pWorld->setBlockMetadataWithNotify(pos, id, 2);
}

bool BlockEnderChest::onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit)
{
	pPlayer->setActivatedBlock(pos);
	InventoryEnderChest* pInventory = pPlayer->getInventoryEnderChest();
	TileEntityEnderChest* pChest = (TileEntityEnderChest*)pWorld->getBlockTileEntity(pos);

	if (pInventory && pChest)
	{
		if (pWorld->isBlockNormalCube(pos.getPosY()))
		{
			return true;
		}
		else if (pWorld->m_isClient)
		{
			return true;
		}
		else
		{
			pInventory->setAssociatedChest(pChest);
			BLOCKMAN::GameCommon::Instance()->getCommonPacketSender()->sendChestInventory(pPlayer, pos, face, hit, pInventory);
			return true;
		}
	}
	else
	{
		return true;
	}
}

TileEntity* BlockEnderChest::createNewTileEntity(World* pWorld)
{
	TileEntityEnderChest* chest = new TileEntityEnderChest();
	chest->setWorldObj(pWorld);
	return chest;
}

BlockSkull::BlockSkull(int id, const BM_Material& mat, const String& name)
: BlockContainer(id, mat, name, false)
{
	setBlockBounds(0.25F, 0.0F, 0.25F, 0.75F, 0.5F, 0.75F);
}

void BlockSkull::setBlockBoundsBasedOnState(IBlockAccess* piBlockAccess, const BlockPos& pos)
{
	int side = piBlockAccess->getBlockMeta(pos) & 7;

	switch (side)
	{
	case 1:
	default:
		setBlockBounds(0.25F, 0.0F, 0.25F, 0.75F, 0.5F, 0.75F);
		break;

	case 2:
		setBlockBounds(0.25F, 0.25F, 0.5F, 0.75F, 0.75F, 1.0F);
		break;

	case 3:
		setBlockBounds(0.25F, 0.25F, 0.0F, 0.75F, 0.75F, 0.5F);
		break;

	case 4:
		setBlockBounds(0.5F, 0.25F, 0.25F, 1.0F, 0.75F, 0.75F);
		break;

	case 5:
		setBlockBounds(0.0F, 0.25F, 0.25F, 0.5F, 0.75F, 0.75F);
	}
}

Box BlockSkull::getCollisionBoundingBox(World* pWorld, const BlockPos& pos)
{
	setBlockBoundsBasedOnState(pWorld, pos);
	return Block::getCollisionBoundingBox(pWorld, pos);
}

void BlockSkull::onBlockPlacedBy(World* pWorld, const BlockPos& pos, EntityLivingBase* pLiving, ItemStackPtr pStack)
{
	int dir = int(Math::Floor((pLiving->rotationYaw * 4.0F / 360.0F) + 2.5f)) & 3;
	pWorld->setBlockMetadataWithNotify(pos, dir, 2);
}

TileEntity* BlockSkull::createNewTileEntity(World* par1World)
{
	return LordNew TileEntitySkull();
}

int BlockSkull::idPicked(World* pWorld, const BlockPos& pos)
{
	return Item::skull->itemID;
}

int BlockSkull::getDamageValue(World* pWorld, const BlockPos& pos)
{
	TileEntity* pTileEntity = pWorld->getBlockTileEntity(pos);
	// todo.
	// return pTileEntity && pTileEntity instanceof TileEntitySkull ? ((TileEntitySkull)pTileEntity).getSkullType() : Block::getDamageValue(pWorld, pos);
	return 0;
}

void BlockSkull::onBlockHarvested(World* pWorld, const BlockPos& pos, int meta, EntityPlayer* pPlayer)
{
	if (pPlayer->capabilities.isCreativeMode)
	{
		meta |= 8;
		pWorld->setBlockMetadataWithNotify(pos, meta, 4);
	}

	Block::onBlockHarvested(pWorld, pos, meta, pPlayer);
}

void BlockSkull::breakBlock(World* pWorld, const BlockPos& pos, int blockID, int meta)
{
	if (pWorld->m_isClient)
		return;
	
	if ((meta & 8) == 0)
	{
		ItemStackPtr pStack = LORD::make_shared<ItemStack>(Item::skull->itemID, 1, getDamageValue(pWorld, pos));

		// todo.
		/*TileEntitySkull var8 = (TileEntitySkull)par1World.getBlockTileEntity(par2, par3, par4);

		if (var8.getSkullType() == 3 && var8.getExtraType() != null && var8.getExtraType().length() > 0)
		{
			pStack.setTagCompound(new NBTTagCompound());
			pStack.getTagCompound().setString("SkullOwner", var8.getExtraType());
		}*/

		dropBlockAsItem_do(pWorld, pos, pStack);
	}

	Block::breakBlock(pWorld, pos, blockID, meta);
}

int BlockSkull::idDropped(int meta, Random& rand, int fortune)
{
	return Item::skull->itemID;
}

void BlockSkull::makeWither(World* pWorld, const BlockPos& pos, TileEntitySkull* pSkull)
{
	if (pSkull->getSkullType() == 1 && pos.y >= 2 && pWorld->m_difficultySetting > 0 && !pWorld->m_isClient)
	{
		int ssandID = BLOCK_ID_SLOW_SAND;

		for (int i = -2; i <= 0; ++i)
		{
			if (pWorld->getBlockId(pos + BlockPos(0, -1, i + 0)) == ssandID &&
				pWorld->getBlockId(pos + BlockPos(0, -1, i + 1)) == ssandID &&
				pWorld->getBlockId(pos + BlockPos(0, -2, i + 1)) == ssandID &&
				pWorld->getBlockId(pos + BlockPos(0, -1, i + 2)) == ssandID &&
				isSkullType(pWorld, pos + BlockPos(0, 0, i + 0), 1) &&
				isSkullType(pWorld, pos + BlockPos(0, 0, i + 1), 1) &&
				isSkullType(pWorld, pos + BlockPos(0, 0, i + 2), 1))
			{
				pWorld->setBlockMetadataWithNotify(pos + BlockPos(0, 0, i + 0), 8, 2);
				pWorld->setBlockMetadataWithNotify(pos + BlockPos(0, 0, i + 1), 8, 2);
				pWorld->setBlockMetadataWithNotify(pos + BlockPos(0, 0, i + 2), 8, 2);
				pWorld->setBlock(pos + BlockPos(0, 0, i + 0), 0, 0, 2);
				pWorld->setBlock(pos + BlockPos(0, 0, i + 1), 0, 0, 2);
				pWorld->setBlock(pos + BlockPos(0, 0, i + 2), 0, 0, 2);
				pWorld->setBlock(pos + BlockPos(0, -1, i + 0), 0, 0, 2);
				pWorld->setBlock(pos + BlockPos(0, -1, i + 1), 0, 0, 2);
				pWorld->setBlock(pos + BlockPos(0, -1, i + 2), 0, 0, 2);
				pWorld->setBlock(pos + BlockPos(0, -2, i + 1), 0, 0, 2);

				if (!pWorld->m_isClient)
				{
					// todo.
					/*EntityWither* var8 = new EntityWither(pWorld);
					var8.setLocationAndAngles((double)par2 + 0.5D, (double)par3 - 1.45D, (double)(par4 + i) + 1.5D, 90.0F, 0.0F);
					var8.renderYawOffset = 90.0F;
					var8.func_82206_m();
					pWorld->spawnEntityInWorld(var8);*/
				}

				for (int j = 0; j < 120; ++j)
				{
					Vector3 loc = Vector3(pos + BlockPos(0, -2, i + 1)) + pWorld->m_Rand.nextVector() * Vector3(1.f, 3.9f, 1.f);
					pWorld->spawnParticle("snowballpoof", loc, Vector3::ZERO);
				}

				pWorld->notifyBlockChange(pos + BlockPos(0, 0, i + 0), 0);
				pWorld->notifyBlockChange(pos + BlockPos(0, 0, i + 1), 0);
				pWorld->notifyBlockChange(pos + BlockPos(0, 0, i + 2), 0);
				pWorld->notifyBlockChange(pos + BlockPos(0, -1, i + 0), 0);
				pWorld->notifyBlockChange(pos + BlockPos(0, -1, i + 1), 0);
				pWorld->notifyBlockChange(pos + BlockPos(0, -1, i + 2), 0);
				pWorld->notifyBlockChange(pos + BlockPos(0, -2, i + 1), 0);
				return;
			}
		}

		for (int i = -2; i <= 0; ++i)
		{
			if (pWorld->getBlockId(pos + BlockPos(i + 0, -1, 0)) == ssandID &&
				pWorld->getBlockId(pos + BlockPos(i + 1, -1, 0)) == ssandID &&
				pWorld->getBlockId(pos + BlockPos(i + 1, -2, 0)) == ssandID &&
				pWorld->getBlockId(pos + BlockPos(i + 2, -1, 0)) == ssandID &&
				isSkullType(pWorld, pos + BlockPos(i + 0, 0, 0), 1) &&
				isSkullType(pWorld, pos + BlockPos(i + 1, 0, 0), 1) &&
				isSkullType(pWorld, pos + BlockPos(i + 2, 0, 0), 1))
			{
				pWorld->setBlockMetadataWithNotify(pos + BlockPos(i + 0, 0, 0), 8, 2);
				pWorld->setBlockMetadataWithNotify(pos + BlockPos(i + 1, 0, 0), 8, 2);
				pWorld->setBlockMetadataWithNotify(pos + BlockPos(i + 2, 0, 0), 8, 2);
				pWorld->setBlock(pos + BlockPos(i + 0, 0, 0), 0, 0, 2);
				pWorld->setBlock(pos + BlockPos(i + 1, 0, 0), 0, 0, 2);
				pWorld->setBlock(pos + BlockPos(i + 2, 0, 0), 0, 0, 2);
				pWorld->setBlock(pos + BlockPos(i + 0, -1, 0), 0, 0, 2);
				pWorld->setBlock(pos + BlockPos(i + 1, -1, 0), 0, 0, 2);
				pWorld->setBlock(pos + BlockPos(i + 2, -1, 0), 0, 0, 2);
				pWorld->setBlock(pos + BlockPos(i + 1, -2, 0), 0, 0, 2);

				if (!pWorld->m_isClient)
				{
					// todo.
					/*
					EntityWither* var8 = new EntityWither(pWorld);
					var8.setLocationAndAngles((double)(par2 + i) + 1.5D, (double)par3 - 1.45D, (double)par4 + 0.5D, 0.0F, 0.0F);
					var8.func_82206_m();
					pWorld->spawnEntityInWorld(var8);
					*/
				}

				for (int j = 0; j < 120; ++j)
				{
					Vector3 loc = Vector3(pos + BlockPos(i + 1, -2, 0)) + pWorld->m_Rand.nextVector() * Vector3(1.f, 3.9f, 1.f);
					pWorld->spawnParticle("snowballpoof", loc, Vector3::ZERO);
				}

				pWorld->notifyBlockChange(pos + BlockPos(i + 0, 0, 0), 0);
				pWorld->notifyBlockChange(pos + BlockPos(i + 1, 0, 0), 0);
				pWorld->notifyBlockChange(pos + BlockPos(i + 2, 0, 0), 0);
				pWorld->notifyBlockChange(pos + BlockPos(i + 0, -1, 0), 0);
				pWorld->notifyBlockChange(pos + BlockPos(i + 1, -1, 0), 0);
				pWorld->notifyBlockChange(pos + BlockPos(i + 2, -1, 0), 0);
				pWorld->notifyBlockChange(pos + BlockPos(i + 1, -2, 0), 0);
				return;
			}
		}
	}
}

bool BlockSkull::isSkullType(World* pWorld, const BlockPos& pos, int skullType)
{
	if (pWorld->getBlockId(pos) != m_blockID)
	{
		return false;
	}
	else
	{
		TileEntity* pTileEntity = pWorld->getBlockTileEntity(pos);
		// todo. change to rtti.
		TileEntitySkull* pSkull = dynamic_cast<TileEntitySkull*>(pTileEntity);
		return pSkull ? pSkull->getSkullType() == skullType : false;
	}
}

BlockWithEffect::BlockWithEffect(int id, const BM_Material & mat, const String & name)
	: Block(id, mat, name, false)
{
}

BlockDirt::BlockDirt(int id, const BM_Material& mat, const String& name)
	: Block(id, mat, name, true)
{
}
BlockNewCrops::BlockNewCrops(int id, const BM_Material & mat, const String & name, int seedsId)
	: BlockFlower(id, mat, name)
	, seedsIdOfCrops(seedsId)
{
	float offset = 0.5F;
	setBlockBounds(0.5F - offset, 0.0F, 0.5F - offset, 0.5F + offset, 0.25F, 0.5F + offset);
	setHardness(0.0F);
	disableStats();
}

int BlockNewCrops::idDropped(int meta, Random & rand, int fortune)
{
	return 0;
}

// copy from BlockCrops
void BlockNewCrops::dropBlockAsItemWithChance(World * pWorld, const BlockPos & pos, int meta, float chance, int fortune)
{
	BlockFlower::dropBlockAsItemWithChance(pWorld, pos, meta, chance, 0);

	if (!pWorld->m_isClient)
	{
		if (meta >= 4)
		{
			int var8 = 3 + fortune;

			for (int i = 0; i < var8; ++i)
			{
				if (pWorld->m_Rand.nextInt(15) <= meta)
				{
					dropBlockAsItem_do(pWorld, pos, LORD::make_shared<ItemStack>(getSeedItem(), 1, 0));
				}
			}
		}
	}
}

bool BlockNewCrops::checkFlowerChange(World * pWorld, const BlockPos & pos)
{
	bool bRet = BlockFlower::checkFlowerChange(pWorld, pos);
	if (bRet)
	{
		OnNewCropsBlockDestroyEvent::emit(pos);
	}

	return bRet;
}

BlockSlime::BlockSlime(int id, const BM_Material& mat, const String& name)
	: Block(id, mat, name, true)
{
	m_speed_attenuation = 0.5F;
	m_immune_fall_damage = true;
}

void BlockSlime::onFallenUpon(World* pWorld, const BlockPos& pos, Entity* pEntity, float distance)
{
	if (distance < 0.2)
		return;

	float dest = distance * 0.8f;
	float totle = 0.0f;
	float motion = 0.0f;

	do 
	{
		motion = (motion / 0.98f) + 0.08f;
		totle += motion;
		if (totle >= dest)
			break;
	} while (true);

	pEntity->motion.y = motion;
}

BlockRoad::BlockRoad(int id, const BM_Material & mat, const String & name)
	: BlockCarpet(id, mat, name)
{
	setBounding();
	setTickRandomly(true);
}

Box BlockRoad::getCollisionBoundingBox(World * pWorld, const BlockPos & pos)
{
	return Box(pos.x + m_min.x, pos.y + m_min.y, pos.z + m_min.z, pos.x + m_max.x, pos.y + 0.1F, pos.z + m_max.z);
}

void BlockRoad::setBounding()
{
	setBlockBounds(0.0F, 0.0F, 0.0F, 1.0F, 0.1F, 1.0F);
}

BlockFruits::BlockFruits(int id, const BM_Material & mat, const String & name)
	: BlockFlower(id, mat, name)
{
	float offset = 0.5F;
	setBlockBounds(0.5F - offset, 0.0F, 0.5F - offset, 0.5F + offset, 0.25F, 0.5F + offset);
	setHardness(0.0F);
	disableStats();
}

int BlockFruits::idDropped(int meta, Random & rand, int fortune)
{
	return 0;
}

void BlockFruits::dropBlockAsItemWithChance(World * pWorld, const BlockPos & pos, int meta, float chance, int fortune)
{
	BlockFlower::dropBlockAsItemWithChance(pWorld, pos, meta, chance, 0);
}

bool BlockFruits::onBlockActivated(World * pWorld, const BlockPos & pos, EntityPlayer * pPlayer, int face, const Vector3 & hit)
{
	if (!pWorld->m_isClient)
	{
		IntProxy num;
		num.value = 1;
		if (SCRIPT_EVENT::BirdSimulatorGatherEvent::invoke(pPlayer->getPlatformUserId(), &num, pos))
		{
			int yaw = (((int)pPlayer->rotationYaw) % 360 + 360 + 45) % 360 / 90 * 90;

			for (int i = 0; i < num.value; i++)
			{
				int xOffset = (int)floor(i / 3);
				int zOffset = i % 3;

				switch (yaw) {
				case 0:
				{
					BlockPos newPos = BlockPos(pos.x - zOffset, pos.y, pos.z + xOffset);
					BLOCKMAN::GameCommon::Instance()->getCommonPacketSender()->onBirdSimulatorGather(pPlayer->getPlatformUserId(), 0, newPos);
				}
				break;
				case 90:
				{
					BlockPos newPos = BlockPos(pos.x - xOffset, pos.y, pos.z - zOffset);
					BLOCKMAN::GameCommon::Instance()->getCommonPacketSender()->onBirdSimulatorGather(pPlayer->getPlatformUserId(), 0, newPos);
				}
				break;
				case 180:
				{
					BlockPos newPos = BlockPos(pos.x + zOffset, pos.y, pos.z - xOffset);
					BLOCKMAN::GameCommon::Instance()->getCommonPacketSender()->onBirdSimulatorGather(pPlayer->getPlatformUserId(), 0, newPos);
				}
				break;
				case 270:
				{
					BlockPos newPos = BlockPos(pos.x + xOffset, pos.y, pos.z + zOffset);
					BLOCKMAN::GameCommon::Instance()->getCommonPacketSender()->onBirdSimulatorGather(pPlayer->getPlatformUserId(), 0, newPos);
				}
				break;
				}

			}

			/*
			for (int i = 0; i < num.value; i++)
			{
				int offset = (int)ceil(num.value * 1.0f / 2) - 1;
				switch (yaw) {
				case 0:
				{
					BlockPos startPos = BlockPos(pos.x - offset, pos.y, pos.z);
					BlockPos newPos = BlockPos(startPos.x + i, startPos.y, startPos.z);
					LOGIC::BirdSimulatorGatherEvent::emit(pPlayer, newPos, m_blockID);
				}
					break;
				case 90:
				{
					BlockPos startPos = BlockPos(pos.x, pos.y, pos.z - offset);
					BlockPos newPos = BlockPos(startPos.x, startPos.y, startPos.z + i);
					LOGIC::BirdSimulatorGatherEvent::emit(pPlayer, newPos, m_blockID);
				}
					break;
				case 180:
				{
					BlockPos startPos = BlockPos(pos.x - offset, pos.y, pos.z);
					BlockPos newPos = BlockPos(startPos.x + i, startPos.y, startPos.z);
					LOGIC::BirdSimulatorGatherEvent::emit(pPlayer, newPos, m_blockID);
				}
					break;
				case 270:
				{
					BlockPos startPos = BlockPos(pos.x, pos.y, pos.z - offset);
					BlockPos newPos = BlockPos(startPos.x, startPos.y, startPos.z + i);
					LOGIC::BirdSimulatorGatherEvent::emit(pPlayer, newPos, m_blockID);
				}
					break;
				}
			}
			*/
		}
	}
	return true;
}

bool BlockFruits::canThisPlantGrowOnThisBlockID(int blockID)
{
	return true;
}

void BlockFruits::onNeighborBlockChange(World * pWorld, const BlockPos & pos, int neighborblockID)
{
	Block::onNeighborBlockChange(pWorld, pos, neighborblockID);
}

BlockNest::BlockNest(int id, const BM_Material & mat, const String & name)
	: BlockCarpet(id, mat, name)
{
	setBounding();
	setTickRandomly(true);
}

Box BlockNest::getCollisionBoundingBox(World * pWorld, const BlockPos & pos)
{
	return Box(pos.x + m_min.x, pos.y + m_min.y, pos.z + m_min.z, pos.x + m_max.x, pos.y + 0.1F, pos.z + m_max.z);
}

bool BlockNest::onBlockActivated(World * pWorld, const BlockPos & pos, EntityPlayer * pPlayer, int face, const Vector3 & hit)
{
	if (pWorld->m_isClient)
	{
		
	}
	return false;
}

void BlockNest::setBounding()
{
	setBlockBounds(0.0F, 0.0F, 0.0F, 1.0F, 0.1F, 1.0F);
}

BlockStainedGlass::BlockStainedGlass(int id, const BM_Material& mat, const String& name)
	: BlockBreakable(id, mat, name, false)
{}

BlockTransparency::BlockTransparency(int id, const BM_Material& mat, const String& name)
	: BlockGlass(id, mat, name)
{}

void BlockTransparency::addCollisionBoxesToList(World* pWorld, const BlockPos& pos, const Box& aabb, AABBList& lst, Entity* pEntity)
{
	auto entityPlayer = dynamic_cast<EntityPlayer*>(pEntity);
	if (entityPlayer)
	{
		Block::addCollisionBoxesToList(pWorld, pos, aabb, lst, pEntity);
	}
}

}
