#ifndef _ServerNetworkCore_H__
#define _ServerNetworkCore_H__
#include "Global.h"
#include "Network/RaknetInstance.h"
#include "Util/Rwq/readerwriterqueue.h"
#include "Util/Event.h"

namespace BLOCKMAN {

	struct ServerNetworkConnSuccEvent : Event<ServerNetworkConnSuccEvent, const ui64> {};
	struct ServerNetworkDisconnectEvent : Event<ServerNetworkDisconnectEvent, const ui64> {};

};

class ServerNetworkCore: public ObjectAlloc
{
public:
	ServerNetworkCore(const char *ip, short port);
	virtual ~ServerNetworkCore(void);

public:
	void host(ui32 maxSession);
	void closeSession(const ui64 raknetId);
	void connTick();
	void printDataPacket() { m_serverInstance->printColletingPacket(); }
	std::shared_ptr<RaknetInstance> getRaknetInstance() { return m_serverInstance; }

private:
	void handleConnectEvent();
	void recvConnEvent(RaknetInstance::ConnectEvent& event);

private:
	char m_ip[20];
	short m_port;

	std::shared_ptr<RaknetInstance> m_serverInstance;
	moodycamel::ReaderWriterQueue<RaknetInstance::ConnectEvent> m_waitHandleConnEvent;
};


#endif
