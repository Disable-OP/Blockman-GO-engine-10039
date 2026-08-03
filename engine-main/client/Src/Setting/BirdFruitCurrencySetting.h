#pragma once
#include <map>
#include "Core.h"

using namespace LORD;
namespace BLOCKMAN
{
	class BirdFruitCurrency {
	public:
		int itemId = 0;
		String icon = "";
		String name = "";
		String meshName = "";
	};

	class BirdFruitCurrencySetting
	{
	public:
		static bool loadSetting();
		static void unloadSetting();

	public:
		static  BirdFruitCurrency* findBirdFruitCurrencyById(int id);

	private:
		static std::map<int, BirdFruitCurrency*> m_birdFruitCurrencyMap;
	};
}
