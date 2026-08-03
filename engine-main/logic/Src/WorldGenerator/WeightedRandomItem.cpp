#include "WeightedRandomItem.h"

#include "Inventory/IInventory.h"
#include "Util/Random.h"
#include "TileEntity/TileEntityInventory.h"
#include "Entity/Enchantment.h"
#include "Item/ItemStack.h"

namespace BLOCKMAN
{

int WeightedRandom::getTotalWeight(const WRItemList& WRItemlst)
{
	int result = 0;
	WeightedRandomItem* pItem = NULL;

	for (WRItemList::const_iterator it = WRItemlst.begin(); it != WRItemlst.end(); ++it)
	{
		result += (*it)->getItemWeight();
	}
	return result;
}

WeightedRandomItem* WeightedRandom::getRandomItem(Random& rand, const WRItemList& WRItemlst, int totalWeight)
{
	LordAssert(totalWeight > 0);

	int randWeight = rand.nextInt(totalWeight);
	WeightedRandomItem* pResult = NULL;

	for (WRItemList::const_iterator it = WRItemlst.begin(); it != WRItemlst.end(); ++it)
	{
		pResult = *it;
		randWeight -= pResult->getItemWeight();
		if (randWeight < 0)
			return pResult;
	}
	return NULL;
}

WeightedRandomItem* WeightedRandom::getRandomItem(Random& rand, const WRItemList& WRItemlst)
{
	return getRandomItem(rand, WRItemlst, getTotalWeight(WRItemlst));
}

int WeightedRandom::getTotalWeight(WeightedRandomItem** arr, int arrLen)
{
	int totalWeight = 0;

	for (int i = 0; i < arrLen; ++i)
	{
		if (arr[i] == NULL)
			continue;
		totalWeight += arr[i]->getItemWeight();
	}

	return totalWeight;
}

WeightedRandomItem* WeightedRandom::getRandomItem(Random& rand, WeightedRandomItem** arr, int arrLen, int totalWeight)
{
	LordAssert(totalWeight > 0);

	int randWeight = rand.nextInt(totalWeight);
	WeightedRandomItem* pResult = NULL;

	for (int i = 0; i < arrLen; ++i)
	{
		pResult = arr[i];
		if (!pResult)
			continue;

		randWeight -= pResult->getItemWeight();
		if (randWeight < 0)
			return pResult;
	}
	return NULL;
}

WeightedRandomItem* WeightedRandom::getRandomItem(Random& rand, WeightedRandomItem** arr, int arrLen)
{
	return getRandomItem(rand, arr, arrLen, getTotalWeight(arr, arrLen));
}


WeightedRandomChestContent::WeightedRandomChestContent(int id, int damage, int minimum, int maximum, int weight)
	: WeightedRandomItem(weight)
	, theMinimumChanceToGenerateItem(minimum)
	, theMaximumChanceToGenerateItem(maximum)
	, isCopy(true)
{
	theItemId = LORD::make_shared<ItemStack>(id, 0, damage);
}


WeightedRandomChestContent::WeightedRandomChestContent(ItemStackPtr pStack, int minimum, int maximum, int weight)
	: WeightedRandomItem(weight)
	, theItemId(pStack)
	, theMinimumChanceToGenerateItem(minimum)
	, theMaximumChanceToGenerateItem(maximum)
	, isCopy(false)
{
}

WeightedRandomChestContent::~WeightedRandomChestContent()
{

}

void WeightedRandomChestContent::generateChestContents(Random& rand, WeightedRandomChestContent** arr, int len, IInventory* pInventory, int par3)
{
	for (int i = 0; i < par3; ++i)
	{
		WeightedRandomChestContent* pWRChestContent = (WeightedRandomChestContent*)WeightedRandom::getRandomItem(rand, (WeightedRandomItem**)arr, len);
		int ss = pWRChestContent->theMinimumChanceToGenerateItem + 
			rand.nextInt(pWRChestContent->theMaximumChanceToGenerateItem - pWRChestContent->theMinimumChanceToGenerateItem + 1);

		if (pWRChestContent->theItemId->getMaxStackSize() >= ss)
		{
			ItemStackPtr pStack = pWRChestContent->theItemId->copy();
			pStack->stackSize = ss;
			pInventory->setInventorySlotContents(rand.nextInt(pInventory->getSizeInventory()), pStack);
		}
		else
		{
			for (int j = 0; j < ss; ++j)
			{
				ItemStackPtr pStack = pWRChestContent->theItemId->copy();
				pStack->stackSize = 1;
				pInventory->setInventorySlotContents(rand.nextInt(pInventory->getSizeInventory()), pStack);
			}
		}
	}
}

void WeightedRandomChestContent::generateDispenserContents(Random& rand, WeightedRandomChestContent** arr, int len, TileEntityDispenser* dispenser, int par3)
{
	for (int i = 0; i < par3; ++i)
	{
		WeightedRandomChestContent* pChest = (WeightedRandomChestContent*)WeightedRandom::getRandomItem(rand, (WeightedRandomItem**)arr, len);
		int var6 = pChest->theMinimumChanceToGenerateItem + 
			rand.nextInt(pChest->theMaximumChanceToGenerateItem - pChest->theMinimumChanceToGenerateItem + 1);

		if (pChest->theItemId->getMaxStackSize() >= var6)
		{
			ItemStackPtr pStack = pChest->theItemId->copy();
			pStack->stackSize = var6;
			dispenser->setInventorySlotContents(rand.nextInt(dispenser->getSizeInventory()), pStack);
		}
		else
		{
			for (int j = 0; j < var6; ++j)
			{
				ItemStackPtr pStack = pChest->theItemId->copy();
				pStack->stackSize = 1;
				dispenser->setInventorySlotContents(rand.nextInt(dispenser->getSizeInventory()), pStack);
			}
		}
	}
}

WeightedRandomChestContent** WeightedRandomChestContent::CombineContent(WeightedRandomChestContent** arr, int len, WeightedRandomChestContent** arr2, int len2)
{
	WeightedRandomChestContent** pResult = (WeightedRandomChestContent**)LordMalloc(sizeof(WeightedRandomChestContent*)*(len + len2));
	int idx = 0;

	for (int i = 0; i < len; ++i)
	{
		pResult[idx++] = arr[i];
	}

	for (int i = 0; i < len2; ++i)
	{
		pResult[idx++] = arr2[i];
	}

	return pResult;
}

SpawnListEntry::SpawnListEntry(int _classID, int weight, int min, int max)
	: WeightedRandomItem(weight)
{
	classID = _classID;
	minGroupCount = min;
	maxGroupCount = max;
}

EnchantmentData::EnchantmentData(Enchantment* pEnch, int level)
	: WeightedRandomItem(pEnch->getWeight())
{
	enchantmentobj = pEnch;
	enchantmentLevel = level;
}

EnchantmentData::EnchantmentData(int enchID, int level)
	: WeightedRandomItem(Enchantment::enchantmentsList[enchID]->getWeight())
{
	enchantmentobj = Enchantment::enchantmentsList[enchID];
	enchantmentLevel = level;
}

}
