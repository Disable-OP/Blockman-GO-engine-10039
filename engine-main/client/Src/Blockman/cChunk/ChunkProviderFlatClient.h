#pragma once

#include "Chunk/ChunkProviderFlat.h"
#include "cChunk/ChunkClient.h"

namespace BLOCKMAN
{
	class ChunkProviderFlatClient : public ChunkProviderFlat
	{
		using ChunkProviderFlat::ChunkProviderFlat;
	private:
		virtual ChunkPtr CreateChunk(int x, int z) override
		{
			return LORD::make_shared<ChunkClient>(m_pWorld, x, z);
		}
		virtual Section* CreateSection(int x, int z, int yBase) override
		{
			return LordNew SectionClient(yBase);
		}
	};
}