#include "WorldClient.h"
#include "cChunk/ChunkClient.h"
#include "cChunk/ChunkServiceClient.h"
#include "cChunk/ChunkReadableStorageFileClient.h"
#include "Blockman.h"

#include "cBlock/cBlock.h"
#include "cBlock/cBlockManager.h"
#include "Object/Root.h"

/** header files from Logic. */
#include "Block/Block.h"
#include "Block/BlockManager.h"
#include "Chunk/Chunk.h"
#include "World/WorldProvider.h"
#include "WorldGenerator/BiomeGen.h"

#include "Render/RenderGlobal.h"
#include "Audio/SoundSystem.h"
#include "game.h"
#include "World/SchematicModel.h"
#include "cWorld/BlockChangeRecorderClient.h"

#include "Util/SkillEffectManager.h"
#include "Entity/EntityPlayer.h"

namespace BLOCKMAN
{

void WorldClient::init2()
{
	m_cloudColour = 0xFFFFFF;
	m_prevRainingStrength = 0;
	m_rainingStrength = 0;
	m_prevThunderingStrength = 0;
	m_thunderingStrength = 0;
	m_lightUpdateBlockList = NULL;
	memset(m_special_sound_tick, 0, sizeof(m_special_sound_tick));
}

WorldClient::WorldClient(const String& name, WorldProvider* _provider, const WorldSettings& settings, int loadRange)
: World(name, _provider, settings, loadRange)
{
	m_isClient = true;

	init2();
	createShop();
	m_lightUpdateBlockList = (int*)LordMalloc(sizeof(int)* 32768);
	memset(m_lightUpdateBlockList, 0, sizeof(int)* 32768);
	
	// par3WorldProvider.registerWorld(this);
	calculateInitialWeather();
}

WorldClient::~WorldClient()
{
	LordSafeFree(m_lightUpdateBlockList);
}

ChunkService* WorldClient::createChunkService(int loadRange)
{
	m_pChunkService = LordNew ChunkServiceClient(this, loadRange);
	auto mapDir = PathUtil::ConcatPath(Root::Instance()->getMapPath(), m_WorldInfo.getMapId() + "/");
	PathUtil::FormatPath(mapDir);
	m_pChunkService->useReadableStorage<ChunkReadableStorageFileClient>(this, mapDir);
	return m_pChunkService;
}

int WorldClient::blockGetRenderType(const BlockPos& pos)
{
	int blockID = getBlockId(pos);

	cBlock* pcBlock = cBlockManager::scBlocks[blockID];

	return pcBlock != NULL ? pcBlock->getRenderType() : -1;
}


void WorldClient::markBlocksDirtyVertical(int x, int z, int y1, int y2)
{
	if (y1 > y2)
		std::swap(y1, y2);


	if (!m_provider->hasNoSky)
	{
		BlockPos pos(x, y1, z);
		for (int i = y1; i <= y2; ++i)
		{
			pos.y = i;
			updateLightByType(SKY_LIGHT_VALUE, pos);
		}
	}

	BlockPos minI(x, y1, z);
	BlockPos maxI(x, y2, z);
	markBlockRangeForRenderUpdate(minI, maxI);
}

void WorldClient::updateAllLightTypes(const BlockPos& pos)
{
	if (!m_provider->hasNoSky)
	{
		updateLightByType(SKY_LIGHT_VALUE, pos);
	}

	updateLightByType(BLOCK_LIGHT_VALUE, pos);
}

int WorldClient::getFullBlockLightValue(const BlockPos& pos)
{
	int y = pos.y;
	if (y < 0)
		return 0;

	if (y >= 256)
		y = 255;

	ChunkPtr pChunck = getChunkFromChunkCoords(pos.x >> 4, pos.z >> 4);
	LordAssert(pChunck);

	return pChunck->getBlockLightValue(pos.getChunkPos(), 0);
}

int WorldClient::getBlockLightValue_impl(const BlockPos& pos, bool flag)
{
	int x = pos.x;
	int y = pos.y;
	int z = pos.z;
	if (!(x >= -MAX_WORLD_X && z >= -MAX_WORLD_Z && x < MAX_WORLD_X && z < MAX_WORLD_Z))
		return 15;

	if (flag)
	{
		int blockID = getBlockId(pos);

		if (BlockManager::sUseNeighborBrightness[blockID])
		{
			int lihgtValue_up = getBlockLightValue_impl(BlockPos(x, y + 1, z), false);
			int lightValue_right = getBlockLightValue_impl(BlockPos(x + 1, y, z), false);
			int lightValue_left = getBlockLightValue_impl(BlockPos(x - 1, y, z), false);
			int lightValue_forward = getBlockLightValue_impl(BlockPos(x, y, z + 1), false);
			int lightValue_backword = getBlockLightValue_impl(BlockPos(x, y, z - 1), false);

			if (lightValue_right > lihgtValue_up)
				lihgtValue_up = lightValue_right;
			if (lightValue_left > lihgtValue_up)
				lihgtValue_up = lightValue_left;
			if (lightValue_forward > lihgtValue_up)
				lihgtValue_up = lightValue_forward;
			if (lightValue_backword > lihgtValue_up)
				lihgtValue_up = lightValue_backword;

			return lihgtValue_up;
		}
	}

	if (y < 0)
		return 0;

	if (y >= 256)
		y = 255;

	ChunkPtr pChunk = getChunkFromChunkCoords(x >> 4, z >> 4);
	LordAssert(pChunk);

	BlockPos chunkPos = pos.getChunkPos();
	chunkPos.y = y;
	return pChunk->getBlockLightValue(chunkPos, m_skylightSubtracted);
}

int WorldClient::getSkyBlockTypeBrightness(int light, const BlockPos& pos)
{
	if (m_provider->hasNoSky && light == SKY_LIGHT_VALUE)
	{
		return 0;
	}
	else
	{
		int x = pos.x;
		int y = pos.y;
		int z = pos.z;
		if (y < 0)
		{
			y = 0;
		}

		if (y >= 256)
		{
			return light;
		}
		else if (x >= -MAX_WORLD_X && z >= -MAX_WORLD_Z && x < MAX_WORLD_X && z < MAX_WORLD_Z)
		{
			int xinChunk = x >> 4;
			int zinChunk = z >> 4;

			if (!chunkExists(xinChunk, zinChunk))
			{
				return light;
			}
			else if (BlockManager::sUseNeighborBrightness[getBlockId(BlockPos(x, y, z))])
			{
				int lightValue_up = getSavedLightValue(light, BlockPos(x, y + 1, z));
				int lightValue_right = getSavedLightValue(light, BlockPos(x + 1, y, z));
				int lightValue_left = getSavedLightValue(light, BlockPos(x - 1, y, z));
				int lightValue_forward = getSavedLightValue(light, BlockPos(x, y, z + 1));
				int lightValue_backward = getSavedLightValue(light, BlockPos(x, y, z - 1));

				if (lightValue_right > lightValue_up)
					lightValue_up = lightValue_right;
				if (lightValue_left > lightValue_up)
					lightValue_up = lightValue_left;
				if (lightValue_forward > lightValue_up)
					lightValue_up = lightValue_forward;
				if (lightValue_backward > lightValue_up)
					lightValue_up = lightValue_backward;

				return lightValue_up; 
			}
			else
			{
				ChunkPtr pChunk = getChunkFromChunkCoords(xinChunk, zinChunk);
				LordAssert(pChunk);
				return pChunk->getSavedLightValue(light, BlockPos(x & 15, y, z & 15));
			}
		}
		else
		{
			return light;                 
		}
	}
}

int WorldClient::getSavedLightValue(int lightValue, const BlockPos& pos)
{
	int y = pos.y;
	if (y < 0)
		y = 0;
	if (y >= 256)
		y = 255;

	if (pos.x >= -MAX_WORLD_X && pos.z >= -MAX_WORLD_Z && pos.x < MAX_WORLD_X && pos.z < MAX_WORLD_Z)
	{
		int xinChunk = pos.x >> 4;
		int zinChunk = pos.z >> 4;

		if (!chunkExists(xinChunk, zinChunk))
		{
			return lightValue;
		}
		else
		{
			ChunkPtr pChunk = getChunkFromChunkCoords(xinChunk, zinChunk);
			if (pChunk)
				return pChunk->getSavedLightValue(lightValue, BlockPos(pos.x & 15, y, pos.z & 15));
			else
				return lightValue;
		}
	}
	else
	{
		return lightValue;
	}
}

void WorldClient::setLightValue(int lightValue, const BlockPos& pos, int newLightValue)
{
	if (!(pos.x >= -MAX_WORLD_X && pos.z >= -MAX_WORLD_Z && pos.x < MAX_WORLD_X && pos.z < MAX_WORLD_Z))
		return;

	if (pos.y < 0 || pos.y >= 256)
		return;


	if (chunkExists(pos.x >> 4, pos.z >> 4))
	{
		ChunkPtr pChunk = getChunkFromChunkCoords(pos.x >> 4, pos.z >> 4);
		LordAssert(pChunk);

		pChunk->setLightValue(lightValue, pos.getChunkPos(), newLightValue);

		for (WorldListeners::iterator it = m_worldListeners.begin(); it != m_worldListeners.end(); ++it)
		{
			IWorldEventListener* listener = *it;
			listener->markBlockForRenderUpdate(pos);
		}
		/*for (int var7 = 0; var7 < worldAccesses.size(); ++var7)
		{
		((IWorldAccess)worldAccesses.get(var7)).markBlockForRenderUpdate(x, y, z);
		}*/
	}
}

int WorldClient::getLightBrightnessForSkyBlocks(const BlockPos& pos, int minBrightness)
{
	int skyLight = getSkyBlockTypeBrightness(SKY_LIGHT_VALUE, pos);
	int blockLight = getSkyBlockTypeBrightness(BLOCK_LIGHT_VALUE, pos);

	if (blockLight < minBrightness)
	{
		blockLight = minBrightness;
	}

	return skyLight << 20 | blockLight << 4;
}

float WorldClient::getBrightness(const BlockPos& pos, int minBrightness)
{
	int brightness = getBlockLightValue(pos);

	if (brightness < minBrightness)
	{
		brightness = minBrightness;
	}

	return m_provider->lightBrightnessTable[brightness];
}

float WorldClient::getLightBrightness(const BlockPos& pos)
{
	return m_provider->lightBrightnessTable[getBlockLightValue(pos)];
}

bool WorldClient::setBlocks(int x, int z, const BlockModifys& modifys)
{
	bool ret = World::setBlocks(x, z, modifys);

// 	for (auto block : modifys)
// 	{
// 		BlockChangeRecorderClient::Instance()->record(block.pos, block.id, (uint8_t)block.meta);
// 	}
	BlockChangeRecorderClient::Instance()->recordBlocks(x, z, modifys);

	if (ret)
	{
		ChunkPtr pChunk = getChunkFromChunkCoords(x, z);
		if (!pChunk)
			return false;
	
		if (modifys.empty())
			return false;

		// 60 tick after to rebuild all the section (with sync to rebuild chunk.);
		int chunkid =( (x + 1024) << 16) | (z + 1024);
		m_reBuildEvent[chunkid] = 10;
	}

	return ret;
}

bool WorldClient::setBlock(const BlockPos& pos, int blockID, int metadata, int flag, bool immediate)
{
	//LordLogInfo("setBlick[%d,%d,%d] id:%d meta:%d", pos.x, pos.y, pos.z, blockID, metadata);
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

	bool modifyed = pChunk->setBlockIDAndMeta(pos.getChunkPos(), blockID, metadata);
	
	updateAllLightTypes(pos);

	if (modifyed)
	{
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

void WorldClient::playSoundByType(const Vector3& pos, SoundType soundType)
{
	float dis = 16.0F;
	float volume = 1.0F; // ����Ĭ����1
	if (volume > 1.0F)
		dis *= volume;

	float disSqr = Blockman::Instance()->renderViewEntity->getDistanceSq(pos);
	if (disSqr > (dis * dis))
		return;
	
	SoundSystem::Instance()->playEffectByType(soundType);
}

void WorldClient::playSoundOfPlaceBlock(const BlockPos & pos, int blockId)
{
	auto block = cBlockManager::scBlocks[blockId];
	//playSound(Vector3(pos) + 0.5f, block->getStepSound().getPlaceSound(), (block->getStepSound().volume + 1.0F) / 2.0F, block->getStepSound().pitch * 0.8F, false);
	playSoundByType(Vector3(pos) + 0.5f, block->getStepSound().placeSound);
}

void WorldClient::playSoundOfStepOnBlock(const BlockPos & pos, int blockId)
{
	auto block = cBlockManager::scBlocks[blockId];
	//playSound(Vector3(pos) + 0.5f, block->getStepSound().getStepSound(), (block->getStepSound().volume + 1.0F) / 2.0F, block->getStepSound().pitch * 0.8F, false);
	playSoundByType(Vector3(pos) + 0.5f, block->getStepSound().stepSound);
}

void WorldClient::doVoidFogParticles(BlockPos pos)
{
	int bound = 16;

	BlockPos checkpos;
	for (int i = 0; i < 1000; ++i)
	{
		checkpos = pos;
		checkpos.x += m_Rand.nextInt(bound) - m_Rand.nextInt(bound);
		checkpos.y += m_Rand.nextInt(bound) - m_Rand.nextInt(bound);
		checkpos.z += m_Rand.nextInt(bound) - m_Rand.nextInt(bound);
		if (!checkChunksExist(checkpos, checkpos))
		{
			continue;
		}
		int blockId = getBlockId(checkpos);

		if (blockId == 0 && m_Rand.nextInt(8) > checkpos.y /*&& provider->getWorldHasVoidParticles()*/)
		{
			Vector3 particlePos = checkpos;
			particlePos.x += m_Rand.nextFloat();
			particlePos.y += m_Rand.nextFloat();
			particlePos.z += m_Rand.nextFloat();
			spawnParticle("depthsuspend", particlePos, BlockPos::ZERO);
		}
		else if (blockId > 0)
		{
			cBlockManager::scBlocks[blockId]->randomDisplayTick(this, checkpos, m_Rand);
		}
	}
}

void WorldClient::createOrDestroyHouseFromSchematic(String path, const BlockPos & start_pos, bool xImage, bool zImage, bool createOrDestroy)
{
	// parser .Schematic file
	auto schematicModel = LordNewT(SchematicModel);
	schematicModel->resetData();

	if (schematicModel->deserialize(path.c_str()))
	{
		int element_count = 0;
		i16 width = schematicModel->getWidth();
		i16 height = schematicModel->getHeight();
		i16 length = schematicModel->getLength();

		std::map< ChunkInfo, BlockModifys > chunk_blocks;
		chunk_blocks.clear();

		// from the bottom to top
		for (i16 y = 0; y < height; ++y)
		{
			for (i16 x = 0; x < width; ++x)
			{
				for (i16 z = 0; z < length; ++z)
				{
					int index = (y * length + z) * width + x;

					ui8 block_id = schematicModel->getBlockId(index);

					if (block_id <= 0) continue;

					ui8 mate = schematicModel->getMate(index);

					if (BlockManager::isStairsBlock((int)block_id))
					{
						mate = BlockManager::StairsBlockRotate(mate, xImage, zImage);
					}

					element_count++;

					int pos_x = (int)x;
					int pos_y = (int)y;
					int pos_z = (int)z;

					int pos_x_old = pos_x;
					int pos_y_old = pos_y;
					int pos_z_old = pos_z;

					// 绕点旋转 270 180 90算法
					if (xImage && !zImage)
					{
						pos_x = length - pos_z_old;
						pos_z = pos_x_old;
					}
					else if (xImage && zImage)
					{
						pos_x = -pos_x_old;
						pos_z = length - pos_z_old;

						pos_x += width;
					}
					else if (!xImage && zImage)
					{
						pos_x = -(length - pos_z_old);
						pos_z = -pos_x_old;

						pos_z += width;
						pos_x += length;
					}
					else
					{

					}

					pos_x += start_pos.x;
					pos_y += start_pos.y;
					pos_z += start_pos.z;

					Vector3i position(pos_x, pos_y, pos_z);

					if (!createOrDestroy)
					{
						block_id = 0;
						mate = 0;
					}

					BlockModifyInfo chunk_block;
					chunk_block.pos = position;
					chunk_block.id = block_id;
					chunk_block.meta = mate;
					ChunkInfo chunk_info(pos_x >> 4, pos_z >> 4);

					auto iter = chunk_blocks.find(chunk_info);
					if (iter != chunk_blocks.end())
					{
						iter->second.push_back(chunk_block);
					}
					else
					{
						BlockModifys block_info;
						block_info.push_back(chunk_block);
						chunk_blocks[chunk_info] = block_info;
					}
				}
			}
		}

		for (auto chunk : chunk_blocks)
		{
			setBlocks(chunk.first.chunkX, chunk.first.chunkZ, chunk.second);
		}

		chunk_blocks.clear();

		LordLogInfo("WorldClient::createOrDestroyHouseFromSchematic create_or_destroy:%d path:%s start_pos x:%d y:%d z:%d", createOrDestroy ? 1 : 0, path.c_str(), start_pos.x, start_pos.y, start_pos.z);
		LordLogInfo("WorldClient::createOrDestroyHouseFromSchematic width:%d height:%d length:%d element_count:%d", width, height, length, element_count);
	}
	else
	{
		LordLogError("WorldClient::createOrDestroyHouseFromSchematic Failed to deserialize schematic: %s", path.c_str());
	}
	LordSafeDelete(schematicModel);
}

void WorldClient::fillAreaByBlockIdAndMate(const BlockPos & start_pos, const BlockPos & end_pos, int block_id, int mate)
{
	// if (block_id <= 0) return;

	int begin_x = start_pos.x < end_pos.x ? start_pos.x : end_pos.x;
	int begin_y = start_pos.y < end_pos.y ? start_pos.y : end_pos.y;
	int begin_z = start_pos.z < end_pos.z ? start_pos.z : end_pos.z;
	int end_x = start_pos.x < end_pos.x ? end_pos.x : start_pos.x;
	int end_y = start_pos.y < end_pos.y ? end_pos.y : start_pos.y;
	int end_z = start_pos.z < end_pos.z ? end_pos.z : start_pos.z;

	std::map< ChunkInfo, BlockModifys > chunk_blocks;
	chunk_blocks.clear();

	for (int y = begin_y; y <= end_y; ++y)
	{
		for (int x = begin_x; x <= end_x; ++x)
		{
			for (int z = begin_z; z <= end_z; ++z)
			{
				Vector3i position(x, y, z);

				BlockModifyInfo chunk_block;
				chunk_block.pos = position;
				chunk_block.id = (ui16)block_id;
				chunk_block.meta = (ui16)mate;
				ChunkInfo chunk_info(x >> 4, z >> 4);

				auto iter = chunk_blocks.find(chunk_info);
				if (iter != chunk_blocks.end())
				{
					iter->second.push_back(chunk_block);
				}
				else
				{
					BlockModifys block_info;
					block_info.push_back(chunk_block);
					chunk_blocks[chunk_info] = block_info;
				}
			}
		}
	}

	for (auto chunk : chunk_blocks)
	{
		setBlocks(chunk.first.chunkX, chunk.first.chunkZ, chunk.second);
	}

	chunk_blocks.clear();

	LordLogInfo("WorldClient::fillAreaByBlockIdAndMate start_pos x:%d y:%d z:%d end_pos x:%d y:%d z:%d block_id:%d mate:%d", start_pos.x, start_pos.y, start_pos.z, end_pos.x, end_pos.y, end_pos.z, block_id, mate);
}

void WorldClient::handleLargeBlocks(const BlockModifys& blocks_info)
{
	std::map< ChunkInfo, BlockModifys > chunk_blocks;
	chunk_blocks.clear();

	for (const auto& blockInfo : blocks_info)
	{
		Vector3i position(blockInfo.pos.x, blockInfo.pos.y, blockInfo.pos.z);

		BlockModifyInfo chunk_block;
		chunk_block.pos = position;
		chunk_block.id = blockInfo.id;
		chunk_block.meta = blockInfo.meta;
		ChunkInfo chunk_info(blockInfo.pos.x >> 4, blockInfo.pos.z >> 4);

		auto iter = chunk_blocks.find(chunk_info);
		if (iter != chunk_blocks.end())
		{
			iter->second.push_back(chunk_block);
		}
		else
		{
			BlockModifys block_info;
			block_info.push_back(chunk_block);
			chunk_blocks[chunk_info] = block_info;
		}
	}

	for (auto chunk : chunk_blocks)
	{
		setBlocks(chunk.first.chunkX, chunk.first.chunkZ, chunk.second);
	}

	chunk_blocks.clear();
}

bool WorldClient::checkRangeBlockIsAir(const Vector3 & beginPos, const Vector3 & endPos)
{
	EntityArr entities = getEntitiesWithinAABB(ENTITY_CLASS_ENTITY, Box(beginPos, endPos));
	if (entities.size() > 0)
	{
		return false;
	}

	for (int x = (int)beginPos.x	; x < endPos.x; x ++ )
	{
		for (int y = (int)beginPos.y; y < endPos.y; y++)
		{
			for (int z = (int)beginPos.z; z < endPos.z; z++)
			{
				i32 blockId = getBlockId(Vector3i(x, y, z));
				if (blockId != 0 )
				{
					return false;
				}
			}
		}
	}
	return true;
}

void WorldClient::setPoisonCircleRange(Vector3 safeRange1, Vector3 safeRange2, Vector3 poisonRange1, Vector3 poisonRange2, float speed)
{
	Blockman::Instance()->m_globalRender->setPoisonCircleRangeAndSpeed(getPoisonCircleRange(safeRange1, safeRange2, poisonRange1, poisonRange2), speed);
}

float WorldClient::getFrameDeltaTime()
{
	//return GameClient::CGame::Instance()->getFrameTime() / 1000.0f;
	return Blockman::s_tickFPS;
}

void WorldClient::spawnParticle(const String& name, const Vector3& pos, const Vector3& vel, EntityPlayer* emmiter)
{
	for (WorldListeners::iterator it = m_worldListeners.begin(); it != m_worldListeners.end(); ++it)
	{
		(*it)->spawnParticle(name, pos, vel, emmiter);
	}
}

void WorldClient::playSoundAtEntityByType(Entity* pEntity, SoundType soundType)
{
	if (pEntity != NULL && soundType > ST_Invalid)
	{
		for (WorldListeners::iterator it = m_worldListeners.begin(); it != m_worldListeners.end(); ++it)
		{
			(*it)->playSound(pEntity->position + Vector3(0.f, -pEntity->yOffset, 0.f), soundType);
		}
	}
}

float WorldClient::getSunBrightness(float rdt)
{
	float celestail = getCelestialAngle(rdt);
	float brightness = 1.0f - (Math::Cos_tab(celestail * Math::PI_2) * 2.0f + 0.2f);

	if (brightness < 0.0f)
	{
		brightness = 0.0f;
	}

	if (brightness > 1.0f)
	{
		brightness = 1.0f;
	}

	brightness = 1.0f - brightness;
	brightness = brightness * (1.0f - (getRainStrength(rdt) * 5.0f) / 16.f);
	brightness = brightness * (1.0f - (getWeightedThunderStrength(rdt) * 5.0f) / 16.f);
	return brightness * 0.8F + 0.2F;
}

void WorldClient::tickRebuildChunks()
{
	for (auto& it : m_reBuildEvent)
	{
		it.second--;
	}

	for (auto it : m_reBuildEvent)
	{
		if (it.second <= 0)
		{
			int z = (it.first & 0x0000FFFF) - 1024;
			int x = (it.first >> 16) - 1024;

			auto pChunk = std::static_pointer_cast<ChunkClient>(getChunkFromChunkCoords(x, z));
			if (!pChunk)
				continue;

			pChunk->m_isReBuildVisibility = false;
			pChunk->m_isSkyLightingUpdated = false;
			pChunk->m_isBlockLightingUpdated = false;
			pChunk->m_isPrepared = false;
			pChunk->m_isModified = true;
			pChunk->m_needRebuildAll = true;

			for (WorldListeners::iterator it = m_worldListeners.begin(); it != m_worldListeners.end(); ++it)
			{
				IWorldEventListener * listener = *it;
				listener->markBlockRangeForRenderUpdate(x*16+1, 1, z*16+1, x*16+14, 254, z*16+14);
			}
			m_reBuildEvent.erase(it.first);
			return;
		}
	}
}

void WorldClient::tickAroundAmbiance()
{
	for (auto it : m_playerEntities)
	{
		EntityPlayer* pPlayer = it.second;
		int chunk_x = int(Math::Floor(pPlayer->position.x)) >> 4;
		int chunk_z = int(Math::Floor(pPlayer->position.z)) >> 4;
		int y = int(pPlayer->position.y);

		// rebuild the 25 chunks around the player.
		int bound = 2;
		for (int ix = chunk_x - bound; ix <= chunk_x + bound; ++ix)
		{
			for (int iz = chunk_z - bound; iz <= chunk_z + bound; ++iz)
			{
				auto pChunk = std::static_pointer_cast<ChunkClient>(getChunkFromChunkCoords(ix, iz));
				if (!pChunk)
					continue;

				int v = 5 - (Math::Abs(chunk_x - ix)) + (Math::Abs(chunk_z - iz));
				pChunk->m_updateCD -= v;
				if (pChunk->m_updateCD <= 0)
				{
					pChunk->m_isBlockLightingUpdated = false;
					//pChunk->m_isSkyLightingUpdated = false;
					pChunk->m_isModified = true;
					pChunk->m_updateCD = 3200;

					markBlockForUpdate(BlockPos((ix << 4) + 8, y, (iz << 4) + 8));
				}
			}
		}
	}
}


Color WorldClient::getSkyColor(Entity* pEntity, float rdt)
{
	float celestial = getCelestialAngle(rdt);
	float brightness = Math::Cos_tab(celestial * Math::PI_2) * 2.0F + 0.5F;

	if (brightness < 0.0F)
	{
		brightness = 0.0F;
	}

	if (brightness > 1.0F)
	{
		brightness = 1.0F;
	}

	int x = int(Math::Floor(pEntity->getPosition().x));
	int z = int(Math::Floor(pEntity->getPosition().z));
	BiomeGenBase* pBiome = getBiomeGenForCoords(x, z);
	float temperature = pBiome->getFloatTemperature();
	int skyColor = pBiome->getSkyColorByTemp(temperature);
	float r = (float)(skyColor >> 16 & 255) / 255.0F;
	float g = (float)(skyColor >> 8 & 255) / 255.0F;
	float b = (float)(skyColor & 255) / 255.0F;
	r *= brightness;
	g *= brightness;
	b *= brightness;
	float rainStrength = getRainStrength(rdt);

	if (rainStrength > 0.0F)
	{
		float f1 = (r * 0.3F + g * 0.59F + b * 0.11F) * 0.6F;
		float f2 = 1.0F - rainStrength * 0.75F;
		r = r * f2 + f1 * (1.0F - f2);
		g = g * f2 + f1 * (1.0F - f2);
		b = b * f2 + f1 * (1.0F - f2);
	}

	float thunderStrength = getWeightedThunderStrength(rdt);

	if (thunderStrength > 0.0F)
	{
		float f1 = (r * 0.3F + g * 0.59F + b * 0.11F) * 0.2F;
		float f2 = 1.0F - thunderStrength * 0.75F;
		r = r * f2 + f1 * (1.0F - f2);
		g = g * f2 + f1 * (1.0F - f2);
		b = b * f2 + f1 * (1.0F - f2);
	}

	if (m_lastLightningBolt > 0)
	{
		float f1 = (float)m_lastLightningBolt - rdt;

		if (f1 > 1.0F)
		{
			f1 = 1.0F;
		}

		f1 *= 0.45F;
		r = r * (1.0F - f1) + 0.8F * f1;
		g = g * (1.0F - f1) + 0.8F * f1;
		b = b * (1.0F - f1) + 1.0F * f1;
	}

	return Color(r, g, b);
}

int WorldClient::getMoonPhase()
{
	return m_provider->getMoonPhase(m_WorldInfo.getWorldTime());
}


Color WorldClient::getCloudColour(float rdt)
{
	float celestial = getCelestialAngle(rdt);
	float brightness = Math::Cos(celestial * Math::PI_2) * 2.0F + 0.5F;

	if (brightness < 0.0F)
	{
		brightness = 0.0F;
	}

	if (brightness > 1.0F)
	{
		brightness = 1.0F;
	}

	float r = (float)(m_cloudColour >> 16 & 255L) / 255.0F;
	float g = (float)(m_cloudColour >> 8 & 255L) / 255.0F;
	float b = (float)(m_cloudColour & 255L) / 255.0F;
	float rainStrength = getRainStrength(rdt);

	if (rainStrength > 0.0F)
	{
		float f1 = (r * 0.3F + g * 0.59F + b * 0.11F) * 0.6F;
		float f2 = 1.0F - rainStrength * 0.95F;
		r = r * f2 + f1 * (1.0F - f2);
		g = g * f2 + f1 * (1.0F - f2);
		b = b * f2 + f1 * (1.0F - f2);
	}

	r *= brightness * 0.9F + 0.1F;
	g *= brightness * 0.9F + 0.1F;
	b *= brightness * 0.85F + 0.15F;
	float thunderStrength = getWeightedThunderStrength(rdt);

	if (thunderStrength > 0.0F)
	{
		float f1 = (r * 0.3F + g * 0.59F + b * 0.11F) * 0.2F;
		float f2 = 1.0F - thunderStrength * 0.95F;
		r = r * f2 + f1 * (1.0F - f2);
		g = g * f2 + f1 * (1.0F - f2);
		b = b * f2 + f1 * (1.0F - f2);
	}

	return Color(r, g, b);
}

Color WorldClient::getFogColor(float rdt)
{
	float var2 = getCelestialAngle(rdt);
	return m_provider->getFogColor(var2, rdt);
}

float WorldClient::getStarBrightness(float rdt)
{
	float celestial = getCelestialAngle(rdt);
	float brightness = 1.0f - (Math::Cos_tab(celestial * Math::PI_2) * 2.0f + 0.25f);

	if (brightness < 0.0f)
	{
		brightness = 0.0f;
	}

	if (brightness > 1.0f)
	{
		brightness = 1.0f;
	}

	return brightness * brightness * 0.5f;
}

void WorldClient::calculateInitialWeather()
{
	if (m_WorldInfo.isRaining())
	{
		m_rainingStrength = 1.0F;

		if (m_WorldInfo.isThundering())
		{
			m_thunderingStrength = 1.0F;
		}
	}
}

void WorldClient::tick()
{
	updateWeather();
	World::tickUpdates(false);
	tickAroundAmbiance();
	SkillEffectManager::Instance()->onUpdate();
	tickRebuildChunks();
	updateSpecialSound();
}

void WorldClient::updateWeather()
{
	if (!m_provider->hasNoSky)
	{
		int thunderTime = m_WorldInfo.getThunderTime();

		if (thunderTime <= 0)
		{
			if (m_WorldInfo.isThundering())
			{
				m_WorldInfo.setThunderTime(m_Rand.nextInt(12000) + 3600);
			}
			else
			{
				m_WorldInfo.setThunderTime(m_Rand.nextInt(168000) + 12000);
			}
		}
		else
		{
			--thunderTime;
			m_WorldInfo.setThunderTime(thunderTime);

			if (thunderTime <= 0)
			{
				m_WorldInfo.setThundering(!m_WorldInfo.isThundering());
			}
		}

		int rainTime = m_WorldInfo.getRainTime();

		if (rainTime <= 0)
		{
			if (m_WorldInfo.isRaining())
			{
				m_WorldInfo.setRainTime(m_Rand.nextInt(12000) + 12000);
			}
			else
			{
				m_WorldInfo.setRainTime(m_Rand.nextInt(168000) + 12000);
			}
		}
		else
		{
			--rainTime;
			m_WorldInfo.setRainTime(rainTime);

			if (rainTime <= 0)
			{
				m_WorldInfo.setRaining(!m_WorldInfo.isRaining());
			}
		}

		m_prevRainingStrength = m_rainingStrength;

		if (m_WorldInfo.isRaining())
		{
			m_rainingStrength = (float)((double)m_rainingStrength + 0.01);
		}
		else
		{
			m_rainingStrength = (float)((double)m_rainingStrength - 0.01);
		}

		if (m_rainingStrength < 0.0F)
		{
			m_rainingStrength = 0.0F;
		}

		if (m_rainingStrength > 1.0F)
		{
			m_rainingStrength = 1.0F;
		}

		m_prevThunderingStrength = m_thunderingStrength;

		if (m_WorldInfo.isThundering())
		{
			m_thunderingStrength = (float)((double)m_thunderingStrength + 0.01);
		}
		else
		{
			m_thunderingStrength = (float)((double)m_thunderingStrength - 0.01);
		}

		if (m_thunderingStrength < 0.0F)
		{
			m_thunderingStrength = 0.0F;
		}

		if (m_thunderingStrength > 1.0F)
		{
			m_thunderingStrength = 1.0F;
		}
	}
}

int WorldClient::computeLightValue(const BlockPos& pos, int lightValue)
{
	if (lightValue == SKY_LIGHT_VALUE && canBlockSeeTheSky(pos))
	{
		return 15;
	}
	else
	{
		int blockID = getBlockId(pos);
		int lv = lightValue == SKY_LIGHT_VALUE ? 0 : BlockManager::sLightValue[blockID];
		int lo = BlockManager::sLightOpacity[blockID];

		if (lo >= 15 && BlockManager::sLightValue[blockID] > 0)
		{
			lo = 1;
		}

		if (lo < 1)
		{
			lo = 1;
		}

		if (lo >= 15)
		{
			return 0;
		}
		else if (lv >= 14)
		{
			return lv;
		}
		else
		{
			for (int side = 0; side < 6; ++side)
			{
				BlockPos neighbor = pos + offsetForSide[side];
				int neighborLight = getSavedLightValue(lightValue, neighbor) - lo;

				if (neighborLight > lv)
				{
					lv = neighborLight;
				}

				if (lv >= 14)
				{
					return lv;
				}
			}

			return lv;
		}
	}
}

bool WorldClient::canPlaySpecialSound(int sst, int max_tick)
{
	if (sst < 0 || sst >= SST_COUNT || sst >= MAX_SPECIAL_SOUND_TYPE) return false;

	int limit_max_tick = MAX_SPECIAL_SOUND_TICK;
	if (max_tick > 0)
	{
		limit_max_tick = max_tick;
	}
	if (m_special_sound_tick[sst] > limit_max_tick)
	{
		m_special_sound_tick[sst] = 0;
		return true;
	}

	return false;
}

void WorldClient::updateSpecialSound()
{
	for (int i = 0; i < MAX_SPECIAL_SOUND_TYPE; ++i)
	{
		m_special_sound_tick[i]++;
		if (m_special_sound_tick[i] > MAX_SPECIAL_SOUND_TICK_LIMIT)
		{
			m_special_sound_tick[i] = 0;
		}
	}
}

bool WorldClient::updateLightByType(int light, const BlockPos& pos)
{
	if (!doChunksNearChunkExist(pos, 16))
		return false;

	int idx = 0;
	int idx2 = 0;

	int* lightUpdateLst = m_lightUpdateBlockList;

	int savedLight = getSavedLightValue(light, pos);
	int computeLight = computeLightValue(pos, light);
	int light1 = 0;
	int light2 = 0;
	int light3 = 0;
	BlockPos lightPos = pos;

	if (computeLight > savedLight)
	{
		lightUpdateLst[idx2++] = 133152;
	}
	else if (computeLight < savedLight)
	{
		lightUpdateLst[idx2++] = 133152 | savedLight << 18;

		while (idx < idx2)
		{
			light1 = lightUpdateLst[idx++];
			lightPos.x = (light1 & 63) - 32 + pos.x;
			lightPos.y = ((light1 >> 6) & 63) - 32 + pos.y;
			lightPos.z = ((light1 >> 12) & 63) - 32 + pos.z;
			light3 = light1 >> 18 & 15;
			light2 = getSavedLightValue(light, lightPos);

			if (light2 == light3)
			{
				setLightValue(light, lightPos, 0);

				if (light3 > 0)
				{
					BlockPos delPos = (lightPos - pos).abs();

					if (delPos.x + delPos.y + delPos.z < 16)
					{
						for (int i = 0; i < 6; ++i)
						{
							BlockPos opacityPos = lightPos + offsetForSide[i];
							int opacity = Math::Max(1, BlockManager::sLightOpacity[getBlockId(opacityPos)]);
							light2 = getSavedLightValue(light, opacityPos);

							if (light2 == light3 - opacity && idx2 < 32768)
							{
								lightUpdateLst[idx2++] =
									(opacityPos.x - pos.x + 32) |
									((opacityPos.y - pos.y + 32) << 6) |
									((opacityPos.z - pos.z + 32) << 12) |
									((light3 - opacity) << 18);
							}
						}
					}
				}
			}
		}

		idx = 0;
	}

	while (idx < idx2)
	{
		light1 = lightUpdateLst[idx++];
		lightPos.x = (light1 & 63) - 32 + pos.x;
		lightPos.y = ((light1 >> 6) & 63) - 32 + pos.y;
		lightPos.z = ((light1 >> 12) & 63) - 32 + pos.z;
		light3 = getSavedLightValue(light, lightPos);
		light2 = computeLightValue(lightPos, light);

		if (light2 != light3)
		{
			setLightValue(light, lightPos, light2);

			if (light2 > light3)
			{
				BlockPos delPos = (lightPos - pos).abs();
				bool indexValid = idx2 < (32768 - 6);

				if (delPos.x + delPos.y + delPos.z < 16 && indexValid)
				{
					BlockPos del;
					BlockPos west = lightPos; --west.x;
					if (getSavedLightValue(light, west) < light2)
					{
						del = west - pos + 32;
						lightUpdateLst[idx2++] = del.x + (del.y << 6) + (del.z << 12);
					}

					BlockPos east = lightPos; ++east.x;
					if (getSavedLightValue(light, east) < light2)
					{
						del = east - pos + 32;
						lightUpdateLst[idx2++] = del.x + (del.y << 6) + (del.z << 12);
					}

					BlockPos down = lightPos; --down.y;
					if (getSavedLightValue(light, down) < light2)
					{
						del = down - pos + 32;
						lightUpdateLst[idx2++] = del.x + (del.y << 6) + (del.z << 12);
					}

					BlockPos upper = lightPos; ++upper.y;
					if (getSavedLightValue(light, upper) < light2)
					{
						del = upper - pos + 32;
						lightUpdateLst[idx2++] = del.x + (del.y << 6) + (del.z << 12);
					}

					BlockPos north = lightPos; --north.z;
					if (getSavedLightValue(light, north) < light2)
					{
						del = north - pos + 32;
						lightUpdateLst[idx2++] = del.x + (del.y << 6) + (del.z << 12);
					}

					BlockPos south = lightPos; ++south.z;
					if (getSavedLightValue(light, south) < light2)
					{
						del = south - pos + 32;
						lightUpdateLst[idx2++] = del.x + (del.y << 6) + (del.z << 12);
					}
				}
			}
		}
	}
	return true;
}

float WorldClient::getMoonPhaseFactor()
{
	return WorldProvider::MOON_PHASE_FACTORS[m_provider->getMoonPhase(m_WorldInfo.getWorldTime())];
}

ui64 WorldClient::findManorByPlayerPosition(EntityPlayer * player)
{
	if (m_manorControl && player)
	{
		Vector3 position = player->position;
		for (auto  item : m_manorControl->getOwners())
		{
			if (item.origin.x < position.x && item.origin.z < position.z && item.termini.x > position.x && item.termini.z > position.z && item.ownerId != player->getPlatformUserId())
			{
				return item.ownerId;
			}
		}
	}
	return 0;
}

}
