/********************************************************************
filename: 	StructureStart.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-11-7
*********************************************************************/
#ifndef __STRUCTURE_START_HEADER__
#define __STRUCTURE_START_HEADER__

#include "Util/StructureBB.h"
#include "Util/Random.h"

namespace BLOCKMAN
{

class StructureComponent;
class World;

class StructureStart : public ObjectAlloc
{
public:
	typedef list<StructureComponent*>::type ComponentList;

public:
	const StructureBB& getBoundingBox() const { return m_boundingBox; }

	ComponentList& getComponents() { return m_components; }

	/** Keeps iterating Structure Pieces and spawning them until the checks tell it to stop */
	void generateStructure(World* pWorld, Random& rand, const StructureBB& aabb);

	/** Calculates total bounding box based on components' bounding boxes and saves it to boundingBox */
	void updateBoundingBox();

	/** offsets the structure Bounding Boxes up to a certain height, typically 63 - 10 */
	void markAvailableHeight(World* pWorld, Random& rand, int offsetY);

	void setRandomHeight(World* pWorld, Random& rand, int par3, int par4);

	/** currently only defined for Villages, returns true if Village has more than 2 non-road components */
	bool isSizeableStructure() { return true; }

protected:
	/** List of all StructureComponents that are part of this structure */
	ComponentList m_components;
	StructureBB m_boundingBox;
};


class StructureMineshaftStart : public StructureStart
{
public:
	StructureMineshaftStart(World* pWorld, Random& rand, int x, int z);
};


class StructureNetherBridgeStart : public StructureStart
{
public:
	StructureNetherBridgeStart(World* pWorld, Random& rand, int x, int z);
};


}

#endif
