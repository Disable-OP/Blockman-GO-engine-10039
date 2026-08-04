#ifndef __INVENTORY_SUBSET_HEADER__
#define __INVENTORY_SUBSET_HEADER__

#include "Inventory/InventoryProxy.h"

namespace BLOCKMAN
{
	class InventorySubset : public InventoryProxy
	{
	private:
		size_t offset = 0;
		size_t size = 0;
	public:
		InventorySubset(IInventory* inventory, size_t offset, size_t size)
			: InventoryProxy(inventory)
			, offset(offset)
			, size(size) {}
		int getSizeInventory() override
		{
			return size;
		}
		ItemStackPtr getStackInSlot(int slot) override
		{
			if (slot >= getSizeInventory())
				return NULL;
			return m_inventory->getStackInSlot(slot + offset);
		}
		ItemStackPtr decrStackSize(int slot, int count) override
		{
			if (slot >= getSizeInventory())
				return NULL;
			return m_inventory->decrStackSize(slot + offset, count);
		}
		ItemStackPtr getStackInSlotOnClosing(int slot) override
		{
			if (slot >= getSizeInventory())
				return NULL;
			return m_inventory->getStackInSlotOnClosing(slot + offset);
		}
		void setInventorySlotContents(int slot, ItemStackPtr pStack) override
		{
			if (slot >= getSizeInventory())
				return;
			return m_inventory->setInventorySlotContents(slot + offset, pStack);
		}
	};
}

#endif // !__INVENTORY_SUBSET_HEADER__

