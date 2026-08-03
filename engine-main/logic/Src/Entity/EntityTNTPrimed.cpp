#include "EntityTNTPrimed.h"
#include "Math/MathLib.h"
#include "World/World.h"
#include "Util/Random.h"

namespace BLOCKMAN
{
	EntityTNTPrimed::EntityTNTPrimed(World * world)
		: Entity(world)
	{
		this->fuse = 80;
		this->preventEntitySpawning = true;
		this->m_isImmuneToFire = true;
		this->setSize(0.98F, 0.98F);
	}

	EntityTNTPrimed::EntityTNTPrimed(World * world, const Vector3 & pos, EntityLivingBase * pShooting)
		: EntityTNTPrimed(world)
	{
		this->setPosition(pos);
		float f =(float) rand->nextGaussian() * (Math::PI * 2.0f);
		motion.x = (-(sin(f)) * 0.02F);
		motion.y = 0.2f;
		motion.z = -((float)cos(f)) * 0.02F;
		this->fuse = 80;
		prevPos.x = pos.x;
		prevPos.y = pos.y;
		prevPos.z = pos.z;
		if(pShooting != NULL) 
			m_tntPlacedByEntityId = pShooting->entityId;
	}
	void EntityTNTPrimed::onUpdate()
	{
		Entity::onUpdate();
		prevPos = position;

		//if (!this.hasNoGravity())
		{
			motion.y -= getGravityVelocity();
		}

		this->moveEntity(motion);
		motion *= 0.98f;
		if (this->onGround)
		{
			motion.x *= 0.70f;
			motion.z *= 0.70f;
			motion.y *= -0.5f;
		}

		--this->fuse;
		if (this->fuse <= 0)
		{
			this->setDead();
			if (!world->m_isClient)
			{
				explode();
			}
		}
		else
		{
			handleWaterMovement();
			world->spawnParticle("smoke", Vector3(position.x, position.y + 0.5f , position.z), Vector3(0, 0, 0));
		}
	}

	EntityLivingBase * EntityTNTPrimed::getTntPlacedBy()
	{
		EntityLivingBase* pPlacedBy = NULL;
		if (m_tntPlacedByEntityId > 0)
		{
			pPlacedBy = dynamic_cast<EntityLivingBase*>(world->getEntity(m_tntPlacedByEntityId));
		}
		return pPlacedBy;
	}

	void EntityTNTPrimed::readEntityFromNBT(NBTTagCompound * pNBT)
	{
		pNBT->setShort("fuse", fuse);
	}

	void EntityTNTPrimed::writeEntityToNBT(NBTTagCompound * pNBT)
	{
		fuse = pNBT->getByte("fuse") & 255;
	}

	void EntityTNTPrimed::explode()
	{
		Vector3 v = Vector3(position.x, position.y + this->height / 16.0f, position.z);
		world->createExplosion(this, getTntPlacedBy(), v, 4.0f, true);
	}

}
