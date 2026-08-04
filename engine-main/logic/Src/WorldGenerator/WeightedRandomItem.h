/********************************************************************
filename: 	WeightedRandomItem.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-11-9
*********************************************************************/
#ifndef __WEIGHTED_RANDOM_ITEM_HEADER__
#define __WEIGHTED_RANDOM_ITEM_HEADER__

#include "Core.h"
#include "Item/ItemStack.h"

using namespace LORD;

namespace BLOCKMAN
{
class IInventory;
class WeightedRandomItem;
class Random;
class TileEntityDispenser;
class ItemStack;
class Enchantment;

typedef list<WeightedRandomItem*>::type WRItemList;

class WeightedRandom
{
public:
	/** Returns the total weight of all items in a collection. */
	static int getTotalWeight(const WRItemList& WRItemlst);

	/** Returns a random choice from the input items, with a total weight value. */
	static WeightedRandomItem* getRandomItem(Random& rand, const WRItemList& WRItemlst, int allWeight);

	/** Returns a random choice from the input items. */
	static WeightedRandomItem* getRandomItem(Random& rand, const WRItemList& WRItemlst);

	/** Returns the total weight of all items in a array. */
	static int getTotalWeight(WeightedRandomItem** arr, int arrLen);

	/*** Returns a random choice from the input array of items, with a total weight value. */
	static WeightedRandomItem* getRandomItem(Random& rand, WeightedRandomItem** arr, int arrLen, int totalWeight);

	/** Returns a random choice from the input items. */
	static WeightedRandomItem* getRandomItem(Random& rand, WeightedRandomItem** arr, int arrLen);
};

class WeightedRandomItem : public ObjectAlloc
{
public:
	WeightedRandomItem(int weight)
	{
		itemWeight = weight;
	}

	int getItemWeight() const { return itemWeight; }

protected:
	/** The Weight is how often the item is chosen(higher number is higher chance(lower is lower)) */
	int itemWeight = 0;
};

class WeightedRandomChestContent : public WeightedRandomItem
{
public:
	WeightedRandomChestContent(int id, int damage, int minimum, int maximum, int weight);
	WeightedRandomChestContent(ItemStackPtr pStack, int minimum, int maximum, int weight);
	~WeightedRandomChestContent();
	
	/** Generates the Chest contents.*/
	static void generateChestContents(Random& rand, WeightedRandomChestContent** arr, int len, IInventory* pInventory, int par3);

	/** Generates the Dispenser contents. */
	static void generateDispenserContents(Random& rand, WeightedRandomChestContent** arr, int len, TileEntityDispenser* dispenser, int par3);
	
	static WeightedRandomChestContent** CombineContent(WeightedRandomChestContent** arr, int len, WeightedRandomChestContent** arr2, int len2);

public:
	/** The Item/Block ID to generate in the Chest. */
	ItemStackPtr theItemId;
	bool isCopy = false;

	/** The minimum chance of item generating. */
	int theMinimumChanceToGenerateItem = 0;

	/** The maximum chance of item generating. */
	int theMaximumChanceToGenerateItem = 0;
};

class SpawnListEntry : public WeightedRandomItem
{
public:
	int classID = 0;
	int minGroupCount = 0;
	int maxGroupCount = 0;

	SpawnListEntry(int _classID, int weight, int min, int max);
};

class EnchantmentData : public WeightedRandomItem
{
public:
	/** Enchantment object associated with this EnchantmentData */
	Enchantment* enchantmentobj = nullptr;
	/** Enchantment level associated with this EnchantmentData */
	int enchantmentLevel = 0;

	EnchantmentData(Enchantment* pEnch, int level);
	EnchantmentData(int enchID, int level);
};

}

#endif
