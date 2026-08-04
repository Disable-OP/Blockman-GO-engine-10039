#include "RedDotManager.h"
#include "Util/ClientEvents.h"
#include <sstream>
#include "rapidjson/writer.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/document.h"
#include "Util/UICommon.h"
#include "Util/FriendManager.h"
#include "Util/MailManager.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "World/Ranch.h"
#include "World/BirdSimulator.h"

using namespace rapidjson;

namespace BLOCKMAN
{

	void RedDotManager::init()
	{

	}

	void RedDotManager::release()
	{
	}

	bool RedDotManager::checkRanchMain()
	{
		return checkHouse() || checkStorage() || checkFactory() || checkFarming() || checkCommunity() || checkAchievement();
	}

	bool RedDotManager::checkOrderMain()
	{
		if (!UICommon::checkRanchParam() || !Blockman::Instance()->m_pPlayer->m_ranch->getStorage())
		{
			return false;
		}
		const auto& orders = Blockman::Instance()->m_pPlayer->m_ranch->getOrders();
		const auto storage = Blockman::Instance()->m_pPlayer->m_ranch->getStorage();
		for (const auto& order : orders)
		{
			if (order.status == 4)
			{
				return true;
			}

			for (const auto& item : order.infos)
			{
				if (!item.isDone &&  Blockman::Instance()->m_pPlayer->m_ranch->getStorage()->checkInventory(item.itemId, item.needNum))
				{
					return true;
				}
			}
		}
		return false;
	}

	bool RedDotManager::checkOrder(i32 orderId)
	{
		if (!UICommon::checkRanchParam() || !Blockman::Instance()->m_pPlayer->m_ranch->getStorage())
		{
			return false;
		}
		const auto& orders = Blockman::Instance()->m_pPlayer->m_ranch->getOrders();
		const auto storage = Blockman::Instance()->m_pPlayer->m_ranch->getStorage();
		for (const auto& order : orders)
		{
			if (order.id == orderId)
			{
				if (order.status == 4)
				{
					return true;
				}

				for (const auto& item : order.infos)
				{
					if (!item.isDone &&  Blockman::Instance()->m_pPlayer->m_ranch->getStorage()->checkInventory(item.itemId, item.needNum))
					{
						return true;
					}
				}
			}
		}
		return false;
	}

	bool RedDotManager::checkOrderItem(i32 orderId, i32 orderIndex)
	{
		if (!UICommon::checkRanchParam() || !Blockman::Instance()->m_pPlayer->m_ranch->getStorage())
		{
			return false;
		}
		const auto& orders = Blockman::Instance()->m_pPlayer->m_ranch->getOrders();
		const auto storage = Blockman::Instance()->m_pPlayer->m_ranch->getStorage();
		for (const auto& order : orders)
		{
			if (order.id == orderId)
			{
				if (order.status ==4)
				{
					return true;
				}

				for (const auto& item : order.infos)
				{
					if (!item.isDone && item.index == orderIndex  && Blockman::Instance()->m_pPlayer->m_ranch->getStorage()->checkInventory(item.itemId, item.needNum))
					{
						return true;
					}
				}
			}
		}
		return false;
	}

	bool RedDotManager::checkHouse()
	{
		if (!UICommon::checkRanchParam() || !UICommon::checkWalletParam() ||
			!Blockman::Instance()->m_pPlayer->m_ranch->getInfo() ||
			!Blockman::Instance()->m_pPlayer->m_ranch->getHouse() ||
			!Blockman::Instance()->m_pPlayer->m_ranch->getStorage())
		{
			return false;
		}
		RanchInfo* info = Blockman::Instance()->m_pPlayer->m_ranch->getInfo();
		RanchHouse* house = Blockman::Instance()->m_pPlayer->m_ranch->getHouse();
		RanchStorage* storage = Blockman::Instance()->m_pPlayer->m_ranch->getStorage();
		if (house->level < house->maxLevel && info->level >= house->needPlayerLevel)
		{
			for (const auto& item : house->upgradeRecipe)
			{
				if (!storage->checkInventory(item.itemId, item.num))
				{
					return false;
				}
			}
			return Blockman::Instance()->m_pPlayer->m_wallet->getMoneyByCoinId(3) >= house->upgradePrice;;
		}
		return false;
	}

	bool RedDotManager::checkStorage()
	{
		if (!UICommon::checkRanchParam() || !UICommon::checkWalletParam() || !Blockman::Instance()->m_pPlayer->m_ranch->getStorage())
		{
			return false;
		}

		RanchStorage* storage = Blockman::Instance()->m_pPlayer->m_ranch->getStorage();

		for (const auto& item : storage->upgradeRecipe)
		{
			if (!storage->checkInventory(item.itemId, item.num))
			{
				return false;
			}
		}
		return Blockman::Instance()->m_pPlayer->m_wallet->getMoneyByCoinId(3) >= storage->upgradePrice;
	}

	bool RedDotManager::checkFactory()
	{
		if (!UICommon::checkRanchParam() || !UICommon::checkWalletParam() || !Blockman::Instance()->m_pPlayer->m_ranch->getInfo())
		{
			return false;
		}
		RanchInfo* info = Blockman::Instance()->m_pPlayer->m_ranch->getInfo();
		const auto& items = Blockman::Instance()->m_pPlayer->m_ranch->getFactoryBuild();
		for (const auto& item : items)
		{
			if (item.num == 0 && (info->level >= item.needLevel) && Blockman::Instance()->m_pPlayer->m_wallet->getMoneyByCoinId(3) >= item.price)
			{
				return true;
			}

		}
		return false;
	}

	bool RedDotManager::checkFactoryById(i32 buildId)
	{
		if (!UICommon::checkRanchParam() || !UICommon::checkWalletParam() || !Blockman::Instance()->m_pPlayer->m_ranch->getInfo())
		{
			return false;
		}
		RanchInfo* info = Blockman::Instance()->m_pPlayer->m_ranch->getInfo();
		const auto& items = Blockman::Instance()->m_pPlayer->m_ranch->getFactoryBuild();
		for (const auto& item : items)
		{
			if (buildId == item.id && item.num == 0 && info->level >= item.needLevel && Blockman::Instance()->m_pPlayer->m_wallet->getMoneyByCoinId(3) >= item.price)
			{
				return true;
			}
		}
		return false;
	}

	bool RedDotManager::checkFarming()
	{
		return checkRaise() || checkPlantById(500000);
	}

	bool RedDotManager::checkRaise()
	{
		if (!UICommon::checkRanchParam() || !UICommon::checkWalletParam() || !Blockman::Instance()->m_pPlayer->m_ranch->getInfo())
		{
			return false;
		}
		RanchInfo* info = Blockman::Instance()->m_pPlayer->m_ranch->getInfo();
		const auto& items = Blockman::Instance()->m_pPlayer->m_ranch->getFarmingBuild();
		for (const auto& item : items)
		{
			if (item.num == 0 && info->level >= item.needLevel && Blockman::Instance()->m_pPlayer->m_wallet->getMoneyByCoinId(3) >= item.price)
			{
				return true;
			}

		}
		return false;
	}

	bool RedDotManager::checkRaiseById(i32 buildId)
	{
		if (!UICommon::checkRanchParam() || !UICommon::checkWalletParam() || !Blockman::Instance()->m_pPlayer->m_ranch->getInfo())
		{
			return false;
		}
		RanchInfo* info = Blockman::Instance()->m_pPlayer->m_ranch->getInfo();
		const auto& items = Blockman::Instance()->m_pPlayer->m_ranch->getFarmingBuild();
		for (const auto& item : items)
		{
			if (item.id == buildId && item.num == 0 && info->level >= item.needLevel && Blockman::Instance()->m_pPlayer->m_wallet->getMoneyByCoinId(3) >= item.price)
			{
				return true;
			}

		}
		return false;
	}

	bool RedDotManager::checkPlant()
	{
		if (!UICommon::checkRanchParam() || !UICommon::checkWalletParam() || !Blockman::Instance()->m_pPlayer->m_ranch->getInfo())
		{
			return false;
		}
		RanchInfo* info = Blockman::Instance()->m_pPlayer->m_ranch->getInfo();
		const auto& items = Blockman::Instance()->m_pPlayer->m_ranch->getPlantBuild();
		for (const auto& item : items)
		{
			if (item.num < item.maxNum && info->level >= item.needLevel && Blockman::Instance()->m_pPlayer->m_wallet->getMoneyByCoinId(3) >= item.price)
			{
				return true;
			}

		}
		return false;
	}

	bool RedDotManager::checkPlantById(i32 buildId)
	{
		if (!UICommon::checkRanchParam() || !UICommon::checkWalletParam() || !Blockman::Instance()->m_pPlayer->m_ranch->getInfo())
		{
			return false;
		}
		RanchInfo* info = Blockman::Instance()->m_pPlayer->m_ranch->getInfo();
		const auto& items = Blockman::Instance()->m_pPlayer->m_ranch->getPlantBuild();
		for (const auto& item : items)
		{
			if (item.id == buildId && item.num < item.maxNum && info->level >= item.needLevel && Blockman::Instance()->m_pPlayer->m_wallet->getMoneyByCoinId(3) >= item.price)
			{
				return true;
			}

		}
		return false;
	}

	bool RedDotManager::checkCommunity()
	{
		return checkFriend() || checkClan() || checkMail();
	}


	bool RedDotManager::checkFriend()
	{
		const auto& friends = FriendManager::getFriends();
		for (const auto & pal : friends)
		{
			if (pal->needHelp)
			{
				return true;
			}
		}
		return false;
	}

	bool RedDotManager::checkClan()
	{
		const auto& friends = FriendManager::getMembers();
		for (const auto & pal : friends)
		{
			if (pal->needHelp)
			{
				return true;
			}
		}
		return false;
	}


	bool RedDotManager::checkMail()
	{
		return MailManager::getMails().size() > 0;
	}

	bool RedDotManager::checkAchievement()
	{
		if (!UICommon::checkRanchParam())
		{
			return false;
		}
		const auto& items = Blockman::Instance()->m_pPlayer->m_ranch->getAchievements();
		for (const auto& item : items)
		{
			if (item.level < item.maxLevel && item.exp >= item.nextExp)
			{
				return true;
			}
		}
		return false;
	}

	bool RedDotManager::checkAchievementById(i64 id)
	{
		if (!UICommon::checkRanchParam())
		{
			return false;
		}
		const auto& items = Blockman::Instance()->m_pPlayer->m_ranch->getAchievements();

		for (const auto& item : items)
		{
			if (item.id == id && item.level < item.maxLevel && item.exp >= item.nextExp)
			{
				return true;
			}
		}
		return false;
	}

	bool RedDotManager::checkBirdTask()
	{
		if (!UICommon::checkBirdParam())
		{
			return false;
		}
		auto tasks = Blockman::Instance()->m_pPlayer->m_birdSimulator->getTasks();
		for (auto &task : tasks)
		{
			bool isDone = true;
			for (auto& item : task.contents)
			{
				if (item.curValue < item.needValue)
				{
					isDone = false;
					break;
				}
			}
			if (isDone)
				return true;
		}
		return false;
	}

}