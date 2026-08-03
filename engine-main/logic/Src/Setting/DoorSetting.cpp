#include "DoorSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"

namespace BLOCKMAN
{
	std::map<int, DoorSetting*> DoorSetting::m_doorSettingMap = {};

	bool DoorSetting::loadSetting(bool bIsClient)
	{
		DoorSetting* pSetting = NULL;
		m_doorSettingMap.clear();
		CsvReader* pReader = new CsvReader();

		std::string path = PathUtil::ConcatPath(Root::Instance()->getRootPath(), "Media/Setting/door.csv").c_str();
		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordLogError("Can not open door setting file %s", path.c_str());
			return false;
		}

		pReader->Read(); // discard the second line
		while (pReader->Read())
		{
			pSetting = new DoorSetting();
			pReader->GetInt("DoorId", pSetting->doorId);
			pReader->GetInt("Camp", pSetting->teamId);
			pReader->GetInt("KeyId", pSetting->keyId);
			pReader->GetInt("CloseTime", pSetting->closeTime);

			m_doorSettingMap[pSetting->doorId] = pSetting;
		}

		pReader->Close();
		LordSafeDelete(pReader);
		return true;
	}

	void DoorSetting::unloadSetting()
	{
		for (auto iter = m_doorSettingMap.begin(); iter != m_doorSettingMap.end(); iter++)
		{
			DoorSetting* pSetting = iter->second;
			delete pSetting;
		}

		m_doorSettingMap.clear();
	}

	DoorSetting * DoorSetting::getDoorSetting(int doorId)
	{
		auto iter = m_doorSettingMap.find(doorId);
		if (iter != m_doorSettingMap.end())
		{
			return iter->second;
		}

		return nullptr;
	}
}


