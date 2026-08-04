#include "CropsSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"

namespace BLOCKMAN
{
	std::map<int, CropsSetting*> CropsSetting::m_cropsSettingMap = {};

	bool CropsSetting::loadSetting(bool isClient)
	{
		CropsSetting* pSetting = NULL;
		m_cropsSettingMap.clear();
		CsvReader* pReader = new CsvReader();
		
		std::string path = PathUtil::ConcatPath(Root::Instance()->getRootPath(), "Media/Setting/crops.csv").c_str();
		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordLogError("Can not open crops setting file %s", path.c_str());
			return false;
		}

		String quantity;
		String colName;
		pReader->Read(); // discard the second line
		while (pReader->Read())
		{
			pSetting = new CropsSetting();
			pReader->GetInt("CropsId", pSetting->cropsId);
			pReader->GetInt("SeedId", pSetting->seedId);
			pReader->GetInt("RecoverTime", pSetting->recoverTime);
			pReader->GetInt("ProductionId", pSetting->productionId);
			pReader->GetInt("CanHarvestNum", pSetting->canHarvestNum);
			pReader->GetString("Quantity", quantity);
			parseQuantity(quantity, pSetting->minQuantity, pSetting->maxQuantity);
			pReader->GetString("StealQuantity", quantity);
			parseQuantity(quantity, pSetting->minStealQuantity, pSetting->maxStealQuantity);

			for (int i = 1; i <= CROPS_TOTAL_STAGE; i++)
			{
				colName = StringUtil::Format("StageTime%d", i);
				pReader->GetInt(colName.c_str(), pSetting->stageTimeArray[i - 1]);
			}
			m_cropsSettingMap[pSetting->cropsId] = pSetting;
		}

		pReader->Close();
		LordSafeDelete(pReader);
		return true;
	}

	void CropsSetting::unloadSetting()
	{
		for (auto iter = m_cropsSettingMap.begin(); iter != m_cropsSettingMap.end(); iter++)
		{
			delete iter->second;
		}

		m_cropsSettingMap.clear();
	}

	CropsSetting * CropsSetting::getCropsSetting(int cropsId)
	{
		auto iter = m_cropsSettingMap.find(cropsId);
		if (iter != m_cropsSettingMap.end())
			return iter->second;

		return NULL;
	}

	int CropsSetting::getStateTime(int stage)
	{
		if (stage < 1 || stage > CROPS_TOTAL_STAGE)
			return 0;

		return stageTimeArray[stage - 1];
	}

	i32 CropsSetting::getTotalTime()
	{
		i32 totalTime = 0;
		for (i32 time : stageTimeArray)
		{
			totalTime += time;
		}
		return totalTime;
	}

	void CropsSetting::parseQuantity(const String & str, int & min, int & max)
	{
		auto arr = StringUtil::Split(str, "#");
		if (arr.size() != 2)
			return;

		min = StringUtil::ParseInt(arr[0]);
		max = StringUtil::ParseInt(arr[1]);
	}

}