#ifndef __INVENTORY_HAND_CRAFTABLE_HEADER__
#define __INVENTORY_HAND_CRAFTABLE_HEADER__

#include "Inventory/InventoryCraftable.h"
#include "Item/Recipes.h"

namespace BLOCKMAN
{
	class InventoryHandCraftable : public InventoryCraftable
	{
	public:
		InventoryHandCraftable(IInventory* inventory) : InventoryCraftable(inventory)
		{
			// calling onUpdate() again in derived class is necessary, because onUpdate() called 
			// in base class constructor will not invoke the overrided version of updateAvailableRecipes()
			onUpdate();
		}

	protected:
		void updateAvailableRecipes() override
		{
			InventoryCraftable::updateAvailableRecipes();
			auto newEnd = std::remove_if(
				m_availableRecipes.begin(), 
				m_availableRecipes.end(),
				&InventoryHandCraftable::recipeLargerThan2x2);
			m_availableRecipes.resize(newEnd - m_availableRecipes.begin());
		}

	private:
		static bool recipeLargerThan2x2(const std::shared_ptr<CraftingRecipe>& recipe)
		{
			return recipe->getRecipeWidth() > 2 || recipe->getRecipeHeight() > 2;
		}
	};
}

#endif // !__INVENTORY_HAND_CRAFTABLE_HEADER__

