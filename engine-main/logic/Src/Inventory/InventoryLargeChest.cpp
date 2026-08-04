#include "InventoryLargeChest.h"

namespace BLOCKMAN
{

InventoryLargeChest::InventoryLargeChest(const String& nameIn, IInventory* upper, IInventory* lower)
{
	name = nameIn;

	if (upper == NULL)
		upper = lower;

	if (lower == NULL)
		lower = upper;

	upperChest = upper;
	lowerChest = lower;
}

String InventoryLargeChest::getInvName()
{
	return upperChest->isInvNameLocalized() ? 
		upperChest->getInvName() :
		(lowerChest->isInvNameLocalized() ? lowerChest->getInvName() : name);
}

ItemStackPtr InventoryLargeChest::getStackInSlot(int slot)
{
	return slot >= upperChest->getSizeInventory() ?
		lowerChest->getStackInSlot(slot - upperChest->getSizeInventory()) : 
		upperChest->getStackInSlot(slot);
}

ItemStackPtr InventoryLargeChest::decrStackSize(int slot, int size)
{
	return slot >= upperChest->getSizeInventory() ? 
		lowerChest->decrStackSize(slot - upperChest->getSizeInventory(), size) :
		upperChest->decrStackSize(slot, size);
}

ItemStackPtr InventoryLargeChest::getStackInSlotOnClosing(int slot)
{
	return slot >= upperChest->getSizeInventory() ?
		lowerChest->getStackInSlotOnClosing(slot - upperChest->getSizeInventory()) :
		upperChest->getStackInSlotOnClosing(slot);
}

void InventoryLargeChest::setInventorySlotContents(int slot, ItemStackPtr pStack)
{
	if (slot >= upperChest->getSizeInventory())
	{
		lowerChest->setInventorySlotContents(slot - upperChest->getSizeInventory(), pStack);
	}
	else
	{
		upperChest->setInventorySlotContents(slot, pStack);
	}
}

void InventoryLargeChest::onInventoryChanged()
{
	upperChest->onInventoryChanged();
	lowerChest->onInventoryChanged();
}

bool InventoryLargeChest::isUseableByPlayer(EntityPlayer* pPlayer)
{
	return upperChest->isUseableByPlayer(pPlayer) && lowerChest->isUseableByPlayer(pPlayer);
}

void InventoryLargeChest::openChest()
{
	upperChest->openChest();
	lowerChest->openChest();
}

void InventoryLargeChest::closeChest()
{
	upperChest->closeChest();
	lowerChest->closeChest();
}



}