/********************************************************************
filename: 	cItem.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-6-23
*********************************************************************/
#ifndef __BLOCK_CLIENT_HEADER__
#define __BLOCK_CLIENT_HEADER__

#include "Item/Item.h"
#include "Item/Items.h"

namespace BLOCKMAN
{

class AtlasSprite;
class TextureAtlasRegister;
class cBlock;

enum class ItemRenderType
{
	BLOCK,
	TEXTURE,
	COMPASS,
	CLOCK,
	MESH
};

class cItem : public ObjectAlloc
{

public:
	cItem(const String& atlas);
	virtual ~cItem() = default;

	virtual ItemRenderType getRenderType() { return ItemRenderType::TEXTURE; }

	/** if the spriteNumber is 3. get the mesh resource by this method */
	virtual const String& getMeshName() const { return StringUtil::BLANK; }
	
	/** Gets an icon index based on an item's damage value */
	virtual AtlasSprite* getIconFromDamage(int damage) { return itemIcon; }

	/** Returns the icon index of the stack given as argument. */
	AtlasSprite* getIconIndex(ItemStackPtr pStack);

	/** Sets bFull3D to True and return the object. */
	void setFull3D() { bFull3D = true; }

	/** Returns True is the item is renderer in full 3D when hold. */
	virtual bool isFull3D() { return bFull3D; }

	/** Returns true if this item should be rotated by 180 degrees around the Y axis when being held in an entities hands. */
	virtual bool shouldRotateAroundWhenRendering() { return false; }

	virtual int getColorFromItemStack(ItemStackPtr pStack, int par2) { return 0x00FFFFFF; }

	/** allows items to add custom lines of information to the mouseover description */
	virtual void addInformation(ItemStackPtr pStack, EntityPlayer* pPlayer, StringArray& fills, bool advance) {}

	virtual bool requiresMultipleRenderPasses() { return false; }

	/** Gets an icon index based on an item's damage value and the given render pass */
	virtual AtlasSprite* getIconFromDamageForRenderPass(int damage, int pass) { return getIconFromDamage(damage); }

	virtual void registerIcons(TextureAtlasRegister* atlas);

	virtual bool IsModelWithMeta() { return false; }

protected:
	String getAtlasName();
	void setAtlasName(const String& name) { atlasName = name; }

protected:
	/** If true, render the object in full 3D, like weapons and tools. */
	bool bFull3D = false;

	/** the sprite name in atlas. */
	String atlasName;

	/** the sprite for render. */
	AtlasSprite* itemIcon = nullptr;
};

class ItemClient : public Item, public cItem
{
public:
	static cItem* citemsList[Item::MAX_ITEM_COUNT];
	static void cInitialize();
	static void cUnInitialize();
	static void registSprite(TextureAtlasRegister* pAtlas);
	static ItemRenderType getRenderType(ItemStackPtr pStack);

public:
	ItemClient(int id, const String& name, const String& atlas);
};

class cItemArrowPotion : public ItemArrowPotion, public cItem {
public:
	cItemArrowPotion(int id, const String& name, const String& atlas);
};

class cItemSpade : public ItemSpade, public cItem
{
public:
	cItemSpade(int id, const String& name, const String& atlas, ToolMaterial* toolMat);

	/** implement the override functions from cItem. */
	virtual bool isFull3D() { return true; }
};

class cItemPickaxe : public ItemPickaxe, public cItem
{
public:
	cItemPickaxe(int id, const String& name, const String& atlas, ToolMaterial* toolMat);
	
	/** implement the override functions from cItem. */
	virtual bool isFull3D() { return true; }
};

class cItemAxe : public ItemAxe, public cItem
{
public:
	cItemAxe(int id, const String& name, const String& atlas, ToolMaterial* toolMat);

	/** implement the override functions from cItem. */
	virtual bool isFull3D() { return true; }
};

class cItemFlintAndSteel : public ItemFlintAndSteel, public cItem
{
public:
	cItemFlintAndSteel(int id, const String& name, const String& atlas);
};

class cItemFood : public ItemFood, public cItem
{
public:
	cItemFood(int id, const String& name, const String& atlas, int amount, float modifier, bool isfavorite);
	cItemFood(int id, const String& name, const String& atlas, int amount, bool isfavorite);
};

class cItemSoup : public ItemSoup, public cItem
{
public:
	cItemSoup(int id, const String& name, const String& atlas, int amount);
};

class cItemAppleGold : public ItemAppleGold, public cItem
{
public:
	cItemAppleGold(int id, const String& name, const String& atlas, int amount, float modifier, bool isfavorite);
};

class cItemSeedFood : public ItemSeedFood, public cItem
{
public:
	cItemSeedFood(int id, const String& name, const String& atlas, int amount, float modifier, int crop, int soil);
};

class cItemBow : public ItemBow, public cItem
{
protected:
	static const int MAX_ATLAS_COUNT = 3;
	static String bowPullIconNameArray[MAX_ATLAS_COUNT];	// = new String[]{ "pulling_0", "pulling_1", "pulling_2" };
	AtlasSprite* iconArray[MAX_ATLAS_COUNT] = { nullptr };

public:
	cItemBow(int id, const String& name, const String& atlas);

	/** implement override functions from cItem. */
	virtual void registerIcons(TextureAtlasRegister* atlas);
	virtual bool IsModelWithMeta() { return true; }

	/** get the item key for diffurent use duration. */
	int getItemKey(int useDuration);
	int getDurationSeg(int useDuration);

	AtlasSprite* getItemIconForUseDuration(int duration) { return iconArray[duration]; }
};

class cItemGun : public ItemGun, public cItem 
{
public:
	cItemGun(int id, int gunId, const String& name, const String& atlas, const String& mesh);

	virtual ItemRenderType getRenderType() override;
	virtual const String& getMeshName() const override;
	virtual void fire(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer);
	virtual String getUnlocalizedName(ItemStackPtr pStack);

private:
	String m_meshName;

private:
	void initCItemGun();
};

class cItemBulletClip : public ItemBulletClip, public cItem
{
public:
	cItemBulletClip(int id, int bulletClipId, const String& name, const String& atlas);
	virtual String getUnlocalizedName(ItemStackPtr pStack);

private:
	void initCItemBulletClip();
};

class cItemHandcuffs : public ItemHandcuffs, public cItem
{
public:
	cItemHandcuffs(int id, const String& name, const String& atlas);
};

class cItemKey : public ItemKey, public cItem
{
public:
	cItemKey(int id, const String& name, const String& atlas);
};

class cItemTeleportScroll : public ItemTeleportScroll, public cItem
{
public:
	cItemTeleportScroll(int id, const String& name, const String& atlas);
};

class cItemCreateBridge : public ItemCreateBridge, public cItem
{
public:
	cItemCreateBridge(int id, const String& name, const String& atlas);
};

class cItemCoal : public ItemCoal, public cItem
{
protected:
	AtlasSprite* m_charcoal = nullptr;

public:
	cItemCoal(int id, const String& name, const String& atlas);

	/** implement override functions from cItem. */
	virtual AtlasSprite* getIconFromDamage(int damage);
	virtual void registerIcons(TextureAtlasRegister* atlas);
};

class cItemSword : public ItemSword, public cItem
{
public:
	cItemSword(int id, const String& name, const String& atlas, ToolMaterial* toolMat);
	
	/** implement override functions from cItem. */
	virtual bool isFull3D() { return true; }
};

class cItemReed : public ItemReed, public cItem
{
public:
	cItemReed(int id, const String& name, const String& atlas, int reedID);
};

class cItemHoe : public ItemHoe, public cItem
{
public:
	cItemHoe(int id, const String& name, const String& atlas, ToolMaterial* toolMat);

	/** implement override functions from cItem*/
	virtual bool isFull3D() { return true; }
};

class cItemSeeds : public ItemSeeds, public cItem
{
public:
	cItemSeeds(int id, const String& name, const String& atlas, int blocktype, int soilID);
};

class cItemHangingEntity : public ItemHangingEntity, public cItem
{
public:
	cItemHangingEntity(int id, const String& name, const String& atlas, int classID);
};

class cItemSign : public ItemSign, public cItem
{
public:
	cItemSign(int id, const String& name, const String& atlas);
};


class cItemDoor : public ItemDoor, public cItem
{
public:
	cItemDoor(int id, const String& name, const String& atlas, const BM_Material& mat);
};

class cItemEnderPearl : public ItemEnderPearl, public cItem
{
public:
	cItemEnderPearl(int id, const String& name, const String& atlas);
};

class cItemGlassBottle : public ItemGlassBottle, public cItem
{
public:
	cItemGlassBottle(int id, const String& name, const String& atlas);

	/** implement override functions from cItem. */
	virtual AtlasSprite* getIconFromDamage(int damage);
	virtual void registerIcons(TextureAtlasRegister* atlas);
};

class cItemEnderEye : public ItemEnderEye, public cItem
{
public:
	cItemEnderEye(int id, const String& name, const String& atlas);
};


class cItemMonsterPlacer : public ItemMonsterPlacer, public cItem
{
protected:
	AtlasSprite* theIcon = nullptr;

public:
	cItemMonsterPlacer(int id, const String& name, const String& atlas);

	/** implement override functions from cItem. */
	virtual int getColorFromItemStack(ItemStackPtr pStack, int par2);
	virtual bool requiresMultipleRenderPasses() { return true; }
	virtual AtlasSprite* getIconFromDamageForRenderPass(int damage, int pass);

	virtual void registerIcons(TextureAtlasRegister* atlas);
};

class cItemArmor : public ItemArmor, public cItem
{
protected:
	/** Holds the 'base' maxDamage that each armorType have. */
	const static int MAX_ARMOR_COUNT = 4;
	static String leather_armor_name[MAX_ARMOR_COUNT];	// = new String[]{ "leather_helmet_overlay", "leather_chestplate_overlay", "leather_leggings_overlay", "leather_boots_overlay" };

	AtlasSprite* leather_sprite = nullptr;
	AtlasSprite* armor_sprite = nullptr;

public:
	static String empty_armor_name[MAX_ARMOR_COUNT];	// = new String[]{ "empty_armor_slot_helmet", "empty_armor_slot_chestplate", "empty_armor_slot_leggings", "empty_armor_slot_boots" };

	/** Used on RenderPlayer to select the correspondent armor to be rendered on the player: 0 is cloth, 1 is chain, 2 is iron, 3 is diamond and 4 is gold. */
	int renderIndex;

	cItemArmor(int id, const String& name, const String& atlas, ArmorMaterial* armorMat, int rendertype, int armortype);

	static AtlasSprite* getSpriteByArmorType(int armorIdx);

	/** implement override functions from cItem. */
	virtual int getColorFromItemStack(ItemStackPtr pStack, int par2);
	virtual bool requiresMultipleRenderPasses() { return material == ArmorMaterial::CLOTH; }
	virtual AtlasSprite* getIconFromDamageForRenderPass(int damage, int pass);
	virtual void registerIcons(TextureAtlasRegister* atlas);
};

class cItemPotion : public ItemPotion, public cItem
{
protected:
	AtlasSprite* sprite_potion_splash = nullptr;
	AtlasSprite* sprite_potion_drinkable = nullptr;
	AtlasSprite* sprite_potion_overlay = nullptr;

public:
	cItemPotion(int id, const String& name, const String& atlas, bool isSplash = false);

	static AtlasSprite* getSpriteByName(const String& spritename);

	/** implement override functions from cItem. */
	virtual AtlasSprite* getIconFromDamage(int damage) { return isSplash(damage) ? sprite_potion_splash : sprite_potion_drinkable; }
	virtual AtlasSprite* getIconFromDamageForRenderPass(int damage, int pass);
	virtual int getColorFromItemStack(ItemStackPtr pStack, int par2);
	virtual bool requiresMultipleRenderPasses() { return true; }
	virtual void registerIcons(TextureAtlasRegister* atlas);
};

class cItemEnchantedBook : public ItemEnchantedBook, public cItem
{
public:
	cItemEnchantedBook(int id, const String& name, const String& atlas);
};

class cItemDye : public ItemDye, public cItem
{
protected:
	static const int MAX_DYE_NUM = 16;
	AtlasSprite* dyeSprites[MAX_DYE_NUM] = { nullptr };

public:
	/** List of dye color names */
	static String dyeSpriteNames[MAX_DYE_NUM];// = new String[]{ "black", "red", "green", "brown", "blue", "purple", "cyan", "silver", "gray", "pink", "lime", "yellow", "light_blue", "magenta", "orange", "white" };

public:
	cItemDye(int id, const String& name, const String& atlas);

	/** implement override functions from cItem. */
	virtual AtlasSprite* getIconFromDamage(int damage);
	virtual void registerIcons(TextureAtlasRegister* atlas);
};

class cItemMapBase : public ItemMapBase, public cItem
{
public:
	cItemMapBase(int id, const String& name, const String& atlas);
};

class cItemEmptyMap : public ItemEmptyMap, public cItem
{
public:
	cItemEmptyMap(int id, const String& name, const String& atlas);
};

class cItemMap : public ItemMap, public cItem
{
public:
	cItemMap(int id, const String& name, const String& atlas);
};

class cItemBlock : public cItem
{
protected:
	AtlasSprite* blockSprite = nullptr;

public:
	cItemBlock(int id);
	/** implement override functions from cItem. */
	virtual AtlasSprite* getIconFromDamage(int damage);
	virtual ItemRenderType getRenderType() override;
	virtual void registerIcons(TextureAtlasRegister* atlas);
	int getBlockId() const { return m_blockID; }

protected:
	int m_blockID = 0;
};

class cItemShears : public ItemShears, public cItem
{
public:
	cItemShears(int id, const String& name, const String& atlas);
};

class cItemBucket : public ItemBucket, public cItem
{
public:
	cItemBucket(int id, const String& name, const String& atlas, int fillID);
};

class cItemBucketMilk : public ItemBucketMilk, public cItem
{
public:
	cItemBucketMilk(int id, const String& name, const String& atlas);
};

class cItemBlockman : public ItemBlockman, public cItem
{
public:
	cItemBlockman(int id, const String& name, const String& atlas, int type);
};


class cItemSaddle : public ItemSaddle, public cItem
{
public:
	cItemSaddle(int id, const String& name, const String& atlas);
};

class cItemRedstone : public ItemRedstone, public cItem
{
public:
	cItemRedstone(int id, const String& name, const String& atlas);
};

class cItemSnowball : public ItemSnowball, public cItem
{
public:
	cItemSnowball(int id, const String& name, const String& atlas);
	static const int SNOW_TYPE_CONUT = 3;
	virtual void registerIcons(TextureAtlasRegister* atlas);
	virtual AtlasSprite* getIconFromDamage(int damage);
	virtual bool IsModelWithMeta() { return true; }
protected:
	static String spriteNames[SNOW_TYPE_CONUT];
	AtlasSprite* sprites[SNOW_TYPE_CONUT] = { nullptr };
};

class cItemTnt : public ItemTnt, public cItemBlock
{
public:
	cItemTnt(int id, const String& name);
};

class cItemGrenade : public ItemGrenade, public cItem
{
public:
	cItemGrenade(int id, const String& name, const String& atlas);
	virtual void registerIcons(TextureAtlasRegister* atlas);
};

class cItemBoat : public ItemBoat, public cItem
{
public:
	cItemBoat(int id, const String& name, const String& atlas);
};

class cItemBook : public ItemBook, public cItem
{
public:
	cItemBook(int id, const String& name, const String& atlas);
};

class cItemEgg : public ItemEgg, public cItem
{
public:
	cItemEgg(int id, const String& name, const String& atlas);
};

class cItemFishingRod : public ItemFishingRod, public cItem
{
public:
	cItemFishingRod(int id, const String& name, const String& atlas);
	
	/** implement override functions from cItem. */
	virtual bool isFull3D() { return true; }
	virtual bool shouldRotateAroundWhenRendering() { return true; }
	virtual void registerIcons(TextureAtlasRegister* atlas);

	AtlasSprite* getCastIcon() { return m_castIcon; }

protected:
	AtlasSprite* m_castIcon = nullptr;
};

class cItemBed : public ItemBed, public cItem
{
public:
	cItemBed(int id, const String& name, const String& atlas);
};

class cItemExpBottle : public ItemExpBottle, public cItem
{
public:
	cItemExpBottle(int id, const String& name, const String& atlas);
};

class cItemFireball : public ItemFireball, public cItem
{
public:
	cItemFireball(int id, const String& name, const String& atlas);
};

class cItemWritableBook : public ItemWritableBook, public cItem
{
public:
	cItemWritableBook(int id, const String& name, const String& atlas);
};

class cItemEditableBook : public ItemEditableBook, public cItem
{
public:
	cItemEditableBook(int id, const String& name, const String& atlas);

	/** implement override functions from cItem. */
	virtual void addInformation(ItemStackPtr pStack, EntityPlayer* pPlayer, StringArray& fills, bool advance);
};

class cItemSkull : public ItemSkull, public cItem
{
public:
	cItemSkull(int id, const String& name, const String& atlas);

	/** implement override functions from cItem. */
	virtual AtlasSprite* getIconFromDamage(int damage);
	virtual void registerIcons(TextureAtlasRegister* atlas);
	
protected:
	static String spriteNames[SKULL_TYPE_COUNT];// {"skeleton", "wither", "zombie", "steve", "creeper"};
	AtlasSprite* sprites[SKULL_TYPE_COUNT] = { nullptr };
};

class cItemCarrotOnAStick : public ItemCarrotOnAStick, public cItem
{
public:
	cItemCarrotOnAStick(int id, const String& name, const String& atlas);

	/** implement override functions from cItem. */
	virtual bool isFull3D() { return true; }
	virtual bool shouldRotateAroundWhenRendering() { return true; }
};

class cItemSimpleFoiled : public ItemSimpleFoiled, public cItem
{
public:
	cItemSimpleFoiled(int id, const String& name, const String& atlas);
};

class cItemFirework : public ItemFirework, public cItem
{
public:
	cItemFirework(int id, const String& name, const String& atlas);

	/** implement override functions from cItem. */
	virtual void addInformation(ItemStackPtr pStack, EntityPlayer* pPlayer, StringArray& fills, bool advance);
};

class cItemFireworkCharge : public ItemFireworkCharge, public cItem
{
public:
	cItemFireworkCharge(int id, const String& name, const String& atlas);

	/** implement override functions from cItem. */
	virtual AtlasSprite* getIconFromDamageForRenderPass(int damage, int pass);
	virtual int getColorFromItemStack(ItemStackPtr pStack, int par2);
	virtual bool requiresMultipleRenderPasses() { return true; }
	virtual void addInformation(ItemStackPtr pStack, EntityPlayer* pPlayer, StringArray& fills, bool advance);
	virtual void registerIcons(TextureAtlasRegister* atlas);

	static NBTBase* getExplosionTag(ItemStackPtr pStack, const String& key);
	static void addExplosionInfo(NBTTagCompound* pNbtCompound, StringArray& fills);

protected:
	AtlasSprite* theIcon = nullptr;
};

class cItemLeash : public ItemLeash, public cItem
{
public:
	cItemLeash(int id, const String& name, const String& atlas);
};

class cItemNameTag : public ItemNameTag, public cItem
{
public:
	cItemNameTag(int id, const String& name, const String& atlas);
};

class cItemBlockImp : public ItemBlock, public cItemBlock
{
public:
	cItemBlockImp(int id);
};

class cItemLeaves : public ItemLeaves, public cItemBlock
{
public:
	cItemLeaves(int id, const String& name);

	/** implement override functions from cItem. */
	virtual AtlasSprite* getIconFromDamage(int damage);
	virtual int getColorFromItemStack(ItemStackPtr pStack, int par2);
};

class cItemSlab : public ItemSlab, public cItemBlock
{
public:
	cItemSlab(int id, const String& name, Block* pHalfSlab, Block* pDoubleSlab, bool isFullBlock);

	/** implement override functions from cItem. */
	virtual AtlasSprite* getIconFromDamage(int damage);
};

class cItemMultiTextureTile : public ItemMultiTextureTile, public cItemBlock
{
public:
	cItemMultiTextureTile(int id, const String& name, const StringArray& names, cBlock* pBlock);

	/** implement override functions from cItem. */
	virtual AtlasSprite* getIconFromDamage(int damage);

protected:
	cBlock* m_pModelBlock = nullptr;
};

class cItemAnvilBlock : public ItemAnvilBlock, public cItemBlock
{
public:
	cItemAnvilBlock(int id, const String& name, cBlock* pBlock);

	/** implement override functions from cItem. */
	virtual AtlasSprite* getIconFromDamage(int damage);

private:
	cBlock* m_pModelBlock = nullptr;
};

class cItemPiston : public ItemPiston, public cItemBlock
{
public:
	cItemPiston(int id);
};

class cItemBlockWithMetadata : public ItemBlockWithMetadata, public cItemBlock
{
public:
	cItemBlockWithMetadata(int id, cBlock* pBlock);

	/** implement override functions from cItem. */
	virtual AtlasSprite* getIconFromDamage(int damage);

protected:
	cBlock* m_pModelBlock = nullptr;
};

class cItemSnow : public ItemSnow, public cItemBlock
{
public:
	cItemSnow(int id, cBlock* pBlock);

	/** implement override functions from cItem. */
	virtual AtlasSprite* getIconFromDamage(int damage);

private:
	cBlock* m_pModelBlock = nullptr;
};

class cItemCloth : public ItemCloth, public cItemBlock
{
public:
	cItemCloth(int id, const String& name);

	/** implement override functions from cItem. */
	virtual AtlasSprite* getIconFromDamage(int damage);
};

class cItemColored : public cItemBlock
{
public:
	cItemColored(int id);

	/** implement override functions from cItem. */
	virtual AtlasSprite* getIconFromDamage(int damage);
	virtual int getColorFromItemStack(ItemStackPtr pStack, int par2);

protected:
	cBlock* m_pModelBlock = nullptr;
};

class cItemColoredImp : public ItemColored, public cItemColored
{
public:
	cItemColoredImp(int id, bool flag);
	cItemColoredImp(int id, bool flag, const StringArray& names);

	cItemColoredImp* setBlockNames(String names[], int len);
};

class cItemLilyPad : public ItemLilyPad, public cItemColored
{
public:
	cItemLilyPad(int id);

	/** implement override functions from cItem. */
	virtual int getColorFromItemStack(ItemStackPtr pStack, int par2);
};

class cItemRecord : public ItemRecord, public cItem
{
public:
	cItemRecord(int id, const String& name, const String& record, const String& atlas);

	/** implement override functions from cItem. */
	virtual void addInformation(ItemStackPtr pStack, EntityPlayer* pPlayer, StringArray& fills, bool advance);
};

// add by zhouyou.
class cItemCompass : public Item, public cItem
{
public:
	static const int COMPASS_FRAME_COUNT = 32;
	cItemCompass(int id, const String& name, const String& atlas);

	/** implement override functions from cItem. */
	virtual AtlasSprite* getIconFromDamage(int damage) { LordAssert(damage >= 0 && damage < COMPASS_FRAME_COUNT); return m_sprites[damage]; }
	virtual ItemRenderType getRenderType() override { return ItemRenderType::COMPASS; }
	virtual void registerIcons(TextureAtlasRegister* atlas);

	AtlasSprite* m_sprites[COMPASS_FRAME_COUNT] = { nullptr };
};

// add by zhouyou
class cItemClock : public Item, public cItem
{
public:
	static const int CLOCK_FRAME_COUNT = 64;
	cItemClock(int id, const String& name, const String& atlas);

	/** implement override functions from cItem. */
	virtual AtlasSprite* getIconFromDamage(int damage) { LordAssert(damage >= 0 && damage < CLOCK_FRAME_COUNT); return m_sprites[damage]; }
	virtual ItemRenderType getRenderType() override { return ItemRenderType::CLOCK; }
	virtual void registerIcons(TextureAtlasRegister* atlas);

	AtlasSprite* m_sprites[CLOCK_FRAME_COUNT] = { nullptr };
};

class cItemToolDigger : public ItemToolDigger, public cItem
{
public:
	cItemToolDigger(int id, const String& name, const String& atlas, const String& meshName, ToolMaterial* toolMat);

	/** implement the override functions from cItem. */
	virtual ItemRenderType getRenderType() override;
	virtual const String& getMeshName() const override;

private:
	String m_meshName;
};

class cItemMedicine : public ItemMedichine, public cItem
{
public:
	cItemMedicine(int id, const String& name, const String& atlas);
};

class cItemProp : public ItemProp, public cItem
{
public:
	cItemProp(int id, const String& name, const String& atlas, const String& mesh);

	virtual ItemRenderType getRenderType() override;
	virtual const String& getMeshName() const override;

private:
	String m_meshName;

};

class cItemSkillProp : public ItemSkillProp, public cItem
{
public:
	cItemSkillProp(int id, const String& name, const String& atlas, const String& mesh);

	virtual ItemRenderType getRenderType() override;
	virtual const String& getMeshName() const override;

private:
	String m_meshName;
};

class cItemSkillBook : public ItemSkillBook, public cItem
{
public:
	cItemSkillBook(int id, const String& name, const String& atlas);

	virtual ItemRenderType getRenderType() override;
	/** implement override functions from cItem. */
	virtual void registerIcons(TextureAtlasRegister* atlas) override;

};

class cItemSkillBookFragmentation : public ItemSkillBookFragmentation, public cItem
{
public:
	cItemSkillBookFragmentation(int id, const String& name, const String& atlas);

	virtual ItemRenderType getRenderType() override;
	/** implement override functions from cItem. */
	virtual void registerIcons(TextureAtlasRegister* atlas) override;

};

class cItemSurpriseTreasureChest : public ItemSurpriseTreasureChest, public cItem
{
public:
	cItemSurpriseTreasureChest(int id, const String& name, const String& atlas);

	virtual ItemRenderType getRenderType() override;
	/** implement override functions from cItem. */
	virtual void registerIcons(TextureAtlasRegister* atlas) override;
};

class cItemBanner : public ItemBanner, public cItemBlock
{
public:
	cItemBanner(int id, const String& name);

	/** implement override functions from cItem. */
	virtual ItemRenderType getRenderType() override;
};

class cItemAppIcon : public ItemAppIcon, public cItem
{
public:
	cItemAppIcon(int id, const String& name, const String& atlas);
};

class cItemBuild : public ItemBuild, public cItem
{
public:
	cItemBuild(int id, const String& name, const String& atlas);
	String getUnlocalizedName(ItemStackPtr pStack) override;
	void registerIcons(TextureAtlasRegister* atlas) override;
};

class cItemHouse : public ItemHouse, public cItem
{
public:
	cItemHouse(int id, const String& name, const String& atlas);
	String getUnlocalizedName(ItemStackPtr pStack) override;
	void registerIcons(TextureAtlasRegister* atlas) override;
	virtual ItemRenderType getRenderType() { return ItemRenderType::TEXTURE; }
};

class cRanchSeeds : public RanchSeeds , public cItem
{
public:
	cRanchSeeds(int id, const String& name, const String& atlas, int blocktype, int soilID);
	void registerIcons(TextureAtlasRegister* atlas) override;
	virtual ItemRenderType getRenderType() { return ItemRenderType::TEXTURE; }
};

class cItemTransparent : public ItemTransparent, public cItem
{
public:
	cItemTransparent(int id, const String& name, const String& atlas);
};

class cItemToolGather : public ItemToolGather, public cItem
{
public:
	cItemToolGather(int id, const String& name, const String& atlas, const String& meshName, ToolMaterial* toolMat);

	/** implement the override functions from cItem. */
	virtual ItemRenderType getRenderType() override;
	virtual const String& getMeshName() const override;

private:
	String m_meshName;
};

class cItemFruitCurrency : public ItemFruitCurrency, public cItem
{
public:
	cItemFruitCurrency(int id, const String& name, const String& atlas, const String& meshName);

	/** implement the override functions from cItem. */
	virtual ItemRenderType getRenderType() override;
	void registerIcons(TextureAtlasRegister* atlas) override;
	String getUnlocalizedName(ItemStackPtr pStack) override;
	virtual const String& getMeshName() const override;

private:
	String m_meshName;
};

}
#endif
