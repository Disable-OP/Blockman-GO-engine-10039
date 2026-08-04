#include "ActorSizeSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"

namespace BLOCKMAN
{

	std::map<String, ActorSize*>  ActorSizeSetting::m_ActorSizeMap = {};

	bool ActorSizeSetting::loadSetting(bool isClient)
	{
		m_ActorSizeMap.clear();
		m_ActorSizeMap.insert({ "DefaultSize", new ActorSize(0.6f, 0.6f, 1.8f) });

		ActorSize* pActorSize = NULL;
		CsvReader* pReader = new CsvReader();

		std::string path = PathUtil::ConcatPath(Root::Instance()->getRootPath(), "Media/Setting/ActorSize.csv").c_str();
		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordLogError("Can not open ActorSize setting file %s", path.c_str());
			return false;
		}
		pReader->Read(); // discard the second line
		while (pReader->Read())
		{
			String actorName = "";
			pActorSize = new ActorSize();
			pReader->GetString("ActorName", actorName);
			pReader->GetFloat("Width", pActorSize->width);
			pReader->GetFloat("Length", pActorSize->length);
			pReader->GetFloat("Height", pActorSize->height);
			m_ActorSizeMap[actorName] = pActorSize;
		}

		pReader->Close();
		LordSafeDelete(pReader);
		return true;
	}

	bool ActorSizeSetting::loadMapSetting(bool isClient)
	{
		ActorSize* pActorSize = NULL;
		CsvReader* pReader = new CsvReader();

		std::string path;
		if (isClient)
		{
			path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting", "ActorSize.csv").c_str();
		}
		else
		{
			path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), "setting", "ActorSize.csv").c_str();
		}
		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			return false;
		}
		pReader->Read(); // discard the second line
		while (pReader->Read())
		{
			String actorName = "";
			pActorSize = new ActorSize();
			pReader->GetString("ActorName", actorName);
			pReader->GetFloat("Width", pActorSize->width);
			pReader->GetFloat("Length", pActorSize->length);
			pReader->GetFloat("Height", pActorSize->height);
			m_ActorSizeMap[actorName] = pActorSize;
		}

		pReader->Close();
		LordSafeDelete(pReader);
		return true;
	}

	void ActorSizeSetting::unloadSetting()
	{
		for (auto iter = m_ActorSizeMap.begin(); iter != m_ActorSizeMap.end(); iter++)
		{
			delete iter->second;
		}
		m_ActorSizeMap.clear();
	}

	ActorSize * ActorSizeSetting::getActorSize(String actorName)
	{
		auto iter = m_ActorSizeMap.find(actorName);
		if (iter != m_ActorSizeMap.end())
		{
			return iter->second;
		}
		else
		{
			if (actorName == "DefaultSize")
			{
				ActorSize* pSize = new ActorSize(0.6f, 0.6f, 1.8f);
				m_ActorSizeMap.insert({ actorName, pSize });
				return pSize;
			}
			else
			{
				return getActorSize("DefaultSize");
			}
		}	
	}

}