#pragma once
#include <map>
#include "Core.h"

using namespace LORD;
namespace BLOCKMAN
{
	class GatherTool {
	public:
		int id = 0;
		int cd = 0;
		int num = 1;
		
	};

	class GatherToolSetting
	{
	public:
		static bool loadSetting(bool bIsClient);
		static void unloadSetting();

	public:
		static GatherTool* findToolById(int id);
		static i32 getToolCdTime(int id);

	private:
		static GatherTool* m_lastFindTool;
		static std::map<int, GatherTool*> m_gatherToolMap;
	};
}
