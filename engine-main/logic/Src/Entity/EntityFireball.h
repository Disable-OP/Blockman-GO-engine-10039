#pragma once
#ifndef __ENTITY_FIREBALL_HEADER__
#define __ENTITY_FIREBALL_HEADER__

#include "Entity.h"
#include "EntityThrowable.h"
#include "World/RayTracyResult.h"

namespace BLOCKMAN
{

	class World;
	class EntityLivingBase;

	class EntityFireball : public EntityThrowable
	{
		RTTI_DECLARE(EntityFireball);
	public:
		EntityFireball(World* pWorld);
		EntityFireball(World* pWorld, const Vector3& pos);
		EntityFireball(World* pWorld, EntityLivingBase* pShooting);

	protected:
		void onImpact(const RayTraceResult& result) override;
		float getGravityVelocity() { return 0.06F; }
	};
}
#endif