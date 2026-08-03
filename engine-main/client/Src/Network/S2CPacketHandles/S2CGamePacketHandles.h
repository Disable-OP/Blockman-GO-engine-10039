#ifndef __S2CGAMEPACKETHANDLES_H__
#define __S2CGAMEPACKETHANDLES_H__

#include "Network/protocol/S2CPackets.h"

class S2CGamePacketHandles
{
public:
	static void handlePacket(std::shared_ptr<S2CPacketCarDirection>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketCarProgress>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketEnableAutoShoot>& packet);
};

#endif