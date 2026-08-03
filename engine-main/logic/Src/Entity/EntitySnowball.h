#pragma once
#ifndef __ENTITY_SNOWBALL_HEADER__
#define __ENTITY_SNOWBALL_HEADER__

#include "Entity.h"
#include "EntityThrowable.h"
#include "World/RayTracyResult.h"

namespace BLOCKMAN
{

	class World;
	class EntityLivingBase;

	class EntitySnowball : public EntityThrowable
	{
		RTTI_DECLARE(EntitySnowball);
	public:
		EntitySnowball(World* pWorld);
		EntitySnowball(World* pWorld, const Vector3& pos);
		EntitySnowball(World* pWorld, EntityLivingBase* pShooting);
		EntitySnowball(World* pWorld, EntityLivingBase* pShooting, int meta);
		void handleStatusUpdate(int id);
		void setMeta(int meta) { m_meta = meta; }
		int getMeta() { return m_meta; }
	protected:
		void onImpact(const RayTraceResult& result) override;
		float getGravityVelocity() { return 0.03F; }
	private:
		int m_meta = 0;
	};
}
#endif