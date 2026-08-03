#pragma once
#ifndef __ENTITY_TNT_PRIMED_HEADER__
#define __ENTITY_TNT_PRIMED_HEADER__

#include "Entity.h"

namespace BLOCKMAN
{
	class World;
	class EntityLivingBase;

	class EntityTNTPrimed : public Entity
	{
		RTTI_DECLARE(EntityTNTPrimed);
	private:
		int fuse = 0;
		int m_tntPlacedByEntityId = 0;
	public:
		EntityTNTPrimed(World * world);
		EntityTNTPrimed(World* pWorld, const Vector3& pos, EntityLivingBase* pShooting);

		void onUpdate();
		EntityLivingBase* getTntPlacedBy();
		void setFuse(int fuse) { this->fuse = fuse; }
		int getFuse() { return this->fuse; }
		virtual float getGravityVelocity() { return 0.04F; }
		virtual bool canTriggerWalking() { return false; }
		virtual bool canBeCollidedWith() { return !this->isDead; }
		void readEntityFromNBT(NBTTagCompound* pNBT);
		void writeEntityToNBT(NBTTagCompound* pNBT) ;
	private:
		void explode();
	};
}

#endif