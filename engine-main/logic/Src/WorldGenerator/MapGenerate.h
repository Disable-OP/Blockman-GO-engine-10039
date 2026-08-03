/********************************************************************
filename: 	MapGenerate.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-11-5
*********************************************************************/
#ifndef __MAP_GENERATE_HEADER__
#define __MAP_GENERATE_HEADER__

#include "BM_Container_def.h"

#include "Util/Random.h"
#include "Util/ChunkPosition.h"

namespace BLOCKMAN
{
class World;
class StructureStart;

class MapGenBase : public ObjectAlloc
{
public:
	MapGenBase();
	~MapGenBase();

	void generate(World* pWorld, int x, int z, i8* pArray);

	/**
	* Recursively called by generate() (generate) and optionally by itself.
	*/
	virtual void recursiveGenerate(World* pWorld, int x, int z, int centerX, int centerZ, i8* pArray) {}

protected:
	/** The number of Chunks to gen-check in any given direction. */
	int m_range = 0;

	/** The RNG used by the MapGen classes. */
	Random m_rand;

	/** This world Ptr. */
	World* m_pWorld = nullptr;
};

class MapGenCaves : public MapGenBase
{
public:
	/** Generates a larger initial cave node than usual. Called 25% of the time. */
	void generateLargeCaveNode(i64 seed, int centerX, int centerZ, i8* pArr, double x, double y, double z)
	{
		generateCaveNode(seed, centerX, centerZ, pArr, x, y, z, 1.0F + m_rand.nextFloat() * 6.0F, 0.0F, 0.0F, -1, -1, 0.5);
	}

	/** Generates a node in the current cave system recursion tree.	*/
	void generateCaveNode(i64 seed, int centerX, int centerZ, i8* pArr, double x, double y, double z, float par12, float radian1, float radian2, int begin, int end, double scaleY);

	/** Recursively called by generate() (generate) and optionally by itself.*/
	void recursiveGenerate(World* pWorld, int x, int z, int centerX, int centerZ, i8* pArr);
};

class MapGenCavesHell : public MapGenBase
{
public:
	/** Generates a larger initial cave node than usual. Called 25% of the time.*/
	void generateLargeCaveNode(i64 seed, int centerX, int centerZ, i8* pArr, double x, double y, double z)
	{
		generateCaveNode(seed, centerX, centerZ, pArr, x, y, z, 1.0F + m_rand.nextFloat() * 6.0F, 0.0F, 0.0F, -1, -1, 0.5);
	}

	/** Generates a node in the current cave system recursion tree.	*/
	void generateCaveNode(i64 seed, int centerX, int centerZ, i8* pArr, double x, double y, double z, float par12, float par13, float par14, int begin, int end, double par17);

	/** Recursively called by generate() (generate) and optionally by itself. */
	void recursiveGenerate(World* pWorld, int x, int z, int centerX, int centerZ, i8* pArr);
};


class MapGenRavine : public MapGenBase
{
public:
	float field_75046_d[1024];

	void generateRavine(i64 seed, int centerX, int centerZ, i8* arr, double x, double y, double z, float par12, float par13, float par14, int begin, int end, double par17);

	/** Recursively called by generate() (generate) and optionally by itself.*/
	void recursiveGenerate(World* pWorld, int x, int z, int centerX, int centerZ, i8* pArr);
};


class MapGenStructure : public MapGenBase
{
public:
	MapGenStructure();

	/** Recursively called by generate() (generate) and optionally by itself. */
	void recursiveGenerate(World* pWorld, int x, int z, int centerX, int centerZ, i8* arr);

	/** Generates structures in specified chunk next to existing structures. Does *not* generate StructureStarts. */
	bool generateStructuresInChunk(World* pWorld, Random& rand, int chunkX, int chunkZ);

	/** Returns true if the structure generator has generated a structure located at the given position tuple. */
	bool hasStructureAt(int x, int y, int z);

	bool func_142038_b(int x, int y, int z);

	ChunkPosition getNearestInstance(World* pWorld, int x, int y, int z);

protected:

	virtual bool canSpawnStructureAtCoords(int x, int z) = 0;

	virtual StructureStart* getStructureStart(int x, int z) = 0;

protected:
	typedef map<i64, StructureStart*>::type StructureMap;
	/**
	* Used to store a list of all structures that have been recursively generated. Used so that during recursive
	* generation, the structure generator can avoid generating structures that intersect ones that have already been placed.
	*/
	StructureMap m_structureMap;
};


class MapGenMineshaft : public MapGenStructure
{
public:
	MapGenMineshaft();

	// MapGenerateMineshaft(Map par1Map);

protected:
	bool canSpawnStructureAtCoords(int x, int z);

	StructureStart* getStructureStart(int x, int z);

protected:
	double m_probability = 0.f;
};

class MapGenNetherBridge : public MapGenStructure
{
public:
	SpwanLstEntryArr spawnList;

	MapGenNetherBridge();
	SpwanLstEntryArr& getSpawnList() { return spawnList; }
	bool canSpawnStructureAtCoords(int x, int z);
	StructureStart* getStructureStart(int x, int z);
};

}

#endif
