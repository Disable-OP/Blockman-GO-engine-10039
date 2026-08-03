/********************************************************************
filename: 	InventoryLargeChest.h
file path:	dev\client\Src\Blockman\Inventory

version:	1
author:		ajohn
company:	supernano
date:		2017-02-27
*********************************************************************/
#ifndef __INVENTORY_LARGE_CHEST_HEADER__
#define __INVENTORY_LARGE_CHEST_HEADER__

#include "IInventory.h"

namespace BLOCKMAN
{

class InventoryLargeChest : public IInventory
{
protected:
	/** Name of the chest. */
	String name;
	/** Inventory object corresponding to double chest upper part */
	IInventory* upperChest = nullptr;
	/** Inventory object corresponding to double chest lower part */
	IInventory* lowerChest = nullptr;

public:
	InventoryLargeChest(const String& nameIn, IInventory* upper, IInventory* lower);
	bool isPartOfLargeChest(IInventory* pInventory) { return upperChest == pInventory || lowerChest == pInventory; }

	/** Implment override function from IInventory. */
	virtual int getSizeInventory() { return upperChest->getSizeInventory() + lowerChest->getSizeInventory(); }
	virtual String getInvName();
	virtual bool isInvNameLocalized() { return upperChest->isInvNameLocalized() || lowerChest->isInvNameLocalized(); }
	virtual ItemStackPtr getStackInSlot(int slot);
	virtual ItemStackPtr decrStackSize(int slot, int size);
	virtual ItemStackPtr getStackInSlotOnClosing(int slot);
	virtual void setInventorySlotContents(int slot, ItemStackPtr pStack);
	virtual int getInventoryStackLimit() { return upperChest->getInventoryStackLimit(); }
	virtual void onInventoryChanged();
	virtual bool isUseableByPlayer(EntityPlayer* pPlayer);
	virtual void openChest();
	virtual void closeChest();
	virtual bool isItemValidForSlot(int slot, ItemStackPtr pStack) { return true; }
};

}

#endif