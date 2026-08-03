#ifndef __CLIENTNETWORK_RECVER_H__
#define __CLIENTNETWORK_RECVER_H__
#include "Network/protocol/DataPacket.h"
#include "Util/Concurrent/ThreadSafeQueue.h"
#include "Core.h"

using namespace  LORD;

class S2CPacketDispatch;
class ClientNetworkRecver:public ObjectAlloc
{
public:

public:
	ClientNetworkRecver();
	virtual ~ClientNetworkRecver(void) {};
	
	void recvTick();
	void recvPacket(std::shared_ptr<DataPacket> &packet);

private:
	
	void handlePacket();

private:
	std::shared_ptr<S2CPacketDispatch> m_packetDispatch;
	ThreadSafeQueue<std::shared_ptr<DataPacket>> m_waitHandlerPacket;

};


#endif
