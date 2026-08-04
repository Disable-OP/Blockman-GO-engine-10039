/********************************************************************
filename: 	PathNavigate.h
file path:	dev\client\Src\Blockman\Entity

version:	1
author:		ajohn
company:	supernano
date:		2017-3-13
*********************************************************************/
#ifndef __PATH_NAVIGATE_HEADER__
#define __PATH_NAVIGATE_HEADER__

#include "EntityLivingBase.h"

namespace BLOCKMAN
{

class EntityLiving;
class World;

class PathPoint : public ObjectAlloc
{
	friend class PathHeap;
	friend class PathFinder;
protected:
	/** A hash of the coordinates used to identify this point */
	int hash = 0;
	/** The index of this point in its assigned path */
	int index = 0;
	/** The distance along the path to this point */
	float totalPathDistance = 0.f;
	/** The linear distance to the next point */
	float distanceToNext = 0.f;
	/** The distance to the target */
	float distanceToTarget = 0.f;
	/** The point preceding this in its assigned path */
	PathPoint* previous = nullptr;

public:
	/** The x coordinate of this point */
	BlockPos m_pos;
	/** Indicates this is the origin */
	bool isFirst = false;

public:
	PathPoint(const BlockPos& pos);
	~PathPoint() { m_destroyCount++; }
	static int makeHash(const BlockPos& pos);
	/** Returns the linear distance to another path point */
	float distanceTo(PathPoint* another);
	/** Returns the square distance to another path point */
	float distanceToSqr(PathPoint* another);
	bool operator==(const PathPoint& another) const;
	bool equals(PathPoint* another);
	int hashCode() { return hash; }
	bool isAssigned() { return index >= 0; }
	static int m_createCount;
	static int m_destroyCount;
};

class PathEntity : public ObjectAlloc
{
protected:
	/** The actual points in the path */
	PathPoint** points = nullptr;
	/** PathEntity Array Index the Entity is currently targeting */
	int currentPathIndex = 0;
	/** The total length of the path */
	int pathLength = 0;

	/* the index map of pathpoints, copied from pathFinder */
	LORD::map<int, PathPoint*>::type m_pointMapForDelete = {};

public:
	PathEntity(PathPoint** points, int len);
	~PathEntity();

	/** Directs this path to the next point in its array */
	void incrementPathIndex() { ++currentPathIndex; }
	/** Returns true if this path has reached the end */
	bool isFinished() { return currentPathIndex >= pathLength; }
	/** returns the last PathPoint of the Array */
	PathPoint* getFinalPathPoint() { return pathLength > 0 ? points[pathLength - 1] : NULL; }
	/** return the PathPoint located at the specified PathIndex, usually the current one */
	PathPoint* getPathPointFromIndex(int idx) { return points[idx]; }
	int getCurrentPathLength() { return pathLength; }
	void setCurrentPathLength(int len) { pathLength = len; }
	int getCurrentPathIndex() { return currentPathIndex; }
	void setCurrentPathIndex(int idx) { currentPathIndex = idx; }
	/** Gets the vector of the PathPoint associated with the given index. */
	Vector3 getVectorFromIndex(Entity* pEntity, int idx);
	/** returns the current PathEntity target node as Vec3D */
	Vector3 getPosition(Entity* pEntity) { return getVectorFromIndex(pEntity, currentPathIndex); }
	/** Returns true if the EntityPath are the same. Non instance related equals. */
	bool isSamePath(PathEntity* another);
	/** Returns true if the final PathPoint in the PathEntity is equal to Vec3D coords. */
	bool isDestinationSame(const Vector3& vec);
	/*save for deleting when destruct*/
	void setPointMap(LORD::map<int, PathPoint*>::type& pointMap) { m_pointMapForDelete = pointMap; }
};

class PathNavigate : public ObjectAlloc
{
protected:
	EntityLiving& theEntity;
	World* worldObj = nullptr;
	/** The PathEntity being followed. */
	PathEntity* currentPath = nullptr;
	float speed = 0.f;
	/** The number of blocks (extra) +/- in each axis that get pulled out as cache for the pathfinder's search space */
	IAttributeInstance* pathSearchRange = nullptr;
	bool noSunPathfind = false;
	/** Time, in number of ticks, following the current path */
	int totalTicks = 0;
	/** The time when the last position check was done (to detect successful movement) */
	int ticksAtLastPos = 0;
	/** Coordinates of the entity's position last time a check was done (part of monitoring getting 'stuck') */
	Vector3 lastPosCheck;
	/** Specifically, if a wooden door block is even considered to be passable by the pathfinder */
	bool canPassOpenWoodenDoors = true; // = true;
	/** If door blocks are considered passable even when closed */
	bool canPassClosedWoodenDoors = false;
	/** If water blocks are avoided (at least by the pathfinder) */
	bool avoidsWater = false;
	/** If the entity can swim. Swimming AI enables this and the pathfinder will also cause the entity to swim straight upwards when underwater */
	bool canSwim = false;

protected:
	void pathFollow();
	Vector3 getEntityPosition();
	/** Gets the safe pathing Y position for the entity depending on if it can path swim or not */
	int getPathableYPos();
	/** If on ground or swimming and can swim */
	bool canNavigate();
	/** Returns true if the entity is in water or lava, false otherwise */
	bool isInFluid();
	/** Trims path data from the end to the first sun covered block */
	void removeSunnyPath();
	/** Returns true when an entity of specified size could safely walk in a straight line between the two points. */
	bool isDirectPathBetweenPoints(const Vector3& pos1, const Vector3& pos2, int xsize, int ysize, int zsize);
	/** Returns true when an entity could stand at a position, including solid blocks under the entire entity. */
	bool isSafeToStandAt(int xoff, int yoff, int zoff, int xsize, int ysize, int zsize, const Vector3& origin, float vex, float vez);
	/** Returns true if an entity does not collide with any solid blocks at the position. */
	bool isPositionClear(int xoff, int yoff, int zoff, int xsize, int ysize, int zsize, const Vector3& origin, float vex, float vez);

public:
	PathNavigate(EntityLiving& living, World* pWorld);
	
	void setAvoidsWater(bool avoids) { avoidsWater = avoids; }
	bool getAvoidsWater() { return avoidsWater; }
	void setBreakDoors(bool breakDoor) { canPassClosedWoodenDoors = breakDoor; }
	bool getCanBreakDoors() { return canPassClosedWoodenDoors; }
	/** Sets if the entity can enter open doors */
	void setEnterDoors(bool passDoor) { canPassOpenWoodenDoors = passDoor; }
	/** Sets if the path should avoid sunlight */
	void setAvoidSun(bool avoidsun) { noSunPathfind = avoidsun; }
	/** Sets the speed */
	void setSpeed(float s) { speed = s; }
	/** Sets if the entity can swim */
	void setCanSwim(bool canswim) { canSwim = canswim; }
	float getSearchRange();
	/** Returns the path to the given coordinates */
	PathEntity* getPathToXYZ(const Vector3& pos);
	/** Try to find and set a path to XYZ. Returns true if successful. */
	bool tryMoveToXYZ(const Vector3& pos, float speed);
	/** Returns the path to the given EntityLiving */
	PathEntity* getPathToEntityLiving(Entity* pEntity);
	/** Try to find and set a path to EntityLiving. Returns true if successful. */
	bool tryMoveToEntityLiving(Entity* pEntity, float speed);
	/** sets the active path data if path is 100% unique compared to old path, checks to adjust path for sun avoiding ents and stores end coords */
	bool setPath(PathEntity* path, double speed);
	/** gets the actively used PathEntity */
	PathEntity* getPath() { return currentPath; }
	void onUpdateNavigation();
	/** If null path or reached the end */
	bool noPath();
	/** sets active PathEntity to null */
	void clearPathEntity() { currentPath = NULL; }
};


}

#endif
