/********************************************************************
filename: 	Items.h
file path:	dev\client\Src\Blockman\Item

version:	1
author:		ajohn
company:	supernano
date:		2017-2-20
*********************************************************************/
#ifndef __ITEMS_HEADER__
#define __ITEMS_HEADER__

#include "Item.h"
#include "BM_Container_def.h"

#include "Core.h"

namespace BLOCKMAN
{

class EntityHanging;
class IBehaviorDispenseItem;
class BM_Material;
class Enchantment;
class EnchantmentData;

class ToolMaterial : public ObjectAlloc
{
protected:
	/** The level of material this tool can harvest */
	int harvestLevel = 0;
	/** The number of uses this material allows. */
	int maxUses = 0;
	/**  The strength of this tool material against blocks which it is effective against. */
	float efficiencyOnProperMaterial = 0.f;
	/** Damage versus entities. */
	float damageVsEntity = 0.f;
	/** Defines the natural enchantability factor of the material. */
	int enchantability = 0;

	ToolMaterial(int level, int maxuses, float effectiency, float damage, int enchantabilityIn);

public:
	static ToolMaterial* WOOD;
	static ToolMaterial* STONE;
	static ToolMaterial* IRON;
	static ToolMaterial* EMERALD;
	static ToolMaterial* GOLD;

	static void initialize();
	static void unInitialize();

	/** The number of uses this material allows. */
	int getMaxUses() { return maxUses; }
	/** The strength of this tool material against blocks which it is effective against. */
	float getEfficiencyOnProperMaterial() { return efficiencyOnProperMaterial; }
	/** Damage versus entities. */
	float getDamageVsEntity() { return damageVsEntity; }
	/** The level of material this tool can harvest */
	int getHarvestLevel() { return harvestLevel; }
	/** Return the natural enchantability factor of the material. */
	int getEnchantability() { return enchantability; }
	/** Return the crafting material for this tool material, used to determine the item that can be used to repair a tool with an anvil */
	int getToolCraftingMaterial();

	String toString();
};

class ArmorMaterial : public ObjectAlloc
{
protected:
	const static int ARMOR_SIZE = 4;
	/**  Holds the maximum damage factor (each piece multiply this by it's own value) of the material, this is the item damage */
	int maxDamageFactor = 0;
	/** Holds the damage reduction (each 1 points is half a shield on gui) of each piece of armor */
	int damageReductionAmountArray[ARMOR_SIZE];
	/** Return the enchantability factor of the material */
	int enchantability = 0;

	ArmorMaterial(int factor, int* amountArray, int enchability);

public:
	static ArmorMaterial* CLOTH;	// (5, new int[] {1, 3, 2, 1}, 15),
	static ArmorMaterial* CHAIN;	// (15, new int[] {2, 5, 4, 1}, 12),
	static ArmorMaterial* IRON;	// (15, new int[] {2, 6, 5, 2}, 9),
	static ArmorMaterial* GOLD;	// (7, new int[] {2, 5, 3, 1}, 25),
	static ArmorMaterial* DIAMOND;	// (33, new int[] {3, 8, 6, 3}, 10);
	
	static void initialize();
	static void unInitialize();

    /** Returns the durability for a armor slot of for this type. */
	int getDurability(int par1);
    /** Return the damage reduction (each 1 point is a half a shield on gui) of the piece index passed */
    int getDamageReductionAmount(int armorIdx) { return damageReductionAmountArray[armorIdx]; }
    /** Return the enchantability factor of the material. */
    int getEnchantability() { return enchantability; }
    /** Return the crafting material for this armor material, used to determine the item that can be used to repair an armor piece with an anvil */
	int getArmorCraftingMaterial();
};


class ItemTool : public Item
{
protected:
	/** Array of blocks the tool has extra effect against. */
	BlockArr blocksEffectiveAgainst;
	int blocksEffectiveAgainstCount = 0;
	float efficiencyOnProperMaterial = 0.f;
	/** Damage versus entities. */
	float damageVsEntity = 0.f;
	/** The material this tool is made from. */
	ToolMaterial* toolMaterial = nullptr;

	float effectiveDistance = 0.f;

protected:
	ItemTool(int id, const String& name, float attackSpeed, ToolMaterial* toolMat);
	ItemTool(int id, const String& name, float attackSpeed, ToolMaterial* toolMat, float effectiveDistanceIn);

public:
	/** implement override functions */
	virtual float getStrVsBlock(ItemStackPtr pStack, Block* pBlock);
	virtual bool hitEntity(ItemStackPtr pStack, EntityLivingBase* pLiving1, EntityLivingBase* pLiving2);
	virtual bool onBlockDestroyed(ItemStackPtr pStack, World* pWorld, int blockID, const BlockPos& pos, EntityLivingBase* pLiving);
	virtual int getItemEnchantability() { return toolMaterial->getEnchantability(); }
	virtual bool getIsRepairable(ItemStackPtr pStack1, ItemStackPtr pStack2);
	virtual void getAttributeModifiers(AttributeModifierMap& outmap);
	virtual float getEffectiveDistance() { return effectiveDistance; }

	/** Return the name for this tool's material. */
	String getToolMaterialName() { return toolMaterial->toString(); }
	void setEffectiveBlocks(const BlockArr& blocks) { blocksEffectiveAgainst = blocks; }
	void setEffectiveDistance(float effectiveDistanceIn) { effectiveDistance = effectiveDistanceIn; }
	void setEfficiency(float efficiency) { efficiencyOnProperMaterial = efficiency; }

};

class ItemSpade : public ItemTool
{
public:
	ItemSpade(int id, const String& name, ToolMaterial* material);
	/** implement override functions */
	virtual bool canHarvestBlock(Block* pBlock);
};

class ItemPickaxe : public ItemTool
{
public:
	ItemPickaxe(int id, const String& name, ToolMaterial* toolMat);
	/** implement override functions */
	virtual bool canHarvestBlock(Block* pBlock);
	virtual float getStrVsBlock(ItemStackPtr pStack, Block* pBlock);
};

class ItemAxe : public ItemTool
{
public:
	ItemAxe(int id, const String& name, ToolMaterial* toolMat);
	/** implement override functions */
	virtual float getStrVsBlock(ItemStackPtr pStack, Block* pBlock);
};

class ItemFlintAndSteel : public Item
{
public:
	ItemFlintAndSteel(int id, const String& name);
	/** implement override functions */
	virtual bool onItemUse(ItemStackPtr pStack, EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos, int face, const Vector3& hit);
};

class ItemFood : public Item
{
protected:
	/** The amount this food item heals the player. */
	int healAmount = 0;
	float saturationModifier = 0.f;
	/** Whether wolves like this food (true for raw and cooked porkchop). */
	bool isWolfsFavoriteMeat = false;
	/** If this field is true, the food can be consumed even if the player don't need to eat.*/
	bool alwaysEdible = false;
	/** represents the potion effect that will occurr upon eating this food. Set by setPotionEffect */
	int potionId = 0;
	/** set by setPotionEffect */
	int potionDuration = 0;
	/** set by setPotionEffect */
	int potionAmplifier = 0;
	/** probably of the set potion effect occurring */
	float potionEffectProbability = false;

public:
	/** Number of ticks to run while 'EnumAction'ing until result. */
	int itemUseDuration = 0;

protected:
	virtual void onFoodEaten(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer);

public:
	ItemFood(int id, const String& name, int amount, float modifier, bool isfavorite);
	ItemFood(int id, const String& name, int amount, bool isfavorite);
	
	/** implement override functions */
	virtual ItemStackPtr onEaten(ItemStackPtr pItemStack, World* pWorld, EntityPlayer* pPlayer);
	virtual int getMaxItemUseDuration(ItemStackPtr pStack) { return 32; }
	virtual ACTION_TYPE getItemUseAction(ItemStackPtr pStack) { return ACTION_TYPE_EAT; }
	virtual ItemStackPtr onItemRightClick(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer);
	virtual bool canLongTouchUse() { return true; }
	
	int getHealAmount() { return healAmount; }
	float getSaturationModifier() { return saturationModifier; }
	bool IsWolfsFavoriteMeat() { return isWolfsFavoriteMeat; }
	void setPotionEffect(int id, int duration, int amplifier, float probability);
	void setAlwaysEdible() { alwaysEdible = true; }
};

class ItemSoup : public ItemFood
{
public:
	ItemSoup(int id, const String& name, int amount);

	/** implement override functions */
	virtual ItemStackPtr onEaten(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer);
};

class ItemAppleGold : public ItemFood
{
protected:
	virtual void onFoodEaten(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer);
	
public:
	ItemAppleGold(int id, const String& name, int amount, float modifier, bool isfavorite);

	/** implement override functions */
	virtual bool hasEffect(ItemStackPtr pStack);
	virtual RARITY_TYPE getRarity(ItemStackPtr pStack);
};

class ItemSeedFood : public ItemFood
{
protected:
	/** Block ID of the crop this seed food should place. */
	int cropId = 0;
	/** Block ID of the soil this seed food should be planted on. */
	int soilId = 0;

public:
	ItemSeedFood(int id, const String& name, int amount, float modifier, int crop, int soil);

	/** implement override functions */
	virtual bool onItemUse(ItemStackPtr pStack, EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos, int face, const Vector3& hit);
};

class ItemBow : public Item
{
public:
	ItemBow(int id, const String& name);

	/** implement override functions from Item*/
	virtual void onPlayerStoppedUsing(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer, int timeleft);
	virtual ItemStackPtr onEaten(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer) { return pStack; }
	virtual int getMaxItemUseDuration(ItemStackPtr pStack) { return 72000; }
	virtual ACTION_TYPE getItemUseAction(ItemStackPtr pStack) { return ACTION_TYPE_BOW; }
	virtual ItemStackPtr onItemRightClick(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer);
	virtual int getItemEnchantability() { return 1; }
	virtual bool canLongTouchUse() { return true; }
};

enum BulletType {
	VIRTUAL,
	ENTITY
};

class GunSetting;
class BulletClipSetting;

class ItemGun : public Item 
{
public:
	ItemGun(int id, int gunId, const String& name);
	const GunSetting* getGunSetting();
	virtual void onPlayerStoppedUsing(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer, int timeleft);
	virtual ItemStackPtr onItemRightClick(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer);
	virtual ACTION_TYPE getItemUseAction(ItemStackPtr pStack) { return ACTION_TYPE_FIRE; }
	virtual int getMaxItemUseDuration(ItemStackPtr pStack) { return 100; }

public:
	virtual void fire(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer);
	void fireShotGun(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer);
	bool sendGunFire(Entity* tracyEntity, RayTraceResult trace, RAYTRACE_TYPE sendTypeOfHit, int sendTraceType, Vector3 beginPos, Vector3 dir, bool isNeedConsumeBullet, bool isHeadshot);
	Vector3 calcDirPos(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer);
	bool traceWorldEntity(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer, RayTraceResult trace, float rangeRatio, float range, bool isNeedConsumeBullet);
	bool traceWorldVehicle(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer, RayTraceResult trace, float rangeRatio, float range, bool isNeedConsumeBullet);
	bool traceWorldActorNpc(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer, RayTraceResult trace, float rangeRatio, float range, bool isNeedConsumeBullet);
	bool LastFire(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer, bool isNeedConsumeBullet, bool isNeedNoHitSend);
	bool getTarget(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer, Vector3& endPos);

public:
	int				gunId;
};

class ItemBulletClip : public Item {
public:
	ItemBulletClip(int id, int bulletClipId, const String& name);
	const BulletClipSetting* getBulletClipSetting();
	virtual bool onItemUse(ItemStackPtr pStack, EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos, int face, const Vector3& hit);
	virtual ItemStackPtr onItemRightClick(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer);

public:
	int				bulletId;
};

class ItemHandcuffs : public Item {
public:
	ItemHandcuffs(int id, const String& name);
};

class ItemKey : public Item 
{
public:
	ItemKey(int id, const String& name);
};

class ItemTeleportScroll : public Item
{
public:
	ItemTeleportScroll(int id, const String& name);

	/** implement override functions */
	virtual ACTION_TYPE getItemUseAction(ItemStackPtr pStack) { return ACTION_TYPE_USE_COMMON_ITEM; }
	virtual ItemStackPtr onItemRightClick(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer);
	virtual bool canLongTouchUse() { return true; }
	virtual bool canContinuousUse() { return false; }
};

class ItemCreateBridge : public Item
{
public:
	ItemCreateBridge(int id, const String& name);

	/** implement override functions */
	virtual ACTION_TYPE getItemUseAction(ItemStackPtr pStack) { return ACTION_TYPE_USE_COMMON_ITEM; }
	virtual ItemStackPtr onItemRightClick(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer);
	virtual bool canLongTouchUse() { return true; }
	virtual bool canContinuousUse() { return false; }
};

class ItemArrowPotion : public Item
{
public:
	ItemArrowPotion(int id, const String& name);
	int getPotionId() { return m_potionId; }
	int getPotionDuration() { return m_potionDuration; }
	int getPotionAmplifier() { return m_potionAmplifier; }
	void setPotionParams(int id, int duration, int amplifier) {
		m_potionId = id;
		m_potionDuration = duration;
		m_potionAmplifier = amplifier;
	}

private:
	int m_potionId = 0;
	int m_potionDuration = 0;
	int m_potionAmplifier = 0;
};

class ItemCoal : public Item
{
public:
	ItemCoal(int id, const String& name);

	/** implement override functions */
	virtual String getUnlocalizedName(ItemStackPtr pStack);
};

class ItemSword : public Item
{
protected:
	float weaponDamage = 0.f;
	ToolMaterial* toolMaterial = nullptr;

public:
	ItemSword(int id, const String& name, ToolMaterial* toolMat);

	float getDamageVsEntity() { return toolMaterial->getDamageVsEntity(); }
	String getToolMaterialName() { return toolMaterial->toString(); }

	/** implement override functions */
	virtual float getStrVsBlock(ItemStackPtr pStack, Block* pBlock);
	virtual bool hitEntity(ItemStackPtr pStack, EntityLivingBase* pLiving1, EntityLivingBase* pLiving2);
	virtual bool onBlockDestroyed(ItemStackPtr pStack, World* pWorld, int blockID, const BlockPos& pos, EntityLivingBase* pLiving);
	virtual ACTION_TYPE getItemUseAction(ItemStackPtr pStack) { return ACTION_TYPE_BLOCK; }
	virtual int getMaxItemUseDuration(ItemStackPtr pStack) { return 72000; }
	virtual ItemStackPtr onItemRightClick(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer);
	virtual bool canHarvestBlock(Block* pBlock);
	virtual int getItemEnchantability() { return toolMaterial->getEnchantability(); }
	virtual bool getIsRepairable(ItemStackPtr pStack1, ItemStackPtr pStack2);
	virtual void getAttributeModifiers(AttributeModifierMap& outmap);
};

class ItemReed : public Item
{
protected:
	/** The ID of the block the reed will spawn when used from inventory bar. */
	int spawnID = 0;

public:
	ItemReed(int id, const String& name, int reedid);

	/** implement override functions */
	virtual bool onItemUse(ItemStackPtr pStack, EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos, int face, const Vector3& hit);
};

class ItemHoe : public Item
{
protected:
	ToolMaterial* theToolMaterial = nullptr;

public:
	ItemHoe(int id, const String& name, ToolMaterial* toolMat);
	
	/** implement override functions */
	virtual bool onItemUse(ItemStackPtr pStack, EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos, int face, const Vector3& hit);
	
	/** Returns the name of the material this tool is made from as it is declared in EnumToolMaterial. */
	String getMaterialName() { return theToolMaterial->toString(); }
};

class ItemSeeds : public Item
{
protected:
	/** The type of block this seed turns into (wheat or pumpkin stems for instance) */
	int blockType = 0;
	/** BlockID of the block the seeds can be planted on. */
	int soilBlockID = 0;

public:
	ItemSeeds(int id, const String& name, int blocktype, int soilID);
	
	/** implement override functions */
	virtual bool onItemUse(ItemStackPtr pStack, EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos, int face, const Vector3& hit);
};

class RanchSeeds : public ItemSeeds
{
public:
	RanchSeeds(int id, const String& name, int blocktype, int soilID);
};

class ItemHangingEntity : public Item
{
protected:
	int hangingClassID = 0; // 0 NULL  1 EntityPainting  2 EntityItemFrame 

	EntityHanging* createHangingEntity(World* pWorld, const BlockPos& pos, int dir);
public:
	ItemHangingEntity(int id, const String& name, int classID);
	
	/** implement override functions */
	virtual bool onItemUse(ItemStackPtr pStack, EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos, int face, const Vector3& hit);
};

class ItemSign : public Item
{
public:
	ItemSign(int id, const String& name);

	/** implement override functions */
	virtual bool onItemUse(ItemStackPtr pStack, EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos, int face, const Vector3& hit);
};

class ItemDoor : public Item
{
protected:
	const BM_Material& doorMaterial;

public:
	ItemDoor(int id, const String& name, const BM_Material& mat);
	
	/** implement override functions */
	virtual bool onItemUse(ItemStackPtr pStack, EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos, int face, const Vector3& hit);

	static void placeDoorBlock(World* pWorld, const BlockPos& pos, int dir, Block* pBlock);

	vector<Vector3i>::type getPlacePositions(World* pWorld, BlockPos& pos, int face);
};

class ItemEnderPearl : public Item
{
public:
	ItemEnderPearl(int id, const String& name);

	/** implement override functions */
	virtual ItemStackPtr onItemRightClick(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer);
};

class ItemGlassBottle : public Item
{
public:
	ItemGlassBottle(int id, const String& name);

	/** implement override functions */
	virtual ItemStackPtr onItemRightClick(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer);
};

class ItemEnderEye : public Item
{
public:
	ItemEnderEye(int id, const String& name);
	
	/** implement override functions */
	virtual bool onItemUse(ItemStackPtr pStack, EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos, int face, const Vector3& hit);
	virtual ItemStackPtr onItemRightClick(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer);
};

class ItemMonsterPlacer : public Item
{
public:
	ItemMonsterPlacer(int id, const String& name);

	/** implement override functions */
	virtual String getItemDisplayName(ItemStackPtr pStack);
	virtual bool requiresMultipleRenderPasses() { return true; }
	virtual bool onItemUse(ItemStackPtr pStack, EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos, int face, const Vector3& hit);
	virtual ItemStackPtr onItemRightClick(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer);
	
	static Entity* spawnCreature(World* pWorld, int entityID, const Vector3& pos);
};

class ItemArmor : public Item
{
protected:
	/** Holds the 'base' maxDamage that each armorType have. */
	const static int MAX_ARMOR_COUNT = 4;
	static int maxDamageArray[MAX_ARMOR_COUNT];		// = new int[] {11, 16, 15, 13};
	static IBehaviorDispenseItem* behaviorDispenseArmor;	// = new BehaviorDispenseArmor();
	
	ArmorMaterial* material = nullptr;

public:
	/** Stores the armor type: 0 is helmet, 1 is plate, 2 is legs and 3 is boots */
	int armorType = 0;
	/** Holds the amount of damage that the armor reduces at full durability. */
	int damageReduceAmount = 0;
	
	ItemArmor(int id, const String& name, ArmorMaterial* armorMat, int armortype);
	
	ArmorMaterial* getArmorMaterial() { return material; }
	/** Return whether the specified armor ItemStack has a color. */
	bool hasColor(ItemStackPtr pStack);
	/** Return the color for the specified armor ItemStack. */
	int getColor(ItemStackPtr pStack);
	/** Remove the color from the specified armor ItemStack. */
	void removeColor(ItemStackPtr pStack);
	void setColor(ItemStackPtr pStack, int color);
	static int* getMaxDamageArray() { return maxDamageArray; }
	static bool isArmor(int itemId)
	{
		return isArmor(itemsList[itemId]);
	}
	static bool isArmor(Item* item)
	{
		return dynamic_cast<ItemArmor*>(item) != NULL;
	}

	/** implement override functions */
	virtual bool requiresMultipleRenderPasses() { return material == ArmorMaterial::CLOTH; }
	virtual int getItemEnchantability() { return material->getEnchantability(); }
	virtual bool getIsRepairable(ItemStackPtr pStack1, ItemStackPtr pStack2);
	virtual ItemStackPtr onItemRightClick(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer);
};

class ItemPotion : public Item
{
public:
	typedef map<int, PotionEffectArr>::type EffectCacheMap;
	typedef map<int, PotionEffect*>::type EffectMap;

protected:
	/** maps potion damage values to lists of effect names */
	EffectCacheMap effectCache;

	/** effects that potion item has */
	EffectMap m_ownEffects;
	static EffectCacheMap field_77835_b;
	bool m_isSplash = false;
public:
	ItemPotion(int id, const String& name, bool isSplash = false);

	/** Returns a list of potion effects for the specified itemstack. */
	void getEffects(ItemStackPtr pStack, PotionEffectArr& outArr);
	/** Returns a list of effects for the specified potion damage value. */
	void getEffects(int damageType, PotionEffectArr& outArr);
	void getEffects(PotionEffectArr& outArr);
	/** returns wether or not a potion is a throwable splash potion based on damage value */
	int getColorFromDamage(int damage);
	bool isEffectInstant(int damage);
	static bool isSplash(int damage) { return (damage & 0x4000) != 0; }
	bool isSplash() { return m_isSplash; }
	/** implement override functions */
	virtual ItemStackPtr onEaten(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer);
	virtual int getMaxItemUseDuration(ItemStackPtr pStack) { return 32; }
	virtual ACTION_TYPE getItemUseAction(ItemStackPtr pStack) { return ACTION_TYPE_DRINK; }
	virtual ItemStackPtr onItemRightClick(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer);
	virtual bool onItemUse(ItemStackPtr pStack, EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos, int face, const Vector3& hit) { return false; }
	virtual bool requiresMultipleRenderPasses() { return true; }
	virtual String getItemDisplayName(ItemStackPtr pStack);
	virtual bool hasEffect(ItemStackPtr pStack);
	virtual void addEffect(PotionEffect* effect);
	virtual bool canLongTouchUse() { return true; }
};

class ItemEnchantedBook : public Item
{
public:
	ItemEnchantedBook(int id, const String& name);
	
	NBTTagList* getStoredEnchantments(ItemStackPtr pStack);
	/** Returns the ItemStack of an enchanted version of this item. */
	void getEnchantedItemStack_do(ItemStackPtr pStack, EnchantmentData* pEnchData);
	/** Returns the ItemStack of an enchanted version of this item. */
	ItemStackPtr getEnchantedItemStack(EnchantmentData* pEnchData);
	void func_92113_a(Enchantment* pEnch, ItemStackArr& outarr);
	WeightedRandomChestContent* func_92114_b(Random* par1Random);
	WeightedRandomChestContent* func_92112_a(Random* pRand, int par2, int par3, int par4);
	
	/** implement override functions */
	virtual bool hasEffect(ItemStackPtr pStack) { return true; }
	virtual bool isItemTool(ItemStackPtr pStack) { return false; }
	virtual RARITY_TYPE getRarity(ItemStackPtr pStack);
};

class ItemDye : public Item
{
protected:
	static const int MAX_DYE_NUM = 16;

public:
	/** List of dye color names */
	static String dyeColorNames[MAX_DYE_NUM];
	static int dyeColors[MAX_DYE_NUM];		
	
public:
	ItemDye(int id, const String& name);
	
	/** implement override functions */
	virtual String getUnlocalizedName(ItemStackPtr pStack);
	virtual bool onItemUse(ItemStackPtr pStack, EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos, int face, const Vector3& hit);
	virtual bool interactWithEntity(ItemStack& pStack, EntityPlayer& pPlayer, EntityLivingBase* pLiving);

	static bool applyBonemeal(ItemStackPtr pStack, World* pWorld, const BlockPos& pos);
	static void spawnBonemealParticles(World* pWorld, const BlockPos& pos, int amount);
	
};

class ItemMapBase : public Item
{
public:
	ItemMapBase(int id, const String& name);
	
	/** implement override functions from Item. */
	virtual bool isMap() { return true; }

	/*** returns null if no update is to be sent */
	/* virtual Packet createMapDataPacket(ItemStack par1ItemStack, World par2World, EntityPlayer par3EntityPlayer) { return null; }*/
};

class ItemEmptyMap : public ItemMapBase
{
public:
	ItemEmptyMap(int id, const String& name);

	/** implement override functions from Item. */
	// ItemStackPtr onItemRightClick(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer);
};

class ItemMap : public ItemMapBase
{
public:
	ItemMap(int id, const String& name);

	//static MapData getMPMapData(short par0, World par1World);
	//MapData getMapData(ItemStack par1ItemStack, World par2World);
	//void updateMapData(World par1World, Entity par2Entity, MapData par3MapData);

	///** implement override functions from Item. */
	//virtual void onUpdate(ItemStackPtr pStack, World* pWorld, Entity* pEntity, int slot, bool isSelected);
	//virtual void onCreated(ItemStack par1ItemStack, World par2World, EntityPlayer par3EntityPlayer);

	///** implement override functions from ItemMapBase. */
	//virtual Packet createMapDataPacket(ItemStack par1ItemStack, World par2World, EntityPlayer par3EntityPlayer);
};

class ItemBlock : public Item
{
protected:
	/** The block ID of the Block associated with this ItemBlock */
	int blockID = 0;

public:
	ItemBlock(int id, const String& name = StringUtil::BLANK);

	/** Returns the blockID for this Item */
	int getBlockID() { return blockID; }
	/** Returns true if the given ItemBlock can be placed on the given side of the given block position. */
	virtual bool canPlaceItemBlockOnSide(World* pWorld, const BlockPos& pos, int face, EntityPlayer* pPlayer, ItemStackPtr pStack);
	void caculatePlacePos(World* pWorld, BlockPos& pos, int face);
	/** implement override functions from Item. */
	virtual bool onItemUse(ItemStackPtr pStack, EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos, int face, const Vector3& hit);
	virtual String getUnlocalizedName(ItemStackPtr pStack);
	virtual String getUnlocalizedName();
};

class ItemShears : public Item
{
public:
	ItemShears(int id, const String& name);

	/** implement override functions from Item. */
	virtual bool onBlockDestroyed(ItemStackPtr pStack, World* porld, int blockID, const BlockPos& pos, EntityLivingBase* pLiving);
	virtual bool canHarvestBlock(Block* pBlock);
	virtual float getStrVsBlock(ItemStackPtr pStack, Block* pBlock);
};

class ItemBucket : public Item
{
protected:
	/** field for checking if the bucket has been filled. */
	int isFull = 0;

public:
	ItemBucket(int id, const String& name, int fillID);

	ItemStackPtr doIt(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer, const Vector3i& posIn, int sideIndex);

	/** implement override functions from Item. */
	virtual ItemStackPtr onItemRightClick(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer);

	/** Attempts to place the liquid contained inside the bucket. */
	bool tryPlaceContainedLiquid(World* pWorld, const BlockPos& pos);
};

class ItemBucketMilk : public Item
{
public:
	ItemBucketMilk(int id, const String& name);
	
	/** implement override functions from Item. */
	virtual ItemStackPtr onEaten(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer);
	virtual int getMaxItemUseDuration(ItemStackPtr pStack) { return 32; }
	virtual ACTION_TYPE getItemUseAction(ItemStackPtr pStack) { return ACTION_TYPE_DRINK; }
	virtual ItemStackPtr onItemRightClick(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer);
};

class ItemBlockman : public Item
{
protected:
	static IBehaviorDispenseItem* dispenserMinecartBehavior; // = new BehaviorDispenseMinecart();

public:
	int minecartType = 0;

	ItemBlockman(int id, const String& name, int type);
	
	/** implement override functions from Item. */
	virtual bool onItemUse(ItemStackPtr pStack, EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos, int face, const Vector3& hit);

	// for initialize 
	static void initialize();
	static void unInitialize();
};

class ItemSaddle : public Item
{
public:
	ItemSaddle(int id, const String& name);

	/** implement override functions from Item. */
	virtual bool hitEntity(ItemStackPtr pStack, EntityLivingBase* pLiving1, EntityLivingBase* pLiving2);
};

class ItemRedstone : public Item
{
public:
	ItemRedstone(int id, const String& name);

	/** implement override functions from Item. */
	virtual bool onItemUse(ItemStackPtr pStack, EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos, int face, const Vector3& hit);
};


class ItemSnowball : public Item
{
public:
	ItemSnowball(int id, const String& name);

	/** implement override functions from Item. */
	virtual ItemStackPtr onItemRightClick(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer);
	virtual bool canLongTouchUse() { return true; }
};

class ItemTnt : public ItemBlock
{
public:
	ItemTnt(int id, const String& name);
	/** implement override functions from Item. */
	virtual ItemStackPtr onItemRightClick(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer) override;
	virtual bool canLongTouchUse() { return true; }
};

class ItemGrenade : public Item 
{
public:
	ItemGrenade(int id, const String& name);
	/** implement override functions from Item. */
	virtual ItemStackPtr onItemRightClick(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer) override;
	virtual bool canLongTouchUse() { return true; }
};

class ItemBoat : public Item
{
public:
	ItemBoat(int id, const String& name);

	/** implement override functions from Item. */
	virtual ItemStackPtr onItemRightClick(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer);
};

class ItemBook : public Item
{
public:
	ItemBook(int id, const String& name);
	
	/** implement override functions from Item. */
	virtual bool isItemTool(ItemStackPtr pStack);
	virtual int getItemEnchantability() { return 1; }
};

class ItemEgg : public Item
{
public:
	ItemEgg(int id, const String& name);
	
	/** implement override functions from Item. */
	virtual ItemStackPtr onItemRightClick(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer);
};

class ItemFishingRod : public Item
{
public:
	ItemFishingRod(int id, const String& name);

	/** implement override functions from Item. */
	virtual ItemStackPtr onItemRightClick(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer);
};

class ItemBed : public Item
{
public:
	ItemBed(int id, const String& name);

	/** implement override functions from Item. */
	virtual bool onItemUse(ItemStackPtr pStack, EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos, int face, const Vector3& hit);
};

class ItemExpBottle : public Item
{
public:
	ItemExpBottle(int id, const String& name);

	/** implement override functions from Item. */
	virtual bool hasEffect(ItemStackPtr pStack) { return true; }
	virtual ItemStackPtr onItemRightClick(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer);
};

class ItemFireball : public Item
{
public:
	ItemFireball(int id, const String& name);

	/** implement override functions from Item. */
	virtual ItemStackPtr onItemRightClick(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer);
	virtual bool onItemUse(ItemStackPtr pStack, EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos, int face, const Vector3& hit);
	virtual int getUseDurationTick() { return 16; }
	virtual bool canLongTouchUse() { return true; }
};

class ItemWritableBook : public Item
{
public:
	ItemWritableBook(int id, const String& name);

	/** implement override functions from Item. */
	virtual ItemStackPtr onItemRightClick(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer);
	virtual bool getShareTag() { return true; }

	static bool validBookTagPages(NBTTagCompound* pNbtCompound);
};

class ItemEditableBook : public Item
{
public:
	ItemEditableBook(int id, const String& name);

	/** implement override functions from Item. */
	virtual String getItemDisplayName(ItemStackPtr pStack);
	virtual ItemStackPtr onItemRightClick(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer);
	virtual bool getShareTag() { return true; }
	virtual bool hasEffect(ItemStackPtr pStack) { return true; }

	static bool validBookTagContents(NBTTagCompound* pNbtCompound);
};

class ItemSkull : public Item
{
public:
	static const int SKULL_TYPE_COUNT = 5;
	static String skullTypes[SKULL_TYPE_COUNT]; // = new String[] {"skeleton", "wither", "zombie", "char", "creeper"};
	
public:
	ItemSkull(int id, const String& name);

	/** implement override functions from Item. */
	virtual bool onItemUse(ItemStackPtr pStack, EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos, int face, const Vector3& hit);
	virtual int getMetadata(int meta) { return meta; }
	virtual String getUnlocalizedName(ItemStackPtr pStack);
	virtual String getItemDisplayName(ItemStackPtr pStack);
};

class ItemCarrotOnAStick : public Item
{
public:
	ItemCarrotOnAStick(int id, const String& name);
	
	/** implement override functions from Item. */
	virtual ItemStackPtr onItemRightClick(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer);
};

class ItemSimpleFoiled : public Item
{
public:
	ItemSimpleFoiled(int id, const String& name);

	/** implement override functions from Item. */
	virtual bool hasEffect(ItemStackPtr pStack) { return true; }
};

class ItemFirework : public Item
{
public:
	ItemFirework(int id, const String& name);

	/** implement override functions from Item. */
	virtual bool onItemUse(ItemStackPtr pStack, EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos, int face, const Vector3& hit);
};

class ItemFireworkCharge : public Item
{
public:
	ItemFireworkCharge(int id, const String& name);
};

class ItemLeash : public Item
{
public:
	ItemLeash(int id, const String& name);

	/** implement override functions from Item. */
	virtual bool onItemUse(ItemStackPtr pStack, EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos, int face, const Vector3& hit);
	
	static bool func_135066_a(EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos);
};

class ItemNameTag : public Item
{
public:
	ItemNameTag(int id, const String& name);

	/** implement override functions from Item. */
	virtual bool interactWithEntity(ItemStackPtr pStack, EntityPlayer* pPlayer, EntityLivingBase* pLiving);
};

class ItemLeaves : public ItemBlock
{
public:
	ItemLeaves(int id, const String& name);

	/** implement override functions from Item. */
	virtual int getMetadata(int meta) { return meta | 4; }
	virtual String getUnlocalizedName(ItemStackPtr pStack);
};

class ItemSlab : public ItemBlock
{
public:	
	ItemSlab(int id, const String& name, Block* pHalfSlab, Block* pDoubleSlab, bool isFullBlock);

	/** implement override functions from Item. */
	virtual int getMetadata(int meta) { return meta; }
	virtual String getUnlocalizedName(ItemStackPtr pStack);
	virtual bool onItemUse(ItemStackPtr pStack, EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos, int face, const Vector3& hit);

	/** implement override functions from ItemBlock. */
	virtual bool canPlaceItemBlockOnSide(World* pWorld, const BlockPos& pos, int face, EntityPlayer* pPlayer, ItemStackPtr pStack);

	vector<Vector3i>::type getPlacePositions(World* pWorld, BlockPos& pos, int face);

private:
	bool m_isFullBlock = false;	
	Block* m_pHalfSlab = nullptr;
	Block* m_pDoubleSlab = nullptr;

	bool func_77888_a(ItemStackPtr pItemStack, EntityPlayer* pEntityPlayer, World* pWorld, const BlockPos& pos, int face);
};

class ItemMultiTextureTile : public ItemBlock
{
public:
	ItemMultiTextureTile(int id, const String& name, const StringArray& names);

	/** implement override functions from Item. */
	virtual int getMetadata(int meta) { return meta; }
	virtual String getUnlocalizedName(ItemStackPtr pStack);

	ItemMultiTextureTile* setBlockNames(String names[], int length);
private:
	StringArray m_blockNames;
};

class ItemAnvilBlock : public ItemMultiTextureTile
{
public:
	ItemAnvilBlock(int id, const String& name);

	/** implement override functions from Item. */
	virtual int getMetadata(int meta) { return meta << 2; }

};

class ItemPiston : public ItemBlock
{
public:
	ItemPiston(int id);

	/** implement override functions from Item. */
	virtual int getMetadata(int meta) { return 7; }
};

class ItemBlockWithMetadata : public ItemBlock
{
public:
	ItemBlockWithMetadata(int id);

	/** implement override functions from Item. */
	virtual int getMetadata(int meta) { return meta; }

private:	
};

class ItemSnow : public ItemBlockWithMetadata
{
public:
	ItemSnow(int id);

	/** implement override functions from Item. */
	virtual bool onItemUse(ItemStackPtr pStack, EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos, int face, const Vector3& hit);
};

class ItemCloth : public ItemBlock
{
public:
	ItemCloth(int id, const String& name);

	/** implement override functions from Item. */
	virtual int getMetadata(int meta) { return meta; }
	virtual String getUnlocalizedName(ItemStackPtr pStack);
};

class ItemColored : public ItemBlock
{
public:
	ItemColored(int id, bool flag);
	ItemColored(int id, bool flag, const StringArray& names);

	/** implement override functions from Item. */
	virtual int getMetadata(int meta) { return meta; }
	virtual String getUnlocalizedName(ItemStackPtr pStack);

	ItemColored* setBlockNames(String names[], int len);
private:	
	vector<String>::type m_blockNames;
};

class ItemLilyPad : public ItemColored
{
public:
	ItemLilyPad(int id);

	/** implement override functions from Item. */
	virtual ItemStackPtr onItemRightClick(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer);
};

class ItemRecord : public Item
{
public:
	/** The name of the record. */
	String recordName;
protected:
	typedef map<String, ItemRecord*>::type RecordMap;
	/** List of all record items and their names. */
	static RecordMap records;

public:
	ItemRecord(int id, const String& name, const String& record);

	/** implement override functions from Item. */
	virtual bool onItemUse(ItemStackPtr pStack, EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos, int face, const Vector3& hit);

	/** Return the title for this record. */
	String getRecordTitle() { return String("C418 - ") + recordName; }

	/** Return an item rarity from EnumRarity */
	// todo.
	// EnumRarity getRarity(ItemStack par1ItemStack) { return EnumRarity.rare; }

	/** Return the record item corresponding to the given name. */
	static ItemRecord* getRecord(const String& name);
};

class ItemToolDigger : public ItemTool
{
public:
	ItemToolDigger(int id, const String& name, ToolMaterial* material);
	/** implement override functions */
	virtual bool canHarvestBlock(Block* pBlock);
	virtual float getStrVsBlock(ItemStackPtr pStack, Block* pBlock) { return efficiencyOnProperMaterial; }
};

class ItemMedichine : public Item
{
private:
	/** The amount this medicine item heals the player. */
	float healAmount = 0;

public:
	ItemMedichine(int id, const String& name, float amount);
	void setHealAmount(float amount) { healAmount = amount; }

	/** implement override functions */
	virtual int getMaxItemUseDuration(ItemStackPtr pStack) { return 32; }
	virtual ACTION_TYPE getItemUseAction(ItemStackPtr pStack) { return ACTION_TYPE_EAT; }
	virtual ItemStackPtr onItemRightClick(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer);
	virtual ItemStackPtr onEaten(ItemStackPtr pItemStack, World* pWorld, EntityPlayer* pPlayer);
	virtual bool canLongTouchUse() { return true; }

	float getHealAmount() { return healAmount; }
};

class ItemProp : public Item
{
	public:
		ItemProp(int id, const String& name);

	private:
		virtual bool onItemUse(ItemStackPtr pStack, EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos, int face, const Vector3& hit);

};

class ItemSkillProp : public Item
{
	public:
		ItemSkillProp(int id, const String& name);

private:
	virtual bool onItemUse(ItemStackPtr pStack, EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos, int face, const Vector3& hit);
};

class ItemSkillBook : public Item 
{
public:
	ItemSkillBook(int id, const String& name);
	bool isFirstRender() { return false; }
	bool isThirdRender() { return false; }
};

class ItemSkillBookFragmentation : public Item
{
public:
	ItemSkillBookFragmentation(int id, const String& name);
	bool isFirstRender() { return false; }
	bool isThirdRender() { return false; }
};
 
class ItemSurpriseTreasureChest : public Item
{
public:
	ItemSurpriseTreasureChest(int id, const String& name);
	bool isFirstRender() { return false; }
	bool isThirdRender() { return false; }
};

class ItemBanner : public ItemAnvilBlock
{
public:
	ItemBanner(int id, const String& name);

	/** implement override functions from Item. */
	virtual String getUnlocalizedName(ItemStackPtr pStack);
	virtual int getMetadata(int meta) { return meta; }
};

class ItemAppIcon : public Item
{
public:
	ItemAppIcon(int id, const String& name);
};

class ItemTransparent : public Item
{
public:
	ItemTransparent(int id, const String& name);
	bool isFirstRender() { return false; }
	bool isThirdRender() { return false; }
};

class ItemBuild : public Item
{
public:
	ItemBuild(int id, const String& name);

	/** implement override functions */
	bool onItemUse(ItemStackPtr pStack, EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos, int face, const Vector3& hit);
	int getActorId();
	int getActorType();
	float getLength();
	float getWidth();
	float getHeight();
};

class ItemHouse : public Item
{

public:
	ItemHouse(int id, const String& name);

	/** implement override functions */
	bool onItemUse(ItemStackPtr pStack, EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos, int face, const Vector3& hit);

	float getLength();
	float getWidth();
	float getHeight();
	String getTemplateName();
};

class ItemToolGather : public ItemTool
{
public:
	ItemToolGather(int id, const String& name, ToolMaterial* material);
	/** implement override functions */
	virtual bool canHarvestBlock(Block* pBlock);
	virtual float getStrVsBlock(ItemStackPtr pStack, Block* pBlock) { return efficiencyOnProperMaterial; }
};

class ItemFruitCurrency : public Item
{
public:
	ItemFruitCurrency(int id, const String& name);
};

}
#endif
