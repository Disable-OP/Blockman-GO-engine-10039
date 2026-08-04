#include "TessMeshCollection.h"

#include "cWorld/WorldClient.h"
#include "cChunk/ChunkClient.h"
#include "World/WorldProvider.h"

#include "Block/BM_Material.h"
#include "Block/BlockManager.h"
#include "Block/Block.h"
#include "Chunk/ChunkService.h"

namespace BLOCKMAN
{

TessMeshTest::TessMeshTest(WorldClient* pWorld)
{
	worldObj = pWorld;
}


bool TessMeshTest::reSetSection(const BlockPos& sectionBegin, int failCnt)
{
	chunkX = sectionBegin.x >> 4;
	chunkY = sectionBegin.y >> 4;
	chunkZ = sectionBegin.z >> 4;
	startPos = sectionBegin;
	isEmpty = true;
	m_isInAsync = false;

	Vector3i sectionPos(chunkX - 1, chunkY - 1, chunkZ - 1);
	ChunkPtr chunks[9];
	SectionClient* sections[27] = { nullptr };
	ChunkService* chunkService = worldObj->getChunkService();

	for (int iz = 0; iz < 3; ++iz)
	{
		for (int ix = 0; ix < 3; ++ix)
		{
			int x = ix + sectionPos.x;
			int z = iz + sectionPos.z;
			if (chunkService->chunkInCache(x, z))
				chunks[iz * 3 + ix] = chunkService->getChunk(x, z);
			else if (!chunkService->ayncGetExists(x, z))
				chunkService->getChunkAsync(x, z);
		}
	}

	for (int iz = 0; iz < 3; ++iz)
	{
		for (int ix = 0; ix < 3; ++ix)
		{
			ChunkPtr pChunk = chunks[iz * 3 + ix];
			if (pChunk == nullptr)
			{
				m_isInAsync = true;
				return false;
			}

			if (!pChunk->getAreLevelsEmpty(startPos.y - 1, startPos.y + 17))
				isEmpty = false;
		}
	}

	if (isEmpty)
		return false;

	chunkObj = chunks[4];
	int realCount = 0;

	for (int iz = 0; iz < 3; ++iz)
	{
		for (int ix = 0; ix < 3; ++ix)
		{
			ChunkPtr pChunk = chunks[iz * 3 + ix];
			for (int iy = 0; iy < 3; ++iy)
			{
				int sectionY = sectionPos.y + iy;
				if (sectionY >= 0 && sectionY < 16)
				{
					int idx = iz * 9 + iy * 3 + ix;
					sections[idx] = (SectionClient*)(pChunk->getSectionArray()[sectionPos.y + iy]);
					if (sections[idx] && sections[idx]->isVisibilityBuild() == false && failCnt % 10 != 0)
						return false;
					realCount++;
				}
			}
		}
	}

	return true;
}

TessMeshCollection::TessMeshCollection(WorldClient* pWorld)
{
	worldObj = pWorld;
}

bool TessMeshCollection::reSetSection(const BlockPos& sectionBegin)
{
	chunkX = sectionBegin.x >> 4;
	chunkY = sectionBegin.y >> 4;
	chunkZ = sectionBegin.z >> 4;
	startPos = sectionBegin;
	isEmpty = true;
	m_isInAsync = false;

	Vector3i sectionPos(chunkX-1, chunkY-1, chunkZ-1);
	ChunkPtr chunks[9];
	SectionClient* sections[27] = { nullptr };
	ChunkService* chunkService = worldObj->getChunkService();

	for (int iz = 0; iz < 3; ++iz)
	{
		for (int ix = 0; ix < 3; ++ix)
		{
			int x = ix + sectionPos.x;
			int z = iz + sectionPos.z;
			if (chunkService->chunkInCache(x, z))
				chunks[iz * 3 + ix] = chunkService->getChunk(x, z);
			else if (!chunkService->ayncGetExists(x, z))
				chunkService->getChunkAsync(x, z);
		}
	}

	for (int iz = 0; iz < 3; ++iz)
	{
		for (int ix = 0; ix < 3; ++ix)
		{
			ChunkPtr pChunk = chunks[iz * 3 + ix];
			if (pChunk == NULL)
			{
				m_isInAsync = true;
				return false;
			}

			if (!pChunk->getAreLevelsEmpty(startPos.y - 1, startPos.y + 17))
				isEmpty = false;
		}
	}

	if (isEmpty)
		return false;

	chunkObj = chunks[4];
	int realCount = 0;

	for (int iz = 0; iz < 3; ++iz)
	{
		for (int ix = 0; ix < 3; ++ix)
		{
			ChunkPtr pChunk = chunks[iz * 3 + ix];
			for (int iy = 0; iy < 3; ++iy)
			{
				int sectionY = sectionPos.y + iy;
				if (sectionY >= 0 && sectionY < 16)
				{
					int idx = iz * 9 + iy * 3 + ix;
					sections[idx] = (SectionClient*)(pChunk->getSectionArray()[sectionPos.y + iy]);
					realCount++;
				}
			}
		}
	}

	memset(blocks, 0, sizeof(ui32) * 20 * 20 * 20);

	for (int iz = 0; iz < 20; ++iz)
	{
		for (int iy = 0; iy < 20; ++iy)
		{
			for (int ix = 0; ix < 20; ++ix)
			{
				int tilex = (ix + 14) >> 4;
				int tiley = (iy + 14) >> 4;
				int tilez = (iz + 14) >> 4;

				SectionClient* pSection = sections[tilez * 9 + tiley * 3 + tilex];
				if (!pSection)
					continue;
				
				int biasx = (ix + 14) - (tilex << 4); // (ix+14) % 16
				int biasy = (iy + 14) - (tiley << 4); // (iy+14) % 16
				int biasz = (iz + 14) - (tilez << 4); // (iz+14) % 16

				blocks[iy][ix][iz] = pSection->getBlockUI32Data(BlockPos(biasx, biasy, biasz));
			}
		}
	}
		
	return true;
}

int TessMeshCollection::getBlockId(const BlockPos& pos)
{
	BlockPos offset = pos - startPos + BlockPos(2,2,2);
	LordAssert(offset.x >= 0 && offset.x < 20);
	LordAssert(offset.y >= 0 && offset.y < 20);
	LordAssert(offset.y >= 0 && offset.y < 20);
	
	return blocks[offset.y][offset.x][offset.z] & 0xFFFF;
}

int TessMeshCollection::getBlockIDAndMeta(const BlockPos& pos)
{
	BlockPos offset = pos - startPos + BlockPos(2,2,2);
	LordAssert(offset.x >= 0 && offset.x < 20);
	LordAssert(offset.y >= 0 && offset.y < 20);
	LordAssert(offset.y >= 0 && offset.y < 20);

	ui32 data = blocks[offset.y][offset.x][offset.z];
	ui32 id = data & 0xFFFF;
	ui32 meta = (data >> 16) & 0xFF;
	return (id << 16) | meta;;
}

float TessMeshCollection::getBrightness(const BlockPos& pos, int minBrightness)
{
	int light = getLightValue(pos);

	if (light < minBrightness)
	{
		light = minBrightness;
	}

	return worldObj->m_provider->lightBrightnessTable[light];
}

int TessMeshCollection::getLightBrightnessForSkyBlocks(const BlockPos& pos, int minBrightness)
{
	int sky_brightness = getSkyBlockTypeBrightness(SKY_LIGHT_VALUE, pos);
	int block_birghtness = getSkyBlockTypeBrightness(BLOCK_LIGHT_VALUE, pos);

	if (block_birghtness < minBrightness)
	{
		block_birghtness = minBrightness;
	}

	return sky_brightness << 20 | block_birghtness << 4;
}

float TessMeshCollection::getLightBrightness(const BlockPos& pos)
{
	return worldObj->m_provider->lightBrightnessTable[getLightValue(pos)];
}

bool TessMeshCollection::checkBlockPos(const BlockPos& pos)
{
	BlockPos offset = pos - startPos + BlockPos(2,2,2);
	if (offset.x >= 0 && offset.z >= 0 && offset.y >= 0 &&
		offset.x < 20 && offset.z < 20 && offset.y < 20)
		return true;
	return false;
}

int TessMeshCollection::getLightValueExt(const BlockPos& pos, bool flag)
{
	BlockPos offset = pos - startPos + BlockPos(2, 2, 2);
	if (offset.x >= 0 && offset.z >= 0 && offset.y >= 0 &&
		offset.x < 20 && offset.z < 20 && offset.y < 20)
	{
		if (flag)
		{
			int blockID = getBlockId(pos);

			if (blockID == BLOCK_ID_STONE_SINGLE_SLAB ||
				blockID == BLOCK_ID_WOOD_SINGLE_SLAB ||
				blockID == BLOCK_ID_REDSAND_STONE_SINGLESLAB ||
				blockID == BLOCK_ID_TILLED_FIELD ||
				blockID == BLOCK_ID_STAIRS_WOOD_OAK ||
				blockID == BLOCK_ID_STAIRS_COBBLE_STONE)
			{
				int light_up = getLightValueExt(pos.getPosY(), false);
				int light_east = getLightValueExt(pos.getPosX(), false);
				int light_west = getLightValueExt(pos.getNegX(), false);
				int light_south = getLightValueExt(pos.getPosZ(), false);
				int light_north = getLightValueExt(pos.getNegZ(), false);

				if (light_east > light_up)
					light_up = light_east;
				if (light_west > light_up)
					light_up = light_west;
				if (light_south > light_up)
					light_up = light_south;
				if (light_north > light_up)
					light_up = light_north;

				return light_up;
			}
		}
		return getSpecialBlockBrightness(SKY_LIGHT_VALUE, pos);
	}
	else
		return 15;
}

int TessMeshCollection::getBlockMeta(const BlockPos& pos)
{
	BlockPos offset = pos - startPos + BlockPos(2,2,2);
	LordAssert(offset.x >= 0 && offset.x < 20);
	LordAssert(offset.y >= 0 && offset.y < 20);
	LordAssert(offset.y >= 0 && offset.y < 20);

	return (blocks[offset.y][offset.x][offset.z] >> 16) & 0xFF;
}

const BM_Material& TessMeshCollection::getBlockMaterial(const BlockPos& pos)
{
	int blockID = getBlockId(pos);
	return blockID == 0 ? BM_Material::BM_MAT_air : BlockManager::sBlocks[blockID]->getMaterial();
}

BiomeGenBase* TessMeshCollection::getBiomeGenForCoords(int x, int z)
{
	return worldObj->getBiomeGenForCoords(x, z);
}

bool TessMeshCollection::isBlockOpaqueCube(const BlockPos& pos)
{
	Block* pBlock = BlockManager::sBlocks[getBlockId(pos)];
	return pBlock == NULL ? false : pBlock->isOpaqueCube();
}

bool TessMeshCollection::isBlockNormalCube(const BlockPos& pos)
{
	Block* pBlock = BlockManager::sBlocks[getBlockId(pos)];
	return pBlock == NULL ? false : pBlock->getMaterial().blocksMovement() && pBlock->renderAsNormalBlock();
}

bool TessMeshCollection::doesBlockHaveSolidTopSurface(const BlockPos& pos)
{
	Block* pBlock = BlockManager::sBlocks[getBlockId(pos)];
	return worldObj->isBlockTopFacingSurfaceSolid(pBlock, getBlockMeta(pos));
}

bool TessMeshCollection::isAirBlock(const BlockPos& pos)
{
	Block* pBlock = BlockManager::sBlocks[getBlockId(pos)];
	return pBlock == NULL;
}

int TessMeshCollection::getSkyBlockTypeBrightness(int lightValue, BlockPos pos)
{
	BlockPos offset = pos - startPos + BlockPos(2,2,2);
	if (offset.x >= 0 && offset.z >= 0 && offset.y >= 0 &&
		offset.x < 20 && offset.z < 20 && offset.y < 20)
	{

		if (BlockManager::sUseNeighborBrightness[getBlockId(pos)])
		{
			int light_up = getSpecialBlockBrightness(lightValue, pos.getPosY());
			int light_east = getSpecialBlockBrightness(lightValue, pos.getPosX());
			int light_west = getSpecialBlockBrightness(lightValue, pos.getNegX());
			int light_south = getSpecialBlockBrightness(lightValue, pos.getPosZ());
			int light_north = getSpecialBlockBrightness(lightValue, pos.getNegZ());

			if (light_east > light_up)
				light_up = light_east;
			if (light_west > light_up)
				light_up = light_west;
			if (light_south > light_up)
				light_up = light_south;
			if (light_north > light_up)
				light_up = light_north;

			return light_up;
		}
		else
		{
			return getSpecialBlockBrightness(lightValue, pos);
		}
	}
	return lightValue;
}

int TessMeshCollection::getSpecialBlockBrightness(int lightValue, BlockPos pos)
{
	BlockPos offset = pos - startPos + BlockPos(2,2,2);
	if (offset.x >= 0 && offset.z >= 0 && offset.y >= 0 &&
		offset.x < 20 && offset.z < 20 && offset.y < 20)
	{
		ui32 data = blocks[offset.y][offset.x][offset.z];
		if (lightValue == SKY_LIGHT_VALUE)
			return (data >> 24) & 0xF;
		else if (lightValue == BLOCK_LIGHT_VALUE)
			return (data >> 28) & 0xF;
		return lightValue;
	}
	return 0;
}

int TessMeshCollection::isBlockProvidingPowerTo(const BlockPos& pos, int side)
{
	int blockID = getBlockId(pos);
	return blockID == 0 ? 0 : BlockManager::sBlocks[blockID]->isProvidingStrongPower(this, pos, side);
}

TileEntity* TessMeshCollection::getBlockTileEntity(const BlockPos& pos)
{
	return chunkObj->getChunkBlockTileEntity(pos.getChunkPos());
}


}
