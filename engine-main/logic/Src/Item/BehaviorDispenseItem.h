/********************************************************************
filename: 	BehaviorDispenseItem.h
file path:	dev\client\Src\Blockman\Item

version:	1
author:		ajohn
company:	supernano
date:		2017-2-22
*********************************************************************/
#ifndef __BEHAVIOR_DISPENSE_ITEM_HEADER__
#define __BEHAVIOR_DISPENSE_ITEM_HEADER__

#include "BM_Container_def.h"
#include "Util/Facing.h"

namespace BLOCKMAN
{

class World;
class ItemStack;

class IBehaviorDispenseItem : public ObjectAlloc
{
public:
	IBehaviorDispenseItem* itemDispenseBehaviorProvider;

	IBehaviorDispenseItem();
	virtual ~IBehaviorDispenseItem();
	/** Dispenses the specified ItemStack from a dispenser. */
	virtual ItemStackPtr dispense(World* pWorld, const Vector3i& pos, ItemStackPtr pStack) = 0;
	static Vector3 getIPositionFromBlockSource(World* pWorld, const Vector3i& pos);
};

class BehaviorDispenseItemProvider : public IBehaviorDispenseItem
{
public:
	virtual ItemStackPtr dispense(World* pWorld, const Vector3i& pos, ItemStackPtr pStack) { return pStack; }
};

class BehaviorDefaultDispenseItem : public IBehaviorDispenseItem
{
protected:
	/** Play the dispense sound from the specified block. */
	void playDispenseSound(World* pWorld, const Vector3i& pos);
	/** Order clients to display dispense particles from the specified block and facing. */
	void spawnDispenseParticles(World* pWorld, const Vector3i& pos, Facing* par2EnumFacing);
	int getWorldEventDataFrom(Facing* facing);

public:
	/** Dispenses the specified ItemStack from a dispenser. */
	virtual ItemStackPtr dispense(World* pWorld, const Vector3i& pos, ItemStackPtr pStack);
	/** Dispense the specified stack, play the dispense sound and spawn particles. */
	ItemStackPtr dispenseStack(World* pWorld, const Vector3i& pos, ItemStackPtr pStack);
	static void doDispense(World* pWorld, ItemStackPtr pStack, int par2, Facing* facing, const Vector3& pos);
};

class DispenserBehaviorPotion : public IBehaviorDispenseItem
{
protected:
	BehaviorDefaultDispenseItem* defaultDispenserItemBehavior; // = new BehaviorDefaultDispenseItem();

public:
	DispenserBehaviorPotion();
	virtual ~DispenserBehaviorPotion();

	virtual ItemStackPtr dispense(World* pWorld, const Vector3i& pos, ItemStackPtr pStack);
};


}
#endif
