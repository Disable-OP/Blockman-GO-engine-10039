#include "Potion.h"

#include "Entity/DamageSource.h"
#include "Entity/EntityLivingBase.h"
#include "Entity/EntityPlayer.h"
#include "World/World.h"

namespace BLOCKMAN
{

Potion* Potion::potionTypes[MAX_POTION_COUNT];
Potion* Potion::moveSpeed		= NULL;		
Potion* Potion::moveSlowdown = NULL;
Potion* Potion::digSpeed = NULL;
Potion* Potion::digSlowdown = NULL;
Potion* Potion::damageBoost = NULL;
Potion* Potion::heal = NULL;
Potion* Potion::harm = NULL;
Potion* Potion::jump = NULL;
Potion* Potion::confusion = NULL;
Potion* Potion::regeneration = NULL;
Potion* Potion::resistance = NULL;
Potion* Potion::fireResistance = NULL;
Potion* Potion::waterBreathing = NULL;
Potion* Potion::invisibility = NULL;
Potion* Potion::blindness = NULL;
Potion* Potion::nightVision = NULL;
Potion* Potion::hunger = NULL;
Potion* Potion::weakness = NULL;
Potion* Potion::poison = NULL;
Potion* Potion::wither = NULL;
Potion* Potion::healthBoost = NULL;
Potion* Potion::absorption = NULL;
Potion* Potion::saturation = NULL;

void Potion::initialize()
{
	memset(potionTypes, 0, sizeof(Potion*) * MAX_POTION_COUNT);
	moveSpeed = LordNew Potion(1, false, 8171462);
	moveSpeed->setPotionName("potion.moveSpeed");
	moveSpeed->setIconIndex(0, 0);
	moveSpeed->registerPotionAttributeModifier(SharedMonsterAttributes::MOVEMENT_SPEED, "91AEAA56-376B-4498-935B-2F7F68070635", 0.2f, 2);

	moveSlowdown = LordNew Potion(2, true, 5926017);
	moveSlowdown->setPotionName("potion.moveSlowdown");
	moveSlowdown->setIconIndex(1, 0);
	moveSlowdown->registerPotionAttributeModifier(SharedMonsterAttributes::MOVEMENT_SPEED, "7107DE5E-7CE8-4030-940E-514C1F160890", -0.15f, 2);

	digSpeed = LordNew Potion(3, false, 14270531);
	digSpeed->setPotionName("potion.digSpeed");
	digSpeed->setIconIndex(2, 0);
	digSpeed->setEffectiveness(1.5f);

	digSlowdown = LordNew Potion(4, true, 4866583);
	digSlowdown->setPotionName("potion.digSlowDown");
	digSlowdown->setIconIndex(3, 0);

	damageBoost = LordNew PotionAttackDamage(5, false, 9643043);
	damageBoost->setPotionName("potion.damageBoost");
	damageBoost->setIconIndex(4, 0);
	damageBoost->registerPotionAttributeModifier(SharedMonsterAttributes::ATTACK_DAMAGE, "648D7064-6A60-4F59-8ABE-C2C23A6DD7A9", 3.0f, 2);

	heal = LordNew PotionHealth(6, false, 16262179);
	heal->setPotionName("potion.heal");

	harm = LordNew PotionHealth(7, true, 4393481);
	harm->setPotionName("potion.harm");

	jump = LordNew Potion(8, false, 7889559);
	jump->setPotionName("potion.jump");
	jump->setIconIndex(2, 1);

	confusion = LordNew Potion(9, true, 5578058);
	confusion->setPotionName("potion.confusion");
	confusion->setIconIndex(3, 1);
	confusion->setEffectiveness(0.25f);

	regeneration = LordNew Potion(10, false, 13458603);
	regeneration->setPotionName("potion.regeneration");
	regeneration->setIconIndex(7, 0);
	regeneration->setEffectiveness(0.25f);

	resistance = LordNew Potion(11, false, 10044730);
	resistance->setPotionName("potion.resistance");
	resistance->setIconIndex(6, 1);

	fireResistance = LordNew Potion(12, false, 14981690);
	fireResistance->setPotionName("potion.fireResistance");
	fireResistance->setIconIndex(7, 1);

	waterBreathing = LordNew Potion(13, false, 3035801);
	waterBreathing->setPotionName("potion.waterBreathing");
	waterBreathing->setIconIndex(0, 2);

	invisibility = LordNew Potion(14, false, 8356754);
	invisibility->setPotionName("potion.invisibility");
	invisibility->setIconIndex(0, 1);

	blindness = LordNew Potion(15, true, 2039587);
	blindness->setPotionName("potion.blindness");
	blindness->setIconIndex(5, 1);
	blindness->setEffectiveness(0.25f);

	nightVision = LordNew Potion(16, false, 2039713);
	nightVision->setPotionName("potion.nightVision");
	nightVision->setIconIndex(4, 1);

	hunger = LordNew Potion(17, true, 5797459);
	hunger->setPotionName("potion.hunger");
	hunger->setIconIndex(1, 1);

	weakness = LordNew PotionAttackDamage(18, true, 4738376);
	weakness->setPotionName("potion.weakness");
	weakness->setIconIndex(5, 0);
	weakness->registerPotionAttributeModifier(SharedMonsterAttributes::ATTACK_DAMAGE, "22653B89-116E-49DC-9B6B-9971489B5BE5", 2.0f, 0);

	poison = LordNew Potion(19, true, 5149489);
	poison->setPotionName("potion.poison");
	poison->setIconIndex(6, 0);
	poison->setEffectiveness(0.25f);

	wither = LordNew Potion(20, true, 3484199);
	wither->setPotionName("potion.wither");
	wither->setIconIndex(1, 2);
	wither->setEffectiveness(0.25f);

	healthBoost = LordNew PotionHealthBoost(21, false, 16284963);
	healthBoost->setPotionName("potion.healthBoost");
	healthBoost->setIconIndex(2, 2);
	healthBoost->registerPotionAttributeModifier(SharedMonsterAttributes::EAMAX_HEALTH, "5D6F0BA2-1186-46AC-B896-C61C5CEE99CC", 4.0f, 0);

	absorption = LordNew PotionAbsoption(22, false, 2445989);
	absorption->setPotionName("potion.absorption");
	absorption->setIconIndex(2, 2);

	saturation = LordNew PotionHealth(23, false, 16262179);
	saturation->setPotionName("potion.saturation");
}

void Potion::unInitialize()
{
	for (int i = 0; i < MAX_POTION_COUNT; ++i)
	{
		LordSafeDelete(potionTypes[i]);
	}
}

Potion::Potion(int idIn, bool badEffect, int color)
	: id(idIn)
	, m_isBadEffect(badEffect)
	, liquidColor(color)
	, statusIconIndex(-1)
	, beneficial(false)
{
	potionTypes[idIn] = this;

	if (m_isBadEffect)
		effectiveness = 0.5f;
	else
		effectiveness = 1.0f;
}

Potion::~Potion()
{
	for (AttributeModifierMap::iterator it = attributeModifierMap.begin();
		it != attributeModifierMap.end(); ++it)
	{
		AttributeModifier* pModifer = it->second;
		LordSafeDelete(pModifer);
	}
}

void Potion::performEffect(EntityLivingBase* pLiving, int amplifier)
{
	EntityPlayer* pplayer = dynamic_cast<EntityPlayer*>(pLiving);
	if (id == regeneration->id)
	{
		if (pLiving->getHealth() < pLiving->getMaxHealth())
			pLiving->heal(1.0F);
	}
	else if (id == poison->id)
	{
		if (pLiving->getHealth() > 1.0F)
			pLiving->attackEntityFrom(DamageSource::magic, 1.0F);
	}
	else if (id == wither->id)
	{
		pLiving->attackEntityFrom(DamageSource::wither, 1.0F);
	}
	else if (id == hunger->id && pplayer)
	{
		pplayer->addExhaustion(0.025F * (float)(amplifier + 1));
	}
	else if (id == saturation->id && pplayer)
	{
		if (!pLiving->world->m_isClient)
		{
			pplayer->getFoodStats()->addStats(amplifier + 1, 1.0F);
		}
	}
	else if ((id != heal->id || pLiving->isEntityUndead()) && (id != harm->id || !pLiving->isEntityUndead()))
	{
		if (id == harm->id && !pLiving->isEntityUndead() || id == heal->id && pLiving->isEntityUndead())
		{
			pLiving->attackEntityFrom(DamageSource::magic, (float)(6 << amplifier));
		}
	}
	else
	{
		pLiving->heal((float)Math::Max(4 << amplifier, 0));
	}
}

void Potion::affectEntity(Entity* source, EntityLivingBase* indirectSource, EntityLivingBase* entityLivingBaseIn, int amplifier, double health)
{
	int amount;

	if ((id != heal->id || entityLivingBaseIn->isEntityUndead()) && (id != harm->id || !entityLivingBaseIn->isEntityUndead()))
	{
		if ((id == harm->id && !entityLivingBaseIn->isEntityUndead()) || (id == heal->id && entityLivingBaseIn->isEntityUndead()))
		{
			amount = (int)(health * (float)(6 << amplifier) + 0.5f);

			if (source == NULL)
			{
				entityLivingBaseIn->attackEntityFrom(DamageSource::magic, (float)amount);
			}
			else
			{
				entityLivingBaseIn->attackEntityFrom(DamageSource::causeIndirectMagicDamage(source, indirectSource), (float)amount);
			}
		}
	}
	else
	{
		amount = (int)(health * (4 << amplifier) + 0.5f);
		entityLivingBaseIn->heal((float)amount);
	}
}

bool Potion::isReady(int duration, int amplifier)
{
	int i;

	if (id == regeneration->id)
	{
		i = 50 >> amplifier;
		return i > 0 ? duration % i == 0 : true;
	}
	else if (id == poison->id)
	{
		i = 25 >> amplifier;
		return i > 0 ? duration % i == 0 : true;
	}
	else if (id == wither->id)
	{
		i = 40 >> amplifier;
		return i > 0 ? duration % i == 0 : true;
	}
	return id == hunger->id;
}

String Potion::getDurationString(PotionEffect* pEffect)
{
	if (pEffect->getIsPotionDurationMax())
	{
		return "**:**";
	}
	else
	{
		int duration = pEffect->getDuration();
		int second = duration / 20;
		int minute = second / 60;
		second %= 60;
		return StringUtil::Format("%d:%2d", minute, second);
	}
}

void Potion::registerPotionAttributeModifier(IAttribute* pAttribute, const String& strID, float ammount, int operation)
{
	AttributeModifier* pModifer = LordNew AttributeModifier(UUID::fromString(strID), getName(), ammount, operation);
	attributeModifierMap.insert(std::make_pair(pAttribute, pModifer));
}

void Potion::removeAttributesModifiersFromEntity(EntityLivingBase* pLiving, IAttributeMap* pAttMap, int amplifier)
{
	for (AttributeModifierMap::iterator it = attributeModifierMap.begin(); it != attributeModifierMap.end(); ++it)
	{
		IAttributeInstance* pinstance = pAttMap->getAttributeInstance(it->first);
		if (!pinstance)
			continue;
		pinstance->removeModifier(it->second);
	}
}

void Potion::applyAttributesModifiersToEntity(EntityLivingBase* pLiving, IAttributeMap* pAttMap, int amplifier)
{
	for (AttributeModifierMap::iterator it = attributeModifierMap.begin(); it != attributeModifierMap.end(); ++it)
	{
		IAttributeInstance* pinstance = pAttMap->getAttributeInstance(it->first);
		if (!pinstance)
			continue;

		AttributeModifier* attributemodifier = it->second;
		pinstance->removeModifier(attributemodifier);
		String applyName = getName() + " " + StringUtil::ToString(amplifier);
		float amount = getAttributeModifierAmount(amplifier, attributemodifier);
		int operation = attributemodifier->getOperation();
		AttributeModifier* m = LordNew AttributeModifier(attributemodifier->getID(), applyName, amount, operation);
		m->setNeedFree(true);
		pinstance->applyModifier(m);
	}
}

PotionAttackDamage::PotionAttackDamage(int id, bool badEffect, int color)
	: Potion(id, badEffect, color)
{}

float PotionAttackDamage::getAttributeModifierAmount(int amplifier, AttributeModifier* modifier)
{
	return id == Potion::weakness->getId() ? float(-0.5F * (float)(amplifier + 1)) : 1.3f * (float)(amplifier + 1);
}

PotionHealth::PotionHealth(int id, bool badEffect, int color)
	: Potion(id, badEffect, color)
{}

PotionHealthBoost::PotionHealthBoost(int id, bool badEffect, int color)
	: Potion(id, badEffect, color)
{}

void PotionHealthBoost::removeAttributesModifiersFromEntity(EntityLivingBase* pLiving, IAttributeMap* pAttMap, int amplifier)
{
	Potion::removeAttributesModifiersFromEntity(pLiving, pAttMap, amplifier);

	if (pLiving->getHealth() > pLiving->getMaxHealth())
	{
		pLiving->setEntityHealth(pLiving->getMaxHealth());
	}
}

PotionAbsoption::PotionAbsoption(int id, bool badEffect, int color)
	: Potion(id, badEffect, color)
{}

void PotionAbsoption::removeAttributesModifiersFromEntity(EntityLivingBase* pLiving, IAttributeMap* pAttMap, int amplifier)
{
	pLiving->setAbsorptionAmount(pLiving->getAbsorptionAmount() - (float)(4 * (amplifier + 1)));
	Potion::removeAttributesModifiersFromEntity(pLiving, pAttMap, amplifier);
}

void PotionAbsoption::applyAttributesModifiersToEntity(EntityLivingBase* pLiving, IAttributeMap* pAttMap, int amplifier)
{
	pLiving->setAbsorptionAmount(pLiving->getAbsorptionAmount() + (float)(4 * (amplifier + 1)));
	Potion::applyAttributesModifiersToEntity(pLiving, pAttMap, amplifier);
}

PotionEffect::PotionEffect(int potionIn)
	: potionID(potionIn)
	, duration(0)
	, amplifier(0)
	, isSplashPotion(false)
	, isAmbient(false)
	, isPotionDurationMax(false)
{}

PotionEffect::PotionEffect(int potionIn, int durationIn)
	: potionID(potionIn)
	, duration(durationIn)
	, amplifier(0)
	, isSplashPotion(false)
	, isAmbient(false)
	, isPotionDurationMax(false)
{}

PotionEffect::PotionEffect(int potionIn, int durationIn, int amplifierIn)
	: potionID(potionIn)
	, duration(durationIn)
	, amplifier(amplifierIn)
	, isSplashPotion(false)
	, isAmbient(false)
	, isPotionDurationMax(false)
{}

PotionEffect::PotionEffect(int potionIn, int durationIn, int amplifierIn, bool ambientIn)
	: potionID(potionIn)
	, duration(durationIn)
	, amplifier(amplifierIn)
	, isSplashPotion(false)
	, isAmbient(ambientIn)
	, isPotionDurationMax(false)
{}

PotionEffect::PotionEffect(const PotionEffect& other)
{
	potionID = other.potionID;
	duration = other.duration;
	amplifier = other.amplifier;
	isSplashPotion = other.isSplashPotion;
	isAmbient = other.isAmbient;
	isPotionDurationMax = other.isPotionDurationMax;
}

void PotionEffect::combine(PotionEffect* other)
{
	if (potionID != other->potionID)
	{
		LordLogError("This method should only be called for matching effects!");
		return;
	}

	if (other->amplifier > amplifier)
	{
		amplifier = other->amplifier;
		duration = other->duration;
	}
	else if (other->amplifier == amplifier && duration < other->duration)
	{
		duration = other->duration;
	}
	else if (!other->isAmbient && isAmbient)
	{
		isAmbient = other->isAmbient;
	}
}

bool PotionEffect::onUpdate(EntityLivingBase* pLiving)
{
	if (duration > 0)
	{
		if (Potion::potionTypes[potionID]->isReady(duration, amplifier))
		{
			performEffect(pLiving);
		}

		deincrementDuration();
	}

	return duration > 0;
}

void PotionEffect::performEffect(EntityLivingBase* pLiving)
{
	if (duration > 0)
	{
		Potion::potionTypes[potionID]->performEffect(pLiving, amplifier);
	}
}

String PotionEffect::toString()
{
	String s;

	if (amplifier > 0)
	{
		s = getEffectName() + " x " + StringUtil::ToString(amplifier + 1) +
			", Duration: " + StringUtil::ToString(duration);
	}
	else
	{
		s = getEffectName() + ", Duration: " + StringUtil::ToString(duration);
	}

	if (isSplashPotion)
	{
		s = s + ", Splash: true";
	}

	return s;
}

const String& PotionEffect::getEffectName() const
{
	return Potion::potionTypes[potionID]->getName();
}

NBTTagCompound* PotionEffect::writeCustomPotionEffectToNBT(NBTTagCompound* nbt)
{
	nbt->setByte("Id", (i8)getPotionID());
	nbt->setByte("Amplifier", (i8)getAmplifier());
	nbt->setInteger("Duration", getDuration());
	nbt->setBool("Ambient", getIsAmbient());
	return nbt;
}

PotionEffect* PotionEffect::readCustomPotionEffectFromNBT(NBTTagCompound* nbt)
{
	i8 id = nbt->getByte("Id");
	i8 amplifier = nbt->getByte("Amplifier");
	int duration = nbt->getInteger("Duration");
	bool ambient = nbt->getBool("Ambient");
	return LordNew PotionEffect(id, amplifier, duration, ambient);
}

PotionRequirement PotionHelper::potionAmplifiers;
map<int, int>::type PotionHelper::field_77925_n;
StringArray PotionHelper::potionPrefixes;

String PotionHelper::field_77924_a;
String PotionHelper::sugarEffect;
String PotionHelper::ghastTearEffect = "+0-1-2-3&4-4+13";
String PotionHelper::spiderEyeEffect;
String PotionHelper::fermentedSpiderEyeEffect;
String PotionHelper::speckledMelonEffect;
String PotionHelper::blazePowderEffect;
String PotionHelper::magmaCreamEffect;
String PotionHelper::redstoneEffect;
String PotionHelper::glowstoneEffect;
String PotionHelper::gunpowderEffect;
String PotionHelper::goldenCarrotEffect;
PotionRequirement PotionHelper::potionRequirements;

void PotionHelper::initialize()
{
	potionRequirements.insert(std::make_pair(Potion::regeneration->getId(), "0 & !1 & !2 & !3 & 0+6"));
	sugarEffect = "-0+1-2-3&4-4+13";
	potionRequirements.insert(std::make_pair(Potion::moveSpeed->getId(), "!0 & 1 & !2 & !3 & 1+6"));
	magmaCreamEffect = "+0+1-2-3&4-4+13";
	potionRequirements.insert(std::make_pair(Potion::fireResistance->getId(), "0 & 1 & !2 & !3 & 0+6"));
	speckledMelonEffect = "+0-1+2-3&4-4+13";
	potionRequirements.insert(std::make_pair(Potion::heal->getId(), "0 & !1 & 2 & !3"));
	spiderEyeEffect = "-0-1+2-3&4-4+13";
	potionRequirements.insert(std::make_pair(Potion::poison->getId(), "!0 & !1 & 2 & !3 & 2+6"));
	fermentedSpiderEyeEffect = "-0+3-4+13";
	potionRequirements.insert(std::make_pair(Potion::weakness->getId(), "!0 & !1 & !2 & 3 & 3+6"));
	potionRequirements.insert(std::make_pair(Potion::harm->getId(), "!0 & !1 & 2 & 3"));
	potionRequirements.insert(std::make_pair(Potion::moveSlowdown->getId(), "!0 & 1 & !2 & 3 & 3+6"));
	blazePowderEffect = "+0-1-2+3&4-4+13";
	potionRequirements.insert(std::make_pair(Potion::damageBoost->getId(), "0 & !1 & !2 & 3 & 3+6"));
	goldenCarrotEffect = "-0+1+2-3+13&4-4";
	potionRequirements.insert(std::make_pair(Potion::nightVision->getId(), "!0 & 1 & 2 & !3 & 2+6"));
	potionRequirements.insert(std::make_pair(Potion::invisibility->getId(), "!0 & 1 & 2 & 3 & 2+6"));
	glowstoneEffect = "+5-6-7";
	potionAmplifiers.insert(std::make_pair(Potion::moveSpeed->getId(), "5"));
	potionAmplifiers.insert(std::make_pair(Potion::digSpeed->getId(), "5"));
	potionAmplifiers.insert(std::make_pair(Potion::damageBoost->getId(), "5"));
	potionAmplifiers.insert(std::make_pair(Potion::regeneration->getId(), "5"));
	potionAmplifiers.insert(std::make_pair(Potion::harm->getId(), "5"));
	potionAmplifiers.insert(std::make_pair(Potion::heal->getId(), "5"));
	potionAmplifiers.insert(std::make_pair(Potion::resistance->getId(), "5"));
	potionAmplifiers.insert(std::make_pair(Potion::poison->getId(), "5"));
	redstoneEffect = "-5+6-7";
	gunpowderEffect = "+14&13-13";
	potionPrefixes.push_back("potion.prefix.mundane");
	potionPrefixes.push_back("potion.prefix.uninteresting");
	potionPrefixes.push_back("potion.prefix.bland");
	potionPrefixes.push_back("potion.prefix.clear");
	potionPrefixes.push_back("potion.prefix.milky");
	potionPrefixes.push_back("potion.prefix.diffuse");
	potionPrefixes.push_back("potion.prefix.artless");
	potionPrefixes.push_back("potion.prefix.thin");
	potionPrefixes.push_back("potion.prefix.awkward");
	potionPrefixes.push_back("potion.prefix.flat");
	potionPrefixes.push_back("potion.prefix.bulky");
	potionPrefixes.push_back("potion.prefix.bungling");
	potionPrefixes.push_back("potion.prefix.buttered");
	potionPrefixes.push_back("potion.prefix.smooth");
	potionPrefixes.push_back("potion.prefix.suave");
	potionPrefixes.push_back("potion.prefix.debonair");
	potionPrefixes.push_back("potion.prefix.thick");
	potionPrefixes.push_back("potion.prefix.elegant");
	potionPrefixes.push_back("potion.prefix.fancy");
	potionPrefixes.push_back("potion.prefix.charming");
	potionPrefixes.push_back("potion.prefix.dashing");
	potionPrefixes.push_back("potion.prefix.refined");
	potionPrefixes.push_back("potion.prefix.cordial");
	potionPrefixes.push_back("potion.prefix.sparkling");
	potionPrefixes.push_back("potion.prefix.potent");
	potionPrefixes.push_back("potion.prefix.foul");
	potionPrefixes.push_back("potion.prefix.odorless");
	potionPrefixes.push_back("potion.prefix.rank");
	potionPrefixes.push_back("potion.prefix.harsh");
	potionPrefixes.push_back("potion.prefix.acrid");
	potionPrefixes.push_back("potion.prefix.gross");
	potionPrefixes.push_back("potion.prefix.stinky");
}

int PotionHelper::calcPotionLiquidColor(const PotionEffectArr& effects)
{
	if (effects.empty())
		return 0x00C65D38;

	float r = 0.0F;
	float g = 0.0F;
	float b = 0.0F;
	float var5 = 0.0F;

	for (PotionEffectArr::const_iterator it = effects.begin(); it != effects.end(); ++it)
	{
		PotionEffect* effect = *it;
		int color = Potion::potionTypes[effect->getPotionID()]->getLiquidColor();

		if (effect->getPotionID() == 16)
		{
			continue;
		}

		for (int i = 0; i <= effect->getAmplifier(); ++i)
		{
			r += (float)(color >> 16 & 255) / 255.0F;
			g += (float)(color >> 8 & 255) / 255.0F;
			b += (float)(color >> 0 & 255) / 255.0F;
			++var5;
		}
	}

	r = r / var5 * 255.0F;
	g = g / var5 * 255.0F;
	b = b / var5 * 255.0F;
	return (int)r << 16 | (int)g << 8 | (int)b;
}

bool PotionHelper::isAmbient(const PotionEffectArr& effects)
{
	for (PotionEffectArr::const_iterator it = effects.begin(); it != effects.end(); ++it)
	{
		PotionEffect* pEffect = *it;
		if (!pEffect->getIsAmbient())
			return false;
	}
	return true;
}

int PotionHelper::getColorFromDamage(int par0, bool par1)
{
	if (!par1)
	{
		if (field_77925_n.find(par0) != field_77925_n.end())
		{
			return field_77925_n[par0];
		}
		else
		{
			PotionEffectArr potionEffects;
			getPotionEffects(par0, false, potionEffects);
			int color = calcPotionLiquidColor(potionEffects);
			field_77925_n.insert(std::make_pair(par0, color));
			return color;
		}
	}
	else
	{
		PotionEffectArr potionEffects;
		getPotionEffects(par0, par1, potionEffects);
		return calcPotionLiquidColor(potionEffects);
	}
}
//thxor, theunit, thsub, theop, unitv, tensv, damageType
int PotionHelper::func_77904_a(bool thxor, bool theunit, bool thsub, int theop, int unitv, int tensv, int damageType)
{
	int effectv = 0;

	if (thxor)
	{
		effectv = isFlagUnset(damageType, unitv);
	}
	else if (theop != -1)
	{
		if (theop == 0 && countSetFlags(damageType) == unitv)
		{
			effectv = 1;
		}
		else if (theop == 1 && countSetFlags(damageType) > unitv)
		{
			effectv = 1;
		}
		else if (theop == 2 && countSetFlags(damageType) < unitv)
		{
			effectv = 1;
		}
	}
	else
	{
		effectv = isFlagSet(damageType, unitv);
	}

	if (theunit)
	{
		effectv *= tensv;
	}

	if (thsub)
	{
		effectv *= -1;
	}

	return effectv;
}

int PotionHelper::countSetFlags(int damageType)
{
	int count;

	for (count = 0; damageType > 0; ++count)
	{
		damageType &= damageType - 1;
	}

	return count;
}

int PotionHelper::parsePotionEffects(const String& effectName, int startIndex, int endIndex, int meta)
{
	if (startIndex < int(effectName.length()) && endIndex >= 0 && startIndex < endIndex)
	{
		return parseAndExpression(std::string_view(effectName.c_str() + startIndex, endIndex - startIndex), meta);
	}
	return 0;
}

void PotionHelper::getPotionEffects(int damageType, bool includeUsable, PotionEffectArr& outarr)
{
	Potion** allPotions = Potion::potionTypes;

	for (int i = 0; i < Potion::MAX_POTION_COUNT; ++i)
	{
		Potion* pPotion = allPotions[i];

		if (!pPotion)
			continue;

		if (pPotion->isUsable() && !includeUsable)
			continue;;

		PotionRequirement::iterator it = potionRequirements.find(pPotion->getId());
		if (it == potionRequirements.end())
			continue;
		
		int effectvalue = parsePotionEffects(it->second, 0, it->second.length(), damageType);

		if (effectvalue > 0)
		{
			int amplifiers = 0;

			PotionRequirement::iterator jt = potionAmplifiers.find(pPotion->getId());
			if(jt != potionAmplifiers.end())
			{
				amplifiers = parsePotionEffects(jt->second, 0, jt->second.length(), damageType);

				if (amplifiers < 0)
				{
					amplifiers = 0;
				}
			}

			if (pPotion->isInstant())
			{
				effectvalue = 1;
			}
			else
			{
				effectvalue = 1200 * (effectvalue * 3 + (effectvalue - 1) * 2);
				effectvalue >>= amplifiers;
				effectvalue = (int)(Math::UnitRandom() * effectvalue * pPotion->getEffectiveness());

				if ((damageType & 16384) != 0)
				{
					effectvalue = (int)(Math::UnitRandom() * (effectvalue * 0.75f + 0.5f));
				}
			}

			PotionEffect* effect = LordNew PotionEffect(pPotion->getId(), effectvalue, amplifiers);

			if ((damageType & 16384) != 0)
			{
				effect->setSplashPotion(true);
			}

			outarr.push_back(effect);
		}
	}
}

int PotionHelper::brewBitOperations(int data, int index, bool removed, bool toggled, bool clear)
{
	if (clear)
	{
		if (!checkFlag(data, index))
		{
			return 0;
		}
	}
	else if (removed)
	{
		data &= ~(1 << index);
	}
	else if (toggled)
	{
		if ((data & 1 << index) == 0)
		{
			data |= 1 << index;
		}
		else
		{
			data &= ~(1 << index);
		}
	}
	else
	{
		data |= 1 << index;
	}

	return data;
}

int PotionHelper::parseOrExpression(std::string_view expression, int meta)
{
	auto opIndex = expression.find_first_of('|');

	if (opIndex == std::string_view::npos)
	{
		return parseAndExpression(expression, meta);
	}

	int parseLeft = parseAndExpression(expression.substr(0, opIndex), meta);
	if (parseLeft > 0)
	{
		return parseLeft;
	}

	int parseRight = parseOrExpression(expression.substr(opIndex + 1), meta);
	if (parseRight > 0)
	{
		return parseRight;
	}

	return 0;
}

int PotionHelper::parseAndExpression(std::string_view expression, int meta)
{
	int opIndex = expression.find_first_of('&');

	if (opIndex == std::string_view::npos)
	{
		return parseArithmeticExpression(expression, meta);
	}

	int parseLeft = parseArithmeticExpression(expression.substr(0, opIndex), meta);
	if (parseLeft <= 0)
	{
		return 0;
	}

	int parseRight = parseAndExpression(expression.substr(opIndex + 1), meta);
	if (parseRight <= 0)
	{
		return 0;
	}

	using std::max;
	return max(parseLeft, parseRight);
}

int PotionHelper::parseArithmeticExpression(std::string_view expression, int meta)
{
	bool thnums = false;
	bool theunit = false;
	bool thetens = false;
	bool thxor = false;
	bool thsub = false;
	i8 theop = -1;
	int unitv = 0;
	int tensv = 0;
	int effectv = 0;

	for (size_t i = 0; i < expression.size(); ++i)
	{
		char ch = expression[i];

		if (ch >= '0' && ch <= '9')
		{
			if (thnums)
			{
				tensv = ch - '0';
				theunit = true;
			}
			else
			{
				unitv *= 10;
				unitv += ch - '0';
				thetens = true;
			}
		}
		else if (ch == '*')
		{
			thnums = true;
		}
		else if (ch == '!')
		{
			if (thetens)
			{
				effectv += func_77904_a(thxor, theunit, thsub, theop, unitv, tensv, meta);
				thxor = false;
				thsub = false;
				thnums = false;
				theunit = false;
				thetens = false;
				tensv = 0;
				unitv = 0;
				theop = -1;
			}

			thxor = true;
		}
		else if (ch == '-')
		{
			if (thetens)
			{
				effectv += func_77904_a(thxor, theunit, thsub, theop, unitv, tensv, meta);
				thxor = false;
				thsub = false;
				thnums = false;
				theunit = false;
				thetens = false;
				tensv = 0;
				unitv = 0;
				theop = -1;
			}

			thsub = true;
		}
		else if (ch != '=' && ch != '<' && ch != '>')
		{
			if (ch == '+' && thetens)
			{
				effectv += func_77904_a(thxor, theunit, thsub, theop, unitv, tensv, meta);
				thxor = false;
				thsub = false;
				thnums = false;
				theunit = false;
				thetens = false;
				tensv = 0;
				unitv = 0;
				theop = -1;
			}
		}
		else
		{
			if (thetens)
			{
				effectv += func_77904_a(thxor, theunit, thsub, theop, unitv, tensv, meta);
				thxor = false;
				thsub = false;
				thnums = false;
				theunit = false;
				thetens = false;
				tensv = 0;
				unitv = 0;
				theop = -1;
			}

			if (ch == '=')
			{
				theop = 0;
			}
			else if (ch == '<')
			{
				theop = 2;
			}
			else if (ch == '>')
			{
				theop = 1;
			}
		}
	}

	if (thetens)
	{
		effectv += func_77904_a(thxor, theunit, thsub, theop, unitv, tensv, meta);
	}

	return effectv;
}

int PotionHelper::applyIngredient(int damageType, const String& effectStr)
{
	int len = effectStr.length();
	bool thnums = false;
	bool toggled = false;
	bool removed = false;
	bool clear = false;
	int effectidx = 0;

	for (int i = 0; i < len; ++i)
	{
		char ch = effectStr[i];

		if (ch >= '0' && ch <= '9')
		{
			effectidx *= 10;
			effectidx += ch - '0';
			thnums = true;
		}
		else if (ch == '!')
		{
			if (thnums)
			{
				damageType = brewBitOperations(damageType, effectidx, removed, toggled, clear);
				clear = false;
				toggled = false;
				removed = false;
				thnums = false;
				effectidx = 0;
			}

			toggled = true;
		}
		else if (ch == '-')
		{
			if (thnums)
			{
				damageType = brewBitOperations(damageType, effectidx, removed, toggled, clear);
				clear = false;
				toggled = false;
				removed = false;
				thnums = false;
				effectidx = 0;
			}

			removed = true;
		}
		else if (ch == '+')
		{
			if (thnums)
			{
				damageType = brewBitOperations(damageType, effectidx, removed, toggled, clear);
				clear = false;
				toggled = false;
				removed = false;
				thnums = false;
				effectidx = 0;
			}
		}
		else if (ch == '&')
		{
			if (thnums)
			{
				damageType = brewBitOperations(damageType, effectidx, removed, toggled, clear);
				clear = false;
				toggled = false;
				removed = false;
				thnums = false;
				effectidx = 0;
			}

			clear = true;
		}
	}

	if (thnums)
	{
		damageType = brewBitOperations(damageType, effectidx, removed, toggled, clear);
	}

	return damageType & 32767;
}

int PotionHelper::func_77908_a(int damageType, int par1, int par2, int par3, int par4, int par5)
{
	return (checkFlag(damageType, par1) ? 16 : 0) | (checkFlag(damageType, par2) ? 8 : 0) | (checkFlag(damageType, par3) ? 4 : 0) | (checkFlag(damageType, par4) ? 2 : 0) | (checkFlag(damageType, par5) ? 1 : 0);
}

}