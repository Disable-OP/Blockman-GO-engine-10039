#include "InventoryFiltered.h"
#include "Inventory/InventoryPlayer.h"
#include "Item/ItemStack.h"
#include "Item/Item.h"

namespace BLOCKMAN
{
	int InventoryFiltered::toActualIndex(int slot)
	{
		warnIfInventoryTooLarge();
		auto originalSize = m_inventory->getSizeInventory();
		int counter = 0;
		for (int i = 0; i < originalSize; ++i)
		{
			if (m_predicate(i))
			{
				if (counter == slot)
				{
					return i;
				}
				++counter;
			}
		}
		return -1;
	}

	void InventoryFiltered::warnIfInventoryTooLarge()
	{
		auto originalSize = m_inventory->getSizeInventory();
		static int threshold = 50;
		if (originalSize > threshold)
		{
			LordLogWarning("size of underlying inventory of InventoryFiltered reached %d. "
				"large underlying inventory might cause performance problems", originalSize);
			threshold = originalSize * 2;
		}
	}

	int InventoryFiltered::getSizeInventory()
	{
		warnIfInventoryTooLarge();
		auto originalSize = m_inventory->getSizeInventory();
		int size = 0;
		for (int i = 0; i < originalSize; ++i)
		{
			if (m_predicate(i))
			{
				++size;
			}
		}
		return size;
	}

	ItemStackPtr InventoryFiltered::getStackInSlot(int slot)
	{
		int index = toActualIndex(slot);
		return index >= 0 ? m_inventory->getStackInSlot(index) : NULL;
	}

	ItemStackPtr InventoryFiltered::decrStackSize(int slot, int count)
	{
		int index = toActualIndex(slot);
		return index >= 0 ? m_inventory->decrStackSize(index, count) : NULL;
	}

	ItemStackPtr InventoryFiltered::getStackInSlotOnClosing(int slot)
	{
		int index = toActualIndex(slot);
		return index >= 0 ? m_inventory->getStackInSlotOnClosing(index) : NULL;
	}

	void InventoryFiltered::setInventorySlotContents(int slot, ItemStackPtr pStack)
	{
		int index = toActualIndex(slot);
		if (index >= 0)
		{
			m_inventory->setInventorySlotContents(index, pStack);
		}
	}

	bool InventoryFiltered::isItemValidForSlot(int slot, ItemStackPtr pStack)
	{
		int index = toActualIndex(slot);
		return index >= 0 ? m_inventory->isItemValidForSlot(index, pStack) : NULL;
	}
}