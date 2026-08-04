#pragma once
#ifndef __BIRD_SIMULATOR_HEADER__
#define __BIRD_SIMULATOR_HEADER__

#include "BM_TypeDef.h"
#include "Network/protocol/PacketDefine.h"
#include <list>

namespace BLOCKMAN
{
	class EntityPlayer;

	enum class SyncBirdSimulatorType {
		DEFAULT,
		BAG,
		STORE,
		PERSONAL_STORE,
		PLAYER_INFO,
		DRESS,
		FOOD,
		TAKS,
		ATLAS,
		SCOPE,
		ACTIVITY,
		TIME_PRICE,
		BUFF,
	};

	enum class BirdCurrencyType
	{
		PLATFORM_MONEY = 1,
		PLATFORM_GOLD = 2,
		GAME_MONEY = 3,
		EGG_TICKET = 4,
	};

	class BirdPlayerInfo
	{
	public:
		i64 score = 0;
		i64 eggTicket = 0;
		bool isConvert = false;
		DEFINE_NETWORK_SERIALIZER(score, eggTicket, isConvert);
		BirdPlayerInfo() {};
	};

	class BirdStoreItem
	{
	public:
		i32 id = 0;
		i32 storeId = 0;
		i32 type = 0;
		i32 price = 0;
		i32 currencyType = 0;
		i32 status = 1; //1 not  buy  2  not use 3 in use
		String name = "";
		String desc = "";
		String icon = "";
		DEFINE_NETWORK_SERIALIZER(id, storeId, type, price, currencyType, status, name, desc, icon);
		BirdStoreItem() {};
	};

	class BirdStore
	{
	public:
		i32 id = 0;
		Vector3i startPos = Vector3i::INVALID;
		Vector3i endPos = Vector3i::INVALID;
		std::vector<BirdStoreItem> items;
		DEFINE_NETWORK_SERIALIZER(id, startPos, endPos, items);
		BirdStore() {};
	};

	class BirdGiftBagItem
	{
	public:
		i32 id = 0;
		i32 num = 0;
		i32 type = 0;
		i32 giftBagId = 0;
		String icon = "";
		bool isAutoUse = false;

		DEFINE_NETWORK_SERIALIZER(id, num, type, giftBagId, icon, isAutoUse);
		BirdGiftBagItem() {};
	};

	class BirdGiftBag
	{
	public:
		i32 id = 0;
		i32 price = 0;
		i32 limit = 0;
		i32 timeLeft = 0;
		i32 currencyType = 0;
		String icon = "";
		String desc = "";
		String name = "";
		String discount = "";
		String topLeftIcon = "";
		String topLeftName = "";
		String topRightIcon = "";
		String topRightName = "";
		std::vector<BirdGiftBagItem> items;
		DEFINE_NETWORK_SERIALIZER(id, price, limit, timeLeft, discount, currencyType, icon, desc, name, topLeftIcon, topLeftName, topRightIcon, topRightName, items);
		BirdGiftBag() {};
	};

	class BirdPersonalStoreTab
	{
	public:
		i32 tabId = 0;
		String tabName = "";
		String tabIcon = "";
		std::vector<BirdGiftBag> items;
		DEFINE_NETWORK_SERIALIZER(tabId, tabName, tabIcon, items);
		BirdPersonalStoreTab() {};
	};

	class BirdDress
	{
	public:
		i32 id = 0;
		i32 num = 0;
		i32 type = 1;
		i32 isUse = 0;
		String bodyId = "";
		String bodyName = "";
		DEFINE_NETWORK_SERIALIZER(id, num, type, isUse, bodyId, bodyName);
		BirdDress() {};
	};

	class BirdInfo
	{
	public:
		i64 id = 0;
		i32 level = 0;
		i32 maxLevel = 0;
		i32 exp = 0;
		i32 nextExp = 0;
		i32 quality = 0;
		i32 speed = 0;

		float gatherTime = 0.f;
		float convertTime = 0.f;
		float gather = 0.f;
		float attack = 0.f;
		float conver = 0.f;
		bool isCarry = false;
		i32 carrySort = 0;

		String name = "";
		String icon = "";
		String skill = "";
		String talent = "";

		String glassesId = "";
		String hatId = "";
		String beakId = "";
		String wingId = "";
		String tailId = "";
		String effectId = "";
		String bodyId = "";

		DEFINE_NETWORK_SERIALIZER(id, level, exp, maxLevel, nextExp, quality, speed, gather, attack, conver, isCarry, name, icon, skill, talent, glassesId, hatId, beakId, wingId, tailId, effectId, bodyId, gatherTime, convertTime);
		BirdInfo() {};
	};

	class BirdBag
	{
	public:
		i32 curCarry = 0;
		i32 maxCarry = 1;
		i32 curCapacity = 0;
		i32 maxCapacity = 1;
		i32 maxCarryLevel = 0;
		i32 maxCapacityLevel = 0;
		i32 expandCarryPrice = 0;
		i32 expandCapacityPrice = 0;
		i32 expandCurrencyType = 0;
		std::vector<BirdInfo> birds;
		DEFINE_NETWORK_SERIALIZER(curCarry, maxCarry, curCapacity, maxCapacity, maxCarryLevel, maxCapacityLevel, expandCarryPrice, expandCapacityPrice, expandCurrencyType, birds);
		BirdBag() {};
	};

	class BirdFood
	{
	public:
		i32 id = 0;
		i32 num = 0;
		String icon = "";
		String desc = "";
		String name = "";
		DEFINE_NETWORK_SERIALIZER(id, num, name, desc, icon);
		BirdFood() {};
	};

	class BirdTaskItem
	{
	public:
		i32 id = 0;
		i32 curValue = 0;
		i32 needValue = 0;
		String name = "";
		String desc = "";
		DEFINE_NETWORK_SERIALIZER(id, curValue, needValue, name, desc);
		BirdTaskItem() {};
	};

	class  BirdTask
	{
	public:
		i32 taskId = 0;
		i32 entityId = 0;
		String taskName = "";
		std::vector<BirdTaskItem> contents;
		DEFINE_NETWORK_SERIALIZER(taskId, entityId, taskName, contents);
		BirdTask() {};
	};

	class BirdEgg
	{
	public:
		i64 id = 0;
		i32 quality = 1;
		float prob = 0.f;
		String icon = "";
		bool isHave = false;
		DEFINE_NETWORK_SERIALIZER(id, quality, icon, isHave);
		BirdEgg() {};
	};

	class BirdAtlas
	{
	public:
		i32 id = 0;
		String eggIcon = "";
		std::vector<BirdEgg> items;
		DEFINE_NETWORK_SERIALIZER(id, eggIcon, items);
		BirdAtlas() {};
	};

	class BirdGain
	{
	public:
		i32 itemId = 0;
		i32 num = 0;
		String icon = "";
		DEFINE_NETWORK_SERIALIZER(itemId, num, icon);
		BirdGain() {};
	};

	class BirdScope
	{
	public:
		i32 id = 0;
		i32 type = 0;
		Vector3i startPos = Vector3i::INVALID;
		Vector3i endPos = Vector3i::INVALID;
		DEFINE_NETWORK_SERIALIZER(id, type, startPos, endPos);
		BirdScope() {};
	};

	class BirdTimePrice
	{
	public:
		i32 id = 0;
		i32 price = 0;
		i64 timeLeft = 0;
		DEFINE_NETWORK_SERIALIZER(price, timeLeft);
		BirdTimePrice() {};
	};

	class BirdBuff
	{
	public:
		i32 id = 0;
		i32 level = 0;
		i32 timeLeft = 0;
		String icon = "";
		DEFINE_NETWORK_SERIALIZER(id, level, timeLeft, icon);
		BirdBuff() {};
	};

	struct BirdLevelSort
	{
		bool operator()(const BirdInfo& first, const BirdInfo& second)
		{
			return first.level > second.level;
		}
	};

	struct BirdQualitySort
	{
		bool operator()(const BirdInfo& first, const BirdInfo& second)
		{
			return first.quality > second.quality;
		}
	};

	struct BirdCarrySort
	{
		bool operator()(const BirdInfo& first, const BirdInfo& second)
		{
			return first.carrySort > second.carrySort;
		}
	};

	struct BirdDressSort
	{
		bool operator()(const BirdDress& first, const BirdDress& second)
		{
			return first.isUse > second.isUse;
		}
	};

	class BirdSimulator : public ObjectAlloc
	{
	public:
		BirdSimulator();
		~BirdSimulator();

	private:
		BirdBag* m_bag = nullptr;
		BirdGiftBag* m_activity = nullptr;
		BirdPlayerInfo* m_playerInfo = nullptr;
		vector<BirdAtlas>::type m_atlas;
		vector<BirdTask>::type m_tasks;
		vector<BirdFood>::type m_foods;
		vector<BirdDress>::type m_dress;
		vector<BirdStore>::type m_stores;
		vector<BirdPersonalStoreTab>::type m_personalStore;

		vector<BirdScope>::type m_scopes;
		vector<BirdTimePrice>::type m_timePrices;
		vector<BirdBuff>::type m_buffs;

	private:
		bool m_isBagChange = false;
		bool m_isAtlasChange = false;
		bool m_isTaskChange = false;
		bool m_isFoodChange = false;
		bool m_isDressChange = false;
		bool m_isStoreChange = false;
		bool m_isActivityChange = false;
		bool m_isPlayerInfoChange = false;
		bool m_isPersonalStoreChange = false;
		bool m_isScopeChange = false;
		bool m_isTimePriceChange = false;
		bool m_isBuffChange = false;

	public:
		void onUpdate(EntityPlayer* player);

		
		void setAtlasFromLua(std::vector<BirdAtlas> atlas);
		void setTasksFromLua(std::vector<BirdTask> tasks);
		void setFoodsFromLua(std::vector<BirdFood> foods);
		void setDressFromLua(std::vector<BirdDress> dress);
		void setStoresFromLua(std::vector<BirdStore> stores);
		void setPersonalStoreFromLua(std::vector<BirdPersonalStoreTab> tabs);
		void setScopesFromLua(std::vector<BirdScope> scopes);
		void setTimePricesFromLua(std::vector<BirdTimePrice> prices);
		void setBuffsFromLua(std::vector<BirdBuff> buffs);

		void setBag(BirdBag* bag);
		void setActivity(BirdGiftBag* activiy);
		void setPlayerInfo(BirdPlayerInfo* playerInfo);

		bool setTaskItem(bool isRemove, const BirdTask& taskItem);
		void setStoreItem(const BirdStore& storeItem);
		void setPersonalStoreTab(const BirdPersonalStoreTab& tab);

		void setAtlas(const vector<BirdAtlas>::type& atlas);
		void setTasks(const vector<BirdTask>::type& tasks);
		void setFoods(const vector<BirdFood>::type& foods);
		void setDress(const vector<BirdDress>::type& dress);
		void setStores(const vector<BirdStore>::type& stores);
		void setPersonalStore(const vector<BirdPersonalStoreTab>::type& tabs);
		void setScopes(const vector<BirdScope>::type& tabs);
		void setTimePrices(const vector<BirdTimePrice>::type& prices);
		void setBuffs(const vector<BirdBuff>::type& buffs);

		BirdBag* getBag() { return m_bag; }
		BirdGiftBag* getActivity() { return m_activity; }
		BirdPlayerInfo* getPlayerInfo() { return m_playerInfo; }


		const vector<BirdAtlas>::type& getAtlas() { return m_atlas; }
		const vector<BirdTask>::type& getTasks() { return m_tasks; }
		const vector<BirdFood>::type& getFoods() { return m_foods; }
		const vector<BirdDress>::type& getDress() { return m_dress; }
		const vector<BirdStore>::type& getStores() { return m_stores; }
		const vector<BirdPersonalStoreTab>::type& getPersonalStore() { return m_personalStore; }
		const vector<BirdScope>::type& getScopes() { return m_scopes; }
		const vector<BirdTimePrice>::type& getTimePrices() { return m_timePrices; }
		const vector<BirdBuff>::type& getBuffs() { return m_buffs; }
		BirdTask getTaskById(i32 taskId);

		BirdStore getStoreById(i32 storeId);
		BirdInfo getBirdById(i64 birdId);
		std::vector<BirdInfo>getBirds();
		std::vector<BirdInfo>getBirdsSort(bool isByLevel);
		std::vector<BirdDress> findDressByType(i32 type, i64 birdId);
		i64 getEggTicket() { return m_playerInfo ? m_playerInfo->eggTicket : 0; }
		i64 getScore() { return m_playerInfo ? m_playerInfo->score : 0; }
		i32 getStoreId(const Vector3 pos);
		bool isConvert() { return m_playerInfo ? m_playerInfo->isConvert : false; }
		BirdScope getScopeByPosition(const Vector3 pos);
		i32 getTimePrice(i32 timeLeft);

	private:
		void dealloc();

	private:

	};
};

#endif