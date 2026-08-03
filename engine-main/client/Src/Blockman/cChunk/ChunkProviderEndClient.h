#pragma once

#include "Chunk/ChunkProviderEnd.h"
#include "cChunk/ChunkClient.h"

namespace BLOCKMAN
{
	class ChunkProviderEndClient : public ChunkProviderEnd
	{
		using ChunkProviderEnd::ChunkProviderEnd;
	private:
		virtual ChunkPtr CreateChunk(int x, int z) override
		{
			return LORD::make_shared<ChunkClient>(m_pWorld, x, z);
		}
	};
}