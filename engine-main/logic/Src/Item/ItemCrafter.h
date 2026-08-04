#ifndef __ITEM_CRAFTER_HEADER__
#define __ITEM_CRAFTER_HEADER__

#include "Core.h"

using namespace LORD;

namespace BLOCKMAN
{
	class ShapedRecipes;
	class InventoryPlayer;
	class IInventory;
	class EntityPlayer;

	class ItemCrafter : public Singleton<ItemCrafter>, public ObjectAlloc
	{
	private:
		bool tryConsumeItemsFromInventory(ShapedRecipes * recipe, IInventory * inventory);

	public:
		bool craftWithinInventory(ShapedRecipes * recipe, InventoryPlayer* inventory);
		bool craftItem(EntityPlayer* player, int recipeId);
	};
}

#endif // !__ITEM_CRAFTER_HEADER__
