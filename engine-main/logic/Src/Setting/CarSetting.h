#pragma once
#include <map>
#include "Core.h"

using namespace LORD;
namespace BLOCKMAN
{
	class CarSetting
	{
	public:
		static bool loadSetting(bool bIsClient);
		static void unloadSetting();
		static CarSetting* getCarSetting(int carId);

		static bool m_sbIsCarFree;

		int					carId;
		int					moneyType;
		int					price;
		int					speedSound;
		int					normalSound;
		int					extraSound;
		int					passengers;
		float               passengersDamage;

		float				slope;
		float				turnSpeed;
		float				traction;
		float				maxSpeed;
		float				weight;

		String				name;
		String				model;
		String				texture;
	private:
		static std::map<int, CarSetting*> m_carSettingMap;
	};
}
