/********************************************************************
filename: 	ServerNetwork
file path:	H:\sandboxol\client\blockmango-client\dev\server\src\Network\ServerNetwork.h

version:	1
author:		qutianxiang
company:    sandboxol
date:		2017/06/20
*********************************************************************/
#ifndef _SERVERNETWORK_H__
#define _SERVERNETWORK_H__
#include "Global.h"
#include "Network/ServerNetworkCore.h"
#include "Network/ServerNetworkRecver.h"
#include "Util/Rwq/readerwriterqueue.h"
#include "Util/SubscriptionGuard.h"
#include "ServerPacketSender.h"
#include <queue>

class C2SPacketDispatcher;
class ClientPeer;
class ServerNetwork:public Singleton<ServerNetwork>, public ObjectAlloc
{
public:
	ServerNetwork(const char *ip, short port);
	virtual ~ServerNetwork(void);

public:
	void logicTick();
	void kickOldLogin(ui64 platformUserId, ui64 rakssid);
	void sendPacket(std::shared_ptr<DataPacket> packet, ui64 rakssid, bool immediate = true);
	void broadCastEntityPacket(std::shared_ptr<DataPacket> packet, ui64 runtimeId);
	void broadCastEntityPacket(std::shared_ptr<DataPacket> packet);
	template<typename RaknetIdList>
	void multicastEntityPacket(std::shared_ptr<DataPacket> packet, const RaknetIdList& targets);
	void flush();
	void host(ui32 maxSession);
	void closeSession(std::shared_ptr<ClientPeer>& clientPeer);
	void foreachPlayer(std::function<void(std::shared_ptr<ClientPeer>)> callBack);
	void broadCastMsgByType(ui64 targetId, ui8 type, i32 time, int msgType, const char* args);
	void sendGameOver(ui64 rakssid, int msgType, int code);
	void broadCastGameTipByType(ui64 targetId, ui8 type, i32 time, int msgType, const char* args);

	std::shared_ptr<ClientPeer> findPlayerByRakssid(ui64 ssid);
	std::shared_ptr<ClientPeer> findPlayerByPlatformUserId(ui64 userId);
	std::shared_ptr<ServerPacketSender> getSender() { return m_packetSender; }
	std::map<ui64, std::shared_ptr<ClientPeer>> getPlayers() { return m_playerMap; }
	std::set<ui64> getPlatformUserIds();
	int getPlayerNumber();
	void printDataPacket() { m_networkCore->printDataPacket(); }
	void deferLoginProcess(ui64 platformUserId, std::shared_ptr<DataPacket> packet);
	bool hasBeenDeferredLogin(ui64 platformUserId);

	bool onServerNetworkConnSucc(const ui64 rakssid);
	bool onServerNetworkDisconnect(const ui64 rakssid);

	ui32 getServerTimestamp(ui64 raknetId);
	void setServerTimestamp(ui64 raknetId, ui32 timestamp);
	void removeServerTimestamp(ui64 raknetId);

	ui32 addTimeDelay(ui64 raknetId, ui32 delay, ui32 maxAccum);
	bool minusTimeDelay(ui64 raknetId, ui32 delay, ui32& delayBeforeMinus);

private:
	std::shared_ptr<ChatComponentNotification> getBroadCastMsgNotify(ui8 type, i32 time, String& lanType, int msgType, const char* args);
	void onDisconnect(std::shared_ptr<ClientPeer>& clientPeer);

private:
	BLOCKMAN::SubscriptionGuard m_subscriptionGuard;

	std::shared_ptr<ServerNetworkCore> m_networkCore;
	std::shared_ptr<ServerNetworkRecver> m_networkRecver;
	std::shared_ptr<ServerPacketSender> m_packetSender;

	std::map<ui64, std::shared_ptr<ClientPeer>> m_playerMap;


	std::map<ui64, ui32> m_serverTimestamp;
	std::map<ui64, ui32> m_clientTimestamp;
	std::map<ui64, ui32> m_timeDelayAccum;
};


#endif
