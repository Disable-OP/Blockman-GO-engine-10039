#pragma once

#include "Core.h"
#include "Chunk/IChunkProvider.h"

namespace BLOCKMAN
{
	class World;
	class FlatGeneratorInfo;
	class Section;

	class ChunkProviderFlat : public IChunkProvider, public LORD::ObjectAlloc
	{
	public:
		ChunkProviderFlat(World* pWorld, const LORD::String& generatorSettings);
		virtual ~ChunkProviderFlat();

		virtual ChunkPtr provideChunk(int x, int z) override;

	protected:
		World* m_pWorld = nullptr;

	private:
		int8_t m_blockID[256] = { 0 };
		int8_t m_blockMeta[256] = { 0 };
		FlatGeneratorInfo* m_pFlatGenInfo = nullptr;

		void initialize();
		virtual ChunkPtr CreateChunk(int x, int z);
		virtual Section* CreateSection(int x, int z, int yBase);
	};

}