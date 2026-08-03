#pragma once
#include <map>
#include "Core.h"

using namespace LORD;
namespace BLOCKMAN
{
	class  Fruit {
	public:
		int score = 0;
		int blockId = 0;
		int maxStage = 0;
		int resetTime = 0;
		int recoveryTime = 0;
	};

	class FruitsSetting
	{
	public:
		static bool loadSetting(bool isClient);
		static void unloadSetting();

	public:
		static  Fruit* findFruitById(int id);

	private:
		static std::map<int, Fruit*> m_fruitMap;
	};
}
