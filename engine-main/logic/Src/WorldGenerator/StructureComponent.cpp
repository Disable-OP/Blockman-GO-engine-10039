#include "StructureComponent.h"
#include "StructurePieceBlockSelector.h"
#include "StructurePieces.h"
#include "WeightedRandomItem.h"

#include "Block/Block.h"
#include "Block/BlockManager.h"
#include "World/World.h"

namespace BLOCKMAN
{

StructureComponent::StructureComponent(int type)
	: m_coordBaseMode(-1)
	, m_componentType(type)
{
}

StructureComponent* StructureComponent::findIntersecting(SCList& scList, const StructureBB& aabb)
{
	StructureComponent* pResult = NULL;

	for (SCList::iterator it = scList.begin(); it != scList.end(); ++it)
	{
		pResult = *it;

		if (!pResult)
			continue;

		if (pResult->getBoundingBox().intersectsWith(aabb))
			break;
	}

	return pResult;
}

ChunkPosition StructureComponent::getCenter() const
{
	return ChunkPosition(m_boundingBox.getCenterX(), m_boundingBox.getCenterY(), m_boundingBox.getCenterZ());
}

bool StructureComponent::isLiquidInStructureBoundingBox(World* pWorld, const StructureBB& par2StructureBoundingBox) const
{
	int minX = Math::Max(m_boundingBox.m_minX - 1, par2StructureBoundingBox.m_minX);
	int minY = Math::Max(m_boundingBox.m_minY - 1, par2StructureBoundingBox.m_minY);
	int minZ = Math::Max(m_boundingBox.m_minZ - 1, par2StructureBoundingBox.m_minZ);
	int maxX = Math::Min(m_boundingBox.m_maxX + 1, par2StructureBoundingBox.m_maxX);
	int maxY = Math::Min(m_boundingBox.m_maxY + 1, par2StructureBoundingBox.m_maxY);
	int maxZ = Math::Min(m_boundingBox.m_maxZ + 1, par2StructureBoundingBox.m_maxZ);
	int x;
	int z;
	int blockID;

	Block** pblockArr = BlockManager::sBlocks;

	for (x = minX; x <= maxX; ++x)
	{
		for (z = minZ; z <= maxZ; ++z)
		{
			blockID = pWorld->getBlockId(BlockPos(x, minY, z));

			if (blockID > 0 && pblockArr[blockID] && pblockArr[blockID]->getMaterial().isLiquid())
			{
				return true;
			}

			blockID = pWorld->getBlockId(BlockPos(x, maxY, z));

			if (blockID > 0 && pblockArr[blockID] && pblockArr[blockID]->getMaterial().isLiquid())
			{
				return true;
			}
		}
	}

	for (x = minX; x <= maxX; ++x)
	{
		for (z = minY; z <= maxY; ++z)
		{
			blockID = pWorld->getBlockId(BlockPos(x, z, minZ));

			if (blockID > 0 && pblockArr[blockID] && pblockArr[blockID]->getMaterial().isLiquid())
			{
				return true;
			}

			blockID = pWorld->getBlockId(BlockPos(x, z, maxZ));

			if (blockID > 0 && pblockArr[blockID] && pblockArr[blockID]->getMaterial().isLiquid())
			{
				return true;
			}
		}
	}

	for (x = minZ; x <= maxZ; ++x)
	{
		for (z = minY; z <= maxY; ++z)
		{
			blockID = pWorld->getBlockId(BlockPos(minX, z, x));

			if (blockID > 0 && pblockArr[blockID] && pblockArr[blockID]->getMaterial().isLiquid())
			{
				return true;
			}

			blockID = pWorld->getBlockId(BlockPos(maxX, z, x));

			if (blockID > 0 && pblockArr[blockID] && pblockArr[blockID]->getMaterial().isLiquid())
			{
				return true;
			}
		}
	}

	return false;
}

int StructureComponent::getXWithOffset(int par1, int par2) const
{
	switch (m_coordBaseMode)
	{
	case 0:
	case 2:
		return m_boundingBox.m_minX + par1;

	case 1:
		return m_boundingBox.m_maxX - par2;

	case 3:
		return m_boundingBox.m_minX + par2;

	default:
		return par1;
	}
}

int StructureComponent::getYWithOffset(int par1) const
{
	return m_coordBaseMode == -1 ? par1 : par1 + m_boundingBox.m_minY;
}

int StructureComponent::getZWithOffset(int par1, int par2) const
{
	switch (m_coordBaseMode)
	{
	case 0:
		return m_boundingBox.m_minZ + par2;

	case 1:
	case 3:
		return m_boundingBox.m_minZ + par1;

	case 2:
		return m_boundingBox.m_maxZ - par2;

	default:
		return par2;
	}
}

int StructureComponent::getMetadataWithOffset(int blockID, int par2) const
{
	if (blockID == BLOCK_ID_RAIL)
	{
		if (m_coordBaseMode == 1 || m_coordBaseMode == 3)
		{
			if (par2 == 1)	
				return 0;

			return 1;
		}
	}

	else if (blockID != BLOCK_ID_DOOR_WOOD && blockID != BLOCK_ID_DOOR_IRON)
	{
		if (blockID != BLOCK_ID_STAIRS_COBBLE_STONE &&
			blockID != BLOCK_ID_STAIRS_WOOD_OAK &&
			blockID != BLOCK_ID_STAIRS_NETHER_BRICK &&
			blockID != BLOCK_ID_STAIRS_STONE_BRICK &&
			blockID != BLOCK_ID_STAIRS_SAND_STONE)
		{
			if (blockID == BLOCK_ID_LADDER)
			{
				if (m_coordBaseMode == 0)
				{
					if (par2 == 2)
						return 3;
					else if (par2 == 3)
						return 2;
				}
				else if (m_coordBaseMode == 1)
				{
					if (par2 == 2)
						return 4;
					else if (par2 == 3)
						return 5;
					else if (par2 == 4)
						return 2;
					else if (par2 == 5)
						return 3;
				}
				else if (m_coordBaseMode == 3)
				{
					if (par2 == 2)
						return 5;
					else if (par2 == 3)
						return 4;
					else if (par2 == 4)
						return 2;
					else if (par2 == 5)
						return 3;
				}
			}
			else if (blockID == BLOCK_ID_STONE_BUTTON)
			{
				if (m_coordBaseMode == 0)
				{
					if (par2 == 3)
						return 4;
					else if (par2 == 4)
						return 3;
				}
				else if (m_coordBaseMode == 1)
				{
					if (par2 == 3)
						return 1;
					else if (par2 == 4)
						return 2;
					else if (par2 == 2)
						return 3;
					else if (par2 == 1)
						return 4;
				}
				else if (m_coordBaseMode == 3)
				{
					if (par2 == 3)
						return 2;
					else if (par2 == 4)
						return 1;
					else if (par2 == 2)
						return 3;
					else if (par2 == 1)
						return 4;
				}
			}
			else if (blockID != BLOCK_ID_TRIP_WIRE_SOURCE && 
				(BlockManager::sBlocks[blockID] == NULL || 
					!(BlockManager::sBlocks[blockID]->checkType(BLOCK_TYPE_DIRECTIONAL))))
			{
				if (blockID == BLOCK_ID_PISTON_BASE || blockID == BLOCK_ID_PISTON_STICKY_BASE || blockID == BLOCK_ID_LEVER || blockID == BLOCK_ID_DISPENSER)
				{
					if (m_coordBaseMode == 0)
					{
						if (par2 == 2 || par2 == 3)
							return oppositeSide[par2];
					}
					else if (m_coordBaseMode == 1)
					{
						if (par2 == 2)
							return 4;
						else if (par2 == 3)
							return 5;
						else if (par2 == 4)
							return 2;
						else if (par2 == 5)
							return 3;
					}
					else if (m_coordBaseMode == 3)
					{
						if (par2 == 2)
							return 5;
						else if (par2 == 3)
							return 4;
						else if (par2 == 4)
							return 2;
						else if (par2 == 5)
							return 3;
					}
				}
			}
			else if (m_coordBaseMode == 0)
			{
				if (par2 == 0 || par2 == 2)
					return dir_rotateOpposite[par2];
			}
			else if (m_coordBaseMode == 1)
			{
				if (par2 == 2)
					return 1;
				else if (par2 == 0)
					return 3;
				else if (par2 == 1)
					return 2;
				else if (par2 == 3)
					return 0;
			}
			else if (m_coordBaseMode == 3)
			{
				if (par2 == 2)
					return 3;
				else if (par2 == 0)
					return 1;
				else if (par2 == 1)
					return 2;
				else if (par2 == 3)
					return 0;
			}
		}
		else if (m_coordBaseMode == 0)
		{
			if (par2 == 2)
				return 3;
			else if (par2 == 3)
				return 2;
		}
		else if (m_coordBaseMode == 1)
		{
			if (par2 == 0)
				return 2;
			else if (par2 == 1)
				return 3;
			else if (par2 == 2)
				return 0;
			else if (par2 == 3)
				return 1;
		}
		else if (m_coordBaseMode == 3)
		{
			if (par2 == 0)
				return 2;
			else if (par2 == 1)
				return 3;
			else if (par2 == 2)
				return 1;
			else if (par2 == 3)
				return 0;
		}
	}
	else if (m_coordBaseMode == 0)
	{
		if (par2 == 0)
			return 2;
		else if (par2 == 2)
			return 0;
	}
	else
	{
		if (m_coordBaseMode == 1)
			return par2 + 1 & 3;
		else if (m_coordBaseMode == 3)
			return par2 + 3 & 3;
	}

	return par2;
}

void StructureComponent::placeBlockAtCurrentPosition(World* pWorld, int blockID, int metadata, int x, int y, int z, const StructureBB& aabb)
{
	int rx = getXWithOffset(x, z);
	int ry = getYWithOffset(y);
	int rz = getZWithOffset(x, z);

	if (aabb.isVecInside(rx, ry, rz))
	{
		pWorld->setBlock(BlockPos(rx, ry, rz), blockID, metadata, 2);
	}
}

int StructureComponent::getBlockIdAtCurrentPosition(World* pWorld, int x, int y, int z, const StructureBB& aabb)
{
	int rx = getXWithOffset(x, z);
	int ry = getYWithOffset(y);
	int rz = getZWithOffset(x, z);

	return !aabb.isVecInside(rx, ry, rz) ? 0 : pWorld->getBlockId(BlockPos(rx, ry, rz));
}

void StructureComponent::fillWithAir(World* pWorld, const StructureBB& aabb, int minX, int minY, int minZ, int maxX, int maxY, int maxZ)
{
	for (int y = minY; y <= maxY; ++y)
	{
		for (int x = minX; x <= maxX; ++x)
		{
			for (int z = minZ; z <= maxZ; ++z)
			{
				placeBlockAtCurrentPosition(pWorld, 0, 0, x, y, z, aabb);
			}
		}
	}
}

void StructureComponent::fillWithBlocks(World* pWorld, const StructureBB& aabb, int minX, int minY, int minZ, 
	int maxX, int maxY, int maxZ, int placeID, int replaceID, bool alwaysreplace)
{
	for (int y = minY; y <= maxY; ++y)
	{
		for (int x = minX; x <= maxX; ++x)
		{
			for (int z = minZ; z <= maxZ; ++z)
			{
				if (!alwaysreplace || getBlockIdAtCurrentPosition(pWorld, x, y, z, aabb) != 0)
				{
					if (y != minY && y != maxY && x != minX && x != maxX && z != minZ && z != maxZ)
					{
						placeBlockAtCurrentPosition(pWorld, replaceID, 0, x, y, z, aabb);
					}
					else
					{
						placeBlockAtCurrentPosition(pWorld, placeID, 0, x, y, z, aabb);
					}
				}
			}
		}
	}
}

void StructureComponent::fillWithMetadataBlocks(World* pWorld, const StructureBB& aabb, int minX, int minY, int minZ, int maxX, int maxY, int maxZ,
	int placeID, int placeMetadata, int replaceID, int replaceMetadata, bool alwaysreplace)
{
	for (int y = minY; y <= maxY; ++y)
	{
		for (int x = minX; x <= maxX; ++x)
		{
			for (int z = minZ; z <= maxZ; ++z)
			{
				if (!alwaysreplace || getBlockIdAtCurrentPosition(pWorld, x, y, z, aabb) != 0)
				{
					if (y != minY && y != maxY && x != minX && x != maxX && z != minZ && z != maxZ)
					{
						placeBlockAtCurrentPosition(pWorld, replaceID, replaceMetadata, x, y, z, aabb);
					}
					else
					{
						placeBlockAtCurrentPosition(pWorld, placeID, placeMetadata, x, y, z, aabb);
					}
				}
			}
		}
	}
}

void StructureComponent::fillWithRandomizedBlocks(World* pWorld, const StructureBB& aabb, int minX, int minY, int minZ, int maxX, int maxY, int maxZ,
	bool alwaysreplace, Random& rand, StructurePieceBlockSelector* selector)
{
	LordAssert(pWorld);
	LordAssert(selector);

	for (int y = minY; y <= maxY; ++y)
	{
		for (int x = minX; x <= maxX; ++x)
		{
			for (int z = minZ; z <= maxZ; ++z)
			{
				if (!alwaysreplace || getBlockIdAtCurrentPosition(pWorld, x, y, z, aabb) != 0)
				{
					selector->selectBlocks(rand, x, y, z, y == minY || y == maxY || x == minX || x == maxX || z == minZ || z == maxZ);
					placeBlockAtCurrentPosition(pWorld, selector->getSelectedBlockId(), selector->getSelectedBlockMetaData(), x, y, z, aabb);
				}
			}
		}
	}
}

void StructureComponent::randomlyFillWithBlocks(World* pWorld, const StructureBB& aabb, Random& rand, float randLimit,
	int minX, int minY, int minZ, int maxX, int maxY, int maxZ, int olaceID, int replaceID, bool alwaysreplace)
{
	LordAssert(pWorld);

	for (int y = minY; y <= maxY; ++y)
	{
		for (int x = minX; x <= maxX; ++x)
		{
			for (int z = minZ; z <= maxZ; ++z)
			{
				if (rand.nextFloat() <= randLimit && (!alwaysreplace || getBlockIdAtCurrentPosition(pWorld, x, y, z, aabb) != 0))
				{
					if (y != minY && y != maxY && x != minX && x != maxX && z != minZ && z != maxZ)
					{
						placeBlockAtCurrentPosition(pWorld, replaceID, 0, x, y, z, aabb);
					}
					else
					{
						placeBlockAtCurrentPosition(pWorld, olaceID, 0, x, y, z, aabb);
					}
				}
			}
		}
	}
}

void StructureComponent::randomlyPlaceBlock(World* pWorld, const StructureBB& aabb, Random& rand, float randLimit, int x, int y, int z, int blockID, int metadata)
{
	if (rand.nextFloat() < randLimit)
	{
		placeBlockAtCurrentPosition(pWorld, blockID, metadata, x, y, z, aabb);
	}
}

void StructureComponent::randomlyRareFillWithBlocks(World* pWorld, const StructureBB& aabb, int minX, int minY, int minZ,
	int maxX, int maxY, int maxZ, int placeID, bool alwaysreplace)
{
	float xSize = (float)(maxX - minX + 1);
	float ySize = (float)(maxY - minY + 1);
	float zSize = (float)(maxZ - minZ + 1);
	float xCentral = (float)minX + xSize / 2.0F;
	float zCentral = (float)minZ + zSize / 2.0F;

	for (int y = minY; y <= maxY; ++y)
	{
		float ySegment = (float)(y - minY) / ySize;

		for (int x = minX; x <= maxX; ++x)
		{
			float xOffsetPercent = ((float)x - xCentral) / (xSize * 0.5F);

			for (int z = minZ; z <= maxZ; ++z)
			{
				float zOffsetPercent = ((float)z - zCentral) / (zSize * 0.5F);

				if (!alwaysreplace || getBlockIdAtCurrentPosition(pWorld, x, y, z, aabb) != 0)
				{
					float var22 = xOffsetPercent * xOffsetPercent + ySegment * ySegment + zOffsetPercent * zOffsetPercent;

					if (var22 <= 1.05)
					{
						placeBlockAtCurrentPosition(pWorld, placeID, 0, x, y, z, aabb);
					}
				}
			}
		}
	}
}

void StructureComponent::clearCurrentPositionBlocksUpwards(World* pWorld, int x, int y, int z, const StructureBB& aabb)
{
	BlockPos pos1;
	pos1.x = getXWithOffset(x, z);
	pos1.y = getYWithOffset(y);
	pos1.z = getZWithOffset(x, z);

	if (aabb.isVecInside(pos1))
	{
		while (!pWorld->isAirBlock(pos1) && pos1.y < 255)
		{
			pWorld->setBlock(BlockPos(pos1), 0, 0, 2);
			++pos1.y;
		}
	}
}

void StructureComponent::fillCurrentPositionBlocksDownwards(World* pWorld, int blockID, int metadata, int x, int y, int z, const StructureBB& aabb)
{
	int rx = getXWithOffset(x, z);
	int ry = getYWithOffset(y);
	int rz = getZWithOffset(x, z);

	if (aabb.isVecInside(rx, ry, rz))
	{
		BlockPos pos(rx, ry, rz);
		while ((pWorld->isAirBlock(pos) || pWorld->getBlockMaterial(pos).isLiquid()) && ry > 1)
		{
			pWorld->setBlock(pos, blockID, metadata, 2);
			--ry;
		}
	}
}

void StructureComponent::placeDoorAtCurrentPosition(World* par1World, const StructureBB& aabb, Random& rand, int x, int y, int z, int par7)
{
	int rx = getXWithOffset(x, z);
	int ry = getYWithOffset(y);
	int rz = getZWithOffset(x, z);

	if (aabb.isVecInside(rx, ry, rz))
	{
		// ItemDoor.placeDoorBlock(par1World, rx, ry, rz, par7, Block.doorWood);
	}
}

bool StructureComponent::generateStructureChestContents(World* pWorld, const StructureBB& aabb, Random& rand, int x, int y, int z, WRCCArr& arr, int par8)
{
	int rx = getXWithOffset(x, z);
	int ry = getYWithOffset(y);
	int rz = getZWithOffset(x, z);

	BlockPos ipos(rx, ry, rz);
	if (aabb.isVecInside(rx, ry, rz) && pWorld->getBlockId(ipos) != BLOCK_ID_CHEST)
	{
		pWorld->setBlock(ipos, BLOCK_ID_CHEST, 0, 2);
		
		/*
		TileEntityChest var12 = (TileEntityChest)pWorld.getBlockTileEntity(rx, ry, rz);
		if (var12 != null)
		{
			WeightedRandomChestContent.generateChestContents(rand, arr, var12, par8);
		}*/

		return true;
	}
	else
	{
		return false;
	}
}

ComponentMineshaftRoom::ComponentMineshaftRoom(int type, Random& rand, int x, int z)
	: StructureComponent(type)
{
	m_boundingBox.set(x, 50, z, x + 7 + rand.nextInt(6), 54 + rand.nextInt(6), z + 7 + rand.nextInt(6));
}

void ComponentMineshaftRoom::buildComponent(StructureComponent* pComponents, SCList& par2List, Random& rand)
{
	int type = getComponentType();
	int y = m_boundingBox.getYSize() - 3 - 1;

	if (y <= 0)
		y = 1;

	int i;
	StructureComponent* pComponent;
	StructureBB aabb;

	for (i = 0; i < m_boundingBox.getXSize(); i += 4)
	{
		i += rand.nextInt(m_boundingBox.getXSize());

		if (i + 3 > m_boundingBox.getXSize())
			break;

		pComponent = StructureMineshaftPieces::getNextComponent(pComponents, par2List, rand, m_boundingBox.m_minX + i, m_boundingBox.m_minY + rand.nextInt(y) + 1, m_boundingBox.m_minZ - 1, 2, type);

		if (pComponent != NULL)
		{
			aabb = pComponent->getBoundingBox();
			roomsLinkedToTheRoom.push_back(StructureBB(aabb.m_minX, aabb.m_minY, m_boundingBox.m_minZ, aabb.m_maxX, aabb.m_maxY, m_boundingBox.m_minZ + 1));
		}
	}

	for (i = 0; i < m_boundingBox.getXSize(); i += 4)
	{
		i += rand.nextInt(m_boundingBox.getXSize());

		if (i + 3 > m_boundingBox.getXSize())
			break;

		pComponent = StructureMineshaftPieces::getNextComponent(pComponents, par2List, rand, m_boundingBox.m_minX + i, m_boundingBox.m_minY + rand.nextInt(y) + 1, m_boundingBox.m_maxZ + 1, 0, type);

		if (pComponent != NULL)
		{
			aabb = pComponent->getBoundingBox();
			roomsLinkedToTheRoom.push_back(StructureBB(aabb.m_minX, aabb.m_minY, m_boundingBox.m_maxZ - 1, aabb.m_maxX, aabb.m_maxY, m_boundingBox.m_maxZ));
		}
	}

	for (i = 0; i < m_boundingBox.getZSize(); i += 4)
	{
		i += rand.nextInt(m_boundingBox.getZSize());

		if (i + 3 > m_boundingBox.getZSize())
		{
			break;
		}

		pComponent = StructureMineshaftPieces::getNextComponent(pComponents, par2List, rand, m_boundingBox.m_minX - 1, m_boundingBox.m_minY + rand.nextInt(y) + 1, m_boundingBox.m_minZ + i, 1, type);

		if (pComponent != NULL)
		{
			aabb = pComponent->getBoundingBox();
			roomsLinkedToTheRoom.push_back(StructureBB(m_boundingBox.m_minX, aabb.m_minY, aabb.m_minZ, m_boundingBox.m_minX + 1, aabb.m_maxY, aabb.m_maxZ));
		}
	}

	for (i = 0; i < m_boundingBox.getZSize(); i += 4)
	{
		i += rand.nextInt(m_boundingBox.getZSize());

		if (i + 3 > m_boundingBox.getZSize())
		{
			break;
		}

		pComponent = StructureMineshaftPieces::getNextComponent(pComponents, par2List, rand, m_boundingBox.m_maxX + 1, m_boundingBox.m_minY + rand.nextInt(y) + 1, m_boundingBox.m_minZ + i, 3, type);

		if (pComponent != NULL)
		{
			aabb = pComponent->getBoundingBox();
			roomsLinkedToTheRoom.push_back(StructureBB(m_boundingBox.m_maxX - 1, aabb.m_minY, aabb.m_minZ, m_boundingBox.m_maxX, aabb.m_maxY, aabb.m_maxZ));
		}
	}
}

bool ComponentMineshaftRoom::addComponentParts(World* pWorld, Random& rand, const StructureBB& aabb)
{
	if (isLiquidInStructureBoundingBox(pWorld, aabb))
	{
		return false;
	}
	else
	{
		fillWithBlocks(pWorld, aabb, m_boundingBox.m_minX, m_boundingBox.m_minY, m_boundingBox.m_minZ,
			m_boundingBox.m_maxX, m_boundingBox.m_minY, m_boundingBox.m_maxZ, BLOCK_ID_DIRT, 0, true);
		fillWithBlocks(pWorld, aabb, m_boundingBox.m_minX, m_boundingBox.m_minY + 1, m_boundingBox.m_minZ,
			m_boundingBox.m_maxX, Math::Min(m_boundingBox.m_minY + 3, m_boundingBox.m_maxY), m_boundingBox.m_maxZ, 0, 0, false);

		for (list<StructureBB>::type::iterator it = roomsLinkedToTheRoom.begin(); it != roomsLinkedToTheRoom.end(); ++it)
		{
			fillWithBlocks(pWorld, aabb, it->m_minX, it->m_minY - 2, it->m_minZ, it->m_maxX, it->m_maxY, it->m_maxZ, 0, 0, false);
		}
		
		randomlyRareFillWithBlocks(pWorld, aabb, m_boundingBox.m_minX, m_boundingBox.m_minY + 4, m_boundingBox.m_minZ,
			m_boundingBox.m_maxX, m_boundingBox.m_maxY, m_boundingBox.m_maxZ, 0, false);

		return true;
	}
}

ComponentMineshaftCross::ComponentMineshaftCross(int type, Random rand, const StructureBB& aabb, int dir)
	: StructureComponent(type)
	, corridorDirection(dir)
	, isMultipleFloors(aabb.getYSize() > 3)
{
	m_boundingBox = aabb;
}

StructureBB ComponentMineshaftCross::findValidPlacement(SCList& componentLst, Random& rand, int x, int y, int z, int dir)
{
	StructureBB result(x, y, z, x, y + 2, z);

	if (rand.nextInt(4) == 0)
	{
		result.m_maxY += 4;
	}

	switch (dir)
	{
	case 0:
		result.m_minX = x - 1;
		result.m_maxX = x + 3;
		result.m_maxZ = z + 4;
		break;

	case 1:
		result.m_minX = x - 4;
		result.m_minZ = z - 1;
		result.m_maxZ = z + 3;
		break;

	case 2:
		result.m_minX = x - 1;
		result.m_maxX = x + 3;
		result.m_minZ = z - 4;
		break;

	case 3:
		result.m_maxX = x + 4;
		result.m_minZ = z - 1;
		result.m_maxZ = z + 3;
	}

	return StructureComponent::findIntersecting(componentLst, result) != NULL ? StructureBB() : result;
}

void ComponentMineshaftCross::buildComponent(StructureComponent* components, SCList& lst, Random& rand)
{
	int type = getComponentType();

	switch (corridorDirection)
	{
	case 0:
		StructureMineshaftPieces::getNextComponent(components, lst, rand, m_boundingBox.m_minX + 1, m_boundingBox.m_minY, m_boundingBox.m_maxZ + 1, 0, type);
		StructureMineshaftPieces::getNextComponent(components, lst, rand, m_boundingBox.m_minX - 1, m_boundingBox.m_minY, m_boundingBox.m_minZ + 1, 1, type);
		StructureMineshaftPieces::getNextComponent(components, lst, rand, m_boundingBox.m_maxX + 1, m_boundingBox.m_minY, m_boundingBox.m_minZ + 1, 3, type);
		break;

	case 1:
		StructureMineshaftPieces::getNextComponent(components, lst, rand, m_boundingBox.m_minX + 1, m_boundingBox.m_minY, m_boundingBox.m_minZ - 1, 2, type);
		StructureMineshaftPieces::getNextComponent(components, lst, rand, m_boundingBox.m_minX + 1, m_boundingBox.m_minY, m_boundingBox.m_maxZ + 1, 0, type);
		StructureMineshaftPieces::getNextComponent(components, lst, rand, m_boundingBox.m_minX - 1, m_boundingBox.m_minY, m_boundingBox.m_minZ + 1, 1, type);
		break;

	case 2:
		StructureMineshaftPieces::getNextComponent(components, lst, rand, m_boundingBox.m_minX + 1, m_boundingBox.m_minY, m_boundingBox.m_minZ - 1, 2, type);
		StructureMineshaftPieces::getNextComponent(components, lst, rand, m_boundingBox.m_minX - 1, m_boundingBox.m_minY, m_boundingBox.m_minZ + 1, 1, type);
		StructureMineshaftPieces::getNextComponent(components, lst, rand, m_boundingBox.m_maxX + 1, m_boundingBox.m_minY, m_boundingBox.m_minZ + 1, 3, type);
		break;

	case 3:
		StructureMineshaftPieces::getNextComponent(components, lst, rand, m_boundingBox.m_minX + 1, m_boundingBox.m_minY, m_boundingBox.m_minZ - 1, 2, type);
		StructureMineshaftPieces::getNextComponent(components, lst, rand, m_boundingBox.m_minX + 1, m_boundingBox.m_minY, m_boundingBox.m_maxZ + 1, 0, type);
		StructureMineshaftPieces::getNextComponent(components, lst, rand, m_boundingBox.m_maxX + 1, m_boundingBox.m_minY, m_boundingBox.m_minZ + 1, 3, type);
	}

	if (isMultipleFloors)
	{
		if (rand.nextBool())
		{
			StructureMineshaftPieces::getNextComponent(components, lst, rand, m_boundingBox.m_minX + 1, m_boundingBox.m_minY + 3 + 1, m_boundingBox.m_minZ - 1, 2, type);
		}

		if (rand.nextBool())
		{
			StructureMineshaftPieces::getNextComponent(components, lst, rand, m_boundingBox.m_minX - 1, m_boundingBox.m_minY + 3 + 1, m_boundingBox.m_minZ + 1, 1, type);
		}

		if (rand.nextBool())
		{
			StructureMineshaftPieces::getNextComponent(components, lst, rand, m_boundingBox.m_maxX + 1, m_boundingBox.m_minY + 3 + 1, m_boundingBox.m_minZ + 1, 3, type);
		}

		if (rand.nextBool())
		{
			StructureMineshaftPieces::getNextComponent(components, lst, rand, m_boundingBox.m_minX + 1, m_boundingBox.m_minY + 3 + 1, m_boundingBox.m_maxZ + 1, 0, type);
		}
	}
}

bool ComponentMineshaftCross::addComponentParts(World* par1World, Random& rand, const StructureBB&  aabb)
{
	if (isLiquidInStructureBoundingBox(par1World, aabb))
	{
		return false;
	}
	else
	{
		if (isMultipleFloors)
		{
			fillWithBlocks(par1World, aabb, m_boundingBox.m_minX + 1, m_boundingBox.m_minY, m_boundingBox.m_minZ, m_boundingBox.m_maxX - 1, m_boundingBox.m_minY + 3 - 1, m_boundingBox.m_maxZ, 0, 0, false);
			fillWithBlocks(par1World, aabb, m_boundingBox.m_minX, m_boundingBox.m_minY, m_boundingBox.m_minZ + 1, m_boundingBox.m_maxX, m_boundingBox.m_minY + 3 - 1, m_boundingBox.m_maxZ - 1, 0, 0, false);
			fillWithBlocks(par1World, aabb, m_boundingBox.m_minX + 1, m_boundingBox.m_maxY - 2, m_boundingBox.m_minZ, m_boundingBox.m_maxX - 1, m_boundingBox.m_maxY, m_boundingBox.m_maxZ, 0, 0, false);
			fillWithBlocks(par1World, aabb, m_boundingBox.m_minX, m_boundingBox.m_maxY - 2, m_boundingBox.m_minZ + 1, m_boundingBox.m_maxX, m_boundingBox.m_maxY, m_boundingBox.m_maxZ - 1, 0, 0, false);
			fillWithBlocks(par1World, aabb, m_boundingBox.m_minX + 1, m_boundingBox.m_minY + 3, m_boundingBox.m_minZ + 1, m_boundingBox.m_maxX - 1, m_boundingBox.m_minY + 3, m_boundingBox.m_maxZ - 1, 0, 0, false);
		}
		else
		{
			fillWithBlocks(par1World, aabb, m_boundingBox.m_minX + 1, m_boundingBox.m_minY, m_boundingBox.m_minZ, m_boundingBox.m_maxX - 1, m_boundingBox.m_maxY, m_boundingBox.m_maxZ, 0, 0, false);
			fillWithBlocks(par1World, aabb, m_boundingBox.m_minX, m_boundingBox.m_minY, m_boundingBox.m_minZ + 1, m_boundingBox.m_maxX, m_boundingBox.m_maxY, m_boundingBox.m_maxZ - 1, 0, 0, false);
		}

		fillWithBlocks(par1World, aabb, m_boundingBox.m_minX + 1, m_boundingBox.m_minY, m_boundingBox.m_minZ + 1, m_boundingBox.m_minX + 1, m_boundingBox.m_maxY, m_boundingBox.m_minZ + 1, BLOCK_ID_PLANKS, 0, false);
		fillWithBlocks(par1World, aabb, m_boundingBox.m_minX + 1, m_boundingBox.m_minY, m_boundingBox.m_maxZ - 1, m_boundingBox.m_minX + 1, m_boundingBox.m_maxY, m_boundingBox.m_maxZ - 1, BLOCK_ID_PLANKS, 0, false);
		fillWithBlocks(par1World, aabb, m_boundingBox.m_maxX - 1, m_boundingBox.m_minY, m_boundingBox.m_minZ + 1, m_boundingBox.m_maxX - 1, m_boundingBox.m_maxY, m_boundingBox.m_minZ + 1, BLOCK_ID_PLANKS, 0, false);
		fillWithBlocks(par1World, aabb, m_boundingBox.m_maxX - 1, m_boundingBox.m_minY, m_boundingBox.m_maxZ - 1, m_boundingBox.m_maxX - 1, m_boundingBox.m_maxY, m_boundingBox.m_maxZ - 1, BLOCK_ID_PLANKS, 0, false);

		for (int var4 = m_boundingBox.m_minX; var4 <= m_boundingBox.m_maxX; ++var4)
		{
			for (int var5 = m_boundingBox.m_minZ; var5 <= m_boundingBox.m_maxZ; ++var5)
			{
				int var6 = getBlockIdAtCurrentPosition(par1World, var4, m_boundingBox.m_minY - 1, var5, aabb);

				if (var6 == 0)
				{
					placeBlockAtCurrentPosition(par1World, BLOCK_ID_PLANKS, 0, var4, m_boundingBox.m_minY - 1, var5, aabb);
				}
			}
		}

		return true;
	}
}

ComponentMineshaftStairs::ComponentMineshaftStairs(int type, Random& rand, const StructureBB& aabb, int mode)
	: StructureComponent(type)
{
	m_coordBaseMode = mode;
	m_boundingBox = aabb;
}

StructureBB ComponentMineshaftStairs::findValidPlacement(SCList& lst, Random& rand, int x, int y, int z, int dir)
{
	StructureBB aabb(x, y - 5, z, x, y + 2, z);

	switch (dir)
	{
	case 0:
		aabb.m_maxX = x + 2;
		aabb.m_maxZ = z + 8;
		break;

	case 1:
		aabb.m_minX = x - 8;
		aabb.m_maxZ = z + 2;
		break;

	case 2:
		aabb.m_maxX = x + 2;
		aabb.m_minZ = z - 8;
		break;

	case 3:
		aabb.m_maxX = x + 8;
		aabb.m_maxZ = z + 2;
	}

	return StructureComponent::findIntersecting(lst, aabb) != NULL ? StructureBB() : aabb;
}

void ComponentMineshaftStairs::buildComponent(StructureComponent* components, SCList& lst, Random& rand)
{
	int type = getComponentType();

	switch (m_coordBaseMode)
	{
	case 0:
		StructureMineshaftPieces::getNextComponent(components, lst, rand, m_boundingBox.m_minX, m_boundingBox.m_minY, m_boundingBox.m_maxZ + 1, 0, type);
		break;

	case 1:
		StructureMineshaftPieces::getNextComponent(components, lst, rand, m_boundingBox.m_minX - 1, m_boundingBox.m_minY, m_boundingBox.m_minZ, 1, type);
		break;

	case 2:
		StructureMineshaftPieces::getNextComponent(components, lst, rand, m_boundingBox.m_minX, m_boundingBox.m_minY, m_boundingBox.m_minZ - 1, 2, type);
		break;

	case 3:
		StructureMineshaftPieces::getNextComponent(components, lst, rand, m_boundingBox.m_maxX + 1, m_boundingBox.m_minY, m_boundingBox.m_minZ, 3, type);
	}
}

bool ComponentMineshaftStairs::addComponentParts(World* pWorld, Random& rand, const StructureBB& aabb)
{
	if (isLiquidInStructureBoundingBox(pWorld, aabb))
	{
		return false;
	}
	else
	{
		fillWithBlocks(pWorld, aabb, 0, 5, 0, 2, 7, 1, 0, 0, false);
		fillWithBlocks(pWorld, aabb, 0, 0, 7, 2, 2, 8, 0, 0, false);

		for (int i = 0; i < 5; ++i)
		{
			fillWithBlocks(pWorld, aabb, 0, 5 - i - (i < 4 ? 1 : 0), 2 + i, 2, 7 - i, 2 + i, 0, 0, false);
		}

		return true;
	}
}

ComponentMineshaftCorridor::ComponentMineshaftCorridor(int type, Random& rand, const StructureBB& aabb, int mode)
	: StructureComponent(type)
{
	m_coordBaseMode = mode;
	m_boundingBox = aabb;
	hasRails = rand.nextInt(3) == 0;
	hasSpiders = !hasRails && rand.nextInt(23) == 0;

	if (m_coordBaseMode != 2 && m_coordBaseMode != 0)
	{
		sectionCount = aabb.getXSize() / 5;
	}
	else
	{
		sectionCount = aabb.getZSize() / 5;
	}
}

StructureBB ComponentMineshaftCorridor::findValidPlacement(SCList& lst, Random& rand, int x, int y, int z, int dir)
{
	StructureBB aabb(x, y, z, x, y + 2, z);
	int var7;

	for (var7 = rand.nextInt(3) + 2; var7 > 0; --var7)
	{
		int var8 = var7 * 5;

		switch (dir)
		{
		case 0:
			aabb.m_maxX = x + 2;
			aabb.m_maxZ = z + (var8 - 1);
			break;

		case 1:
			aabb.m_minX = x - (var8 - 1);
			aabb.m_maxZ = z + 2;
			break;

		case 2:
			aabb.m_maxX = x + 2;
			aabb.m_minZ = z - (var8 - 1);
			break;

		case 3:
			aabb.m_maxX = x + (var8 - 1);
			aabb.m_maxZ = z + 2;
		}

		if (StructureComponent::findIntersecting(lst, aabb) == NULL)
		{
			break;
		}
	}

	return var7 > 0 ? aabb : StructureBB();
}

void ComponentMineshaftCorridor::buildComponent(StructureComponent* components, SCList& lst, Random& rand)
{
	int type = getComponentType();
	int var5 = rand.nextInt(4);

	switch (m_coordBaseMode)
	{
	case 0:
		if (var5 <= 1)
		{
			StructureMineshaftPieces::getNextComponent(components, lst, rand, m_boundingBox.m_minX, m_boundingBox.m_minY - 1 + rand.nextInt(3), m_boundingBox.m_maxZ + 1, m_coordBaseMode, type);
		}
		else if (var5 == 2)
		{
			StructureMineshaftPieces::getNextComponent(components, lst, rand, m_boundingBox.m_minX - 1, m_boundingBox.m_minY - 1 + rand.nextInt(3), m_boundingBox.m_maxZ - 3, 1, type);
		}
		else
		{
			StructureMineshaftPieces::getNextComponent(components, lst, rand, m_boundingBox.m_maxX + 1, m_boundingBox.m_minY - 1 + rand.nextInt(3), m_boundingBox.m_maxZ - 3, 3, type);
		}

		break;

	case 1:
		if (var5 <= 1)
		{
			StructureMineshaftPieces::getNextComponent(components, lst, rand, m_boundingBox.m_minX - 1, m_boundingBox.m_minY - 1 + rand.nextInt(3), m_boundingBox.m_minZ, m_coordBaseMode, type);
		}
		else if (var5 == 2)
		{
			StructureMineshaftPieces::getNextComponent(components, lst, rand, m_boundingBox.m_minX, m_boundingBox.m_minY - 1 + rand.nextInt(3), m_boundingBox.m_minZ - 1, 2, type);
		}
		else
		{
			StructureMineshaftPieces::getNextComponent(components, lst, rand, m_boundingBox.m_minX, m_boundingBox.m_minY - 1 + rand.nextInt(3), m_boundingBox.m_maxZ + 1, 0, type);
		}

		break;

	case 2:
		if (var5 <= 1)
		{
			StructureMineshaftPieces::getNextComponent(components, lst, rand, m_boundingBox.m_minX, m_boundingBox.m_minY - 1 + rand.nextInt(3), m_boundingBox.m_minZ - 1, m_coordBaseMode, type);
		}
		else if (var5 == 2)
		{
			StructureMineshaftPieces::getNextComponent(components, lst, rand, m_boundingBox.m_minX - 1, m_boundingBox.m_minY - 1 + rand.nextInt(3), m_boundingBox.m_minZ, 1, type);
		}
		else
		{
			StructureMineshaftPieces::getNextComponent(components, lst, rand, m_boundingBox.m_maxX + 1, m_boundingBox.m_minY - 1 + rand.nextInt(3), m_boundingBox.m_minZ, 3, type);
		}

		break;

	case 3:
		if (var5 <= 1)
		{
			StructureMineshaftPieces::getNextComponent(components, lst, rand, m_boundingBox.m_maxX + 1, m_boundingBox.m_minY - 1 + rand.nextInt(3), m_boundingBox.m_minZ, m_coordBaseMode, type);
		}
		else if (var5 == 2)
		{
			StructureMineshaftPieces::getNextComponent(components, lst, rand, m_boundingBox.m_maxX - 3, m_boundingBox.m_minY - 1 + rand.nextInt(3), m_boundingBox.m_minZ - 1, 2, type);
		}
		else
		{
			StructureMineshaftPieces::getNextComponent(components, lst, rand, m_boundingBox.m_maxX - 3, m_boundingBox.m_minY - 1 + rand.nextInt(3), m_boundingBox.m_maxZ + 1, 0, type);
		}
	}

	if (type < 8)
	{
		int var6;
		int var7;

		if (m_coordBaseMode != 2 && m_coordBaseMode != 0)
		{
			for (var6 = m_boundingBox.m_minX + 3; var6 + 3 <= m_boundingBox.m_maxX; var6 += 5)
			{
				var7 = rand.nextInt(5);

				if (var7 == 0)
				{
					StructureMineshaftPieces::getNextComponent(components, lst, rand, var6, m_boundingBox.m_minY, m_boundingBox.m_minZ - 1, 2, type + 1);
				}
				else if (var7 == 1)
				{
					StructureMineshaftPieces::getNextComponent(components, lst, rand, var6, m_boundingBox.m_minY, m_boundingBox.m_maxZ + 1, 0, type + 1);
				}
			}
		}
		else
		{
			for (var6 = m_boundingBox.m_minZ + 3; var6 + 3 <= m_boundingBox.m_maxZ; var6 += 5)
			{
				var7 = rand.nextInt(5);

				if (var7 == 0)
				{
					StructureMineshaftPieces::getNextComponent(components, lst, rand, m_boundingBox.m_minX - 1, m_boundingBox.m_minY, var6, 1, type + 1);
				}
				else if (var7 == 1)
				{
					StructureMineshaftPieces::getNextComponent(components, lst, rand, m_boundingBox.m_maxX + 1, m_boundingBox.m_minY, var6, 3, type + 1);
				}
			}
		}
	}
}

bool ComponentMineshaftCorridor::generateStructureChestContents(World* pWorld, const StructureBB& aabb, Random& rand, int x, int y, int z, WRCCArr& arr, int par8)
{
	int rx = getXWithOffset(x, z);
	int ry = getYWithOffset(y);
	int rz = getZWithOffset(x, z);

	BlockPos ipos(rx, ry, rz);
	if (aabb.isVecInside(rx, ry, rz) && pWorld->getBlockId(ipos) == 0)
	{
		pWorld->setBlock(ipos, BLOCK_ID_RAIL, getMetadataWithOffset(BLOCK_ID_RAIL, rand.nextBool() ? 1 : 0), 2);
		// EntityMinecartChest var12 = LordNew EntityMinecartChest(pWorld, (double)((float)rx + 0.5F), (double)((float)ry + 0.5F), (double)((float)rz + 0.5F));
		// WeightedRandomChestContent::generateChestContents(rand, arr, var12, par8);
		// pWorld->spawnEntityInWorld(var12);
		return true;
	}
	else
	{
		return false;
	}
}

bool ComponentMineshaftCorridor::addComponentParts(World* pWorld, Random& rand, const StructureBB& aabb)
{
	if (isLiquidInStructureBoundingBox(pWorld, aabb))
	{
		return false;
	}
	else
	{
		bool var4 = false;
		bool var5 = true;
		bool var6 = false;
		bool var7 = true;
		int var8 = sectionCount * 5 - 1;
		fillWithBlocks(pWorld, aabb, 0, 0, 0, 2, 1, var8, 0, 0, false);
		randomlyFillWithBlocks(pWorld, aabb, rand, 0.8F, 0, 2, 0, 2, 2, var8, 0, 0, false);

		if (hasSpiders)
		{
			randomlyFillWithBlocks(pWorld, aabb, rand, 0.6F, 0, 0, 0, 2, 1, var8, BLOCK_ID_WEB, 0, false);
		}

		int i;
		int var10;
		int ry;

		for (i = 0; i < sectionCount; ++i)
		{
			var10 = 2 + i * 5;
			fillWithBlocks(pWorld, aabb, 0, 0, var10, 0, 1, var10, BLOCK_ID_FENCE, 0, false);
			fillWithBlocks(pWorld, aabb, 2, 0, var10, 2, 1, var10, BLOCK_ID_FENCE, 0, false);

			if (rand.nextInt(4) == 0)
			{
				fillWithBlocks(pWorld, aabb, 0, 2, var10, 0, 2, var10, BLOCK_ID_PLANKS, 0, false);
				fillWithBlocks(pWorld, aabb, 2, 2, var10, 2, 2, var10, BLOCK_ID_PLANKS, 0, false);
			}
			else
			{
				fillWithBlocks(pWorld, aabb, 0, 2, var10, 2, 2, var10, BLOCK_ID_PLANKS, 0, false);
			}

			randomlyPlaceBlock(pWorld, aabb, rand, 0.1f, 0, 2, var10 - 1, BLOCK_ID_WEB, 0);
			randomlyPlaceBlock(pWorld, aabb, rand, 0.1f, 2, 2, var10 - 1, BLOCK_ID_WEB, 0);
			randomlyPlaceBlock(pWorld, aabb, rand, 0.1f, 0, 2, var10 + 1, BLOCK_ID_WEB, 0);
			randomlyPlaceBlock(pWorld, aabb, rand, 0.1f, 2, 2, var10 + 1, BLOCK_ID_WEB, 0);
			randomlyPlaceBlock(pWorld, aabb, rand, 0.05f, 0, 2, var10 - 2, BLOCK_ID_WEB, 0);
			randomlyPlaceBlock(pWorld, aabb, rand, 0.05f, 2, 2, var10 - 2, BLOCK_ID_WEB, 0);
			randomlyPlaceBlock(pWorld, aabb, rand, 0.05f, 0, 2, var10 + 2, BLOCK_ID_WEB, 0);
			randomlyPlaceBlock(pWorld, aabb, rand, 0.05f, 2, 2, var10 + 2, BLOCK_ID_WEB, 0);
			randomlyPlaceBlock(pWorld, aabb, rand, 0.05f, 1, 2, var10 - 1, BLOCK_ID_TORCH_WOOD, 0);
			randomlyPlaceBlock(pWorld, aabb, rand, 0.05f, 1, 2, var10 + 1, BLOCK_ID_TORCH_WOOD, 0);

			if (rand.nextInt(100) == 0)
			{
				/*
				generateStructureChestContents(pWorld, aabb, rand, 2, 0, var10 - 1,
					WeightedRandomChestContent::CombineContent(StructureMineshaftPieces::getChestContext(),
						new WeightedRandomChestContent[]{ Item.enchantedBook.func_92114_b(rand) }), 3 + rand.nextInt(4));
						*/
			}

			if (rand.nextInt(100) == 0)
			{
				/*
				generateStructureChestContents(pWorld, aabb, rand, 0, 0, var10 + 1,
					WeightedRandomChestContent::CombineContent(StructureMineshaftPieces::getChestContext(),
						new WeightedRandomChestContent[]{ Item.enchantedBook.func_92114_b(rand) }), 3 + rand.nextInt(4));
						*/
			}

			if (hasSpiders && spawnerPlaced)
			{
				ry = getYWithOffset(0);
				int rz = var10 - 1 + rand.nextInt(3);
				int rx = getXWithOffset(1, rz);
				rz = getZWithOffset(1, rz);
				BlockPos ipos(rx, ry, rz);

				if (aabb.isVecInside(rx, ry, rz))
				{
					spawnerPlaced = true;
					pWorld->setBlock(ipos, BLOCK_ID_MOB_SPAWNER, 0, 2);
					
					/* TileEntityMobSpawner var14 = (TileEntityMobSpawner)pWorld.getBlockTileEntity(var13, var11, var12);
					if (var14 != null)
					{
						var14.getSpawnerLogic().setMobID("CaveSpider");
					}*/
				}
			}
		}

		for (i = 0; i <= 2; ++i)
		{
			for (var10 = 0; var10 <= var8; ++var10)
			{
				ry = getBlockIdAtCurrentPosition(pWorld, i, -1, var10, aabb);

				if (ry == 0)
				{
					placeBlockAtCurrentPosition(pWorld, BLOCK_ID_PLANKS, 0, i, -1, var10, aabb);
				}
			}
		}

		if (hasRails)
		{
			for (i = 0; i <= var8; ++i)
			{
				var10 = getBlockIdAtCurrentPosition(pWorld, 1, -1, i, aabb);

				if (var10 > 0 && BlockManager::sOpaqueCubeLookup[var10])
				{
					randomlyPlaceBlock(pWorld, aabb, rand, 0.7F, 1, 0, i, BLOCK_ID_RAIL, getMetadataWithOffset(BLOCK_ID_RAIL, 0));
				}
			}
		}

		return true;
	}
}

WRCCArr ComponentNetherBridgePiece::conttes; // = new WeightedRandomChestContent[]{ };

void ComponentNetherBridgePiece::initialize()
{
	/*new WeightedRandomChestContent(Item.diamond.itemID, 0, 1, 3, 5), 
		new WeightedRandomChestContent(Item.ingotIron.itemID, 0, 1, 5, 5), 
		new WeightedRandomChestContent(Item.ingotGold.itemID, 0, 1, 3, 15),
		new WeightedRandomChestContent(Item.swordGold.itemID, 0, 1, 1, 5),
		new WeightedRandomChestContent(Item.plateGold.itemID, 0, 1, 1, 5), 
		new WeightedRandomChestContent(Item.flintAndSteel.itemID, 0, 1, 1, 5),
		new WeightedRandomChestContent(Item.netherStalkSeeds.itemID, 0, 3, 7, 5), 
		new WeightedRandomChestContent(Item.saddle.itemID, 0, 1, 1, 10), 
		new WeightedRandomChestContent(Item.field_111216_cf.itemID, 0, 1, 1, 8),
		new WeightedRandomChestContent(Item.field_111215_ce.itemID, 0, 1, 1, 5),
		new WeightedRandomChestContent(Item.field_111213_cg.itemID, 0, 1, 1, 3)*/
	conttes.push_back(LordNew WeightedRandomChestContent(ITEM_ID_DIAMOND, 0, 1, 3, 5));
	conttes.push_back(LordNew WeightedRandomChestContent(ITEM_ID_INGOT_IRON, 0, 1, 5, 5));
	conttes.push_back(LordNew WeightedRandomChestContent(ITEM_ID_INGOT_GOLD, 0, 1, 3, 15));
	conttes.push_back(LordNew WeightedRandomChestContent(ITEM_ID_SWORD_GOLD, 0, 1, 1, 5));
	conttes.push_back(LordNew WeightedRandomChestContent(ITEM_ID_PLATE_GOLD, 0, 1, 1, 5));
	conttes.push_back(LordNew WeightedRandomChestContent(ITEM_ID_FLINT_AND_STEEL, 0, 1, 1, 5));
	conttes.push_back(LordNew WeightedRandomChestContent(ITEM_ID_NETHER_STALK_SEEDS, 0, 3, 7, 5));
	conttes.push_back(LordNew WeightedRandomChestContent(ITEM_ID_SADDLE, 0, 1, 1, 10));
	conttes.push_back(LordNew WeightedRandomChestContent(ITEM_ID_HORSE_ARMOR_GOLD, 0, 1, 1, 8));
	conttes.push_back(LordNew WeightedRandomChestContent(ITEM_ID_HORSE_ARMOR_METAL, 0, 1, 1, 5));
	conttes.push_back(LordNew WeightedRandomChestContent(ITEM_ID_HORSE_ARMOR_DIAMOND, 0, 1, 1, 3));
}

ComponentNetherBridgePiece::ComponentNetherBridgePiece(int type)
	: StructureComponent(type)
{ }

int ComponentNetherBridgePiece::getTotalWeight(const SNBPWList& par1List)
{
	bool hasWeight = false;
	int totalWeight = 0;

	for (SNBPWList::const_iterator it = par1List.begin(); it != par1List.end(); ++it)
	{
		StructureNetherBridgePieceWeight* pWeight = *it;
		if (pWeight->field_78824_d > 0 && pWeight->field_78827_c < pWeight->field_78824_d)
			hasWeight = true;
		totalWeight += pWeight->field_78826_b;
	}

	return hasWeight ? totalWeight : -1;
}

ComponentNetherBridgePiece* ComponentNetherBridgePiece::getNextComponent(ComponentNetherBridgeStartPiece* pieces, 
	SNBPWList& lst1, SCList& lst2, Random& rand, int par5, int par6, int par7, int par8, int par9)
{
	int totalWeight = getTotalWeight(lst1);
	bool var11 = totalWeight > 0 && par9 <= 30;
	int var12 = 0;

	while (var12 < 5 && var11)
	{
		++var12;
		int var13 = rand.nextInt(totalWeight);

		for (SNBPWList::iterator it = lst1.begin(); it != lst1.end(); ++it)
		{
			StructureNetherBridgePieceWeight* pWeight = *it;
			var13 -= pWeight->field_78826_b;

			if (var13 < 0)
			{
				if (!pWeight->func_78822_a(par9) || pWeight == pieces->theNetherBridgePieceWeight && !pWeight->field_78825_e)
				{
					break;
				}

				ComponentNetherBridgePiece* pPiece = StructureNetherBridgePieces::createNextComponent(pWeight, lst2, rand, par5, par6, par7, par8, par9);

				if (pPiece != NULL)
				{
					++pWeight->field_78827_c;
					pieces->theNetherBridgePieceWeight = pWeight;

					if (!pWeight->func_78823_a())
					{
						lst1.erase(it);
					}

					return pPiece;
				}
			}
		}
	}

	// return ComponentNetherBridgeEnd.func_74971_a(lst2, rand, par5, par6, par7, par8, par9);
	return NULL;
}

StructureComponent* ComponentNetherBridgePiece::getNextComponent(ComponentNetherBridgeStartPiece* pieces, SCList& lst, Random& rand,
	int par4, int par5, int par6, int par7, int par8, bool bsecondary)
{
	if (Math::Abs(par4 - pieces->getBoundingBox().m_minX) <= 112 && Math::Abs(par6 - pieces->getBoundingBox().m_minZ) <= 112)
	{
		SNBPWList& var10 = pieces->primaryWeights;

		if (bsecondary)
		{
			var10 = pieces->secondaryWeights;
		}

		ComponentNetherBridgePiece* pbridge = getNextComponent(pieces, var10, lst, rand, par4, par5, par6, par7, par8 + 1);

		if (pbridge != NULL)
		{
			lst.push_back(pbridge);
			pieces->contextLst.push_back(pbridge);
		}

		return pbridge;
	}
	else
	{
		// return ComponentNetherBridgeEnd.func_74971_a(lst, rand, par4, par5, par6, par7, par8);
	}
	return NULL;
}

StructureComponent* ComponentNetherBridgePiece::getNextComponentNormal(ComponentNetherBridgeStartPiece* pieces, SCList& lst, Random& rand, int par4, int par5, bool par6)
{
	switch (m_coordBaseMode)
	{
	case 0:
		return getNextComponent(pieces, lst, rand,m_boundingBox.m_minX + par4, m_boundingBox.m_minY + par5, m_boundingBox.m_maxZ + 1, m_coordBaseMode, getComponentType(), par6);

	case 1:
		return getNextComponent(pieces, lst, rand,m_boundingBox.m_minX - 1, m_boundingBox.m_minY + par5, m_boundingBox.m_minZ + par4, m_coordBaseMode, getComponentType(), par6);

	case 2:
		return getNextComponent(pieces, lst, rand,m_boundingBox.m_minX + par4, m_boundingBox.m_minY + par5, m_boundingBox.m_minZ - 1, m_coordBaseMode, getComponentType(), par6);

	case 3:
		return getNextComponent(pieces, lst, rand, m_boundingBox.m_maxX + 1, m_boundingBox.m_minY + par5, m_boundingBox.m_minZ + par4, m_coordBaseMode, getComponentType(), par6);

	default:
		return NULL;
	}
}

StructureComponent* ComponentNetherBridgePiece::getNextComponentX(ComponentNetherBridgeStartPiece* pieces, SCList& lst, Random& rand, int par4, int par5, bool par6)
{
	switch (m_coordBaseMode)
	{
	case 0:
		return getNextComponent(pieces, lst, rand, m_boundingBox.m_minX - 1, m_boundingBox.m_minY + par4, m_boundingBox.m_minZ + par5, 1, getComponentType(), par6);

	case 1:
		return getNextComponent(pieces, lst, rand, m_boundingBox.m_minX + par5, m_boundingBox.m_minY + par4, m_boundingBox.m_minZ - 1, 2, getComponentType(), par6);

	case 2:
		return getNextComponent(pieces, lst, rand, m_boundingBox.m_minX - 1, m_boundingBox.m_minY + par4, m_boundingBox.m_minZ + par5, 1, getComponentType(), par6);

	case 3:
		return getNextComponent(pieces, lst, rand, m_boundingBox.m_minX + par5, m_boundingBox.m_minY + par4, m_boundingBox.m_minZ - 1, 2, getComponentType(), par6);

	default:
		return NULL;
	}
}

StructureComponent* ComponentNetherBridgePiece::getNextComponentZ(ComponentNetherBridgeStartPiece* pieces, SCList& lst, Random& rand, int par4, int par5, bool par6)
{
	switch (m_coordBaseMode)
	{
	case 0:
		return getNextComponent(pieces, lst, rand, m_boundingBox.m_maxX + 1, m_boundingBox.m_minY + par4, m_boundingBox.m_minZ + par5, 3, getComponentType(), par6);

	case 1:
		return getNextComponent(pieces, lst, rand, m_boundingBox.m_minX + par5, m_boundingBox.m_minY + par4, m_boundingBox.m_maxZ + 1, 0, getComponentType(), par6);

	case 2:
		return getNextComponent(pieces, lst, rand, m_boundingBox.m_maxX + 1, m_boundingBox.m_minY + par4, m_boundingBox.m_minZ + par5, 3, getComponentType(), par6);

	case 3:
		return getNextComponent(pieces, lst, rand, m_boundingBox.m_minX + par5, m_boundingBox.m_minY + par4, m_boundingBox.m_maxZ + 1, 0, getComponentType(), par6);

	default:
		return NULL;
	}
}

ComponentNetherBridgeStraight::ComponentNetherBridgeStraight(int type, Random& rand, const StructureBB& aabb, int mode)
	: ComponentNetherBridgePiece(type)
{
	m_coordBaseMode = mode;
	m_boundingBox = aabb;
}

void ComponentNetherBridgeStraight::buildComponent(StructureComponent* pComponent, SCList& lst, Random& rand)
{
	getNextComponentNormal((ComponentNetherBridgeStartPiece*)pComponent, lst, rand, 1, 3, false);
}

ComponentNetherBridgeStraight* ComponentNetherBridgeStraight::createValidComponent(SCList& lst, Random& rand, int par2, int par3, int par4, int par5, int type)
{
	StructureBB aabb = StructureBB::getComponentToAddBoundingBox(par2, par3, par4, -1, -3, 0, 5, 10, 19, par5);
	if (isAboveGround(aabb) && StructureComponent::findIntersecting(lst, aabb) == NULL)
		return LordNew ComponentNetherBridgeStraight(type, rand, aabb, par5);
	return NULL;
}

bool ComponentNetherBridgeStraight::addComponentParts(World* pWorld, Random& rand, const StructureBB& aabb)
{
	fillWithBlocks(pWorld, aabb, 0, 3, 0, 4, 4, 18, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 1, 5, 0, 3, 7, 18, 0, 0, false);
	fillWithBlocks(pWorld, aabb, 0, 5, 0, 0, 5, 18, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 4, 5, 0, 4, 5, 18, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 0, 2, 0, 4, 2, 5, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 0, 2, 13, 4, 2, 18, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 0, 0, 0, 4, 1, 3, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 0, 0, 15, 4, 1, 18, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);

	for (int var4 = 0; var4 <= 4; ++var4)
	{
		for (int var5 = 0; var5 <= 2; ++var5)
		{
			fillCurrentPositionBlocksDownwards(pWorld, BLOCK_ID_NETHER_BRICK, 0, var4, -1, var5, aabb);
			fillCurrentPositionBlocksDownwards(pWorld, BLOCK_ID_NETHER_BRICK, 0, var4, -1, 18 - var5, aabb);
		}
	}

	fillWithBlocks(pWorld, aabb, 0, 1, 1, 0, 4, 1, BLOCK_ID_NETHER_FENCE, BLOCK_ID_NETHER_FENCE, false);
	fillWithBlocks(pWorld, aabb, 0, 3, 4, 0, 4, 4, BLOCK_ID_NETHER_FENCE, BLOCK_ID_NETHER_FENCE, false);
	fillWithBlocks(pWorld, aabb, 0, 3, 14, 0, 4, 14, BLOCK_ID_NETHER_FENCE, BLOCK_ID_NETHER_FENCE, false);
	fillWithBlocks(pWorld, aabb, 0, 1, 17, 0, 4, 17, BLOCK_ID_NETHER_FENCE, BLOCK_ID_NETHER_FENCE, false);
	fillWithBlocks(pWorld, aabb, 4, 1, 1, 4, 4, 1, BLOCK_ID_NETHER_FENCE, BLOCK_ID_NETHER_FENCE, false);
	fillWithBlocks(pWorld, aabb, 4, 3, 4, 4, 4, 4, BLOCK_ID_NETHER_FENCE, BLOCK_ID_NETHER_FENCE, false);
	fillWithBlocks(pWorld, aabb, 4, 3, 14, 4, 4, 14, BLOCK_ID_NETHER_FENCE, BLOCK_ID_NETHER_FENCE, false);
	fillWithBlocks(pWorld, aabb, 4, 1, 17, 4, 4, 17, BLOCK_ID_NETHER_FENCE, BLOCK_ID_NETHER_FENCE, false);
	return true;
}

ComponentNetherBridgeCrossing3::ComponentNetherBridgeCrossing3(int type, Random& rand, const StructureBB& aabb, int mode)
	: ComponentNetherBridgePiece(type)
{
	m_coordBaseMode = mode;
	m_boundingBox = aabb;
}

ComponentNetherBridgeCrossing3::ComponentNetherBridgeCrossing3(Random& rand, int x, int z)
	: ComponentNetherBridgePiece(0)
{
	m_coordBaseMode = rand.nextInt(4);

	switch (m_coordBaseMode)
	{
	case 0:
	case 2:
		m_boundingBox.set(x, 64, z, x + 19 - 1, 73, z + 19 - 1);
		break;

	default:
		m_boundingBox.set(x, 64, z, x + 19 - 1, 73, z + 19 - 1);
	}
}

void ComponentNetherBridgeCrossing3::buildComponent(StructureComponent* component, SCList& lst, Random& rand)
{
	getNextComponentNormal((ComponentNetherBridgeStartPiece*)component, lst, rand, 8, 3, false);
	getNextComponentX((ComponentNetherBridgeStartPiece*)component, lst, rand, 3, 8, false);
	getNextComponentZ((ComponentNetherBridgeStartPiece*)component, lst, rand, 3, 8, false);
}

ComponentNetherBridgeCrossing3* ComponentNetherBridgeCrossing3::createValidComponent(SCList& lst, Random& rand, int par2, int par3, int par4, int par5, int par6)
{
	StructureBB aabb = StructureBB::getComponentToAddBoundingBox(par2, par3, par4, -8, -3, 0, 19, 10, 19, par5);
	if (isAboveGround(aabb) && StructureComponent::findIntersecting(lst, aabb) == NULL)
		return LordNew ComponentNetherBridgeCrossing3(par6, rand, aabb, par5);
	return NULL;
}

bool ComponentNetherBridgeCrossing3::addComponentParts(World* pWorld, Random& rand, const StructureBB& aabb)
{
	fillWithBlocks(pWorld, aabb, 7, 3, 0, 11, 4, 18, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 0, 3, 7, 18, 4, 11, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 8, 5, 0, 10, 7, 18, 0, 0, false);
	fillWithBlocks(pWorld, aabb, 0, 5, 8, 18, 7, 10, 0, 0, false);
	fillWithBlocks(pWorld, aabb, 7, 5, 0, 7, 5, 7, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 7, 5, 11, 7, 5, 18, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 11, 5, 0, 11, 5, 7, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 11, 5, 11, 11, 5, 18, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 0, 5, 7, 7, 5, 7, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 11, 5, 7, 18, 5, 7, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 0, 5, 11, 7, 5, 11, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 11, 5, 11, 18, 5, 11, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 7, 2, 0, 11, 2, 5, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 7, 2, 13, 11, 2, 18, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 7, 0, 0, 11, 1, 3, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 7, 0, 15, 11, 1, 18, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	int var4;
	int var5;

	for (var4 = 7; var4 <= 11; ++var4)
	{
		for (var5 = 0; var5 <= 2; ++var5)
		{
			fillCurrentPositionBlocksDownwards(pWorld, BLOCK_ID_NETHER_BRICK, 0, var4, -1, var5, aabb);
			fillCurrentPositionBlocksDownwards(pWorld, BLOCK_ID_NETHER_BRICK, 0, var4, -1, 18 - var5, aabb);
		}
	}

	fillWithBlocks(pWorld, aabb, 0, 2, 7, 5, 2, 11, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 13, 2, 7, 18, 2, 11, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 0, 0, 7, 3, 1, 11, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 15, 0, 7, 18, 1, 11, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);

	for (var4 = 0; var4 <= 2; ++var4)
	{
		for (var5 = 7; var5 <= 11; ++var5)
		{
			fillCurrentPositionBlocksDownwards(pWorld, BLOCK_ID_NETHER_BRICK, 0, var4, -1, var5, aabb);
			fillCurrentPositionBlocksDownwards(pWorld, BLOCK_ID_NETHER_BRICK, 0, 18 - var4, -1, var5, aabb);
		}
	}

	return true;
}

ComponentNetherBridgeCrossing::ComponentNetherBridgeCrossing(int type, Random& rand, const StructureBB& aabb, int mode)
	: ComponentNetherBridgePiece(type)
{
	m_coordBaseMode = mode;
	m_boundingBox = aabb;
}

void ComponentNetherBridgeCrossing::buildComponent(StructureComponent* component, SCList& lst, Random& rand)
{
	getNextComponentNormal((ComponentNetherBridgeStartPiece*)component, lst, rand, 2, 0, false);
	getNextComponentX((ComponentNetherBridgeStartPiece*)component, lst, rand, 0, 2, false);
	getNextComponentZ((ComponentNetherBridgeStartPiece*)component, lst, rand, 0, 2, false);
}

ComponentNetherBridgeCrossing* ComponentNetherBridgeCrossing::createValidComponent(SCList& lst, Random& rand, int par2, int par3, int par4, int par5, int type)
{
	StructureBB aabb = StructureBB::getComponentToAddBoundingBox(par2, par3, par4, -2, 0, 0, 7, 9, 7, par5);
	if (isAboveGround(aabb) && StructureComponent::findIntersecting(lst, aabb) == NULL )
		return LordNew ComponentNetherBridgeCrossing(type, rand, aabb, par5);
	return NULL;
}

bool ComponentNetherBridgeCrossing::addComponentParts(World* pWorld, Random& rand, const StructureBB& aabb)
{
	fillWithBlocks(pWorld, aabb, 0, 0, 0, 6, 1, 6, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 0, 2, 0, 6, 7, 6, 0, 0, false);
	fillWithBlocks(pWorld, aabb, 0, 2, 0, 1, 6, 0, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 0, 2, 6, 1, 6, 6, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 5, 2, 0, 6, 6, 0, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 5, 2, 6, 6, 6, 6, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 0, 2, 0, 0, 6, 1, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 0, 2, 5, 0, 6, 6, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 6, 2, 0, 6, 6, 1, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 6, 2, 5, 6, 6, 6, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 2, 6, 0, 4, 6, 0, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 2, 5, 0, 4, 5, 0, BLOCK_ID_NETHER_FENCE, BLOCK_ID_NETHER_FENCE, false);
	fillWithBlocks(pWorld, aabb, 2, 6, 6, 4, 6, 6, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 2, 5, 6, 4, 5, 6, BLOCK_ID_NETHER_FENCE, BLOCK_ID_NETHER_FENCE, false);
	fillWithBlocks(pWorld, aabb, 0, 6, 2, 0, 6, 4, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 0, 5, 2, 0, 5, 4, BLOCK_ID_NETHER_FENCE, BLOCK_ID_NETHER_FENCE, false);
	fillWithBlocks(pWorld, aabb, 6, 6, 2, 6, 6, 4, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 6, 5, 2, 6, 5, 4, BLOCK_ID_NETHER_FENCE, BLOCK_ID_NETHER_FENCE, false);

	for (int var4 = 0; var4 <= 6; ++var4)
	{
		for (int var5 = 0; var5 <= 6; ++var5)
		{
			fillCurrentPositionBlocksDownwards(pWorld, BLOCK_ID_NETHER_BRICK, 0, var4, -1, var5, aabb);
		}
	}

	return true;
}

ComponentNetherBridgeStairs::ComponentNetherBridgeStairs(int type, Random& rand, const StructureBB& aabb, int mode)
	: ComponentNetherBridgePiece(type)
{
	m_coordBaseMode = mode;
	m_boundingBox = aabb;
}

void ComponentNetherBridgeStairs::buildComponent(StructureComponent* component, SCList& lst, Random& rand)
{
	getNextComponentZ((ComponentNetherBridgeStartPiece*)component, lst, rand, 6, 2, false);
}

ComponentNetherBridgeStairs* ComponentNetherBridgeStairs::createValidComponent(SCList& lst, Random& rand, int par2, int par3, int par4, int par5, int type)
{
	StructureBB aabb = StructureBB::getComponentToAddBoundingBox(par2, par3, par4, -2, 0, 0, 7, 11, 7, par5);
	if (isAboveGround(aabb) && StructureComponent::findIntersecting(lst, aabb) == NULL)
		return LordNew ComponentNetherBridgeStairs(type, rand, aabb, par5);
	return NULL;
}

bool ComponentNetherBridgeStairs::addComponentParts(World* pWorld, Random& rand, const StructureBB&  aabb)
{
	fillWithBlocks(pWorld, aabb, 0, 0, 0, 6, 1, 6, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 0, 2, 0, 6, 10, 6, 0, 0, false);
	fillWithBlocks(pWorld, aabb, 0, 2, 0, 1, 8, 0, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 5, 2, 0, 6, 8, 0, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 0, 2, 1, 0, 8, 6, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 6, 2, 1, 6, 8, 6, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 1, 2, 6, 5, 8, 6, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 0, 3, 2, 0, 5, 4, BLOCK_ID_NETHER_FENCE, BLOCK_ID_NETHER_FENCE, false);
	fillWithBlocks(pWorld, aabb, 6, 3, 2, 6, 5, 2, BLOCK_ID_NETHER_FENCE, BLOCK_ID_NETHER_FENCE, false);
	fillWithBlocks(pWorld, aabb, 6, 3, 4, 6, 5, 4, BLOCK_ID_NETHER_FENCE, BLOCK_ID_NETHER_FENCE, false);
	placeBlockAtCurrentPosition(pWorld, BLOCK_ID_NETHER_BRICK, 0, 5, 2, 5, aabb);
	fillWithBlocks(pWorld, aabb, 4, 2, 5, 4, 3, 5, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 3, 2, 5, 3, 4, 5, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 2, 2, 5, 2, 5, 5, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 1, 2, 5, 1, 6, 5, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 1, 7, 1, 5, 7, 4, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 6, 8, 2, 6, 8, 4, 0, 0, false);
	fillWithBlocks(pWorld, aabb, 2, 6, 0, 4, 8, 0, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 2, 5, 0, 4, 5, 0, BLOCK_ID_NETHER_FENCE, BLOCK_ID_NETHER_FENCE, false);

	for (int var4 = 0; var4 <= 6; ++var4)
	{
		for (int var5 = 0; var5 <= 6; ++var5)
		{
			fillCurrentPositionBlocksDownwards(pWorld, BLOCK_ID_NETHER_BRICK, 0, var4, -1, var5, aabb);
		}
	}

	return true;
}

ComponentNetherBridgeThrone::ComponentNetherBridgeThrone(int type, Random& rand, const StructureBB& aabb, int mode)
	: ComponentNetherBridgePiece(type)
	, hasSpawner(false)
{
	m_coordBaseMode = mode;
	m_boundingBox = aabb;
}

ComponentNetherBridgeThrone* ComponentNetherBridgeThrone::createValidComponent(SCList& lst, Random& rand, int par2, int par3, int par4, int par5, int type)
{
	StructureBB aabb = StructureBB::getComponentToAddBoundingBox(par2, par3, par4, -2, 0, 0, 7, 8, 9, par5);
	if (isAboveGround(aabb) && StructureComponent::findIntersecting(lst, aabb) == NULL)
		return LordNew ComponentNetherBridgeThrone(type, rand, aabb, par5);
	return NULL;
}

bool ComponentNetherBridgeThrone::addComponentParts(World* pWorld, Random& rand, const StructureBB& aabb)
{
	fillWithBlocks(pWorld, aabb, 0, 2, 0, 6, 7, 7, 0, 0, false);
	fillWithBlocks(pWorld, aabb, 1, 0, 0, 5, 1, 7, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 1, 2, 1, 5, 2, 7, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 1, 3, 2, 5, 3, 7, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 1, 4, 3, 5, 4, 7, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 1, 2, 0, 1, 4, 2, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 5, 2, 0, 5, 4, 2, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 1, 5, 2, 1, 5, 3, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 5, 5, 2, 5, 5, 3, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 0, 5, 3, 0, 5, 8, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 6, 5, 3, 6, 5, 8, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 1, 5, 8, 5, 5, 8, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	placeBlockAtCurrentPosition(pWorld, BLOCK_ID_NETHER_FENCE, 0, 1, 6, 3, aabb);
	placeBlockAtCurrentPosition(pWorld, BLOCK_ID_NETHER_FENCE, 0, 5, 6, 3, aabb);
	fillWithBlocks(pWorld, aabb, 0, 6, 3, 0, 6, 8, BLOCK_ID_NETHER_FENCE, BLOCK_ID_NETHER_FENCE, false);
	fillWithBlocks(pWorld, aabb, 6, 6, 3, 6, 6, 8, BLOCK_ID_NETHER_FENCE, BLOCK_ID_NETHER_FENCE, false);
	fillWithBlocks(pWorld, aabb, 1, 6, 8, 5, 7, 8, BLOCK_ID_NETHER_FENCE, BLOCK_ID_NETHER_FENCE, false);
	fillWithBlocks(pWorld, aabb, 2, 8, 8, 4, 8, 8, BLOCK_ID_NETHER_FENCE, BLOCK_ID_NETHER_FENCE, false);
	int vy;
	int vx;

	if (!hasSpawner)
	{
		vy = getYWithOffset(5);
		vx = getXWithOffset(3, 5);
		int vz = getZWithOffset(3, 5);

		BlockPos ipos(vx, vy, vz);
		if (aabb.isVecInside(vx, vy, vz))
		{
			hasSpawner = true;
			pWorld->setBlock(ipos, BLOCK_ID_MOB_SPAWNER, 0, 2);

			/*TileEntityMobSpawner var7 = (TileEntityMobSpawner)pWorld.getBlockTileEntity(vx, vy, vz);
			if (var7 != null)
			{
				var7.getSpawnerLogic().setMobID("Blaze");
			}*/
		}
	}

	for (vy = 0; vy <= 6; ++vy)
	{
		for (vx = 0; vx <= 6; ++vx)
		{
			fillCurrentPositionBlocksDownwards(pWorld, BLOCK_ID_NETHER_BRICK, 0, vy, -1, vx, aabb);
		}
	}

	return true;
}

ComponentNetherBridgeEntrance::ComponentNetherBridgeEntrance(int type, Random& rand, const StructureBB& aabb, int mode)
	: ComponentNetherBridgePiece(type)
{
	m_coordBaseMode = mode;
	m_boundingBox = aabb;
}

/** Initiates construction of the Structure Component picked, at the current Location of StructGen */
void ComponentNetherBridgeEntrance::buildComponent(StructureComponent* component, SCList& lst, Random& rand)
{
	getNextComponentNormal(reinterpret_cast<ComponentNetherBridgeStartPiece*>(component), lst, rand, 5, 3, true);
}

/** Creates and returns a new component piece. Or null if it could not find enough room to place it.*/
ComponentNetherBridgeEntrance* ComponentNetherBridgeEntrance::createValidComponent(SCList& lst, Random& rand, int par2, int par3, int par4, int par5, int type)
{
	StructureBB aabb = StructureBB::getComponentToAddBoundingBox(par2, par3, par4, -5, -3, 0, 13, 14, 13, par5);
	if (isAboveGround(aabb) && StructureComponent::findIntersecting(lst, aabb) == NULL)
		return LordNew ComponentNetherBridgeEntrance(type, rand, aabb, par5);
	return NULL;
}

/** second Part of Structure generating, this for example places Spiderwebs, Mob Spawners, it closes Mineshafts at the end, it adds Fences... */
bool ComponentNetherBridgeEntrance::addComponentParts(World* pWorld, Random& rand, const StructureBB& aabb)
{
	fillWithBlocks(pWorld, aabb, 0, 3, 0, 12, 4, 12, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 0, 5, 0, 12, 13, 12, 0, 0, false);
	fillWithBlocks(pWorld, aabb, 0, 5, 0, 1, 12, 12, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 11, 5, 0, 12, 12, 12, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 2, 5, 11, 4, 12, 12, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 8, 5, 11, 10, 12, 12, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 5, 9, 11, 7, 12, 12, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 2, 5, 0, 4, 12, 1, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 8, 5, 0, 10, 12, 1, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 5, 9, 0, 7, 12, 1, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 2, 11, 2, 10, 12, 10, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 5, 8, 0, 7, 8, 0, BLOCK_ID_NETHER_FENCE, BLOCK_ID_NETHER_FENCE, false);
	int vx;

	for (vx = 1; vx <= 11; vx += 2)
	{
		fillWithBlocks(pWorld, aabb, vx, 10, 0, vx, 11, 0, BLOCK_ID_NETHER_FENCE, BLOCK_ID_NETHER_FENCE, false);
		fillWithBlocks(pWorld, aabb, vx, 10, 12, vx, 11, 12, BLOCK_ID_NETHER_FENCE, BLOCK_ID_NETHER_FENCE, false);
		fillWithBlocks(pWorld, aabb, 0, 10, vx, 0, 11, vx, BLOCK_ID_NETHER_FENCE, BLOCK_ID_NETHER_FENCE, false);
		fillWithBlocks(pWorld, aabb, 12, 10, vx, 12, 11, vx, BLOCK_ID_NETHER_FENCE, BLOCK_ID_NETHER_FENCE, false);
		placeBlockAtCurrentPosition(pWorld, BLOCK_ID_NETHER_BRICK, 0, vx, 13, 0, aabb);
		placeBlockAtCurrentPosition(pWorld, BLOCK_ID_NETHER_BRICK, 0, vx, 13, 12, aabb);
		placeBlockAtCurrentPosition(pWorld, BLOCK_ID_NETHER_BRICK, 0, 0, 13, vx, aabb);
		placeBlockAtCurrentPosition(pWorld, BLOCK_ID_NETHER_BRICK, 0, 12, 13, vx, aabb);
		placeBlockAtCurrentPosition(pWorld, BLOCK_ID_NETHER_FENCE, 0, vx + 1, 13, 0, aabb);
		placeBlockAtCurrentPosition(pWorld, BLOCK_ID_NETHER_FENCE, 0, vx + 1, 13, 12, aabb);
		placeBlockAtCurrentPosition(pWorld, BLOCK_ID_NETHER_FENCE, 0, 0, 13, vx + 1, aabb);
		placeBlockAtCurrentPosition(pWorld, BLOCK_ID_NETHER_FENCE, 0, 12, 13, vx + 1, aabb);
	}

	placeBlockAtCurrentPosition(pWorld, BLOCK_ID_NETHER_FENCE, 0, 0, 13, 0, aabb);
	placeBlockAtCurrentPosition(pWorld, BLOCK_ID_NETHER_FENCE, 0, 0, 13, 12, aabb);
	placeBlockAtCurrentPosition(pWorld, BLOCK_ID_NETHER_FENCE, 0, 0, 13, 0, aabb);
	placeBlockAtCurrentPosition(pWorld, BLOCK_ID_NETHER_FENCE, 0, 12, 13, 0, aabb);

	for (vx = 3; vx <= 9; vx += 2)
	{
		fillWithBlocks(pWorld, aabb, 1, 7, vx, 1, 8, vx, BLOCK_ID_NETHER_FENCE, BLOCK_ID_NETHER_FENCE, false);
		fillWithBlocks(pWorld, aabb, 11, 7, vx, 11, 8, vx, BLOCK_ID_NETHER_FENCE, BLOCK_ID_NETHER_FENCE, false);
	}

	fillWithBlocks(pWorld, aabb, 4, 2, 0, 8, 2, 12, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 0, 2, 4, 12, 2, 8, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 4, 0, 0, 8, 1, 3, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 4, 0, 9, 8, 1, 12, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 0, 0, 4, 3, 1, 8, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 9, 0, 4, 12, 1, 8, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	int vy;

	for (vx = 4; vx <= 8; ++vx)
	{
		for (vy = 0; vy <= 2; ++vy)
		{
			fillCurrentPositionBlocksDownwards(pWorld, BLOCK_ID_NETHER_BRICK, 0, vx, -1, vy, aabb);
			fillCurrentPositionBlocksDownwards(pWorld, BLOCK_ID_NETHER_BRICK, 0, vx, -1, 12 - vy, aabb);
		}
	}

	for (vx = 0; vx <= 2; ++vx)
	{
		for (vy = 4; vy <= 8; ++vy)
		{
			fillCurrentPositionBlocksDownwards(pWorld, BLOCK_ID_NETHER_BRICK, 0, vx, -1, vy, aabb);
			fillCurrentPositionBlocksDownwards(pWorld, BLOCK_ID_NETHER_BRICK, 0, 12 - vx, -1, vy, aabb);
		}
	}

	fillWithBlocks(pWorld, aabb, 5, 5, 5, 7, 5, 7, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 6, 1, 6, 6, 4, 6, 0, 0, false);
	placeBlockAtCurrentPosition(pWorld, BLOCK_ID_NETHER_BRICK, 0, 6, 0, 6, aabb);
	placeBlockAtCurrentPosition(pWorld, BLOCK_ID_LAVAMOVING, 0, 6, 5, 6, aabb);
	vx = getXWithOffset(6, 6);
	vy = getYWithOffset(5);
	int vz = getZWithOffset(6, 6);

	if (aabb.isVecInside(vx, vy, vz))
	{
		/*pWorld->scheduledUpdatesAreImmediate = true;
		Block.blocksList[Block.lavaMoving.blockID].updateTick(pWorld, vx, vy, vz, rand);
		pWorld->scheduledUpdatesAreImmediate = false;*/
	}

	return true;
}

ComponentNetherBridgeCorridor5::ComponentNetherBridgeCorridor5(int type, Random& rand, const StructureBB& aabb, int mode)
	: ComponentNetherBridgePiece(type)
{
	m_coordBaseMode = mode;
	m_boundingBox = aabb;
}

void ComponentNetherBridgeCorridor5::buildComponent(StructureComponent* component, SCList& lst, Random& rand)
{
	getNextComponentNormal((ComponentNetherBridgeStartPiece*)component, lst, rand, 1, 0, true);
}

ComponentNetherBridgeCorridor5* ComponentNetherBridgeCorridor5::createValidComponent(SCList& lst, Random& rand, int x, int y, int z, int mode, int type)
{
	StructureBB aabb = StructureBB::getComponentToAddBoundingBox(x, y, z, -1, 0, 0, 5, 7, 5, mode);
	if (isAboveGround(aabb) && StructureComponent::findIntersecting(lst, aabb) == NULL)
		return  LordNew ComponentNetherBridgeCorridor5(type, rand, aabb, mode);
	return NULL;
}

bool ComponentNetherBridgeCorridor5::addComponentParts(World* pWorld, Random& rand, const StructureBB& aabb)
{
	fillWithBlocks(pWorld, aabb, 0, 0, 0, 4, 1, 4, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 0, 2, 0, 4, 5, 4, 0, 0, false);
	fillWithBlocks(pWorld, aabb, 0, 2, 0, 0, 5, 4, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 4, 2, 0, 4, 5, 4, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 0, 3, 1, 0, 4, 1, BLOCK_ID_NETHER_FENCE, BLOCK_ID_NETHER_FENCE, false);
	fillWithBlocks(pWorld, aabb, 0, 3, 3, 0, 4, 3, BLOCK_ID_NETHER_FENCE, BLOCK_ID_NETHER_FENCE, false);
	fillWithBlocks(pWorld, aabb, 4, 3, 1, 4, 4, 1, BLOCK_ID_NETHER_FENCE, BLOCK_ID_NETHER_FENCE, false);
	fillWithBlocks(pWorld, aabb, 4, 3, 3, 4, 4, 3, BLOCK_ID_NETHER_FENCE, BLOCK_ID_NETHER_FENCE, false);
	fillWithBlocks(pWorld, aabb, 0, 6, 0, 4, 6, 4, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);

	for (int var4 = 0; var4 <= 4; ++var4)
	{
		for (int var5 = 0; var5 <= 4; ++var5)
		{
			fillCurrentPositionBlocksDownwards(pWorld, BLOCK_ID_NETHER_BRICK, 0, var4, -1, var5, aabb);
		}
	}

	return true;
}

ComponentNetherBridgeCorridor2::ComponentNetherBridgeCorridor2(int type, Random& rand, const StructureBB& aabb, int mode)
	: ComponentNetherBridgePiece(type)
{
	m_coordBaseMode = mode;
	m_boundingBox = aabb;
	field_111020_b = rand.nextInt(3) == 0;
}

/** Initiates construction of the Structure Component picked, at the current Location of StructGen */
void ComponentNetherBridgeCorridor2::buildComponent(StructureComponent* component, SCList& lst, Random& rand)
{
	getNextComponentZ((ComponentNetherBridgeStartPiece*)component, lst, rand, 0, 1, true);
}

/** Creates and returns a new component piece. Or null if it could not find enough room to place it. */
ComponentNetherBridgeCorridor2* ComponentNetherBridgeCorridor2::createValidComponent(SCList& lst, Random& rand, int x, int y, int z, int mode, int type)
{
	StructureBB aabb = StructureBB::getComponentToAddBoundingBox(x, y, z, -1, 0, 0, 5, 7, 5, mode);
	if (isAboveGround(aabb) && StructureComponent::findIntersecting(lst, aabb) == NULL)
		return LordNew ComponentNetherBridgeCorridor2(type, rand, aabb, mode);
	return NULL;
}

/** second Part of Structure generating, this for example places Spiderwebs, Mob Spawners, it closes Mineshafts at the end, it adds Fences... */
bool ComponentNetherBridgeCorridor2::addComponentParts(World* pWorld, Random& rand, const StructureBB& aabb)
{
	fillWithBlocks(pWorld, aabb, 0, 0, 0, 4, 1, 4, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 0, 2, 0, 4, 5, 4, 0, 0, false);
	fillWithBlocks(pWorld, aabb, 0, 2, 0, 0, 5, 4, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 0, 3, 1, 0, 4, 1, BLOCK_ID_NETHER_FENCE, BLOCK_ID_NETHER_FENCE, false);
	fillWithBlocks(pWorld, aabb, 0, 3, 3, 0, 4, 3, BLOCK_ID_NETHER_FENCE, BLOCK_ID_NETHER_FENCE, false);
	fillWithBlocks(pWorld, aabb, 4, 2, 0, 4, 5, 0, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 1, 2, 4, 4, 5, 4, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 1, 3, 4, 1, 4, 4, BLOCK_ID_NETHER_FENCE, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 3, 3, 4, 3, 4, 4, BLOCK_ID_NETHER_FENCE, BLOCK_ID_NETHER_BRICK, false);
	int var4;
	int var5;

	if (field_111020_b)
	{
		var4 = getYWithOffset(2);
		var5 = getXWithOffset(1, 3);
		int var6 = getZWithOffset(1, 3);

		if (aabb.isVecInside(var5, var4, var6))
		{
			field_111020_b = false;
			generateStructureChestContents(pWorld, aabb, rand, 1, 2, 3, conttes, 2 + rand.nextInt(4));
		}
	}

	fillWithBlocks(pWorld, aabb, 0, 6, 0, 4, 6, 4, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);

	for (var4 = 0; var4 <= 4; ++var4)
	{
		for (var5 = 0; var5 <= 4; ++var5)
		{
			fillCurrentPositionBlocksDownwards(pWorld, BLOCK_ID_NETHER_BRICK, 0, var4, -1, var5, aabb);
		}
	}

	return true;
}

ComponentNetherBridgeCorridor::ComponentNetherBridgeCorridor(int type, Random& rand, const StructureBB& aabb, int mode)
	: ComponentNetherBridgePiece(type)
{
	m_coordBaseMode = mode;
	m_boundingBox = aabb;
	field_111021_b = rand.nextInt(3) == 0;
}

void ComponentNetherBridgeCorridor::buildComponent(StructureComponent* component, SCList& lst, Random& rand)
{
	getNextComponentX((ComponentNetherBridgeStartPiece*)component, lst, rand, 0, 1, true);
}

ComponentNetherBridgeCorridor* ComponentNetherBridgeCorridor::createValidComponent(SCList& lst, Random& rand, int x, int y, int z, int mode, int type)
{
	StructureBB aabb = StructureBB::getComponentToAddBoundingBox(x, y, z, -1, 0, 0, 5, 7, 5, mode);
	if (isAboveGround(aabb) && StructureComponent::findIntersecting(lst, aabb) == NULL)
		return LordNew ComponentNetherBridgeCorridor(type, rand, aabb, mode);
	return NULL;
}

bool ComponentNetherBridgeCorridor::addComponentParts(World* pWorld, Random& rand, const StructureBB& aabb)
{
	fillWithBlocks(pWorld, aabb, 0, 0, 0, 4, 1, 4, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 0, 2, 0, 4, 5, 4, 0, 0, false);
	fillWithBlocks(pWorld, aabb, 4, 2, 0, 4, 5, 4, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 4, 3, 1, 4, 4, 1, BLOCK_ID_NETHER_FENCE, BLOCK_ID_NETHER_FENCE, false);
	fillWithBlocks(pWorld, aabb, 4, 3, 3, 4, 4, 3, BLOCK_ID_NETHER_FENCE, BLOCK_ID_NETHER_FENCE, false);
	fillWithBlocks(pWorld, aabb, 0, 2, 0, 0, 5, 0, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 0, 2, 4, 3, 5, 4, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 1, 3, 4, 1, 4, 4, BLOCK_ID_NETHER_FENCE, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 3, 3, 4, 3, 4, 4, BLOCK_ID_NETHER_FENCE, BLOCK_ID_NETHER_BRICK, false);
	int var4;
	int var5;

	if (field_111021_b)
	{
		var4 = getYWithOffset(2);
		var5 = getXWithOffset(3, 3);
		int var6 = getZWithOffset(3, 3);

		if (aabb.isVecInside(var5, var4, var6))
		{
			field_111021_b = false;
			generateStructureChestContents(pWorld, aabb, rand, 3, 2, 3, conttes, 2 + rand.nextInt(4));
		}
	}

	fillWithBlocks(pWorld, aabb, 0, 6, 0, 4, 6, 4, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);

	for (var4 = 0; var4 <= 4; ++var4)
	{
		for (var5 = 0; var5 <= 4; ++var5)
		{
			fillCurrentPositionBlocksDownwards(pWorld, BLOCK_ID_NETHER_BRICK, 0, var4, -1, var5, aabb);
		}
	}

	return true;
}

ComponentNetherBridgeCorridor3::ComponentNetherBridgeCorridor3(int type, Random& rand, const StructureBB& aabb, int mode)
	: ComponentNetherBridgePiece(type)
{
	m_coordBaseMode = mode;
	m_boundingBox = aabb;
}

void ComponentNetherBridgeCorridor3::buildComponent(StructureComponent* component, SCList& lst, Random& rand)
{
	getNextComponentNormal((ComponentNetherBridgeStartPiece*)component, lst, rand, 1, 0, true);
}

ComponentNetherBridgeCorridor3* ComponentNetherBridgeCorridor3::createValidComponent(SCList& lst, Random& rand, int x, int y, int z, int mode, int type)
{
	StructureBB aabb = StructureBB::getComponentToAddBoundingBox(x, y, z, -1, -7, 0, 5, 14, 10, mode);
	if (isAboveGround(aabb) && StructureComponent::findIntersecting(lst, aabb) == NULL)
		return LordNew ComponentNetherBridgeCorridor3(type, rand, aabb, mode);
	return NULL;
}

bool ComponentNetherBridgeCorridor3::addComponentParts(World* pWorld, Random& rand, const StructureBB& aabb)
{
	int metadata = getMetadataWithOffset(BLOCK_ID_STAIRS_NETHER_BRICK, 2);

	for (int var5 = 0; var5 <= 9; ++var5)
	{
		int var6 = Math::Max(1, 7 - var5);
		int var7 = Math::Min(Math::Max(var6 + 5, 14 - var5), 13);
		int var8 = var5;
		fillWithBlocks(pWorld, aabb, 0, 0, var5, 4, var6, var5, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
		fillWithBlocks(pWorld, aabb, 1, var6 + 1, var5, 3, var7 - 1, var5, 0, 0, false);

		if (var5 <= 6)
		{
			placeBlockAtCurrentPosition(pWorld, BLOCK_ID_STAIRS_NETHER_BRICK, metadata, 1, var6 + 1, var5, aabb);
			placeBlockAtCurrentPosition(pWorld, BLOCK_ID_STAIRS_NETHER_BRICK, metadata, 2, var6 + 1, var5, aabb);
			placeBlockAtCurrentPosition(pWorld, BLOCK_ID_STAIRS_NETHER_BRICK, metadata, 3, var6 + 1, var5, aabb);
		}

		fillWithBlocks(pWorld, aabb, 0, var7, var5, 4, var7, var5, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
		fillWithBlocks(pWorld, aabb, 0, var6 + 1, var5, 0, var7 - 1, var5, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
		fillWithBlocks(pWorld, aabb, 4, var6 + 1, var5, 4, var7 - 1, var5, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);

		if ((var5 & 1) == 0)
		{
			fillWithBlocks(pWorld, aabb, 0, var6 + 2, var5, 0, var6 + 3, var5, BLOCK_ID_NETHER_FENCE, BLOCK_ID_NETHER_FENCE, false);
			fillWithBlocks(pWorld, aabb, 4, var6 + 2, var5, 4, var6 + 3, var5, BLOCK_ID_NETHER_FENCE, BLOCK_ID_NETHER_FENCE, false);
		}

		for (int var9 = 0; var9 <= 4; ++var9)
		{
			fillCurrentPositionBlocksDownwards(pWorld, BLOCK_ID_NETHER_BRICK, 0, var9, -1, var8, aabb);
		}
	}

	return true;
}

ComponentNetherBridgeCorridor4::ComponentNetherBridgeCorridor4(int type, Random& rand, const StructureBB& aabb, int mode)
	: ComponentNetherBridgePiece(type)
{
	m_coordBaseMode = mode;
	m_boundingBox = aabb;
}

void ComponentNetherBridgeCorridor4::buildComponent(StructureComponent* component, SCList& lst, Random& rand)
{
	i8 var4 = 1;

	if (m_coordBaseMode == 1 || m_coordBaseMode == 2)
	{
		var4 = 5;
	}

	getNextComponentX((ComponentNetherBridgeStartPiece*)component, lst, rand, 0, var4, rand.nextInt(8) > 0);
	getNextComponentZ((ComponentNetherBridgeStartPiece*)component, lst, rand, 0, var4, rand.nextInt(8) > 0);
}

ComponentNetherBridgeCorridor4* ComponentNetherBridgeCorridor4::createValidComponent(SCList& lst, Random& rand, int x, int y, int z, int mode, int type)
{
	StructureBB aabb = StructureBB::getComponentToAddBoundingBox(x, y, z, -3, 0, 0, 9, 7, 9, mode);
	if (isAboveGround(aabb) && StructureComponent::findIntersecting(lst, aabb) == NULL)
		return LordNew ComponentNetherBridgeCorridor4(type, rand, aabb, mode);
	return NULL;
}

bool ComponentNetherBridgeCorridor4::addComponentParts(World* pWorld, Random& rand, const StructureBB& aabb)
{
	fillWithBlocks(pWorld, aabb, 0, 0, 0, 8, 1, 8, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 0, 2, 0, 8, 5, 8, 0, 0, false);
	fillWithBlocks(pWorld, aabb, 0, 6, 0, 8, 6, 5, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 0, 2, 0, 2, 5, 0, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 6, 2, 0, 8, 5, 0, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 1, 3, 0, 1, 4, 0, BLOCK_ID_NETHER_FENCE, BLOCK_ID_NETHER_FENCE, false);
	fillWithBlocks(pWorld, aabb, 7, 3, 0, 7, 4, 0, BLOCK_ID_NETHER_FENCE, BLOCK_ID_NETHER_FENCE, false);
	fillWithBlocks(pWorld, aabb, 0, 2, 4, 8, 2, 8, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 1, 1, 4, 2, 2, 4, 0, 0, false);
	fillWithBlocks(pWorld, aabb, 6, 1, 4, 7, 2, 4, 0, 0, false);
	fillWithBlocks(pWorld, aabb, 0, 3, 8, 8, 3, 8, BLOCK_ID_NETHER_FENCE, BLOCK_ID_NETHER_FENCE, false);
	fillWithBlocks(pWorld, aabb, 0, 3, 6, 0, 3, 7, BLOCK_ID_NETHER_FENCE, BLOCK_ID_NETHER_FENCE, false);
	fillWithBlocks(pWorld, aabb, 8, 3, 6, 8, 3, 7, BLOCK_ID_NETHER_FENCE, BLOCK_ID_NETHER_FENCE, false);
	fillWithBlocks(pWorld, aabb, 0, 3, 4, 0, 5, 5, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 8, 3, 4, 8, 5, 5, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 1, 3, 5, 2, 5, 5, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 6, 3, 5, 7, 5, 5, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 1, 4, 5, 1, 5, 5, BLOCK_ID_NETHER_FENCE, BLOCK_ID_NETHER_FENCE, false);
	fillWithBlocks(pWorld, aabb, 7, 4, 5, 7, 5, 5, BLOCK_ID_NETHER_FENCE, BLOCK_ID_NETHER_FENCE, false);

	for (int var4 = 0; var4 <= 5; ++var4)
	{
		for (int var5 = 0; var5 <= 8; ++var5)
		{
			fillCurrentPositionBlocksDownwards(pWorld, BLOCK_ID_NETHER_BRICK, 0, var5, -1, var4, aabb);
		}
	}

	return true;
}

ComponentNetherBridgeCrossing2::ComponentNetherBridgeCrossing2(int type, Random& rand, const StructureBB& aabb, int mode)
	: ComponentNetherBridgePiece(type)
{
	m_coordBaseMode = mode;
	m_boundingBox = aabb;
}

void ComponentNetherBridgeCrossing2::buildComponent(StructureComponent* component, SCList& lst, Random& rand)
{
	getNextComponentNormal((ComponentNetherBridgeStartPiece*)component, lst, rand, 1, 0, true);
	getNextComponentX((ComponentNetherBridgeStartPiece*)component, lst, rand, 0, 1, true);
	getNextComponentZ((ComponentNetherBridgeStartPiece*)component, lst, rand, 0, 1, true);
}

ComponentNetherBridgeCrossing2* ComponentNetherBridgeCrossing2::createValidComponent(SCList& lst, Random& rand, int x, int y, int z, int mode, int type)
{
	StructureBB aabb = StructureBB::getComponentToAddBoundingBox(x, y, z, -1, 0, 0, 5, 7, 5, mode);
	if (isAboveGround(aabb) && StructureComponent::findIntersecting(lst, aabb) == NULL)
		return LordNew ComponentNetherBridgeCrossing2(type, rand, aabb, mode);
	return NULL;
}

bool ComponentNetherBridgeCrossing2::addComponentParts(World* pWorld, Random& rand, const StructureBB& aabb)
{
	fillWithBlocks(pWorld, aabb, 0, 0, 0, 4, 1, 4, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 0, 2, 0, 4, 5, 4, 0, 0, false);
	fillWithBlocks(pWorld, aabb, 0, 2, 0, 0, 5, 0, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 4, 2, 0, 4, 5, 0, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 0, 2, 4, 0, 5, 4, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 4, 2, 4, 4, 5, 4, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 0, 6, 0, 4, 6, 4, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);

	for (int var4 = 0; var4 <= 4; ++var4)
	{
		for (int var5 = 0; var5 <= 4; ++var5)
		{
			fillCurrentPositionBlocksDownwards(pWorld, BLOCK_ID_NETHER_BRICK, 0, var4, -1, var5, aabb);
		}
	}

	return true;
}

ComponentNetherBridgeNetherStalkRoom::ComponentNetherBridgeNetherStalkRoom(int type, Random& rand, const StructureBB& aabb, int mode)
	: ComponentNetherBridgePiece(type)
{
	m_coordBaseMode = mode;
	m_boundingBox = aabb;
}

void ComponentNetherBridgeNetherStalkRoom::buildComponent(StructureComponent* component, SCList& lst, Random& rand)
{
	getNextComponentNormal((ComponentNetherBridgeStartPiece*)component, lst, rand, 5, 3, true);
	getNextComponentNormal((ComponentNetherBridgeStartPiece*)component, lst, rand, 5, 11, true);
}

ComponentNetherBridgeNetherStalkRoom* ComponentNetherBridgeNetherStalkRoom::createValidComponent(SCList& lst, Random& rand, int x, int y, int z, int mode, int type)
{
	StructureBB aabb = StructureBB::getComponentToAddBoundingBox(x, y, z, -5, -3, 0, 13, 14, 13, mode);
	if (isAboveGround(aabb) && StructureComponent::findIntersecting(lst, aabb) == NULL)
		return LordNew ComponentNetherBridgeNetherStalkRoom(type, rand, aabb, mode);
	return NULL;
}

bool ComponentNetherBridgeNetherStalkRoom::addComponentParts(World* pWorld, Random& rand, const StructureBB& aabb)
{
	fillWithBlocks(pWorld, aabb, 0, 3, 0, 12, 4, 12, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 0, 5, 0, 12, 13, 12, 0, 0, false);
	fillWithBlocks(pWorld, aabb, 0, 5, 0, 1, 12, 12, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 11, 5, 0, 12, 12, 12, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 2, 5, 11, 4, 12, 12, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 8, 5, 11, 10, 12, 12, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 5, 9, 11, 7, 12, 12, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 2, 5, 0, 4, 12, 1, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 8, 5, 0, 10, 12, 1, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 5, 9, 0, 7, 12, 1, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 2, 11, 2, 10, 12, 10, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	int var4;

	for (var4 = 1; var4 <= 11; var4 += 2)
	{
		fillWithBlocks(pWorld, aabb, var4, 10, 0, var4, 11, 0, BLOCK_ID_NETHER_FENCE, BLOCK_ID_NETHER_FENCE, false);
		fillWithBlocks(pWorld, aabb, var4, 10, 12, var4, 11, 12, BLOCK_ID_NETHER_FENCE, BLOCK_ID_NETHER_FENCE, false);
		fillWithBlocks(pWorld, aabb, 0, 10, var4, 0, 11, var4, BLOCK_ID_NETHER_FENCE, BLOCK_ID_NETHER_FENCE, false);
		fillWithBlocks(pWorld, aabb, 12, 10, var4, 12, 11, var4, BLOCK_ID_NETHER_FENCE, BLOCK_ID_NETHER_FENCE, false);
		placeBlockAtCurrentPosition(pWorld, BLOCK_ID_NETHER_BRICK, 0, var4, 13, 0, aabb);
		placeBlockAtCurrentPosition(pWorld, BLOCK_ID_NETHER_BRICK, 0, var4, 13, 12, aabb);
		placeBlockAtCurrentPosition(pWorld, BLOCK_ID_NETHER_BRICK, 0, 0, 13, var4, aabb);
		placeBlockAtCurrentPosition(pWorld, BLOCK_ID_NETHER_BRICK, 0, 12, 13, var4, aabb);
		placeBlockAtCurrentPosition(pWorld, BLOCK_ID_NETHER_FENCE, 0, var4 + 1, 13, 0, aabb);
		placeBlockAtCurrentPosition(pWorld, BLOCK_ID_NETHER_FENCE, 0, var4 + 1, 13, 12, aabb);
		placeBlockAtCurrentPosition(pWorld, BLOCK_ID_NETHER_FENCE, 0, 0, 13, var4 + 1, aabb);
		placeBlockAtCurrentPosition(pWorld, BLOCK_ID_NETHER_FENCE, 0, 12, 13, var4 + 1, aabb);
	}

	placeBlockAtCurrentPosition(pWorld, BLOCK_ID_NETHER_FENCE, 0, 0, 13, 0, aabb);
	placeBlockAtCurrentPosition(pWorld, BLOCK_ID_NETHER_FENCE, 0, 0, 13, 12, aabb);
	placeBlockAtCurrentPosition(pWorld, BLOCK_ID_NETHER_FENCE, 0, 0, 13, 0, aabb);
	placeBlockAtCurrentPosition(pWorld, BLOCK_ID_NETHER_FENCE, 0, 12, 13, 0, aabb);

	for (var4 = 3; var4 <= 9; var4 += 2)
	{
		fillWithBlocks(pWorld, aabb, 1, 7, var4, 1, 8, var4, BLOCK_ID_NETHER_FENCE, BLOCK_ID_NETHER_FENCE, false);
		fillWithBlocks(pWorld, aabb, 11, 7, var4, 11, 8, var4, BLOCK_ID_NETHER_FENCE, BLOCK_ID_NETHER_FENCE, false);
	}

	var4 = getMetadataWithOffset(BLOCK_ID_STAIRS_NETHER_BRICK, 3);
	int var5;
	int var6;
	int var7;

	for (var5 = 0; var5 <= 6; ++var5)
	{
		var6 = var5 + 4;

		for (var7 = 5; var7 <= 7; ++var7)
		{
			placeBlockAtCurrentPosition(pWorld, BLOCK_ID_STAIRS_NETHER_BRICK, var4, var7, 5 + var5, var6, aabb);
		}

		if (var6 >= 5 && var6 <= 8)
		{
			fillWithBlocks(pWorld, aabb, 5, 5, var6, 7, var5 + 4, var6, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
		}
		else if (var6 >= 9 && var6 <= 10)
		{
			fillWithBlocks(pWorld, aabb, 5, 8, var6, 7, var5 + 4, var6, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
		}

		if (var5 >= 1)
		{
			fillWithBlocks(pWorld, aabb, 5, 6 + var5, var6, 7, 9 + var5, var6, 0, 0, false);
		}
	}

	for (var5 = 5; var5 <= 7; ++var5)
	{
		placeBlockAtCurrentPosition(pWorld, BLOCK_ID_STAIRS_NETHER_BRICK, var4, var5, 12, 11, aabb);
	}

	fillWithBlocks(pWorld, aabb, 5, 6, 7, 5, 7, 7, BLOCK_ID_NETHER_FENCE, BLOCK_ID_NETHER_FENCE, false);
	fillWithBlocks(pWorld, aabb, 7, 6, 7, 7, 7, 7, BLOCK_ID_NETHER_FENCE, BLOCK_ID_NETHER_FENCE, false);
	fillWithBlocks(pWorld, aabb, 5, 13, 12, 7, 13, 12, 0, 0, false);
	fillWithBlocks(pWorld, aabb, 2, 5, 2, 3, 5, 3, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 2, 5, 9, 3, 5, 10, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 2, 5, 4, 2, 5, 8, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 9, 5, 2, 10, 5, 3, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 9, 5, 9, 10, 5, 10, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 10, 5, 4, 10, 5, 8, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	var5 = getMetadataWithOffset(BLOCK_ID_STAIRS_NETHER_BRICK, 0);
	var6 = getMetadataWithOffset(BLOCK_ID_STAIRS_NETHER_BRICK, 1);
	placeBlockAtCurrentPosition(pWorld, BLOCK_ID_STAIRS_NETHER_BRICK, var6, 4, 5, 2, aabb);
	placeBlockAtCurrentPosition(pWorld, BLOCK_ID_STAIRS_NETHER_BRICK, var6, 4, 5, 3, aabb);
	placeBlockAtCurrentPosition(pWorld, BLOCK_ID_STAIRS_NETHER_BRICK, var6, 4, 5, 9, aabb);
	placeBlockAtCurrentPosition(pWorld, BLOCK_ID_STAIRS_NETHER_BRICK, var6, 4, 5, 10, aabb);
	placeBlockAtCurrentPosition(pWorld, BLOCK_ID_STAIRS_NETHER_BRICK, var5, 8, 5, 2, aabb);
	placeBlockAtCurrentPosition(pWorld, BLOCK_ID_STAIRS_NETHER_BRICK, var5, 8, 5, 3, aabb);
	placeBlockAtCurrentPosition(pWorld, BLOCK_ID_STAIRS_NETHER_BRICK, var5, 8, 5, 9, aabb);
	placeBlockAtCurrentPosition(pWorld, BLOCK_ID_STAIRS_NETHER_BRICK, var5, 8, 5, 10, aabb);
	fillWithBlocks(pWorld, aabb, 3, 4, 4, 4, 4, 8, BLOCK_ID_SLOW_SAND, BLOCK_ID_SLOW_SAND, false);
	fillWithBlocks(pWorld, aabb, 8, 4, 4, 9, 4, 8, BLOCK_ID_SLOW_SAND, BLOCK_ID_SLOW_SAND, false);
	fillWithBlocks(pWorld, aabb, 3, 5, 4, 4, 5, 8, BLOCK_ID_NETHER_STALK, BLOCK_ID_NETHER_STALK, false);
	fillWithBlocks(pWorld, aabb, 8, 5, 4, 9, 5, 8, BLOCK_ID_NETHER_STALK, BLOCK_ID_NETHER_STALK, false);
	fillWithBlocks(pWorld, aabb, 4, 2, 0, 8, 2, 12, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 0, 2, 4, 12, 2, 8, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 4, 0, 0, 8, 1, 3, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 4, 0, 9, 8, 1, 12, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 0, 0, 4, 3, 1, 8, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	fillWithBlocks(pWorld, aabb, 9, 0, 4, 12, 1, 8, BLOCK_ID_NETHER_BRICK, BLOCK_ID_NETHER_BRICK, false);
	int var8;

	for (var7 = 4; var7 <= 8; ++var7)
	{
		for (var8 = 0; var8 <= 2; ++var8)
		{
			fillCurrentPositionBlocksDownwards(pWorld, BLOCK_ID_NETHER_BRICK, 0, var7, -1, var8, aabb);
			fillCurrentPositionBlocksDownwards(pWorld, BLOCK_ID_NETHER_BRICK, 0, var7, -1, 12 - var8, aabb);
		}
	}

	for (var7 = 0; var7 <= 2; ++var7)
	{
		for (var8 = 4; var8 <= 8; ++var8)
		{
			fillCurrentPositionBlocksDownwards(pWorld, BLOCK_ID_NETHER_BRICK, 0, var7, -1, var8, aabb);
			fillCurrentPositionBlocksDownwards(pWorld, BLOCK_ID_NETHER_BRICK, 0, 12 - var7, -1, var8, aabb);
		}
	}

	return true;
}

ComponentNetherBridgeStartPiece::ComponentNetherBridgeStartPiece(Random& rand, int x, int z)
	: ComponentNetherBridgeCrossing3(rand, x, z)
	, theNetherBridgePieceWeight(NULL)
{
	SNBPWArr& pWeights = StructureNetherBridgePieces::getPrimaryComponents();
	int var5 = pWeights.size();
	int var6;
	StructureNetherBridgePieceWeight* var7;

	for (var6 = 0; var6 < var5; ++var6)
	{
		var7 = pWeights[var6];
		var7->field_78827_c = 0;
		primaryWeights.push_back(var7);
	}

	SNBPWArr& pWeights2 = StructureNetherBridgePieces::getSecondaryComponents();
	var5 = pWeights2.size();

	for (var6 = 0; var6 < var5; ++var6)
	{
		var7 = pWeights2[var6];
		var7->field_78827_c = 0;
		secondaryWeights.push_back(var7);
	}
}

}
