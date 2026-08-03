#ifndef __ENTITY_GRENADE_H__
#define __ENTITY_GRENADE_H__

#include "EntityThrowable.h"

namespace BLOCKMAN
{
	class World;
	class EntityLivingBase;

	class EntityGrenade : public EntityThrowable 
	{
		RTTI_DECLARE(EntityGrenade);
	private:
		int m_itemId = 0;
	public:
		EntityGrenade(World* pWorld);
		EntityGrenade(World* pWorld, const Vector3& pos);
		EntityGrenade(World* pWorld, EntityLivingBase* pShooting, int itemId);

		int getItemId() { return m_itemId; }

	protected:
		virtual float getGravityVelocity() override { return 0.03F; }
		virtual void onImpact(const RayTraceResult& result);

	private:
		void explode();
	};
}

#endif // __ENTITY_GRENADE_H__
