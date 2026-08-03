#include "InventoryEnderChest.h"

#include "Item/ItemStack.h"
#include "TileEntity/TileEntitys.h"

namespace BLOCKMAN
{

InventoryBasic::InventoryBasic(const String& title, bool customName, int slotscount)
	: inventoryTitle(title)
	, hasCustomName(customName)
	, slotsCount(slotscount)
{
	inventoryContents = new ItemStackPtr[slotsCount];
	prevInventoryContents = new ItemStackPtr[slotsCount];
}

InventoryBasic::~InventoryBasic()
{
	delete [] inventoryContents;
	delete [] prevInventoryContents;
}

ItemStackPtr InventoryBasic::decrStackSize(int slot, int size)
{
	if (slot < 0 || slot >= slotsCount)
		return NULL;
	if (!inventoryContents[slot])
		return NULL;

	ItemStackPtr pStack = NULL;
	if (inventoryContents[slot]->stackSize <= size)
	{
		pStack = inventoryContents[slot];
		inventoryContents[slot] = NULL;
		onInventoryChanged();
		return pStack;
	}
	else
	{
		pStack = inventoryContents[slot]->splitStack(size);

		if (inventoryContents[slot]->stackSize == 0)
		{
			inventoryContents[slot] = NULL;
		}

		onInventoryChanged();
		return pStack;
	}
}

ItemStackPtr InventoryBasic::getStackInSlot(int slot)
{
	if (slot < 0 || slot >= slotsCount)
	{
		LordLogError("getStackInSlot error %d, out of range", slot);
		return NULL;
	}
		
	return inventoryContents[slot];
}

ItemStackPtr InventoryBasic::getPrevStackInSlot(int slot)
{
	if (slot < 0 || slot >= slotsCount)
	{
		LordLogError("getPrevStackInSlot error %d, out of range", slot);
		return NULL;
	}

	return prevInventoryContents[slot];
}

ItemStackPtr InventoryBasic::getStackInSlotOnClosing(int slot)
{
	if (slot < 0 || slot >= slotsCount)
		return NULL;

	if (inventoryContents[slot] != NULL)
	{
		ItemStackPtr pStack = inventoryContents[slot];
		inventoryContents[slot] = NULL;
		return pStack;
	}
	return NULL;
}

void InventoryBasic::setInventorySlotContents(int slot, ItemStackPtr pStack)
{
	if (slot < 0 || slot >= slotsCount)
		return;

	inventoryContents[slot] = pStack;

	if (pStack != NULL && pStack->stackSize > getInventoryStackLimit())
	{
		pStack->stackSize = getInventoryStackLimit();
	}

	onInventoryChanged();
}

void InventoryBasic::setPrevInventorySlotContents(int slot, ItemStackPtr pStack)
{
	if (!(slot >= 0 && slot < slotsCount))
		return;

	prevInventoryContents[slot] = pStack;
}

void InventoryBasic::setCustomTitle(const String& title)
{
	hasCustomName = true;
	inventoryTitle = title;
}

void InventoryBasic::onInventoryChanged()
{
	for (InventoryListeners::iterator it = changeListeners.begin(); it != changeListeners.end(); ++it)
	{
		(*it)->onInventoryChanged(this);
	}
}

InventoryEnderChest::InventoryEnderChest()
	: InventoryBasic("container.enderchest", false, 27)
{
	associatedChest = NULL;
}

void InventoryEnderChest::loadInventoryFromNBT(NBTTagList* pNbtLst)
{
	for (int i = 0; i < getSizeInventory(); ++i)
	{
		setInventorySlotContents(i, NULL);
	}

	for (int i = 0; i < pNbtLst->tagCount(); ++i)
	{
		NBTTagCompound* pNbtItem = (NBTTagCompound*)pNbtLst->tagAt(i);
		int slot = pNbtItem->getByte("Slot") & 255;

		if (slot >= 0 && slot < getSizeInventory())
		{
			setInventorySlotContents(slot, ItemStack::loadItemStackFromNBT(pNbtItem));
		}
	}
}

NBTTagList* InventoryEnderChest::saveInventoryToNBT()
{
	NBTTagList* pNbtLst = LordNew NBTTagList("EnderItems");

	for (int i = 0; i < getSizeInventory(); ++i)
	{
		ItemStackPtr pStack = getStackInSlot(i);

		if (pStack != NULL)
		{
			NBTTagCompound* pNbtStack = LordNew NBTTagCompound();
			pNbtStack->setByte("Slot", (i8)i);
			pStack->writeToNBT(pNbtStack);
			pNbtLst->appendTag(pNbtStack);
		}
	}

	return pNbtLst;
}

bool InventoryEnderChest::isUseableByPlayer(EntityPlayer* pPlayer)
{
	return associatedChest != NULL && !associatedChest->isUseableByPlayer(pPlayer) ? false : InventoryBasic::isUseableByPlayer(pPlayer);
}

void InventoryEnderChest::openChest()
{
	if (associatedChest != NULL)
	{
		associatedChest->openChest();
	}

	InventoryBasic::openChest();
}

void InventoryEnderChest::closeChest()
{
	if (associatedChest != NULL)
	{
		associatedChest->closeChest();
	}

	InventoryBasic::closeChest();
	associatedChest = NULL;
}

ItemStackInfo InventoryEnderChest::getItemStackInfo(int slot)
{
	auto stack = getStackInSlot(slot);
	if (stack)
	{
		return stack->getItemStackInfo();
	}
	static ItemStackInfo info = ItemStackInfo();
	return info;
}

}