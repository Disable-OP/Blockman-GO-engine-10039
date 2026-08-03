#include "EntityThrowable.h"
#include "EntityLivingBase.h"
#include "Math/MathLib.h"
#include "Util/Random.h"
#include "World/World.h"
#include "BM_TypeDef.h"
#include "Block/BlockManager.h"
#include "Block/Block.h"


namespace BLOCKMAN
{

	void EntityThrowable::init_entityThrowable()
	{
		m_blockPos = BlockPos(-1, -1, -1);
		inTile = 0;
		inData = 0;
		inGround = false;
		ticksInGround = 0;
		ticksInAir = 0;
		throwableShake = 0;
		m_ignoreEntityId = 0;
		m_throwerEntityId = 0;
		ignoreTime = 0;
		throwerName = "";
	}

	EntityThrowable::EntityThrowable(World * pWorld)
		: Entity(pWorld)
	{
		init_entityThrowable();
		setSize(0.25F, 0.25F);
	}

	EntityThrowable::EntityThrowable(World * pWorld, const Vector3 & pos)
		: EntityThrowable(pWorld)
	{
		init_entityThrowable();
		setPosition(pos);
	}

	EntityThrowable::EntityThrowable(World * pWorld, EntityLivingBase * pShooting)
		: EntityThrowable(pWorld, pShooting->position)
	{
		m_throwerEntityId = pShooting->entityId;
		throwerName = pShooting->getEntityName();
	}

	bool EntityThrowable::isInRangeToRenderDist(float distanceSqr) const
	{
		float len = boundingBox.getAverageEdgeLength() * 4.0f;
		len *= 64.0f;

		return distanceSqr < len * len;
	}

	void EntityThrowable::setHeadingFromThrower(Entity * entityThrower, float rotationPitchIn, float rotationYawIn, float pitchOffset, float velocity, float inaccuracy)
	{
		float f = -sin(rotationYawIn * 0.017453292F) * cos(rotationPitchIn * 0.017453292F);
		float f1 = -sin((rotationPitchIn + pitchOffset) * 0.017453292F);
		float f2 = cos(rotationYawIn * 0.017453292F) * cos(rotationPitchIn * 0.017453292F);
		setThrowableHeading(Vector3(f, f1, f2), velocity, inaccuracy);
		motion.x += entityThrower->motion.x;
		motion.z += entityThrower->motion.z;
		if (!entityThrower->onGround)
		{
			motion.y += entityThrower->motion.y;
		}
	}

	void EntityThrowable::setThrowableHeading(const Vector3 & pos, float velocity, float inaccuracy)
	{
		float x = pos.x;
		float y = pos.y;
		float z = pos.z;
		float f = sqrt(x * x + y * y + z * z);
		if (f < 0.00001f) {
			f = 0.00001f;
		}
		x = x / f;
		y = y / f;
		z = z / f;
		x = (float)(x + rand->nextGaussian() * 0.0075f * inaccuracy);
		y = (float)(y + rand->nextGaussian() * 0.0075f *  inaccuracy);
		z = (float)(z + rand->nextGaussian() * 0.0075f * inaccuracy);
		x = x * velocity;
		y = y * velocity;
		z = z * velocity;
		motion.x = x;
		motion.y = y;
		motion.z = z;
		float f1 = sqrt(x * x + z * z);
		rotationYaw = (float)(atan2(x, z) * (180 / Math::PI));
		rotationPitch = (float)(atan2(y, f1) * (180 / Math::PI));
		prevRotationYaw = rotationYaw;
		prevRotationPitch = rotationPitch;
		ticksInGround = 0;
	}

	void EntityThrowable::setVelocity(const Vector3 & vel)
	{
		motion.x = vel.x;
		motion.y = vel.y;
		motion.z = vel.z;

		if (prevRotationPitch == 0.0F && prevRotationYaw == 0.0F)
		{
			float f = sqrt(vel.x * vel.x + vel.z * vel.z);
			rotationYaw = atan2(vel.x, vel.z) * (180 / Math::PI);
			rotationPitch = atan2(vel.y, f * (180 / Math::PI));
			prevRotationYaw = rotationYaw;
			prevRotationPitch = rotationPitch;
		}
	}

	void EntityThrowable::onUpdate()
	{
		lastTickPos = position;
		Entity::onUpdate();

		if (throwableShake > 0)
		{
			--throwableShake;
		}

		if (inGround)
		{
			int blockID = world->getBlockId(m_blockPos);
			int metadata = world->getBlockMeta(m_blockPos);
			if (blockID == inTile && metadata == inData)
			{
				++ticksInGround;

				if (ticksInGround == 1200)
				{
					setDead();
				}

				return;
			}
			inGround = false;
			motion.x *= rand->nextFloat() * 0.2F;
			motion.y *= rand->nextFloat() * 0.2F;
			motion.z *= rand->nextFloat() * 0.2F;
			ticksInGround = 0;
			ticksInAir = 0;
		}
		else
		{
			++ticksInAir;
		}

		Vector3 begin = position;
		Vector3 end = position + motion;
		RayTraceResult trace = world->rayTraceBlocks(begin, end, false, true);
		begin = position;
		end = position + motion;

		if (trace.result)
		{
			end = trace.hitVec;
		}

		Entity* tracyEntity = nullptr;
		EntityArr entities = world->getEntitiesWithinAABBExcludingEntity(this, boundingBox.addCoord(motion.x, motion.y, motion.z).expland(1.f, 1.f, 1.f));

		float d0 = 0.0f;
		bool flag = false;
		float expendSize;
		for (size_t i = 0; i < entities.size(); i++)
		{
			Entity* entity1 = entities[i];

			if (entity1->canBePushed())
			{
				if (entity1->entityId == m_ignoreEntityId)
				{
					flag = true;
				}
				else if (m_throwerEntityId != 0 && ticksExisted < 2 && m_ignoreEntityId == 0)
				{
					m_ignoreEntityId = entity1->entityId;
					flag = true;
				}
				else
				{
					flag = false;
					expendSize = 0.3F;
					Box entity_box = entity1->boundingBox.expland(expendSize, expendSize, expendSize);
					RayTraceResult tracy_entity = RayTraceResult::calculateIntercept(entity_box, begin, end);
					if (tracy_entity.result)
					{
						float dis = begin.distanceTo(tracy_entity.hitVec);

						if (dis < d0 || d0 == 0.0f)
						{
							tracyEntity = entity1;
							d0 = dis;
						}
					}
				}
			}
		}

		if (m_ignoreEntityId > 0)
		{
			if (flag)
			{
				ignoreTime = 2;
			}
			else if (ignoreTime-- <= 0)
			{
				m_ignoreEntityId = 0;
			}
		}

		if (tracyEntity)
		{
			trace.set(tracyEntity);
		}

		if (trace.result)
		{
			if (trace.typeOfHit == RAYTRACE_TYPE::RAYTRACE_TYPE_BLOCK && world->getBlockId(trace.getBlockPos()) == BlockManager::portal->getBlockID())
			{
				//setPortal(trace.getBlockPos());
			}
			else
			{
				onImpact(trace);
			}
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
		float f1 = 0.99F;
		float f2 = getGravityVelocity();

		if (isInWater())
		{
			float f3 = 0.25F;
			for (int j = 0; j < 4; ++j)
			{
				world->spawnParticle("bubble", Vector3(position.x - motion.x * f3, position.y - motion.y * f3, position.z - motion.z * f3), motion);
			}
			f1 = 0.8F;
		}
		motion.x *= f1;
		motion.y *= f1;
		motion.z *= f1;


		motion.y -= f2;

		setPosition(position);
	}

	void EntityThrowable::writeEntityToNBT(NBTTagCompound * pNbtArrow)
	{
		pNbtArrow->setShort("xTile", (i16)m_blockPos.x);
		pNbtArrow->setShort("yTile", (i16)m_blockPos.y);
		pNbtArrow->setShort("zTile", (i16)m_blockPos.z);
		pNbtArrow->setByte("inTile", (i8)inTile);
		pNbtArrow->setByte("inData", (i8)inData);
		pNbtArrow->setByte("shake", (i8)throwableShake);
		pNbtArrow->setByte("inGround", (i8)(inGround ? 1 : 0));

	}

	void EntityThrowable::readEntityFromNBT(NBTTagCompound * pNbtArrow)
	{
		m_blockPos.x = pNbtArrow->getShort("xTile");
		m_blockPos.y = pNbtArrow->getShort("yTile");
		m_blockPos.z = pNbtArrow->getShort("zTile");
		inTile = pNbtArrow->getByte("inTile") & 255;
		inData = pNbtArrow->getByte("inData") & 255;
		throwableShake = pNbtArrow->getByte("shake") & 255;
		inGround = pNbtArrow->getByte("inGround") == 1;
		m_throwerEntityId = 0;
	}

	EntityLivingBase * EntityThrowable::getThrower()
	{
		EntityLivingBase* pThrower = NULL;

		if (m_throwerEntityId > 0)
		{
			pThrower = dynamic_cast<EntityLivingBase*>(world->getEntity(m_throwerEntityId));
		}

		return pThrower;
	}
}