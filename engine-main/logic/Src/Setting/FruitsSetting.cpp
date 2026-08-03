#include "FruitsSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"

namespace BLOCKMAN
{
	std::map<int, Fruit*>  FruitsSetting::m_fruitMap = {};

	bool FruitsSetting::loadSetting(bool isClient)
	{
		Fruit* pFruit = NULL;
		m_fruitMap.clear();
		CsvReader* pReader = new CsvReader();

		 
		std::string path = "";
		if (isClient) {
			path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "dynamic/FruitsBlock.csv").c_str();
		} else {
			path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), "dynamic/FruitsBlock.csv").c_str();
		}
		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordLogInfo("Can not open Fruit  file %s", path.c_str());
			return false;
		}

		pReader->Read(); // discard the second line

		while (pReader->Read())
		{
			pFruit = new Fruit();
			pReader->GetInt("Score", pFruit->score);
			pReader->GetInt("BlockId", pFruit->blockId);
			pReader->GetInt("MaxStage", pFruit->maxStage);
			pReader->GetInt("ResetTime", pFruit->resetTime);
			pReader->GetInt("RecoveryTime", pFruit->recoveryTime);
			m_fruitMap[pFruit->blockId] = pFruit;
		}

		pReader->Close();
		LordSafeDelete(pReader);
		return true;
	}

	void  FruitsSetting::unloadSetting()
	{
		for (auto iter = m_fruitMap.begin(); iter != m_fruitMap.end(); iter++)
		{
			if (iter->second)
			{
				delete iter->second;
				iter->second = nullptr;
			}
		}

		m_fruitMap.clear();
	}

	Fruit * FruitsSetting::findFruitById(int id)
	{
		auto item = m_fruitMap.find(id);
		if (item != m_fruitMap.end())
		{
			return item->second;
		}
		return nullptr;
	}

}


