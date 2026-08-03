#ifndef __CHUNK_SERVICE_CLIENT_HEADER__
#define __CHUNK_SERVICE_CLIENT_HEADER__

#include "Chunk/ChunkService.h"
#include "Util/ClientEvents.h"

namespace BLOCKMAN
{
	class ChunkServiceClient : public ChunkService
	{
	public:
		ChunkServiceClient(World* world, unsigned range);
		~ChunkServiceClient();

	private:
		ClientBlockChangeRecordClearEvent::Subscription m_eventSubscription;

		virtual void prepareChunk(const ChunkPtr& chunk) override;

		// Server-authoritative worldgen: when the local disk-based provider
		// returns a NonexistentChunk (no .mca file for this region), ask
		// the server for the chunk. The server replies async via
		// S2CPacketChunkData, which is decoded and injected by
		// S2CChunkPacketHandles::handlePacket(S2CPacketChunkData).
		virtual void onChunkMiss(int x, int z) override;
	};
}

#endif // !__CHUNK_SERVICE_CLIENT_HEADER__
