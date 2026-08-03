#include "CarSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"

namespace BLOCKMAN
{
	std::map<int, CarSetting*> CarSetting::m_carSettingMap = {};
	bool CarSetting::m_sbIsCarFree = false;

	bool CarSetting::loadSetting(bool bIsClient)
	{
		CarSetting* pSetting = NULL;
		m_carSettingMap.clear();
		CsvReader* pReader = new CsvReader();

		std::string path = PathUtil::ConcatPath(Root::Instance()->getRootPath(), "Media/Setting/car.csv").c_str();
		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordLogError("Can not open car setting file %s", path.c_str());
			return false;
		}

		pReader->Read(); // discard the second line
		while (pReader->Read())
		{
			pSetting = new CarSetting();
			pReader->GetInt("CarId", pSetting->carId);
			pReader->GetInt("MoneyType", pSetting->moneyType);
			pReader->GetInt("Price", pSetting->price);
			pReader->GetInt("SpeedSound", pSetting->speedSound);
			pReader->GetInt("NormalSound", pSetting->normalSound);
			pReader->GetInt("ExtraSound", pSetting->extraSound);
			pReader->GetInt("Passengers", pSetting->passengers);
			pReader->GetFloat("PassengersDamage", pSetting->passengersDamage);

			pReader->GetFloat("Slope", pSetting->slope);
			pReader->GetFloat("TurnSpeed", pSetting->turnSpeed);
			pReader->GetFloat("Traction", pSetting->traction);
			pReader->GetFloat("MaxSpeed", pSetting->maxSpeed);
			pReader->GetFloat("Weight", pSetting->weight);

			pReader->GetString("Name", pSetting->name);
			pReader->GetString("Model", pSetting->model);
			pReader->GetString("Texture", pSetting->texture);
			m_carSettingMap[pSetting->carId] = pSetting;
		}

		pReader->Close();
		LordSafeDelete(pReader);
		return true;
	}

	void CarSetting::unloadSetting()
	{
		for (auto iter = m_carSettingMap.begin(); iter != m_carSettingMap.end(); iter++)
		{
			CarSetting* pSetting = iter->second;
			delete pSetting;
		}

		m_carSettingMap.clear();
	}

	CarSetting * CarSetting::getCarSetting(int carId)
	{
		auto iter = m_carSettingMap.find(carId);
		if (iter != m_carSettingMap.end())
		{
			return iter->second;
		}

		return nullptr;
	}
}
