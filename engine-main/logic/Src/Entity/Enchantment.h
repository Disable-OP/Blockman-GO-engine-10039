/********************************************************************
filename: 	Enchantment.h
file path:	dev\client\Src\Blockman\Entity

version:	1
author:		ajohn
company:	supernano
date:		2017-2-9
*********************************************************************/
#ifndef __ENCHANTMENT_HEADER__
#define __ENCHANTMENT_HEADER__

#include "BM_TypeDef.h"
#include "BM_Container_def.h"

#include "WorldGenerator/WeightedRandomItem.h"

namespace BLOCKMAN
{

class DamageSource;
class EntityLivingBase;
class Item;
class ItemStack;
class Entity;
class Random;
class EnchantmentData;

class Enchantment : public ObjectAlloc
{
public:
	const static int MAX_ENCH_COUNT = 256;
	static Enchantment** enchantmentsList;		// = new Enchantment[256];

	/** The list of enchantments applicable by the anvil from a book */
	static Enchantment** enchantmentsBookList;

	/** Converts environmental damage to armour damage */
	static Enchantment* protection;				// = new EnchantmentProtection(0, 10, 0);
	/** Protection against fire */
	static Enchantment* fireProtection;			// = new EnchantmentProtection(1, 5, 1);
	/** Less fall damage */
	static Enchantment* featherFalling;			// = new EnchantmentProtection(2, 5, 2);
	/** Protection against explosions */
	static Enchantment* blastProtection;		// = new EnchantmentProtection(3, 2, 3);
	/** Protection against projectile entities (e.g. arrows) */
	static Enchantment* projectileProtection;	// = new EnchantmentProtection(4, 5, 4);
	/** Decreases the rate of air loss underwater; increases time between damage while suffocating */
	static Enchantment* respiration;			// = new EnchantmentOxygen(5, 2);
	/** Increases underwater mining rate */
	static Enchantment* aquaAffinity;			// = new EnchantmentWaterWorker(6, 2);
	static Enchantment* thorns;					// = new EnchantmentThorns(7, 1);
	/** Extra damage to mobs */
	static Enchantment* sharpness;				// = new EnchantmentDamage(16, 10, 0);
	/** Extra damage to zombies, zombie pigmen and skeletons */
	static Enchantment* smite;					// = new EnchantmentDamage(17, 5, 1);
	/** Extra damage to spiders, cave spiders and silverfish */
	static Enchantment* baneOfArthropods;		// = new EnchantmentDamage(18, 5, 2);
	/** Knocks mob and players backwards upon hit */
	static Enchantment* knockback;				// = new EnchantmentKnockback(19, 5);
	/** Lights mobs on fire */
	static Enchantment* fireAspect;				// = new EnchantmentFireAspect(20, 2);
	/** Mobs have a chance to drop more loot */
	static Enchantment* looting;				// = new EnchantmentLootBonus(21, 2, EnumEnchantmentType.weapon);
	/** Faster resource gathering while in use */
	static Enchantment* efficiency;				// = new EnchantmentDigging(32, 10);
	/** Blocks mined will drop themselves, even if it should drop something else (e.g. stone will drop stone, not cobblestone) */
	static Enchantment* silkTouch;				// = new EnchantmentUntouching(33, 1);
	/** Sometimes, the tool's durability will not be spent when the tool is used */
	static Enchantment* unbreaking;				// = new EnchantmentDurability(34, 5);
	/** Can multiply the drop rate of items from blocks */
	static Enchantment* fortune;				// = new EnchantmentLootBonus(35, 2, EnumEnchantmentType.digger);
	/** Power enchantment for bows, add's extra damage to arrows. */
	static Enchantment* power;					// = new EnchantmentArrowDamage(48, 10);
	/** Knockback enchantments for bows, the arrows will knockback the target when hit. */
	static Enchantment* punch;					// = new EnchantmentArrowKnockback(49, 2);
	/** Flame enchantment for bows. Arrows fired by the bow will be on fire. Any target hit will also set on fire. */
	static Enchantment* flame;					// = new EnchantmentArrowFire(50, 2);
	/** Infinity enchantment for bows. The bow will not consume arrows anymore, but will still required at least one arrow on inventory use the bow. */
	static Enchantment* infinity;				// = new EnchantmentArrowInfinite(51, 1);

	// poison
	static Enchantment* poison;

protected:
	int			weight = 0;
	/** Used in localisation and stats. */
	String		name = "";
	ui32		classType = 0;

public:
	int			effectId = 0;
	/** The EnumEnchantmentType given to this Enchantment. */
	ENCHANTMENT_TYPE type = ENCHANTMENT_TYPE_INVALID;

protected:
	Enchantment(int id, int w, ENCHANTMENT_TYPE t);
	
public:

	static void initialize();
	static void unInitialize();

	int getWeight() { return weight; }
	bool isClass(ui32 type) { return (classType & type) != 0; }

	/** Returns the minimum level that the enchantment can have. */
	virtual int getMinLevel() {	return 1; }

	/** Returns the maximum level that the enchantment can have. */
	virtual int getMaxLevel() { return 1; }

	/** Returns the minimal value of enchantability needed on the enchantment level passed. */
	virtual int getMinEnchantability(int l) { return 1 + l * 10; }

	/** Returns the maximum value of enchantability nedded on the enchantment level passed. */
	virtual int getMaxEnchantability(int l) { return getMinEnchantability(l) + 5; }

	/** Calculates de damage protection of the enchantment based on level and damage source passed. */
	virtual int calcModifierDamage(int l, DamageSource* par2DamageSource) { return 0; }

	/** Return the name of key in translation table of this enchantment. */
	virtual String getName() { return String("enchantment.") + name; }

	/** Calculates de (magic) damage done by the enchantment on a living entity based on level and entity passed. */
	virtual float calcModifierLiving(int l, EntityLivingBase* par2EntityLivingBase) { return 0.0F; }

	/** Determines if the enchantment passed can be applyied together with this enchantment. */
	virtual bool canApplyTogether(Enchantment* pEnchantment) { return this != pEnchantment; }

	/** Sets the enchantment name */
	void setName(const String& str) { name = str; }

	virtual bool canApply(ItemStackPtr pStack);

	bool canEnchantItem(Item* pItem);

	static Enchantment *getEnchantmentById(int id);
};

class EnchantmentProtection : public Enchantment
{
protected:
	static const int COUNT = 5;
	/** Holds the name to be translated of each protection type. */
	static String protectionName[COUNT];		// = new String[]{ "all", "fire", "fall", "explosion", "projectile" };
	/** Holds the base factor of enchantability needed to be able to use the enchant. */
	static int baseEnchantability[COUNT];		// = new int[] {1, 10, 5, 5, 3};
	/** Holds how much each level increased the enchantability factor to be able to use this enchant. */
	static int levelEnchantability[COUNT];		// = new int[] {11, 8, 6, 8, 6};
	/** Used on the formula of base enchantability, this is the 'window' factor of values to be able to use thing enchant. */
	static int thresholdEnchantability[COUNT];	// = new int[] {20, 12, 10, 12, 15};

public:
	/** Defines the type of protection of the enchantment, 0 = all, 1 = fire, 2 = fall (feather fall), 3 = explosion and 4 = projectile. */
	int protectionType = 0;

	EnchantmentProtection(int id, int w, int t);	

	/** implement virtual functions */
	virtual int getMinEnchantability(int l) { return baseEnchantability[protectionType] + (l - 1) * levelEnchantability[protectionType]; }
	virtual int getMaxEnchantability(int l) { return getMinEnchantability(l) + thresholdEnchantability[protectionType]; }
	virtual int getMaxLevel() {	return 4; }
	virtual int calcModifierDamage(int l, DamageSource* ds);
	virtual String getName() { return String("enchantment.protect.") + protectionName[protectionType]; }
	virtual bool canApplyTogether(Enchantment* pEnchantment);

	static int getFireTimeForEntity(Entity* pEntity, int t);
	static float getBlastDamageReduction(Entity* pEntity, float r);
};

class EnchantmentOxygen : public Enchantment
{
public:
	EnchantmentOxygen(int id, int w);

	/** implement virtual functions */
	virtual int getMinEnchantability(int l) { return 10 * l; }
	virtual int getMaxEnchantability(int l) { return getMinEnchantability(l) + 30; }
	virtual int getMaxLevel() { return 3; }
};

class EnchantmentWaterWorker : public Enchantment
{
public:
	EnchantmentWaterWorker(int id, int w);

	/** implement virtual functions */
	virtual int getMinEnchantability(int l) { return 1; }
	virtual int getMaxEnchantability(int l) { return getMinEnchantability(l) + 40; }
	virtual int getMaxLevel() { return 1; }
};

class EnchantmentThorns : public Enchantment
{
public:
	EnchantmentThorns(int id, int w);

	/** implement virtual functions */
	virtual int getMinEnchantability(int l) { return 10 + 20 * (l - 1); }
	virtual int getMaxEnchantability(int l) { return Enchantment::getMinEnchantability(l) + 50; }
	virtual int getMaxLevel() { return 3; }
	virtual bool canApply(ItemStackPtr pStack);

	static bool shouldHit(int l, Random* pRand);
	static int getDamage(int par0, Random* pRand);
	static void damageArmor(Entity* pEntity, EntityLivingBase* pLiving, Random* pRand);
};

class EnchantmentDamage : public Enchantment
{
protected:
	static const int COUNT = 3;
	/** Holds the name to be translated of each protection type. */
	static String protectionName[COUNT];			// = new String[]{ "all", "undead", "arthropods" };
	/** Holds the base factor of enchantability needed to be able to use the enchant. */
	static int baseEnchantability[COUNT];			// = new int[] {1, 5, 5};
	/** Holds how much each level increased the enchantability factor to be able to use this enchant. */
	static int levelEnchantability[COUNT];			// = new int[] {11, 8, 8};
	/** Used on the formula of base enchantability, this is the 'window' factor of values to be able to use thing enchant. */
	static int thresholdEnchantability[COUNT];		// = new int[] {20, 20, 20};

public:
	/** Defines the type of damage of the enchantment, 0 = all, 1 = undead, 3 = arthropods */
	int damageType = 0;

public:
	EnchantmentDamage(int id, int w, int t);

	/** implement virtual functions */
	virtual int getMinEnchantability(int l) { return baseEnchantability[damageType] + (l - 1) * levelEnchantability[damageType]; }
	virtual int getMaxEnchantability(int l) { return getMinEnchantability(l) + thresholdEnchantability[damageType]; }
	virtual int getMaxLevel() { return 5; }
	virtual float calcModifierLiving(int l, EntityLivingBase* pLiving);
	virtual String getName() { return String("enchantment.damage.") + protectionName[damageType]; }
	virtual bool canApplyTogether(Enchantment* pEnchantment) { return !(pEnchantment->isClass(ENCHANTMENT_CLASS_DAMAGE)); }
	virtual bool canApply(ItemStackPtr pStack);
};

class EnchantmentKnockback : public Enchantment
{
public:
	EnchantmentKnockback(int id, int w);

	/** implement virtual functions */
	virtual int getMinEnchantability(int l) { return 5 + 20 * (l - 1); }
	virtual int getMaxEnchantability(int l) { return Enchantment::getMinEnchantability(l) + 50; }
	virtual int getMaxLevel() {	return 2; }
};

class EnchantmentFireAspect : public Enchantment
{
public:
	EnchantmentFireAspect(int id, int w);

	/** implement virtual functions */
	virtual int getMinEnchantability(int par1) { return 10 + 20 * (par1 - 1); }
	virtual int getMaxEnchantability(int par1) { return Enchantment::getMinEnchantability(par1) + 50; }
	virtual int getMaxLevel() { return 2; }
};

class EnchantmentPoison : public Enchantment
{
public:
	EnchantmentPoison(int id, int w);

	/** implement virtual functions */
	virtual int getMinEnchantability(int par1) { return 10 + 20 * (par1 - 1); }
	virtual int getMaxEnchantability(int par1) { return Enchantment::getMinEnchantability(par1) + 50; }
	virtual int getMaxLevel() { return 2; }
};

class EnchantmentLootBonus : public Enchantment
{
public:
	EnchantmentLootBonus(int id, int w, ENCHANTMENT_TYPE t);

	/** implement virtual functions */
	virtual int getMinEnchantability(int l) { return 15 + (l - 1) * 9; }
	virtual int getMaxEnchantability(int l) { return Enchantment::getMinEnchantability(l) + 50; }
	virtual int getMaxLevel() { return 3; }
	virtual bool canApplyTogether(Enchantment* pEnchantment);
};

class EnchantmentDigging : public Enchantment
{
public:
	EnchantmentDigging(int id, int w);

	/** implement virtual functions */
	virtual int getMinEnchantability(int l) { return 1 + 10 * (l - 1); }
	virtual int getMaxEnchantability(int l) { return Enchantment::getMinEnchantability(l) + 50; }
	virtual int getMaxLevel() { return 5; }
	virtual bool canApply(ItemStackPtr pStack);
};

class EnchantmentUntouching : public Enchantment
{
public:
	EnchantmentUntouching(int id, int w);

	/** implement virtual functions */
	virtual int getMinEnchantability(int l) { return 15; }
	virtual int getMaxEnchantability(int l) { return Enchantment::getMinEnchantability(l) + 50; }
	virtual int getMaxLevel() { return 1; }
	virtual bool canApplyTogether(Enchantment* pEnchantment);
	virtual bool canApply(ItemStackPtr pStack);
};

class EnchantmentDurability : public Enchantment
{
public:
	EnchantmentDurability(int id, int w);

	/** implement virtual functions */
	virtual int getMinEnchantability(int l) { return 5 + (l - 1) * 8; }
	virtual int getMaxEnchantability(int l) { return Enchantment::getMinEnchantability(l) + 50; }
	virtual int getMaxLevel() {	return 3; }
	virtual bool canApply(ItemStackPtr pStack);
	
	static bool negateDamage(ItemStackPtr pStack, int l, Random* pRandom);
};

class EnchantmentArrowDamage : public Enchantment
{
public:
	EnchantmentArrowDamage(int id, int w);
	
	/** implement virtual functions */
	virtual int getMinEnchantability(int l) { return 1 + (l - 1) * 10; }
	virtual int getMaxEnchantability(int l) { return getMinEnchantability(l) + 15; }
	virtual int getMaxLevel() {	return 5; }
};

class EnchantmentArrowKnockback : public Enchantment
{
public:
	EnchantmentArrowKnockback(int id, int w);

	/** implement virtual functions */
	virtual int getMinEnchantability(int l) { return 12 + (l - 1) * 20; }
	virtual int getMaxEnchantability(int l) { return getMinEnchantability(l) + 25; }
	virtual int getMaxLevel() {	return 2; }
};

class EnchantmentArrowFire : public Enchantment
{
public:
	EnchantmentArrowFire(int id, int w);

	/** implement virtual functions */
	virtual int getMinEnchantability(int l) { return 20; }
	virtual int getMaxEnchantability(int l) { return 50; }
	virtual int getMaxLevel() { return 1; }
};

class EnchantmentArrowInfinite : public Enchantment
{
public:
	EnchantmentArrowInfinite(int id, int w);

	/** implement virtual functions */
	virtual int getMinEnchantability(int l) { return 20; }
	virtual int getMaxEnchantability(int l) { return 50; }
	virtual int getMaxLevel() {	return 1; }
};

class IEnchantmentModifier : public ObjectAlloc
{
public:
	virtual void calculateModifier(Enchantment* pEnchantment, int level) = 0;
};

class EnchantmentModifierDamage : public IEnchantmentModifier
{
public:
	/** Used to calculate the damage modifier (extra armor) on enchantments that the player have on equipped armors. */
	int damageModifier;
	/** Used as parameter to calculate the damage modifier (extra armor) on enchantments that the player have on equipped armors. */
	DamageSource* source;

	EnchantmentModifierDamage() :damageModifier(0), source(NULL) { }
	virtual void calculateModifier(Enchantment* pEnchantment, int level);
};

class EnchantmentModifierLiving : public IEnchantmentModifier
{
public:
	/** Used to calculate the (magic) extra damage based on enchantments of current equipped player item. */
	float livingModifier;
	/** Used as parameter to calculate the (magic) extra damage based on enchantments of current equipped player item. */
	EntityLivingBase* entityLiving;

	EnchantmentModifierLiving() : livingModifier(0.f), entityLiving(NULL) {}
	virtual void calculateModifier(Enchantment* pEnchantment, int level);
};

class EnchantmentHelper
{
protected:
	/** Is the random seed of enchantment effects. */
	static Random* enchantmentRand; // = new Random();
	/** Used to calculate the extra armor of enchantments on armors equipped on player. */
	static EnchantmentModifierDamage* enchantmentModifierDamage; 
	/** Used to calculate the (magic) extra damage done by enchantments on current equipped item of player. */
	static EnchantmentModifierLiving* enchantmentModifierLiving; 
				
	/** Executes the enchantment modifier on the array of ItemStack passed. */
	static void applyEnchantmentModifierArray(IEnchantmentModifier* pModifier, const ItemStackArr& stacks);

public:
	typedef map<int, int>::type EnchantMentMap;
	typedef vector<EnchantmentData*>::type EnchantmentDataArr;
	typedef map<int, EnchantmentData*>::type EnchantmentDataMap;

	static void initialize();
	static void unInitalize();

	/** Returns the level of enchantment on the ItemStack passed. */
	static int getEnchantmentLevel(int enchID, ItemStackPtr pStack);
	/** Return the enchantments for the specified stack. */
	static void getEnchantments(ItemStackPtr pStack, EnchantMentMap& outmap);
	/** Set the enchantments for the specified stack. */
	static void setEnchantments(const EnchantMentMap& inmap, ItemStackPtr pStack);
	/** Returns the biggest level of the enchantment on the array of ItemStack passed. */
	static int getMaxEnchantmentLevel(int enchID, const ItemStackArr& stacks);
	/** Executes the enchantment modifier on the ItemStack passed. */
	static void applyEnchantmentModifier(IEnchantmentModifier* pModifier, ItemStackPtr pStack);
	/** Returns the modifier of protection enchantments on armors equipped on player. */
	static int getEnchantmentModifierDamage(const ItemStackArr& stacks, DamageSource* pSource);
	/** Return the (magic) extra damage of the enchantments on player equipped item. */
	static float getEnchantmentModifierLiving(EntityLivingBase* pLiving1, EntityLivingBase* pLiving2);
	/** Returns the knockback value of enchantments on equipped player item. */
	static int getKnockbackModifier(EntityLivingBase* pLiving1, EntityLivingBase* pLiving2);
	static int getFireAspectModifier(EntityLivingBase* pLiving);
	// return poison value
	static int getPoisonModifier(EntityLivingBase* pLiving);
	/** Returns the 'Water Breathing' modifier of enchantments on player equipped armors. */
	static int getRespiration(EntityLivingBase* pLiving);
	/** Return the extra efficiency of tools based on enchantments on equipped player item. */
	static int getEfficiencyModifier(EntityLivingBase* pLiving);
	/** Returns the silk touch status of enchantments on current equipped item of player. */
	static bool getSilkTouchModifier(EntityLivingBase* pLiving);
	/** Returns the fortune enchantment modifier of the current equipped item of player. */
	static int getFortuneModifier(EntityLivingBase* pLiving);
	/** Returns the looting enchantment modifier of the current equipped item of player. */
	static int getLootingModifier(EntityLivingBase* pLiving);
	/** Returns the aqua affinity status of enchantments on current equipped item of player. */
	static bool getAquaAffinityModifier(EntityLivingBase* pLiving);
	static int getThornsModifier(EntityLivingBase* pLiving);
	static ItemStackPtr getEnchantedItem(Enchantment* pEnch, EntityLivingBase* pLiving);
	/** Returns the enchantability of itemstack, it's uses a singular formula for each index (2nd parameter: 0, 1 and 2),
		cutting to the max enchantability power of the table (3rd parameter) */
	static int calcItemStackEnchantability(Random* pRand, int enchNum, int power, ItemStackPtr pStack);
	/** Adds a random enchantment to the specified item. Args: random, itemStack, enchantabilityLevel */
	static ItemStackPtr addRandomEnchantment(Random* pRand, ItemStackPtr pStack, int allowTreasure);
	/** Create a list of random EnchantmentData (enchantments) that can be added together to the ItemStack, the 3rd
		parameter is the total enchantability level. */
	static void buildEnchantmentList(Random* pRand, ItemStackPtr pStack, int allowTreasure, EnchantmentDataArr& outArr);
	/** Creates a 'Map' of EnchantmentData (enchantments) possible to add on the ItemStack and the enchantability level passed. */
	static void mapEnchantmentData(int enchAbility, ItemStackPtr pStack, WRItemList& outarr);
};

}

#endif