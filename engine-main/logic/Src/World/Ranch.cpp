#include "Ranch.h"
#include "Common.h"
#include "Entity/EntityPlayer.h"
#include "Network/CommonPacketSender.h"
#include "World.h"

namespace BLOCKMAN
{

	i32 RanchStorage::getTotalNum()
	{
		int curNum = 0;
		for (auto item : items)
		{
			curNum += item.num;
		}
		return curNum;
	}

	bool RanchStorage::canSave(i32 num)
	{
		return getTotalNum() + num <= capacity;
	}

	void RanchStorage::setItemNum(i32 itemId, i32 num)
	{
		for (auto item : items)
		{
			if (item.itemId == itemId)
			{
				item.num = num;
				break;
			}
		}
	}

	i32 RanchStorage::getItemNum(i32 itemId)
	{
		for (auto item : items)
		{
			if (item.itemId == itemId)
			{
				return item.num;
			}
		}
		return 0;
	}

	bool RanchStorage::checkInventory(i32 itemId, i32 num)
	{
		for (auto item : items)
		{
			if (item.itemId == itemId)
			{
				return item.num >= num;
			}
		}
		return false;
	}


	Ranch::Ranch()
	{

	}

	Ranch::~Ranch()
	{
		dealloc();
	}

	void Ranch::onUpdate(EntityPlayer* player)
	{
		if (m_infoIsChange && m_info)
		{
			GameCommon::Instance()->getCommonPacketSender()->syncRanch(player, (int) SyncRanchType::INFO);
			m_infoIsChange = false;
		}

		if (m_houseIsChange && m_house)
		{
			GameCommon::Instance()->getCommonPacketSender()->syncRanch(player, (int)SyncRanchType::HOUSE);
			m_houseIsChange = false;
		}

		if (m_storageIsChange && m_storage)
		{
			GameCommon::Instance()->getCommonPacketSender()->syncRanch(player, (int)SyncRanchType::STORAGE);
			m_storageIsChange = false;
		}

		if (m_ordersChange)
		{
			GameCommon::Instance()->getCommonPacketSender()->syncRanch(player, (int)SyncRanchType::ORDER);
			m_ordersChange = false;
		}


		if (m_achievementsChange)
		{
			GameCommon::Instance()->getCommonPacketSender()->syncRanch(player, (int)SyncRanchType::ACHIEVEMENT);
			m_achievementsChange = false;
		}

		if (m_timePricesChange)
		{
			GameCommon::Instance()->getCommonPacketSender()->syncRanch(player, (int)SyncRanchType::TIME_PRICE);
			m_timePricesChange = false;
		}
		
		if (m_shortcutFreeTimesChange)
		{
			GameCommon::Instance()->getCommonPacketSender()->syncRanch(player, (int)SyncRanchType::SHORTCUT_FREE_TIMES);
			m_shortcutFreeTimesChange = false;
		}

		if (m_clanRankChange)
		{
			GameCommon::Instance()->getCommonPacketSender()->syncRanchRank(player, (int)RanchRankType::CLAN);
			m_clanRankChange = false;
		}

		if (m_prosperityRankChange)
		{
			GameCommon::Instance()->getCommonPacketSender()->syncRanchRank(player, (int)RanchRankType::PROSPERITY);
			m_prosperityRankChange = false;
		}

		if (m_achievementRankChange)
		{
			GameCommon::Instance()->getCommonPacketSender()->syncRanchRank(player, (int)RanchRankType::ACHIEVEMENT);
			m_achievementRankChange = false;
		}


		if (m_factoryBuildChange)
		{
			GameCommon::Instance()->getCommonPacketSender()->syncRanchBuild(player, (int)RanchBuildType::FACTORY);
			m_factoryBuildChange = false;
		}

		if (m_farmingBuildChange)
		{
			GameCommon::Instance()->getCommonPacketSender()->syncRanchBuild(player, (int)RanchBuildType::FARMING);
			m_farmingBuildChange = false;
		}

		if (m_plantBuildChange)
		{
			GameCommon::Instance()->getCommonPacketSender()->syncRanchBuild(player, (int)RanchBuildType::PLANT);
			m_plantBuildChange = false;
		}


		if (player->world->m_isClient)
		{
			for (auto& item : m_orders)
			{
				if (item.status == 3 && item.timeLeft > 0)
				{
					item.timeLeft -= int(player->world->getFrameDeltaTime() * 1000);
					item.timeLeft = (item.timeLeft < 0 ) ? 0 : item.timeLeft;
				}

				if (item.status == 1 && item.refreshTimeLeft > 0)
				{
					item.refreshTimeLeft -= int (player->world->getFrameDeltaTime() * 1000);
					item.refreshTimeLeft = (item.refreshTimeLeft < 0 ) ? 0 : item.refreshTimeLeft;
				}
			}
		}
	}

	void Ranch::setInfo(RanchInfo* info)
	{
		if (!m_info)
		{
			m_info = new RanchInfo();
		}

		m_info->index = info->index;
		m_info->level = info->level;
		m_info->exp = info->exp;
		m_info->giftNum = info->giftNum;
		m_info->sendGiftCount = info->sendGiftCount;
		m_info->nextExp = info->nextExp;
		m_info->prosperity = info->prosperity;
		m_info->achievementPoint = info->achievementPoint;
		m_info->taskStartLevel = info->taskStartLevel;
		m_info->exploreStartLevel = info->exploreStartLevel;
		m_info->prosperityRank = info->prosperityRank;
		m_info->achievementRank = info->achievementRank;
		m_info->clanRank = info->clanRank;
		m_info->isCanFlying = info->isCanFlying;
		m_info->isDestroyHouse = info->isDestroyHouse;
		m_info->isDestroyWarehouse = info->isDestroyWarehouse;
		m_info->startPos = info->startPos;
		m_info->endPos = info->endPos;
		m_infoIsChange = true;
	}

	void Ranch::setHouse(RanchHouse * house)
	{
		if (!m_house)
		{
			m_house = new RanchHouse();
		}
		m_house->id = house->id;
		m_house->level = house->level;
		m_house->nextLevel = house->nextLevel;
		m_house->maxLevel = house->maxLevel;
		m_house->upgradePrice = house->upgradePrice;
		m_house->needPlayerLevel = house->needPlayerLevel;
		m_house->icon = house->icon;
		m_house->name = house->name;
		m_house->nextIcon = house->nextIcon;
		m_house->nextName = house->nextName;
		m_house->upgradeRecipe = house->upgradeRecipe;
		m_houseIsChange = true;
		
	}

	void Ranch::setStorage(RanchStorage * storage)
	{
		if (!m_storage)
		{
			m_storage = new RanchStorage();
		}
		m_storage->level = storage->level;
		m_storage->maxLevel = storage->maxLevel;
		m_storage->capacity = storage->capacity;
		m_storage->nextCapacity = storage->nextCapacity;
		m_storage->upgradePrice = storage->upgradePrice;
		m_storage->needPlayerLevel = storage->needPlayerLevel;
		m_storage->items = storage->items;
		m_storage->upgradeRecipe = storage->upgradeRecipe;
		m_storageIsChange = true;
	}

	void Ranch::updateStorageById(i32 itemId, i32 num)
	{
		if (m_storage)
		{
			m_storage->setItemNum(itemId, num);
			m_storageIsChange = true;
		}
	}

	void Ranch::setRankByTypeFromLua(int rankType, std::vector<RanchRank> ranks)
	{
		vector<RanchRank>::type items = vector<RanchRank>::type();
		for (auto item : ranks)
		{
			items.push_back(item);
		}
		setRankByType(rankType, items);
	}

	void Ranch::setRankByType(int rankType, const vector<RanchRank>::type& ranks)
	{
		switch ((RanchRankType)rankType) {
		case RanchRankType::CLAN:
			setClanRank(ranks);
			break;
		case RanchRankType::PROSPERITY:
			setProsperityRank(ranks);
			break;
		case RanchRankType::ACHIEVEMENT:
			setAchievementRank(ranks);
			break;
		}
	}

	void Ranch::setBuildByTypeFromLua(int buildType, std::vector<RanchBuild> builds)
	{
		vector<RanchBuild>::type items = vector<RanchBuild>::type();
		for (auto item : builds)
		{
			items.push_back(item);
		}
		setBuildByType(buildType, items);
	}

	void Ranch::setOrdersFromLua(std::vector<RanchOrder> orders)
	{
		m_orders.clear();
		for (auto item : orders)
		{
			m_orders.push_back(item);
		}
		m_ordersChange = true;
	}

	void Ranch::setAchievementsFromLua(std::vector<RanchAchievement> achievements)
	{
		m_achievements.clear();
		for (auto item : achievements)
		{
			m_achievements.push_back(item);
		}
		m_achievementsChange = true;
	}

	void Ranch::setTimePricesFromLua(std::vector<RanchTimePrice> timePrices)
	{
		m_timePrices.clear();
		for (auto item : timePrices)
		{
			m_timePrices.push_back(item);
		}
		m_timePricesChange = true;
	}

	void Ranch::setShortcutFreeTimesFromLua(std::vector<std::vector<i32>> times)
	{
		m_shortcutFreeTimes.clear();
		for (const auto& item : times)
		{
			m_shortcutFreeTimes[item[0]] = item[1];
		}
		m_shortcutFreeTimesChange = true;
	}

	void Ranch::setBuildByType(int buildType, const vector<RanchBuild>::type & builds)
	{
		switch ((RanchBuildType)buildType) {
		case RanchBuildType::FARMING:
			setFarmingBuild(builds);
			break;
		case RanchBuildType::FACTORY:
			setFactoryBuild(builds);
			break;
		case RanchBuildType::PLANT:
			setPlantBuild(builds);
			break;
		}
	}

	void Ranch::setOrders(const vector<RanchOrder>::type& orders)
	{
		m_orders.clear();
		for (auto item : orders)
		{
			m_orders.push_back(item);
		}
	}

	void Ranch::setAchievements(const vector<RanchAchievement>::type& achievements)
	{
		m_achievements.clear();
		for (auto item : achievements)
		{
			item.receive = (item.level < item.maxLevel && item.exp >= item.nextExp) ? 1 : 0;
			m_achievements.push_back(item);
		}
		sort(m_achievements.begin(), m_achievements.end(), AchievementSort());
	}

	void Ranch::setTimePrices(const vector<RanchTimePrice>::type & timePrices)
	{
		m_timePrices.clear();
		for (auto item : timePrices)
		{
			m_timePrices.push_back(item);
		}

		if (m_timePrices.size() > 1)
		{
			sort(m_timePrices.begin(), m_timePrices.end(), TimeSort());
		}
	}

	void Ranch::setShortcutFreeTimes(const map<i32, i32>::type times)
	{
		m_shortcutFreeTimes.clear();
		for (auto item : times)
		{
			m_shortcutFreeTimes[item.first] = item.second;
		}
	}

	i32 Ranch::getTimePrice(i32 timeLeft)
	{
		for (const auto& item : m_timePrices)
		{
			if (timeLeft<=item.timeLeft)
			{
				return item.price;
			}
		}
		return m_timePrices.size() > 0 ? m_timePrices[m_timePrices.size() - 1].price : 0;
	}

	i32 Ranch::getItemTimePrice(i32 timeLeft, i32 itemId)
	{
		auto freeItem = m_shortcutFreeTimes.find(itemId);
		if (freeItem != m_shortcutFreeTimes.end() && freeItem->second > 0)
		{
			return 0;
		}

		return getTimePrice(timeLeft);
	}

	const vector<RanchRank>::type& Ranch::getRankByType(int rankType)
	{
		switch ((RanchRankType)rankType) {
		case RanchRankType::CLAN:
			return m_clanRank;
		case RanchRankType::PROSPERITY:
			return m_prosperityRank;
		case RanchRankType::ACHIEVEMENT:
			return m_achievementRank;
		}
		return m_prosperityRank;
	}

	const vector<RanchBuild>::type & Ranch::getBuildByType(int buildType)
	{
		switch ((RanchBuildType)buildType) {
		case RanchBuildType::FARMING:
			return m_farmingBuild;
		case RanchBuildType::FACTORY:
			return m_factoryBuild;
		case RanchBuildType::PLANT:
			return m_plantBuild;
		}
		return m_farmingBuild;
	}

	void Ranch::setClanRank(const vector<RanchRank>::type& ranks)
	{
		m_clanRank.clear();
		for (auto rank : ranks)
		{
			m_clanRank.push_back(rank);
		}
		m_clanRankChange = true;
	}

	void Ranch::setProsperityRank(const vector<RanchRank>::type& ranks)
	{
		m_prosperityRank.clear();
		for (auto rank : ranks)
		{
			m_prosperityRank.push_back(rank);
		}
		m_prosperityRankChange = true;
	}

	void Ranch::setAchievementRank(const vector<RanchRank>::type& ranks)
	{
		m_achievementRank.clear();
		for (auto rank : ranks)
		{
			m_achievementRank.push_back(rank);
		}
		m_achievementRankChange = true;
	}

	void Ranch::setPlantBuild(const vector<RanchBuild>::type & builds)
	{
		m_plantBuild.clear();
		for (auto build : builds)
		{
			m_plantBuild.push_back(build);
		}
		m_plantBuildChange = true;
	}

	void Ranch::setFactoryBuild(const vector<RanchBuild>::type & builds)
	{
		m_factoryBuild.clear();
		for (auto build : builds )
		{
			m_factoryBuild.push_back(build);
		}
		m_factoryBuildChange = true;
	}

	void Ranch::setFarmingBuild(const vector<RanchBuild>::type & builds)
	{
		m_farmingBuild.clear();
		for (auto build : builds)
		{
			m_farmingBuild.push_back(build);
		}
		m_farmingBuildChange = true;
	}

	void Ranch::dealloc()
	{
		if (m_info)
		{
			delete m_info;
			m_info = nullptr;
		}
		
		if (m_storage)
		{
			delete m_storage;
			m_storage = nullptr;
		}
		
		if (m_house)
		{
			delete m_house;
			m_house = nullptr;
		}

		m_clanRank.clear();
		m_prosperityRank.clear();
		m_achievementRank.clear();
		m_factoryBuild.clear();
		m_farmingBuild.clear();
		m_plantBuild.clear();
		m_orders.clear();
		m_achievements.clear();
	}

}
