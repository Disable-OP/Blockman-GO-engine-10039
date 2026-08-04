#include "EntityMob.h"
#include "EntityPlayer.h"
#include "DamageSource.h"
#include "Enchantment.h"

#include "World/World.h"

namespace BLOCKMAN
{

EntityMob::EntityMob(World* pWorld)
: EntityCreature(pWorld)
{
	experienceValue = 5;
}

void EntityMob::onLivingUpdate()
{
	updateArmSwingProgress();

	// todo.
	// in logic side ,there is no birghtness value.
	if (getBrightness(1.0F) > 0.5F)
	{
		entityAge += 2;
	}

	EntityLiving::onLivingUpdate();
}

void EntityMob::onUpdate()
{
	EntityLiving::onUpdate();

	if (!world->m_isClient && world->m_difficultySetting == 0)
	{
		setDead();
	}
}

Entity* EntityMob::findPlayerToAttack()
{
	EntityPlayer* pPlayer = world->getClosestVulnerablePlayerToEntity(this, 16.0f);
	return pPlayer && canEntityBeSeen(pPlayer) ? pPlayer : NULL;
}

bool EntityMob::attackEntityFrom(DamageSource* pSource, float amount)
{
	if (isEntityInvulnerable())
		return false;
	
	if (EntityLivingBase::attackEntityFrom(pSource, amount))
	{
		Entity* attacker = pSource->getEntity();

		if (riddenByEntity != attacker && ridingEntity != attacker)
		{
			if (attacker != this)
			{
				entityToAttack = attacker;
			}

			return true;
		}
		else
		{
			return true;
		}
	}
	return false;
}

bool EntityMob::attackEntityAsMob(Entity* pEntity)
{
	float damage = getEntityAttribute(SharedMonsterAttributes::ATTACK_DAMAGE)->getAttributeValue();
	int knockBack = 0;

	if (pEntity->isClass(ENTITY_CLASS_LIVING_BASE))
	{
		damage += EnchantmentHelper::getEnchantmentModifierLiving(this, (EntityLivingBase*)pEntity);
		knockBack += EnchantmentHelper::getKnockbackModifier(this, (EntityLivingBase*)pEntity);
	}

	bool hited = pEntity->attackEntityFrom(DamageSource::causeMobDamage(this), damage);

	if (hited)
	{
		if (knockBack > 0)
		{
			float vx = -Math::Sin(rotationYaw * Math::DEG2RAD) * knockBack * 0.5F;
			float vz = Math::Cos(rotationYaw * Math::DEG2RAD) * knockBack * 0.5F;
			pEntity->addVelocity(Vector3(vx, 0.1f, vz));
			motion.x *= 0.6f;
			motion.z *= 0.6f;
		}

		int fireAspect = EnchantmentHelper::getFireAspectModifier(this);

		if (fireAspect > 0)
		{
			pEntity->setFire(fireAspect * 4);
		}

		if (pEntity->isClass(ENTITY_CLASS_LIVING_BASE))
		{
			EnchantmentThorns::damageArmor(this, (EntityLivingBase*)pEntity, rand);
		}
	}

	return hited;
}

void EntityMob::attackEntity(Entity* pEntity, float damage)
{
	if (attackTime <= 0 && damage < 2.0F && pEntity->boundingBox.vMax.y > boundingBox.vMin.y && pEntity->boundingBox.vMin.y < boundingBox.vMax.y)
	{
		attackTime = 20;
		attackEntityAsMob(pEntity);
	}
}

float EntityMob::getBlockPathWeight(const BlockPos& pos)
{
	return 0.5f - world->getLightBrightness(pos);
}

bool EntityMob::isValidLightLevel()
{
	BlockPos pos;
	pos.x = int(Math::Floor(position.x));
	pos.y = int(Math::Floor(boundingBox.vMin.y));
	pos.z = int(Math::Floor(position.z));

	if (world->getSavedLightValue(SKY_LIGHT_VALUE, pos) > rand->nextInt(32))
		return false;
	
	int light = world->getBlockLightValue(pos);

	// logic side , no thundering, no rain, no light.
	/*
	if (worldObj->isThundering())
	{
		int var5 = this.worldObj.skylightSubtracted;
		this.worldObj.skylightSubtracted = 10;
		light = this.worldObj.getBlockLightValue(var1, var2, var3);
		this.worldObj.skylightSubtracted = var5;
	}
	*/

	return light <= rand->nextInt(8);
}

bool EntityMob::getCanSpawnHere()
{
	return world->m_difficultySetting > 0 && isValidLightLevel() && EntityCreature::getCanSpawnHere();
}

void EntityMob::applyEntityAttributes()
{
	EntityLiving::applyEntityAttributes();
	attributeMap->registerAttribute(SharedMonsterAttributes::ATTACK_DAMAGE);
}

}