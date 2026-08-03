#include "Wallet.h"
#include "Inventory/InventoryPlayer.h"
#include "Std/iterator.h"
#include "Inventory/CoinManager.h"

namespace BLOCKMAN
{
	Wallet::Wallet(InventoryPlayer* inventory)
		: m_inventory(inventory)
	{
		m_subscriptionGuard.add(inventory->onInventoryContentChanged([this] { m_inventoryChanged = true; }));
	}

	void Wallet::consumeCoin(int coinId, int num)
	{
		if (coinId < 0 || coinId >= (int)std::size(m_coins))
		{
			LordLogError("Wallet::consumeCoin coinId %d out of range!", coinId);
			return;
		}

		int consumeCoin = num;
		int itemId = CoinManager::Instance()->coinIdToItemId(coinId);
		if (itemId == 0)
		{
			LordLogError("Wallet::consumeCoin coinIdToItemId error! coinId %d, itemId %d", coinId, itemId);
			return;
		}

		auto mainInventory = m_inventory->getMainInventory();
		for (int i = 0; i < mainInventory->getSizeInventory() && consumeCoin > 0; ++i)
		{
			auto itemStack = mainInventory->getStackInSlot(i);
			if (itemStack && itemStack->itemID == itemId)
			{
				int decreaseSize = (std::min)(static_cast<int>(itemStack->stackSize), consumeCoin);
				mainInventory->decrStackSize(i, decreaseSize);
				consumeCoin -= decreaseSize;
			}
		}
		m_coins[coinId] -= num;
	}

	void Wallet::onUpdate()
	{
		if (!m_inventoryChanged)
		{
			return;
		}
		m_inventoryChanged = false;

		for (size_t coinId = 0; coinId < std::size(m_coins); ++coinId)
		{
			m_coins[coinId] = 0;
		}

		auto mainInventory = m_inventory->getMainInventory();
		for (int i = 0; i < mainInventory->getSizeInventory(); ++i)
		{
			auto itemStack = mainInventory->getStackInSlot(i);
			if (!itemStack)
			{
				continue;
			}
			int coinId = CoinManager::Instance()->itemIdToCoinId(itemStack->itemID);
			if (coinId <= 0)
			{
				continue;
			}
			m_coins[coinId] += itemStack->stackSize;
		}
	}
}