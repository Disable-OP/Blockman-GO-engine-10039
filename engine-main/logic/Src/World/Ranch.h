#pragma once
#ifndef __RANCH_HEADER__
#define __RANCH_HEADER__

#include "BM_TypeDef.h"
#include "Network/protocol/PacketDefine.h"
#include "Setting/RanchSetting.h"
#include <list>

namespace BLOCKMAN
{
	class EntityPlayer;

	enum class SyncRanchType {
		DEFAULT,
		OWNER,
		INFO,
		STORAGE,
		HOUSE,
		ORDER,
		ACHIEVEMENT,
		TIME_PRICE,
		SHORTCUT_FREE_TIMES
	};

	enum class RanchRankType {
		DEFAULT,
		CLAN,
		PROSPERITY,
		ACHIEVEMENT
	};

	enum class RanchBuildType {
		DEFAULT,
		FARMING,
		FACTORY,
		PLANT
	};

	enum class RanchOperationType {
		NONE,
		HOUSE_UPGRADE,
		MANOR_RECEIVE,
		MANOR_EXPAND,
		STORAGE_UPGRADE,
		USE_CUBE_UPGRADE_STORAGE,
	};

	class RanchInfo
	{
	public:
		i32 index = 0;
		i32 level = 0;
		i32 exp = 0;
		i32 giftNum = 0;
		i32 sendGiftCount = 0;
		i32 achievementPoint = 0;
		i32 nextExp = 0;
		i32 prosperity = 0;
		i32 taskStartLevel = 0;
		i32 exploreStartLevel = 0;
		i32 prosperityRank = 0;
		i32 achievementRank = 0;
		i32 clanRank = 0;
		bool isCanFlying = false;
		bool isDestroyHouse = false;
		bool isDestroyWarehouse = false;
		Vector3 startPos = Vector3::ZERO;
		Vector3 endPos = Vector3::ZERO;

		DEFINE_NETWORK_SERIALIZER(index, level, exp, giftNum, sendGiftCount, achievementPoint, nextExp, prosperity, 
			taskStartLevel, exploreStartLevel, prosperityRank, achievementRank, clanRank, startPos, endPos , isCanFlying, isDestroyHouse , isDestroyWarehouse);
		RanchInfo() {};
	};

	class RanchStorage {
	public:
		i32 level = 0;
		i32 maxLevel = 1;
		i32 capacity = 0;
		i32 nextCapacity = 0;
		i32 upgradePrice = 0;
		i32 needPlayerLevel = 0;
		std::vector<RanchCommon> items = std::vector<RanchCommon>();
		std::vector<RanchCommon> upgradeRecipe = std::vector<RanchCommon>();
		DEFINE_NETWORK_SERIALIZER(level, maxLevel, capacity, nextCapacity, items, upgradePrice, needPlayerLevel, upgradeRecipe);
		RanchStorage() {};

	public:
		i32 getTotalNum();
		bool canSave(i32 num);
		void setItemNum(i32 itemId, i32 num);
		i32 getItemNum(i32 itemId);
		bool checkInventory(i32 itemId, i32 num);
	};

	class RanchHouse
	{
	public:
		i32 id = 0;
		i32 level = 0;
		i32 nextLevel = 0;
		i32 maxLevel = 1;
		i32 upgradePrice = 0;
		i32 needPlayerLevel = 0;
		String icon = "";
		String name = "";
		String nextIcon = "";
		String nextName = "";
		std::vector<RanchCommon> upgradeRecipe;
		DEFINE_NETWORK_SERIALIZER(id, level, nextLevel, maxLevel, needPlayerLevel, upgradePrice, icon, name, nextIcon, nextName, upgradeRecipe);
		RanchHouse() {}
	};

	class RanchBuild
	{
	public:
		i32 id = 0;
		i32 price = 0;
		i32 num = 0;
		i32 maxNum = 1;
		i32 needLevel = 0;
		DEFINE_NETWORK_SERIALIZER(id, price, num, maxNum, needLevel);
		RanchBuild() {}
	};

	class RanchRank
	{
	public:
		ui64 id = 0;
		i32 level = 0;
		i32 rank = 0;
		i64 value = 0;
		String name = "";
		String userNum = "0/0";
		DEFINE_NETWORK_SERIALIZER(id, level, rank, value, name, userNum)
		RanchRank() {}
	};

	class RanchOrderInfo
	{
	public:
		
		i32 index = 0;
		i32 itemId = 0;
		i64 helpId = 0;
		i64 helperId = 0;
		i32 needNum = 0;
		i32 helperSex = 0;
		i32 fullBoxNum = 0;
		i32 boxAmount = 0;
		String uniqueId = "";
		bool isDone = false;
		bool isHelp = false;
		bool isHot = false;
		
		DEFINE_NETWORK_SERIALIZER(index, itemId, needNum, helpId, helperId, helperSex, uniqueId, isDone, isHelp, isHot);
		RanchOrderInfo() {}
	};

	class RanchOrder
	{
	public:
		i32 id = 0;
		i32 price = 0;
		i32 status = 0;
		i32 timeLeft = 0;
		i32 refreshTimeLeft = 0;
		String msg = "";
		String guestIcon = "";
		bool isHot = false;
		std::vector<RanchOrderInfo> infos;
		std::vector<RanchCommon> rewards;
		DEFINE_NETWORK_SERIALIZER(id, price, status, timeLeft, refreshTimeLeft, msg, guestIcon, isHot, infos, rewards);
		RanchOrder() {}
	};

	class RanchAchievement 
	{
	public:
		i64 id = 0;
		i32 exp = 0;
		i32 level = 0;
		i32 nextExp = 1;
		i32 maxLevel = 10;
		i32 playerExp = 0;
		i32 achievement = 0;
		bool receive = 0;

		String icon = "";
		String desc = "";
		String name = "";
		DEFINE_NETWORK_SERIALIZER(id, exp, level, nextExp, maxLevel, playerExp, achievement, icon, desc, name);
		RanchAchievement() {};
	};

	class RanchTimePrice 
	{
	public:
		i32 id = 0;
		i32 price = 0;
		i64 timeLeft = 0;
		DEFINE_NETWORK_SERIALIZER(id, price, timeLeft);
		RanchTimePrice() {};
	};

	struct TimeSort
	{
		bool operator()(const RanchTimePrice& first, const RanchTimePrice& second)
		{
			return first.timeLeft <  second.timeLeft;
		}
	};

	struct AchievementSort
	{
		bool operator()(const RanchAchievement& first, const RanchAchievement& second)
		{
			return first.receive > second.receive;
		}
	};


	class Ranch : public ObjectAlloc
	{
	public:
		Ranch();
		~Ranch();

	private:
		bool m_infoIsChange = false;
		bool m_houseIsChange = false;
		bool m_storageIsChange = false;
		bool m_clanRankChange = false;
		bool m_prosperityRankChange = false;
		bool m_achievementRankChange = false;
		bool m_plantBuildChange = false;
		bool m_factoryBuildChange = false;
		bool m_farmingBuildChange = false;
		bool m_ordersChange = false;
		bool m_achievementsChange = false;
		bool m_timePricesChange = false;
		bool m_shortcutFreeTimesChange = false;

	private:
		void setClanRank(const vector<RanchRank>::type& ranks);
		void setProsperityRank(const vector<RanchRank>::type& ranks);
		void setAchievementRank(const vector<RanchRank>::type& ranks);

		void setPlantBuild(const vector<RanchBuild>::type& builds);
		void setFactoryBuild(const vector<RanchBuild>::type& builds);
		void setFarmingBuild(const vector<RanchBuild>::type& builds);

	public:
		void onUpdate(EntityPlayer* player);
		void setInfo(RanchInfo* info);
		void setHouse(RanchHouse* house);
		void setStorage(RanchStorage* storage);
		void updateStorageById(i32 itemId, i32 num);
		void setRankByTypeFromLua(int rankType, std::vector<RanchRank> ranks);
		void setBuildByTypeFromLua(int buildType, std::vector<RanchBuild> builds);
		void setOrdersFromLua(std::vector<RanchOrder> orders);
		void setAchievementsFromLua(std::vector<RanchAchievement> achievements);
		void setTimePricesFromLua(std::vector<RanchTimePrice> timePrices);
		void setShortcutFreeTimesFromLua(std::vector<std::vector<i32>> times);
		
		void setRankByType(int rankType, const vector<RanchRank>::type& ranks);
		void setBuildByType(int buildType, const vector<RanchBuild>::type& builds);
		void setOrders(const vector<RanchOrder>::type& orders);
		void setAchievements(const vector<RanchAchievement>::type& achievements);
		void setTimePrices(const vector<RanchTimePrice>::type& timePrices);
		void setShortcutFreeTimes(const map<i32, i32>::type times);

		i32 getTimePrice(i32 timeLeft);
		i32 getItemTimePrice(i32 timeLeft, i32 itemId);

		RanchInfo* getInfo() { return m_info; }
		RanchHouse* getHouse() { return m_house; }
		RanchStorage* getStorage() { return m_storage; }
		const vector<RanchRank>::type& getClanRank() { return m_clanRank; }
		const vector<RanchRank>::type& getProsperityRank() { return m_prosperityRank; }
		const vector<RanchRank>::type& getAchievementRank() { return m_achievementRank; }

		const vector<RanchBuild>::type& getPlantBuild() { return m_plantBuild; }
		const vector<RanchBuild>::type& getFactoryBuild() { return m_factoryBuild; }
		const vector<RanchBuild>::type& getFarmingBuild() { return m_farmingBuild; }
		const vector<RanchRank>::type& getRankByType(int rankType);
		const vector<RanchBuild>::type& getBuildByType(int buildType);
		const vector<RanchOrder>::type& getOrders() { return m_orders; }
		const vector<RanchAchievement>::type& getAchievements() { return m_achievements; }
		const vector<RanchTimePrice>::type& getTimePrices() { return m_timePrices; }
		const map<i32, i32>::type& getShortcutFreeTimes() { return m_shortcutFreeTimes; }

	private:
		void dealloc();

	private:
		RanchInfo * m_info = nullptr;
		RanchHouse* m_house = nullptr;
		RanchStorage* m_storage = nullptr;

		vector<RanchRank>::type m_clanRank;
		vector<RanchRank>::type m_prosperityRank;
		vector<RanchRank>::type m_achievementRank;

		vector<RanchBuild>::type m_plantBuild;
		vector<RanchBuild>::type m_factoryBuild;
		vector<RanchBuild>::type m_farmingBuild;

		vector<RanchOrder>::type m_orders;
		vector<RanchAchievement>::type m_achievements;

		vector<RanchTimePrice>::type m_timePrices;
		map<i32, i32>::type m_shortcutFreeTimes;

	};
};

#endif