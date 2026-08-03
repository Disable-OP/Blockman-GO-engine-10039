#include "Core.h"
#include "EntityFireball.h"
#include "World/World.h"
#include "Entity/DamageSource.h"
#include "Script/Event/LogicScriptEvents.h"

namespace BLOCKMAN
{
	EntityFireball::EntityFireball(World * pWorld)
		:EntityThrowable(pWorld)
	{
	}

	EntityFireball::EntityFireball(World * pWorld, const Vector3 & pos)
		: EntityThrowable(pWorld, pos)
	{
	}

	EntityFireball::EntityFireball(World * pWorld, EntityLivingBase * pShooting)
		: EntityThrowable(pWorld, pShooting)
	{
	}

	void EntityFireball::onImpact(const RayTraceResult& result)
	{
		SCRIPT_EVENT::EntityHitEvent::invoke(ITEM_ID_FIRE_BALL, result.entityHit ? result.hitVec : result.blockPos, m_throwerEntityId);
		if (result.entityHit)
		{
			DamageSource* pSource = NULL;
			Entity* thrower = (Entity*)getThrower();
			if (!thrower)
				pSource = DamageSource::causeThrownDamage(this, this);
			else
				pSource = DamageSource::causeThrownDamage(this, thrower);
			result.entityHit->attackEntityFrom(pSource, 0.5f);
		}
		world->setEntityState(this, 3);
		setDead();
	}
}
