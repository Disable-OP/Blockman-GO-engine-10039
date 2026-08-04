#include "RoomClient.h"
#include "Memory/LordMemory.h"
#include "rapidjson/writer.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "RoomManager.h"
#include "GameVersionDefine.h"

#if LORD_PLATFORM != LORD_PLATFORM_WINDOWS
#include <sys/types.h>
#include <unistd.h>
#endif

using namespace LORD;

#define G2R_CONNECT 1
#define G2R_DISCONNECT 3
#define G2R_HEARTBEAT 4
#define G2R_GAME_STATUS 150
#define G2R_USER_IN 151
#define G2R_USER_OUT 152
#define G2R_SYNC_USERS 155
#define G2R_USER_MANOR_SOLD 156

#define R2G_CONNECT_REPLY 2
#define R2G_USER_ATTR 154
#define R2G_USER_MANOR_RELEASE 157
#define R2G_BROADCAST_MESSAGE 158

void RoomGameConfig::debugPrint() const {
	LordLogInfo("======RoomGameConfig begin=======");
	
	LordLogInfo("gameId=%s, gameName=%s, gameIp=%s, serverPort=%d, monitorAddr=%s",
		gameId.c_str(), gameName.c_str(), gameIp.c_str(), serverPort, monitorAddr.c_str());

	LordLogInfo("gameType=%s, logDir=%s, scriptDir=%s, mapDir=%s, maxPlayers=%d, mapID=%s",
		gameType.c_str(), logDir.c_str(), scriptDir.c_str(), mapDir.c_str(), maxPlayers, mapID.c_str());
	
	LordLogInfo("userConfig=%s, propAddr=%s, rankAddr=%s",
		userConfig.c_str(), propAddr.c_str(), rankAddr.c_str());

	LordLogInfo("rewardAddr=%s, secret=%s, propAddr=%s, isDebug=%d, isChina=%d",
		rewardAddr.c_str(), secret.c_str(), propAddr.c_str(), (int)isDebug, (int)isChina);

	LordLogInfo("testScriptDir=%s", testScriptDir.c_str());

	LordLogInfo("testScriptCommonDir=%s", testScriptCommonDir.c_str());
	LordLogInfo("======RoomGameConfig end=======");

}

AsyncClientCallbackImpl::AsyncClientCallbackImpl(RoomMsgHandler* handler) {
	m_handler = handler;
}

void AsyncClientCallbackImpl::onConnected(AsyncClient* client) {
	LordLogInfo("room client connected, host=%s, port=%d",client->getHost().c_str(), client->getPort());
	m_handler->onConnected();
}

RoomClient::RoomClient(const std::string& host, unsigned short port, RoomMsgHandler* handler) {
	LordLogInfo("RoomClient construct, begin to init AsyncClient with  host=%s, port=%d", host.c_str(), port);
	m_asyncClient = LORD::make_shared<AsyncClient>(host, port, LordNew AsyncClientCallbackImpl(handler));
	m_handler = handler;
}

void RoomClient::sendConnect(const std::string& gameId, int startResult, const std::string& startInfo, bool isReconnect, const RoomGameConfig& config) {
	std::shared_ptr<rapidjson::Document> doc = LORD::make_shared<rapidjson::Document>();
	doc->SetObject();
	rapidjson::Document::AllocatorType& allocator = doc->GetAllocator();

	doc->AddMember("id", rapidjson::StringRef(gameId.c_str()), allocator);
	doc->AddMember("rt", startResult, allocator);
	doc->AddMember("info", rapidjson::StringRef(startInfo.c_str()), allocator);
	doc->AddMember("rc", isReconnect, allocator);

	rapidjson::Value cfg(rapidjson::kObjectType);
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
	int pid = GetCurrentProcessId();
#else
	int pid = getpid();
#endif
	cfg.AddMember("pid", pid, allocator);

	cfg.AddMember("id", rapidjson::StringRef(gameId.c_str()), allocator);
	cfg.AddMember("name", rapidjson::StringRef(config.gameName.c_str()), allocator);
	cfg.AddMember("ip", rapidjson::StringRef(config.gameIp.c_str()), allocator);
	cfg.AddMember("port", config.serverPort, allocator);
	cfg.AddMember("monitoraddr", rapidjson::StringRef(config.monitorAddr.c_str()), allocator);
	cfg.AddMember("gtype", rapidjson::StringRef(config.gameType.c_str()), allocator);
	cfg.AddMember("logdir", rapidjson::StringRef(config.logDir.c_str()), allocator);
	cfg.AddMember("scriptdir", rapidjson::StringRef(config.scriptDir.c_str()), allocator);
	cfg.AddMember("mapdir", rapidjson::StringRef(config.mapDir.c_str()), allocator);
	cfg.AddMember("teamnum", config.teamNumber, allocator);
	cfg.AddMember("maxplayer", config.maxPlayers, allocator);
	cfg.AddMember("mapid", rapidjson::StringRef(config.mapID.c_str()), allocator);
	cfg.AddMember("secret", rapidjson::StringRef(config.secret.c_str()), allocator);
	cfg.AddMember("engineVersion", BLOCKMAN::EngineVersionSetting::getEngineVersion(), allocator);
	cfg.AddMember("gameRankParams", rapidjson::StringRef(config.gameRankParams.c_str()), allocator);

	doc->AddMember("config", cfg, allocator);

	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	doc->Accept(writer);
	const char* jsonStr = buffer.GetString();

	_sendMsg(G2R_CONNECT, buffer.GetSize(), jsonStr);

	LordLogInfo("connect packet sent to room server, pid=%d", pid);
}

void RoomClient::sendDisconnect(const std::string& gameId){
	std::shared_ptr<rapidjson::Document> doc = LORD::make_shared<rapidjson::Document>();
	doc->SetObject();
	rapidjson::Document::AllocatorType& allocator = doc->GetAllocator();
	doc->AddMember("id", rapidjson::StringRef(gameId.c_str()), allocator);

	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	doc->Accept(writer);
	const char* jsonStr = buffer.GetString();

	_sendMsg(G2R_DISCONNECT, buffer.GetSize(), jsonStr);
}

void RoomClient::sendGameStatus(const std::string& gameId, int maxPlayerNum, int currentPlayerNum, int status){
	std::shared_ptr<rapidjson::Document> doc = LORD::make_shared<rapidjson::Document>();
	doc->SetObject();
	rapidjson::Document::AllocatorType& allocator = doc->GetAllocator();
	doc->AddMember("id", rapidjson::StringRef(gameId.c_str()), allocator);

	doc->AddMember("max", maxPlayerNum, allocator);
	doc->AddMember("cur", currentPlayerNum, allocator);
	doc->AddMember("stat", status, allocator);

	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	doc->Accept(writer);
	const char* jsonStr = buffer.GetString();

	_sendMsg(G2R_GAME_STATUS, buffer.GetSize(), jsonStr);

	m_lastSentGameStatus = status;
	m_lastSentCurrentPlayerNum = currentPlayerNum;

	LordLogInfo("game_status packet sent to room server status = %d", status);
}

void RoomClient::resendGameStatus(const std::string& gameId, int maxPlayerNum) {
	sendGameStatus(gameId, maxPlayerNum, m_lastSentCurrentPlayerNum, m_lastSentGameStatus);
}

void RoomClient::sendSyncUsers(const UserAttrInfoMap& userAttrInfoMap, const ManorAttrInfoMap& manorAttrInfoMap) {
	std::shared_ptr<rapidjson::Document> doc = LORD::make_shared<rapidjson::Document>();
	doc->SetObject();
	rapidjson::Document::AllocatorType& allocator = doc->GetAllocator();

	rapidjson::Value userArr(rapidjson::kArrayType);
	for (const auto& i_a : userAttrInfoMap) {

		rapidjson::Value user(rapidjson::kObjectType);

		rapidjson::Value i(rapidjson::kNumberType);
		i.SetInt64(i_a.second.userId);
		user.AddMember("id", i, allocator);

		user.AddMember("clz", i_a.second.classes, allocator);
		user.AddMember("team", i_a.second.team, allocator);
		user.AddMember("rid", i_a.second.regionId, allocator);
		user.AddMember("pioneer", i_a.second.pioneer, allocator);
		user.AddMember("sex", i_a.second.sex, allocator);
		user.AddMember("vip", i_a.second.vip, allocator);
		user.AddMember("tid", i_a.second.targetUserId, allocator);
		user.AddMember("loc", i_a.second.manorId, allocator);

		rapidjson::Value skin(rapidjson::kObjectType);
		skin.AddMember("custom_face", i_a.second.faceId, allocator);
		skin.AddMember("custom_hair", i_a.second.hairId, allocator);
		skin.AddMember("clothes_tops", i_a.second.topsId, allocator);
		skin.AddMember("clothes_pants", i_a.second.pantsId, allocator);
		skin.AddMember("custom_shoes", i_a.second.shoesId, allocator);
		skin.AddMember("custom_glasses", i_a.second.glassesId, allocator);
		skin.AddMember("custom_scarf", i_a.second.scarfId, allocator);
		skin.AddMember("custom_wing", i_a.second.wingId, allocator);
		skin.AddMember("custom_hat", i_a.second.hatId, allocator);
		skin.AddMember("custom_decorate_hat", i_a.second.decoratehatId, allocator);
		skin.AddMember("custom_hand", i_a.second.armId, allocator);
		skin.AddMember("custom_tail", i_a.second.tailId, allocator);
		skin.AddMember("custom_wing_flag", i_a.second.extrawingId, allocator);
		skin.AddMember("custom_foot_halo", i_a.second.footHaloId, allocator);
		skin.AddMember("custom_back_effect", i_a.second.backEffectId, allocator);
		skin.AddMember("custom_head_effect", i_a.second.headEffectId, allocator);
		skin.AddMember("custom_bag", i_a.second.bagId, allocator);

		std::string emptyColorStr;
		skin.AddMember("skin_color", rapidjson::StringRef(emptyColorStr.c_str()), allocator);

		user.AddMember("skin", skin, allocator);
		
		user.AddMember("animation_idle", i_a.second.defaultIdle, allocator);
		
		userArr.PushBack(user, allocator);
	}

	rapidjson::Value manorArr(rapidjson::kArrayType);
	for (const auto& item : manorAttrInfoMap)
	{
		rapidjson::Value manor(rapidjson::kObjectType);
		rapidjson::Value tid(rapidjson::kNumberType);
		tid.SetInt64(item.second.targetUserId);
		manor.AddMember("tid", tid, allocator);
		manor.AddMember("loc", item.second.location, allocator);
		manorArr.PushBack(manor, allocator);
	}

	doc->AddMember("users", userArr, allocator);
	doc->AddMember("manors", manorArr, allocator);

	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	doc->Accept(writer);
	const char* jsonStr = buffer.GetString();

	_sendMsg(G2R_SYNC_USERS, buffer.GetSize(), jsonStr);

	LordLogInfo("sync_users packet sent to room server");
}

void RoomClient::sendUserIn(int64_t userId, int teamId) {
	std::shared_ptr<rapidjson::Document> doc = LORD::make_shared<rapidjson::Document>();
	doc->SetObject();
	rapidjson::Document::AllocatorType& allocator = doc->GetAllocator();
	rapidjson::Value u(rapidjson::kNumberType);
	u.SetInt64(userId);
	doc->AddMember("id", u, allocator);
	doc->AddMember("team", teamId, allocator);
	
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	doc->Accept(writer);
	const char* jsonStr = buffer.GetString();

	_sendMsg(G2R_USER_IN, buffer.GetSize(), jsonStr);
}

void RoomClient::sendUserOut(int64_t userId, int teamId) {
	std::shared_ptr<rapidjson::Document> doc = LORD::make_shared<rapidjson::Document>();
	doc->SetObject();
	rapidjson::Document::AllocatorType& allocator = doc->GetAllocator();
	rapidjson::Value u(rapidjson::kNumberType);
	u.SetInt64(userId);
	doc->AddMember("id", u, allocator);
	doc->AddMember("team", teamId, allocator);

	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	doc->Accept(writer);
	const char* jsonStr = buffer.GetString();

	_sendMsg(G2R_USER_OUT, buffer.GetSize(), jsonStr);
}

void RoomClient::sendUserSellManor(int64_t userId)
{
	std::shared_ptr<rapidjson::Document> doc = LORD::make_shared<rapidjson::Document>();
	doc->SetObject();
	rapidjson::Document::AllocatorType& allocator = doc->GetAllocator();
	rapidjson::Value u(rapidjson::kNumberType);
	u.SetInt64(userId);
	doc->AddMember("userId", u, allocator);

	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	doc->Accept(writer);
	const char* jsonStr = buffer.GetString();
	LordLogInfo("RoomClient::sendUserSellManor jsonStr %s", jsonStr);
	_sendMsg(G2R_USER_MANOR_SOLD, buffer.GetSize(), jsonStr);
}

void RoomClient::sendHeartBeat(int userNumber)
{
	std::shared_ptr<rapidjson::Document> doc = LORD::make_shared<rapidjson::Document>();
	doc->SetObject();
	rapidjson::Document::AllocatorType& allocator = doc->GetAllocator();
	rapidjson::Value u(rapidjson::kNumberType);
	u.SetInt64(userNumber);
	doc->AddMember("un", u, allocator);

	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	doc->Accept(writer);
	const char* jsonStr = buffer.GetString();

	_sendMsg(G2R_HEARTBEAT, buffer.GetSize(), jsonStr);
}

void RoomClient::_sendMsg(int type, int dataLength, const char* data) {
	vcs::Buffer buf;
	size_t packetLength = dataLength + 4;
	buf.writeInt(packetLength);
	buf.writeInt(type);
	buf.write(data, dataLength);

	char* encodedMsg = (char*)LordMalloc(packetLength + 4);
	buf.read(encodedMsg, packetLength + 4);
	m_asyncClient->write(encodedMsg, packetLength + 4);

	LordSafeFree(encodedMsg);
}

void RoomClient::_parseRecv() {
	m_asyncClient->read(&m_recvBuffer);

	if (m_recvBuffer.size() < 8) {
		return;
	}

	int32_t msgLength = m_recvBuffer.peekInt();
	if (size_t(msgLength + 4) > m_recvBuffer.size()) {
		return;
	}

	msgLength = m_recvBuffer.readInt();
	int32_t msgType = m_recvBuffer.readInt();

	char* msgData = nullptr;
	if (msgLength - 4 > 0) {
		msgData = (char*)LordMalloc(msgLength - 4 + 1);
        msgData[msgLength - 4] = '\0';
		m_recvBuffer.read(msgData, msgLength - 4);
	}

    LordLogInfo("recv from room, len:%d, type:%d, data:%s", msgLength, msgType, msgData);
	switch (msgType) {
	case R2G_USER_ATTR:
		_parseUserAttr(msgData);
		break;

	case R2G_CONNECT_REPLY:
		_parseConnectReply(msgData);
		break;
	case R2G_USER_MANOR_RELEASE:
		_parseUserManorRelease(msgData);
		break;
	case R2G_BROADCAST_MESSAGE:
		_parseBroadcastMessage(msgData);
		break;
	}

	LordSafeFree(msgData);
}

void RoomClient::_parseConnectReply(const char* data) {
	// connect reply has no payload

	m_handler->onConnectReply();
}

void RoomClient::_parseUserAttr(const char* data) {
	if (!data) {
		LordLogError("_parseUserAttr fail, data is nullptr");
		return;
	}

	rapidjson::Document doc;
	doc.Parse(data);
	if (doc.HasParseError()) {
		LordLogError("_parseUserAttr fail, parse json fail, [%s]", data);
		return;
	}

	if (!doc.HasMember("id") || !doc.HasMember("clz") 
		|| !doc.HasMember("team") || !doc.HasMember("rid")
		|| !doc.HasMember("pioneer") || !doc.HasMember("sex")
		|| !doc.HasMember("vip"))
	{
		LordLogError("_parseUserAttr fail, lack field, jsonStr=[%s]", data);
		return;
	}

	LordLogInfo("begin to _parseUserAttr, jsonStr=[%s]", data);

	int64_t userId = doc["id"].GetInt64();
	int32_t classes = doc["clz"].GetInt();
	int32_t team = doc["team"].GetInt();
	int32_t regionId = doc["rid"].GetInt();
	bool pioneer = doc["pioneer"].GetBool();
	int32_t sex = doc["sex"].GetInt();
	int32_t vip = doc["vip"].GetInt();
	int64_t targetUserId = 0;
	int32_t manorId = 0;
	if (doc.HasMember("loc") && doc.HasMember("tid"))
	{
		targetUserId = doc["tid"].GetInt64();
		manorId = doc["loc"].GetInt();
	}

	int32_t faceId = 0;
	int32_t hairId = 0;
	int32_t topsId = 0;
	int32_t pantsId = 0;
	int32_t shoesId = 0;
	int32_t glassesId = 0;
	int32_t scarfId = 0;
	int32_t wingId = 0;
	int32_t hatId = 0;
	int32_t decoratehatId = 0;
	int32_t armId = 0;
	int32_t tailId = 0;
	int32_t extrawingId = 0;
	int32_t footHaloId = 0;
	int32_t backEffectId = 0;
	int32_t crownId = 0;
	int32_t headEffectId = 0;
	Color skinColor = Color(0, 0, 0 , 0);
	int32_t defaultIdle = 0;
	int32_t bagId = 0;

	if (doc.HasMember("skin") && doc["skin"].IsObject()) {

		for (auto& m : doc["skin"].GetObject()) {
			std::string key = m.name.GetString();
			if (key.compare("custom_face") == 0) {
				faceId = std::stoi(m.value.GetString());
			}
			else if (key.compare("custom_hair") == 0) {
				hairId = std::stoi(m.value.GetString());
			}
			else if (key.compare("clothes_tops") == 0) {
				topsId = std::stoi(m.value.GetString());
			}
			else if (key.compare("clothes_pants") == 0) {
				pantsId = std::stoi(m.value.GetString());
			}
			else if (key.compare("custom_shoes") == 0) {
				shoesId = std::stoi(m.value.GetString());
			}
			else if (key.compare("custom_glasses") == 0) {
				glassesId = std::stoi(m.value.GetString());
			}
			else if (key.compare("custom_scarf") == 0) {
				scarfId = std::stoi(m.value.GetString());
			}
			else if (key.compare("custom_wing") == 0) {
				wingId = std::stoi(m.value.GetString());
			}
			else if (key.compare("custom_hat") == 0 ) {
				hatId =  std::stoi(m.value.GetString());
			}
			else if (key.compare("custom_decorate_hat") == 0) {
				decoratehatId = std::stoi(m.value.GetString());
			}
			else if (key.compare("custom_hand") == 0) {
				armId = std::stoi(m.value.GetString());
			}
			else if (key.compare("custom_tail") == 0) {
				tailId = std::stoi(m.value.GetString());
			}
			else if (key.compare("custom_wing_flag") == 0) {
				extrawingId = std::stoi(m.value.GetString());
			}
			else if (key.compare("custom_foot_halo") == 0) {
				footHaloId = std::stoi(m.value.GetString());
			}
			else if (key.compare("custom_back_effect") == 0) {
				backEffectId = std::stoi(m.value.GetString());
			}
			else if (key.compare("custom_crown") == 0) {
				crownId = std::stoi(m.value.GetString());
			}
			else if (key.compare("custom_head_effect") == 0) {
				headEffectId = std::stoi(m.value.GetString());
			}
			else if(key.compare("skin_color") == 0)
			{
				String color = m.value.GetString();
				StringArray arry = StringUtil::Split(color, "-");
				if (arry.size() == 4)
				{
					skinColor = Color(std::stof(arry[0].c_str()) / 255.f, std::stof(arry[1].c_str()) / 255.f, std::stof(arry[2].c_str()) / 255.f, std::stof(arry[3].c_str()) / 255.f);
				}
				else
				{
					LordLogInfo("_parseUserAttr skinColor fail , unknow field in skin, jsonStr=[%s]", data);
				}
			}
			else if (key.compare("animation_idle") == 0) {
				defaultIdle = std::stoi(m.value.GetString());
			}
			else if (key.compare("custom_bag") == 0) {
				bagId = std::stoi(m.value.GetString());
			}
			else 
			{
				LordLogWarning("_parseUserAttr fail, unknow field in skin, jsonStr=[%s]", data);
			}
		}
	}

	UserAttrInfo attr;
	attr.userId = userId;
	attr.classes = classes;
	attr.team = team;
	attr.regionId = regionId;
	attr.pioneer = pioneer;
	attr.sex = sex;
	attr.vip = vip;
	attr.targetUserId = targetUserId;
	attr.manorId = manorId;

	attr.faceId = faceId > 0 ? faceId : 1;
	attr.hairId = hairId > 0 ? hairId : 1;
	attr.topsId = topsId > 0 ? topsId : 1;
	attr.pantsId = pantsId > 0 ? pantsId : 1;
	attr.shoesId = shoesId > 0 ? shoesId :1;
	attr.glassesId = glassesId;
	attr.scarfId = scarfId;
	attr.wingId = wingId;
	attr.hatId = hatId;
	attr.decoratehatId = decoratehatId;
	attr.armId = armId;
	attr.tailId = tailId;
	attr.extrawingId = extrawingId;
	attr.footHaloId = footHaloId;
	attr.backEffectId = backEffectId;
	attr.crownId = crownId;
	attr.headEffectId = headEffectId;
	attr.skinColor = skinColor;
	attr.defaultIdle = defaultIdle;
	attr.bagId = bagId;
	attr.updateTimestamp = LORD::Time::Instance()->getMilliseconds() / 1000;
	m_handler->onUserAttr(attr);

	LordLogInfo("_parseUserAttr succ, userId=[%d], targetUserId[%d] , manorId[%d], sex=[%d], faceId=[%d], hairId=[%d], topsId=[%d],"
		"pantsId=[%d], shoesId=[%d], glassesId=[%d], scarfId=[%d], wingId=[%d], tailId=[%d], extrawingId=[%d], footHaloId=[%d], armId=[%d], decoratehatId=[%d], crownId=[%d], skinColor r[%f], skinColor g[%f], skinColor b[%f], skinColor a[%f], defaultIdle[%d]",
		userId,  targetUserId, manorId, sex, faceId, hairId, topsId, pantsId, shoesId, glassesId, scarfId, wingId, tailId, extrawingId, footHaloId, armId, decoratehatId, crownId, skinColor.r, skinColor.g, skinColor.b, skinColor.a, defaultIdle);
}

void RoomClient::_parseUserManorRelease(const char * data)
{
	if (!data) {
		LordLogError("_parseUserManorRelease fail, data is nullptr");
		return;
	}

	rapidjson::Document doc;
	doc.Parse(data);
	if (doc.HasParseError()) {
		LordLogError("_parseUserManorRelease fail, parse json fail, [%s]", data);
		return;
	}

	if (!doc.HasMember("userId"))
	{
		LordLogError("_parseUserManorRelease fail, lack field, jsonStr=[%s]", data);
		return;
	}

	LordLogInfo("_parseUserManorRelease succeed, lack succeed, jsonStr=[%s]", data);

	int64_t userId = doc.FindMember("userId")->value.GetInt64();
	m_handler->onUserManorRelease(userId);
}

void RoomClient::_parseBroadcastMessage(const char * data)
{
	if (!data) {
		LordLogError("_parseBroadcastMessage fail, data is nullptr");
		return;
	}

	rapidjson::Document doc;
	doc.Parse(data);
	if (doc.HasParseError()) {
		LordLogError("_parseBroadcastMessage fail, parse json fail, [%s]", data);
		return;
	}

	BroadcastMessage message = BroadcastMessage();

	if (doc.HasMember("type") && !doc.FindMember("type")->value.IsNull())
	{
		message.type = doc.FindMember("type")->value.GetInt();
	}

	if (doc.HasMember("scope") && !doc.FindMember("scope")->value.IsNull())
	{
		message.scope = doc.FindMember("scope")->value.GetString();
	}

	if (doc.HasMember("gameType") && !doc.FindMember("gameType")->value.IsNull())
	{
		message.gameType = doc.FindMember("gameType")->value.GetString();
	}

	if (doc.HasMember("targets") && !doc.FindMember("targets")->value.IsNull())
	{
		const auto & targets = doc.FindMember("targets")->value.GetArray();
		for (const auto & item : targets)
		{
			message.targets.push_back(item.GetInt64());
		}
	}

	if (doc.HasMember("content") && !doc.FindMember("content")->value.IsNull())
	{
		message.content = doc.FindMember("content")->value.GetString();
	}
	LordLogInfo("_parseBroadcastMessage, jsonStr=[%s]", data);
	m_handler->onReceive(message);
}

void RoomClient::tick() {
	m_asyncClient->dispatch();
	_parseRecv();
}
