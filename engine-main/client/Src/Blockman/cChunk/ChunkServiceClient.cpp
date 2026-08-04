#include "ChunkServiceClient.h"
#include "Chunk/Chunk.h"
#include "cWorld/BlockChangeRecorderClient.h"
#include "cWorld/SignTextsChangeRecorder.h"
#include "game.h"
#include "Network/ClientNetwork.h"
#include "Network/ClientPacketSender.h"

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

	void ChunkServiceClient::onChunkMiss(int x, int z)
	{
		// Fire-and-forget — the server will reply async via S2CPacketChunkData.
		// We don't block here: getChunk() returns the NonexistentChunk to the
		// caller (renderer sees empty/air), and when the network reply
		// arrives, injectChunk() puts the real chunk in the cache. The next
		// frame's getChunk() hits the cache.
		auto network = GameClient::CGame::Instance()->getNetwork();
		if (network && network->getSender())
		{
			network->getSender()->sendRequestChunk(x, z);
		}
	}
}