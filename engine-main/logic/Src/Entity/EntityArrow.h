/********************************************************************
filename: 	EntityArrow.h
file path:	dev\client\Src\Blockman\Entity

version:	1
author:		ajohn
company:	supernano
date:		2017-3-3
*********************************************************************/
#ifndef __ENTITY_ARROW_HEADER__
#define __ENTITY_ARROW_HEADER__

#include "Entity.h"
#include "Item/Potion.h"

namespace BLOCKMAN
{

class World;
class EntityLivingBase;

class EntityArrow : public Entity, public IProjectile
{
	RTTI_DECLARE(EntityArrow);

protected:
	BlockPos m_blockPos;
	int m_arrowID = 262; // item id.
	int inTile = 0;
	int inData = 0;
	bool inGround = false;
	int ticksInGround = 0;
	int ticksInAir = 0;
	float damage = 2.f; // = 2.0D;
	/** The amount of knockback an arrow applies when it hits a mob. */
	int knockbackStrength = 0;

	// data
	i8 data_critical = 0;

	bool m_fromInfiniteEnchantedBow = false;

public:
	/** 1 if the player can pick up the arrow */
	int canBePickedUp = 0;
	/** Seems to be some sort of timer for animating an arrow. */
	int arrowShake = 0;
	/** The owner of this arrow. */
	int m_shootingEntityId = 0;
	PotionEffect* m_potionEffect = nullptr;
	Item* m_arrowPotionItem = nullptr;
	void setPotionEffectAndItem(PotionEffect* effect, Item* arrowPotionItem) {
		m_potionEffect = effect;
		m_arrowPotionItem = arrowPotionItem;
	}

	void setFromInfiniteEnchantedBow(bool value) {
		m_fromInfiniteEnchantedBow = value;
	}

	Entity* getShootingEntity();
	void setArrowID(int id) { m_arrowID = id; }
	int getArrowID() const { return m_arrowID; }

protected:
	void init_entityArrow();

	/** implement override functions from Entity */
	virtual void entityInit() { data_critical = 0; }
	virtual bool canTriggerWalking() { return false; }

	void onArrowHit(EntityLivingBase* pLiving);

public:
	EntityArrow(World* pWorld);
	EntityArrow(World* pWorld, const Vector3& pos);
	EntityArrow(World* pWorld, EntityLivingBase* pShooting, EntityLivingBase* pTarget, float velocity, float inaccuracy);
	EntityArrow(World* pWorld, EntityLivingBase* pShooting, float velocity);

//	EntityArrow(World * pWorld, EntityLivingBase* pShooting, const Vector3 & playerPos, float playerYaw, float playerPitch, float cdTime);
	EntityArrow(World * pWorld, EntityLivingBase* pShooting, const Vector3 & playerPos, float playerYaw, float playerPitch, float cdTime,int arrowID = 262);

	~EntityArrow() {
		inGround = false;
		LordSafeDelete(m_potionEffect);
	}
	float getShadowSize() { return 0.0F; }
	void setDamage(float d) { damage = d; }
	float getDamage() { return damage; }
	void setKnockbackStrength(int s) { knockbackStrength = s; }
	bool canAttackWithItem() { return false; }
	/** Whether the arrow has a stream of critical hit particles flying behind it. */
	void setIsCritical(bool critical);
	/** Whether the arrow has a stream of critical hit particles flying behind it. */
	bool getIsCritical() { return (data_critical & 1) != 0; }
	void setOnFire(bool onFire) { setFlag(ENTITY_STATUS_BURNING, onFire); }

	/** implement override functions from IProjectile */
	virtual void setThrowableHeading(const Vector3& pos, float velocity, float inaccuracy);
	virtual void setPositionAndRotation2(const Vector3& pos, float yaw, float pitch, int increment);
	virtual void setVelocity(const Vector3& pos);
	virtual void onUpdate();
	virtual void writeEntityToNBT(NBTTagCompound* pNbtArrow);
	virtual void readEntityFromNBT(NBTTagCompound* pNbtArrow);
	virtual void onCollideWithPlayer(EntityPlayer* pPlayer);

private:
	void spawnPotionParticles(int times);
};


}

#endif