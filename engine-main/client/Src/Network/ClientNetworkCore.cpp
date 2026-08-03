#include "ClientNetworkCore.h"
#include "Util/ClientEvents.h"
#include "Util/ClientDataReport.h"
#include "game.h"

ClientNetworkCore::ClientNetworkCore()
	:m_status(emDisconnect)
{
	m_rakinstance = LORD::make_shared<RaknetInstance>("", 0);
}

ClientNetworkCore::~ClientNetworkCore(void)
{
	tearDown();
}

void ClientNetworkCore::tearDown()
{
	if (m_destroyed)
	{
		return;
	}
	m_rakinstance->tearDown();
	m_destroyed = true;
}


void ClientNetworkCore::connect(const char *ip, ui16 port)
{
	strcpy(m_hostIP, ip);
	m_hostPort = port;

	LordLogDebug("begin to connect to game server [%s:%u]", ip, port);
	m_status = emConncting;
	m_rakinstance->connect(ip, port, [this](RaknetInstance::ConnectEvent &event){this->recvConnEvent(event);} );
}

void ClientNetworkCore::recvConnEvent(RaknetInstance::ConnectEvent & event)
{
	this->m_waitHandleConnEvent.push(event);
}

void ClientNetworkCore::handleConnectEvent()
{
	RaknetInstance::ConnectEvent connEvent;
	while (m_waitHandleConnEvent.try_pop(connEvent))
	{
		ui64 id = connEvent.rakssid;
		switch (connEvent.type)
		{
		case RaknetInstance::emConnectSuc:
		{
			m_rakssid = id;
			m_status = ClientNetworkCore::emConnected;

			LordLogInfo("_ClientNetwork_handleConnectEvent_emConnectSuc");

			BLOCKMAN::NetworkConnectionSuccessfulEvent::emit();
		}
		break;

		case RaknetInstance::emConnectFailed:
		{
			m_status = ClientNetworkCore::emConnectFailed;
			LordLogInfo("_ClientNetwork_handleConnectEvent_emConnectFailed");
			static bool isRetryConnect = false;
			if (!isRetryConnect)
			{
				isRetryConnect = true;
				BLOCKMAN::NetworkRetryConnectionEvent::emit();
			}
			else
			{
				BLOCKMAN::NetworkConnectionFailureEvent::emit();
				GameClient::CGame::Instance()->dataReport(BLOCKMAN::ClientDataReport::GAME_ERROR_CODE, BLOCKMAN::ClientDataReport::ERROR_CODE_NETWORK_CONNECT_FAILS);
				GameClient::CGame::Instance()->dataReportNotType(BLOCKMAN::ClientDataReport::ENTER_GAME_STATUS, BLOCKMAN::ClientDataReport::ERROR_CODE_NETWORK_CONNECT_FAILS);
			}
		}
		break;

		case RaknetInstance::emConnectKickOut:
		{
			m_status = ClientNetworkCore::emDisconnect;
			LordLogInfo("_ClientNetwork_handleConnectEvent_emConnectKickOut");
			BLOCKMAN::NetworkConnectionKickOutEvent::emit();
		}
		break;

		case RaknetInstance::emConnectTimeout:
		{
			if (m_status != ClientNetworkCore::emLoginSuc) {
				GameClient::CGame::Instance()->dataReport(BLOCKMAN::ClientDataReport::GAME_ERROR_CODE, BLOCKMAN::ClientDataReport::ERROR_CODE_NETWORK_TIMEOUT);
				GameClient::CGame::Instance()->dataReportNotType(BLOCKMAN::ClientDataReport::ENTER_GAME_STATUS, BLOCKMAN::ClientDataReport::ERROR_CODE_NETWORK_TIMEOUT);
			}
			else {
				GameClient::CGame::Instance()->dataReportNotType(BLOCKMAN::ClientDataReport::GAME_DISCONNECT, BLOCKMAN::ClientDataReport::GAME_DISCONNECT_TIMEOUT);
			}
			m_status = ClientNetworkCore::emDisconnect;
			LordLogInfo("_ClientNetwork_handleConnectEvent_emConnectTimeout");
			BLOCKMAN::NetworkConnectionTimeoutEvent::emit();
		}
		break;
		case RaknetInstance::emDisconnect:
		{
			m_status = ClientNetworkCore::emDisconnect;
			LordLogInfo("_ClientNetwork_handleConnectEvent_emDisconnect");
			BLOCKMAN::NetworkConnectionDisconnectEvent::emit();
			GameClient::CGame::Instance()->dataReportNotType(BLOCKMAN::ClientDataReport::GAME_DISCONNECT, BLOCKMAN::ClientDataReport::GAME_DISCONNECT_NORMAL);
		}
		break;
		default:
		{
			m_status = ClientNetworkCore::emDisconnect;
			LordLogInfo("_ClientNetwork_handleConnectEvent_emDisconnect");
			BLOCKMAN::NetworkConnectionDisconnectEvent::emit();
			GameClient::CGame::Instance()->dataReportNotType(BLOCKMAN::ClientDataReport::GAME_DISCONNECT, BLOCKMAN::ClientDataReport::GAME_DISCONNECT_UNKNOWN);
		}
		break;
		}
	}
}


