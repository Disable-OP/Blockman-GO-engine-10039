#include "StructurePieceBlockSelector.h"
#include "BM_TypeDef.h"

namespace BLOCKMAN
{

void StructureScatteredFeatureStones::selectBlocks(Random& rand, int x, int y, int z, bool var5)
{
	if (rand.nextFloat() < 0.4F)
	{
		m_selectedBlockId = BLOCK_ID_COBBLE_STONE;
	}
	else
	{
		m_selectedBlockId = BLOCK_ID_COBBLE_STONE_MOSSY;
	}
}

void StructureStrongholdStones::selectBlocks(Random& rand, int x, int y, int z, bool var5)
{
	if (var5)
	{
		m_selectedBlockId = BLOCK_ID_BRICK;
		float probability = rand.nextFloat();

		if (probability < 0.2f)
		{
			m_selectedBlockMetaData = 2;
		}
		else if (probability < 0.5F)
		{
			m_selectedBlockMetaData = 1;
		}
		else if (probability < 0.55F)
		{
			m_selectedBlockId = BLOCK_ID_SILVER_FISH;
			m_selectedBlockMetaData = 2;
		}
		else
		{
			m_selectedBlockMetaData = 0;
		}
	}
	else
	{
		m_selectedBlockId = 0;
		m_selectedBlockMetaData = 0;
	}
}

}