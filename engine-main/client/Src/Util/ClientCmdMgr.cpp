#include "ClientCmdMgr.h"

#include "Core.h"
#include "Memory/MemAllocDef.h"
#include "game.h"
#include "Log/Log.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "World/GameSettings.h"
#include "Block/BlockManager.h"
#include "Block/Block.h"
#include "Chunk/ChunkService.h"

using namespace BLOCKMAN;
using namespace LORD;

String ClientCmdMgr::CMD_STARTER = "/c";
String ClientCmdMgr::CMD_TOGGLE_DEBUG = "toggle_debug";
String ClientCmdMgr::CMD_SET_SMOOTH = "set_smooth";
String ClientCmdMgr::CMD_GET_BLOCK = "get_block";
String ClientCmdMgr::CMD_SAVE = "save";
String ClientCmdMgr::CMD_CRASH = "crash";

ClientCmdMgr::ClientCmdMap ClientCmdMgr::m_cmdMap;

void ClientCmdMgr::init() {
	m_cmdMap[CMD_TOGGLE_DEBUG] = { 1, &ClientCmdMgr::toggleDebug };
	m_cmdMap[CMD_SET_SMOOTH] = { 1, &ClientCmdMgr::setSmooth };
	m_cmdMap[CMD_GET_BLOCK] = { 3, &ClientCmdMgr::getBlock };
	m_cmdMap[CMD_SAVE] = { 0, &ClientCmdMgr::save };
	m_cmdMap[CMD_CRASH] = { 0, &ClientCmdMgr::crash };
}

bool ClientCmdMgr::consumeCmd(const String& msg)
{
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

	String cmdName = arr[1];
	if (m_cmdMap.find(cmdName) == m_cmdMap.end()) {
		return false;
	}

	if (arr.size() != m_cmdMap[cmdName].argsNum + 2) {
		return false;
	}

	auto it = arr.begin();
	++it;
	++it;
	StringArray argsArr(it, arr.end());
	return m_cmdMap[cmdName].func(argsArr);
}

bool ClientCmdMgr::toggleDebug(LORD::StringArray& argsArr)
{
	bool on = false;
	if (argsArr[0] == "on") {
		on = true;
	}
	GameClient::CGame::Instance()->toggleDebugMessageShown(on);

	return true;
}

bool ClientCmdMgr::setSmooth(LORD::StringArray & argsArr)
{
	GameSettings::setMouseHistoryDuration((std::max)(0, StringUtil::ParseI32(argsArr[0], 100)));
	return true;
}

bool ClientCmdMgr::getBlock(LORD::StringArray & argsArr)
{
	int x = StringUtil::ParseInt(argsArr[0], 0);
	int y = StringUtil::ParseInt(argsArr[1], 0);
	int z = StringUtil::ParseInt(argsArr[2], 0);

	BlockPos pos(x, y, z);
	String result = "client block info: ";

	int blockID = Blockman::Instance()->getWorld()->getBlockId(pos);
	String name;
	if (blockID == 0) {
		name = "air";
	}
	else {
		name = BlockManager::sBlocks[blockID]->getUnlocalizedName();
	}

	result += StringUtil::Format(" (%d,%d,%d)id %d, name %s", pos.x, pos.y, pos.z, blockID, name.c_str());

	blockID = Blockman::Instance()->getWorld()->getBlockId(pos.getPosY());
	if (blockID == 0) {
		name = "air";
	}
	else {
		name = BlockManager::sBlocks[blockID]->getUnlocalizedName();
	}
	result += StringUtil::Format("---UpperBlock id %d, name %s", blockID, name.c_str());


	Blockman::Instance()->setClientDebugString(result);
	return true;
}

bool ClientCmdMgr::save(LORD::StringArray & argsArr)
{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
	Blockman::Instance()->getWorld()->getChunkService()->saveAllChunks(true);
	LordLogInfo("++++++++++++++++++++++++++ save success ++++++++++++++++++++++++++++");
	return true;
#else
	return false;
#endif
}

bool ClientCmdMgr::crash(LORD::StringArray & argsArr)
{
	char* p = nullptr;
	p[0] = 'c';

	return true;
}