#include "ChunkServer.h"
#include "Server.h"
#include "Network/ServerNetwork.h"
#include "Blockman/World/BlockChangeRecorderServer.h"

#include "Script/GameServerEvents.h"
namespace BLOCKMAN
{

	bool ChunkServer::setBlockIDAndMetaWithoutSync(const BlockPos & pos, int blockID, int meta)
	{
		return Chunk::setBlockIDAndMeta(pos, blockID, meta);
	}

	bool ChunkServer::setBlockMetaWithoutSync(const BlockPos & pos, int meta)
	{
		return Chunk::setBlockMeta(pos, meta);
	}

	bool ChunkServer::setBlockIDAndMeta(const BlockPos & pos, int blockID, int meta, bool immediate)
	{
		int srcId = getBlockID(pos);
		int srcMeta = getBlockMeta(pos);


		if (Chunk::setBlockIDAndMeta(pos, blockID, meta))
		{
			auto absoluteBlockPos = toAbsoluteBlockPos(pos);
			BlockChangeRecorderServer::Instance()->record(absoluteBlockPos, srcId, srcMeta);

			if (immediate)
			{
				auto sender = Server::Instance()->getNetwork()->getSender();
				Server::Instance()->getWorld()->addSingleUpdateBlock(absoluteBlockPos, blockID, meta);
				// sender->broadCastUpdateBlock(absoluteBlockPos);
			}
			else
			{
				BlockChangeRecorderServer::Instance()->addToSyncList(absoluteBlockPos);
			}
				
			return true;
		}
		return false;
	}

	bool ChunkServer::setBlockMeta(const BlockPos & pos, int meta)
	{
		int srcId = getBlockID(pos);
		int srcMeta = getBlockMeta(pos);
		if (Chunk::setBlockMeta(pos, meta))
		{
			auto absoluteBlockPos = toAbsoluteBlockPos(pos);
			auto sender = Server::Instance()->getNetwork()->getSender();
			BlockChangeRecorderServer::Instance()->record(absoluteBlockPos, srcId, srcMeta);
			Server::Instance()->getWorld()->addSingleUpdateBlock(absoluteBlockPos, srcId, meta);
			// sender->broadCastUpdateBlock(absoluteBlockPos);
			return true;
		}
		return false;
	}

}
