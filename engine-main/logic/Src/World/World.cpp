#include "World.h"
#include "Chunk/Chunk.h"
#include "ChunkCollection.h"
#include "WorldProvider.h"
#include "WorldChunkManager.h"
#include "Section.h"
#include "Common.h"
#include "Math/LordMath.h"

#include "Block/Blocks.h"
#include "Block/BlockManager.h"
#include "TileEntity/TileEntity.h"
#include "WorldGenerator/BiomeGen.h"
#include "Chunk/ChunkService.h"
#include "Entity/Entity.h"
#include "Entity/Explosion.h"
#include "Entity/EntityPlayer.h"
#include "Entity/PathFinder.h"
#include "Entity/EntityItem.h"
#include "Entity/EntityMerchant.h"
#include "Entity/EntityVehicle.h"
#include "Entity/EntityRankNpc.h"
#include "Entity/EntityAircraft.h"
#include "Entity/EntityActorNpc.h"
#include "Entity/EntitySessionNpc.h"
#include "Entity/EntityCreature.h"
#include "Entity/EntityBlockmanEmpty.h"
#include "Entity/EntityActorCannon.h"
#include "CprCallBackWrap.h"
#include "rapidjson/document.h"
#include "Script/Event/LogicScriptEvents.h"
#include "Entity/EntityBuildNpc.h"
#include "Setting/LogicSetting.h"
#include "Setting/ActorSizeSetting.h"
#include "Entity/EntityBird.h"
#include "Object/Root.h"
namespace BLOCKMAN
{
void World::init()
{
	m_scheduledUpdatesAreImmediate = false;
	m_skylightSubtracted = 0;
	m_lastLightningBolt = 0;
	m_difficultySetting = 0;
	m_provider = NULL;
	m_pChunkService = NULL;
	m_findingSpawnPoint = false;
	m_isClient = false;
	m_cprCallBackWrap = LordNew CprCallBackWrap(this);
}

void World::reset()
{
	m_pChunkService->reset();
}

World::World(const String& name, WorldProvider* _provider, const WorldSettings& settings, int loadRange)
	: m_WorldInfo(name, settings)
	, m_Rand()
{
	init();
	m_provider = _provider;
	m_provider->registerWorld(this);
	// saveHandler = par1ISaveHandler;
	// theProfiler = par5Profiler;
	// worldInfo = new WorldInfo(par4WorldSettings, par2Str);
	// provider = par3WorldProvider;
	// mapStorage = new MapStorage(par1ISaveHandler);
	// worldLogAgent = par6ILogAgent;
	// VillageCollection var7 = (VillageCollection)mapStorage.loadData(VillageCollection.class, "villages");

	/*
	if (var7 == null)
	{
	villageCollectionObj = new VillageCollection(this);
	mapStorage.setData("villages", villageCollectionObj);
	}
	else
	{
	villageCollectionObj = var7;
	villageCollectionObj.func_82566_a(this);
	}
	*/

	// par3WorldProvider.registerWorld(this);

	/** could not call the createChunkService in the constructor function. */
	// m_pChunkService = createChunkService(loadRange);
	calculateInitialSkylight();
}

World::~World()
{
	LordSafeDelete(m_pChunkService);

	for (auto it : m_loadedEntityMap)
	{
		LordDelete(it.second);
	}
	m_loadedEntityMap.clear();

	for (auto it : m_loadedTileEntityList)
	{
		LordSafeDelete(it);
	}

	LordSafeDelete(m_provider);
	LordSafeDelete(m_shop);
	if (m_manorControl)
	{
		LordSafeDelete(m_manorControl);
	}
	LordSafeDelete(m_cprCallBackWrap);
}

void World::createShop()
{
	m_shop = LordNew Shop();
}

int World::getFirstUncoveredBlock(int x, int z)
{
	BlockPos pos(x, 63, z);

	for (; !isAirBlock(pos.getPosY()); ++pos.y)
	{
		;
	}

	return getBlockId(pos);
}

int World::getBlockId(const BlockPos& pos)
{
	if (pos.x >= -MAX_WORLD_X && pos.z >= -MAX_WORLD_Z && pos.x < MAX_WORLD_X && pos.z < MAX_WORLD_Z)
	{
		if (pos.y < 0)
		{
			return 0;
		}
		else if (pos.y >= 256)
		{
			return 0;
		}
		else
		{
			ChunkPtr pChunk = NULL;

			try
			{
				pChunk = getChunkFromChunkCoords(pos.x >> 4, pos.z >> 4);
				if (pChunk == NULL)
				{
					LordLogError("ERROR: can not find chunk [%d,%d]", pos.x >> 4, pos.z >> 4);
					return 0;
				}
				return pChunk->getBlockID(pos.getChunkPos());
			}
			catch (...)
			{
				LordException("Can not found chunk, localtion(%d,%d,%d)", pos.x, pos.y, pos.z);
			}
		}
	}
	return 0;
}

bool World::blockHasTileEntity(const BlockPos& pos)
{
	int blockID = getBlockId(pos);

	Block** pBlockArr = BlockManager::sBlocks;
	LordAssert(pBlockArr);

	return pBlockArr[blockID] != NULL &&
		pBlockArr[blockID]->hasTileEntity();
}

bool World::blockExists(const BlockPos& pos)
{
	return pos.y >= 0 && pos.y < 256 ? chunkExists(pos.x >> 4, pos.z >> 4) : false;
}

bool World::doChunksNearChunkExist(const BlockPos& pos, int dis)
{
	BlockPos min = pos - dis;
	BlockPos max = pos + dis;
	return checkChunksExist(min, max);
}

bool World::checkChunksExist(const BlockPos& min, const BlockPos& max)
{
	if (max.y >= 0 && min.y < 256)
	{
		int minX = min.x >> 4;
		int minZ = min.z >> 4;
		int maxX = max.x >> 4;
		int maxZ = max.z >> 4;

		for (int x = minX; x <= maxX; ++x)
		{
			for (int z = minZ; z <= maxZ; ++z)
			{
				if (!chunkExists(x, z))
				{
					return false;
				}
			}
		}

		return true;
	}
	else
	{
		return false;
	}
}

bool World::chunkExists(int x, int z)
{
	LordAssert(m_pChunkService);
	return m_pChunkService->chunkInCache(x, z);
}

ChunkPtr World::getChunkFromBlockCoords(int x, int z)
{
	return getChunkFromChunkCoords(x >> 4, z >> 4);
}

ChunkPtr World::getChunkFromChunkCoords(int x, int z)
{
	LordAssert(m_pChunkService);
	return m_pChunkService->getChunk(x, z);
}

bool World::canLightningStrikeAt(const BlockPos& pos)
{
	// logic side. no raining.
	if (!m_WorldInfo.isRaining())
	{
		return false;
	} 
	else if (!canBlockSeeTheSky(pos))
	{
		return false;
	}
	else if (getPrecipitationHeight(pos.x, pos.z) > pos.y)
	{
		return false;
	}
	else
	{
		BiomeGenBase* pBiome = getBiomeGenForCoords(pos.x, pos.z);
		return pBiome->getEnableSnow() ? false : pBiome->canSpawnLightningBolt();
	}
}

bool World::canPlaceEntityOnSide(int blockID, const BlockPos& pos, bool par5, int side, Entity* pEntity, ItemStackPtr pStack)
{
	int thisID = getBlockId(pos);
	Block* thisBlock = BlockManager::sBlocks[thisID];
	Block* placeBlock = BlockManager::sBlocks[blockID];
	Box placeAABB = placeBlock->getCollisionBoundingBox(this, pos);

	if (par5)
	{
		placeAABB.reset();
	}

	if (!placeAABB.isNull() && !checkNoEntityCollision(placeAABB, pEntity))
	{
		return false;
	}
	else
	{
		if (thisBlock && (thisID == BLOCK_ID_WATERMOVING ||
			thisID == BLOCK_ID_WATERSTILL ||
			thisID == BLOCK_ID_LAVAMOVING ||
			thisID == BLOCK_ID_LAVASTILL ||
			thisID == BLOCK_ID_FIRE ||
			thisBlock->getMaterial().isReplaceable()))
		{
			thisBlock = NULL;
		}

		return thisBlock && thisBlock->getMaterial() == BM_Material::BM_MAT_circuits &&
			thisID == BLOCK_ID_ANVIL ? true : blockID > 0 && !thisBlock && placeBlock->canPlaceBlockOnSide(this, pos, side, pStack);
	}
}

bool World::setBlocks(int x, int z, const BlockModifys& modifys)
{
	ChunkPtr pChunk = getChunkFromChunkCoords(x, z);
	if (!pChunk)
		return false;

	return pChunk->setBlocks(modifys);
}

bool World::setBlock(const BlockPos& pos, int blockID, int metadata, int flag, bool immediate)
{
	if (!(pos.x >= -MAX_WORLD_X && pos.z >= -MAX_WORLD_Z && pos.x < MAX_WORLD_X && pos.z < MAX_WORLD_Z))
		return false;

	if (pos.y < 0)
	{
		return false;
	}
	else if (pos.y >= 256)
	{
		return false;
	}

	ChunkPtr pChunk = getChunkFromChunkCoords(pos.x >> 4, pos.z >> 4);
	int tblockID = 0;

	if ((flag & 1) != 0)
	{
		tblockID = pChunk->getBlockID(pos.getChunkPos());
	}

	bool modifyed = pChunk->setBlockIDAndMeta(pos.getChunkPos(), blockID, metadata, immediate);


	if (modifyed)
	{
		//add by maxicheng			
		pChunk->updateHeightMap(pos.getChunkPos());

		if ((flag & 2) != 0 && (!m_isClient || (flag & 4) == 0))
		{
			markBlockForUpdate(pos);
		}

		if (!m_isClient && (flag & 1) != 0)
		{
			notifyBlockChange(pos, tblockID);
			Block* pBlock = BlockManager::sBlocks[blockID];

			if (pBlock != NULL && pBlock->hasComparatorInputOverride())
			{
				updateComparatorOutputLevel(pos, blockID);
			}
		}
	}

	return modifyed;
}

void World::updateComparatorOutputLevel(const BlockPos& pos, int blockID)
{
	for (int i = 0; i < 4; ++i)
	{
		BlockPos dirPos = pos + dir_offset[i];
		int neighborID = getBlockId(dirPos);

		if (neighborID != 0)
		{
			Block* pBlock = BlockManager::sBlocks[neighborID];

			if (BlockManager::redstoneComparatorIdle->isSameDiode(neighborID))
			{
				pBlock->onNeighborBlockChange(this, dirPos, blockID);
			}
			else if (BlockManager::isNormalCube(neighborID))
			{
				dirPos += dir_offset[i];
				neighborID = getBlockId(dirPos);
				pBlock = BlockManager::sBlocks[neighborID];

				if (BlockManager::redstoneComparatorIdle->isSameDiode(neighborID))
				{
					pBlock->onNeighborBlockChange(this, dirPos, blockID);
				}
			}
		}
	}
}

const BM_Material& World::getBlockMaterial(const BlockPos& pos)
{
	int blockID = getBlockId(pos);

	Block** pBlockArr = BlockManager::sBlocks;
	LordAssert(pBlockArr);

	return blockID == 0 ? BM_Material::BM_MAT_air : pBlockArr[blockID]->getMaterial();
}

int World::getBlockMeta(const BlockPos& pos)
{
	if (!(pos.x >= -MAX_WORLD_X && pos.z >= -MAX_WORLD_Z && pos.x < MAX_WORLD_X && pos.z < MAX_WORLD_Z))
		return 0;

	if (pos.y < 0 || pos.y >= 256)
		return 0;

	ChunkPtr pChunk = getChunkFromChunkCoords(pos.x >> 4, pos.z >> 4);
	LordAssert(pChunk);

	return pChunk->getBlockMeta(pos.getChunkPos());
}

int World::getBlockIDAndMeta(const BlockPos& pos)
{
	if (!(pos.x >= -MAX_WORLD_X && pos.z >= -MAX_WORLD_Z && pos.x < MAX_WORLD_X && pos.z < MAX_WORLD_Z))
		return 0;

	if (pos.y < 0 || pos.y >= 256)
		return 0;

	ChunkPtr pChunk = getChunkFromChunkCoords(pos.x >> 4, pos.z >> 4);
	LordAssert(pChunk);

	return pChunk->getBlockIDAndMeta(pos.getChunkPos());
}

bool World::setBlockMetadataWithNotify(const BlockPos& pos, int metadata, int flag)
{
	if (!(pos.x >= -MAX_WORLD_X && pos.z >= -MAX_WORLD_Z && pos.x < MAX_WORLD_X && pos.z < MAX_WORLD_Z))
		return 0;
	if (pos.y < 0)
		return false;
	else if (pos.y >= 256)
		return false;

	ChunkPtr pChunk = getChunkFromChunkCoords(pos.x >> 4, pos.z >> 4);
	LordAssert(pChunk);

	bool modifyed = pChunk->setBlockMeta(pos.getChunkPos(), metadata);

	if (modifyed)
	{
		int blockID = pChunk->getBlockID(pos.getChunkPos());

		if ((flag & 2) != 0 && (!m_isClient || (flag & 4) == 0))
		{
			markBlockForUpdate(pos);
		}

		if (!m_isClient && (flag & 1) != 0)
		{
			notifyBlockChange(pos, blockID);
			Block* pBlock = BlockManager::sBlocks[blockID];

			if (pBlock != NULL && pBlock->hasComparatorInputOverride())
			{
				updateComparatorOutputLevel(pos, blockID);
			}
		}
	}

	return modifyed;
}

bool World::destroyBlock(const BlockPos& pos, bool isDropItem)
{
	int blockID = getBlockId(pos);

	if (blockID > 0)
	{
		int metadata = getBlockMeta(pos);
		// playAuxSFX(2001, x, y, z, blockID + (metadata << 12));

		if (isDropItem)
		{
			Block* pBlock = BlockManager::sBlocks[blockID];
			LordAssert(pBlock);
			pBlock->dropBlockAsItem(this, pos, metadata, 0);
		}

		return setBlock(pos, 0, 0, 3);
	}
	else
	{
		return false;
	}
}

void World::addTileEntity(TileEntity * tileEntity)
{
	if (m_scanningTileEntities)
	{
		m_addedTileEntityList.insert(tileEntity);
	}
	else
	{
		m_loadedTileEntityList.insert(tileEntity);
	}
}

void World::markBlockForUpdate(const BlockPos& pos)
{
	for (WorldListeners::iterator it = m_worldListeners.begin(); it != m_worldListeners.end(); ++it)
	{
		IWorldEventListener* listener = *it;
		listener->markBlockForUpdate(pos);
	}
}

void World::markBlockRangeForRenderUpdate(const BlockPos& mini, const BlockPos& maxi)
{
	for (WorldListeners::iterator it = m_worldListeners.begin(); it != m_worldListeners.end(); ++it)
	{
		IWorldEventListener* listener = *it;
		listener->markBlockRangeForRenderUpdate(mini.x, mini.y, mini.z, maxi.x, maxi.y, maxi.z);
	}
}

void World::notifyBlocksOfNeighborChange(const BlockPos& pos, int blockID)
{
	int x = pos.x;
	int y = pos.y;
	int z = pos.z;

	notifyBlockOfNeighborChange(BlockPos(x - 1, y, z), blockID);
	notifyBlockOfNeighborChange(BlockPos(x + 1, y, z), blockID);
	notifyBlockOfNeighborChange(BlockPos(x, y - 1, z), blockID);
	notifyBlockOfNeighborChange(BlockPos(x, y + 1, z), blockID);
	notifyBlockOfNeighborChange(BlockPos(x, y, z - 1), blockID);
	notifyBlockOfNeighborChange(BlockPos(x, y, z + 1), blockID);
}

void World::notifyBlocksOfNeighborChange(const BlockPos& pos, int changeBlockID, int side)
{
	int x = pos.x;
	int y = pos.y;
	int z = pos.z;

	if (side != 4)
		notifyBlockOfNeighborChange(BlockPos(x - 1, y, z), changeBlockID);
	if (side != 5)
		notifyBlockOfNeighborChange(BlockPos(x + 1, y, z), changeBlockID);
	if (side != 0)
		notifyBlockOfNeighborChange(BlockPos(x, y - 1, z), changeBlockID);
	if (side != 1)
		notifyBlockOfNeighborChange(BlockPos(x, y + 1, z), changeBlockID);
	if (side != 2)
		notifyBlockOfNeighborChange(BlockPos(x, y, z - 1), changeBlockID);
	if (side != 3)
		notifyBlockOfNeighborChange(BlockPos(x, y, z + 1), changeBlockID);
}

void World::notifyBlockOfNeighborChange(const BlockPos& pos, int changeBlockID)
{
	if (!m_isClient)
	{
		int oldBlockID = getBlockId(pos);
		Block* pBlock = BlockManager::sBlocks[oldBlockID];

		if (pBlock)
		{
			try
			{
				pBlock->onNeighborBlockChange(this, pos, changeBlockID);
			}
			catch (...)
			{
				LordException("World::notifyBlockOfNeighborChange error");
			}
		}
	}
}

bool World::canBlockSeeTheSky(const BlockPos& pos)
{
	ChunkPtr pChunck = getChunkFromChunkCoords(pos.x >> 4, pos.z >> 4);
	LordAssert(pChunck);
	return pChunck->canBlockSeeTheSky(pos.getChunkPos());
}

WorldChunkManager* World::getWorldChunkManager()
{
	return m_provider->worldChunkMgr;
}

BiomeGenBase* World::getBiomeGenForCoords(int x, int z)
{
	if (blockExists(BlockPos(x, 0, z)))
	{
		ChunkPtr pChunk = getChunkFromBlockCoords(x, z);

		if (pChunk != NULL)
		{
			return pChunk->getBiomeGenForWorldCoords(x & 15, z & 15, m_provider->worldChunkMgr);
		}
	}

	return m_provider->worldChunkMgr->getBiomeGenAt(x, z);
}

int World::getHeightValue(int x, int z)
{
	if (!(x >= -MAX_WORLD_X && z >= -MAX_WORLD_Z && x < MAX_WORLD_X && z < MAX_WORLD_Z))
		return 0;

	if (!chunkExists(x >> 4, z >> 4))
	{
		return 0;
	}
	else
	{
		ChunkPtr pChunk = getChunkFromChunkCoords(x >> 4, z >> 4);
		LordAssert(pChunk);
		return pChunk->getHeightValue(x & 15, z & 15);
	}
}

int World::getChunkHeightMapMinimum(int x, int z)
{
	if (!(x >= -MAX_WORLD_X && z >= -MAX_WORLD_Z && x < MAX_WORLD_X && z < MAX_WORLD_Z))
		return 0;

	if (!chunkExists(x >> 4, z >> 4))
	{
		return 0;
	}
	else
	{
		ChunkPtr pChunk = getChunkFromChunkCoords(x >> 4, z >> 4);
		LordAssert(pChunk);
		return pChunk->getMinimumHeight();
	}
}

void World::markBlockForRenderUpdate(const BlockPos& pos)
{
	/*for (int i = 0; i < worldAccesses.size(); ++i)
	{
	((IWorldAccess)worldAccesses.get(i)).markBlockForRenderUpdate(x, y, z);
	}*/
}

int World::spawnItemInWorld(int id, int num, int meta, float life, Vector3 position, Vector3 motion, bool isCombine, bool isEverLife)
{
	auto stack = LORD::make_shared<ItemStack>(id, num, meta);
	EntityItem* pResult = LordNew EntityItem(this, position, stack);
	pResult->age = 20 * life > LogicSetting::Instance()->getEntityItemLife() ? 0 : (int)(LogicSetting::Instance()->getEntityItemLife() - 20 * life);
	pResult->motion = motion;
	pResult->isCombine = isCombine;
	pResult->isEverLife = isEverLife;
	if (spawnEntityInWorld(pResult)) 
	{
		return pResult->entityId;
	}
	return 0;
}

bool World::spawnEntityInWorld(Entity* pEntity)
{
	int chunkX = int(Math::Floor(pEntity->position.x / 16.0f));
	int chunkZ = int(Math::Floor(pEntity->position.z / 16.0f));
	bool forceSpawn = pEntity->forceSpawn;
	EntityPlayer* pPlayer = dynamic_cast<EntityPlayer*>(pEntity);
	EntityCreature* pCreature = dynamic_cast<EntityCreature*>(pEntity);

	if (pCreature)
	{
		pCreature->setCurrentTargetEntityId(0, false);
	}

	if (pPlayer)
	{
		forceSpawn = true;
	}

	/*if (!forceSpawn && !chunkExists(chunkX, chunkZ))
	{
		return false;
	}
	else
	{*/
		if (pPlayer)
		{
			m_playerEntities[pPlayer->entityId] = pPlayer;
			updateAllPlayersSleepingFlag();
		}

		getChunkFromChunkCoords(chunkX, chunkZ)->addEntity(pEntity);
		m_loadedEntityMap[pEntity->entityId] = pEntity;
		onEntityAdded(pEntity);
		return true;
	//}
}

void World::onEntityAdded(Entity* pEntity)
{
	//LordLogInfo("onEntityAdded, entityId=%d, name=%s", pEntity->entityId, pEntity->getEntityName().c_str());
	for (WorldListeners::iterator it = m_worldListeners.begin(); it != m_worldListeners.end(); ++it)
	{
		IWorldEventListener* listener = *it;
		listener->onEntityCreate(pEntity);
	}
}

void World::onEntityRemoved(Entity* pEntity)
{
	//LordLogInfo("onEntityRemoved, name=%s", pEntity->getEntityName().c_str());
	for (WorldListeners::iterator it = m_worldListeners.begin(); it != m_worldListeners.end(); ++it)
	{
		IWorldEventListener* listener = *it;
		listener->onEntityDestroy(pEntity);
	}
}

void World::destroyBlockInWorldPartially(int breakerID, const BlockPos& pos, int progress)
{
	for (WorldListeners::iterator it = m_worldListeners.begin(); it != m_worldListeners.end(); ++it)
	{
		IWorldEventListener* listener = *it;
		listener->destroyBlockPartially(breakerID, pos, progress);
	}
}

EntityArr World::getEntityCreatureAI(String loadBtTree)
{
	EntityArr entityes;
	for (auto it = m_loadedEntityMap.begin(); it != m_loadedEntityMap.end(); it++)
	{
		EntityCreature* pCreature = dynamic_cast<EntityCreature*>(it->second);
		if (pCreature && pCreature->getMonsterBtTree() == loadBtTree)
		{
			entityes.push_back(it->second);
		}
	}

	return entityes;
}

Entity* World::getEntityCreatureAIByType(CRETURE_AI_TYPE CreatureAIType)
{
	for (auto it = m_loadedEntityMap.begin(); it != m_loadedEntityMap.end(); it++)
	{
		EntityCreature* pCreature = dynamic_cast<EntityCreature*>(it->second);
		if (pCreature && pCreature->getMonsterType() == CreatureAIType)
		{
			return it->second;
		}
	}
	return nullptr;
}

EntityArr World::getEntityCreatureMonsterAI()
{
	EntityArr entityes;
	for (auto it = m_loadedEntityMap.begin(); it != m_loadedEntityMap.end(); it++)
	{
		EntityCreature* pCreature = dynamic_cast<EntityCreature*>(it->second);
		if (pCreature && pCreature->isEntityAI && !pCreature->isBase())
		{
			entityes.push_back(it->second);
		}
	}
	return entityes;
}

EntityArr World::getEntityCreatureBaseAI()
{
	EntityArr entityes;
	for (auto it = m_loadedEntityMap.begin(); it != m_loadedEntityMap.end(); it++)
	{
		EntityCreature* pCreature = dynamic_cast<EntityCreature*>(it->second);
		if (pCreature && pCreature->isEntityAI && pCreature->isBase())
		{
			entityes.push_back(it->second);
		}
	}
	return entityes;
}

EntityArr World::getEntityCreatureBossAI()
{
	EntityArr entityes;
	for (auto it = m_loadedEntityMap.begin(); it != m_loadedEntityMap.end(); it++)
	{
		EntityCreature* pCreature = dynamic_cast<EntityCreature*>(it->second);
		if (pCreature && pCreature->isEntityAI && pCreature->isBoss())
		{
			entityes.push_back(it->second);
		}
	}
	return entityes;
}

EntityArr World::getBulletEntity(int shootingEntityId)
{
	EntityArr entityes;
	for (auto it = m_loadedEntityMap.begin(); it != m_loadedEntityMap.end(); it++)
	{
		EntityBullet* pBullet = dynamic_cast<EntityBullet*>(it->second);
		if (pBullet && pBullet->m_shootingEntityId == shootingEntityId)
		{
			entityes.push_back(it->second);
		}
	}
	return entityes;
}

EntityPlayer* World::getPlayerEntity(int entityId)
{
	auto it = m_playerEntities.find(entityId);
	if (it == m_playerEntities.end()) {
		return nullptr;
	}

	return it->second;
}

EntityPlayer* World::getPlayerByPlatformUserId(ui64 userId)
{
	for (auto player : m_playerEntities)
	{
		if (player.second->getPlatformUserId() == userId)
		{
			return player.second;
		}
	}
	return nullptr;
}

EntityPlayer * World::getFirstPlayerEntity()
{
	for (auto it = m_loadedEntityMap.begin(); it != m_loadedEntityMap.end(); it++)
	{
		EntityPlayer* pPlayer = dynamic_cast<EntityPlayer*>(it->second);
		if (pPlayer)
		{
			return pPlayer;
		}
	}
	return nullptr;
}

EntityAircraft * World::getAircraft()
{
	for (auto it = m_loadedEntityMap.begin(); it != m_loadedEntityMap.end(); it++)
	{
		EntityAircraft* pAircraft = dynamic_cast<EntityAircraft*>(it->second);
		if (pAircraft)
		{
			return pAircraft;
		}
	}

	return NULL;
}

Entity* World::getEntity(int entityId)
{
	auto it = m_loadedEntityMap.find(entityId);
	if (it == m_loadedEntityMap.end()) {
		return nullptr;
	}
	
	return it->second;
}

void World::removeEntity(int entityId)
{
	auto it = m_loadedEntityMap.find(entityId);
	if (it == m_loadedEntityMap.end()) {
		return;
	}

	Entity* pEntity = it->second;
	removeEntity(pEntity);
}

void World::removeEntity(Entity* pEntity)
{
	if (pEntity->riddenByEntity != NULL)
	{
		pEntity->riddenByEntity->mountEntity(NULL);
	}

	if (pEntity->ridingEntity != NULL)
	{
		pEntity->mountEntity(NULL);
	}

	pEntity->setDead();

	EntityPlayer* pPlayer = dynamic_cast<EntityPlayer*>(pEntity);
	if (pPlayer)
	{
		m_playerEntities.erase(pPlayer->entityId);
		updateAllPlayersSleepingFlag();
	}
	EntityCreature* pCreature = dynamic_cast<EntityCreature*>(pEntity);
	if (pCreature)
	{
		pCreature->setDead();
		pCreature->setCurrentTargetEntityId(0, false);
	}

}

void World::removePlayerEntityDangerously(EntityPlayer* pPlayer)
{
	if (pPlayer)
	{
		pPlayer->setDead();
		m_playerEntities.erase(pPlayer->entityId);
		updateAllPlayersSleepingFlag();
	}
}

AABBList& World::getCollidingBoundingBoxes(Entity* pEntity, const Box& aabb)
{
	constexpr int detectionDistance = 1;
	m_collidingAABBs.clear();
	int minx = int(Math::Floor(aabb.vMin.x)) - detectionDistance;
	int maxx = int(Math::Floor(aabb.vMax.x + 1.0f)) + detectionDistance;
	int miny = int(Math::Floor(aabb.vMin.y)) - detectionDistance;
	int maxy = int(Math::Floor(aabb.vMax.y + 1.0f)) + detectionDistance;
	int minz = int(Math::Floor(aabb.vMin.z)) - detectionDistance;
	int maxz = int(Math::Floor(aabb.vMax.z + 1.0f)) + detectionDistance;

	for (int x = minx; x < maxx; ++x)
	{
		for (int z = minz; z < maxz; ++z)
		{
			if (blockExists(BlockPos(x, 64, z)))
			{
				for (int y = miny; y < maxy; ++y)
				{
					Block* pBlock = BlockManager::sBlocks[getBlockId(BlockPos(x, y, z))];

					if (pBlock != NULL)
					{
						pBlock->addCollisionBoxesToList(this, BlockPos(x, y, z), aabb, m_collidingAABBs, pEntity);
					}
				}
			}
		}
	}

	float expandSize = 0.25f;
	EntityArr entities = getEntitiesWithinAABBExcludingEntity(pEntity, aabb.expland(expandSize, expandSize, expandSize));

	for (int i = 0; i < int(entities.size()); ++i)
	{
		EntityPlayer* player = dynamic_cast<EntityPlayer*>(entities[i]);
		EntityVehicle* vehicle = dynamic_cast<EntityVehicle*>(pEntity);
		EntitySessionNpc* sessionNpc = dynamic_cast<EntitySessionNpc*>(entities[i]);
		EntityPlayer* mePlayer = dynamic_cast<EntityPlayer*>(pEntity);
		// player and player->vehicle not collide any more
		if (vehicle && player && player->getVehicle() == vehicle)
		{
			continue;
		}

		if (mePlayer && sessionNpc && !sessionNpc->isCanCollidedByUser(mePlayer->getPlatformUserId()))
		{
			continue;
		}

		if (!pEntity->canBeCollidedWith() && !entities[i]->canBeCollidedWith())
		{
			continue;
		}

		Box entity_box = entities[i]->getBoundingBox();

		if (!entity_box.isNull() && entity_box.isIntersected(aabb))
		{
			m_collidingAABBs.push_back(entity_box);
		}

		entity_box = pEntity->getCollisionBox(entities[i]);

		if (!entity_box.isNull() && entity_box.isIntersected(aabb))
		{
			m_collidingAABBs.push_back(entity_box);
		}
	}

	return m_collidingAABBs;
}

AABBList& World::getCollidingBlockBounds(const Box& aabb)
{
	m_collidingAABBs.clear();
	int minx = int(Math::Floor(aabb.vMin.x));
	int maxx = int(Math::Floor(aabb.vMin.x + 1.0f));
	int miny = int(Math::Floor(aabb.vMin.y));
	int maxy = int(Math::Floor(aabb.vMin.y + 1.0f));
	int minz = int(Math::Floor(aabb.vMin.z));
	int maxz = int(Math::Floor(aabb.vMin.z + 1.0f));

	for (int x = minx; x < maxx; ++x)
	{
		for (int z = minz; z < maxz; ++z)
		{
			if (blockExists(BlockPos(x, 64, z)))
			{
				for (int y = miny - 1; y < maxy; ++y)
				{
					Block* pBlock = BlockManager::sBlocks[getBlockId(BlockPos(x, y, z))];

					if (pBlock != NULL)
					{
						pBlock->addCollisionBoxesToList(this, BlockPos(x, y, z), aabb, m_collidingAABBs, NULL);
					}
				}
			}
		}
	}

	return m_collidingAABBs;
}

EntityArr World::getEntitiesWithinAABBExcludingEntity(Entity* pEntity, const Box& aabb, bool includeWatchModePlayer)
{
	EntityArr entities;
	int minChunkX = int(Math::Floor((aabb.vMin.x - 2.0f) / 16.0f));
	int maxChunkX = int(Math::Floor((aabb.vMax.x + 2.0f) / 16.0f));
	int minChunkZ = int(Math::Floor((aabb.vMin.z - 2.0f) / 16.0f));
	int maxChunkZ = int(Math::Floor((aabb.vMax.z + 2.0f) / 16.0f));

	for (int x = minChunkX-1; x <= maxChunkX+1; ++x)
	{
		for (int z = minChunkZ-1; z <= maxChunkZ+1; ++z)
		{
			if (chunkExists(x, z))
			{
				getChunkFromChunkCoords(x, z)->getEntitiesWithinAABBForEntity(pEntity, aabb, entities, includeWatchModePlayer);
			}
		}
	}

	return entities;
}

EntityArr World::getEntitiesWithinAABB(ui32 EntityType, const Box& aabb, bool includeWatchModePlayer)
{
	EntityArr entities;
	int minChunkX = int(Math::Floor((aabb.vMin.x - 2.0f) / 16.0f));
	int maxChunkX = int(Math::Floor((aabb.vMax.x + 2.0f) / 16.0f));
	int minChunkZ = int(Math::Floor((aabb.vMin.z - 2.0f) / 16.0f));
	int maxChunkZ = int(Math::Floor((aabb.vMax.z + 2.0f) / 16.0f));

	for (int x = minChunkX; x <= maxChunkX; ++x)
	{
		for (int z = minChunkZ; z <= maxChunkZ; ++z)
		{
			if (chunkExists(x, z))
			{
				getChunkFromChunkCoords(x, z)->getEntitiesWithinAABB(EntityType, aabb, entities, includeWatchModePlayer);
			}
		}
	}

	return entities;
}

EntityArr World::getEntitiesWithinRayTracy(Entity* pEntity, int type, const Vector3& beginPos, const Vector3& dir, float len)
{
	EntityArr entities;

	LORD::Ray ray(beginPos, dir);
	LORD::HitInfo hitinfo;
	float trim = len;
	
	for (auto it : m_loadedEntityMap)
	{
		Entity* pEntity = it.second;
		if (!pEntity->isClass(type))
			continue;

		const Box& aabb = pEntity->boundingBox;
		bool hit = ray.hitBox(aabb, trim, hitinfo);
		if (hit && trim <= len)
			entities.push_back(pEntity);
	}

	return entities;
}

bool World::isEntityHeadWithinRayTracy(const Box& aabb, const Vector3& beginPos, const Vector3& dir, float len)
{
	LORD::Ray ray(beginPos, dir);
	LORD::HitInfo hitinfo;
	float trim = len;
	bool hit = ray.hitBox(aabb, trim, hitinfo);
	if (hit && trim <= len)
		return true;
	return false;
}

Explosion* World::createExplosion(Entity* exploder, EntityLivingBase* explosivePlacedBy, const Vector3& pos, float strength, bool bSmoking)
{
	return newExplosion(exploder, explosivePlacedBy, pos, strength, false, bSmoking);
}

Explosion* World::newExplosion(Entity* exploder, EntityLivingBase* explosivePlacedBy, const Vector3& pos, float strength, bool bFlaming, bool bSmoking)
{
	Explosion* pExplosion = LordNew Explosion(this, exploder, explosivePlacedBy, pos, strength);
	pExplosion->isFlaming = bFlaming;
	pExplosion->isSmoking = bSmoking;
	int entityId = -1;
	if (explosivePlacedBy)
	{
		entityId = explosivePlacedBy->entityId;
	}

	ExplosionAttr attr;
	attr.isBreakBlock = true;
	attr.isCanHurt = true;
	attr.isHurtEntityItem = true;
	attr.custom_explosionSize = 1.0f;
	SCRIPT_EVENT::BlockTNTExplodeEvent::invoke(entityId, pos, &attr);
	pExplosion->doExplosionA(attr.isBreakBlock, attr.isCanHurt, attr.isHurtEntityItem , attr.custom_explosionSize);
	pExplosion->doExplosionB(m_isClient, attr.custom_explosionSize);

	if (!m_isClient)
	{
		for (auto item : m_playerEntities)
		{
			BLOCKMAN::Entity* entity = (Entity*) (item.second);
			if (entity->getDistanceSq(pos) < 4096.0f)
			{
				BLOCKMAN::PlayerKnockMap& m = pExplosion->getPlayerKnockbackMap();
				if (m.find(entity) != m.end()) {
					BLOCKMAN::GameCommon::Instance()->getCommonPacketSender()->sendExplosion(item.second, pos, strength, pExplosion->affectedBlocksPos, m[entity], pExplosion->isSmoking, attr.custom_explosionSize);
				}
				else
				{
					BLOCKMAN::GameCommon::Instance()->getCommonPacketSender()->sendExplosion(item.second, pos, strength, pExplosion->affectedBlocksPos, Vector3(0,0,0), pExplosion->isSmoking, attr.custom_explosionSize);
				}
				
			}
		}
	}
	
	return pExplosion;
}

float World::getBlockDensity(const Vector3& vec, const Box& aabb)
{
	float segX = 1.0f / ((aabb.vMax.x - aabb.vMin.x) * 2.0f + 1.0f);
	float segY = 1.0f / ((aabb.vMax.y - aabb.vMin.y) * 2.0f + 1.0f);
	float segZ = 1.0f / ((aabb.vMax.z - aabb.vMin.z) * 2.0f + 1.0f);
	int notTraced = 0;
	int total = 0;

	for (float x = 0.0f; x <= 1.0f; x = x + segX)
	{
		for (float y = 0.0f; y <= 1.0f; y = y + segY)
		{
			for (float z = 0.0f; z <= 1.0f; z = z + segZ)
			{
				float fx = aabb.vMin.x + (aabb.vMax.x - aabb.vMin.x) * x;
				float fy = aabb.vMin.y + (aabb.vMax.y - aabb.vMin.y) * y;
				float fz = aabb.vMin.z + (aabb.vMax.z - aabb.vMin.z) * z;

				if (!rayTraceBlocks(Vector3(fx, fy, fz), vec).result)
				{
					++notTraced;
				}

				++total;
			}
		}
	}

	return (float)notTraced / (float)total;
}

bool World::extinguishFire(EntityPlayer* pPlayer, const BlockPos& pos, int face)
{
	BlockPos pos2 = pos;
	if (face == BM_FACE_DOWN)
		--pos2.y;
	else if (face == BM_FACE_UP)
		++pos2.y;
	else if (face == BM_FACE_NORTH)
		--pos2.z;
	else if (face == BM_FACE_SOUTH)
		++pos2.z;
	else if (face == BM_FACE_WEST)
		--pos2.x;
	else if (face == BM_FACE_EAST)
		++pos2.x;

	if (getBlockId(pos2) == BLOCK_ID_FIRE)
	{
		// playAuxSFXAtEntity(pPlayer, 1004, x, y, z, 0);
		setBlockToAir(pos2);
		return true;
	}
	return false;
}

RayTraceResult World::rayTraceBlocks(Vector3 start, Vector3 end)
{
	return rayTraceBlocks(start, end, false, false);
}

RayTraceResult World::rayTraceBlocks(Vector3 start, Vector3 end, bool stopOnLiquid)
{
	return rayTraceBlocks(start, end, stopOnLiquid, false);
}

RayTraceResult World::rayTraceBlocks(Vector3 start, Vector3 end, bool stopOnLiquid, bool ignoreBlockWithoutBoundingBox)
{
	RayTraceResult result;
	if (start.isInvalid())
		return result;
	if (end.isInvalid())
		return result;

	int endx = int(Math::Floor(end.x));
	int endy = int(Math::Floor(end.y));
	int endz = int(Math::Floor(end.z));
	int startx = int(Math::Floor(start.x));
	int starty = int(Math::Floor(start.y));
	int startz = int(Math::Floor(start.z));
	int blockID = getBlockId(BlockPos(startx, starty, startz));
	int metadata = getBlockMeta(BlockPos(startx, starty, startz));
	Block* pBlock = BlockManager::sBlocks[blockID];

	if ((!ignoreBlockWithoutBoundingBox ||
		pBlock == NULL ||
		pBlock->getCollisionBoundingBox(this, BlockPos(startx, starty, startz)).isNull()) &&
		blockID > 0 && pBlock->canCollideCheck(metadata, stopOnLiquid))
	{
		result = pBlock->collisionRayTrace(this, BlockPos(startx, starty, startz), start, end);

		if (result.result)
			return result;
	}

	blockID = 200;

	while (blockID-- >= 0)
	{
		if (start.isInvalid())
			return result;

		if (startx == endx && starty == endy && startz == endz)
		{
			return result;
		}

		bool flagx = true;
		bool flagy = true;
		bool flagz = true;
		float minx = 999.0f;
		float miny = 999.0f;
		float minz = 999.0f;

		if (endx > startx)
			minx = startx + 1.0f;
		else if (endx < startx)
			minx = startx + 0.0f;
		else
			flagx = false;

		if (endy > starty)
			miny = starty + 1.0f;
		else if (endy < starty)
			miny = starty + 0.0f;
		else
			flagy = false;

		if (endz > startz)
			minz = startz + 1.0f;
		else if (endz < startz)
			minz = startz + 0.0f;
		else
			flagz = false;

		float maxx = 999.0f;
		float maxy = 999.0f;
		float maxz = 999.0f;
		float lx = end.x - start.x;
		float ly = end.y - start.y;
		float lz = end.z - start.z;

		if (flagx)
			maxx = (minx - start.x) / lx;
		if (flagy)
			maxy = (miny - start.y) / ly;
		if (flagz)
			maxz = (minz - start.z) / lz;

		bool var33 = false;
		int face;

		if (maxx < maxy && maxx < maxz)
		{
			if (endx > startx)
				face = 4;
			else
				face = 5;

			start.x = minx;
			start.y += ly * maxx;
			start.z += lz * maxx;
		}
		else if (maxy < maxz)
		{
			if (endy > starty)
				face = 0;
			else
				face = 1;

			start.x += lx * maxy;
			start.y = miny;
			start.z += lz * maxy;
		}
		else
		{
			if (endz > startz)
				face = 2;
			else
				face = 3;

			start.x += lx * maxz;
			start.y += ly * maxz;
			start.z = minz;
		}

		Vector3 v(start.x, start.y, start.z);
		v.x = Math::Floor(start.x);
		startx = int(v.x);

		if (face == 5)
		{
			--startx;
			++v.x;
		}

		v.y = Math::Floor(start.y);
		starty = int(v.y);

		if (face == 1)
		{
			--starty;
			++v.y;
		}

		v.z = Math::Floor(start.z);
		startz = int(v.z);

		if (face == 3)
		{
			--startz;
			++v.z;
		}

		int blockID1 = getBlockId(BlockPos(startx, starty, startz));
		int metadata1 = getBlockMeta(BlockPos(startx, starty, startz));
		Block* pBlock1 = BlockManager::sBlocks[blockID1];

		if ((!ignoreBlockWithoutBoundingBox || pBlock1 == NULL || (!pBlock1->getCollisionBoundingBox(this, BlockPos(startx, starty, startz)).isNull())) &&
			blockID1 > 0 && pBlock1->canCollideCheck(metadata1, stopOnLiquid))
		{
			result = pBlock1->collisionRayTrace(this, BlockPos(startx, starty, startz), start, end);

			if (result.result)
				return result;
		}
	}
	return result;
}

int World::calculateSkylightSubtracted(float rdt)
{
	float var2 = getCelestialAngle(rdt);
	float var3 = 1.0f - (Math::Cos_tab(var2 * (float)Math::PI * 2.0f) * 2.0f + 0.5f);

	if (var3 < 0.0f)
	{
		var3 = 0.0f;
	}

	if (var3 > 1.0f)
	{
		var3 = 1.0f;
	}

	var3 = 1.0f - var3;
	// var3 = (float)((double)var3 * (1.0 - (double)(getRainStrength(rdt) * 5.0f) / 16.0));
	// var3 = (float)((double)var3 * (1.0 - (double)(getWeightedThunderStrength(rdt) * 5.0f) / 16.0));
	var3 = 1.0f - var3;
	return (int)(var3 * 11.0f);
}

float World::getCelestialAngle(float rdt)
{
	return m_provider->calculateCelestialAngle(m_WorldInfo.getWorldTime(), rdt);
}

float World::getCelestialAngleRadians(float rdt)
{
	float var2 = getCelestialAngle(rdt);
	return var2 * (float)Math::PI * 2.0f;
}

int World::getPrecipitationHeight(int x, int z)
{
	ChunkPtr pChunk = getChunkFromBlockCoords(x, z);
	LordAssert(pChunk);

	return pChunk->getPrecipitationHeight(x & 15, z & 15);
}

int World::getTopSolidOrLiquidBlock(int x, int z)
{
	ChunkPtr pChunk = getChunkFromBlockCoords(x, z);
	LordAssert(pChunk);

	int y = pChunk->getTopFilledSegment() + 15;
	x &= 15;

	for (z &= 15; y > 0; --y)
	{
		int blockID = pChunk->getBlockID(BlockPos(x, y, z));

		if (blockID != 0)
		{
			Block* pBlock = BlockManager::sBlocks[blockID];
			if (pBlock && pBlock->getMaterial().blocksMovement() && pBlock->getMaterial() != BM_Material::BM_MAT_leaves)
				return y + 1;
		}
	}

	return -1;
}

bool World::checkNoEntityCollision(const Box& aabb, Entity* pEntityIn)
{
	EntityArr entities = getEntitiesWithinAABBExcludingEntity(NULL, aabb);

	for (int i = 0; i < int(entities.size()); ++i)
	{
		Entity* pEntity = entities[i];

		if (!pEntity->isDead && pEntity->preventEntitySpawning && pEntity != pEntityIn)
		{
			return false;
		}
	}

	return true;
}

bool World::checkBlockCollision(const Box& aabb)
{
	int minX = (int)Math::Floor(aabb.vMin.x);
	int maxX = (int)Math::Floor(aabb.vMax.x + 1.0f);
	int minY = (int)Math::Floor(aabb.vMin.y);
	int maxY = (int)Math::Floor(aabb.vMax.y + 1.0f);
	int minZ = (int)Math::Floor(aabb.vMin.z);
	int maxZ = (int)Math::Floor(aabb.vMax.z + 1.0f);

	if (aabb.vMin.x < 0.0f)
		--minX;

	if (aabb.vMin.y < 0.0f)
		--minY;

	if (aabb.vMin.z < 0.0f)
		--minZ;

	Block** pBlockArr = BlockManager::sBlocks;
	for (int x = minX; x < maxX; ++x)
	{
		for (int y = minY; y < maxY; ++y)
		{
			for (int z = minZ; z < maxZ; ++z)
			{
				Block* pBlock = pBlockArr[getBlockId(BlockPos(x, y, z))];

				if (pBlock != NULL)
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool World::isMaterialInBB(const Box& bb, const BM_Material& mat)
{
	int minx = int(Math::Floor(bb.vMin.x));
	int maxx = int(Math::Ceil(bb.vMax.x));
	int miny = int(Math::Floor(bb.vMin.y));
	int maxy = int(Math::Ceil(bb.vMax.y));
	int minz = int(Math::Floor(bb.vMin.z));
	int maxz = int(Math::Ceil(bb.vMax.z));

	for (int x = minx; x < maxx; ++x)
	{
		for (int y = miny; y < maxy; ++y)
		{
			for (int z = minz; z < maxz; ++z)
			{
				Block* pBlock = BlockManager::sBlocks[getBlockId(BlockPos(x, y, z))];

				if (pBlock != NULL && pBlock->getMaterial() == mat)
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool World::handleMaterialAcceleration(const Box& bb, const BM_Material& mat, Entity* pEntity)
{
	int minx = int(Math::Floor(bb.vMin.x));
	int maxx = int(Math::Ceil(bb.vMax.x));
	int miny = int(Math::Floor(bb.vMin.y));
	int maxy = int(Math::Ceil(bb.vMax.y));
	int minz = int(Math::Floor(bb.vMin.z));
	int maxz = int(Math::Ceil(bb.vMax.z));

	if (!checkChunksExist(BlockPos(minx, miny, minz), BlockPos(maxx, maxy, maxz)))
		return false;

	bool inWater = false;
	Vector3 vec = Vector3::ZERO;

	for (int x = minx; x < maxx; ++x)
	{
		for (int y = miny; y < maxy; ++y)
		{
			for (int z = minz; z < maxz; ++z)
			{
				BlockPos pos(x, y, z);
				Block* pBlock = BlockManager::sBlocks[getBlockId(pos)];

				if (pBlock != NULL && pBlock->getMaterial() == mat)
				{
					float h2 = (float)(y + 1) - BlockFluid::getFluidHeightPercent(getBlockMeta(pos));

					if (maxy >= h2)
					{
						inWater = true;
						vec = pBlock->velocityToAddToEntity(this, pos, pEntity, vec);
					}
				}
			}
		}
	}

	if (vec.len() > 0.0f && pEntity->isPushedByWater())
	{
		vec.normalize();
		pEntity->motion += vec * 0.014f;
	}

	return inWater;
}

bool World::isAnyLiquid(const Box& aabb)
{
	int minX = (int)Math::Floor(aabb.vMin.x);
	int maxX = (int)Math::Floor(aabb.vMax.x + 1.0f);
	int minY = (int)Math::Floor(aabb.vMin.y);
	int maxY = (int)Math::Floor(aabb.vMax.y + 1.0f);
	int minZ = (int)Math::Floor(aabb.vMin.z);
	int maxZ = (int)Math::Floor(aabb.vMax.z + 1.0f);

	if (aabb.vMin.x < 0.0f)
		--minX;

	if (aabb.vMin.y < 0.0f)
		--minY;

	if (aabb.vMin.z < 0.0f)
		--minZ;

	Block** pBlockArr = BlockManager::sBlocks;
	for (int x = minX; x < maxX; ++x)
	{
		for (int y = minY; y < maxY; ++y)
		{
			for (int z = minZ; z < maxZ; ++z)
			{
				Block* pBlock = pBlockArr[getBlockId(BlockPos(x, y, z))];

				if (pBlock != NULL && pBlock->getMaterial().isLiquid())
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool World::canBlockFreeze(const BlockPos& pos, bool flag)
{
	BiomeGenBase* pBiome = getBiomeGenForCoords(pos.x, pos.z);
	float temperature = pBiome->getFloatTemperature();

	if (temperature > 0.15F)
	{
		return false;
	}
	else
	{
		// add by zhouyou. do not check the lightvalue in lgoic side.
		if (pos.y >= 0 && pos.y < 256 /*&& getSavedLightValue(BLOCK_LIGHT_VALUE, pos) < 10 */)
		{
			int blockID = getBlockId(pos);

			if ((blockID == BLOCK_ID_WATERSTILL ||
				blockID == BLOCK_ID_WATERMOVING) &&
				getBlockMeta(pos) == 0)
			{
				if (!flag)
				{
					return true;
				}

				bool flag2 = true;

				if (flag2 && getBlockMaterial(BlockPos(pos.x - 1, pos.y, pos.z)) != BM_Material::BM_MAT_water)
					flag2 = false;

				if (flag2 && getBlockMaterial(BlockPos(pos.x + 1, pos.y, pos.z)) != BM_Material::BM_MAT_water)
					flag2 = false;

				if (flag2 && getBlockMaterial(BlockPos(pos.x, pos.y, pos.z - 1)) != BM_Material::BM_MAT_water)
					flag2 = false;

				if (flag2 && getBlockMaterial(BlockPos(pos.x, pos.y, pos.z + 1)) != BM_Material::BM_MAT_water)
					flag2 = false;

				if (!flag2)
				{
					return true;
				}
			}
		}

		return false;
	}
}

bool World::canSnowAt(const BlockPos& pos)
{
	BiomeGenBase* pBiome = getBiomeGenForCoords(pos.x, pos.z);
	float temperature = pBiome->getFloatTemperature();

	if (temperature > 0.15F)
	{
		return false;
	}
	else
	{
		// add by zhouyou. do not check light value in logic side.
		if (pos.y >= 0 && pos.y < 256 /*&& getSavedLightValue(BLOCK_LIGHT_VALUE, pos) < 10*/)
		{
			BlockPos pos_down = pos;
			--pos_down.y;
			int block_downID = getBlockId(pos_down);
			int blockID = getBlockId(pos);

			BlockSnow* pSnow = dynamic_cast<BlockSnow*>(BlockManager::sBlocks[BLOCK_ID_SNOW]);
			if (blockID == 0 &&
				pSnow->canPlaceBlockAt(this, pos) &&  // modify by zhouyou
				block_downID != 0 && block_downID != BLOCK_ID_ICE &&
				BlockManager::sBlocks[block_downID]->getMaterial().blocksMovement())
			{
				return true;
			}
		}

		return false;
	}
}

bool World::isBoundingBoxBurning(const Box& aabb)
{
	int minX = (int)Math::Floor(aabb.vMin.x);
	int maxX = (int)Math::Floor(aabb.vMax.x + 1.0f);
	int minY = (int)Math::Floor(aabb.vMin.y);
	int maxY = (int)Math::Floor(aabb.vMax.y + 1.0f);
	int minZ = (int)Math::Floor(aabb.vMin.z);
	int maxZ = (int)Math::Floor(aabb.vMax.z + 1.0f);

	if (!checkChunksExist(BlockPos(minX, minY, minZ), BlockPos(maxX, maxY, maxZ)))
		return false;

	for (int x = minX; x < maxX; ++x)
	{
		for (int y = minY; y < maxY; ++y)
		{
			for (int z = minZ; z < maxZ; ++z)
			{
				int blockID = getBlockId(BlockPos(x, y, z));

				if (blockID == BLOCK_ID_FIRE ||
					blockID == BLOCK_ID_LAVAMOVING ||
					blockID == BLOCK_ID_LAVASTILL)
				{
					return true;
				}
			}
		}
	}

	return false;
}

TileEntity* World::getBlockTileEntity(const BlockPos& pos)
{
	if (!(pos.y >= 0 && pos.y < 256))
		return NULL;

	TileEntity* pTileEntity = NULL;
	TileEntity* pResult = NULL;

	if (m_scanningTileEntities)
	{
		for (TileEntitiesArr::iterator it = m_addedTileEntityList.begin(); it != m_addedTileEntityList.end(); ++it)
		{
			pTileEntity = *it;

			if (!pTileEntity->isInvalid() &&
				pTileEntity->m_pos == pos)
			{
				pResult = pTileEntity;
				break;
			}
		}
	}


	if (pTileEntity == NULL)
	{
		ChunkPtr pChunk = this->getChunkFromChunkCoords(pos.x >> 4, pos.z >> 4);

		if (pChunk)
		{
			pTileEntity = pChunk->getChunkBlockTileEntity(pos.getChunkPos());
		}
	}

	if (pResult == NULL)
	{
		for (TileEntitiesArr::iterator it = m_addedTileEntityList.begin(); it != m_addedTileEntityList.end(); ++it)
		{
			pTileEntity = *it;

			if (!pTileEntity->isInvalid() && pTileEntity->m_pos == pos)
			{
				pResult = pTileEntity;
				break;
			}
		}
	}

	return pTileEntity;
}

void World::setBlockTileEntity(const BlockPos& pos, TileEntity* pTileEntity, const ChunkPtr& ownChunk/* = nullptr*/)
{
	if (!(pTileEntity && !pTileEntity->isInvalid()))
		return;

	if (m_scanningTileEntities)
	{
		pTileEntity->m_pos = pos;

		for (TileEntitiesArr::iterator it = m_addedTileEntityList.begin(); it != m_addedTileEntityList.end();)
		{
			TileEntity* pAdded = *it;
			LordAssert(pAdded);
			if (pAdded->m_pos == pos)
			{
				pAdded->invalidate();
				it = m_addedTileEntityList.erase(it);
				LordSafeDelete(pAdded);
			}
			else
			{
				++it;
			}
		}

		m_addedTileEntityList.insert(pTileEntity);
	}
	else
	{
		m_loadedTileEntityList.insert(pTileEntity);
		ChunkPtr pChunk = ownChunk;
		if (!pChunk) {
			pChunk = getChunkFromChunkCoords(pos.x >> 4, pos.z >> 4);
		}

		if (pChunk)
		{
			pChunk->setChunkBlockTileEntity(pos.getChunkPos(), pTileEntity);
		}
	}
}

void World::removeBlockTileEntity(const BlockPos& pos)
{
	TileEntity* pTileEntity = getBlockTileEntity(pos);

	if (pTileEntity && m_scanningTileEntities)
	{
		pTileEntity->invalidate();
		m_addedTileEntityList.erase(pTileEntity);
	}
	else
	{
		if (pTileEntity != NULL)
		{
			m_addedTileEntityList.erase(pTileEntity);
			m_loadedTileEntityList.erase(pTileEntity);
		}

		ChunkPtr pChunk = getChunkFromChunkCoords(pos.x >> 4, pos.z >> 4);

		if (pChunk != NULL)
		{
			pChunk->removeChunkBlockTileEntity(pos.getChunkPos());
		}
	}
}

bool World::isBlockOpaqueCube(const BlockPos& pos)
{
	Block* pBlock = BlockManager::sBlocks[getBlockId(pos)];
	return pBlock == NULL ? false : pBlock->isOpaqueCube();
}

bool World::isBlockNormalCube(const BlockPos& pos)
{
	return BlockManager::isNormalCube(getBlockId(pos));
}

bool World::isBlockFullCube(const BlockPos& pos)
{
	int blockID = getBlockId(pos);

	if (blockID <= 0)
		return false;

	Block* pBlock = BlockManager::sBlocks[blockID];

	if (pBlock != NULL)
	{
		Box box = pBlock->getCollisionBoundingBox(this, pos);
		return box.getAverageEdgeLength() >= 1.0f;
	}
	else
	{
		return false;
	}
}

bool World::doesBlockHaveSolidTopSurface(const BlockPos& pos)
{
	Block* pBlock = BlockManager::sBlocks[getBlockId(pos)];
	return isBlockTopFacingSurfaceSolid(pBlock, getBlockMeta(pos));
}

bool World::isBlockTopFacingSurfaceSolid(const Block* pBlock, int metadata)
{
	return pBlock == NULL ? false :
		(pBlock->getMaterial().isOpaque() && pBlock->renderAsNormalBlock() ? true :
		(pBlock->checkType(BLOCK_TYPE_STAIRS) ? (metadata & 4) == 4 :
			(pBlock->checkType(BLOCK_TYPE_HALFSLAB) ? (metadata & 8) == 8 :
			(pBlock->checkType(BLOCK_TYPE_HOPPER) ? true :
				(pBlock->checkType(BLOCK_TYPE_SNOW) ? (metadata & 7) == 7 : false)))));
}

bool World::isBlockNormalCubeDefault(const BlockPos& pos, bool par4)
{
	if (!(pos.x >= -MAX_WORLD_X && pos.z >= -MAX_WORLD_Z && pos.x < MAX_WORLD_X && pos.z < MAX_WORLD_Z))
		return par4;

	ChunkPtr pChunk = m_pChunkService->getChunk(pos.x >> 4, pos.z >> 4);

	if (pChunk != NULL && !pChunk->isNonexistent())
	{
		Block* pBlock = BlockManager::sBlocks[getBlockId(pos)];
		return pBlock == NULL ? false : pBlock->getMaterial().isOpaque() && pBlock->renderAsNormalBlock();
	}
	else
	{
		return par4;
	}
}

void World::calculateInitialSkylight()
{
	int v = calculateSkylightSubtracted(1.0F);

	if (v != m_skylightSubtracted)
	{
		m_skylightSubtracted = v;
	}
}

void World::tick()
{
	tickUpdates(false);
	if (m_shop)
	{
		m_shop->onUpdate();
	}
	checkCprHttpFuture();
}

bool World::tickUpdates(bool par1)
{
	return true;
}

int World::isBlockProvidingPowerTo(const BlockPos& pos, int dir)
{
	int blockID = getBlockId(pos);
	return blockID == 0 ? 0 : BlockManager::sBlocks[blockID]->isProvidingStrongPower(this, pos, dir);
}

int World::getBlockPowerInput(const BlockPos& pos)
{
	int power = 0;
	for (int side = 0; side < 6; ++side)
	{
		power = Math::Max(power, isBlockProvidingPowerTo(pos + offsetForSide[side], side));
		if (power >= 15)
			return power;
	}
	return power;
}

bool World::getIndirectPowerOutput(const BlockPos& pos, int dir)
{
	return getIndirectPowerLevelTo(pos, dir) > 0;
}

int World::getIndirectPowerLevelTo(const BlockPos& pos, int dir)
{
	if (isBlockNormalCube(pos))
	{
		return getBlockPowerInput(pos);
	}
	else
	{
		int blockID = getBlockId(pos);
		return blockID == 0 ? 0 : BlockManager::sBlocks[blockID]->isProvidingWeakPower(this, pos, dir);
	}
}

bool World::isBlockIndirectlyGettingPowered(const BlockPos& pos)
{
	for (int side = 0; side < 6; ++side)
	{
		if (getIndirectPowerLevelTo(pos + offsetForSide[side], side) > 0)
			return true;
	}
	return false;
}

int World::getStrongestIndirectPower(const BlockPos& pos)
{
	int maxPower = 0;

	for (int side = 0; side < 6; ++side)
	{
		int neightborPower = getIndirectPowerLevelTo(pos + offsetForSide[side], side);

		if (neightborPower >= 15)
		{
			return 15;
		}

		if (neightborPower > maxPower)
		{
			maxPower = neightborPower;
		}
	}

	return maxPower;
}

EntityArr World::getClosestCreatureBossToPlayer(Entity* par1Entity, float distance)
{
	return getClosestCreatureBoss(par1Entity->position, distance);
}

EntityArr World::getClosestCreatureBoss(const Vector3& pos, float distance)
{
	float min_dis_sqr = distance * distance;
	EntityArr entityes;
	float mindis = 999.f;

	for (EntityPlayers::iterator it = m_playerEntities.begin(); it != m_playerEntities.end(); ++it)
	{
		EntityPlayer* pPlayer = it->second;
		if (!pPlayer)
			continue;

		if (!pPlayer->capabilities.disableDamage && !pPlayer->capabilities.isWatchMode && pPlayer->isEntityAlive())
		{
			float dis_sqr = pPlayer->getDistanceSq(pos);
			if (dis_sqr < min_dis_sqr)
			{
				entityes.push_back(pPlayer);
			}
		}
	}

	return entityes;
}

EntityPlayer* World::getClosestPlayerToEntity(Entity* par1Entity, float distance)
{
	return getClosestPlayer(par1Entity->position, distance);
}

EntityPlayer* World::getClosestPlayer(const Vector3& pos, float distance)
{
	float min_dis_sqr = distance * distance;
	EntityPlayer* pResult = NULL;

	for (EntityPlayers::iterator it = m_playerEntities.begin(); it != m_playerEntities.end(); ++it)
	{
		EntityPlayer* pPlayer = it->second;
		if (!pPlayer || pPlayer->isDead)
			continue;

		float dis_sqr = pPlayer->getDistanceSq(pos);

		if (dis_sqr < min_dis_sqr)
		{
			min_dis_sqr = dis_sqr;
			pResult = pPlayer;
		}
	}

	return pResult;
}



EntityPlayer* World::getClosestVulnerablePlayerToEntity(Entity* entity, float radius)
{
	return getClosestVulnerablePlayer(entity->position, radius);
}

EntityPlayer* World::getClosestVulnerablePlayer(const Vector3& pos, float radius)
{
	float mindis = 999.f;
	EntityPlayer* pResult = NULL;

	for (auto it : m_playerEntities)
	{
		EntityPlayer* pPlayer = it.second;

		if (!pPlayer->capabilities.disableDamage && !pPlayer->capabilities.isWatchMode && pPlayer->isEntityAlive())
		{
			float disSqr = pPlayer->getDistanceSq(pos);
			float r = radius;

			if (pPlayer->isSneaking())
			{
				r = radius * 0.8f;
			}

			if (pPlayer->isInvisible())
			{
				float armorVisiblity = pPlayer->getArmorVisibility();

				if (armorVisiblity < 0.1F)
				{
					armorVisiblity = 0.1F;
				}

				r *= 0.7F * armorVisiblity;
			}

			if ((radius < 0.0f || disSqr < r * r) && disSqr < mindis)
			{
				mindis = disSqr;
				pResult = pPlayer;
			}
		}
	}

	return pResult;
}

void World::setWorldWeather(String weather, int time)
{
	if (weather == "rain")
	{
		m_WorldInfo.setRainTime(time);
		m_WorldInfo.setRaining(true);
	}
	if (weather == "thunder")
	{
		m_WorldInfo.setThunderTime(time);
		m_WorldInfo.setThundering(true);
	}
}

void World::setSpawnLocation(const Vector3i& position)
{
	m_WorldInfo.setSpawnPosition(position);
	m_pChunkService->setCenter(position.x >> 4, position.z >> 4);
}

void World::addBlockEvent(const BlockPos& pos, int blockID, int id, int param)
{
	if (blockID <= 0)
		return;

	Block* pBlock = BlockManager::sBlocks[blockID];
	LordAssert(pBlock);

	pBlock->onBlockEventReceived(this, pos, id, param);
}

bool World::isBlockHighHumidity(const BlockPos& pos)
{
	BiomeGenBase* pBiome = getBiomeGenForCoords(pos.x, pos.z);
	return pBiome->isHighHumidity();
}

int World::getUniqueDataId(const String& par1Str)
{
	// return mapStorage.getUniqueDataId(par1Str);
	return 0;
}

int World::getActualHeight() const
{
	return m_provider->hasNoSky ? 128 : 256;
}

Random& World::setRandomSeed(int s1, int s2, int s3)
{
	i64 seed = (i64)s1 * 341873128712L + (i64)s2 * 132897987541L + m_WorldInfo.getSeed() + (i64)s3;
	m_Rand.setSeed(seed);
	return m_Rand;
}

void World::updateTileEntityChunkAndDoNothing(const BlockPos& pos, TileEntity* pTileEntity)
{
	if (blockExists(pos))
	{
		getChunkFromBlockCoords(pos.x, pos.z)->setChunkModified();
	}
}

PathEntity* World::getEntityPathToXYZ(Entity* pEntity, const BlockPos& pos, float mindis, bool passOpenWoodDoor, bool passCloseWoodDoor, bool avoidsWater, bool canSwim)
{
	BlockPos pos1 = pEntity->position.getFloor();
	int bound = (int)(mindis + 8.0F);

	ChunkCollection* chunks = LordNew ChunkCollection(this, pos1 - bound, pos1 + bound, 0);
	PathFinder* pFinder = LordNew PathFinder(chunks, passOpenWoodDoor, passCloseWoodDoor, avoidsWater, canSwim);
	PathEntity* pPath = pFinder->createEntityPathTo(pEntity, pos, mindis);
	LordDelete pFinder;
	LordDelete chunks;
	return pPath;
}

PathEntity* World::getPathEntityToEntity(Entity* start, Entity* end, float mindis, bool passOpenWoodDoor, bool passCloseWoodDoor, bool avoidsWater, bool canSwim)
{
	BlockPos pos1 = (start->position + Vector3::UNIT_Y).getFloor();
	int bound = (int)(mindis + 16.0F);

	//ChunkCollection* chunks = LordNew ChunkCollection(this, pos1 - bound, pos1 + bound, 0);
	PathFinder* pFinder = LordNew PathFinder(nullptr, passOpenWoodDoor, passCloseWoodDoor, avoidsWater, canSwim);
	PathEntity* pPath = pFinder->createEntityPathTo(start, end, mindis);
	LordDelete pFinder;
	//LordDelete chunks;
	return pPath;
}

void World::removeEntityFromChunk(Entity* pEntity)
{
	int chunkx = pEntity->chunkCoord.x;
	int chunkz = pEntity->chunkCoord.z;

	if (pEntity->addedToChunk && chunkExists(chunkx, chunkz))
	{
		getChunkFromChunkCoords(chunkx, chunkz)->removeEntity(pEntity);
	}
}

void World::updateEntities()
{
	Entity* pEntity = NULL;

	for (EntityList::iterator it = m_unloadedEntityList.begin(); it != m_unloadedEntityList.end(); ++it)
	{
		pEntity = *it;
		removeEntityFromChunk(pEntity);
	}

	for (EntityList::iterator it = m_unloadedEntityList.begin(); it != m_unloadedEntityList.end(); ++it)
	{
		onEntityRemoved(*it);
	}

	m_unloadedEntityList.clear();

	for (EntityMap::iterator it = m_loadedEntityMap.begin(); it != m_loadedEntityMap.end();)
	{
		pEntity = it->second;

		if (pEntity->ridingEntity != NULL)
		{
			if (!pEntity->ridingEntity->isDead && pEntity->ridingEntity->riddenByEntity == pEntity)
			{
				++it;
				continue;
			}

			pEntity->ridingEntity->riddenByEntity = NULL;
			pEntity->ridingEntity = NULL;
		}

		EntityPlayer* player = dynamic_cast<EntityPlayer*>(pEntity);

		if (!pEntity->isDead ||(player != NULL && player->capabilities.isWatchMode))
		{
			updateEntity(pEntity);
		}

		bool isDelay = false;
		if (player) {
			isDelay = true;
			player->decrTicksToFree();
		}

		EntityCreature* creature = dynamic_cast<EntityCreature*>(pEntity);
		if (creature) {
			isDelay = true;
			creature->decrTicksToFree();
		}

		if ((!isDelay && pEntity->isDead)
			|| (player && player->isReadyToFree()) 
			|| (creature && creature->isReadyToFree()))
		{
			removeEntityFromChunk(pEntity);

			pEntity->whenRemoved();
			it = m_loadedEntityMap.erase(it);
			onEntityRemoved(pEntity);

			LordSafeDelete(pEntity);
		}
		else {
			++it;
		}

	}

	m_scanningTileEntities = true;
	for (auto it = m_loadedTileEntityList.begin(); it != m_loadedTileEntityList.end();)
	{
		TileEntity* pTileEntity = *it;

		if (!pTileEntity->isInvalid() && pTileEntity->hasWorldObj() && blockExists(pTileEntity->m_pos))
		{
			pTileEntity->updateEntity();
		}
		if (pTileEntity->isInvalid())
		{
			it = m_loadedTileEntityList.erase(it);

			if (chunkExists(pTileEntity->m_pos.x >> 4, pTileEntity->m_pos.z >> 4))
			{
				ChunkPtr pChunk = getChunkFromChunkCoords(pTileEntity->m_pos.x >> 4, pTileEntity->m_pos.z >> 4);

				if (pChunk)
				{
					pChunk->removeChunkBlockTileEntity(pTileEntity->m_pos.getChunkPos());
				}
			}

			LordSafeDelete(pTileEntity);
		}
		else
			++it;
	}

	m_scanningTileEntities = false;

	for (TileEntitiesArr::iterator it = m_entityRemoval.begin(); it != m_entityRemoval.end(); ++it)
	{
		TileEntity* pTileEntity = *it;
		m_loadedTileEntityList.erase(*it);
		LordSafeDelete(pTileEntity);
	}
	m_entityRemoval.clear();

	for (TileEntitiesArr::iterator it = m_addedTileEntityList.begin(); it != m_addedTileEntityList.end(); ++it)
	{
		TileEntity* pTileEntity = *it;

		if (pTileEntity->isInvalid()) {
			if (m_loadedTileEntityList.find(pTileEntity) == m_loadedTileEntityList.end()) {
				LordSafeDelete(pTileEntity);
			}

			continue;
		}

		if (m_loadedTileEntityList.find(pTileEntity) == m_loadedTileEntityList.end())
		{
			m_loadedTileEntityList.insert(pTileEntity);
		}

		if (chunkExists(pTileEntity->m_pos.x >> 4, pTileEntity->m_pos.z >> 4))
		{
			ChunkPtr pChunk = getChunkFromChunkCoords(pTileEntity->m_pos.x >> 4, pTileEntity->m_pos.z >> 4);

			if (pChunk)
			{
				pChunk->setChunkBlockTileEntity(pTileEntity->m_pos.getChunkPos(), pTileEntity);
			}
		}

		markBlockForUpdate(pTileEntity->m_pos);
	}

	m_addedTileEntityList.clear();
}

void World::updateEntity(Entity* pEntity)
{
	updateEntityWithOptionalForce(pEntity, true);
}

void World::updateEntityWithOptionalForce(Entity* pEntity, bool forceUpdate)
{
	//LordLogDebug("updateEntityWithOptionalForce  1111");
	int x = int(Math::Floor(pEntity->position.x));
	int z = int(Math::Floor(pEntity->position.z));
	int range = 1;

	if (!forceUpdate || checkChunksExist(BlockPos(x - range, 0, z - range), BlockPos(x + range, 0, z + range)))
	{
		pEntity->lastTickPos = pEntity->position;
		pEntity->prevRotationYaw = pEntity->rotationYaw;
		pEntity->prevRotationPitch = pEntity->rotationPitch;

		//LordLogDebug("updateEntityWithOptionalForce  2222");
		if (forceUpdate && pEntity->addedToChunk)
		{
			++pEntity->ticksExisted;

			if (pEntity->ridingEntity)
			{
				pEntity->updateRidden();
			}
			else
			{
				pEntity->onUpdate();
			}
		}

		/*
		if (Double.isNaN(pEntity.posX) || Double.isInfinite(pEntity.posX))
		{
		pEntity.posX = pEntity.lastTickPosX;
		}

		if (Double.isNaN(pEntity.posY) || Double.isInfinite(pEntity.posY))
		{
		pEntity.posY = pEntity.lastTickPosY;
		}

		if (Double.isNaN(pEntity.posZ) || Double.isInfinite(pEntity.posZ))
		{
		pEntity.posZ = pEntity.lastTickPosZ;
		}

		if (Double.isNaN((double)pEntity.rotationPitch) || Double.isInfinite((double)pEntity.rotationPitch))
		{
		pEntity.rotationPitch = pEntity.prevRotationPitch;
		}

		if (Double.isNaN((double)pEntity.rotationYaw) || Double.isInfinite((double)pEntity.rotationYaw))
		{
		pEntity.rotationYaw = pEntity.prevRotationYaw;
		}
		*/

		//LordLogDebug("updateEntityWithOptionalForce  3333");
		int chunkx = int(Math::Floor(pEntity->position.x / 16.0f));
		int chunky = int(Math::Floor(pEntity->position.y / 16.0f));
		int chunkz = int(Math::Floor(pEntity->position.z / 16.0f));

		if (!pEntity->addedToChunk || pEntity->chunkCoord.x != chunkx || pEntity->chunkCoord.y != chunky || pEntity->chunkCoord.z != chunkz)
		{
			//LordLogDebug("updateEntityWithOptionalForce  4444");
			if (pEntity->addedToChunk && chunkExists(pEntity->chunkCoord.x, pEntity->chunkCoord.z))
			{
				getChunkFromChunkCoords(pEntity->chunkCoord.x, pEntity->chunkCoord.z)->removeEntityAtIndex(pEntity, pEntity->chunkCoord.y);
			}

			if (chunkExists(chunkx, chunkz))
			{
				pEntity->addedToChunk = true;
				getChunkFromChunkCoords(chunkx, chunkz)->addEntity(pEntity);
			}
			else
			{
				pEntity->addedToChunk = false;
			}
		}

		//LordLogDebug("updateEntityWithOptionalForce  5555");
		if (forceUpdate && pEntity->addedToChunk && pEntity->riddenByEntity)
		{
			//LordLogDebug("updateEntityWithOptionalForce  6666");
			if (!pEntity->riddenByEntity->isDead &&
				pEntity->riddenByEntity->ridingEntity == pEntity)
			{
				//LordLogDebug("updateEntityWithOptionalForce  9999");
				updateEntity(pEntity->riddenByEntity);
			}
			else
			{
				//LordLogDebug("updateEntityWithOptionalForce  aaaa");
				pEntity->riddenByEntity->ridingEntity = NULL;
				pEntity->riddenByEntity = NULL;
			}
		}
		//LordLogDebug("updateEntityWithOptionalForce  7777");
	}
}

void World::playSoundAtEntityByType(Entity* pEntity, SoundType soundType)
{
	playSoundByType(pEntity->position - Vector3(0.f, pEntity->yOffset, 0.f), soundType);
	if (pEntity != nullptr)
	{
		for (auto it : m_worldListeners)
		{
			IWorldEventListener* listener = it;
			listener->playSound(pEntity->position - Vector3(0.f, pEntity->yOffset, 0.f), soundType);
		}
	}
}

void World::playSoundToNearExceptByType(EntityPlayer* pPlayer, SoundType soundType)
{
	if (pPlayer)
	{
		for (auto it : m_worldListeners)
		{
			IWorldEventListener* listener = it;
			listener->playSoundToNearExcept(pPlayer, pPlayer->position - Vector3(0.f, pPlayer->yOffset, 0.f), soundType);
		}
	}
}

void World::playSoundEffectByType(const Vector3& pos, SoundType soundType)
{
	playSoundByType(pos, soundType);
	if (soundType > ST_Invalid && soundType < ST_Total)
	{
		for (auto it : m_worldListeners)
		{
			IWorldEventListener* listener = it;
			listener->playSound(pos, soundType);
		}
	}
}

void World::playRecord(const String& record, const BlockPos& pos)
{
	for (auto it : m_worldListeners)
	{
		IWorldEventListener* listener = it;
		it->playRecord(record, pos);
	}
}

void World::playAuxSFXAtEntity(EntityPlayer* pPlayer, int type, const BlockPos& pos, int data)
{
	for (auto it : m_worldListeners)
	{
		IWorldEventListener* listener = it;
		listener->playAuxSFX(pPlayer, type, pos, data);
	}
}

void World::resetSignText(Vector3i vec3, int line, const char *text)
{
	auto entity = dynamic_cast<TileEntitySign*>(getBlockTileEntity(vec3));
	if (!entity)
	{
		LordLogError("TileEntitySign is NULL--- x:%d y:%d z:%d text:%s", vec3.x, vec3.y, vec3.z, text);
		return;
	}
	entity->clearAllTexts();
	entity->setText(line, text);
}

int World::addMerchant(Vector3 pos, float yaw, String nameLang)
{
	auto merchant = LordNew EntityMerchant(this, pos);
	merchant->rotationYaw = yaw;
	if (nameLang.length() > 0)
		merchant->setNameLang(nameLang);
	if (spawnEntityInWorld(merchant))
		return merchant->entityId;
	else
		return 0;
}

void World::changePlayerActor(EntityPlayer* player, const String actorName, const String bodyId)
{
	if (player != nullptr)
	{
		player->m_isPeopleActor = false;
		player->m_actorName = actorName;
		player->m_actorBodyId = bodyId;
		for (auto it : m_worldListeners)
		{
			IWorldEventListener* listener = it;
			listener->onPlayerChangeActor(player);
		}
		auto size = ActorSizeSetting::getActorSize(player->m_actorName);
		player->setSize(size->width, size->length, size->height);
	}
}

void World::changeEntityActor(Entity * entity, const String actorName)
{
	if (entity != nullptr)
	{
		for (auto it : m_worldListeners)
		{
			IWorldEventListener* listener = it;
			listener->onEntityChangeActor(entity, actorName);
		}
	}
}

void World::restorePlayerActor(EntityPlayer * player)
{
	if (player != nullptr && player->m_isPeopleActor == false)
	{
		player->m_isPeopleActor = true;
		player->m_actorName = "";
		player->m_actorBodyId = "";
		player->setSize(0.6F, 1.8F);
		for (auto it : m_worldListeners)
		{
			IWorldEventListener* listener = it;
			listener->onPlayerRestoreActor(player);
		}
	}
	else
	{
		LordLogInfo("restore player actor is people actor is false");
	}
}

void World::addGoodsGroupToShop(i32 category, String icon, String name)
{
	if (!m_shop)
	{
		return;
	}
	m_shop->addGoodsGroup(category, icon, name);
}

void World::addGoodsToShop(i32 category, Goods goods)
{
	if (!m_shop)
	{
		return;
	}
	m_shop->addGoods(category, goods);
}

void World::addRespawnGoodsToShop(RespawnGoods goods)
{
	if (!m_shop)
	{
		return;
	}
	m_shop->addRespawnGoods(goods);
}

int World::addVehicle(Vector3 pos, int carId, float yaw)
{
	auto vehicle = LordNew EntityVehicle(this, carId, pos, yaw);
	if (spawnEntityInWorld(vehicle))
	{
		return vehicle->entityId;
	}
	else
	{
		return 0;
	}
}

int World::addRankNpc(Vector3 pos, float yaw, String nameLang)
{
	auto rankNpc = LordNew EntityRankNpc(this, pos);
	rankNpc->rotationYaw = yaw;
	if (spawnEntityInWorld(rankNpc)) 
	{
		if (nameLang.length() > 0)
			rankNpc->setNameLang(nameLang);

		return rankNpc->entityId;
	}
	else
		return 0;
}

void World::resetVehiclePosition(int entityId, Vector3 pos)
{
	auto vehicle = dynamic_cast<EntityVehicle*>(getEntity(entityId));
	if (vehicle)
	{
		vehicle->setPositionAndUpdate(pos);
	}
}

void World::resetVehiclePositionAndYaw(int entityId, Vector3 pos, float yaw)
{
	auto vehicle = dynamic_cast<EntityVehicle*>(getEntity(entityId));
	if (vehicle)
	{
		vehicle->setPositionAndYaw(pos, yaw);
	}
}

bool World::hasPlayerByVehicle(int entityId)
{
	auto vehicle = dynamic_cast<EntityVehicle*>(getEntity(entityId));
	if (vehicle)
	{
		return vehicle->hasPlayer();
	}
	return false;
}

int World::addAircraft(Vector3 pos, float yaw)
{
	auto aircraft = LordNew EntityAircraft(this, 1, pos, yaw);
	if (spawnEntityInWorld(aircraft))
		return aircraft->entityId;
	else
		return 0;
}

int World::tryAllPlayerTakeAircraft(Vector3 startPos, Vector3 endPos, float speed)
{
	auto aircraft = getAircraft();
	if (aircraft)
	{
		aircraft->setAirLine(startPos, endPos, speed);
		for (auto player : m_playerEntities)
		{
			if (player.second)
			{
				player.second->takeOnAircraft(aircraft);
			}
		}
		return aircraft->entityId;
	}
	else
	{
		LordLogError("by aircraft failed, please add aircraft entity");
		return -1;
	}
}

void World::tryAllPlayerOffAircraft()
{
	auto aircraft = getAircraft();
	if (aircraft)
	{
		aircraft->getOffAllPassengers();
	}
	else
	{
		LordLogError("by aircraft failed, please add aircraft entity");
	}
}

vector<Vector2>::type World::getPoisonCircleRange(Vector3 safeRange1, Vector3 safeRange2, Vector3 poisonRange1, Vector3 poisonRange2)
{
	Real maxSafeX = 0, minSafeX = 0, maxSafeZ = 0, minSafeZ = 0;
	Real maxPoisonX = 0, minPoisonX = 0, maxPoisonZ = 0, minPoisonZ = 0;

	maxSafeX = Math::Max(safeRange1.x, safeRange2.x);
	minSafeX = Math::Min(safeRange1.x, safeRange2.x);
	maxSafeZ = Math::Max(safeRange1.z, safeRange2.z);
	minSafeZ = Math::Min(safeRange1.z, safeRange2.z);

	maxPoisonX = Math::Max(poisonRange1.x, poisonRange2.x);
	minPoisonX = Math::Min(poisonRange1.x, poisonRange2.x);
	maxPoisonZ = Math::Max(poisonRange1.z, poisonRange2.z);
	minPoisonZ = Math::Min(poisonRange1.z, poisonRange2.z);

	vector<Vector2>::type range;
	range.push_back(Vector2(minSafeX, minSafeZ));
	range.push_back(Vector2(maxSafeX, maxSafeZ));
	range.push_back(Vector2(minPoisonX, minPoisonZ));
	range.push_back(Vector2(maxPoisonX, maxPoisonZ));

	return range;
}

int World::addActorNpc(Vector3 pos, float yaw, String actionName) 
{
	auto actorNpc = LordNew EntityActorNpc(this, pos);
	actorNpc->rotationYaw = yaw;

	if (actionName.length() > 0)
	{
		actorNpc->setActorName(actionName);
	}
	if (spawnEntityInWorld(actorNpc))
	{
		return actorNpc->entityId;
	}
	else
	{
		return 0;
	}
}

int World::addActorNpcWithObstruct(Vector3 pos, float yaw, String actionName, bool isObstruct)
{
	auto actorNpc = LordNew EntityActorNpc(this, pos);
	actorNpc->rotationYaw = yaw;
	actorNpc->setCanObstruct(isObstruct);

	if (actionName.length() > 0)
	{
		actorNpc->setActorName(actionName);
	}
	if (spawnEntityInWorld(actorNpc))
	{
		return actorNpc->entityId;
	}
	else
	{
		return 0;
	}
}

int World::addSessionNpc(Vector3 pos, float yaw, i32 sessionType, String nameLang, String actorName, String actorBody, String actorBodyId, String sessionContent, bool isPerson, bool isCanObstruct)
{
	auto sessionNpc = LordNew EntitySessionNpc(this, pos);
	sessionNpc->rotationYaw = yaw;
	sessionNpc->setPerson(isPerson);
	sessionNpc->setSessionType(sessionType);
	sessionNpc->setCanCollidedByUser(0, isCanObstruct);

	if (actorName.length() > 0)
	{
		sessionNpc->setActorNameByUser(0, actorName);
	}

	if (sessionContent.length() > 0)
	{
		sessionNpc->setSessionContentByUser(0, sessionContent);
	}

	if (nameLang.length() > 0) {
		sessionNpc->setNameLangByUser(0, nameLang);
	}

	if (actorBody.length() > 0)
	{
		sessionNpc->setActorBodyByUser(0, actorBody);
	}

	if (actorBodyId.length() > 0)
	{
		sessionNpc->setActorBodyIdByUser(0, actorBodyId);
	}

	if (spawnEntityInWorld(sessionNpc))
	{
		return sessionNpc->entityId;
	}
	return 0;
}

void World::setSessionNpcEffect(int entityId, String effectName)
{
	auto npc = dynamic_cast<EntitySessionNpc*>(getEntity(entityId));
	if (npc)
	{
		npc->setEffectName(effectName);
	}
}

int World::addCreature(Vector3 pos, int monsterId, float yaw, String actorName)
{
	auto creature = LordNew EntityCreature(this, monsterId, pos, yaw, actorName);
	if (spawnEntityInWorld(creature))
		return creature->entityId;
	else
		return 0;
}

int World::addCreatureWithName(Vector3 pos, int monsterId, float yaw, String actorName, String nameLang)
{
	auto creature = LordNew EntityCreature(this, monsterId, pos, yaw, actorName);
	creature->setNameLang(nameLang);
	if (spawnEntityInWorld(creature))
		return creature->entityId;
	else
		return 0;
}

void World::setCreatureTeam(int entityId, int teamId)
{
	auto creature = dynamic_cast<EntityCreature*>(getEntity(entityId));
	if (creature)
		creature->setTeamId(teamId);
}

int World::addActorNpcWithName(Vector3 pos, float yaw, String actorType, String headName, String skillName)
{
	return addActorNpcWithNameAndObstruct(pos, yaw, actorType, headName, skillName, true);
}

int World::addActorNpcWithNameAndObstruct(Vector3 pos, float yaw, String actorType, String headName, String skillName, bool isCanObstruct)
{
	auto ActorNpc = LordNew EntityActorNpc(this, pos);
	ActorNpc->rotationYaw = yaw;
	ActorNpc->setCanObstruct(isCanObstruct);
	if (actorType.length() > 0)
	{
		ActorNpc->setActorName(actorType);
	}

	if (headName.length() > 0)
	{
		ActorNpc->setHeadName(headName);
	}

	if (skillName.length() > 0)
	{
		ActorNpc->setSkillName(skillName);
	}

	if (spawnEntityInWorld(ActorNpc))
	{
		return ActorNpc->entityId;
	}
	else
	{
		return 0;
	}
	
}

void World::updateManorOwner(ui64 rakssid, std::vector<ManorOwner> owners)
{
	if (!m_manorControl)
	{
		m_manorControl = LordNew ManorControl();
	}
	m_manorControl->updateOwner(owners);
}

Vector3 World::getPositionByEntityId(int entityId)
{
	auto entity = getEntity(entityId);
	if (entity)
	{
		return entity->getPosition();
	}

	return Vector3::ZERO;
}

void World::addCprFutureToList(CprHttpFuture cprFuture)
{
	m_cprHttpFutureList.push_back(std::move(cprFuture));
}

void World::checkCprHttpFuture()
{
	std::future_status status;
	for (auto iter = m_cprHttpFutureList.begin(); iter != m_cprHttpFutureList.end(); )
	{
		status = (*iter).wait_for(std::chrono::microseconds(0));
		if (status == std::future_status::ready)
		{
			m_cprHttpFutureList.erase(iter++);
		}
		else
		{
			iter++;
		}
	}
}

int World::addActorNpcWithJsonResult(Vector3 pos, float yaw, String JsonResult)
{
	if (JsonResult.length() == 0)
		return false;

	rapidjson::Document* doc = new rapidjson::Document;
	doc->Parse(JsonResult.c_str());

	if (doc->HasParseError())
	{
		LordLogError("The addActorNpc result content  HasParseError.");
		LordDelete(doc);
		return 0;
	}

	auto ActorNpc = LordNew EntityActorNpc(this, pos);
	ActorNpc->rotationYaw = yaw;

	String ActorName = "";
	String HeadName = "";
	String SkillName = "";
	String EffectName = "";
	bool IsCanObstruct = true;
	bool IsCollided = true;

	if (doc->HasMember("ActorName"))
	{
		ActorName = doc->FindMember("ActorName")->value.GetString();
		if (ActorName.length() > 0)
			ActorNpc->setActorName(ActorName);
	}

	if (doc->HasMember("HeadName"))
	{
		HeadName = doc->FindMember("HeadName")->value.GetString();
		if (HeadName.length() > 0)
			ActorNpc->setHeadName(HeadName);
	}

	if (doc->HasMember("SkillName"))
	{
		SkillName = doc->FindMember("SkillName")->value.GetString();
		if (SkillName.length() > 0)
			ActorNpc->setSkillName(SkillName);
	}

	if (doc->HasMember("EffectName"))
	{
		EffectName = doc->FindMember("EffectName")->value.GetString();
		if (EffectName.length() > 0)
			ActorNpc->setHaloEffectName(EffectName);
	}

	if (doc->HasMember("IsCanObstruct"))
	{
		IsCanObstruct = doc->FindMember("IsCanObstruct")->value.GetBool();
		ActorNpc->setCanObstruct(IsCanObstruct);
	}

	if (doc->HasMember("IsCollided"))
	{
		IsCollided = doc->FindMember("IsCollided")->value.GetBool();
		ActorNpc->setCanCollided(IsCollided);
	}

	if (spawnEntityInWorld(ActorNpc))
	{
		LordDelete(doc);
		return ActorNpc->entityId;
	}
	else
	{
		LordDelete(doc);
		return 0;
	}
}
int World::addBuildNpc(Vector3& pos, float yaw, ui64 userId, i32 actorId)
{
	EntityBuildNpc* entity = LordNew EntityBuildNpc(this, pos);
	entity->m_userId = userId;
	entity->rotationYaw = yaw;
	entity->setPosition(pos);
	entity->setActorId(actorId);
	if (spawnEntityInWorld(entity))
	{
		return entity->entityId;
	}
	return 0;
}

int World::addActorNpcWithCollided(Vector3 pos, float yaw, String actionName, bool isCanCollided)
{
	auto actorNpc = LordNew EntityActorNpc(this, pos);
	actorNpc->rotationYaw = yaw;
	actorNpc->setCanCollided(isCanCollided);

	if (actionName.length() > 0)
	{
		actorNpc->setActorName(actionName);
	}
	if (spawnEntityInWorld(actorNpc))
	{
		return actorNpc->entityId;
	}
	else
	{
		return 0;
	}
}

bool World::rangeCheckBlock(i32 blockId, const BlockPos & startPos, const BlockPos & endPos)
{
	BlockPos newStartPos = startPos.getNW();
	BlockPos newEndPos = endPos.getSE();
	int i = 0;
	for (int x = newStartPos.x; x < newEndPos.x+1 ; x++)
	{
		for (int z = newStartPos.z; z < newEndPos.z +1; z++)
		{
			if (x == newStartPos.x || x == newEndPos.x || z == newStartPos.z || z == newEndPos.z)
			{

				i++;
				//LordLogInfo("================================%d,[x=%d , z=%d]", i, x, z);
				if (blockId == getBlockId(BlockPos(x > endPos.x ? endPos.x : x, startPos.y, z > endPos.z ? endPos.z : z )))
				{

					//LordLogInfo("true>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>%d,[x=%d , z=%d]", i, x > endPos.x ? endPos.x : x, z > endPos.z ? endPos.z : z);
					return true;
				}
				else
				{
					//LordLogInfo("false<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<%d,[x=%d , z=%d]", i, x > endPos.x ? endPos.x : x, z > endPos.z ? endPos.z : z);
				}
			}
		}
	}
	return false;
}

vector<EntityBird*>::type World::getPlayerBirds(ui64 userId)
{
	vector<EntityBird*>::type birds = vector<EntityBird*>::type();
	for (auto it = m_loadedEntityMap.begin(); it != m_loadedEntityMap.end(); it++)
	{
		EntityBird* pBird = dynamic_cast<EntityBird*>(it->second);
		if (pBird && pBird->m_userId)
		{
			birds.push_back(pBird);
		}
	}
	return birds;
}

int World::addRailCar(Vector3 pos, float yaw, String actorName, String nameLang, bool isRun)
{
	auto blockman = LordNew EntityBlockmanEmpty(this, pos, yaw, isRun);
	if (actorName.length() > 0)
		blockman->setActorName(actorName);
	if (nameLang.length() > 0)
		blockman->setNameLang(nameLang);
	if (spawnEntityInWorld(blockman))
		return blockman->entityId;
	else
		return 0;
}

int World::addActorCannon(Vector3 beginPos, Vector3 fallOnPos, String actionName)
{
	auto actorCannon = LordNew EntityActorCannon(this, beginPos, fallOnPos);

	float deltaX = fallOnPos.x - beginPos.x;
	float deltaZ = fallOnPos.z - beginPos.z;
	float a = Math::ATan2(deltaZ, deltaX);
	float ret = a * Math::RAD2DEG;
	if (ret > 360.0f) {
		ret -= 360.0f;
	}
	if (ret < 0.0f) {
		ret += 360.0f;
	}
	ret -= 90.0f;
	actorCannon->rotationYaw = ret;

	if (actionName.length() > 0)
	{
		actorCannon->setActorName(actionName);
	}
	if (spawnEntityInWorld(actorCannon))
	{
		return actorCannon->entityId;
	}
	else
	{
		return 0;
	}
}

}
