#include "ServerNetwork.h"
#include "ClientPeer.h"
#include "Common.h"
#include "Network/protocol/DataPacketFactory.h"

#include "Core.h"
#include "Blockman/Entity/EntityPlayerMP.h"
#include "Setting/MultiLanTipSetting.h"
#include "Util/LanguageKey.h"
#include "Object/Root.h"
#include "Network/protocol/S2CPackets.h"
#include "Network/protocol/AutoRegisterPacketS2C.h"

#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
#define I64FMT "%I64d"
#define U64FMT "%I64u"
#else
#define I64FMT "%lld"
#define U64FMT "%llu"
#endif

using namespace std::placeholders;
ServerNetwork::ServerNetwork(const char *ip, short port)
{
	AutoRegisterS2C s2c;
	s2c.autoRegister<S2CPacketType::ProtocolEnd>();

	m_networkCore = LORD::make_shared<ServerNetworkCore>(ip, port);
	m_networkRecver = LORD::make_shared<ServerNetworkRecver>();
	m_packetSender = LORD::make_shared<ServerPacketSender>();



	LordNew(GameCommon);
	std::shared_ptr<CommonPacketSender> pCommon = std::dynamic_pointer_cast<CommonPacketSender>(this->m_packetSender);
	GameCommon::Instance()->setCommonPacketSender(pCommon);

	m_subscriptionGuard.add(BLOCKMAN::ServerNetworkConnSuccEvent::subscribe(std::bind(&ServerNetwork::onServerNetworkConnSucc, this, _1)));
	m_subscriptionGuard.add(BLOCKMAN::ServerNetworkDisconnectEvent::subscribe(std::bind(&ServerNetwork::onServerNetworkDisconnect, this, _1)));
}

ServerNetwork::~ServerNetwork(void)
{
	GameCommon* pGameComm = GameCommon::Instance();
	LordSafeDelete(pGameComm);
}

bool ServerNetwork::onServerNetworkConnSucc(const ui64 rakssid) {
	auto player = ClientPeer::createPlayer(rakssid);
	this->m_playerMap[rakssid] = player;

	return true;
}

bool ServerNetwork::onServerNetworkDisconnect(const ui64 rakssid) {
	auto delPlayer = m_playerMap.find(rakssid);
	if (delPlayer != m_playerMap.end())
	{
		LordLogInfo("handleConnectEvent RaknetInstance::emDisconnect with session id=" U64FMT, rakssid);
		onDisconnect(delPlayer->second);
	}
	else {
		LordLogError("deleteSession but can not find player with session id=" U64FMT, rakssid);
	}

	return true;
}

void ServerNetwork::host(ui32 maxSession)
{
	m_networkCore->host(maxSession);
	m_networkCore->getRaknetInstance()->recv([this](std::shared_ptr<DataPacket> packet) {m_networkRecver->recvPacket(packet); }, false);
	m_networkCore->getRaknetInstance()->setHeartbeatCallback([]() { Server::Instance()->setRaknetState(); });
}

void ServerNetwork::sendPacket(std::shared_ptr<DataPacket> packet,ui64 rakssid, bool immediate /* = false */)
{
	if (!packet->GetNumberOfBytesUsed())
		packet->encode();
	m_networkCore->getRaknetInstance()->send(packet, rakssid, immediate);
}

void ServerNetwork::flush()
{
	m_networkCore->getRaknetInstance()->flush();
}

void ServerNetwork::deferLoginProcess(ui64 platformUserId, std::shared_ptr<DataPacket> packet) {
	m_networkRecver->deferLoginProcess(platformUserId, packet);
}

bool ServerNetwork::hasBeenDeferredLogin(ui64 platformUserId) {
	return m_networkRecver->hasBeenDeferredLogin(platformUserId);
}

std::shared_ptr<ClientPeer> ServerNetwork::findPlayerByRakssid(ui64 ssid)
{
	std::shared_ptr<ClientPeer> pret;
	for (auto player : m_playerMap)
	{
		if (player.first == ssid)
		{
			pret = player.second;
			break;
		}
	}

	return pret;
}

std::shared_ptr<ClientPeer> ServerNetwork::findPlayerByPlatformUserId(ui64 userId)
{
	for (auto player : m_playerMap)
	{
		if (player.second->getPlatformUserId() == userId)
		{
			return player.second;
		}
	}
	return nullptr;
}

std::set<ui64> ServerNetwork::getPlatformUserIds()
{
	std::set<ui64> platformUserIds;
	for (auto player : m_playerMap)
	{
		platformUserIds.insert(player.second->getPlatformUserId());
	}
	return platformUserIds;
}

int ServerNetwork::getPlayerNumber()
{
	return m_playerMap.size(); 
}

void ServerNetwork::broadCastEntityPacket(std::shared_ptr<DataPacket> packet, ui64 runtimeId)
{
	packet->encode();
	for (auto player : m_playerMap)
	{
		if (player.second->hasLogon() && !(player.second->getEntityRuntimeID() == runtimeId))
		{
			m_networkCore->getRaknetInstance()->send(packet, player.second->getRakssid(), true);
		}
	}
}

void ServerNetwork::broadCastEntityPacket(std::shared_ptr<DataPacket> packet)
{
	packet->encode();
	for (auto player : m_playerMap)
	{
		if (player.second->hasLogon())
		{
			m_networkCore->getRaknetInstance()->send(packet, player.second->getRakssid(), true);
		}
	}
}

template<typename RaknetIdList>
void ServerNetwork::multicastEntityPacket(std::shared_ptr<DataPacket> packet, const RaknetIdList & targets)
{
	packet->encode();
	for (const auto& targetId : targets)
	{
		auto iter = m_playerMap.find(targetId);
		if (iter == m_playerMap.end())
		{
			LordLogWarning("raknet id %llu is not in m_playerMap", targetId);
			continue;
		}
		if (iter->second->hasLogon())
		{
			m_networkCore->getRaknetInstance()->send(packet, targetId, true);
		}
	}
}

template void ServerNetwork::multicastEntityPacket(std::shared_ptr<DataPacket> packet, const LORD::vector<ui64>::type & targets);

void ServerNetwork::foreachPlayer(std::function<void(std::shared_ptr<ClientPeer>)> callBack)
{
	for (auto player : m_playerMap)
	{
		callBack(player.second);
	}
}

void ServerNetwork::onDisconnect(std::shared_ptr<ClientPeer>& clientPeer)
{
	m_networkRecver->clearDeferLogin(clientPeer->getPlatformUserId());
	clientPeer->logout();
	m_playerMap.erase(clientPeer->getRakssid());
	m_serverTimestamp.erase(clientPeer->getRakssid());
	m_clientTimestamp.erase(clientPeer->getRakssid());
	m_timeDelayAccum.erase(clientPeer->getRakssid());
}

void ServerNetwork::closeSession(std::shared_ptr<ClientPeer>& clientPeer)
{
	m_networkCore->closeSession(clientPeer->getRakssid());
	onDisconnect(clientPeer);
}

void ServerNetwork::logicTick()
{
	m_networkCore->connTick();
	m_networkRecver->recvTick();
	this->flush();
}

void ServerNetwork::kickOldLogin(ui64 platformUserId, ui64 rakssid)
{
	std::vector<std::shared_ptr<ClientPeer>> clientArr;
	for (auto i_c : m_playerMap) {
		if (i_c.second->getPlatformUserId() == platformUserId && i_c.second->getRakssid() !=rakssid ) {
			clientArr.push_back(i_c.second);
		}
	}

	for (auto c : clientArr) {
		LordLogWarning("kickOldLogin of same platform uid " U64FMT ", ServerNetwork clean up, raknet id " U64FMT ", name:%s",  platformUserId, c->getRakssid(), c->getName().c_str());

		ServerNetwork::Instance()->getSender()->sendLoginResult(c->getRakssid(), (int32_t)NETWORK_DEFINE::PacketLoginResult::emErrBeKicked);
		closeSession(c);
	}
}

void ServerNetwork::broadCastMsgByType(ui64 targetId, ui8 type, i32 time, int msgType, const char* args)
{
	std::shared_ptr<ChatComponentNotification> notify = nullptr;

	if (targetId == 0)
	{
		for (auto player : m_playerMap)
		{
			if (player.second->hasLogon())
			{
				String lan = player.second->getLanguage();
				notify = getBroadCastMsgNotify(type, time, lan, msgType, args);
				ServerNetwork::Instance()->getSender()->sendNotification(player.second->getRakssid(), notify);
			}
		}
	}
	else
	{
		auto peer = findPlayerByRakssid(targetId);
		if (peer)
		{
			String lan = peer->getLanguage();
			notify = getBroadCastMsgNotify(type, time, lan, msgType, args);
			ServerNetwork::Instance()->getSender()->sendNotification(targetId, notify);
		}
	}
}

void ServerNetwork::sendGameOver(ui64 rakssid, int msgType, int code)
{
	auto player = findPlayerByRakssid(rakssid);
	if (player)
	{
		LordLogInfo("sendGameOver,platform uid " U64FMT ", name %s, rakssId " U64FMT,
			player->getPlatformUserId(), player->getName().c_str(), rakssid);
		String lan = player->getLanguage();
		std::string msg = MultiLanTipSetting::getMessage(lan, msgType, "");
		ServerNetwork::Instance()->getSender()->sendGameover(rakssid, msg.c_str(), code);
	}
	else
	{
		LordLogError("sendGameOver fail, can't find player, rakssId " U64FMT, rakssid);
	}
}

std::shared_ptr<ChatComponentNotification> ServerNetwork::getBroadCastMsgNotify(ui8 type, i32 time, String& lanType, int msgType, const char* args)
{
	std::string msg = MultiLanTipSetting::getMessage(lanType, msgType, args);
	ChatComponentNotification::Type chatType = static_cast<ChatComponentNotification::Type>(type);
	return LORD::make_shared<ChatComponentNotification>(String(msg.c_str()), time, chatType);
}

void ServerNetwork::broadCastGameTipByType(ui64 targetId, ui8 type, i32 time, int msgType, const char * args)
{
	ChatComponentNotification::Type chatType = static_cast<ChatComponentNotification::Type>(type);
	ChatComponentNotification *notify = LordNew ChatComponentNotification(String(args), time, chatType);

	ServerNetwork::Instance()->getSender()->sendGameTipNotification(targetId, msgType, notify);
}


ui32 ServerNetwork::getServerTimestamp(ui64 raknetId)
{
	if (m_serverTimestamp.find(raknetId) != m_serverTimestamp.end()) {
		return m_serverTimestamp[raknetId];
	}
	else {
		return 0;
	}
}

void ServerNetwork::setServerTimestamp(ui64 raknetId, ui32 timestamp)
{
	m_serverTimestamp[raknetId] = timestamp;
}

void ServerNetwork::removeServerTimestamp(ui64 raknetId)
{
	m_serverTimestamp.erase(raknetId);
}

ui32 ServerNetwork::addTimeDelay(ui64 raknetId, ui32 delay, ui32 maxAccum)
{
	if (m_timeDelayAccum.find(raknetId) == m_timeDelayAccum.end()) {
		m_timeDelayAccum[raknetId] = 0;
	}

	m_timeDelayAccum[raknetId] += delay;
	if (m_timeDelayAccum[raknetId] >= maxAccum) {
		m_timeDelayAccum[raknetId] = maxAccum;
	}

	return m_timeDelayAccum[raknetId];
}

bool ServerNetwork::minusTimeDelay(ui64 raknetId, ui32 need, ui32& delayBeforeMinus)
{
	if (m_timeDelayAccum.find(raknetId) == m_timeDelayAccum.end()) {
		m_timeDelayAccum[raknetId] = 0;
	}

	delayBeforeMinus = m_timeDelayAccum[raknetId];

	if (m_timeDelayAccum[raknetId] < need) {
		m_timeDelayAccum[raknetId] = 0;
		return false;
	}

	m_timeDelayAccum[raknetId] -= need;
	return true;
}
