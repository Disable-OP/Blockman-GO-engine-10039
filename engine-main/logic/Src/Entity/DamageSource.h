/********************************************************************
filename: 	DamageSource.h
file path:	dev\client\Src\Blockman\Entity

version:	1
author:		ajohn
company:	supernano
date:		2017-2-8
*********************************************************************/
#ifndef __DAMAGE_SOURCE_HEADER__
#define __DAMAGE_SOURCE_HEADER__

#include "Core.h"
using namespace LORD;

namespace BLOCKMAN
{

class EntityLivingBase;
class EntityPlayer;
class EntityArrow;
class EntityBullet;
class EntityFireball;
class Explosion;
class Entity;
class ChatMessageComponent;

class DamageSource : public ObjectAlloc
{
public:
	static DamageSource* inFire;		// = (new DamageSource("inFire")).setFireDamage();
	static DamageSource* onFire;		// = (new DamageSource("onFire")).setDamageBypassesArmor().setFireDamage();
	static DamageSource* lava;			// = (new DamageSource("lava")).setFireDamage();
	static DamageSource* inWall;		// = (new DamageSource("inWall")).setDamageBypassesArmor();
	static DamageSource* drown;			// = (new DamageSource("drown")).setDamageBypassesArmor();
	static DamageSource* starve;		// = (new DamageSource("starve")).setDamageBypassesArmor();
	static DamageSource* cactus;		// = new DamageSource("cactus");
	static DamageSource* fall;			// = (new DamageSource("fall")).setDamageBypassesArmor();
	static DamageSource* outOfWorld;	// = (new DamageSource("outOfWorld")).setDamageBypassesArmor().setDamageAllowedInCreativeMode();
	static DamageSource* generic;		// = (new DamageSource("generic")).setDamageBypassesArmor();
	static DamageSource* magic;			// = (new DamageSource("magic")).setDamageBypassesArmor().setMagicDamage();
	static DamageSource* wither;		// = (new DamageSource("wither")).setDamageBypassesArmor();
	static DamageSource* anvil;			// = new DamageSource("anvil");
	static DamageSource* fallingBlock;	// = new DamageSource("fallingBlock");

protected:
	/** This kind of damage can be blocked or not. */
	bool	m_isUnblockable = false;
	bool	isDamageAllowedInCreativeMode = false;
	float	hungerDamage = 0.f;
	/** This kind of damage is based on fire or not. */
	bool	fireDamage = false;
	/** This kind of damage is based on a projectile or not. */
	bool	projectile = false;
	/** Whether this damage source will have its damage amount scaled based on the current difficulty. */
	bool	difficultyScaled = false;
	bool	magicDamage = false;
	bool	explosion = false;

protected:
	
	DamageSource(const String& type);
	void setDamageBypassesArmor() { m_isUnblockable = true; hungerDamage = 0.0F; }
	void setDamageAllowedInCreativeMode() { isDamageAllowedInCreativeMode = true; }
	void setFireDamage() { fireDamage = true; }

public:
	String damageType = "";
	float  knockBackDis;

	static void initialize();
	static void unInitialize();

	static DamageSource* causeMobDamage(EntityLivingBase* pLiving);
	static DamageSource* causePlayerDamage(EntityPlayer* pPlayer);
	static DamageSource* causeArrowDamage(EntityArrow* pArrow, Entity* pEntity);
	static DamageSource* causeBulletDamage(EntityBullet* pBullet, Entity* pEntity);
	static DamageSource* causeFireballDamage(EntityFireball* pFireball, Entity* pEntity);
	static DamageSource* causeThrownDamage(Entity* pEntity1, Entity* pEntity2);
	static DamageSource* causeIndirectMagicDamage(Entity* pEntity1, Entity* pEntity2);
	static DamageSource* causeThornsDamage(Entity* pEntity);
	static DamageSource* setExplosionSource(Explosion* pExplosion);
	static DamageSource* causePlayerGunDamage(EntityPlayer* pPlayer, bool isHeadshot);

	Entity* getSourceOfDamage() { return getEntity(); }
	virtual Entity* getEntity() { return NULL; }
	virtual bool isDifficultyScaled() { return difficultyScaled; }

	bool isProjectile		()	{ return projectile; }
	bool isExplosion		()	{ return explosion; }
	bool canHarmInCreative	()	{ return isDamageAllowedInCreativeMode; }
	bool isFireDamage		()	{ return fireDamage; }
	bool isMagicDamage		()	{ return magicDamage; }
	bool isUnblockable		()	{ return m_isUnblockable; }
	float getHungerDamage	()	{ return hungerDamage; }
	const String& getDamageType() { return damageType; }

	void setProjectile		()	{ projectile = true; }
	void setExplosion		()	{ explosion = true; }
	void setDifficultyScaled()	{ difficultyScaled = true; }
	void setMagicDamage		()	{ magicDamage = true; }
};

class EntityDamageSource : public DamageSource
{
protected:
	Entity* damageSourceEntity = nullptr;

public:
	EntityDamageSource(const String& type, Entity* pEntity);
	virtual Entity* getEntity() { return damageSourceEntity; }
	virtual bool isDifficultyScaled();
};

class EntityDamageSourceIndirect : public EntityDamageSource
{
protected:
	Entity* indirectEntity = nullptr;

public:
	EntityDamageSourceIndirect(const String& type, Entity* pEntity1, Entity* pEntity2);
	virtual Entity* getEntity() { return indirectEntity; }
	Entity* getSourceOfDamage() { return damageSourceEntity; }
};

}
#endif