#include "ServerShop.h"
#include "Server.h"
#include "Network/ServerPacketSender.h"
#include "Network/ServerNetwork.h"
#include "Script/GameServerEvents.h"

namespace BLOCKMAN
{

	ServerShop::ServerShop()
	{
	}

	ServerShop::~ServerShop()
	{
	}

	void ServerShop::onUpdate()
	{
		if (m_isChange)
		{
			m_isChange = false;
			Server::Instance()->getNetwork()->getSender()->broadCastUpdateShop();
		}
		World* world = Server::Instance()->getWorld();
		int delta = world->getFrameDeltaTime() > 0.05f ? (int)(world->getFrameDeltaTime() * 1000) : 50;
		checkRespawnRecord(delta);
	}

	void ServerShop::addGoods(i32 category, const Goods & goods)
	{
		Goods newGoods = goods;
		goodsCount++;
		newGoods.uniqueId = goodsCount;
		Shop::addGoods(category, newGoods);
	}

	void ServerShop::addRespawnGoods(RespawnGoods & goods)
	{
		RespawnGoods newGoods = goods;
		goodsCount++;
		newGoods.uniqueId = goodsCount;
		Shop::addRespawnGoods(newGoods);
	}

	i32 ServerShop::buyGoods(i32 groupIndex, i32 goodsIndex)
	{
		i32 limit = Shop::buyGoods(groupIndex, goodsIndex);
		if (limit != -1)
		{
			Server::Instance()->getNetwork()->getSender()->broadCastUpdateShopGoods(groupIndex, goodsIndex, limit);
		}
		return limit;
	}

	i32 ServerShop::resumeBuyGoods(i32 groupIndex, i32 goodsIndex)
	{
		i32 limit = Shop::resumeBuyGoods(groupIndex, goodsIndex);
		if (limit != -1)
		{
			Server::Instance()->getNetwork()->getSender()->broadCastUpdateShopGoods(groupIndex, goodsIndex, limit);
		}
		return limit;
	}

	i64 ServerShop::checkRespawnRecord(int delta)
	{
		i64 rakssId = Shop::checkRespawnRecord(delta);
		if (rakssId != -1)
		{
			SCRIPT_EVENT::PlayerBuyRespawnResultEvent::invoke(rakssId, 0);
		}
		return rakssId;
	}

}