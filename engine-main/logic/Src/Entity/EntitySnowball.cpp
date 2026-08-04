#include "Core.h"
#include "EntitySnowball.h"
#include "World/World.h"
#include "Entity/DamageSource.h"
#include "Script/Event/LogicScriptEvents.h"
#include "Entity/EntityPlayer.h"
namespace BLOCKMAN
{
	EntitySnowball::EntitySnowball(World * pWorld)
		:EntityThrowable(pWorld)
	{
	}

	EntitySnowball::EntitySnowball(World * pWorld, const Vector3 & pos)
		: EntityThrowable(pWorld, pos)
	{
	}

	EntitySnowball::EntitySnowball(World * pWorld, EntityLivingBase * pShooting)
		: EntityThrowable(pWorld, pShooting)
	{
	}

	void EntitySnowball::handleStatusUpdate(int id)
	{
		if (id == 3)
		{
			for (int i = 0; i < 8; ++i)
			{
				world->spawnParticle("snowball", position, Vector3(0,0,0));
			}
		}
	}

	void EntitySnowball::onImpact(const RayTraceResult& result)
	{
		SCRIPT_EVENT::EntityHitEvent::invoke(ITEM_ID_SNOW_BALL, result.entityHit ? result.hitVec : result.blockPos, m_throwerEntityId);

		if (result.entityHit)
		{
			
			int i = 0;

			/*if (dynamic_cast<EntityBlaze*>(result.entityHit))
			{
			i = 3;
			}*/

			DamageSource* pSource = NULL;

			if (!getThrower())
			{
				pSource = DamageSource::causeThrownDamage(this, this);
			}
			else
			{
				Entity* thrower = (Entity*)getThrower();

				pSource = DamageSource::causeThrownDamage(this, thrower);
			}

			
			result.entityHit->attackEntityFrom(pSource, (float)i);
			
		}

		for (int i = 0; i < 8; ++i)
		{
			world->spawnParticle("snowballpoof", position, Vector3::ZERO);
		}

		//if (!world->m_isClient)
		{
			world->setEntityState(this, 3);
			setDead();
		}
	}
}
