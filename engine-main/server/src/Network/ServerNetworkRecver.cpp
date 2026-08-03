#include "ServerNetworkRecver.h"
#include "ClientPeer.h"
#include "Network/protocol/DataPacketFactory.h"

#include "C2SPacketHandles/C2SInitPacketHandles.h"
#include "C2SPacketHandles/C2SPlayerActionPacketHandles.h"
#include "C2SPacketHandles/C2SBreakBlockPacketHandles.h"

#include "Network/protocol/AutoRegisterPacketC2S.h"

#include "Network/C2SPacketDispatcher.h"
#include "Core.h"
#include "Object/Root.h"

#define MAX_DEFER_LOGIN_TIME 2000 // milliseconds

using namespace std::placeholders;
ServerNetworkRecver::ServerNetworkRecver()
{

	AutoRegisterC2S c2s;
	c2s.autoRegister<C2SPacketType::ProtocolEnd>();
	m_packetDispatcher = LORD::make_shared<C2SPacketDispatcher>();

	m_packetDispatcher->setPreHandle(std::bind(&ServerNetworkRecver::onPreHandlePacket, this, _1, _2));

	m_packetDispatcher->registerClass<C2SInitPacketHandles>();
	m_packetDispatcher->registerClass<C2SPlayerActionPacketHandles>();
	m_packetDispatcher->registerClass<C2SBreakBlockPacketHandles>();

}

ServerNetworkRecver::~ServerNetworkRecver()
{

}

void ServerNetworkRecver::recvPacket(std::shared_ptr<DataPacket>& packet)
{
	this->m_waitHandlerPacket.enqueue(packet);
}


bool ServerNetworkRecver::onPreHandlePacket(std::shared_ptr<ClientPeer> &peer, std::shared_ptr<DataPacket>&packet)
{
	
	if (!peer) {
		return false;
	}
	
	if (peer && peer->getEntityPlayer() == nullptr
		&& packet->pid() != static_cast<short>(C2SPacketType::Login)
		&& packet->pid() != static_cast<short>(C2SPacketType::CheckCSVersion)
		&& packet->pid() != static_cast<short>(C2SPacketType::Rebirth)) 
	{
		return false;
	}

	return true;
}

void ServerNetworkRecver::handlePacket()
{
	std::shared_ptr<DataPacket> ptr;
	while(m_waitHandlerPacket.try_dequeue(ptr))
	{
		m_packetDispatcher->handlePacket(ptr);
	}

	if (!m_deferLoginQueue.empty()) {
		LORD::ui32 currentTime = LORD::Root::Instance()->getCurrentTime();
		while (!m_deferLoginQueue.empty()) {
			auto dl = m_deferLoginQueue.front();
			if (currentTime - dl.recvTime < MAX_DEFER_LOGIN_TIME) {
				break;
			}

			if (m_deferLoginPlatformUserIds.find(dl.platformUserId) != m_deferLoginPlatformUserIds.end()) {
				m_packetDispatcher->handlePacket(dl.loginPacket);
			}
			
			m_deferLoginQueue.pop();
			m_deferLoginPlatformUserIds.erase(dl.platformUserId);
		}
	}
}

void ServerNetworkRecver::deferLoginProcess(ui64 platformUserId, std::shared_ptr<DataPacket> packet) {
	ServerNetworkRecver::DeferLogin dl;
	dl.loginPacket = packet;
	dl.recvTime = LORD::Root::Instance()->getCurrentTime();
	dl.platformUserId = platformUserId;

	m_deferLoginPlatformUserIds.insert(platformUserId);
	m_deferLoginQueue.push(dl);
}

bool ServerNetworkRecver::hasBeenDeferredLogin(ui64 platformUserId) {
	if (m_deferLoginPlatformUserIds.find(platformUserId) == m_deferLoginPlatformUserIds.end()) {
		return false;
	}
	else {
		return true;
	}
}

void ServerNetworkRecver::clearDeferLogin(ui64 platformUserId) {
	m_deferLoginPlatformUserIds.erase(platformUserId);
}

void ServerNetworkRecver::recvTick()
{
	this->handlePacket();
}
