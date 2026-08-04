#include "GatherToolSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"

namespace BLOCKMAN
{
	GatherTool*  GatherToolSetting::m_lastFindTool = nullptr;
	std::map<int, GatherTool*>  GatherToolSetting::m_gatherToolMap = {};

	bool GatherToolSetting::loadSetting(bool bIsClient)
	{
		GatherTool* pGatherTool = NULL;
		m_gatherToolMap.clear();
		CsvReader* pReader = new CsvReader();

		std::string path = "";
		if (bIsClient) {
			path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "dynamic/GatherTool.csv").c_str();
		} else {
			path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), "dynamic/GatherTool.csv").c_str();
		}
		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordLogInfo("Can not open  GatherTool  file %s", path.c_str());
			return false;
		}

		pReader->Read(); // discard the second line
		float cd = 0.f;
		while (pReader->Read())
		{
			pGatherTool = new GatherTool();
			pReader->GetInt("Id", pGatherTool->id);
			pReader->GetInt("Num", pGatherTool->num);
			pReader->GetFloat("CD", cd);
			pGatherTool->cd = (int) (cd * 1000);
			m_gatherToolMap[pGatherTool->id] = pGatherTool;
		}

		pReader->Close();
		LordSafeDelete(pReader);
		return true;
	}

	void  GatherToolSetting::unloadSetting()
	{
		for (auto iter = m_gatherToolMap.begin(); iter != m_gatherToolMap.end(); iter++)
		{
			if (iter->second)
			{
				delete iter->second;
				iter->second = nullptr;

			}
		}

		m_gatherToolMap.clear();
		m_lastFindTool = nullptr;
	}

	GatherTool * GatherToolSetting::findToolById(int id)
	{
		auto item = m_gatherToolMap.find(id);
		if (item != m_gatherToolMap.end())
		{
			return item->second;
		}
		return m_gatherToolMap[0];
	}

	i32 GatherToolSetting::getToolCdTime(int id)
	{
		if (m_lastFindTool && m_lastFindTool->id == id)
		{
			return m_lastFindTool->cd;
		}

		m_lastFindTool = findToolById(id);
		if (m_lastFindTool)
		{
			return m_lastFindTool->cd;
		}
		return 1000;
	}

}


