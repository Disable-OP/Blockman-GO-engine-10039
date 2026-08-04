#ifndef __S2CRANCHPACKETHANDLES_H__
#define __S2CRANCHPACKETHANDLES_H__
#include "Network/protocol/S2CPackets.h"

class S2CRanchPacketHandles
{
public:
	static void handlePacket(std::shared_ptr<S2CPacketSyncRanchInfo>&packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncRanchHouse>&packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncRanchStorage>&packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncRanchRank>&packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncRanchBuild>&packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncRanchOrder>&packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncRanchAchievement>&packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncRanchTimePrice>&packet);
	static void handlePacket(std::shared_ptr<S2CPacketRanchGain>&packet);
	static void handlePacket(std::shared_ptr<S2CPacketRanchUnlockItem>&packet);
	static void handlePacket(std::shared_ptr<S2CPacketSyncRanchShortcutFreeTimes>&packet);
	
};

#endif
