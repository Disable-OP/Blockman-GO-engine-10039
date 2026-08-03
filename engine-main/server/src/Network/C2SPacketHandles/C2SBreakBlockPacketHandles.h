#ifndef __C2S_BREAK_BLOCK_PACKET_HANDLES_HEADER__
#define __C2S_BREAK_BLOCK_PACKET_HANDLES_HEADER__

#include "Core.h"
#include "Network/protocol/C2SPackets.h"
#include "Std/chrono.h"

class ClientPeer;

class C2SBreakBlockPacketHandles
{
private:
	struct BreakBlockRecord
	{
		std::chrono::time_point<std::chrono::steady_clock> startTime;
		Vector3i position;
	};
	static map<int, BreakBlockRecord>::type breakBlockRecords;

public:
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketStartBreakBlock> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketAbortBreakBlock> &packet);
	static void handlePacket(std::shared_ptr<ClientPeer>&  clientPeer, std::shared_ptr<C2SPacketFinishBreakBlock> &packet);
};

#endif // !__C2S_BLOCK_BREAK_PACKET_HANDLES_HEADER__
