#ifndef __INVENTORY_PROXY_HEADER__
#define __INVENTORY_PROXY_HEADER__

#include "Inventory/IInventory.h"

namespace BLOCKMAN
{
	class InventoryProxy : public IInventory
	{
	protected:
		IInventory* m_inventory = nullptr;

	public:
		InventoryProxy(IInventory* inventory) : m_inventory(inventory) {}
		virtual void onUpdate() {}
		int getSizeInventory() override
		{
			return m_inventory->getSizeInventory();
		}
		ItemStackPtr getStackInSlot(int slot) override
		{
			return m_inventory->getStackInSlot(slot);
		}
		ItemStackPtr decrStackSize(int slot, int count) override
		{
			return m_inventory->decrStackSize(slot, count);
		}
		ItemStackPtr getStackInSlotOnClosing(int slot) override
		{
			return m_inventory->getStackInSlotOnClosing(slot);
		}
		void setInventorySlotContents(int slot, ItemStackPtr pStack) override
		{
			return m_inventory->setInventorySlotContents(slot, pStack);
		}
		String getInvName() override
		{
			return m_inventory->getInvName();
		}
		bool isInvNameLocalized() override
		{
			return m_inventory->isInvNameLocalized();
		}
		int getInventoryStackLimit() override
		{
			return m_inventory->getInventoryStackLimit();
		}
		void onInventoryChanged() override
		{
			m_inventory->onInventoryChanged();
		}
		bool isUseableByPlayer(EntityPlayer* pPlayer) override
		{
			return m_inventory->isUseableByPlayer(pPlayer);
		}
		void openChest() override
		{
			m_inventory->openChest();
		}
		void closeChest() override
		{
			m_inventory->closeChest();
		}
		bool isItemValidForSlot(int slot, ItemStackPtr pStack) override
		{
			return m_inventory->isItemValidForSlot(slot, pStack);
		}
	};
}

#endif // !__INVENTORY_PROXY_HEADER__
