#include "AIMoveToRestriction.h"

#include "Entity/EntityCreature.h"
#include "Entity/PathNavigate.h"
#include "Util/Random.h"

namespace BLOCKMAN
{

AIMoveToRestriction::AIMoveToRestriction(EntityCreature* pCreature, float speed)
: theEntity(pCreature)
, movementSpeed(speed)
{
	setMutexBits(1);
}

bool AIMoveToRestriction::shouldExecute()
{
	if (theEntity->isWithinHomeDistanceCurrentPosition())
	{
		return false;
	}
	else
	{
		BlockPos pos = theEntity->getHomePosition();
		Vector3 toward = RandomPositionGenerator::findRandomTargetBlockTowards(theEntity, 16, 7, Vector3(pos));

		if (toward == Vector3::INVALID)
		{
			return false;
		}
		else
		{
			m_movePos = toward;
			return true;
		}
	}
}

bool AIMoveToRestriction::continueExecuting()
{
	return !theEntity->getNavigator().noPath();
}

void AIMoveToRestriction::startExecuting()
{
	theEntity->getNavigator().tryMoveToXYZ(m_movePos, movementSpeed);
}

Vector3 RandomPositionGenerator::staticVector = Vector3::ZERO;

Vector3 RandomPositionGenerator::findRandomTarget(EntityCreature* pCreature, int xz, int y)
{
	return findRandomTargetBlock(pCreature, xz, y, Vector3::INVALID);
}

/** finds a random target within par1(x,z) and par2 (y) blocks in the direction of the point par3 */
Vector3 RandomPositionGenerator::findRandomTargetBlockTowards(EntityCreature* pCreature, int xz, int y, const Vector3& v)
{
	staticVector = v - pCreature->position;
	return findRandomTargetBlock(pCreature, xz, y, staticVector);
}

/** finds a random target within par1(x,z) and par2 (y) blocks in the reverse direction of the point par3 */
Vector3 RandomPositionGenerator::findRandomTargetBlockAwayFrom(EntityCreature* pCreature, int xz, int y, const Vector3& v)
{
	staticVector = pCreature->position - v;
	return findRandomTargetBlock(pCreature, xz, y, staticVector);
}

Vector3 RandomPositionGenerator::findRandomTargetBlock(EntityCreature* pCreature, int xz, int y, const Vector3& v)
{
	Random* pRand = pCreature->getRNG();
	bool hasResult = false;
	BlockPos pos = BlockPos::ZERO;
	float maxWeight = -99999.0F;
	bool inRange;

	if (pCreature->hasHome())
	{
		float disSqr = (pCreature->getHomePosition() - pCreature->position.getFloor()).lenSqr() + 4.0F;
		float homeDis = (pCreature->getMaximumHomeDistance() + (float)xz);
		inRange = disSqr < homeDis * homeDis;
	}
	else
	{
		inRange = false;
	}

	for (int i = 0; i < 10; ++i)
	{
		BlockPos pos1;
		pos1.x = pRand->nextInt(2 * xz) - xz;
		pos1.y = pRand->nextInt(2 * y) - y;
		pos1.z = pRand->nextInt(2 * xz) - xz;

		if (v == Vector3::INVALID || pos1.x * v.x + pos1.z * v.z >= 0.f)
		{
			pos1 += pCreature->position.getFloor();

			if (!inRange || pCreature->isWithinHomeDistanceFromPosition(pos1))
			{
				float weight = pCreature->getBlockPathWeight(pos1);

				if (weight > maxWeight)
				{
					maxWeight = weight;
					pos = pos1;
					hasResult = true;
				}
			}
		}
	}

	if (hasResult)
	{
		return (Vector3)pos;
	}
	return Vector3::INVALID;
}

}