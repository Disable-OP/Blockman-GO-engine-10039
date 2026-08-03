/********************************************************************
filename: 	C2SInitPacketHandles
file path:	H:\sandboxol\client\blockmango-client\dev\server\src\Network\PacketHandles\C2SInitPacketHandles.h

version:	1
author:		qutianxiang
company:    sandboxol
date:		2017/07/25
*********************************************************************/
#ifndef __INITPACKETHANDLES_H__
#define __INITPACKETHANDLES_H__
#include <memory>
#include "Network/protocol/C2SPackets.h"

class ClientPeer;

class C2SInitPacketHandles
{
public:
	static void handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketLogin>& packet);
	static void handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketRebirth> & packet);
	static void handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketCheckCSVersion>& packet);
	static void handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketUpdateUserMoney>& packet);
	
};

#endif
