/********************************************************************
filename: 	TileEntityDispenser.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-11-3
*********************************************************************/
#ifndef __TILE_ENTITY_DISPENSER_HEADER__
#define __TILE_ENTITY_DISPENSER_HEADER__

#include "Inventory/IInventory.h"
#include "TileEntity.h"
#include "Util/Random.h"
#include "Setting/GunSetting.h"
#include "Setting/BulletClipSetting.h"

namespace BLOCKMAN
{
class ItemStack;
class Potion;
class EntityItem;

class TileEntityChest : public TileEntity, public IInventory
{
	RTTI_DECLARE(TileEntityChest);
protected:
	static const int CHEST_CAPACITY = 36;
	ItemStackPtr chestContents[CHEST_CAPACITY];//  = LORD::make_shared<ItemStack>[36];
											 /** Server sync counter (once per 20 ticks) */
	ItemStackPtr prevChestContents[CHEST_CAPACITY];
	int ticksSinceSync = 0;
	int cachedChestType = 0;
	String customTitleName;

public:
	static const int CHEST_INVSIZE = 36;
	/** Determines if the check for adjacent chests has taken place. */
	bool adjacentChestChecked = false;
	/** Contains the chest tile located adjacent to this one (if any) */
	TileEntityChest* adjacentChestZNeg = nullptr;
	/** Contains the chest tile located adjacent to this one (if any) */
	TileEntityChest* adjacentChestXPos = nullptr;
	/** Contains the chest tile located adjacent to this one (if any) */
	TileEntityChest* adjacentChestXNeg = nullptr;
	/** Contains the chest tile located adjacent to this one (if any) */
	TileEntityChest* adjacentChestZPos = nullptr;
	/** The current angle of the lid (between 0 and 1) */
	float lidAngle = 0.f;
	/** The angle of the lid last tick */
	float prevLidAngle = 0.f;
	/** The number of players currently using this chest */
	int numUsingPlayers = 0;
	vector<int>::type openChestPlayers;

protected:
	/** connect neighbor chest */
	void setNeighbor(TileEntityChest* pChest, int side);
	bool isChestAt(const BlockPos& pos);

public:
	TileEntityChest(int chestType = -1);
	virtual ~TileEntityChest();

	/** Sets the custom display name to use when opening a GUI for this specific TileEntityChest. */
	void setChestGuiName(const String& title) { customTitleName = title; }
	/** Performs the check for adjacent chests to determine if this chest is double or not. */
	void checkForAdjacentChests();
	int getChestType();
	int getBlockMetadata();
	void changeChestState(bool isOpen);
	void initByItem(int slot, int itemId, int itemNum, int itemMeta);
	void initByGunItem(int slot, int itemId, int itemNum, int itemMeta, int bullets);
	void addEchantmentItem(int slot, int itemId, int itemNum, int itemMeta, std::vector<std::vector<int>> enchantments);

	void clearChest();
	/** implement override functions from Inventory */
	virtual int getSizeInventory() { return CHEST_INVSIZE; }
	virtual ItemStackPtr getStackInSlot(int slot) { LordAssert(slot >= 0 && slot < CHEST_INVSIZE); return chestContents[slot]; }
	virtual ItemStackPtr getPrevStackInSlot(int slot) { LordAssert(slot >= 0 && slot < CHEST_INVSIZE); return prevChestContents[slot]; }
	virtual ItemStackPtr decrStackSize(int slot, int size);
	virtual ItemStackPtr getStackInSlotOnClosing(int slot);
	virtual void setInventorySlotContents(int slot, ItemStackPtr pStack);
	virtual void setPrevInventorySlotContents(int slot, ItemStackPtr pStack);
	virtual String getInvName() { return isInvNameLocalized() ? customTitleName : "container.chest"; }
	virtual bool isInvNameLocalized() { return customTitleName.length() > 0; }
	virtual int getInventoryStackLimit() { return 64; }
	virtual bool isUseableByPlayer(EntityPlayer* pPlayer);
	virtual void openChest();
	virtual void closeChest();
	virtual bool isChestOpen();
	virtual bool isItemValidForSlot(int slot, ItemStackPtr pStack) { return true; }
	virtual void onInventoryChanged();

	/** implement override functions from TileEntity */
	virtual void readFromNBT(NBTTagCompound* pNbtCompound);
	virtual void writeToNBT(NBTTagCompound* pNbtCompound);
	virtual void updateContainingBlockInfo();
	virtual void updateEntity();
	virtual bool receiveClientEvent(int id, int type);
	virtual void invalidate();
};


class TileEntityFurnace : public TileEntity, public ISidedInventory
{
	RTTI_DECLARE(TileEntityFurnace);
protected:
	static int slots_top[1];	// = new int[] {0};
	static int slots_bottom[2];	// = new int[] {2, 1};
	static int slots_sides[1];	// = new int[] {1};

								/** The ItemStacks that hold the items currently being used in the furnace */
	static const int FURNACE_INVSIZE = 3;
	ItemStackPtr furnaceItemStacks[FURNACE_INVSIZE]; // = LORD::make_shared<ItemStack>[3];
	String customTitleName;

public:
	/** The number of ticks that the furnace will keep burning */
	int furnaceBurnTime = 0;
	/** The number of ticks that a fresh copy of the currently-burning item would keep the furnace burning for */
	int currentItemBurnTime = 0;
	/** The number of ticks that the current item has been cooking for */
	int furnaceCookTime = 0;

public:
	TileEntityFurnace();
	virtual ~TileEntityFurnace();

	/** Sets the custom display name to use when opening a GUI linked to this tile entity.*/
	void setGuiDisplayName(const String& name) { customTitleName = name; }
	/** Returns an integer between 0 and the passed value representing how close the current item is to being completely cooked */
	int getCookProgressScaled(int cooktime) { return furnaceCookTime * cooktime / 200; }
	/*** Returns an integer between 0 and the passed value representing how much burn time is left on the current fuel item */
	int getBurnTimeRemainingScaled(int cooktime);
	/** Returns true if the furnace is currently burning */
	bool isBurning() { return furnaceBurnTime > 0; }
	/** Returns true if the furnace can smelt an item, i.e. has a source item, destination stack isn't full, etc. */
	bool canSmelt();
	/** Turn one item from the furnace source stack into the appropriate smelted item in the furnace result stack */
	void smeltItem();
	/** Returns the number of ticks that the supplied fuel item will keep the furnace burning, or 0 if the item isn't fuel */
	static int getItemBurnTime(ItemStackPtr pStack);
	/** Return true if item is a fuel source (getItemBurnTime() > 0). */
	static bool isItemFuel(ItemStackPtr pStack) { return getItemBurnTime(pStack) > 0; }

	/** implement override functions from Inventory */
	virtual int getSizeInventory() { return FURNACE_INVSIZE; }
	virtual ItemStackPtr getStackInSlot(int slot) { LordAssert(slot >= 0 && slot < FURNACE_INVSIZE); return furnaceItemStacks[slot]; }
	virtual ItemStackPtr decrStackSize(int slot, int size);
	virtual ItemStackPtr getStackInSlotOnClosing(int slot);
	virtual void setInventorySlotContents(int slot, ItemStackPtr pStack);
	virtual String getInvName() { return isInvNameLocalized() ? customTitleName : "container.furnace"; }
	virtual bool isInvNameLocalized() { return customTitleName.length() > 0; }
	virtual int getInventoryStackLimit() { return 64; }
	virtual bool isUseableByPlayer(EntityPlayer* pPlayer);
	virtual void onInventoryChanged();
	virtual void openChest() {}
	virtual void closeChest() {}
	virtual bool isItemValidForSlot(int slot, ItemStackPtr pStack);

	/** implement override functions from ISidedInventory */
	virtual int* getAccessibleSlotsFromSide(int side, int& outsize);
	virtual bool canInsertItem(int slot, ItemStackPtr pStack, int side) { return isItemValidForSlot(slot, pStack); }
	virtual bool canExtractItem(int slot, ItemStackPtr pStack, int side);

	/** implement override functions from TileEntity */
	virtual void readFromNBT(NBTTagCompound* pNbtCompound);
	virtual void writeToNBT(NBTTagCompound* pNbtCompound);
	virtual void updateEntity();

};

class TileEntityDispenser : public TileEntity, public IInventory
{
	RTTI_DECLARE(TileEntityDispenser);
public:
	TileEntityDispenser(int id = TILE_ENTITY_TRAP);
	virtual ~TileEntityDispenser();

	int getRandomStackFromInventory();

	/** Add item stack in first available inventory slot */
	int addItem(ItemStackPtr pStack);

	void setCustomName(const String& par1Str) { customName = par1Str; }

	/** implement override functions from Inventory */
	virtual int getSizeInventory() { return 9; }
	virtual ItemStackPtr getStackInSlot(int slot) { /*LordAssert(dispenserContents);*/ if (dispenserContents) return dispenserContents[slot]; else return nullptr; }
	virtual ItemStackPtr decrStackSize(int slot, int size);
	virtual ItemStackPtr getStackInSlotOnClosing(int slot);
	virtual void setInventorySlotContents(int slot, ItemStackPtr pStack);
	virtual String getInvName() { return isInvNameLocalized() ? customName : "container.dispenser"; }
	virtual bool isInvNameLocalized() { return customName != StringUtil::BLANK; }
	virtual int getInventoryStackLimit() { return 64; }
	virtual bool isUseableByPlayer(EntityPlayer* pPlayer);
	virtual void openChest() {}
	virtual void closeChest() {}
	virtual bool isItemValidForSlot(int slot, ItemStackPtr pStack) { return true; }

	/** implement override functions from TileEntity */
	virtual void readFromNBT(NBTTagCompound* par1NBTTagCompound);
	virtual void writeToNBT(NBTTagCompound* par1NBTTagCompound);


protected:
	ItemStackPtr* dispenserContents = nullptr;

	/** random number generator for instance. Used in random item stack selection. */
	Random dispenserRandom;
	String customName;

};

class TileEntityDropper : public TileEntityDispenser
{
	RTTI_DECLARE(TileEntityDropper);
public:
	TileEntityDropper();

	/** implement override functions from Inventory */
	virtual String getInvName() { return isInvNameLocalized() ? customName : "container.dropper"; }
};

class TileEntityBrewingStand : public TileEntity, public ISidedInventory
{
	RTTI_DECLARE(TileEntityBrewingStand);
protected:
	static int SLOTS_FOR_UP[3];
	static int SLOTS_FOR_DOWN[3]; // = new int[] {0, 1, 2};

	static const int BREW_COUNT = 4;
	/** The itemstacks currently placed in the slots of the brewing stand */
	ItemStackPtr brewingItemStacks[BREW_COUNT]; // = LORD::make_shared<ItemStack>[4];
	int brewTime = 0;

	/** an integer with each bit specifying whether that slot of the stand contains a potion */
	int filledSlots = 0;
	int ingredientID = 0;
	String customName;

public:
	TileEntityBrewingStand();
	virtual ~TileEntityBrewingStand();

	/** implement override functions from Inventory */
	virtual int getSizeInventory() { return BREW_COUNT; }
	virtual ItemStackPtr getStackInSlot(int slot) { return slot >= 0 && slot < BREW_COUNT ? brewingItemStacks[slot] : NULL; }
	virtual ItemStackPtr decrStackSize(int slot, int size);
	virtual ItemStackPtr getStackInSlotOnClosing(int slot);
	virtual void setInventorySlotContents(int slot, ItemStackPtr pStack);
	virtual String getInvName() { return isInvNameLocalized() ? customName : "container.brewing"; }
	virtual bool isInvNameLocalized() { return !customName.empty(); }
	virtual int getInventoryStackLimit() { return 64; }
	virtual bool isUseableByPlayer(EntityPlayer* pPlayer);
	virtual void openChest() {}
	virtual void closeChest() {}
	virtual bool isItemValidForSlot(int slot, ItemStackPtr pStack);

	/** implement override functions from ISidedInventory */
	virtual bool canInsertItem(int slot, ItemStackPtr pStack, int side) { return isItemValidForSlot(slot, pStack); }
	virtual int* getAccessibleSlotsFromSide(int side, int& outsize);
	virtual bool canExtractItem(int slot, ItemStackPtr pStack, int side) { return true; }

	/** implement override functions from TileEntity */
	virtual void updateEntity();
	virtual void readFromNBT(NBTTagCompound* pNbt);
	virtual void writeToNBT(NBTTagCompound* pNbt);

	void setCustomName(const String& name) { customName = name; }
	int getBrewTime() { return brewTime; }
	void setBrewTime(int time) { brewTime = time; }

	/** returns an integer with each bit specifying wether that slot of the stand contains a potion */
	int getFilledSlots();

protected:
	bool canBrew();
	void brewPotions();

	/** The result of brewing a potion of the specified damage value with an ingredient itemstack. */
	int getPotionResult(int damageType, ItemStackPtr pStack);
};

class TileEntityBeacon : public TileEntity, public IInventory
{
	RTTI_DECLARE(TileEntityBeacon);
public:
	/** List of effects that Beacon can apply */
	static Potion* effectsList[4][2]; // = new Potion[][]{ { Potion.moveSpeed, Potion.digSpeed },{ Potion.resistance, Potion.jump },{ Potion.damageBoost },{ Potion.regeneration } };

public:
	TileEntityBeacon();
	~TileEntityBeacon();

	static void initialize();

	/** implement override functions from Inventory */
	virtual int getSizeInventory() { return 1; }
	virtual ItemStackPtr getStackInSlot(int slot) { return slot == 0 ? m_payment : NULL; }
	virtual ItemStackPtr decrStackSize(int slot, int size);
	virtual ItemStackPtr getStackInSlotOnClosing(int slot);
	virtual void setInventorySlotContents(int slot, ItemStackPtr pStack);
	virtual String getInvName() { return isInvNameLocalized() ? m_customName : "container.beacon"; }
	virtual bool isInvNameLocalized() { return !m_customName.empty(); }
	virtual int getInventoryStackLimit() { return 1; }
	virtual bool isUseableByPlayer(EntityPlayer* pPlayer);
	virtual void openChest() {}
	virtual void closeChest() {}
	virtual bool isItemValidForSlot(int slot, ItemStackPtr pStack);

	/** implement override functions from TileEntity */
	virtual void updateEntity();
	virtual void readFromNBT(NBTTagCompound* pNbt);
	virtual void writeToNBT(NBTTagCompound* pNbt);
	virtual float getMaxRenderDistanceSquared() { return 65536.0f; }
	// virtual Packet getDescriptionPacket()

	float shouldRender();

	int getPrimaryEffect() { return m_primaryEff; }
	int getSecondaryEffect() { return m_secondaryEff; }
	int getLevels() { return m_level; }

	void setLevels(int lvl) { m_level = lvl; }
	void setPrimaryEffect(int effectID);
	void setSecondaryEffect(int effectID);

	void setCustomName(const String& name) { m_customName = name; }

protected:
	void addEffectsToPlayers();

	/** Checks if the Beacon has a valid pyramid underneath and direct sunlight above */
	void updateState();

protected:
	i64 m_renderCount = 0;
	float m_renderScale = 0.f;
	bool m_bActive = false;

	/** Level of this beacon's pyramid. */
	int m_level = 0;

	/** Primary potion effect given by this beacon. */
	int m_primaryEff = 0;

	/** Secondary potion effect given by this beacon. */
	int m_secondaryEff = 0;

	/** Item given to this beacon as payment. */
	ItemStackPtr m_payment = nullptr;
	String m_customName;
};

class TileEntityHopper : public TileEntity, public IHopper
{
	RTTI_DECLARE(TileEntityHopper);
protected:
	static const int HOPPER_ITEM_SIZE = 5;
	ItemStackPtr m_itemStacks[HOPPER_ITEM_SIZE];

	/** The name that is displayed if the hopper was renamed */
	String m_customName;
	int m_transferCD = 0;

public:
	TileEntityHopper();
	virtual ~TileEntityHopper();

	/** implement override functions from Inventory */
	virtual int getSizeInventory() { return HOPPER_ITEM_SIZE; }
	virtual ItemStackPtr getStackInSlot(int slot) { return m_itemStacks[slot]; }
	virtual ItemStackPtr decrStackSize(int slot, int size);
	virtual ItemStackPtr getStackInSlotOnClosing(int slot);
	virtual void setInventorySlotContents(int slot, ItemStackPtr pStack);
	virtual String getInvName() { return isInvNameLocalized() ? m_customName : "container.hopper"; }
	virtual bool isInvNameLocalized() { return !m_customName.empty(); }
	virtual void setInventoryName(const String& name) { m_customName = name; }
	virtual int getInventoryStackLimit() { return 64; }
	virtual bool isUseableByPlayer(EntityPlayer* pPlayer);
	virtual void openChest() {}
	virtual void closeChest() {}
	virtual bool isItemValidForSlot(int slot, ItemStackPtr pStack) { return true; }

	/** implement override functions from IHopper */
	virtual Vector3 getPos() { return Vector3(m_pos); }
	virtual World* getWorldObj() { return worldObj; }

	/** implement override functions from TileEntity */
	virtual void readFromNBT(NBTTagCompound* pNbt);
	virtual void writeToNBT(NBTTagCompound* pNbt);
	virtual void onInventoryChanged() { TileEntity::onInventoryChanged(); }
	virtual void updateEntity();

	void setTransferCooldown(int cd) { m_transferCD = cd; }
	bool isCoolingDown() { return m_transferCD > 0; }

	bool updateHopper();

	/** Sucks one item into the given hopper from an inventory or EntityItem above it. */
	static bool suckItemsIntoHopper(IHopper* pHopper);

	/** Attempts to place the passed EntityItem's stack into the inventory using as many slots as possible. Returns false if the stackSize of the drop was not depleted. */
	static bool insertItem(IInventory* pInventory, EntityItem* pItem);

	/** Inserts a stack into an inventory. Args: Inventory, stack, side. Returns leftover items. */
	static ItemStackPtr insertStack(IInventory* pInventory, ItemStackPtr pStack, int side);

	/** Looks for anything, that can hold items (like chests, furnaces, etc.) one block above the given hopper. */
	static IInventory* getInventoryAboveHopper(IHopper* pHopper);

	static EntityItem* getCaptureItem(World* pWorld, const Vector3& pos);

	/** Gets an inventory at the given location to extract items into or take items from. Can find either a tile entity or regular entity implementing IInventory. */
	static IInventory* getInventoryAtLocation(World* pWorld, const Vector3& pos);

protected:
	bool insertItemToInventory();

	/** Pulls from the specified slot in the inventory and places in any available slot in the hopper. Returns true if the entire stack was moved */
	static bool pullItemFromSlot(IHopper* pHopper, IInventory* pInventory, int slot, int side);

	/** Can this hopper insert the specified item from the specified slot on the specified side? */
	static bool canInsertItemToInventory(IInventory* pInventory, ItemStackPtr pStack, int slot, int side);

	/** Can this hopper extract the specified item from the specified slot on the specified side? */
	static bool canExtractItemFromInventory(IInventory* pInventory, ItemStackPtr pStack, int slot, int side);

	/** Insert the specified stack to the specified inventory and return any leftover items */
	static ItemStackPtr insertStackInSlot(IInventory* pInventory, ItemStackPtr pStack, int slot, int side);

	/** Gets the inventory the hopper is pointing at. */
	IInventory* getOutputInventory();

	static bool areItemStacksEqualItem(ItemStackPtr pStack1, ItemStackPtr pStack2);
};

}

#endif
