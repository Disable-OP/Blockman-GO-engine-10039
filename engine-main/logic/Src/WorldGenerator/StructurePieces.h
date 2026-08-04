/********************************************************************
filename: 	StructurePieces.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-11-9
*********************************************************************/
#ifndef __STRUCTURE_MINESHAFT_PIECES_HEADER__
#define __STRUCTURE_MINESHAFT_PIECES_HEADER__

#include "StructureComponent.h"

namespace BLOCKMAN
{

class WeightedRandomChestContent;

class StructureMineshaftPieces
{
public:
	/** List of contents that can generate in Mineshafts. */
	static WRCCArr mineshaftChestContents;

	static void initialize();
	static void uninitialize();

	static StructureComponent* getRandomComponent(SCList& scLst, Random& rand, int x, int y, int z, int par5, int type);

	static StructureComponent* getNextMineShaftComponent(StructureComponent* pComponent, SCList& scLst, Random& rand, int par3, int par4, int par5, int par6, int par7);

	static StructureComponent* getNextComponent(StructureComponent* pComponents, SCList& scLst, Random& rand, int par3, int par4, int par5, int par6, int par7);
	
	static WRCCArr& getChestContext() { return mineshaftChestContents; }
};

class StructureNetherBridgePieces
{	
protected:
	static SNBPWArr primaryComponents;// new StructureNetherBridgePieceWeight[]{ new StructureNetherBridgePieceWeight(ComponentNetherBridgeStraight.class, 30, 0, true), new StructureNetherBridgePieceWeight(ComponentNetherBridgeCrossing3.class, 10, 4), new StructureNetherBridgePieceWeight(ComponentNetherBridgeCrossing.class, 10, 4), new StructureNetherBridgePieceWeight(ComponentNetherBridgeStairs.class, 10, 3), new StructureNetherBridgePieceWeight(ComponentNetherBridgeThrone.class, 5, 2), new StructureNetherBridgePieceWeight(ComponentNetherBridgeEntrance.class, 5, 1) };
	static SNBPWArr secondaryComponents;// new StructureNetherBridgePieceWeight[]{ new StructureNetherBridgePieceWeight(ComponentNetherBridgeCorridor5.class, 25, 0, true), new StructureNetherBridgePieceWeight(ComponentNetherBridgeCrossing2.class, 15, 5), new StructureNetherBridgePieceWeight(ComponentNetherBridgeCorridor2.class, 5, 10), new StructureNetherBridgePieceWeight(ComponentNetherBridgeCorridor.class, 5, 10), new StructureNetherBridgePieceWeight(ComponentNetherBridgeCorridor3.class, 10, 3, true), new StructureNetherBridgePieceWeight(ComponentNetherBridgeCorridor4.class, 7, 2), new StructureNetherBridgePieceWeight(ComponentNetherBridgeNetherStalkRoom.class, 5, 2) };

public:
	static void initialize();
	static void uninitialize();

	static ComponentNetherBridgePiece* createNextComponentRandom(StructureNetherBridgePieceWeight* weight, SCList& par1List, Random& rand,
		int par3, int par4, int par5, int par6, int par7);
	
	static ComponentNetherBridgePiece* createNextComponent(StructureNetherBridgePieceWeight* weight, SCList& par1List, Random& rand,
		int par3, int par4, int par5, int par6, int par7);

	static SNBPWArr& getPrimaryComponents() { return primaryComponents; }

	static SNBPWArr& getSecondaryComponents() { return secondaryComponents; }
};


}

#endif
