/********************************************************************
filename: 	InventoryPlayer.h
file path:	dev\client\Src\Blockman\Inventory

version:	1
author:		ajohn
company:	supernano
date:		2016-11-9
*********************************************************************/
#ifndef __INVENTORY_PLAYER_HEADER__
#define __INVENTORY_PLAYER_HEADER__

#include <memory>
#include "IInventory.h"
#include "Nbt/NBT.h"
#include "Util/Listenable.h"
#include "Item/ItemStack.h"
#include "Util/SubscriptionGuard.h"

namespace BLOCKMAN
{

class EntityPlayer;
class ItemStack;
class Item;
class Block;
class InventoryProxy;
class InventoryCraftable;

class InventoryPlayer : public IInventory
{
public:
	const static int MAIN_INVENTORY_COUNT = 36;
	const static int ARMOR_INVENTORY_COUNT = 4;
	const static int HOTBAR_COUNT = 9;

private:
	InventoryProxy* m_mainInventoryProxy = nullptr;
	InventoryProxy* m_armorInventoryProxy = nullptr;
	InventoryProxy* m_armorFilteredInventoryProxy = nullptr;
	InventoryCraftable* m_inventoryAllCraftable = nullptr;
	InventoryCraftable* m_inventoryHandCraftable = nullptr;
	Listenable<ItemStackPtr>::Subscription m_currentItemChangeSubscription;
	SubscriptionGuard m_subscriptionGuard;
	SubscriptionGuard m_itemStackSubscriptionGuards[MAIN_INVENTORY_COUNT + ARMOR_INVENTORY_COUNT];
	LocalEvent m_itemInHandChangeEvent;
	LocalEvent m_inventoryChangeEvent;
	LocalEvent m_inventorySyncEvent;
	bool m_monitoredItemInHand = false;
	bool m_inventoryChanged = false;
	bool m_armorDestroyed = false;
	Listenable<bool> m_inventorySync;

	bool onItemStackChanged(size_t index, ItemStackPtr old_itemStack, ItemStackPtr new_itemStack);
	bool onCurrentItemIndexChanged(int old_index, int new_index);
	bool onInventorySync(bool old_changed, bool new_changed);
	void monitorSelectedHotbarSlot();

protected:
	/** The current ItemStack. */
	ItemStackPtr currentItemStack = nullptr;
	ItemStackPtr itemStack = nullptr;
	/** Index of the dirty inventory, client changes the inventory before server, but this change is invalid, it will make the inventory dirty */
	int inventoryDirtyIndex = -1;

public:
	/** An array of 36 item stacks indicating the main player inventory (including the visible bar). */
	Listenable<ItemStackPtr> mainInventory[MAIN_INVENTORY_COUNT];
	/** An array of 4 item stacks containing the currently worn armor pieces. */
	Listenable<ItemStackPtr> armorInventory[ARMOR_INVENTORY_COUNT];
	Listenable<ItemStackPtr> hotbar[HOTBAR_COUNT];

	ItemStackPtr prevMainInventory[MAIN_INVENTORY_COUNT];
	ItemStackPtr prevArmorInventory[ARMOR_INVENTORY_COUNT];

	/** The index of the currently held item (0-8). */
	Listenable<int> currentItemIndex;
	/** The player whose inventory this is. */
	EntityPlayer* player = nullptr;
	/** Set true whenever the inventory changes. Nothing sets it false so you will have to write your own code to check it and reset the value. */
	bool inventoryChanged = false;

protected:
	/** Returns a slot index in main inventory containing a specific itemID */
	
	int getInventorySlotContainItemAndDamage(int itemid, int subType);
	/** stores an itemstack in the users inventory */
	int storeItemStack(ItemStackPtr pStack);
	
public:
	InventoryPlayer(EntityPlayer* pPlayer);
	~InventoryPlayer();

	LocalEvent::Subscription onItemInHandChanged(std::function<void()> callback);
	LocalEvent::Subscription onInventoryContentChanged(std::function<void()> callback);
	LocalEvent::Subscription onInventoryContentSync(std::function<void()> callback);

	int getInventorySlotContainItem(int itemid);

	/** Implement override functions. */
	virtual int getSizeInventory() { return MAIN_INVENTORY_COUNT + ARMOR_INVENTORY_COUNT; }
	virtual ItemStackPtr getStackInSlot(int slot);
	virtual ItemStackInfo getItemStackInfo(int slot);
	virtual ItemStackPtr decrStackSize(int slot, int count);
	virtual ItemStackPtr getStackInSlotOnClosing(int slot);
	virtual void setInventorySlotContents(int slot, ItemStackPtr pStack);
	virtual String getInvName() { return "container.inventory"; }
	virtual bool isInvNameLocalized() { return false; }
	virtual int getInventoryStackLimit() { return 64; }
	virtual void openChest() {}
	virtual void closeChest() {}
	virtual bool isItemValidForSlot(int slot, ItemStackPtr pStack) { return true; }
	
	void clearPrev();
	/** Returns the item stack currently held by the player. */
	ItemStackPtr getCurrentItem();
	int getInventoryIndexOfCurrentItem();
	/** Get the size of the player hotbar inventory */
	static int getHotbarSize() { return HOTBAR_COUNT; }
	/** Returns the first item stack that is empty. */
	int getFirstEmptyStackInInventory();
	int getFirstEmptyStackInHotbar();
	/** Returns the last item stack that is empty. */
	int getLastEmptyStackInInventory();
	int getLastEmptyStackInHotbar();

	void putItemToHotbar(int slot);
	void removeItemFromHotbar(int slot);
	int getItemSlotByHotbat(int item);
	/** Sets a specific itemID as the current item being held (only if it exists on the hotbar)	*/
	void setCurrentItem(int itemID, int subType, bool hasSubType, bool isCreativeMode) = delete;
	void setCurrentItem(ItemStackPtr itemStack);
	void removeCurrentItemFromHotbar()
	{
		removeItemFromHotbar(currentItemIndex);
	}
	/** Switch the current item to the next one or the previous one */
	void changeCurrentItem(int idx);
	/** switch the current item to the next one */
	void changeCurrentItemToNext();
	/** Clear this player's inventory, using the specified ID and metadata as filters or -1 for no filter. */
	int clearInventory(int itemID, int subType);
	void addItemToInventory(Item* item, int subType);
	/** This function stores as many items of an ItemStack as possible in a matching slot and returns the quantity of left over items. */
	int storePartialItemStack(ItemStackPtr pStack, bool isReverse = false);
	/** Decrement the number of animations remaining. Only called on client side. This is used to handle the animation of receiving a block. */
	void decrementAnimations();
	/** removed one item of specified itemID from inventory (if it is in a stack, the stack size will reduce with 1) */
	bool consumeInventoryItem(int itemid, int num = 1);
	/** Get if a specifiied item id is inside the inventory. */
	bool hasItem(int itemid);
	bool hasItemInSlot(int slot);
	ItemStackPtr getArrowItemStack();
	ItemStackPtr getBulletItemStack(int id);
	/** Adds the item stack to the inventory, returns false if it is impossible. */
	bool addItemStackToInventory(ItemStackPtr pStack, bool isReverse = false);
	bool canItemStackAddedToInventory(const ItemStackPtr pStack);
	/** Gets the strength of the current item (tool) against the specified block, 1.0f if not holding anything. */
	float getStrVsBlock(Block* pBlock);
	/** Writes the inventory out as a list of compound tags. This is where the slot indices are used (+100 for armor, +80	* for crafting). */
	NBTTagList* writeToNBT(NBTTagList* pNBTList);
	/** Reads from the given tag list and fills the slots in the inventory with the correct items. */
	void readFromNBT(NBTTagList* pNBTList);
	/** Returns whether the current item (tool) can harvest from the specified block . */
	bool canHarvestBlock(Block* pBlock);
	/** returns a player armor item (as itemstack) contained in specified armor slot. */
	ItemStackPtr armorItemInSlot(int slot) { return armorInventory[slot]; }
	/** Based on the damage values and maximum damage values of each armor item, returns the current armor value. */
	int getTotalArmorValue();
	/** Damages armor in each slot by the specified amount. */
	void damageArmor(float damage);
	bool equipArmor(ItemStackPtr armorStack);
	bool unequipArmor(ItemStackPtr armorStack);
	void destroyArmor(int slotIndex);
	/** Drop all armor and main inventory items. */
	void dropAllItems();
	/** Called when an the contents of an Inventory change, usually */
	void onInventoryChanged() { inventoryChanged = true; }
	void setItemStack(ItemStackPtr pStack) { itemStack = pStack; }
	ItemStackPtr getItemStack() { return itemStack; }
	/** Do not make give this method the name canInteractWith because it clashes with Container */
	bool isUseableByPlayer(EntityPlayer* pPlayer);
	/** Returns true if the specified ItemStack exists in the inventory. */
	bool hasItemStack(ItemStackPtr pStack);
	/** Copy the ItemStack contents from another InventoryPlayer instance */
	void copyInventory(InventoryPlayer& inventory);
	ItemStackPtr getFirstSpareGun(int bulletItemId);
	ItemStackPtr getKeyById(int keyId);
	void updateInventoryCd(float fDeltaTime);
	bool isCanAddItem(int itemId , int itemMeta, int addNum);
	void setInventorySync(bool inventorySync) { m_inventorySync = inventorySync; }
	bool notifyUpdateToProxy();
	IInventory* getMainInventory();
	IInventory* getArmorInventory();
	IInventory* getArmorFilteredInventory();
	InventoryCraftable* getInventoryAllCraftable();
	InventoryCraftable* getInventoryHandCraftable();
	void onUpdate();
	int getInventoryDirtyIndex() { return inventoryDirtyIndex; }
	void setInventoryDirtyIndex(int index) { inventoryDirtyIndex = index; }
	ItemStackPtr getItemIdBySlot(int nSlot);
	/** Get if the inventory contains a item with this id and meta. */
	bool isContainItem(int itemId, int itemMeta);
	bool isEnchantment(int slot);
	int getItemStackEnchantmentId(int slot, int enchantmentIndex);
	int getItemStackEnchantmentLv(int slot, int enchantmentIndex);
};


}

#endif