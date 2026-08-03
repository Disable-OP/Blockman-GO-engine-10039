/********************************************************************
filename: 	EntityBullet.h
file path:	dev\client\Src\Blockman\Entity

version:	1
author:		ajohn
company:	supernano
date:		2018-01-09
*********************************************************************/
#ifndef __ENTITY_BULLET_HEADER__
#define __ENTITY_BULLET_HEADER__

#include "Entity.h"
#include "Item/Potion.h"

namespace BLOCKMAN
{

class World;
class EntityLivingBase;
class GunSetting;

class EntityBullet : public Entity , public IProjectile
{
	RTTI_DECLARE(EntityBullet);

protected:
	BlockPos m_blockPos;
	int m_bulletID = 185; // item id.
	int m_gunID;
	int ticksInAir = 0;
	float damage = 2.f; // = 2.0D;
	/** The amount of knockback an bullet applies when it hits a mob. */
	int knockbackStrength = 0;
	float m_shootingRange = 500.f;

public:
	/** The owner of this bullet. */
	int     m_shootingEntityId = 0;
	Entity* getShootingEntity();
	
protected:
	void init_entityBullet();

	/** implement override functions from Entity */
	virtual void entityInit() { }
	virtual bool canTriggerWalking() { return false; }

	void onBulletHit(EntityLivingBase* pLiving);

public:
	EntityBullet(World* pWorld);
	EntityBullet(World* pWorld, EntityLivingBase* pShooting, const Vector3 & playerPos, float playerYaw, float playerPitch, int bulletID, const GunSetting* pSetting);
	~EntityBullet()
	{
	}
	float getShadowSize() { return 0.0F; }
	void setDamage(float d) { damage = d; }
	float getDamage() { return damage; }
	void setKnockbackStrength(int s) { knockbackStrength = s; }
	bool canAttackWithItem() { return false; }
	void setBulletID(int id) { m_bulletID = id; }
	int getBulletID() const { return m_bulletID; }
	void setGunID(int id) { m_gunID = id; }
	int getGunID() const { return m_gunID; }
	void setShootingRange(float d) { m_shootingRange = d; };

	/** implement override functions from IProjectile */
	virtual void setThrowableHeading(const Vector3& pos, float velocity, float inaccuracy);
	virtual void setPositionAndRotation2(const Vector3& pos, float yaw, float pitch, int increment);
	virtual void setVelocity(const Vector3& pos);
	virtual void onUpdate();
	virtual void writeEntityToNBT(NBTTagCompound* pNbtbullet);
	virtual void readEntityFromNBT(NBTTagCompound* pNbtbullet);
	virtual void onCollideWithPlayer(EntityPlayer* pPlayer);
};


}

#endif