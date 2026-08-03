#ifndef __CHUNK_SERVICE_SERVER_HEADER__
#define __CHUNK_SERVICE_SERVER_HEADER__

#include "Chunk/ChunkService.h"

namespace BLOCKMAN
{
	class ChunkServiceServer : public ChunkService
	{
	public:
		ChunkServiceServer(World* world)
			: ChunkService(world, 650, false)
		{}
		virtual bool chunkInCache(int x, int z) override
		{
			return getChunk(x, z) != nullptr;
		}

	};
}

#endif