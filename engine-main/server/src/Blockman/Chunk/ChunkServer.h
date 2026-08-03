/********************************************************************
filename: 	ChunkServer
file path:	h:\sandboxol\client\blockmango-client\dev\server\src\Chunk\ChunkServer.h

version:	1
author:		qutianxiang
company:    sandboxol
date:		2017/06/20
*********************************************************************/
#ifndef __CHUNK_SERVER_HEADER__
#define __CHUNK_SERVER_HEADER__
#include "Chunk/Chunk.h"

namespace BLOCKMAN
{
	class ChunkServer : public Chunk
	{
		using Chunk::Chunk;
	public:
		bool setBlockIDAndMeta(const BlockPos& pos, int blockID, int meta, bool immediate = true) override;
		bool setBlockMeta(const BlockPos& pos, int meta) override;

		bool setBlockIDAndMetaWithoutSync(const BlockPos& pos, int blockID, int meta) override;
	    bool setBlockMetaWithoutSync(const BlockPos& pos, int meta) override;
	};

	NBTB_BEGIN(ChunkServer, Chunk)
	NBTB_END

}

#endif
