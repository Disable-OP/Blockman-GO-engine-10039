#include "AboutShopping.h"

namespace BLOCKMAN
{
	void CommodityList::addCommodity(CommodityCategory category, const Commodity & commodity)
	{
		Commodity newCommodity = commodity;
		commodityCount++;
		newCommodity.uniqueId = commodityCount;
		for (auto & group : m_commodities)
		{
			if (group.getCategory() == category)
			{
				group.addCommodity(newCommodity);
				return;
			}
		}
		CommodityGroup newGroup(m_commodities.size(), category);
		newGroup.addCommodity(newCommodity);
		m_commodities.push_back(std::move(newGroup));
	}
}