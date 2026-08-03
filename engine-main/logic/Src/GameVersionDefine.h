#ifndef __GAME_VERSION_DEFINE_H__
#define __GAME_VERSION_DEFINE_H__

#include <string>
#include "g3log/loglevels.hpp"

namespace BLOCKMAN
{
	class EngineVersionSetting
	{
	public:
		static bool loadSetting();
		static int getEngineVersion() { return m_engineVersion; }
		static bool canUseCmd() { return m_useCmd; }
		static LEVELS getLogLevel() { return m_logLevel; };

	private:
		static int m_engineVersion;
		static bool m_useCmd;
		static LEVELS m_logLevel;
	};
}

#endif // __GAME_VERSION_DEFINE_H__
