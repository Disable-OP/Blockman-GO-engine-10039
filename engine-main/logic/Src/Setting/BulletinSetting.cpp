#include "BulletinSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"

namespace BLOCKMAN
{
	std::map<int, Bulletin*>  BulletinSetting::m_bulletinMap = {};


	bool BulletinSetting::loadSetting(bool bIsClient)
	{
		Bulletin* pBulletin = NULL;
		m_bulletinMap.clear();
		CsvReader* pReader = new CsvReader();

		std::string path = "";
		if (bIsClient) {
			path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "dynamic/Bulletin.csv").c_str();
		} else {
			path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), "dynamic/Bulletin.csv").c_str();
		}
		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordLogInfo("Can not open  Bulletin  file %s", path.c_str());
			return false;
		}

		pReader->Read(); // discard the second line
		
		while (pReader->Read())
		{
			pBulletin = new Bulletin();
			pReader->GetInt("Id", pBulletin->id);
			pReader->GetString("Name", pBulletin->name);
			pReader->GetString("ActorName", pBulletin->actorName);
			pReader->GetString("ActorBody", pBulletin->actorBody);
			pReader->GetString("ActorBodyId", pBulletin->actorBodyId);
			pReader->GetString("AwaitAnimate", pBulletin->awaitAnimate);
			pReader->GetFloat("Length", pBulletin->length);
			pReader->GetFloat("Width", pBulletin->width);
			pReader->GetFloat("Height", pBulletin->height);
			m_bulletinMap[pBulletin->id] = pBulletin;
		}

		pReader->Close();
		LordSafeDelete(pReader);
		return true;
	}

	void  BulletinSetting::unloadSetting()
	{
		for (auto iter = m_bulletinMap.begin(); iter != m_bulletinMap.end(); iter++)
		{
			if (iter->second)
			{
				delete iter->second;
				iter->second = nullptr;

			}
		}

		m_bulletinMap.clear();
	}

	Bulletin * BulletinSetting::getBulletinById(int id)
	{
		auto item = m_bulletinMap.find(id);
		if (item != m_bulletinMap.end())
		{
			return item->second;
		}
		return nullptr;
	}

}


