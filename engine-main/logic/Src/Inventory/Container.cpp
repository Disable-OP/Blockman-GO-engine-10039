#include "Container.h"
#include "Slot.h"
#include "Crafting.h"
#include "InventoryPlayer.h"
#include "InventoryCrafting.h"
#include "CraftingManager.h"

#include "Item/Items.h"
#include "Item/ItemStack.h"
#include "Entity/EntityPlayer.h"

namespace BLOCKMAN
{

Container::Container()
	: transactionID(0)
	, dragMode(-1)
	, dragEvent(0)
{}

Container::~Container()
{
}

Slot* Container::addSlotToContainer(Slot* pSlot)
{
	pSlot->slotNumber = inventorySlots.size();
	inventorySlots.push_back(pSlot);
	inventoryItemStacks.push_back(NULL);
	return pSlot;
}

void Container::addCraftingToCrafters(ICrafting* pCrafting)
{
	LordAssert(std::find(crafters.begin(), crafters.end(), pCrafting) == crafters.end());

	crafters.push_back(pCrafting);
	ItemStackArr stacks;
	getInventory(stacks);
	pCrafting->sendContainerAndContentsToPlayer(this, stacks);
	detectAndSendChanges();
}

void Container::removeCraftingFromCrafters(ICrafting* pCrafting)
{
	crafters.remove(pCrafting);
}

void Container::getInventory(ItemStackArr& fillto)
{
	for (int i = 0; i < int(inventorySlots.size()); ++i)
	{
		fillto.push_back(inventorySlots[i]->getStack());
	}
}

void Container::detectAndSendChanges()
{
	for (int i = 0; i < int(inventorySlots.size()); ++i)
	{
		ItemStackPtr pStack = inventorySlots[i]->getStack();
		ItemStackPtr pStack2 = inventoryItemStacks[i];

		if (!ItemStack::areItemStacksEqual(pStack2, pStack))
		{
			pStack2 = (pStack == NULL ? NULL : pStack->copy());
			inventoryItemStacks[i] = pStack2;

			for (CraftingArr::iterator it = crafters.begin(); it != crafters.end(); ++it)
			{
				(*it)->sendSlotContents(this, i, pStack2);
			}
		}
	}
}

Slot* Container::getSlotFromInventory(IInventory* pInventory, int slot)
{
	for (int i = 0; i < int(inventorySlots.size()); ++i)
	{
		Slot* pSlot = inventorySlots[i];

		if (pSlot->isSlotInInventory(pInventory, slot))
		{
			return pSlot;
		}
	}

	return NULL;
}

ItemStackPtr Container::transferStackInSlot(EntityPlayer* pPlayer, int slot)
{
	Slot* pSlot = inventorySlots[slot];
	return pSlot != NULL ? pSlot->getStack() : NULL;
}

ItemStackPtr Container::slotClick(int slot, int dragType, int clickType, EntityPlayer* pPlayer)
{
	ItemStackPtr pStack = NULL;
	InventoryPlayer* playerInventory = pPlayer->inventory;
	int size;
	ItemStackPtr pStack2;

	if (clickType == 5)
	{
		int i = dragEvent;
		dragEvent = getDragEvent(dragType);

		if ((i != 1 || dragEvent != 2) && i != dragEvent)
		{
			resetDrag();
		}
		else if (playerInventory->getItemStack() == NULL)
		{
			resetDrag();
		}
		else if (dragEvent == 0)
		{
			dragMode = extractDragMode(dragType);

			if (isValidDragMode(dragMode))
			{
				dragEvent = 1;
				dragSlots.clear();
			}
			else
			{
				resetDrag();
			}
		}
		else if (dragEvent == 1)
		{
			Slot* pSlot = inventorySlots[slot];

			if (pSlot != NULL && canAddItemToSlot(pSlot, playerInventory->getItemStack(), true) && 
				pSlot->isItemValid(playerInventory->getItemStack()) &&
				playerInventory->getItemStack()->stackSize > int(dragSlots.size()) && canDragIntoSlot(pSlot))
			{
				dragSlots.insert(pSlot);
			}
		}
		else if (dragEvent == 2)
		{
			if (!dragSlots.empty())
			{
				pStack2 = playerInventory->getItemStack()->copy();
				size = playerInventory->getItemStack()->stackSize;

				for (SlotsSet::iterator it = dragSlots.begin(); it != dragSlots.end(); ++it)
				{
					Slot* pSlot = *it;

					if (pSlot && canAddItemToSlot(pSlot, playerInventory->getItemStack(), true) &&
						pSlot->isItemValid(playerInventory->getItemStack()) && 
						playerInventory->getItemStack()->stackSize >= int(dragSlots.size()) &&
						canDragIntoSlot(pSlot))
					{
						ItemStackPtr pStack3 = pStack2->copy();
						int size2 = pSlot->getHasStack() ? pSlot->getStack()->stackSize : 0;
						computeStackSize(dragSlots, dragMode, pStack3, size2);

						if (pStack3->stackSize > pStack3->getMaxStackSize())
						{
							pStack3->stackSize = pStack3->getMaxStackSize();
						}

						if (pStack3->stackSize > pSlot->getSlotStackLimit())
						{
							pStack3->stackSize = pSlot->getSlotStackLimit();
						}

						size -= pStack3->stackSize - size2;
						pSlot->putStack(pStack3);
					}
				}

				pStack2->stackSize = size;

				if (pStack2->stackSize <= 0)
				{
					pStack2 = NULL;
				}

				playerInventory->setItemStack(pStack2);
			}

			resetDrag();
		}
		else
		{
			resetDrag();
		}
	}
	else if (dragEvent != 0)
	{
		resetDrag();
	}
	else
	{
		Slot* pSlot = NULL;
		int slotIndex;
		ItemStackPtr pStack3 = NULL;

		if ((clickType == 0 || clickType == 1) && (dragType == 0 || dragType == 1))
		{
			if (slot == -999)
			{
				if (playerInventory->getItemStack() != NULL && slot == -999)
				{
					if (dragType == 0)
					{
						pPlayer->dropPlayerItem(playerInventory->getItemStack());
						playerInventory->setItemStack(NULL);
					}

					if (dragType == 1)
					{
						pPlayer->dropPlayerItem(playerInventory->getItemStack()->splitStack(1));

						if (playerInventory->getItemStack()->stackSize == 0)
						{
							playerInventory->setItemStack(NULL);
						}
					}
				}
			}
			else if (clickType == 1)
			{
				if (slot < 0)
				{
					return NULL;
				}

				pSlot = inventorySlots[slot];

				if (pSlot && pSlot->canTakeStack(pPlayer))
				{
					pStack2 = transferStackInSlot(pPlayer, slot);

					if (pStack2)
					{
						size = pStack2->itemID;
						pStack = pStack2->copy();

						if (pSlot && pSlot->getStack() && pSlot->getStack()->itemID == size)
						{
							retrySlotClick(slot, dragType, true, pPlayer);
						}
					}
				}
			}
			else
			{
				if (slot < 0)
				{
					return NULL;
				}

				pSlot = inventorySlots[slot];

				if (pSlot)
				{
					pStack2 = pSlot->getStack();
					ItemStackPtr var20 = playerInventory->getItemStack();

					if (pStack2)
					{
						pStack = pStack2->copy();
					}

					if (!pStack2)
					{
						if (var20 && pSlot->isItemValid(var20))
						{
							slotIndex = dragType == 0 ? var20->stackSize : 1;

							if (slotIndex > pSlot->getSlotStackLimit())
							{
								slotIndex = pSlot->getSlotStackLimit();
							}

							if (var20->stackSize >= slotIndex)
							{
								pSlot->putStack(var20->splitStack(slotIndex));
							}

							if (var20->stackSize == 0)
							{
								playerInventory->setItemStack(NULL);
							}
						}
					}
					else if (pSlot->canTakeStack(pPlayer))
					{
						if (!var20)
						{
							slotIndex = dragType == 0 ? pStack2->stackSize : (pStack2->stackSize + 1) / 2;
							pStack3 = pSlot->decrStackSize(slotIndex);
							playerInventory->setItemStack(pStack3);

							if (pStack2->stackSize == 0)
							{
								pSlot->putStack(NULL);
							}

							pSlot->onPickupFromSlot(pPlayer, playerInventory->getItemStack());
						}
						else if (pSlot->isItemValid(var20))
						{
							if (pStack2->itemID == var20->itemID && pStack2->getItemDamage() == var20->getItemDamage() && ItemStack::areItemStackTagsEqual(pStack2, var20))
							{
								slotIndex = dragType == 0 ? var20->stackSize : 1;

								if (slotIndex > pSlot->getSlotStackLimit() - pStack2->stackSize)
								{
									slotIndex = pSlot->getSlotStackLimit() - pStack2->stackSize;
								}

								if (slotIndex > var20->getMaxStackSize() - pStack2->stackSize)
								{
									slotIndex = var20->getMaxStackSize() - pStack2->stackSize;
								}

								var20->splitStack(slotIndex);

								if (var20->stackSize == 0)
								{
									playerInventory->setItemStack(NULL);
								}

								pStack2->stackSize += slotIndex;
							}
							else if (var20->stackSize <= pSlot->getSlotStackLimit())
							{
								pSlot->putStack(var20);
								playerInventory->setItemStack(pStack2);
							}
						}
						else if (pStack2->itemID == var20->itemID && var20->getMaxStackSize() > 1 && 
							(!pStack2->getHasSubtypes() || pStack2->getItemDamage() == var20->getItemDamage()) &&
							ItemStack::areItemStackTagsEqual(pStack2, var20))
						{
							slotIndex = pStack2->stackSize;

							if (slotIndex > 0 && slotIndex + var20->stackSize <= var20->getMaxStackSize())
							{
								var20->stackSize += slotIndex;
								pStack2 = pSlot->decrStackSize(slotIndex);

								if (pStack2->stackSize == 0)
								{
									pSlot->putStack(NULL);
								}

								pSlot->onPickupFromSlot(pPlayer, playerInventory->getItemStack());
							}
						}
					}

					pSlot->onSlotChanged();
				}
			}
		}
		else if (clickType == 2 && dragType >= 0 && dragType < 9)
		{
			pSlot = inventorySlots[slot];

			if (pSlot->canTakeStack(pPlayer))
			{
				pStack2 = playerInventory->getStackInSlot(dragType);
				bool var18 = (pStack2 == NULL || pSlot->inventory == playerInventory && pSlot->isItemValid(pStack2));
				slotIndex = -1;

				if (!var18)
				{
					slotIndex = playerInventory->getFirstEmptyStackInInventory();
					var18 |= slotIndex > -1;
				}

				if (pSlot->getHasStack() && var18)
				{
					pStack3 = pSlot->getStack();
					playerInventory->setInventorySlotContents(dragType, pStack3->copy());

					if ((pSlot->inventory != playerInventory || !pSlot->isItemValid(pStack2)) && pStack2)
					{
						if (slotIndex > -1)
						{
							playerInventory->addItemStackToInventory(pStack2);
							pSlot->decrStackSize(pStack3->stackSize);
							pSlot->putStack(NULL);
							pSlot->onPickupFromSlot(pPlayer, pStack3);
						}
					}
					else
					{
						pSlot->decrStackSize(pStack3->stackSize);
						pSlot->putStack(pStack2);
						pSlot->onPickupFromSlot(pPlayer, pStack3);
					}
				}
				else if (!pSlot->getHasStack() && pStack2 && pSlot->isItemValid(pStack2))
				{
					playerInventory->setInventorySlotContents(dragType, NULL);
					pSlot->putStack(pStack2);
				}
			}
		}
		else if (clickType == 3 && pPlayer->capabilities.isCreativeMode && !playerInventory->getItemStack() && slot >= 0)
		{
			pSlot = inventorySlots[slot];

			if (pSlot && pSlot->getHasStack())
			{
				pStack2 = pSlot->getStack()->copy();
				pStack2->stackSize = pStack2->getMaxStackSize();
				playerInventory->setItemStack(pStack2);
			}
		}
		else if (clickType == 4 && !playerInventory->getItemStack() && slot >= 0)
		{
			pSlot = inventorySlots[slot];

			if (pSlot && pSlot->getHasStack() && pSlot->canTakeStack(pPlayer))
			{
				pStack2 = pSlot->decrStackSize(dragType == 0 ? 1 : pSlot->getStack()->stackSize);
				pSlot->onPickupFromSlot(pPlayer, pStack2);
				pPlayer->dropPlayerItem(pStack2);
			}
		}
		else if (clickType == 6 && slot >= 0)
		{
			pSlot = inventorySlots[slot];
			pStack2 = playerInventory->getItemStack();

			if (pStack2 && (!pSlot || !pSlot->getHasStack() || !pSlot->canTakeStack(pPlayer)))
			{
				size = dragType == 0 ? 0 : inventorySlots.size() - 1;
				slotIndex = dragType == 0 ? 1 : -1;

				for (int var21 = 0; var21 < 2; ++var21)
				{
					for (int var23 = size; var23 >= 0 && var23 < int(inventorySlots.size()) && pStack2->stackSize < pStack2->getMaxStackSize(); var23 += slotIndex)
					{
						Slot* pSlot2 = inventorySlots[var23];

						if (pSlot2->getHasStack() && canAddItemToSlot(pSlot2, pStack2, true) &&
							pSlot2->canTakeStack(pPlayer) && canMergeSlot(pStack2, pSlot2) && 
							(var21 != 0 || pSlot2->getStack()->stackSize != pSlot2->getStack()->getMaxStackSize()))
						{
							int var14 = Math::Min(pStack2->getMaxStackSize() - pStack2->stackSize, static_cast<int>(pSlot2->getStack()->stackSize));
							ItemStackPtr var15 = pSlot2->decrStackSize(var14);
							pStack2->stackSize += var14;

							if (var15->stackSize <= 0)
							{
								pSlot2->putStack(NULL);
							}

							pSlot2->onPickupFromSlot(pPlayer, var15);
						}
					}
				}
			}

			detectAndSendChanges();
		}
	}

	return pStack;
}

void Container::retrySlotClick(int slot, int dragType, bool par3, EntityPlayer* pPlayer)
{
	slotClick(slot, dragType, 1, pPlayer);
}

void Container::onContainerClosed(EntityPlayer* pPlayer)
{
	InventoryPlayer* playerInventory = pPlayer->inventory;

	if (playerInventory->getItemStack() != NULL)
	{
		pPlayer->dropPlayerItem(playerInventory->getItemStack());
		playerInventory->setItemStack(NULL);
	}
}

void Container::putStackInSlot(int slot, ItemStackPtr pStack)
{
	getSlot(slot)->putStack(pStack);
}

void Container::putStacksInSlots(const ItemStackArr& stacks)
{
	for (int i = 0; i < int(stacks.size()); ++i)
	{
		getSlot(i)->putStack(stacks[i]);
	}
}

i16 Container::getNextTransactionID(InventoryPlayer* pInvPlayer)
{
	++transactionID;
	return transactionID;
}

bool Container::isPlayerNotUsingContainer(EntityPlayer* pPlayer)
{
	return playerList.find(pPlayer) == playerList.end();
}

void Container::setPlayerIsPresent(EntityPlayer* pPlayer, bool remove)
{
	if (remove)
	{
		PlayerSet::iterator it = playerList.find(pPlayer);
		LordAssert(it != playerList.end());
		playerList.erase(it);
	}
	else
	{
		PlayerSet::iterator it = playerList.find(pPlayer);
		LordAssert(it == playerList.end());
		playerList.insert(pPlayer);
	}
}

bool Container::mergeItemStack(ItemStackPtr pStack, int start, int end, bool reverse)
{
	bool flag = false;
	int i = start;

	if (reverse)
	{
		i = end - 1;
	}

	Slot* pSlot = NULL;
	ItemStackPtr pSlotStack;

	if (pStack->isStackable())
	{
		while (pStack->stackSize > 0 && (!reverse && i < end || reverse && i >= start))
		{
			pSlot = inventorySlots[i];
			pSlotStack = pSlot->getStack();

			if (pSlotStack != NULL && pSlotStack->itemID == pStack->itemID &&
				(!pStack->getHasSubtypes() || pStack->getItemDamage() == pSlotStack->getItemDamage()) &&
				ItemStack::areItemStackTagsEqual(pStack, pSlotStack))
			{
				int size = pSlotStack->stackSize + pStack->stackSize;

				if (size <= pStack->getMaxStackSize())
				{
					pStack->stackSize = 0;
					pSlotStack->stackSize = size;
					pSlot->onSlotChanged();
					flag = true;
				}
				else if (pSlotStack->stackSize < pStack->getMaxStackSize())
				{
					pStack->stackSize -= pStack->getMaxStackSize() - pSlotStack->stackSize;
					pSlotStack->stackSize = pStack->getMaxStackSize();
					pSlot->onSlotChanged();
					flag = true;
				}
			}

			if (reverse)
			{
				--i;
			}
			else
			{
				++i;
			}
		}
	}

	if (pStack->stackSize > 0)
	{
		if (reverse)
		{
			i = end - 1;
		}
		else
		{
			i = start;
		}

		while (!reverse && i < end || reverse && i >= start)
		{
			pSlot = inventorySlots[i];
			pSlotStack = pSlot->getStack();

			if (pSlotStack == NULL)
			{
				pSlot->putStack(pStack->copy());
				pSlot->onSlotChanged();
				pStack->stackSize = 0;
				flag = true;
				break;
			}

			if (reverse)
			{
				--i;
			}
			else
			{
				++i;
			}
		}
	}

	return flag;
}

bool Container::canAddItemToSlot(Slot* pSlot, ItemStackPtr pStack, bool stackSizeMatters)
{
	bool bResult = pSlot == NULL || !pSlot->getHasStack();

	if (pSlot && pSlot->getHasStack() && pStack && 
		pStack->isItemEqual(pSlot->getStack()) &&
		ItemStack::areItemStackTagsEqual(pSlot->getStack(), pStack))
	{
		int size = stackSizeMatters ? 0 : pStack->stackSize;
		bResult |= pSlot->getStack()->stackSize + size <= pStack->getMaxStackSize();
	}

	return bResult;
}

void Container::computeStackSize(SlotsSet& slots, int dragmode, ItemStackPtr pstack, int size)
{
	switch (dragmode)
	{
	case 0:
		pstack->stackSize = int(Math::Floor((float)pstack->stackSize / (float)slots.size()));
		break;

	case 1:
		pstack->stackSize = 1;
	}

	pstack->stackSize += size;
}

int Container::calcRedstoneFromInventory(IInventory* pInventory)
{
	if (!pInventory)
	{
		return 0;
	}
	else
	{
		int count = 0;
		float f = 0.0F;

		for (int i = 0; i < pInventory->getSizeInventory(); ++i)
		{
			ItemStackPtr pStack = pInventory->getStackInSlot(i);

			if (pStack)
			{
				f += (float)pStack->stackSize / (float)Math::Min(pInventory->getInventoryStackLimit(), pStack->getMaxStackSize());
				++count;
			}
		}

		f /= (float)pInventory->getSizeInventory();
		return int(Math::Floor(f * 14.0F) + (count > 0 ? 1 : 0));
	}
}

ContainerPlayer::ContainerPlayer(InventoryPlayer* pInvPlayer, bool isLocal, EntityPlayer* pPlayer)
	: isLocalWorld(isLocal)
	, thePlayer(pPlayer)
{
	craftMatrix = LordNew InventoryCrafting(this, 2, 2);
	craftResult = LordNew InventoryCraftResult();

	addSlotToContainer(LordNew SlotCrafting(pInvPlayer->player, craftMatrix, craftResult, 0, 144, 36));

	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			addSlotToContainer(LordNew Slot(craftMatrix, j + i * 2, 88 + j * 18, 26 + i * 18));
		}
	}

	for (int i = 0; i < 4; ++i)
	{
		addSlotToContainer(LordNew SlotArmor(this, pInvPlayer, pInvPlayer->getSizeInventory() - 1 - i, 8, 8 + i * 18, i));
	}

	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 9; ++j)
		{
			addSlotToContainer(LordNew Slot(pInvPlayer, j + (i + 1) * 9, 8 + j * 18, 84 + i * 18));
		}
	}

	for (int i = 0; i < 9; ++i)
	{
		addSlotToContainer(LordNew Slot(pInvPlayer, i, 8 + i * 18, 142));
	}

	onCraftMatrixChanged(craftMatrix);
}

ContainerPlayer::~ContainerPlayer()
{
	for (SlotsArr::iterator it = inventorySlots.begin(); it != inventorySlots.end(); ++it)
	{
		Slot* pslot = *it;
		LordSafeDelete(pslot);
	}
	LordSafeDelete(craftMatrix);
	LordSafeDelete(craftResult);
}

void ContainerPlayer::onCraftMatrixChanged(IInventory* pInventory)
{
	craftResult->setInventorySlotContents(0, CraftingManager::Instance()->findMatchingRecipe(craftMatrix, thePlayer->getWorldPtr()));
}

void ContainerPlayer::onContainerClosed(EntityPlayer* pPlayer)
{
	Container::onContainerClosed(pPlayer);

	for (int i = 0; i < 4; ++i)
	{
		ItemStackPtr pSlot = craftMatrix->getStackInSlotOnClosing(i);

		if (pSlot)
		{
			pPlayer->dropPlayerItem(pSlot);
		}
	}

	craftResult->setInventorySlotContents(0, NULL);
}

ItemStackPtr ContainerPlayer::transferStackInSlot(EntityPlayer* pPlayer, int slot)
{
	ItemStackPtr pStack = NULL;
	Slot* pSlot = inventorySlots[slot];

	if (pSlot && pSlot->getHasStack())
	{
		ItemStackPtr pSlotStack = pSlot->getStack();
		pStack = pSlotStack->copy();
		ItemArmor* pArmor = dynamic_cast<ItemArmor*>(pStack->getItem());

		if (slot == 0)
		{
			if (!mergeItemStack(pSlotStack, 9, 45, true))
			{
				return NULL;
			}

			pSlot->onSlotChange(pSlotStack, pStack);
		}
		else if (slot >= 1 && slot < 5)
		{
			if (!mergeItemStack(pSlotStack, 9, 45, false))
			{
				return NULL;
			}
		}
		else if (slot >= 5 && slot < 9)
		{
			if (!mergeItemStack(pSlotStack, 9, 45, false))
			{
				return NULL;
			}
		}
		else if (pArmor	&&
			!(inventorySlots[5 + pArmor->armorType]->getHasStack()))
		{
			int slot2 = 5 + pArmor->armorType;

			if (!mergeItemStack(pSlotStack, slot2, slot2 + 1, false))
			{
				return NULL;
			}
		}
		else if (slot >= 9 && slot < 36)
		{
			if (!mergeItemStack(pSlotStack, 36, 45, false))
			{
				return NULL;
			}
		}
		else if (slot >= 36 && slot < 45)
		{
			if (!mergeItemStack(pSlotStack, 9, 36, false))
			{
				return NULL;
			}
		}
		else if (!mergeItemStack(pSlotStack, 9, 45, false))
		{
			return NULL;
		}

		if (pSlotStack->stackSize == 0)
		{
			pSlot->putStack(NULL);
		}
		else
		{
			pSlot->onSlotChanged();
		}

		if (pSlotStack->stackSize == pStack->stackSize)
		{
			return NULL;
		}

		pSlot->onPickupFromSlot(pPlayer, pSlotStack);
	}

	return pStack;
}

bool ContainerPlayer::canMergeSlot(ItemStackPtr pStack, Slot* pSlot)
{
	return pSlot->inventory != craftResult && Container::canMergeSlot(pStack, pSlot);
}

ContainerChest::ContainerChest(IInventory* inventory1, IInventory* inventory2)
{
	lowerChestInventory = inventory2;
	numRows = inventory2->getSizeInventory() / 9;
	inventory2->openChest();
	int var3 = (numRows - 4) * 18;

	for (int r = 0; r < numRows; ++r)
	{
		for (int c = 0; c < 9; ++c)
		{
			addSlotToContainer(new Slot(inventory2, c + r * 9, 8 + c * 18, 18 + r * 18));
		}
	}

	for (int r = 0; r < 3; ++r)
	{
		for (int c = 0; c < 9; ++c)
		{
			addSlotToContainer(new Slot(inventory1, c + r * 9 + 9, 8 + c * 18, 103 + r * 18 + var3));
		}
	}

	for (int r = 0; r < 9; ++r)
	{
		addSlotToContainer(new Slot(inventory1, r, 8 + r * 18, 161 + var3));
	}
}

bool ContainerChest::canInteractWith(EntityPlayer* pPlayer)
{
	return lowerChestInventory->isUseableByPlayer(pPlayer);
}

ItemStackPtr ContainerChest::transferStackInSlot(EntityPlayer* pPlayer, int slot)
{
	ItemStackPtr pResult = nullptr;
	Slot* pSlot = inventorySlots[slot];

	if (pSlot && pSlot->getHasStack())
	{
		ItemStackPtr pStack = pSlot->getStack();
		pResult = pStack->copy();

		if (slot < numRows * 9)
		{
			if (!mergeItemStack(pStack, numRows * 9, inventorySlots.size(), true))
			{
				return nullptr;
			}
		}
		else if (!mergeItemStack(pStack, 0, numRows * 9, false))
		{
			return nullptr;
		}

		if (pStack->stackSize == 0)
		{
			pSlot->putStack(nullptr);
		}
		else
		{
			pSlot->onSlotChanged();
		}
	}

	return pResult;
}

void ContainerChest::onContainerClosed(EntityPlayer* pPlayer)
{
	Container::onContainerClosed(pPlayer);
	lowerChestInventory->closeChest();
}

}