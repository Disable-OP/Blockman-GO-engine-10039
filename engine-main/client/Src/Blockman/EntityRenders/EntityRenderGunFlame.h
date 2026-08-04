#pragma once

#include "EntityRender.h"

namespace BLOCKMAN
{
	class EntityLivingBase;
	class GunSetting;

	class EntityRenderGunFlame : public EntityRender
	{
	protected:
		LineRenderable* m_line = nullptr;

	public:
		EntityRenderGunFlame();
		~EntityRenderGunFlame();

		/** implement override functions for EntityRender */
		virtual void doRender(Entity* pEntity, const Vector3& pos, float yaw, float rdt) override;

	protected:
		Vector3 calcPosition(EntityLivingBase* shooting, GunSetting* gunSetting, float rdt, float& scale);
	};

}