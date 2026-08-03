#include "ServerNetworkCore.h"

#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
#define I64FMT "%I64d"
#define U64FMT "%I64u"
#else
#define I64FMT "%lld"
#define U64FMT "%llu"
#endif

using namespace std::placeholders;
ServerNetworkCore::ServerNetworkCore(const char *ip, short port)
{
	strcpy(m_ip, ip);
	m_port = port;
	m_serverInstance = LORD::make_shared<RaknetInstance>(ip, port);
}


ServerNetworkCore::~ServerNetworkCore(void)
{

}

void ServerNetworkCore::host(ui32 maxSession)
{
	m_serverInstance->listen(maxSession, 
		[this](RaknetInstance::ConnectEvent &connEvent){this->recvConnEvent(connEvent);});

}


void ServerNetworkCore::recvConnEvent(RaknetInstance::ConnectEvent & event)
{
	this->m_waitHandleConnEvent.enqueue(event);
}

void ServerNetworkCore::handleConnectEvent()
{
	RaknetInstance::ConnectEvent connEvent;
	while (m_waitHandleConnEvent.try_dequeue(connEvent))
	{
		ui64 id = connEvent.rakssid;
		switch (connEvent.type)
		{
		case RaknetInstance::emNewConnect:
		{
			BLOCKMAN::ServerNetworkConnSuccEvent::emit(id);
			LordLogInfo("_createNewSession_sessionid_" U64FMT, id);
		}
		break;

		case RaknetInstance::emConnectKickOut:
		case RaknetInstance::emConnectTimeout:
		case RaknetInstance::emDisconnect:
		{
			BLOCKMAN::ServerNetworkDisconnectEvent::emit(id);
			LordLogInfo("_disconnectSession_sessionid_" U64FMT, id);
		}
		break;

		default:
			LordLogError("handleConnectEvent RaknetInstance::other error with session id=" U64FMT, id);
			break;
		}
	}
}



void ServerNetworkCore::closeSession(const ui64 raknetId)
{
	m_serverInstance->closeSession(raknetId);
}

void ServerNetworkCore::connTick()
{
	this->handleConnectEvent();
}


