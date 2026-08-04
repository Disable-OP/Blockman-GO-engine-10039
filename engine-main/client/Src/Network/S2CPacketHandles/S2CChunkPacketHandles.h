#ifndef __S2CCHUNKPACKETHANDLES_H__
#define __S2CCHUNKPACKETHANDLES_H__

#include <memory>
#include "Network/protocol/S2CPackets.h"

// Client-side handlers for the server-authoritative worldgen packet family.
//
//   S2CPacketWorldInfo (283) — sent by the server on connect, BEFORE any chunk
//   data. Carries the world's spawn point + world type + dimension. The client
//   uses this to override its local hardcoded spawn and to know whether to
//   expect chunk data packets (when worldType == TERRAIN_TYPE_CUSTOM).
//
//   S2CPacketChunkData (284) — full chunk payload. The blob is an
//   Anvil-compatible zlib-compressed NBT stream produced by the server's
//   encodeChunkToBlob() helper. We decode it using the same NBT pipeline the
//   disk reader uses, then inject the resulting Chunk into the client's
//   ChunkService cache so subsequent getChunk() calls hit the cache instead
//   of triggering another network round-trip.
class S2CChunkPacketHandles
{
public:
        static void handlePacket(std::shared_ptr<S2CPacketWorldInfo>& packet);
        static void handlePacket(std::shared_ptr<S2CPacketChunkData>& packet);
};
#endif
