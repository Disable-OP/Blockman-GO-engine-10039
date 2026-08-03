#pragma once
#include <map>
#include "Core.h"

using namespace LORD;
namespace BLOCKMAN
{
	class Bulletin {
	public:
		int id = 0;
		String name = "";
		String actorName = "";
		String actorBody = "";
		String actorBodyId = "";
		String awaitAnimate = "";
		float length = 0;
		float width = 0;
		float height = 0;
	};

	class BulletinSetting
	{
	public:
		static bool loadSetting(bool bIsClient);
		static void unloadSetting();

	public:
		static Bulletin* getBulletinById(int id);

	private:
		static std::map<int, Bulletin*> m_bulletinMap;
	};
}
