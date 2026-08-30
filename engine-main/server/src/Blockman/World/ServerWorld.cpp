#include "ServerWorld.h"
#include "ServerWorldProvider.h"
#include "Blockman/Chunk/ChunkServiceServer.h"
#include "Blockman/Chunk/ChunkReadableStorageFileServer.h"
#include "Blockman/Chunk/ChunkProviderServerFile.h"   // disk-first / generate-on-miss provider
#include "Chunk/ChunkProviderCustom.h"
#include "Chunk/ChunkProviderGenerate.h"
#include "WorldServerListener.h"
#include "ServerShop.h"

#include "Blockman/Entity/EntityTracker.h"
#include "Blockman/World/BlockChangeRecorderServer.h"
#include "Object/Root.h"
#include "Std/chrono.h"
#include "Block/Block.h"
#include "Block/BlockManager.h"
#include "Server.h"
#include "Network/ServerNetwork.h"
#include "Script/GameServerEvents.h"
#include "CommodityManager.h"
#include "BlockDoorManager.h"
#include "Blockman/Entity/EntityCreatureAI.h"
#include "World/SchematicModel.h"
#include "BlockCropsManager.h"
#include "Entity/EntityActorNpc.h"
#include "Entity/EntityBlockmanEmpty.h"
#include "Common.h"
#include "Entity/EntityBuildNpc.h"
#include "Entity/EntityLandNpc.h"
#include "Entity/EntityBulletin.h"
#include "Blockman/Entity/EntityBirdAI.h"
#include "Blockman/World/BlockFruitsManager.h"
#include "Setting/SkillSetting.h"
#include "LuaRegister/Template/TableVisitor.h"
#include "WorldGenerator/BiomeGen.h"   // BiomeGenBase::decorate (populate step)
#include "Util/Random.h"               // per-chunk decoration seed

// Custom world-type value. Lives in the "extended" range so it does not
// collide with vanilla TERRAIN_TYPE_* values (0..3). Picked up by
// ServerWorldProviderSurface::createChunkGenerator() to return
// ChunkProviderCustom (sky islands).
namespace {
constexpr int TERRAIN_TYPE_CUSTOM = 100;
}

namespace BLOCKMAN
{

i64 test_rand[] = {
        3315535095586843436LL,
        7811472220827660346LL,
        27977402478636135LL,
        -2961405825330349164LL,
        -9039153669293870600LL,
        6727893985866532711LL,
        -1777905325754646240LL,
        -3969516832809676251LL,
        -3378829271918789601LL,
        -2720273531442913371LL
};

Vector3i spwanPos[] = {
        Vector3i(153, 64, 219),
        Vector3i(-76, 64, 132),
        Vector3i(-24, 64, 256),
        Vector3i(215, 64, 264),
        Vector3i(0, 64, 0),
        Vector3i(-240, 64, 240),
        Vector3i(-475, 64, 60),
        Vector3i(224, 64, 248),
        Vector3i(289, 65, -67),
        Vector3i(-180, 64, 256)
};

static constexpr int testIndex = 4;

i64 TickEntry::nextTickEntryID = 0;
bool TickEntry::operator == (const TickEntry& rhs) const
{
        return m_pos == rhs.m_pos && BlockManager::isAssociatedBlockID(blockID, rhs.blockID);
}

bool TickEntry::operator > (const TickEntry& rhs) const
{
        if (scheduledTime == rhs.scheduledTime)
        {
                if (priority == rhs.priority)
                {
                        return tickEntryID > rhs.tickEntryID;
                }
                return priority > rhs.priority;
        }

        return scheduledTime > rhs.scheduledTime;
}

bool TickEntry::operator< (const TickEntry& rhs) const
{
        if (scheduledTime == rhs.scheduledTime)
        {
                if (priority == rhs.priority)
                {
                        return tickEntryID < rhs.tickEntryID;
                }
                return priority < rhs.priority;
        }

        return scheduledTime < rhs.scheduledTime;
}

ServerWorld::ServerWorld(const String& name, WorldProvider* _provider, const WorldSettings& settings, int loadRange)
        :World(name, _provider, settings, loadRange)
{
        LordNew EntityTracker(this);
        LordNew BlockChangeRecorderServer;
        LordNew CommodityManager;
        m_subscriptionGuard.add(SignTextsChangeEvent::subscribe([](TileEntitySign* tileEntity) -> bool
        {
                Server::Instance()->getNetwork()->getSender()->broadcastSetSignTexts(tileEntity);
                return true;
        }));
        createShop();
        m_isClient = false;
        m_blockEventIndex = 0;
}

ServerWorld::~ServerWorld(void)
{
        LordDelete BlockChangeRecorderServer::Instance();
        LordDelete(EntityTracker::Instance());
        LordDelete CommodityManager::Instance();
}

ServerWorld *ServerWorld::createWorld(const String& name)
{
        WorldSettings worldSetting(test_rand[testIndex], GAME_TYPE_SURVIVAL, false, false, TERRAIN_TYPE_DEFAULT);
        GameSettings  *gamesSetting = LordNew GameSettings();
        WorldProvider *worldProvider = ServerWorldProvider::getProviderForDimension(0);


        ServerWorld *pServerWorld =     LordNew ServerWorld(name, worldProvider, worldSetting, gamesSetting->getLoadChunksRange());

        pServerWorld->createChunkService(gamesSetting->getLoadChunksRange());
        pServerWorld->m_gamesSetting = gamesSetting;
        
        IWorldEventListener* listener = LordNew WorldServerListener(pServerWorld);
        pServerWorld->addWorldListener(listener);

        //pServerWorld->generate();
        return pServerWorld;
}

ServerWorld *ServerWorld::createWorld(const String& name, int64_t worldSeed, int worldType)
{
        // Pick the seed: caller-supplied if non-zero, else the built-in default.
        // The default is intentionally NOT zero — a zero seed produces a
        // pathological world (no random scatter), so we substitute a known
        // interesting one.
        i64 seed = (worldSeed != 0) ? worldSeed : test_rand[testIndex];

        // Map our extended worldType values onto the TERRAIN_TYPE enum that
        // WorldSettings understands. TERRAIN_TYPE_CUSTOM is mapped to
        // TERRAIN_TYPE_DEFAULT here — the actual custom-generator dispatch
        // happens in ServerWorldProviderSurface::createChunkGenerator()
        // (which checks worldObj->getWorldInfo().getTerrainType()).
        // We stash the CUSTOM marker by passing it through WorldSettings as
        // the "generateOptions" string — a hack, but it survives the path
        // from here to the provider without touching WorldInfo's enum.
        TERRAIN_TYPE terrainType = TERRAIN_TYPE_DEFAULT;
        String generatorOptions;
        if (worldType == TERRAIN_TYPE_CUSTOM)
        {
                terrainType = TERRAIN_TYPE_DEFAULT;   // base type — provider checks options
                generatorOptions = "custom";           // signal to provider
        }
        else if (worldType >= 0 && worldType < TERRAIN_TYPE_COUNT)
        {
                terrainType = (TERRAIN_TYPE)worldType;
        }

        WorldSettings worldSetting(seed, GAME_TYPE_SURVIVAL, false, false, terrainType);
        if (!generatorOptions.empty())
        {
                worldSetting.func_82750_a(generatorOptions);
        }

        GameSettings  *gamesSetting = LordNew GameSettings();
        WorldProvider *worldProvider = ServerWorldProvider::getProviderForDimension(0);

        ServerWorld *pServerWorld = LordNew ServerWorld(name, worldProvider, worldSetting, gamesSetting->getLoadChunksRange());
        pServerWorld->createChunkService(gamesSetting->getLoadChunksRange());
        pServerWorld->m_gamesSetting = gamesSetting;

        IWorldEventListener* listener = LordNew WorldServerListener(pServerWorld);
        pServerWorld->addWorldListener(listener);

        return pServerWorld;
}

void ServerWorld::destroy()
{
        for (auto i : m_worldListeners) {
                WorldServerListener* listener = dynamic_cast<WorldServerListener*>(i);
                LordSafeDelete(listener);
        }
        m_worldListeners.clear();

        LordSafeDelete(m_gamesSetting);
}

ChunkService * ServerWorld::createChunkService(int loadRange)
{
        m_pChunkService = LordNew ChunkServiceServer(this);
        // Authoritative server world source: DISK-FIRST, GENERATE-ON-MISS.
        // ChunkProviderServerFile loads persisted chunks (player edits +
        // previously populated terrain) from the Anvil region files, and only
        // runs the real vanilla overworld generator (ChunkProviderGenerate —
        // biome terrain, caves, ravines, surface) for chunks never generated
        // before, persisting them immediately. One object serves as BOTH the
        // provider and the storage so a single Anvil manager owns all I/O.
        auto provider = LORD::make_shared<ChunkProviderServerFile>(
                        this, Root::Instance()->getMapPath(), this->getSeed());
        m_pChunkService->useProviderStorage(provider, provider);
        return m_pChunkService;
}

void ServerWorld::populateChunk(int x, int z)
{
        if (!m_pChunkService)
                return;

        // getChunk generates the chunk on first access (ChunkProviderGenerate).
        auto chunk = m_pChunkService->getChunk(x, z);
        if (!chunk || chunk->isNonexistent())
                return;

        // Already populated (this run or a previous session — the flag is
        // persisted in the Anvil NBT as "TerrainPopulated").
        if (chunk->m_isTerrainPopulated)
                return;

        // Vanilla semantics: decoration of (x, z) requires the east, south and
        // south-east neighbour chunks to exist, because trees and structures
        // may spill over the +x/+z borders. Generating them here also warms the
        // cache so the client's next chunk request is fast.
        auto east  = m_pChunkService->getChunk(x + 1, z);
        auto south = m_pChunkService->getChunk(x, z + 1);
        auto se    = m_pChunkService->getChunk(x + 1, z + 1);
        if (!east || !south || !se
                || east->isNonexistent() || south->isNonexistent() || se->isNonexistent())
        {
                return; // not decoratable yet — retried on a later request
        }

        // Vanilla per-chunk decoration seed:
        //   rand.setSeed(worldSeed); k = x*341873128712 + z*132897987541;
        //   rand.setSeed(k);
        i64 mix = (i64)x * 341873128712LL + (i64)z * 132897987541LL;
        Random rand;
        rand.setSeed(this->getSeed());
        rand.setSeed(this->getSeed() ^ mix);

        BiomeGenBase* biome = getBiomeGenForCoords(x * 16 + 8, z * 16 + 8);
        if (biome)
        {
                biome->decorate(this, rand, x * 16, z * 16);
        }

        chunk->m_isTerrainPopulated = true;

        // FIX [SYMPTOM-4]: removed the synchronous per-chunk disk save from
        // the chunk-request path (NBT serialize + zlib + file IO on the tick
        // thread, multiplied by every chunk served at join). The periodic
        // saveAllChunks (every 600 ticks) and the shutdown save persist the
        // decoration with at most ~30s of exposure.

        LordLogInfo("populateChunk: decorated chunk (%d, %d), biome %d", x, z, biome ? (int)biome->m_ID : -1);
}


void ServerWorld::fireTNT(Vector3i tntPos)
{
        auto blockId = this->getBlockId(tntPos);
        if (blockId == BLOCK_ID_TNT)
        {
                auto block = BlockManager::sBlocks[blockId];
                if (block)
                        block->onBlockDestroyedByPlayer(this, tntPos, 1);
        }
}

void ServerWorld::addAirDrop(float x, float y, float z)
{
        ServerNetwork::Instance()->getSender()->sendAirDropPosition(Vector3(x ,y ,z));
}

int ServerWorld::addCreature(Vector3 pos, int monsterId, float yaw, String actorName)
{
        auto creature = LordNew EntityCreatureAI(this, monsterId, pos, yaw, actorName);
        if (spawnEntityInWorld(creature))
        {
                creature->selectBtTreeByMonsterId(monsterId);
                return creature->entityId;
        }
        else
        {
                return 0;
        }
}

int ServerWorld::addCreatureWithName(Vector3 pos, int monsterId, float yaw, String actorName, String nameLang)
{
        auto creature = LordNew EntityCreatureAI(this, monsterId, pos, yaw, actorName);
        creature->setNameLang(nameLang);
        if (spawnEntityInWorld(creature))
        {
                creature->selectBtTreeByMonsterId(monsterId);
                return creature->entityId;
        }
        else
        {
                return 0;
        }
}

int ServerWorld::addCreatureWithPlayer(Vector3 pos, ui64 userId, int monsterId, float yaw, String actorName, String nameLang)
{
        auto creature = LordNew EntityCreatureAI(this, monsterId, pos, yaw, actorName);
        creature->setNameLang(nameLang);
        creature->setUserId(userId);
        //creature->setCanBeCollidedWith(false);
        if (spawnEntityInWorld(creature))
        {
                creature->selectBtTreeByMonsterId(monsterId);
                return creature->entityId;
        }
        else
        {
                return 0;
        }
}

void ServerWorld::setCreatureTeam(int entityId, int teamId)
{
        World::setCreatureTeam(entityId, teamId);
}

void ServerWorld::killCreature(int entityId)
{
        auto entity = dynamic_cast<EntityCreatureAI*>(getEntity(entityId));
        if (entity)
        {
                entity->changeCreatureAction(CREATURE_AS_DEATH);
                entity->setCurrentTargetEntityId(0, false);
        }
}

void ServerWorld::changeCreatureAttackTarget(int entityId, int targetId)
{
        auto entity = dynamic_cast<EntityCreatureAI*>(getEntity(entityId));
        if (entity) {
                entity->changeCreatureAction(CREATURE_AS_MOVE);
                entity->setCurrentTargetEntityId(targetId, true);
        }
}

bool ServerWorld::setCropsBlock(ui64 platformId, const BlockPos & pos, int blockID, int stage, int curStageTime, int stealCount, std::time_t lastUpdateTime, int residueHarvestNum)

{
        bool bRet = setBlock(pos, blockID, stage - 1, 3);

        if (bRet)
        {
                BlockCropsInfo* pInfo = new BlockCropsInfo();
                pInfo->ownerPlatformId = platformId;
                pInfo->blockId = blockID;
                pInfo->blockPos = pos;
                pInfo->curStage = stage;
                pInfo->curStageTime = curStageTime;
                pInfo->lastUpdateTime = lastUpdateTime;
                pInfo->stealCount = stealCount;
                pInfo->residueHarvestNum = residueHarvestNum;
                Server::Instance()->getBlockCropsManager()->addBlockCrops(pInfo->blockPos, pInfo);
        }

        return bRet;
}

bool ServerWorld::setFruitsBlock(const BlockPos & pos, int blockID)
{
        bool bRet = setBlock(pos, blockID, 0, 3, true);
        if (bRet)
        {
                Server::Instance()->getBlockFruitsManager()->addBlockFruits(pos, blockID);
        }
        return bRet;
}

void ServerWorld::generate()
{
        setSpawnLocation(spwanPos[testIndex]);

        constexpr int centerX = 0;
        constexpr int centerZ = 0;
        int loadWide;
        loadWide = 32;  // for speed in debug mode.  //modify by maxicheng 128->32

        i64 startTime = LORD::Time::Instance()->getMillisecondsCPU();
        int counter = 0;
        for (int i = -loadWide; i <= loadWide; i += 16)
        {
                for (int j = -loadWide; j <= loadWide; j += 16)
                {
                        i64 curTime = LORD::Time::Instance()->getMillisecondsCPU();

                        if (curTime - startTime > 1000L)
                        {
                                int allCount = int(Math::Pow(float((loadWide / 16) * 2 + 1), 2.f));
                                LordLogInfo("Preparing spawn area %d (%d/%d)", counter * 100 / allCount, counter, allCount);
                                startTime = curTime;
                        }

                        ++counter;
                        (void) m_pChunkService->getChunk((centerX + i) >> 4, (centerZ + j) >> 4);
                }
        }
        LordLogInfo("Finished to prepare spawn area. Number of loaded chunks %d", counter);
}

void ServerWorld::tick()
{
        if (!m_WorldInfo.isTimeStopped())
        {
                setWorldTime(getWorldTime() + 1); // accelerate the time...
                static uint16_t ticks = 0;
                if (++ticks % 60 == 0)
                {
                        Server::Instance()->getNetwork()->getSender()->broadcastSyncWorldTime(getWorldTime());
                        if (m_WorldInfo.isRaining())
                        {
                                LordLogInfo("raining...");
                                Server::Instance()->getNetwork()->getSender()->broadcastSyncWorldWeather("rain", m_WorldInfo.getRainTime());
                        }
                        if (m_WorldInfo.isThundering())
                        {
                                Server::Instance()->getNetwork()->getSender()->broadcastSyncWorldWeather("thunder", m_WorldInfo.getThunderTime());
                        }
                }
        }

        LORD::ui32 t0 = LORD::Root::Instance()->getCurrentTime();
        World::tick();
        LORD::ui32 t1 = LORD::Root::Instance()->getCurrentTime();
        GameCommon::logIfExpired(t0, t1, 10, "m_serverWorld::tick->World::tick");

        m_WorldInfo.incrementTotalWorldTime(m_WorldInfo.getWorldTotalTime() + 1L);
        updateEntities();
        LORD::ui32 t2 = LORD::Root::Instance()->getCurrentTime();
        GameCommon::logIfExpired(t1, t2, 30, "m_serverWorld::tick->World::updateEntities");

        EntityTracker::Instance()->updateTrackedEntities();
        LORD::ui32 t3 = LORD::Root::Instance()->getCurrentTime();
        GameCommon::logIfExpired(t2, t3, 10, "m_serverWorld::tick->EntityTracker::updateTrackedEntities");

        sendAndApplyBlockEvents();
        LORD::ui32 t4 = LORD::Root::Instance()->getCurrentTime();
        GameCommon::logIfExpired(t3, t4, 10, "m_serverWorld::tick->ServerWorld::sendAndApplyBlockEvents");

        // Periodic world persistence: flush all cached chunks to Anvil region
        // files every 600 ticks (~30 s). Player edits + populated decoration
        // survive an app kill / crash. saveAllChunks only writes chunks still
        // in the cache (loaded/generated area) — cheap for a 1-player world.
        if (++m_saveTickCounter >= 600)
        {
                m_saveTickCounter = 0;
                try
                {
                        if (m_pChunkService)
                                m_pChunkService->saveAllChunks(true);
                }
                catch (const std::exception& e)
                {
                        LordLogError("periodic saveAllChunks failed: %s", e.what());
                }
                catch (...)
                {
                        LordLogError("periodic saveAllChunks failed: unknown exception");
                }
        }
}

bool ServerWorld::tickUpdates(bool par1)
{
        int size = m_tickEntryTreeSet.size();

        if (size != m_tickEntryHashSet.size())
        {
                LordException("TickNextTick list out of synch");
        }
        else
        {
                if (size > 1000)
                        size = 1000;

                TickEntry entry;

                for (int i = 0; i < size; ++i)
                {
                        entry = *(m_tickEntryTreeSet.begin());

                        if (!par1 && entry.scheduledTime > m_WorldInfo.getWorldTotalTime())
                        {
                                break;
                        }

                        m_tickEntryTreeSet.erase(m_tickEntryTreeSet.begin());
                        m_tickEntryHashSet.erase(entry);
                        m_tickEntryList.push_back(entry);
                }

                for (auto it = m_tickEntryList.begin(); it != m_tickEntryList.end(); )
                {
                        entry = *it;
                        ++it;
                        m_tickEntryList.pop_front();

                        int bound = 0;
                        if (checkChunksExist(entry.m_pos - bound, entry.m_pos + bound))
                        {
                                int blockID = getBlockId(entry.m_pos);

                                if (blockID > 0 && BlockManager::isAssociatedBlockID(blockID, entry.blockID))
                                {
                                        try
                                        {
                                                BlockManager::sBlocks[blockID]->updateTick(this, entry.m_pos, m_Rand);
                                        }
                                        catch (Exception e)
                                        {
                                                int meta = 0;
                                                try
                                                {
                                                        meta = getBlockMeta(entry.m_pos);
                                                }
                                                catch (Exception e1)
                                                {
                                                        meta = -1;
                                                }

                                                LordLogError("Error Position (%d, %d, %d) block ID %d and meta %d ", entry.m_pos.x, entry.m_pos.y, entry.m_pos.z, blockID, meta);
                                        }
                                }
                        }
                        else
                        {
                                scheduleBlockUpdate(entry.m_pos, entry.blockID, 0);
                        }
                }

                m_tickEntryList.clear();
                return !m_tickEntryTreeSet.empty();
        }
        return false;
}

void ServerWorld::addBlockEvent(const BlockPos& pos, int blockID, int id, int param)
{
        BlockEventList& events = m_blockEventCache[m_blockEventIndex];
        BlockEventData data(pos, blockID, id, param);

        auto it = find(events.begin(), events.end(), data);
        if (it == events.end())
                events.push_back(data);
}

bool ServerWorld::isBlockTickScheduledThisTick(const BlockPos& pos, int blockID)
{
        for (auto it : m_tickEntryList)
        {
                if (it.m_pos == pos && BlockManager::isAssociatedBlockID(blockID, it.blockID))
                        return true;
        }
        return false;
}

void ServerWorld::scheduleBlockUpdate(const BlockPos& pos, int blockID, int tickrate)
{
        scheduleBlockUpdateWithPriority(pos, blockID, tickrate, 0);
}

void ServerWorld::scheduleBlockUpdateWithPriority(const BlockPos& pos, int blockID, int tickrate, int priority)
{
        TickEntry entry(pos, blockID);
        int bound = 0;

        if (m_scheduledUpdatesAreImmediate && blockID > 0)
        {
                if (BlockManager::sBlocks[blockID]->isUpdateScheduled())
                {
                        bound = 8;

                        if (checkChunksExist(entry.m_pos, entry.m_pos))
                        {
                                int blockID2 = getBlockId(entry.m_pos);

                                if (blockID2 == entry.blockID && blockID2 > 0)
                                {
                                        BlockManager::sBlocks[blockID2]->updateTick(this, entry.m_pos, m_Rand);
                                }
                        }

                        return;
                }

                tickrate = 1;
        }

        if (checkChunksExist(pos, pos))
        {
                if (blockID > 0)
                {
                        entry.setScheduledTime((i64)tickrate + m_WorldInfo.getWorldTotalTime());
                        entry.setPriority(priority);
                }

                if (m_tickEntryHashSet.find(entry) == m_tickEntryHashSet.end())
                {
                        m_tickEntryHashSet.insert(entry);
                        m_tickEntryTreeSet.insert(entry);
                }
        }
}

void ServerWorld::scheduleBlockUpdateFromLoad(const BlockPos& pos, int blockID, int tickrate, int priority)
{
        TickEntry entry(pos, blockID);
        entry.setPriority(priority);

        if (blockID > 0)
        {
                entry.setScheduledTime((long)tickrate + m_WorldInfo.getWorldTotalTime());
        }

        if (m_tickEntryHashSet.find(entry) == m_tickEntryHashSet.end())
        {
                m_tickEntryHashSet.insert(entry);
                m_tickEntryTreeSet.insert(entry);
        }
}

float ServerWorld::getFrameDeltaTime()
{
        return Server::Instance()->getServerFramTime();
}

void ServerWorld::sendAndApplyBlockEvents()
{
        while (!m_blockEventCache[m_blockEventIndex].empty())
        {
                int processIdx = m_blockEventIndex;
                m_blockEventIndex ^= 1;

                for (auto it : m_blockEventCache[processIdx])
                {
                        if (onBlockEventReceived(it))
                        {
                                //this.mcServer.getConfigurationManager().sendToAllNear((double)var3.getX(), (double)var3.getY(), (double)var3.getZ(), 64.0D, this.provider.dimensionId, new Packet54PlayNoteBlock(var3.getX(), var3.getY(), var3.getZ(), var3.getBlockID(), var3.getEventID(), var3.getEventParameter()));
                        }
                }
                m_blockEventCache[processIdx].clear();
        }
}

bool ServerWorld::onBlockEventReceived(const BlockEventData& data)
{
        int blockID = getBlockId(data.m_pos);
        return blockID == data.m_blockID ? BlockManager::sBlocks[blockID]->onBlockEventReceived(this, data.m_pos, data.m_eventID, data.m_eventParam) : false;
}

void ServerWorld::createShop()
{
        m_shop = LordNew ServerShop();
}

void ServerWorld::addBlockDoor(const BlockPos & pos, int maxTick)
{
        Server::Instance()->getBlockDoorManager()->addPos(pos, maxTick);
}

void ServerWorld::removeBlockDoor(const BlockPos & pos)
{
        Server::Instance()->getBlockDoorManager()->deletePos(pos);
}

void ServerWorld::showBuyRespawnToPlayer(ui64 rakssId, int times)
{
        RespawnGoods goods = Server::Instance()->getWorld()->getShop()->getRespawnGoodsByTime(times);
        if (goods.uniqueId == -1)
        {
                SCRIPT_EVENT::PlayerBuyRespawnResultEvent::invoke(rakssId, 0);
                return;
        }
        Server::Instance()->getWorld()->getShop()->addRespawnRecord(rakssId, goods.time);
        ServerNetwork::Instance()->getSender()->sendShowBuyRespawn(rakssId, times);
}

void ServerWorld::setPoisonCircleRange(Vector3 safeRange1, Vector3 safeRange2, Vector3 poisonRange1, Vector3 poisonRange2, float speed)
{
        ServerNetwork::Instance()->getSender()->broadCastPoisonCircleRange(getPoisonCircleRange(safeRange1, safeRange2, poisonRange1, poisonRange2), speed);
}

int ServerWorld::tryAllPlayerTakeAircraft(Vector3 startPos, Vector3 endPos, float speed)
{
        int entityId = World::tryAllPlayerTakeAircraft(startPos, endPos, speed);
        if (entityId != -1)
        {
                ServerNetwork::Instance()->getSender()->broadCastAircraftStartFly(entityId);
        }
        return entityId;
}

void ServerWorld::createOrDestroyHouseFromSchematic(String fileName, const BlockPos& start_pos, bool xImage, bool zImage, bool createOrDestroy)
{
        // parse the schematic file
        auto schematicModel = LordNewT(SchematicModel);
        schematicModel->resetData();
        String path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), "dynamic", fileName).c_str();

        if (schematicModel->deserialize(path.c_str()))
        {
                int element_count = 0;
                i16 width = schematicModel->getWidth();
                i16 height = schematicModel->getHeight();
                i16 length = schematicModel->getLength();

                // setblock with no broadcast immediately
                if (createOrDestroy)
                {
                        std::vector<NETWORK_DEFINE::S2CPacketUpdateBlockInfo> special_blocks;

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

                                                element_count++;

                                                int pos_x = (int)x;
                                                int pos_y = (int)y;
                                                int pos_z = (int)z;

                                                int pos_x_old = pos_x;
                                                int pos_y_old = pos_y;
                                                int pos_z_old = pos_z;

                                                // �Ƶ���ת 270 180 90�㷨
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

                                                if (BlockManager::isAttachmentBlock((int)block_id))
                                                {
                                                        NETWORK_DEFINE::S2CPacketUpdateBlockInfo attachment_block(BlockPos(pos_x, pos_y, pos_z), (int)block_id, (int)mate);
                                                        special_blocks.push_back(attachment_block);
                                                }
                                                else
                                                {
                                                        if (BlockManager::isStairsBlock((int)block_id))
                                                        {
                                                                mate = BlockManager::StairsBlockRotate(mate, xImage, zImage);
                                                        }
                                                        setBlock(BlockPos(pos_x, pos_y, pos_z), (int)block_id, (int)mate, 3, false);
                                                }
                                        }
                                }
                        }

                        // special blocks must be created lastly
                        for (auto block : special_blocks)
                        {
                                int flag = 3;
                                if (BlockManager::isDoorBlock((int)block.getBlockId()))
                                {
                                        flag = 2;
                                }
                                setBlock(BlockPos(block.x, block.y, block.z), (int)block.getBlockId(), (int)block.getBlockMeta(), flag, false);
                        }
                        special_blocks.clear();
                }
                // destroy block
                else
                {
                        std::vector<NETWORK_DEFINE::S2CPacketUpdateBlockInfo> normal_blocks;

                        // special blocks must be destroyed firstly
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

                                                
                                                int pos_x = (int)x;
                                                int pos_y = (int)y;
                                                int pos_z = (int)z;

                                                int pos_x_old = pos_x;
                                                int pos_y_old = pos_y;
                                                int pos_z_old = pos_z;

                                                // �Ƶ���ת 270 180 90�㷨
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

                                                if (BlockManager::isAttachmentBlock((int)block_id))
                                                {
                                                        setBlock(BlockPos(pos_x, pos_y, pos_z), 0, 0, 3, false);
                                                }
                                                else
                                                {
                                                        NETWORK_DEFINE::S2CPacketUpdateBlockInfo normal_block(BlockPos(pos_x, pos_y, pos_z), 0, 0);
                                                        normal_blocks.push_back(normal_block);
                                                }
                                        }
                                }
                        }

                        for (auto block : normal_blocks)
                        {
                                setBlock(BlockPos(block.x, block.y, block.z), (int)block.getBlockId(), (int)block.getBlockMeta(), 3, false);
                        }
                        normal_blocks.clear();
                }

                ServerNetwork::Instance()->getSender()->sendCreateHouseFromSchematic(fileName.c_str(), start_pos, xImage, zImage, createOrDestroy);

                //LordLogInfo("ServerWorld::createOrDestroyHouseFromSchematic create_or_destroy:%d path:%s start_pos x:%d y:%d z:%d", createOrDestroy ? 1 : 0, path.c_str(), start_pos.x, start_pos.y, start_pos.z);
                //LordLogInfo("ServerWorld::createOrDestroyHouseFromSchematic width:%d height:%d length:%d element_count:%d", width, height, length, element_count);
        }
        else
        {
                LordLogError("ServerWorld::createOrDestroyHouseFromSchematic Failed to deserialize schematic: %s", path.c_str());
        }
        LordSafeDelete(schematicModel);
}

void ServerWorld::fillAreaByBlockIdAndMate(const BlockPos & start_pos, const BlockPos & end_pos, int block_id, int mate)
{
        // if (block_id <= 0) return;

        int begin_x = start_pos.x < end_pos.x ? start_pos.x : end_pos.x;
        int begin_y = start_pos.y < end_pos.y ? start_pos.y : end_pos.y;
        int begin_z = start_pos.z < end_pos.z ? start_pos.z : end_pos.z;
        int end_x = start_pos.x < end_pos.x ? end_pos.x : start_pos.x;
        int end_y = start_pos.y < end_pos.y ? end_pos.y : start_pos.y;
        int end_z = start_pos.z < end_pos.z ? end_pos.z : start_pos.z;

        for (int y = begin_y; y <= end_y; ++y)
        {
                for (int x = begin_x; x <= end_x; ++x)
                {
                        for (int z = begin_z; z <= end_z; ++z)
                        {
                                setBlock(BlockPos(x, y, z), block_id, mate, 3, false);
                        }
                }
        }

        //LordLogInfo("ServerWorld::fillAreaByBlockIdAndMate start_pos x:%d y:%d z:%d end_pos x:%d y:%d z:%d block_id:%d mate:%d", start_pos.x, start_pos.y, start_pos.z, end_pos.x, end_pos.y, end_pos.z, block_id, mate);

        ServerNetwork::Instance()->getSender()->sendfillAreaByBlockIdAndMate(start_pos, end_pos, block_id, mate);
}

void ServerWorld::updateManorOwner(ui64 rakssid, std::vector<ManorOwner> owners)
{
        World::updateManorOwner(rakssid, owners);
        ServerNetwork::Instance()->getSender()->sendUpdateManorOwner(rakssid);
}

bool ServerWorld::isInChangeRecord(BlockPos blockPos, int srcId, int srcMeta)
{
        return BlockChangeRecorderServer::Instance()->isInChangeSet(blockPos, srcId, srcMeta);
}

void ServerWorld::destroyAgentPlayer()
{
        for (auto it : m_loadedEntityMap)
        {
                EntityCreatureAI* pAI = dynamic_cast<EntityCreatureAI*>(it.second);
                if (pAI)
                {
                        pAI->unloadAgent();
                }
        }
}

void ServerWorld::addSingleUpdateBlock(BlockPos pos, int block_id, int meta)
{
        EntityPlayers& players = Server::Instance()->getWorld()->getPlayers();

        for (EntityPlayers::iterator it = players.begin(); it != players.end(); ++it)
        {
                EntityPlayerMP* pPlayer = dynamic_cast<EntityPlayerMP*>(it->second);
                if (!pPlayer)
                        continue;

                pPlayer->addSingleUpdateBlock(pos, block_id, meta);
        }
}

void ServerWorld::addSkillEffect(int skillEffectId, Vector3 position)
{
        SkillEffect* effect = SkillSetting::getSkillEffect(skillEffectId);
        if (effect)
        {
                ServerNetwork::Instance()->getSender()->sendEntitySkillEffect(position, effect->name, effect->duration, effect->width, effect->height, effect->color, effect->density);
        }
}

void ServerWorld::updateActorNpcContent(int entityId, String content)
{
        EntityActorNpc* pEntity = dynamic_cast<EntityActorNpc*>(getEntity(entityId));
        if (!pEntity)
                return;
        pEntity->setContent(content);
        ServerNetwork::Instance()->getSender()->broadCastActorNpcContent(entityId, content);
}

void ServerWorld::addSimpleEffect(String effectName, Vector3 position, float yaw, int duration, int targetId, float scale)
{
        if (effectName.length() == 0)
                return;
        if (targetId == 0)
                ServerNetwork::Instance()->getSender()->broadCastAddSimpleEffect(effectName, position, yaw, duration, scale);
        else
                ServerNetwork::Instance()->getSender()->sendAddSimpleEffect(effectName, position, yaw, duration, targetId, scale);
}

void ServerWorld::setRailSpeedInfo(int entityId, float maxSpeed, float curSpeed, float addSpeed)
{
        auto blockman = dynamic_cast<EntityBlockmanEmpty*>(getEntity(entityId));
        if (blockman)
        {
                blockman->setCurrentMaxSpeed(maxSpeed);
                blockman->setCurSpeed(curSpeed);
                blockman->setSpeedAddtion(addSpeed);
        }
}

void ServerWorld::setRailCarMaxSpeed(int entityId, float maxSpeed)
{
        auto blockman = dynamic_cast<EntityBlockmanEmpty*>(getEntity(entityId));
        if (blockman)
        {
                blockman->setCurrentMaxSpeed(maxSpeed);
        }
}

void ServerWorld::setRailCarCurSpeed(int entityId, float curSpeed)
{
        auto blockman = dynamic_cast<EntityBlockmanEmpty*>(getEntity(entityId));
        if (blockman)
        {
                blockman->setCurSpeed(curSpeed);
        }
}

void ServerWorld::setRailCarAddSpeed(int entityId, float addSpeed)
{
        auto blockman = dynamic_cast<EntityBlockmanEmpty*>(getEntity(entityId));
        if (blockman)
        {
                blockman->setSpeedAddtion(addSpeed);
        }
}

void ServerWorld::setRailCarRecordRoute(int entityId, bool isRecord)
{
        auto blockman = dynamic_cast<EntityBlockmanEmpty*>(getEntity(entityId));
        if (blockman)
        {
                blockman->setNeedRecordRoute(isRecord);
        }
}

void ServerWorld::setRailCarActor(int entityId, String actorName)
{
        auto blockman = dynamic_cast<EntityBlockmanEmpty*>(getEntity(entityId));
        if (blockman)
        {
                blockman->setActorName(actorName);
        }
}

void ServerWorld::setRailCarName(int entityId, String nameLang)
{
        auto blockman = dynamic_cast<EntityBlockmanEmpty*>(getEntity(entityId));
        if (blockman)
        {
                blockman->setNameLang(nameLang);
        }
}

void ServerWorld::startRailCarRun(int entityId, float angle)
{
        auto blockman = dynamic_cast<EntityBlockmanEmpty*>(getEntity(entityId));
        if (blockman)
        {
                blockman->changeDirection(angle);
        }
}

void ServerWorld::changeRailCarDirection(int entityId)
{
        auto blockman = dynamic_cast<EntityBlockmanEmpty*>(getEntity(entityId));
        if (blockman)
        {
                blockman->reverseDirection();
        }
}

void ServerWorld::setRailCarFrozen(int entityId, float frozenTime)
{
        auto blockman = dynamic_cast<EntityBlockmanEmpty*>(getEntity(entityId));
        if (blockman)
        {
                blockman->setOnFrozen(frozenTime);
        }
        ServerNetwork::Instance()->getSender()->syncEntityFrozenTime(0, entityId, frozenTime);
}

bool ServerWorld::rangeCheckBlockByEntityId(i32 entityId, i32 blockId)
{
        auto entity = dynamic_cast<EntityBuildNpc*>(getEntity(entityId));
        if (entity)
        {
                return entity->isCanWorking(blockId);
        }
        return false;
}

void ServerWorld::playCloseup(const Vector3& position, float farDistance, float nearDistance, float velocity, float duration, float yaw, float pitch)
{
        if (farDistance >= 32.0f || farDistance <= nearDistance || velocity <= 0.0f || duration <= 0.0f)
        {
                return;
        }

        auto playerMap = ServerNetwork::Instance()->getPlayers();
        for (auto iter : playerMap)
        {
                if (iter.second->hasLogon())
                {
                        auto player = iter.second->getEntityPlayer();
                        if (player && player->position.distanceTo(position) <= 100.0f)
                        {
                                ServerNetwork::Instance()->getSender()->sendPlayCloseup(player->getRaknetID(), player->position, position, farDistance, nearDistance, velocity, duration, yaw, pitch);
                                player->setEntityInvulnerable(true);
                                iter.second->resetPos(position.x, position.y, position.z, yaw);
                        }
                }
        }
}

void ServerWorld::updateBuildNpc(i32 entityId, i32 maxQueueNum, i32 productCapacity, i32 queueUnlockPrice, i32 queueUnlockCurrencyType, std::vector<RanchCommon> products , std::vector<ProductQueue> unlockQueues, std::vector<ProductRecipe> recipes)
{
        auto entity = dynamic_cast<EntityBuildNpc*>(getEntity(entityId));
        if (entity)
        {
                entity->m_maxQueueNum = maxQueueNum;
                entity->m_productCapacity = productCapacity;

                entity->m_queueUnlockPrice = queueUnlockPrice;
                entity->m_queueUnlockCurrencyType = queueUnlockCurrencyType;

                vector<RanchCommon>::type p = vector<RanchCommon>::type();
                for (auto item : products)
                {
                        p.push_back(item);
                }

                vector<ProductQueue>::type q = vector<ProductQueue>::type();
                for (auto item : unlockQueues)
                {
                        q.push_back(item);
                }

                vector<ProductRecipe>::type r = vector<ProductRecipe>::type();
                for (auto item : recipes)
                {
                        r.push_back(item);
                }
                entity->setRecipes(r);
                entity->setProducts(p);
                entity->setUnlockQueues(q);

                Server::Instance()->getNetwork()->getSender()->syncEntityBuildNpc(0, entity);
        }
}

int ServerWorld::addLandNpc(Vector3 pos, float yaw, ui64 userId, i32 landCode, String nameLang, String actorName, String actorBody, String actorBodyId)
{
        auto landNpc = LordNew EntityLandNpc(this, pos);
        landNpc->rotationYaw = yaw;

        landNpc->setUserId(userId);
        landNpc->setLandCode(landCode);
        if (actorName.length() > 0)
        {
                landNpc->setActorName(actorName);
        }

        if (nameLang.length() > 0) {
                landNpc->setNameLang(nameLang);
        }

        if (actorBody.length() > 0)
        {
                landNpc->setActorBody(actorBody);
        }

        if (actorBodyId.length() > 0)
        {
                landNpc->setActorBodyId(actorBodyId);
        }

        if (spawnEntityInWorld(landNpc))
        {
                return landNpc->entityId;
        }
        return 0;
}

void ServerWorld::updateLandNpc(i32 entityId, i32 status, i32 price, i32 totalTime, i32 timeLeft, std::vector<RanchCommon> recipe, std::vector<RanchCommon> reward)
{
        auto landNpc = dynamic_cast<EntityLandNpc*>(getEntity(entityId));
        if (landNpc)
        {
                landNpc->setStatus(status);
                landNpc->setTimeLeft(timeLeft);

                landNpc->setPrice(price);
                landNpc->setTotalTime(totalTime);

                vector<RanchCommon>::type items = vector<RanchCommon>::type();
                for (auto item : recipe) {
                        items.push_back(item);
                }
                landNpc->setRecipe(items);

                items.clear();
                for (auto item : reward) {
                        items.push_back(item);
                }

                landNpc->setReward(items);

                Server::Instance()->getNetwork()->getSender()->syncEntityLandNpc(0, landNpc);
        }
}

void ServerWorld::setEntitySelected(ui64 rakssId, int entityId, int type)
{
        auto entity = getEntity(entityId);
        if (entity == NULL)
                return;
        ServerNetwork::Instance()->getSender()->sendEntitySelected(rakssId, entityId, type);
}

int ServerWorld::addEntityBulletin(Vector3 pos, float yaw, i32 bulletinId)
{
        auto entityBulletin = LordNew EntityBulletin(this, pos);
        entityBulletin->rotationYaw = yaw;
        entityBulletin->setBulletinId(bulletinId);
        
        if (spawnEntityInWorld(entityBulletin))
        {
                return entityBulletin->entityId;
        }
        return 0;
}

void ServerWorld::updateEntityBulletin(ui64 rakssid, i32 entityId, i32 bulletinId)
{
        auto entityBulletin = dynamic_cast<EntityBulletin*>(getEntity(entityId));
        if (entityBulletin)
        {
                entityBulletin->setBulletinId(bulletinId);
                Server::Instance()->getNetwork()->getSender()->syncEntityBulletin(rakssid, entityBulletin);
        }
}

int ServerWorld::addEntityBird(Vector3 pos, TableVisitor birdMgr, Vector3i homePos, ui64 userId, i64 birdId, String actorName, String actorBody, String actorBodyId)
{
        std::shared_ptr<TableVisitor> birgMgrPtr(new TableVisitor(birdMgr));
        auto entity = LordNew EntityBirdAI(this, birgMgrPtr, homePos, birdId, pos, 0, actorName);
        entity->m_userId = userId;

        if (actorName.length() > 0)
        {
                entity->setActorName(actorName);
        }

        if (actorBody.length() > 0)
        {
                entity->setActorBody(actorBody);
        }

        if (actorBodyId.length() > 0)
        {
                entity->setActorBodyId(actorBodyId);
        }

        if (spawnEntityInWorld(entity))
        {
                return entity->entityId;
        }
        return 0;
}

void ServerWorld::setBirdDress(i32 entityId, String dressGlasses, String dressHat, String dressBeak, String dressWing, String dressTail, String dressEffect)
{
        auto entity = dynamic_cast<EntityBirdAI*>(getEntity(entityId));
        if (entity)
        {
                entity->setDressGlasses(dressGlasses);
                entity->setDressHat(dressHat);
                entity->setDressBeak(dressBeak);
                entity->setDressWing(dressWing);
                entity->setDressTail(dressTail);
                entity->setDressEffect(dressEffect);
                ServerNetwork::Instance()->getSender()->syncEntityBird(0, entity);
        }
}

void ServerWorld::updateBirdDress(i32 entityId, String masterName, String slaveName)
{
        auto entity = dynamic_cast<EntityBirdAI*>(getEntity(entityId));
        if (entity)
        {
                entity->changeDress(masterName, slaveName);
                ServerNetwork::Instance()->getSender()->syncEntityBird(0, entity);
        }
}

void ServerWorld::updateSessionNpc(i32 entityId, ui64 rakssid, String nameLang, String actorName, String actorBody, String actorBodyId, String sessionContent, String actorAction, i32 timeLeft, bool isCanCollided)
{
        auto entity = dynamic_cast<EntitySessionNpc*>(getEntity(entityId));
        auto player = ServerNetwork::Instance()->findPlayerByRakssid(rakssid);

        if (entity && player)
        {
                ui64 userId = entity->isPerson() && player ? player->getPlatformUserId() : 0;
                entity->setTimeLeftByUser(userId, timeLeft);
                entity->setNameLangByUser(userId, nameLang);
                entity->setActorNameByUser(userId, actorName);
                entity->setActorBodyByUser(userId, actorBody);
                entity->setActorBodyIdByUser(userId, actorBodyId);
                entity->setActorActionByUser(userId, actorAction);
                entity->setSessionContentByUser(userId, sessionContent);
                entity->setCanCollidedByUser(userId, isCanCollided);
                ServerNetwork::Instance()->getSender()->syncSessionNpc(rakssid, entity);
        }
}

void ServerWorld::setEntityHealth(i32 entityId, float health, float maxHealth)
{
        auto creature = dynamic_cast<EntityLivingBase*>(getEntity(entityId));
        if (creature)
        {
                creature->setEntityMaxHealth(maxHealth);
                creature->setEntityHealth(health);
                ServerNetwork::Instance()->getSender()->syncEntityHealth(0, entityId, health, maxHealth);
        }
}

void ServerWorld::setCreatureHome(i32 entityId, Vector3i pos, int dis)
{
        auto creature = dynamic_cast<EntityCreatureAI*>(getEntity(entityId));
        if (creature)
        {
                creature->setHomePosAndDistance(pos, dis);
        }
}


}
