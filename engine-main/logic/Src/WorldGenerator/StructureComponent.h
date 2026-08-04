/********************************************************************
filename: 	StructureComponent.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-11-7
*********************************************************************/
#ifndef __STRUCTURE_COMPONENT_HEADER__
#define __STRUCTURE_COMPONENT_HEADER__

#include "BM_Container_def.h"

#include "StructureNetherBridgePieceWeight.h"
#include "WeightedRandomItem.h"

#include "Util/StructureBB.h"
#include "Util/Random.h"
#include "Util/ChunkPosition.h"

namespace BLOCKMAN
{

class World;
class StructurePieceBlockSelector;
class StructureComponent;
class WeightedRandomChestContent;
class StructureNetherBridgePieceWeight;
class ComponentNetherBridgeStartPiece;
class ComponentVillageStartPiece;

enum COMPONENT_BRIDGE_TYPE
{
	COMPONENT_BRIDGE_TYPE_INVALID = -1,

	COMPONENT_BRIDGE_TYPE_STRAIGHT,
	COMPONENT_BRIDGE_TYPE_STAIRS,
	COMPONENT_BRIDGE_TYPE_THRONE,
	COMPONENT_BRIDGE_TYPE_ENTRANCE,
	COMPONENT_BRIDGE_TYPE_CROSSING,
	COMPONENT_BRIDGE_TYPE_CROSSING2,
	COMPONENT_BRIDGE_TYPE_CROSSING3,
	COMPONENT_BRIDGE_TYPE_CORRIDOR,
	COMPONENT_BRIDGE_TYPE_CORRIDOR2,
	COMPONENT_BRIDGE_TYPE_CORRIDOR3,
	COMPONENT_BRIDGE_TYPE_CORRIDOR4,
	COMPONENT_BRIDGE_TYPE_CORRIDOR5,
	COMPONENT_BRIDGE_TYPE_NETHESTALKROOM,

	COMPONENT_BRIDGE_TYPE_COUNT,
};

class StructureComponent : public ObjectAlloc
{

public:
	/**
	* second Part of Structure generating, this for example places Spiderwebs, Mob Spawners, it closes Mineshafts at
	* the end, it adds Fences...
	*/
	virtual bool addComponentParts(World* pWorld, Random& rand, const StructureBB& aabb) = 0;

	const StructureBB& getBoundingBox() const { return m_boundingBox; }
	StructureBB& getBoundingBox() { return m_boundingBox; }
	int getComponentType() const { return m_componentType; }

	/** Discover if bounding box can fit within the current bounding box object. */
	static StructureComponent* findIntersecting(SCList& scList, const StructureBB& aabb);

	ChunkPosition getCenter() const;

	/** checks the entire StructureBoundingBox for Liquids */
	bool isLiquidInStructureBoundingBox(World* pWorld, const StructureBB& par2StructureBoundingBox) const;

	int getXWithOffset(int par1, int par2) const;
	
	int getYWithOffset(int par1) const;

	int getZWithOffset(int par1, int par2) const;

	/** Returns the direction-shifted metadata for blocks that require orientation, e.g. doors, stairs, ladders. Parameters: block ID, original metadata */
	int getMetadataWithOffset(int blockID, int par2) const;

	/** current Position depends on currently set Coordinates mode, is computed here */
	void placeBlockAtCurrentPosition(World* pWorld, int blockID, int metadata, int x, int y, int z, const StructureBB& aabb);

	int getBlockIdAtCurrentPosition(World* pWorld, int x, int y, int z, const StructureBB& aabb);

	/** arguments: (World worldObj, StructureBoundingBox structBB, int minX, int minY, int minZ, int maxX, int maxY, int maxZ) */
	void fillWithAir(World* pWorld, const StructureBB& aabb, int minX, int minY, int minZ, int maxX, int maxY, int maxZ);
	
	/**	arguments: (pWorld, aabb, minX, minY, minZ, maxX, maxY, maxZ, placeID, replaceID, alwaysreplace) */
	void fillWithBlocks(World* pWorld, const StructureBB& aabb, int minX, int minY, int minZ, int maxX, int maxY, int maxZ, int placeID, int replaceID, bool alwaysreplace);

	/** arguments: */
	void fillWithMetadataBlocks(World* pWorld, const StructureBB& aabb, int minX, int minY, int minZ, int maxX, int maxY, int maxZ,
		int placeID, int placeMetadata, int replaceID, int replaceMetadata, bool alwaysreplace);

	/** arguments: */
	void fillWithRandomizedBlocks(World* pWorld, const StructureBB& aabb, int minX, int minY, int minZ, int maxX, int maxY, int maxZ,
		bool alwaysreplace, Random& rand, StructurePieceBlockSelector* selector);
	
	/** arguments: */
	void randomlyFillWithBlocks(World* pWorld, const StructureBB& aabb, Random& rand, float randLimit,
		int minX, int minY, int minZ, int maxX, int maxY, int maxZ, int olaceID, int replaceID, bool alwaysreplace);

	/** Randomly decides if placing or not. Used for Decoration such as Torches and Spiderwebs */
	void randomlyPlaceBlock(World* pWorld, const StructureBB& aabb, Random& rand, float randLimit, int x, int y, int z, int blockID, int metadata);

	/** arguments: */
	void randomlyRareFillWithBlocks(World* pWorld, const StructureBB& aabb, int minX, int minY, int minZ, int maxX, int maxY, int maxZ, int placeID, bool alwaysreplace);
	
	/** Deletes all continuous blocks from selected position upwards. Stops at hitting air. */
	void clearCurrentPositionBlocksUpwards(World* pWorld, int x, int y, int z, const StructureBB& aabb);

	/** Overwrites air and liquids from selected position downwards, stops at hitting anything else. */
	void fillCurrentPositionBlocksDownwards(World* pWorld, int blockID, int metadata, int x, int y, int z, const StructureBB& aabb);

	void placeDoorAtCurrentPosition(World* par1World, const StructureBB& aabb, Random& rand, int x, int y, int z, int par7);
	
	/** Initiates construction of the Structure Component picked, at the current Location of StructGen */
	void buildComponent(StructureComponent* structureComponent, SCList& par2List, Random& par3Random) {}

	bool generateStructureChestContents(World* pWorld, const StructureBB& aabb, Random& rand, int x, int y, int z, WRCCArr& arr, int par8);

protected:
	StructureBB m_boundingBox;

	/** switches the Coordinate System base off the Bounding Box */
	int m_coordBaseMode = 0;

	/** The type ID of this component. */
	int m_componentType = 0;

	StructureComponent(int type);
};


class ComponentMineshaftRoom : public StructureComponent
{
public:
	ComponentMineshaftRoom(int type, Random& rand, int x, int z);

	/** Initiates construction of the Structure Component picked, at the current Location of StructGen */
	void buildComponent(StructureComponent* pComponent, SCList& par2List, Random& par3Random);
	
	/** second Part of Structure generating, this for example places Spiderwebs, Mob Spawners, it closes Mineshafts at the end, it adds Fences... */
	virtual bool addComponentParts(World* var1, Random& var2, const StructureBB& var3);

protected:
	/** List of other Mineshaft components linked to this room. */
	list<StructureBB>::type roomsLinkedToTheRoom;
};

class ComponentMineshaftCross : public StructureComponent
{
public:
	ComponentMineshaftCross(int type, Random rand, const StructureBB& aabb, int dir);

	static StructureBB findValidPlacement(SCList& componentLst, Random& rand, int x, int y, int z, int dir);

	/** Initiates construction of the Structure Component picked, at the current Location of StructGen */
	void buildComponent(StructureComponent* components, SCList& lst, Random& rand);
	
	/** second Part of Structure generating, this for example places Spiderwebs, Mob Spawners, it closes Mineshafts at the end, it adds Fences... */
	virtual bool addComponentParts(World* par1World, Random& rand, const StructureBB&  aabb);

protected:
	int corridorDirection = 0;
	bool isMultipleFloors = false;
};

class ComponentMineshaftStairs : public StructureComponent
{
public:
	ComponentMineshaftStairs(int type, Random& rand, const StructureBB& aabb, int mode);

	/** Trys to find a valid place to put this component. */
	static StructureBB findValidPlacement(SCList& lst, Random& rand, int x, int y, int z, int dir);

	/** Initiates construction of the Structure Component picked, at the current Location of StructGen */
	void buildComponent(StructureComponent* components, SCList& lst, Random& rand);

	/** second Part of Structure generating, this for example places Spiderwebs, Mob Spawners, it closes Mineshafts at the end, it adds Fences... */
	virtual bool addComponentParts(World* pWorld, Random& rand, const StructureBB& aabb);

};

class ComponentMineshaftCorridor : public StructureComponent
{

public:
	ComponentMineshaftCorridor(int type, Random& rand, const StructureBB& aabb, int mode);

	static StructureBB findValidPlacement(SCList& lst, Random& rand, int x, int y, int z, int dir);

	/** Initiates construction of the Structure Component picked, at the current Location of StructGen */
	void buildComponent(StructureComponent* components, SCList& lst, Random& rand);

	/** Used to generate chests with items in it. ex: Temple Chests, Village Blacksmith Chests, Mineshaft Chests. */
	bool generateStructureChestContents(World* pWorld, const StructureBB& aabb, Random& rand, int x, int y, int z, WRCCArr& arr, int par8);

	/** second Part of Structure generating, this for example places Spiderwebs, Mob Spawners, it closes Mineshafts at the end, it adds Fences... */
	virtual bool addComponentParts(World* par1World, Random& rand, const StructureBB& aabb);

protected:
	bool hasRails = false;
	bool hasSpiders = false;
	bool spawnerPlaced = false;

	/** A count of the different sections of this mine. The space between ceiling supports. */
	int sectionCount = 0;
};

class ComponentNetherBridgePiece : public StructureComponent
{

protected:
	static WRCCArr conttes; // = new WeightedRandomChestContent[]{ new WeightedRandomChestContent(Item.diamond.itemID, 0, 1, 3, 5), new WeightedRandomChestContent(Item.ingotIron.itemID, 0, 1, 5, 5), new WeightedRandomChestContent(Item.ingotGold.itemID, 0, 1, 3, 15), new WeightedRandomChestContent(Item.swordGold.itemID, 0, 1, 1, 5), new WeightedRandomChestContent(Item.plateGold.itemID, 0, 1, 1, 5), new WeightedRandomChestContent(Item.flintAndSteel.itemID, 0, 1, 1, 5), new WeightedRandomChestContent(Item.netherStalkSeeds.itemID, 0, 3, 7, 5), new WeightedRandomChestContent(Item.saddle.itemID, 0, 1, 1, 10), new WeightedRandomChestContent(Item.field_111216_cf.itemID, 0, 1, 1, 8), new WeightedRandomChestContent(Item.field_111215_ce.itemID, 0, 1, 1, 5), new WeightedRandomChestContent(Item.field_111213_cg.itemID, 0, 1, 1, 3) };

	static void initialize();

	ComponentNetherBridgePiece(int type);

	int getTotalWeight(const SNBPWList& par1List);

	ComponentNetherBridgePiece* getNextComponent(ComponentNetherBridgeStartPiece* pieces, SNBPWList& lst1, SCList& lst2, Random& rand, 
		int par5, int par6, int par7, int par8, int par9);
	
	/** Finds a random component to tack on to the bridge. Or builds the end. */
	StructureComponent* getNextComponent(ComponentNetherBridgeStartPiece* pieces, SCList& lst, Random& rand, int par4, int par5, int par6, int par7, int par8, bool par9);

	/** Gets the next component in any cardinal direction */
	StructureComponent* getNextComponentNormal(ComponentNetherBridgeStartPiece* pieces, SCList& lst, Random& rand, int par4, int par5, bool par6);

	/** Gets the next component in the +/- X direction */
	StructureComponent* getNextComponentX(ComponentNetherBridgeStartPiece* pieces, SCList& lst, Random& rand, int par4, int par5, bool par6);

	/** Gets the next component in the +/- Z direction */
	StructureComponent* getNextComponentZ(ComponentNetherBridgeStartPiece* pieces, SCList& lst, Random& rand, int par4, int par5, bool par6);

	/** Checks if the bounding box's minY is > 10 */
	static bool isAboveGround(const StructureBB& aabb) { return (!aabb.isInvalid()) && aabb.m_minY > 10; }
};


class ComponentNetherBridgeStraight : public ComponentNetherBridgePiece
{
public:
	ComponentNetherBridgeStraight(int type, Random& rand, const StructureBB& aabb, int mode);
	
	/** Initiates construction of the Structure Component picked, at the current Location of StructGen */
	void buildComponent(StructureComponent* pComponent, SCList& lst, Random& rand);

	/** Creates and returns a new component piece. Or null if it could not find enough room to place it. */
	static ComponentNetherBridgeStraight* createValidComponent(SCList& lst, Random& rand, int par2, int par3, int par4, int par5, int par6);

	/** second Part of Structure generating, this for example places Spiderwebs, Mob Spawners, it closes Mineshafts at the end, it adds Fences... */
	bool addComponentParts(World* pWorld, Random& rand, const StructureBB& aabb);
};

class ComponentNetherBridgeCrossing3 : public ComponentNetherBridgePiece
{
public:
	ComponentNetherBridgeCrossing3(int type, Random& rand, const StructureBB& aabb, int mode);
	ComponentNetherBridgeCrossing3(Random& rand, int x, int z);

	/** Initiates construction of the Structure Component picked, at the current Location of StructGen */
	void buildComponent(StructureComponent* component, SCList& lst, Random& rand);

	/** Creates and returns a new component piece. Or null if it could not find enough room to place it. */
	static ComponentNetherBridgeCrossing3* createValidComponent(SCList& lst, Random& rand, int par2, int par3, int par4, int par5, int par6);

	/** second Part of Structure generating, this for example places Spiderwebs, Mob Spawners, it closes Mineshafts at the end, it adds Fences... */
	bool addComponentParts(World* pWorld, Random& rand, const StructureBB& aabb);
};

class ComponentNetherBridgeCrossing : public ComponentNetherBridgePiece
{
public:
	ComponentNetherBridgeCrossing(int type, Random& rand, const StructureBB& aabb, int mode);

	/** Initiates construction of the Structure Component picked, at the current Location of StructGen */
	void buildComponent(StructureComponent* component, SCList& lst, Random& rand);

	/** Creates and returns a new component piece. Or null if it could not find enough room to place it. */
	static ComponentNetherBridgeCrossing* createValidComponent(SCList& lst, Random& rand, int par2, int par3, int par4, int par5, int type);

	/** second Part of Structure generating, this for example places Spiderwebs, Mob Spawners, it closes Mineshafts at the end, it adds Fences... */
	bool addComponentParts(World* pWorld, Random& rand, const StructureBB& aabb);
};

class ComponentNetherBridgeStairs : public ComponentNetherBridgePiece
{
public:
	ComponentNetherBridgeStairs(int type, Random& rand, const StructureBB& aabb, int mode);

	/** Initiates construction of the Structure Component picked, at the current Location of StructGen */
	void buildComponent(StructureComponent* component, SCList& lst, Random& rand);

	/** Creates and returns a new component piece. Or null if it could not find enough room to place it. */
	static ComponentNetherBridgeStairs* createValidComponent(SCList& lst, Random& rand, int par2, int par3, int par4, int par5, int type);

	/** second Part of Structure generating, this for example places Spiderwebs, Mob Spawners, it closes Mineshafts at the end, it adds Fences... */
	bool addComponentParts(World* pWorld, Random& rand, const StructureBB&  aabb);
};

class ComponentNetherBridgeThrone : public ComponentNetherBridgePiece
{
protected:
	bool hasSpawner = false;

public:
	ComponentNetherBridgeThrone(int type, Random& rand, const StructureBB& aabb, int mode);

	/** Creates and returns a new component piece. Or null if it could not find enough room to place it. */
	static ComponentNetherBridgeThrone* createValidComponent(SCList& lst, Random& rand, int par2, int par3, int par4, int par5, int type);

	/** second Part of Structure generating, this for example places Spiderwebs, Mob Spawners, it closes Mineshafts at the end, it adds Fences... */
	bool addComponentParts(World* pWorld, Random& rand, const StructureBB&  aabb);
};

class ComponentNetherBridgeEntrance : public ComponentNetherBridgePiece
{
public:
	ComponentNetherBridgeEntrance(int type, Random& rand, const StructureBB& aabb, int mode);
	
	/** Initiates construction of the Structure Component picked, at the current Location of StructGen */
	void buildComponent(StructureComponent* component, SCList& lst, Random& rand);

	/** Creates and returns a new component piece. Or null if it could not find enough room to place it.*/
	static ComponentNetherBridgeEntrance* createValidComponent(SCList& lst, Random& rand, int par2, int par3, int par4, int par5, int type);

	/** second Part of Structure generating, this for example places Spiderwebs, Mob Spawners, it closes Mineshafts at the end, it adds Fences... */
	bool addComponentParts(World* pWorld, Random& rand, const StructureBB&  aabb);
};

class ComponentNetherBridgeCorridor5 : public ComponentNetherBridgePiece
{
public:
	ComponentNetherBridgeCorridor5(int type, Random& rand, const StructureBB& aabb, int mode);

	/** Initiates construction of the Structure Component picked, at the current Location of StructGen */
	void buildComponent(StructureComponent* component, SCList& lst, Random& rand);

	/** Creates and returns a new component piece. Or null if it could not find enough room to place it. */
	static ComponentNetherBridgeCorridor5* createValidComponent(SCList& lst, Random& rand, int x, int y, int z, int mode, int type);

	/** second Part of Structure generating, this for example places Spiderwebs, Mob Spawners, it closes Mineshafts at the end, it adds Fences... */
	bool addComponentParts(World* pWorld, Random& rand, const StructureBB& aabb);
};

class ComponentNetherBridgeCorridor2 : public ComponentNetherBridgePiece
{
public:
	ComponentNetherBridgeCorridor2(int type, Random& rand, const StructureBB& aabb, int mode);

	/** Initiates construction of the Structure Component picked, at the current Location of StructGen */
	void buildComponent(StructureComponent* component, SCList& lst, Random& rand);

	/** Creates and returns a new component piece. Or null if it could not find enough room to place it. */
	static ComponentNetherBridgeCorridor2* createValidComponent(SCList& lst, Random& rand, int x, int y, int z, int mode, int type);

	/** second Part of Structure generating, this for example places Spiderwebs, Mob Spawners, it closes Mineshafts at the end, it adds Fences... */
	bool addComponentParts(World* pWorld, Random& rand, const StructureBB& aabb);

protected:
	bool field_111020_b = false;
};

class ComponentNetherBridgeCorridor : public ComponentNetherBridgePiece
{
protected:
	bool field_111021_b = false;

public:
	ComponentNetherBridgeCorridor(int type, Random& rand, const StructureBB& aabb, int mode);

	/** Initiates construction of the Structure Component picked, at the current Location of StructGen */
	void buildComponent(StructureComponent* component, SCList& lst, Random& rand);

	/** Creates and returns a new component piece. Or null if it could not find enough room to place it.	*/
	static ComponentNetherBridgeCorridor* createValidComponent(SCList& lst, Random& rand, int x, int y, int z, int mode, int type);

	/** second Part of Structure generating, this for example places Spiderwebs, Mob Spawners, it closes Mineshafts at the end, it adds Fences... */
	bool addComponentParts(World* pWorld, Random& rand, const StructureBB& aabb);
};

class ComponentNetherBridgeCorridor3 : public ComponentNetherBridgePiece
{
public:
	ComponentNetherBridgeCorridor3(int type, Random& rand, const StructureBB& aabb, int mode);

	/** Initiates construction of the Structure Component picked, at the current Location of StructGen */
	void buildComponent(StructureComponent* component, SCList& lst, Random& rand);

	/** Creates and returns a new component piece. Or null if it could not find enough room to place it. */
	static ComponentNetherBridgeCorridor3* createValidComponent(SCList& lst, Random& rand, int x, int y, int z, int mode, int type);

	/** second Part of Structure generating, this for example places Spiderwebs, Mob Spawners, it closes Mineshafts at the end, it adds Fences... */
	bool addComponentParts(World* pWorld, Random& rand, const StructureBB& aabb);
};

class ComponentNetherBridgeCorridor4 : public ComponentNetherBridgePiece
{
public:
	ComponentNetherBridgeCorridor4(int type, Random& rand, const StructureBB& aabb, int mode);

	/** Initiates construction of the Structure Component picked, at the current Location of StructGen */
	void buildComponent(StructureComponent* component, SCList& lst, Random& rand);

	/** Creates and returns a new component piece. Or null if it could not find enough room to place it. */
	static ComponentNetherBridgeCorridor4* createValidComponent(SCList& lst, Random& rand, int x, int y, int z, int mode, int type);

	/** second Part of Structure generating, this for example places Spiderwebs, Mob Spawners, it closes Mineshafts at the end, it adds Fences... */
	bool addComponentParts(World* pWorld, Random& rand, const StructureBB& aabb);
};


class ComponentNetherBridgeCrossing2 : public ComponentNetherBridgePiece
{
public:
	ComponentNetherBridgeCrossing2(int type, Random& rand, const StructureBB& aabb, int mode);

	/** Initiates construction of the Structure Component picked, at the current Location of StructGen */
	void buildComponent(StructureComponent* component, SCList& lst, Random& rand);

	/** Creates and returns a new component piece. Or null if it could not find enough room to place it. */
	static ComponentNetherBridgeCrossing2* createValidComponent(SCList& lst, Random& rand, int x, int y, int z, int mode, int type);

	/** second Part of Structure generating, this for example places Spiderwebs, Mob Spawners, it closes Mineshafts at the end, it adds Fences... */
	bool addComponentParts(World* pWorld, Random& rand, const StructureBB& aabb);
};

class ComponentNetherBridgeNetherStalkRoom : public ComponentNetherBridgePiece
{
public:
	ComponentNetherBridgeNetherStalkRoom(int type, Random& rand, const StructureBB& aabb, int mode);

	/** Initiates construction of the Structure Component picked, at the current Location of StructGen */
	void buildComponent(StructureComponent* component, SCList& lst, Random& rand);

	/** Creates and returns a new component piece. Or null if it could not find enough room to place it. */
	static ComponentNetherBridgeNetherStalkRoom* createValidComponent(SCList& lst, Random& rand, int x, int y, int z, int mode, int type);

	/** second Part of Structure generating, this for example places Spiderwebs, Mob Spawners, it closes Mineshafts at the end, it adds Fences... */
	bool addComponentParts(World* pWorld, Random& rand, const StructureBB& aabb);
};

class ComponentNetherBridgeStartPiece : public ComponentNetherBridgeCrossing3
{
	/** Instance of StructureNetherBridgePieceWeight. */
public:
	StructureNetherBridgePieceWeight* theNetherBridgePieceWeight;

	/** Contains the list of valid piece weights for the set of nether bridge structure pieces.	*/
	SNBPWList primaryWeights;

	/** Contains the list of valid piece weights for the secondary set of nether bridge structure pieces.	*/
	SNBPWList secondaryWeights;

	SCList contextLst;

	ComponentNetherBridgeStartPiece(Random& par1Random, int par2, int par3);
};

}

#endif
