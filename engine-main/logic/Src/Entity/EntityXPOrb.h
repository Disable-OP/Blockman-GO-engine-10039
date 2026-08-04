/********************************************************************
filename: 	EntityXPOrb.h
file path:	dev\client\Src\Blockman\Entity

version:	1
author:		ajohn
company:	supernano
date:		2017-3-6
*********************************************************************/
#ifndef __ENTITY_XPORB_HEADER__
#define __ENTITY_XPORB_HEADER__

#include "Entity.h"

namespace BLOCKMAN
{

class EntityPlayer;

class EntityXPOrb : public Entity
{
	RTTI_DECLARE(EntityXPOrb);

protected:
	/** The health of this XP orb. */
	int xpOrbHealth = 0;
	/** This is how much XP this orb has. */
	int xpValue = 0;
	/** The closest EntityPlayer to this orb. */
	EntityPlayer* closestPlayer = nullptr;
	/** Threshold color for tracking players */
	int xpTargetColor = 0;

public:
	/** A constantly increasing value that RenderXPOrb uses to control the colour shifting (Green / yellow) */
	int xpColor = 0;
	/** The age of the XP orb in ticks. */
	int xpOrbAge = 0;
	int delayBeforeCanPickup = 0;
	static int xp_table[10];

protected:
	/** implement override functions from Entity */
	virtual bool canTriggerWalking() { return false; }
	virtual void entityInit() {}
	virtual void dealFireDamage(int amount);
	
public:
	EntityXPOrb(World* pWorld, const Vector3& pos, int xp);
	EntityXPOrb(World* pWorld);
	
	int getXpValue() { return xpValue; }
	int getTextureByXP();
	static int getXPSplit(int par0);

	/** implement override functions from Entity */
	virtual int getBrightnessForRender(float rdt);
	virtual void onUpdate();
	virtual bool handleWaterMovement();
	virtual bool attackEntityFrom(DamageSource* pSource, float damage);
	virtual void writeEntityToNBT(NBTTagCompound* pNbtCompound);
	virtual void readEntityFromNBT(NBTTagCompound* pNbtCompound);
	virtual void onCollideWithPlayer(EntityPlayer* pPlayer);
	virtual bool canAttackWithItem() { return false; }
};

}

#endif