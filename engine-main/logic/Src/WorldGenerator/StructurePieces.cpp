#include "StructurePieces.h"
#include "StructureComponent.h"
#include "BM_TypeDef.h"

#include "Item/Item.h"

namespace BLOCKMAN
{

WRCCArr	StructureMineshaftPieces::mineshaftChestContents;

void StructureMineshaftPieces::initialize()
{
	mineshaftChestContents.push_back(LordNew WeightedRandomChestContent(ITEM_ID_INGOT_IRON, 0, 1, 5, 10));
	mineshaftChestContents.push_back(LordNew WeightedRandomChestContent(ITEM_ID_INGOT_GOLD, 0, 1, 3, 5));
	mineshaftChestContents.push_back(LordNew WeightedRandomChestContent(ITEM_ID_REDSTONE, 0, 4, 9, 5));
	mineshaftChestContents.push_back(LordNew WeightedRandomChestContent(ITEM_ID_DYE_POWER, 4, 4, 9, 5));
	mineshaftChestContents.push_back(LordNew WeightedRandomChestContent(ITEM_ID_DIAMOND, 0, 1, 2, 3));
	mineshaftChestContents.push_back(LordNew WeightedRandomChestContent(ITEM_ID_COAL, 0, 3, 8, 10));
	mineshaftChestContents.push_back(LordNew WeightedRandomChestContent(ITEM_ID_BREAD, 0, 1, 3, 15));
	mineshaftChestContents.push_back(LordNew WeightedRandomChestContent(ITEM_ID_PICK_AXE_IRON, 0, 1, 1, 1));
	mineshaftChestContents.push_back(LordNew WeightedRandomChestContent(BLOCK_ID_RAIL, 0, 4, 8, 1));
	mineshaftChestContents.push_back(LordNew WeightedRandomChestContent(ITEM_ID_MELON_SEEDS, 0, 2, 4, 10));
	mineshaftChestContents.push_back(LordNew WeightedRandomChestContent(ITEM_ID_PUMPKIN_SEEDS, 0, 2, 4, 10));
	mineshaftChestContents.push_back(LordNew WeightedRandomChestContent(ITEM_ID_SADDLE, 0, 1, 1, 3));
	mineshaftChestContents.push_back(LordNew WeightedRandomChestContent(ITEM_ID_HORSE_ARMOR_METAL, 0, 1, 1, 1));
}

void StructureMineshaftPieces::uninitialize()
{
	for (WRCCArr::iterator it = mineshaftChestContents.begin(); it != mineshaftChestContents.end(); ++it)
		LordDelete(*it);
	mineshaftChestContents.clear();
}

StructureComponent* StructureMineshaftPieces::getRandomComponent(SCList& scLst, Random& rand, int par2, int par3, int par4, int par5, int par6)
{
	int probability = rand.nextInt(100);
	StructureBB aabb;

	if (probability >= 80)
	{
		aabb = ComponentMineshaftCross::findValidPlacement(scLst, rand, par2, par3, par4, par5);

		if (!aabb.isInvalid())
		{
			return LordNew ComponentMineshaftCross(par6, rand, aabb, par5);
		}
	}
	else if (probability >= 70)
	{
		aabb = ComponentMineshaftStairs::findValidPlacement(scLst, rand, par2, par3, par4, par5);

		if (!aabb.isInvalid())
		{
			return LordNew ComponentMineshaftStairs(par6, rand, aabb, par5);
		}
	}
	else
	{
		aabb = ComponentMineshaftCorridor::findValidPlacement(scLst, rand, par2, par3, par4, par5);

		if (!aabb.isInvalid())
		{
			return LordNew ComponentMineshaftCorridor(par6, rand, aabb, par5);
		}
	}

	return NULL;
}

StructureComponent* StructureMineshaftPieces::getNextMineShaftComponent(StructureComponent* pComponent, SCList& scLst, Random& rand, int par3, int par4, int par5, int par6, int par7)
{
	if (par7 > 8)
	{
		return NULL;
	}
	else if (Math::Abs(par3 - pComponent->getBoundingBox().m_minX) <= 80 &&
		Math::Abs(par5 - pComponent->getBoundingBox().m_minZ) <= 80)
	{
		StructureComponent* pResult = getRandomComponent(scLst, rand, par3, par4, par5, par6, par7 + 1);

		if (pResult != NULL)
		{
			scLst.push_back(pResult);
			pResult->buildComponent(pComponent, scLst, rand);
		}

		return pResult;
	}
	else
	{
		return NULL;
	}
}

StructureComponent* StructureMineshaftPieces::getNextComponent(StructureComponent* pComponent, SCList& scLst, Random& rand, int par3, int par4, int par5, int par6, int par7)
{
	return getNextMineShaftComponent(pComponent, scLst, rand, par3, par4, par5, par6, par7);
}


SNBPWArr StructureNetherBridgePieces::primaryComponents;// new StructureNetherBridgePieceWeight[]{ new StructureNetherBridgePieceWeight(ComponentNetherBridgeStraight.class, 30, 0, true), new StructureNetherBridgePieceWeight(ComponentNetherBridgeCrossing3.class, 10, 4), new StructureNetherBridgePieceWeight(ComponentNetherBridgeCrossing.class, 10, 4), new StructureNetherBridgePieceWeight(ComponentNetherBridgeStairs.class, 10, 3), new StructureNetherBridgePieceWeight(ComponentNetherBridgeThrone.class, 5, 2), new StructureNetherBridgePieceWeight(ComponentNetherBridgeEntrance.class, 5, 1) };
SNBPWArr StructureNetherBridgePieces::secondaryComponents;// new StructureNetherBridgePieceWeight[]{ new StructureNetherBridgePieceWeight(ComponentNetherBridgeCorridor5.class, 25, 0, true), new StructureNetherBridgePieceWeight(ComponentNetherBridgeCrossing2.class, 15, 5), new StructureNetherBridgePieceWeight(ComponentNetherBridgeCorridor2.class, 5, 10), new StructureNetherBridgePieceWeight(ComponentNetherBridgeCorridor.class, 5, 10), new StructureNetherBridgePieceWeight(ComponentNetherBridgeCorridor3.class, 10, 3, true), new StructureNetherBridgePieceWeight(ComponentNetherBridgeCorridor4.class, 7, 2), new StructureNetherBridgePieceWeight(ComponentNetherBridgeNetherStalkRoom.class, 5, 2) };

void StructureNetherBridgePieces::initialize()
{
	primaryComponents.push_back(LordNew StructureNetherBridgePieceWeight(COMPONENT_BRIDGE_TYPE_STRAIGHT, 30, 0, true));
	primaryComponents.push_back(LordNew StructureNetherBridgePieceWeight(COMPONENT_BRIDGE_TYPE_CROSSING3, 10, 4));
	primaryComponents.push_back(LordNew StructureNetherBridgePieceWeight(COMPONENT_BRIDGE_TYPE_CROSSING, 10, 4));
	primaryComponents.push_back(LordNew StructureNetherBridgePieceWeight(COMPONENT_BRIDGE_TYPE_STAIRS, 10, 3));
	primaryComponents.push_back(LordNew StructureNetherBridgePieceWeight(COMPONENT_BRIDGE_TYPE_THRONE, 5, 2));
	primaryComponents.push_back(LordNew StructureNetherBridgePieceWeight(COMPONENT_BRIDGE_TYPE_ENTRANCE, 5, 1));

	secondaryComponents.push_back(LordNew StructureNetherBridgePieceWeight(COMPONENT_BRIDGE_TYPE_CORRIDOR5, 25, 0, true));
	secondaryComponents.push_back(LordNew StructureNetherBridgePieceWeight(COMPONENT_BRIDGE_TYPE_CROSSING2, 15, 5));
	secondaryComponents.push_back(LordNew StructureNetherBridgePieceWeight(COMPONENT_BRIDGE_TYPE_CORRIDOR2, 5, 10));
	secondaryComponents.push_back(LordNew StructureNetherBridgePieceWeight(COMPONENT_BRIDGE_TYPE_CORRIDOR, 5, 10));
	secondaryComponents.push_back(LordNew StructureNetherBridgePieceWeight(COMPONENT_BRIDGE_TYPE_CORRIDOR3, 10, 3, true));
	secondaryComponents.push_back(LordNew StructureNetherBridgePieceWeight(COMPONENT_BRIDGE_TYPE_CORRIDOR4, 7, 2));
	secondaryComponents.push_back(LordNew StructureNetherBridgePieceWeight(COMPONENT_BRIDGE_TYPE_NETHESTALKROOM, 5, 2));
}

void StructureNetherBridgePieces::uninitialize()
{
	for (SNBPWArr::iterator it = primaryComponents.begin(); it != primaryComponents.end(); ++it)
		LordDelete(*it);
	primaryComponents.clear();

	for (SNBPWArr::iterator it = secondaryComponents.begin(); it != secondaryComponents.end(); ++it)
		LordDelete(*it);
	secondaryComponents.clear();
}

ComponentNetherBridgePiece* StructureNetherBridgePieces::createNextComponentRandom(StructureNetherBridgePieceWeight* weight, 
	SCList& par1List, Random& rand, int par3, int par4, int par5, int par6, int par7)
{
	int type = weight->m_type;
	ComponentNetherBridgePiece* pResult = NULL;

	switch (type)
	{
	case COMPONENT_BRIDGE_TYPE_STRAIGHT:
		pResult = ComponentNetherBridgeStraight::createValidComponent(par1List, rand, par3, par4, par5, par6, par7); break;
	case COMPONENT_BRIDGE_TYPE_STAIRS:
		pResult = ComponentNetherBridgeStairs::createValidComponent(par1List, rand, par3, par4, par5, par6, par7); break;
	case COMPONENT_BRIDGE_TYPE_THRONE:
		pResult = ComponentNetherBridgeThrone::createValidComponent(par1List, rand, par3, par4, par5, par6, par7); break;
	case COMPONENT_BRIDGE_TYPE_ENTRANCE:
		pResult = ComponentNetherBridgeEntrance::createValidComponent(par1List, rand, par3, par4, par5, par6, par7); break;
	case COMPONENT_BRIDGE_TYPE_CROSSING:
		pResult = ComponentNetherBridgeCrossing::createValidComponent(par1List, rand, par3, par4, par5, par6, par7); break;
	case COMPONENT_BRIDGE_TYPE_CROSSING2:
		pResult = ComponentNetherBridgeCrossing2::createValidComponent(par1List, rand, par3, par4, par5, par6, par7); break;
	case COMPONENT_BRIDGE_TYPE_CROSSING3:
		pResult = ComponentNetherBridgeCrossing3::createValidComponent(par1List, rand, par3, par4, par5, par6, par7); break;
	case COMPONENT_BRIDGE_TYPE_CORRIDOR:
		pResult = ComponentNetherBridgeCorridor::createValidComponent(par1List, rand, par3, par4, par5, par6, par7); break;
	case COMPONENT_BRIDGE_TYPE_CORRIDOR2:
		pResult = ComponentNetherBridgeCorridor2::createValidComponent(par1List, rand, par3, par4, par5, par6, par7); break;
	case COMPONENT_BRIDGE_TYPE_CORRIDOR3:
		pResult = ComponentNetherBridgeCorridor3::createValidComponent(par1List, rand, par3, par4, par5, par6, par7); break;
	case COMPONENT_BRIDGE_TYPE_CORRIDOR4:
		pResult = ComponentNetherBridgeCorridor4::createValidComponent(par1List, rand, par3, par4, par5, par6, par7); break;
	case COMPONENT_BRIDGE_TYPE_CORRIDOR5:
		pResult = ComponentNetherBridgeCorridor5::createValidComponent(par1List, rand, par3, par4, par5, par6, par7); break;
	case COMPONENT_BRIDGE_TYPE_NETHESTALKROOM:
		pResult = ComponentNetherBridgeNetherStalkRoom::createValidComponent(par1List, rand, par3, par4, par5, par6, par7); break;

	default: break;
	}
	
	return pResult;
}

ComponentNetherBridgePiece* StructureNetherBridgePieces::createNextComponent(StructureNetherBridgePieceWeight* weight, SCList& par1List, Random& rand,
	int par3, int par4, int par5, int par6, int par7)
{
	return createNextComponentRandom(weight, par1List, rand, par3, par4, par5, par6, par7);
}

}
