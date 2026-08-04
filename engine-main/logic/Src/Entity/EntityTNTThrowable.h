#ifndef ENTITY_TNT_THROWABLE
#define ENTITY_TNT_THROWABLE

#include "EntityThrowable.h"
namespace BLOCKMAN
{
	class World;
	class EntityLivingBase;

	class EntityTNTThrowable : public EntityThrowable
	{
		RTTI_DECLARE(EntityTNTThrowable);

	public:
		EntityTNTThrowable(World* pWorld);
		EntityTNTThrowable(World* pWorld, const Vector3& pos);
		EntityTNTThrowable(World* pWorld, EntityLivingBase* pShooting);

	protected:
		virtual float getGravityVelocity() override { return 0.03F; }
		virtual void onImpact(const RayTraceResult& result);

	private:
		void explode();
	};
}

#endif // ENTITY_TNT_THROWABLE
