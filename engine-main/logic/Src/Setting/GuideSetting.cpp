#include "GuideSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"

namespace BLOCKMAN
{
	std::map<int, GuideVideoItem*>  GuideSetting::m_guideVideoMap = {};

	void  GuideSetting::unloadSetting()
	{
		for (auto iter = m_guideVideoMap.begin(); iter != m_guideVideoMap.end(); iter++)
		{
			GuideVideoItem* pGuideSetting = iter->second;
			delete pGuideSetting;
		}

		m_guideVideoMap.clear();
	}

	bool  GuideSetting::loadSetting()
	{
		unloadSetting();
		GuideVideoItem* pGuideSetting = NULL;
		m_guideVideoMap.clear();
		CsvReader* pReader = new CsvReader();

		std::string path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "dynamic/guideVideo.csv").c_str();
		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordLogError("Can not open  House Items file %s", path.c_str());
			return false;
		}

		pReader->Read(); // discard the second line
		while (pReader->Read())
		{
			pGuideSetting = new GuideVideoItem();
			pReader->GetInt("ID", pGuideSetting->id);
			pReader->GetString("Path", pGuideSetting->path);
			pReader->GetInt("Type", pGuideSetting->type);
			m_guideVideoMap[pGuideSetting->id] = pGuideSetting;
		}

		pReader->Close();
		LordSafeDelete(pReader);
		return true;
	}

	GuideVideoItem *  GuideSetting::getGuideVideoItem(int  videoId)
	{
		auto iter = m_guideVideoMap.find(videoId);
		if (iter != m_guideVideoMap.end())
		{
			return iter->second;
		}
		return nullptr;
	}

	int GuideSetting::getGuideVideoType(int videoId)
	{
		auto guide = getGuideVideoItem(videoId);
		if (guide)
		{
			return guide->type;
		}
		return 0;
	}

	String GuideSetting::getGuideVideoPath(int videoId)
	{
		const auto guide = getGuideVideoItem(videoId);
		if (guide)
		{
			if (guide->type == 1)
			{
				String path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), guide->path);
				return path;
			}
			else
			{
				return guide->path;
			}
		}
		return "";
	}
}


