#include "EntityGrenade.h"
#include "World/World.h"
#include "World/WorldEffectManager.h"
#include "Setting/GrenadeSetting.h"
#include "Script/Event/LogicScriptEvents.h"
#include "Entity/EntityPlayer.h"

namespace BLOCKMAN
{
	EntityGrenade::EntityGrenade(World* pWorld)
		: EntityThrowable(pWorld)
	{

	}

	EntityGrenade::EntityGrenade(World* pWorld, const Vector3& pos)
		: EntityThrowable(pWorld, pos)
	{

	}

	EntityGrenade::EntityGrenade(World* pWorld, EntityLivingBase* pShooting, int itemId)
		: EntityThrowable(pWorld, pShooting)
	{
		m_itemId = itemId;
	}

	void EntityGrenade::onImpact(const RayTraceResult& result)
	{
		if (!world->m_isClient)
		{
			SCRIPT_EVENT::EntityHitEvent::invoke(ITEM_ID_GRENADE, result.entityHit ? result.hitVec : result.blockPos, m_throwerEntityId);
			explode();
		}
		else
		{
			SGrenade* grenade = GrenadeSetting::getGrenadeSetting(m_itemId);
			if (grenade->isExplosionEffect && grenade->effectTime > 0)
			{
				WorldEffectManager::Instance()->addSimpleEffect(grenade->effectName, position, rotationYaw, grenade->effectTime);
			}
		}
		this->setDead();
	}

	void EntityGrenade::explode()
	{
		SGrenade* grenade = GrenadeSetting::getGrenadeSetting(m_itemId);
		if (grenade->isExplosion)
		{
			Vector3 pos = Vector3(position.x, position.y + this->height / 16.0f, position.z);
			world->createExplosion(this, getThrower(), pos, grenade->explosionPower, grenade->isExplosionSmoke);
		}
	}
}


