#include "HouseSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"

namespace BLOCKMAN
{
	std::map<int, HouseItem*>  HouseSetting::m_houseItemMap = {};

	bool HouseSetting::loadSetting(bool bIsClient)
	{
		loadHouseItem(bIsClient);
		return true;
	}

	void  HouseSetting::unloadSetting()
	{
		for (auto iter = m_houseItemMap.begin(); iter != m_houseItemMap.end(); iter++)
		{
			HouseItem* pHouseItem = iter->second;
			delete pHouseItem;
		}

		m_houseItemMap.clear();
	}

	bool  HouseSetting::loadHouseItem(bool bIsClient)
	{
		HouseItem* pHouseItem = NULL;
		m_houseItemMap.clear();
		CsvReader* pReader = new CsvReader();

		std::string path = PathUtil::ConcatPath(Root::Instance()->getRootPath(), "Media/Setting/house.csv").c_str();
		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordLogError("Can not open  House Items file %s", path.c_str());
			return false;
		}

		pReader->Read(); // discard the second line
		while (pReader->Read())
		{
			pHouseItem = new HouseItem();
			pReader->GetInt("ItemId", pHouseItem-> id);
			pReader->GetString("Name", pHouseItem->name);
			pReader->GetString("Desc", pHouseItem->desc);
			pReader->GetString("Icon", pHouseItem->icon);
			pReader->GetFloat("Length", pHouseItem->length);
			pReader->GetFloat("Width", pHouseItem->width);
			pReader->GetFloat("Height", pHouseItem->height);
			pReader->GetString("Template", pHouseItem->templateName);
			m_houseItemMap[pHouseItem->id] = pHouseItem;
		}

		pReader->Close();
		LordSafeDelete(pReader);
		return true;
	}

	HouseItem *  HouseSetting::getHouseItem(int  houseItemId)
	{
		auto iter = m_houseItemMap.find(houseItemId);
		if (iter != m_houseItemMap.end())
		{
			return iter->second;
		}
		return nullptr;
	}
}


