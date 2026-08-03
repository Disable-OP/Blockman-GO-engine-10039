#include "Server.h"
#include "Util/Event.h"
#include "Network/RoomClient.h"
#include "rapidjson/writer.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "LuaRegister/Template/LuaEngine.h"
#if LORD_PLATFORM == LORD_PLATFORM_LINUX
#include <signal.h>
#endif

RoomGameConfig getTestRGConfig(const char * curPath) {
	RoomGameConfig cfg;
	cfg.gameId = "g100211978";
	cfg.gameName = "gameName-debug";
	cfg.gameIp = "0.0.0.0";
	cfg.serverPort = 19130;
	cfg.monitorAddr = "120.92.133.131:9912";
	cfg.gameType = "";
	
	cfg.testGameDataDir = Server::getTestGameDataDir(curPath);
	cfg.testScriptDir = Server::getTestScriptDir(curPath);
	cfg.testScriptCommonDir = Server::getTestScriptCommonDir(curPath);

	cfg.maxPlayers = 8;
	cfg.mapID = ""; // default value
	cfg.userConfig = "users.json";
	cfg.propAddr = "http://dev.sandboxol.com:8943";
	cfg.rankAddr = "http://dev.sandboxol.com:8945";
	cfg.rewardAddr = "http://dev.sandboxol.com:8945";

	cfg.secret = "pq0194mxoqfh48L362G6R09T737E273X";
	cfg.isDebug = true;

	cfg.isChina = true;
	cfg.blockymodsUrl = "http://120.92.158.119";
	cfg.blockmanUrl = "http://dev.sandboxol.com:8961";
	cfg.blockymodsRewardAddr = "http://120.92.158.119";
	cfg.dbIp = "tcp://120.92.136.119:3306";
	cfg.dbUsername = "sandbox";
	cfg.dbPassword = "sandbox2018";
	cfg.dbName = "game";
	cfg.redisDbIp = "120.92.136.119";
	cfg.redisPort = 6379;
	cfg.redisDbPassword = "sandbox_redis_2018";
	cfg.dbHttpServiceUrl = "http://120.92.141.96:8910";
    cfg.heartbeatInterval = 5;
	cfg.gameRankParams = "";
	return cfg;
}

RoomGameConfig getRGConfigFromCmdline(const char* cfgJsonStr) {
	RoomGameConfig cfg;

	std::shared_ptr<rapidjson::Document> doc = LORD::make_shared<rapidjson::Document>();
	doc->Parse(cfgJsonStr);
	if (doc->HasParseError()) {
		std::abort();
	}
	cfg.gameIp = doc->HasMember("ip") ? doc->FindMember("ip")->value.GetString() : "";
	cfg.serverPort = doc->HasMember("port") ? doc->FindMember("port")->value.GetInt() : 0;
	cfg.logDir = doc->HasMember("logdir") ? doc->FindMember("logdir")->value.GetString() : "";
	cfg.scriptDir = doc->HasMember("scriptdir") ? doc->FindMember("scriptdir")->value.GetString() : "";
	cfg.testScriptDir = doc->HasMember("testscriptdir") ? doc->FindMember("testscriptdir")->value.GetString() : "";
	cfg.commonScriptDir = doc->HasMember("scriptcommondir") ? doc->FindMember("scriptcommondir")->value.GetString() : "";
	cfg.testScriptCommonDir = doc->HasMember("testscriptcommondir") ? doc->FindMember("testscriptcommondir")->value.GetString() : "";
	cfg.mapDir = doc->HasMember("mapdir") ? doc->FindMember("mapdir")->value.GetString() : "";
	cfg.gameId = doc->HasMember("id") ? doc->FindMember("id")->value.GetString() : "";
	cfg.mapID = doc->HasMember("mapid") ? doc->FindMember("mapid")->value.GetString() : "";
	cfg.gameName = doc->HasMember("name") ? doc->FindMember("name")->value.GetString() : "";
	cfg.monitorAddr = doc->HasMember("monitoraddr") ? doc->FindMember("monitoraddr")->value.GetString() : "";
	cfg.maxPlayers = doc->HasMember("maxplayer") ? doc->FindMember("maxplayer")->value.GetInt() : 0;
	cfg.teamNumber = doc->HasMember("teamnum") ? doc->FindMember("teamnum")->value.GetInt() : 0;
	cfg.gameType = doc->HasMember("gtype") ? doc->FindMember("gtype")->value.GetString() : "";
	cfg.propAddr = doc->HasMember("propAddr") ? doc->FindMember("propAddr")->value.GetString() : "";
	cfg.rankAddr = doc->HasMember("rankAddr") ? doc->FindMember("rankAddr")->value.GetString() : "";
	cfg.rewardAddr = doc->HasMember("rewardAddr") ? doc->FindMember("rewardAddr")->value.GetString() : "";
	cfg.blockymodsUrl = doc->HasMember("blockymodsUrl") ? doc->FindMember("blockymodsUrl")->value.GetString() : "";
	cfg.blockymodsRewardAddr = doc->HasMember("blockymodsRewardAddr") ? doc->FindMember("blockymodsRewardAddr")->value.GetString() : "";
	cfg.blockmanUrl = doc->HasMember("blockmanUrl") ? doc->FindMember("blockmanUrl")->value.GetString() : "";
	cfg.isDebug = doc->HasMember("isDebug") ? doc->FindMember("isDebug")->value.GetBool() : false;
	cfg.userConfig = doc->HasMember("userConf") ? doc->FindMember("userConf")->value.GetString() : "";
	cfg.secret = doc->HasMember("secret") ? doc->FindMember("secret")->value.GetString() : "";
	cfg.isChina = doc->HasMember("isChina") ? doc->FindMember("isChina")->value.GetBool() : false;
	cfg.heartbeatInterval = doc->HasMember("heartbeatInterval") ? doc->FindMember("heartbeatInterval")->value.GetInt() : 5;
	if (doc->HasMember("dbconfig")) {
		rapidjson::Value a = doc->FindMember("dbconfig")->value.GetObject();
		cfg.dbIp = a.HasMember("addr") ? a.FindMember("addr")->value.GetString() : "";
		cfg.dbUsername = a.HasMember("user") ? a.FindMember("user")->value.GetString() : "";
		cfg.dbPassword = a.HasMember("password") ? a.FindMember("password")->value.GetString() : "";
		cfg.dbName = a.HasMember("dbname") ? a.FindMember("dbname")->value.GetString() : "";
	}

	if (doc->HasMember("redisConfig")) {
		rapidjson::Value a = doc->FindMember("redisConfig")->value.GetObject();
		cfg.redisDbIp = a.HasMember("ip") ? a.FindMember("ip")->value.GetString() : "";
		cfg.redisDbPassword = a.HasMember("password") ? a.FindMember("password")->value.GetString() : "";
		cfg.redisPort = a.HasMember("port") ? a.FindMember("port")->value.GetInt() : 0;
	}

	cfg.dbHttpServiceUrl = doc->HasMember("gameDataServiceAddr") ? doc->FindMember("gameDataServiceAddr")->value.GetString() : "";
	cfg.gameRankParams = doc->HasMember("gameRankParams") ? doc->FindMember("gameRankParams")->value.GetString() : "";

    return cfg;
}

std::atomic_bool g_serverStopping{false};

#if LORD_PLATFORM == LORD_PLATFORM_LINUX
static void sighandler(int sig_no)
{
	LordLogInfo("Receive signal to stop server, sig_no is %d", sig_no);
	if (g_serverStopping) {
		LordLogInfo("Receive signal to stop server but server is stopping, do nothing, sig_no is %d", sig_no);
		return;
	}
	Server::Instance()->stop();
}
#endif

int main(int argc, char* argv[])
{
try {
		g_serverStopping = false;
#if LORD_PLATFORM == LORD_PLATFORM_LINUX
		signal(SIGUSR1, sighandler);
#endif
		auto pserver = LordNew Server();
		RoomGameConfig rgConfig;
		if (argc == 1) {
			rgConfig = getTestRGConfig(argv[0]);
		}
		else {
			rgConfig = getRGConfigFromCmdline(argv[1]);
		}
		pserver->init(rgConfig);
		pserver->start();
		pserver->waitForStopEvent();
		g_serverStopping = true;
		pserver->stopThread();
		pserver->unInit();

		LordDelete pserver;

#ifdef TRACK_EVENT_SUBSCRIPTION
		BLOCKMAN::EventManager::reportLeaks();
#endif
	}
	catch (const std::exception& e) {
		LuaDebug::enumStack(LuaEngine::getInstance()->getLuaState());
		LordLogError("main thread exception occur: %s", e.what());
		throw e;
	}
}
