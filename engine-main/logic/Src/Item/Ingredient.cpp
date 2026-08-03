#include "Ingredient.h"
#include "Item/ItemStack.h"

namespace BLOCKMAN
{
	Ingredient::Ingredient(int itemId, int itemData)
		: m_itemId(itemId)
	{
		m_itemDataList.push_back(itemData);
	}

	bool Ingredient::matches(ItemStackPtr itemStack)
	{
		if (!itemStack)
		{
			return false;
		}
		for (const auto& itemData : m_itemDataList)
		{
			if (m_itemId == itemStack->itemID)
			{
				if (itemData == 32767 || itemStack->itemDamage == itemData)
				{
					return true;
				}
			}
		}

		return false;
	}
}