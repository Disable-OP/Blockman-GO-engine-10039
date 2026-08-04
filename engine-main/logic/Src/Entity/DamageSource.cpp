#include "DamageSource.h"
#include "Entity.h"
#include "Explosion.h"
#include "EntityLivingBase.h"
#include "EntityPlayer.h"
#include "Item/ItemStack.h"
#include "BM_TypeDef.h"

namespace BLOCKMAN
{

DamageSource* DamageSource::inFire			= NULL;		
DamageSource* DamageSource::onFire			= NULL;		
DamageSource* DamageSource::lava			= NULL;		
DamageSource* DamageSource::inWall			= NULL;		
DamageSource* DamageSource::drown			= NULL;		
DamageSource* DamageSource::starve			= NULL;		
DamageSource* DamageSource::cactus			= NULL;		
DamageSource* DamageSource::fall			= NULL;		
DamageSource* DamageSource::outOfWorld		= NULL;	
DamageSource* DamageSource::generic			= NULL;	
DamageSource* DamageSource::magic			= NULL;		
DamageSource* DamageSource::wither			= NULL;		
DamageSource* DamageSource::anvil			= NULL;			
DamageSource* DamageSource::fallingBlock	= NULL;	

void DamageSource::initialize()
{
	inFire		= LordNew DamageSource("inFire");		inFire->setFireDamage();
	onFire		= LordNew DamageSource("onFire");		onFire->setDamageBypassesArmor();		onFire->setFireDamage();
	lava		= LordNew DamageSource("lava");			lava->setFireDamage();
	inWall		= LordNew DamageSource("inWall");		inWall->setDamageBypassesArmor();
	drown		= LordNew DamageSource("drown");		drown->setDamageBypassesArmor();
	starve		= LordNew DamageSource("starve");		starve->setDamageBypassesArmor();
	cactus		= LordNew DamageSource("cactus");
	fall		= LordNew DamageSource("fall");			fall->setDamageBypassesArmor();
	outOfWorld	= LordNew DamageSource("outOfWorld");	outOfWorld->setDamageBypassesArmor();	outOfWorld->setDamageAllowedInCreativeMode();
	generic		= LordNew DamageSource("generic");		generic->setDamageBypassesArmor();
	magic		= LordNew DamageSource("magic");		magic->setDamageBypassesArmor();		magic->setMagicDamage();
	wither		= LordNew DamageSource("wither");		wither->setDamageBypassesArmor();
	anvil		= LordNew DamageSource("anvil");		
	fallingBlock= LordNew DamageSource("fallingBlock");	
}

void DamageSource::unInitialize()
{
	LordSafeDelete(inFire);
	LordSafeDelete(onFire);
	LordSafeDelete(lava);
	LordSafeDelete(inWall);
	LordSafeDelete(drown);
	LordSafeDelete(starve);
	LordSafeDelete(cactus);
	LordSafeDelete(fall);
	LordSafeDelete(outOfWorld);
	LordSafeDelete(generic);
	LordSafeDelete(magic);
	LordSafeDelete(wither);
	LordSafeDelete(anvil);
	LordSafeDelete(fallingBlock);
}

DamageSource::DamageSource(const String& type)
	: damageType(type)
	, m_isUnblockable(false)
	, isDamageAllowedInCreativeMode(false)
	, hungerDamage(0.3f)
	, fireDamage(false)
	, projectile(false)
	, difficultyScaled(false)
	, magicDamage(false)
	, explosion(false)
	, knockBackDis(0.4f)
{
}

DamageSource* DamageSource::causeMobDamage(EntityLivingBase* pLiving)
{
	return LordNew EntityDamageSource("mob", (Entity*)pLiving);
}

DamageSource* DamageSource::causePlayerDamage(EntityPlayer* pPlayer)
{
	return LordNew EntityDamageSource("player", (Entity*)pPlayer);
}

DamageSource* DamageSource::causeArrowDamage(EntityArrow* pArrow, Entity* pEntity)
{
	DamageSource* pResult = LordNew EntityDamageSourceIndirect("arrow", (Entity*)pArrow, pEntity);
	pResult->setProjectile();
	return pResult;
}

DamageSource* DamageSource::causeBulletDamage(EntityBullet* pBullet, Entity* pEntity)
{
	DamageSource* pResult = LordNew EntityDamageSourceIndirect("bullet", (Entity*)pBullet, pEntity);
	pResult->setProjectile();
	return pResult;
}

DamageSource* DamageSource::causeFireballDamage(EntityFireball* pFireball, Entity* pEntity)
{
	DamageSource* pResult = NULL;
	if (pEntity == NULL)
	{
		pResult = LordNew EntityDamageSourceIndirect("onFire", (Entity*)pFireball, (Entity*)pFireball);
		pResult->setFireDamage();
		pResult->setProjectile();
	}
	else
	{
		pResult = LordNew EntityDamageSourceIndirect("fireball", (Entity*)pFireball, pEntity);
		pResult->setFireDamage();
		pResult->setProjectile();
	}

	return pResult;
}

DamageSource* DamageSource::causeThrownDamage(Entity* pEntity1, Entity* pEntity2)
{
	DamageSource* pResult = LordNew EntityDamageSourceIndirect("thrown", pEntity1, pEntity2);
	pResult->setProjectile();
	return pResult;
}

DamageSource* DamageSource::causeIndirectMagicDamage(Entity* pEntity1, Entity* pEntity2)
{
	DamageSource* pResult = LordNew EntityDamageSourceIndirect("indirectMagic", pEntity1, pEntity2);
	pResult->setDamageBypassesArmor();
	pResult->setMagicDamage();
	return pResult;
}

DamageSource* DamageSource::causeThornsDamage(Entity* pEntity)
{
	DamageSource* pResult = LordNew EntityDamageSource("thorns", pEntity);
	pResult->setMagicDamage();
	return pResult;
}

DamageSource* DamageSource::setExplosionSource(Explosion* pExplosion)
{
	DamageSource* pResult = NULL;
	if (pExplosion && pExplosion->getExplosivePlacedBy())
		pResult = LordNew EntityDamageSource("explosion.player", (Entity*)(pExplosion->getExplosivePlacedBy()));
	else
		pResult = LordNew DamageSource("explosion");

	pResult->setDifficultyScaled();
	pResult->setExplosion();
	return pResult;
}

DamageSource * DamageSource::causePlayerGunDamage(EntityPlayer * pPlayer, bool isHeadshot)
{
	if (isHeadshot)
		return LordNew EntityDamageSource("player.gun.headshot", (Entity*)pPlayer);
	else
		return LordNew EntityDamageSource("player.gun", (Entity*)pPlayer);
}

EntityDamageSource::EntityDamageSource(const String& type, Entity* pEntity)
	: DamageSource(type)
	, damageSourceEntity(pEntity)
{
}

bool EntityDamageSource::isDifficultyScaled()
{
	if (!damageSourceEntity)
		return false;
	EntityLivingBase* pLiving = dynamic_cast<EntityLivingBase*>(damageSourceEntity);
	EntityPlayer* pPlayer = dynamic_cast<EntityPlayer*>(damageSourceEntity);
	return pLiving && !pPlayer;
}

EntityDamageSourceIndirect::EntityDamageSourceIndirect(const String& type, Entity* pEntity1, Entity* pEntity2)
	: EntityDamageSource(type, pEntity1)
	, indirectEntity(pEntity2)
{
}

}