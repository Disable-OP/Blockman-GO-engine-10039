#ifndef __CLIENTNETWORK_H__
#define __CLIENTNETWORK_H__
#include "ClientNetworkCore.h"
#include "ClientNetworkRecver.h"
#include "Network/protocol/DataPacket.h"
#include "Util/Concurrent/ThreadSafeQueue.h"
#include "Util/SubscriptionGuard.h"
#include "Core.h"
#include "Object/Singleton.h"
#include "ClientPacketSender.h"
#include "Network/DataCache.h"

using namespace  LORD;

class S2CPacketDispatch;
class ClientNetwork:public Singleton<ClientNetwork>, public ObjectAlloc
{
public:

public:
	ClientNetwork();
	virtual ~ClientNetwork(void);
	
	void logicTick();
	void sendPacket(std::shared_ptr<DataPacket> packet, bool immediate = false);
	void connect(const char *ip, ui16 port);
	bool onNetworkConnectionSuccessful();
	void logout();

	void tearDown() { m_networkCore->tearDown(); };
	void flush() { m_networkCore->flush(); };
	void closeSession(ui64 ssid) { m_networkCore->closeSession(ssid); }
	bool isConnected() { return m_networkCore->isConnected(); }
	ClientNetworkCore::NetStatus getStatus(){ return m_networkCore->getStatus(); }
	void setStatus(ClientNetworkCore::NetStatus status) { m_networkCore->setStatus(status); }
	bool isAlive() { return m_networkCore->isAlive(); }

	std::shared_ptr<ClientPacketSender>  getSender() { return m_packetSender; }
	std::shared_ptr<DataCache> getDataCache() { return m_dataCache; }

private:
	SubscriptionGuard m_subscriptionGuard;

	std::shared_ptr<ClientNetworkCore> m_networkCore;

	std::shared_ptr<ClientNetworkRecver> m_networkRecver;

	std::shared_ptr<ClientPacketSender> m_packetSender;

	std::shared_ptr<DataCache> m_dataCache;
};


#endif
