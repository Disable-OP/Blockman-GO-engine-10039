#pragma once
#include <map>
#include "Core.h"

using namespace LORD;
namespace BLOCKMAN
{
	class DoorSetting
	{
	public:
		static bool loadSetting(bool bIsClient);
		static void unloadSetting();
		static DoorSetting* getDoorSetting(int doorId);

		int				doorId;
		int				teamId;
		int				keyId;
		int			    closeTime;

	private:
		static std::map<int, DoorSetting*> m_doorSettingMap;
	};
}
