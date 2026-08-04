/********************************************************************
filename: 	StructurePieceBlockSelector.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-11-7
*********************************************************************/
#ifndef __STRUCTURE_PIECE_BLOCK_SELECTOR_HEADER__
#define __STRUCTURE_PIECE_BLOCK_SELECTOR_HEADER__

#include "Util/Random.h"

namespace BLOCKMAN
{

class StructurePieceBlockSelector : public ObjectAlloc
{
public:
	virtual void selectBlocks(Random& rand, int x, int y, int z, bool var5) = 0;
	
	int getSelectedBlockId() const { return m_selectedBlockId; }
	int getSelectedBlockMetaData() const { return m_selectedBlockMetaData; }


protected:
	int m_selectedBlockId = 0;
	int m_selectedBlockMetaData = 0;
};


class StructureScatteredFeatureStones : public StructurePieceBlockSelector
{
public:
	StructureScatteredFeatureStones() {}
	virtual void selectBlocks(Random& rand, int x, int y, int z, bool var5);
protected:

};


class StructureStrongholdStones : public StructurePieceBlockSelector
{
public:

	void selectBlocks(Random& rand, int x, int y, int z, bool var5);
};

}

#endif
