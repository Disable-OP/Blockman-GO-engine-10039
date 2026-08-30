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
		// FIX [SYMPTOM-1]: chunks that arrive over the network never went
		// through the disk path (ChunkReadableStorageFile::prepareChunk),
		// which is the only place that called Chunk::onChunkLoad(). A
		// network-injected chunk therefore kept m_isChunkLoaded == false
		// forever, and EntityLivingBase::handleEntityDrop treated the
		// player's own chunk as "not loaded", replacing real gravity with
		// a fixed motion.y = -0.1 (jump apex ~0.32 blocks, floaty fall).
		// Marking the chunk loaded here restores vanilla physics
		// (0.42 jump / 0.08 gravity -> 1.25-block jump) for every chunk
		// source. onChunkLoad() is idempotent for the disk path.
		chunk->onChunkLoad();
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