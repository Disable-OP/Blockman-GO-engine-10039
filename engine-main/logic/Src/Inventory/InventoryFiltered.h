#ifndef __INVENTORY_ARMOR_SUBSET_HEADER__
#define __INVENTORY_ARMOR_SUBSET_HEADER__

#include <functional>
#include "Inventory/InventoryProxy.h"

namespace BLOCKMAN
{
	class InventoryFiltered : public InventoryProxy
	{
	private:
		std::function<bool(int)> m_predicate;
		int toActualIndex(int slot);
		void warnIfInventoryTooLarge();

	public:
		InventoryFiltered(IInventory* inventory, std::function<bool(int)> predicate) 
			: InventoryProxy(inventory), m_predicate(predicate) {}
		int getSizeInventory() override;
		ItemStackPtr getStackInSlot(int slot) override;
		ItemStackPtr decrStackSize(int slot, int count) override;
		ItemStackPtr getStackInSlotOnClosing(int slot) override;
		void setInventorySlotContents(int slot, ItemStackPtr pStack) override;
		bool isItemValidForSlot(int slot, ItemStackPtr pStack) override;
	};
}

#endif
