#pragma once

#include "Chunk/ChunkProviderHell.h"
#include "cChunk/ChunkClient.h"

namespace BLOCKMAN
{
	class ChunkProviderHellClient : public ChunkProviderHell
	{
		using ChunkProviderHell::ChunkProviderHell;
	private:
		virtual ChunkPtr CreateChunk(int x, int z) override
		{
			return LORD::make_shared<ChunkClient>(m_pWorld, x, z);
		}
	};
}