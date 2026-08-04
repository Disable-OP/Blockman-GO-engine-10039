#pragma once
#ifndef __CHEST_LOTTERY_SETTING_HEADER__
#define __CHEST_LOTTERY_SETTING_HEADER__

#include "Core.h"

using namespace LORD;

namespace BLOCKMAN
{
	enum LotteryChestType
	{
		NONE_CHEST = 0,
		BRONZE_CHEST,
		SILVER_CHEST,
		GOLD_CHEST,
		ULTIMATE_CHEST
	};

	class LotteryChest
	{
	public:
		int Id = 0;
		LotteryChestType Type = NONE_CHEST;
		String Name = "";
		String Image = "";
		String Actor = "";
		String MatImage = "";
		int NeedKey = 0;
		int AddIntegral = 0;
		int NeedIntegral = 0;

		int HasNum = 0;
		bool HasChange = false;
	};

	class LotteryChestReward
	{
	public:
		int Id = 0;
		LotteryChestType Type = NONE_CHEST;
		String Title = "";
		String Name = "";
		String Image = "";
		String Desc = "";
		int Count = 0;
	};

	class ChestLotterySetting
	{
	private:
		static map<int, LotteryChest*>::type m_lottery_chest_map;
		static map<int, LotteryChestReward*>::type m_lottery_chest_reward_map;
		
	public:
		static void	loadSetting();
		static void	unloadSetting();

		static map<int, LotteryChest*>::type getLotteryChests() { return m_lottery_chest_map; }

		static LotteryChest* findLotteryChestById(int id);
		static LotteryChest* findLotteryChestByType(LotteryChestType type);

		static list<LotteryChestReward*>::type findLotteryChestRewardByType(LotteryChestType type);
		static LotteryChestReward* findLotteryChestRewardById(int id);

		static void parseUpdateLotteryChestData(const String& data);

	private:
		static void loadChestSetting();
		static void loadChestRewardSetting();
	};
}

#endif