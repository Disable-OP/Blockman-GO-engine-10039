#pragma once

#include "Chunk/ChunkReadableStorageFile.h"
#include "Blockman/World/AnvilManagerServer.h"

namespace BLOCKMAN
{
	class ChunkReadableStorageFileServer : public ChunkReadableStorageFile
	{
	public:
		ChunkReadableStorageFileServer(World* world, const LORD::String& regionDir)
			: ChunkReadableStorageFile(world)
		{
			m_anvilManager.reset(new AnvilManagerServer(regionDir));
		}
	};
}