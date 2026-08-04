#include "Shop.h"

namespace BLOCKMAN
{

	Shop::Shop()
	{
	}

	Shop::~Shop()
	{
	}

	void Shop::onUpdate()
	{
		
	}

	void Shop::addGoodsGroup(i32 category, String icon, String name)
	{
		for (auto & group : m_goods)
		{
			if (group.getCategory() == category)
			{
				group.setIcon(icon);
				group.setName(name);
				return;
			}
		}
		GoodsGroup newGroup(m_goods.size(), category, icon, name);
		m_goods.push_back(newGroup);
	}

	void Shop::addGoods(i32 category, const Goods& goods)
	{
		m_isChange = true;
		for (auto & group : m_goods)
		{
			if (group.getCategory() == category)
			{
				group.addGoods(goods);
				return;
			}
		}
		GoodsGroup newGroup(m_goods.size(), category, "", "");
		newGroup.addGoods(goods);
		m_goods.push_back(std::move(newGroup));
	}

	void Shop::resetGoods(const vector<GoodsGroup>::type & goods)
	{
		m_goods = goods;
		m_isChange = true;
	}

	void Shop::updateGoods(i32 groupIndex, i32 goodsIndex, i32 limit)
	{
		auto goods = m_goods[groupIndex][goodsIndex];
		goods.limit = limit;
		m_goods[groupIndex][goodsIndex] = goods;
	}

	void Shop::clearGoods()
	{
		m_goods.clear();
		m_isChange = true;
	}

	void Shop::addRespawnGoods(RespawnGoods & goods)
	{
		m_isChange = true;
		goods.index = m_respawnGoods.size();
		m_respawnGoods.push_back(std::move(goods));
	}

	void Shop::resetRespawnGoods(const vector<RespawnGoods>::type & goods)
	{
		m_respawnGoods = goods;
		m_isChange = true;
	}

	RespawnGoods Shop::getRespawnGoodsByTime(i32 times)
	{
		if (m_respawnGoods.empty())
		{
			return RespawnGoods();
		}
		if (times < 0)
		{
			times = 0;
		}
		if (times >= (int)m_respawnGoods.size())
		{
			times = m_respawnGoods.size() - 1;
		}
		return m_respawnGoods[times];
	}

	i32 Shop::buyGoods(i32 groupIndex, i32 goodsIndex) {
		Goods goods = m_goods[groupIndex][goodsIndex];
		if (goods.limit == 0)
		{
			return -1;
		}
		goods.buy();
		if (goods.limit != -1)
		{
			m_goods[groupIndex][goodsIndex] = goods;
			return goods.limit;
		}
		return -1;
	}

	i32 Shop::resumeBuyGoods(i32 groupIndex, i32 goodsIndex)
	{
		Goods goods = m_goods[groupIndex][goodsIndex];
		if (goods.limit != -1)
		{
			goods.limit = goods.limit + 1;
			m_goods[groupIndex][goodsIndex] = goods;
			return goods.limit;
		}
		return -1;
	}

	void Shop::addRespawnRecord(i64 rakssId, i32 time)
	{
		BuyRespawnRecord record = BuyRespawnRecord();
		record.rakssId = rakssId;
		record.time = time * 1000 + 500;
		m_respawnRecords.push_back(record);
	}

	void Shop::removeRespawnRecord(i64 rakssId)
	{
		for (auto it = m_respawnRecords.begin(); it != m_respawnRecords.end(); ++it)
		{
			if (it->rakssId == rakssId)
			{
				m_respawnRecords.erase(it);
				break;
			}
		}
	}

	i64 Shop::checkRespawnRecord(int delta)
	{
		for (auto record : m_respawnRecords)
		{
			record.time -= delta;
		}
		for (auto it = m_respawnRecords.begin(); it != m_respawnRecords.end(); ++it)
		{
			if (it->time <= 0)
			{
				i64 rakssId = it->rakssId;
				m_respawnRecords.erase(it);
				return rakssId;
			}
		}
		return -1;
	}

}