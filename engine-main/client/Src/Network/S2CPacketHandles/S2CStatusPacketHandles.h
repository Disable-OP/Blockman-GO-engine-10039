#ifndef __S2CSTATUSPACKETHANDLES_H__
#define __S2CSTATUSPACKETHANDLES_H__
#include "Network/protocol/S2CPackets.h"

class S2CStatusPacketHandles
{
public:
	static void handlePacket(std::shared_ptr<S2CPacketUpdateAttribute>&packet);
	static void handlePacket(std::shared_ptr<S2CPacketPlayerInventory>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketPoisonCircleRange>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncPlayerMovementInput>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncPlayerRotation>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncPlayerJumpChanged>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncPlayerDownChanged>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncPlayerSetFlyingState>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncPlayerSpecialJump>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncPlayerSetSpYaw>& packet);
};

#endif
