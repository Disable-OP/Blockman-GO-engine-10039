#include "cItem.h"

#include "cBlock/cBlock.h"
#include "cBlock/cBlockManager.h"
#include "Render/TextureAtlas.h"

#include "Block/Block.h"
#include "Block/BlockManager.h"
#include "Block/BM_Material.h"
#include "Item/ItemStack.h"
#include "Item/Potion.h"
#include "Item/PotionManager.h"
#include "cItem/cPotionManager.h"
#include "Entity/EntityPlayer.h"
#include "World/RayTracyResult.h"
#include "World/World.h"
#include "Network/ClientNetwork.h"
#include "Setting/GunSetting.h"
#include "Setting/BulletClipSetting.h"
#include "Setting/GrenadeSetting.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Setting/HouseSetting.h"
#include "Setting/BuildingSetting.h"
#include "World/GameSettings.h"
#include "Setting/BirdFruitCurrencySetting.h"

namespace BLOCKMAN
{

cItem::cItem(const String& atlas)
: atlasName(atlas)
, bFull3D(false)
, itemIcon(NULL)
{

}

AtlasSprite* cItem::getIconIndex(ItemStackPtr pStack)
{
	return getIconFromDamage(pStack->getItemDamage());
}

void cItem::registerIcons(TextureAtlasRegister* atlas)
{
	itemIcon = atlas->getAtlasSprite("items.json", getAtlasName());
}

String cItem::getAtlasName()
{
	String result = atlasName;
	if (atlasName == StringUtil::BLANK)
	{
		// result = StringUtil::Format("MISSING_ICON_ITEM_%d_%s", itemID, unlocalizedName.c_str());
		result = "MISSING_ICON_ITEM";
	}
	return result;
}

ItemClient::ItemClient(int id, const String& name, const String& atlas)
: Item(id, name)
, cItem(atlas)
{}

cItem* ItemClient::citemsList[MAX_ITEM_COUNT];

void ItemClient::cInitialize()
{
	itemRand = LordNew Random();
	memset(itemsList, 0, sizeof(Item*) * MAX_ITEM_COUNT);
	memset(citemsList, 0, sizeof(cItem*) * MAX_ITEM_COUNT);

	shovelIron = LordNew cItemSpade(0, "shovelIron", "iron_shovel", ToolMaterial::IRON);
	pickaxeIron = LordNew cItemPickaxe(1, "pickaxeIron", "iron_pickaxe", ToolMaterial::IRON);
	axeIron = LordNew cItemAxe(2, "hatchetIron", "iron_axe", ToolMaterial::IRON);
	flintAndSteel = LordNew cItemFlintAndSteel(3, "flintAndSteel", "flint_and_steel");
	appleRed = LordNew cItemFood(4, "apple", "apple", 4, 0.3F, false);
	bow = LordNew cItemBow(5, "bow", "bow");
	arrow = LordNew ItemClient(6, "arrow", "arrow");
	coal = LordNew cItemCoal(7, "coal", "coal");
	diamond = LordNew ItemClient(8, "diamond", "diamond");
	ingotIron = LordNew ItemClient(9, "ingotIron", "iron_ingot");
	ingotGold = LordNew ItemClient(10, "ingotGold", "gold_ingot");
	swordIron = LordNew cItemSword(11, "swordIron", "iron_sword", ToolMaterial::IRON);
	swordWood = LordNew cItemSword(12, "swordWood", "wood_sword", ToolMaterial::WOOD);
	shovelWood = LordNew cItemSpade(13, "shovelWood", "wood_shovel", ToolMaterial::WOOD);
	pickaxeWood = LordNew cItemPickaxe(14, "pickaxeWood", "wood_pickaxe", ToolMaterial::WOOD);
	axeWood = LordNew cItemAxe(15, "hatchetWood", "wood_axe", ToolMaterial::WOOD);
	swordStone = LordNew cItemSword(16, "swordStone", "stone_sword", ToolMaterial::STONE);
	shovelStone = LordNew cItemSpade(17, "shovelStone", "stone_shovel", ToolMaterial::STONE);
	pickaxeStone = LordNew cItemPickaxe(18, "pickaxeStone", "stone_pickaxe", ToolMaterial::STONE);
	axeStone = LordNew cItemAxe(19, "hatchetStone", "stone_axe", ToolMaterial::STONE);
	swordDiamond = LordNew cItemSword(20, "swordDiamond", "diamond_sword", ToolMaterial::EMERALD);
	shovelDiamond = LordNew cItemSpade(21, "shovelDiamond", "diamond_shovel", ToolMaterial::EMERALD);
	pickaxeDiamond = LordNew cItemPickaxe(22, "pickaxeDiamond", "diamond_pickaxe", ToolMaterial::EMERALD);
	axeDiamond = LordNew cItemAxe(23, "hatchetDiamond", "diamond_axe", ToolMaterial::EMERALD);
	stick = LordNew ItemClient(24, "stick", "stick");
	bowlEmpty = LordNew ItemClient(25, "bowl", "bowl");
	bowlSoup = LordNew cItemSoup(26, "mushroomStew", "mushroom_stew", 6);
	swordGold = LordNew cItemSword(27, "swordGold", "gold_sword", ToolMaterial::GOLD);
	shovelGold = LordNew cItemSpade(28, "shovelGold", "gold_shovel", ToolMaterial::GOLD);
	pickaxeGold = LordNew cItemPickaxe(29, "pickaxeGold", "gold_pickaxe", ToolMaterial::GOLD);
	axeGold = LordNew cItemAxe(30, "hatchetGold", "gold_axe", ToolMaterial::GOLD);
	silk	= LordNew cItemReed(31, "string", "string", BLOCK_ID_TRIP_WIRE); 
	feather = LordNew ItemClient(32, "feather", "feather");
	gunpowder = LordNew ItemClient(33, "sulphur", "gunpowder"); gunpowder->setPotionEffect(PotionHelper::gunpowderEffect);
	hoeWood = LordNew cItemHoe(34, "hoeWood", "wood_hoe", ToolMaterial::WOOD);
	hoeStone = LordNew cItemHoe(35, "hoeStone", "stone_hoe", ToolMaterial::STONE);
	hoeIron = LordNew cItemHoe(36, "hoeIron", "iron_hoe", ToolMaterial::IRON);
	hoeDiamond = LordNew cItemHoe(37, "hoeDiamond", "diamond_hoe", ToolMaterial::EMERALD);
	hoeGold = LordNew cItemHoe(38, "hoeGold", "gold_hoe", ToolMaterial::GOLD);
	seeds = LordNew cItemSeeds(39, "seeds", "seeds_wheat", BLOCK_ID_CROPS, BLOCK_ID_TILLED_FIELD);
	wheat = LordNew ItemClient(40, "wheat", "wheat");
	bread = LordNew cItemFood(41, "bread", "bread", 5, 0.6F, false);
	helmetLeather = LordNew cItemArmor(42, "helmetCloth", "leather_helmet", ArmorMaterial::CLOTH, 0, 0);
	plateLeather = LordNew cItemArmor(43, "chestplateCloth", "leather_chestplate", ArmorMaterial::CLOTH, 0, 1);
	legsLeather = LordNew cItemArmor(44, "leggingsCloth", "leather_leggings", ArmorMaterial::CLOTH, 0, 2);
	bootsLeather = LordNew cItemArmor(45, "bootsCloth", "leather_boots", ArmorMaterial::CLOTH, 0, 3);
	helmetChain = LordNew cItemArmor(46, "helmetChain", "chainmail_helmet", ArmorMaterial::CHAIN, 1, 0);
	plateChain = LordNew cItemArmor(47, "chestplateChain", "chainmail_chestplate", ArmorMaterial::CHAIN, 1, 1);
	legsChain = LordNew cItemArmor(48, "leggingsChain", "chainmail_leggings", ArmorMaterial::CHAIN, 1, 2);
	bootsChain = LordNew cItemArmor(49, "bootsChain", "chainmail_boots", ArmorMaterial::CHAIN, 1, 3);
	helmetIron = LordNew cItemArmor(50, "helmetIron", "iron_helmet", ArmorMaterial::IRON, 2, 0);
	plateIron = LordNew cItemArmor(51, "chestplateIron", "iron_chestplate", ArmorMaterial::IRON, 2, 1);
	legsIron = LordNew cItemArmor(52, "leggingsIron", "iron_leggings", ArmorMaterial::IRON, 2, 2);
	bootsIron = LordNew cItemArmor(53, "bootsIron", "iron_boots", ArmorMaterial::IRON, 2, 3);
	helmetDiamond = LordNew cItemArmor(54, "helmetDiamond", "diamond_helmet", ArmorMaterial::DIAMOND, 3, 0);
	plateDiamond = LordNew cItemArmor(55, "chestplateDiamond", "diamond_chestplate", ArmorMaterial::DIAMOND, 3, 1);
	legsDiamond = LordNew cItemArmor(56, "leggingsDiamond", "diamond_leggings", ArmorMaterial::DIAMOND, 3, 2);
	bootsDiamond = LordNew cItemArmor(57, "bootsDiamond", "diamond_boots", ArmorMaterial::DIAMOND, 3, 3);
	helmetGold = LordNew cItemArmor(58, "helmetGold", "gold_helmet", ArmorMaterial::GOLD, 4, 0);
	plateGold = LordNew cItemArmor(59, "chestplateGold", "gold_chestplate", ArmorMaterial::GOLD, 4, 1);
	legsGold = LordNew cItemArmor(60, "leggingsGold", "gold_leggings", ArmorMaterial::GOLD, 4, 2);
	bootsGold = LordNew cItemArmor(61, "bootsGold", "gold_boots", ArmorMaterial::GOLD, 4, 3);
	flint = LordNew ItemClient(62, "flint", "flint");
	porkRaw = LordNew cItemFood(63, "porkchopRaw", "porkchop_raw", 3, 0.3F, true);
	porkCooked = LordNew cItemFood(64, "porkchopCooked", "porkchop_cooked", 8, 0.8F, true);
	painting = LordNew cItemHangingEntity(65, "painting", "painting", 1);
	appleGold = LordNew cItemAppleGold(66, "appleGold", "apple_golden", 4, 1.2F, false); ((ItemFood*)appleGold)->setAlwaysEdible(); ((ItemFood*)appleGold)->setPotionEffect(Potion::regeneration->getId(), 5, 1, 1.0F);
	sign = LordNew cItemSign(67, "sign", "sign");
	doorWood = LordNew cItemDoor(68, "doorWood", "door_wood", BM_Material::BM_MAT_wood);
	bucketEmpty = LordNew cItemBucket(69, "bucket", "bucket_empty", 0); bucketEmpty->setMaxStackSize(16);
	bucketWater = LordNew cItemBucket(70, "bucketWater", "bucket_water", BLOCK_ID_WATERMOVING); bucketWater->setContainerItem(bucketEmpty);
	bucketLava = LordNew cItemBucket(71, "bucketLava", "bucket_lava", BLOCK_ID_LAVAMOVING); bucketLava->setContainerItem(bucketEmpty);
	blockmanEmpty = LordNew cItemBlockman(72, "blockman", "minecart_normal", 0);
	saddle = LordNew cItemSaddle(73, "saddle", "saddle");
	doorIron = LordNew cItemDoor(74, "doorIron", "door_iron", BM_Material::BM_MAT_iron);
	redstone = LordNew cItemRedstone(75, "redstone", "redstone_dust"); redstone->setPotionEffect(PotionHelper::redstoneEffect);
	snowball = LordNew cItemSnowball(76, "snowball", "snowball");
	boat = LordNew cItemBoat(77, "boat", "boat");
	leather = LordNew ItemClient(78, "leather", "leather");
	bucketMilk = LordNew cItemBucketMilk(79, "milk", "bucket_milk"); bucketMilk->setContainerItem(bucketEmpty);
	brick = LordNew ItemClient(80, "brick", "brick");
	clay = LordNew ItemClient(81, "clay", "clay_ball");
	reed			= LordNew cItemReed(82, "reeds", "reeds", BLOCK_ID_REED);
	paper = LordNew ItemClient(83, "paper", "paper");
	book = LordNew cItemBook(84, "book", "book_normal");
	slimeBall = LordNew ItemClient(85, "slimeball", "slimeball");
	blockmanCrate = LordNew cItemBlockman(86, "blockmanChest", "minecart_chest", 1);
	blockmanPowered = LordNew cItemBlockman(87, "blockmanFurnace", "minecart_furnace", 2);
	egg = LordNew cItemEgg(88, "egg", "egg");
	compass = LordNew cItemCompass(89, "compass", "compass");
	fishingRod = LordNew cItemFishingRod(90, "fishingRod", "fishing_rod");
	pocketSundial = LordNew cItemClock(91, "clock", "clock");
	glowstone = LordNew ItemClient(92, "yellowDust", "glowstone_dust"); glowstone->setPotionEffect(PotionHelper::glowstoneEffect);
	fishRaw = LordNew cItemFood(93, "fishRaw", "fish_raw", 2, 0.3F, false);
	fishCooked = LordNew cItemFood(94, "fishCooked", "fish_cooked", 5, 0.6F, false);
	dyePowder = LordNew cItemDye(95, "dyePowder", "dye_powder");
	bone = LordNew ItemClient(96, "bone", "bone");
	sugar = LordNew ItemClient(97, "sugar", "sugar"); sugar->setPotionEffect(PotionHelper::sugarEffect);
	cake			= LordNew cItemReed(98, "cake", "cake", BLOCK_ID_CAKE); cake->setMaxStackSize(1);
	bed = LordNew cItemBed(99, "bed", "bed"); bed->setMaxStackSize(1);
	redstoneRepeater = LordNew cItemReed(100, "diode", "repeater", BLOCK_ID_REDSTONE_REPEATER_IDLE);
	cookie = LordNew cItemFood(101, "cookie", "cookie", 2, 0.1F, false);
	//imap			= (ItemMap)(LordNew ItemMap(102)).setUnlocalizedName("map").setAtlasName("map_filled");
	shears = LordNew cItemShears(103, "shears", "shears");
	melon = LordNew cItemFood(104, "melon", "melon", 2, 0.3F, false);
	pumpkinSeeds = LordNew cItemSeeds(105, "seeds_pumpkin", "seeds_pumpkin", BLOCK_ID_PUMPKIN_STEM, BLOCK_ID_TILLED_FIELD);
	melonSeeds = LordNew cItemSeeds(106, "seeds_melon", "seeds_melon", BLOCK_ID_MELON_STEM, BLOCK_ID_TILLED_FIELD);
	beefRaw = LordNew cItemFood(107, "beefRaw", "beef_raw", 3, 0.3F, true);
	beefCooked = LordNew cItemFood(108, "beefCooked", "beef_cooked", 8, 0.8F, true);
	chickenRaw = LordNew cItemFood(109, "chickenRaw", "chicken_raw", 2, 0.3F, true); ((ItemFood*)chickenRaw)->setPotionEffect(Potion::hunger->getId(), 30, 0, 0.3F);
	chickenCooked = LordNew cItemFood(110, "chickenCooked", "chicken_cooked", 6, 0.6F, true);
	rottenFlesh = LordNew cItemFood(111, "rottenFlesh", "rotten_flesh", 4, 0.1F, true); ((ItemFood*)rottenFlesh)->setPotionEffect(Potion::hunger->getId(), 30, 0, 0.8F);
	enderPearl = LordNew ItemClient(112, "enderPearl", "ender_pearl");
	blazeRod = LordNew ItemClient(113, "blazeRod", "blaze_rod");
	ghastTear = LordNew ItemClient(114, "ghastTear", "ghast_tear"); ghastTear->setPotionEffect(PotionHelper::ghastTearEffect);
	goldNugget = LordNew ItemClient(115, "goldNugget", "gold_nugget");
	netherStalkSeeds = LordNew cItemSeeds(116, "netherStalkSeeds", "nether_wart", BLOCK_ID_NETHER_STALK, BLOCK_ID_SLOW_SAND); netherStalkSeeds->setPotionEffect("+4");
	potion = LordNew cItemPotion(117, "potion", "potion");
	glassBottle = LordNew cItemGlassBottle(118, "glassBottle", "potion_bottle_empty");
	spiderEye = LordNew cItemFood(119, "spiderEye", "spider_eye", 2, 0.8F, false); ((ItemFood*)spiderEye)->setPotionEffect(Potion::poison->getId(), 5, 0, 1.0F); spiderEye->setPotionEffect(PotionHelper::spiderEyeEffect);
	fermentedSpiderEye = LordNew ItemClient(120, "fermentedSpiderEye", "spider_eye_fermented"); fermentedSpiderEye->setPotionEffect(PotionHelper::fermentedSpiderEyeEffect);
	blazePowder = LordNew ItemClient(121, "blazePowder", "blaze_powder"); blazePowder->setPotionEffect(PotionHelper::blazePowderEffect);
	magmaCream = LordNew ItemClient(122, "magmaCream", "magma_cream"); magmaCream->setPotionEffect(PotionHelper::magmaCreamEffect);
	brewingStand	= LordNew cItemReed(123, "brewingStand", "brewing_stand", BLOCK_ID_BREWING_STAND); 
	cauldron		= LordNew cItemReed(124, "cauldron", "cauldron", BLOCK_ID_CAULDRON);
	eyeOfEnder = LordNew cItemEnderEye(125, "eyeOfEnder", "ender_eye");
	speckledMelon = LordNew ItemClient(126, "speckledMelon", "melon_speckled"); speckledMelon->setPotionEffect(PotionHelper::speckledMelonEffect);
	monsterPlacer = LordNew cItemMonsterPlacer(127, "monsterPlacer", "spawn_egg");
	expBottle = LordNew cItemExpBottle(128, "expBottle", "experience_bottle");
	fireballCharge = LordNew cItemFireball(129, "fireball", "fireball");
	writableBook = LordNew cItemWritableBook(130, "writingBook", "book_writable");
	writtenBook = LordNew cItemEditableBook(131, "writtenBook", "book_written");
	emerald = LordNew ItemClient(132, "emerald", "emerald");
	itemFrame = LordNew cItemHangingEntity(133, "frame", "item_frame", 2);
	flowerPot		= LordNew cItemReed(134, "flowerPot", "flower_pot", BLOCK_ID_FLOWER_POT); 
	carrot = LordNew cItemSeedFood(135, "carrots", "carrot", 4, 0.6F, BLOCK_ID_CARROT, BLOCK_ID_TILLED_FIELD);
	potato = LordNew cItemSeedFood(136, "potato", "potato", 1, 0.3F, BLOCK_ID_POTATO, BLOCK_ID_TILLED_FIELD);
	bakedPotato = LordNew cItemFood(137, "potatoBaked", "potato_baked", 6, 0.6F, false);
	poisonousPotato = LordNew cItemFood(138, "potatoPoisonous", "potato_poisonous", 2, 0.3F, false); ((ItemFood*)poisonousPotato)->setPotionEffect(Potion::poison->getId(), 5, 0, 0.6F);
	//emptyMap		= LordNew ItemEmptyMap(139)).setUnlocalizedName("emptyMap").setAtlasName("map_empty");
	goldenCarrot = LordNew cItemFood(140, "carrotGolden", "carrot_golden", 6, 1.2F, false); goldenCarrot->setPotionEffect(PotionHelper::goldenCarrotEffect);
	skull = LordNew cItemSkull(141, "skull", "skull");
	carrotOnAStick = LordNew cItemCarrotOnAStick(142, "carrotOnAStick", "carrot_on_a_stick");
	netherStar = LordNew cItemSimpleFoiled(143, "netherStar", "nether_star");
	pumpkinPie = LordNew cItemFood(144, "pumpkinPie", "pumpkin_pie", 8, 0.3F, false);
	firework = LordNew cItemFirework(145, "fireworks", "fireworks");
	fireworkCharge = LordNew cItemFireworkCharge(146, "fireworksCharge", "fireworks_charge");
	enchantedBook = LordNew cItemEnchantedBook(147, "enchantedBook", "book_enchanted"); enchantedBook->setMaxStackSize(1);
	comparator	= LordNew cItemReed(148, "comparator", "comparator", BLOCK_ID_REDSTONE_COMPARATOR_IDLE);
	netherrackBrick = LordNew ItemClient(149, "netherbrick", "netherbrick");
	netherQuartz = LordNew ItemClient(150, "netherquartz", "quartz");
	blockmanTnt = LordNew cItemBlockman(151, "blockmanTnt", "minecart_tnt", 3);
	blockmanHopper= LordNew cItemBlockman(152,"blockmanHopper","minecart_hopper", 5);
	rabbitCooked = LordNew cItemFood(156, "rabbitCooked", "rabbit_cooked", 5, 1.2F, true);
	iron_horse_armor = LordNew ItemClient(161, "horsearmormetal", "iron_horse_armor"); iron_horse_armor->setMaxStackSize(1);
	golden_horse_armor = LordNew ItemClient(162, "horsearmorgold", "gold_horse_armor"); golden_horse_armor->setMaxStackSize(1);
	diamond_horse_armor = LordNew ItemClient(163, "horsearmordiamond", "diamond_horse_armor"); diamond_horse_armor->setMaxStackSize(1);
	lead = LordNew cItemLeash(164, "leash", "lead");
	name_tag = LordNew cItemNameTag(165, "nameTag", "name_tag");
	arrowPoison = LordNew cItemArrowPotion(166, "arrow_poison", "arrow_poison"); arrowPoison->setPotionParams(PotionManager::effectPoison->getPotionID(), int(Math::Floor(PotionManager::effectPoison->getDuration() / 8.f)), PotionManager::effectPoison->getAmplifier());
	arrowLongPoison = LordNew cItemArrowPotion(167, "arrow_long_poison", "arrow_poison"); arrowLongPoison->setPotionParams(PotionManager::effectLongPoison->getPotionID(), int(Math::Floor(PotionManager::effectLongPoison->getDuration() / 8.f)), PotionManager::effectLongPoison->getAmplifier());
	arrowStrongPoison = LordNew cItemArrowPotion(168, "arrow_strong_poison", "arrow_poison"); arrowStrongPoison->setPotionParams(PotionManager::effectStrongPoison->getPotionID(), int(Math::Floor(PotionManager::effectStrongPoison->getDuration() / 8.f)), PotionManager::effectStrongPoison->getAmplifier());
	grenade = LordNew cItemGrenade(169, "grenade", "grenade");
	gun_01 = LordNew cItemGun(170, 1, "gun_01", "apple_golden", "gun_01.mesh");
	gun_02 = LordNew cItemGun(171, 2, "gun_02", "apple_golden", "gun_02.mesh");
	gun_03 = LordNew cItemGun(172, 3, "gun_05", "apple_golden", "gun_03.mesh");
	gun_04 = LordNew cItemGun(173, 4, "gun_04", "apple_golden", "gun_04.mesh");
	gun_05 = LordNew cItemGun(174, 5, "gun_06", "apple_golden", "gun_03.mesh");
	gun_06 = LordNew cItemGun(175, 6, "gun_09", "apple_golden", "gun_09.mesh");
	gun_07 = LordNew cItemGun(176, 7, "gun_08", "apple_golden", "gun_08.mesh");
	gun_08 = LordNew cItemGun(177, 8, "gun_11", "apple_golden", "gun_11.mesh");
	gun_09 = LordNew cItemGun(178, 9, "gun_10", "apple_golden", "gun_10.mesh");
	gun_10 = LordNew cItemGun(179, 10, "gun_37", "apple_golden", "gun_37.mesh");
	gun_11 = LordNew cItemGun(180, 11, "gun_27", "apple_golden", "gun_27.mesh");
	gun_12 = LordNew cItemGun(181, 12, "gun_36", "apple_golden", "gun_36.mesh");
	gun_13 = LordNew cItemGun(182, 13, "gun_38", "apple_golden", "gun_38.mesh");
	gun_14 = LordNew cItemGun(183, 14, "gun_25", "apple_golden", "gun_25.mesh");
	gun_15 = LordNew cItemGun(184, 15, "gun_29", "apple_golden", "gun_29.mesh");
	bulletClip_01 = LordNew cItemBulletClip(185, 1, "bulletClip_01", "apple_golden");
	bulletClip_02 = LordNew cItemBulletClip(186, 2, "bulletClip_02", "apple_golden");
	bulletClip_03 = LordNew cItemBulletClip(187, 3, "bulletClip_03", "apple_golden");
	bulletClip_04 = LordNew cItemBulletClip(188, 4, "bulletClip_04", "apple_golden");
	bulletClip_05 = LordNew cItemBulletClip(189, 5, "bulletClip_05", "apple_golden");
	bulletClip_06 = LordNew cItemBulletClip(190, 6, "bulletClip_06", "apple_golden");
	bulletClip_07 = LordNew cItemBulletClip(191, 7, "bulletClip_07", "apple_golden");
	bulletClip_08 = LordNew cItemBulletClip(192, 8, "bulletClip_08", "apple_golden");
	bulletClip_09 = LordNew cItemBulletClip(193, 9, "bulletClip_09", "apple_golden");
	bulletClip_10 = LordNew cItemBulletClip(194, 10, "bulletClip_10", "apple_golden");
	handcuffs = LordNew cItemHandcuffs(195, "handcuffs", "handcuffs");
	key_01 = LordNew cItemKey(196, "key01", "keyToDoor");
	key_02 = LordNew cItemKey(197, "key02", "keyToDoor");
	key_03 = LordNew cItemKey(198, "key03", "keyToDoor");
	key_04 = LordNew cItemKey(199, "key04", "keyToDoor");
	key_05 = LordNew cItemKey(200, "key05", "keyToDoor");
	key_06 = LordNew cItemKey(201, "key06", "keyToDoor");
	key_07 = LordNew cItemKey(202, "key07", "keyToDoor");
	key_08 = LordNew cItemKey(203, "key08", "keyToDoor");
	key_09 = LordNew cItemKey(204, "key09", "keyToDoor");
	key_10 = LordNew cItemKey(205, "key10", "keyToDoor");
	tpScroll_01 = LordNew cItemTeleportScroll(206, "tpScroll01", "tpScroll01");
	tpScroll_02 = LordNew cItemTeleportScroll(207, "tpScroll02", "tpScroll02");
	tpScroll_03 = LordNew cItemTeleportScroll(208, "tpScroll03", "tpScroll02");
	tpScroll_04 = LordNew cItemTeleportScroll(209, "tpScroll04", "tpScroll02");
	tpScroll_05 = LordNew cItemTeleportScroll(210, "tpScroll05", "tpScroll02");
	createBridge_01 = LordNew cItemCreateBridge(211, "createBridge01", "bridgeEgg01");
	createBridge_02 = LordNew cItemCreateBridge(212, "createBridge02", "bridgeEgg01");
	createBridge_03 = LordNew cItemCreateBridge(213, "createBridge03", "bridgeEgg01");
	createBridge_04 = LordNew cItemCreateBridge(214, "createBridge04", "bridgeEgg01");
	createBridge_05 = LordNew cItemCreateBridge(215, "createBridge05", "bridgeEgg01");
	shovelRenovation = LordNew cItemToolDigger(216, "shovelRenovation", "shovel_01", "gun_2000.mesh", ToolMaterial::IRON);
	spatula = LordNew cItemToolDigger(217, "spatula", "shovel_02", "gun_2001.mesh", ToolMaterial::IRON);
	shovelToy = LordNew cItemToolDigger(218, "shovelToy", "shovel_03", "gun_2002.mesh", ToolMaterial::IRON);
	digBucket = LordNew cItemToolDigger(219, "digBucket", "shovel_04", "gun_2003.mesh", ToolMaterial::IRON);
	digShovelIron = LordNew cItemToolDigger(220, "digShovelIron", "shovel_06", "gun_2005.mesh", ToolMaterial::IRON);
	shovelRake = LordNew cItemToolDigger(221, "shovelRake", "shovel_05", "gun_2004.mesh", ToolMaterial::IRON);
	shovelMarching = LordNew cItemToolDigger(222, "shovelMarching", "shovel_08", "gun_2007.mesh", ToolMaterial::IRON);
	shovelBigScooper = LordNew cItemToolDigger(223, "shovelBigScooper", "shovel_07", "gun_2006.mesh", ToolMaterial::IRON);
	shovelJackHammer = LordNew cItemToolDigger(224, "shovelJackHammer", "shovel_10", "gun_2009.mesh", ToolMaterial::IRON);
	shovelDrill = LordNew cItemToolDigger(225, "shovelDrill", "shovel_11", "gun_2010.mesh", ToolMaterial::IRON);
	shovelEngineerGold = LordNew cItemToolDigger(226, "shovelEngineerGold", "shovel_09", "gun_2008.mesh", ToolMaterial::IRON);
	medichinePack = LordNew cItemMedicine(227, "medichinePack", "medichine_pack");
	medichinePotion = LordNew cItemMedicine(228, "medichinePotion", "potion_bottle_fill");

	record13 = LordNew cItemRecord(230, "record", "13", "record_13");
	recordCat = LordNew cItemRecord(231, "record", "cat", "record_cat");
	recordBlocks = LordNew cItemRecord(232, "record", "blocks", "record_blocks");
	recordChirp = LordNew cItemRecord(233, "record", "chirp", "record_chirp");
	recordFar = LordNew cItemRecord(234, "record", "far", "record_far");
	recordMall = LordNew cItemRecord(235, "record", "mall", "record_mall");
	recordMellohi = LordNew cItemRecord(236, "record", "mellohi", "record_mellohi");
	recordStal = LordNew cItemRecord(237, "record", "stal", "record_stal");
	recordStrad = LordNew cItemRecord(238, "record", "strad", "record_strad");
	recordWard = LordNew cItemRecord(239, "record", "ward", "record_ward");
	record11 = LordNew cItemRecord(240, "record", "11", "record_11");
	recordWait = LordNew cItemRecord(241, "record", "wait", "record_wait");

	//TODO The atlas is not, please add yourself if you need it
	goldHeart = LordNew cItemProp(242, "goldHeart","goldHeart", "heart01.mesh");
	goldShoes = LordNew cItemProp(243, "goldShoes", "goldShoes", "shoes01.mesh");
	goldArrow = LordNew cItemProp(244, "goldArrow", "goldArrow", "arrow01.mesh");
	questionMark = LordNew cItemProp(245, "questionMark", "questionMark", "questionmark01.mesh");

	propGold = LordNew cItemProp(246, "propGold", "propGold", StringUtil::BLANK);
	propExp = LordNew cItemProp(247, "propExp", "propExp", StringUtil::BLANK);

	gun_16 = LordNew cItemGun(444, 16, "gun_28", "apple_golden", "gun_28.mesh");
	gun_17 = LordNew cItemGun(445, 17, "gun_34", "apple_golden", "gun_34.mesh");
	gun_18 = LordNew cItemGun(446, 18, "gun_23", "apple_golden", "gun_23.mesh");
	gun_19 = LordNew cItemGun(447, 19, "gun_35", "apple_golden", "gun_35.mesh");
	gun_20 = LordNew cItemGun(448, 20, "gun_26", "apple_golden", "gun_26.mesh");
	gun_21 = LordNew cItemGun(449, 21, "gun_24", "apple_golden", "gun_24.mesh");
	gun_22 = LordNew cItemGun(450, 22, "gun_18", "apple_golden", "gun_18.mesh");
	gun_23 = LordNew cItemGun(451, 23, "gun_33", "apple_golden", "gun_33.mesh");
	gun_24 = LordNew cItemGun(452, 24, "gun_13", "apple_golden", "gun_13.mesh");
	gun_25 = LordNew cItemGun(453, 25, "gun_21", "apple_golden", "gun_21.mesh");
	gun_26 = LordNew cItemGun(454, 26, "gun_17", "apple_golden", "gun_17.mesh");
	gun_27 = LordNew cItemGun(455, 27, "gun_20", "apple_golden", "gun_20.mesh");
	gun_28 = LordNew cItemGun(456, 28, "gun_31", "apple_golden", "gun_31.mesh");
	gun_29 = LordNew cItemGun(457, 29, "gun_19", "apple_golden", "gun_19.mesh");
	gun_30 = LordNew cItemGun(458, 30, "gun_15", "apple_golden", "gun_15.mesh");



	tomato = LordNew cItemFood(246, "tomato", "tomato", 6, 0.6F, false);
	cabbage = LordNew cItemFood(247, "cabbage", "cabbage", 6, 0.6F, false);
	cabbage = LordNew cItemFood(248, "pea", "pea", 6, 0.6F, false);

	/*extend item from 700~1255, as 444~999*/
	//Dev_1 900~1199 as 644~943  //Dev_2 700~899 as 444~643 //Dev_100+  1200~1255 944~999
	seedsOfNewCrops_01 = LordNew cItemSeeds(644, "seeds_wheat", "seeds_wheat1", 1256, BLOCK_ID_TILLED_FIELD);
	seedsOfNewCrops_02 = LordNew cItemSeeds(645, "seeds_carrot", "seeds_carrot", 1257, BLOCK_ID_TILLED_FIELD);
	seedsOfNewCrops_03 = LordNew cItemSeeds(646, "seeds_tomato", "seeds_tomato", 1258, BLOCK_ID_TILLED_FIELD);
	seedsOfNewCrops_04 = LordNew cItemSeeds(647, "seeds_cabbage", "seeds_cabbage", 1259, BLOCK_ID_TILLED_FIELD);
	seedsOfNewCrops_05 = LordNew cItemSeeds(648, "seeds_pea", "seeds_pea", 1260, BLOCK_ID_TILLED_FIELD);
	gemstone = LordNew cItemProp(649, "gemstone", "gemstone", "jewel02.mesh");
	
	building_01 = LordNew cItemBuild(651, "building_01", "600000_building_01");
	building_02 = LordNew cItemBuild(652, "building_02", "600001_building_02");
	building_03 = LordNew cItemBuild(653, "building_03", "600002_building_03");
	building_04 = LordNew cItemBuild(654, "building_04", "600003_building_04");
	building_05 = LordNew cItemBuild(655, "building_05", "cabbage");
	building_06 = LordNew cItemBuild(656, "building_06", "cabbage");
	building_07 = LordNew cItemBuild(657, "building_07", "cabbage");
	building_08 = LordNew cItemBuild(658, "building_08", "cabbage");
	building_09 = LordNew cItemBuild(659, "building_09", "700001_building_09");
	building_10 = LordNew cItemBuild(660, "building_10", "700002_building_10");
	building_11 = LordNew cItemBuild(661, "building_11", "700003_building_11");
	building_12 = LordNew cItemBuild(662, "building_12", "cabbage");
	building_13 = LordNew cItemBuild(663, "building_13", "700005_building_13");
	building_14 = LordNew cItemBuild(664, "building_14", "700006_building_14");
	building_15 = LordNew cItemBuild(665, "building_15", "700007_building_15");
	building_16 = LordNew cItemBuild(666, "building_16", "700008_building_16");
	building_17 = LordNew cItemBuild(667, "building_17", "700009_building_17");
	building_18 = LordNew cItemBuild(668, "building_18", "700010_building_18");
	building_19 = LordNew cItemBuild(669, "building_19", "700011_building_19");
	building_20 = LordNew cItemBuild(670, "building_20", "700012_building_20");
	building_21 = LordNew cItemBuild(671, "building_21", "700013_building_21");
	building_22 = LordNew cItemBuild(672, "building_22", "700014_building_22");
	building_23 = LordNew cItemBuild(673, "building_23", "700015_building_23");
	building_24 = LordNew cItemBuild(674, "building_24", "cabbage");
	building_25 = LordNew cItemBuild(675, "building_25", "cabbage");
	building_26 = LordNew cItemBuild(676, "building_26", "cabbage");
	building_27 = LordNew cItemBuild(677, "building_27", "cabbage");
	building_28 = LordNew cItemBuild(678, "building_28", "cabbage");
	building_29 = LordNew cItemBuild(679, "building_29", "cabbage");
	building_30 = LordNew cItemBuild(680, "building_30", "cabbage");


	house_01 = LordNew cItemHouse(681, "house_01", "pea");
	house_02 = LordNew cItemHouse(682, "house_02", "pea");
	house_03 = LordNew cItemHouse(683, "house_03", "pea");
	house_04 = LordNew cItemHouse(684, "house_04", "pea");
	house_05 = LordNew cItemHouse(685, "house_05", "pea");
	house_06 = LordNew cItemHouse(686, "house_06", "pea");
	house_07 = LordNew cItemHouse(687, "house_07", "pea");
	house_08 = LordNew cItemHouse(688, "house_08", "pea");
	house_09 = LordNew cItemHouse(689, "house_09", "pea");
	house_10 = LordNew cItemHouse(690, "house_10", "pea");

	ranchSeed_100001 = LordNew cRanchSeeds(701, "500001_wheat_seed", "500001_wheat_seed", 1261, BLOCK_ID_TILLED_FIELD);
	ranchSeed_100002 = LordNew cRanchSeeds(702, "500002_corn_seed", "500002_corn_seed", 1262, BLOCK_ID_TILLED_FIELD);
	ranchSeed_100003 = LordNew cRanchSeeds(703, "500003_carrots_seed", "500003_carrots_seed", 1263, BLOCK_ID_TILLED_FIELD);
	ranchSeed_100004 = LordNew cRanchSeeds(704, "500004_sugar_cane_seed", "500004_sugar_cane_seed", 1264, BLOCK_ID_TILLED_FIELD);
	ranchSeed_100005 = LordNew cRanchSeeds(705, "500005_cotton_seed", "500005_cotton_seed", 1265, BLOCK_ID_TILLED_FIELD);
	ranchSeed_100006 = LordNew cRanchSeeds(706, "500006_strawberry_seed", "500006_strawberry_seed", 1266, BLOCK_ID_TILLED_FIELD);
	ranchSeed_100007 = LordNew cRanchSeeds(707, "500007_tomato_seed", "500007_tomato_seed", 1267, BLOCK_ID_TILLED_FIELD);
	ranchSeed_100008 = LordNew cRanchSeeds(708, "500008_pine_seed", "500008_pine_seed", 1268, BLOCK_ID_TILLED_FIELD);
	ranchSeed_100009 = LordNew cRanchSeeds(709, "500009_potatoes_seed", "500009_potatoes_seed", 1269, BLOCK_ID_TILLED_FIELD);
	ranchSeed_100010 = LordNew cRanchSeeds(710, "500010_cocoa_beans_seed", "500010_cocoa_beans_seed", 1270, BLOCK_ID_TILLED_FIELD);
	ranchSeed_100011 = LordNew cRanchSeeds(711, "500011_rubber_wood_seed", "500011_rubber_wood_seed", 1271, BLOCK_ID_TILLED_FIELD);
	ranchSeed_100012 = LordNew cRanchSeeds(712, "500012_silk_seed", "500012_silk_seed", 1272, BLOCK_ID_TILLED_FIELD);
	ranchSeed_100013 = LordNew cRanchSeeds(713, "500013_chili_seed", "500013_chili_seed", 1273, BLOCK_ID_TILLED_FIELD);
	ranchSeed_100014 = LordNew cRanchSeeds(714, "500014_rice_seed", "500014_rice_seed", 1274, BLOCK_ID_TILLED_FIELD);
	ranchSeed_100015 = LordNew cRanchSeeds(715, "500015_the_rose_seed", "500015_the_rose_seed", 1275, BLOCK_ID_TILLED_FIELD);
	ranchSeed_100016 = LordNew cRanchSeeds(716, "500016_jasmine_seed", "500016_jasmine_seed", 1276, BLOCK_ID_TILLED_FIELD);
	ranchSeed_100017 = LordNew cRanchSeeds(717, "500017_coffee_beans_seed", "500017_coffee_beans_seed", 1277, BLOCK_ID_TILLED_FIELD);

	building_31 = LordNew cItemBuild(731, "building_31", "cabbage");
	building_32 = LordNew cItemBuild(732, "building_32", "cabbage");
	building_33 = LordNew cItemBuild(733, "building_33", "cabbage");
	building_34 = LordNew cItemBuild(734, "building_34", "cabbage");
	building_35 = LordNew cItemBuild(735, "building_35", "cabbage");
	building_36 = LordNew cItemBuild(736, "building_36", "cabbage");
	building_37 = LordNew cItemBuild(737, "building_37", "cabbage");
	building_38 = LordNew cItemBuild(738, "building_38", "800001_building_38");
	building_39 = LordNew cItemBuild(739, "building_39", "800002_building_39");
	building_40 = LordNew cItemBuild(740, "building_40", "800003_building_40");
	building_41 = LordNew cItemBuild(741, "building_41", "cabbage");
	building_42 = LordNew cItemBuild(742, "building_42", "800005_building_42");
	building_43 = LordNew cItemBuild(743, "building_43", "cabbage");
	building_44 = LordNew cItemBuild(744, "building_44", "800007_building_44");
	building_45 = LordNew cItemBuild(745, "building_45", "800008_building_45");
	building_46 = LordNew cItemBuild(746, "building_46", "cabbage");
	building_47 = LordNew cItemBuild(747, "building_47", "800010_building_47");
	building_48 = LordNew cItemBuild(748, "building_48", "cabbage");
	building_49 = LordNew cItemBuild(749, "building_49", "cabbage");
	building_50 = LordNew cItemBuild(750, "building_50", "cabbage");
	building_51 = LordNew cItemBuild(751, "building_51", "cabbage");
	building_52 = LordNew cItemBuild(752, "building_52", "cabbage");
	building_53 = LordNew cItemBuild(753, "building_53", "cabbage");
	building_54 = LordNew cItemBuild(754, "building_54", "cabbage");
	building_55 = LordNew cItemBuild(755, "building_55", "cabbage");
	building_56 = LordNew cItemBuild(756, "building_56", "cabbage");
	building_57 = LordNew cItemBuild(757, "building_57", "cabbage");
	building_58 = LordNew cItemBuild(758, "building_58", "cabbage");
	building_59 = LordNew cItemBuild(759, "building_59", "cabbage");
	building_60 = LordNew cItemBuild(760, "building_60", "cabbage");
	building_61 = LordNew cItemBuild(761, "building_61", "cabbage");
	building_62 = LordNew cItemBuild(762, "building_62", "cabbage");
	building_63 = LordNew cItemBuild(763, "building_63", "cabbage");
	building_64 = LordNew cItemBuild(764, "building_64", "cabbage");
	building_65 = LordNew cItemBuild(765, "building_65", "cabbage");
	building_66 = LordNew cItemBuild(766, "building_66", "cabbage");
	building_67 = LordNew cItemBuild(767, "building_67", "cabbage");
	building_68 = LordNew cItemBuild(768, "building_68", "cabbage");
	building_69 = LordNew cItemBuild(769, "building_69", "cabbage");
	building_70 = LordNew cItemBuild(770, "building_70", "cabbage");

	toolGather_01 = LordNew cItemToolGather(771, "toolGather_01", "weapon22", "weapon_22.mesh", ToolMaterial::IRON);
	toolGather_02 = LordNew cItemToolGather(772, "toolGather_02", "weapon23", "weapon_23.mesh", ToolMaterial::IRON);
	toolGather_03 = LordNew cItemToolGather(773, "toolGather_03", "weapon24", "weapon_24.mesh", ToolMaterial::IRON);
	toolGather_04 = LordNew cItemToolGather(774, "toolGather_04", "weapon25", "weapon_25.mesh", ToolMaterial::IRON);
	toolGather_05 = LordNew cItemToolGather(775, "toolGather_05", "weapon26", "weapon_26.mesh", ToolMaterial::IRON);
	toolGather_06 = LordNew cItemToolGather(776, "toolGather_06", "weapon27", "weapon_27.mesh", ToolMaterial::IRON);
	toolGather_07 = LordNew cItemToolGather(777, "toolGather_07", "weapon28", "weapon_28.mesh", ToolMaterial::IRON);
	toolGather_08 = LordNew cItemToolGather(778, "toolGather_08", "weapon29", "weapon_29.mesh", ToolMaterial::IRON);
	toolGather_09 = LordNew cItemToolGather(779, "toolGather_09", "weapon30", "weapon_30.mesh", ToolMaterial::IRON);
	toolGather_10 = LordNew cItemToolGather(780, "toolGather_10", "weapon31", "weapon_31.mesh", ToolMaterial::IRON);
	toolGather_11 = LordNew cItemToolGather(781, "toolGather_11", "weapon32", "weapon_32.mesh", ToolMaterial::IRON);
	toolGather_12 = LordNew cItemToolGather(782, "toolGather_12", "weapon33", "weapon_33.mesh", ToolMaterial::IRON);
	toolGather_13 = LordNew cItemToolGather(783, "toolGather_13", "weapon34", "weapon_34.mesh", ToolMaterial::IRON);
	toolGather_14 = LordNew cItemToolGather(784, "toolGather_14", "weapon35", "weapon_35.mesh", ToolMaterial::IRON);
	toolGather_15 = LordNew cItemToolGather(785, "toolGather_15", "weapon36", "weapon_36.mesh", ToolMaterial::IRON);
	toolGather_16 = LordNew cItemToolGather(786, "toolGather_16", "weapon37", "weapon_37.mesh", ToolMaterial::IRON);
	toolGather_17 = LordNew cItemToolGather(787, "toolGather_17", "weapon38", "weapon_38.mesh", ToolMaterial::IRON);
	toolGather_18 = LordNew cItemToolGather(788, "toolGather_18", "weapon39", "weapon_39.mesh", ToolMaterial::IRON);
	toolGather_19 = LordNew cItemToolGather(789, "toolGather_19", "weapon40", "weapon_40.mesh", ToolMaterial::IRON);
	toolGather_20 = LordNew cItemToolGather(790, "toolGather_21", "weapon29", "weapon_29.mesh", ToolMaterial::IRON);
	toolGather_21 = LordNew cItemToolGather(791, "toolGather_21", "weapon29", "weapon_29.mesh", ToolMaterial::IRON);
	toolGather_22 = LordNew cItemToolGather(792, "toolGather_22", "weapon29", "weapon_29.mesh", ToolMaterial::IRON);
	toolGather_23 = LordNew cItemToolGather(793, "toolGather_23", "weapon29", "weapon_29.mesh", ToolMaterial::IRON);
	toolGather_24 = LordNew cItemToolGather(794, "toolGather_24", "weapon29", "weapon_29.mesh", ToolMaterial::IRON);
	toolGather_25 = LordNew cItemToolGather(795, "toolGather_25", "weapon29", "weapon_29.mesh", ToolMaterial::IRON);
	toolGather_26 = LordNew cItemToolGather(796, "toolGather_26", "weapon29", "weapon_29.mesh", ToolMaterial::IRON);
	toolGather_27 = LordNew cItemToolGather(797, "toolGather_27", "weapon29", "weapon_29.mesh", ToolMaterial::IRON);
	toolGather_28 = LordNew cItemToolGather(798, "toolGather_28", "weapon29", "weapon_29.mesh", ToolMaterial::IRON);
	toolGather_29 = LordNew cItemToolGather(799, "toolGather_29", "weapon29", "weapon_29.mesh", ToolMaterial::IRON);
	toolGather_30 = LordNew cItemToolGather(800, "toolGather_30", "weapon29", "weapon_29.mesh", ToolMaterial::IRON);

	fruitCurrency_01 = LordNew cItemFruitCurrency(801, "fruitCurrency_01", "fruitCurrency_01", "g1041_coin_01.mesh");
	fruitCurrency_02 = LordNew cItemFruitCurrency(802, "fruitCurrency_02", "fruitCurrency_02", "g1041_coin_02.mesh");
	fruitCurrency_03 = LordNew cItemFruitCurrency(803, "fruitCurrency_03", "fruitCurrency_03", "g1041_coin_03.mesh");
	fruitCurrency_04 = LordNew cItemFruitCurrency(804, "fruitCurrency_04", "fruitCurrency_04", "g1041_coin_04.mesh");
	fruitCurrency_05 = LordNew cItemFruitCurrency(805, "fruitCurrency_05", "fruitCurrency_05", "g1041_coin_05.mesh");
	fruitCurrency_06 = LordNew cItemFruitCurrency(806, "fruitCurrency_06", "fruitCurrency_06", "g1041_coin_06.mesh");
	fruitCurrency_07 = LordNew cItemFruitCurrency(807, "fruitCurrency_07", "fruitCurrency_07", "g1041_coin_07.mesh");
	fruitCurrency_08 = LordNew cItemFruitCurrency(808, "fruitCurrency_08", "fruitCurrency_08", "g1041_coin_08.mesh");
	fruitCurrency_09 = LordNew cItemFruitCurrency(809, "fruitCurrency_09", "fruitCurrency_09", "g1041_coin_09.mesh");
	fruitCurrency_10 = LordNew cItemFruitCurrency(810, "fruitCurrency_10", "fruitCurrency_10", "g1041_coin_10.mesh");
	fruitCurrency_11 = LordNew cItemFruitCurrency(811, "fruitCurrency_11", "fruitCurrency_11", "g1041_coin_11.mesh");
	fruitCurrency_12 = LordNew cItemFruitCurrency(812, "fruitCurrency_12", "fruitCurrency_12", "g1041_coin_12.mesh");
	fruitCurrency_13 = LordNew cItemFruitCurrency(813, "fruitCurrency_13", "fruitCurrency_13", "g1041_coin_13.mesh");
	fruitCurrency_14 = LordNew cItemFruitCurrency(814, "fruitCurrency_14", "fruitCurrency_14", "g1041_coin_14.mesh");
	fruitCurrency_15 = LordNew cItemFruitCurrency(815, "fruitCurrency_15", "fruitCurrency_15", "g1041_coin_15.mesh");
	fruitCurrency_16 = LordNew cItemFruitCurrency(816, "fruitCurrency_16", "fruitCurrency_16", "g1041_coin_16.mesh");
	fruitCurrency_17 = LordNew cItemFruitCurrency(817, "fruitCurrency_17", "fruitCurrency_17", "g1041_coin_17.mesh");
	fruitCurrency_18 = LordNew cItemFruitCurrency(818, "fruitCurrency_18", "fruitCurrency_18", "g1041_coin_18.mesh");
	fruitCurrency_19 = LordNew cItemFruitCurrency(819, "fruitCurrency_19", "fruitCurrency_19", "g1041_coin_19.mesh");
	fruitCurrency_20 = LordNew cItemFruitCurrency(820, "fruitCurrency_20", "fruitCurrency_20", "g1041_coin_20.mesh");
	fruitCurrency_21 = LordNew cItemFruitCurrency(821, "fruitCurrency_21", "fruitCurrency_21", "g1041_coin_21.mesh");
	fruitCurrency_22 = LordNew cItemFruitCurrency(822, "fruitCurrency_22", "fruitCurrency_22", "g1041_coin_22.mesh");
	fruitCurrency_23 = LordNew cItemFruitCurrency(823, "fruitCurrency_23", "fruitCurrency_23", "g1041_coin_23.mesh");
	fruitCurrency_24 = LordNew cItemFruitCurrency(824, "fruitCurrency_24", "fruitCurrency_24", "g1041_coin_24.mesh");
	fruitCurrency_25 = LordNew cItemFruitCurrency(825, "fruitCurrency_25", "fruitCurrency_25", "g1041_coin_25.mesh");
	fruitCurrency_26 = LordNew cItemFruitCurrency(826, "fruitCurrency_26", "fruitCurrency_26", "g1041_coin_26.mesh");
	fruitCurrency_27 = LordNew cItemFruitCurrency(827, "fruitCurrency_27", "fruitCurrency_27", "g1041_coin_27.mesh");
	fruitCurrency_28 = LordNew cItemFruitCurrency(828, "fruitCurrency_28", "fruitCurrency_28", "g1041_coin_28.mesh");
	fruitCurrency_29 = LordNew cItemFruitCurrency(829, "fruitCurrency_29", "fruitCurrency_29", "g1041_coin_29.mesh");
	fruitCurrency_30 = LordNew cItemFruitCurrency(830, "fruitCurrency_30", "fruitCurrency_30", "g1041_coin_30.mesh");
	fruitCurrency_31 = LordNew cItemFruitCurrency(831, "fruitCurrency_31", "fruitCurrency_31", "g1041_coin_31.mesh");
	fruitCurrency_32 = LordNew cItemFruitCurrency(832, "fruitCurrency_32", "fruitCurrency_32", "g1041_coin_32.mesh");
	fruitCurrency_33 = LordNew cItemFruitCurrency(833, "fruitCurrency_33", "fruitCurrency_33", "g1041_coin_33.mesh");
	fruitCurrency_34 = LordNew cItemFruitCurrency(834, "fruitCurrency_34", "fruitCurrency_34", "g1041_coin_34.mesh");
	fruitCurrency_35 = LordNew cItemFruitCurrency(835, "fruitCurrency_35", "fruitCurrency_35", "g1041_coin_35.mesh");
	fruitCurrency_36 = LordNew cItemFruitCurrency(836, "fruitCurrency_36", "fruitCurrency_36", "g1041_coin_36.mesh");
	fruitCurrency_37 = LordNew cItemFruitCurrency(837, "fruitCurrency_37", "fruitCurrency_37", "g1041_coin_37.mesh");
	fruitCurrency_38 = LordNew cItemFruitCurrency(838, "fruitCurrency_38", "fruitCurrency_38", "g1041_coin_38.mesh");
	fruitCurrency_39 = LordNew cItemFruitCurrency(839, "fruitCurrency_39", "fruitCurrency_39", "g1041_coin_39.mesh");
	fruitCurrency_40 = LordNew cItemFruitCurrency(840, "fruitCurrency_40", "fruitCurrency_40", "g1041_coin_40.mesh");
	fruitCurrency_41 = LordNew cItemFruitCurrency(841, "fruitCurrency_41", "fruitCurrency_41", "g1041_coin_41.mesh");
	fruitCurrency_42 = LordNew cItemFruitCurrency(842, "fruitCurrency_42", "fruitCurrency_42", "g1041_coin_42.mesh");
	fruitCurrency_43 = LordNew cItemFruitCurrency(843, "fruitCurrency_43", "fruitCurrency_43", "g1041_coin_43.mesh");
	fruitCurrency_44 = LordNew cItemFruitCurrency(844, "fruitCurrency_44", "fruitCurrency_44", "g1041_coin_44.mesh");
	fruitCurrency_45 = LordNew cItemFruitCurrency(845, "fruitCurrency_45", "fruitCurrency_45", "g1041_coin_45.mesh");
	fruitCurrency_46 = LordNew cItemFruitCurrency(846, "fruitCurrency_46", "fruitCurrency_46", "g1041_coin_46.mesh");
	fruitCurrency_47 = LordNew cItemFruitCurrency(847, "fruitCurrency_47", "fruitCurrency_47", "g1041_coin_47.mesh");
	fruitCurrency_48 = LordNew cItemFruitCurrency(848, "fruitCurrency_48", "fruitCurrency_48", "g1041_coin_48.mesh");
	fruitCurrency_49 = LordNew cItemFruitCurrency(849, "fruitCurrency_49", "fruitCurrency_49", "g1041_coin_49.mesh");
	fruitCurrency_50 = LordNew cItemFruitCurrency(850, "fruitCurrency_50", "fruitCurrency_50", "g1041_coin_50.mesh");

	//throwing skill
	itemThrowableSkillProp_01 = LordNew cItemSkillProp(459, "skill_01", "skill_01", "skill_01.mesh");
	itemThrowableSkillProp_02 = LordNew cItemSkillProp(460, "skill_02", "skill_02", "skill_02.mesh");
	itemThrowableSkillProp_03 = LordNew cItemSkillProp(461, "skill_03", "skill_03", "skill_03.mesh");
	//Reserve throwing skill Id, if necessary, please reset the atlas and so on.
	itemThrowableSkillProp_04 = LordNew cItemSkillProp(462, "skill_04", "skill_04", "skill_04.mesh");
	itemThrowableSkillProp_05 = LordNew cItemSkillProp(463, "skill_05", "skill_05", "skill_05.mesh");
	itemThrowableSkillProp_06 = LordNew cItemSkillProp(464, "skill_06", "skill_06", "skill_06.mesh");
	itemThrowableSkillProp_07 = LordNew cItemSkillProp(465, "skill_07", "skill_07", "skill_07.mesh");
	itemThrowableSkillProp_08 = LordNew cItemSkillProp(466, "skill_08", "skill_08", "skill_08.mesh");
	itemThrowableSkillProp_09 = LordNew cItemSkillProp(467, "skill_09", "skill_09", "skill_09.mesh");
	itemThrowableSkillProp_10 = LordNew cItemSkillProp(468, "skill_03", "skill_03", "skill_03.mesh");

	// eat skill 
	itemEatSkillProp_01 = LordNew cItemSkillProp(469, "skill_01", "skill_01", "skill_01.mesh");
	itemEatSkillProp_02 = LordNew cItemSkillProp(470, "skill_02", "skill_02", "skill_02.mesh");
	itemEatSkillProp_03 = LordNew cItemSkillProp(471, "skill_03", "skill_03", "skill_03.mesh");
	//Reserve eat skill Id, if necessary, please reset the atlas and so on.
	itemEatSkillProp_04 = LordNew cItemSkillProp(472, "skill_01", "skill_01", "skill_01.mesh");
	itemEatSkillProp_05 = LordNew cItemSkillProp(473, "skill_02", "skill_02", "skill_02.mesh");
	itemEatSkillProp_06 = LordNew cItemSkillProp(474, "skill_03", "skill_03", "skill_03.mesh");
	itemEatSkillProp_07 = LordNew cItemSkillProp(475, "skill_01", "skill_01", "skill_01.mesh");
	itemEatSkillProp_08 = LordNew cItemSkillProp(476, "skill_02", "skill_02", "skill_02.mesh");
	itemEatSkillProp_09 = LordNew cItemSkillProp(477, "skill_03", "skill_03", "skill_03.mesh");
	itemEatSkillProp_10 = LordNew cItemSkillProp(478, "skill_03", "skill_03", "skill_03.mesh");
	

	//skill book
	itemSkillFlashBook = LordNew cItemSkillBook(479, "skill.book.1", "skill_flash");
	itemSkillAccelerateBook = LordNew cItemSkillBook(480, "skill.book.2", "skill_accelerate");
	itemSkillStealthBook = LordNew cItemSkillBook(481, "skill.book.3", "skill_stealth");
	itemSkillFlyBook = LordNew cItemSkillBook(482, "skill.book.4", "skill_fly");
	itemSkillInvincibleBook = LordNew cItemSkillBook(483, "skill.book.5", "skill_invincible");
	itemSkillTransferBook = LordNew cItemSkillBook(484, "skill.book.6", "skill_transfer");
	itemSkillDarkBook = LordNew cItemSkillBook(485, "skill.book.7", "skill_dark");

	//skill book fragmentation
	itemSkillFlashBookFragmentation = LordNew cItemSkillBookFragmentation(486, "skill.chip.1", "skill_flash_fragmentation");
	itemSkillAccelerateBookFragmentation = LordNew cItemSkillBookFragmentation(487, "skill.chip.2", "skill_accelerate_fragmentation");
	itemSkillStealthBookFragmentation = LordNew cItemSkillBookFragmentation(488, "skill.chip.3", "skill_stealth_fragmentation");
	itemSkillFlyBookFragmentation = LordNew cItemSkillBookFragmentation(489, "skill.chip.4", "skill_fly_fragmentation");
	itemSkillInvincibleBookFragmentation = LordNew cItemSkillBookFragmentation(490, "skill.chip.5", "skill_invincible_fragmentation");
	itemSkillTransferBookFragmentation = LordNew cItemSkillBookFragmentation(491,"skill.chip.6", "skill_transfer_fragmentation");
	itemSkillDarkBookFragmentation = LordNew cItemSkillBookFragmentation(492, "skill.chip.7", "skill_dark_fragmentation");

	itemSurpriseTreasureChest_01 = LordNew cItemSurpriseTreasureChest(493, "skill.chip.chest.1", "surprise_treasure_chest_01");
	itemSurpriseTreasureChest_02 = LordNew cItemSurpriseTreasureChest(494, "skill.chip.chest.2", "surprise_treasure_chest_02");
	itemSurpriseTreasureChest_03 = LordNew cItemSurpriseTreasureChest(495, "skill.chip.chest.3", "surprise_treasure_chest_03");

	equipmentSet1 = LordNew cItemProp(496, "equipment.set.1", "equipment_set_1", StringUtil::BLANK);
	equipmentSet2 = LordNew cItemProp(497, "equipment.set.2", "equipment_set_2", StringUtil::BLANK);
	equipmentSet3 = LordNew cItemProp(498, "equipment.set.3", "equipment_set_3", StringUtil::BLANK);
	equipmentSet4 = LordNew cItemProp(499, "equipment.set.4", "equipment_set_4", StringUtil::BLANK);
	equipmentSet5 = LordNew cItemProp(500, "equipment.set.5", "equipment_set_5", StringUtil::BLANK);

	purplePickaxe = LordNew cItemPickaxe(501, "purple.pickaxe", "purple_pickaxe", ToolMaterial::EMERALD);
	smokeGrenade = LordNew cItemGrenade(502, "grenade.smoke", "grenade_smoke");

	gun_39 = LordNew cItemGun(503, 39, "gun_39", "apple_golden", "gun_39.mesh");
	gun_40 = LordNew cItemGun(504, 40, "gun_40", "apple_golden", "gun_40.mesh");
	gun_41 = LordNew cItemGun(505, 41, "gun_41", "apple_golden", "gun_41.mesh");
	gun_42 = LordNew cItemGun(506, 42, "gun_42", "apple_golden", "gun_42.mesh");
	gun_43 = LordNew cItemGun(507, 43, "gun_43", "apple_golden", "gun_43.mesh");
	gun_44 = LordNew cItemGun(508, 44, "gun_44", "apple_golden", "gun_44.mesh");
	gun_45 = LordNew cItemGun(509, 45, "gun_45", "apple_golden", "gun_45.mesh");
	gun_46 = LordNew cItemGun(510, 46, "gun_46", "apple_golden", "gun_46.mesh");
	gun_47 = LordNew cItemGun(511, 47, "gun_47", "apple_golden", "gun_47.mesh");
	gun_48 = LordNew cItemGun(512, 48, "gun_48", "apple_golden", "gun_48.mesh");

	tremorGrenade = LordNew ItemClient(513, "grenade.tremor", "grenade_tremor");
	frozenBall = LordNew ItemClient(514, "frozen.ball", "frozen_ball");

	gun_49 = LordNew cItemGun(515, 49, "gun_49", "apple_golden", "gun_49.mesh");
	gun_50 = LordNew cItemGun(516, 50, "gun_50", "apple_golden", "gun_50.mesh");
	gun_51 = LordNew cItemGun(517, 51, "gun_51", "apple_golden", "gun_51.mesh");
	gun_52 = LordNew cItemGun(518, 52, "gun_52", "apple_golden", "gun_52.mesh");
	gun_53 = LordNew cItemGun(519, 53, "gun_53", "apple_golden", "gun_53.mesh");
	gun_54 = LordNew cItemGun(520, 54, "gun_54", "apple_golden", "gun_54.mesh");
	gun_55 = LordNew cItemGun(521, 55, "gun_55", "apple_golden", "gun_55.mesh");
	gun_56 = LordNew cItemGun(522, 56, "gun_56", "apple_golden", "gun_56.mesh");
	gun_57 = LordNew cItemGun(523, 57, "gun_57", "apple_golden", "gun_57.mesh");
	gun_58 = LordNew cItemGun(524, 58, "gun_58", "apple_golden", "gun_58.mesh");

	baseballStick = LordNew cItemProp(525, "baseball_stick", "baseball_stick", "g1036_weapon_lvl1.mesh");
	spikeStick = LordNew cItemProp(526, "spike_stick", "spike_stick", "g1036_weapon_lvl2.mesh");
	stonePillar = LordNew cItemProp(527, "stone_pillar", "stone_pillar", "g1036_weapon_lvl3.mesh");

	shortenRebirthTimeItem = LordNew cItemAppIcon(528, "shorten_rebirth_time", "items_shorten_rebirth_time");
	increaseHpItem = LordNew cItemAppIcon(529, "increase_hp", "items_increase_hp");
	diamondDefensePackItem = LordNew cItemAppIcon(530, "diamond_defense_pack", "items_diamond_defense_pack");

	grenade_skill_01 = LordNew ItemClient(530, "grenade_skill", "skill_21");
	grenade_skill_02 = LordNew ItemClient(531, "grenade_skill", "skill_21");
	colorful_grenade_skill_01 = LordNew ItemClient(532, "colorful_grenade_skill", "skill_23");
	colorful_grenade_skill_02 = LordNew ItemClient(533, "colorful_grenade_skill", "skill_23");
	change_animal_skill_01 = LordNew cItemTransparent(534, "change_animal_skill", "skill_20");
	change_animal_skill_02 = LordNew cItemTransparent(535, "change_animal_skill", "skill_20");
	speed_up_skill_01 = LordNew cItemTransparent(536, "speed_up_skill", "skill_22");
	speed_up_skill_02 = LordNew cItemTransparent(537, "speed_up_skill", "skill_22");

	gun_59 = LordNew cItemGun(538, 59, "gun_59", "apple_golden", "gun_59.mesh");
	gun_60 = LordNew cItemGun(539, 60, "gun_60", "apple_golden", "gun_60.mesh");
	gun_61 = LordNew cItemGun(540, 61, "gun_61", "apple_golden", "gun_61.mesh");
	gun_62 = LordNew cItemGun(541, 62, "gun_62", "apple_golden", "gun_62.mesh");
	gun_63 = LordNew cItemGun(542, 63, "gun_63", "apple_golden", "gun_63.mesh");
	gun_64 = LordNew cItemGun(543, 64, "gun_64", "apple_golden", "gun_64.mesh");
	gun_65 = LordNew cItemGun(544, 65, "gun_65", "apple_golden", "gun_65.mesh");
	gun_66 = LordNew cItemGun(545, 66, "gun_66", "apple_golden", "gun_66.mesh");
	gun_67 = LordNew cItemGun(546, 67, "gun_67", "apple_golden", "gun_67.mesh");
	gun_68 = LordNew cItemGun(547, 68, "gun_68", "apple_golden", "gun_68.mesh");
	gun_69 = LordNew cItemGun(548, 69, "gun_69", "apple_golden", "gun_69.mesh");
	gun_70 = LordNew cItemGun(549, 70, "gun_70", "apple_golden", "gun_70.mesh");
	gun_71 = LordNew cItemGun(550, 71, "gun_71", "apple_golden", "gun_71.mesh");
	gun_72 = LordNew cItemGun(551, 72, "gun_72", "apple_golden", "gun_72.mesh");
	gun_73 = LordNew cItemGun(552, 73, "gun_73", "apple_golden", "gun_73.mesh");
	gun_74 = LordNew cItemGun(553, 74, "gun_74", "apple_golden", "gun_74.mesh");
	gun_75 = LordNew cItemGun(554, 75, "gun_75", "apple_golden", "gun_75.mesh");
	gun_76 = LordNew cItemGun(555, 76, "gun_76", "apple_golden", "gun_76.mesh");
	gun_77 = LordNew cItemGun(556, 77, "gun_77", "apple_golden", "gun_77.mesh");
	gun_78 = LordNew cItemGun(557, 78, "gun_78", "apple_golden", "gun_78.mesh");
	gun_79 = LordNew cItemGun(558, 79, "gun_79", "apple_golden", "gun_79.mesh");
	gun_80 = LordNew cItemGun(559, 80, "gun_80", "apple_golden", "gun_80.mesh");
	gun_81 = LordNew cItemGun(560, 81, "gun_81", "apple_golden", "gun_81.mesh");
	gun_82 = LordNew cItemGun(561, 82, "gun_82", "apple_golden", "gun_82.mesh");
	gun_83 = LordNew cItemGun(562, 83, "gun_83", "apple_golden", "gun_83.mesh");
	gun_84 = LordNew cItemGun(563, 84, "gun_84", "apple_golden", "gun_84.mesh");
	gun_85 = LordNew cItemGun(564, 85, "gun_85", "apple_golden", "gun_85.mesh");
	gun_86 = LordNew cItemGun(565, 86, "gun_86", "apple_golden", "gun_86.mesh");
	gun_87 = LordNew cItemGun(566, 87, "gun_87", "apple_golden", "gun_87.mesh");
	gun_88 = LordNew cItemGun(567, 88, "gun_88", "apple_golden", "gun_88.mesh");
	gun_89 = LordNew cItemGun(568, 89, "gun_89", "apple_golden", "gun_89.mesh");
	gun_90 = LordNew cItemGun(569, 90, "gun_90", "apple_golden", "gun_90.mesh");
	gun_91 = LordNew cItemGun(570, 91, "gun_91", "apple_golden", "gun_91.mesh");
	gun_92 = LordNew cItemGun(571, 92, "gun_92", "apple_golden", "gun_92.mesh");
	gun_93 = LordNew cItemGun(572, 93, "gun_93", "apple_golden", "gun_93.mesh");
	gun_94 = LordNew cItemGun(573, 94, "gun_94", "apple_golden", "gun_94.mesh");
	gun_95 = LordNew cItemGun(574, 95, "gun_95", "apple_golden", "gun_95.mesh");
	gun_96 = LordNew cItemGun(575, 96, "gun_96", "apple_golden", "gun_96.mesh");
	gun_97 = LordNew cItemGun(576, 97, "gun_97", "apple_golden", "gun_97.mesh");
	gun_98 = LordNew cItemGun(577, 98, "gun_98", "apple_golden", "gun_98.mesh");
	gun_99 = LordNew cItemGun(578, 99, "gun_99", "apple_golden", "gun_99.mesh");
	gun_100 = LordNew cItemGun(579, 100, "gun_100", "apple_golden", "gun_100.mesh");
	gun_101 = LordNew cItemGun(580, 101, "gun_101", "apple_golden", "gun_101.mesh");
	gun_102 = LordNew cItemGun(581, 102, "gun_102", "apple_golden", "gun_102.mesh");
	gun_103 = LordNew cItemGun(582, 103, "gun_103", "apple_golden", "gun_103.mesh");
	gun_104 = LordNew cItemGun(583, 104, "gun_104", "apple_golden", "gun_104.mesh");
	gun_105 = LordNew cItemGun(584, 105, "gun_105", "apple_golden", "gun_105.mesh");
	gun_106 = LordNew cItemGun(585, 106, "gun_106", "apple_golden", "gun_106.mesh");
	gun_107 = LordNew cItemGun(586, 107, "gun_107", "apple_golden", "gun_107.mesh");
	gun_108 = LordNew cItemGun(587, 108, "gun_108", "apple_golden", "gun_108.mesh");
	gun_109 = LordNew cItemGun(588, 109, "gun_109", "apple_golden", "gun_109.mesh");
	gun_110 = LordNew cItemGun(589, 110, "gun_110", "apple_golden", "gun_110.mesh");
	gun_111 = LordNew cItemGun(590, 111, "gun_111", "apple_golden", "gun_111.mesh");
	gun_112 = LordNew cItemGun(591, 112, "gun_112", "apple_golden", "gun_112.mesh");
	gun_113 = LordNew cItemGun(592, 113, "gun_113", "apple_golden", "gun_113.mesh");
	gun_114 = LordNew cItemGun(593, 114, "gun_114", "apple_golden", "gun_114.mesh");
	gun_115 = LordNew cItemGun(594, 115, "gun_115", "apple_golden", "gun_115.mesh");
	gun_116 = LordNew cItemGun(595, 116, "gun_116", "apple_golden", "gun_116.mesh");
	gun_117 = LordNew cItemGun(596, 117, "gun_117", "apple_golden", "gun_117.mesh");
	gun_118 = LordNew cItemGun(597, 118, "gun_118", "apple_golden", "gun_118.mesh");

	pixel_gun_prop_1 = LordNew cItemProp(598, "pixel_gun_prop_1", "pixel_gun_prop_1", "pixel_gun_prop_1.mesh");
	pixel_gun_prop_2 = LordNew cItemProp(599, "pixel_gun_prop_2", "pixel_gun_prop_2", "pixel_gun_prop_2.mesh");
	pixel_gun_prop_3 = LordNew cItemProp(600, "pixel_gun_prop_3", "pixel_gun_prop_3", "pixel_gun_prop_3.mesh");
	pixel_gun_prop_4 = LordNew cItemProp(601, "pixel_gun_prop_4", "pixel_gun_prop_4", "pixel_gun_prop_4.mesh");
	pixel_gun_prop_5 = LordNew cItemProp(602, "pixel_gun_prop_5", "pixel_gun_prop_5", "pixel_gun_prop_5.mesh");
	pixel_gun_prop_6 = LordNew cItemProp(603, "pixel_gun_prop_6", "pixel_gun_prop_6", "pixel_gun_prop_6.mesh");
	pixel_gun_prop_7 = LordNew cItemProp(604, "pixel_gun_prop_7", "pixel_gun_prop_7", "pixel_gun_prop_7.mesh");
	pixel_gun_prop_8 = LordNew cItemProp(605, "pixel_gun_prop_8", "pixel_gun_prop_8", "pixel_gun_prop_8.mesh");
	pixel_gun_prop_9 = LordNew cItemProp(606, "pixel_gun_prop_9", "pixel_gun_prop_9", "pixel_gun_prop_9.mesh");
	pixel_gun_prop_10 = LordNew cItemProp(607, "pixel_gun_prop_10", "pixel_gun_prop_10", "pixel_gun_prop_10.mesh");
	pixel_gun_prop_11 = LordNew cItemProp(608, "pixel_gun_prop_11", "pixel_gun_prop_11", "pixel_gun_prop_11.mesh");
	pixel_gun_prop_12 = LordNew cItemProp(609, "pixel_gun_prop_12", "pixel_gun_prop_12", "pixel_gun_prop_12.mesh");
	pixel_gun_prop_13 = LordNew cItemProp(610, "pixel_gun_prop_13", "pixel_gun_prop_13", "pixel_gun_prop_13.mesh");
	pixel_gun_prop_14 = LordNew cItemProp(611, "pixel_gun_prop_14", "pixel_gun_prop_14", "pixel_gun_prop_14.mesh");
	pixel_gun_prop_15 = LordNew cItemProp(612, "pixel_gun_prop_15", "pixel_gun_prop_15", "pixel_gun_prop_15.mesh");
	pixel_gun_prop_16 = LordNew cItemProp(613, "pixel_gun_prop_16", "pixel_gun_prop_16", "pixel_gun_prop_16.mesh");
	pixel_gun_prop_17 = LordNew cItemProp(614, "pixel_gun_prop_17", "pixel_gun_prop_17", "pixel_gun_prop_17.mesh");
	pixel_gun_prop_18 = LordNew cItemProp(615, "pixel_gun_prop_18", "pixel_gun_prop_18", "pixel_gun_prop_18.mesh");
	pixel_gun_prop_19 = LordNew cItemProp(616, "pixel_gun_prop_19", "pixel_gun_prop_19", "pixel_gun_prop_19.mesh");
	pixel_gun_prop_20 = LordNew cItemProp(617, "pixel_gun_prop_20", "pixel_gun_prop_20", "pixel_gun_prop_20.mesh");
	pixel_gun_prop_21 = LordNew cItemProp(618, "pixel_gun_prop_21", "pixel_gun_prop_21", "pixel_gun_prop_21.mesh");
	pixel_gun_prop_22 = LordNew cItemProp(619, "pixel_gun_prop_22", "pixel_gun_prop_22", "skill_11.mesh");
	pixel_gun_prop_23 = LordNew cItemProp(620, "pixel_gun_prop_23", "pixel_gun_prop_23", "skill_11.mesh");
	pixel_gun_prop_24 = LordNew cItemProp(621, "pixel_gun_prop_24", "pixel_gun_prop_24", "skill_11.mesh");
	pixel_gun_prop_25 = LordNew cItemProp(622, "pixel_gun_prop_25", "pixel_gun_prop_25", "skill_11.mesh");
	pixel_gun_prop_26 = LordNew cItemProp(623, "pixel_gun_prop_26", "pixel_gun_prop_26", "skill_11.mesh");
	pixel_gun_prop_27 = LordNew cItemProp(624, "pixel_gun_prop_27", "pixel_gun_prop_27", "skill_11.mesh");
	pixel_gun_prop_28 = LordNew cItemProp(625, "pixel_gun_prop_28", "pixel_gun_prop_28", "skill_11.mesh");
	pixel_gun_prop_29 = LordNew cItemProp(626, "pixel_gun_prop_29", "pixel_gun_prop_29", "skill_11.mesh");
	pixel_gun_prop_30 = LordNew cItemProp(627, "pixel_gun_prop_30", "pixel_gun_prop_30", "skill_11.mesh");

	//throwing skill 1200-1211  944-955
	itemThrowableSkillProp_11 = LordNew cItemSkillProp(944, "skill_11", "skill_11", "skill_11.mesh");
	itemThrowableSkillProp_12 = LordNew cItemSkillProp(945, "skill_12", "skill_12", "skill_12.mesh");
	itemThrowableSkillProp_13 = LordNew cItemSkillProp(946, "skill_13", "skill_13", "skill_13.mesh");
	itemThrowableSkillProp_14 = LordNew cItemSkillProp(947, "skill_14", "skill_14", "skill_14.mesh");
	itemThrowableSkillProp_15 = LordNew cItemSkillProp(948, "skill_15", "skill_15", "skill_15.mesh");
	itemThrowableSkillProp_16 = LordNew cItemSkillProp(949, "skill_16", "skill_16", "skill_16.mesh");
	itemThrowableSkillProp_17 = LordNew cItemSkillProp(950, "skill_17", "skill_17", "skill_17.mesh");
	itemThrowableSkillProp_18 = LordNew cItemSkillProp(951, "skill_18", "skill_18", "skill_18.mesh");
	itemThrowableSkillProp_19 = LordNew cItemSkillProp(952, "skill_19", "skill_19", "skill_19.mesh");
	itemThrowableSkillProp_20 = LordNew cItemSkillProp(953, "skill_20", "skill_20", "skill_20.mesh");

	// pro 1212-1235  956-979
	itemThrowableSkillProp_04_plus = LordNew cItemSkillProp(956, "skill_04_plus", "skill_04_plus", "skill_04_plus.mesh");
	itemThrowableSkillProp_05_plus = LordNew cItemSkillProp(957, "skill_05_plus", "skill_05_plus", "skill_05_plus.mesh");
	itemThrowableSkillProp_06_plus = LordNew cItemSkillProp(958, "skill_06_plus", "skill_06_plus", "skill_06_plus.mesh");
	itemThrowableSkillProp_07_plus = LordNew cItemSkillProp(959, "skill_07_plus", "skill_07_plus", "skill_07_plus.mesh");
	itemThrowableSkillProp_08_plus = LordNew cItemSkillProp(960, "skill_08_plus", "skill_08_plus", "skill_08_plus.mesh");
	itemThrowableSkillProp_09_plus = LordNew cItemSkillProp(961, "skill_09_plus", "skill_09_plus", "skill_09_plus.mesh");
	itemThrowableSkillProp_11_plus = LordNew cItemSkillProp(962, "skill_11_plus", "skill_11_plus", "skill_11_plus.mesh");
	itemThrowableSkillProp_12_plus = LordNew cItemSkillProp(963, "skill_12_plus", "skill_12_plus", "skill_12_plus.mesh");
	itemThrowableSkillProp_13_plus = LordNew cItemSkillProp(964, "skill_13_plus", "skill_13_plus", "skill_13_plus.mesh");
	itemThrowableSkillProp_14_plus = LordNew cItemSkillProp(965, "skill_14_plus", "skill_14_plus", "skill_14_plus.mesh");
	itemThrowableSkillProp_15_plus = LordNew cItemSkillProp(966, "skill_15_plus", "skill_15_plus", "skill_15_plus.mesh");
	itemThrowableSkillProp_16_plus = LordNew cItemSkillProp(967, "skill_16_plus", "skill_16_plus", "skill_16_plus.mesh");
	itemThrowableSkillProp_17_plus = LordNew cItemSkillProp(968, "skill_17_plus", "skill_17_plus", "skill_17_plus.mesh");
	itemThrowableSkillProp_18_plus = LordNew cItemSkillProp(969, "skill_18_plus", "skill_18_plus", "skill_18_plus.mesh");
	itemThrowableSkillProp_19_plus = LordNew cItemSkillProp(970, "skill_19_plus", "skill_19_plus", "skill_19_plus.mesh");
	itemThrowableSkillProp_20_plus = LordNew cItemSkillProp(971, "skill_20_plus", "skill_20_plus", "skill_20_plus.mesh");

	//weapon  1236-1255  980-999
	bow_red = LordNew cItemBow(980, "bow_red", "bow_red");
	bow_black = LordNew cItemBow(981, "bow_black", "bow_black");
	redArrow = LordNew ItemClient(982, "redArrow", "redArrow");
	blackArrow = LordNew ItemClient(983, "blackArrow", "blackArrow");

	weapon_01 = LordNew cItemProp(984, "weapon_01", "weapon_01", StringUtil::BLANK);
	weapon_02 = LordNew cItemProp(985, "weapon_02", "weapon_02", "weapon_02.mesh");
	weapon_03 = LordNew cItemGun(986, 31, "weapon_03", "weapon_03", "weapon_03.mesh");
	weapon_04 = LordNew cItemProp(987, "weapon_04", "weapon_04", "weapon_04.mesh");
	weapon_05 = LordNew cItemProp(988, "weapon_05", "weapon_05", "weapon_05.mesh");
	weapon_06 = LordNew cItemProp(989, "weapon_06", "weapon_06", "weapon_06.mesh");
	weapon_07 = LordNew cItemProp(990, "weapon_07", "weapon_07", "weapon_07.mesh");
	weapon_08 = LordNew cItemProp(991, "weapon_08", "weapon_08", "weapon_08.mesh");
	weapon_09 = LordNew cItemProp(992, "weapon_09", "weapon_09", "weapon_09.mesh");
	weapon_10 = LordNew cItemProp(993, "weapon_10", "weapon_10", "weapon_10.mesh");
	weapon_11 = LordNew cItemProp(994, "weapon_11", "weapon_11", "weapon_11.mesh");
	weapon_12 = LordNew cItemProp(995, "weapon_12", "weapon_12", "weapon_12.mesh");
	weapon_13 = LordNew cItemProp(996, "weapon_13", "weapon_13", "weapon_13.mesh");
	weapon_14 = LordNew cItemProp(997, "weapon_14", "weapon_14", "weapon_14.mesh");
	weapon_15 = LordNew cItemProp(998, "weapon_15", "weapon_15", "weapon_15.mesh");
	weapon_16 = LordNew cItemProp(999, "weapon_16", "weapon_16", "weapon_16.mesh");
	weapon_17 = LordNew cItemProp(1000, "weapon_17", "weapon_17", "weapon_17.mesh");
	weapon_18 = LordNew cItemProp(1001, "weapon_18", "weapon_18", "weapon_18.mesh");

	initializeItemNameToIdMap();

	cBlockManager::cRegistBlockItems();
	cPotionManager::cRegisterPotionItem();

	for (int i = 0; i < MAX_ITEM_COUNT; ++i)
	{
		if (!itemsList[i])
			continue;
		citemsList[i] = dynamic_cast<cItem*>(itemsList[i]);
		LordAssert(citemsList[i]);
	}
}

void ItemClient::cUnInitialize()
{
	for (int i = 0; i < MAX_ITEM_COUNT; ++i)
	{
		LordSafeDelete(citemsList[i]);
	}
	LordSafeDelete(itemRand);
}

void ItemClient::registSprite(TextureAtlasRegister* pAtlas)
{
	cItem* pItem = NULL;
	for (int i = 0; i < MAX_ITEM_COUNT; ++i)
	{
		pItem = citemsList[i];
		if (!pItem)
			continue;
		if (pItem->getRenderType() == ItemRenderType::BLOCK)
			continue;
		pItem->registerIcons(pAtlas);
	}
}


ItemRenderType ItemClient::getRenderType(ItemStackPtr pStack)
{
	if (pStack == NULL)
		return ItemRenderType::TEXTURE;

	if (pStack->getItem() != NULL)
	{
		int id = pStack->getItem()->itemID;
		if(id >= 0 && id < MAX_ITEM_COUNT && citemsList[id] != NULL)
			return citemsList[id]->getRenderType();
	}

	return ItemRenderType::TEXTURE;
}


cItemSpade::cItemSpade(int id, const String& name, const String& atlas, ToolMaterial* toolMat)
: ItemSpade(id, name, toolMat)
, cItem(atlas)
{}

cItemPickaxe::cItemPickaxe(int id, const String& name, const String& atlas, ToolMaterial* toolMat)
: ItemPickaxe(id, name, toolMat)
, cItem(atlas)
{}

cItemAxe::cItemAxe(int id, const String& name, const String& atlas, ToolMaterial* toolMat)
: ItemAxe(id, name, toolMat)
, cItem(atlas)
{}

cItemFlintAndSteel::cItemFlintAndSteel(int id, const String& name, const String& atlas)
: ItemFlintAndSteel(id, name)
, cItem(atlas)
{}

cItemFood::cItemFood(int id, const String& name, const String& atlas, int amount, float modifier, bool isfavorite)
: ItemFood(id, name, amount, modifier, isfavorite)
, cItem(atlas)
{}

cItemFood::cItemFood(int id, const String& name, const String& atlas, int amount, bool isfavorite)
: ItemFood(id, name, amount, isfavorite)
, cItem(atlas)
{}

cItemSoup::cItemSoup(int id, const String& name, const String& atlas, int amount)
: ItemSoup(id, name, amount)
, cItem(atlas)
{}

cItemAppleGold::cItemAppleGold(int id, const String& name, const String& atlas, int amount, float modifier, bool isfavorite)
: ItemAppleGold(id, name, amount, modifier, isfavorite)
, cItem(atlas)
{}

cItemSeedFood::cItemSeedFood(int id, const String& name, const String& atlas, int amount, float modifier, int crop, int soil)
: ItemSeedFood(id, name, amount, modifier, crop, soil)
, cItem(atlas)
{}

String cItemBow::bowPullIconNameArray[MAX_ATLAS_COUNT] = { "pulling_0", "pulling_1", "pulling_2" };

cItemBow::cItemBow(int id, const String& name, const String& atlas)
: ItemBow(id, name)
, cItem(atlas)
{}

void cItemBow::registerIcons(TextureAtlasRegister* atlas)
{
	itemIcon = atlas->getAtlasSprite(getAtlasName() + "_standby");

	for (int i = 0; i < MAX_ATLAS_COUNT; ++i)
	{
		iconArray[i] = atlas->getAtlasSprite(getAtlasName() + "_" + bowPullIconNameArray[i]);
	}
}

int cItemBow::getItemKey(int useDuration)
{
	int itemKey = itemID;

	if (useDuration > 0)
		itemKey = 256 + 200;
	if (useDuration > 13)
		itemKey = 256 + 201;
	if (useDuration >= 18)
		itemKey = 256 + 202;
	
	return itemKey;
}

int cItemBow::getDurationSeg(int useDuration)
{
	int dur = 0;
	if (useDuration > 0)
		dur = 1;
	if (useDuration > 13)
		dur = 2;
	if (useDuration >= 18)
		dur = 3;
	return dur;
}

cItemGun::cItemGun(int id, int gunId, const String& name, const String& atlas, const String& mesh)
	: ItemGun(id, gunId, name)
	, cItem(atlas)
	, m_meshName(mesh)
{
	initCItemGun();
}

ItemRenderType cItemGun::getRenderType()
{
	return ItemRenderType::MESH;
}

const String& cItemGun::getMeshName() const
{
	return m_meshName;
}

void cItemGun::fire(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer)
{
	const GunSetting* setting = getGunSetting();
	auto player = Blockman::Instance()->m_pPlayer;
	if (player && !player->m_bIsInAimSight && setting && (setting->gunType < RIFLE_EFFECT_GUN || setting->gunType > MOMENTARY_PIERCING_LASER_EFFECT_GUN))
	{
		Blockman::Instance()->getWorld()->spawnParticle("gunFlame", Vector3::ZERO, Vector3::ZERO, player);
	}

	ItemGun::fire(pStack, pWorld, pPlayer);

	// adjust the pitch for gun Recoil
	float recoil = ((float)pWorld->m_Rand.nextInt((int)(pPlayer->getGunRecoil(setting) * 100))) / 100;
	Blockman::Instance()->m_pPlayer->rotationPitch -= recoil;
	if (Blockman::Instance()->m_gameSettings->getPersonView() == GameSettings::SPVT_FIRST)
	{
		Blockman::Instance()->m_pPlayer->m_isShooting = true;
	}
}

void cItemGun::initCItemGun()
{
	const GunSetting* pSetting = getGunSetting();
	if (!pSetting)
		return;

	setAtlasName(pSetting->gunIcon);
	m_meshName = pSetting->gunModel;
}

String cItemGun::getUnlocalizedName(ItemStackPtr pStack)
{
	const GunSetting* pSetting = getGunSetting();
	if (!pSetting)
		return Item::getUnlocalizedName(pStack);

	return pSetting->gunName;
}

cItemBulletClip::cItemBulletClip(int id, int bulletClipId, const String& name, const String& atlas)
	: ItemBulletClip(id, bulletClipId, name)
	, cItem(atlas)
{
	initCItemBulletClip();
}

void cItemBulletClip::initCItemBulletClip()
{
	const BulletClipSetting* pSetting = getBulletClipSetting();
	if (!pSetting)
		return;

	setAtlasName(pSetting->icon);
}

String cItemBulletClip::getUnlocalizedName(ItemStackPtr pStack)
{
	const BulletClipSetting* pSetting = getBulletClipSetting();
	if (!pSetting)
		return Item::getUnlocalizedName(pStack);

	return pSetting->bulletName;
}

cItemHandcuffs::cItemHandcuffs(int id, const String& name, const String& atlas)
	: ItemHandcuffs(id, name)
	, cItem(atlas)
{
}

cItemCoal::cItemCoal(int id, const String& name, const String& atlas)
: ItemCoal(id, name)
, cItem(atlas)
{}

AtlasSprite* cItemCoal::getIconFromDamage(int damage)
{
	return damage == 1 ? m_charcoal : cItem::getIconFromDamage(damage);
}

void cItemCoal::registerIcons(TextureAtlasRegister* atlas)
{
	cItem::registerIcons(atlas);
	m_charcoal = atlas->getAtlasSprite("charcoal");
}

cItemSword::cItemSword(int id, const String& name, const String& atlas, ToolMaterial* toolMat)
: ItemSword(id, name, toolMat)
, cItem(atlas)
{}

cItemReed::cItemReed(int id, const String& name, const String& atlas, int reedID)
: ItemReed(id, name, reedID)
, cItem(atlas)
{}

cItemHoe::cItemHoe(int id, const String& name, const String& atlas, ToolMaterial* toolMat)
: ItemHoe(id, name, toolMat)
, cItem(atlas)
{}

cItemSeeds::cItemSeeds(int id, const String& name, const String& atlas, int blocktype, int soilID)
: ItemSeeds(id, name, blocktype, soilID)
, cItem(atlas)
{}

cItemHangingEntity::cItemHangingEntity(int id, const String& name, const String& atlas, int classID)
: ItemHangingEntity(id, name, classID)
, cItem(atlas)
{}

cItemSign::cItemSign(int id, const String& name, const String& atlas)
: ItemSign(id, name)
, cItem(atlas)
{}

cItemDoor::cItemDoor(int id, const String& name, const String& atlas, const BM_Material& mat)
: ItemDoor(id, name, mat)
, cItem(atlas)
{}

cItemEnderPearl::cItemEnderPearl(int id, const String& name, const String& atlas)
: ItemEnderPearl(id, name)
, cItem(atlas)
{}

cItemGlassBottle::cItemGlassBottle(int id, const String& name, const String& atlas)
: ItemGlassBottle(id, name)
, cItem(atlas)
{}

AtlasSprite* cItemGlassBottle::getIconFromDamage(int damage)
{
	return cItem::getIconFromDamage(0);
}

void cItemGlassBottle::registerIcons(TextureAtlasRegister * atlas)
{
	itemIcon = atlas->getAtlasSprite("items.json", getAtlasName());
}

cItemEnderEye::cItemEnderEye(int id, const String& name, const String& atlas)
: ItemEnderEye(id, name)
, cItem(atlas)
{}

cItemMonsterPlacer::cItemMonsterPlacer(int id, const String& name, const String& atlas)
: ItemMonsterPlacer(id, name)
, cItem(atlas)
{}

int cItemMonsterPlacer::getColorFromItemStack(ItemStackPtr pStack, int par2)
{
	// todo.
	// EntityEggInfo var3 = (EntityEggInfo)EntityList.entityEggs.get(Integer.valueOf(pStack.getItemDamage()));
	// return var3 != null ? (par2 == 0 ? var3.primaryColor : var3.secondaryColor) : 16777215;
	return 0x00FFFFFF;
}

AtlasSprite* cItemMonsterPlacer::getIconFromDamageForRenderPass(int damage, int pass)
{
	return pass > 0 ? theIcon : cItem::getIconFromDamageForRenderPass(damage, pass);
}

void cItemMonsterPlacer::registerIcons(TextureAtlasRegister* atlas)
{
	cItem::registerIcons(atlas);
	theIcon = atlas->getAtlasSprite(getAtlasName() + "_overlay");
}

cItemArmor::cItemArmor(int id, const String& name, const String& atlas, ArmorMaterial* armorMat, int rendertype, int armortype)
: ItemArmor(id, name, armorMat, armortype)
, cItem(atlas)
, renderIndex(rendertype)
{}

AtlasSprite* cItemArmor::getSpriteByArmorType(int armorIdx)
{
	switch (armorIdx)
	{
	case 0:
		return ((cItemArmor*)Item::helmetDiamond)->armor_sprite;
	case 1:
		return ((cItemArmor*)Item::plateDiamond)->armor_sprite;
	case 2:
		return ((cItemArmor*)Item::legsDiamond)->armor_sprite;
	case 3:
		return ((cItemArmor*)Item::bootsDiamond)->armor_sprite;
	}
	return NULL;
}

int cItemArmor::getColorFromItemStack(ItemStackPtr pStack, int par2)
{
	if (par2 > 0)
	{
		return 0x00FFFFFF;
	}
	else
	{
		int color = getColor(pStack);

		if (color < 0)
		{
			color = 0x00FFFFFF;
		}

		return color;
	}
}

AtlasSprite* cItemArmor::getIconFromDamageForRenderPass(int damage, int pass)
{
	return pass == 1 ? leather_sprite : cItem::getIconFromDamageForRenderPass(damage, pass);
}

void cItemArmor::registerIcons(TextureAtlasRegister* atlas)
{
	cItem::registerIcons(atlas);

	if (material == ArmorMaterial::CLOTH)
	{
		leather_sprite = atlas->getAtlasSprite(leather_armor_name[armorType]);
	}

	armor_sprite = atlas->getAtlasSprite(empty_armor_name[armorType]);
}


String cItemArmor::leather_armor_name[MAX_ARMOR_COUNT] = { "leather_helmet_overlay", "leather_chestplate_overlay", "leather_leggings_overlay", "leather_boots_overlay" };
String cItemArmor::empty_armor_name[MAX_ARMOR_COUNT] = { "empty_armor_slot_helmet", "empty_armor_slot_chestplate", "empty_armor_slot_leggings", "empty_armor_slot_boots" };


cItemPotion::cItemPotion(int id, const String& name, const String& atlas, bool isSplash/* = false*/)
: ItemPotion(id, name, isSplash)
, cItem(atlas)
{}

AtlasSprite* cItemPotion::getSpriteByName(const String& spritename)
{
	if (spritename == "bottle_drinkable")
		return ((cItemPotion*)Item::potion)->sprite_potion_drinkable;
	else if (spritename == "bottle_splash")
		return ((cItemPotion*)Item::potion)->sprite_potion_splash;
	else if (spritename == "overlay")
		return ((cItemPotion*)Item::potion)->sprite_potion_overlay;
	return NULL;
}

AtlasSprite* cItemPotion::getIconFromDamageForRenderPass(int damage, int pass)
{
	return pass == 0 ? sprite_potion_overlay : cItem::getIconFromDamageForRenderPass(damage, pass);
}

int cItemPotion::getColorFromItemStack(ItemStackPtr pStack, int par2)
{
	return par2 > 0 ? 0x00FFFFFF : getColorFromDamage(pStack->getItemDamage());
}

void cItemPotion::registerIcons(TextureAtlasRegister* atlas)
{
	sprite_potion_drinkable = atlas->getAtlasSprite(getAtlasName() + "_bottle_drinkable");
	sprite_potion_splash = atlas->getAtlasSprite(getAtlasName() + "_bottle_splash");
	sprite_potion_overlay = atlas->getAtlasSprite(getAtlasName() + "_overlay");
}

cItemEnchantedBook::cItemEnchantedBook(int id, const String& name, const String& atlas)
: ItemEnchantedBook(id, name)
, cItem(atlas)
{}

String cItemDye::dyeSpriteNames[MAX_DYE_NUM] = { "black", "red", "green", "brown", "blue", "purple", "cyan", "silver", "gray", "pink", "lime", "yellow", "light_blue", "magenta", "orange", "white" };

cItemDye::cItemDye(int id, const String& name, const String& atlas)
: ItemDye(id, name)
, cItem(atlas)
{
	for (int i = 0; i < MAX_DYE_NUM; ++i)
		dyeSprites[i] = nullptr;
}

AtlasSprite* cItemDye::getIconFromDamage(int damage)
{
	int idx = Math::Clamp(damage, 0, 15);
	if (dyeSprites[idx])
	{
		return dyeSprites[idx];
	}
	return cItem::getIconFromDamage(damage);
}

void cItemDye::registerIcons(TextureAtlasRegister* atlas)
{
	for (int i = 0; i < MAX_DYE_NUM; ++i)
	{
		dyeSprites[i] = atlas->getAtlasSprite(getAtlasName() + "_" + dyeSpriteNames[i]);
	}
	cItem::registerIcons(atlas);
}

cItemMapBase::cItemMapBase(int id, const String& name, const String& atlas)
: ItemMapBase(id, name)
, cItem(atlas)
{}

cItemEmptyMap::cItemEmptyMap(int id, const String& name, const String& atlas)
: ItemEmptyMap(id, name)
, cItem(atlas)
{}

cItemMap::cItemMap(int id, const String& name, const String& atlas)
: ItemMap(id, name)
, cItem(atlas)
{}

cItemBlock::cItemBlock(int id)
: cItem("empty")
, blockSprite(nullptr)
{
	int itemId = id + 256;
	LordAssert(Item::isBlockItem(itemId));
	if (Item::isBasicBlockItem(itemId))
	{
		m_blockID = itemId;
	}
	else
	{
		m_blockID = itemId - Item::EC_BLOCK_TO_ITEM_OFFSET;
	}
}

AtlasSprite* cItemBlock::getIconFromDamage(int damage)
{
	return blockSprite ? blockSprite : cBlockManager::scBlocks[m_blockID]->getSpriteFromSide(1);
}

ItemRenderType cItemBlock::getRenderType()
{
	return cBlockManager::scBlocks[m_blockID]->getItemSpriteName() != StringUtil::BLANK ? ItemRenderType::TEXTURE : ItemRenderType::BLOCK;
}

void cItemBlock::registerIcons(TextureAtlasRegister* atlas)
{
	String spriteName = cBlockManager::scBlocks[m_blockID]->getItemSpriteName();

	if (spriteName != StringUtil::BLANK)
	{
		blockSprite = atlas->getAtlasSprite(spriteName);
	}
}

cItemShears::cItemShears(int id, const String& name, const String& atlas)
: ItemShears(id, name)
, cItem(atlas)
{}

cItemBucket::cItemBucket(int id, const String& name, const String& atlas, int fillID)
: ItemBucket(id, name, fillID)
, cItem(atlas)
{}

cItemBucketMilk::cItemBucketMilk(int id, const String& name, const String& atlas)
: ItemBucketMilk(id, name)
, cItem(atlas)
{}

cItemBlockman::cItemBlockman(int id, const String& name, const String& atlas, int type)
: ItemBlockman(id, name, type)
, cItem(atlas)
{}

cItemSaddle::cItemSaddle(int id, const String& name, const String& atlas)
: ItemSaddle(id, name)
, cItem(atlas)
{}

cItemRedstone::cItemRedstone(int id, const String& name, const String& atlas)
: ItemRedstone(id, name)
, cItem(atlas)
{}

String cItemSnowball::spriteNames[SNOW_TYPE_CONUT] = { "0","1","2" };

cItemSnowball::cItemSnowball(int id, const String& name, const String& atlas)
: ItemSnowball(id, name)
, cItem(atlas)
{}

AtlasSprite* cItemSnowball::getIconFromDamage(int meta)
{
	if (meta < 0 || meta >= SNOW_TYPE_CONUT)
	{
		meta = 0;
	}
	return sprites[meta];
}

void cItemSnowball::registerIcons(TextureAtlasRegister* atlas)
{
	for (int i = 0; i < SNOW_TYPE_CONUT; ++i)
	{
		sprites[i] = atlas->getAtlasSprite(getAtlasName() + "_" + spriteNames[i]);
	}
}


cItemTnt::cItemTnt(int id, const String& name)
	: ItemTnt(id, name)
	, cItemBlock(id)
{}

cItemGrenade::cItemGrenade(int id, const String& name, const String& atlas)
	: ItemGrenade(id, name)
	, cItem(atlas)
{}

void cItemGrenade::registerIcons(TextureAtlasRegister * atlas)
{
	auto grenade = GrenadeSetting::getGrenadeSetting(itemID);
	itemIcon = atlas->getAtlasSprite("items.json", grenade->itemIcon);
}

cItemBoat::cItemBoat(int id, const String& name, const String& atlas)
: ItemBoat(id, name)
, cItem(atlas)
{}

cItemBook::cItemBook(int id, const String& name, const String& atlas)
: ItemBook(id, name)
, cItem(atlas)
{}

cItemEgg::cItemEgg(int id, const String& name, const String& atlas)
: ItemEgg(id, name)
, cItem(atlas)
{}

cItemFishingRod::cItemFishingRod(int id, const String& name, const String& atlas)
: ItemFishingRod(id, name)
, cItem(atlas)
{}

void cItemFishingRod::registerIcons(TextureAtlasRegister* atlas)
{
	itemIcon = atlas->getAtlasSprite(getAtlasName() + "_uncast");
	m_castIcon = atlas->getAtlasSprite(getAtlasName() + "_cast");
}

cItemBed::cItemBed(int id, const String& name, const String& atlas)
: ItemBed(id, name)
, cItem(atlas)
{}

cItemExpBottle::cItemExpBottle(int id, const String& name, const String& atlas)
: ItemExpBottle(id, name)
, cItem(atlas)
{}

cItemFireball::cItemFireball(int id, const String& name, const String& atlas)
: ItemFireball(id, name)
, cItem(atlas)
{}

cItemWritableBook::cItemWritableBook(int id, const String& name, const String& atlas)
: ItemWritableBook(id, name)
, cItem(atlas)
{}

cItemEditableBook::cItemEditableBook(int id, const String& name, const String& atlas)
: ItemEditableBook(id, name)
, cItem(atlas)
{}

void cItemEditableBook::addInformation(ItemStackPtr pStack, EntityPlayer* pPlayer, StringArray& fills, bool advance)
{
	if (pStack->hasTagCompound())
	{
		NBTTagCompound* pNbtStack = pStack->getTagCompound();
		NBTTagString* pNbtAuthor = (NBTTagString*)pNbtStack->getTag("author");

		if (pNbtAuthor)
		{
			String prompt = "book.byAuthor ";
			prompt += pNbtAuthor->data;
			fills.push_back(prompt);
			// todo.
			// par3List.add(EnumChatFormatting.GRAY + String.format(StatCollector.translateToLocalFormatted("book.byAuthor", new Object[] {pNbtAuthor.data}), new Object[0]));
		}
	}
}

String cItemSkull::spriteNames[SKULL_TYPE_COUNT] = {"skeleton", "wither", "zombie", "steve", "creeper"};

cItemSkull::cItemSkull(int id, const String& name, const String& atlas)
: ItemSkull(id, name)
, cItem(atlas)
{
	for (int i = 0; i < SKULL_TYPE_COUNT; ++i)
		sprites[i] = nullptr;
}

AtlasSprite* cItemSkull::getIconFromDamage(int meta)
{
	if (meta < 0 || meta >= SKULL_TYPE_COUNT)
	{
		meta = 0;
	}

	if (sprites[meta] != NULL)
	{
		return sprites[meta];
	}

	return cItem::getIconFromDamage(meta);
}

void cItemSkull::registerIcons(TextureAtlasRegister* atlas)
{
	for (int i = 0; i < SKULL_TYPE_COUNT; ++i)
	{
		sprites[i] = atlas->getAtlasSprite(getAtlasName() + "_" + spriteNames[i]);
	}
	cItem::registerIcons(atlas);
}

cItemCarrotOnAStick::cItemCarrotOnAStick(int id, const String& name, const String& atlas)
: ItemCarrotOnAStick(id, name)
, cItem(atlas)
{}

cItemSimpleFoiled::cItemSimpleFoiled(int id, const String& name, const String& atlas)
: ItemSimpleFoiled(id, name)
, cItem(atlas)
{}

cItemFirework::cItemFirework(int id, const String& name, const String& atlas)
: ItemFirework(id, name)
, cItem(atlas)
{}

void cItemFirework::addInformation(ItemStackPtr pStack, EntityPlayer* pPlayer, StringArray& fills, bool advance)
{
	if (!pStack->hasTagCompound())
		return;
	
	NBTTagCompound* pNbtFireworks = pStack->getTagCompound()->getCompoundTag("Fireworks");

	if (!pNbtFireworks)
		return;
		
	if (pNbtFireworks->hasKey("Flight"))
	{
		// todo.
		// fills.add(StatCollector.translateToLocal("item.fireworks.flight") + " " + pNbtFireworks.getByte("Flight"));
		fills.push_back(StringUtil::Format("item.fireworks.flight %d", pNbtFireworks->getByte("Flight")));
	}

	NBTTagList* pNbtExplosion = pNbtFireworks->getTagList("Explosions");

	if (pNbtExplosion && pNbtExplosion->tagCount() > 0)
	{
		for (int i = 0; i < pNbtExplosion->tagCount(); ++i)
		{
			NBTTagCompound* pNbtTag = (NBTTagCompound*)pNbtExplosion->tagAt(i);
			StringArray strArr;
			cItemFireworkCharge::addExplosionInfo(pNbtTag, strArr);

			if (!strArr.empty())
			{
				for (int j = 1; j < int(strArr.size()); ++j)
				{
					String temp = strArr[j];
					strArr[j] = "  " + temp;
				}

				fills.insert(fills.end(), strArr.begin(), strArr.end());
			}
		}
	}
}

cItemFireworkCharge::cItemFireworkCharge(int id, const String& name, const String& atlas)
: ItemFireworkCharge(id, name)
, cItem(atlas)
{}

AtlasSprite* cItemFireworkCharge::getIconFromDamageForRenderPass(int damage, int pass)
{
	if (pass > 0 && theIcon)
	{
		return theIcon;
	}
	return cItem::getIconFromDamageForRenderPass(damage, pass);
}

int cItemFireworkCharge::getColorFromItemStack(ItemStackPtr pStack, int par2)
{
	if (par2 != 1)
		return cItem::getColorFromItemStack(pStack, par2);
	
	NBTBase* pNbtBase = getExplosionTag(pStack, "Colors");

	if (!pNbtBase)
		return 0x8A8A8A;
	
	NBTTagIntArray* pNbtArr = (NBTTagIntArray*)pNbtBase;

	if (pNbtArr->arraySize == 1)
		return pNbtArr->intArray[0];

	int r = 0;
	int g = 0;
	int b = 0;
	const int* pIntArr = pNbtArr->intArray;
	int len = pNbtArr->arraySize;

	for (int i = 0; i < len; ++i)
	{
		int color = pIntArr[i];
		r += (color & 16711680) >> 16;
		g += (color & 65280) >> 8;
		b += (color & 255) >> 0;
	}

	r /= len;
	g /= len;
	b /= len;
	return r << 16 | g << 8 | b;
}

void cItemFireworkCharge::registerIcons(TextureAtlasRegister* atlas)
{
	cItem::registerIcons(atlas);
	theIcon = atlas->getAtlasSprite(getAtlasName() + "_overlay");
}

void cItemFireworkCharge::addInformation(ItemStackPtr pStack, EntityPlayer* pPlayer, StringArray& fills, bool advance)
{
	if (!pStack->hasTagCompound())
		return;

	NBTTagCompound* pNbtExplosion = pStack->getTagCompound()->getCompoundTag("Explosion");
	if (pNbtExplosion)
	{
		addExplosionInfo(pNbtExplosion, fills);
	}
}


NBTBase* cItemFireworkCharge::getExplosionTag(ItemStackPtr pStack, const String& key)
{
	if (pStack->hasTagCompound())
	{
		NBTTagCompound* pNbtExplosion = pStack->getTagCompound()->getCompoundTag("Explosion");

		if (pNbtExplosion)
		{
			return pNbtExplosion->getTag(key);
		}
	}

	return NULL;
}

void cItemFireworkCharge::addExplosionInfo(NBTTagCompound* pNbtCompound, StringArray& fills)
{
	int type = pNbtCompound->getByte("Type");

	if (type >= 0 && type <= 4)
	{
		// todo.
		// fills.add(StatCollector.translateToLocal("item.fireworksCharge.type." + type).trim());
		fills.push_back(StringUtil::Format("item.fireworksCharge.type.%d", type));
	}
	else
	{
		// todo.
		// fills.add(StatCollector.translateToLocal("item.fireworksCharge.type").trim());
		fills.push_back("item.fireworksCharge.type");
	}

	int* colors = pNbtCompound->getIntArray("Colors");
	int len = pNbtCompound->getArraySize("Colors");
	int var8;
	int var9;

	if (len > 0)
	{
		bool var4 = true;
		String var5 = "";
		int* var6 = colors;
		int var7 = len;

		for (var8 = 0; var8 < var7; ++var8)
		{
			var9 = var6[var8];

			if (!var4)
			{
				var5 = var5 + ", ";
			}

			var4 = false;
			bool var10 = false;

			for (int var11 = 0; var11 < 16; ++var11)
			{
				if (var9 == ItemDye::dyeColors[var11])
				{
					var10 = true;
					// todo.
					// var5 = var5 + StatCollector.translateToLocal("item.fireworksCharge." + ItemDye::dyeColorNames[var11]);
					var5 += "item.fireworksCharge.";
					var5 += ItemDye::dyeColorNames[var11];
					break;
				}
			}

			if (!var10)
			{
				// todo.
				// var5 = var5 + StatCollector.translateToLocal("item.fireworksCharge.customColor");
				var5 += "item.fireworksCharge.customColor";
			}
		}

		fills.push_back(var5);
	}

	int* var13 = pNbtCompound->getIntArray("FadeColors");
	len = pNbtCompound->getArraySize("FadeColors");
	bool var15;

	if (len > 0)
	{
		var15 = true;
		// todo.
		// String var14 = StatCollector.translateToLocal("item.fireworksCharge.fadeTo") + " ";
		String var14 = "item.fireworksCharge.fadeTo ";
		int* var16 = var13;
		var8 = len;

		for (var9 = 0; var9 < var8; ++var9)
		{
			int var18 = var16[var9];

			if (!var15)
			{
				var14 = var14 + ", ";
			}

			var15 = false;
			bool var19 = false;

			for (int var12 = 0; var12 < 16; ++var12)
			{
				if (var18 == ItemDye::dyeColors[var12])
				{
					var19 = true;
					// todo.
					// var14 = var14 + StatCollector.translateToLocal("item.fireworksCharge." + ItemDye::dyeColorNames[var12]);
					var14 += "item.fireworksCharge.";
					var14 += ItemDye::dyeColorNames[var12];
					break;
				}
			}

			if (!var19)
			{
				// todo.
				// var14 = var14 + StatCollector.translateToLocal("item.fireworksCharge.customColor");
				var14 += "item.fireworksCharge.customColor";
			}
		}

		fills.push_back(var14);
	}

	var15 = pNbtCompound->getBool("Trail");

	if (var15)
	{
		// todo.
		// fills.add(StatCollector.translateToLocal("item.fireworksCharge.trail"));
		fills.push_back("item.fireworksCharge.trail");
	}

	bool var17 = pNbtCompound->getBool("Flicker");

	if (var17)
	{
		// todo.
		// fills.add(StatCollector.translateToLocal("item.fireworksCharge.flicker"));
		fills.push_back("item.fireworksCharge.flicker");
	}
}

cItemLeash::cItemLeash(int id, const String& name, const String& atlas)
: ItemLeash(id, name)
, cItem(atlas)
{}

cItemNameTag::cItemNameTag(int id, const String& name, const String& atlas)
: ItemNameTag(id, name)
, cItem(atlas)
{}


cItemLeaves::cItemLeaves(int id, const String& name)
	: ItemLeaves(id, name)
	, cItemBlock(id)
{
}

AtlasSprite* cItemLeaves::getIconFromDamage(int damage)
{
	return cBlockManager::scBlocks[BLOCK_ID_LEAVES]->getIcon(0, damage);
}

int cItemLeaves::getColorFromItemStack(ItemStackPtr pStack, int par2)
{
	int damage = pStack->getItemDamage();	
	
	return	(damage & 1) == 1 ? 0x619961 :
			((damage & 2) == 2 ? 0x80A755 : 0x48B518);
}

cItemSlab::cItemSlab(int id, const String& name, Block* pHalfSlab, Block* pDoubleSlab, bool isFullBlock)
	:ItemSlab(id, name, pHalfSlab, pDoubleSlab, isFullBlock)
	, cItemBlock(id)
{
}

AtlasSprite* cItemSlab::getIconFromDamage(int damage)
{
	return cBlockManager::scBlocks[blockID]->getIcon(2, damage);
}

cItemMultiTextureTile::cItemMultiTextureTile(int id, const String& name, const StringArray& names, cBlock* pBlock)
	: ItemMultiTextureTile(id, name, names)
	, cItemBlock(id)
	, m_pModelBlock(pBlock)
{
}

AtlasSprite* cItemMultiTextureTile::getIconFromDamage(int damage)
{
	return m_pModelBlock->getIcon(2, damage);
}

cItemAnvilBlock::cItemAnvilBlock(int id, const String& name, cBlock * pBlock)
	: ItemAnvilBlock(id, name)
	, cItemBlock(id)
	, m_pModelBlock(pBlock)
{
}

AtlasSprite* cItemAnvilBlock::getIconFromDamage(int damage)
{
	return m_pModelBlock->getIcon(2, damage);
}

cItemPiston::cItemPiston(int id)
	: ItemPiston(id)
	, cItemBlock(id)
{
}

cItemBlockWithMetadata::cItemBlockWithMetadata(int id, cBlock * pBlock)
	: ItemBlockWithMetadata(id)
	, cItemBlock(id)
	, m_pModelBlock(pBlock)
{
}

AtlasSprite* cItemBlockWithMetadata::getIconFromDamage(int damage)
{
	return m_pModelBlock->getIcon(2, damage);
}

cItemSnow::cItemSnow(int id, cBlock* pBlock)
	: ItemSnow(id)
	, cItemBlock(id)
	, m_pModelBlock(pBlock)
{
}

AtlasSprite* cItemSnow::getIconFromDamage(int damage)
{
	return m_pModelBlock->getIcon(2, damage);
}

cItemCloth::cItemCloth(int id, const String& name)
	: ItemCloth(id, name)
	, cItemBlock(id)
{
}

AtlasSprite* cItemCloth::getIconFromDamage(int damage)
{
	return cBlockManager::scBlocks[BLOCK_ID_CLOTH]->getIcon(2, BlockColored::getBlockFromDye(damage));
}

cItemColored::cItemColored(int id)
	: cItemBlock(id)
{
	m_pModelBlock = cBlockManager::scBlocks[m_blockID];
}

AtlasSprite* cItemColored::getIconFromDamage(int damage)
{
	return m_pModelBlock->getIcon(0, damage);
}

int cItemColored::getColorFromItemStack(ItemStackPtr pStack, int par2)
{
	return m_pModelBlock->getRenderColor(pStack->getItemDamage());
}

cItemLilyPad::cItemLilyPad(int id)
	: ItemLilyPad(id)
	, cItemColored(id)
{
}

int cItemLilyPad::getColorFromItemStack(ItemStackPtr pStack, int par2)
{
	return cBlockManager::scBlocks[BLOCK_ID_WATERLILY]->getRenderColor(pStack->getItemDamage());
}

cItemBlockImp::cItemBlockImp(int id)
	: ItemBlock(id)
	, cItemBlock(id)
{
}

cItemColoredImp::cItemColoredImp(int id, bool flag, const StringArray& names)
	: ItemColored(id, flag, names)
	, cItemColored(id)
{
}

cItemColoredImp::cItemColoredImp(int id, bool flag)
	: ItemColored(id, flag)
	, cItemColored(id)
{
}

cItemColoredImp * cItemColoredImp::setBlockNames(String names[], int len)
{
	ItemColored::setBlockNames(names, len);
	return this;
}

cItemRecord::cItemRecord(int id, const String& name, const String& record, const String& atlas)
	: ItemRecord(id, name, record)
	, cItem(atlas)
{}

void cItemRecord::addInformation(ItemStackPtr pStack, EntityPlayer* pPlayer, StringArray& fills, bool advance)
{
	fills.push_back(getRecordTitle());
}

cItemCompass::cItemCompass(int id, const String& name, const String& atlas)
	: Item(id, name)
	, cItem(atlas)
{
	for (int i = 0; i < COMPASS_FRAME_COUNT; ++i)
		m_sprites[i] = nullptr;
}

void cItemCompass::registerIcons(TextureAtlasRegister* atlas)
{
	for (int i = 0; i < COMPASS_FRAME_COUNT; ++i)
	{
		m_sprites[i] = atlas->getAtlasSprite("compass.json", StringUtil::Format("%s_%d", getAtlasName().c_str(), i));
	}
}

cItemClock::cItemClock(int id, const String& name, const String& atlas)
	: Item(id, name)
	, cItem(atlas)
{
	for (int i = 0; i < CLOCK_FRAME_COUNT; ++i)
		m_sprites[i] = nullptr;
}

void cItemClock::registerIcons(TextureAtlasRegister* atlas)
{
	for (int i = 0; i < CLOCK_FRAME_COUNT; ++i)
	{
		m_sprites[i] = atlas->getAtlasSprite("clock.json", StringUtil::Format("%s_%d", getAtlasName().c_str(), i));
	}
}

cItemArrowPotion::cItemArrowPotion(int id, const String & name, const String & atlas)
	: ItemArrowPotion(id, name), cItem(atlas)
{
}

cItemKey::cItemKey(int id, const String & name, const String & atlas)
	: ItemKey(id, name)
	, cItem(atlas)
{
}

cItemTeleportScroll::cItemTeleportScroll(int id, const String & name, const String & atlas)
	: ItemTeleportScroll(id, name)
	, cItem(atlas)
{
}

cItemCreateBridge::cItemCreateBridge(int id, const String & name, const String & atlas)
	: ItemCreateBridge(id, name)
	, cItem(atlas)
{
}

cItemToolDigger::cItemToolDigger(int id, const String & name, const String & atlas, const String & meshName, ToolMaterial * toolMat)
	: ItemToolDigger(id, name, toolMat)
	, cItem(atlas)
	, m_meshName(meshName)
{
}

ItemRenderType cItemToolDigger::getRenderType()
{
	return ItemRenderType::MESH;
}

const String & cItemToolDigger::getMeshName() const
{
	return m_meshName;
}

cItemMedicine::cItemMedicine(int id, const String & name, const String & atlas)
	: ItemMedichine(id, name, 0.0f)
	, cItem(atlas)
{
}

cItemProp::cItemProp(int id, const String& name, const String& atlas, const String& mesh)
	: ItemProp(id, name)
	, cItem(atlas)
	, m_meshName(mesh)
{
}

ItemRenderType cItemProp::getRenderType()
{
	return getMeshName() == StringUtil::BLANK ? ItemRenderType::TEXTURE : ItemRenderType::MESH;
}

const String& cItemProp::getMeshName() const
{
	return m_meshName;
}

cItemSkillProp::cItemSkillProp(int id, const String& name, const String& atlas, const String& mesh)
	: ItemSkillProp(id, name)
	, cItem(atlas)
	, m_meshName(mesh)
{
}

ItemRenderType cItemSkillProp::getRenderType()
{
	return getMeshName() == StringUtil::BLANK ? ItemRenderType::TEXTURE : ItemRenderType::MESH;
}

const String& cItemSkillProp::getMeshName() const
{
	return m_meshName;
}

cItemSkillBook::cItemSkillBook(int id, const String& name, const String& atlas)
	: ItemSkillBook(id, name)
	, cItem(atlas)
{
}

ItemRenderType cItemSkillBook::getRenderType()
{
	return ItemRenderType::TEXTURE;
}


void cItemSkillBook::registerIcons(TextureAtlasRegister* atlas)
{
	itemIcon = atlas->getAtlasSprite("throwpot.json", getAtlasName());
}


cItemSkillBookFragmentation::cItemSkillBookFragmentation(int id, const String& name, const String& atlas)
	: ItemSkillBookFragmentation(id, name)
	, cItem(atlas)
{
}

ItemRenderType cItemSkillBookFragmentation::getRenderType()
{
	return  ItemRenderType::TEXTURE;
}


void cItemSkillBookFragmentation::registerIcons(TextureAtlasRegister* atlas)
{
	itemIcon = atlas->getAtlasSprite("throwpot.json", getAtlasName());
}

cItemSurpriseTreasureChest::cItemSurpriseTreasureChest(int id, const String& name, const String& atlas)
	: ItemSurpriseTreasureChest(id, name)
	, cItem(atlas)
{
}

ItemRenderType cItemSurpriseTreasureChest::getRenderType()
{
	return  ItemRenderType::TEXTURE;
}

void cItemSurpriseTreasureChest::registerIcons(TextureAtlasRegister* atlas)
{
	itemIcon = atlas->getAtlasSprite("throwpot.json", getAtlasName());
}

cItemBanner::cItemBanner(int id, const String& name)
	: ItemBanner(id, name)
	, cItemBlock(id)
{}

ItemRenderType cItemBanner::getRenderType()
{
	return ItemRenderType::BLOCK;
}

cItemAppIcon::cItemAppIcon(int id, const String & name, const String & atlas)
	: ItemAppIcon(id, name)
	, cItem(atlas)
{
}

cItemTransparent::cItemTransparent(int id, const String& name, const String& atlas)
	: ItemTransparent(id, name)
	, cItem(atlas)
{
}

cItemBuild::cItemBuild(int id, const String & name, const String & atlas)
	: ItemBuild(id, name)
	, cItem(atlas)
{
}

String cItemBuild::getUnlocalizedName(ItemStackPtr pStack)
{
	auto pSetting = BuildingSetting::getBuildingByItemId(itemID);
	String name = pSetting && pSetting->name.length() > 0 ? pSetting->name : getUnlocalizedName(pStack);
	return StringUtil::Replace(name, ".name", "");
}

void cItemBuild::registerIcons(TextureAtlasRegister * atlas)
{
	auto pSetting = BuildingSetting::getBuildingByItemId(itemID);
	String icon = pSetting && pSetting->icon.length() > 0 ? pSetting->icon : getAtlasName();
	itemIcon = atlas->getAtlasSprite("items.json", icon);
}

cItemHouse::cItemHouse(int id, const String & name, const String & atlas)
	: ItemHouse(id, name)
	, cItem(atlas)
{
}

String cItemHouse::getUnlocalizedName(ItemStackPtr pStack)
{
	HouseItem* pSetting = HouseSetting::getHouseItem(itemID);
	String name = pSetting && pSetting->name.length() > 0 ? pSetting->name : getUnlocalizedName(pStack);
	return StringUtil::Replace(name, ".name", "");
}

void cItemHouse::registerIcons(TextureAtlasRegister * atlas)
{
	HouseItem* pSetting = HouseSetting::getHouseItem(itemID);
	String icon = pSetting && pSetting->icon.length() > 0 ? pSetting->icon : getAtlasName();
	itemIcon = atlas->getAtlasSprite("items.json", icon);
}

cRanchSeeds::cRanchSeeds(int id, const String & name, const String & atlas, int blocktype, int soilID)
	: RanchSeeds(id, name, blocktype, soilID)
	, cItem(atlas)
{
}

void cRanchSeeds::registerIcons(TextureAtlasRegister * atlas)
{
	itemIcon = atlas->getAtlasSprite("items.json", getAtlasName());
}

cItemToolGather::cItemToolGather(int id, const String & name, const String & atlas, const String & meshName, ToolMaterial * toolMat)
	: ItemToolGather(id, name, toolMat)
	, cItem(atlas)
	, m_meshName(meshName)
{
}

ItemRenderType cItemToolGather::getRenderType()
{
	return ItemRenderType::MESH;
}

const String & cItemToolGather::getMeshName() const
{
	return m_meshName;
}

cItemFruitCurrency::cItemFruitCurrency(int id, const String & name, const String & atlas, const String & meshName)
	: ItemFruitCurrency(id, name)
	, cItem(atlas)
	, m_meshName(meshName)
{
}

ItemRenderType cItemFruitCurrency::getRenderType()
{
	return ItemRenderType::MESH;
}

void cItemFruitCurrency::registerIcons(TextureAtlasRegister * atlas)
{
	auto item = BirdFruitCurrencySetting::findBirdFruitCurrencyById(itemID);
	String icon = item && item->icon.length() > 0 ? item->icon : getAtlasName();
	itemIcon = atlas->getAtlasSprite("items.json", icon);
}

String cItemFruitCurrency::getUnlocalizedName(ItemStackPtr pStack)
{
	auto item = BirdFruitCurrencySetting::findBirdFruitCurrencyById(itemID);
	String name = item && item->name.length() > 0 ? item->name : getUnlocalizedName(pStack);
	return StringUtil::Replace(name, ".name", "");
}

const String & cItemFruitCurrency::getMeshName() const
{
	auto item = BirdFruitCurrencySetting::findBirdFruitCurrencyById(itemID);
	return item ? item ->meshName : m_meshName;
}

}
