/********************************************************************
filename: 	IInventory.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-11-9
*********************************************************************/
#ifndef __INTERFACE_INVENTORY_HEADER__
#define __INTERFACE_INVENTORY_HEADER__

#include "Core.h"
#include "Item/ItemStack.h"
using namespace LORD;

namespace BLOCKMAN
{
class ItemStack;
class EntityPlayer;
class World;

class IInventory : public ObjectAlloc
{
public:
	virtual ~IInventory() {};
	/** Returns the number of slots in the inventory. */
	virtual int getSizeInventory() = 0;
	/** Returns the stack in slot i */
	virtual ItemStackPtr getStackInSlot(int slot) = 0;
	/** Returns the previous stack in slot i */
	virtual ItemStackPtr getPrevStackInSlot(int slot) { return nullptr; }
	/** Removes from an inventory slot (first arg) up to a specified number (second arg) of items and returns them in a new stack. */
	virtual ItemStackPtr decrStackSize(int slot, int count) = 0;
	/** When some containers are closed they call this on each slot, then drop whatever it returns as an EntityItem */
	virtual ItemStackPtr getStackInSlotOnClosing(int slot) = 0;
	/** Sets the given item stack to the specified slot in the inventory (can be crafting or armor sections). */
	virtual void setInventorySlotContents(int slot, ItemStackPtr pStack) = 0;
	/** Sets the given item stack to the specified slot in the previous inventory */
	virtual void setPrevInventorySlotContents(int slot, ItemStackPtr pStack) {}
	/** Returns the name of the inventory.*/
	virtual String getInvName() = 0;
	/** If this returns false, the inventory name will be used as an unlocalized name */
	virtual bool isInvNameLocalized() = 0;
	/** Returns the maximum stack size for a inventory slot. Seems to always be 64, possibly will be extended. */
	virtual int getInventoryStackLimit() = 0;
	/** Called when an the contents of an Inventory change, usually */
	virtual void onInventoryChanged() {}
	/** Do not make give this method the name canInteractWith because it clashes with Container */
	virtual bool isUseableByPlayer(EntityPlayer* pPlayer) = 0;
	virtual void openChest() = 0;
	virtual void closeChest() = 0;
	/** Returns true if automation is allowed to insert the given stack (ignoring stack size) into the given slot. */
	virtual bool isItemValidForSlot(int slot, ItemStackPtr pStack) = 0;
	virtual void swapStacksInSlots(int slot1, int slot2);
	virtual int findItemStack(ItemStackPtr itemStack);
	virtual ItemStackPtr findCanSaveItemStackById(int itemId, int itemMeta);

	virtual int getItemNum(int id);
	virtual void removeItem(int id, int num);
	virtual void clear();
};

class ISidedInventory : public IInventory
{
public:
	/** Returns an array containing the indices of the slots that can be accessed by automation on the given side of this block. */
	virtual int* getAccessibleSlotsFromSide(int side, int& outSize) = 0;
	/** Returns true if automation can insert the given item in the given slot from the given side.*/
	virtual bool canInsertItem(int slot, ItemStackPtr pStack, int side) = 0;
	/** Returns true if automation can extract the given item in the given slot from the given side.*/
	virtual bool canExtractItem(int slot, ItemStackPtr pStack, int side) = 0;
};

class IHopper : public IInventory
{
public:
	/** Returns the worldObj for this tileEntity. */
	virtual World* getWorldObj() = 0;
	/** Gets the world position for this hopper entity. */
	virtual Vector3 getPos() = 0;
};

}

#endif
