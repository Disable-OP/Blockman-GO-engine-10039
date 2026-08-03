#include "CombatTracker.h"
#include "BM_TypeDef.h"
#include "EntityLivingBase.h"
#include "EntityPlayer.h"
#include "DamageSource.h"

#include "Block/Block.h"
#include "Block/BlockManager.h"
#include "World/World.h"

namespace BLOCKMAN
{

CombatEntry::CombatEntry(DamageSource* damageSrcIn, int timeIn, float healthAmount, float damageAmount, const String& fallSuffixIn, float fallDistanceIn)
	: damageSrc(damageSrcIn)
	, time(timeIn)
	, damage(damageAmount)
	, health(healthAmount)
	, fallSuffix(fallSuffixIn)
	, fallDistance(fallDistanceIn)
{}

bool CombatEntry::isLivingDamageSrc()
{
	EntityLivingBase* pLiving = dynamic_cast<EntityLivingBase*>(damageSrc->getEntity());
	return pLiving != NULL; 
}

float CombatEntry::getDamageAmount()
{
	return damageSrc == DamageSource::outOfWorld ? Math::MAX_FLOAT : fallDistance;
}

CombatTracker::CombatTracker(EntityLivingBase* fighterIn)
	: fighter(fighterIn)
	, lastDamageTime(0)
	, combatStartTime(0)
	, combatEndTime(0)
	, inCombat(false)
	, takingDamage(false)
{
}

CombatTracker::~CombatTracker()
{
	for (CombatEntries::iterator it = combatEntries.begin(); it != combatEntries.end(); ++it)
	{
		LordDelete *it;
	}
}

void CombatTracker::calculateFallSuffix()
{
	resetFallSuffix();

	if (fighter->isOnLadder())
	{
		int blockID = fighter->world->getBlockId(fighter->position.getFloor());

		if (blockID == BLOCK_ID_LADDER)
			fallSuffix = "ladder";
		else if (blockID == BLOCK_ID_VINE)
			fallSuffix = "vines";
	}
	else if (fighter->isInWater())
	{
		fallSuffix = "water";
	}
}

void CombatTracker::trackDamage(DamageSource* damageSrc, float healthIn, float damageAmount)
{
	reset();
	calculateFallSuffix();
	CombatEntry* combatentry = LordNew CombatEntry(damageSrc, fighter->ticksExisted, healthIn, damageAmount, fallSuffix, fighter->fallDistance);
	combatEntries.push_back(combatentry);
	lastDamageTime = fighter->ticksExisted;
	takingDamage = true;

	if (combatentry->isLivingDamageSrc() && !inCombat && fighter->isEntityAlive())
	{
		inCombat = true;
		combatStartTime = fighter->ticksExisted;
		combatEndTime = combatStartTime;
		// todo.
		// fighter->sendEnterCombat();
	}
}

EntityLivingBase* CombatTracker::getBestAttacker()
{
	EntityLivingBase* entitylivingbase = NULL;
	EntityPlayer* entityplayer = NULL;
	float f = 0.0F;
	float f1 = 0.0F;

	for (CombatEntries::iterator it = combatEntries.begin(); it != combatEntries.end(); ++it)
	{
		CombatEntry* pEntry = *it;
		EntityPlayer* pPlayer = dynamic_cast<EntityPlayer*>(pEntry->getDamageSrc()->getEntity());
		if (pPlayer && (entityplayer == NULL || pEntry->getDamage() > f1))
		{
			f1 = pEntry->getDamage();
			entityplayer = pPlayer;
		}

		EntityLivingBase* pLiving = dynamic_cast<EntityLivingBase*>(pEntry->getDamageSrc()->getEntity());
		if (pLiving && (entitylivingbase == NULL || pEntry->getDamage() > f))
		{
			f = pEntry->getDamage();
			entitylivingbase = pLiving;
		}
	}

	if (entityplayer != NULL && f1 >= f / 3.0F)
	{
		return entityplayer;
	}
	else
	{
		return entitylivingbase;
	}
}

CombatEntry* CombatTracker::getBestCombatEntry()
{
	CombatEntry* combatentry = NULL;
	CombatEntry* combatentry1 = NULL;
	CombatEntry* combatLast = NULL;
	float f = 0.0F;
	float f1 = 0.0F;

	for (CombatEntries::iterator it = combatEntries.begin(); it != combatEntries.end(); ++it)
	{
		CombatEntry* combatentry2 = *it;
		CombatEntry* combatentry3 = combatLast;
		combatLast = combatentry2;

		if ((combatentry2->getDamageSrc() == DamageSource::fall ||
			combatentry2->getDamageSrc() == DamageSource::outOfWorld) &&
			combatentry2->getDamageAmount() > 0.0F && 
			(combatentry == NULL || combatentry2->getDamageAmount() > f1))
		{
			if (combatentry3)
				combatentry = combatentry3;
			else
				combatentry = combatentry2;

			f1 = combatentry2->getDamageAmount();
		}

		if (combatentry2->getFallSuffix() != StringUtil::BLANK &&
			(combatentry1 == NULL || combatentry2->getDamage() > f))
		{
			combatentry1 = combatentry2;
			f = combatentry2->getDamage();
		}
	}

	if (f1 > 5.0F && combatentry != NULL)
	{
		return combatentry;
	}
	else if (f > 5.0F && combatentry1 != NULL)
	{
		return combatentry1;
	}
	return NULL;
}

String CombatTracker::getFallSuffix(CombatEntry* entry)
{
	return entry->getFallSuffix() == StringUtil::BLANK ? "generic" : entry->getFallSuffix();
}

int CombatTracker::getCombatDuration()
{
	return inCombat ? fighter->ticksExisted - combatStartTime : combatEndTime - combatStartTime;
}

void CombatTracker::reset()
{
	int i = inCombat ? 300 : 100;

	if (takingDamage && (!fighter->isEntityAlive() || fighter->ticksExisted - lastDamageTime > i))
	{
		bool flag = inCombat;
		takingDamage = false;
		inCombat = false;
		combatEndTime = fighter->ticksExisted;
		
		for (CombatEntries::iterator it = combatEntries.begin(); it != combatEntries.end(); ++it)
		{
			LordDelete *it;
		}
		combatEntries.clear();
	}
}

}