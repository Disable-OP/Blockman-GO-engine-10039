#include "GunExtraAttrSetting.h"

namespace BLOCKMAN
{

	std::map<ui64, GunExtraAttrMap*> GunExtraAttrSetting::m_GunExtraAttrMaps = {};

	void GunExtraAttrMap::clearSetting()
	{
		for (auto iter = m_GunExtraAttrMap.begin(); iter != m_GunExtraAttrMap.end(); ++iter)
		{
			delete iter->second;
		}
		m_GunExtraAttrMap.clear();
	}

	GunExtraAttr* GunExtraAttrMap::getGunExtraAttr(int gunId)
	{
		auto iter = m_GunExtraAttrMap.find(gunId);
		if (iter != m_GunExtraAttrMap.end())
		{
			return iter->second;
		}
		else
		{
			GunExtraAttr* attr = new GunExtraAttr();
			m_GunExtraAttrMap.insert({ gunId, attr });
			return attr;
		}
	}

	void GunExtraAttrMap::addGunDamage(int gunId, float damage)
	{
		auto attr = getGunExtraAttr(gunId);
		attr->add_damage += damage;
	}

	void GunExtraAttrMap::addGunBulletNum(int gunId, int bullet_num)
	{
		auto attr = getGunExtraAttr(gunId);
		attr->add_bullet_num += bullet_num;
	}

	void GunExtraAttrMap::subGunRecoil(int gunId, float recoil)
	{
		auto attr = getGunExtraAttr(gunId);
		attr->sub_recoil += recoil;
	}

	void GunExtraAttrMap::subGunFireCd(int gunId, float fire_cd)
	{
		auto attr = getGunExtraAttr(gunId);
		attr->sub_fire_cd += fire_cd;
	}

	void GunExtraAttrSetting::unloadSetting()
	{
		for (auto iter = m_GunExtraAttrMaps.begin(); iter != m_GunExtraAttrMaps.end(); ++iter)
		{
			iter->second->clearSetting();
			delete iter->second;
		}
		m_GunExtraAttrMaps.clear();
	}

	GunExtraAttrMap* GunExtraAttrSetting::getGunExtraAttrMap(ui64 userId)
	{
		auto iter = m_GunExtraAttrMaps.find(userId);
		if (iter != m_GunExtraAttrMaps.end())
		{
			return iter->second;
		}
		else
		{
			GunExtraAttrMap* map = new GunExtraAttrMap();
			m_GunExtraAttrMaps.insert({ userId, map });
			return map;
		}
	}

	void GunExtraAttrSetting::clearGunExtraAttrMap(ui64 userId)
	{
		auto iter = m_GunExtraAttrMaps.find(userId);
		if (iter != m_GunExtraAttrMaps.end())
		{
			iter->second->clearSetting();
			delete iter->second;
			m_GunExtraAttrMaps.erase(iter);
		}
	}
}