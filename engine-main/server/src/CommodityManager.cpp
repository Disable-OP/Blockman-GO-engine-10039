#include "CommodityManager.h"
#include "Server.h"
#include "Network/ServerPacketSender.h"
#include "Network/ServerNetwork.h"

namespace BLOCKMAN
{
	
	CommodityManager::CommodityManager()
	{

	}

	CommodityManager::~CommodityManager()
	{
		for (LORD::map<int, CommodityList*>::type::iterator it = m_commodities_map.begin();
			it != m_commodities_map.end(); ++it)
		{
			LordDelete it->second;
		}
		m_commodities_map.clear();
		m_mappings.clear();
	}

	void CommodityManager::addCommodity(int index, CommodityCategory category, const Commodity & commodity)
	{
		CommodityList* commodityList = getCommodityList(index);
		if (!commodityList)
		{
			commodityList = LordNew CommodityList;
		}
		commodityList->addCommodity(category, commodity);
		if (commodityList->isNew)
		{
			commodityList->isNew = false;
			m_commodities_map[index] = commodityList;
		}
	}

	CommodityList* CommodityManager::getCommodityList(int index)
	{
		for (LORD::map<int, CommodityList*>::type::iterator it = m_commodities_map.begin();
			it != m_commodities_map.end(); ++it)
		{
			if (it->first == index)
			{
				return it->second;
			}
		}
		return nullptr;
	}

	void CommodityManager::clearCommoditys(int index)
	{
		auto cList = getCommodityList(index);
		if (cList)
			getCommodityList(index)->clearCommoditys();
	}

	void CommodityManager::addMapping(int entityId, ui64 rakssId, int index)
	{
		for (auto & mapping : m_mappings) 
		{
			if (mapping.entityId == entityId && mapping.rakssId == rakssId)
			{
				mapping.index = index;
				Server::Instance()->getNetwork()->getSender()->sendUpdateMerchant(rakssId, entityId, index);
				return;
			}
		}
		m_mappings.push_back(CommodityMapping(entityId, rakssId, index));
		Server::Instance()->getNetwork()->getSender()->sendUpdateMerchant(rakssId, entityId, index);
	}

	CommodityMapping CommodityManager::getMapping(int entityId, ui64 rakssId)
	{
		for (auto & mapping : m_mappings)
		{
			if (mapping.entityId == entityId && mapping.rakssId == rakssId)
			{
				return mapping;
			}
		}
		return CommodityMapping(entityId, rakssId, 0);
	}
}

