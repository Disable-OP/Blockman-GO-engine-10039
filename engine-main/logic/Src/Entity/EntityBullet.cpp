#include "EntityBullet.h"
#include "EntityLivingBase.h"
#include "EntityPlayer.h"
#include "DamageSource.h"
#include "Enchantment.h"

#include "World/World.h"
#include "World/RayTracyResult.h"
#include "Block/Block.h"
#include "Block/BlockManager.h"
#include "Item/Item.h"
#include "Item/Items.h"
#include "Item/ItemStack.h"
#include "Inventory/InventoryPlayer.h"
#include "Script/Event/LogicScriptEvents.h"
#include "Setting/GunSetting.h"

namespace BLOCKMAN
{
void EntityBullet::init_entityBullet()
{ 
	m_blockPos = BlockPos(-1, -1, -1);
	ticksInAir = 0;
	damage = 2.0f;
	knockbackStrength = 1;
	m_shootingEntityId = 0;
}

EntityBullet::EntityBullet(World* pWorld)
	: Entity(pWorld)
{
	init_entityBullet();
	renderDistanceWeight = 10.0f;
	setSize(0.5F, 0.5F);
}

EntityBullet::EntityBullet(World* pWorld, EntityLivingBase* pShooting, const Vector3& playerPos, float playerYaw, float playerPitch, int bulletID, const GunSetting* pSetting)
	: Entity(pWorld)
	, m_bulletID(bulletID)
{
	init_entityBullet();
	renderDistanceWeight = 10.0f;
	if(pShooting != NULL) m_shootingEntityId = pShooting->entityId;
	setKnockbackStrength((int)pSetting->knockbackDistance);
	setDamage(pSetting->damage);
	setShootingRange(pSetting->shootRange);

	setSize(0.5F, 0.5F);
	// add 0.5f for pos.
	setLocationAndAngles(playerPos, playerYaw, playerPitch);
	position.x -= Math::Cos(rotationYaw * Math::DEG2RAD) * 0.16F;
	position.y -= 0.15f;
	position.z -= Math::Sin(rotationYaw * Math::DEG2RAD) * 0.16F;
	setPosition(position);
	yOffset = 0.0F;
	motion.x = -float(Math::Sin(double(rotationYaw) * Math::DEG2RAD) * Math::Cos(double(rotationPitch) * Math::DEG2RAD));
	motion.z = float(Math::Cos(double(rotationYaw) * Math::DEG2RAD) * Math::Cos(double(rotationPitch) * Math::DEG2RAD));
	motion.y = -float(Math::Sin(double(rotationPitch) * Math::DEG2RAD));
	float velocity = pSetting->bulletSpeed; // velocity.
	setThrowableHeading(motion, velocity * 1.5F, 1.0F);
	LordLogInfo("EntityBullet created, entityId[%d],x[%f] y[%f] z[%f] motion.x[%f] motion.y[%f] motion.z[%f] yaw[%f] pitch[%f]",
		entityId, position.x, position.y, position.z, motion.x, motion.y, motion.z, rotationYaw, rotationPitch);
}

void EntityBullet::setThrowableHeading(const Vector3& pos, float velocity, float inaccuracy)
{
	Vector3 dir = pos.normalizedCopy();
	motion = dir * velocity;
	float dir_xz = Math::Sqrt(dir.x * dir.x + dir.z * dir.z);
	prevRotationYaw = rotationYaw = Math::ATan2(dir.x, dir.z) * Math::RAD2DEG;
	prevRotationPitch = rotationPitch = Math::ATan2(dir.y, dir_xz) * Math::RAD2DEG;
}

void EntityBullet::setPositionAndRotation2(const Vector3& pos, float yaw, float pitch, int increment)
{
	setPosition(pos);
	setRotation(yaw, pitch);
}

void EntityBullet::setVelocity(const Vector3& vel)
{
	motion = vel;

	if (prevRotationPitch == 0.0F && prevRotationYaw == 0.0F)
	{
		float dir_xz = Math::Sqrt(vel.x * vel.x + vel.z * vel.z);
		prevRotationYaw = rotationYaw = Math::ATan2(vel.x, vel.z) * Math::RAD2DEG;
		prevRotationPitch = rotationPitch = Math::ATan2(vel.y, dir_xz) * Math::RAD2DEG;
		prevRotationPitch = rotationPitch;
		prevRotationYaw = rotationYaw;
		setLocationAndAngles(position, rotationYaw, rotationPitch);
	}
}

void EntityBullet::onUpdate()
{
	Entity::onUpdate();

	LordLogInfo("Bullet Pos %f, %f, %f", position.x, position.y, position.z);
	
	Entity* shootingEntity = getShootingEntity();
	if (prevRotationPitch == 0.0F && prevRotationYaw == 0.0F)
	{
		float dir_xz = Math::Sqrt(motion.x * motion.x + motion.z * motion.z);
		prevRotationYaw = rotationYaw = Math::ATan2(motion.x, motion.z) * Math::RAD2DEG;
		prevRotationPitch = rotationPitch = Math::ATan2(motion.y, dir_xz) * Math::RAD2DEG;
	}

	int blockID = world->getBlockId(m_blockPos);

	if (blockID > 0)
	{
		Block* pBlock = BlockManager::sBlocks[blockID];
		pBlock->setBlockBoundsBasedOnState(world, m_blockPos);
		Box blockAABB = pBlock->getCollisionBoundingBox(world, m_blockPos);

		if (!blockAABB.isNull() && blockAABB.isPointInside(position))
		{
			setDead();
		}
	}
	
	{
		++ticksInAir;
		Vector3 begin = position; 
		Vector3 entityEnd = position + motion;	// 与角色的碰撞
		Vector3 blockEnd = position + motion;	// 与方块的碰撞
		RayTraceResult trace = world->rayTraceBlocks(begin, blockEnd, false, true);
		float min_dis = 100000.0f;
		float ratio = 1.f;

		if (trace.result)
		{
			blockEnd = trace.hitVec; 
			// 子弹的速度很快，先判断与方块的碰撞，再在这个距离范围内判断与角色的碰撞
			min_dis = begin.distanceTo(blockEnd);
			ratio = min_dis / motion.len();
		}

		Entity* tracyEntity = NULL;
		EntityArr entities = world->getEntitiesWithinAABBExcludingEntity(this, boundingBox.addCoord(motion.x * ratio, motion.y * ratio, motion.z * ratio).expland(0.3f, 0.3f, 0.3f));
		
		float expendSize;

		for (int i = 0; i < int(entities.size()); ++i)
		{
			Entity* pEntity = entities[i];
			float distance = begin.distanceTo(pEntity->position);
			if (pEntity->canBePushed() && distance <= m_shootingRange && (pEntity->entityId != m_shootingEntityId || ticksInAir >= 5))
			{
				expendSize = 0.3F;
				Box entity_box = pEntity->boundingBox.expland(expendSize, expendSize, expendSize);
				RayTraceResult tracy_entity = RayTraceResult::calculateIntercept(entity_box, begin, entityEnd);

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


		if (trace.result && trace.entityHit)
		{
			EntityPlayer* tracyPlayer = dynamic_cast<EntityPlayer*>(trace.entityHit);
			EntityPlayer* shootPlayer = dynamic_cast<EntityPlayer*>(shootingEntity);

			if (tracyPlayer && tracyPlayer->capabilities.disableDamage/* ||
				shootPlayer  && !shootPlayer->isOpposite(var20) */)   // todo. 
			{
				trace.reset();
			}
		}

		float motionLen;
		float motionLen_xz;

		if (trace.result)
		{
			SCRIPT_EVENT::EntityHitEvent::invoke(m_bulletID, trace.entityHit ? trace.hitVec : trace.blockPos, m_shootingEntityId);
			if (trace.entityHit)
			{
				if (world->m_isClient)
				{
					setDead();
				}
				else 
				{
					float amount = damage;

					DamageSource* pSource = NULL;

					if (!shootingEntity)
					{
						pSource = DamageSource::causeBulletDamage(this, this);
					}
					else
					{
						pSource = DamageSource::causeBulletDamage(this, shootingEntity);
					}
									

					if (trace.entityHit->attackEntityFrom(pSource, amount))
					{
						EntityLivingBase* pLiving = dynamic_cast<EntityLivingBase*>(trace.entityHit);
						if (pLiving)
						{
							if (knockbackStrength > 0)
							{
								motionLen_xz = Math::Sqrt(motion.x * motion.x + motion.z * motion.z);

								if (motionLen_xz > 0.0F)
								{
									//trace.entityHit->addVelocity(Vector3(motion.x * knockbackStrength * 0.6f / motionLen_xz, 0.1f, motion.z * knockbackStrength * 0.6f / motionLen_xz));
								}
							}

							if (shootingEntity)
							{
								EnchantmentThorns::damageArmor(shootingEntity, pLiving, rand);
							}

							onBulletHit(pLiving);
							// todo.
							/*EntityPlayer* tracePlayer = dynamic_cast<EntityPlayer*>(trace->entityHit);
							if (shootingEntity && trace->entityHit != shootingEntity && tracePlayer && shootingEntity instanceof EntityPlayerMP)
							{
								((EntityPlayerMP)shootingEntity).playerNetServerHandler.sendPacketToPlayer(new Packet70GameEvent(6, 0));
							}*/
						}

						//playSound("random.bowhit", 1.0F, 1.2F / (rand->nextFloat() * 0.2F + 0.9F));
						playSoundByType(ST_BowHit);

						// todo.
						/*if (!(trace.entityHit instanceof EntityEnderman))
						{
							setDead();
						}*/
					}
					else
					{
						motion.x *= -0.1f;
						motion.y *= -0.1f;
						motion.z *= -0.1f;
						rotationYaw += 180.0F;
						prevRotationYaw += 180.0F;
						ticksInAir = 0;
					}

					LordSafeDelete(pSource);
					setDead();
				}
			}
			else
			{
				m_blockPos = trace.getBlockPos();
				int inTile = world->getBlockId(m_blockPos);
				motion = trace.hitVec - position;
				motionLen = motion.len();
				position -= motion / motionLen * 0.05f;
				playSoundByType(ST_BowHit);
				
				if (inTile != 0)
				{
					if (!world->m_isClient) {
						BlockManager::sBlocks[inTile]->onEntityCollidedWithBlock(world, m_blockPos, this);
					}
				}
				setDead();
			}
		}
		
		position += motion;
		m_shootingRange -= motion.len();
		if (m_shootingRange <= 0.f)
			setDead();

		motionLen = Math::Sqrt(motion.x * motion.x + motion.z * motion.z);
		rotationYaw = Math::ATan2(motion.x, motion.z) * Math::RAD2DEG;

		for (rotationPitch = Math::ATan2(motion.y, motionLen) * Math::RAD2DEG; rotationPitch - prevRotationPitch < -180.0F; prevRotationPitch -= 360.0F)
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
		float var24 = 0.99F;
		expendSize = 0.05F;

		if (isInWater())
		{
			for (int i = 0; i < 4; ++i)
			{
				motionLen_xz = 0.25F;
				// worldObj.spawnParticle("bubble", posX - motion.x * (double)motionLen_xz, posY - motion.y * (double)motionLen_xz, posZ - motion.z * (double)motionLen_xz, motion.x, motion.y, motion.z);
			}

			var24 = 0.8F;
		}

		motion.x *= var24;
		motion.y *= var24;
		motion.z *= var24;
		motion.y -= expendSize;
		setPosition(position);
		if (!world->m_isClient) {
			doBlockCollisions();
		}
	}
}

void EntityBullet::onBulletHit(EntityLivingBase * pLiving)
{
	// add.
}

void EntityBullet::writeEntityToNBT(NBTTagCompound* pNbtBullet)
{
	pNbtBullet->setShort("xTile", (i16)m_blockPos.x);
	pNbtBullet->setShort("yTile", (i16)m_blockPos.y);
	pNbtBullet->setShort("zTile", (i16)m_blockPos.z);
	pNbtBullet->setFloat("damage", damage);
}

void EntityBullet::readEntityFromNBT(NBTTagCompound* pNbtBullet)
{
	m_blockPos.x = pNbtBullet->getShort("xTile");
	m_blockPos.y = pNbtBullet->getShort("yTile");
	m_blockPos.z = pNbtBullet->getShort("zTile");

	if (pNbtBullet->hasKey("damage"))
	{
		damage = pNbtBullet->getFloat("damage");
	}
}

void EntityBullet::onCollideWithPlayer(EntityPlayer* pPlayer)
{
}

Entity * EntityBullet::getShootingEntity()
{
	Entity* pShooter = NULL;
	if (m_shootingEntityId > 0)
	{
		pShooter = world->getEntity(m_shootingEntityId);
	}

	return pShooter;
}

}