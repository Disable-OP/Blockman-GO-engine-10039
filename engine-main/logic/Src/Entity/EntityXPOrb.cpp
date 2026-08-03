#include "EntityXPOrb.h"
#include "EntityPlayer.h"
#include "DamageSource.h"

#include "Block/BM_Material.h"
#include "Block/Block.h"
#include "Block/BlockManager.h"
#include "World/World.h"

namespace BLOCKMAN
{



int EntityXPOrb::xp_table[10] = { 3, 7, 17, 37, 73, 149, 307, 617, 1237, 2477 };

EntityXPOrb::EntityXPOrb(World* pWorld, const Vector3& pos, int xp)
	: Entity(pWorld)
{
	xpOrbHealth = 5;
	closestPlayer = NULL;
	xpTargetColor = 0;
	xpColor = 0;
	xpOrbAge = 0;
	delayBeforeCanPickup = 0;

	setSize(0.5F, 0.5F);
	yOffset = height / 2.0F;
	setPosition(pos);
	rotationYaw = (float)(Math::UnitRandom() * 360.0f);
	motion.x = (Math::UnitRandom() * 0.2f - 0.1f) * 2.0f;
	motion.y = (Math::UnitRandom() * 0.2f) * 2.0f;
	motion.z = (Math::UnitRandom() * 0.2f - 0.1f) * 2.0f;
	xpValue = xp;
}

EntityXPOrb::EntityXPOrb(World* pWorld)
	: Entity(pWorld)
{
	xpOrbHealth = 5;
	xpValue = 0;
	closestPlayer = NULL;
	xpTargetColor = 0;
	xpColor = 0;
	xpOrbAge = 0;
	delayBeforeCanPickup = 0;

	setSize(0.25F, 0.25F);
	yOffset = height / 2.0F;
}

int EntityXPOrb::getBrightnessForRender(float rdt)
{
	float var2 = 0.5F;

	if (var2 < 0.0F)
	{
		var2 = 0.0F;
	}

	if (var2 > 1.0F)
	{
		var2 = 1.0F;
	}

	int var3 = Entity::getBrightnessForRender(rdt);
	int var4 = var3 & 255;
	int var5 = var3 >> 16 & 255;
	var4 += (int)(var2 * 15.0F * 16.0F);

	if (var4 > 240)
	{
		var4 = 240;
	}

	return var4 | var5 << 16;
}

void EntityXPOrb::onUpdate()
{
	Entity::onUpdate();

	if (delayBeforeCanPickup > 0)
	{
		--delayBeforeCanPickup;
	}

	prevPos = position;
	motion.y -= 0.03f;

	BlockPos pos = position.getFloor();
	if (world->getBlockMaterial(pos) == BM_Material::BM_MAT_lava)
	{
		motion.y = 0.2f;
		motion.x = (rand->nextFloat() - rand->nextFloat()) * 0.2F;
		motion.z = (rand->nextFloat() - rand->nextFloat()) * 0.2F;
		//playSound("random.fizz", 0.4F, 2.0F + rand->nextFloat() * 0.4F);
		playSoundByType(ST_Fizz);
	}

	pushOutOfBlocks(getCentorPos());
	float mindis = 8.0f;

	if (xpTargetColor < xpColor - 20 + entityId % 100)
	{
		if (!closestPlayer || closestPlayer->getDistanceSqToEntity(this) > mindis * mindis)
		{
			closestPlayer = world->getClosestPlayerToEntity(this, mindis);
		}

		xpTargetColor = xpColor;
	}

	if (closestPlayer)
	{
		float var3 = (closestPlayer->position.x - position.x) / mindis;
		float var5 = (closestPlayer->position.y + closestPlayer->getEyeHeight() - position.y) / mindis;
		float var7 = (closestPlayer->position.z - position.z) / mindis;
		float var9 = Math::Sqrt(var3 * var3 + var5 * var5 + var7 * var7);
		float var11 = 1.0f - var9;

		if (var11 > 0.0f)
		{
			var11 *= var11;
			motion.x += var3 / var9 * var11 * 0.1f;
			motion.y += var5 / var9 * var11 * 0.1f;
			motion.z += var7 / var9 * var11 * 0.1f;
		}
	}

	moveEntity(motion);
	float g = 0.98F;

	if (onGround)
	{
		g = 0.58800006F;
		pos.y = int(Math::Floor(boundingBox.vMin.y)) - 1;
		int blockid = world->getBlockId(pos);

		if (blockid > 0)
		{
			g = BlockManager::sBlocks[blockid]->slipperiness() * 0.98F;
		}
	}

	motion.x *= g;
	motion.y *= 0.98f;
	motion.z *= g;

	if (onGround)
	{
		motion.y *= -0.9f;
	}

	++xpColor;
	++xpOrbAge;

	if (xpOrbAge >= 6000)
	{
		setDead();
	}
}

bool EntityXPOrb::handleWaterMovement()
{
	return world->handleMaterialAcceleration(boundingBox, BM_Material::BM_MAT_water, this);
}

void EntityXPOrb::dealFireDamage(int amount)
{
	attackEntityFrom(DamageSource::inFire, (float)amount);
}

bool EntityXPOrb::attackEntityFrom(DamageSource* pSource, float damage)
{
	if (isEntityInvulnerable())
	{
		return false;
	}
	else
	{
		setBeenAttacked();
		xpOrbHealth = (int)((float)xpOrbHealth - damage);

		if (xpOrbHealth <= 0)
		{
			setDead();
		}

		return false;
	}
}

void EntityXPOrb::writeEntityToNBT(NBTTagCompound* pNbtCompound)
{
	pNbtCompound->setShort("Health", (i16)((i8)xpOrbHealth));
	pNbtCompound->setShort("Age", (i16)xpOrbAge);
	pNbtCompound->setShort("Value", (i16)xpValue);
}

void EntityXPOrb::readEntityFromNBT(NBTTagCompound* pNbtCompound)
{
	xpOrbHealth = pNbtCompound->getShort("Health") & 255;
	xpOrbAge = pNbtCompound->getShort("Age");
	xpValue = pNbtCompound->getShort("Value");
}

void EntityXPOrb::onCollideWithPlayer(EntityPlayer* pPlayer)
{
	if (world->m_isClient)
		return;

	if (delayBeforeCanPickup == 0 && pPlayer->xpCooldown == 0)
	{
		pPlayer->xpCooldown = 2;
		//playSound("random.orb", 0.1F, 0.5F * ((rand->nextFloat() - rand->nextFloat()) * 0.7F + 1.8F));
		playSoundByType(ST_Orb);
		pPlayer->onItemPickup(this, 1);
		pPlayer->addExperience(xpValue);
		setDead();
	}
}

int EntityXPOrb::getTextureByXP()
{
	for (int i = 0; i < (sizeof(xp_table)/ sizeof(xp_table[0])); ++i)
		if (xpValue < xp_table[i])
			return i;
	return (sizeof(xp_table) / sizeof(xp_table[0]));
}

int EntityXPOrb::getXPSplit(int par0)
{
	for (int i = (sizeof(xp_table) / sizeof(xp_table[0])) - 1; i >= 0; --i)
		if (par0 >= xp_table[i])
			return xp_table[i];

	return 1;
}

}