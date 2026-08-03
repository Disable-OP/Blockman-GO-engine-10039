#include "EntitySkillThrowable.h"
#include "EntityPlayer.h"
#include "Script/Event/LogicScriptEvents.h"

namespace BLOCKMAN
{
	EntitySkillThrowable::EntitySkillThrowable(World * pWorld)
		: EntityThrowable(pWorld)
	{
	}

	EntitySkillThrowable::EntitySkillThrowable(World * pWorld, const Vector3 & pos)
		: EntityThrowable(pWorld, pos)
	{
	}

	EntitySkillThrowable::EntitySkillThrowable(World * pWorld, EntityLivingBase * pShooting)
		: EntityThrowable(pWorld, pShooting)
	{
		this->m_throwerId = pShooting->entityId;
		this->m_startPos = position;
	}

	EntitySkillThrowable::~EntitySkillThrowable()
	{
		if (m_bombEffect != NULL)
		{
			m_bombEffect->mDuration = 0;
			m_bombEffect = NULL;
		}
	}

	void EntitySkillThrowable::onUpdate()
	{
		if (isParabola())
		{
			EntityThrowable::onUpdate();
		}
		else
		{
			lastTickPos = position;
			Entity::onUpdate();
			Entity * thrower = world->getEntity(m_throwerId);
			if (!thrower)
			{
				setDead();
				return;
			}
			Vector3 begin = position;
			Vector3 end = position + motion;
			RayTraceResult trace = world->rayTraceBlocks(begin, end, false, true);
			begin = position;
			end = position + motion;
			float distance = this->m_startPos.distanceTo(end);
			if (trace.result)
			{
				end = trace.hitVec;
			}

			Entity* tracyEntity = NULL;
			EntityArr entities = world->getEntitiesWithinAABBExcludingEntity(this, boundingBox.addCoord(motion.x, motion.y, motion.z).expland(1.f, 1.f, 1.f));
			float min_dis = m_releaseDistance;
			float expendSize;

			for (int i = 0; i < int(entities.size()); ++i)
			{
				Entity* pEntity = entities[i];
				if (pEntity->canBePushed() && pEntity != thrower)
				{
					expendSize = 0.3F;
					Box entity_box = pEntity->boundingBox.expland(expendSize, expendSize, expendSize);
					RayTraceResult tracy_entity = RayTraceResult::calculateIntercept(entity_box, begin, end);

					if (tracy_entity.result)
					{
						float dis = begin.distanceTo(tracy_entity.hitVec);

						if (dis < min_dis)
						{
							tracyEntity = pEntity;
							min_dis = dis;
						}
					}
				}
			}
			if (trace.result || (distance >= m_releaseDistance && distance > 0) || tracyEntity)
			{
				if (tracyEntity)
					end = tracyEntity->position;

				if (!world->m_isClient)
				{
					Vector3 lastEnd = end;
					if (distance >= m_releaseDistance && distance > 0) {
						lastEnd.x = (end.x - this->m_startPos.x) * m_releaseDistance / distance + this->m_startPos.x;
						lastEnd.y = (end.y - this->m_startPos.y) * m_releaseDistance / distance + this->m_startPos.y;
						lastEnd.z = (end.z - this->m_startPos.z) * m_releaseDistance / distance + this->m_startPos.z;
					}
					SCRIPT_EVENT::ItemSkillAttackEvent::invoke(this->m_throwerId, this->m_skillItemId, lastEnd);
				}
				setDead();
				return;
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
			//float f1 = 0.99F;
			//float f2 = getGravityVelocity();
			float f1 = getMoveSpeed();
			float f2 = getDropSpeed();

			motion.x *= f1;
			motion.y *= f1;
			motion.z *= f1;

			motion.y -= f2;
			setPosition(position);
		}
		onEffectUpdate();
	}

	void EntitySkillThrowable::setMoveSpeed(float moveSpeed)
	{
		m_moveSpeed = moveSpeed;
		if (m_moveSpeed > 0)
		{
			motion *= m_moveSpeed;
		}
	}

	void EntitySkillThrowable::setDropSpeed(float dropSpeed)
	{
		m_dropSpeed = dropSpeed;
		m_gravity = m_dropSpeed > 0 ? m_dropSpeed / 20 : 0.03f;
	}

	void EntitySkillThrowable::setEffectName(String effectName)
	{
		m_effectName = effectName;
		if (m_effectName.length() <= 7)
			return;
		if (!world->m_isClient)
			return;
		m_bombEffect = WorldEffectManager::Instance()->addSimpleEffect(m_effectName, position, rotationYaw, -1);
	}

	void EntitySkillThrowable::onImpact(const RayTraceResult& result)
	{
		if (!world->m_isClient)
			SCRIPT_EVENT::ItemSkillAttackEvent::invoke(this->m_throwerId, this->m_skillItemId, result.hitVec);
		setDead();
	}

	void EntitySkillThrowable::onEffectUpdate()
	{
		if (m_bombEffect == NULL)
			return;
		m_bombEffect->mPosition = position;
		m_bombEffect->mQuaternion = Quaternion(Vector3::UNIT_Y, -rotationYaw * Math::DEG2RAD);
	}

}