/********************************************************************
filename: 	ItemStack.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-11-8
*********************************************************************/
#ifndef __ITEM_STACK_HEADER__
#define __ITEM_STACK_HEADER__

#include "BM_TypeDef.h"
#include "Core.h"
#include "Entity/Attributes.h"
#include <memory>
#include "Util/Listenable.h"
#include "Util/LocalEvent.h"

using namespace LORD;

namespace BLOCKMAN
{
	class Block;
	class Item;
	class EntityPlayer;
	class NBTTagCompound;
	class Enchantment;
	class World;
	class Random;
	class EntityLivingBase;
	class Entity;
	class GunSetting;
	class BulletClipSetting;

	class ItemStack;
	using ItemStackPtr = std::shared_ptr<ItemStack>;

	class ItemStackInfo
	{
	public:
		int id = 0;
		int meta = 0;
		int num = 0;
		int bullets = 0;
	};

	class ItemStack : public ObjectAlloc, public std::enable_shared_from_this<ItemStack>
	{
	public:
		// static DecimalFormat field_111284_a = new DecimalFormat("#.###");
		/** Size of the stack. */
		Listenable<int> stackSize;
		/** Number of animation frames to go when receiving an item (by walking into it, for example). */
		int animationsToGo = 0;
		/** ID of the item. */
		int itemID = 0;
		/*subType Id of the item */
		int subItemID = 0;
		/** A NBTTagMap containing data about an ItemStack. Can only be used for non stackable items */
		NBTTagCompound* stackTagCompound = nullptr;
		/** Damage dealt to the item or number of use. Raise when using items. */
		Listenable<int> itemDamage;
		/** Item frame this stack is on, or null if not on an item frame. */
		// private EntityItemFrame itemFrame;

		// 枪械相关，当前弹夹剩余的子弹数
		int m_currentClipBulletNum = 0;

		// 枪械的总子弹数
		int m_totalBulletNum = 0;

		// need adjust stackIndex
		bool m_needAdjustStackIndex = true;

		ItemStackInfo m_itemStackInfo;
	protected:
		float m_cd = 0.0f;	//the interval of use this item

	public:
		int getCurrentClipBulletNum() { return m_currentClipBulletNum; }
		int getTotalBulletNum() { return m_totalBulletNum; }
		void setCurrentClipBulletNum(int currentClipBulletNum) { m_currentClipBulletNum = currentClipBulletNum; }
		void setTotalBulletNum(int totalBulletNum) { m_totalBulletNum = totalBulletNum; }
		bool reloadBulletClip(int canAddBulletNum); /* Reload the Bullet clip */
		bool consumeBullet();

	protected:
		LocalEvent m_enchantmentAddedEvent;
		/** compares ItemStack argument to the instance ItemStack; returns true if both ItemStacks are equal */
		bool isItemStackEqual(ItemStackPtr pStack);

	public:
		ItemStack();
		ItemStack(Block* pBlock, int size = 1, int meta = 0); /* meta represents the special attribute, like damage, subItemId*/
		ItemStack(Item* pItem, int size = 1, int meta = 0);
		ItemStack(int id, int size, int meta);
		ItemStack(int id, int subId, int size, int damage);

		bool isOnCd() { return m_cd > 0.0f; }
		void updateCdTime(float fDeltaTime);  // update by client::gui_mailControl
		void setCd(float cd) { m_cd = cd; }
		static ItemStackPtr loadItemStackFromNBT(NBTTagCompound* pNbtCompound);
		/** Remove the argument from the stack size. Return a new stack object with argument size. */
		ItemStackPtr splitStack(int count);
		/** Returns the object corresponding to the stack. */
		Item* getItem();
		/*Return adapted BulletClipItem Setting of the stack if it's a ItemGun */
		const BulletClipSetting* getBulletClipSetting();
		/*Return gun Setting of the stack if it's a ItemGun */
		const GunSetting* getGunSetting();

		bool tryPlaceItemIntoWorld(EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos, int face, const Vector3& hit);
		/** Returns the strength of the stack against a given block. */
		float getStrVsBlock(Block* pBlock);
		/** Called whenever this item stack is equipped and right clicked. */
		ItemStackPtr useItemRightClick(World* pWorld, EntityPlayer* pPlayer);
		ItemStackPtr onFoodEaten(World* pWorld, EntityPlayer* pPlayer);
		/** Write the stack fields to a NBT object. Return the new NBT object. */
		NBTTagCompound* writeToNBT(NBTTagCompound* pCompound);
		/** Read the stack fields from a NBT object. */
		void readFromNBT(NBTTagCompound* pCompound);
		/** Returns maximum size of the stack. */
		int getMaxStackSize();
		/** Returns true if the ItemStack can hold 2 or more units of the item. */
		bool isStackable();
		/** true if this itemStack is damageable */
		bool isItemStackDamageable();
		bool getHasSubtypes();
		/** returns true when a damageable item is damaged */
		bool isItemDamaged();
		/** gets the damage of an itemstack, for displaying purposes */
		int getItemDamageForDisplay() { return itemDamage; }
		/** gets the stack size of an itemstack */
		int getItemStackSize() { int size = stackSize; return size; }
		/** gets the damage of an itemstack */
		int getItemDamage() { return itemDamage; }
		/** Sets the item damage of the ItemStack. */
		void setItemDamage(int par1);
		/** Returns the max damage an item in the stack can take. */
		int getMaxDamage();
		/** Attempts to damage the ItemStack with par1 amount of damage, If the ItemStack has the Unbreaking enchantment
		* there is a chance for each point of damage to be negated. Returns true if it takes more damage than getMaxDamage().
		* Returns false otherwise or if the ItemStack can't be damaged or if all points of damage are negated. */
		bool attemptDamageItem(int damage, Random* pRandom);
		/** Damages the item in the ItemStackPtr*/
		void damageItem(int damage, EntityLivingBase* pLiving);
		/** Calls the corresponding fct in di */
		void hitEntity(EntityLivingBase* pLiving, EntityPlayer* pPlayer);
		void onBlockDestroyed(World* pWorld, int blockID, const BlockPos& pos, EntityPlayer* pPlayer);
		/** Checks if the itemStack object can harvest a specified block */
		bool canHarvestBlock(Block* pBlock);
		bool interactWithEntity(EntityPlayer* pPlayer, EntityLivingBase* pLiving);
		/** Returns a new stack with the same properties. */
		ItemStackPtr copy();
		static bool areItemStackTagsEqual(ItemStackPtr pStack1, ItemStackPtr pStack2);
		/** compares ItemStack argument1 with ItemStack argument2; returns true if both ItemStacks are equal */
		static bool areItemStacksEqual(ItemStackPtr pStack1, ItemStackPtr pStack2);
		/** compares ItemStack argument to the instance ItemStack; returns true if the Items contained in both ItemStacks are equal	*/
		bool isItemEqual(ItemStackPtr pStack);
		String getItemName();
		/** Creates a copy of a ItemStack, a null parameters will return a null. */
		static ItemStackPtr copyItemStack(ItemStackPtr pStack);
		String toString();
		/** Called each tick as long the ItemStack in on player inventory. Used to progress the pickup animation and update maps. */
		void updateAnimation(World* pWorld, Entity* pEntity, int slot, bool isCurrentItem);
		void onCrafting(World* pWorld, EntityPlayer* pPlayer, int amount);
		int getMaxItemUseDuration();
		ACTION_TYPE getItemUseAction();
		/** Called when the player releases the use item button. Args: world, entityplayer, itemInUseCount */
		void onPlayerStoppedUsing(World* pWorld, EntityPlayer* pPlayer, int timeLeft);
		/** Returns true if the ItemStack has an NBTTagCompound. Currently used to store enchantments. */
		bool hasTagCompound() { return stackTagCompound != NULL; }
		/** Returns the NBTTagCompound of the ItemStack. */
		NBTTagCompound* getTagCompound() { return stackTagCompound; }
		NBTTagList* getEnchantmentTagList();
		/** Assigns a NBTTagCompound to the ItemStack, blockman validates that only non-stackable items can have it. */
		void setTagCompound(NBTTagCompound* compond) { stackTagCompound = compond; }
		/** returns the display name of the itemstack */
		String getDisplayName();
		/** Sets the item's name (used by anvil to rename the items). */
		void setItemName(const String& name);
		void clearCustomName();
		/** Returns true if the itemstack has a display name */
		bool hasDisplayName();
		/** Return a list of strings containing information about the item */
		StringArray getTooltip(EntityPlayer* pPlayer, bool advanced);
		bool hasEffect();
		RARITY_TYPE getRarity();
		/** True if it is a tool and has no enchantments to begin with */
		bool isItemEnchantable();
		/** Adds an enchantment with a desired level on the ItemStack. */
		void addEnchantment(Enchantment* enchantment, int level);
		void addEnchantmentbyId(int id, int level);
		/** True if the item has enchantment data */
		bool isItemEnchanted();
		void setTagInfo(const String& name, NBTBase* pNbt);
		bool canEditBlocks();
		/** Return whether this stack is on an item frame. */
		// bool isOnItemFrame()
		/** Set the item frame this stack is on. */
		// void setItemFrame(EntityItemFrame par1EntityItemFrame)
		/** Return the item frame this stack is on. Returns null if not on an item frame. */
		// EntityItemFrame getItemFrame()
		/** Get this stack's repair cost, or 0 if no repair cost is defined. */
		int getRepairCost();
		/** Set this stack's repair cost. */
		void setRepairCost(int cost);
		void getAttributeModifiers(AttributeModifierMap& outMap);
		LocalEvent::Subscription onEnchantmentAdded(std::function<void()> callback)
		{
			return m_enchantmentAddedEvent.subscribe(std::move(callback));
		}
		ItemStackInfo getItemStackInfo();

		bool isGunItem();
		bool isThrowItemSkill();
		bool isItemSkill();
		bool isAutoCastSkill();
		bool isShowCrossHair();
		bool isItemSkillBook();

		bool isFirstRender();
		bool isThirdRender();
	};

}

#endif
