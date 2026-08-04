#include "EntityArrow.h"
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

namespace BLOCKMAN
{
void EntityArrow::init_entityArrow()
{ 
	m_blockPos = BlockPos(-1, -1, -1);
	inTile = 0;
	inData = 0;
	inGround = false;
	ticksInGround = 0;
	ticksInAir = 0;
	damage = 2.0f;
	knockbackStrength = 1;
	canBePickedUp = 0;
	arrowShake = 0;
	m_shootingEntityId = 0;
}

EntityArrow::EntityArrow(World* pWorld)
	: Entity(pWorld)
{
	init_entityArrow();
	renderDistanceWeight = 10.0f;
	setSize(0.5F, 0.5F);
}

EntityArrow::EntityArrow(World* pWorld, const Vector3& pos)
	: Entity(pWorld)
{
	init_entityArrow();
	renderDistanceWeight = 10.0f;
	setSize(0.5F, 0.5F);
	setPosition(pos);
	yOffset = 0.0F;
}

EntityArrow::EntityArrow(World* pWorld, EntityLivingBase* pShooting, EntityLivingBase* pTarget, float velocity, float inaccuracy)
	: Entity(pWorld)
{
	init_entityArrow();
	renderDistanceWeight = 10.0f;
	if (pShooting != NULL) m_shootingEntityId = pShooting->entityId;

	if (dynamic_cast<EntityPlayer*>(pShooting))
	{
		canBePickedUp = 1;
	}

	position.y = pShooting->position.y + pShooting->getEyeHeight() - 0.1f;
	float dir_x = pTarget->position.x - pShooting->position.x;
	float dir_y = pTarget->boundingBox.vMin.y + (pTarget->height / 3.0F) - position.y;
	float dir_z = pTarget->position.z - pShooting->position.z;
	float dir_xz = Math::Sqrt(dir_x * dir_x + dir_z * dir_z);

	if (dir_xz >= 1.0E-7)
	{
		float yaw = (Math::ATan2(dir_z, dir_x) * Math::RAD2DEG) - 90.0F;
		float pitch = (-(Math::ATan2(dir_y, dir_xz) *  Math::RAD2DEG));
		float off_x = dir_x / dir_xz;
		float off_z = dir_z / dir_xz;
		setLocationAndAngles(pShooting->position + Vector3(off_x, 0.f, off_z), yaw, pitch);
		yOffset = 0.0F;
		float off_y = dir_xz * 0.2F;
		setThrowableHeading(Vector3(dir_x, dir_y + off_y, dir_z), velocity, inaccuracy);
	}
}

EntityArrow::EntityArrow(World* pWorld, EntityLivingBase* pShooting, float velocity)
	: Entity(pWorld)
{
	init_entityArrow();
	renderDistanceWeight = 10.0f;
	if (pShooting != NULL) m_shootingEntityId = pShooting->entityId;

	if (dynamic_cast<EntityPlayer*>(pShooting))
	{
		canBePickedUp = 1;
	}

	setSize(0.5F, 0.5F);
	setLocationAndAngles(pShooting->position + Vector3(0.f, pShooting->getEyeHeight(), 0.f),
		pShooting->rotationYaw, pShooting->rotationPitch);
	position.x -= Math::Cos(rotationYaw * Math::DEG2RAD) * 0.16F;
	position.y -= 0.1f;
	position.z -= Math::Sin(rotationYaw * Math::DEG2RAD) * 0.16F;
	setPosition(position);
	yOffset = 0.0F;
	motion.x = -Math::Sin(rotationYaw * Math::DEG2RAD) * Math::Cos(rotationPitch * Math::DEG2RAD);
	motion.z = Math::Cos(rotationYaw * Math::DEG2RAD) * Math::Cos(rotationPitch * Math::DEG2RAD);
	motion.y = -Math::Sin(rotationPitch * Math::DEG2RAD);
	setThrowableHeading(motion, velocity * 1.5F, 1.0F);

	LordLogInfo("EntityArrow,created, entityId[%d],x[%f] y[%f] z[%f] motion.x[%f] motion.y[%f] motion.z[%f] yaw[%f] pitch[%f]",
		entityId, position.x, position.y, position.z, motion.x, motion.y, motion.z, rotationYaw, rotationPitch);
}

EntityArrow::EntityArrow(World* pWorld, EntityLivingBase* pShooting, const Vector3& playerPos, float playerYaw, float playerPitch, float cdTime, int arrowID)
	: Entity(pWorld)
	,m_arrowID(arrowID)
{
	init_entityArrow();
	renderDistanceWeight = 10.0f;
	if (pShooting != NULL) m_shootingEntityId = pShooting->entityId;

	if (dynamic_cast<EntityPlayer*>(pShooting))
	{
		canBePickedUp = 1;
	}

	setSize(0.5F, 0.5F);
	setLocationAndAngles(playerPos, playerYaw, playerPitch);
	position.x -= Math::Cos(rotationYaw * Math::DEG2RAD) * 0.16F;
	position.y -= 0.1f;
	position.z -= Math::Sin(rotationYaw * Math::DEG2RAD) * 0.16F;
	setPosition(position);
	yOffset = 0.0F;
	motion.x = -float(Math::Sin(double(rotationYaw) * Math::DEG2RAD) * Math::Cos(double(rotationPitch) * Math::DEG2RAD));
	motion.z = float(Math::Cos(double(rotationYaw) * Math::DEG2RAD) * Math::Cos(double(rotationPitch) * Math::DEG2RAD));
	motion.y = -float(Math::Sin(double(rotationPitch) * Math::DEG2RAD));
	float velocity = cdTime * 2.0f;
	setThrowableHeading(motion, velocity * 1.5F, 1.0F);
	LordLogInfo("EntityArrow,created, entityId[%d],x[%f] y[%f] z[%f] motion.x[%f] motion.y[%f] motion.z[%f] yaw[%f] pitch[%f]",
		entityId, position.x, position.y, position.z, motion.x, motion.y, motion.z, rotationYaw, rotationPitch);
}

void EntityArrow::setThrowableHeading(const Vector3& pos, float velocity, float inaccuracy)
{
	Vector3 dir = pos.normalizedCopy();
	/*dir.x += float(rand->nextGaussian() * (rand->nextBool() ? -1.f : 1.f) * 0.0075f * inaccuracy);
	dir.y += float(rand->nextGaussian() * (rand->nextBool() ? -1.f : 1.f) * 0.0075f * inaccuracy);
	dir.z += float(rand->nextGaussian() * (rand->nextBool() ? -1.f : 1.f) * 0.0075f * inaccuracy);*/
	motion = dir * velocity;
	float dir_xz = Math::Sqrt(dir.x * dir.x + dir.z * dir.z);
	prevRotationYaw = rotationYaw = float(Math::ATan2(double(dir.x), double(dir.z)) * Math::RAD2DEG);
	prevRotationPitch = rotationPitch = float(Math::ATan2(double(dir.y), double(dir_xz)) * Math::RAD2DEG);
	ticksInGround = 0;
}

void EntityArrow::setPositionAndRotation2(const Vector3& pos, float yaw, float pitch, int increment)
{
	setPosition(pos);
	setRotation(yaw, pitch);
}

void EntityArrow::setVelocity(const Vector3& vel)
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
		ticksInGround = 0;
	}
}

void EntityArrow::onUpdate()
{
	Entity::onUpdate();

	// LordLogInfo("Arrow yaw %f, pitch %f", rotationYaw, rotationPitch);
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
			inGround = true;
		}
	}

	if (arrowShake > 0)
	{
		--arrowShake;
	}

	if (inGround)
	{
		SCRIPT_EVENT::EntityHitEvent::invoke(ITEM_ID_ARROW, m_blockPos, m_shootingEntityId);

		int blockID = world->getBlockId(m_blockPos);
		int metadata = world->getBlockMeta(m_blockPos);

		if (blockID == inTile && metadata == inData)
		{
			++ticksInGround;

			if (ticksInGround == 1200 || (m_fromInfiniteEnchantedBow && ticksInGround == 20))
			{
				if (!world->m_isClient) {
					setDead();
				}
			}
			if (ticksInGround % 5 == 0)
			{
				spawnPotionParticles(1);
			}
		}
		else
		{
			inGround = false;
			motion.x *= rand->nextFloat() * 0.2F;
			motion.y *= rand->nextFloat() * 0.2F;
			motion.z *= rand->nextFloat() * 0.2F;
			ticksInGround = 0;
			ticksInAir = 0;
		}
	}
	else
	{
		++ticksInAir;
		Vector3 begin = position; 
		Vector3 end = position + motion; 
		RayTraceResult trace = world->rayTraceBlocks(begin, end, false, true);
		
		if (trace.result)
		{
			end = trace.hitVec; 
		}

		Entity* tracyEntity = NULL;
		EntityArr entities = world->getEntitiesWithinAABBExcludingEntity(this, boundingBox.addCoord(motion.x, motion.y, motion.z).expland(1.f, 1.f, 1.f));
		float min_dis = 100000.0f;
		float expendSize;

		for (int i = 0; i < int(entities.size()); ++i)
		{
			Entity* pEntity = entities[i];
			if (pEntity->canBePushed() && (pEntity != shootingEntity || ticksInAir >= 5))
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
			SCRIPT_EVENT::EntityHitEvent::invoke(ITEM_ID_ARROW, trace.entityHit ? trace.hitVec : trace.blockPos, m_shootingEntityId);
			if (trace.entityHit)
			{
				if (world->m_isClient) {
					setDead();
				}
				else {
					motionLen = Math::Sqrt(motion.x * motion.x + motion.y * motion.y + motion.z * motion.z);
					int amount = int(Math::Ceil(motionLen * damage));

					if (getIsCritical())
					{
						amount += rand->nextInt(amount / 2 + 2);
					}

					DamageSource* pSource = NULL;

					if (!shootingEntity)
					{
						pSource = DamageSource::causeArrowDamage(this, this);
					}
					else
					{
						pSource = DamageSource::causeArrowDamage(this, shootingEntity);
					}

					if (trace.entityHit->attackEntityFrom(pSource, (float)amount))
					{
						if (isBurning() /*&& !(trace.entityHit instanceof EntityEnderman)*/)
						{
							trace.entityHit->setFire(5);
						}
						EntityLivingBase* pLiving = dynamic_cast<EntityLivingBase*>(trace.entityHit);
						if (pLiving)
						{
							if (!world->m_isClient)
							{
								pLiving->setArrowCountInEntity(pLiving->getArrowCountInEntity() + 1);
							}

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

							onArrowHit(pLiving);
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
				inTile = world->getBlockId(m_blockPos);
				inData = world->getBlockMeta(m_blockPos);
				motion = trace.hitVec - position;
				motionLen = motion.len();
				position -= motion / (motionLen + 0.01f) * 0.05f;
				//playSound("random.bowhit", 1.0F, 1.2F / (rand->nextFloat() * 0.2F + 0.9F));
				playSoundByType(ST_BowHit);
				inGround = true;
				arrowShake = 7;
				setIsCritical(false);

				if (inTile != 0)
				{
					if (!world->m_isClient) {
						BlockManager::sBlocks[inTile]->onEntityCollidedWithBlock(world, m_blockPos, this);
					}
				}
			}
		}

		if (getIsCritical())
		{
			for (int i = 0; i < 4; ++i)
			{
				// worldObj.spawnParticle("crit", posX + motion.x * (double)i / 4.0D, posY + motion.y * (double)i / 4.0D, posZ + motion.z * (double)i / 4.0D, -motion.x, -motion.y + 0.2D, -motion.z);
			}
		}

		position += motion;
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
		spawnPotionParticles(2);
		if (!world->m_isClient) {
			doBlockCollisions();
		}
	}
}

void EntityArrow::onArrowHit(EntityLivingBase * pLiving)
{
	if (m_potionEffect) {
		pLiving->addPotionEffect(LordNew PotionEffect(*m_potionEffect));
	}
}

void EntityArrow::writeEntityToNBT(NBTTagCompound* pNbtArrow)
{
	pNbtArrow->setShort("xTile", (i16)m_blockPos.x);
	pNbtArrow->setShort("yTile", (i16)m_blockPos.y);
	pNbtArrow->setShort("zTile", (i16)m_blockPos.z);
	pNbtArrow->setByte("inTile", (i8)inTile);
	pNbtArrow->setByte("inData", (i8)inData);
	pNbtArrow->setByte("shake", (i8)arrowShake);
	pNbtArrow->setByte("inGround", (i8)(inGround ? 1 : 0));
	pNbtArrow->setByte("pickup", (i8)canBePickedUp);
	pNbtArrow->setFloat("damage", damage);
}

void EntityArrow::readEntityFromNBT(NBTTagCompound* pNbtArrow)
{
	m_blockPos.x = pNbtArrow->getShort("xTile");
	m_blockPos.y = pNbtArrow->getShort("yTile");
	m_blockPos.z = pNbtArrow->getShort("zTile");
	inTile = pNbtArrow->getByte("inTile") & 255;
	inData = pNbtArrow->getByte("inData") & 255;
	arrowShake = pNbtArrow->getByte("shake") & 255;
	inGround = pNbtArrow->getByte("inGround") == 1;

	if (pNbtArrow->hasKey("damage"))
	{
		damage = pNbtArrow->getFloat("damage");
	}

	if (pNbtArrow->hasKey("pickup"))
	{
		canBePickedUp = pNbtArrow->getByte("pickup");
	}
	else if (pNbtArrow->hasKey("player"))
	{
		canBePickedUp = pNbtArrow->getBool("player") ? 1 : 0;
	}
}

void EntityArrow::onCollideWithPlayer(EntityPlayer* pPlayer)
{
	if (!world->m_isClient && inGround && arrowShake <= 0)
	{
		bool flag = canBePickedUp == 1 || canBePickedUp == 2 && pPlayer->capabilities.isCreativeMode;

		ItemStackPtr itemStack;
		if (m_potionEffect) {
			itemStack = LORD::make_shared<ItemStack>(m_arrowPotionItem, 1);
		}
		else {
			itemStack = LORD::make_shared<ItemStack>(Item::arrow, 1);
			if (getArrowID() == Item::redArrow->itemID)
				itemStack = LORD::make_shared<ItemStack>(Item::redArrow, 1);
			if (getArrowID() == Item::blackArrow->itemID)
				itemStack = LORD::make_shared<ItemStack>(Item::blackArrow, 1);
		}

		if (canBePickedUp == 1 && !pPlayer->inventory->addItemStackToInventory(itemStack))
		{
			flag = false;
		}

		if (flag)
		{
			//playSound("random.pop", 0.2F, ((rand->nextFloat() - rand->nextFloat()) * 0.7F + 1.0F) * 2.0F);
			playSoundByType(ST_Pop);
			pPlayer->onItemPickup(this, 1);
			setDead();
		}
	}
}

void EntityArrow::spawnPotionParticles(int times)
{
	if (m_potionEffect && world->m_isClient)
	{
		int color = Potion::potionTypes[m_potionEffect->getPotionID()]->getLiquidColor();
		if (color != -1)
		{
			float r = 0;
			float g = 0;
			float b = 0;
			for (int i = 0; i <= m_potionEffect->getAmplifier(); ++i)
			{
				r += (float)(color >> 16 & 255) / 255.0F;
				g += (float)(color >> 8 & 255) / 255.0F;
				b += (float)(color >> 0 & 255) / 255.0F;
			}

			for (int i = 0; i < times; i++)
			{
				Vector3 pos= Vector3(position.x + (rand->nextFloat() - 0.5f) * width, position.y + rand->nextFloat() * height, position.z + (rand->nextFloat() - 0.5f) * width);
				world->spawnParticle("mobSpell", pos, Vector3(r, g, b));
			}
		}
	}
}

void EntityArrow::setIsCritical(bool critical)
{
	if (critical)
	{
		data_critical |= 1;
	}
	else
	{
		data_critical &= -2;
	}
}

Entity * EntityArrow::getShootingEntity()
{
	Entity* pEntity = NULL;
	if (m_shootingEntityId > 0)
	{
		pEntity = world->getEntity(m_shootingEntityId);
	}
	return pEntity;
}

}