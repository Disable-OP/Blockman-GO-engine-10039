#pragma once
#include <map>
#include "Core.h"

using namespace LORD;
namespace BLOCKMAN
{

	class HouseItem {
	public:
		int id = 0;
		String name = "";
		String desc = "";
		String icon = "";
		
		float length = 0;
		float width = 0;
		float height = 0;
		String templateName = "";
	};

	class HouseSetting
	{
	private:
		static bool loadHouseItem(bool bIsClient);

	public:
		static bool loadSetting(bool bIsClient);
		static void unloadSetting();
	public:
		static HouseItem* getHouseItem(int houseItemId);

	private:
		static std::map<int, HouseItem*> m_houseItemMap;
	};
}
