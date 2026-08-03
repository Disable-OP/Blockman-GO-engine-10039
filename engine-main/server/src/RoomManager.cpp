#include "RoomManager.h"
#include "Script/GameServerEvents.h"
#include "Network/ServerNetwork.h"
#include "Enums/Enums.h"

using namespace LORD;
RoomManager::RoomManager(const std::string& host, unsigned short port, const RoomGameConfig& rgConfig) {
	m_rgConfig = rgConfig;
	m_roomClient = LORD::make_shared<RoomClient>(host, port, this);
}

void RoomManager::tick() {
	m_roomClient->tick();
}

void RoomManager::onConnected() {
	LordLogInfo("connect to room succ and begin to send connect packet, m_isFirstConnect:%s", m_isFirstConnect ? "true" : "false");
	m_roomClient->sendConnect(m_rgConfig.gameId, 1, "success", !m_isFirstConnect, m_rgConfig);
}

void RoomManager::reviseUserAttrInfoMap() {
	std::set<ui64> onlinePlatformUserIds = ServerNetwork::Instance()->getPlatformUserIds();
	ui64 currentTime = LORD::Time::Instance()->getMilliseconds() / 1000;
	std::set<ui64> stalePlatformUserIds;
	for (const auto& attrInfo : m_userAttrInfoMap) {
		if (onlinePlatformUserIds.find(attrInfo.first) != onlinePlatformUserIds.end()) {
			continue;
		}
		if (currentTime - attrInfo.second.updateTimestamp <= 30) {
			continue;
		}

		stalePlatformUserIds.insert(attrInfo.first);
	}

	for (auto id : stalePlatformUserIds) {
		m_userAttrInfoMap.erase(id);
	}
}

void RoomManager::onConnectReply() {
	LordLogInfo("recv room connect reply, m_isFirstConnect:%s", m_isFirstConnect ? "true" : "false");
	if (m_isFirstConnect) {
		m_isFirstConnect = false;
	}
	else {
		LordLogInfo("reconnect succ and recv connect reply, begin to resend game_status and sync_users packet");
		reviseUserAttrInfoMap();
		m_roomClient->sendSyncUsers(m_userAttrInfoMap, m_manorAttrInfoMap);
		m_roomClient->resendGameStatus(m_rgConfig.gameId, m_rgConfig.maxPlayers);
	}
}

void RoomManager::onUserAttr(const UserAttrInfo& attr)
{
	SCRIPT_EVENT::PlayerDynamicAttrEvent::invoke(attr.userId, attr.classes, attr.team, attr.regionId, attr.pioneer, attr.vip);
	SCRIPT_EVENT::PlayerDynamicAttrAndManorInfoEvent::invoke(attr.userId, attr.targetUserId, attr.manorId, attr.classes, attr.team, attr.regionId, attr.pioneer, attr.vip);
	m_userAttrInfoMap[attr.userId] = attr;
	if (attr.manorId > 0)
	{
		int32_t mapSize = m_manorAttrInfoMap.size();
		ManorAttrInfo manorInfo = ManorAttrInfo();
		manorInfo.location = attr.manorId;
		manorInfo.targetUserId = attr.targetUserId;
		m_manorAttrInfoMap[attr.targetUserId] = manorInfo;
		LordLogInfo("RoomManager::onUserAttr m_manorAttrInfoMap:  set before size [%d], set later size [%d]", mapSize, m_manorAttrInfoMap.size());
	}
}

void RoomManager::onUserManorRelease(int64_t userId)
{
	SCRIPT_EVENT::UserManorReleaseEvent::invoke(userId);
	setUserAttrInfoManorId(userId, 0);
	auto clientPeer = ServerNetwork::Instance()->findPlayerByPlatformUserId(userId);
	if (clientPeer)
	{
		ServerNetwork::Instance()->closeSession(clientPeer);
	}
	
}

void RoomManager::onReceive(const BroadcastMessage & message)
{
	LordLogInfo("RoomManager::onReceive BroadcastMessage type = [%d]", message.type);
	switch ((BroadcastType) message.type)
	{
	case BroadcastType::RANCH_ORDER_HELP:
	{

		for (i64 targetId : message.targets)
		{
			auto clientPeer = ServerNetwork::Instance()->findPlayerByPlatformUserId(targetId);
			if (clientPeer)
			{
				ServerNetwork::Instance()->getSender()->sendBroadcastMessage(clientPeer->getRakssid(), message.type, message.content);
			}
		}
	}
	break;
	case BroadcastType::RANCH_ORDER_FINISH:
	{
		for (i64 targetId : message.targets)
		{
			auto clientPeer = ServerNetwork::Instance()->findPlayerByPlatformUserId(targetId);
			if (clientPeer)
			{
				SCRIPT_EVENT::RefreshPlayerRanchOrderEvent::invoke(clientPeer->getRakssid());
				ServerNetwork::Instance()->getSender()->sendBroadcastMessage(clientPeer->getRakssid(), message.type, message.content);
			}
		}
	}
		break;
	case BroadcastType::RANCH_RANK_ENTER_GAME:
		ServerNetwork::Instance()->getSender()->sendBroadcastMessage(0, message.type, message.content);
		break;
	default:
		ServerNetwork::Instance()->getSender()->sendBroadcastMessage(0, message.type, message.content);
		break;
	}
}

bool RoomManager::UserAttrExisted(int64_t userId)
{
	if (m_userAttrInfoMap.find(userId) == m_userAttrInfoMap.end()) {
		return false;
	}
	else {
		return true;
	}
}

bool RoomManager::getUserAttrInfo(int64_t userId, UserAttrInfo& attrInfo) {
	if (m_userAttrInfoMap.find(userId) == m_userAttrInfoMap.end()) {
		return false;
	}

	attrInfo = m_userAttrInfoMap[userId];
	return true;
}

void RoomManager::clearWhenUserOut(int64_t userId)
{
	m_userAttrInfoMap.erase(userId);
}

void RoomManager::setUserAttrInfoManorId(int64_t userId, int32_t manorId)
{
	int32_t mapSize = m_manorAttrInfoMap.size();
	m_manorAttrInfoMap.erase(userId);
	for (auto info : m_userAttrInfoMap)
	{
		if (info.second.targetUserId == userId)
		{
			info.second.manorId = manorId;
		}
	}
	LordLogInfo("RoomManager::setUserAttrInfoManorId m_manorAttrInfoMap: Sell manor ,  targetUserId [%lld] ,  remove before size [%d], remove later size [%d]", userId, mapSize, m_manorAttrInfoMap.size());
}

