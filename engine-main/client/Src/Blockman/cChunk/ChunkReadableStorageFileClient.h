#pragma once

#include "Chunk/ChunkReadableStorageFile.h"
#include "cWorld/AnvilManagerClient.h"

namespace BLOCKMAN
{
	class ChunkReadableStorageFileClient : public ChunkReadableStorageFile
	{
	public:
		ChunkReadableStorageFileClient(World* world, const LORD::String& regionDir)
			: ChunkReadableStorageFile(world)
		{
			m_anvilManager.reset(new AnvilManagerClient(regionDir));
		}
	};
}