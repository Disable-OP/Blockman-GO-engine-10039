#ifndef __ITEM_CRAFTER_HEADER__
#define __ITEM_CRAFTER_HEADER__

#include "Core.h"

using namespace LORD;

namespace BLOCKMAN
{
	class IRecipe;
	class ShapedRecipes;
	class InventoryPlayer;
	class IInventory;
	class EntityPlayer;

	class ItemCrafter : public Singleton<ItemCrafter>, public ObjectAlloc
	{
	private:
		// FIX [SYMPTOM-3]: generalized to IRecipe so shapeless recipes work too.
		bool tryConsumeItemsFromInventory(IRecipe * recipe, IInventory * inventory);

	public:
		bool craftWithinInventory(IRecipe * recipe, InventoryPlayer* inventory);
		bool craftItem(EntityPlayer* player, int recipeId);
	};
}

#endif // !__ITEM_CRAFTER_HEADER__
