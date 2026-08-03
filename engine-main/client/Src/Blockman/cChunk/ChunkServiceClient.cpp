#include "ChunkServiceClient.h"
#include "Chunk/Chunk.h"
#include "cWorld/BlockChangeRecorderClient.h"
#include "cWorld/SignTextsChangeRecorder.h"

namespace BLOCKMAN
{
	ChunkServiceClient::ChunkServiceClient(World * world, unsigned range)
		: ChunkService(world, range, true)
	{
		m_eventSubscription = ClientBlockChangeRecordClearEvent::subscribe([this](int x, int z) -> bool
		{
			auto chunk = getChunk(x, z);
			saveChunk(x, z, false);
			return true;
		});
	}

	ChunkServiceClient::~ChunkServiceClient()
	{
		m_eventSubscription.unsubscribe();
	}

	void ChunkServiceClient::prepareChunk(const ChunkPtr & chunk)
	{
		ChunkService::prepareChunk(chunk);
		chunk->setSectionXZ();
		BlockChangeRecorderClient::Instance()->applyChanges(chunk);
		SignTextsChangeRecorder::Instance()->applyChanges(chunk);
	}
}