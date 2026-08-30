#include "Chunk.h"
#include "World/Section.h"
#include "World/World.h"
#include "World/WorldChunkManager.h"
#include "World/WorldProvider.h"
#include "World/NibbleArray.h"

#include "Block/Block.h"
#include "Block/Blocks.h"
#include "TileEntity/TileEntity.h"
#include "WorldGenerator/BiomeGen.h"
#include "Block/BlockManager.h"
#include "Entity/Entity.h"
#include "Entity/EntityPlayer.h"
#include "Trigger/TriggerManagers.h"

namespace BLOCKMAN
{

void Chunk::initialize()
{
	m_isChunkLoaded = false;
	m_pWorld = NULL;
	m_posX = 0;
	m_posZ = 0;
	m_isTerrainPopulated = false;
	m_isModified = false;
	m_isPrepared = false;
	m_hasEntities = false;
	m_lastSaveTime = 0LL;
	m_sendUpdates = false;
	m_inhabitedTime = 0LL;

	// entityLists = new List[16];

	m_entities.resize(16);
	m_precipitationHeightMap = (int*)LordMalloc(sizeof(int) * 256);
	for (int i = 0; i < 256; ++i)
	{
		m_precipitationHeightMap[i] = INVALID_PRECIPITATION;
		m_biomeArray[i] = -1;
	}
}

Chunk::Chunk(World* pWorld, int x, int z)
{
	initialize();
	
	m_pWorld = pWorld;
	m_posX = x;
	m_posZ = z;
}

Chunk::~Chunk()
{
	LordSafeFree(m_precipitationHeightMap);

	Section* pSection = NULL;
	for (int i = 0; i < 16; ++i)
	{
		LordSafeDelete(m_pSections[i]);
	}
}

void Chunk::setData(i8* pdata, int len)
{
	int height = len / 256;

	for (int x = 0; x < 16; ++x)
	{
		for (int z = 0; z < 16; ++z)
		{
			for (int y = 0; y < height; ++y)
			{
				int blockID = pdata[x << 11 | z << 7 | y];

				if (blockID != 0)
				{
					int yBase = y >> 4;

					if (!m_pSections[yBase])
					{
						m_pSections[yBase] = CreateSection(m_posX, m_posZ, yBase << 4);
					}

					m_pSections[yBase]->setBlockID(BlockPos(x, y & 15, z), blockID);
				}
			}
		}
	}
}

Section* Chunk::CreateSection(int x, int z, int ybase)
{
	return LordNew Section(ybase);
}

int Chunk::getTopFilledSegment()
{
	for (int i = 15; i >= 0; --i)
	{
		if (m_pSections[i] != NULL)
		{
			return m_pSections[i]->getYLocation();
		}
	}

	return 0;
}

void Chunk::setSectionXZ()
{
	for (int i = 15; i >= 0; --i)
	{
		if (m_pSections[i] != NULL)
		{
			m_pSections[i]->setPosXZ(m_posX, m_posZ);
		}
	}
}

int Chunk::updateHeightMap(const BlockPos& pos)
{
	int x = pos.x;
	int y = pos.y;
	int z = pos.z;	
	int top = getTopFilledSegment();
	int height = top + 16 - 1;
	

	while (height > 0 && getBlockLightOpacity(BlockPos(x, height, z)) == 0)
	{
		--height;
	}
	++height;
	
	return m_heightMap[z << 4 | x] = height;
	
}

void Chunk::generateHeightMap()
{
	int top = getTopFilledSegment();

	for (int ix = 0; ix < 16; ++ix)
	{		
		for (int iz = 0; iz < 16; ++iz)
		{
			m_precipitationHeightMap[ix + (iz << 4)] = INVALID_PRECIPITATION;
			// FIX [SYMPTOM-4]: getTopFilledSegment() returns the top section's
			// BLOCK-space base (e.g. 240 for section 15). The scan must start
			// at top + 15 (the highest block of that section — exactly what
			// updateHeightMap uses). "top * 16 - 1" started 16x higher (e.g.
			// y=3839), so every column first walked ~3800 all-air lookups:
			// ~950,000 wasted opacity queries per generated/loaded chunk — a
			// major slice of the server's chunk-serving cost.
			int height = top + 16 - 1;

			while (height > 0 && getBlockLightOpacity(BlockPos(ix, height, iz)) == 0)
			{
				--height;
			}
			++height;

			m_heightMap[iz << 4 | ix] = height;
								
			
		}
	}

	m_isModified = true;
	m_isPrepared = false;
}

int Chunk::getBlockLightOpacity(const BlockPos& pos)
{
	return BlockManager::sLightOpacity[getBlockID(pos)]; 
}

int Chunk::getBlockID(const BlockPos& pos)
{
	if (pos.y >> 4 >= 16)
	{
		return 0;
	}
	else
	{
		Section* pSection = m_pSections[pos.y >> 4];
		return pSection != NULL ? pSection->getBlockID(pos.getSection()) : 0;
	}
}

int Chunk::getBlockMeta(const BlockPos& pos)
{
	if (pos.y >> 4 >= 16)
	{
		return 0;
	}
	else
	{
		Section* pSection = m_pSections[pos.y >> 4];
		return pSection != NULL ? pSection->getBlockMeta(pos.getSection()) : 0;
	}
}

int Chunk::getBlockIDAndMeta(const BlockPos& pos)
{
	if (pos.y >> 4 >= 16)
		return 0;

	Section* pSection = m_pSections[pos.y >> 4];
	if (!pSection)
		return 0;

	BlockPos secpos = pos.getSection();
	int id = pSection->getBlockID(secpos);
	int meta = pSection->getBlockMeta(secpos);
	return (id << 16) | meta;
}

bool Chunk::setBlocks(const BlockModifys& modifys)
{
	Block** blocklst = BlockManager::sBlocks;

	for (auto it : modifys)
	{
		BlockPos chunkPos(it.pos.x & 15, it.pos.y, it.pos.z & 15);
		int cur_blockID = getBlockID(chunkPos);
		int cur_metadata = getBlockMeta(chunkPos);

		if (cur_blockID == it.id && cur_metadata == it.meta)
			continue;

		int yBase = it.pos.y >> 4;

		if (it.id != 0)
		{
			if (!m_pSections[yBase])
			{
				m_pSections[yBase] = CreateSection(m_posX, m_posZ, yBase << 4);
			}
		}

		m_pSections[yBase]->setBlockID(chunkPos.getSection(), it.id);
		if (cur_blockID != 0)
		{
			if (!m_pWorld->m_isClient)
			{
				blocklst[cur_blockID]->breakBlock(m_pWorld, it.pos, cur_blockID, cur_metadata);
			}
			else if (dynamic_cast<ITileEntityProvider*>(blocklst[cur_blockID]) && cur_blockID != it.id)
			{
				m_pWorld->removeBlockTileEntity(it.pos);
			}
		}

		if (m_pSections[yBase]->getBlockID(chunkPos.getSection()) != it.id)
			continue;

		m_pSections[yBase]->setBlockMeta(chunkPos.getSection(), it.meta);

		TileEntity* pEntity = NULL;

		if (it.id != 0)
		{
			if (!m_pWorld->m_isClient)
			{
				blocklst[it.id]->onBlockAdded(m_pWorld, it.pos);
			}

			ITileEntityProvider* pTileEntityProvider = dynamic_cast<ITileEntityProvider*>(blocklst[it.id]);
			if (pTileEntityProvider)
			{
				pEntity = getChunkBlockTileEntity(chunkPos);

				if (pEntity == NULL)
				{
					pEntity = pTileEntityProvider->createNewTileEntity(m_pWorld);
					m_pWorld->setBlockTileEntity(chunkPos, pEntity);
				}

				if (pEntity)
				{
					pEntity->updateContainingBlockInfo();
				}
			}
		}
		else if (cur_blockID > 0 && dynamic_cast<ITileEntityProvider*>(blocklst[cur_blockID]))
		{
			pEntity = getChunkBlockTileEntity(it.pos);

			if (pEntity != NULL)
			{
				pEntity->updateContainingBlockInfo();
			}
		}
	}

	return true;
}

bool Chunk::setBlockIDAndMeta(const BlockPos& pos, int blockID, int medatada, bool immediate)
{
	int idx = pos.z << 4 | pos.x;
	if (pos.y >= m_precipitationHeightMap[idx] - 1)
	{
		m_precipitationHeightMap[idx] = INVALID_PRECIPITATION;
	}

	int height = m_heightMap[idx];
	int cur_blockID = getBlockID(pos);
	int cur_metadata = getBlockMeta(pos);
	Block** blocklst = BlockManager::sBlocks;

	if (cur_blockID == blockID && cur_metadata == medatada)
	{
		return false;
	}
	else
	{
		Section* pSection = m_pSections[pos.y >> 4];
		bool bChanged = false;

		if (pSection == NULL)
		{
			if (blockID == 0)
			{
				return false;
			}

			pSection = m_pSections[pos.y >> 4] = CreateSection(m_posX, m_posZ, pos.y >> 4 << 4);
			bChanged = pos.y >= height;
		}

		int x_world = m_posX * 16 + pos.x;
		int z_world = m_posZ * 16 + pos.z;
		BlockPos wPos(x_world, pos.y, z_world);

		if (cur_blockID != 0 && !m_pWorld->m_isClient)
		{
			blocklst[cur_blockID]->onSetBlockIDWithMetaData(m_pWorld, wPos, cur_metadata);
		}

		pSection->setBlockID(pos.getSection(), blockID);

		if (cur_blockID != 0)
		{
			if (!m_pWorld->m_isClient)
			{
				blocklst[cur_blockID]->breakBlock(m_pWorld, wPos, cur_blockID, cur_metadata);
			}
			else if (dynamic_cast<ITileEntityProvider*>(blocklst[cur_blockID]) && cur_blockID != blockID)
			{
				m_pWorld->removeBlockTileEntity(wPos);
			}
		}

		BlockPos sectionPos = pos.getSection();
		if (pSection->getBlockID(sectionPos) != blockID)
		{
			return false;
		}
		else
		{
			TRIGGER_MANAGER::notifyBlockChange(pos, cur_blockID, blockID);
			pSection->setBlockMeta(sectionPos, medatada);
				
			TileEntity* pEntity = NULL;

			if (blockID != 0)
			{
				if (!m_pWorld->m_isClient)
				{
					blocklst[blockID]->onBlockAdded(m_pWorld, BlockPos(x_world, pos.y, z_world));
				}

				ITileEntityProvider* pTileEntityProvider = dynamic_cast<ITileEntityProvider*>(blocklst[blockID]);
				if (pTileEntityProvider)
				{
					pEntity = getChunkBlockTileEntity(pos);

					if (pEntity == NULL)
					{
						pEntity = pTileEntityProvider->createNewTileEntity(m_pWorld);
						m_pWorld->setBlockTileEntity(wPos, pEntity);
					}

					if (pEntity)
					{
						pEntity->updateContainingBlockInfo();
					}
				}
			}
			else if (cur_blockID > 0 && dynamic_cast<ITileEntityProvider*>(blocklst[cur_blockID]))
			{
				pEntity = getChunkBlockTileEntity(pos);

				if (pEntity != NULL)
				{
					pEntity->updateContainingBlockInfo();
				}
			}

			m_isModified = true;
			m_isPrepared = false;
			return true;
		}
	}
}

bool Chunk::setBlockMeta(const BlockPos& pos, int metadata)
{
	Section* pSection = m_pSections[pos.y >> 4];
	BlockPos secPos = pos.getSection();

	if (pSection == NULL)
	{
		return false;
	}
	else
	{
		int meta = pSection->getBlockMeta(secPos);

		if (meta == metadata)
		{
			return false;
		}
		else
		{
			m_isModified = true;
			m_isPrepared = false;
			pSection->setBlockMeta(secPos, metadata);
			int id = pSection->getBlockID(secPos);

			if (id > 0)
			{
				Block* pBlock = BlockManager::sBlocks[id];
				ITileEntityProvider* pEntityProvider = dynamic_cast<ITileEntityProvider*>(pBlock);
				if (pEntityProvider)
				{
					TileEntity* pTileEntity = getChunkBlockTileEntity(pos);

					if (pTileEntity != nullptr)
					{
						pTileEntity->updateContainingBlockInfo();
						pTileEntity->blockMetadata = metadata;
					}
				}
			}

			return true;
		}
	}
}

void Chunk::addEntity(Entity* pEntity)
{
	m_hasEntities = true;
	int chunkX = int(Math::Floor(pEntity->position.x / 16.0f));
	int chunkZ = int(Math::Floor(pEntity->position.z / 16.0f));

	LordAssert(chunkX == m_posX && chunkZ == m_posZ);

	int sectionY = int(Math::Floor(pEntity->position.y / 16.0f));

	if (sectionY < 0)
	{
		sectionY = 0;
	}

	if (sectionY >= int(m_entities.size()))
	{
		sectionY = int(m_entities.size()) - 1;
	}

	pEntity->addedToChunk = true;
	pEntity->chunkCoord = Vector3i(m_posX, sectionY, m_posZ);
	m_entities[sectionY].insert(pEntity);
}

void Chunk::removeEntity(Entity* pEntity)
{
	removeEntityAtIndex(pEntity, pEntity->chunkCoord.y);
}

void Chunk::removeEntityAtIndex(Entity* pEntity, int index)
{
	if (index < 0)
		index = 0;

	if (index >= int(m_entities.size()))
		index = int(m_entities.size()) - 1;

	m_entities[index].erase(pEntity);
}

void Chunk::getEntitiesWithinAABBForEntity(Entity* pEntity, const Box& aabb, EntityArr& fillTo, bool includeWatchModePlayer)
{
	int y1 = int(Math::Floor((aabb.vMin.y - 2.0f) / 16.0f));
	int y2 = int(Math::Floor((aabb.vMax.y + 2.0f) / 16.0f));

	if (y1 < 0)
	{
		y1 = 0;
		y2 = Math::Max(y1, y2);
	}

	if (y2 >= int(m_entities.size()))
	{
		y2 = int(m_entities.size()) - 1;
		y1 = Math::Min(y1, y2);
	}

	for (int y = y1; y <= y2; ++y)
	{
		EntityList& entities = m_entities[y];

		for (EntityList::iterator it = entities.begin(); it != entities.end(); ++it)
		{
			Entity* pEnt = *it;

			if (!includeWatchModePlayer)
			{
				if (auto entityPlayer = dynamic_cast<EntityPlayer*>(pEnt))
				{
					if (entityPlayer->capabilities.isWatchMode)
					{
						continue;
					}
				}
			}

			if (pEnt != pEntity && pEnt->boundingBox.isIntersected(aabb))
			{
				fillTo.push_back(pEnt);
				int partsCount = pEnt->getPartsCount();

				if (partsCount > 0)
				{
					Entity** parts = pEnt->getParts();
					for (int i = 0; i < partsCount; ++i)
					{
						pEnt = parts[i];

						if (pEnt != pEntity && pEnt->boundingBox.isIntersected(aabb))
						{
							fillTo.push_back(pEnt);
						}
					}
				}
			}
		}
	}
}


void Chunk::getEntitiesWithinAABB(ui32 entityType, const Box& aabb, EntityArr& fillTo, bool includeWatchModePlayer)
{
	int y1 = int(Math::Floor((aabb.vMin.y - 2.0f) / 16.0f));
	int y2 = int(Math::Floor((aabb.vMax.y + 2.0f) / 16.0f));

	if (y1 < 0)
		y1 = 0;
	else if (y1 >= int(m_entities.size()))
		y1 = int(m_entities.size()) - 1;

	if (y2 >= int(m_entities.size()))
		y2 = int(m_entities.size()) - 1;
	else if (y2 < 0)
		y2 = 0;

	for (int i = y1; i <= y2; ++i)
	{
		EntityList& entities = m_entities[i];

		for (EntityList::iterator it = entities.begin(); it != entities.end(); ++it)
		{
			Entity* pEntity = *it;
			if (!pEntity)
				continue;

			if (!includeWatchModePlayer)
			{
				if (auto entityPlayer = dynamic_cast<EntityPlayer*>(pEntity))
				{
					if (entityPlayer->capabilities.isWatchMode)
					{
						continue;
					}
				}
			}

			if(pEntity->isClass(entityType) && pEntity->boundingBox.isIntersected(aabb))
			{
				fillTo.push_back(pEntity);
			}
		}
	}
}

TileEntity* Chunk::getChunkBlockTileEntity(const BlockPos& pos)
{
	ChunkPosition chunkPos(pos);
	
	TileEntity* ptileEntity = NULL;
	ChunkTileEntityMap::iterator it = m_tileEntityMap.find(chunkPos);
	if (it != m_tileEntityMap.end())
		ptileEntity = it->second;

	if (!ptileEntity)
	{
		int blockID = getBlockID(pos);

		if (blockID <= 0 || !BlockManager::sBlocks[blockID]->hasTileEntity())
		{
			return NULL;
		}

		if (!ptileEntity)
		{
			ptileEntity = (dynamic_cast<ITileEntityProvider*>(BlockManager::sBlocks[blockID]))->createNewTileEntity(m_pWorld);
			if (ptileEntity) {
				setChunkBlockTileEntity(pos, ptileEntity);
				m_pWorld->setBlockTileEntity(pos + BlockPos(m_posX * 16, 0, m_posZ * 16), ptileEntity, shared_from_this());
			}
		}

		it = m_tileEntityMap.find(chunkPos);
		if (it != m_tileEntityMap.end())
			ptileEntity = it->second;
	}

	if (ptileEntity != NULL && ptileEntity->isInvalid())
	{
		m_tileEntityMap.erase(it);
		return NULL;
	}
	else
	{
		return ptileEntity;
	}
	return NULL; // silence warning
}

void Chunk::addTileEntity(TileEntity* pTileEntity)
{
	BlockPos pos = pTileEntity->m_pos - BlockPos(m_posX * 16, 0, m_posZ * 16);
	setChunkBlockTileEntity(pos, pTileEntity);

	if (m_isChunkLoaded)
	{
		m_pWorld->m_loadedTileEntityList.insert(pTileEntity);
	}
}

void Chunk::setChunkBlockTileEntity(const BlockPos& pos, TileEntity* pTileEntity)
{
	ChunkPosition chunkPos(pos);
	pTileEntity->setWorldObj(m_pWorld);
	pTileEntity->m_pos = pos + BlockPos(m_posX * 16, 0, m_posZ * 16);

	ITileEntityProvider* pEntityProvider = dynamic_cast<ITileEntityProvider*>(BlockManager::sBlocks[getBlockID(pos)]);
	if (getBlockID(pos) != 0 && pEntityProvider)
	{
		ChunkTileEntityMap::iterator it = m_tileEntityMap.find(chunkPos);
		if (it != m_tileEntityMap.end())
		{
			it->second->invalidate();
		}

		pTileEntity->validate();
		m_tileEntityMap[chunkPos] = pTileEntity;
	}
}

void Chunk::removeChunkBlockTileEntity(const BlockPos& pos)
{
	ChunkPosition cp(pos);

	if (m_isChunkLoaded)
	{
		TileEntity* pTileEntity = NULL;
		
		ChunkTileEntityMap::iterator it = m_tileEntityMap.find(cp);
		if (it != m_tileEntityMap.end())
			pTileEntity = it->second;

		if (pTileEntity != NULL)
		{
			m_tileEntityMap.erase(it);
			pTileEntity->invalidate();
		}
	}
}

void Chunk::onChunkLoad()
{
	m_isChunkLoaded = true;
	// worldObj->addTileEntity(chunkTileEntityMap.values());

	/*
	for (int var1 = 0; var1 < this.entityLists.length; ++var1)
	{
		Iterator var2 = this.entityLists[var1].iterator();

		while (var2.hasNext())
		{
			Entity var3 = (Entity)var2.next();
			var3.func_110123_P();
		}

		this.worldObj.addLoadedEntities(this.entityLists[var1]);
	}
	*/
}

void Chunk::onChunkUnload()
{
	m_isChunkLoaded = false;

	for (ChunkTileEntityMap::iterator it = m_tileEntityMap.begin(); it != m_tileEntityMap.end(); ++it)
	{
		TileEntity* pTileEntity = it->second;
		m_pWorld->markTileEntityForDespawn(pTileEntity);
	}

	for (int i = 0; i < int(m_entities.size()); ++i)
	{
		m_pWorld->unloadEntities(m_entities[i]);
	}
}

bool Chunk::needsSaving(bool par1)
{
	if (par1)
	{
		if (m_hasEntities && m_pWorld->getTotalWorldTime() != m_lastSaveTime || m_isModified)
		{
			return true;
		}
	}
	else if (m_hasEntities && m_pWorld->getTotalWorldTime() >= m_lastSaveTime + 600L)
	{
		return true;
	}

	return m_isModified;
}

Random* Chunk::getRandomWithSeed(i64 par1)
{
	return LordNew Random(m_pWorld->getSeed() +
		(i64)(m_posX * m_posX * 4987142) +
		(i64)(m_posX * 5947611) +
		(i64)(m_posZ * m_posZ) * 4392871L +
		(i64)(m_posZ * 389711) ^ par1);
}

int Chunk::getPrecipitationHeight(int x, int z)
{
	int idx = x | z << 4;
	int height = m_precipitationHeightMap[idx];

	if (height == INVALID_PRECIPITATION)
	{
		int maxHeight = getTopFilledSegment() + 15;
		height = -1;

		while (maxHeight > 0 && height == -1)
		{
			int blockID = getBlockID(BlockPos(x, maxHeight, z));
			BM_Material mat = blockID == 0 ? BM_Material::BM_MAT_air : BlockManager::sBlocks[blockID]->getMaterial();

			if (!mat.blocksMovement() && !mat.isLiquid())
			{
				--maxHeight;
			}
			else
			{
				height = maxHeight + 1;
			}
		}

		m_precipitationHeightMap[idx] = height;
	}

	return height;
}

bool Chunk::getAreLevelsEmpty(int min, int max)
{
	if (min < 0)
	{
		min = 0;
	}

	if (max >= 256)
	{
		max = 255;
	}

	for (int i = min; i <= max; i += 16)
	{
		Section* section = m_pSections[i >> 4];

		if (section != NULL && !section->isEmpty())
		{
			return false;
		}
	}

	return true;
}

void Chunk::fillChunk(i8* data, int par2, int par3, bool par4)
{
	throw std::logic_error("method not implemented");
	//int bytesFilled = 0;
	//int sectionIndex;

	//for (sectionIndex = 0; sectionIndex < 16; ++sectionIndex)
	//{
	//	if ((par2 & 1 << sectionIndex) != 0)
	//	{
	//		if (m_pSections[sectionIndex] == NULL)
	//		{
	//			m_pSections[sectionIndex] = CreateSection(m_posX, m_posZ, sectionIndex << 4);
	//		}

	//		i8* var8 = m_pSections[sectionIndex]->getBlockLSBArray();
	//		memcpy(var8, data + bytesFilled, Section::BLOCK_ARRAY_SIZE);
	//		bytesFilled += Section::BLOCK_ARRAY_SIZE;
	//	}
	//	else if (par4 && m_pSections[sectionIndex] != NULL)
	//	{
	//		m_pSections[sectionIndex] = NULL;
	//	}
	//}

	//NibbleArray* pNibble;

	//for (sectionIndex = 0; sectionIndex < 16; ++sectionIndex)
	//{
	//	if ((par2 & 1 << sectionIndex) != 0 && m_pSections[sectionIndex] != NULL)
	//	{
	//		pNibble = m_pSections[sectionIndex]->getMetadataArray();
	//		memcpy(pNibble->m_data, data + bytesFilled, pNibble->m_length);
	//		bytesFilled += pNibble->m_length;
	//	}
	//}

	//
	//for (sectionIndex = 0; sectionIndex < 16; ++sectionIndex)
	//{
	//	if ((par3 & 1 << sectionIndex) != 0)
	//	{
	//		if (m_pSections[sectionIndex] == NULL)
	//		{
	//			bytesFilled += 2048;
	//		}
	//	}
	//}

	//if (par4)
	//{
	//	memcpy(m_biomeArray, data + bytesFilled, 256);
	//	int var10000 = bytesFilled + 256;
	//}

	//for (sectionIndex = 0; sectionIndex < 16; ++sectionIndex)
	//{
	//	if (m_pSections[sectionIndex] != NULL && (par2 & 1 << sectionIndex) != 0)
	//	{
	//		m_pSections[sectionIndex]->removeInvalidBlocks();
	//	}
	//}

	//generateHeightMap();

	//for (ChunkTileEntityMap::iterator it = m_tileEntityMap.begin(); it!=m_tileEntityMap.end(); ++it)
	//{
	//	it->second->updateContainingBlockInfo();
	//}
}

BiomeGenBase* Chunk::getBiomeGenForWorldCoords(int x, int z, WorldChunkManager* par3WorldChunkManager)
{
	int var4 = m_biomeArray[z << 4 | x] & 255;

	if (var4 == 255)
	{
		BiomeGenBase* var5 = par3WorldChunkManager->getBiomeGenAt((m_posX << 4) + x, (m_posZ << 4) + z);
		var4 = var5->m_ID;
		m_biomeArray[z << 4 | x] = (i8)(var4 & 255);
	}

	if (var4 >= 64)
	{
		return BiomeGenBase::plains;
	}
	else
	{
		return BiomeGenBase::biomeList[var4] == NULL ? BiomeGenBase::plains : BiomeGenBase::biomeList[var4];
	}
}

void Chunk::parseTileEntitiesTag(const NbtTagEvent & event, NbtParser & parser)
{
	LordAssert(parser.currentEvent().isListStart());
	for (parser.nextEvent(); !parser.currentEvent().isListEnd(); parser.nextEvent())
	{
		LordAssert(parser.currentEvent().isCompoundStart());
		parser.nextEvent();
		TileEntity* pTileEntity = nullptr;
		if (parser.currentEvent().isCompoundEnd())
		{
			continue;
		}
		if (parser.currentEvent().tagType == NbtTagType::TAG_STRING && String(parser.currentEvent().tagName) == "id")
		{
			pTileEntity = TileEntity::deserializeTileEntity(parser.currentEvent().stringValue, parser, false);
			if (!pTileEntity)
			{
				parser.jumpToEndEvent();
				continue;
			}
		}
		else
		{
			static bool hasLogged = false;
			if (!hasLogged)
			{
				hasLogged = true;
				LordLogWarning("The first tag of TileEntity is not \"id\", there will be performance loss. this warnings output only once");
			}
			std::unique_ptr<NBTTagCompound> root(LordNew NBTTagCompound);
			do
			{
				auto tag = NBTBase::read(parser);
				root->setTag(tag->getName(), tag);
				parser.nextEvent();
			} while (!parser.currentEvent().isCompoundEnd());
			pTileEntity = TileEntity::createAndLoadEntity(root.get());
			if (!pTileEntity)
			{
				continue;
			}
		}
		addTileEntity(pTileEntity);
		LordAssert(parser.currentEvent().isCompoundEnd());
	}
	LordAssert(parser.currentEvent().isListEnd());
}

void Chunk::generateTileEntitiesTag(const String & tagName, NbtGenerator & generator) const
{
	size_t listSize = 0;
	for (const auto& pair : m_tileEntityMap)
	{
		if (pair.second->serializable())
		{
			++listSize;
		}
	}
	generator.writeTagListStart(tagName, NbtTagType::TAG_COMPOUND, listSize);

	size_t count = 0;
	for (const auto& pair : m_tileEntityMap)
	{
		if (pair.second->serializeTileEntity(generator))
		{
			++count;
		}
	}

	if (count != listSize)
	{
		throw InvalidNbtFormatError("Value in size field of \"TileEntities\" tag is different from the number of sub tags written inside");
	}
}

}
