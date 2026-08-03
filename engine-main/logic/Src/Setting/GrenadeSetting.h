#ifndef __GRENADE_SETTING_HEADER__
#define __GRENADE_SETTING_HEADER__

#include <map>
#include "Core.h"

using namespace LORD;

namespace BLOCKMAN
{
	class SGrenade
	{
	public:
		int itemId = 425;
		String itemIcon = "grenade";

		bool isExplosion = true;
		float explosionPower = 4.0f;
		bool isExplosionSmoke = true;
		
		bool isExplosionEffect = false;
		String effectName = "";
		int effectTime = 0;
	};

	class GrenadeSetting
	{
	public:
		static bool	loadSetting(bool isClient);
		static void	unloadSetting();
		static SGrenade* getGrenadeSetting(int itemId);

	private:
		static std::map<int, SGrenade*> m_GrenadeMap;
	};
}

#endif // !__ACTOR_SIZE_SETTING_HEADER__
