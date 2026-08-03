#include "GameCmdMgr.h"
#include "Core.h"
#include "ClientPeer.h"
#include "Memory/MemAllocDef.h"
#include "Entity/Enchantment.h"
#include "Inventory/InventoryPlayer.h"
#include "ServerNetwork.h"
#include "RoomManager.h"
#include "Block/BlockManager.h"
#include "Block/Block.h"
#include "Entity/EntityArrow.h"
#include "Entity/EntityRankNpc.h"
#include "Entity/EntityAircraft.h"
#include "Entity/EntityVehicle.h"
#include "Entity/EntitySessionNpc.h"
#include "Entity/EntityMerchant.h"
#include "Blockman/Entity/EntityCreatureAI.h"
#include "Object/Root.h"
#include "Script/GameServerEvents.h"

using namespace BLOCKMAN;
using namespace LORD;

String GameCmdMgr::CMD_STARTER = "/pangu";
String GameCmdMgr::CMD_TELEPORT = "tp";
String GameCmdMgr::CMD_ADD_ITEM = "ai";
String GameCmdMgr::CMD_ENCHANT = "enchant";
String GameCmdMgr::CMD_SET_WORLD_TIME = "set_world_time";
String GameCmdMgr::CMD_SET_WATCH_MODE = "set_watch_mode";
String GameCmdMgr::CMD_SET_ALLOW_FLYING = "set_allow_flying";
String GameCmdMgr::CMD_SET_MODEL = "set_model";
String GameCmdMgr::CMD_GET_BLOCK = "get_block";
String GameCmdMgr::CMD_ADD_ENTITY = "ae";
String GameCmdMgr::CMD_SET_BLOCK = "setb";
String GameCmdMgr::CMD_START_PLANE = "sp";
String GameCmdMgr::CMD_CREATE_HOUSE = "ch";
String GameCmdMgr::CMD_FILL_BLOCK = "fb";
String GameCmdMgr::CMD_SHUTDOWN_SERVER = "shutdown";
String GameCmdMgr::CMD_CREATE_MAP = "cm";

GameCmdMgr::GameCmdMap GameCmdMgr::m_cmdMap;
bool GameCmdMgr::m_useCmd = true;

void GameCmdMgr::init() {
	m_cmdMap[CMD_TELEPORT] = {3, &GameCmdMgr::teleport};
	m_cmdMap[CMD_ADD_ITEM] = {2, &GameCmdMgr::addItem};
	m_cmdMap[CMD_ENCHANT] = { 2, &GameCmdMgr::enchant };
	m_cmdMap[CMD_SET_WORLD_TIME] = { 1, &GameCmdMgr::setWorldTime };
	m_cmdMap[CMD_SET_WATCH_MODE] = { 1, &GameCmdMgr::setWatchMode };
	m_cmdMap[CMD_SET_ALLOW_FLYING] = { 1, &GameCmdMgr::setAllowFlying };
	m_cmdMap[CMD_SET_MODEL] = { 2, &GameCmdMgr::setModel };
	m_cmdMap[CMD_GET_BLOCK] = { 3, &GameCmdMgr::getBlock };
	m_cmdMap[CMD_ADD_ENTITY] = { 1, &GameCmdMgr::addEntity };// args mutable
	m_cmdMap[CMD_SET_BLOCK] = { 4, &GameCmdMgr::setBlock };
	m_cmdMap[CMD_START_PLANE] = { 0, &GameCmdMgr::startPlane };
	m_cmdMap[CMD_CREATE_HOUSE] = { 6, &GameCmdMgr::createHouse };
	m_cmdMap[CMD_FILL_BLOCK] = { 8, &GameCmdMgr::fillBlock };
	m_cmdMap[CMD_SHUTDOWN_SERVER] = { 0, &GameCmdMgr::ShutdownServer };
	m_cmdMap[CMD_CREATE_MAP] = { 0, &GameCmdMgr::createMap };
}

bool GameCmdMgr::consumeCmd(std::shared_ptr<ClientPeer>&  clientPeer, const String& msg)
{
	if (!m_useCmd)
		return false;

	if (!StringUtil::StartWith(msg, CMD_STARTER, false)) {
		return false;
	}

	StringArray arr = StringUtil::Split(msg, " ");
	if (arr.size() < 2) {
		return false;
	}

	if (arr[0] != CMD_STARTER) {
		return false;
	}

	LordLogWarning("\"/pangu\" command is used by player %s: %s", clientPeer->getName().c_str(), msg.c_str())

	String cmdName = arr[1];
	if (m_cmdMap.find(cmdName) == m_cmdMap.end()) {
		return false;
	}
	
	if (arr.size() < size_t(m_cmdMap[cmdName].argsNum + 2)) {
		return false;
	}

	auto it = arr.begin();
	++it;
	++it;
	StringArray argsArr(it, arr.end());
	return m_cmdMap[cmdName].func(clientPeer, argsArr);
}

bool BLOCKMAN::GameCmdMgr::setWorldTime(std::shared_ptr<ClientPeer>& clientPeer, StringArray& argsArr)
{
	if (argsArr[0] == "day") {
		Server::Instance()->getWorld()->setWorldTime(6000);
	}
	else if (argsArr[0] == "night") {
		Server::Instance()->getWorld()->setWorldTime(18000);
	}
	else {
		int time = StringUtil::ParseInt(argsArr[0], 0);
		Server::Instance()->getWorld()->setWorldTime(time);
	}

	return true;
}

bool BLOCKMAN::GameCmdMgr::setModel(std::shared_ptr<ClientPeer>& clientPeer, StringArray& argsArr)
{
	auto sender = Server::Instance()->getNetwork()->getSender();
	UserAttrInfo attrInfo;
	attrInfo.userId = clientPeer->getPlatformUserId();
	attrInfo.classes = 0;
	attrInfo.team = 0;
	attrInfo.regionId = 0;
	attrInfo.pioneer = false;
	attrInfo.sex = 1;
	attrInfo.vip = 0;
	attrInfo.targetUserId = 0;
	attrInfo.manorId = 0;
	attrInfo.defaultIdle = 0;

	attrInfo.faceId = 0;
	attrInfo.hairId = 0;
	attrInfo.topsId = 0;
	attrInfo.pantsId = 0;
	attrInfo.shoesId = 0;
	attrInfo.glassesId = 0;
	attrInfo.decoratehatId = 0;
	attrInfo.extrawingId = 0;
	attrInfo.armId = 0;
	attrInfo.bagId = 0;

	if (Server::Instance()->getEnableRoom())
	{
		Server::Instance()->getRoomManager()->getUserAttrInfo(clientPeer->getPlatformUserId(), attrInfo);
	}

	if (argsArr[0] == "hair") {
		attrInfo.hairId = StringUtil::ParseInt(argsArr[1], 0);
	}
	else if (argsArr[0] == "face") {
		attrInfo.faceId = StringUtil::ParseInt(argsArr[1], 0);
	}
	else if (argsArr[0] == "tops") {
		attrInfo.topsId = StringUtil::ParseInt(argsArr[1], 0);
	}
	else if (argsArr[0] == "pants") {
		attrInfo.pantsId = StringUtil::ParseInt(argsArr[1], 0);
	}
	else if (argsArr[0] == "shoes") {
		attrInfo.shoesId = StringUtil::ParseInt(argsArr[1], 0);
	}
	else if (argsArr[0] == "glasses") {
		attrInfo.glassesId = StringUtil::ParseInt(argsArr[1], 0);
	}
	else {
		return false;
	}

	if (Server::Instance()->getEnableRoom())
	{
		Server::Instance()->getRoomManager()->onUserAttr(attrInfo);
	}
	sender->sendUserAttrToTrackingPlayers(clientPeer->getEntityPlayer()->entityId, attrInfo);

	return true;
}

bool BLOCKMAN::GameCmdMgr::setWatchMode(std::shared_ptr<ClientPeer>& clientPeer, LORD::StringArray & argsArr)
{
	bool isWatchMode = argsArr[0] == "on";
	if (clientPeer->getEntityPlayer()->capabilities.isWatchMode != isWatchMode)
	{
		clientPeer->getEntityPlayer()->setWatchMode(isWatchMode);
	}
	return true;
}

bool BLOCKMAN::GameCmdMgr::setAllowFlying(std::shared_ptr<ClientPeer>& clientPeer, LORD::StringArray & argsArr)
{
	bool allowFlying = argsArr[0] == "on";
	if (clientPeer->getEntityPlayer()->capabilities.allowFlying != allowFlying)
	{
		clientPeer->getEntityPlayer()->setAllowFlying(allowFlying);
	}
	return true;
}

bool BLOCKMAN::GameCmdMgr::teleport(std::shared_ptr<ClientPeer>& clientPeer, StringArray& argsArr)
{
	float x = StringUtil::ParseFloat(argsArr[0], 0.0);
	float y = StringUtil::ParseFloat(argsArr[1], 0.0);
	float z = StringUtil::ParseFloat(argsArr[2], 0.0);

	clientPeer->getEntityPlayer()->setPositionAndUpdate(x, y, z);

	return true;
}

bool BLOCKMAN::GameCmdMgr::getBlock(std::shared_ptr<ClientPeer>& clientPeer, StringArray& argsArr)
{
	int x = StringUtil::ParseInt(argsArr[0], 0);
	int y = StringUtil::ParseInt(argsArr[1], 0);
	int z = StringUtil::ParseInt(argsArr[2], 0);

	BlockPos pos(x, y, z);
	String result = "server block info: ";
	int blockID = Server::Instance()->getWorld()->getBlockId(pos);
	String name;
	if (blockID == 0) {
		name = "air";
	}
	else {
		name = BLOCKMAN::BlockManager::sBlocks[blockID]->getUnlocalizedName();
	}

	result += StringUtil::Format("(%d,%d,%d)id %d, name %s ", pos.x, pos.y, pos.z, blockID, name.c_str());

	blockID = Server::Instance()->getWorld()->getBlockId(pos.getPosY());
	if (blockID == 0) {
		name = "air";
	}
	else {
		name = BlockManager::sBlocks[blockID]->getUnlocalizedName();
	}
	result += StringUtil::Format("---UpperBlock id %d, name %s", blockID, name.c_str());

	ServerNetwork::Instance()->getSender()->sendDebugString(clientPeer->getRakssid(), result);
	return true;
}

bool BLOCKMAN::GameCmdMgr::addItem(std::shared_ptr<ClientPeer>& clientPeer, StringArray& argsArr)
{
	int itemId = StringUtil::ParseInt(argsArr[0], 0);
	int itemNum = StringUtil::ParseInt(argsArr[1], 0);
	int damage = 0;
	
	if (argsArr.size() > 2)
	{
		damage = StringUtil::ParseInt(argsArr[2], 0);
	}
	
	clientPeer->getEntityPlayer()->addItem(itemId, itemNum, damage, 0);

	return true;
}

bool BLOCKMAN::GameCmdMgr::enchant(std::shared_ptr<ClientPeer>& clientPeer, LORD::StringArray & argsArr)
{
	int id = StringUtil::ParseInt(argsArr[0], 0);
	int level = StringUtil::ParseInt(argsArr[1], 0);
	if (id >= 0 && id < Enchantment::MAX_ENCH_COUNT)
	{
		if (auto currentItem = clientPeer->getEntityPlayer()->inventory->getCurrentItem())
		{
			currentItem->addEnchantment(Enchantment::enchantmentsList[id], level);
		}
	}

	return true;
}

/* /pangu add_entity 1 x y z etc.*/
bool BLOCKMAN::GameCmdMgr::addEntity(std::shared_ptr<ClientPeer>& clientPeer, StringArray& argsArr)
{
	EntityPlayerMP* pPlayer = clientPeer->getEntityPlayer();
	int entityType = StringUtil::ParseInt(argsArr[0], 0);
	float x = pPlayer->position.x;
	float y = pPlayer->position.y;
	float z = pPlayer->position.z;
	Vector3 pos(x, y, z);
	String name;

	Entity* pEntity = NULL;
	switch (entityType)
	{
	case 1: //arrow /pangu ae 1
		pEntity = LordNew EntityArrow(pPlayer->world, pPlayer, pos,
			pPlayer->rotationYaw, pPlayer->rotationPitch, 0);
		break;
	case 2: //merchant /pangu ae 2 team
		if (argsArr.size() != 2)
			return false;

		name = argsArr[1];
		pEntity = LordNew EntityMerchant(pPlayer->world, pos);
		pEntity->rotationYaw = pPlayer->rotationYaw;
		((EntityMerchant*)pEntity)->setName(name);
		break;
	case 3: //rankNpc /pangu ae 3 team
		if (argsArr.size() != 2)
			return false;

		name = argsArr[1];
		pEntity = LordNew EntityRankNpc(pPlayer->world, pos);
		pEntity->rotationYaw = pPlayer->rotationYaw;
		((EntityRankNpc*)pEntity)->setName(name);
		break;
	case 4: //aircraft /pangu ae 4
		pEntity = LordNew EntityAircraft(pPlayer->world, 1, pos, 0);
		pEntity->rotationYaw = pPlayer->rotationYaw;
		break;
	case 5: //vehicle /pangu ae 5
		pEntity = LordNew EntityVehicle(pPlayer->world, 1, pos, 0);
		pEntity->rotationYaw = pPlayer->rotationYaw;
		break;
	case 6: //rankNpc /pangu ae 6 team
		if (argsArr.size() != 2)
			return false;

		name = argsArr[1];
		pEntity = LordNew EntitySessionNpc(pPlayer->world, pos);
		pEntity->rotationYaw = pPlayer->rotationYaw;
		((EntitySessionNpc*)pEntity)->setSessionType(1);
		((EntitySessionNpc*)pEntity)->setSessionContent("");
		break;
	case 7: //creature /pangu ae 7
		pEntity = LordNew EntityCreatureAI(pPlayer->world, 201002, pos, pPlayer->rotationYaw,"");
		((EntityCreatureAI*)pEntity)->selectBtTreeByMonsterId(201002);
		//((EntityCreatureAI*)pEntity)->setCurrentAI("BaseMonsterAI");
		break;
	default:
		break;
	}

	if (!pEntity)
		return false;

	pPlayer->world->spawnEntityInWorld(pEntity);
	return true;
}

// /pangu sb x y z 251
bool BLOCKMAN::GameCmdMgr::setBlock(std::shared_ptr<ClientPeer>& clientPeer, StringArray& argsArr)
{
	int x = StringUtil::ParseInt(argsArr[0], 0);
	int y = StringUtil::ParseInt(argsArr[1], 0);
	int z = StringUtil::ParseInt(argsArr[2], 0);
	int id = StringUtil::ParseInt(argsArr[3], 0);

	clientPeer->getEntityPlayer()->world->setBlock({ x, y, z }, id);

	return true;
}

// /pangu sp
bool BLOCKMAN::GameCmdMgr::startPlane(std::shared_ptr<ClientPeer>& clientPeer, StringArray& argsArr)
{
	auto pAircraft = clientPeer->getEntityPlayer()->world->getAircraft();
	if (pAircraft)
	{
		pAircraft->startPlaneTest();
		clientPeer->getEntityPlayer()->takeOnAircraft(pAircraft);
		Server::Instance()->getNetwork()->getSender()->broadCastAircraftStartFly(pAircraft->entityId);
	}
	return true;
}

bool BLOCKMAN::GameCmdMgr::createHouse(std::shared_ptr<ClientPeer>& clientPeer, StringArray& argsArr)
{
	int x = StringUtil::ParseInt(argsArr[0], 0);
	int y = StringUtil::ParseInt(argsArr[1], 0);
	int z = StringUtil::ParseInt(argsArr[2], 0);
	int xImage = StringUtil::ParseInt(argsArr[3], 0);
	int zImage = StringUtil::ParseInt(argsArr[4], 0);

	int createOrDestroy = StringUtil::ParseInt(argsArr[5], 0);

	auto rootPath = Root::Instance()->getRootPath();
	// String path = (PathUtil::ConcatPath(rootPath, "test.schematic")).c_str();
	String path = "test.schematic";
	clientPeer->getEntityPlayer()->world->createOrDestroyHouseFromSchematic(path, BlockPos(x, y, z), xImage == 1 ? true : false, zImage == 1 ? true : false, createOrDestroy == 1 ? true : false);


	return true;
}

bool BLOCKMAN::GameCmdMgr::fillBlock(std::shared_ptr<ClientPeer>& clientPeer, LORD::StringArray & argsArr)
{
	int start_pos_x = StringUtil::ParseInt(argsArr[0], 0);
	int start_pos_y = StringUtil::ParseInt(argsArr[1], 0);
	int start_pos_z = StringUtil::ParseInt(argsArr[2], 0);
	int end_pos_x = StringUtil::ParseInt(argsArr[3], 0);
	int end_pos_y = StringUtil::ParseInt(argsArr[4], 0);
	int end_pos_z = StringUtil::ParseInt(argsArr[5], 0);
	int block_id = StringUtil::ParseInt(argsArr[6], 0);
	int mate = StringUtil::ParseInt(argsArr[7], 0);

	clientPeer->getEntityPlayer()->world->fillAreaByBlockIdAndMate(BlockPos(start_pos_x, start_pos_y, start_pos_z), BlockPos(end_pos_x, end_pos_y, end_pos_z), block_id, mate);

	return true;
}

bool BLOCKMAN::GameCmdMgr::ShutdownServer(std::shared_ptr<ClientPeer>& clientPeer, LORD::StringArray & argsArr)
{
	Server::Instance()->stop();
	return true;
}

bool BLOCKMAN::GameCmdMgr::createMap(std::shared_ptr<ClientPeer>& clientPeer, StringArray& argsArr)
{
	SCRIPT_EVENT::CreateExploreMapEvent::invoke(clientPeer->getRakssid());
	return true;
}
