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
	};
}

#endif // !__CHUNK_SERVICE_CLIENT_HEADER__
