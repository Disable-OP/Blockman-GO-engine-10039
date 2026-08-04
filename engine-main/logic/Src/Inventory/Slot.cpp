#include "Slot.h"

#include "Item/Item.h"
#include "Item/Items.h"
#include "Item/ItemStack.h"

#include "Inventory/IInventory.h"
#include "Inventory/InventoryPlayer.h"
#include "Entity/EntityPlayer.h"

namespace BLOCKMAN
{

Slot::Slot(IInventory* pInventory, int slotIdx, int xPos, int yPos)
{
	inventory = pInventory;
	slotIndex = slotIdx;
	xDisplayPosition = xPos;
	yDisplayPosition = yPos;
}

void Slot::onSlotChange(ItemStackPtr pStack1, ItemStackPtr pStack2)
{
	if (pStack1 != NULL && pStack2 != NULL)
	{
		if (pStack1->itemID == pStack2->itemID)
		{
			int size = pStack2->stackSize - pStack1->stackSize;

			if (size > 0)
			{
				onCrafting(pStack1, size);
			}
		}
	}
}

ItemStackPtr Slot::getStack()
{
	return inventory->getStackInSlot(slotIndex);
}

void Slot::putStack(ItemStackPtr pStack)
{
	inventory->setInventorySlotContents(slotIndex, pStack);
	onSlotChanged();
}

void Slot::onSlotChanged()
{
	inventory->onInventoryChanged();
}

int Slot::getSlotStackLimit()
{
	return inventory->getInventoryStackLimit();
}

ItemStackPtr Slot::decrStackSize(int size)
{
	return inventory->decrStackSize(slotIndex, size);
}

SlotCrafting::SlotCrafting(EntityPlayer* pPlayer, IInventory* pInventory1, IInventory* pInventory2, int slotIdx, int xPos, int yPos)
	: Slot(pInventory2, slotIdx, xPos, yPos)
	, thePlayer(pPlayer)
	, craftMatrix(pInventory1)
	, amountCrafted(0)
{
}

ItemStackPtr SlotCrafting::decrStackSize(int size)
{
	if (getHasStack())
	{
		amountCrafted += std::min<int>(size, getStack()->stackSize);
	}

	return Slot::decrStackSize(size);
}

void SlotCrafting::onCrafting(ItemStackPtr pStack, int size)
{
	amountCrafted += size;
	onCrafting(pStack);
}

void SlotCrafting::onCrafting(ItemStackPtr pStack)
{
	pStack->onCrafting(thePlayer->world, thePlayer, amountCrafted);
	amountCrafted = 0;

	// todo.
	/*if (pStack->itemID == BLOCK_ID_WORK_BENCH)
	{
		thePlayer->addStat(AchievementList::buildWorkBench, 1);
	}
	else if (pStack->itemID == Item::pickaxeWood->itemID)
	{
		thePlayer->addStat(AchievementList.buildPickaxe, 1);
	}
	else if (pStack->itemID == Block.furnaceIdle.blockID)
	{
		thePlayer.addStat(AchievementList.buildFurnace, 1);
	}
	else if (pStack->itemID == Item.hoeWood.itemID)
	{
		thePlayer.addStat(AchievementList.buildHoe, 1);
	}
	else if (pStack->itemID == Item.bread.itemID)
	{
		thePlayer.addStat(AchievementList.makeBread, 1);
	}
	else if (pStack->itemID == Item.cake.itemID)
	{
		thePlayer.addStat(AchievementList.bakeCake, 1);
	}
	else if (pStack->itemID == Item.pickaxeStone.itemID)
	{
		thePlayer.addStat(AchievementList.buildBetterPickaxe, 1);
	}
	else if (pStack->itemID == Item.swordWood.itemID)
	{
		thePlayer.addStat(AchievementList.buildSword, 1);
	}
	else if (pStack->itemID == Block.enchantmentTable.blockID)
	{
		thePlayer.addStat(AchievementList.enchantments, 1);
	}
	else if (pStack->itemID == Block.bookShelf.blockID)
	{
		thePlayer.addStat(AchievementList.bookcase, 1);
	}*/
}

void SlotCrafting::onPickupFromSlot(EntityPlayer* pPlayer, ItemStackPtr pStack)
{
	onCrafting(pStack);

	for (int i = 0; i < craftMatrix->getSizeInventory(); ++i)
	{
		ItemStackPtr pStack = craftMatrix->getStackInSlot(i);

		if (pStack)
		{
			craftMatrix->decrStackSize(i, 1);

			if (pStack->getItem()->hasContainerItem())
			{
				ItemStackPtr pContainerStack = LORD::make_shared<ItemStack>(pStack->getItem()->getContainerItem());

				if (!pStack->getItem()->doesContainerItemLeaveCraftingGrid(pStack) ||
					!thePlayer->inventory->addItemStackToInventory(pContainerStack))
				{
					if (!craftMatrix->getStackInSlot(i))
					{
						craftMatrix->setInventorySlotContents(i, pContainerStack);
					}
					else
					{
						thePlayer->dropPlayerItem(pContainerStack);
					}
				}
			}
		}
	}
}

SlotArmor::SlotArmor(ContainerPlayer* pContainerPlayer, IInventory* pInventory, int slotIdx, int xPos, int yPos, int armortype)
	: Slot(pInventory, slotIdx, xPos, yPos)
	, parent(pContainerPlayer)
	, armorType(armortype)
{
}

bool SlotArmor::isItemValid(ItemStackPtr pStack)
{
	if (!pStack)
		return false;

	ItemArmor* pArmor = dynamic_cast<ItemArmor*>(pStack->getItem());
	if (pArmor)
	{
		return pArmor->armorType == armorType;
	}
	else
	{
		if (pStack->getItem()->itemID != BLOCK_ID_PUMPKIN &&
			pStack->getItem()->itemID != BLOCK_ID_SKULL)
			return false;
		else
			return armorType == 0;
	}

	return false; // silence the warning.
}

}
