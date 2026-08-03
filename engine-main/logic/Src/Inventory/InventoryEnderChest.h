/********************************************************************
filename: 	InventoryEnderChest.h
file path:	dev\client\Src\Blockman\Inventory

version:	1
author:		ajohn
company:	supernano
date:		2016-11-9
*********************************************************************/
#ifndef __INVENTORY_ENDERCHEST_HEADER__
#define __INVENTORY_ENDERCHEST_HEADER__

#include "IInventory.h"
#include "Nbt/NBT.h"

namespace BLOCKMAN
{

class InventoryBasic;
class TileEntityEnderChest;

class IInvBasic
{
public:
	/** Called by InventoryBasic.onInventoryChanged() on a array that is never filled.*/
	virtual void onInventoryChanged(InventoryBasic* pInventory) = 0;
};

typedef list<IInvBasic*>::type InventoryListeners;

class InventoryBasic : public IInventory
{
protected:
	String inventoryTitle;
	int slotsCount = 0;
	ItemStackPtr* inventoryContents = nullptr;
	ItemStackPtr* prevInventoryContents = nullptr;
	InventoryListeners changeListeners;
	bool hasCustomName = false;

public:
	InventoryBasic(const String& title, bool customName, int slotscount);
	~InventoryBasic();

	void addInventoryContent(IInvBasic* pInvBasic) { changeListeners.push_back(pInvBasic); }
	void removeInventoryContent(IInvBasic* pInvBasic) { changeListeners.remove(pInvBasic); }
	
	/** Implment override function. */
	virtual ItemStackPtr getStackInSlot(int slot);
	virtual ItemStackPtr getPrevStackInSlot(int slot);
	virtual ItemStackPtr decrStackSize(int slot, int size);
	virtual ItemStackPtr getStackInSlotOnClosing(int slot);
	virtual void setInventorySlotContents(int slot, ItemStackPtr pStack);
	virtual void setPrevInventorySlotContents(int slot, ItemStackPtr pStack);
	virtual int getSizeInventory() { return slotsCount; }
	virtual String getInvName() { return inventoryTitle; }
	virtual bool isInvNameLocalized() { return hasCustomName; }
	virtual void setCustomTitle(const String& title);
	virtual int getInventoryStackLimit() { return 64; }
	virtual bool isUseableByPlayer(EntityPlayer* pPlayer) { return true; }
	virtual void onInventoryChanged();
	virtual void openChest() {}
	virtual void closeChest() {}
	virtual bool isItemValidForSlot(int slot, ItemStackPtr pStack) { return true; }
};

class InventoryEnderChest : public InventoryBasic
{
protected:
	TileEntityEnderChest* associatedChest;

public:
	InventoryEnderChest();

	void setAssociatedChest(TileEntityEnderChest* pChest) { associatedChest = pChest; }

	/** Implment override function from IInventory. */
	virtual void loadInventoryFromNBT(NBTTagList* par1NBTTagList);
	virtual NBTTagList* saveInventoryToNBT();
	virtual bool isUseableByPlayer(EntityPlayer* pPlayer);
	virtual void openChest();
	virtual void closeChest();
	virtual bool isItemValidForSlot(int slot, ItemStackPtr pStack) { return true; }
	virtual ItemStackInfo getItemStackInfo(int slot);
};

}

#endif