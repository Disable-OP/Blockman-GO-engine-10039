#include "BirdFruitCurrencySetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"

namespace BLOCKMAN
{
	std::map<int, BirdFruitCurrency*>  BirdFruitCurrencySetting::m_birdFruitCurrencyMap = {};

	bool BirdFruitCurrencySetting::loadSetting()
	{
		BirdFruitCurrency* pBirdFruitCurrency = NULL;
		m_birdFruitCurrencyMap.clear();
		CsvReader* pReader = new CsvReader();
		std::string path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "dynamic/BirdFruitCurrencys.csv").c_str();

		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordLogInfo("Can not open BirdFruitCurrency  file %s", path.c_str());
			return false;
		}

		pReader->Read(); // discard the second line

		while (pReader->Read())
		{
			pBirdFruitCurrency = new BirdFruitCurrency();
			pReader->GetInt("ItemId", pBirdFruitCurrency->itemId);
			pReader->GetString("Icon", pBirdFruitCurrency->icon);
			pReader->GetString("Name", pBirdFruitCurrency->name);
			pReader->GetString("MeshName", pBirdFruitCurrency->meshName);
			m_birdFruitCurrencyMap[pBirdFruitCurrency->itemId] = pBirdFruitCurrency;
		}

		pReader->Close();
		LordSafeDelete(pReader);
		return true;
	}

	void  BirdFruitCurrencySetting::unloadSetting()
	{
		for (auto iter = m_birdFruitCurrencyMap.begin(); iter != m_birdFruitCurrencyMap.end(); iter++)
		{
			if (iter->second)
			{
				delete iter->second;
				iter->second = nullptr;
			}
		}

		m_birdFruitCurrencyMap.clear();
	}

	BirdFruitCurrency * BirdFruitCurrencySetting::findBirdFruitCurrencyById(int id)
	{
		auto item = m_birdFruitCurrencyMap.find(id);
		if (item != m_birdFruitCurrencyMap.end())
		{
			return item->second;
		}
		return nullptr;
	}

}


