#ifndef __SERVER_SHOP_HEADER__
#define __SERVER_SHOP_HEADER__

#include "World/Shop.h"

namespace BLOCKMAN
{
	class ServerShop : public Shop
	{
	public:
		ServerShop();
		~ServerShop();
		void onUpdate();
		void addGoods(i32 category, const Goods & goods);
		void addRespawnGoods(RespawnGoods & goods);
		i32 buyGoods(i32 groupIndex, i32 goodsIndex);
		i32 resumeBuyGoods(i32 groupIndex, i32 goodsIndex);
		i64 checkRespawnRecord(int delta);
	private:
		int goodsCount = 0;
	};
}

#endif // !__SERVER_SHOP_HEADER__
