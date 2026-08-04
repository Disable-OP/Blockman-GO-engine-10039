#pragma once

#include "Chunk/ChunkProviderGenerate.h"
#include "cChunk/ChunkClient.h"

namespace BLOCKMAN
{
	class ChunkProviderGenerateClient : public ChunkProviderGenerate
	{
		using ChunkProviderGenerate::ChunkProviderGenerate;
	private:
		virtual ChunkPtr CreateChunk(int x, int z) override
		{
			return LORD::make_shared<ChunkClient>(m_pWorld, x, z);
		}
	};
}