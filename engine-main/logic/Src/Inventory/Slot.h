/********************************************************************
filename: 	Slot.h
file path:	dev\client\Src\Blockman\Inventory

version:	1
author:		ajohn
company:	supernano
date:		2017-02-27
*********************************************************************/
#ifndef __SLOT_HEADER__
#define __SLOT_HEADER__

#include "BM_Container_def.h"
#include "Core.h"
using namespace LORD;

namespace BLOCKMAN
{

class IInventory;
class ItemStack;
class ContainerPlayer;
class EntityPlayer;

class Slot : public ObjectAlloc
{
protected:
	/** The index of the slot in the inventory. */
	int slotIndex = 0;

public:
	/** The inventory we want to extract a slot from. */
	IInventory* inventory = nullptr;
	/** the id of the slot(also the index in the inventory arraylist) */
	int slotNumber = 0;
	/** display position of the inventory slot on the screen x axis */
	int xDisplayPosition = 0;
	/** display position of the inventory slot on the screen y axis */
	int yDisplayPosition = 0;

protected:
	/*** the itemStack passed in is the output - ie, iron ingots, and pickaxes, not ore and wood. */
	virtual void onCrafting(ItemStackPtr pStack, int size) {}
	/** the itemStack passed in is the output - ie, iron ingots, and pickaxes, not ore and wood.*/
	virtual void onCrafting(ItemStackPtr pStack) {}

public:
	Slot(IInventory* pInventory, int slotIdx, int xPos, int yPos);
	
	/** if par2 has more items than par1, onCrafting(item,countIncrease) is called */
	void onSlotChange(ItemStackPtr pStack1, ItemStackPtr pStack2);
	virtual void onPickupFromSlot(EntityPlayer* pPlayer, ItemStackPtr pStack) { onSlotChanged(); }
	/** Check if the stack is a valid item for this slot. Always true beside for the armor slots. */
	virtual bool isItemValid(ItemStackPtr pItemStack) { return true; }
	/** Helper fnct to get the stack in the slot. */
	ItemStackPtr getStack();
	/** Returns if this slot contains a stack. */
	bool getHasStack() { return getStack() != NULL; }
	/** Helper method to put a stack in the slot. */
	void putStack(ItemStackPtr pStack);
	void onSlotChanged();
	/** Returns the maximum stack size for a given slot (usually the same as getInventoryStackLimit(), but 1 in the case of armor slots) */
	virtual int getSlotStackLimit();
	/** Decrease the size of the stack in slot (first int arg) by the amount of the second int arg. Returns the new stack. */
	virtual ItemStackPtr decrStackSize(int size);
	/** returns true if this slot is in par2 of par1 */
	bool isSlotInInventory(IInventory* pInventory, int slot) { return pInventory == inventory && slot == slotIndex; }
	/** Return whether this slot's stack can be taken from this slot. */
	bool canTakeStack(EntityPlayer* pPlayer) { return true; }
	bool canBeHovered() { return true; }
};

class SlotCrafting : public Slot
{
protected:
	/** The craft matrix inventory linked to this result slot. */
	IInventory* craftMatrix = nullptr;
	/** The player that is using the GUI where this slot resides. */
	EntityPlayer* thePlayer = nullptr;
	/** The number of items that have been crafted so far. Gets passed to ItemStack.onCrafting before being reset. */
	int amountCrafted = 0;

protected:
	/** Implement override function from Slot */
	virtual void onCrafting(ItemStackPtr pStack, int size);
	virtual void onCrafting(ItemStackPtr pStack);
	
public:
	SlotCrafting(EntityPlayer* pPlayer, IInventory* pInventory1, IInventory* pInventory2, int slotIdx, int xPos, int yPos);
	
	/** Implement override function from Slot */
	virtual bool isItemValid(ItemStackPtr pStack) { return false; }
	virtual ItemStackPtr decrStackSize(int size);
	virtual void onPickupFromSlot(EntityPlayer* pPlayer, ItemStackPtr pStack);
};

class SlotArmor : public Slot
{
public:
	/** The armor type that can be placed on that slot, it uses the same values of armorType field on ItemArmor. */
	int armorType = 0;
	/** The parent class of this clot, ContainerPlayer, SlotArmor is a Anon inner class. */
	ContainerPlayer* parent = nullptr;

public:
	SlotArmor(ContainerPlayer* pContainerPlayer, IInventory* pInventory, int slotIdx, int xPos, int yPos, int armortype);
	
	/** Implement override function from Slot */
	virtual int getSlotStackLimit() { return 1; }
	virtual bool isItemValid(ItemStackPtr pStack);
};


}

#endif