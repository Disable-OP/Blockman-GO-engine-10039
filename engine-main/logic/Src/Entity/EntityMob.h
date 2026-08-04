/********************************************************************
filename: 	EntityMob.h
file path:	dev\client\Src\Blockman\Entity

version:	1
author:		ajohn
company:	supernano
date:		2017-07-3
*********************************************************************/
#ifndef __ENTITY_MOB_HEADER__
#define __ENTITY_MOB_HEADER__

#include "EntityCreature.h"

namespace BLOCKMAN
{

class EntityMob : public EntityCreature /* implements IMob*/
{
public:
	EntityMob(World* pWorld);

	/** implement override  function from EntityLiving. */
	virtual void onLivingUpdate();
	virtual void onUpdate();
	virtual bool attackEntityFrom(DamageSource* pSource, float amount);
	virtual bool attackEntityAsMob(Entity* pEntity);
	virtual bool getCanSpawnHere();
	virtual float getBlockPathWeight(const BlockPos& pos);

protected:
	/** implement override  function from EntityCreature. */
	virtual Entity* findPlayerToAttack();
	virtual void attackEntity(Entity* pEntity, float damage);
	virtual void applyEntityAttributes();

	/** Checks to make sure the light is not too bright where the mob is spawning */
	bool isValidLightLevel();

	
};

}
#endif