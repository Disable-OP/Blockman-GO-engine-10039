/********************************************************************
filename: 	Container.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-11-9
*********************************************************************/
#ifndef __CONTAINER_HEADER__
#define __CONTAINER_HEADER__

#include "BM_Container_def.h"
#include "Core.h"
using namespace LORD;

namespace BLOCKMAN
{

class Slot;
class ICrafting;
class EntityPlayer;
class IInventory;
class InventoryPlayer;
class InventoryCrafting;

class Container : public ObjectAlloc
{
public:
	typedef vector<Slot*>::type SlotsArr;
	typedef set<Slot*>::type SlotsSet;
	typedef list<ICrafting*>::type CraftingArr;
	typedef set<EntityPlayer*>::type PlayerSet;

	/** the list of all items(stacks) for the corresponding slot */
	ItemStackArr inventoryItemStacks;	// = new ArrayList();
	/** the list of all slots in the inventory */
	SlotsArr inventorySlots;			// = new ArrayList();
	int windowId = 0;
	/*** list of all people that need to be notified when this craftinventory changes */
	CraftingArr crafters;				// = new ArrayList();
	PlayerSet playerList;				// = new HashSet();

protected:
	i16 transactionID = 0;
	int dragMode = -1;					// = -1;
	int dragEvent = 0;
	SlotsSet dragSlots;					// = new HashSet();

protected:
	/** the slot is assumed empty */
	Slot* addSlotToContainer(Slot* pSlot);
	void retrySlotClick(int slot, int dragType, bool par3, EntityPlayer* pPlayer);
	/** merges provided ItemStack with the first avaliable one in the container/player inventory */
	bool mergeItemStack(ItemStackPtr pStack, int start, int end, bool reverse);
	void resetDrag() { dragEvent = 0; dragSlots.clear(); }

public:
	Container();
	virtual ~Container();
	
	void addCraftingToCrafters(ICrafting* pCrafting);
	/** Remove this crafting listener from the listener list. */
	void removeCraftingFromCrafters(ICrafting* pCrafting);
	/** returns a list if itemStacks, for each slot. */
	void getInventory(ItemStackArr& fillto);
	/** Looks for changes made in the container, sends them to every listener. */
	void detectAndSendChanges();
	/** enchants the item on the table using the specified slot; also deducts XP from player */
	bool enchantItem(EntityPlayer* pPlayer, int slot) { return false; }
	Slot* getSlotFromInventory(IInventory* pInventory, int slot);
	Slot* getSlot(int slot) { return inventorySlots[slot]; }
	/** Called when a player shift-clicks on a slot. You must override this or you will crash when someone does that. */
	virtual ItemStackPtr transferStackInSlot(EntityPlayer* pPlayer, int slot);
	ItemStackPtr slotClick(int slot, int dragType, int clickType, EntityPlayer* pPlayer);
	virtual bool canMergeSlot(ItemStackPtr pStack, Slot* pSlot) { return true; }
	/** Called when the container is closed. */
	virtual void onContainerClosed(EntityPlayer* pPlayer);
	/** Callback for when the crafting matrix is changed. */
	virtual void onCraftMatrixChanged(IInventory* pIInventory) { detectAndSendChanges(); }
	/** args: slotID, itemStack to put in slot */
	void putStackInSlot(int slot, ItemStackPtr pStack);
	/** places itemstacks in first x slots, x being aitemstack.lenght */
	void putStacksInSlots(const ItemStackArr& stacks);
	void updateProgressBar(int par1, int par2) {}
	/** Gets a unique transaction ID. Parameter is unused. */
	i16 getNextTransactionID(InventoryPlayer* pInvPlayer);
	/** NotUsing because adding a player twice is an error */
	bool isPlayerNotUsingContainer(EntityPlayer* pPlayer);
	/** adds or removes the player from the container based on par2 */
	void setPlayerIsPresent(EntityPlayer* pPlayer, bool remove);
	virtual bool canInteractWith(EntityPlayer* pPlayer) = 0;
	/** Returns true if the player can "drag-spilt" items into this slot,. returns true by default. */
	bool canDragIntoSlot(Slot* pSlot) { return true; }

	static int extractDragMode(int dragmode) { return dragmode >> 2 & 3; }
	static int getDragEvent(int clickedButton) { return clickedButton & 3; }
	static int getQuickcraftMask(int par0, int par1) { return par0 & 3 | (par1 & 3) << 2; }
	static bool isValidDragMode(int par0) { return par0 == 0 || par0 == 1; }
	static bool canAddItemToSlot(Slot* pSlot, ItemStackPtr pStack, bool par2);
	static void computeStackSize(SlotsSet& slots, int dragmode, ItemStackPtr pstack, int size);
	static int calcRedstoneFromInventory(IInventory* pInventory);
};

class ContainerPlayer : public Container
{
protected:
	EntityPlayer* thePlayer = nullptr;
public:
	/** The crafting matrix inventory. */
	InventoryCrafting* craftMatrix = nullptr; // = new InventoryCrafting(this, 2, 2);
	IInventory* craftResult = nullptr;		// = new InventoryCraftResult();
	/** Determines if inventory manipulation should be handled. */
	bool isLocalWorld = false;

public:
	ContainerPlayer(InventoryPlayer* pInvPlayer, bool isLocal, EntityPlayer* pPlayer);
	~ContainerPlayer();
	
	/** implement override functions from Container. */
	virtual void onCraftMatrixChanged(IInventory* pInventory);
	virtual void onContainerClosed(EntityPlayer* pPlayer);
	virtual bool canInteractWith(EntityPlayer* pPlayer) { return true; }
	virtual ItemStackPtr transferStackInSlot(EntityPlayer* pPlayer, int slot);
	virtual bool canMergeSlot(ItemStackPtr pStack, Slot* pSlot);
};

class ContainerChest : public Container
{
protected:
	IInventory* lowerChestInventory = nullptr;
	int numRows = 0;

public:
	ContainerChest(IInventory* inventory1, IInventory* inventory2);

	/** implement override functions from Container. */
	bool canInteractWith(EntityPlayer* pPlayer) override;
	ItemStackPtr transferStackInSlot(EntityPlayer* pPlayer, int slot) override;
	void onContainerClosed(EntityPlayer* pPlayer) override;

	/** Return this chest container's lower chest inventory. */
	IInventory* getLowerChestInventory() { return lowerChestInventory; }
};

}

#endif