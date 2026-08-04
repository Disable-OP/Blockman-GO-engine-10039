/********************************************************************
filename: 	C2SChunkPacketHandles.h
file path:	engine-main/server/src/Network/C2SPacketHandles/C2SChunkPacketHandles.h

version:	1
author:		Blockman-GO-engine-10039 contributors
date:		2026-08-03

purpose:	Server-side handlers for the server-authoritative worldgen
            chunk-request packets (see docs/WORLDGEN.md). When a client
            asks for a chunk, the server generates (or loads from Anvil)
            and streams an S2CPacketChunkData back.
*********************************************************************/
#ifndef __C2SCHUNKPACKETHANDLES_H__
#define __C2SCHUNKPACKETHANDLES_H__

#include <memory>
#include "Network/protocol/C2SPackets.h"

class ClientPeer;

class C2SChunkPacketHandles
{
public:
	// Handle C2SPacketRequestChunk — client wants one chunk.
	static void handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketRequestChunk>& packet);

	// Handle C2SPacketRequestChunkBulk — client wants N chunks in one shot.
	// m_coords is a flat array of (chunkX, chunkZ) pairs; length must be even.
	static void handlePacket(std::shared_ptr<ClientPeer>& clientPeer, std::shared_ptr<C2SPacketRequestChunkBulk>& packet);
};

#endif
