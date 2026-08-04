#ifndef __S2C_BIRD_SIMULATOR_PACKET_HANDLES_H__
#define __S2C_BIRD_SIMULATOR_PACKET_HANDLES_H__
#include "Network/protocol/S2CPackets.h"

class S2CBirdSimulatorPacketHandles
{
public:
	static void handlePacket(std::shared_ptr<S2CPacketSyncBirdBag>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncBirdStore>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncBirdPersonalStore>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncBirdPlayerInfo>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncBirdDress>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncBirdFood>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncBirdTask>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncBirdAtlas>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncBirdScope>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncBirdActivity>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketBirdGain>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketBirdNestOperation>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketBirdLotteryResult>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncBirdTimePrice>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketBirdAddScore>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncBirdBuff>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncBirdSimulatorStoreItem>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncBirdSimulatorTaskItem>& packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncBirdSimulatorPersonStoreTab>& packet);
	
	
};

#endif
