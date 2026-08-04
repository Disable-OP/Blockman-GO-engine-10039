#pragma once
#include <map>
#include "Core.h"

using namespace LORD;
namespace BLOCKMAN
{

	class GuideVideoItem {
	public:
		int id = 0;
		int type = 1;
		String path = "";
	};

	class GuideSetting
	{

	public:
		static bool loadSetting();
		static void unloadSetting();
	public:
		static GuideVideoItem* getGuideVideoItem(int videoId);
		static int getGuideVideoType(int videoId);
		static String  getGuideVideoPath(int videoId);

	private:
		static std::map<int, GuideVideoItem*> m_guideVideoMap;
	};
}
