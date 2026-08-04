#pragma once
#include <map>
#include "Core.h"
using namespace LORD;

namespace BLOCKMAN
{
	class ActionStateSetting
	{
	public:
		static bool						loadSetting();
		static void						unloadSetting();
		static String					getSkillName(int state, int itemId, int occupation = 0);

	private:
		static i64 getIndex(int state, int itemId, int occupation = 0);
		static std::map<i64, String>		     m_stateToSkill;
	};
}