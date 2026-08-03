#include "Triggers.h"
#include "Entity/EntityPlayer.h"
#include "Inventory/InventoryPlayer.h"

namespace BLOCKMAN
{
	using namespace LORD;

	TriggerActivatorList TriggerClick::checkCondition(EntityPlayer* player)
	{
		auto itemInHand = player->inventory->getCurrentItem();
		if (m_noRestriction)
		{
			return { player };
		}
		int itemId = itemInHand ? itemInHand->itemID : NO_ITEM;
		if (m_itemIds.find(itemId) != m_itemIds.end())
		{
			return { player };
		}
		return {};
	}

	TriggerActivatorList TriggerInRange::checkCondition(Data & data, World * world, const BlockPos & pos)
	{
		// TODO:
		return TriggerActivatorList();
	}

	TriggerActivatorList TriggerTimeInterval::checkCondition(Data & data)
	{
		// TODO:
		return TriggerActivatorList();
	}

	TriggerActivatorList TriggerBlockAdjacent::checkCondition(World *, const BlockPos & pos)
	{
		// TODO:
		return TriggerActivatorList();
	}
}