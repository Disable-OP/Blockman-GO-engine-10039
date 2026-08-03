#include "EntityCreatureBullet.h"
#include "EntityPlayer.h"
#include "EntityCreature.h"
#include "Script/Event/LogicScriptEvents.h"
#include "Object/Root.h"

namespace BLOCKMAN
{
	EntityCreatureBullet::EntityCreatureBullet(World * pWorld, EntityLivingBase * thrower, EntityLivingBase * targeter, CreatureBulletType type)
		: EntityCreatureBullet(pWorld, type)
	{
		throwerId = thrower->hashCode();
		targeterId = targeter->hashCode();
		Vector3 pos = thrower->getPosition();
		pos.y = pos.y + 1.0f;
		setPosition(pos);
		setSize(0.25F, 0.25F);
		initBombEffect();
		currUpdateTime = Root::Instance()->getCurrentTime();
		lastUpdateTime = currUpdateTime;
	}

	EntityCreatureBullet::~EntityCreatureBullet()
	{
		if (bombEffect != NULL)
		{
			bombEffect->mDuration = 0;
			bombEffect = NULL;
		}
	}

	String EntityCreatureBullet::getBombEffectName()
	{
		if (!world->m_isClient)
			return "";
		String effectName = "";
		switch (getType())
		{
		case CreatureBulletType::TowerBomb:
			effectName = "tower_bomb.effect";
			break;
		default:
			break;
		}
		return effectName;
	}

	void EntityCreatureBullet::initBombEffect()
	{
		String effectName = getBombEffectName();
		if (effectName.length() <= 7)
			return;
		bombEffect = WorldEffectManager::Instance()->addSimpleEffect(effectName, position, rotationYaw, -1);
	}

	void EntityCreatureBullet::onUpdate()
	{
		currUpdateTime = Root::Instance()->getCurrentTime();
		lastTickPos = position;
		Entity::onUpdate();

		if (throwerId == 0 || targeterId == 0)
		{
			setDead();
			return;
		}
		EntityLivingBase* targeter = dynamic_cast<EntityLivingBase*>(world->getEntity(targeterId));
		if (!targeter || targeter->isDead)
		{
			setDead();
			return;
		}

		Vector3 begin = position;
		Vector3 end = targeter->getPosition();
		if (targeter->isClass(ENTITY_CLASS_PLAYER))
			end.y += 0.2f;
		else
			end.y += 1.0f;
		float distance = begin.distanceTo(end);
		float temp = 0.5f * ((currUpdateTime - lastUpdateTime) / 20.0f);
		motion.x = temp * (end.x - begin.x) / distance;
		motion.y = temp * (end.y - begin.y) / distance;
		motion.z = temp * (end.z - begin.z) / distance;
		end = position + motion;
		Entity* tracyEntity = nullptr;
		EntityArr entities = world->getEntitiesWithinAABBExcludingEntity(this, boundingBox.addCoord(motion.x, motion.y, motion.z).expland(1.f, 1.f, 1.f));

		for (size_t i = 0; i < entities.size(); i++)
		{
			Entity* entity = entities[i];
			if (entity == targeter && entity->canBePushed())
			{
				tracyEntity = entity;
			}
		}

		if (tracyEntity)
		{
			if (!world->m_isClient)
			{
				if (targeter->isClass(ENTITY_CLASS_PLAYER))
					SCRIPT_EVENT::CreatureAttackPlayerEvent::invoke(targeterId, throwerId);
				else
					SCRIPT_EVENT::CreatureAttackCreatureEvent::invoke(targeterId, throwerId);
			}
			setDead();
		}

		position += motion;

		float f = sqrt(motion.x * motion.x + motion.z * motion.z);
		rotationYaw = (float)atan2(motion.x, motion.z) * (180.0f / Math::PI);
		for (rotationPitch = atan2(motion.y, f) * (180.0f / Math::PI); rotationPitch - prevRotationPitch < -180.0F; prevRotationPitch -= 360.0F)
		{
			;
		}

		while (rotationPitch - prevRotationPitch >= 180.0F)
		{
			prevRotationPitch += 360.0F;
		}

		while (rotationYaw - prevRotationYaw < -180.0F)
		{
			prevRotationYaw -= 360.0F;
		}

		while (rotationYaw - prevRotationYaw >= 180.0F)
		{
			prevRotationYaw += 360.0F;
		}
		rotationPitch = prevRotationPitch + (rotationPitch - prevRotationPitch) * 0.2F;
		rotationYaw = prevRotationYaw + (rotationYaw - prevRotationYaw) * 0.2F;

		setPosition(position);
		onEffectUpdate();

		lastUpdateTime = currUpdateTime;
	}

	void EntityCreatureBullet::onEffectUpdate()
	{
		if (bombEffect == NULL)
			return;
		bombEffect->mPosition = position;
		bombEffect->mQuaternion = Quaternion(Vector3::UNIT_Y, -rotationYaw * Math::DEG2RAD);
	}

}