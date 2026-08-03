#ifndef __INGREDIENT_HEADER__
#define __INGREDIENT_HEADER__

#include "Std/type_traits.h"
#include "Core.h"

using namespace LORD;

namespace BLOCKMAN
{
	class ItemStack;

	class Ingredient
	{
	private:
		using ItemStackPtr = std::shared_ptr<ItemStack>;
		int m_itemId = 0;
		vector<int>::type m_itemDataList;

	public:
		Ingredient(int itemId, int itemData = 0);
		template<typename ItemDataList, typename = std::enable_if<!std::is_integral<ItemDataList>::value>>
		Ingredient(int itemId, const ItemDataList& itemDataList)
			: m_itemId(itemId)
			, m_itemDataList(itemDataList.begin(), itemDataList.end())
		{
		}
		bool matches(ItemStackPtr itemStack);
		int getItemId()
		{
			return m_itemId;
		}
		vector<int>::type& getCandidateItemDataList()
		{
			return m_itemDataList;
		}
	};
}

#endif // !__INGREDIENT_HEADER__
