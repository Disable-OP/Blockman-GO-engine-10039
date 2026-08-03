/********************************************************************
filename: 	Potion.h
file path:	dev\client\Src\Blockman\Item

version:	1
author:		ajohn
company:	supernano
date:		2017-2-14
*********************************************************************/
#ifndef __POTION_HEADER__
#define __POTION_HEADER__

#include "BM_Container_def.h"
#include "Entity/Attributes.h"
#if (LORD_PLATFORM  != LORD_PLATFORM_MAC_IOS)
#include "Std/string_view.h"
#endif

#include "Core.h"
using namespace LORD;

namespace BLOCKMAN
{

class PotionEffect;
class EntityLivingBase;

class Potion : public ObjectAlloc
{
public:
	static const int MAX_POTION_COUNT = 32;
	static Potion* potionTypes[MAX_POTION_COUNT];
	static Potion* moveSpeed;		// = (new Potion(1, false, 8171462)).setPotionName("potion.moveSpeed").setIconIndex(0, 0).func_111184_a(SharedMonsterAttributes.field_111263_d, "91AEAA56-376B-4498-935B-2F7F68070635", 0.20000000298023224D, 2);
	static Potion* moveSlowdown;	// = (new Potion(2, true, 5926017)).setPotionName("potion.moveSlowdown").setIconIndex(1, 0).func_111184_a(SharedMonsterAttributes.field_111263_d, "7107DE5E-7CE8-4030-940E-514C1F160890", -0.15000000596046448D, 2);
	static Potion* digSpeed;		// = (new Potion(3, false, 14270531)).setPotionName("potion.digSpeed").setIconIndex(2, 0).setEffectiveness(1.5D);
	static Potion* digSlowdown;		// = (new Potion(4, true, 4866583)).setPotionName("potion.digSlowDown").setIconIndex(3, 0);
	static Potion* damageBoost;		// = (new PotionAttackDamage(5, false, 9643043)).setPotionName("potion.damageBoost").setIconIndex(4, 0).func_111184_a(SharedMonsterAttributes.field_111264_e, "648D7064-6A60-4F59-8ABE-C2C23A6DD7A9", 3.0D, 2);
	static Potion* heal;			// = (new PotionHealth(6, false, 16262179)).setPotionName("potion.heal");
	static Potion* harm;			// = (new PotionHealth(7, true, 4393481)).setPotionName("potion.harm");
	static Potion* jump;			// = (new Potion(8, false, 7889559)).setPotionName("potion.jump").setIconIndex(2, 1);
	static Potion* confusion;		// = (new Potion(9, true, 5578058)).setPotionName("potion.confusion").setIconIndex(3, 1).setEffectiveness(0.25D);
	static Potion* regeneration;	// = (new Potion(10, false, 13458603)).setPotionName("potion.regeneration").setIconIndex(7, 0).setEffectiveness(0.25D);
	static Potion* resistance;		// = (new Potion(11, false, 10044730)).setPotionName("potion.resistance").setIconIndex(6, 1);
	static Potion* fireResistance;	// = (new Potion(12, false, 14981690)).setPotionName("potion.fireResistance").setIconIndex(7, 1);
	static Potion* waterBreathing;	// = (new Potion(13, false, 3035801)).setPotionName("potion.waterBreathing").setIconIndex(0, 2);
	static Potion* invisibility;	// = (new Potion(14, false, 8356754)).setPotionName("potion.invisibility").setIconIndex(0, 1);
	static Potion* blindness;		// = (new Potion(15, true, 2039587)).setPotionName("potion.blindness").setIconIndex(5, 1).setEffectiveness(0.25D);
	static Potion* nightVision;		// = (new Potion(16, false, 2039713)).setPotionName("potion.nightVision").setIconIndex(4, 1);
	static Potion* hunger;			// = (new Potion(17, true, 5797459)).setPotionName("potion.hunger").setIconIndex(1, 1);
	static Potion* weakness;		// = (new PotionAttackDamage(18, true, 4738376)).setPotionName("potion.weakness").setIconIndex(5, 0).func_111184_a(SharedMonsterAttributes.field_111264_e, "22653B89-116E-49DC-9B6B-9971489B5BE5", 2.0D, 0);
	static Potion* poison;			// = (new Potion(19, true, 5149489)).setPotionName("potion.poison").setIconIndex(6, 0).setEffectiveness(0.25D);
	static Potion* wither;			// = (new Potion(20, true, 3484199)).setPotionName("potion.wither").setIconIndex(1, 2).setEffectiveness(0.25D);
	static Potion* healthBoost;		// = (new PotionHealthBoost(21, false, 16284963)).setPotionName("potion.healthBoost").setIconIndex(2, 2).func_111184_a(SharedMonsterAttributes.field_111267_a, "5D6F0BA2-1186-46AC-B896-C61C5CEE99CC", 4.0D, 0);
	static Potion* absorption;		// = (new PotionAbsoption(22, false, 2445989)).setPotionName("potion.absorption").setIconIndex(2, 2);
	static Potion* saturation;		// = (new PotionHealth(23, false, 16262179)).setPotionName("potion.saturation");

protected:
	typedef map<IAttribute*, AttributeModifier*>::type AttributeModifierMap;
	AttributeModifierMap	attributeModifierMap;
	int id = 0;
	bool m_isBadEffect = false;
	/** Is the color of the liquid for this potion. */
	int liquidColor = 0;
	/** The name of the Potion. */
	String name; // = "";
	/** The index for the icon displayed when the potion effect is active. */
	int statusIconIndex = -1; // = -1;
	float effectiveness = 0.f;
	bool beneficial = false;

protected:
	Potion(int idIn, bool badEffect, int color);
	virtual ~Potion();

	/** Sets the index for the icon displayed in the player's inventory when the status is active.*/
	void setIconIndex(int offset, int slot) { statusIconIndex = offset + slot * 8; }
	void setEffectiveness(float e) { effectiveness = e; }

public:
	static void initialize();
	static void unInitialize();

	/** returns the ID of the potion */
	int getId() { return id; }
	void performEffect(EntityLivingBase* pLiving, int t);
	/** Hits the provided entity with this potion's instant effect. */
	void affectEntity(Entity* source, EntityLivingBase* indirectSource, EntityLivingBase* entityLivingBaseIn, int amplifier, double health);
	/** Returns true if the potion has an instant effect instead of a continuous one (eg Harming) */
	virtual bool isInstant() { return false; }
	/** checks if Potion effect is ready to be applied this tick. */
	virtual bool isReady(int par1, int par2);
	/** Set the potion name. */
	void setPotionName(const String& n) { name = n; }
	/** returns the name of the potion */
	const String& getName() { return name; }
	/** Returns true if the potion has a associated status icon to display in then inventory when active. */
	bool hasStatusIcon() { return statusIconIndex >= 0; }
	/** Returns the index for the icon to display when the potion is active. */
	int getStatusIconIndex() { return statusIconIndex; }
	/** This method returns true if the potion effect is bad - negative - for the entity. */
	bool isBadEffect() { return m_isBadEffect; }
	static String getDurationString(PotionEffect* pEffect);
	float getEffectiveness() { return effectiveness; }
	bool isUsable() { return beneficial; }
	/** Returns the color of the potion liquid. */
	int getLiquidColor() { return liquidColor; }
	void registerPotionAttributeModifier(IAttribute* pAttribute, const String& strID, float ammount, int operation);
	const AttributeModifierMap& getAttributeModifierMap() const { return attributeModifierMap; }
	virtual void removeAttributesModifiersFromEntity(EntityLivingBase* pLiving, IAttributeMap* pAttMap, int amplifier);
	virtual void applyAttributesModifiersToEntity(EntityLivingBase* pLiving, IAttributeMap* pAttMap, int amplifier);
	virtual float getAttributeModifierAmount(int amplifier, AttributeModifier* modifier)
	{
		return modifier->getAmount() * (amplifier + 1);
	}

};

class PotionAttackDamage : public Potion
{
public:
	PotionAttackDamage(int id, bool badEffect, int color);
	virtual float getAttributeModifierAmount(int amplifier, AttributeModifier* modifier);
};

class PotionHealth : public Potion
{
public:
	PotionHealth(int id, bool badEffect, int color);
	virtual bool isInstant() { return true; }
	virtual bool isReady(int duration, int amplifier) { return duration >= 1; }
};

class PotionHealthBoost : public Potion
{
public:
	PotionHealthBoost(int id, bool badEffect, int color);
	virtual void removeAttributesModifiersFromEntity(EntityLivingBase* pLiving, IAttributeMap* pAttMap, int amplifier);
};

class PotionAbsoption : public Potion
{
public:
	PotionAbsoption(int id, bool badEffect, int color);
	virtual void removeAttributesModifiersFromEntity(EntityLivingBase* pLiving, IAttributeMap* pAttMap, int amplifier);
	virtual void applyAttributesModifiersToEntity(EntityLivingBase* pLiving, IAttributeMap* pAttMap, int amplifier);
};

class PotionEffect : public ObjectAlloc
{
protected:
	int potionID = 0;
	/** The duration of the potion effect */
	int duration = 0;
	/** The amplifier of the potion effect */
	int amplifier = 0;
	/** Whether the potion is a splash potion */
	bool isSplashPotion = false;
	/** Whether the potion effect came from a beacon */
	bool isAmbient = false;
	/** True if potion effect duration is at maximum, false otherwise. */
	bool isPotionDurationMax = false;
	
protected:

	int deincrementDuration() { return --duration; }

public:
	PotionEffect(int potionIn);
	PotionEffect(int potionIn, int durationIn);
	PotionEffect(int potionIn, int durationIn, int amplifierIn);
	PotionEffect(int potionIn, int durationIn, int amplifierIn, bool ambientIn);
	PotionEffect(const PotionEffect& other);
	
	/** merges the input PotionEffect into this one if amplifier <= tomerge.amplifier. */
	void combine(PotionEffect* other);
	int getPotionID() { return potionID; }
	int getDuration() { return duration; }
	int getAmplifier() { return amplifier; }
	void setSplashPotion(bool bsplashPotion) { isSplashPotion = bsplashPotion; }
	bool getIsSplashPotion() { return isSplashPotion; }
	bool getIsAmbient() { return isAmbient; }
	bool onUpdate(EntityLivingBase* pLiving);
	void performEffect(EntityLivingBase* pLiving);
	const String& getEffectName() const;
	String toString();
	int hasCode() { return potionID; }
	/** Write a custom potion effect to a potion item's NBT data. */
	NBTTagCompound* writeCustomPotionEffectToNBT(NBTTagCompound* nbt);
	/** Read a custom potion effect from a potion item's NBT data.*/
	static PotionEffect* readCustomPotionEffectFromNBT(NBTTagCompound* nbt);

	void setPotionDurationMax(bool maxDuration) { isPotionDurationMax = maxDuration; }
	bool getIsPotionDurationMax() { return isPotionDurationMax; }
};

typedef map<int, String>::type PotionRequirement;

class PotionHelper : public ObjectAlloc
{
protected:
	/** Potion effect amplifier map */
	static PotionRequirement potionAmplifiers;
	static map<int, int>::type field_77925_n;
	/** An array of possible potion prefix names, as translation IDs. */
	static StringArray potionPrefixes;

public:
	static String field_77924_a;// = null;
	static String sugarEffect;
	static String ghastTearEffect;// = "+0-1-2-3&4-4+13";
	static String spiderEyeEffect;
	static String fermentedSpiderEyeEffect;
	static String speckledMelonEffect;
	static String blazePowderEffect;
	static String magmaCreamEffect;
	static String redstoneEffect;
	static String glowstoneEffect;
	static String gunpowderEffect;
	static String goldenCarrotEffect;
	static PotionRequirement potionRequirements;

protected:
	/** Returns 1 if the flag is set, 0 if it is not set. */
	static int isFlagSet(int damageType, int v) { return checkFlag(damageType, v) ? 1 : 0; }
	/** Returns 0 if the flag is set, 1 if it is not set. */
	static int isFlagUnset(int damageType, int v) { return checkFlag(damageType, v) ? 0 : 1; }
	static int func_77904_a(bool thxor, bool theunit, bool thsub, int theop, int unitv, int tensv, int damageType);
	/** Count the number of bits in an integer set to ON. */
	static int countSetFlags(int damageType);
	static int parsePotionEffects(const String& effectName, int startIndex, int endIndex, int meta);
	/** Does bit operations for brewPotionData */
	static int brewBitOperations(int data, int index, bool removed, bool toggled, bool clear);

private:
	static int parseOrExpression(std::string_view expression, int meta);
	static int parseAndExpression(std::string_view expression, int meta);
	static int parseArithmeticExpression(std::string_view expression, int meta);
	
public:
	static void initialize();

	/** Is the bit given set to 1? */
	static bool checkFlag(int par0, int par1) {	return (par0 & 1 << par1) != 0; }
	static int func_77909_a(int par0) { return func_77908_a(par0, 5, 4, 3, 2, 1); }
	/** Given a {@link Collection}<{@link PotionEffect}> will return an Integer color. */
	static int calcPotionLiquidColor(const PotionEffectArr& effects);
	static bool isAmbient(const PotionEffectArr& effects);
	static int getColorFromDamage(int par0, bool par1);
	static String func_77905_c(int par0) { int var1 = func_77909_a(par0); return potionPrefixes[var1]; }
	/** Returns a list of effects for the specified potion damage value. */
	static void getPotionEffects(int damageType, bool includeUnUsable, PotionEffectArr& outarr);
	/** Generate a data value for a potion, given its previous data value and the encoded string of new effects it will receive	*/
	static int applyIngredient(int damageType, const String& effectStr);
	static int func_77908_a(int par0, int par1, int par2, int par3, int par4, int par5);
};

}
#endif
