#pragma once
#include <map>
#include "Core.h"
#define CROPS_TOTAL_STAGE 4

using namespace LORD;
namespace BLOCKMAN
{
	class CropsSetting
	{
	public:
		static bool loadSetting(bool isClient);
		static void unloadSetting();
		static CropsSetting* getCropsSetting(int cropsId);
		int    getStateTime(int stage);
		i32   getTotalTime();

		int cropsId = 0;
		int seedId = 0;
		int recoverTime = 0;
		int minQuantity = 0;
		int maxQuantity = 0;
		int minStealQuantity = 0;
		int maxStealQuantity = 0;
		int productionId = 0;
		int canHarvestNum = 1;

	private:
		static void parseQuantity(const String& str, int& min, int& max);
		int stageTimeArray[CROPS_TOTAL_STAGE] = { 0 };
		static std::map<int, CropsSetting*> m_cropsSettingMap;
	};
}