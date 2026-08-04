#ifndef __INVENTORY_CRAFTABLE_HEADER__
#define __INVENTORY_CRAFTABLE_HEADER__

#include <stdexcept>
#include <memory>
#include "Inventory/InventoryProxy.h"

namespace BLOCKMAN
{
	class CraftingRecipe;
	class ShapedRecipes;

	class InventoryCraftable : public InventoryProxy
	{
	protected:
		vector<std::shared_ptr<ItemStack>>::type m_craftableItems;
		vector<std::shared_ptr<CraftingRecipe>>::type m_availableRecipes;
		set<ShapedRecipes*>::type calculateAvailableShapedRecipes();
		
		virtual void updateAvailableRecipes();
		void updateCraftableItemStacks();

	public:
		InventoryCraftable(IInventory* inventory);
		std::shared_ptr< CraftingRecipe> getRecipe(int slot)
		{
			return m_availableRecipes[slot];
		}
		virtual void onUpdate() override;
		virtual int getSizeInventory() override
		{
			return m_craftableItems.size();
		}
		virtual ItemStackPtr getStackInSlot(int slot) override
		{
			return m_craftableItems[slot];
		}
		virtual ItemStackPtr decrStackSize(int slot, int count) override
		{
			throw std::logic_error(std::string("method not implemented: ") + __FUNCTION__);
		}
		virtual ItemStackPtr getStackInSlotOnClosing(int slot) override
		{
			throw std::logic_error(std::string("method not implemented: ") + __FUNCTION__);
		}
		virtual void setInventorySlotContents(int slot, ItemStackPtr pStack) override
		{
			throw std::logic_error(std::string("method not implemented: ") + __FUNCTION__);
		}
		virtual bool isItemValidForSlot(int slot, ItemStackPtr pStack) override
		{
			throw std::logic_error(std::string("method not implemented: ") + __FUNCTION__);
		}
	};
}

#endif // !__INVENTORY_CRAFTABLE_HEADER__

