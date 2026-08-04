/********************************************************************
filename: 	AIMoveToRestriction.h
file path:	dev\client\Src\Blockman\AI

version:	1
author:		ajohn
company:	supernano
date:		2017-07-05
*********************************************************************/
#ifndef __ENTITY_MOVETO_RESTRICTION_HEADER__
#define __ENTITY_MOVETO_RESTRICTION_HEADER__

#include "AIBase.h"
class EntityCreature;

namespace BLOCKMAN
{

class EntityCreature;
class AIMoveToRestriction : public AIBase
{
protected:
	EntityCreature* theEntity;
	Vector3 m_movePos;
	float movementSpeed;

public:
	AIMoveToRestriction(EntityCreature* pCreature, float speed);

	/** implement override function from AIBase */
	virtual bool shouldExecute();
	virtual bool continueExecuting();
	virtual void startExecuting();
};

class RandomPositionGenerator
{
protected:
	/** used to store a driection when the user passes a point to move towards or away from. */
	static Vector3 staticVector; // = Vec3.createVectorHelper(0.0D, 0.0D, 0.0D);

public:
	/** finds a random target within par1(x,z) and par2 (y) blocks */
	static Vector3 findRandomTarget(EntityCreature* pCreature, int xz, int y);

	/** finds a random target within par1(x,z) and par2 (y) blocks in the direction of the point par3 */
	static Vector3 findRandomTargetBlockTowards(EntityCreature* pCreature, int xz, int y, const Vector3& v);

	/** finds a random target within par1(x,z) and par2 (y) blocks in the reverse direction of the point par3 */
	static Vector3 findRandomTargetBlockAwayFrom(EntityCreature* pCreature, int xz, int y, const Vector3& v);

protected:
	/** searches 10 blocks at random in a within par1(x,z) and par2 (y) distance, ignores those not in the direction of
	* par3Vec3, then points to the tile for which creature.getBlockPathWeight returns the highest number */
	static Vector3 findRandomTargetBlock(EntityCreature* pCreature, int xz, int y, const Vector3& v);
};


}

#endif