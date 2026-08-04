#include "PathFinder.h"
#include "PathNavigate.h"

#include "Entity/Entity.h"
#include "World/World.h"
#include "Block/Block.h"
#include "Block/BlockManager.h"

namespace BLOCKMAN
{

PathHeap::PathHeap()
{
	capacity = 128;
	pathPoints = (PathPoint**)LordMalloc(sizeof(PathPoint*) * capacity);
	memset(pathPoints, 0, sizeof(PathPoint*) * capacity);
	count = 0;
}

PathHeap::~PathHeap()
{
	LordSafeFree(pathPoints);
	count = 0;
}

void PathHeap::addPoint(PathPoint* point)
{
	LordAssert(point->index < 0);

	if (count == capacity)
	{
		PathPoint** newPoints = (PathPoint**)LordMalloc(sizeof(PathPoint*) * capacity * 2);
		memcpy(newPoints, pathPoints, sizeof(PathPoint*) * capacity);
		memset(newPoints + capacity, 0, sizeof(PathPoint*) * capacity);
		LordSafeFree(pathPoints);
		pathPoints = newPoints;
		capacity = capacity * 2;
	}

	pathPoints[count] = point;
	point->index = count;
	sortBack(count++);
}

PathPoint* PathHeap::dequeue()
{
	PathPoint* front = pathPoints[0];
	pathPoints[0] = pathPoints[--count];
	pathPoints[count] = NULL;

	if (count > 0)
	{
		sortForward(0);
	}

	front->index = -1;
	return front;
}

void PathHeap::changeDistance(PathPoint* point, float newDistanceToTarget)
{
	float oldDistanceToTarget = point->distanceToTarget;
	point->distanceToTarget = newDistanceToTarget;

	if (newDistanceToTarget < oldDistanceToTarget)
	{
		sortBack(point->index);
	}
	else
	{
		sortForward(point->index);
	}
}

void PathHeap::sortBack(int idx)
{
	PathPoint* point = pathPoints[idx];
	int i;

	for (float f = point->distanceToTarget; idx > 0; idx = i)
	{
		i = (idx - 1) >> 1;
		PathPoint* point1 = pathPoints[i];

		if (f >= point1->distanceToTarget)
		{
			break;
		}

		pathPoints[idx] = point1;
		point1->index = idx;
	}

	pathPoints[idx] = point;
	point->index = idx;
}

void PathHeap::sortForward(int idx)
{
	PathPoint* point = pathPoints[idx];
	float f = point->distanceToTarget;

	while (true)
	{
		int i = 1 + (idx << 1);
		int j = i + 1;

		if (i >= count)
		{
			break;
		}

		PathPoint* point1 = pathPoints[i];
		float f1 = point1->distanceToTarget;
		PathPoint* point2 = NULL;
		float f2 = Math::MAX_FLOAT;

		if (j < count)
		{
			point2 = pathPoints[j];
			f2 = point2->distanceToTarget;
		}

		if (f1 < f2)
		{
			if (f1 >= f)
			{
				break;
			}

			pathPoints[idx] = point1;
			point1->index = idx;
			idx = i;
		}
		else
		{
			if (f2 >= f)
			{
				break;
			}

			pathPoints[idx] = point2;
			point2->index = idx;
			idx = j;
		}
	}

	pathPoints[idx] = point;
	point->index = idx;
}

PathFinder::PathFinder(IBlockAccess* blockAccess, bool woddenDoor, bool movementBlock, bool inWater, bool canDrown)
	: worldMap(blockAccess)
	, isWoddenDoorAllowed(woddenDoor)
	, isMovementBlockAllowed(movementBlock)
	, isPathingInWater(inWater)
	, canEntityDrown(canDrown)
{
	pathOptions = (PathPoint**)LordMalloc(sizeof(PathPoint*) * 32);
}

PathFinder::~PathFinder()
{
	LordSafeFree(pathOptions);
	pointMap.clear();
}

PathEntity* PathFinder::createEntityPathTo(Entity* fromEntity, Entity* to, float minDis)
{
	return createEntityPathTo(fromEntity, to->getWidelyPos(), minDis);
}

PathEntity* PathFinder::createEntityPathTo(Entity* fromEntity, const BlockPos& toPosInt, float minDis)
{
	return createEntityPathTo(fromEntity, Vector3(toPosInt) + Vector3::HALF, minDis);
}

PathEntity* PathFinder::createEntityPathTo(Entity* fromEntity, const Vector3& toPosFloat, float minDis)
{
	path.clearPath();
	pointMap.clear(); // need delete. 
	bool inWater = isPathingInWater;
	BlockPos toPosInt = fromEntity->getWidelyPos().getFloor();
	
	if (canEntityDrown && fromEntity->isInWater())
	{
		toPosInt.y = (int)fromEntity->boundingBox.vMin.y;
		for (int blockID = fromEntity->world->getBlockId(toPosInt);
		blockID == BLOCK_ID_WATERMOVING || blockID == BLOCK_ID_WATERSTILL;
			blockID = fromEntity->world->getBlockId(toPosInt))
		{
			++toPosInt.y;
		}

		inWater = isPathingInWater;
		isPathingInWater = false;
	}

	PathPoint* fromPoint = openPoint(BlockPos(int(Math::Floor(fromEntity->boundingBox.vMin.x)), toPosInt.y, int(Math::Floor(fromEntity->boundingBox.vMin.z))));
	//PathPoint* point2 = openPoint(BlockPos(int(Math::Floor(ipos.x - (from->width / 2.0F))), int(Math::Floor(float(ipos.y))), int(Math::Floor(ipos.z - (from->width / 2.0F)))));
	PathPoint* toPoint = openPoint(BlockPos(int(Math::Floor(toPosFloat.x - (fromEntity->width / 2.0F))), int(Math::Floor(float(toPosFloat.y))), int(Math::Floor(toPosFloat.z - (fromEntity->width / 2.0F)))));
	PathPoint* sizePoint = LordNew PathPoint(BlockPos(int(Math::Floor(fromEntity->width + 1.0F)), int(Math::Floor(fromEntity->height + 1.0F)), int(Math::Floor(fromEntity->width + 1.0F))));
	PathEntity* path = addToPath(fromEntity, fromPoint, toPoint, sizePoint, minDis);
	LordDelete sizePoint;
	isPathingInWater = inWater;
	return path;
}

void PathFinder::deletePathPointMap()
{
	for (auto iter = pointMap.begin(); iter != pointMap.end(); iter++)
	{
		LordSafeDelete(iter->second);
	}

	pointMap.clear();
}

PathEntity* PathFinder::addToPath(Entity* fromEntity, PathPoint* fromPoint, PathPoint* toPoint, PathPoint* sizePoint, float mindis)
{
	fromPoint->totalPathDistance = 0.0F;
	fromPoint->distanceToNext = fromPoint->distanceToSqr(toPoint);
	fromPoint->distanceToTarget = fromPoint->distanceToNext;
	path.clearPath();
	path.addPoint(fromPoint);
	PathPoint* closestPointToDest = fromPoint;
	while (!path.isPathEmpty())
	{
		PathPoint* front = path.dequeue();

		if (front->equals(toPoint))
		{
			return createEntityPath(fromPoint, toPoint);
		}

		if (front->distanceToSqr(toPoint) < closestPointToDest->distanceToSqr(toPoint))
		{
			closestPointToDest = front;
		}

		front->isFirst = true;
		int options = 0;
		if(fromEntity->isFlyEntity())
			options = findPathOptions_fly(fromEntity, front, sizePoint, toPoint, mindis);
		else
			options = findPathOptions(fromEntity, front, sizePoint, toPoint, mindis);

		for (int i = 0; i < options; ++i)
		{
			PathPoint* pOption = pathOptions[i];
			float totalDistanceThroughFront = front->totalPathDistance + front->distanceToSqr(pOption);

			if (!pOption->isAssigned() || totalDistanceThroughFront < pOption->totalPathDistance)
			{
				pOption->previous = front;
				pOption->totalPathDistance = totalDistanceThroughFront;
				pOption->distanceToNext = pOption->distanceToSqr(toPoint);

				if (pOption->isAssigned())
				{
					path.changeDistance(pOption, pOption->totalPathDistance + pOption->distanceToNext);
				}
				else
				{
					pOption->distanceToTarget = pOption->totalPathDistance + pOption->distanceToNext;
					path.addPoint(pOption);
				}
			}
		}
		
	}

	if (closestPointToDest == fromPoint)
	{
		deletePathPointMap();
		return NULL;
	}
	else
	{
		return createEntityPath(fromPoint, closestPointToDest);
	}
}

int PathFinder::findPathOptions_fly(Entity* fromEntity, PathPoint* currentPoint, PathPoint* sizePoint,
	PathPoint* toPoint, float mindis)
{

	int idx = 0;
	i8 yOff = 0;

	PathPoint* pS = getSafePoint_fly(fromEntity, currentPoint->m_pos.getPosZ(), sizePoint, yOff);
	PathPoint* pW = getSafePoint_fly(fromEntity, currentPoint->m_pos.getNegX(), sizePoint, yOff);
	PathPoint* pE = getSafePoint_fly(fromEntity, currentPoint->m_pos.getPosX(), sizePoint, yOff);
	PathPoint* pN = getSafePoint_fly(fromEntity, currentPoint->m_pos.getNegZ(), sizePoint, yOff);

	PathPoint* pSE = getSafePoint_fly(fromEntity, currentPoint->m_pos.getPosZ().getPosX(), sizePoint, yOff);
	PathPoint* pSW = getSafePoint_fly(fromEntity, currentPoint->m_pos.getPosZ().getNegX(), sizePoint, yOff);
	PathPoint* pNE = getSafePoint_fly(fromEntity, currentPoint->m_pos.getNegZ().getPosX(), sizePoint, yOff);
	PathPoint* pNW = getSafePoint_fly(fromEntity, currentPoint->m_pos.getNegZ().getNegX(), sizePoint, yOff);
	PathPoint* pUp = getSafePoint_fly(fromEntity, currentPoint->m_pos.getPosY(), sizePoint, yOff);
	PathPoint* pDown = getSafePoint_fly(fromEntity, currentPoint->m_pos.getNegY(), sizePoint, yOff);

	if (pS && !pS->isFirst && pS->distanceTo(toPoint) < mindis)
	{
		pathOptions[idx++] = pS;
	}

	if (pW && !pW->isFirst && pW->distanceTo(toPoint) < mindis)
	{
		pathOptions[idx++] = pW;
	}

	if (pE && !pE->isFirst && pE->distanceTo(toPoint) < mindis)
	{
		pathOptions[idx++] = pE;
	}

	if (pN && !pN->isFirst && pN->distanceTo(toPoint) < mindis)
	{
		pathOptions[idx++] = pN;
	}

	if (pSE && !pSE->isFirst && pSE->distanceTo(toPoint) < mindis)
	{
		pathOptions[idx++] = pSE;
	}

	if (pSW && !pSW->isFirst && pSW->distanceTo(toPoint) < mindis)
	{
		pathOptions[idx++] = pSW;
	}

	if (pNE && !pNE->isFirst && pNE->distanceTo(toPoint) < mindis)
	{
		pathOptions[idx++] = pNE;
	}

	if (pNW && !pNW->isFirst && pNW->distanceTo(toPoint) < mindis)
	{
		pathOptions[idx++] = pNW;
	}

	if (pUp && !pUp->isFirst && pUp->distanceTo(toPoint) < mindis)
	{
		pathOptions[idx++] = pUp;
	}

	if (pDown && !pDown->isFirst && pDown->distanceTo(toPoint) < mindis)
	{
		pathOptions[idx++] = pDown;
	}

	return idx;
}

int PathFinder::findPathOptions(Entity* fromEntity, PathPoint* currentPoint, PathPoint* sizePoint, PathPoint* toPoint, float mindis)
{
	int idx = 0;
	i8 yOff = 0;

	if (getVerticalOffset(fromEntity, currentPoint->m_pos.getPosY(), sizePoint) == 1)
	{
		yOff = 1;
	}

	PathPoint* pS = getSafePoint(fromEntity, currentPoint->m_pos.getPosZ(), sizePoint, yOff);
	PathPoint* pW = getSafePoint(fromEntity, currentPoint->m_pos.getNegX(), sizePoint, yOff);
	PathPoint* pE = getSafePoint(fromEntity, currentPoint->m_pos.getPosX(), sizePoint, yOff);
	PathPoint* pN = getSafePoint(fromEntity, currentPoint->m_pos.getNegZ(), sizePoint, yOff);

	PathPoint* pSE = getSafePoint(fromEntity, currentPoint->m_pos.getPosZ().getPosX(), sizePoint, yOff);
	PathPoint* pSW = getSafePoint(fromEntity, currentPoint->m_pos.getPosZ().getNegX(), sizePoint, yOff);
	PathPoint* pNE = getSafePoint(fromEntity, currentPoint->m_pos.getNegZ().getPosX(), sizePoint, yOff);
	PathPoint* pNW = getSafePoint(fromEntity, currentPoint->m_pos.getNegZ().getNegX(), sizePoint, yOff);

	if (pS && !pS->isFirst && pS->distanceTo(toPoint) < mindis)
	{
		pathOptions[idx++] = pS;
	}

	if (pW && !pW->isFirst && pW->distanceTo(toPoint) < mindis)
	{
		pathOptions[idx++] = pW;
	}

	if (pE && !pE->isFirst && pE->distanceTo(toPoint) < mindis)
	{
		pathOptions[idx++] = pE;
	}

	if (pN && !pN->isFirst && pN->distanceTo(toPoint) < mindis)
	{
		pathOptions[idx++] = pN;
	}

	if (pSE && !pSE->isFirst && pSE->distanceTo(toPoint) < mindis)
	{
		pathOptions[idx++] = pSE;
	}

	if (pSW && !pSW->isFirst && pSW->distanceTo(toPoint) < mindis)
	{
		pathOptions[idx++] = pSW;
	}

	if (pNE && !pNE->isFirst && pNE->distanceTo(toPoint) < mindis)
	{
		pathOptions[idx++] = pNE;
	}

	if (pNW && !pNW->isFirst && pNW->distanceTo(toPoint) < mindis)
	{
		pathOptions[idx++] = pNW;
	}



	return idx;
}
PathPoint* PathFinder::getSafePoint_fly(Entity* pEntity, const BlockPos& pos, PathPoint* point, int yOffset)
{
	PathPoint* pResult = NULL;
	if (pEntity->world->getBlockId(pos) == 0)
		pResult = openPoint(pos);

	return pResult;
}

PathPoint* PathFinder::getSafePoint(Entity* pEntity, const BlockPos& pos, PathPoint* point, int yOffset)
{
	PathPoint* pResult = NULL;
	int offset = getVerticalOffset(pEntity, pos, point);

	if (offset == 2)
	{
		return openPoint(pos);
	}
	else
	{
		if (offset == 1)
		{
			pResult = openPoint(pos);
		}

		BlockPos pos1 = pos;
		pos1.y += yOffset;
		if (!pResult && yOffset > 0 && offset != -3 && offset != -4 && getVerticalOffset(pEntity, pos1, point) == 1)
		{
			pResult = openPoint(pos1);
		}
		else
		{
			pos1.y -= yOffset; 
		}

		if (pResult)
		{
			int var9 = 0;
			int var10 = 0;

			BlockPos pos2 = pos1;
			while (pos2.y > 0)
			{
				--pos2.y;
				var10 = getVerticalOffset(pEntity, pos2, point);

				if (isPathingInWater && var10 == -1)
				{
					return NULL;
				}

				if (var10 != 1)
				{
					break;
				}

				if (var9++ >= pEntity->getMaxFallHeight())
				{
					return NULL;
				}

				if (pos2.y > 0)
				{
					pResult = openPoint(pos2);
				}
			}

			if (var10 == -2)
			{
				return NULL;
			}
		}

		return pResult;
	}
}

PathPoint* PathFinder::openPoint(const BlockPos& pos)
{
	int hash = PathPoint::makeHash(pos);

	PathPoint* pReuslt = NULL;
	PathPointMap::iterator it = pointMap.find(hash);
	if (it == pointMap.end())
	{
		pReuslt = LordNew PathPoint(pos);
		pointMap.insert(std::make_pair(hash, pReuslt));
	}
	else
		pReuslt = it->second;

	return pReuslt;
}

PathPoint* PathFinder::openPoint(const BlockPos& pos,  PathPointMap & pathPointMap)
{
	int hash = PathPoint::makeHash(pos);

	PathPoint* pReuslt = NULL;
	PathPointMap::iterator it = pathPointMap.find(hash);
	if (it == pathPointMap.end())
	{
		pReuslt = LordNew PathPoint(pos);
		pathPointMap.insert(std::make_pair(hash, pReuslt));
	}
	else
		pReuslt = it->second;

	return pReuslt;
}

PathEntity* PathFinder::createEntityPath(PathPoint* from, PathPoint* to)
{
	int count = 1;
	PathPoint* point = nullptr;

	for (point = to; point->previous; point = point->previous)
	{
		++count;
	}

	PathPoint** points = (PathPoint**)LordMalloc(sizeof(PathPoint*) * count);
	point = to;
	
	int idx = count;
	--idx;
	for (points[idx] = to; point->previous; points[idx] = point)
	{
		point = point->previous;
		--idx;
	}

	PathEntity* pPathEntity = LordNew PathEntity(points, count);
	pPathEntity->setPointMap(pointMap);
	return pPathEntity;
}

int PathFinder::getVerticalOffset(Entity* par1Entity, const BlockPos& pos, PathPoint* point)
{
	return func_82565_a(par1Entity, pos, point, isPathingInWater, isMovementBlockAllowed, isWoddenDoorAllowed);
}

int PathFinder::func_82565_a(Entity* pEntity, const BlockPos& pos, PathPoint* point, bool inWater, bool allowBlock, bool allowDoor)
{
	bool var8 = false;

	for (int ix = pos.x; ix < pos.x + point->m_pos.x; ++ix)
	{
		for (int iy = pos.y; iy < pos.y + point->m_pos.y; ++iy)
		{
			for (int iz = pos.z; iz < pos.z + point->m_pos.z; ++iz)
			{
				BlockPos ipos(ix, iy, iz);
				int blockID = pEntity->world->getBlockId(ipos);

				if (blockID > 0)
				{
					if (blockID == BLOCK_ID_TRAP_DOOR)
					{
						var8 = true;
					}
					else if (blockID != BLOCK_ID_WATERMOVING && blockID != BLOCK_ID_WATERSTILL)
					{
						if (!allowDoor && blockID == BLOCK_ID_DOOR_WOOD)
						{
							return 0;
						}
					}
					else
					{
						if (inWater)
						{
							return -1;
						}

						var8 = true;
					}

					Block* pBlock = BlockManager::sBlocks[blockID];
					
					if (blockID == BLOCK_ID_RAIL_POWERED || 
						blockID == BLOCK_ID_RAIL_ACTIVATOR ||
						blockID == BLOCK_ID_RAIL_DETECTOR)
					{
						BlockPos ipos1 = pEntity->position.getFloor();
						int blockID1 = pEntity->world->getBlockId(ipos1);
						int blockID2 = pEntity->world->getBlockId(ipos1.getNegY());

						if (blockID1 != BLOCK_ID_RAIL_POWERED &&
							blockID1 != BLOCK_ID_RAIL_ACTIVATOR &&
							blockID1 != BLOCK_ID_RAIL_DETECTOR &&
							blockID2 != BLOCK_ID_RAIL_POWERED &&
							blockID2 != BLOCK_ID_RAIL_ACTIVATOR &&
							blockID2 != BLOCK_ID_RAIL_DETECTOR)
						{
							return -3;
						}
					}
					else if (!pBlock->getBlocksMovement(pEntity->world, ipos) &&
						(!allowBlock || blockID != BLOCK_ID_DOOR_WOOD))
					{
						if (blockID == BLOCK_ID_FENCE ||
							blockID == BLOCK_ID_NETHER_FENCE ||
							blockID == BLOCK_ID_FENCE_GATE ||
							blockID == BLOCK_ID_COBBLE_STONE_WALL)
						{
							return -3;
						}

						if (blockID == BLOCK_ID_TRAP_DOOR)
						{
							return -4;
						}

						const BM_Material& mat = pBlock->getMaterial();

						if (mat != BM_Material::BM_MAT_lava)
						{
							return 0;
						}

						if (!pEntity->handleLavaMovement())
						{
							return -2;
						}
					}
				}
			}
		}
	}

	return var8 ? 2 : 1;
}

PathEntity * PathFinder::createPathFromPosList(const std::vector<BlockPos> & posList, bool reserve)
{
	if (posList.empty())
		return nullptr;

	PathPointMap pointMap;
	PathPoint** newPoints = (PathPoint**)LordMalloc(sizeof(PathPoint*) * posList.size());

	int k = 0;
	for (int i = posList.size(); i > 0; i--)
	{
		PathPoint *newPoint = PathFinder::openPoint(posList[i-1], pointMap);
		if(reserve)
			newPoints[k] = newPoint;
		else
			newPoints[i-1] = newPoint;
		k++;
	}

	PathEntity* pPathEntity = LordNew PathEntity(newPoints, posList.size());
	pPathEntity->setPointMap(pointMap);
	return pPathEntity;
}

}