#pragma once
#ifndef __COMMODITY_MANAGER_HEADER__
#define __COMMODITY_MANAGER_HEADER__

#include "BM_TypeDef.h"
#include "Network/protocol/PacketDefine.h"
#include "Util/AboutShopping.h"

namespace BLOCKMAN
{
	class CommodityManager : public Singleton<CommodityManager>, public ObjectAlloc
	{
	public:
		CommodityManager();
		~CommodityManager();
		void addCommodity(int index, CommodityCategory category, const Commodity & commodity);
		void clearCommoditys(int index);
		void addMapping(int entityId, ui64 rakssId, int index);
		CommodityMapping getMapping(int entityId, ui64 rakssId);
		CommodityList* getCommodityList(int index);

	private:
		LORD::map<int, CommodityList*>::type m_commodities_map;
		vector<CommodityMapping>::type m_mappings;
	};

}

#endif // !__COMMODITY_MANAGER_HEADER__