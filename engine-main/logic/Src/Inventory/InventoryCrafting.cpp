#include "InventoryCrafting.h"
#include "Container.h"

#include "Item/ItemStack.h"

namespace BLOCKMAN
{

InventoryCrafting::InventoryCrafting(Container* pContainer, int width, int height)
{
	stackVectorLen = width * height;
	stackVector.resize(stackVectorLen);
	eventHandler = pContainer;
	inventoryWidth = width;
}

InventoryCrafting::~InventoryCrafting()
{
	stackVector.clear();
}

ItemStackPtr InventoryCrafting::getStackInRowAndColumn(int xPos, int yPos)
{
	if (xPos >= 0 && xPos < inventoryWidth)
	{
		int slot = xPos + yPos * inventoryWidth;
		return getStackInSlot(slot);
	}
	else
	{
		return nullptr;
	}
}

String InventoryCrafting::getInvName()
{
	return "container.crafting";
}

ItemStackPtr InventoryCrafting::getStackInSlotOnClosing(int slot)
{
	if (stackVector[slot])
	{
		ItemStackPtr pStack = stackVector[slot];
		stackVector[slot] = nullptr;
		return pStack;
	}
	return nullptr;
}

ItemStackPtr InventoryCrafting::decrStackSize(int slot, int size)
{
	if (stackVector[slot])
	{
		ItemStackPtr pStack;

		if (stackVector[slot]->stackSize <= size)
		{
			pStack = stackVector[slot];
			stackVector[slot] = nullptr;
			eventHandler->onCraftMatrixChanged(this);
			return pStack;
		}
		else
		{
			pStack = stackVector[slot]->splitStack(size);

			if (stackVector[slot]->stackSize == 0)
			{
				stackVector[slot] = nullptr;
			}

			eventHandler->onCraftMatrixChanged(this);
			return pStack;
		}
	}

	return nullptr;
}

void InventoryCrafting::setInventorySlotContents(int slot, ItemStackPtr pStack)
{
	stackVector[slot] = pStack;
	eventHandler->onCraftMatrixChanged(this);
}

InventoryCraftResult::InventoryCraftResult()
{
	stackResult = nullptr;
}

InventoryCraftResult::~InventoryCraftResult()
{
	// LordSafeDelete(stackResult);
}

ItemStackPtr InventoryCraftResult::decrStackSize(int slot, int size)
{
	if (stackResult)
	{
		ItemStackPtr pStack = stackResult;
		stackResult = nullptr;
		return pStack;
	}
	return nullptr;
}

ItemStackPtr InventoryCraftResult::getStackInSlotOnClosing(int slot)
{
	if (stackResult)
	{
		ItemStackPtr pStack = stackResult;
		stackResult = nullptr;
		return pStack; 
	}
	return nullptr;
}

}