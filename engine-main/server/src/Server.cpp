#include "Server.h"

#include "World/World.h"
#include "World/GameSettings.h"
#include "Chunk/Chunk.h"
#include "World/Section.h"
#include "World/WorldSettings.h"
#include "World/WorldProvider.h"

#include "Block/BlockManager.h"
#include "WorldGenerator/BiomeGen.h"
#include "WorldGenerator/GenLayer.h"
#include "WorldGenerator/StructurePieces.h"
#include "WorldGenerator/WorldGenerator.h"
#include "Util/Concurrent/Promise.h"
#include "Util/Facing.h"
#include "TileEntity/TileEntity.h"
#include "Entity/Attributes.h"
#include "Entity/DamageSource.h"
#include "Entity/Enchantment.h"
#include "Entity/EntityLivingBase.h"
#include "Item/Item.h"
#include "Item/Items.h"
#include "Item/Potion.h"
#include "Item/PotionManager.h"
#include "Stats/StatList.h"
#include "Inventory/InventoryPlayer.h"
#include "Item/ItemStack.h"
#include "Block/Block.h"
#include "Block/BlockManager.h"
#include "Inventory/Container.h"
#include "Inventory/CraftingManager.h"
#include "TileEntity/TileEntityInventory.h"
#include "Object/Root.h"

#include "Network/ServerNetwork.h"
#include "Network/GameCmdMgr.h"
#include <iostream>

#include "Global.h"
#include "Util/CommonEvents.h"
#include "Blockman/Entity/EntityPlayerMP.h"
#include "Item/ItemCrafter.h"
#include "Util/SecTimer.h"
#include "Script/GameServerEvents.h"
#include "RoomManager.h"
#include "Script/ScriptManager.h"
#include "Script/Setting/ScriptSetting.h"
#include "Util/UThread.h"
#include "Setting/MultiLanTipSetting.h"
#include "Misc/IdMapping.h"
#include "Block/BlockLoader.h"
#include "GameVersionDefine.h"
#include "DB/ServerDB.h"
#include "DB/ServerRedisDB.h"
#include "Setting/DoorSetting.h"
#include "Setting/CarSetting.h"
#include "Setting/LogicSetting.h"
#include "Setting/GunSetting.h"
#include "Setting/BulletClipSetting.h"
#include "Setting/ActorSizeSetting.h"
#include "Setting/GameRuleSetting.h"
#include "Setting/MonsterSetting.h"
#include "Setting/CropsSetting.h"
#include "Setting/GunExtraAttrSetting.h"
#include "Setting/GrenadeSetting.h"
#include "Inventory/CoinManager.h"
#include "Blockman/Trigger/TriggerModuleServer.h"
#include "Behaviac/behaviac_generated/types/behaviac_types.h"
#include "DB/MysqlHttpRequest.h"
#include "DB/RedisHttpRequest.h"
#include "Common.h"
#include "Setting/RanchSetting.h"
#include "Setting/HouseSetting.h"
#include "Setting/BuildingSetting.h"
#include "Setting/BulletinSetting.h"
#include "Setting/FruitsSetting.h"
#include "Util/NewRandom.h"

#if LORD_PLATFORM == LORD_PLATFORM_LINUX
#include <unistd.h>
#include <string.h>
#endif

#define PROJECT_ROOT_PATH "blockmango-client"

#define DISABLE_ROOM 0

#define INIT_POS_X 4
#define INIT_POS_Y 60
#define INIT_POS_Z -17

#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif

#if LORD_PLATFORM == LORD_PLATFORM_LINUX
std::string Server::getCurrDir() {
	char buf[1024] = { 0 };
	if (!getcwd(buf, 1024)) {
		LordLogError("getcwd fail, error: %s", strerror(errno));
		std::abort();
	}
	std::string currDir = buf;

	return currDir;
}
#endif

std::string Server::getTestScriptDir(const char *curPath)
{
	//PROJECT_ROOT_PATH/MineGame/HungryGame/src
#if LORD_PLATFORM == LORD_PLATFORM_LINUX
	std::string scriptDir = getCurrDir();
#else
	std::string scriptDir = curPath;
#endif
	size_t pos = scriptDir.rfind(PROJECT_ROOT_PATH);
	scriptDir = scriptDir.substr(0, pos + std::string(PROJECT_ROOT_PATH).length());
	String scriptPath = scriptDir.c_str();
	bool isSuccess = ScriptSetting::loadScriptDir(scriptPath);
	if (isSuccess)
	{
		scriptDir = scriptPath.c_str();
		scriptDir += PATH_SEPARATOR;
		scriptDir += "src";
		LordLogInfo("TestScriptDir:%s", scriptDir.c_str());
		return scriptDir;
	}
	return "";
}

std::string Server::getTestScriptCommonDir(const char *curPath)
{
	//PROJECT_ROOT_PATH/MineGame/HungryGame/src
#if LORD_PLATFORM == LORD_PLATFORM_LINUX
	std::string scriptCommonDir = getCurrDir();
#else
	std::string scriptCommonDir = curPath;
#endif

	size_t pos = scriptCommonDir.rfind(PROJECT_ROOT_PATH);
	scriptCommonDir = scriptCommonDir.substr(0, pos + std::string(PROJECT_ROOT_PATH).length());


	scriptCommonDir += PATH_SEPARATOR;
	scriptCommonDir += "MineGame";
	scriptCommonDir += PATH_SEPARATOR;
	scriptCommonDir += "Common";

	LordLogInfo("TestScriptCommonDir:%s", scriptCommonDir.c_str());

	return scriptCommonDir;
}

std::string Server::getTestGameDataDir(const char *curPath)
{
	//PROJECT_ROOT_PATH/res/client
#if LORD_PLATFORM == LORD_PLATFORM_LINUX
	std::string gameDataDir = getCurrDir();
#else
	std::string gameDataDir = curPath;
#endif

	size_t pos = gameDataDir.rfind(PROJECT_ROOT_PATH);
	gameDataDir = gameDataDir.substr(0, pos + std::string(PROJECT_ROOT_PATH).length());


	gameDataDir += PATH_SEPARATOR;
	gameDataDir += "res";
	gameDataDir += PATH_SEPARATOR;
	gameDataDir += "client";

	LordLogInfo("mapDir:%s", gameDataDir.c_str());

	return gameDataDir;
}


Server::Server()
{
#if LORD_PLATFORM == LORD_PLATFORM_LINUX
    signal(SIGPIPE, SIG_IGN);

    sigset_t signal_mask;
    sigemptyset (&signal_mask);
    sigaddset (&signal_mask, SIGPIPE);
    int rc = pthread_sigmask (SIG_BLOCK, &signal_mask, NULL);
    if (rc != 0) {
        printf("block sigpipe error\n");
    }
	m_enableRoom =true;
#else
	m_enableRoom = (DISABLE_ROOM == 0);
#endif
}

Server::~Server()
{
}

void Server::initRecipes()
{
	auto manager = CraftingManager::Instance();
	manager->loadRecipe("crafting_table");

	// planks
	manager->loadRecipe("acacia_planks");
	manager->loadRecipe("birch_planks");
	manager->loadRecipe("dark_oak_planks");
	manager->loadRecipe("jungle_planks");
	manager->loadRecipe("oak_planks");
	manager->loadRecipe("spruce_planks");

	manager->loadRecipe("stick");

	// armors
	manager->loadRecipe("leather_helmet");
	manager->loadRecipe("leather_chestplate");
	manager->loadRecipe("leather_leggings");
	manager->loadRecipe("leather_boots");
	manager->loadRecipe("iron_helmet");
	manager->loadRecipe("iron_chestplate");
	manager->loadRecipe("iron_leggings");
	manager->loadRecipe("iron_boots");
	manager->loadRecipe("golden_helmet");
	manager->loadRecipe("golden_chestplate");
	manager->loadRecipe("golden_leggings");
	manager->loadRecipe("golden_boots");
	manager->loadRecipe("diamond_helmet");
	manager->loadRecipe("diamond_chestplate");
	manager->loadRecipe("diamond_leggings");
	manager->loadRecipe("diamond_boots");

	// tools and weapon
	manager->loadRecipe("wooden_sword");
	manager->loadRecipe("wooden_shovel");
	manager->loadRecipe("wooden_pickaxe");
	manager->loadRecipe("wooden_axe");
	manager->loadRecipe("wooden_hoe");
	manager->loadRecipe("stone_sword");
	manager->loadRecipe("stone_shovel");
	manager->loadRecipe("stone_pickaxe");
	manager->loadRecipe("stone_axe");
	manager->loadRecipe("stone_hoe");
	manager->loadRecipe("iron_sword");
	manager->loadRecipe("iron_shovel");
	manager->loadRecipe("iron_pickaxe");
	manager->loadRecipe("iron_axe");
	manager->loadRecipe("iron_hoe");
	manager->loadRecipe("golden_sword");
	manager->loadRecipe("golden_shovel");
	manager->loadRecipe("golden_pickaxe");
	manager->loadRecipe("golden_axe");
	manager->loadRecipe("golden_hoe");
	manager->loadRecipe("diamond_sword");
	manager->loadRecipe("diamond_shovel");
	manager->loadRecipe("diamond_pickaxe");
	manager->loadRecipe("diamond_axe");
	manager->loadRecipe("diamond_hoe");
}

void Server::initializeWorldModule()
{
	UThread::setThisThreadName("EntryThread");
	UThread::setThisThreadPriority(ThreadPriority::HIGH);
	LordLogInfo("EntryThread started, id: %d", UThread::getThisThreadNativeId());

	BiomeGenBase::initialize();
	StructureMineshaftPieces::initialize();
	StructureNetherBridgePieces::initialize();
	WorldGenDungeons::initialize();
	BlockManager::initialize();
	TileEntity::initialize();

	SharedMonsterAttributes::initialize();
	DamageSource::initialize();
	Enchantment::initialize();
	EnchantmentHelper::initialize();
	EntityLivingBase::initialize();

	ToolMaterial::initialize();
	ArmorMaterial::initialize();
	Potion::initialize();
	PotionManager::initialize();
	Item::initialize();

	CraftingManager* pCrafting = LordNew CraftingManager();
	initRecipes();
	LordNew ItemCrafter;
	PotionHelper::initialize();
	StatList::initialize();
	TileEntityBeacon::initialize();
	Facing::initialize();
	GameCmdMgr::init();
}

void Server::unInitializeWorlModule()
{
	Facing::unInitialize();
	StatList::unInitialize();
	ItemCrafter* pItemCrafter = ItemCrafter::Instance();
	LordSafeDelete(pItemCrafter);
	CraftingManager* pCrafting = CraftingManager::Instance();
	LordSafeDelete(pCrafting);

	Item::unInitialize();
	PotionManager::unInitialize();
	Potion::unInitialize();
	ArmorMaterial::unInitialize();
	ToolMaterial::unInitialize();

	EntityLivingBase::unInitialize();
	EnchantmentHelper::unInitalize();
	Enchantment::unInitialize();
	DamageSource::unInitialize();
	SharedMonsterAttributes::unInitialize();

	BlockManager::unInitialize();
	WorldGenDungeons::uninitialize();
	StructureNetherBridgePieces::uninitialize();
	StructureMineshaftPieces::uninitialize();
	BiomeGenBase::uninitialize();
}

void Server::init(const RoomGameConfig& rgConfig)
{
	tickPerHeartBeat = rgConfig.heartbeatInterval;
	m_config = rgConfig;
	m_initPos.x = INIT_POS_X;
	m_initPos.y = INIT_POS_Y;
	m_initPos.z = INIT_POS_Z;

	LordNew(Root);
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
	String logDir = String(m_config.logDir.c_str());
	if (!PathUtil::IsEndWithSeperator(logDir)) {
		logDir += "\\";
	}
	Root::Instance()->setWriteablePath(logDir.c_str());
	//Root::Instance()->setWriteablePath(""); // use current working dir
#else
	String logDir = String(m_config.logDir.c_str());
	if (!PathUtil::IsEndWithSeperator(logDir)) {
		logDir += "/";
	}
	Root::Instance()->setWriteablePath(logDir.c_str());
#endif

	Root::RootCfg cfg;
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
	cfg.rootPath = String(".\\");
#else 
	cfg.rootPath = String("./");
#endif
	cfg.resCfgFile = cfg.rootPath + "resource.cfg";
	cfg.bEditorMode = false;
	cfg.bFixedAspect = false;
	cfg.fAspectRadio = 9.0f / 16.0f;

#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
	String mapDir = String(m_config.testGameDataDir.c_str());
	mapDir += "./map/";
	mapDir += m_config.gameName.c_str();
	mapDir += "/";
#else
	String mapDir = String(m_config.mapDir.c_str());
	if (!PathUtil::IsEndWithSeperator(mapDir)) {
		mapDir += "/";
	}
	mapDir += m_config.gameName.c_str();
	mapDir += "/";
#endif

	Root::Instance()->initialize(cfg);
	Root::Instance()->setMapPath(mapDir);
	this->initSetting();
	this->initializeWorldModule();
	LordLogInfo("using map dir:%s", mapDir.c_str());
	LordLogInfo("current engine version: %d", EngineVersionSetting::getEngineVersion());
	m_config.debugPrint();

	IdMapping::readConfig(PathUtil::ConcatPath(mapDir, "id_mappings.ini"));
	(LordNew TriggerModuleServer)->initialize();
	LordNew BlockLoader();
	BlockLoader::Instance()->loadCustomBlocksInMap(mapDir);
	
	LordNew NewRandom();
	LordNew CoinManager();
	LordNew LogicSetting();

#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
	String scriptDir = String(m_config.testScriptDir.c_str());
#else // LORD_PLATFORM != LORD_PLATFORM_WINDOWS
	String scriptDir = String(m_config.scriptDir.c_str());
#endif // LORD_PLATFORM

#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
	if (scriptDir.length() > 0)
	{
		ScriptManager::Instance()->loadScript(m_config.testScriptCommonDir.c_str(), false);
		ScriptManager::Instance()->loadScript(scriptDir.c_str(), true);
		m_scriptLoaded = true;
		LordLogInfo("script loadded on windows!!!!!!");
		LordLogInfo("using script dir:%s", scriptDir.c_str());
	}
	else
	{
		LordLogInfo("script disable on windows!!!!!!");
	}
#else // LORD_PLATFORM != LORD_PLATFORM_WINDOWS
	if (m_config.commonScriptDir.length() > 0) {
		ScriptManager::Instance()->loadScript(m_config.commonScriptDir.c_str(), false);
	}

	if (scriptDir.length() > 0 && !m_config.isDebug)
	{
		ScriptManager::Instance()->loadScript(scriptDir.c_str(), true);
		m_scriptLoaded = true;
		LordLogInfo("script loadded on linux!!!!!!");
		LordLogInfo("using script dir:%s", scriptDir.c_str());
	}
	else
	{
		LordLogInfo("script disable on linux!!!!!!");
	}
#endif // LORD_PLATFORM

	m_tickThread = LORD::make_shared<UThread>("ServerTickThread", &Server::tick, this);
	m_tickThread->setPriority(ThreadPriority::HIGH);

	String tmpStr = String(m_config.gameName.c_str());
	m_serverWorld = ServerWorld::createWorld(tmpStr);

	tmpStr = String(m_config.monitorAddr.c_str());
	StringArray arr = StringUtil::Split(tmpStr, ":");

	if (m_enableRoom)
	{
		m_roomManager = LORD::make_shared<RoomManager>(std::string(arr[0].c_str()), StringUtil::ParseInt(arr[1]), m_config);
		m_roomManager->tick();
	}

	m_serverNetwork = LORD::make_shared<ServerNetwork>(m_config.gameIp.c_str(), m_config.serverPort);
	//m_serverDB = LORD::make_shared<ServerDB>(m_config.dbIp, m_config.dbUsername, m_config.dbPassword, m_config.dbName);
	//m_serverRedisDB = LORD::make_shared<ServerRedisDB>(m_config.redisDbIp, m_config.redisDbPassword, m_config.redisPort);
	m_pBlockDoorManager = LORD::make_shared<BlockDoorManager>(m_serverWorld);
	m_pBlockCropsManager = LORD::make_shared<BlockCropsManager>(m_serverWorld);
	m_pBlockFruitsManager = LORD::make_shared<BlockFruitsManager>(m_serverWorld);
	m_mysqlHttpRequest = LORD::make_shared<MysqlHttpRequest>(m_config.dbHttpServiceUrl);
	m_redisHttpRequest = LORD::make_shared<RedisHttpRequest>(m_config.dbHttpServiceUrl);

	std::string configPath = mapDir.c_str();
	configPath += "/config.yml";
	SCRIPT_EVENT::GameInitEvent::invoke(configPath.c_str(), "v1", m_serverWorld, m_config, &m_initPos);

#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
	initTestAttr();
#endif
}

void Server::initTestAttr()
{
	UserAttrInfo attr1 = UserAttrInfo();
	attr1.userId = 112;
	attr1.classes = 0;
	attr1.team = 1;
	attr1.regionId = 1001;
	attr1.vip = 1;
	attr1.pioneer = true;
	attr1.targetUserId = 112;
	attr1.manorId = 1;
	attr1.defaultIdle = 0;

	UserAttrInfo attr2 = UserAttrInfo();
	attr2.userId = 144;
	attr2.classes = 1;
	attr2.team = 2;
	attr2.regionId = 1001;
	attr2.vip = 2;
	attr2.pioneer = true;
	attr2.targetUserId = 144;
	attr2.manorId = 3;
	attr2.defaultIdle = 0;

	UserAttrInfo attr3 = UserAttrInfo();
	attr3.userId = 160;
	attr3.classes = 2;
	attr3.team = 1;
	attr3.regionId = 1001;
	attr3.vip = 3;
	attr3.pioneer = true;
	attr3.targetUserId = 160;
	attr3.manorId = 2;
	attr3.defaultIdle = 0;

	UserAttrInfo attr4 = UserAttrInfo();
	attr4.userId = 176;
	attr4.classes = 3;
	attr4.team = 2;
	attr4.regionId = 1001;
	attr4.vip = 4;
	attr4.pioneer = true;
	attr4.targetUserId = 176;
	attr4.manorId = 4;
	attr4.defaultIdle = 0;

	if (m_enableRoom)
	{
		m_roomManager->onUserAttr(attr1);
		m_roomManager->onUserAttr(attr2);
		m_roomManager->onUserAttr(attr3);
		m_roomManager->onUserAttr(attr4);
	}
}

void Server::unInit()
{
	uninitSetting();
	LordDelete(BlockLoader::Instance());
	LordDelete CoinManager::Instance();
	LordDelete LogicSetting::Instance();
	m_serverWorld->destroy();
	LordSafeDelete(m_serverWorld);
	this->unInitializeWorlModule();
	TriggerModule::Instance()->uninitialize();
	LordDelete(TriggerModule::Instance());
	Root* root = Root::Instance();
	root->destroy();
	LordSafeDelete(root);
}

void Server::stopThread()
{
	m_tickThread->stopSync();
	//m_serverDB->stop();
	//m_serverRedisDB->stop();
}

void Server::start()
{
	this->m_tickThread->start();
	LordLogInfo("ServerTickThread started, id: %d", m_tickThread->getNativeId());
	m_serverNetwork->host(1000);
}

void Server::waitForStopEvent()
{
	if (m_enableRoom)
	{
		RoomGameConfig* rg = m_roomManager->getRoomGameConfig();
		m_roomManager->getRoomClient()->sendGameStatus(
			rg->gameId,
			rg->maxPlayers, 0,
			static_cast<int>(RoomGameStatus::PREPARING)
		);
		LordLogInfo("send game status preparing to room server");
	}

	m_stopEvent.wait();
}

void Server::tick(StopFlag shouldStop)
{
	int tick = 0;
	initBehaviac();
	try {
		// todo  complate tick 
		LORD::ui32 expectedFrameTime = 50; // ms
		LORD::ui32 startTime = 0;
		LORD::ui32 endTime = 0;
		while (!shouldStop)
		{
			tick++;
			Root::Instance()->serverTick();
			startTime = LORD::Root::Instance()->getCurrentTime();
			behaviac::Workspace::GetInstance()->SetDoubleValueSinceStartup(startTime);
			behaviac::Workspace::GetInstance()->SetFrameSinceStartup(tick);
			PROMISE::runCallbacks();
			LORD::ui32 t0 = LORD::Root::Instance()->getCurrentTime();
			GameCommon::logIfExpired(startTime, t0, 10, "PROMISE::runCallbacks");

			if(m_enableRoom)
				m_roomManager->tick();
			LORD::ui32 t1 = LORD::Root::Instance()->getCurrentTime();
			GameCommon::logIfExpired(t0, t1, 10, "m_roomManager::tick");

			m_serverNetwork->logicTick();
			LORD::ui32 t2 = LORD::Root::Instance()->getCurrentTime();
			GameCommon::logIfExpired(t1, t2, 30, "m_serverNetwork::logicTick");

			m_serverWorld->tick();
			LORD::ui32 t3 = LORD::Root::Instance()->getCurrentTime();
			GameCommon::logIfExpired(t2, t3, 60, "m_serverWorld::tick");

			m_pBlockDoorManager->tick();
			LORD::ui32 t4 = LORD::Root::Instance()->getCurrentTime();
			GameCommon::logIfExpired(t3, t4, 10, "m_pBlockDoorManager::tick");

			m_pBlockCropsManager->updateCrops();
			LORD::ui32 t5 = LORD::Root::Instance()->getCurrentTime();
			GameCommon::logIfExpired(t4, t5, 10, "m_pBlockCropsManager::updateCrops");

			//m_serverDB->checkResultTick();
			//m_serverRedisDB->checkResultTick();		
			m_mysqlHttpRequest->updateWriteRequest();
			LORD::ui32 t6 = LORD::Root::Instance()->getCurrentTime();
			GameCommon::logIfExpired(t5, t6, 10, "m_mysqlHttpRequest::updateWriteRequest");

			m_redisHttpRequest->UpdateWriteRequest();
			LORD::ui32 t7 = LORD::Root::Instance()->getCurrentTime();
			GameCommon::logIfExpired(t6, t7, 10, "m_redisHttpRequest::updateWriteRequest");

			sendHeartBeat();
			SecTimer::update();
			LORD::ui32 t8 = LORD::Root::Instance()->getCurrentTime();
			GameCommon::logIfExpired(t7, t8, 10, "SecTimer::update");

			m_pBlockFruitsManager->updateFruits();
			LORD::ui32 t9 = LORD::Root::Instance()->getCurrentTime();
			GameCommon::logIfExpired(t8, t9, 10, "m_pBlockFruitsManager::updateFruits");

			m_asyncCall.realCall();
			endTime = LORD::Root::Instance()->getCurrentTime();
			GameCommon::logIfExpired(t9, endTime, 20, "m_asyncCall.realCall");
			
			m_serverFrameTime = endTime - startTime;
			if (m_serverFrameTime > expectedFrameTime) {
				if (m_serverFrameTime >= 60)
				{
					LordLogWarning("===runtime=== Server::tick slow, frame time %u ms", m_serverFrameTime);
				}
			} else if (m_serverFrameTime < expectedFrameTime) {
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
				Sleep(expectedFrameTime - m_serverFrameTime);
#else
				usleep((expectedFrameTime - m_serverFrameTime) * 1000);
#endif
				m_serverFrameTime = expectedFrameTime;
			}
			endTime = LORD::Root::Instance()->getCurrentTime();
			m_serverFrameTime = endTime - startTime;
		}

		uninitBehaviac();
		uninitMainLogicThread();

		m_serverNetwork->printDataPacket();
		this->m_stopEvent.setAll(); 
	}
	catch (const std::exception& e) {
		LordLogError("tick thread exception occur: %s", e.what());
		throw;
	}
}

void Server::sendHeartBeat()
{
	ui32 nowTime = LORD::Root::Instance()->getCurrentTime();
	if (nowTime - m_lastSendHeartBeatTime > tickPerHeartBeat * 1000)
	{
		if (!m_isRaknetAlive)
		{
			LordLogError("m_isRaknetAlive == false, stop sending HeartBeat to room service!!!!");
			return;
		}
		m_lastSendHeartBeatTime = nowTime;
		m_isRaknetAlive = false;
		int count = m_serverNetwork->getPlayerNumber();
		if (m_enableRoom)
		{
			m_roomManager->getRoomClient()->sendHeartBeat(count);
		}
		//LordLogInfo("sendHeartBeat to room, tick %d, player count %d, game_id: %s", tick, count, m_config.gameId.c_str());
	}
}

void Server::initBehaviac()
{
	static bool hasInit = false;
	if (hasInit)
		return;
	behaviac::Config::SetLogging(true);
	behaviac::Config::SetSocketing(true);
	behaviac::Config::SetSocketPort(60636);
	behaviac::Config::SetSocketBlocking(false);
	std::string path = (PathUtil::ConcatPath(Root::Instance()->getRootPath(), "bt/exported")).c_str();
	behaviac::Workspace::GetInstance()->SetFilePath(path.c_str());
	behaviac::Workspace::GetInstance()->SetFileFormat(behaviac::Workspace::EFF_xml);
	LordLogInfo("BEHAVIAC_BUILD_CONFIG_STR is %s", BEHAVIAC_BUILD_CONFIG_STR);


	hasInit = true;
}

void Server::uninitBehaviac()
{
	m_serverWorld->destroyAgentPlayer();
	behaviac::Workspace::GetInstance()->Cleanup();
}

void Server::uninitMainLogicThread()
{
	if (m_redisHttpRequest)
	{
		m_redisHttpRequest->uninitDB();
	}

	if (m_mysqlHttpRequest)
	{
		m_mysqlHttpRequest->uninitDB();
	}
}

void Server::stop()
{
	if (m_enableRoom)
	{
		m_roomManager->getRoomClient()->sendDisconnect(m_config.gameId);
		LordLogInfo("send disconnect to room, game_id: %s", m_config.gameId.c_str());
	}

	setStopEvent();
}

void Server::initDB(std::string gameType)
{
	//m_serverDB->initDB();
	m_mysqlHttpRequest->initDB(gameType);
}

void Server::startRedis()
{
	//m_serverRedisDB->initDB();
	m_redisHttpRequest->initDB();
}

void Server::initSetting()
{
	MultiLanTipSetting::loadSetting(false);
	MultiLanTipSetting::loadMapSetting(false);
	GunSetting::loadSetting(false);
	BulletClipSetting::loadSetting(false);
	EngineVersionSetting::loadSetting();
	GameCmdMgr::setCmdUsable(EngineVersionSetting::canUseCmd());
	g3::log_levels::setHighest(EngineVersionSetting::getLogLevel());
	DoorSetting::loadSetting(false);
	CarSetting::loadSetting(false);
	CropsSetting::loadSetting(false);
	ActorSizeSetting::loadSetting(false);
	ActorSizeSetting::loadMapSetting(false);
	GameRuleSetting::loadSetting(false);
	GrenadeSetting::loadSetting(false);
	RanchSetting::loadSetting(false);
	HouseSetting::loadSetting(false);
	BuildingSetting::loadSetting(false);
	BulletinSetting::loadSetting(false);
	FruitsSetting::loadSetting(false);
}

void Server::uninitSetting()
{
	MultiLanTipSetting::unloadSetting();
	GunSetting::unloadSetting();
	BulletClipSetting::unloadSetting();
	DoorSetting::unloadSetting();
	CarSetting::unloadSetting();
	MonsterSetting::unloadSetting();
	CropsSetting::unloadSetting();
	SkillSetting::unloadSetting();
	SkillItemSetting::unloadSetting();

	ActorSizeSetting::unloadSetting();
	GameRuleSetting::unloadSetting();
	
	GunExtraAttrSetting::unloadSetting();
	GrenadeSetting::unloadSetting();
	
	RanchSetting::unloadSetting();
	HouseSetting::unloadSetting();
	BuildingSetting::unloadSetting();
	BulletinSetting::unloadSetting();
	FruitsSetting::unloadSetting();

}

void Server::setGameType(String& gameType)
{
	m_gameType = gameType;
	CarSetting::m_sbIsCarFree = GameTypeSetting::IsCarFree(m_gameType);
	LogicSetting::Instance()->setGameType(GameTypeSetting::GetGameType(gameType));
}

ClientGameType Server::getClientGameType()
{
	return LogicSetting::Instance()->getGameType();
}

void Server::setServerInfo(std::string serverInfo)
{
	m_serverInfo = serverInfo;
}

 String Server::getDataServerUrl()
{
	return m_config.blockymodsRewardAddr.c_str();
}
