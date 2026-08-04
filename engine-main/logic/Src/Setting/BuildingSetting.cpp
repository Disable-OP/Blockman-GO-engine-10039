#include "BuildingSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"

namespace BLOCKMAN
{
	std::map<int, Building*>  BuildingSetting::m_buildingMap = {};


	bool BuildingSetting::loadSetting(bool bIsClient)
	{
		Building* pBuilding = NULL;
		m_buildingMap.clear();
		CsvReader* pReader = new CsvReader();

		std::string path = PathUtil::ConcatPath(Root::Instance()->getRootPath(), "Media/Setting/RanchBuilding.csv").c_str();
		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordLogError("Can not open  Building  file %s", path.c_str());
			return false;
		}

		pReader->Read(); // discard the second line
		
		while (pReader->Read())
		{
			pBuilding = new Building();
			pReader->GetInt("ActorId", pBuilding->actorId);
			pReader->GetInt("ItemId", pBuilding->itemId);
			pReader->GetInt("Type", pBuilding->type);
			pReader->GetString("Name", pBuilding->name);
			pReader->GetString("ActorName", pBuilding->actorName);
			pReader->GetString("ActorBody", pBuilding->actorBody);
			pReader->GetString("ActorBodyId", pBuilding->actorBodyId);
			pReader->GetString("AwaitAnimate", pBuilding->awaitAnimate);
			pReader->GetString("FinishAnimate", pBuilding->finishAnimate);
			pReader->GetString("ManufactureAnimate", pBuilding->manufactureAnimate);
			pReader->GetFloat("Length", pBuilding->length);
			pReader->GetFloat("Width", pBuilding->width);
			pReader->GetFloat("Height", pBuilding->height);
			pReader->GetString("Icon", pBuilding->icon);
			String lockIcon = "";
			pReader->GetString("LockIcon", lockIcon);
			pBuilding->lockIcon = StringUtil::Format("set:ranch_items.json image:%s", lockIcon.c_str());
			m_buildingMap[pBuilding->actorId] = pBuilding;
		}

		pReader->Close();
		LordSafeDelete(pReader);
		return true;
	}

	void  BuildingSetting::unloadSetting()
	{
		for (auto iter = m_buildingMap.begin(); iter != m_buildingMap.end(); iter++)
		{
			if (iter->second)
			{
				delete iter->second;
				iter->second = nullptr;

			}
		}

		m_buildingMap.clear();
	}

	Building *  BuildingSetting::getBuildingByItemId(int  itemId)
	{
		for (auto item : m_buildingMap)
		{
			if (item.second->itemId == itemId)
			{
				return item.second;
			}
		}
		return nullptr;
	}

	Building * BuildingSetting::getBuildingByActorId(int actorId)
	{
		auto item = m_buildingMap.find(actorId);
		if (item != m_buildingMap.end())
		{
			return item->second;
		}
		return nullptr;
	}

	String BuildingSetting::getLockIcon(int actorId)
	{
		auto item = getBuildingByActorId(actorId);
		return item ? item->lockIcon : "";
	}
}


