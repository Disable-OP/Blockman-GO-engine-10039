#ifndef __GUN_EXTRA_ATTR_SETTING_HEADER__
#define __GUN_EXTRA_ATTR_SETTING_HEADER__

#include <map>
#include "Core.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GunExtraAttr
	{
	public:
		float add_damage = 0.0f;
		int add_bullet_num = 0;
		float sub_recoil = 0.0f;
		float sub_fire_cd = 0.0f;
	};

	class GunExtraAttrMap
	{
	private:
		std::map<int, GunExtraAttr*> m_GunExtraAttrMap;
	public:
		void clearSetting();
		GunExtraAttr* getGunExtraAttr(int gunId);
		void addGunDamage(int gunId, float damage);
		void addGunBulletNum(int gunId, int bullet_num);
		void subGunRecoil(int gunId, float recoil);
		void subGunFireCd(int gunId, float fire_cd);
	};

	class GunExtraAttrSetting
	{
	public:
		static void	unloadSetting();
		static GunExtraAttrMap* getGunExtraAttrMap(ui64 userId);
		static void clearGunExtraAttrMap(ui64 userId);

	private:
		static std::map<ui64, GunExtraAttrMap*> m_GunExtraAttrMaps;
	};
}

#endif // !__GUN_EXTRA_ATTR_SETTING_HEADER__
