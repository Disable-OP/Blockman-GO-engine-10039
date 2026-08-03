#include "InventoryPlayer.h"
#include "Item/ItemStack.h"
#include "Item/Item.h"
#include "Item/Items.h"
#include "Entity/EntityPlayer.h"
#include "Common.h"
#include "Inventory/InventorySubset.h"
#include "Inventory/InventoryFiltered.h"
#include "Inventory/InventoryCraftable.h"
#include "Inventory/InventoryHandCraftable.h"
#include "Util/CommonEvents.h"
#include "Setting/LogicSetting.h"

#include "Block/Block.h"
#include "Block/BM_Material.h"
#include "World/World.h"
#include "Setting/BulletClipSetting.h"
#include "Setting/GunSetting.h"
#include "Entity/Enchantment.h"

namespace BLOCKMAN
{

using namespace std::placeholders;

InventoryPlayer::InventoryPlayer(EntityPlayer* pPlayer)
	: player(pPlayer)
{
	size_t index = 0;
	for (auto& ptr : mainInventory)
	{
		m_subscriptionGuard.add(ptr.subscribe(std::bind(&InventoryPlayer::onItemStackChanged, this, index++, _1, _2)));
		ptr = nullptr;
	}
	for (auto& ptr : armorInventory)
	{
		m_subscriptionGuard.add(ptr.subscribe(std::bind(&InventoryPlayer::onItemStackChanged, this, index++, _1, _2)));
		ptr = nullptr;
	}
	for (size_t i = 0; i < HOTBAR_COUNT; ++i)
	{
		hotbar[i] = nullptr;
	}
	m_subscriptionGuard.add(m_inventorySync.subscribe(std::bind(&InventoryPlayer::onInventorySync, this, _1, _2)));

	currentItemIndex = 0;
	currentItemStack = nullptr;
	itemStack = nullptr;
}

LocalEvent::Subscription InventoryPlayer::onItemInHandChanged(std::function<void()> callback)
{
	if (!m_monitoredItemInHand)
	{
		m_subscriptionGuard.add(currentItemIndex.subscribe(std::bind(&InventoryPlayer::onCurrentItemIndexChanged, this, _1, _2)));
		monitorSelectedHotbarSlot();
		m_monitoredItemInHand = true;
	}
	return m_itemInHandChangeEvent.subscribe(callback);
}

LocalEvent::Subscription InventoryPlayer::onInventoryContentChanged(std::function<void()> callback)
{
	return m_inventoryChangeEvent.subscribe(callback);
}

LocalEvent::Subscription InventoryPlayer::onInventoryContentSync(std::function<void()> callback)
{
	return m_inventorySyncEvent.subscribe(callback);
}

InventoryPlayer::~InventoryPlayer()
{
	m_currentItemChangeSubscription.unsubscribe();
	for (int i = 0; i < MAIN_INVENTORY_COUNT; ++i)
	{
		mainInventory[i] = nullptr;
	}
	for (int i = 0; i < ARMOR_INVENTORY_COUNT; ++i)
	{
		armorInventory[i] = nullptr;
	}
	for (int i = 0; i < HOTBAR_COUNT; ++i)
	{
		hotbar[i] = nullptr;
	}
	clearPrev();
	LordSafeDelete(m_mainInventoryProxy);
	LordSafeDelete(m_armorInventoryProxy);
	LordSafeDelete(m_armorFilteredInventoryProxy);
	LordSafeDelete(m_inventoryAllCraftable);
	LordSafeDelete(m_inventoryHandCraftable);
	m_subscriptionGuard.unsubscribeAll();
}

void InventoryPlayer::clearPrev()
{
	for (int i = 0; i < MAIN_INVENTORY_COUNT; ++i)
	{
		prevMainInventory[i] = nullptr;
	}
	for (int i = 0; i < ARMOR_INVENTORY_COUNT; ++i)
	{
		prevArmorInventory[i] = nullptr;
	}
}

ItemStackPtr InventoryPlayer::getCurrentItem()
{
	if (currentItemIndex < 9 && currentItemIndex >= 0)
	{
		return hotbar[currentItemIndex];
	}
	else
	{
		return nullptr;
	}
}

int InventoryPlayer::getInventoryIndexOfCurrentItem()
{
	return findItemStack(getCurrentItem());
}

bool InventoryPlayer::onItemStackChanged(size_t index, ItemStackPtr old_itemStack, ItemStackPtr new_itemStack)
{
	auto& subscriptionGuard = m_itemStackSubscriptionGuards[index];
	subscriptionGuard.unsubscribeAll();
	if (new_itemStack)
	{
		subscriptionGuard.add(new_itemStack->stackSize.subscribe([this](int, int) -> bool {
			if (!player->isThrowingSnowball() && !player->isPlacingBlock()) {
				m_inventoryChangeEvent.emit();
				return true;
			}
			int index = getInventoryIndexOfCurrentItem();
			prevMainInventory[index] = mainInventory[index] == nullptr ? nullptr : mainInventory[index]->copy();
			return true;
		}));
		subscriptionGuard.add(new_itemStack->itemDamage.subscribe([this](int, int) -> bool {
			m_inventoryChangeEvent.emit();
			return true;
		}));
		subscriptionGuard.add(new_itemStack->onEnchantmentAdded([this] {
			m_inventoryChangeEvent.emit();
		}));
	}

	if (index >= MAIN_INVENTORY_COUNT && index < MAIN_INVENTORY_COUNT + ARMOR_INVENTORY_COUNT && m_armorDestroyed) {
		BLOCKMAN::GameCommon::Instance()->getCommonPacketSender()->sendDestroyArmorToTrackingPlayers(this->player->entityId, index);
		m_armorDestroyed = false;
	}

	if (!player->isThrowingSnowball() && !player->isPlacingBlock()) {
		m_inventoryChangeEvent.emit();
	}
	else
	{
		int index = getInventoryIndexOfCurrentItem();
		prevMainInventory[index] = mainInventory[index] == nullptr ? nullptr : mainInventory[index]->copy();
	}
	
	return true;
}

bool InventoryPlayer::onCurrentItemIndexChanged(int old_index, int new_index)
{
	m_currentItemChangeSubscription.unsubscribe();
	monitorSelectedHotbarSlot();
	m_itemInHandChangeEvent.emit();
	return true;
}

bool InventoryPlayer::onInventorySync(bool old_changed, bool new_changed)
{
 	if (!old_changed && new_changed) {
		m_inventorySyncEvent.emit();
		m_inventorySync = false;
	}
	return true;
}

void InventoryPlayer::monitorSelectedHotbarSlot()
{
	m_currentItemChangeSubscription = hotbar[currentItemIndex].subscribe([this](ItemStackPtr, ItemStackPtr) -> bool
	{
		m_itemInHandChangeEvent.emit();
		return true;
	});
}

int InventoryPlayer::getInventorySlotContainItem(int itemid)
{
	for (int i = 0; i < MAIN_INVENTORY_COUNT; ++i)
	{
		ItemStackPtr stack = mainInventory[i];
		if (mainInventory[i] != nullptr &&
			stack->itemID == itemid)
		{
			return i;
		}
	}

	return -1;
}

int InventoryPlayer::getInventorySlotContainItemAndDamage(int itemid, int subType)
{
	for (int i = 0; i < MAIN_INVENTORY_COUNT; ++i)
	{
		if (mainInventory[i] != nullptr &&
			mainInventory[i]->itemID == itemid &&
			mainInventory[i]->getItemDamage() == subType)
		{
			return i;
		}
	}

	return -1;
}

int InventoryPlayer::storeItemStack(ItemStackPtr pStack)
{
	for (int i = 0; i < MAIN_INVENTORY_COUNT; ++i)
	{
		if (mainInventory[i] != nullptr &&
			mainInventory[i]->itemID == pStack->itemID &&
			mainInventory[i]->isStackable() &&
			mainInventory[i]->stackSize < mainInventory[i]->getMaxStackSize() &&
			mainInventory[i]->stackSize < getInventoryStackLimit() &&
			(!mainInventory[i]->getHasSubtypes() ||
				mainInventory[i]->getItemDamage() == pStack->getItemDamage()) &&
			ItemStack::areItemStackTagsEqual(mainInventory[i], pStack))
		{
			return i;
		}
	}

	return -1;
}

int InventoryPlayer::getFirstEmptyStackInInventory()
{
	int size = LogicSetting::Instance()->getMaxInventorySize();
	for (int i = 0; i < size; ++i)
	{
		if (mainInventory[i] == nullptr)
		{
			return i;
		}
	}
	return -1;
}

int InventoryPlayer::getFirstEmptyStackInHotbar()
{
	for (int i = 0; i < HOTBAR_COUNT; ++i)
	{
		if (hotbar[i] == nullptr)
		{
			return i;
		}
	}
	return -1;
}

int InventoryPlayer::getLastEmptyStackInInventory()
{
	int size = LogicSetting::Instance()->getMaxInventorySize();
	for (int i = 0; i < size; ++i)
	{
		if (mainInventory[size - i - 1] == nullptr)
		{
			return size - i - 1;
		}
	}
	return -1;
}

int InventoryPlayer::getLastEmptyStackInHotbar()
{
	for (int i = 0; i < HOTBAR_COUNT; ++i)
	{
		if (hotbar[HOTBAR_COUNT - i - 1] == nullptr)
		{
			return HOTBAR_COUNT - i - 1;
		}
	}
	return -1;
}

void InventoryPlayer::putItemToHotbar(int slot)
{
	if (slot < MAIN_INVENTORY_COUNT)
	{
		int originalItemIndex = -1;
		for (int i = 0; i < HOTBAR_COUNT; ++i)
		{
			if (hotbar[i] == static_cast<ItemStackPtr>(mainInventory[slot]))
			{
				originalItemIndex = i;
			}
		}
		ItemStackPtr tmp = hotbar[currentItemIndex];
		hotbar[currentItemIndex] = mainInventory[slot];
		if (originalItemIndex != -1) {
			hotbar[originalItemIndex] = tmp;
		}
	}
}

void InventoryPlayer::removeItemFromHotbar(int slot)
{
	if (slot >= 0 && slot < HOTBAR_COUNT)
	{
		hotbar[slot] = nullptr;
	}
}

int InventoryPlayer::getItemSlotByHotbat(int ItemId)
{
	int nSlot = -1;
	for (int i = 0; i < HOTBAR_COUNT; ++i)
	{
		ItemStackPtr tmp = hotbar[i];
		if (tmp != nullptr && tmp->itemID == ItemId)		{
			nSlot = i;
			break;
		}
	}
	return nSlot;
}

//void InventoryPlayer::setCurrentItem(int itemID, int subType, bool hasSubType, bool isCreativeMode)
//{
//	if (isCreativeMode && itemID > 0)
//	{
//		addItemToInventory(Item::itemsList[itemID], subType);
//	}
//}

void InventoryPlayer::setCurrentItem(ItemStackPtr itemStack)
{
	int inventoryIndex = findItemStack(hotbar[currentItemIndex]);
	hotbar[currentItemIndex] = itemStack;
	mainInventory[inventoryIndex] = itemStack;
}

void InventoryPlayer::changeCurrentItem(int idx)
{
	if (idx > 0)
		idx = 1;

	if (idx < 0)
		idx = -1;

	for (currentItemIndex -= idx; currentItemIndex < 0; currentItemIndex += 9)
	{
		;
	}

	while (currentItemIndex >= 9)
	{
		currentItemIndex -= 9;
	}
}

void InventoryPlayer::changeCurrentItemToNext()
{
	int count = 9;

	do 
	{
		count--;
		if (currentItemIndex >= 8)
		{
			currentItemIndex = 0;
		}
		else
		{
			++currentItemIndex;
		}
	} 
	while (hotbar[currentItemIndex] == nullptr && count > 0);
}

int InventoryPlayer::clearInventory(int itemID, int subType)
{
	int stackSize = 0;
	ItemStackPtr pStack;

	for (int i = 0; i < MAIN_INVENTORY_COUNT; ++i)
	{
		pStack = mainInventory[i];

		if (pStack != nullptr &&
			(itemID <= -1 || pStack->itemID == itemID) &&
			(subType <= -1 || pStack->getItemDamage() == subType))
		{
			stackSize += pStack->stackSize;
		}
	}

	for (int i = 0; i < ARMOR_INVENTORY_COUNT; ++i)
	{
		pStack = armorInventory[i];

		if (pStack != nullptr &&
			(itemID <= -1 || pStack->itemID == itemID) &&
			(subType <= -1 || pStack->getItemDamage() == subType))
		{
			stackSize += pStack->stackSize;
		}
	}

	for (int i = 0; i < HOTBAR_COUNT; ++i)
	{
		hotbar[i] = nullptr;
	}

	if (itemStack != nullptr)
	{
		if (itemID > -1 && itemStack->itemID != itemID)
		{
			return stackSize;
		}

		if (subType > -1 && itemStack->getItemDamage() != subType)
		{
			return stackSize;
		}

		stackSize += itemStack->stackSize;
		setItemStack(nullptr);
	}

	return stackSize;
}

void InventoryPlayer::addItemToInventory(Item* item, int subtype)
{
	if (!item)
		return;

	if (currentItemStack)
	{
		int inventoryIndex = getInventorySlotContainItemAndDamage(currentItemStack->itemID, currentItemStack->getItemDamageForDisplay());
		if (currentItemStack->isItemEnchantable() &&
			inventoryIndex >= 0 && mainInventory[inventoryIndex] == static_cast<ItemStackPtr>(hotbar[currentItemIndex]))
		{
			return;
		}
	}

	int slotIdx = getInventorySlotContainItemAndDamage(item->itemID, subtype);

	if (slotIdx >= 0)
	{
		int stackSize = mainInventory[slotIdx]->stackSize;
		mainInventory[slotIdx] = LORD::make_shared<ItemStack>(Item::itemsList[item->itemID], stackSize, subtype);
		hotbar[currentItemIndex] = mainInventory[slotIdx];
	}
	else
	{
		int hotbarEmptySlot = getFirstEmptyStackInHotbar();
		if (hotbarEmptySlot >= 0)
		{
			currentItemIndex = hotbarEmptySlot;
		}
		for (int i = 0; i < MAIN_INVENTORY_COUNT; ++i)
		{
			if (mainInventory[i] == static_cast<ItemStackPtr>(hotbar[currentItemIndex]))
			{
				mainInventory[i] = LORD::make_shared<ItemStack>(Item::itemsList[item->itemID], 1, subtype);
				hotbar[currentItemIndex] = mainInventory[i];
				break;
			}
		}
	}
}

int InventoryPlayer::storePartialItemStack(ItemStackPtr pStack, bool isReverse)
{
	int itemid = pStack->itemID;
	int stacksize = pStack->stackSize;
	int slot = -1;

	if (pStack->getMaxStackSize() == 1)
	{
		slot = isReverse ? getLastEmptyStackInInventory() : getFirstEmptyStackInInventory();

		if (slot < 0)
		{
			return stacksize;
		}
		else
		{
			if (mainInventory[slot] == nullptr)
			{
				mainInventory[slot] = ItemStack::copyItemStack(pStack);
				mainInventory[slot]->stackSize = 1;
				int hotbarSlot = -1;
				if (isReverse)
				{
					if (slot < HOTBAR_COUNT)
					{
						hotbarSlot = getLastEmptyStackInHotbar();
					}
				}
				else
				{
					hotbarSlot = getFirstEmptyStackInHotbar();
				}
				if (hotbarSlot >= 0)
				{
					hotbar[hotbarSlot] = mainInventory[slot];
				}
				return stacksize - 1;
			}

			return stacksize;
		}
	}
	else
	{
		slot = storeItemStack(pStack);

		if (slot < 0)
		{
			slot = isReverse ? getLastEmptyStackInInventory() : getFirstEmptyStackInInventory();
		}

		if (slot < 0)
		{
			return stacksize;
		}
		else
		{
			if (mainInventory[slot] == nullptr)
			{
				mainInventory[slot] = LORD::make_shared<ItemStack>(itemid, 0, pStack->getItemDamage());

				if (pStack->hasTagCompound())
				{
					mainInventory[slot]->setTagCompound((NBTTagCompound*)pStack->getTagCompound()->copy());
				}
				int hotbarSlot = -1;
				if (isReverse)
				{
					if (slot < HOTBAR_COUNT)
					{
						hotbarSlot = getLastEmptyStackInHotbar();
					}
				}
				else
				{
					hotbarSlot = getFirstEmptyStackInHotbar();
				}
				if (hotbarSlot >= 0)
				{
					hotbar[hotbarSlot] = mainInventory[slot];
				}
			}

			int stacksize2 = stacksize;

			if (stacksize > mainInventory[slot]->getMaxStackSize() - mainInventory[slot]->stackSize)
			{
				stacksize2 = mainInventory[slot]->getMaxStackSize() - mainInventory[slot]->stackSize;
			}

			if (stacksize2 > getInventoryStackLimit() - mainInventory[slot]->stackSize)
			{
				stacksize2 = getInventoryStackLimit() - mainInventory[slot]->stackSize;
			}

			if (stacksize2 == 0)
			{
				return stacksize;
			}
			else
			{
				stacksize -= stacksize2;
				mainInventory[slot]->stackSize += stacksize2;
				mainInventory[slot]->animationsToGo = 5;
				return stacksize;
			}
		}
	}
}

void InventoryPlayer::decrementAnimations()
{
	for (int i = 0; i < MAIN_INVENTORY_COUNT; ++i)
	{
		if (mainInventory[i] != nullptr)
		{
			mainInventory[i]->updateAnimation(player->world, player, i, hotbar[currentItemIndex] == static_cast<ItemStackPtr>(mainInventory[i]));
		}
	}
}

bool InventoryPlayer::consumeInventoryItem(int itemid, int num)
{
	int slot = getInventorySlotContainItem(itemid);

	if (slot < 0)
	{
		return false;
	}
	else
	{
		// if (--mainInventory[slot]->stackSize <= 0)
		mainInventory[slot]->stackSize -= num;
		if (mainInventory[slot]->stackSize <= 0)
		{
			for (int i = 0; i < HOTBAR_COUNT; ++i)
			{
				if (hotbar[i] == static_cast<ItemStackPtr>(mainInventory[slot]))
				{
					hotbar[i] = nullptr;
				}
			}
			mainInventory[slot] = nullptr;
		}

		return true;
	}
}

bool InventoryPlayer::hasItem(int itemid)
{
	int slot = getInventorySlotContainItem(itemid);
	return slot >= 0;
}

bool InventoryPlayer::hasItemInSlot(int slot)
{
	if (slot < 0 || slot >= HOTBAR_COUNT)
	{
		return false;
	}

	return hotbar[slot] != nullptr;
}

ItemStackPtr InventoryPlayer::getArrowItemStack()
{
	Item::arrow->itemID;
	for (int i = 0; i < MAIN_INVENTORY_COUNT; ++i)
	{
		ItemStackPtr stack = mainInventory[i];
		if (stack != nullptr) {
			if (stack->itemID == Item::arrow->itemID || dynamic_cast<ItemArrowPotion*>(stack->getItem())
				|| stack->itemID == Item::redArrow->itemID|| stack->itemID == Item::blackArrow->itemID) {
				return stack;
			}
		}
	}

	return nullptr;
}

ItemStackPtr InventoryPlayer::getBulletItemStack(int id)
{
	for (int i = 0; i < MAIN_INVENTORY_COUNT; ++i)
	{
		ItemStackPtr stack = mainInventory[i];
		if (stack != nullptr)
		{
			if (stack->itemID == id)
				return stack;
		}
	}
	return nullptr;
}

bool InventoryPlayer::addItemStackToInventory(ItemStackPtr pStack, bool isReverse)
{
	bool nResult = false;
	int slot = -1;

	do 
	{
		ERROR_BREAK(pStack);
		ERROR_BREAK(pStack->stackSize != 0);
		if (pStack->isItemDamaged())
		{
			slot = isReverse ? getLastEmptyStackInInventory() : getFirstEmptyStackInInventory();

			if (slot >= 0)
			{
				mainInventory[slot] = ItemStack::copyItemStack(pStack);
				mainInventory[slot]->animationsToGo = 5;
				pStack->stackSize = 0;
				nResult = true;
				int hotbarSlot = -1;
				if (isReverse)
				{
					if (slot < HOTBAR_COUNT)
					{
						hotbarSlot = getLastEmptyStackInHotbar();
					}
				}
				else
				{
					hotbarSlot = getFirstEmptyStackInHotbar();
				}
				if (hotbarSlot >= 0)
				{
					hotbar[hotbarSlot] = mainInventory[slot];
				}
			}
			else if (player->capabilities.isCreativeMode)
			{
				pStack->stackSize = 0;
				nResult = true;
			}
			else
				nResult = false;
		}
		else
		{
			int stackSize;
			do
			{
				stackSize = pStack->stackSize;
				pStack->stackSize = storePartialItemStack(pStack, isReverse);
			} while (pStack->stackSize > 0 && pStack->stackSize < stackSize);

			if (pStack->stackSize == stackSize && player->capabilities.isCreativeMode)
			{
				pStack->stackSize = 0;
				nResult = true;
			}
			else
			{
				nResult = pStack->stackSize < stackSize;
			}
		}

	} while (0);

	return nResult;
}

bool InventoryPlayer::canItemStackAddedToInventory(const ItemStackPtr pStack)
{
	if (!pStack || pStack->stackSize == 0)
	{
		return false;
	}

	if (player->capabilities.isCreativeMode)
	{
		return true;
	}

	if (pStack->isItemDamaged())
	{
		return getFirstEmptyStackInInventory() >= 0;
	}

	const int maxItemStackSize = (std::min)(pStack->getMaxStackSize(), getInventoryStackLimit());
	int decreaseSize = 0;
	for (int i = 0; i < MAIN_INVENTORY_COUNT; ++i)
	{
		if (mainInventory[i] == nullptr)
		{
			decreaseSize += maxItemStackSize;
		}
		else if (mainInventory[i] != nullptr &&
			mainInventory[i]->itemID == pStack->itemID &&
			mainInventory[i]->isStackable() &&
			mainInventory[i]->stackSize < mainInventory[i]->getMaxStackSize() &&
			mainInventory[i]->stackSize < getInventoryStackLimit() &&
			(!mainInventory[i]->getHasSubtypes() ||
				mainInventory[i]->getItemDamage() == pStack->getItemDamage()) &&
			ItemStack::areItemStackTagsEqual(mainInventory[i], pStack))
		{
			decreaseSize += maxItemStackSize - mainInventory[i]->stackSize;
		}

		if (decreaseSize >= pStack->stackSize)
		{
			return true;
		}
	}
	return false;
}

ItemStackPtr InventoryPlayer::decrStackSize(int slot, int count)
{
	auto* pItemArr = mainInventory;

	if (slot >= MAIN_INVENTORY_COUNT)
	{
		pItemArr = armorInventory;
		slot -= MAIN_INVENTORY_COUNT;
		if (slot >= ARMOR_INVENTORY_COUNT)
			return nullptr;
	}

	if (pItemArr[slot] != nullptr)
	{
		ItemStackPtr pStack;

		if (pItemArr[slot]->stackSize <= count)
		{
			pStack = pItemArr[slot];
			if (pItemArr == mainInventory)
			{
				for (int i = 0; i < HOTBAR_COUNT; ++i)
				{
					if (hotbar[i] == static_cast<ItemStackPtr>(pItemArr[slot]))
					{
						hotbar[i] = nullptr;
					}
				}
			}
			pItemArr[slot] = nullptr;
			return pStack;
		}
		else
		{
			pStack = pItemArr[slot]->splitStack(count);

			if (pItemArr[slot]->stackSize == 0)
			{
				if (pItemArr == mainInventory)
				{
					for (int i = 0; i < HOTBAR_COUNT; ++i)
					{
						if (hotbar[i] == static_cast<ItemStackPtr>(pItemArr[slot]))
						{
							hotbar[i] = nullptr;
						}
					}
				}
				pItemArr[slot] = nullptr;
			}

			return pStack;
		}
	}
	return nullptr;
}

ItemStackPtr InventoryPlayer::getStackInSlotOnClosing(int slot)
{
	auto* pItemArr = mainInventory;

	if (slot >= MAIN_INVENTORY_COUNT)
	{
		pItemArr = armorInventory;
		slot -= MAIN_INVENTORY_COUNT;
		if (slot >= ARMOR_INVENTORY_COUNT)
			return nullptr;
	}

	if (pItemArr[slot] != nullptr)
	{
		ItemStackPtr pStack = pItemArr[slot];
		pItemArr[slot] = nullptr;
		return pStack;
	}

	return nullptr;
}

void InventoryPlayer::setInventorySlotContents(int slot, ItemStackPtr pStack)
{
	if (slot >= 0 && slot < MAIN_INVENTORY_COUNT)
	{
		ItemStackPtr old_itemStack = mainInventory[slot];
		for (auto& itemStack : hotbar)
		{
			if (itemStack == old_itemStack)
			{
				itemStack = pStack;
				break;
			}
		}
		mainInventory[slot] = pStack;
	}
}

float InventoryPlayer::getStrVsBlock(Block* pBlock)
{
	float strength = 1.0F;

	if (hotbar[currentItemIndex] != nullptr)
	{
		strength *= hotbar[currentItemIndex]->getStrVsBlock(pBlock);
	}

	return strength;
}

NBTTagList* InventoryPlayer::writeToNBT(NBTTagList* pNBTList)
{
	NBTTagCompound* pCompound;

	for (int i = 0; i < MAIN_INVENTORY_COUNT; ++i)
	{
		if (mainInventory[i] == nullptr)
			continue;

		pCompound = LordNew NBTTagCompound();
		pCompound->setByte("Slot", (i8)i);
		mainInventory[i]->writeToNBT(pCompound);
		pNBTList->appendTag(pCompound);
	}

	for (int i = 0; i < ARMOR_INVENTORY_COUNT; ++i)
	{
		if (armorInventory[i] == nullptr)
			continue;
		
		pCompound = LordNew NBTTagCompound();
		pCompound->setByte("Slot", (i8)(i + 100));
		armorInventory[i]->writeToNBT(pCompound);
		pNBTList->appendTag(pCompound);
	}

	return pNBTList;
}

void InventoryPlayer::readFromNBT(NBTTagList* pNBTList)
{
	for (int i = 0; i < pNBTList->tagCount(); ++i)
	{
		NBTTagCompound* pCompound = (NBTTagCompound*)pNBTList->tagAt(i);
		int slot = pCompound->getByte("Slot") & 255;
		ItemStackPtr pStack = ItemStack::loadItemStackFromNBT(pCompound);

		if (pStack != nullptr)
		{
			if (slot >= 0 && slot < MAIN_INVENTORY_COUNT)
			{
				LordAssert(mainInventory[slot] == nullptr);
				mainInventory[slot] = pStack;
			}

			if (slot >= 100 && slot < ARMOR_INVENTORY_COUNT + 100)
			{
				LordAssert(armorInventory[slot - 100] == nullptr);
				armorInventory[slot - 100] = pStack;
			}
		}
	}
}

ItemStackPtr InventoryPlayer::getStackInSlot(int slot)
{
	auto* pItemArr = mainInventory;

	if (slot >= MAIN_INVENTORY_COUNT)
	{
		slot -= MAIN_INVENTORY_COUNT;
		pItemArr = armorInventory;

		if (slot >= ARMOR_INVENTORY_COUNT)
			return nullptr;
	}

	return pItemArr[slot];
}

ItemStackInfo InventoryPlayer::getItemStackInfo(int slot)
{
	auto stack = getStackInSlot(slot);
	if (stack)
	{
		return stack->getItemStackInfo();
	}
	static ItemStackInfo info = ItemStackInfo();
	return info;
}

bool InventoryPlayer::canHarvestBlock(Block* pBlock)
{
	if (pBlock->getMaterial().isToolNotRequired())
	{
		return true;
	}
	else
	{
		ItemStackPtr pStack = getCurrentItem();
		return pStack != nullptr ? pStack->canHarvestBlock(pBlock) : false;
	}
}

int InventoryPlayer::getTotalArmorValue()
{
	int nResult = 0;

	for (int i = 0; i < ARMOR_INVENTORY_COUNT; ++i)
	{
		if (armorInventory[i] == nullptr)
			continue;

		ItemArmor* pArmor = dynamic_cast<ItemArmor*>(armorInventory[i]->getItem());
		if (pArmor)
		{
			nResult += pArmor->damageReduceAmount;
		}
	}

	return nResult;
}

void InventoryPlayer::damageArmor(float damage)
{
	damage /= 4.0F;

	if (damage < 1.0F)
	{
		damage = 1.0F;
	}

	for (int i = 0; i < ARMOR_INVENTORY_COUNT; ++i)
	{
		if (armorInventory[i] == nullptr)
			continue;

		ItemArmor* pArmor = dynamic_cast<ItemArmor*>(armorInventory[i]->getItem());
		if (pArmor)
		{
			armorInventory[i]->damageItem((int)damage, player);

			if (armorInventory[i]->stackSize == 0)
			{
				m_armorDestroyed = true;
				armorInventory[i] = nullptr;
			}
		}
	}
}

bool InventoryPlayer::equipArmor(ItemStackPtr armorStack)
{

	if (!armorStack) {
		LordLogError("armorStack is nullptr, but is tried to be equiped");
		return false;
	}

	int inventoryIndex = getMainInventory()->findItemStack(armorStack);
	if (inventoryIndex < 0)
	{
		LordLogError("item does not belong to main inventory, but is tried to be equiped");
		return false;
	}
	auto armorItem = dynamic_cast<ItemArmor*>(Item::itemsList[armorStack->itemID]);
	if (!armorItem)
	{
		LordLogError("item %d is not an armor, but is tried to be equiped", armorStack->itemID);
		return false;
	}
	decrStackSize(inventoryIndex, 1);
	int armorIndex = armorItem->armorType;
	if (armorInventory[armorIndex] != nullptr)
	{
		addItemStackToInventory(armorInventory[armorIndex]);
	}
	armorInventory[armorIndex] = armorStack;

	return true;
}

bool InventoryPlayer::unequipArmor(ItemStackPtr armorStack)
{
	int armorIndex = getArmorInventory()->findItemStack(armorStack);
	if (armorIndex < 0)
	{
		LordLogError("item does not belong to armor inventory, but is tried to be unequiped");
		return false;
	}
	if (addItemStackToInventory(armorInventory[armorIndex]))
	{
		armorInventory[armorIndex] = nullptr;
		return true;
	}
	return false;
}

void InventoryPlayer::destroyArmor(int slotIndex)
{
	int armorIndex = slotIndex - MAIN_INVENTORY_COUNT;
	if (armorIndex < 0)
	{
		LordLogError("item does not belong to armor inventory, but is tried to be destroyed");
		return;
	}
	armorInventory[armorIndex] = nullptr;
}

void InventoryPlayer::dropAllItems()
{
	for (int i = 0; i < MAIN_INVENTORY_COUNT; ++i)
	{
		if (mainInventory[i] != nullptr)
		{
			player->dropPlayerItemWithRandomChoice(mainInventory[i], true);
			mainInventory[i] = nullptr;
		}
	}

	for (int i = 0; i < ARMOR_INVENTORY_COUNT; ++i)
	{
		if (armorInventory[i] != nullptr)
		{
			player->dropPlayerItemWithRandomChoice(armorInventory[i], true);
			armorInventory[i] = nullptr;
		}
	}

	for (int i = 0; i < HOTBAR_COUNT; ++i)
	{
		hotbar[i] = nullptr;
	}
}

bool InventoryPlayer::isUseableByPlayer(EntityPlayer* pPlayer)
{
	return player->isDead ? false : pPlayer->getDistanceSqToEntity(player) <= 64.0f;
}

bool InventoryPlayer::hasItemStack(ItemStackPtr pStack)
{
	for (int i = 0; i < ARMOR_INVENTORY_COUNT; ++i)
	{
		if (armorInventory[i] != nullptr && armorInventory[i]->isItemEqual(pStack))
		{
			return true;
		}
	}

	for (int i = 0; i < MAIN_INVENTORY_COUNT; ++i)
	{
		if (mainInventory[i] != nullptr && mainInventory[i]->isItemEqual(pStack))
		{
			return true;
		}
	}

	return false;
}

void InventoryPlayer::copyInventory(InventoryPlayer& inventory)
{
	for (int i = 0; i < MAIN_INVENTORY_COUNT; ++i)
	{
		mainInventory[i] = ItemStack::copyItemStack(inventory.mainInventory[i]);
	}

	for (int i = 0; i < ARMOR_INVENTORY_COUNT; ++i)
	{
		armorInventory[i] = ItemStack::copyItemStack(inventory.armorInventory[i]);
	}

	for (int i = 0; i < HOTBAR_COUNT; ++i)
	{
		for (int j = 0; j < MAIN_INVENTORY_COUNT; ++j)
		{
			if (inventory.hotbar[i] == static_cast<ItemStackPtr>(inventory.mainInventory[j]))
			{
				hotbar[i] = mainInventory[j];
				break;
			}
		}
	}

	currentItemIndex = inventory.currentItemIndex;
}

ItemStackPtr InventoryPlayer::getFirstSpareGun(int bulletItemId)
{
	for (int i = 0; i < MAIN_INVENTORY_COUNT; ++i)
	{
		if (mainInventory[i] != nullptr)
		{
			ItemGun* pGun = dynamic_cast<ItemGun*>(mainInventory[i]->getItem());
			const GunSetting* pSetting = mainInventory[i]->getGunSetting();
			if (pGun && pSetting && pSetting->adaptedBulletId == bulletItemId && pSetting->maxBulletNum > mainInventory[i]->getTotalBulletNum())
			{
				return mainInventory[i];
			}
		}
	}

	return nullptr;
}

ItemStackPtr InventoryPlayer::getKeyById(int keyId)
{
	for (int i = 0; i < MAIN_INVENTORY_COUNT; ++i)
	{
		if (mainInventory[i] != nullptr)
		{
			Item* pItem = mainInventory[i]->getItem();
			if(pItem && pItem->itemID == keyId)
				return mainInventory[i];
		}
	}

	return nullptr;
}

bool InventoryPlayer::notifyUpdateToProxy()
{
	if (m_mainInventoryProxy)
	{
		m_mainInventoryProxy->onUpdate();
	}
	if (m_armorInventoryProxy)
	{
		m_armorInventoryProxy->onUpdate();
	}
	if (m_armorFilteredInventoryProxy)
	{
		m_armorFilteredInventoryProxy->onUpdate();
	}
	if (m_inventoryAllCraftable)
	{
		m_inventoryAllCraftable->onUpdate();
	}
	if (m_inventoryHandCraftable)
	{
		m_inventoryHandCraftable->onUpdate();
	}
	return true;
}

IInventory* InventoryPlayer::getMainInventory()
{
	if (!m_mainInventoryProxy)
	{
		m_mainInventoryProxy = LordNew InventorySubset(this, 0, MAIN_INVENTORY_COUNT);
	}
	return m_mainInventoryProxy;
}

IInventory* InventoryPlayer::getArmorInventory()
{
	if (!m_armorInventoryProxy)
	{
		m_armorInventoryProxy = LordNew InventorySubset(this, MAIN_INVENTORY_COUNT, ARMOR_INVENTORY_COUNT);
	}
	return m_armorInventoryProxy;
}

IInventory* InventoryPlayer::getArmorFilteredInventory()
{
	if (!m_armorFilteredInventoryProxy)
	{
		auto mainInventory = getMainInventory();
		m_armorFilteredInventoryProxy = LordNew InventoryFiltered(mainInventory, 
			[mainInventory](int slot) -> bool 
		{
			auto itemStack = mainInventory->getStackInSlot(slot);
			return itemStack ? ItemArmor::isArmor(itemStack->itemID) : false;
		});
	}
	return m_armorFilteredInventoryProxy;
}

InventoryCraftable * InventoryPlayer::getInventoryAllCraftable()
{
	if (!m_inventoryAllCraftable)
	{
		m_inventoryAllCraftable = LordNew InventoryCraftable(getMainInventory());
	}
	return m_inventoryAllCraftable;
}

InventoryCraftable * InventoryPlayer::getInventoryHandCraftable()
{
	if (!m_inventoryHandCraftable)
	{
		m_inventoryHandCraftable = LordNew InventoryHandCraftable(getMainInventory());
	}
	return m_inventoryHandCraftable;
}

void InventoryPlayer::onUpdate()
{
	if (m_inventoryChanged)
	{
		m_inventoryChanged = false;
		notifyUpdateToProxy();
	}
}

void InventoryPlayer::updateInventoryCd(float fDeltaTime)
{
	for (int i = 0; i < MAIN_INVENTORY_COUNT; ++i)
	{
		if (mainInventory[i] != nullptr)
		{
			mainInventory[i]->updateCdTime(fDeltaTime);
		}
	}
}

bool InventoryPlayer::isCanAddItem(int itemId, int itemMeta, int addNum)
{
	if (itemId >= 10000)
	{
		return true;
	}

	Item* pItem = Item::getItemById(itemId);
	if (pItem && dynamic_cast<ItemAppIcon*>(pItem))
	{
		return true;
	}

	for (int i = 0; i < MAIN_INVENTORY_COUNT; ++i)
	{
		if (mainInventory[i] != nullptr &&
			mainInventory[i]->itemID == itemId &&
			mainInventory[i]->getItem()->getMetadata(mainInventory[i]->getItemDamage()) == itemMeta)
		{
			if (mainInventory[i]->getMaxStackSize() - mainInventory[i]->stackSize >=addNum)
			{
				return true;
			}
		}
	}

	return getFirstEmptyStackInInventory() != -1;
}

ItemStackPtr InventoryPlayer::getItemIdBySlot(int nSlot)
{
	int nItemId = -1;
	ItemStackPtr tmp = hotbar[nSlot];
	if (tmp != nullptr) {
		return tmp;
	}
	return nullptr;
}

bool InventoryPlayer::isContainItem(int itemId, int itemMeta)
{
	for (int i = 0; i < MAIN_INVENTORY_COUNT; ++i)
	{
		if (mainInventory[i] != nullptr &&
			mainInventory[i]->itemID == itemId &&
			mainInventory[i]->getItem()->getMetadata(mainInventory[i]->getItemDamage()) == itemMeta)
		{
			return true;
		}
	}
	return false;
}
bool InventoryPlayer::isEnchantment(int slot)
{
	if (slot >= 0 && slot < MAIN_INVENTORY_COUNT)
	{
		return mainInventory[slot]->isItemEnchanted();
	}

	return false;
}

int InventoryPlayer::getItemStackEnchantmentId(int slot, int enchantmentIndex)
{
	ItemStackPtr itemStack = getStackInSlot(slot);
	if (itemStack)
	{
		EnchantmentHelper::EnchantMentMap outmap;
		EnchantmentHelper::getEnchantments(itemStack, outmap);
		int index = 1;
		for (auto iter : outmap)
		{
			if (index == enchantmentIndex)
			{
				return iter.first;
			}
			index++;
		}
	}
	return 0;
}

int InventoryPlayer::getItemStackEnchantmentLv(int slot, int enchantmentIndex)
{
	ItemStackPtr itemStack = getStackInSlot(slot);
	if (itemStack)
	{
		EnchantmentHelper::EnchantMentMap outmap;
		EnchantmentHelper::getEnchantments(itemStack, outmap);
		int index = 1;
		for (auto iter : outmap)
		{
			if (index == enchantmentIndex)
			{
				return iter.second;
			}
			index++;
		}
	}
	return 0;
}

}
