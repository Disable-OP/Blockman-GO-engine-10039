#include "ChestLotterySetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"
#include "rapidjson/document.h"

namespace BLOCKMAN
{
	map<int, LotteryChest*>::type ChestLotterySetting::m_lottery_chest_map = {};
	map<int, LotteryChestReward*>::type ChestLotterySetting::m_lottery_chest_reward_map = {};

	void ChestLotterySetting::loadSetting()
	{
		loadChestSetting();
		loadChestRewardSetting();
	}

	void ChestLotterySetting::unloadSetting()
	{
		for (auto iter : m_lottery_chest_map)
		{
			delete iter.second;
		}
		m_lottery_chest_map.clear();
		for (auto iter : m_lottery_chest_reward_map)
		{
			delete iter.second;
		}
		m_lottery_chest_reward_map.clear();
	}

	void ChestLotterySetting::loadChestSetting()
	{
		LotteryChest* pLotteryChest = NULL;
		CsvReader* pReader = new CsvReader();
		std::string path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting", "ChestSetting.csv").c_str();
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
			pLotteryChest = new LotteryChest();
			pReader->GetInt("Id", pLotteryChest->Id);
			int Type = 0;
			pReader->GetInt("Type", Type);
			pLotteryChest->Type = (LotteryChestType)Type;
			pReader->GetString("Name", pLotteryChest->Name);
			pReader->GetString("Actor", pLotteryChest->Actor);
			pReader->GetString("Image", pLotteryChest->Image);
			pReader->GetString("MatImage", pLotteryChest->MatImage);
			pReader->GetInt("NeedKey", pLotteryChest->NeedKey);
			pReader->GetInt("AddIntegral", pLotteryChest->AddIntegral);
			pReader->GetInt("NeedIntegral", pLotteryChest->NeedIntegral);
			m_lottery_chest_map[pLotteryChest->Id] = pLotteryChest;
		}
		LordSafeDelete(pReader);
	}

	void ChestLotterySetting::loadChestRewardSetting()
	{
		LotteryChestReward* pLotteryChestReward = NULL;
		CsvReader* pReader = new CsvReader();
		std::string path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting", "ChestRewardSetting.csv").c_str();
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
			pLotteryChestReward = new LotteryChestReward();
			pReader->GetInt("Id", pLotteryChestReward->Id);
			int Type = 0;
			pReader->GetInt("Type", Type);
			pLotteryChestReward->Type = (LotteryChestType)Type;
			pReader->GetString("Title", pLotteryChestReward->Title);
			pReader->GetString("Name", pLotteryChestReward->Name);
			pReader->GetString("Image", pLotteryChestReward->Image);
			pReader->GetString("Desc", pLotteryChestReward->Desc);
			pReader->GetInt("Count", pLotteryChestReward->Count);
			m_lottery_chest_reward_map[pLotteryChestReward->Id] = pLotteryChestReward;
		}
		LordSafeDelete(pReader);
	}

	LotteryChest * ChestLotterySetting::findLotteryChestById(int id)
	{
		for (auto iter : m_lottery_chest_map)
		{
			if (iter.second->Id == id)
			{
				return iter.second;
			}
		}
		return nullptr;
	}

	LotteryChest * ChestLotterySetting::findLotteryChestByType(LotteryChestType type)
	{
		for (auto iter : m_lottery_chest_map)
		{
			if (iter.second->Type == type)
			{
				return iter.second;
			}
		}
		return nullptr;
	}

	list<LotteryChestReward*>::type ChestLotterySetting::findLotteryChestRewardByType(LotteryChestType type)
	{
		list<LotteryChestReward*>::type result;
		for (auto iter : m_lottery_chest_reward_map)
		{
			if (iter.second->Type == type)
			{
				result.push_back(iter.second);
			}
		}
		return result;
	}

	LotteryChestReward * ChestLotterySetting::findLotteryChestRewardById(int id)
	{
		for (auto iter : m_lottery_chest_reward_map)
		{
			if (iter.second->Id == id)
			{
				return iter.second;
			}
		}
		return nullptr;
	}

	void ChestLotterySetting::parseUpdateLotteryChestData(const String & data)
	{
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
			if (!obj.HasMember("Id"))
				continue;
			int Id = obj.FindMember("Id")->value.GetInt();
			LotteryChest* chest = findLotteryChestById(Id);
			if (!chest)
				continue;
			int HasNum = obj.HasMember("HasNum") ? obj.FindMember("HasNum")->value.GetInt() : chest->HasNum;
			chest->HasNum = HasNum;
			chest->HasChange = true;
		}
		LordDelete(doc);
	}
};