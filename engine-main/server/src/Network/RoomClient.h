#ifndef ROOM_CLIENT_H
#define ROOM_CLIENT_H
#include <string>
#include "Core.h"
#include "Memory/MemAllocDef.h"
#include "Network/AsyncClient/AsyncClient.h"

struct UserAttrInfo;
struct ManorAttrInfo;
struct BroadcastMessage;

enum class RoomGameStatus {
	RUNNING = 2,
	PREPARING = 7,
	STARTED  = 8,
	STARTED_CAN_ENTER = 10,
};

class RoomGameConfig {
public:
	std::string gameId;
	std::string gameName;
	std::string gameIp;
	int serverPort;
	std::string monitorAddr;
	std::string gameType;
	std::string logDir;
	std::string scriptDir;
	std::string commonScriptDir;
	std::string mapDir;
	int maxPlayers;
	int teamNumber;
	std::string mapID;
	std::string userConfig;
	std::string propAddr;
	std::string rankAddr;

	std::string rewardAddr;
	std::string secret;
	bool isDebug;
	bool isChina;

	std::string testGameDataDir;
	std::string testScriptDir;
	std::string testScriptCommonDir;

	std::string blockymodsUrl;
	std::string blockmanUrl;
	std::string blockymodsRewardAddr;
	std::string dbIp;
	std::string dbUsername;
	std::string dbPassword;
	std::string dbName;
	std::string redisDbIp;
	std::string redisDbPassword;
	int redisPort;
	std::string dbHttpServiceUrl;

    int heartbeatInterval;
	std::string gameRankParams;

	// ---------------------------------------------------------------------
	// World generation (added for in-process LOCAL_MODE server).
	//
	// worldSeed: 64-bit seed passed from Java (ServerService.nativeServerStart
	//            → RoomGameConfig → Server::init → ServerWorld::createWorld →
	//            WorldSettings → ChunkProviderCustom/Generate). When 0, the
	//            server falls back to its built-in default seed.
	//
	// worldType: TERRAIN_TYPE_* value (see BM_TypeDef.h). TERRAIN_TYPE_CUSTOM
	//            (=100) requests the sky-islands generator (ChunkProviderCustom).
	//            Other values fall through to the vanilla providers.
	// ---------------------------------------------------------------------
	int64_t     worldSeed = 0;
	int         worldType = 0;  // default = TERRAIN_TYPE_DEFAULT
public:
	void debugPrint() const;
};

class RoomMsgHandler : public LORD::ObjectAlloc {
public:
	virtual void onConnected() = 0;
	virtual void onConnectReply() = 0;
	virtual void onUserAttr(const UserAttrInfo& attr) = 0;
	virtual void onUserManorRelease(int64_t userId) = 0;
	virtual void onReceive(const BroadcastMessage& message) = 0;
};

class AsyncClientCallbackImpl : public AsyncClientCallback, public LORD::ObjectAlloc {
public:
	AsyncClientCallbackImpl(RoomMsgHandler* handler);
	virtual void onConnected(AsyncClient* client);

private:
	RoomMsgHandler* m_handler;
};

class RoomClient : public LORD::ObjectAlloc {
public:
	typedef LORD::map<int64_t, UserAttrInfo>::type UserAttrInfoMap;
	typedef LORD::map<int64_t, ManorAttrInfo>::type ManorAttrInfoMap;

public:
	RoomClient(const std::string& host, unsigned short port, RoomMsgHandler* handler);
	void sendConnect(const std::string& gameId, int startResult, const std::string& startInfo, bool isReconnect, const RoomGameConfig& config);
	void sendDisconnect(const std::string& gameId);
	void sendGameStatus(const std::string& gameId, int maxPlayerNum, int currentPlayerNum, int status);
	void resendGameStatus(const std::string& gameId, int maxPlayerNum);
	void sendUserIn(int64_t userId, int teamId);
	void sendUserOut(int64_t userId, int teamId);
	void sendUserSellManor(int64_t userId);
	void sendHeartBeat(int userNumber);
	void sendSyncUsers(const UserAttrInfoMap& userAttrInfoMap, const ManorAttrInfoMap& manorAttrInfoMap);
	void tick();

private:
	void _parseRecv();

	void _parseConnectReply(const char* data);
	void _parseUserAttr(const char* data);
	void _parseUserManorRelease(const char* data);
	void _parseBroadcastMessage(const char* data);

	void _sendMsg(int type, int length, const char* data);

private:
	RoomMsgHandler* m_handler;
	std::shared_ptr<AsyncClient> m_asyncClient;
	vcs::Buffer m_recvBuffer;
	int m_lastSentGameStatus = 7; // RoomGameStatus::PREPARING
	int m_lastSentCurrentPlayerNum = 0;
};



#endif // ROOM_CLIENT_H
