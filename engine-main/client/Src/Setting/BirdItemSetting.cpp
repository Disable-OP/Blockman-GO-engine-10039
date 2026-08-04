#include "BirdItemSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"

namespace BLOCKMAN
{
	std::map<int, BirdItem*>  BirdItemSetting::m_birdItemMap = {};

	bool BirdItemSetting::loadSetting()
	{
		BirdItem* pBirdItem = NULL;
		m_birdItemMap.clear();
		CsvReader* pReader = new CsvReader();
		std::string path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "dynamic/BirdItems.csv").c_str();

		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordLogInfo("Can not open BirdItem  file %s", path.c_str());
			return false;
		}

		pReader->Read(); // discard the second line

		while (pReader->Read())
		{
			pBirdItem = new BirdItem();
			pReader->GetInt("Id", pBirdItem->id);
			pReader->GetString("Icon", pBirdItem->icon);
			pReader->GetString("Name", pBirdItem->name);
			m_birdItemMap[pBirdItem->id] = pBirdItem;
		}

		pReader->Close();
		LordSafeDelete(pReader);
		return true;
	}

	void  BirdItemSetting::unloadSetting()
	{
		for (auto iter = m_birdItemMap.begin(); iter != m_birdItemMap.end(); iter++)
		{
			if (iter->second)
			{
				delete iter->second;
				iter->second = nullptr;
			}
		}

		m_birdItemMap.clear();
	}

	BirdItem * BirdItemSetting::findBirdItemById(int id)
	{
		auto item = m_birdItemMap.find(id);
		if (item != m_birdItemMap.end())
		{
			return item->second;
		}
		return nullptr;
	}

}


