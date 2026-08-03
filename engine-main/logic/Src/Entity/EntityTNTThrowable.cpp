#include "EntityTNTThrowable.h"
#include "World/World.h"
#include "Script/Event/LogicScriptEvents.h"
#include "Entity/EntityPlayer.h"

namespace BLOCKMAN
{
	EntityTNTThrowable::EntityTNTThrowable(World* pWorld)
		: EntityThrowable(pWorld)
	{

	}

	EntityTNTThrowable::EntityTNTThrowable(World* pWorld, const Vector3& pos)
		: EntityThrowable(pWorld, pos)
	{

	}

	EntityTNTThrowable::EntityTNTThrowable(World* pWorld, EntityLivingBase* pShooting)
		: EntityThrowable(pWorld, pShooting)
	{

	}

	void EntityTNTThrowable::onImpact(const RayTraceResult& result)
	{
		if (!world->m_isClient)
		{
			SCRIPT_EVENT::EntityHitEvent::invoke(ITEM_ID_TNT, result.entityHit ? result.hitVec : result.blockPos, m_throwerEntityId);
			explode();
		}
		this->setDead();
	}

	void EntityTNTThrowable::explode()
	{
		Vector3 v = Vector3(position.x, position.y + this->height / 16.0f, position.z);
		world->createExplosion(this, getThrower(), v, 4.0f, true);
	}
}


