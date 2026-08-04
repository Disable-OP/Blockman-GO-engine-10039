#pragma once
#include "Entity.h"
#include "World/RayTracyResult.h"

namespace BLOCKMAN
{
	class World;
	class EntityLivingBase;

	class EntityFishHook : public Entity, public IProjectile
	{
		RTTI_DECLARE(EntityFishHook);

	public:
		Matrix4 m_thirdWorldMat = Matrix4::IDENTITY;

	protected:
		bool m_inGround = false;
		bool m_inWater = false;
		int m_anglerEntityId = 0;
		int m_targetEntityId = 0;
		int m_inTile = 0;
		int m_inData = 0;
		BlockPos m_blockPos;

	public:
		EntityFishHook(World* pWorld, EntityLivingBase* pAngler);
		~EntityFishHook() { m_inGround = false; }

		virtual void setHeadingFromThrower(Entity* entityThrower, float rotationPitchIn, float rotationYawIn, float pitchOffset, float velocity, float inaccuracy);
		virtual void setThrowableHeading(const Vector3& pos, float velocity, float inaccuracy);
		virtual void setVelocity(const Vector3& vel);
		virtual void onUpdate();
		virtual void writeEntityToNBT(NBTTagCompound* pNbtArrow);
		virtual void readEntityFromNBT(NBTTagCompound* pNbtArrow);
		virtual bool isInRangeToRenderDist(float distanceSqr) const override;

		Entity* getAngler();
		Entity* getTarget();
		void catchFish();

	protected:
		float getGravityVelocity() { return 0.06f; }
	};
}