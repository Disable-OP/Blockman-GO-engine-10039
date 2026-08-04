#pragma once
#include <map>
#include "Core.h"

using namespace LORD;
namespace BLOCKMAN
{
	class  BirdItem {
	public:
		int id = 0;
		String icon = "";
		String name = "";
	};

	class BirdItemSetting
	{
	public:
		static bool loadSetting();
		static void unloadSetting();

	public:
		static  BirdItem* findBirdItemById(int id);

	private:
		static std::map<int, BirdItem*> m_birdItemMap;
	};
}
