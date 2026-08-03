/********************************************************************
filename: 	EntityItem.h
file path:	dev\client\Src\Blockman\Entity

version:	1
author:		ajohn
company:	supernano
date:		2017-3-3
*********************************************************************/
#ifndef __ENTITY_ITEM_HEADER__
#define __ENTITY_ITEM_HEADER__

#include "Entity.h"

namespace BLOCKMAN
{

class World;
class ItemStack;

class EntityItem : public Entity
{
	RTTI_DECLARE(EntityItem);

protected:
	/** The health of this EntityItem. (For example, damage for tools) */
	int health = 0;

	ItemStackPtr data_itemstack = nullptr;

public:
	/** The age of this EntityItem (used to animate it up and down as well as expire it) */
	int age = 0;
	int isCombine = true;
	bool isEverLife = false;
	int delayBeforeCanPickup = 0;
	/** The EntityItem's random initial float height. */
	float hoverStart = 0.f;

protected:
	void init_entityItem();
	/** Looks for other itemstacks nearby and tries to stack them together */
	void searchForOtherItemsNearby();

	/** implement override functions from Entity */
	virtual bool canTriggerWalking() { return false; }
	virtual void entityInit() { data_itemstack = nullptr; }
	virtual void dealFireDamage(int amount);
	
public:
	EntityItem(World* pWorld, const Vector3& pos);
	EntityItem(World* pWorld, const Vector3& pos, ItemStackPtr pStack);
	EntityItem(World* pWorld);

	virtual ~EntityItem();
	
	/** Tries to merge this item with the item passed as the parameter. Returns true if successful. */
	bool combineItems(EntityItem* pItem);
	/** sets the age of the item so that it'll despawn one minute after it has been dropped (instead of five). */
	void setAgeToCreativeDespawnTime() { age = 4800; }
	/** Returns the ItemStack corresponding to the Entity */
	ItemStackPtr getEntityItem();
	/** Sets the ItemStack for this entity */
	void setEntityItemStack(ItemStackPtr pStack) { data_itemstack = pStack; }

	/** implement override functions from Entity */
	virtual void onUpdate();
	virtual bool handleWaterMovement();
	virtual bool attackEntityFrom(DamageSource* source, float amount);
	virtual void writeEntityToNBT(NBTTagCompound* pNbtCompound);
	virtual void readEntityFromNBT(NBTTagCompound* pNbtCompound);
	virtual void onCollideWithPlayer(EntityPlayer* pPlayer);
	virtual String getEntityName();
	virtual bool canAttackWithItem() { return false; }
};


}

#endif