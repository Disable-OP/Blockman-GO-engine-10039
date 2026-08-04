#include "BirdSimulator.h"
#include "Common.h"
#include "Entity/EntityPlayer.h"
#include "Network/CommonPacketSender.h"
#include "World/World.h"

namespace BLOCKMAN
{
	BirdSimulator::BirdSimulator()
	{

	}

	BirdSimulator::~BirdSimulator()
	{
		dealloc();
	}

	void BirdSimulator::onUpdate(EntityPlayer* player)
	{
		if (m_isFoodChange)
		{
			GameCommon::Instance()->getCommonPacketSender()->syncBirdSimulator(player, (int)SyncBirdSimulatorType::FOOD);
			m_isFoodChange = false;
		}

		if (m_isDressChange)
		{
			GameCommon::Instance()->getCommonPacketSender()->syncBirdSimulator(player, (int)SyncBirdSimulatorType::DRESS);
			m_isDressChange = false;
		}

		if (m_isBagChange && m_bag)
		{
			GameCommon::Instance()->getCommonPacketSender()->syncBirdSimulator(player, (int)SyncBirdSimulatorType::BAG);
			m_isBagChange = false;
		}

		if (m_isActivityChange && m_activity)
		{
			GameCommon::Instance()->getCommonPacketSender()->syncBirdSimulator(player, (int)SyncBirdSimulatorType::ACTIVITY);
			m_isActivityChange = false;
		}

		if (m_isPlayerInfoChange && m_playerInfo)
		{
			player->setBirdConvert(m_playerInfo->isConvert);
			GameCommon::Instance()->getCommonPacketSender()->syncBirdSimulator(player, (int)SyncBirdSimulatorType::PLAYER_INFO);
			m_isPlayerInfoChange = false;
		}

		if (m_isAtlasChange)
		{
			GameCommon::Instance()->getCommonPacketSender()->syncBirdSimulator(player, (int)SyncBirdSimulatorType::ATLAS);
			m_isAtlasChange = false;
		}

		if (m_isTaskChange)
		{
			GameCommon::Instance()->getCommonPacketSender()->syncBirdSimulator(player, (int)SyncBirdSimulatorType::TAKS);
			m_isTaskChange = false;
		}

		if (m_isStoreChange)
		{
			GameCommon::Instance()->getCommonPacketSender()->syncBirdSimulator(player, (int)SyncBirdSimulatorType::STORE);
			m_isStoreChange = false;
		}

		if (m_isPersonalStoreChange)
		{
			GameCommon::Instance()->getCommonPacketSender()->syncBirdSimulator(player, (int)SyncBirdSimulatorType::PERSONAL_STORE);
			m_isPersonalStoreChange = false;
		}

		if (m_isScopeChange)
		{
			GameCommon::Instance()->getCommonPacketSender()->syncBirdSimulator(player, (int)SyncBirdSimulatorType::SCOPE);
			m_isScopeChange = false;
		}

		if (m_isTimePriceChange)
		{
			GameCommon::Instance()->getCommonPacketSender()->syncBirdSimulator(player, (int)SyncBirdSimulatorType::TIME_PRICE);
			m_isTimePriceChange = false;
		}

		if (m_isBuffChange)
		{
			GameCommon::Instance()->getCommonPacketSender()->syncBirdSimulator(player, (int)SyncBirdSimulatorType::BUFF);
			m_isBuffChange = false;
		}

		if (m_activity)
		{
			m_activity->timeLeft -=  int(player->world->getFrameDeltaTime() * 1000);
			m_activity->timeLeft = (m_activity->timeLeft < 0) ? -1 : m_activity->timeLeft;
		}
	}

	void BirdSimulator::setAtlasFromLua(std::vector<BirdAtlas> atlas)
	{
		m_atlas.clear();
		for (auto item : atlas)
		{
			m_atlas.push_back(item);
		}
		m_isAtlasChange = true;
	}

	void BirdSimulator::setTasksFromLua(std::vector<BirdTask> tasks)
	{
		m_tasks.clear();
		for (auto item : tasks)
		{
			m_tasks.push_back(item);
		}
		m_isTaskChange = true;
	}

	void BirdSimulator::setFoodsFromLua(std::vector<BirdFood> foods)
	{
		m_foods.clear();
		for (auto item : foods)
		{
			m_foods.push_back(item);
		}
		m_isFoodChange = true;
	}

	void BirdSimulator::setDressFromLua(std::vector<BirdDress> dress)
	{
		m_dress.clear();
		for (auto item : dress)
		{
			m_dress.push_back(item);
		}
		m_isDressChange = true;
	}

	void BirdSimulator::setStoresFromLua(std::vector<BirdStore> stores)
	{
		m_stores.clear();
		for (auto item : stores)
		{
			m_stores.push_back(item);
		}
		m_isStoreChange = true;
	}

	void BirdSimulator::setPersonalStoreFromLua(std::vector<BirdPersonalStoreTab> tabs)
	{
		m_personalStore.clear();
		for (auto item : tabs)
		{
			m_personalStore.push_back(item);
		}
		m_isPersonalStoreChange = true;
	}

	void BirdSimulator::setScopesFromLua(std::vector<BirdScope> scopes)
	{
		m_scopes.clear();
		for (auto item : scopes)
		{
			m_scopes.push_back(item);
		}
		m_isScopeChange = true;
	}

	void BirdSimulator::setTimePricesFromLua(std::vector<BirdTimePrice> prices)
	{
		m_timePrices.clear();
		for (auto item : prices)
		{
			m_timePrices.push_back(item);
		}
		m_isTimePriceChange = true;
	}

	void BirdSimulator::setBuffsFromLua(std::vector<BirdBuff> buffs)
	{
		m_buffs.clear();
		for (auto item : buffs)
		{
			m_buffs.push_back(item);
		}
		m_isBuffChange = true;
	}

	void BirdSimulator::setBag(BirdBag * bag)
	{
		if (!m_bag)
		{
			m_bag = new BirdBag();
		}
		m_bag->curCarry = bag->curCarry;
		m_bag->maxCarry = bag->maxCarry;
		m_bag->curCapacity = bag->curCapacity;
		m_bag->maxCapacity = bag->maxCapacity;
		m_bag->maxCarryLevel = bag->maxCarryLevel;
		m_bag->maxCapacityLevel = bag->maxCapacityLevel;
		m_bag->expandCarryPrice = bag->expandCarryPrice;
		m_bag->expandCapacityPrice = bag->expandCapacityPrice;
		m_bag->expandCurrencyType = bag->expandCurrencyType;
		m_bag->birds.clear();
		for (auto& bird : bag->birds )
		{
			bird.carrySort = bird.isCarry ? 1 : 0;
		}
		m_bag->birds = bag->birds;
		m_isBagChange = true;
	}

	void BirdSimulator::setActivity(BirdGiftBag * activiy)
	{
		if (!m_activity)
		{
			m_activity = new BirdGiftBag();
		}
		m_activity->id = activiy->id;
		m_activity->price = activiy->price;
		m_activity->timeLeft = activiy->timeLeft;
		m_activity->currencyType = activiy->currencyType;
		m_activity->icon = activiy->icon;
		m_activity->desc = activiy->desc;
		m_activity->items = activiy->items;
		m_isActivityChange = true;
	}

	void BirdSimulator::setPlayerInfo(BirdPlayerInfo * playerInfo)
	{
		if (!m_playerInfo)
		{
			m_playerInfo = new BirdPlayerInfo();
		}
		m_playerInfo->score = playerInfo->score;
		m_playerInfo->eggTicket = playerInfo->eggTicket;
		m_playerInfo->isConvert = playerInfo->isConvert;
		m_isPlayerInfoChange = true;
	}

	bool BirdSimulator::setTaskItem(bool isRemove, const BirdTask & taskItem)
	{

		for (auto item = m_tasks.begin(); item != m_tasks.end(); item ++)
		{
			BirdTask& task = *item;
			if (task.taskId == taskItem.taskId)
			{
				if (isRemove)
				{
					m_tasks.erase(item);
					return true;
				}
				else
				{
					task = taskItem;
				}
				return false;
			}
		}

		if (!isRemove)
		{
			m_tasks.push_back(taskItem);
			return true;
		}

		return false;
	}

	void BirdSimulator::setStoreItem(const BirdStore & storeItem)
	{
		for (auto& item : m_stores)
		{
			if (item.id == storeItem.id)
			{
				item = storeItem;
				return;
			}
		}
		m_stores.push_back(storeItem);
	}

	void BirdSimulator::setPersonalStoreTab(const BirdPersonalStoreTab & tab)
	{
		for (auto& item : m_personalStore)
		{
			if (item.tabId == tab.tabId)
			{
				item = tab;
				return;
			}
		}
		m_personalStore.push_back(tab);
	}

	void BirdSimulator::setAtlas(const vector<BirdAtlas>::type & atlas)
	{
		m_atlas.clear();
		for (auto item : atlas)
		{
			m_atlas.push_back(item);
		}
	}

	void BirdSimulator::setTasks(const vector<BirdTask>::type& tasks)
	{
		m_tasks.clear();
		for (auto item : tasks)
		{
			m_tasks.push_back(item);
		}
	}

	void BirdSimulator::setFoods(const vector<BirdFood>::type& foods)
	{
		m_foods.clear();
		for (auto item : foods)
		{
			m_foods.push_back(item);
		}
	}

	void BirdSimulator::setDress(const vector<BirdDress>::type & dress)
	{
		m_dress.clear();
		for (auto item : dress)
		{
			m_dress.push_back(item);
		}
	}


	void BirdSimulator::setStores(const vector<BirdStore>::type& stores)
	{
		m_stores.clear();
		for (auto item : stores)
		{
			m_stores.push_back(item);
		}
	}

	void BirdSimulator::setPersonalStore(const vector<BirdPersonalStoreTab>::type & tabs)
	{
		m_personalStore.clear();
		for (auto item : tabs)
		{
			m_personalStore.push_back(item);
		}
	}

	void BirdSimulator::setScopes(const vector<BirdScope>::type & scopes)
	{
		m_scopes.clear();
		for (auto item : scopes)
		{
			m_scopes.push_back(item);
		}
	}

	void BirdSimulator::setTimePrices(const vector<BirdTimePrice>::type & prices)
	{
		m_timePrices.clear();
		for (auto item : prices)
		{
			m_timePrices.push_back(item);
		}
	}

	void BirdSimulator::setBuffs(const vector<BirdBuff>::type & buffs)
	{
		m_buffs.clear();
		for (auto item : buffs)
		{
			m_buffs.push_back(item);
		}
	}

	BirdTask BirdSimulator::getTaskById(i32 taskId)
	{
		for (auto& item : m_tasks)
		{
			if (item.taskId == taskId)
			{
				return item;
			}
		}
		return BirdTask();
	}

	BirdStore BirdSimulator::getStoreById(i32 storeId)
	{
		for (auto item : m_stores)
		{
			if (item.id == storeId)
			{
				return item;
			}
		}
		return BirdStore();
	}

	BirdInfo BirdSimulator::getBirdById(i64 birdId)
	{
		if (m_bag && birdId > 0)
		{
			for (auto& bird : m_bag->birds)
			{
				if (bird.id==birdId)
				{
					return bird;
				}
			}
		}
		return BirdInfo();
	}

	std::vector<BirdInfo> BirdSimulator::getBirds()
	{
		return m_bag ? m_bag->birds : std::vector<BirdInfo>();
	}

	std::vector<BirdInfo> BirdSimulator::getBirdsSort(bool isByLevel)
	{
		std::vector<BirdInfo> birds = std::vector<BirdInfo>();

		if (m_bag && m_bag->birds.size() > 0)
		{
			std::vector<BirdInfo> carrys = std::vector<BirdInfo>();
			std::vector<BirdInfo> unCarrys = std::vector<BirdInfo>();
			for (auto& item : m_bag->birds)
			{
				if (item.isCarry)
				{
					carrys.push_back(item);
				}
				else
				{
					unCarrys.push_back(item);
				}
			}

			if (isByLevel)
			{
				if (carrys.size() > 0)
				{
					sort(carrys.begin(), carrys.end(), BirdLevelSort());
				}

				if (unCarrys.size() > 0)
				{
					sort(unCarrys.begin(), unCarrys.end(), BirdLevelSort());
				}
			}
			else
			{
				if (carrys.size() > 0)
				{
					sort(carrys.begin(), carrys.end(), BirdQualitySort());
				}

				if (unCarrys.size() > 0)
				{
					sort(unCarrys.begin(), unCarrys.end(), BirdQualitySort());
				}
			}
			birds = carrys;
			for (auto& item : unCarrys)
			{
				birds.push_back(item);
			}
		}
		return birds;
	}

	std::vector<BirdDress> BirdSimulator::findDressByType(i32 type, i64 birdId)
	{
		std::vector<BirdDress> items = std::vector<BirdDress>();
		for (auto item : m_dress)
		{
			if (item.type == type && (item.isUse == 0  || item.isUse == birdId))
			{
				items.push_back(item);
			}
		}
		return items;
	}

	i32 BirdSimulator::getStoreId(const Vector3 pos)
	{
		for (const auto & item : m_stores)
		{
			if (item.startPos.x <= pos.x && item.endPos.x >= pos.x && item.startPos.z <= pos.z && item.endPos.z >= pos.z && item.startPos.y <= pos.y &&  item.endPos.y >= pos.y)
			{
				return item.id;
			}
		}
		return 0;
	}

	BirdScope BirdSimulator::getScopeByPosition(const Vector3 pos)
	{
		for (const auto & item : m_scopes)
		{
			if (item.startPos.x <= pos.x && item.endPos.x >= pos.x && item.startPos.z <= pos.z && item.endPos.z >= pos.z && item.startPos.y <= pos.y &&  item.endPos.y >= pos.y)
			{
				return item;
			}
		}
		return BirdScope();
	}

	i32 BirdSimulator::getTimePrice(i32 timeLeft)
	{
		for (const auto& item : m_timePrices)
		{
			if (timeLeft <= item.timeLeft)
			{
				return item.price;
			}
		}
		return m_timePrices.size() > 0 ? m_timePrices[m_timePrices.size() - 1].price : 0;
	}

	void BirdSimulator::dealloc()
	{
		if (m_bag)
		{
			delete m_bag;
			m_bag = nullptr;
		}

		if (m_playerInfo)
		{
			delete m_playerInfo;
			m_playerInfo = nullptr;
		}

		if (m_activity)
		{
			delete m_activity;
			m_activity = nullptr;
		}

		m_tasks.clear();
		m_foods.clear();
		m_dress.clear();
		m_stores.clear();
		m_personalStore.clear();
		m_scopes.clear();
		m_timePrices.clear();
	}

}
