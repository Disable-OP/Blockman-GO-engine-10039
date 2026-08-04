#include "SeasonSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"
#include "rapidjson/document.h"

namespace BLOCKMAN
{
	map<int, Season*>::type SeasonSetting::m_season_map = {};
	map<int, SeasonReward*>::type SeasonSetting::m_season_reward_map = {};
	map<int, SeasonRankReward*>::type SeasonSetting::m_season_rank_reward_map = {};
	
	list<SeasonRankItem>::type SeasonSetting::m_season_rank_list = {};

	void SeasonSetting::loadSetting()
	{
		loadSeasonSetting();
		loadSeasonRewardSetting();
		loadSeasonRankRewardSetting();
	}

	void SeasonSetting::unloadSetting()
	{
		for (auto iter : m_season_map)
		{
			delete iter.second;
		}
		m_season_map.clear();

		for (auto iter : m_season_reward_map)
		{
			delete iter.second;
		}
		m_season_reward_map.clear();

		for (auto iter : m_season_rank_reward_map)
		{
			delete iter.second;
		}
		m_season_rank_reward_map.clear();

		m_season_rank_list.clear();
	}

	void SeasonSetting::loadSeasonSetting()
	{
		Season* pSeason = NULL;
		CsvReader* pReader = new CsvReader();
		std::string path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting", "SeasonSetting.csv").c_str();
		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordSafeDelete(pReader);
			return;
		}
		pReader->Read();
		pReader->Read();
		while (pReader->Read())
		{
			pSeason = new Season();
			pReader->GetInt("Id", pSeason->Id);
			int HonorId = 0;
			pReader->GetInt("HonorId", HonorId);
			pSeason->HonorId = (SeasonHonorId)HonorId;
			pReader->GetString("Name", pSeason->Name);
			pReader->GetString("HonorName", pSeason->HonorName);
			pReader->GetString("HonorIcon", pSeason->HonorIcon);
			pReader->GetString("HonorFlag", pSeason->HonorFlag);
			pReader->GetInt("DefaultScore", pSeason->Score);
			String UpRewardIdsStr;
			pReader->GetString("UpRewardIds", UpRewardIdsStr);
			StringArray UpRewardIds = StringUtil::Split(UpRewardIdsStr, "#");
			for (String RewardId : UpRewardIds)
			{
				pSeason->UpRewardIds.push_back(StringUtil::ParseI32(RewardId));
			}
			String NormalRewardIdsStr;
			pReader->GetString("NormalRewardIds", NormalRewardIdsStr);
			StringArray NormalRewardIds = StringUtil::Split(NormalRewardIdsStr, "#");
			for (String RewardId : NormalRewardIds)
			{
				pSeason->NormalRewardIds.push_back(StringUtil::ParseI32(RewardId));
			}
			m_season_map[pSeason->Id] = pSeason;
		}
		LordSafeDelete(pReader);
	}

	void SeasonSetting::loadSeasonRewardSetting()
	{
		SeasonReward* pSeasonReward = NULL;
		CsvReader* pReader = new CsvReader();
		std::string path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting", "SeasonRewardSetting.csv").c_str();
		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordSafeDelete(pReader);
			return;
		}
		pReader->Read();
		pReader->Read();
		while (pReader->Read())
		{
			pSeasonReward = new SeasonReward();
			pReader->GetInt("Id", pSeasonReward->Id);
			pReader->GetString("Icon", pSeasonReward->Icon);
			int Type = 0;
			pReader->GetInt("Type", Type);
			pSeasonReward->Type = (SeasonRewardType)Type;
			pReader->GetInt("Count", pSeasonReward->Count);
			m_season_reward_map[pSeasonReward->Id] = pSeasonReward;
		}
		LordSafeDelete(pReader);
	}

	void SeasonSetting::loadSeasonRankRewardSetting()
	{
		SeasonRankReward* pSeasonRankReward = NULL;
		CsvReader* pReader = new CsvReader();
		std::string path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting", "SeasonRankRewardSetting.csv").c_str();
		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordSafeDelete(pReader);
			return;
		}
		pReader->Read();
		pReader->Read();
		while (pReader->Read())
		{
			pSeasonRankReward = new SeasonRankReward();
			pReader->GetInt("Id", pSeasonRankReward->Id);
			pReader->GetInt("SeasonId", pSeasonRankReward->SeasonId);
			pReader->GetString("Icon", pSeasonRankReward->Icon);
			String RankRangeStr;
			pReader->GetString("RankRange", RankRangeStr);
			StringArray RankRange = StringUtil::Split(RankRangeStr, "#");
			if (RankRange.size() == 1)
			{
				pSeasonRankReward->MinRank = StringUtil::ParseI32(RankRange[0]);
				pSeasonRankReward->MaxRank = pSeasonRankReward->MinRank;
			}
			if (RankRange.size() == 2)
			{
				pSeasonRankReward->MinRank = StringUtil::ParseI32(RankRange[0]);
				pSeasonRankReward->MaxRank = StringUtil::ParseI32(RankRange[1]);
			}
			int Type = 0;
			pReader->GetInt("Type", Type);
			pSeasonRankReward->Type = (SeasonRewardType)Type;
			pReader->GetInt("Count", pSeasonRankReward->Count);
			m_season_rank_reward_map[pSeasonRankReward->Id] = pSeasonRankReward;
		}
		LordSafeDelete(pReader);
	}

	bool SeasonSetting::sortRankFunc(SeasonRankItem rank1, SeasonRankItem rank2)
	{
		if (rank1.Honor != rank2.Honor)
		{
			return rank1.Honor > rank2.Honor;
		}
		return rank1.Rank < rank2.Rank;
	}

	list<SeasonRankItem>::type SeasonSetting::sortRankItems(list<SeasonRankItem>::type ranks)
	{
		ranks.sort(sortRankFunc);
		/*int rank = 0;
		for (auto rank_item : ranks)
		{
			rank++;
			rank_item.Rank = rank;
		}*/
		return ranks;
	}

	Season * SeasonSetting::findSeasonByHonorId(SeasonHonorId honorId)
	{
		for (auto iter : m_season_map)
		{
			if (iter.second->HonorId == honorId)
			{
				return iter.second;
			}
		}
		return nullptr;
	}

	SeasonReward * SeasonSetting::findSeasonRewardById(int id)
	{
		for (auto iter : m_season_reward_map)
		{
			if (iter.second->Id == id)
			{
				return iter.second;
			}
		}
		return nullptr;
	}

	SeasonRankReward * SeasonSetting::findSeasonRankRewardBySeasonIdAndRank(int seasonId, int rank)
	{
		for (auto iter : m_season_rank_reward_map)
		{
			if (iter.second->SeasonId == seasonId && rank >= iter.second->MinRank && rank <= iter.second->MaxRank)
			{
				return iter.second;
			}
		}
		return nullptr;
	}

	void SeasonSetting::parseSeasonRuleData(const String & data)
	{
		if (data.length() == 0)
			return;
		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(data.c_str());
		if (doc->HasParseError())
		{
			LordDelete(doc);
			return;
		}
		auto score_data = doc->GetObject();
	
		for (auto score_item = score_data.MemberBegin(); score_item != score_data.MemberEnd(); ++score_item)
		{
			int type = StringUtil::ParseI32(score_item->name.GetString());
			int score = score_data.FindMember(score_item->name)->value.GetInt();
			Season* season = findSeasonByHonorId((SeasonHonorId)type);
			if (season)
			{
				season->Score = score;
			}
		}
		LordDelete(doc);
	}

	void SeasonSetting::parseSeasonRankData(int type, const String & data)
	{
		if (data.length() == 0)
			return;
		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(data.c_str());
		if (doc->HasParseError())
		{
			LordDelete(doc);
			return;
		}
		if (!doc->IsArray())
		{
			LordDelete(doc);
			return;
		}
		auto items = doc->GetArray();
		for (size_t i = 0; i < items.Size(); ++i)
		{
			auto obj = items[i].GetObject();
			int rank = obj.FindMember("rank")->value.GetInt();
			String nickName = obj.FindMember("nickName")->value.GetString();
			int integral = obj.FindMember("integral")->value.GetInt();
			m_season_rank_list.push_back(SeasonRankItem(rank, nickName, integral, (SeasonRankType)type));
		}
		LordDelete(doc);
	}

	list<SeasonRankItem>::type SeasonSetting::findSeasonRankItemByType(SeasonRankType type)
	{
		list<SeasonRankItem>::type result;
		for (auto rank_item : m_season_rank_list)
		{
			if (rank_item.Type == type)
			{
				result.push_back(rank_item);
			}
		}
		return sortRankItems(result);
	}

}