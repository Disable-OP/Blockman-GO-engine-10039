#include "ChunkProviderFlat.h"
#include "WorldGenerator/FlatGeneratorInfo.h"
#include "World/Section.h"
#include "Chunk/Chunk.h"
#include "World/World.h"
#include "World/WorldChunkManager.h"
#include "WorldGenerator/BiomeGen.h"

namespace BLOCKMAN
{
	using namespace LORD;

	void ChunkProviderFlat::initialize()
	{
		m_pWorld = NULL;

		memset(m_blockID, 0, 256);
		memset(m_blockMeta, 0, 256);
		m_pFlatGenInfo = NULL;
	}

	ChunkProviderFlat::ChunkProviderFlat(World* pWorld, const String& generatorSettings)
	{
		initialize();
		m_pWorld = pWorld;
		m_pFlatGenInfo = FlatGeneratorInfo::createFlatGeneratorFromString(generatorSettings);
		LordAssert(m_pFlatGenInfo);

		FlatLayerInfoLst& infos = m_pFlatGenInfo->getFlatLayers();

		for (FlatLayerInfoLst::iterator it = infos.begin(); it != infos.end(); ++it)
		{
			for (int y = it->getMinY(); y < it->getMinY() + it->getLayerCount(); ++y)
			{
				m_blockID[y] = (i8)(it->getFillBlock() & 255);
				m_blockMeta[y] = (i8)(it->getFillBlockMeta());
			}
		}
	}

	ChunkProviderFlat::~ChunkProviderFlat()
	{
		LordSafeDelete(m_pFlatGenInfo);
	}

	ChunkPtr ChunkProviderFlat::CreateChunk(int x, int z)
	{
		return LORD::make_shared<Chunk>(m_pWorld, x, z);
	}

	Section* ChunkProviderFlat::CreateSection(int x, int z, int yBase)
	{
		return LordNew Section(yBase);
	}

	ChunkPtr ChunkProviderFlat::provideChunk(int x, int z)
	{
		ChunkPtr pChunk = CreateChunk(x, z);

		for (int i = 0; i < 256; ++i)
		{
			int sectionIdx = i >> 4;
			Section* pSection = pChunk->getSectionArray()[sectionIdx];

			if (pSection == NULL)
			{
				pSection = CreateSection(x, z, i);
				pChunk->getSectionArray()[sectionIdx] = pSection;
			}

			for (int x = 0; x < 16; ++x)
			{
				for (int z = 0; z < 16; ++z)
				{
					BlockPos ipos(x, i & 15, z);
					pSection->setBlockID(ipos, m_blockID[i] & 255);
					pSection->setBlockMeta(ipos, m_blockMeta[i]);
				}
			}
		}

		pChunk->generateSkylightMap();
		BiomeGenBase** biomes = m_pWorld->getWorldChunkManager()->loadBlockGeneratorData(NULL, 0, x * 16, z * 16, 16, 16);
		i8* biomeIdx = pChunk->getBiomeArray();

		for (int i = 0; i < 256; ++i)
		{
			biomeIdx[i] = (i8)biomes[i]->m_ID;
		}

		pChunk->generateSkylightMap();
		return pChunk;
	}
}