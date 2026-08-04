#pragma once
#include <map>
#include "Core.h"

using namespace LORD;
namespace BLOCKMAN
{
	class Building {
	public:
		int actorId = 0;
		int itemId = 0;
		int type = 0;
		String name = "";
		String icon = "";
		String lockIcon = "";
		String actorName = "";
		String actorBody = "";
		String actorBodyId = "";
		String awaitAnimate = "";
		String finishAnimate = "";
		String manufactureAnimate = "";
		float length = 0;
		float width = 0;
		float height = 0;
	};

	class BuildingSetting
	{
	public:
		static bool loadSetting(bool bIsClient);
		static void unloadSetting();

	public:
		static Building* getBuildingByItemId(int itemId);
		static Building* getBuildingByActorId(int actorId);
		static String getLockIcon(int actorId);

	private:
		static std::map<int, Building*> m_buildingMap;
	};
}
