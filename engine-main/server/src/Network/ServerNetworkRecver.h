
#ifndef _SERVERNETWORKRECVER_H__
#define _SERVERNETWORKRECVER_H__
#include "Global.h"
#include "Util/Rwq/readerwriterqueue.h"
#include "Network/protocol/DataPacket.h"
#include <queue>

class C2SPacketDispatcher;
class ClientPeer;
class ServerNetworkRecver: public ObjectAlloc
{
public:
	struct DeferLogin {
		std::shared_ptr<DataPacket> loginPacket;
		ui32 recvTime;
		ui64 platformUserId;
	};

public:
	ServerNetworkRecver();
	virtual ~ServerNetworkRecver();

public:
	void recvTick();
	void recvPacket(std::shared_ptr<DataPacket>& packet);

	void deferLoginProcess(ui64 platformUserId, std::shared_ptr<DataPacket> packet);
	bool hasBeenDeferredLogin(ui64 platformUserId);
	void clearDeferLogin(ui64 platformUserId);

private:
	void handlePacket();
	bool onPreHandlePacket(std::shared_ptr<ClientPeer> &peer, std::shared_ptr<DataPacket>&packet);
	
private:

	std::shared_ptr<C2SPacketDispatcher> m_packetDispatcher;

	moodycamel::ReaderWriterQueue<std::shared_ptr<DataPacket>> m_waitHandlerPacket;

	std::queue<DeferLogin> m_deferLoginQueue;
	std::set<ui64> m_deferLoginPlatformUserIds;
};


#endif
