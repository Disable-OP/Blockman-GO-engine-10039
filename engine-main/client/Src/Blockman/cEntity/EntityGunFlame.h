#pragma once

#include "Entity/Entity.h"

namespace BLOCKMAN
{
	class SimpleEffect;
	class World;
	class EntityLivingBase;
	class GunSetting;

	class EntityGunFlame : public Entity
	{
		RTTI_DECLARE(EntityGunFlame);

	public:
		SimpleEffect* m_flameEffect = nullptr;
		String m_flameEffectName = "";
		EntityLivingBase* m_pShooting = nullptr;
		bool m_isGunFlameEffectInit = false;
		GunSetting* m_gunSetting = nullptr;
		float m_scale = 1.0f;
		bool m_isFirstPerspective = false;

	protected:
		int m_existenceTime = 0;
		int m_currTime = 0;
		int m_beginTime = 0;

	public:
		EntityGunFlame(World* pWorld, EntityLivingBase* pShooting, GunSetting* pSetting, int existenceTime = 200);
		~EntityGunFlame();

		virtual void onUpdate() override;
		virtual void writeEntityToNBT(NBTTagCompound* pNbtbullet) override {}
		virtual void readEntityFromNBT(NBTTagCompound* pNbtbullet) override {}

	protected:
		Vector3 calcPosition(float& scale);
	};
}