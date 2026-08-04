#include "Core.h"
#include "EntityFishHook.h"
#include "Entity/EntityPlayer.h"
#include "World/World.h"
#include "Block/Block.h"
#include "Block/BlockManager.h"
#include "Entity/DamageSource.h"

namespace BLOCKMAN
{
	EntityFishHook::EntityFishHook(World * pWorld, EntityLivingBase * pAngler)
		: Entity(pWorld)
	{
		m_blockPos = BlockPos(-1, -1, -1);
		m_anglerEntityId = pAngler->entityId;

		setSize(0.25f, 0.9f);
		setPosition(pAngler->position);
	}

	void EntityFishHook::setHeadingFromThrower(Entity * entityThrower, float rotationPitchIn, float rotationYawIn, float pitchOffset, float velocity, float inaccuracy)
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

	void EntityFishHook::setThrowableHeading(const Vector3 & pos, float velocity, float inaccuracy)
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
	}

	void EntityFishHook::setVelocity(const Vector3 & vel)
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

	void EntityFishHook::onUpdate()
	{
		lastTickPos = position;
		Entity::onUpdate();
		Entity* anglerEntity = getAngler();
		EntityPlayer* pPlayer = dynamic_cast<EntityPlayer*>(anglerEntity);
		if (!anglerEntity)
		{
			setDead();
			return;
		}

		if (isDead)
		{
			if (pPlayer)
			{
				pPlayer->m_fishEntity = nullptr;
			}
		}

		if (!world->m_isClient && (anglerEntity->position - position).len() > 32.0f)
		{
			pPlayer->m_fishEntity = nullptr;
			setDead();
			return;
		}

		if (isInWater())
		{
			float f3 = 0.25F;
			for (int j = 0; j < 4; ++j)
			{
				world->spawnParticle("bubble", Vector3(position.x - motion.x * f3, position.y - motion.y * f3, position.z - motion.z * f3), motion);
			}
			position.y += 0.1f;
			setPosition(position);
			m_inWater = true;
			return;
		}

		if (m_inWater)
		{
			return;
		}

		if (m_targetEntityId)
		{
			Entity* pTarget = getTarget();
			if (pTarget)
			{
				if (pTarget->isDead)
				{
					m_targetEntityId = 0;
					motion.x *= 0;
					motion.y *= rand->nextFloat() * 0.2F;
					motion.z *= 0;
					return;
				}
				position = pTarget->position;
				position.y += pTarget->getHeight() / 2.0f;
				return;
			}
			else
			{
				m_targetEntityId = 0;
				motion.x *= 0;
				motion.y *= rand->nextFloat() * 0.2F;
				motion.z *= 0;
				return;
			}
		}

		if (m_inGround)
		{
			int blockID = world->getBlockId(m_blockPos);
			int metadata = world->getBlockMeta(m_blockPos);
			if (blockID == m_inTile && metadata == m_inData)
			{
				return;
			}
			m_inGround = false;
			motion.x *= 0;
			motion.y *= rand->nextFloat() * 0.2F;
			motion.z *= 0;
		}

		Vector3 begin = position;
		Vector3 end = position + motion;
		RayTraceResult trace = world->rayTraceBlocks(begin, end, false, true);

		if (trace.result)
		{
			end = trace.hitVec;
		}

		Entity* tracyEntity = NULL;
		EntityArr entities = world->getEntitiesWithinAABBExcludingEntity(this, boundingBox.addCoord(motion.x, motion.y, motion.z).expland(1.0f, 0.35f, 1.0f));
		float min_dis = 100000.0f;
		float expendSize;

		for (int i = 0; i < int(entities.size()); ++i)
		{
			Entity* pEntity = entities[i];
			if (pEntity->canBePushed() && pEntity != anglerEntity)
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

		if (tracyEntity)
		{
			trace.set(tracyEntity);
		}

		float motionLen;

		if (trace.result)
		{
			if (trace.entityHit)
			{
				EntityPlayer* pEntityPlayer = dynamic_cast<EntityPlayer*>(trace.entityHit);
				if (pEntityPlayer)
				{
					m_targetEntityId = trace.entityHit->entityId;
					if (!world->m_isClient)
					{
						//TODO: sendPacket to client for keeping the same target entity id
						DamageSource* pSource = NULL;
						if (!getAngler())
						{
							pSource = DamageSource::causeThrownDamage(this, this);
						}
						else
						{
							Entity* angler = (Entity*)getAngler();
							pSource = DamageSource::causeThrownDamage(this, angler);
						}
						int amount = 2;
						trace.entityHit->attackEntityFrom(pSource, (float)amount);
						LordSafeDelete(pSource);
					}
					return;
				}
			}
			else
			{
				m_blockPos = trace.getBlockPos();
				m_inTile = world->getBlockId(m_blockPos);
				m_inData = world->getBlockMeta(m_blockPos);
				motion = trace.hitVec - position;
				motionLen = motion.len();
				position -= motion / motionLen * 0.05f;
				m_inGround = true;

				if (m_inTile != 0)
				{
					if (!world->m_isClient) {
						BlockManager::sBlocks[m_inTile]->onEntityCollidedWithBlock(world, m_blockPos, this);
					}
				}
			}
		}

		position += motion;

		float f1 = 0.9f;
		float f2 = getGravityVelocity();

		motion.x *= f1;
		motion.y *= f1;
		motion.z *= f1;
		motion.y -= f2;

		setPosition(position);
	}

	void EntityFishHook::writeEntityToNBT(NBTTagCompound * pNbtArrow)
	{
		pNbtArrow->setShort("xTile", (i16)m_blockPos.x);
		pNbtArrow->setShort("yTile", (i16)m_blockPos.y);
		pNbtArrow->setShort("zTile", (i16)m_blockPos.z);
		pNbtArrow->setByte("inTile", (i8)m_inTile);
		pNbtArrow->setByte("inData", (i8)m_inData);
		pNbtArrow->setByte("inGround", (i8)(m_inGround ? 1 : 0));

	}

	void EntityFishHook::readEntityFromNBT(NBTTagCompound * pNbtArrow)
	{
		m_blockPos.x = pNbtArrow->getShort("xTile");
		m_blockPos.y = pNbtArrow->getShort("yTile");
		m_blockPos.z = pNbtArrow->getShort("zTile");
		m_inTile = pNbtArrow->getByte("inTile") & 255;
		m_inData = pNbtArrow->getByte("inData") & 255;
		m_inGround = pNbtArrow->getByte("inGround") == 1;
		m_anglerEntityId = 0;
	}

	bool EntityFishHook::isInRangeToRenderDist(float distanceSqr) const
	{
		float len = boundingBox.getAverageEdgeLength() * 4.0f;
		len *= 64.0f;

		return distanceSqr < len * len;
	}

	Entity* EntityFishHook::getAngler()
	{
		Entity* pEntity = NULL;
		if (m_anglerEntityId > 0)
		{
			pEntity = world->getEntity(m_anglerEntityId);
		}
		return pEntity;
	}

	Entity* EntityFishHook::getTarget()
	{
		Entity* pEntity = NULL;
		if (m_targetEntityId > 0)
		{
			pEntity = world->getEntity(m_targetEntityId);
		}
		return pEntity;
	}

	void EntityFishHook::catchFish()
	{
		EntityLivingBase *liveEntity = dynamic_cast<EntityLivingBase*>(getTarget());
		if (nullptr != liveEntity && getAngler())
		{
			Vector3 temp = getAngler()->position - getTarget()->position;
			temp.y = temp.y * 0.5f + 2.0f;

			float v0;
			float height;
			float vSum = 0.0f;
			float vTemp = 0.0f;
			if (temp.y > 0.0f)
			{
				height = temp.y;
				while (vSum < height)
				{
					vTemp = (vTemp / 0.98f + 0.08f) / 0.98f;
					vSum += vTemp;
				}
				v0 = vTemp;
			}
			else
			{
				height = 0.0f;
				v0 = 0.0f;
			}

			liveEntity->m_isPulled = true;
			float lensqr = Math::Sqrt(temp.x * temp.x + temp.z * temp.z);
			if (lensqr < 0.00001f)
				return;
			float scale = 2.0f;
			liveEntity->motion.x += temp.x / lensqr * scale;
			liveEntity->motion.y += v0;
			liveEntity->motion.z += temp.z / lensqr * scale;
		}
	}
}
