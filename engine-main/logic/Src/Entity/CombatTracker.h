/********************************************************************
filename: 	CombatTracker.h
file path:	dev\client\Src\Blockman\Entity

version:	1
author:		ajohn
company:	supernano
date:		2017-2-14
*********************************************************************/
#ifndef __COMBAT_TRACKER_HEADER__
#define __COMBAT_TRACKER_HEADER__

#include "Core.h"
using namespace LORD;

namespace BLOCKMAN
{

class DamageSource;
class EntityLivingBase;

class CombatEntry : public ObjectAlloc
{
protected:
	DamageSource* damageSrc = nullptr;
	int time = 0;
	float damage = 0.f;
	float health = 0.f;
	String fallSuffix = "";
	float fallDistance = 0.f;

public:
	CombatEntry(DamageSource* damageSrcIn, int timeIn, float healthAmount, float damageAmount, const String& fallSuffixIn, float fallDistanceIn);
	/** Get the DamageSource of the CombatEntry instance. */
	DamageSource* getDamageSrc() { return damageSrc; }
	float getDamage() {	return damage; }
	/** Returns true if {@link net.blockman.util.DamageSource#getEntity() damage source} is a living entity */
	bool isLivingDamageSrc();
	const String& getFallSuffix() { return fallSuffix; }
	/*ITextComponent getDamageSrcDisplayName()
	{
		return this.getDamageSrc().getEntity() == null ? null : this.getDamageSrc().getEntity().getDisplayName();
	}*/
	float getDamageAmount();
	CombatEntry* getBestCombatEntry();
};

typedef list<CombatEntry*>::type CombatEntries;

class CombatTracker : public ObjectAlloc
{
protected:
	CombatEntries combatEntries;
	/** The entity tracked. */
	EntityLivingBase* fighter = nullptr;
	int lastDamageTime = 0;
	int combatStartTime = 0;
	int combatEndTime = 0;
	bool inCombat = false;
	bool takingDamage = false;
	String fallSuffix = "";

public:
	CombatTracker(EntityLivingBase* fighterIn);
	~CombatTracker();

	void calculateFallSuffix();
	void trackDamage(DamageSource* damageSrc, float healthIn, float damageAmount);
	EntityLivingBase* getBestAttacker();
	String getFallSuffix(CombatEntry* entry);
	int getCombatDuration();
	/** Resets this trackers list of combat entries */
	void reset();
	/** Returns EntityLivingBase assigned for this CombatTracker */
	CombatEntry* getBestCombatEntry();
	EntityLivingBase* getFighter() { return fighter; }
	void resetFallSuffix() { fallSuffix = StringUtil::BLANK; }
};


}
#endif