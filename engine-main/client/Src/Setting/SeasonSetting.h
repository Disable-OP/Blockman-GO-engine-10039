#pragma once
#ifndef __SEASON_SETTING_HEADER__
#define __SEASON_SETTING_HEADER__

#include "Core.h"

using namespace LORD;

namespace BLOCKMAN
{
	enum SeasonHonorId
	{
		SEASON_NONE = -1,
		BRONZE,
		SILVER,
		GOLD,
		JEWEL,
		KING
	};

	enum SeasonRankType
	{
		TYPE_NONE = -1,
		TYPE_RIGHT,
		TYPE_LEFT
	};

	enum SeasonRewardType
	{
		REWARD_MONEY = 1,
		REWARD_KEY,
		REWARD_HONOR_UP,
		REWARD_CHEST
	};

	class Season
	{
	public:
		int Id = 0;
		SeasonHonorId HonorId = SEASON_NONE;
		String Name = "";
		String HonorName = "";
		String HonorIcon = "";
		String HonorFlag = "";
		int Score = 0;

		list<int>::type UpRewardIds;
		list<int>::type NormalRewardIds;
	};

	class SeasonReward
	{
	public:
		int Id = 0;
		String Icon = "";
		SeasonRewardType Type;
		int Count = 0;
	};

	class SeasonRankReward
	{
	public:
		int Id = 0;
		int SeasonId = 0;
		String Icon = "";
		int MinRank = 0;
		int MaxRank = 0;
		SeasonRewardType Type;
		int Count = 0;
	};

	class SeasonRankItem
	{
	public:
		int Rank = 0;
		String Name = "";
		int Honor = 0;
		SeasonRankType Type = TYPE_NONE;

	public:
		SeasonRankItem(int rank, String name, int honor, SeasonRankType type)
			: Rank(rank), Name(name), Honor(honor), Type(type)
		{
		}
	};

	class SeasonSetting
	{
	private:
		static map<int, Season*>::type m_season_map;
		static map<int, SeasonReward*>::type m_season_reward_map;
		static map<int, SeasonRankReward*>::type m_season_rank_reward_map;

		static list<SeasonRankItem>::type m_season_rank_list;

	public:
		static void	loadSetting();
		static void	unloadSetting();

		static Season* findSeasonByHonorId(SeasonHonorId honorId);
		static SeasonReward* findSeasonRewardById(int id);
		static SeasonRankReward* findSeasonRankRewardBySeasonIdAndRank(int seasonId, int rank);

		static void parseSeasonRuleData(const String & data);
		static void parseSeasonRankData(int type, const String & data);
		static list<SeasonRankItem>::type findSeasonRankItemByType(SeasonRankType type);

	private:
		static void loadSeasonSetting();
		static void loadSeasonRewardSetting();
		static void loadSeasonRankRewardSetting();

		static bool sortRankFunc(SeasonRankItem rank1, SeasonRankItem rank2);
		static list<SeasonRankItem>::type sortRankItems(list<SeasonRankItem>::type ranks);
	};
}

#endif