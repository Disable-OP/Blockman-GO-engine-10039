/********************************************************************
filename: 	InventoryCrafting.h
file path:	dev\client\Src\Blockman\Inventory

version:	1
author:		ajohn
company:	supernano
date:		2017-02-27
*********************************************************************/
#ifndef __INVENTORY_CRAFTING_HEADER__
#define __INVENTORY_CRAFTING_HEADER__

#include "IInventory.h"
#include "BM_Container_def.h"

namespace BLOCKMAN
{

class Container;

class InventoryCrafting : public IInventory
{
	typedef vector<ItemStackPtr>::type StackVector;
protected:
	/** vector of the stacks in the crafting matrix. */
	StackVector stackVector;
	int stackVectorLen = 0;
	/** the width of the crafting inventory */
	int inventoryWidth = 0;
	/** Class containing the callbacks for the events on_GUIClosed and on_CraftMaxtrixChanged.*/
	Container* eventHandler = nullptr;

public:
	InventoryCrafting(Container* pContainer, int width, int height);
	~InventoryCrafting();

	ItemStackPtr getStackInRowAndColumn(int xPos, int yPos);

	/** Implment override functions from IInventory. */
	virtual int getSizeInventory() { return stackVectorLen; }
	virtual ItemStackPtr getStackInSlot(int slot) { return slot >= stackVectorLen ? NULL : stackVector[slot]; }
	virtual String getInvName();
	virtual bool isInvNameLocalized() { return false; }
	virtual ItemStackPtr getStackInSlotOnClosing(int slot);
	virtual ItemStackPtr decrStackSize(int slot, int size);
	virtual void setInventorySlotContents(int slot, ItemStackPtr pStack);
	virtual int getInventoryStackLimit() { return 64; }
	virtual void onInventoryChanged() {}
	virtual bool isUseableByPlayer(EntityPlayer* pPlayer) { return true; }
	virtual void openChest() {}
	virtual void closeChest() {}
	virtual bool isItemValidForSlot(int slot, ItemStackPtr pStack) { return true; }
};

class InventoryCraftResult : public IInventory
{
protected:
	/** A list of one item containing the result of the crafting formula */
	ItemStackPtr stackResult = nullptr;// = LORD::make_shared<ItemStack>[1];

public:
	InventoryCraftResult();
	~InventoryCraftResult();

	/** Implment override functions from IInventory. */
	virtual int getSizeInventory() { return 1; }
	virtual ItemStackPtr getStackInSlot(int slot) { return stackResult; }
	virtual String getInvName() { return "Result"; }
	virtual bool isInvNameLocalized() { return false; }
	virtual ItemStackPtr decrStackSize(int slot, int size);
	virtual ItemStackPtr getStackInSlotOnClosing(int slot);
	virtual void setInventorySlotContents(int slot, ItemStackPtr pStack) { stackResult = pStack; }
	virtual int getInventoryStackLimit() { return 64; }
	virtual void onInventoryChanged() {}
	virtual bool isUseableByPlayer(EntityPlayer* pPlayer) { return true; }
	virtual void openChest() {}
	virtual void closeChest() {}
	virtual bool isItemValidForSlot(int slot, ItemStackPtr pStack) { return true; }
};

}

#endif
