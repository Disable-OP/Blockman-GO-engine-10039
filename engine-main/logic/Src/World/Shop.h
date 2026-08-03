#ifndef __SHOP_HEADER__
#define __SHOP_HEADER__

#include "Network/protocol/PacketDefine.h"
#include "BM_TypeDef.h"

namespace BLOCKMAN
{
	class Goods
	{
	public:
		i32 uniqueId; 
		i32 index;
		i32 itemId = 0;
		i32 itemMeta = 0;
		i32 itemNum = 0;
		i32 coinId = 0;
		i32 blockymodsPrice = 0;
		i32 blockmanPrice = 0;
		i32 limit = -1;
		String icon = "";
		String desc = "";
	public:
		bool canBuy() { return limit == -1 || limit > 0; }
		void buy() { limit = limit == -1 ? limit : limit - 1 >= 0 ? limit - 1 : 0; }
		DEFINE_NETWORK_SERIALIZER(uniqueId, index, itemId, itemMeta, itemNum, coinId, blockymodsPrice, blockmanPrice, limit, icon, desc);
	};

	class GoodsGroup
	{
	private:
		i32 m_index;
		i32 m_category;
		String m_icon;
		String m_name;
		vector<Goods>::type m_goods;

	public:
		GoodsGroup() = default;
		GoodsGroup(const GoodsGroup&) = default;
		GoodsGroup(GoodsGroup&&) = default;
		GoodsGroup& operator=(const GoodsGroup&) = default;
		GoodsGroup& operator=(GoodsGroup&&) = default;

		GoodsGroup(int index, i32 category, String icon, String name)
			: m_index(index)
			, m_category(category)
			, m_icon(icon)
			, m_name(name)
		{}

		i32 getCategory() const
		{
			return m_category;
		}

		String getIcon() const
		{
			return m_icon;
		}

		String getName() const
		{
			return m_name;
		}

		void setName(String name)
		{
			m_name = name;
		}

		void setIcon(String icon)
		{
			m_icon = icon;
		}

		vector<Goods>::type getGoods()
		{
			return m_goods;
		}

		Goods& operator[](size_t index)
		{
			return m_goods[index];
		}

		const Goods& operator[](size_t index) const
		{
			return m_goods[index];
		}

		size_t size() const
		{
			return m_goods.size();
		}

		i32 getIndex() {
			return m_index;
		}

		void addGoods(Goods goods)
		{
			goods.index = m_goods.size();
			m_goods.push_back(std::move(goods));
		}

		DEFINE_NETWORK_SERIALIZER(m_index, m_category, m_icon, m_name, m_goods);
	};

	class RespawnGoods
	{
	public:
		i32 uniqueId = -1;
		i32 index;
		i32 time;
		i32 coinId;
		i32 blockymodsPrice;
		i32 blockmanPrice;
		i32 times;
		DEFINE_NETWORK_SERIALIZER(uniqueId, index, time, coinId, blockymodsPrice, blockmanPrice, times);
	};

	class BuyRespawnRecord
	{
	public:
		i64 rakssId;
		i32 time;
	};

	class Shop : public ObjectAlloc
	{
	private:
		vector<GoodsGroup>::type m_goods;
		vector<RespawnGoods>::type m_respawnGoods;
		vector<BuyRespawnRecord>::type m_respawnRecords;
	public:
		bool m_isChange = false;
	public:
		Shop();
		~Shop();
		vector<GoodsGroup>::type getGoods() { return m_goods; }
		virtual void onUpdate();
		virtual void addGoodsGroup(i32 category, String icon, String name);
		virtual void addGoods(i32 category, const Goods & goods);
		virtual void resetGoods(const vector<GoodsGroup>::type& goods);
		virtual void updateGoods(i32 groupIndex, i32 goodsIndex, i32 limit);
		virtual void clearGoods();
		virtual void addRespawnGoods(RespawnGoods & goods);
		virtual void resetRespawnGoods(const vector<RespawnGoods>::type& goods);
		RespawnGoods getRespawnGoodsByTime(i32 times);
		vector<RespawnGoods>::type getRespawnGoods() { return m_respawnGoods; }
		virtual i32 buyGoods(i32 groupIndex, i32 goodsIndex);
		virtual i32 resumeBuyGoods(i32 groupIndex, i32 goodsIndex);

		virtual vector<BuyRespawnRecord>::type getRespawnRecords() { return m_respawnRecords; }
		virtual void addRespawnRecord(i64 rakssId, i32 time);
		virtual void removeRespawnRecord(i64 rakssId);
		virtual i64 checkRespawnRecord(int delta);

	};

}

#endif // !__SHOP_HEADER__

