#include "EntityLiving.h"
#include "EntityItem.h"
#include "EntityPlayer.h"
#include "Enchantment.h"

#include "Util/Random.h"
#include "World/World.h"
#include "Nbt/NBT.h"
#include "Item/ItemStack.h"
#include "Item/Item.h"
#include "Item/Items.h"
#include "Inventory/InventoryPlayer.h"
#include "AI/AITask.h"

namespace BLOCKMAN
{

EntityLookHelper::EntityLookHelper(EntityLiving& living)
	: entity(living)
	, deltaLookYaw(0.f)
	, deltaLookPitch(0.f)
	, isLooking(false)
	, position(Vector3::ZERO)
{
}

void EntityLookHelper::setLookPositionWithEntity(Entity* pEntity, float deltaYaw, float deltaPitch)
{
	position = pEntity->position;

	EntityLivingBase* pLiving = dynamic_cast<EntityLivingBase*>(pEntity);
	if (pLiving)
	{
		position.y = pEntity->position.y + pEntity->getEyeHeight();
	}
	else
	{
		position.y = (pEntity->boundingBox.vMin.y + pEntity->boundingBox.vMax.y) / 2.0f;
	}

	deltaLookYaw = deltaYaw;
	deltaLookPitch = deltaPitch;
	isLooking = true;
}

void EntityLookHelper::setLookPosition(const Vector3& lookat, float deltaYaw, float deltaPitch)
{
	position = lookat;
	deltaLookYaw = deltaYaw;
	deltaLookPitch = deltaPitch;
	isLooking = true;
}

void EntityLookHelper::onUpdateLook()
{
	entity.rotationPitch = 0.0F;

	if (isLooking)
	{
		isLooking = false;
		float dx = position.x - entity.position.x;
		float dy = position.y - (entity.position.y + entity.getEyeHeight());
		float dz = position.z - entity.position.z;
		float proj_xz = Math::Sqrt(dx * dx + dz * dz);
		float yaw = (Math::ATan2(dz, dx) * 180.0f / Math::PI) - 90.0F;
		float pitch = -(Math::ATan2(dy, proj_xz) * 180.f / Math::PI);
		entity.rotationPitch = updateRotation(entity.rotationPitch, pitch, deltaLookPitch);
		entity.rotationYawHead = updateRotation(entity.rotationYawHead, yaw, deltaLookYaw);
	}
	else
	{
		entity.rotationYawHead = updateRotation(entity.rotationYawHead, entity.renderYawOffset, 10.0F);
	}

	float deltaAngle = Math::WrapDegree(entity.rotationYawHead - entity.renderYawOffset);

	if (!entity.getNavigator().noPath())
	{
		if (deltaAngle < -75.0F)
		{
			entity.rotationYawHead = entity.renderYawOffset - 75.0F;
		}

		if (deltaAngle > 75.0F)
		{
			entity.rotationYawHead = entity.renderYawOffset + 75.0F;
		}
	}
}

float EntityLookHelper::updateRotation(float angle1, float angle2, float angleMax)
{
	float angle = Math::WrapDegree(angle2 - angle1);

	if (angle > angleMax)
	{
		angle = angleMax;
	}

	if (angle < -angleMax)
	{
		angle = -angleMax;
	}

	return angle1 + angle;
}

EntityMoveHelper::EntityMoveHelper(EntityLiving& living)
	: entity(living)
	, speed(0.f)
	, update(false)
{
	position = living.position;
}

void EntityMoveHelper::setMoveTo(const Vector3& pos, float speedIn)
{
	position = pos;
	speed = speedIn;
	update = true;
}

void EntityMoveHelper::onUpdateMoveHelper()
{
	entity.setMoveForward(0.0F);

	if (update)
	{
		update = false;
		int entity_y = int(Math::Floor(entity.boundingBox.vMin.y + 0.5f));
		float dx = position.x - entity.position.x;
		float dz = position.z - entity.position.z;
		float dy = position.y - entity_y;
		float disSqr = dx * dx + dy * dy + dz * dz;

		if (disSqr >= 2.5E-7f)
		{
			float yaw = (Math::ATan2(dz, dx) * 180.0f / Math::PI) - 90.0F;
			entity.rotationYaw = limitAngle(entity.rotationYaw, yaw, 30.0F);
			entity.setAIMoveSpeed((speed * entity.getEntityAttribute(SharedMonsterAttributes::MOVEMENT_SPEED)->getAttributeValue()));

			if (dy > 0.0f && dx * dx + dz * dz < 1.0f)
			{
				entity.getJumpHelper().setJumping();
			}
		}
	}
}

float EntityMoveHelper::limitAngle(float angle1, float angle2, float angleMax)
{
	float angle = Math::WrapDegree(angle2 - angle1);

	if (angle > angleMax)
	{
		angle = angleMax;
	}

	if (angle < -angleMax)
	{
		angle = -angleMax;
	}

	return angle1 + angle;
}

EntityJumpHelper::EntityJumpHelper(EntityLiving& living)
	: entity(living)
	, isJumping(false)
{
}

void EntityJumpHelper::doJump()
{
	entity.setJumping(isJumping);
	isJumping = false;
}

EntityBodyHelper::EntityBodyHelper(EntityLivingBase& living)
	: theLiving(living)
	, rotationTickCounter(0)
	, prevRenderYawHead(0.f)
{
}

void EntityBodyHelper::updateRenderAngles()
{
	float dx = theLiving.position.x - theLiving.prevPos.x;
	float dz = theLiving.position.z - theLiving.prevPos.z;

	if (dx * dx + dz * dz > 2.5E-7f)
	{
		theLiving.renderYawOffset = theLiving.rotationYaw;
		theLiving.rotationYawHead = computeAngleWithBound(theLiving.renderYawOffset, theLiving.rotationYawHead, 75.0F);
		prevRenderYawHead = theLiving.rotationYawHead;
		rotationTickCounter = 0;
	}
	else
	{
		float angleBound = 75.0F;

		if (Math::Abs(theLiving.rotationYawHead - prevRenderYawHead) > 15.0F)
		{
			rotationTickCounter = 0;
			prevRenderYawHead = theLiving.rotationYawHead;
		}
		else
		{
			++rotationTickCounter;
			bool var6 = true;

			if (rotationTickCounter > 10)
			{
				angleBound = Math::Max(1.0F - (rotationTickCounter - 10) / 10.0F, 0.0F) * 75.0F;
			}
		}

		theLiving.renderYawOffset = computeAngleWithBound(theLiving.rotationYawHead, theLiving.renderYawOffset, angleBound);
	}
}

float EntityBodyHelper::computeAngleWithBound(float angle1, float angle2, float bound)
{
	float angle = Math::WrapDegree(angle1 - angle2);

	if (angle < -bound)
	{
		angle = -bound;
	}

	if (angle >= bound)
	{
		angle = bound;
	}

	return angle1 - angle;
}

EntitySenses::EntitySenses(EntityLiving& living)
	: entityObj(living)
{
}

void EntitySenses::clearSensingCache()
{
	seenEntities.clear();
	unseenEntities.clear();
}

bool EntitySenses::canSee(Entity* pEntity)
{
	if (seenEntities.find(pEntity) != seenEntities.end())
	{
		return true;
	}
	else if (unseenEntities.find(pEntity) != unseenEntities.end())
	{
		return false;
	}
	else
	{
		bool cansee = entityObj.canEntityBeSeen(pEntity);

		if (cansee)
		{
			seenEntities.insert(pEntity);
		}
		else
		{
			unseenEntities.insert(pEntity);
		}

		return cansee;
	}
}

EntityLiving::EntityLiving(World* pWorld)
	: EntityLivingBase(pWorld)
	, lookHelper(*this)
	, moveHelper(*this)
	, jumpHelper(*this)
	, bodyHelper(*this)
	, navigator(*this, pWorld)
	, senses(*this)
	, experienceValue(0)
	, m_canPickUpLoot(false)
	, persistenceRequired(false)
	, defaultPitch(0.f)
	, m_currentTargetEntityId(0)
	, numTicksToChaseTarget(0)
	, isLeashed(false)
	, leashedToEntity(NULL)
	, leashNBTTag(NULL)
	, livingSoundTime(0)
{
	for (int i = 0; i < EQUIP_SLOT; ++i)
	{
		equipment[i] = NULL;
		equipmentDropChances[i] = 0.085F;
	}
	m_tasks = LordNew AITasks();
	m_targetTasks = LordNew AITasks();
}

EntityLiving::~EntityLiving()
{
	LordSafeDelete(m_targetTasks);
	LordSafeDelete(m_tasks);
}

void EntityLiving::applyEntityAttributes()
{
	EntityLivingBase::applyEntityAttributes();
	attributeMap->registerAttribute(SharedMonsterAttributes::FOLLOW_RANGE)->setBaseValue(16.0f);
}

void EntityLiving::entityInit()
{
	EntityLivingBase::entityInit();
	data_ai_flag = 0;
	data_custom_name = StringUtil::BLANK;
}

void EntityLiving::playLivingSound()
{
	//String sound = getLivingSound();

	//if (!sound.empty())
	//{
	//	playSound(sound, getSoundVolume(), getSoundPitch());
	//}

	SoundType soundType = getLivingSoundType();
	if (soundType > ST_Invalid && soundType < ST_Total)
	{
		playSoundByType(soundType);
	}
}

void EntityLiving::onEntityUpdate()
{
	EntityLivingBase::onEntityUpdate();

	if (isEntityAlive() && rand->nextInt(1000) < livingSoundTime++)
	{
		livingSoundTime = -getTalkInterval();
		playLivingSound();
	}
}

int EntityLiving::getExperiencePoints(EntityPlayer* pPlayer)
{
	if (experienceValue > 0)
	{
		int exp = experienceValue;
		ItemStackArr stacks = getLastActiveItems();

		for (int i = 0; i < int(stacks.size()); ++i)
		{
			if (stacks[i] && equipmentDropChances[i] <= 1.0F)
			{
				exp += 1 + rand->nextInt(3);
			}
		}

		return exp;
	}
	else
	{
		return experienceValue;
	}
}

void EntityLiving::spawnExplosionParticle()
{
	for (int i = 0; i < 20; ++i)
	{
		float dsize = 10.0f;
		Vector3 vel = rand->nextGaussianVec() * 0.02f;
		Vector3 spawn = position + rand->nextVector() * Vector3(2.f * width, height, 2.f * width) - Vector3(width, 0.f, width) - vel * dsize;

		world->spawnParticle("explode", spawn, vel);
	}
}

void EntityLiving::onUpdate()
{
	EntityLivingBase::onUpdate();

	if (!world->m_isClient)
	{
		updateLeashedState();
	}
}

float EntityLiving::updateDistance(float yaw, float distance)
{
	if (isAIEnabled())
	{
		bodyHelper.updateRenderAngles();
		return distance;
	}
	return EntityLivingBase::updateDistance(yaw, distance);
}

void EntityLiving::dropFewItems(bool wasRecentlyHit, int lootingModifier)
{
	int itemid = getDropItemId();

	if (itemid > 0)
	{
		int count = rand->nextInt(3);

		if (lootingModifier > 0)
		{
			count += rand->nextInt(lootingModifier + 1);
		}

		for (int i = 0; i < count; ++i)
		{
			dropItem(itemid, 1);
		}
	}
}

void EntityLiving::writeEntityToNBT(NBTTagCompound* pNbtEntity)
{
	EntityLivingBase::writeEntityToNBT(pNbtEntity);
	pNbtEntity->setBool("CanPickUpLoot", canPickUpLoot());
	pNbtEntity->setBool("PersistenceRequired", persistenceRequired);

	NBTTagList* pNbtEquip = LordNew NBTTagList();
	NBTTagCompound* pNbtCompound;
	for (int i = 0; i < EQUIP_SLOT; ++i)
	{
		pNbtCompound = LordNew NBTTagCompound();

		if (equipment[i])
		{
			equipment[i]->writeToNBT(pNbtCompound);
		}

		pNbtEquip->appendTag(pNbtCompound);
	}
	pNbtEntity->setTag("Equipment", pNbtEquip);

	NBTTagList* pNbtDropChances = LordNew NBTTagList();
	for (int i = 0; i < EQUIP_SLOT; ++i)
	{
		pNbtDropChances->appendTag(LordNew NBTTagFloat(StringUtil::ToString(i), equipmentDropChances[i]));
	}
	pNbtEntity->setTag("DropChances", pNbtDropChances);

	pNbtEntity->setString("CustomName", getCustomNameTag());
	pNbtEntity->setBool("CustomNameVisible", isAIDisable());
	pNbtEntity->setBool("Leashed", isLeashed);

	if (leashedToEntity)
	{
		pNbtCompound = LordNew NBTTagCompound("Leash");

		EntityLivingBase* pLivingbase = dynamic_cast<EntityLivingBase*>(leashedToEntity);
		if (pLivingbase)
		{
			pNbtCompound->setLong("UUIDMost", leashedToEntity->getUniqueID().getMostSignificantBits());
			pNbtCompound->setLong("UUIDLeast", leashedToEntity->getUniqueID().getLeastSignificantBits());
		}

		// todo.
		/*EntityHanging* pHanging = dynamic_cast<EntityHanging*>(leashedToEntity);
		if (pHanging)
		{
			pNbtCompound->setInteger("X", pHanging->xPosition);
			pNbtCompound->setInteger("Y", pHanging->yPosition);
			pNbtCompound->setInteger("Z", pHanging->zPosition);
		}*/

		pNbtEntity->setTag("Leash", pNbtCompound);
	}
}

void EntityLiving::readEntityFromNBT(NBTTagCompound* pNbtEntity)
{
	EntityLivingBase::readEntityFromNBT(pNbtEntity);
	setCanPickUpLoot(pNbtEntity->getBool("CanPickUpLoot"));
	persistenceRequired = pNbtEntity->getBool("PersistenceRequired");

	if (pNbtEntity->hasKey("CustomName") && pNbtEntity->getString("CustomName").length() > 0)
	{
		setCustomNameTag(pNbtEntity->getString("CustomName"));
	}

	setNoAI(pNbtEntity->getBool("CustomNameVisible"));
	
	NBTTagList* pNbtEquip;
	if (pNbtEntity->hasKey("Equipment"))
	{
		pNbtEquip = pNbtEntity->getTagList("Equipment");

		for (int i = 0; i < EQUIP_SLOT; ++i)
		{
			equipment[i] = ItemStack::loadItemStackFromNBT((NBTTagCompound*)pNbtEquip->tagAt(i));
		}
	}

	NBTTagList* pNbtDropChances;
	if (pNbtEntity->hasKey("DropChances"))
	{
		pNbtDropChances = pNbtEntity->getTagList("DropChances");

		for (int i = 0; i < pNbtDropChances->tagCount(); ++i)
		{
			equipmentDropChances[i] = ((NBTTagFloat*)pNbtEquip->tagAt(i))->data;
		}
	}

	isLeashed = pNbtEntity->getBool("Leashed");

	if (isLeashed && pNbtEntity->hasKey("Leash"))
	{
		leashNBTTag = pNbtEntity->getCompoundTag("Leash");
	}
}

void EntityLiving::setAIMoveSpeed(float speed)
{
	EntityLivingBase::setAIMoveSpeed(speed);
	setMoveForward(speed);
}

void EntityLiving::onLivingUpdate()
{
	EntityLivingBase::onLivingUpdate();

	if (!world->m_isClient && canPickUpLoot() && !isDead /*&& worldObj.getGameRules().getGameRuleBooleanValue("mobGriefing")*/)
	{
		EntityArr entities = world->getEntitiesWithinAABB(ENTITY_CLASS_ITEM, boundingBox.expland(1.0f, 0.0f, 1.0f));
		
		for (EntityArr::iterator it = entities.begin(); it != entities.end(); ++it)
		{
			EntityItem* pItem = dynamic_cast<EntityItem*>(*it);

			if (!pItem->isDead && pItem->getEntityItem())
			{
				ItemStackPtr pStack = pItem->getEntityItem();
				int armorPos = getArmorPosition(pStack);

				if (armorPos > -1)
				{
					bool hasSword = true;
					ItemStackPtr pArmor = getCurrentItemOrArmor(armorPos);

					if (pArmor)
					{
						if (armorPos == 0)
						{
							ItemSword* pStackItem = dynamic_cast<ItemSword*>(pStack->getItem());
							ItemSword* pArmorItem = dynamic_cast<ItemSword*>(pArmor->getItem());
							if (pStackItem && !pArmorItem)
							{
								hasSword = true;
							}
							else if (pStackItem && pArmorItem)
							{
								if (pStackItem->getDamageVsEntity() == pArmorItem->getDamageVsEntity())
								{
									hasSword = pStack->getItemDamage() > pArmor->getItemDamage() ||
										pStack->hasTagCompound() && !pArmor->hasTagCompound();
								}
								else
								{
									hasSword = pStackItem->getDamageVsEntity() > pArmorItem->getDamageVsEntity();
								}
							}
							else
							{
								hasSword = false;
							}
						}
						else
						{
							ItemArmor* pStackItem = dynamic_cast<ItemArmor*>(pStack->getItem());
							ItemArmor* pArmorItem = dynamic_cast<ItemArmor*>(pArmor->getItem());
							if (pStackItem && !pArmorItem)
							{
								hasSword = true;
							}
							else if (pStackItem && pArmorItem)
							{
								if (pStackItem->damageReduceAmount == pArmorItem->damageReduceAmount)
								{
									hasSword = pStack->getItemDamage() > pArmor->getItemDamage() ||
										pStack->hasTagCompound() && !pArmor->hasTagCompound();
								}
								else
								{
									hasSword = pStackItem->damageReduceAmount > pArmorItem->damageReduceAmount;
								}
							}
							else
							{
								hasSword = false;
							}
						}
					}

					if (hasSword)
					{
						if (pArmor && rand->nextFloat() - 0.1F < equipmentDropChances[armorPos])
						{
							entityDropItem(pArmor, 0.0F);
						}

						setCurrentItemOrArmor(armorPos, pStack);
						equipmentDropChances[armorPos] = 2.0F;
						persistenceRequired = true;
						onItemPickup(pItem, 1);
						pItem->setDead();
					}
				}
			}
		}
	}
}

void EntityLiving::despawnEntity()
{
	if (persistenceRequired)
	{
		entityAge = 0;
	}
	else
	{
		EntityPlayer* pPlayer = world->getClosestPlayerToEntity(this, -1.0f);

		if (pPlayer)
		{
			Vector3 dir = pPlayer->position - position;
			float disLenSqr = dir.lenSqr();

			if (canDespawn() && disLenSqr > 16384.0f)
			{
				setDead();
			}

			if (entityAge > 600 && rand->nextInt(800) == 0 && disLenSqr > 1024.0f && canDespawn())
			{
				setDead();
			}
			else if (disLenSqr < 1024.0f)
			{
				entityAge = 0;
			}
		}
	}
}

void EntityLiving::updateAITasks()
{
	++entityAge;
	despawnEntity();
	senses.clearSensingCache();
	m_targetTasks->onUpdateTasks();
	m_tasks->onUpdateTasks();
	navigator.onUpdateNavigation();
	updateAITick();
	moveHelper.onUpdateMoveHelper();
	lookHelper.onUpdateLook();
	jumpHelper.doJump();
}

void EntityLiving::updateEntityActionState()
{
	EntityLivingBase::updateEntityActionState();
	moveStrafing = 0.0F;
	moveForward = 0.0F;
	despawnEntity();
	float distance = 8.0F;

	if (rand->nextFloat() < 0.02F)
	{
		EntityPlayer* pPlayer = world->getClosestPlayerToEntity(this, distance);

		if (pPlayer)
		{
			if (!pPlayer->isDead && !pPlayer->isLogout())
			{
				m_currentTargetEntityId = pPlayer->entityId;
			}

			numTicksToChaseTarget = 10 + rand->nextInt(20);
		}
		else
		{
			randomYawVelocity = (rand->nextFloat() - 0.5F) * 20.0F;
		}
	}

	if (m_currentTargetEntityId > 0)
	{
		Entity* currentTarget = world->getEntity(m_currentTargetEntityId);
		if (currentTarget != NULL)
		{
			faceEntity(currentTarget, 10.0F, (float)getVerticalFaceSpeed());

			if (numTicksToChaseTarget-- <= 0 || currentTarget->isDead || currentTarget->getDistanceSqToEntity(this) > (distance * distance))
			{
				m_currentTargetEntityId = 0;
			}
		}
	}
	else
	{
		if (rand->nextFloat() < 0.05F)
		{
			randomYawVelocity = (rand->nextFloat() - 0.5F) * 20.0F;
		}

		rotationYaw += randomYawVelocity;
		rotationPitch = defaultPitch;
	}

	bool inWater = isInWater();
	bool inLava = handleLavaMovement();

	if (inWater || inLava)
	{
		isJumping = rand->nextFloat() < 0.8F;
	}
}

void EntityLiving::faceEntity(Entity* pTarget, float yawBound, float pitchBound)
{
	if (!pTarget)
		return;
	float dx = pTarget->position.x - position.x;
	float dz = pTarget->position.z - position.z;
	float dy = 0.f;

	EntityLivingBase* pLiving = dynamic_cast<EntityLivingBase*>(pTarget);
	if (pLiving)
	{
		// eye to eye!
		dy = pLiving->position.y + pLiving->getEyeHeight() - (position.y + getEyeHeight());
	}
	else
	{
		dy = (pTarget->boundingBox.vMin.y + pTarget->boundingBox.vMax.y) / 2.0f - (position.y + getEyeHeight());
	}

	float dproj_xz = Math::Sqrt(dx * dx + dz * dz);
	float yaw = (Math::ATan2(dz, dx) * 180.0f / Math::PI) - 90.0F;
	float pitch = -(Math::ATan2(dy, dproj_xz) * 180.0f / Math::PI);
	rotationPitch = updateRotation(rotationPitch, pitch, pitchBound);
	rotationYaw = updateRotation(rotationYaw, yaw, yawBound);
}

float EntityLiving::updateRotation(float angle1, float angle2, float angleBound)
{
	float angle = Math::WrapDegree(angle2 - angle1);

	if (angle > angleBound)
	{
		angle = angleBound;
	}

	if (angle < -angleBound)
	{
		angle = -angleBound;
	}

	return angle1 + angle;
}

bool EntityLiving::getCanSpawnHere()
{
	return
		world->checkNoEntityCollision(boundingBox) && 
		world->getCollidingBoundingBoxes(this, boundingBox).empty() &&
		!world->isAnyLiquid(boundingBox);
}

int EntityLiving::getMaxFallHeight()
{
	if (!getAttackTarget())
	{
		return 3;
	}
	else
	{
		int i = (int)(getHealth() - getMaxHealth() * 0.33F);
		i -= (3 - world->m_difficultySetting) * 4;

		if (i < 0)
		{
			i = 0;
		}

		return i + 3;
	}
}

void EntityLiving::dropEquipment(bool wasRecentlyHit, int lootingModifier)
{
	ItemStackArr stacks = getLastActiveItems();
	for (int i = 0; i < int(stacks.size()); ++i)
	{
		ItemStackPtr pStack = getCurrentItemOrArmor(i);
		bool chance = equipmentDropChances[i] > 1.0F;

		if (pStack && (wasRecentlyHit || chance) && rand->nextFloat() - lootingModifier * 0.01F < equipmentDropChances[i])
		{
			if (!chance && pStack->isItemStackDamageable())
			{
				int maxCount = Math::Max(pStack->getMaxDamage() - 25, 1);
				int count = pStack->getMaxDamage() - rand->nextInt(rand->nextInt(maxCount) + 1);

				if (count > maxCount)
				{
					count = maxCount;
				}

				if (count < 1)
				{
					count = 1;
				}

				pStack->setItemDamage(count);
			}

			entityDropItem(pStack, 0.0F);
		}
	}
}

void EntityLiving::addRandomArmor()
{
	/*
	if (rand->nextFloat() < 0.15F * world->calculateDifficulty(position))
	{
		int var1 = rand->nextInt(2);
		float var2 = world->m_difficultySetting == 3 ? 0.1F : 0.25F;

		if (rand->nextFloat() < 0.095F)
		{
			++var1;
		}

		if (rand->nextFloat() < 0.095F)
		{
			++var1;
		}

		if (rand->nextFloat() < 0.095F)
		{
			++var1;
		}

		for (int i = 3; i >= 0; --i)
		{
			ItemStackPtr pStack = getArmorFromSlot(i);

			if (i < 3 && rand->nextFloat() < var2)
			{
				break;
			}

			if (!pStack)
			{
				Item* pItem = getArmorItemForSlot(i + 1, var1);

				if (pItem)
				{
					setCurrentItemOrArmor(i + 1, LORD::make_shared<ItemStack>(pItem));
				}
			}
		}
	}
	*/
}

int EntityLiving::getArmorPosition(ItemStackPtr pStack)
{
	if (pStack->itemID != BLOCK_ID_PUMPKIN && pStack->itemID != Item::skull->itemID)
	{
		ItemArmor* pArmor = dynamic_cast<ItemArmor*>(pStack->getItem());
		if (pArmor)
		{
			switch (pArmor->armorType)
			{
			case 0:
				return 4;

			case 1:
				return 3;

			case 2:
				return 2;

			case 3:
				return 1;
			}
		}

		return 0;
	}
	return 4;
}

Item* EntityLiving::getArmorItemForSlot(int armorSlot, int tier)
{
	switch (armorSlot)
	{
	case 4:
		if (tier == 0)
			return Item::helmetLeather;
		else if (tier == 1)
			return Item::helmetGold;
		else if (tier == 2)
			return Item::helmetChain;
		else if (tier == 3)
			return Item::helmetIron;
		else if (tier == 4)
			return Item::helmetDiamond;

	case 3:
		if (tier == 0)
			return Item::plateLeather;
		else if (tier == 1)
			return Item::plateGold;
		else if (tier == 2)
			return Item::plateChain;
		else if (tier == 3)
			return Item::plateIron;
		else if (tier == 4)
			return Item::plateDiamond;

	case 2:
		if (tier == 0)
			return Item::legsLeather;
		else if (tier == 1)
			return Item::legsGold;
		else if (tier == 2)
			return Item::legsChain;
		else if (tier == 3)
			return Item::legsIron;
		else if (tier == 4)
			return Item::legsDiamond;

	case 1:
		if (tier == 0)
			return Item::bootsLeather;
		else if (tier == 1)
			return Item::bootsGold;
		else if (tier == 2)
			return Item::bootsChain;
		else if (tier == 3)
			return Item::bootsIron;
		else if (tier == 4)
			return Item::bootsDiamond;
	}
	return NULL;
}

void EntityLiving::enchantEquipment()
{
	/*
	float diffulty = world->calculateDifficulty(position);

	if (getHeldItem() && rand->nextFloat() < 0.25F * diffulty)
	{
		EnchantmentHelper::addRandomEnchantment(rand, getHeldItem(), (int)(5.0F + diffulty * rand->nextInt(18)));
	}

	for (int i = 0; i < 4; ++i)
	{
		ItemStackPtr pStack = getArmorFromSlot(i);

		if (pStack && rand->nextFloat() < 0.5F * diffulty)
		{
			EnchantmentHelper::addRandomEnchantment(rand, pStack, (int)(5.0F + diffulty * rand->nextInt(18)));
		}
	}
	*/
}

String EntityLiving::getEntityName()
{
	return hasCustomNameTag() ? getCustomNameTag() : Entity::getEntityName();
}

bool EntityLiving::processInitialInteract(EntityPlayer* pPlayer)
{
	if (getLeashed() && getLeashedToEntity() == pPlayer)
	{
		clearLeashed(true, !pPlayer->capabilities.isCreativeMode);
		return true;
	}
	else
	{
		ItemStackPtr pStack = pPlayer->inventory->getCurrentItem();

		if (pStack && pStack->itemID == Item::lead->itemID && canBeLeashedTo())
		{
			// todo.
			/* EntityTameable* pTameable = dynamic_cast<EntityTameable*>(this);
			if (!(pTameable) || !(pTameable->isTamed()))
			{
				setLeashedToEntity(pPlayer, true);
				--pStack->stackSize;
				return true;
			}

			pTameable->getOwnerName();
			if (pPlayer->getCommandSenderName().equalsIgnoreCase(((EntityTameable)this).getOwnerName()))
			{
				setLeashedToEntity(pPlayer, true);
				--pStack->stackSize;
				return true;
			}*/
		}

		return interact(pPlayer) ? true : Entity::processInitialInteract(pPlayer);
	}
}

void EntityLiving::setLeashedToEntity(Entity* pEntity, bool sendNotification)
{
	isLeashed = true;
	leashedToEntity = pEntity;

	// todo.
	if (!world->m_isClient && sendNotification /*&& worldObj instanceof WorldServer*/)
	{
		// ((WorldServer)worldObj).getEntityTracker().sendPacketToAllPlayersTrackingEntity(this, new Packet39AttachEntity(1, this, leashedToEntity));
	}
}

void EntityLiving::updateLeashedState()
{
	if (leashNBTTag)
	{
		recreateLeash();
	}

	if (isLeashed)
	{
		if (!leashedToEntity || leashedToEntity->isDead)
		{
			clearLeashed(true, true);
		}
	}
}

void EntityLiving::clearLeashed(bool sendPacket, bool dropLead)
{
	if (isLeashed)
	{
		isLeashed = false;
		leashedToEntity = NULL;

		if (!world->m_isClient && dropLead)
		{
			dropItem(Item::lead->itemID, 1);
		}

		// todo.
		if (!world->m_isClient && sendPacket /* && worldObj instanceof WorldServer*/)
		{
			//((WorldServer)worldObj).getEntityTracker().sendPacketToAllPlayersTrackingEntity(this, new Packet39AttachEntity(1, this, (Entity)null));
		}
	}
}

bool EntityLiving::canBeLeashedTo()
{
	// todo.
	return !getLeashed() /*&& !(this instanceof IMob)*/;
}

void EntityLiving::recreateLeash()
{
	if (isLeashed && leashNBTTag)
	{
		if (leashNBTTag->hasKey("UUIDMost") && leashNBTTag->hasKey("UUIDLeast"))
		{
			UUID uuid = UUID(leashNBTTag->getLong("UUIDMost"), leashNBTTag->getLong("UUIDLeast"));
			EntityArr entities = world->getEntitiesWithinAABB(ENTITY_CLASS_LIVING, boundingBox.expland(10.0f, 10.0f, 10.0f));
			for (EntityArr::iterator it = entities.begin(); it != entities.end(); ++it)
			{
				EntityLivingBase* pLiving = dynamic_cast<EntityLivingBase*>(*it);
				if (!pLiving)
					continue;

				if (pLiving->getUniqueID() == uuid)
				{
					leashedToEntity = pLiving;
					break;
				}
			}
		}
		else if (leashNBTTag->hasKey("X") && leashNBTTag->hasKey("Y") && leashNBTTag->hasKey("Z"))
		{
			int ix = leashNBTTag->getInteger("X");
			int iy = leashNBTTag->getInteger("Y");
			int iz = leashNBTTag->getInteger("Z");

			// todo.
			/*
			EntityLeashKnot var4 = EntityLeashKnot.func_110130_b(worldObj, ix, iy, iz);
			if (var4 == null)
			{
				var4 = EntityLeashKnot.func_110129_a(worldObj, ix, iy, iz);
			}
			leashedToEntity = var4;
			*/
		}
		else
		{
			clearLeashed(false, true);
		}
	}

	leashNBTTag = NULL;
}

EntityLivingData* EntityLiving::onInitialSpawn(EntityLivingData* livingdata)
{
	AttributeModifier* m = LordNew AttributeModifier("Random spawn bonus", float(rand->nextGaussian()) * 0.05f, 1);
	m->setNeedFree(true);
	getEntityAttribute(SharedMonsterAttributes::FOLLOW_RANGE)->applyModifier(m);
	return livingdata;
}


}