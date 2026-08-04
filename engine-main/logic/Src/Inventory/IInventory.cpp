#include "IInventory.h"
#include "Item/ItemStack.h"

namespace BLOCKMAN
{
	void IInventory::swapStacksInSlots(int slot1, int slot2)
	{
		auto temp = getStackInSlot(slot1);
		setInventorySlotContents(slot1, getStackInSlot(slot2));
		setInventorySlotContents(slot2, temp);
	}

	int IInventory::findItemStack(ItemStackPtr itemStack)
	{
		int size = getSizeInventory();
		for (int i = 0; i < size; ++i)
		{
			if (getStackInSlot(i) == itemStack)
			{
				return i;
			}
		}
		return -1;
	}

	ItemStackPtr IInventory::findCanSaveItemStackById(int itemId, int itemMeta)
	{
		int size = getSizeInventory();
		for (int i = 0; i < size; ++i)
		{
			if (getStackInSlot(i) && getStackInSlot(i)->itemID == itemId && getStackInSlot(i)->itemDamage == itemMeta && getStackInSlot(i)->stackSize < getStackInSlot(i)->getMaxStackSize())
			{
				return getStackInSlot(i);
			}
		}
		return nullptr;
	}

	int IInventory::getItemNum(int id)
	{
		int rt = 0;
		for (int i = 0; i < getSizeInventory(); i++)
		{
			if (getStackInSlot(i) && getStackInSlot(i)->itemID == id)
				rt += getStackInSlot(i)->stackSize;
		}

		return rt;
	}

	void IInventory::clear()
	{
		for (int i = 0; i < getSizeInventory(); i++)
		{
			decrStackSize(i, getStackInSlot(i) ? getStackInSlot(i)->stackSize : 0);
		}
	}

	void IInventory::removeItem(int id, int num)
	{
		int numTemp = num;
		for (int i = 0; i < getSizeInventory(); i++)
		{
			auto stack = getStackInSlot(i);
			if (stack && stack->itemID == id)
			{

				this->decrStackSize(i, numTemp);
				if (stack->stackSize >= numTemp)
				{
					break;
				}
				else
				{
					numTemp -= stack->stackSize;
				}
			}	
		}

	}
}
