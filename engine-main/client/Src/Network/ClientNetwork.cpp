#include <functional>
#include "ClientNetwork.h"
#include "ClientNetworkCore.h"
#include "Util/ClientEvents.h"
#include "Network/RaknetInstance.h"
#include "Common.h"
#include "Network/protocol/AutoRegisterPacketC2S.h"
#include "Network/protocol/C2SPackets.h"


ClientNetwork::ClientNetwork()
{
	m_networkCore = LORD::make_shared<ClientNetworkCore>();
	m_subscriptionGuard.add(BLOCKMAN::NetworkConnectionSuccessfulEvent::subscribe(std::bind(&ClientNetwork::onNetworkConnectionSuccessful, this)));

	m_packetSender = LORD::make_shared<ClientPacketSender>();

	m_dataCache = LORD::make_shared<DataCache>();

	std::shared_ptr<CommonPacketSender> pCommon = std::dynamic_pointer_cast<CommonPacketSender>(this->m_packetSender);
	BLOCKMAN::GameCommon::Instance()->setCommonPacketSender(pCommon);

	AutoRegisterC2S c2s;
	c2s.autoRegister<C2SPacketType::ProtocolEnd>();
	m_networkRecver = LORD::make_shared<ClientNetworkRecver>();
}

ClientNetwork::~ClientNetwork(void)
{
	tearDown();
	m_subscriptionGuard.unsubscribeAll();
}


void ClientNetwork::connect(const char *ip, ui16 port)
{
	m_networkCore->connect(ip, port);
	m_networkCore->getRaknetInstance()->recv([this](std::shared_ptr<DataPacket> packet) {m_networkRecver->recvPacket(packet); }, false);
}

bool ClientNetwork::onNetworkConnectionSuccessful()
{
	m_packetSender->sendCheckCSVersion();
	return true;
}

void ClientNetwork::logout()
{
	auto packet = LORD::make_shared<C2SPacketLogout>();
	sendPacket(packet, true);
	m_networkCore->logout();
}

void ClientNetwork::sendPacket(std::shared_ptr<DataPacket> packet, bool immediate /* = false */)
{
	packet->encode();
	m_networkCore->sendPacket(packet, immediate);
}

void ClientNetwork::logicTick()
{
	m_networkCore->connTick();

	if (m_networkCore->getStatus() == ClientNetworkCore::NetStatus::emLogout)
	{
		return;
	}

	m_networkRecver->recvTick();

	m_networkCore->flush();
}


