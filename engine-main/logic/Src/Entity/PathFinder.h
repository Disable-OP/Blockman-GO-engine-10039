/********************************************************************
filename: 	PathFinder.h
file path:	dev\client\Src\Blockman\Entity

version:	1
author:		ajohn
company:	supernano
date:		2017-3-13
*********************************************************************/
#ifndef __PATH_FINDER_HEADER__
#define __PATH_FINDER_HEADER__

#include "BM_TypeDef.h"

namespace BLOCKMAN
{

class IBlockAccess;
class PathPoint;
class Entity;
class PathEntity;

class PathHeap
{
protected:
	/** Contains the points in this path */
	PathPoint** pathPoints = nullptr; // = new PathPoint[1024];
	int capacity = 0;
	/** The number of points in this path */
	int count = 0;

protected:
	/** Sorts a point to the left */
	void sortBack(int par1);
	/** Sorts a point to the right */
	void sortForward(int par1);
	
public:
	PathHeap();
	~PathHeap();

	/** Adds a point to the path */
	void addPoint(PathPoint* point);
	/** Returns and removes the first point in the path */
	PathPoint* dequeue();

	/** Clears the path */
	void clearPath() { count = 0; }
	/** Changes the provided point's distance to target */
	void changeDistance(PathPoint* point, float newDistanceToTarget);
	/** Returns true if this path contains no points */
	bool isPathEmpty() { return count == 0; }
};


class PathFinder : public ObjectAlloc
{
	typedef map<int, PathPoint*>::type PathPointMap;
protected:
	/** Used to find obstacles */
	IBlockAccess* worldMap = nullptr;
	/** The path being generated */
	PathHeap path;
	/** The points in the path */

	PathPointMap pointMap;
	/** Selection of path points to add to the path */
	PathPoint** pathOptions = nullptr; // = new PathPoint[32];
	/** should the PathFinder go through wodden door blocks */
	bool isWoddenDoorAllowed = false;
	/** should the PathFinder disregard BlockMovement type materials in its path */
	bool isMovementBlockAllowed = false;
	bool isPathingInWater = false;
	/** tells the FathFinder to not stop pathing underwater */
	bool canEntityDrown = false;

	void deletePathPointMap();

protected:
	/** Adds a path from start to end and returns the whole path (args: unused, start, end, unused, maxDistance) */
	PathEntity* addToPath(Entity* fromEntity, PathPoint* fromPoint, PathPoint* toPoint, PathPoint* sizePoint, float mindis);
	/** populates pathOptions with available points and returns the number of options found */
	int findPathOptions(Entity* fromEntity, PathPoint* currentPoint, PathPoint* sizePoint, PathPoint* toPoint, float mindis);

	int findPathOptions_fly(Entity* fromEntity, PathPoint* currentPoint, PathPoint* sizePoint, PathPoint* toPoint, float mindis);


	/** Returns a point that the entity can safely move to */
	PathPoint* getSafePoint(Entity* pEntity, const BlockPos& pos, PathPoint* point, int yOffset);

	PathPoint* getSafePoint_fly(Entity* pEntity, const BlockPos& pos, PathPoint* point, int yOffset);

	/** Returns a mapped point or creates and adds one */
	PathPoint* openPoint(const BlockPos& pos);

	static PathPoint* openPoint(const BlockPos&pos,  PathPointMap & pathPointMap);
	/** Returns a new PathEntity for a given start and end point */
	PathEntity* createEntityPath(PathPoint* from, PathPoint* to);

	
	
public:
	PathFinder(IBlockAccess* blockAccess, bool woddenDoor, bool movementBlock, bool inWater, bool canDrown);
	~PathFinder();

	/** Creates a path from one entity to another within a minimum distance */
	PathEntity* createEntityPathTo(Entity* fromEntity, Entity* toEntity, float minDis);
	/** Creates a path from an entity to a specified location within a minimum distance */
	PathEntity* createEntityPathTo(Entity* fromEntity, const BlockPos& toPosInt, float minDis);
	/** Internal implementation of creating a path from an entity to a point */
	PathEntity* createEntityPathTo(Entity* fromEntity, const Vector3& toPosFloat, float minDis);
	/** Checks if an entity collides with blocks at a position. Returns 1 if clear, 0 for colliding with any solid block,
	* -1 for water(if avoiding water) but otherwise clear, -2 for lava, -3 for fence, -4 for closed trapdoor, 2 if
	* otherwise clear except for open trapdoor or water(if not avoiding) */
	int getVerticalOffset(Entity* par1Entity, const BlockPos& pos, PathPoint* point);
	static int func_82565_a(Entity* pEntity, const BlockPos& pos, PathPoint* point, bool inWater, bool allowBlock, bool allowDoor);
	static PathEntity * createPathFromPosList(const std::vector<BlockPos> & posList, bool reserve = false);
};

}

#endif