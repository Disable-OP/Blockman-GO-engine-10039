
#include "Entity/Attributes.h"
#include "Entity/CombatTracker.h"
#include "Entity/DamageSource.h"
#include "Entity/Enchantment.h"
#include "Entity/Entity.h"
#include "Entity/EntityArrow.h"
#include "Entity/EntityCreature.h"
#include "Entity/EntityItem.h"
#include "Entity/EntityLiving.h"
#include "Entity/EntityLivingBase.h"
#include "Entity/EntityMob.h"
#include "Entity/EntityPlayer.h"
#include "Entity/EntityXPOrb.h"
#include "Entity/Explosion.h"
#include "Entity/PathFinder.h"
#include "Entity/PathNavigate.h"
#include "Entity/EntityThrowable.h"
#include "Entity/EntitySkillThrowable.h"
#include "LuaRegister/Template/LuaRegister.h"
#include "World/RayTracyResult.h"

using namespace BLOCKMAN;

using namespace LUA_REGISTER;


/*
// Register Class AttributeMap
_BEGIN_REGISTER_CLASS(AttributeMap)
_CLASSREGISTER_AddBaseClass(IAttributeMap)
_CLASSREGISTER_AddMember(getAttributeInstance, AttributeMap::getAttributeInstance)
_CLASSREGISTER_AddMember(getAttributeInstanceByName, AttributeMap::getAttributeInstanceByName)
_CLASSREGISTER_AddMember(getAttributeInstanceSet, AttributeMap::getAttributeInstanceSet)
_CLASSREGISTER_AddMember(getWatchedAttributes, AttributeMap::getWatchedAttributes)
_CLASSREGISTER_AddMember(onAttributeModified, AttributeMap::onAttributeModified)
_CLASSREGISTER_AddMember(registerAttribute, AttributeMap::registerAttribute)
_END_REGISTER_CLASS()
*/




// Register Class AttributeModifier
_BEGIN_REGISTER_CLASS(AttributeModifier)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddCtor(const String & , float , int )
_CLASSREGISTER_AddCtor(const UUID & , const String & , float , int )
_CLASSREGISTER_AddMember(getAmount, AttributeModifier::getAmount)
_CLASSREGISTER_AddMember(getID, AttributeModifier::getID)
_CLASSREGISTER_AddMember(getName, AttributeModifier::getName)
_CLASSREGISTER_AddMember(getOperation, AttributeModifier::getOperation)
_CLASSREGISTER_AddMember(hashCode, AttributeModifier::hashCode)
_CLASSREGISTER_AddMember(isSaved, AttributeModifier::isSaved)
_CLASSREGISTER_AddMember(setSaved, AttributeModifier::setSaved)
_CLASSREGISTER_AddMember(toString, AttributeModifier::toString)
_END_REGISTER_CLASS()




// Register Class BaseAttribute
_BEGIN_REGISTER_CLASS(BaseAttribute)
_CLASSREGISTER_AddBaseClass(IAttribute)
_CLASSREGISTER_AddMember(getDefaultValue, BaseAttribute::getDefaultValue)
//_CLASSREGISTER_AddMember(getName, BaseAttribute::getName)
_CLASSREGISTER_AddMember(getParent, BaseAttribute::getParent)
_CLASSREGISTER_AddMember(getShouldWatch, BaseAttribute::getShouldWatch)
_CLASSREGISTER_AddMember(setShouldWatch, BaseAttribute::setShouldWatch)
_END_REGISTER_CLASS()


/* temp 
// Register Class IAttributeMap
_BEGIN_REGISTER_CLASS(IAttributeMap)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddMember(applyAttributeModifiers, IAttributeMap::applyAttributeModifiers)
_CLASSREGISTER_AddMember(getAllAttributes, IAttributeMap::getAllAttributes)
_CLASSREGISTER_AddMember(getAttributeInstance, IAttributeMap::getAttributeInstance)
_CLASSREGISTER_AddMember(getAttributeInstanceByName, IAttributeMap::getAttributeInstanceByName)
_CLASSREGISTER_AddMember(onAttributeModified, IAttributeMap::onAttributeModified)
_CLASSREGISTER_AddMember(registerAttribute, IAttributeMap::registerAttribute)
_CLASSREGISTER_AddMember(removeAttributeModifiers, IAttributeMap::removeAttributeModifiers)
_END_REGISTER_CLASS()
*/




/* temp 
// Register Class ModifiableAttributeInstance
_BEGIN_REGISTER_CLASS(ModifiableAttributeInstance)
_CLASSREGISTER_AddBaseClass(IAttributeInstance)
_CLASSREGISTER_AddCtor(IAttributeMap * , IAttribute * )
_CLASSREGISTER_AddMember(applyModifier, ModifiableAttributeInstance::applyModifier)
_CLASSREGISTER_AddMember(computeValue, ModifiableAttributeInstance::computeValue)
_CLASSREGISTER_AddMember(flagForUpdate, ModifiableAttributeInstance::flagForUpdate)
_CLASSREGISTER_AddMember(getAppliedModifiers, ModifiableAttributeInstance::getAppliedModifiers)
_CLASSREGISTER_AddMember(getAttribute, ModifiableAttributeInstance::getAttribute)
_CLASSREGISTER_AddMember(getAttributeValue, ModifiableAttributeInstance::getAttributeValue)
_CLASSREGISTER_AddMember(getBaseValue, ModifiableAttributeInstance::getBaseValue)
_CLASSREGISTER_AddMember(getModifier, ModifiableAttributeInstance::getModifier)
_CLASSREGISTER_AddMember(getModifiers, ModifiableAttributeInstance::getModifiers)
_CLASSREGISTER_AddMember(getModifiersByOperation, ModifiableAttributeInstance::getModifiersByOperation)
_CLASSREGISTER_AddMember(hasModifier, ModifiableAttributeInstance::hasModifier)
_CLASSREGISTER_AddMember(removeAllModifiers, ModifiableAttributeInstance::removeAllModifiers)
_CLASSREGISTER_AddMember_Override(removeModifier, ModifiableAttributeInstance::removeModifier,void ,AttributeModifier * )
_CLASSREGISTER_AddMember_Override(removeModifier1, ModifiableAttributeInstance::removeModifier,void ,const UUID & )
_CLASSREGISTER_AddMember(setBaseValue, ModifiableAttributeInstance::setBaseValue)
_END_REGISTER_CLASS()
*/




// Register Class RangedAttribute
_BEGIN_REGISTER_CLASS(RangedAttribute)
_CLASSREGISTER_AddBaseClass(BaseAttribute)
_CLASSREGISTER_AddCtor(IAttribute * , const String & , float , float , float )
_CLASSREGISTER_AddMember(clampValue, RangedAttribute::clampValue)
_CLASSREGISTER_AddMember(getDescription, RangedAttribute::getDescription)
_CLASSREGISTER_AddMember(setDescription, RangedAttribute::setDescription)
_END_REGISTER_CLASS()




// Register Class SharedMonsterAttributes
_BEGIN_REGISTER_CLASS(SharedMonsterAttributes)
_CLASSREGISTER_AddStaticMember(initialize, SharedMonsterAttributes::initialize)
_CLASSREGISTER_AddStaticMember(readAttributeModifierFromNBT, SharedMonsterAttributes::readAttributeModifierFromNBT)
_CLASSREGISTER_AddStaticMember(setAttributeModifiers, SharedMonsterAttributes::setAttributeModifiers)
_CLASSREGISTER_AddStaticMember(unInitialize, SharedMonsterAttributes::unInitialize)
_CLASSREGISTER_AddStaticMember(writeAttributeModifierToNBT, SharedMonsterAttributes::writeAttributeModifierToNBT)
_CLASSREGISTER_AddStaticMember(writeBaseAttributeMapToNBT, SharedMonsterAttributes::writeBaseAttributeMapToNBT)
_END_REGISTER_CLASS()




// Register Class CombatEntry
_BEGIN_REGISTER_CLASS(CombatEntry)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddCtor(DamageSource * , int , float , float , const String & , float )
//_CLASSREGISTER_AddMember(getBestCombatEntry, CombatEntry::getBestCombatEntry)
_CLASSREGISTER_AddMember(getDamage, CombatEntry::getDamage)
_CLASSREGISTER_AddMember(getDamageAmount, CombatEntry::getDamageAmount)
_CLASSREGISTER_AddMember(getDamageSrc, CombatEntry::getDamageSrc)
_CLASSREGISTER_AddMember(getFallSuffix, CombatEntry::getFallSuffix)
_CLASSREGISTER_AddMember(isLivingDamageSrc, CombatEntry::isLivingDamageSrc)
_END_REGISTER_CLASS()




// Register Class CombatTracker
_BEGIN_REGISTER_CLASS(CombatTracker)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddCtor(EntityLivingBase * )
_CLASSREGISTER_AddMember(calculateFallSuffix, CombatTracker::calculateFallSuffix)
_CLASSREGISTER_AddMember(getBestAttacker, CombatTracker::getBestAttacker)
_CLASSREGISTER_AddMember(getBestCombatEntry, CombatTracker::getBestCombatEntry)
_CLASSREGISTER_AddMember(getCombatDuration, CombatTracker::getCombatDuration)
_CLASSREGISTER_AddMember(getFallSuffix, CombatTracker::getFallSuffix)
_CLASSREGISTER_AddMember(getFighter, CombatTracker::getFighter)
_CLASSREGISTER_AddMember(reset, CombatTracker::reset)
_CLASSREGISTER_AddMember(resetFallSuffix, CombatTracker::resetFallSuffix)
_CLASSREGISTER_AddMember(trackDamage, CombatTracker::trackDamage)
_END_REGISTER_CLASS()




// Register Class DamageSource
_BEGIN_REGISTER_CLASS(DamageSource)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddMember(canHarmInCreative, DamageSource::canHarmInCreative)
_CLASSREGISTER_AddStaticMember(causeArrowDamage, DamageSource::causeArrowDamage)
_CLASSREGISTER_AddStaticMember(causeFireballDamage, DamageSource::causeFireballDamage)
_CLASSREGISTER_AddStaticMember(causeIndirectMagicDamage, DamageSource::causeIndirectMagicDamage)
_CLASSREGISTER_AddStaticMember(causeMobDamage, DamageSource::causeMobDamage)
_CLASSREGISTER_AddStaticMember(causePlayerDamage, DamageSource::causePlayerDamage)
_CLASSREGISTER_AddStaticMember(causeThornsDamage, DamageSource::causeThornsDamage)
_CLASSREGISTER_AddStaticMember(causeThrownDamage, DamageSource::causeThrownDamage)
_CLASSREGISTER_AddMember(getDamageType, DamageSource::getDamageType)
_CLASSREGISTER_AddMember(getEntity, DamageSource::getEntity)
_CLASSREGISTER_AddMember(getHungerDamage, DamageSource::getHungerDamage)
_CLASSREGISTER_AddMember(getSourceOfDamage, DamageSource::getSourceOfDamage)
_CLASSREGISTER_AddStaticMember(initialize, DamageSource::initialize)
_CLASSREGISTER_AddMember(isDifficultyScaled, DamageSource::isDifficultyScaled)
_CLASSREGISTER_AddMember(isExplosion, DamageSource::isExplosion)
_CLASSREGISTER_AddMember(isFireDamage, DamageSource::isFireDamage)
_CLASSREGISTER_AddMember(isMagicDamage, DamageSource::isMagicDamage)
_CLASSREGISTER_AddMember(isProjectile, DamageSource::isProjectile)
_CLASSREGISTER_AddMember(isUnblockable, DamageSource::isUnblockable)
_CLASSREGISTER_AddMember(setDifficultyScaled, DamageSource::setDifficultyScaled)
_CLASSREGISTER_AddMember(setExplosion, DamageSource::setExplosion)
_CLASSREGISTER_AddStaticMember(setExplosionSource, DamageSource::setExplosionSource)
_CLASSREGISTER_AddMember(setMagicDamage, DamageSource::setMagicDamage)
_CLASSREGISTER_AddMember(setProjectile, DamageSource::setProjectile)
_CLASSREGISTER_AddStaticMember(unInitialize, DamageSource::unInitialize)
_END_REGISTER_CLASS()




// Register Class EntityDamageSource
_BEGIN_REGISTER_CLASS(EntityDamageSource)
_CLASSREGISTER_AddBaseClass(DamageSource)
_CLASSREGISTER_AddCtor(const String & , Entity * )
_CLASSREGISTER_AddMember(getEntity, EntityDamageSource::getEntity)
_CLASSREGISTER_AddMember(isDifficultyScaled, EntityDamageSource::isDifficultyScaled)
_END_REGISTER_CLASS()




// Register Class EntityDamageSourceIndirect
_BEGIN_REGISTER_CLASS(EntityDamageSourceIndirect)
_CLASSREGISTER_AddBaseClass(EntityDamageSource)
_CLASSREGISTER_AddCtor(const String & , Entity * , Entity * )
_CLASSREGISTER_AddMember(getEntity, EntityDamageSourceIndirect::getEntity)
_CLASSREGISTER_AddMember(getSourceOfDamage, EntityDamageSourceIndirect::getSourceOfDamage)
_END_REGISTER_CLASS()




// Register Class Enchantment
_BEGIN_REGISTER_CLASS(Enchantment)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddMember(calcModifierDamage, Enchantment::calcModifierDamage)
_CLASSREGISTER_AddMember(calcModifierLiving, Enchantment::calcModifierLiving)
_CLASSREGISTER_AddMember(canApply, Enchantment::canApply)
_CLASSREGISTER_AddMember(canApplyTogether, Enchantment::canApplyTogether)
_CLASSREGISTER_AddMember(canEnchantItem, Enchantment::canEnchantItem)
_CLASSREGISTER_AddMember(getMaxEnchantability, Enchantment::getMaxEnchantability)
_CLASSREGISTER_AddMember(getMaxLevel, Enchantment::getMaxLevel)
_CLASSREGISTER_AddMember(getMinEnchantability, Enchantment::getMinEnchantability)
_CLASSREGISTER_AddMember(getMinLevel, Enchantment::getMinLevel)
_CLASSREGISTER_AddMember(getName, Enchantment::getName)
_CLASSREGISTER_AddMember(getWeight, Enchantment::getWeight)
_CLASSREGISTER_AddStaticMember(initialize, Enchantment::initialize)
_CLASSREGISTER_AddMember(isClass, Enchantment::isClass)
_CLASSREGISTER_AddMember(setName, Enchantment::setName)
_CLASSREGISTER_AddStaticMember(unInitialize, Enchantment::unInitialize)
_END_REGISTER_CLASS()




// Register Class EnchantmentArrowDamage
_BEGIN_REGISTER_CLASS(EnchantmentArrowDamage)
_CLASSREGISTER_AddBaseClass(Enchantment)
_CLASSREGISTER_AddCtor(int , int )
_CLASSREGISTER_AddMember(getMaxEnchantability, EnchantmentArrowDamage::getMaxEnchantability)
_CLASSREGISTER_AddMember(getMaxLevel, EnchantmentArrowDamage::getMaxLevel)
_CLASSREGISTER_AddMember(getMinEnchantability, EnchantmentArrowDamage::getMinEnchantability)
_END_REGISTER_CLASS()




// Register Class EnchantmentArrowFire
_BEGIN_REGISTER_CLASS(EnchantmentArrowFire)
_CLASSREGISTER_AddBaseClass(Enchantment)
_CLASSREGISTER_AddCtor(int , int )
_CLASSREGISTER_AddMember(getMaxEnchantability, EnchantmentArrowFire::getMaxEnchantability)
_CLASSREGISTER_AddMember(getMaxLevel, EnchantmentArrowFire::getMaxLevel)
_CLASSREGISTER_AddMember(getMinEnchantability, EnchantmentArrowFire::getMinEnchantability)
_END_REGISTER_CLASS()




// Register Class EnchantmentArrowInfinite
_BEGIN_REGISTER_CLASS(EnchantmentArrowInfinite)
_CLASSREGISTER_AddBaseClass(Enchantment)
_CLASSREGISTER_AddCtor(int , int )
_CLASSREGISTER_AddMember(getMaxEnchantability, EnchantmentArrowInfinite::getMaxEnchantability)
_CLASSREGISTER_AddMember(getMaxLevel, EnchantmentArrowInfinite::getMaxLevel)
_CLASSREGISTER_AddMember(getMinEnchantability, EnchantmentArrowInfinite::getMinEnchantability)
_END_REGISTER_CLASS()




// Register Class EnchantmentArrowKnockback
_BEGIN_REGISTER_CLASS(EnchantmentArrowKnockback)
_CLASSREGISTER_AddBaseClass(Enchantment)
_CLASSREGISTER_AddCtor(int , int )
_CLASSREGISTER_AddMember(getMaxEnchantability, EnchantmentArrowKnockback::getMaxEnchantability)
_CLASSREGISTER_AddMember(getMaxLevel, EnchantmentArrowKnockback::getMaxLevel)
_CLASSREGISTER_AddMember(getMinEnchantability, EnchantmentArrowKnockback::getMinEnchantability)
_END_REGISTER_CLASS()




// Register Class EnchantmentDamage
_BEGIN_REGISTER_CLASS(EnchantmentDamage)
_CLASSREGISTER_AddBaseClass(Enchantment)
_CLASSREGISTER_AddCtor(int , int , int )
_CLASSREGISTER_AddMember(calcModifierLiving, EnchantmentDamage::calcModifierLiving)
_CLASSREGISTER_AddMember(canApply, EnchantmentDamage::canApply)
_CLASSREGISTER_AddMember(canApplyTogether, EnchantmentDamage::canApplyTogether)
_CLASSREGISTER_AddMember(getMaxEnchantability, EnchantmentDamage::getMaxEnchantability)
_CLASSREGISTER_AddMember(getMaxLevel, EnchantmentDamage::getMaxLevel)
_CLASSREGISTER_AddMember(getMinEnchantability, EnchantmentDamage::getMinEnchantability)
_CLASSREGISTER_AddMember(getName, EnchantmentDamage::getName)
_END_REGISTER_CLASS()




// Register Class EnchantmentDigging
_BEGIN_REGISTER_CLASS(EnchantmentDigging)
_CLASSREGISTER_AddBaseClass(Enchantment)
_CLASSREGISTER_AddCtor(int , int )
_CLASSREGISTER_AddMember(canApply, EnchantmentDigging::canApply)
_CLASSREGISTER_AddMember(getMaxEnchantability, EnchantmentDigging::getMaxEnchantability)
_CLASSREGISTER_AddMember(getMaxLevel, EnchantmentDigging::getMaxLevel)
_CLASSREGISTER_AddMember(getMinEnchantability, EnchantmentDigging::getMinEnchantability)
_END_REGISTER_CLASS()




// Register Class EnchantmentDurability
_BEGIN_REGISTER_CLASS(EnchantmentDurability)
_CLASSREGISTER_AddBaseClass(Enchantment)
_CLASSREGISTER_AddCtor(int , int )
_CLASSREGISTER_AddMember(canApply, EnchantmentDurability::canApply)
_CLASSREGISTER_AddMember(getMaxEnchantability, EnchantmentDurability::getMaxEnchantability)
_CLASSREGISTER_AddMember(getMaxLevel, EnchantmentDurability::getMaxLevel)
_CLASSREGISTER_AddMember(getMinEnchantability, EnchantmentDurability::getMinEnchantability)
_CLASSREGISTER_AddStaticMember(negateDamage, EnchantmentDurability::negateDamage)
_END_REGISTER_CLASS()




// Register Class EnchantmentFireAspect
_BEGIN_REGISTER_CLASS(EnchantmentFireAspect)
_CLASSREGISTER_AddBaseClass(Enchantment)
_CLASSREGISTER_AddCtor(int , int )
_CLASSREGISTER_AddMember(getMaxEnchantability, EnchantmentFireAspect::getMaxEnchantability)
_CLASSREGISTER_AddMember(getMaxLevel, EnchantmentFireAspect::getMaxLevel)
_CLASSREGISTER_AddMember(getMinEnchantability, EnchantmentFireAspect::getMinEnchantability)
_END_REGISTER_CLASS()




// Register Class EnchantmentHelper
_BEGIN_REGISTER_CLASS(EnchantmentHelper)
_CLASSREGISTER_AddStaticMember(addRandomEnchantment, EnchantmentHelper::addRandomEnchantment)
_CLASSREGISTER_AddStaticMember(applyEnchantmentModifier, EnchantmentHelper::applyEnchantmentModifier)
_CLASSREGISTER_AddStaticMember(buildEnchantmentList, EnchantmentHelper::buildEnchantmentList)
_CLASSREGISTER_AddStaticMember(calcItemStackEnchantability, EnchantmentHelper::calcItemStackEnchantability)
_CLASSREGISTER_AddStaticMember(getAquaAffinityModifier, EnchantmentHelper::getAquaAffinityModifier)
_CLASSREGISTER_AddStaticMember(getEfficiencyModifier, EnchantmentHelper::getEfficiencyModifier)
_CLASSREGISTER_AddStaticMember(getEnchantedItem, EnchantmentHelper::getEnchantedItem)
_CLASSREGISTER_AddStaticMember(getEnchantmentLevel, EnchantmentHelper::getEnchantmentLevel)
_CLASSREGISTER_AddStaticMember(getEnchantmentModifierDamage, EnchantmentHelper::getEnchantmentModifierDamage)
_CLASSREGISTER_AddStaticMember(getEnchantmentModifierLiving, EnchantmentHelper::getEnchantmentModifierLiving)
_CLASSREGISTER_AddStaticMember(getEnchantments, EnchantmentHelper::getEnchantments)
_CLASSREGISTER_AddStaticMember(getFireAspectModifier, EnchantmentHelper::getFireAspectModifier)
_CLASSREGISTER_AddStaticMember(getFortuneModifier, EnchantmentHelper::getFortuneModifier)
_CLASSREGISTER_AddStaticMember(getKnockbackModifier, EnchantmentHelper::getKnockbackModifier)
_CLASSREGISTER_AddStaticMember(getLootingModifier, EnchantmentHelper::getLootingModifier)
_CLASSREGISTER_AddStaticMember(getMaxEnchantmentLevel, EnchantmentHelper::getMaxEnchantmentLevel)
_CLASSREGISTER_AddStaticMember(getRespiration, EnchantmentHelper::getRespiration)
_CLASSREGISTER_AddStaticMember(getSilkTouchModifier, EnchantmentHelper::getSilkTouchModifier)
_CLASSREGISTER_AddStaticMember(getThornsModifier, EnchantmentHelper::getThornsModifier)
_CLASSREGISTER_AddStaticMember(initialize, EnchantmentHelper::initialize)
_CLASSREGISTER_AddStaticMember(mapEnchantmentData, EnchantmentHelper::mapEnchantmentData)
_CLASSREGISTER_AddStaticMember(setEnchantments, EnchantmentHelper::setEnchantments)
_CLASSREGISTER_AddStaticMember(unInitalize, EnchantmentHelper::unInitalize)
_END_REGISTER_CLASS()




// Register Class EnchantmentKnockback
_BEGIN_REGISTER_CLASS(EnchantmentKnockback)
_CLASSREGISTER_AddBaseClass(Enchantment)
_CLASSREGISTER_AddCtor(int , int )
_CLASSREGISTER_AddMember(getMaxEnchantability, EnchantmentKnockback::getMaxEnchantability)
_CLASSREGISTER_AddMember(getMaxLevel, EnchantmentKnockback::getMaxLevel)
_CLASSREGISTER_AddMember(getMinEnchantability, EnchantmentKnockback::getMinEnchantability)
_END_REGISTER_CLASS()




// Register Class EnchantmentLootBonus
_BEGIN_REGISTER_CLASS(EnchantmentLootBonus)
_CLASSREGISTER_AddBaseClass(Enchantment)
_CLASSREGISTER_AddCtor(int , int , ENCHANTMENT_TYPE )
_CLASSREGISTER_AddMember(canApplyTogether, EnchantmentLootBonus::canApplyTogether)
_CLASSREGISTER_AddMember(getMaxEnchantability, EnchantmentLootBonus::getMaxEnchantability)
_CLASSREGISTER_AddMember(getMaxLevel, EnchantmentLootBonus::getMaxLevel)
_CLASSREGISTER_AddMember(getMinEnchantability, EnchantmentLootBonus::getMinEnchantability)
_END_REGISTER_CLASS()




// Register Class EnchantmentModifierDamage
_BEGIN_REGISTER_CLASS(EnchantmentModifierDamage)
_CLASSREGISTER_AddBaseClass(IEnchantmentModifier)
_CLASSREGISTER_AddCtor()
_CLASSREGISTER_AddMember(calculateModifier, EnchantmentModifierDamage::calculateModifier)
_END_REGISTER_CLASS()




// Register Class EnchantmentModifierLiving
_BEGIN_REGISTER_CLASS(EnchantmentModifierLiving)
_CLASSREGISTER_AddBaseClass(IEnchantmentModifier)
_CLASSREGISTER_AddCtor()
_CLASSREGISTER_AddMember(calculateModifier, EnchantmentModifierLiving::calculateModifier)
_END_REGISTER_CLASS()




// Register Class EnchantmentOxygen
_BEGIN_REGISTER_CLASS(EnchantmentOxygen)
_CLASSREGISTER_AddBaseClass(Enchantment)
_CLASSREGISTER_AddCtor(int , int )
_CLASSREGISTER_AddMember(getMaxEnchantability, EnchantmentOxygen::getMaxEnchantability)
_CLASSREGISTER_AddMember(getMaxLevel, EnchantmentOxygen::getMaxLevel)
_CLASSREGISTER_AddMember(getMinEnchantability, EnchantmentOxygen::getMinEnchantability)
_END_REGISTER_CLASS()




// Register Class EnchantmentProtection
_BEGIN_REGISTER_CLASS(EnchantmentProtection)
_CLASSREGISTER_AddBaseClass(Enchantment)
_CLASSREGISTER_AddCtor(int , int , int )
_CLASSREGISTER_AddMember(calcModifierDamage, EnchantmentProtection::calcModifierDamage)
_CLASSREGISTER_AddMember(canApplyTogether, EnchantmentProtection::canApplyTogether)
_CLASSREGISTER_AddStaticMember(getBlastDamageReduction, EnchantmentProtection::getBlastDamageReduction)
_CLASSREGISTER_AddStaticMember(getFireTimeForEntity, EnchantmentProtection::getFireTimeForEntity)
_CLASSREGISTER_AddMember(getMaxEnchantability, EnchantmentProtection::getMaxEnchantability)
_CLASSREGISTER_AddMember(getMaxLevel, EnchantmentProtection::getMaxLevel)
_CLASSREGISTER_AddMember(getMinEnchantability, EnchantmentProtection::getMinEnchantability)
_CLASSREGISTER_AddMember(getName, EnchantmentProtection::getName)
_END_REGISTER_CLASS()




// Register Class EnchantmentThorns
_BEGIN_REGISTER_CLASS(EnchantmentThorns)
_CLASSREGISTER_AddBaseClass(Enchantment)
_CLASSREGISTER_AddCtor(int , int )
_CLASSREGISTER_AddMember(canApply, EnchantmentThorns::canApply)
_CLASSREGISTER_AddStaticMember(damageArmor, EnchantmentThorns::damageArmor)
_CLASSREGISTER_AddStaticMember(getDamage, EnchantmentThorns::getDamage)
_CLASSREGISTER_AddMember(getMaxEnchantability, EnchantmentThorns::getMaxEnchantability)
_CLASSREGISTER_AddMember(getMaxLevel, EnchantmentThorns::getMaxLevel)
_CLASSREGISTER_AddMember(getMinEnchantability, EnchantmentThorns::getMinEnchantability)
_CLASSREGISTER_AddStaticMember(shouldHit, EnchantmentThorns::shouldHit)
_END_REGISTER_CLASS()




// Register Class EnchantmentUntouching
_BEGIN_REGISTER_CLASS(EnchantmentUntouching)
_CLASSREGISTER_AddBaseClass(Enchantment)
_CLASSREGISTER_AddCtor(int , int )
_CLASSREGISTER_AddMember(canApply, EnchantmentUntouching::canApply)
_CLASSREGISTER_AddMember(canApplyTogether, EnchantmentUntouching::canApplyTogether)
_CLASSREGISTER_AddMember(getMaxEnchantability, EnchantmentUntouching::getMaxEnchantability)
_CLASSREGISTER_AddMember(getMaxLevel, EnchantmentUntouching::getMaxLevel)
_CLASSREGISTER_AddMember(getMinEnchantability, EnchantmentUntouching::getMinEnchantability)
_END_REGISTER_CLASS()




// Register Class EnchantmentWaterWorker
_BEGIN_REGISTER_CLASS(EnchantmentWaterWorker)
_CLASSREGISTER_AddBaseClass(Enchantment)
_CLASSREGISTER_AddCtor(int , int )
_CLASSREGISTER_AddMember(getMaxEnchantability, EnchantmentWaterWorker::getMaxEnchantability)
_CLASSREGISTER_AddMember(getMaxLevel, EnchantmentWaterWorker::getMaxLevel)
_CLASSREGISTER_AddMember(getMinEnchantability, EnchantmentWaterWorker::getMinEnchantability)
_END_REGISTER_CLASS()








// Register Class Entity
_BEGIN_REGISTER_CLASS(Entity)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddMember(addEntityID, Entity::addEntityID)
_CLASSREGISTER_AddMember(addNotRiddenEntityID, Entity::addNotRiddenEntityID)
_CLASSREGISTER_AddMember(addToPlayerScore, Entity::addToPlayerScore)
_CLASSREGISTER_AddMember(addVelocity, Entity::addVelocity)
_CLASSREGISTER_AddMember(applyEntityCollision, Entity::applyEntityCollision)
_CLASSREGISTER_AddMember(attackEntityFrom, Entity::attackEntityFrom)
_CLASSREGISTER_AddMember(canAttackWithItem, Entity::canAttackWithItem)
_CLASSREGISTER_AddMember(canBeCollidedWith, Entity::canBeCollidedWith)
_CLASSREGISTER_AddMember(canBePushed, Entity::canBePushed)
_CLASSREGISTER_AddMember(canRenderOnFire, Entity::canRenderOnFire)
_CLASSREGISTER_AddMember(copyDataFrom, Entity::copyDataFrom)
_CLASSREGISTER_AddMember(copyLocationAndAnglesFrom, Entity::copyLocationAndAnglesFrom)
_CLASSREGISTER_AddMember(doBlockCollisions, Entity::doBlockCollisions)
_CLASSREGISTER_AddMember(doesEntityNotTriggerPressurePlate, Entity::doesEntityNotTriggerPressurePlate)
_CLASSREGISTER_AddMember(dropItem, Entity::dropItem)
_CLASSREGISTER_AddMember(dropItemWithOffset, Entity::dropItemWithOffset)
_CLASSREGISTER_AddMember(entityDropItem, Entity::entityDropItem)
_CLASSREGISTER_AddMember(extinguish, Entity::extinguish)
_CLASSREGISTER_AddMember(func_110123_P, Entity::func_110123_P)
_CLASSREGISTER_AddMember(getAir, Entity::getAir)
_CLASSREGISTER_AddMember(getBottomPos, Entity::getBottomPos)
_CLASSREGISTER_AddMember(getBoundingBox, Entity::getBoundingBox)
_CLASSREGISTER_AddMember(getBrightness, Entity::getBrightness)
_CLASSREGISTER_AddMember(getBrightnessForRender, Entity::getBrightnessForRender)
_CLASSREGISTER_AddMember(getCentorPos, Entity::getCentorPos)
_CLASSREGISTER_AddMember(getCollisionBorderSize, Entity::getCollisionBorderSize)
_CLASSREGISTER_AddMember(getCollisionBox, Entity::getCollisionBox)
_CLASSREGISTER_AddMember(getDistance, Entity::getDistance)
_CLASSREGISTER_AddMember(getDistanceSq, Entity::getDistanceSq)
_CLASSREGISTER_AddMember(getDistanceSqToEntity, Entity::getDistanceSqToEntity)
_CLASSREGISTER_AddMember(getDistanceToEntity, Entity::getDistanceToEntity)
_CLASSREGISTER_AddMember(getEntityName, Entity::getEntityName)
_CLASSREGISTER_AddMember(getExplosionResistance, Entity::getExplosionResistance)
_CLASSREGISTER_AddMember(getEyeHeight, Entity::getEyeHeight)
_CLASSREGISTER_AddMember(getLastActiveItems, Entity::getLastActiveItems)
_CLASSREGISTER_AddMember(getLookVec, Entity::getLookVec)
_CLASSREGISTER_AddMember(getMaxFallHeight, Entity::getMaxFallHeight)
_CLASSREGISTER_AddMember(getMountedYOffset, Entity::getMountedYOffset)
_CLASSREGISTER_AddMember(getParts, Entity::getParts)
_CLASSREGISTER_AddMember(getPartsCount, Entity::getPartsCount)
_CLASSREGISTER_AddMember(getPortalCooldown, Entity::getPortalCooldown)
_CLASSREGISTER_AddMember(getRotationYawHead, Entity::getRotationYawHead)
_CLASSREGISTER_AddMember(getShadowSize, Entity::getShadowSize)
_CLASSREGISTER_AddStaticMember(getSuperClassID, Entity::getSuperClassID)
_CLASSREGISTER_AddMember(getTopperPos, Entity::getTopperPos)
_CLASSREGISTER_AddMember(getTranslatedEntityName, Entity::getTranslatedEntityName)
_CLASSREGISTER_AddMember(getUniqueID, Entity::getUniqueID)
_CLASSREGISTER_AddMember(getWidelyPos, Entity::getWidelyPos)
_CLASSREGISTER_AddMember(getYOffset, Entity::getYOffset)
_CLASSREGISTER_AddMember(handleLavaMovement, Entity::handleLavaMovement)
_CLASSREGISTER_AddMember(handleWaterMovement, Entity::handleWaterMovement)
_CLASSREGISTER_AddMember(hashCode, Entity::hashCode)
_CLASSREGISTER_AddMember(hitByEntity, Entity::hitByEntity)
_CLASSREGISTER_AddMember(isBurning, Entity::isBurning)
_CLASSREGISTER_AddMember(isEating, Entity::isEating)
_CLASSREGISTER_AddMember(isEntityAlive, Entity::isEntityAlive)
_CLASSREGISTER_AddMember(isEntityEqual, Entity::isEntityEqual)
_CLASSREGISTER_AddMember(isEntityInsideOpaqueBlock, Entity::isEntityInsideOpaqueBlock)
_CLASSREGISTER_AddMember(isEntityInvulnerable, Entity::isEntityInvulnerable)
_CLASSREGISTER_AddMember(isImmuneToFire, Entity::isImmuneToFire)
_CLASSREGISTER_AddMember_Override(isInRangeToRenderDist, Entity::isInRangeToRenderDist,bool ,float )
_CLASSREGISTER_AddMember_Override(isInRangeToRenderDist1, Entity::isInRangeToRenderDist,bool ,const Vector3 & )
_CLASSREGISTER_AddMember(isInRangeToRenderVec3D, Entity::isInRangeToRenderVec3D)
_CLASSREGISTER_AddMember(isInWater, Entity::isInWater)
_CLASSREGISTER_AddMember(isInsideOfMaterial, Entity::isInsideOfMaterial)
_CLASSREGISTER_AddMember(isInvisible, Entity::isInvisible)
_CLASSREGISTER_AddMember(isInvisibleToPlayer, Entity::isInvisibleToPlayer)
_CLASSREGISTER_AddMember(isOffsetPositionInLiquid, Entity::isOffsetPositionInLiquid)
_CLASSREGISTER_AddMember(isPushedByWater, Entity::isPushedByWater)
_CLASSREGISTER_AddMember(isRiding, Entity::isRiding)
_CLASSREGISTER_AddMember(isSneaking, Entity::isSneaking)
_CLASSREGISTER_AddMember(isSprinting, Entity::isSprinting)
_CLASSREGISTER_AddMember(isWet, Entity::isWet)
_CLASSREGISTER_AddMember(mountEntity, Entity::mountEntity)
_CLASSREGISTER_AddMember(moveEntity, Entity::moveEntity)
_CLASSREGISTER_AddMember(moveFlying, Entity::moveFlying)
_CLASSREGISTER_AddStaticMember(multiplyBy32AndRound, Entity::multiplyBy32AndRound)
_CLASSREGISTER_AddMember(onCollideWithPlayer, Entity::onCollideWithPlayer)
_CLASSREGISTER_AddMember(onEntityUpdate, Entity::onEntityUpdate)
_CLASSREGISTER_AddMember(onKillEntity, Entity::onKillEntity)
_CLASSREGISTER_AddMember(onStruckByLightning, Entity::onStruckByLightning)
_CLASSREGISTER_AddMember(onUpdate, Entity::onUpdate)
_CLASSREGISTER_AddMember(performHurtAnimation, Entity::performHurtAnimation)
_CLASSREGISTER_AddMember(playSoundByType, Entity::playSoundByType)
_CLASSREGISTER_AddMember(processInitialInteract, Entity::processInitialInteract)
_CLASSREGISTER_AddMember(pushOutOfBlocks, Entity::pushOutOfBlocks)
_CLASSREGISTER_AddMember(readFromNBT, Entity::readFromNBT)
_CLASSREGISTER_AddMember(setAir, Entity::setAir)
_CLASSREGISTER_AddMember(setAngles, Entity::setAngles)
_CLASSREGISTER_AddMember(setCurrentItemOrArmor, Entity::setCurrentItemOrArmor)
_CLASSREGISTER_AddMember(setDead, Entity::setDead)
_CLASSREGISTER_AddMember(setEating, Entity::setEating)
_CLASSREGISTER_AddMember(setFire, Entity::setFire)
_CLASSREGISTER_AddMember(setInPortal, Entity::setInPortal)
_CLASSREGISTER_AddMember(setInWeb, Entity::setInWeb)
_CLASSREGISTER_AddMember(setInvisible, Entity::setInvisible)
_CLASSREGISTER_AddMember(setLocationAndAngles, Entity::setLocationAndAngles)
_CLASSREGISTER_AddMember(setPosition, Entity::setPosition)
_CLASSREGISTER_AddMember(setPositionAndRotation, Entity::setPositionAndRotation)
_CLASSREGISTER_AddMember(setPositionAndRotation2, Entity::setPositionAndRotation2)
_CLASSREGISTER_AddMember(setRotationYawHead, Entity::setRotationYawHead)
_CLASSREGISTER_AddMember(setSneaking, Entity::setSneaking)
_CLASSREGISTER_AddMember(setSprinting, Entity::setSprinting)
_CLASSREGISTER_AddMember(setVelocity, Entity::setVelocity)
_CLASSREGISTER_AddMember(setWorld, Entity::setWorld)
_CLASSREGISTER_AddMember(toString, Entity::toString)
_CLASSREGISTER_AddMember(updateRidden, Entity::updateRidden)
_CLASSREGISTER_AddMember(updateRiderPosition, Entity::updateRiderPosition)
_CLASSREGISTER_AddMember(verifyExplosion, Entity::verifyExplosion)
_CLASSREGISTER_AddMember(writeToNBT, Entity::writeToNBT)
_END_REGISTER_CLASS()




// Register Class EntityArrow
_BEGIN_REGISTER_CLASS(EntityArrow)
_CLASSREGISTER_AddBaseClass(Entity )
_CLASSREGISTER_AddBaseClass(IProjectile)
_CLASSREGISTER_AddCtor(World * )
_CLASSREGISTER_AddCtor(World * , const Vector3 & )
_CLASSREGISTER_AddCtor(World * , EntityLivingBase * , EntityLivingBase * , float , float )
_CLASSREGISTER_AddCtor(World * , EntityLivingBase * , float )
_CLASSREGISTER_AddCtor(World * , EntityLivingBase * , const Vector3 & , float , float , float )
_CLASSREGISTER_AddMember(canAttackWithItem, EntityArrow::canAttackWithItem)
_CLASSREGISTER_AddMember(getDamage, EntityArrow::getDamage)
_CLASSREGISTER_AddMember(getIsCritical, EntityArrow::getIsCritical)
_CLASSREGISTER_AddMember(getShadowSize, EntityArrow::getShadowSize)
_CLASSREGISTER_AddMember(onCollideWithPlayer, EntityArrow::onCollideWithPlayer)
_CLASSREGISTER_AddMember(onUpdate, EntityArrow::onUpdate)
_CLASSREGISTER_AddMember(readEntityFromNBT, EntityArrow::readEntityFromNBT)
_CLASSREGISTER_AddMember(setDamage, EntityArrow::setDamage)
_CLASSREGISTER_AddMember(setIsCritical, EntityArrow::setIsCritical)
_CLASSREGISTER_AddMember(setKnockbackStrength, EntityArrow::setKnockbackStrength)
_CLASSREGISTER_AddMember(setPositionAndRotation2, EntityArrow::setPositionAndRotation2)
_CLASSREGISTER_AddMember(setThrowableHeading, EntityArrow::setThrowableHeading)
_CLASSREGISTER_AddMember(setVelocity, EntityArrow::setVelocity)
_CLASSREGISTER_AddMember(writeEntityToNBT, EntityArrow::writeEntityToNBT)
_END_REGISTER_CLASS()



/*
// Register Class EntityCreature
_BEGIN_REGISTER_CLASS(EntityCreature)
_CLASSREGISTER_AddBaseClass(EntityLiving)
_CLASSREGISTER_AddCtor(World * )
_CLASSREGISTER_AddMember(detachHome, EntityCreature::detachHome)
_CLASSREGISTER_AddMember(getBlockPathWeight, EntityCreature::getBlockPathWeight)
_CLASSREGISTER_AddMember(getCanSpawnHere, EntityCreature::getCanSpawnHere)
_CLASSREGISTER_AddMember(getEntityToAttack, EntityCreature::getEntityToAttack)
_CLASSREGISTER_AddMember(getHomePosition, EntityCreature::getHomePosition)
_CLASSREGISTER_AddMember(getMaximumHomeDistance, EntityCreature::getMaximumHomeDistance)
_CLASSREGISTER_AddMember(hasHome, EntityCreature::hasHome)
_CLASSREGISTER_AddMember(hasPath, EntityCreature::hasPath)
_CLASSREGISTER_AddMember(isWithinHomeDistanceCurrentPosition, EntityCreature::isWithinHomeDistanceCurrentPosition)
_CLASSREGISTER_AddMember(isWithinHomeDistanceFromPosition, EntityCreature::isWithinHomeDistanceFromPosition)
_CLASSREGISTER_AddMember(setHomePosAndDistance, EntityCreature::setHomePosAndDistance)
_CLASSREGISTER_AddMember(setPathToEntity, EntityCreature::setPathToEntity)
_CLASSREGISTER_AddMember(setTarget, EntityCreature::setTarget)
_END_REGISTER_CLASS()
*/




// Register Class EntityItem
_BEGIN_REGISTER_CLASS(EntityItem)
_CLASSREGISTER_AddBaseClass(Entity)
_CLASSREGISTER_AddCtor(World * , const Vector3 & )
_CLASSREGISTER_AddCtor(World * , const Vector3 & , ItemStackPtr )
_CLASSREGISTER_AddCtor(World * )
_CLASSREGISTER_AddMember(attackEntityFrom, EntityItem::attackEntityFrom)
_CLASSREGISTER_AddMember(canAttackWithItem, EntityItem::canAttackWithItem)
_CLASSREGISTER_AddMember(combineItems, EntityItem::combineItems)
_CLASSREGISTER_AddMember(getEntityItem, EntityItem::getEntityItem)
_CLASSREGISTER_AddMember(getEntityName, EntityItem::getEntityName)
_CLASSREGISTER_AddMember(handleWaterMovement, EntityItem::handleWaterMovement)
_CLASSREGISTER_AddMember(onCollideWithPlayer, EntityItem::onCollideWithPlayer)
_CLASSREGISTER_AddMember(onUpdate, EntityItem::onUpdate)
_CLASSREGISTER_AddMember(readEntityFromNBT, EntityItem::readEntityFromNBT)
_CLASSREGISTER_AddMember(setAgeToCreativeDespawnTime, EntityItem::setAgeToCreativeDespawnTime)
_CLASSREGISTER_AddMember(setEntityItemStack, EntityItem::setEntityItemStack)
_CLASSREGISTER_AddMember(writeEntityToNBT, EntityItem::writeEntityToNBT)
_END_REGISTER_CLASS()




// Register Class EntityBodyHelper
/*
_BEGIN_REGISTER_CLASS(EntityBodyHelper)
_CLASSREGISTER_AddCtor(EntityLivingBase & )
_CLASSREGISTER_AddMember(updateRenderAngles, EntityBodyHelper::updateRenderAngles)
_END_REGISTER_CLASS()
*/




// Register Class EntityJumpHelper
_BEGIN_REGISTER_CLASS(EntityJumpHelper)
_CLASSREGISTER_AddCtor(EntityLiving & )
_CLASSREGISTER_AddMember(doJump, EntityJumpHelper::doJump)
_CLASSREGISTER_AddMember(setJumping, EntityJumpHelper::setJumping)
_END_REGISTER_CLASS()




// Register Class EntityLiving
_BEGIN_REGISTER_CLASS(EntityLiving)
_CLASSREGISTER_AddBaseClass(EntityLivingBase)
_CLASSREGISTER_AddCtor(World * )
_CLASSREGISTER_AddMember(canBeLeashedTo, EntityLiving::canBeLeashedTo)
_CLASSREGISTER_AddMember(canBeSteered, EntityLiving::canBeSteered)
_CLASSREGISTER_AddMember(canPickUpLoot, EntityLiving::canPickUpLoot)
_CLASSREGISTER_AddMember(clearLeashed, EntityLiving::clearLeashed)
_CLASSREGISTER_AddMember(eatGrassBonus, EntityLiving::eatGrassBonus)
_CLASSREGISTER_AddMember(enablePersistence, EntityLiving::enablePersistence)
_CLASSREGISTER_AddMember(faceEntity, EntityLiving::faceEntity)
_CLASSREGISTER_AddMember(getAlwaysRenderNameTagForRender, EntityLiving::getAlwaysRenderNameTagForRender)
_CLASSREGISTER_AddMember(getArmorFromSlot, EntityLiving::getArmorFromSlot)
_CLASSREGISTER_AddStaticMember(getArmorItemForSlot, EntityLiving::getArmorItemForSlot)
_CLASSREGISTER_AddStaticMember(getArmorPosition, EntityLiving::getArmorPosition)
_CLASSREGISTER_AddMember(getAttackTarget, EntityLiving::getAttackTarget)
_CLASSREGISTER_AddMember(getCanSpawnHere, EntityLiving::getCanSpawnHere)
_CLASSREGISTER_AddMember(getCurrentItemOrArmor, EntityLiving::getCurrentItemOrArmor)
_CLASSREGISTER_AddMember(getCustomNameTag, EntityLiving::getCustomNameTag)
_CLASSREGISTER_AddMember(getEntityName, EntityLiving::getEntityName)
_CLASSREGISTER_AddMember(getEntitySenses, EntityLiving::getEntitySenses)
_CLASSREGISTER_AddMember(getHeldItem, EntityLiving::getHeldItem)
_CLASSREGISTER_AddMember(getJumpHelper, EntityLiving::getJumpHelper)
_CLASSREGISTER_AddMember(getLastActiveItems, EntityLiving::getLastActiveItems)
_CLASSREGISTER_AddMember(getLeashed, EntityLiving::getLeashed)
_CLASSREGISTER_AddMember(getLeashedToEntity, EntityLiving::getLeashedToEntity)
_CLASSREGISTER_AddMember(getLookHelper, EntityLiving::getLookHelper)
_CLASSREGISTER_AddMember(getMaxFallHeight, EntityLiving::getMaxFallHeight)
_CLASSREGISTER_AddMember(getMaxSpawnedInChunk, EntityLiving::getMaxSpawnedInChunk)
_CLASSREGISTER_AddMember(getMoveHelper, EntityLiving::getMoveHelper)
_CLASSREGISTER_AddMember(getNavigator, EntityLiving::getNavigator)
_CLASSREGISTER_AddMember(getRenderSizeModifier, EntityLiving::getRenderSizeModifier)
_CLASSREGISTER_AddMember(getTalkInterval, EntityLiving::getTalkInterval)
_CLASSREGISTER_AddMember(getVerticalFaceSpeed, EntityLiving::getVerticalFaceSpeed)
_CLASSREGISTER_AddMember(hasCustomNameTag, EntityLiving::hasCustomNameTag)
_CLASSREGISTER_AddMember(isAIDisable, EntityLiving::isAIDisable)
_CLASSREGISTER_AddMember(isNoDespawnRequired, EntityLiving::isNoDespawnRequired)
_CLASSREGISTER_AddMember(onEntityUpdate, EntityLiving::onEntityUpdate)
_CLASSREGISTER_AddMember(onInitialSpawn, EntityLiving::onInitialSpawn)
_CLASSREGISTER_AddMember(onLivingUpdate, EntityLiving::onLivingUpdate)
_CLASSREGISTER_AddMember(onUpdate, EntityLiving::onUpdate)
_CLASSREGISTER_AddMember(playLivingSound, EntityLiving::playLivingSound)
_CLASSREGISTER_AddMember(processInitialInteract, EntityLiving::processInitialInteract)
_CLASSREGISTER_AddMember(readEntityFromNBT, EntityLiving::readEntityFromNBT)
_CLASSREGISTER_AddMember(setAIMoveSpeed, EntityLiving::setAIMoveSpeed)
_CLASSREGISTER_AddMember(setAttackTarget, EntityLiving::setAttackTarget)
_CLASSREGISTER_AddMember(setCanPickUpLoot, EntityLiving::setCanPickUpLoot)
_CLASSREGISTER_AddMember(setCurrentItemOrArmor, EntityLiving::setCurrentItemOrArmor)
_CLASSREGISTER_AddMember(setCustomNameTag, EntityLiving::setCustomNameTag)
_CLASSREGISTER_AddMember(setEquipmentDropChance, EntityLiving::setEquipmentDropChance)
_CLASSREGISTER_AddMember(setLeashedToEntity, EntityLiving::setLeashedToEntity)
_CLASSREGISTER_AddMember(setNoAI, EntityLiving::setNoAI)
_CLASSREGISTER_AddMember(spawnExplosionParticle, EntityLiving::spawnExplosionParticle)
_CLASSREGISTER_AddMember(writeEntityToNBT, EntityLiving::writeEntityToNBT)
_END_REGISTER_CLASS()






// Register Class EntityLookHelper
_BEGIN_REGISTER_CLASS(EntityLookHelper)
_CLASSREGISTER_AddCtor(EntityLiving & )
_CLASSREGISTER_AddMember(getIsLooking, EntityLookHelper::getIsLooking)
_CLASSREGISTER_AddMember(getLookPos, EntityLookHelper::getLookPos)
_CLASSREGISTER_AddMember(onUpdateLook, EntityLookHelper::onUpdateLook)
_CLASSREGISTER_AddMember(setLookPosition, EntityLookHelper::setLookPosition)
_CLASSREGISTER_AddMember(setLookPositionWithEntity, EntityLookHelper::setLookPositionWithEntity)
_END_REGISTER_CLASS()




// Register Class EntityMoveHelper
_BEGIN_REGISTER_CLASS(EntityMoveHelper)
_CLASSREGISTER_AddCtor(EntityLiving & )
_CLASSREGISTER_AddMember(getSpeed, EntityMoveHelper::getSpeed)
_CLASSREGISTER_AddMember(isUpdating, EntityMoveHelper::isUpdating)
_CLASSREGISTER_AddMember(onUpdateMoveHelper, EntityMoveHelper::onUpdateMoveHelper)
_CLASSREGISTER_AddMember(setMoveTo, EntityMoveHelper::setMoveTo)
_END_REGISTER_CLASS()




// Register Class EntitySenses
_BEGIN_REGISTER_CLASS(EntitySenses)
_CLASSREGISTER_AddCtor(EntityLiving & )
_CLASSREGISTER_AddMember(canSee, EntitySenses::canSee)
_CLASSREGISTER_AddMember(clearSensingCache, EntitySenses::clearSensingCache)
_END_REGISTER_CLASS()




// Register Class EntityLivingBase
_BEGIN_REGISTER_CLASS(EntityLivingBase)
_CLASSREGISTER_AddBaseClass(Entity)
_CLASSREGISTER_AddMember(addPotionEffect, EntityLivingBase::addPotionEffect)
_CLASSREGISTER_AddMember(attackEntityAsMob, EntityLivingBase::attackEntityAsMob)
_CLASSREGISTER_AddMember(attackEntityFrom, EntityLivingBase::attackEntityFrom)
_CLASSREGISTER_AddMember(canBeCollidedWith, EntityLivingBase::canBeCollidedWith)
_CLASSREGISTER_AddMember(canBePushed, EntityLivingBase::canBePushed)
_CLASSREGISTER_AddMember(canBreatheUnderwater, EntityLivingBase::canBreatheUnderwater)
_CLASSREGISTER_AddMember(canEntityBeSeen, EntityLivingBase::canEntityBeSeen)
_CLASSREGISTER_AddMember(clearActivePotions, EntityLivingBase::clearActivePotions)
_CLASSREGISTER_AddMember(dismountEntity, EntityLivingBase::dismountEntity)
_CLASSREGISTER_AddMember(getAIMoveSpeed, EntityLivingBase::getAIMoveSpeed)
_CLASSREGISTER_AddMember(getAbsorptionAmount, EntityLivingBase::getAbsorptionAmount)
_CLASSREGISTER_AddMember(getActivePotionEffect, EntityLivingBase::getActivePotionEffect)
_CLASSREGISTER_AddMember(getActivePotionEffects, EntityLivingBase::getActivePotionEffects)
_CLASSREGISTER_AddMember(getAge, EntityLivingBase::getAge)
_CLASSREGISTER_AddMember(getAlwaysRenderNameTagForRender, EntityLivingBase::getAlwaysRenderNameTagForRender)
_CLASSREGISTER_AddMember(getArrowCountInEntity, EntityLivingBase::getArrowCountInEntity)
_CLASSREGISTER_AddMember(getAttackingEntity, EntityLivingBase::getAttackingEntity)
_CLASSREGISTER_AddMember(getAttributeMap, EntityLivingBase::getAttributeMap)
_CLASSREGISTER_AddMember(getCombatTracker, EntityLivingBase::getCombatTracker)
_CLASSREGISTER_AddMember(getCreatureAttribute, EntityLivingBase::getCreatureAttribute)
_CLASSREGISTER_AddMember(getEntityAttribute, EntityLivingBase::getEntityAttribute)
_CLASSREGISTER_AddMember(getEyeHeight, EntityLivingBase::getEyeHeight)
_CLASSREGISTER_AddMember(getHealth, EntityLivingBase::getHealth)
_CLASSREGISTER_AddMember(getLastActiveItems, EntityLivingBase::getLastActiveItems)
_CLASSREGISTER_AddMember(getLastActiveItemsCount, EntityLivingBase::getLastActiveItemsCount)
_CLASSREGISTER_AddMember(getLastAttacker, EntityLivingBase::getLastAttacker)
_CLASSREGISTER_AddMember(getLastAttackerTime, EntityLivingBase::getLastAttackerTime)
_CLASSREGISTER_AddMember(getLook, EntityLivingBase::getLook)
_CLASSREGISTER_AddMember(getLookVec, EntityLivingBase::getLookVec)
_CLASSREGISTER_AddMember(getMaxHealth, EntityLivingBase::getMaxHealth)
_CLASSREGISTER_AddMember(getPosition, EntityLivingBase::getPosition)
_CLASSREGISTER_AddMember(getRNG, EntityLivingBase::getRNG)
_CLASSREGISTER_AddMember(getRotationYawHead, EntityLivingBase::getRotationYawHead)
_CLASSREGISTER_AddMember(getSwingProgress, EntityLivingBase::getSwingProgress)
_CLASSREGISTER_AddMember(getTotalArmorValue, EntityLivingBase::getTotalArmorValue)
_CLASSREGISTER_AddMember(heal, EntityLivingBase::heal)
_CLASSREGISTER_AddStaticMember(initialize, EntityLivingBase::initialize)
_CLASSREGISTER_AddMember(isChild, EntityLivingBase::isChild)
_CLASSREGISTER_AddMember(isClientWorld, EntityLivingBase::isClientWorld)
_CLASSREGISTER_AddMember(isEntityAlive, EntityLivingBase::isEntityAlive)
_CLASSREGISTER_AddMember(isEntityUndead, EntityLivingBase::isEntityUndead)
_CLASSREGISTER_AddMember(isOnLadder, EntityLivingBase::isOnLadder)
_CLASSREGISTER_AddMember(isPlayerSleeping, EntityLivingBase::isPlayerSleeping)
_CLASSREGISTER_AddMember_Override(isPotionActive, EntityLivingBase::isPotionActive,bool ,int )
_CLASSREGISTER_AddMember_Override(isPotionActive1, EntityLivingBase::isPotionActive,bool ,Potion * )
_CLASSREGISTER_AddMember(isPotionApplicable, EntityLivingBase::isPotionApplicable)
_CLASSREGISTER_AddMember(knockBack, EntityLivingBase::knockBack)
_CLASSREGISTER_AddMember(moveEntityWithHeading, EntityLivingBase::moveEntityWithHeading)
_CLASSREGISTER_AddMember(onDeath, EntityLivingBase::onDeath)
_CLASSREGISTER_AddMember(onEntityUpdate, EntityLivingBase::onEntityUpdate)
_CLASSREGISTER_AddMember(onItemPickup, EntityLivingBase::onItemPickup)
_CLASSREGISTER_AddMember(onLivingUpdate, EntityLivingBase::onLivingUpdate)
_CLASSREGISTER_AddMember(onUpdate, EntityLivingBase::onUpdate)
_CLASSREGISTER_AddMember(performHurtAnimation, EntityLivingBase::performHurtAnimation)
_CLASSREGISTER_AddMember(rayTrace, EntityLivingBase::rayTrace)
_CLASSREGISTER_AddMember(readEntityFromNBT, EntityLivingBase::readEntityFromNBT)
_CLASSREGISTER_AddMember(removePotionEffect, EntityLivingBase::removePotionEffect)
_CLASSREGISTER_AddMember(removePotionEffectClient, EntityLivingBase::removePotionEffectClient)
_CLASSREGISTER_AddMember(renderBrokenItemStack, EntityLivingBase::renderBrokenItemStack)
_CLASSREGISTER_AddMember(setAIMoveSpeed, EntityLivingBase::setAIMoveSpeed)
_CLASSREGISTER_AddMember(setAbsorptionAmount, EntityLivingBase::setAbsorptionAmount)
_CLASSREGISTER_AddMember(setArrowCountInEntity, EntityLivingBase::setArrowCountInEntity)
_CLASSREGISTER_AddMember(setEntityHealth, EntityLivingBase::setEntityHealth)
_CLASSREGISTER_AddMember(setJumping, EntityLivingBase::setJumping)
_CLASSREGISTER_AddMember(setLastAttacker, EntityLivingBase::setLastAttacker)
_CLASSREGISTER_AddMember(setMoveForward, EntityLivingBase::setMoveForward)
_CLASSREGISTER_AddMember(setMoveStrafe, EntityLivingBase::setMoveStrafe)
_CLASSREGISTER_AddMember(setPositionAndRotation2, EntityLivingBase::setPositionAndRotation2)
_CLASSREGISTER_AddMember(setPositionAndUpdate, EntityLivingBase::setPositionAndUpdate)
_CLASSREGISTER_AddMember(setRotationYawHead, EntityLivingBase::setRotationYawHead)
_CLASSREGISTER_AddMember(setSprinting, EntityLivingBase::setSprinting)
_CLASSREGISTER_AddMember(swingItem, EntityLivingBase::swingItem)
_CLASSREGISTER_AddStaticMember(unInitialize, EntityLivingBase::unInitialize)
_CLASSREGISTER_AddMember(updateRidden, EntityLivingBase::updateRidden)
_CLASSREGISTER_AddMember(writeEntityToNBT, EntityLivingBase::writeEntityToNBT)
_END_REGISTER_CLASS()



/*
// Register Class EntityMob
_BEGIN_REGISTER_CLASS(EntityMob)
_CLASSREGISTER_AddBaseClass(EntityCreature /* implements IMob*///)
/*
_CLASSREGISTER_AddCtor(World * )
_CLASSREGISTER_AddMember(attackEntityAsMob, EntityMob::attackEntityAsMob)
_CLASSREGISTER_AddMember(attackEntityFrom, EntityMob::attackEntityFrom)
_CLASSREGISTER_AddMember(getBlockPathWeight, EntityMob::getBlockPathWeight)
_CLASSREGISTER_AddMember(getCanSpawnHere, EntityMob::getCanSpawnHere)
_CLASSREGISTER_AddMember(onLivingUpdate, EntityMob::onLivingUpdate)
_CLASSREGISTER_AddMember(onUpdate, EntityMob::onUpdate)
_END_REGISTER_CLASS()
*/




// Register Class EntityPlayer
_BEGIN_REGISTER_CLASS(EntityPlayer)
_CLASSREGISTER_AddBaseClass(EntityLivingBase)
_CLASSREGISTER_AddCtor(World * , const String & )
_CLASSREGISTER_AddMember(addChatMessage, EntityPlayer::addChatMessage)
_CLASSREGISTER_AddMember(addExhaustion, EntityPlayer::addExhaustion)
_CLASSREGISTER_AddMember(addExperience, EntityPlayer::addExperience)
_CLASSREGISTER_AddMember(addExperienceLevel, EntityPlayer::addExperienceLevel)
_CLASSREGISTER_AddMember(addMovementStat, EntityPlayer::addMovementStat)
_CLASSREGISTER_AddMember(addScore, EntityPlayer::addScore)
_CLASSREGISTER_AddMember(addStat, EntityPlayer::addStat)
_CLASSREGISTER_AddMember(addToPlayerScore, EntityPlayer::addToPlayerScore)
_CLASSREGISTER_AddMember(attackEntityFrom, EntityPlayer::attackEntityFrom)
_CLASSREGISTER_AddMember(attackTargetEntityWithCurrentItem, EntityPlayer::attackTargetEntityWithCurrentItem)
_CLASSREGISTER_AddMember(attckedByPlayer, EntityPlayer::attckedByPlayer)
_CLASSREGISTER_AddMember(canAttackPlayer, EntityPlayer::canAttackPlayer)
_CLASSREGISTER_AddMember(canEat, EntityPlayer::canEat)
_CLASSREGISTER_AddMember(canHarvestBlock, EntityPlayer::canHarvestBlock)
_CLASSREGISTER_AddMember(canPlayerEdit, EntityPlayer::canPlayerEdit)
_CLASSREGISTER_AddMember(clearItemInUse, EntityPlayer::clearItemInUse)
_CLASSREGISTER_AddMember(clonePlayer, EntityPlayer::clonePlayer)
_CLASSREGISTER_AddMember(closeScreen, EntityPlayer::closeScreen)
_CLASSREGISTER_AddMember(destroyCurrentEquippedItem, EntityPlayer::destroyCurrentEquippedItem)
_CLASSREGISTER_AddMember(dropOneItem, EntityPlayer::dropOneItem)
_CLASSREGISTER_AddMember(dropPlayerItem, EntityPlayer::dropPlayerItem)
_CLASSREGISTER_AddMember(dropPlayerItemWithRandomChoice, EntityPlayer::dropPlayerItemWithRandomChoice)
_CLASSREGISTER_AddMember(getAIMoveSpeed, EntityPlayer::getAIMoveSpeed)
_CLASSREGISTER_AddMember(getAbsorptionAmount, EntityPlayer::getAbsorptionAmount)
_CLASSREGISTER_AddMember(getActivatedBlock, EntityPlayer::getActivatedBlock)
_CLASSREGISTER_AddMember(getAlwaysRenderNameTagForRender, EntityPlayer::getAlwaysRenderNameTagForRender)
_CLASSREGISTER_AddMember(getArmorVisibility, EntityPlayer::getArmorVisibility)
_CLASSREGISTER_AddMember(getBedLocation, EntityPlayer::getBedLocation)
_CLASSREGISTER_AddMember(getBedOrientationInDegrees, EntityPlayer::getBedOrientationInDegrees)
_CLASSREGISTER_AddMember(getCommandSenderName, EntityPlayer::getCommandSenderName)
_CLASSREGISTER_AddMember(getCurrentArmor, EntityPlayer::getCurrentArmor)
_CLASSREGISTER_AddMember(getCurrentEquippedItem, EntityPlayer::getCurrentEquippedItem)
_CLASSREGISTER_AddMember(getCurrentItemOrArmor, EntityPlayer::getCurrentItemOrArmor)
_CLASSREGISTER_AddMember(getCurrentPlayerStrVsBlock, EntityPlayer::getCurrentPlayerStrVsBlock)
_CLASSREGISTER_AddMember(getEntityName, EntityPlayer::getEntityName)
_CLASSREGISTER_AddMember(getEyeHeight, EntityPlayer::getEyeHeight)
_CLASSREGISTER_AddMember(getFoodStats, EntityPlayer::getFoodStats)
_CLASSREGISTER_AddMember(getHeldItem, EntityPlayer::getHeldItem)
_CLASSREGISTER_AddMember(getHeldItemId, EntityPlayer::getHeldItemId)
_CLASSREGISTER_AddMember_Override(getHideCape1, EntityPlayer::getHideCape,bool )
_CLASSREGISTER_AddMember(getInventoryEnderChest, EntityPlayer::getInventoryEnderChest)
_CLASSREGISTER_AddMember(getItemInUse, EntityPlayer::getItemInUse)
_CLASSREGISTER_AddMember(getItemInUseCount, EntityPlayer::getItemInUseCount)
_CLASSREGISTER_AddMember(getItemInUseDuration, EntityPlayer::getItemInUseDuration)
_CLASSREGISTER_AddMember(getLastActiveItems, EntityPlayer::getLastActiveItems)
_CLASSREGISTER_AddMember(getScore, EntityPlayer::getScore)
_CLASSREGISTER_AddMember(getSleepTimer, EntityPlayer::getSleepTimer)
_CLASSREGISTER_AddMember(getTotalArmorValue, EntityPlayer::getTotalArmorValue)
_CLASSREGISTER_AddMember(getWorldPtr, EntityPlayer::getWorldPtr)
_CLASSREGISTER_AddMember(getYOffset, EntityPlayer::getYOffset)
_CLASSREGISTER_AddMember(interactWith, EntityPlayer::interactWith)
_CLASSREGISTER_AddMember(isBlocking, EntityPlayer::isBlocking)
_CLASSREGISTER_AddMember(isCurrentToolAdventureModeExempt, EntityPlayer::isCurrentToolAdventureModeExempt)
_CLASSREGISTER_AddMember(isEntityInsideOpaqueBlock, EntityPlayer::isEntityInsideOpaqueBlock)
_CLASSREGISTER_AddMember(isInvisibleToPlayer, EntityPlayer::isInvisibleToPlayer)
_CLASSREGISTER_AddMember(isPlayerFullyAsleep, EntityPlayer::isPlayerFullyAsleep)
_CLASSREGISTER_AddMember(isPlayerSleeping, EntityPlayer::isPlayerSleeping)
_CLASSREGISTER_AddMember(isPushedByWater, EntityPlayer::isPushedByWater)
_CLASSREGISTER_AddMember(isSpawnForced, EntityPlayer::isSpawnForced)
_CLASSREGISTER_AddMember(isUsingItem, EntityPlayer::isUsingItem)
_CLASSREGISTER_AddMember(mountEntity, EntityPlayer::mountEntity)
_CLASSREGISTER_AddMember(moveEntityWithHeading, EntityPlayer::moveEntityWithHeading)
_CLASSREGISTER_AddMember(onCriticalHit, EntityPlayer::onCriticalHit)
_CLASSREGISTER_AddMember(onDeath, EntityPlayer::onDeath)
_CLASSREGISTER_AddMember(onEnchantmentCritical, EntityPlayer::onEnchantmentCritical)
_CLASSREGISTER_AddMember(onKillEntity, EntityPlayer::onKillEntity)
_CLASSREGISTER_AddMember(onLivingUpdate, EntityPlayer::onLivingUpdate)
_CLASSREGISTER_AddMember(onUpdate, EntityPlayer::onUpdate)
_CLASSREGISTER_AddMember(playSound, EntityPlayer::playSound)
_CLASSREGISTER_AddMember(preparePlayerToSpawn, EntityPlayer::preparePlayerToSpawn)
_CLASSREGISTER_AddMember(readEntityFromNBT, EntityPlayer::readEntityFromNBT)
_CLASSREGISTER_AddMember(respawnPlayer, EntityPlayer::respawnPlayer)
_CLASSREGISTER_AddMember(sendPlayerAbilities, EntityPlayer::sendPlayerAbilities)
_CLASSREGISTER_AddMember(setAbsorptionAmount, EntityPlayer::setAbsorptionAmount)
_CLASSREGISTER_AddMember(setActivatedBlock, EntityPlayer::setActivatedBlock)
_CLASSREGISTER_AddMember(setCurrentItemOrArmor, EntityPlayer::setCurrentItemOrArmor)
_CLASSREGISTER_AddMember(setDead, EntityPlayer::setDead)
_CLASSREGISTER_AddMember(setGameType, EntityPlayer::setGameType)
_CLASSREGISTER_AddMember(setInWeb, EntityPlayer::setInWeb)
_CLASSREGISTER_AddMember(setItemInUse, EntityPlayer::setItemInUse)
_CLASSREGISTER_AddMember(setOnFire, EntityPlayer::setOnFire)
_CLASSREGISTER_AddMember(setScore, EntityPlayer::setScore)
_CLASSREGISTER_AddMember(setSpawnChunk, EntityPlayer::setSpawnChunk)
_CLASSREGISTER_AddMember(shouldHeal, EntityPlayer::shouldHeal)
_CLASSREGISTER_AddMember(simpleAttackPlayerWithCurrentItem, EntityPlayer::simpleAttackPlayerWithCurrentItem)
_CLASSREGISTER_AddMember(sleepInBedAt, EntityPlayer::sleepInBedAt)
_CLASSREGISTER_AddMember(stopUsingItem, EntityPlayer::stopUsingItem)
_CLASSREGISTER_AddMember(syncPlayerMovement, EntityPlayer::syncPlayerMovement)
_CLASSREGISTER_AddMember(triggerAchievement, EntityPlayer::triggerAchievement)
_CLASSREGISTER_AddMember(updateRidden, EntityPlayer::updateRidden)
_CLASSREGISTER_AddStaticMember(verifyRespawnCoordinates, EntityPlayer::verifyRespawnCoordinates)
_CLASSREGISTER_AddMember(wakeUpPlayer, EntityPlayer::wakeUpPlayer)
_CLASSREGISTER_AddMember(writeEntityToNBT, EntityPlayer::writeEntityToNBT)
_CLASSREGISTER_AddMember(xpBarCap, EntityPlayer::xpBarCap)
_CLASSREGISTER_AddMember(setTeam, EntityPlayer::setTeamId)
_CLASSREGISTER_AddMember(setTeamId, EntityPlayer::setTeamId)
_CLASSREGISTER_AddMember(addOwnVehicle, EntityPlayer::addOwnVehicle)
_CLASSREGISTER_AddMember(syncOwnVehicle, EntityPlayer::syncOwnVehicle)
_CLASSREGISTER_AddMember(getYaw, EntityPlayer::getYaw)
_CLASSREGISTER_AddMember(getSex, EntityPlayer::getSex)
_CLASSREGISTER_AddMember(isWatchMode, EntityPlayer::isWatchMode)
_CLASSREGISTER_AddMember(leaveVehicle, EntityPlayer::leaveVehicle)
_END_REGISTER_CLASS()




// Register Class FoodStats
_BEGIN_REGISTER_CLASS(FoodStats)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddCtor()
_CLASSREGISTER_AddMember(addExhaustion, FoodStats::addExhaustion)
_CLASSREGISTER_AddMember_Override(addStats, FoodStats::addStats,void ,int ,float )
_CLASSREGISTER_AddMember_Override(addStats1, FoodStats::addStats,void ,ItemFood * )
_CLASSREGISTER_AddMember(foodLevelListenable, FoodStats::foodLevelListenable)
_CLASSREGISTER_AddMember(getFoodLevel, FoodStats::getFoodLevel)
_CLASSREGISTER_AddMember(getPrevFoodLevel, FoodStats::getPrevFoodLevel)
_CLASSREGISTER_AddMember(getSaturationLevel, FoodStats::getSaturationLevel)
_CLASSREGISTER_AddMember(needFood, FoodStats::needFood)
_CLASSREGISTER_AddMember(onUpdate, FoodStats::onUpdate)
_CLASSREGISTER_AddMember(readNBT, FoodStats::readNBT)
_CLASSREGISTER_AddMember(saturationLevelListenable, FoodStats::saturationLevelListenable)
_CLASSREGISTER_AddMember(setFoodLevel, FoodStats::setFoodLevel)
_CLASSREGISTER_AddMember(setFoodSaturationLevel, FoodStats::setFoodSaturationLevel)
_CLASSREGISTER_AddMember(writeNBT, FoodStats::writeNBT)
_END_REGISTER_CLASS()




// Register Class PlayerCapabilities
_BEGIN_REGISTER_CLASS(PlayerCapabilities)
_CLASSREGISTER_AddCtor()
_CLASSREGISTER_AddMember(getFlySpeed, PlayerCapabilities::getFlySpeed)
_CLASSREGISTER_AddMember(getWalkSpeed, PlayerCapabilities::getWalkSpeed)
_CLASSREGISTER_AddMember(readCapabilitiesFromNBT, PlayerCapabilities::readCapabilitiesFromNBT)
_CLASSREGISTER_AddMember(setFlySpeed, PlayerCapabilities::setFlySpeed)
_CLASSREGISTER_AddMember(setPlayerWalkSpeed, PlayerCapabilities::setPlayerWalkSpeed)
_CLASSREGISTER_AddMember(writeCapabilitiesToNBT, PlayerCapabilities::writeCapabilitiesToNBT)
_END_REGISTER_CLASS()




// Register Class EntityXPOrb
_BEGIN_REGISTER_CLASS(EntityXPOrb)
_CLASSREGISTER_AddBaseClass(Entity)
_CLASSREGISTER_AddCtor(World * , const Vector3 & , int )
_CLASSREGISTER_AddCtor(World * )
_CLASSREGISTER_AddMember(attackEntityFrom, EntityXPOrb::attackEntityFrom)
_CLASSREGISTER_AddMember(canAttackWithItem, EntityXPOrb::canAttackWithItem)
_CLASSREGISTER_AddMember(getBrightnessForRender, EntityXPOrb::getBrightnessForRender)
_CLASSREGISTER_AddMember(getTextureByXP, EntityXPOrb::getTextureByXP)
_CLASSREGISTER_AddStaticMember(getXPSplit, EntityXPOrb::getXPSplit)
_CLASSREGISTER_AddMember(getXpValue, EntityXPOrb::getXpValue)
_CLASSREGISTER_AddMember(handleWaterMovement, EntityXPOrb::handleWaterMovement)
_CLASSREGISTER_AddMember(onCollideWithPlayer, EntityXPOrb::onCollideWithPlayer)
_CLASSREGISTER_AddMember(onUpdate, EntityXPOrb::onUpdate)
_CLASSREGISTER_AddMember(readEntityFromNBT, EntityXPOrb::readEntityFromNBT)
_CLASSREGISTER_AddMember(writeEntityToNBT, EntityXPOrb::writeEntityToNBT)
_END_REGISTER_CLASS()




// Register Class Explosion
_BEGIN_REGISTER_CLASS(Explosion)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddCtor(World * , Entity * , EntityLivingBase * , const Vector3 & , float )
_CLASSREGISTER_AddMember(doExplosionA, Explosion::doExplosionA)
_CLASSREGISTER_AddMember(doExplosionB, Explosion::doExplosionB)
_CLASSREGISTER_AddMember(getPlayerKnockbackMap, Explosion::getPlayerKnockbackMap)
_END_REGISTER_CLASS()




// Register Class PathFinder
_BEGIN_REGISTER_CLASS(PathFinder)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddCtor(IBlockAccess * , bool , bool , bool , bool )
_CLASSREGISTER_AddMember_Override(createEntityPathTo, PathFinder::createEntityPathTo,PathEntity * ,Entity * ,Entity * ,float )
_CLASSREGISTER_AddMember_Override(createEntityPathTo1, PathFinder::createEntityPathTo,PathEntity * ,Entity * ,const BlockPos & ,float )
_CLASSREGISTER_AddMember_Override(createEntityPathTo2, PathFinder::createEntityPathTo,PathEntity * ,Entity * ,const Vector3 & ,float )
_CLASSREGISTER_AddStaticMember(func_82565_a, PathFinder::func_82565_a)
_CLASSREGISTER_AddMember(getVerticalOffset, PathFinder::getVerticalOffset)
_END_REGISTER_CLASS()




// Register Class PathHeap
_BEGIN_REGISTER_CLASS(PathHeap)
_CLASSREGISTER_AddCtor()
_CLASSREGISTER_AddMember(addPoint, PathHeap::addPoint)
_CLASSREGISTER_AddMember(changeDistance, PathHeap::changeDistance)
_CLASSREGISTER_AddMember(clearPath, PathHeap::clearPath)
_CLASSREGISTER_AddMember(dequeue, PathHeap::dequeue)
_CLASSREGISTER_AddMember(isPathEmpty, PathHeap::isPathEmpty)
_END_REGISTER_CLASS()




// Register Class PathEntity
_BEGIN_REGISTER_CLASS(PathEntity)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddCtor(PathPoint *  * , int )
_CLASSREGISTER_AddMember(getCurrentPathIndex, PathEntity::getCurrentPathIndex)
_CLASSREGISTER_AddMember(getCurrentPathLength, PathEntity::getCurrentPathLength)
_CLASSREGISTER_AddMember(getFinalPathPoint, PathEntity::getFinalPathPoint)
_CLASSREGISTER_AddMember(getPathPointFromIndex, PathEntity::getPathPointFromIndex)
_CLASSREGISTER_AddMember(getPosition, PathEntity::getPosition)
_CLASSREGISTER_AddMember(getVectorFromIndex, PathEntity::getVectorFromIndex)
_CLASSREGISTER_AddMember(incrementPathIndex, PathEntity::incrementPathIndex)
_CLASSREGISTER_AddMember(isDestinationSame, PathEntity::isDestinationSame)
_CLASSREGISTER_AddMember(isFinished, PathEntity::isFinished)
_CLASSREGISTER_AddMember(isSamePath, PathEntity::isSamePath)
_CLASSREGISTER_AddMember(setCurrentPathIndex, PathEntity::setCurrentPathIndex)
_CLASSREGISTER_AddMember(setCurrentPathLength, PathEntity::setCurrentPathLength)
_END_REGISTER_CLASS()




// Register Class PathNavigate
_BEGIN_REGISTER_CLASS(PathNavigate)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddCtor(EntityLiving & , World * )
_CLASSREGISTER_AddMember(clearPathEntity, PathNavigate::clearPathEntity)
_CLASSREGISTER_AddMember(getAvoidsWater, PathNavigate::getAvoidsWater)
_CLASSREGISTER_AddMember(getCanBreakDoors, PathNavigate::getCanBreakDoors)
_CLASSREGISTER_AddMember(getPath, PathNavigate::getPath)
_CLASSREGISTER_AddMember(getPathToEntityLiving, PathNavigate::getPathToEntityLiving)
_CLASSREGISTER_AddMember(getPathToXYZ, PathNavigate::getPathToXYZ)
_CLASSREGISTER_AddMember(getSearchRange, PathNavigate::getSearchRange)
_CLASSREGISTER_AddMember(noPath, PathNavigate::noPath)
_CLASSREGISTER_AddMember(onUpdateNavigation, PathNavigate::onUpdateNavigation)
_CLASSREGISTER_AddMember(setAvoidSun, PathNavigate::setAvoidSun)
_CLASSREGISTER_AddMember(setAvoidsWater, PathNavigate::setAvoidsWater)
_CLASSREGISTER_AddMember(setBreakDoors, PathNavigate::setBreakDoors)
_CLASSREGISTER_AddMember(setCanSwim, PathNavigate::setCanSwim)
_CLASSREGISTER_AddMember(setEnterDoors, PathNavigate::setEnterDoors)
_CLASSREGISTER_AddMember(setPath, PathNavigate::setPath)
_CLASSREGISTER_AddMember(setSpeed, PathNavigate::setSpeed)
_CLASSREGISTER_AddMember(tryMoveToEntityLiving, PathNavigate::tryMoveToEntityLiving)
_CLASSREGISTER_AddMember(tryMoveToXYZ, PathNavigate::tryMoveToXYZ)
_END_REGISTER_CLASS()




// Register Class PathPoint
_BEGIN_REGISTER_CLASS(PathPoint)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddCtor(const BlockPos & )
_CLASSREGISTER_AddMember(distanceTo, PathPoint::distanceTo)
_CLASSREGISTER_AddMember(distanceToSqr, PathPoint::distanceToSqr)
_CLASSREGISTER_AddMember(equals, PathPoint::equals)
_CLASSREGISTER_AddMember(hashCode, PathPoint::hashCode)
_CLASSREGISTER_AddMember(isAssigned, PathPoint::isAssigned)
_CLASSREGISTER_AddStaticMember(makeHash, PathPoint::makeHash)
_END_REGISTER_CLASS()

// Register Class EntityThrowable
_BEGIN_REGISTER_CLASS(EntityThrowable)
_CLASSREGISTER_AddBaseClass(Entity)
_END_REGISTER_CLASS()

// Register Class EntitySkillThrowable
_BEGIN_REGISTER_CLASS(EntitySkillThrowable)
_CLASSREGISTER_AddBaseClass(EntityThrowable)
_END_REGISTER_CLASS()




