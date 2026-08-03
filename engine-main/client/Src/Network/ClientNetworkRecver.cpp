#include "ClientNetworkRecver.h"
#include "game.h"
#include "Network/S2CPacketHandles/S2CInitPacketHandles.h"
#include "Network/S2CPacketHandles/S2CPlayerActionPacketHandles.h"
#include "Network/S2CPacketHandles/S2CStatusPacketHandles.h"
#include "Network/S2CPacketHandles/S2CChatPacketHandles.h"
#include "Network/S2CPacketHandles/S2CEntityPacketHandles.h"
#include "Network/S2CPacketHandles/S2CGamePacketHandles.h"
#include "Network/S2CPacketHandles/S2CRanchPacketHandles.h"
#include "Network/S2CPacketHandles/S2CBirdSimulatorPacketHandles.h"
#include "Network/protocol/AutoRegisterPacketS2C.h"
#include "Network/S2CPacketDispatch.h"
#include "Object/Root.h"

ClientNetworkRecver::ClientNetworkRecver()
{
	AutoRegisterS2C s2c;
	s2c.autoRegister<S2CPacketType::ProtocolEnd>();

	m_packetDispatch = LORD::make_shared<S2CPacketDispatch>();
	m_packetDispatch->registerClass<S2CInitPacketHandles>();
	m_packetDispatch->registerClass<S2CPlayerActionPacketHandles>();
	m_packetDispatch->registerClass<S2CStatusPacketHandles>();
	m_packetDispatch->registerClass<S2CChatPacketHandles>();
	m_packetDispatch->registerClass<S2CEntityPacketHandles>();
	m_packetDispatch->registerClass<S2CGamePacketHandles>();
	m_packetDispatch->registerClass<S2CRanchPacketHandles>();
	m_packetDispatch->registerClass<S2CBirdSimulatorPacketHandles>();
}

void ClientNetworkRecver::recvPacket(std::shared_ptr<DataPacket> &packet)
{
	this->m_waitHandlerPacket.push(packet);
}

void ClientNetworkRecver::handlePacket()
{
	std::shared_ptr<DataPacket> ptr;
	while(m_waitHandlerPacket.try_pop(ptr))
	{
		if(ptr)
		{
			m_packetDispatch->handlePacket(ptr);
		}
	}
}

void ClientNetworkRecver::recvTick()
{
	if (!GameClient::CGame::Instance()->isEnterBack())
	{
		this->handlePacket();
	}
}


