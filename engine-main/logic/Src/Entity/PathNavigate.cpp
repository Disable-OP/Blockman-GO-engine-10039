#include "PathNavigate.h"

#include "World/World.h"
#include "Entity/EntityLiving.h"
#include "Block/Block.h"
#include "Block/BlockManager.h"

namespace BLOCKMAN
{


	int PathPoint::m_createCount = 0;
	int PathPoint::m_destroyCount = 0;
PathPoint::PathPoint(const BlockPos& pos)
	: index(-1)
	, totalPathDistance(0)
	, distanceToNext(0)
	, distanceToTarget(0)
	, previous(NULL)
	, isFirst(false)
	, m_pos(pos)
{
	hash = makeHash(pos);
	m_createCount++;
}

int PathPoint::makeHash(const BlockPos& pos)
{
	return pos.y & 255 |
		(pos.x & 32767) << 8 | 
		(pos.z & 32767) << 24 | 
		(pos.x < 0 ? Math::MIN_INT : 0) | 
		(pos.z < 0 ? 32768 : 0);
}

float PathPoint::distanceTo(PathPoint* another)
{
	LordAssert(another);
	Vector3 del = Vector3(another->m_pos - m_pos);
	return del.len();
}

float PathPoint::distanceToSqr(PathPoint* another)
{
	LordAssert(another);
	Vector3 del = Vector3(another->m_pos - m_pos);
	return del.lenSqr();
}

bool PathPoint::operator==(const PathPoint& another) const
{
	if (this == &another)
		return true;
	return hash == another.hash && m_pos == another.m_pos;
}

bool PathPoint::equals(PathPoint* another)
{
	if (this == another)
		return true;
	return hash == another->hash && m_pos == another->m_pos;
}

PathEntity::PathEntity(PathPoint** ppoints, int len)
{
	this->points = ppoints;
	pathLength = len;
}

PathEntity::~PathEntity()
{
	LordSafeFree(points);
	for (auto iter = m_pointMapForDelete.begin(); iter != m_pointMapForDelete.end(); iter++)
	{
		LordSafeDelete(iter->second);
	}
	m_pointMapForDelete.clear();
}

Vector3 PathEntity::getVectorFromIndex(Entity* pEntity, int idx)
{
	float x = points[idx]->m_pos.x + (int)(pEntity->width + 1.0F) * 0.5f;
	float y = float(points[idx]->m_pos.y);
	float z = points[idx]->m_pos.z + (int)(pEntity->width + 1.0F) * 0.5f;
	return Vector3(x, y, z);
}


bool PathEntity::isSamePath(PathEntity* another)
{
	if (!another)
	{
		return false;
	}
	
	if (another->pathLength != pathLength)
	{
		return false;
	}

	for (int i = 0; i < pathLength; ++i)
	{
		if (points[i]->m_pos != another->points[i]->m_pos)
		{
			return false;
		}
	}

	return true;
}

bool PathEntity::isDestinationSame(const Vector3& vec)
{
	PathPoint* point = getFinalPathPoint();

	if (!point)
		return false;
	return point->m_pos.x == int(vec.x) && point->m_pos.z == int(vec.z);
}

PathNavigate::PathNavigate(EntityLiving& living, World* pWorld)
	: theEntity(living)
{
	worldObj = pWorld;
	pathSearchRange = living.getEntityAttribute(SharedMonsterAttributes::FOLLOW_RANGE);
}

float PathNavigate::getSearchRange()
{
	return (float)pathSearchRange->getAttributeValue(); 
}

PathEntity* PathNavigate::getPathToXYZ(const Vector3& pos)
{
	if (!canNavigate())
		return NULL;
	
	return worldObj->getEntityPathToXYZ(&theEntity, BlockPos(int(Math::Floor(pos.x)), (int)pos.y, int(Math::Floor(pos.z))),
		getSearchRange(), canPassOpenWoodenDoors, canPassClosedWoodenDoors, avoidsWater, canSwim);
}

bool PathNavigate::tryMoveToXYZ(const Vector3& pos, float speed)
{
	PathEntity* path = getPathToXYZ(pos);
	return setPath(path, speed);
}

PathEntity* PathNavigate::getPathToEntityLiving(Entity* pEntity)
{
	if (!canNavigate())
		return NULL;

	return worldObj->getPathEntityToEntity(&theEntity, pEntity, getSearchRange(), canPassOpenWoodenDoors, canPassClosedWoodenDoors, avoidsWater, canSwim);
}

bool PathNavigate::tryMoveToEntityLiving(Entity* pEntity, float speed)
{
	PathEntity* path = getPathToEntityLiving(pEntity);
	return path != NULL ? setPath(path, speed) : false;
}

bool PathNavigate::setPath(PathEntity* path, double speed)
{
	if (!path)
	{
		currentPath = NULL;
		return false;
	}
	else
	{
		if (!path->isSamePath(currentPath))
		{
			currentPath = path;
		}

		if (noSunPathfind)
		{
			removeSunnyPath();
		}

		if (currentPath->getCurrentPathLength() == 0)
		{
			return false;
		}
		else
		{
			this->speed = (float)speed;
			Vector3 pos = getEntityPosition();
			ticksAtLastPos = totalTicks;
			lastPosCheck = pos;
			return true;
		}
	}
}

void PathNavigate::onUpdateNavigation()
{
	++totalTicks;

	if (!noPath())
	{
		if (canNavigate())
		{
			pathFollow();
		}

		if (!noPath())
		{
			Vector3 pos = currentPath->getPosition(&theEntity);
			theEntity.getMoveHelper().setMoveTo(pos, speed);
		}
	}
}

void PathNavigate::pathFollow()
{
	Vector3 pos = getEntityPosition();
	int pathLen = currentPath->getCurrentPathLength();

	for (int i = currentPath->getCurrentPathIndex(); i < currentPath->getCurrentPathLength(); ++i)
	{
		if (currentPath->getPathPointFromIndex(i)->m_pos.y != (int)pos.y)
		{
			pathLen = i;
			break;
		}
	}

	float var8 = theEntity.width * theEntity.width;
	for (int i = currentPath->getCurrentPathIndex(); i < pathLen; ++i)
	{
		if (pos.squareDistanceTo(currentPath->getVectorFromIndex(&theEntity, i)) < (double)var8)
		{
			currentPath->setCurrentPathIndex(i + 1);
		}
	}

	int w = int(Math::Ceil(theEntity.width));
	int h = (int)theEntity.height + 1;
	for (int i = pathLen - 1; i >= currentPath->getCurrentPathIndex(); --i)
	{
		if (isDirectPathBetweenPoints(pos, currentPath->getVectorFromIndex(&theEntity, i), i, h, w))
		{
			currentPath->setCurrentPathIndex(i);
			break;
		}
	}

	if (totalTicks - ticksAtLastPos > 100)
	{
		if (pos.squareDistanceTo(lastPosCheck) < 2.25f)
		{
			clearPathEntity();
		}

		ticksAtLastPos = totalTicks;
		lastPosCheck = pos;
	}
}

bool PathNavigate::noPath()
{
	return !currentPath || currentPath->isFinished();
}

Vector3 PathNavigate::getEntityPosition()
{
	return Vector3(float(theEntity.position.x), float(getPathableYPos()), float(theEntity.position.z));
}

int PathNavigate::getPathableYPos()
{
	if (theEntity.isInWater() && canSwim)
	{
		int ix = int(Math::Floor(theEntity.position.x));
		int iy = (int)theEntity.boundingBox.vMin.y;
		int iz = int(Math::Floor(theEntity.position.z));
		int blockID = worldObj->getBlockId(BlockPos(ix, iy, iz));

		int count = 0;
		do
		{
			if (blockID != BLOCK_ID_WATERMOVING && blockID != BLOCK_ID_WATERSTILL)
			{
				return iy;
			}

			++iy;
			blockID = worldObj->getBlockId(BlockPos(ix, iy, iz));
			++count;
		} while (count <= 16);

		return (int)theEntity.boundingBox.vMin.y;
	}
	else
	{
		return (int)(theEntity.boundingBox.vMin.y + 0.5f);
	}
}

bool PathNavigate::canNavigate()
{
	return theEntity.onGround || canSwim && isInFluid();
}

bool PathNavigate::isInFluid()
{
	return theEntity.isInWater() || theEntity.handleLavaMovement();
}

void PathNavigate::removeSunnyPath()
{
	if (!worldObj->canBlockSeeTheSky(BlockPos(int(Math::Floor(theEntity.position.x)), (int)(theEntity.boundingBox.vMin.y + 0.5f), int(Math::Floor(theEntity.position.z)))))
	{
		for (int i = 0; i < currentPath->getCurrentPathLength(); ++i)
		{
			PathPoint* point = currentPath->getPathPointFromIndex(i);

			if (worldObj->canBlockSeeTheSky(point->m_pos))
			{
				currentPath->setCurrentPathLength(i - 1);
				return;
			}
		}
	}
}

bool PathNavigate::isDirectPathBetweenPoints(const Vector3& pos1, const Vector3& pos2, int xsize, int ysize, int zsize)
{
	int ix = int(Math::Floor(pos1.x));
	int iz = int(Math::Floor(pos1.z));
	float dx = pos2.x - pos1.x;
	float dz = pos2.z - pos1.z;
	float proj_xz = dx * dx + dz * dz;

	if (proj_xz < 1.0E-8f)
	{
		return false;
	}
	else
	{
		float var14 = 1.0f / Math::Sqrt(proj_xz);
		dx *= var14;
		dz *= var14;
		xsize += 2;
		zsize += 2;

		if (!isSafeToStandAt(ix, (int)pos1.y, iz, xsize, ysize, zsize, pos1, dx, dz))
		{
			return false;
		}
		else
		{
			xsize -= 2;
			zsize -= 2;
			float var16 = 1.0f / Math::Abs(dx);
			float var18 = 1.0f / Math::Abs(dz);
			float var20 = ix - pos1.x;
			float var22 = iz - pos1.z;

			if (dx >= 0.0f)
			{
				++var20;
			}

			if (dz >= 0.0f)
			{
				++var22;
			}

			var20 /= dx;
			var22 /= dz;
			int var24 = dx < 0.0f ? -1 : 1;
			int var25 = dz < 0.0f ? -1 : 1;
			int var26 = int(Math::Floor(pos2.x));
			int var27 = int(Math::Floor(pos2.z));
			int var28 = var26 - ix;
			int var29 = var27 - iz;

			do
			{
				if (var28 * var24 <= 0 && var29 * var25 <= 0)
				{
					return true;
				}

				if (var20 < var22)
				{
					var20 += var16;
					ix += var24;
					var28 = var26 - ix;
				}
				else
				{
					var22 += var18;
					iz += var25;
					var29 = var27 - iz;
				}
			} while (isSafeToStandAt(ix, (int)pos1.y, iz, xsize, ysize, zsize, pos1, dx, dz));

			return false;
		}
	}
}

bool PathNavigate::isSafeToStandAt(int xoff, int yoff, int zoff, int xsize, int ysize, int zsize, const Vector3& origin, float vex, float vez)
{
	int ext_x = xoff - xsize / 2;
	int ext_z = zoff - zsize / 2;

	if (!isPositionClear(ext_x, yoff, ext_z, xsize, ysize, zsize, origin, vex, vez))
	{
		return false;
	}
	else
	{
		for (int ix = ext_x; ix < ext_x + xsize; ++ix)
		{
			for (int iz = ext_z; iz < ext_z + zsize; ++iz)
			{
				float var16 = ix + 0.5f - origin.x;
				float var18 = iz + 0.5f - origin.z;

				if (var16 * vex + var18 * vez >= 0.0f)
				{
					int blockID = worldObj->getBlockId(BlockPos(ix, yoff - 1, iz));

					if (blockID <= 0)
					{
						return false;
					}

					const BM_Material& mat = BlockManager::sBlocks[blockID]->getMaterial();

					if (mat == BM_Material::BM_MAT_water && !theEntity.isInWater())
					{
						return false;
					}

					if (mat == BM_Material::BM_MAT_lava)
					{
						return false;
					}
				}
			}
		}

		return true;
	}
}

bool PathNavigate::isPositionClear(int xoff, int yoff, int zoff, int xsize, int ysize, int zsize, const Vector3& origin, float vex, float vez)
{
	for (int ix = xoff; ix < xoff + xsize; ++ix)
	{
		for (int iy = yoff; iy < yoff + ysize; ++iy)
		{
			for (int iz = zoff; iz < zoff + zsize; ++iz)
			{
				float var15 = ix + 0.5f - origin.x;
				float var17 = iz + 0.5f - origin.z;

				if (var15 * vex + var17 * vez >= 0.0f)
				{
					BlockPos pos(ix, iy, iz);
					int blockID = worldObj->getBlockId(pos);

					if (blockID > 0 && !BlockManager::sBlocks[blockID]->getBlocksMovement(worldObj, pos))
					{
						return false;
					}
				}
			}
		}
	}

	return true;
}

}