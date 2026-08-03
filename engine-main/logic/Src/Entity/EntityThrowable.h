#ifndef __ENTITY_THROWABLE_HEADER__
#define __ENTITY_THROWABLE_HEADER__

#include "Entity.h"
#include "World/RayTracyResult.h"

namespace BLOCKMAN
{

	class World;
	class EntityLivingBase;

	class EntityThrowable : public Entity, public IProjectile
	{
		RTTI_DECLARE(EntityThrowable);

	protected:
		bool inGround = false;
		BlockPos m_blockPos;
		int inTile = 0;
		int inData = 0;
		int m_throwerEntityId = 0;

	public:
		int throwableShake = 0;
		int m_ignoreEntityId = 0;

	private:
		int ignoreTime = 0;
		int ticksInGround = 0;
		int ticksInAir = 0;
		String throwerName;

	protected:
		void init_entityThrowable();
		virtual float getGravityVelocity() { return 0.03F; }
		virtual void onImpact(const RayTraceResult& result) = 0;

		
	public:
		EntityThrowable(World* pWorld);
		EntityThrowable(World* pWorld, const Vector3& pos);
		EntityThrowable(World* pWorld, EntityLivingBase* pShooting);
		~EntityThrowable() { inGround = false; }

		virtual void setHeadingFromThrower(Entity* entityThrower, float rotationPitchIn, float rotationYawIn, float pitchOffset, float velocity, float inaccuracy);
		virtual void setThrowableHeading(const Vector3& pos, float velocity, float inaccuracy);
		virtual void setVelocity(const Vector3& vel);
		virtual void onUpdate();
		
		virtual void writeEntityToNBT(NBTTagCompound* pNbtArrow);
		virtual void readEntityFromNBT(NBTTagCompound* pNbtArrow);
		virtual EntityLivingBase* getThrower();

		virtual bool isInRangeToRenderDist(float distanceSqr) const override;

	};
}

#endif