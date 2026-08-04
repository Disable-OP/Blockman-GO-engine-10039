#include "BlockManager.h"
#include "Block.h"
#include "Blocks.h"
#include "BM_Material.h"
#include "BM_TypeDef.h"

#include "Item/Item.h"
#include "Item/Items.h"

namespace BLOCKMAN
{

Block**	BlockManager::sBlocks = NULL;
i8*		BlockManager::sOpaqueCubeLookup = NULL;
int*	BlockManager::sLightOpacity = NULL;
i8*		BlockManager::sCanBlockGrass = NULL;
int*	BlockManager::sLightValue = NULL;
i8*		BlockManager::sUseNeighborBrightness = NULL;

Block* BlockManager::stone = NULL;
Block* BlockManager::grass = NULL;
Block* BlockManager::dirt = NULL;
Block* BlockManager::cobblestone = NULL;
Block* BlockManager::planks = NULL;
Block* BlockManager::sapling = NULL;
Block* BlockManager::bedrock = NULL;
Block* BlockManager::waterMoving = NULL;
Block* BlockManager::waterStill = NULL;
Block* BlockManager::lavaMoving = NULL;
Block* BlockManager::lavaStill = NULL;
Block* BlockManager::sand = NULL;
Block* BlockManager::gravel = NULL;
Block* BlockManager::oreGold = NULL;
Block* BlockManager::oreIron = NULL;
Block* BlockManager::oreCoal = NULL;
Block* BlockManager::wood = NULL;
Block* BlockManager::leaves = NULL;
Block* BlockManager::sponge = NULL;
Block* BlockManager::glass = NULL;
Block* BlockManager::oreLapis = NULL;
Block* BlockManager::blockLapis = NULL;
Block* BlockManager::dispenser = NULL;
Block* BlockManager::sandStone = NULL;
Block* BlockManager::music = NULL;
Block* BlockManager::bed = NULL;
Block* BlockManager::railPowered = NULL;
Block* BlockManager::railDetector = NULL;
BlockPistonBase* BlockManager::pistonStickyBase = NULL;
Block* BlockManager::web = NULL;
Block* BlockManager::tallGrass = NULL;
Block* BlockManager::deadBush = NULL;
BlockPistonBase* BlockManager::pistonBase = NULL;
BlockPistonExtension* BlockManager::pistonExtension = NULL;
Block* BlockManager::cloth = NULL;
BlockPistonMoving* BlockManager::pistonMoving = NULL;
Block* BlockManager::plantYellow = NULL;
Block* BlockManager::plantRed = NULL;
Block* BlockManager::mushroomBrown = NULL;
Block* BlockManager::mushroomRed = NULL;
Block* BlockManager::blockGold = NULL;
Block* BlockManager::blockIron = NULL;
Block* BlockManager::stoneDoubleSlab = NULL;
Block* BlockManager::stoneSingleSlab = NULL;
Block* BlockManager::brick = NULL;
Block* BlockManager::tnt = NULL;
Block* BlockManager::bookShelf = NULL;
Block* BlockManager::cobblestoneMossy = NULL;
Block* BlockManager::obsidian = NULL;
Block* BlockManager::torchWood = NULL;
Block* BlockManager::fire = NULL;
Block* BlockManager::mobSpawner = NULL;
Block* BlockManager::stairsWoodOak = NULL;
Block* BlockManager::chest = NULL;
BlockRedstoneWire* BlockManager::redstoneWire = NULL;
Block* BlockManager::oreDiamond = NULL;
Block* BlockManager::blockDiamond = NULL;
Block* BlockManager::workbench = NULL;
Block* BlockManager::crops = NULL;
Block* BlockManager::tilledField = NULL;
Block* BlockManager::furnaceIdle = NULL;
Block* BlockManager::furnaceBurning = NULL;
Block* BlockManager::signPost = NULL;
Block* BlockManager::doorWood = NULL;
Block* BlockManager::ladder = NULL;
Block* BlockManager::rail = NULL;
Block* BlockManager::stairsCobblestone = NULL;
Block* BlockManager::signWall = NULL;
Block* BlockManager::lever = NULL;
Block* BlockManager::pressurePlateStone = NULL;
Block* BlockManager::doorIron = NULL;
Block* BlockManager::pressurePlatePlanks = NULL;
Block* BlockManager::oreRedstone = NULL;
Block* BlockManager::oreRedstoneGlowing = NULL;
Block* BlockManager::torchRedstoneIdle = NULL;
Block* BlockManager::torchRedstoneActive = NULL;
Block* BlockManager::stoneButton = NULL;
Block* BlockManager::snow = NULL;
Block* BlockManager::ice = NULL;
Block* BlockManager::blockSnow = NULL;
Block* BlockManager::cactus = NULL;
Block* BlockManager::blockClay = NULL;
Block* BlockManager::reed = NULL;
Block* BlockManager::jukebox = NULL;
Block* BlockManager::fence = NULL;
Block* BlockManager::pumpkin = NULL;
Block* BlockManager::netherrack = NULL;
Block* BlockManager::slowSand = NULL;
Block* BlockManager::glowStone = NULL;
Block* BlockManager::portal = NULL;
Block* BlockManager::pumpkinLantern = NULL;
Block* BlockManager::cake = NULL;
BlockRedstoneRepeater* BlockManager::redstoneRepeaterIdle = NULL;
BlockRedstoneRepeater* BlockManager::redstoneRepeaterActive = NULL;
Block* BlockManager::lockedChest = NULL;
Block* BlockManager::trapdoor = NULL;
Block* BlockManager::silverfish = NULL;
Block* BlockManager::stoneBrick = NULL;
Block* BlockManager::mushroomCapBrown = NULL;
Block* BlockManager::mushroomCapRed = NULL;
Block* BlockManager::fenceIron = NULL;
Block* BlockManager::thinGlass = NULL;
Block* BlockManager::melon = NULL;
Block* BlockManager::pumpkinStem = NULL;
Block* BlockManager::melonStem = NULL;
Block* BlockManager::vine = NULL;
Block* BlockManager::fenceGate = NULL;
Block* BlockManager::stairsBrick = NULL;
Block* BlockManager::stairsStoneBrick = NULL;
Block* BlockManager::mycelium = NULL;
Block* BlockManager::waterlily = NULL;
Block* BlockManager::netherBrick = NULL;
Block* BlockManager::netherFence = NULL;
Block* BlockManager::stairsNetherBrick = NULL;
Block* BlockManager::netherStalk = NULL;
Block* BlockManager::enchantmentTable = NULL;
Block* BlockManager::brewingStand = NULL;
Block* BlockManager::cauldron = NULL;
Block* BlockManager::endPortal = NULL;
Block* BlockManager::endPortalFrame = NULL;
Block* BlockManager::whiteStone = NULL;
Block* BlockManager::dragonEgg = NULL;
Block* BlockManager::redstoneLampIdle = NULL;
Block* BlockManager::redstoneLampActive = NULL;
Block* BlockManager::woodDoubleSlab = NULL;
Block* BlockManager::woodSingleSlab = NULL;
Block* BlockManager::cocoaPlant = NULL;
Block* BlockManager::stairsSandStone = NULL;
Block* BlockManager::oreEmerald = NULL;
Block* BlockManager::enderChest = NULL;
BlockTripWireSource* BlockManager::tripWireSource = NULL;
Block* BlockManager::tripWire = NULL;
Block* BlockManager::blockEmerald = NULL;
Block* BlockManager::stairsWoodSpruce = NULL;
Block* BlockManager::stairsWoodBirch = NULL;
Block* BlockManager::stairsWoodJungle = NULL;
Block* BlockManager::commandBlock = NULL;
Block* BlockManager::beacon = NULL;
Block* BlockManager::cobblestoneWall = NULL;
Block* BlockManager::flowerPot = NULL;
Block* BlockManager::carrot = NULL;
Block* BlockManager::potato = NULL;
Block* BlockManager::woodenButton = NULL;
Block* BlockManager::skull = NULL;
Block* BlockManager::anvil = NULL;
Block* BlockManager::chestTrapped = NULL;
Block* BlockManager::pressurePlateGold = NULL;
Block* BlockManager::pressurePlateIron = NULL;
BlockComparator* BlockManager::redstoneComparatorIdle = NULL;
BlockComparator* BlockManager::redstoneComparatorActive = NULL;
Block* BlockManager::daylightSensor = NULL;
Block* BlockManager::blockRedstone = NULL;
Block* BlockManager::oreNetherQuartz = NULL;
Block* BlockManager::hopperBlock = NULL;
Block* BlockManager::blockNetherQuartz = NULL;
Block* BlockManager::stairsNetherQuartz = NULL;
Block* BlockManager::railActivator = NULL;
Block* BlockManager::dropper = NULL;
Block* BlockManager::clayHardenedStained = NULL;
Block* BlockManager::wood2 = NULL;
Block* BlockManager::stairsWoodDarkOak = NULL;
Block* BlockManager::slime = NULL;
Block* BlockManager::seaLantern = NULL;
Block* BlockManager::hayBlock = NULL;
Block* BlockManager::woolCarpet = NULL;
Block* BlockManager::clayHardened = NULL;
Block* BlockManager::blockCoal = NULL;
Block* BlockManager::packedIce = NULL;
Block* BlockManager::railReceive = NULL;
Block* BlockManager::redSandstoneDoubleSlab = NULL;
Block* BlockManager::redSandstoneSingleSlab = NULL;
Block* BlockManager::roadCarpet = NULL;
BlockDoor* BlockManager::keyDoor_01 = NULL;
BlockDoor* BlockManager::keyDoor_02 = NULL;
BlockDoor* BlockManager::keyDoor_03 = NULL;
BlockDoor* BlockManager::keyDoor_04 = NULL;
BlockDoor* BlockManager::keyDoor_05 = NULL;
BlockDoor* BlockManager::keyDoor_06 = NULL;
BlockDoor* BlockManager::keyDoor_07 = NULL;
BlockDoor* BlockManager::keyDoor_08 = NULL;
BlockDoor* BlockManager::keyDoor_09 = NULL;
BlockDoor* BlockManager::keyDoor_10 = NULL;
BlockWithEffect* BlockManager::blockWithEffect = NULL;
BlockWithEffect* BlockManager::blockWithEffect_02 = NULL;
BlockWithEffect* BlockManager::blockWithEffect_03 = NULL;
BlockWithEffect* BlockManager::blockWithEffect_04 = NULL;
Block* BlockManager::pressurePlateWeightWood = NULL;
Block* BlockManager::chestNew = NULL;
Block* BlockManager::enderChestNew = NULL;
Block* BlockManager::chestTrappedNew = NULL;
BlockNewCrops* BlockManager::newCrops_01 = NULL;
BlockNewCrops* BlockManager::newCrops_02 = NULL;
BlockNewCrops* BlockManager::newCrops_03 = NULL;
BlockNewCrops* BlockManager::newCrops_04 = NULL;
BlockNewCrops* BlockManager::newCrops_05 = NULL;
Block* BlockManager::concrete = NULL;
Block* BlockManager::concrete_powder = NULL;
Block* BlockManager::air_wall = NULL;
Block* BlockManager::stained_glass = NULL;

Block* BlockManager::banner_00 = NULL;
Block* BlockManager::banner_01 = NULL;
Block* BlockManager::banner_02 = NULL;
Block* BlockManager::banner_03 = NULL;
Block* BlockManager::banner_04 = NULL;
Block* BlockManager::banner_05 = NULL;
Block* BlockManager::banner_06 = NULL;
Block* BlockManager::banner_07 = NULL;
Block* BlockManager::banner_08 = NULL;
Block* BlockManager::banner_09 = NULL;
Block* BlockManager::banner_10 = NULL;
Block* BlockManager::banner_11 = NULL;
Block* BlockManager::banner_12 = NULL;
Block* BlockManager::banner_13 = NULL;
Block* BlockManager::banner_14 = NULL;
Block* BlockManager::banner_15 = NULL;

BlockNewCrops* BlockManager::ranchCrop_100001 = NULL;
BlockNewCrops* BlockManager::ranchCrop_100002 = NULL;
BlockNewCrops* BlockManager::ranchCrop_100003 = NULL;
BlockNewCrops* BlockManager::ranchCrop_100004 = NULL;
BlockNewCrops* BlockManager::ranchCrop_100005 = NULL;
BlockNewCrops* BlockManager::ranchCrop_100006 = NULL;
BlockNewCrops* BlockManager::ranchCrop_100007 = NULL;
BlockNewCrops* BlockManager::ranchCrop_100008 = NULL;
BlockNewCrops* BlockManager::ranchCrop_100009 = NULL;
BlockNewCrops* BlockManager::ranchCrop_100010 = NULL;
BlockNewCrops* BlockManager::ranchCrop_100011 = NULL;
BlockNewCrops* BlockManager::ranchCrop_100012 = NULL;
BlockNewCrops* BlockManager::ranchCrop_100013 = NULL;
BlockNewCrops* BlockManager::ranchCrop_100014 = NULL;
BlockNewCrops* BlockManager::ranchCrop_100015 = NULL;
BlockNewCrops* BlockManager::ranchCrop_100016 = NULL;
BlockNewCrops* BlockManager::ranchCrop_100017 = NULL;

BlockNest*  BlockManager::nest_01 = NULL;
BlockNest*  BlockManager::nest_02 = NULL;

BlockFruits*  BlockManager::fruits_01 = NULL;
BlockFruits*  BlockManager::fruits_02 = NULL;
BlockFruits*  BlockManager::fruits_03 = NULL;
BlockFruits*  BlockManager::fruits_04 = NULL;
BlockFruits*  BlockManager::fruits_05 = NULL;
BlockFruits*  BlockManager::fruits_06 = NULL;
BlockFruits*  BlockManager::fruits_07 = NULL;
BlockFruits*  BlockManager::fruits_08 = NULL;
BlockFruits*  BlockManager::fruits_09 = NULL;
BlockFruits*  BlockManager::fruits_10 = NULL;
BlockFruits*  BlockManager::fruits_11 = NULL;
BlockFruits*  BlockManager::fruits_12 = NULL;

Block* BlockManager::transparency = NULL;

void BlockManager::unInitialize()
{
	LordSafeFree(sUseNeighborBrightness);
	LordSafeFree(sLightValue);
	LordSafeFree(sCanBlockGrass);
	LordSafeFree(sLightOpacity);
	LordSafeFree(sOpaqueCubeLookup);

	Block* pBlock = NULL;
	for (int i = 0; i < MAX_BLOCK_COUNT; ++i)
	{
		pBlock = sBlocks[i];
		if (!pBlock)
			continue;
		LordDelete(pBlock);
	}
	LordSafeFree(sBlocks);
}

void BlockManager::initialize()
{
	LordAssert(!sBlocks);
	LordAssert(!sOpaqueCubeLookup);
	LordAssert(!sLightOpacity);
	LordAssert(!sCanBlockGrass);
	LordAssert(!sLightValue);
	LordAssert(!sUseNeighborBrightness);

	sBlocks = (Block**)LordMalloc(sizeof(Block*) * MAX_BLOCK_COUNT);
	sOpaqueCubeLookup = (i8*)LordMalloc(sizeof(i8) * MAX_BLOCK_COUNT);
	sLightOpacity = (int*)LordMalloc(sizeof(int) * MAX_BLOCK_COUNT);
	sCanBlockGrass = (i8*)LordMalloc(sizeof(i8) * MAX_BLOCK_COUNT);
	sLightValue = (int*)LordMalloc(sizeof(int) * MAX_BLOCK_COUNT);
	sUseNeighborBrightness = (i8*)LordMalloc(sizeof(i8) * MAX_BLOCK_COUNT);
	
	memset(sBlocks, 0, sizeof(Block*) * MAX_BLOCK_COUNT);
	memset(sOpaqueCubeLookup, 0, sizeof(i8) * MAX_BLOCK_COUNT);
	memset(sLightOpacity, 0, sizeof(int) * MAX_BLOCK_COUNT);
	memset(sCanBlockGrass, 0, sizeof(i8) * MAX_BLOCK_COUNT);
	memset(sLightValue, 0, sizeof(int) * MAX_BLOCK_COUNT);
	memset(sUseNeighborBrightness, 0, sizeof(i8) * MAX_BLOCK_COUNT);

	stone =			LordNew Block(1, BM_Material::BM_MAT_rock, "stone"); stone->setHardness(3.0f); stone->setResistance(15.f);
	grass =			LordNew BlockGrass(2, BM_Material::BM_MAT_grass, "grass"); grass->setHardness(0.6f); 
	//dirt =			LordNew Block(3, BM_Material::BM_MAT_ground, "dirt"); dirt->setHardness(0.5f); 
	dirt            = LordNew BlockDirt(3, BM_Material::BM_MAT_ground, "dirt"); dirt->setHardness(0.5f);
	cobblestone =	LordNew Block(4, BM_Material::BM_MAT_rock, "stonebrick"); cobblestone->setHardness(2.0F);  cobblestone->setResistance(10.0F); 
	planks =		LordNew BlockWood(5, BM_Material::BM_MAT_wood, "wood");  planks->setHardness(2.0F); planks->setResistance(5.0F);
	sapling =		LordNew BlockSapling(6, BM_Material::BM_MAT_plants, "sapling"); sapling->setHardness(0.0F); sapling->setLightOpacity(0);
	bedrock =		LordNew Block(7, BM_Material::BM_MAT_rock, "bedrock"); bedrock->setHardness(-1.0f); bedrock->setResistance(6000000.0F); bedrock->disableStats();
	waterMoving =	LordNew BlockFlowing(8, BM_Material::BM_MAT_water, "water"); waterMoving->setHardness(100.0F); waterMoving->setLightOpacity(3);
	waterStill =	LordNew BlockStationary(9, BM_Material::BM_MAT_water, "water"); waterStill->setHardness(100.0F); waterStill->setLightOpacity(3); waterStill->disableStats();
	lavaMoving =	LordNew BlockFlowing(10, BM_Material::BM_MAT_lava, "lava"); lavaMoving->setHardness(0.0F); lavaMoving->setLightValue(1.0F); lavaMoving->disableStats(); lavaMoving->setLightOpacity(0);
	lavaStill =		LordNew BlockStationary(11, BM_Material::BM_MAT_lava, "lava"); lavaStill->setHardness(100.0F); lavaStill->setLightValue(1.0F); lavaStill->disableStats(); lavaStill->setLightOpacity(0);
	sand =			LordNew BlockSand(12, BM_Material::BM_MAT_sand, "sand"); sand->setHardness(0.5F);
	gravel =		LordNew BlockSand(13, BM_Material::BM_MAT_sand, "gravel"); gravel->setHardness(0.6F);
	oreGold =		LordNew BlockOre(14, BM_Material::BM_MAT_rock, "oreGold"); oreGold->setHardness(3.0F); oreGold->setResistance(5.0F);
	oreIron =		LordNew BlockOre(15, BM_Material::BM_MAT_rock, "oreIron"); oreIron->setHardness(3.0F); oreIron->setResistance(5.0F);
	oreCoal =		LordNew BlockOre(16, BM_Material::BM_MAT_rock, "oreCoal"); oreCoal->setHardness(3.0F); oreCoal->setResistance(5.0F);
	wood =			LordNew BlockLog1(17, BM_Material::BM_MAT_wood, "log"); wood->setHardness(2.0F);
	leaves =		LordNew BlockLeaves(18, BM_Material::BM_MAT_leaves, "leaves"); leaves->setHardness(0.2F); leaves->setLightOpacity(1);
	sponge =		LordNew Block(19, BM_Material::BM_MAT_sponge, "sponge"); sponge->setHardness(0.6F);
	glass =			LordNew BlockGlass(20, BM_Material::BM_MAT_glass, "glass"); glass->setHardness(0.3F); glass->setLightOpacity(0);
	oreLapis =		LordNew BlockOre(21, BM_Material::BM_MAT_rock, "oreLapis"); oreLapis->setHardness(3.0F); oreLapis->setResistance(5.0F);
	blockLapis =	LordNew Block(22, BM_Material::BM_MAT_rock, "blockLapis"); blockLapis->setHardness(3.0F); blockLapis->setResistance(5.0F);
	dispenser =		LordNew BlockDispenser(23, BM_Material::BM_MAT_rock, "dispenser"); dispenser->setHardness(3.5F);
	sandStone =		LordNew BlockSandStone(24, BM_Material::BM_MAT_rock, "sandstone"); sandStone->setHardness(0.8F);
	music =			LordNew BlockNote(25, BM_Material::BM_MAT_wood, "musicBlock"); music->setHardness(0.8F);
	bed =			LordNew BlockBed(26, "bed"); bed->setHardness(0.2F);
	railPowered =	LordNew BlockRailPowered(27, BM_Material::BM_MAT_circuits, "goldenRail"); railPowered->setHardness(0.7F);
	railDetector =	LordNew BlockDetectorRail(28, BM_Material::BM_MAT_circuits, "detectorRail"); railDetector->setHardness(0.7F);
	pistonStickyBase=LordNew BlockPistonBase(29, BM_Material::BM_MAT_piston, "pistonStickyBase", true); pistonStickyBase->setHardness(0.5f);
	web =			LordNew BlockWeb(30, BM_Material::BM_MAT_web, "web"); web->setLightOpacity(1); web->setHardness(4.0F);
	tallGrass =		LordNew BlockTallGrass(31, BM_Material::BM_MAT_vine, "tallgrass"); tallGrass->setHardness(0.0F);
	deadBush =		LordNew BlockDeadBush(32, BM_Material::BM_MAT_vine, "deadbush"); deadBush->setHardness(0.0F);
	pistonBase =	LordNew BlockPistonBase(33, BM_Material::BM_MAT_piston, "pistonBase", false); pistonBase->setHardness(0.5f);
	pistonExtension=LordNew BlockPistonExtension(34, BM_Material::BM_MAT_piston, "pistonExtension"); pistonExtension->setHardness(0.5f);
	cloth =			LordNew BlockColored(35, BM_Material::BM_MAT_cloth, "cloth"); cloth->setHardness(0.8F);
	pistonMoving =	LordNew BlockPistonMoving(36, BM_Material::BM_MAT_piston, "pistonMoving");
	plantYellow =	LordNew BlockFlower(37, BM_Material::BM_MAT_plants, "flower"); plantYellow->setHardness(0.0F);
	plantRed =		LordNew BlockFlower(38, BM_Material::BM_MAT_plants, "rose"); plantRed->setHardness(0.0F);
	mushroomBrown =	LordNew BlockMushroom(39, BM_Material::BM_MAT_plants, "mushroom"); mushroomBrown->setHardness(0.0F); mushroomBrown->setLightValue(0.125F);
	mushroomRed =	LordNew BlockMushroom(40, BM_Material::BM_MAT_plants, "mushroom"); mushroomRed->setHardness(0.0F);
	blockGold =		LordNew BlockOreStorage(41, BM_Material::BM_MAT_iron, "blockGold"); blockGold->setHardness(3.0F); blockGold->setResistance(10.0F);
	blockIron =		LordNew BlockOreStorage(42, BM_Material::BM_MAT_iron, "blockIron"); blockIron->setHardness(5.0F); blockIron->setResistance(10.0F);
	stoneDoubleSlab=LordNew BlockStep(43, BM_Material::BM_MAT_rock, "stoneSlab", true); stoneDoubleSlab->setHardness(2.0F); stoneDoubleSlab->setResistance(10.0F);
	stoneSingleSlab=LordNew BlockStep(44, BM_Material::BM_MAT_rock, "stoneSlab", false); stoneSingleSlab->setHardness(2.0F); stoneSingleSlab->setResistance(10.0F);
	brick =			LordNew Block(45, BM_Material::BM_MAT_rock, "brick"); brick->setHardness(2.0F); brick->setResistance(10.0F);
	tnt =			LordNew BlockTNT(46, BM_Material::BM_MAT_tnt, "tnt"); tnt->setHardness(0.0F);
	bookShelf =		LordNew BlockBookshelf(47, BM_Material::BM_MAT_wood, "bookshelf"); bookShelf->setHardness(1.5F);
	cobblestoneMossy=LordNew Block(48, BM_Material::BM_MAT_rock, "stoneMoss"); cobblestoneMossy->setHardness(2.0F); cobblestoneMossy->setResistance(10.0F);
	obsidian =		LordNew Block(49, BM_Material::BM_MAT_rock, "obsidian"); obsidian->setHardness(50.0F); obsidian->setResistance(2000.0F);
	torchWood =		LordNew BlockTorch(50, BM_Material::BM_MAT_circuits, "torch"); torchWood->setHardness(0.0F); torchWood->setLightValue(0.9375F);
	fire =			LordNew BlockFire(51, BM_Material::BM_MAT_fire, "fire"); fire->setHardness(0.0F); fire->setLightValue(1.0F); 
	mobSpawner =	LordNew BlockMobSpawner(52, BM_Material::BM_MAT_rock, "mobSpawner");  mobSpawner->setHardness(5.0F); mobSpawner->setLightOpacity(0);
	stairsWoodOak =	LordNew BlockStairs(53, BM_Material::BM_MAT_wood, "stairsWood", planks, 0); stairsWoodOak->setHardness(2.0f); stairsWoodOak->setResistance(5.0F);
	chest =			LordNew BlockChest(54, BM_Material::BM_MAT_wood, "chest", BlockChest::BTT_BLOCK_NOT_TRAPPED); chest->setHardness(2.5F);
	redstoneWire =	LordNew BlockRedstoneWire(55, BM_Material::BM_MAT_circuits, "redstoneDust"); redstoneWire->setHardness(0.0F);
	oreDiamond =	LordNew BlockOre(56, BM_Material::BM_MAT_rock, "oreDiamond"); oreDiamond->setHardness(3.0F);  oreDiamond->setResistance(5.0F);
	blockDiamond =	LordNew BlockOreStorage(57, BM_Material::BM_MAT_iron, "blockDiamond"); blockDiamond->setHardness(5.0F); blockDiamond->setResistance(10.0F);
	workbench =		LordNew BlockWorkbench(58, BM_Material::BM_MAT_wood, "workbench"); workbench->setHardness(2.5F);
	crops =			LordNew BlockCrops(59, BM_Material::BM_MAT_plants, "crops"); crops->setHardness(0.f);
	tilledField =	LordNew BlockFarmland(60, BM_Material::BM_MAT_ground, "farmland");//(new BlockFarmland(60)).setHardness(0.6F).setStepSound(soundGravelFootstep).setUnlocalizedName("farmland").func_111022_d("farmland");
	furnaceIdle =	LordNew BlockFurnace(61, BM_Material::BM_MAT_rock, "furnace", false); furnaceIdle->setHardness(3.5F);
	furnaceBurning =LordNew BlockFurnace(62, BM_Material::BM_MAT_rock, "furnace", true); furnaceBurning->setHardness(3.5F); furnaceBurning->setLightValue(0.875F);
	signPost =		LordNew BlockSign(63, BM_Material::BM_MAT_wood, "sign", 0, true); signPost->setHardness(1.0F); signPost->disableStats();
	doorWood =		LordNew BlockDoor(64, BM_Material::BM_MAT_wood, "doorWood"); doorWood->setHardness(3.0F); doorWood->disableStats(); 
	ladder =		LordNew BlockLadder(65, BM_Material::BM_MAT_circuits, "ladder"); ladder->setHardness(0.4F); 
	rail =			LordNew BlockRail(66, BM_Material::BM_MAT_circuits, "rail"); rail->setHardness(0.7F);
	stairsCobblestone = LordNew BlockStairs(67, BM_Material::BM_MAT_rock, "stairsStone", cobblestone, 0); stairsCobblestone->setHardness(2.0f); stairsCobblestone->setResistance(10.0f); //(new BlockStairs(67, cobblestone, 0)).setUnlocalizedName("stairsStone");
	signWall =		LordNew BlockSign(68, BM_Material::BM_MAT_wood, "sign", 1, false); signWall->setHardness(1.0F); signWall->disableStats();
	lever =			LordNew BlockLever(69, BM_Material::BM_MAT_circuits, "lever"); lever->setHardness(0.5F);
	pressurePlateStone =	LordNew BlockPressurePlate(70, BM_Material::BM_MAT_rock, "pressurePlate", MOB_TYPE_PLAYERS); pressurePlateStone->setHardness(0.5F);
	doorIron =		LordNew BlockDoor(71, BM_Material::BM_MAT_iron, "doorIron"); doorIron->setHardness(5.0F); doorIron->disableStats(); 
	pressurePlatePlanks = LordNew BlockPressurePlate(72, BM_Material::BM_MAT_wood, "pressurePlate", MOB_TYPE_PLAYERS); pressurePlatePlanks->setHardness(0.5F);
	oreRedstone =	LordNew BlockRedstoneOre(73, BM_Material::BM_MAT_rock, "oreRedstone", false);  oreRedstone->setHardness(3.0F); oreRedstone->setResistance(5.0F);
	oreRedstoneGlowing = LordNew BlockRedstoneOre(74, BM_Material::BM_MAT_rock, "oreRedstone", true); oreRedstoneGlowing->setHardness(3.0F); oreRedstoneGlowing->setResistance(5.0F); oreRedstoneGlowing->setLightValue(0.625F);
	torchRedstoneIdle =	LordNew BlockRedstoneTorch(75, BM_Material::BM_MAT_circuits, "notGate", false); torchRedstoneIdle->setHardness(0.0F);
	torchRedstoneActive = LordNew BlockRedstoneTorch(76, BM_Material::BM_MAT_circuits, "notGate", true); torchRedstoneActive->setHardness(0.0F); torchRedstoneActive->setLightValue(0.5F);//torchRedstoneActive->setCreativeTab(CreativeTabs::tabRedstone);
	stoneButton =	LordNew BlockButtonStone(77, BM_Material::BM_MAT_circuits, "button"); stoneButton->setHardness(0.5F); 
	snow = LordNew BlockSnow(78, BM_Material::BM_MAT_snow, "snow"); snow->setHardness(0.1F); snow->setLightOpacity(0);
	ice =			LordNew BlockIce(79, BM_Material::BM_MAT_ice, "ice"); ice->setHardness(0.5F); ice->setLightOpacity(3); 
	blockSnow =		LordNew BlockSnowBlock(80, BM_Material::BM_MAT_craftedSnow, "snow"); blockSnow->setHardness(0.2F);//(new BlockSnowBlock(80)).setHardness(0.2F).setStepSound(soundSnowFootstep).setUnlocalizedName("snow").func_111022_d("snow");
	cactus =		LordNew BlockCactus(81, BM_Material::BM_MAT_cactus, "cactus"); cactus->setHardness(0.4F);
	blockClay =		LordNew BlockClay(82, BM_Material::BM_MAT_clay, "clay"); blockClay->setHardness(0.6F); 
	reed =			LordNew BlockReed(83, BM_Material::BM_MAT_plants, "reeds"); reed->setHardness(0.0F);
	jukebox =		LordNew BlockJukeBox(84, BM_Material::BM_MAT_wood, "jukebox"); jukebox->setHardness(2.0F); jukebox->setResistance(10.0F);
	fence =			LordNew BlockFence(85, BM_Material::BM_MAT_wood, "fence"); fence->setHardness(2.0F); fence->setResistance(5.0F); 
	pumpkin =		LordNew BlockPumpkin(86, BM_Material::BM_MAT_pumpkin, "pumpkin", false); pumpkin->setHardness(1.0F);
	netherrack =	LordNew Block(87, BM_Material::BM_MAT_rock, "hellrock"); netherrack->setHardness(0.4F);
	slowSand =		LordNew BlockSoulSand(88, BM_Material::BM_MAT_sand, "hellsand"); slowSand->setHardness(0.5F);
	glowStone =		LordNew BlockGlowStone(89, BM_Material::BM_MAT_glass, "lightgem"); glowStone->setHardness(0.3F); glowStone->setLightValue(1.f);
	portal =		LordNew BlockPortal(90, BM_Material::BM_MAT_portal, "portal"); portal->setHardness(-1.0F); portal->setLightValue(0.75F);
	pumpkinLantern =LordNew BlockPumpkin(91, BM_Material::BM_MAT_pumpkin, "litpumpkin", true); pumpkinLantern->setHardness(1.0F); pumpkinLantern->setLightValue(1.0F);
	cake =			LordNew BlockCake(92, BM_Material::BM_MAT_cake, "cake"); cake->setHardness(0.5F); 
	redstoneRepeaterIdle = LordNew BlockRedstoneRepeater(93, BM_Material::BM_MAT_circuits, "diode", false); redstoneRepeaterIdle->setHardness(0.0F); redstoneRepeaterIdle->disableStats();
	redstoneRepeaterActive = LordNew BlockRedstoneRepeater(94, BM_Material::BM_MAT_circuits, "diode", true); redstoneRepeaterActive->setHardness(0.0F); redstoneRepeaterActive->setLightValue(0.625F); redstoneRepeaterActive->disableStats();
	lockedChest =	LordNew Block(95, BM_Material::BM_MAT_wood, "lockedchest"); lockedChest->setHardness(0.0F); lockedChest->setLightValue(1.0F); lockedChest->setTickRandomly(true);
	trapdoor =		LordNew BlockTrapDoor(96, BM_Material::BM_MAT_wood, "trapdoor"); trapdoor->setHardness(3.0F);
	silverfish =	LordNew BlockSilverfish(97, BM_Material::BM_MAT_clay, "monsterStoneEgg"); silverfish->setHardness(0.75F);
	stoneBrick =	LordNew BlockStoneBrick(98, BM_Material::BM_MAT_rock, "stonebricksmooth"); stoneBrick->setHardness(1.5F); stoneBrick->setResistance(10.0F);
	mushroomCapBrown=LordNew BlockMushroomCap(99, BM_Material::BM_MAT_wood, "mushroom", 0); mushroomCapBrown->setHardness(0.2F);
	mushroomCapRed = LordNew BlockMushroomCap(100, BM_Material::BM_MAT_wood, "mushroom", 1); mushroomCapRed->setHardness(0.2F);
	fenceIron =		LordNew BlockPane(101, BM_Material::BM_MAT_iron, "fenceIron", true); fenceIron->setHardness(5.0F); fenceIron->setResistance(10.0F);
	thinGlass =		LordNew BlockPane(102, BM_Material::BM_MAT_glass, "thinGlass", false);
	melon =			LordNew BlockMelon(103, BM_Material::BM_MAT_pumpkin, "melon"); melon->setHardness(1.0F);
	pumpkinStem =	LordNew BlockStem(104, BM_Material::BM_MAT_plants, "pumpkinStem", pumpkin); pumpkinStem->setHardness(0.0F);//(new BlockStem(104, pumpkin)).setHardness(0.0F).setStepSound(soundWoodFootstep).setUnlocalizedName("pumpkinStem").func_111022_d("pumpkin_stem");
	melonStem =		LordNew BlockStem(105, BM_Material::BM_MAT_plants, "pumpkinStem", melon); melonStem->setHardness(0.0F);//(new BlockStem(105, melon)).setHardness(0.0F).setStepSound(soundWoodFootstep).setUnlocalizedName("pumpkinStem").func_111022_d("melon_stem");
	vine =			LordNew BlockVine(106, BM_Material::BM_MAT_vine, "vine"); vine->setHardness(0.2F); vine->setLightOpacity(0);
	fenceGate =		LordNew BlockFenceGate(107, BM_Material::BM_MAT_wood, "fenceGate"); fenceGate->setHardness(2.0F); fenceGate->setResistance(5.0F);
	stairsBrick =	LordNew BlockStairs(108, BM_Material::BM_MAT_rock, "stairsBrick", brick, 0); stairsBrick->setHardness(2.0f); stairsBrick->setResistance(10.0f);
	stairsStoneBrick=LordNew	BlockStairs(109, BM_Material::BM_MAT_rock, "stairsStoneBrickSmooth", stoneBrick, 0);  stairsStoneBrick->setHardness(1.5f); stairsStoneBrick->setResistance(10.0f);
	mycelium =		LordNew BlockMycelium(110, BM_Material::BM_MAT_grass, "mycel"); mycelium->setHardness(0.6F);
	waterlily =		LordNew BlockLilyPad(111, BM_Material::BM_MAT_plants, "waterlily"); waterlily->setHardness(0.0F);
	netherBrick =	LordNew Block(112, BM_Material::BM_MAT_rock, "netherBrick"); netherBrick->setHardness(2.0F); netherBrick->setResistance(10.0F);
	netherFence =	LordNew BlockFence(113, BM_Material::BM_MAT_rock, "netherFence"); netherFence->setHardness(2.0F); netherFence->setResistance(10.0F);
	stairsNetherBrick = LordNew BlockStairs(114, BM_Material::BM_MAT_rock, "stairsNetherBrick", netherBrick, 0); stairsNetherBrick->setHardness(2.0F); stairsNetherBrick->setResistance(10.0F);
	netherStalk =	LordNew BlockNetherStalk(115, BM_Material::BM_MAT_plants, "netherStalk");
	enchantmentTable=LordNew  BlockEnchantmentTable(116, BM_Material::BM_MAT_rock, "enchantmentTable"); enchantmentTable->setHardness(5.0F); enchantmentTable->setResistance(2000.0F);
	brewingStand =	LordNew BlockBrewingStand(117, BM_Material::BM_MAT_iron, "brewingStand"); brewingStand->setHardness(0.5F); brewingStand->setLightValue(0.125F);
	cauldron =		LordNew BlockCauldron(118, BM_Material::BM_MAT_iron, "cauldron"); cauldron->setHardness(2.0F);
	endPortal = LordNew BlockEndPortal(119, BM_Material::BM_MAT_portal, "portal"); endPortal->setHardness(-1.0F); endPortal->setLightValue(1.f);  endPortal->setResistance(6000000.0F);
	endPortalFrame = LordNew BlockEndPortalFrame(120, BM_Material::BM_MAT_rock, "endPortalFrame"); endPortalFrame->setHardness(-1.0F); endPortalFrame->setResistance(6000000.0F); endPortalFrame->setLightValue(0.125F); //setCreativeTab(CreativeTabs.tabDecorations).
	whiteStone =	LordNew Block(121, BM_Material::BM_MAT_rock, "whiteStone"); whiteStone->setHardness(3.0F); whiteStone->setResistance(15.0F);
	dragonEgg =		LordNew BlockDragonEgg(122, BM_Material::BM_MAT_dragonEgg, "dragonEgg"); dragonEgg->setHardness(3.0F); dragonEgg->setResistance(15.0F); dragonEgg->setLightValue(0.125F);
	redstoneLampIdle = LordNew BlockRedstoneLight(123, BM_Material::BM_MAT_redstoneLight, "redstoneLight", false); redstoneLampIdle->setHardness(0.3F);
	redstoneLampActive = LordNew BlockRedstoneLight(124, BM_Material::BM_MAT_redstoneLight, "redstoneLight", true); redstoneLampActive->setHardness(0.3F); redstoneLampActive->setLightValue(1.f);
	woodDoubleSlab = LordNew BlockWoodSlab(125, BM_Material::BM_MAT_wood, "woodSlab", true); woodDoubleSlab->setHardness(2.0F); woodDoubleSlab->setResistance(5.0F);//(BlockHalfSlab)(new BlockWoodSlab(125, true)).setHardness(2.0F).setResistance(5.0F).setStepSound(soundWoodFootstep).setUnlocalizedName("woodSlab");
	woodSingleSlab = LordNew BlockWoodSlab(126, BM_Material::BM_MAT_wood, "woodslab", false); woodSingleSlab->setHardness(2.0F); woodSingleSlab->setResistance(5.0F);//(BlockHalfSlab)(new BlockWoodSlab(126, false)).setHardness(2.0F).setResistance(5.0F).setStepSound(soundWoodFootstep).setUnlocalizedName("woodSlab");
	cocoaPlant =	 LordNew BlockCocoa(127, BM_Material::BM_MAT_plants, "cocoa"); cocoaPlant->setHardness(0.2F); cocoaPlant->setResistance(5.0F);
	stairsSandStone = LordNew BlockStairs(128, BM_Material::BM_MAT_rock, "stairsSandStone", sandStone, 0); stairsSandStone->setHardness(0.8f); //(new BlockStairs(128, sandStone, 0)).setUnlocalizedName("stairsSandStone");	
	oreEmerald =	LordNew BlockOre(129, BM_Material::BM_MAT_rock, "oreEmerald");  oreEmerald->setHardness(3.0F); oreEmerald->setResistance(5.0F);
	enderChest = LordNew BlockEnderChest(130, BM_Material::BM_MAT_rock, "enderChest", BlockEnderChest::BECT_OLD); enderChest->setHardness(22.5F); enderChest->setResistance(1000.0F); enderChest->setLightValue(0.5F);
	tripWireSource = LordNew BlockTripWireSource(131, BM_Material::BM_MAT_circuits, "tripWireSource");//(BlockTripWireSource)(new BlockTripWireSource(131)).setUnlocalizedName("tripWireSource").func_111022_d("trip_wire_source");
	tripWire =		LordNew BlockTripWire(132, BM_Material::BM_MAT_circuits, "tripWire");
	blockEmerald =	LordNew BlockOreStorage(133, BM_Material::BM_MAT_iron, "blockEmerald"); blockEmerald->setHardness(5.0F); blockEmerald->setResistance(10.0F);
	stairsWoodSpruce = LordNew BlockStairs(134, BM_Material::BM_MAT_wood, "stairsWoodSpruce", planks, 1); stairsWoodSpruce->setHardness(2.0f); stairsWoodSpruce->setResistance(5.f);//(new BlockStairs(134, planks, 1)).setUnlocalizedName("stairsWoodSpruce");
	stairsWoodBirch = LordNew BlockStairs(135, BM_Material::BM_MAT_wood, "stairsWoodBirch", planks, 2); stairsWoodBirch->setHardness(2.0f); stairsWoodBirch->setResistance(5.f);//(new BlockStairs(135, planks, 2)).setUnlocalizedName("stairsWoodBirch");
	stairsWoodJungle = LordNew BlockStairs(136, BM_Material::BM_MAT_wood, "stairsWoodJungle", planks, 3); stairsWoodJungle->setHardness(2.0f); stairsWoodJungle->setResistance(5.f);//(new BlockStairs(136, planks, 3)).setUnlocalizedName("stairsWoodJungle");
	commandBlock =	LordNew BlockCommandBlock(137, BM_Material::BM_MAT_iron, "commandBlock"); commandBlock->setBlockUnbreakable(); commandBlock->setResistance(6000000.0F);
	beacon =		LordNew BlockBeacon(138, BM_Material::BM_MAT_glass, "beacon"); beacon->setLightValue(1.0F);
	cobblestoneWall = LordNew BlockWall(139, cobblestone->getMaterial(), "cobbleWall");  cobblestoneWall->setHardness(cobblestone->m_blockHardness); cobblestoneWall->setResistance(cobblestone->m_blockResistance / 3.0F); //cobblestoneWall->setCreativeTab()
	flowerPot =		LordNew BlockFlowerPot(140, BM_Material::BM_MAT_circuits, "flowerPot"); flowerPot->setHardness(0.0F);
	carrot =		LordNew BlockCarrot(141, BM_Material::BM_MAT_plants, "carrots");//(new BlockCarrot(141)).setUnlocalizedName("carrots").func_111022_d("carrots");
	potato =		LordNew BlockPotato(142, BM_Material::BM_MAT_plants, "potatoes");//(new BlockPotato(142)).setUnlocalizedName("potatoes").func_111022_d("potatoes");
	woodenButton =	LordNew BlockButtonWood(143, BM_Material::BM_MAT_circuits, "button"); woodenButton->setHardness(0.5F); 
	skull =			LordNew BlockSkull(144, BM_Material::BM_MAT_circuits, "skull"); skull->setHardness(1.0F);
	anvil =			LordNew BlockAnvil(145, BM_Material::BM_MAT_anvil, "anvil"); anvil->setHardness(5.0F); anvil->setResistance(2000.0F);
	chestTrapped = LordNew BlockChest(146, BM_Material::BM_MAT_wood, "chestTrap", BlockChest::BTT_BLOCK_TRAPPED); chestTrapped->setHardness(2.5F);
	pressurePlateGold = LordNew BlockPressurePlateWeighted(147, BM_Material::BM_MAT_iron, "weightedPlate_light", 64, MOB_TYPE_ITEM); pressurePlateGold->setHardness(0.5F);
	pressurePlateIron = LordNew BlockPressurePlateWeighted(148, BM_Material::BM_MAT_iron, "weightedPlate_heavy", 640, MOB_TYPE_ITEM); pressurePlateIron->setHardness(0.5F);
	redstoneComparatorIdle = LordNew BlockComparator(149, BM_Material::BM_MAT_circuits, "comparator", false); redstoneComparatorIdle->setHardness(0.0F); redstoneComparatorIdle->disableStats();
	redstoneComparatorActive = LordNew BlockComparator(150, BM_Material::BM_MAT_circuits, "comparator", true); redstoneComparatorActive->setHardness(0.0F); redstoneComparatorActive->setLightValue(0.625F); redstoneComparatorActive->disableStats();
	daylightSensor = LordNew BlockDaylightDetector(151, BM_Material::BM_MAT_wood, "daylightDetector"); daylightSensor->setHardness(0.2F);
	blockRedstone =	LordNew BlockPoweredOre(152, BM_Material::BM_MAT_iron, "blockRedstone"); blockRedstone->setHardness(5.0F); blockRedstone->setResistance(10.0F);
	oreNetherQuartz = LordNew BlockOre(153, BM_Material::BM_MAT_rock, "netherquartz"); oreNetherQuartz->setHardness(3.0F); oreNetherQuartz->setResistance(5.0F);
	hopperBlock =		LordNew BlockHopper(154, BM_Material::BM_MAT_iron, "hopper"); hopperBlock->setHardness(3.0F); hopperBlock->setResistance(8.0F);
	blockNetherQuartz = LordNew BlockQuartz(155, BM_Material::BM_MAT_rock, "quartzBlock"); blockNetherQuartz->setHardness(0.8F);
	stairsNetherQuartz = LordNew BlockStairs(156, BM_Material::BM_MAT_rock, "stairsQuartz", blockNetherQuartz, 0); stairsNetherQuartz->setHardness(0.8f);
	railActivator =		LordNew BlockRailPowered(157, BM_Material::BM_MAT_circuits, "activatorRail"); railActivator->setHardness(0.7F);
	dropper =			LordNew BlockDropper(158, BM_Material::BM_MAT_rock, "dropper"); dropper->setHardness(3.5F); 
	clayHardenedStained =	LordNew BlockColored(159, BM_Material::BM_MAT_rock, "clayHardenedStained"); clayHardenedStained->setHardness(1.25F); clayHardenedStained->setResistance(7.0F);
	stained_glass = LordNew BlockStainedGlass(160, BM_Material::BM_MAT_glass, "stained_glass"); stained_glass->setHardness(0.3F); stained_glass->setLightOpacity(0);
	wood2 = LordNew BlockLog2(162, BM_Material::BM_MAT_wood, "log"); wood2->setHardness(2.0F);
	stairsWoodDarkOak = LordNew BlockStairs(164, BM_Material::BM_MAT_wood, "stairsWoodDarkOak", planks, 5); stairsWoodDarkOak->setHardness(2.0f); stairsWoodDarkOak->setResistance(5.f);
	slime = LordNew BlockSlime(165, BM_Material::BM_MAT_rock, "slime"); slime->setHardness(0.5F);
	seaLantern = LordNew Block(168, BM_Material::BM_MAT_glass, "seaLantern"); seaLantern->setHardness(0.3F); seaLantern->setLightValue(1.f);
	blockWithEffect = LordNew BlockWithEffect(169, BM_Material::BM_MAT_rock, "normal_chest"); blockWithEffect->setHardness(3.0f); blockWithEffect->setResistance(15.f);
	hayBlock =	LordNew BlockHay(170, BM_Material::BM_MAT_grass, "hayBlock"); hayBlock->setHardness(0.5F);
	woolCarpet =	LordNew BlockCarpet(171, BM_Material:: BM_MAT_carpet, "woolCarpet"); woolCarpet->setHardness(0.1F); woolCarpet->setLightOpacity(0);
	clayHardened =	LordNew Block(172, BM_Material::BM_MAT_rock, "clayHardened"); clayHardened->setHardness(1.25F); clayHardened->setResistance(7.0F);
	blockCoal =	LordNew Block(173, BM_Material::BM_MAT_rock, "blockCoal"); blockCoal->setHardness(5.0F); blockCoal->setResistance(10.0F);
	packedIce = LordNew BlockPackedIce(174, BM_Material::BM_MAT_packedIce, "icePacked"); packedIce->setHardness(0.5F);
	redSandstoneDoubleSlab = LordNew BlockRedStoneSlab(181, BM_Material::BM_MAT_sand, "redSandstoneSlab", true); redSandstoneDoubleSlab->setHardness(2.0F); redSandstoneDoubleSlab->setResistance(10.0F);
	redSandstoneSingleSlab = LordNew BlockRedStoneSlab(182, BM_Material::BM_MAT_sand, "redSandstoneSlab", false); redSandstoneSingleSlab->setHardness(2.0F); redSandstoneSingleSlab->setResistance(10.0F);

	blockWithEffect_02 = LordNew BlockWithEffect(207, BM_Material::BM_MAT_rock, "iron_chest"); blockWithEffect_02->setHardness(3.0f); blockWithEffect_02->setResistance(15.f);
	blockWithEffect_03 = LordNew BlockWithEffect(210, BM_Material::BM_MAT_rock, "silver_chest"); blockWithEffect_03->setHardness(3.0f); blockWithEffect_03->setResistance(15.f);
	blockWithEffect_04 = LordNew BlockWithEffect(211, BM_Material::BM_MAT_rock, "gold_chest"); blockWithEffect_04->setHardness(3.0f); blockWithEffect_04->setResistance(15.f);
	chestNew = LordNew BlockChest(219, BM_Material::BM_MAT_wood, "chestNew", BlockChest::BTT_BLOCK_NOT_TRAPPED_NEW); chestNew->setHardness(2.5F);
	pressurePlateWeightWood = LordNew BlockPressurePlateWeighted(217, BM_Material::BM_MAT_wood, "pressurePlateWeightWood", 10, MOB_TYPE_PLAYERS); pressurePlateWeightWood->setHardness(0.5F);
	enderChestNew = LordNew BlockEnderChest(220, BM_Material::BM_MAT_rock, "enderChestNew", BlockEnderChest::BECT_NEW); enderChestNew->setHardness(22.5F); enderChestNew->setResistance(1000.0F); enderChestNew->setLightValue(0.5F);
	chestTrappedNew = LordNew BlockChest(221, BM_Material::BM_MAT_wood, "chestTrappedNew", BlockChest::BTT_BLOCK_TRAPPED_NEW); chestTrappedNew->setHardness(2.5F);
	railReceive = LordNew BlockRailPowered(240, BM_Material::BM_MAT_circuits, "receiveRail"); railPowered->setHardness(0.7F);

	keyDoor_01 = LordNew BlockDoor(241, BM_Material::BM_MAT_wood, "keyDoor_01"); keyDoor_01->setHardness(3.0F); keyDoor_01->disableStats();
	keyDoor_02 = LordNew BlockDoor(242, BM_Material::BM_MAT_wood, "keyDoor_02"); keyDoor_02->setHardness(3.0F); keyDoor_02->disableStats();
	keyDoor_03 = LordNew BlockDoor(243, BM_Material::BM_MAT_wood, "keyDoor_03"); keyDoor_03->setHardness(3.0F); keyDoor_03->disableStats();
	keyDoor_04 = LordNew BlockDoor(244, BM_Material::BM_MAT_wood, "keyDoor_04"); keyDoor_04->setHardness(3.0F); keyDoor_04->disableStats();
	keyDoor_05 = LordNew BlockDoor(245, BM_Material::BM_MAT_wood, "keyDoor_05"); keyDoor_05->setHardness(3.0F); keyDoor_05->disableStats();
	keyDoor_06 = LordNew BlockDoor(246, BM_Material::BM_MAT_wood, "keyDoor_06"); keyDoor_06->setHardness(3.0F); keyDoor_06->disableStats();
	keyDoor_07 = LordNew BlockDoor(247, BM_Material::BM_MAT_wood, "keyDoor_07"); keyDoor_07->setHardness(3.0F); keyDoor_07->disableStats();
	keyDoor_08 = LordNew BlockDoor(248, BM_Material::BM_MAT_wood, "keyDoor_08"); keyDoor_08->setHardness(3.0F); keyDoor_08->disableStats();
	keyDoor_09 = LordNew BlockDoor(249, BM_Material::BM_MAT_wood, "keyDoor_09"); keyDoor_09->setHardness(3.0F); keyDoor_09->disableStats();
	keyDoor_10 = LordNew BlockDoor(250, BM_Material::BM_MAT_wood, "keyDoor_10"); keyDoor_10->setHardness(3.0F); keyDoor_10->disableStats();
	
	concrete = LordNew BlockColored(251, BM_Material::BM_MAT_rock, "concrete"); concrete->setHardness(1.25F); concrete->setResistance(7.0F);
	concrete_powder = LordNew BlockColored(252, BM_Material::BM_MAT_sand, "concrete_powder"); concrete_powder->setHardness(0.5F);
	air_wall = LordNew BlockAirWall(253, BM_Material::BM_MAT_glass, "air_wall"); air_wall->setHardness(-1.0F); air_wall->setLightOpacity(0); air_wall->setResistance(6000000.0F);
	
	roadCarpet = LordNew BlockRoad(255, BM_Material::BM_MAT_rock, "roadCarpet"); roadCarpet->setHardness(0.5F); roadCarpet->setLightOpacity(0);

	/*extend block from 1256~1999*/
	/*Crops id 1256~1399 */
	newCrops_01 = LordNew BlockNewCrops(1256, BM_Material::BM_MAT_plants, "newCrops_01", 900);
	newCrops_02 = LordNew BlockNewCrops(1257, BM_Material::BM_MAT_plants, "newCrops_02", 901);
	newCrops_03 = LordNew BlockNewCrops(1258, BM_Material::BM_MAT_plants, "newCrops_03", 902);
	newCrops_04 = LordNew BlockNewCrops(1259, BM_Material::BM_MAT_plants, "newCrops_04", 903);
	newCrops_05 = LordNew BlockNewCrops(1260, BM_Material::BM_MAT_plants, "newCrops_05", 904);

	ranchCrop_100001 = LordNew BlockNewCrops(1261, BM_Material::BM_MAT_plants, "100001_wheat", 100001);
	ranchCrop_100002 = LordNew BlockNewCrops(1262, BM_Material::BM_MAT_plants, "100002_corn", 100002);
	ranchCrop_100003 = LordNew BlockNewCrops(1263, BM_Material::BM_MAT_plants, "100003_carrots", 100003);
	ranchCrop_100004 = LordNew BlockNewCrops(1264, BM_Material::BM_MAT_plants, "100004_sugar_cane", 100004);
	ranchCrop_100005 = LordNew BlockNewCrops(1265, BM_Material::BM_MAT_plants, "100005_cotton", 100005);
	ranchCrop_100006 = LordNew BlockNewCrops(1266, BM_Material::BM_MAT_plants, "100006_strawberry", 100006);
	ranchCrop_100007 = LordNew BlockNewCrops(1267, BM_Material::BM_MAT_plants, "100007_tomato", 100007);
	ranchCrop_100008 = LordNew BlockNewCrops(1268, BM_Material::BM_MAT_plants, "100008_pine", 100008);
	ranchCrop_100009 = LordNew BlockNewCrops(1269, BM_Material::BM_MAT_plants, "100009_potatoes", 100009);
	ranchCrop_100010 = LordNew BlockNewCrops(1270, BM_Material::BM_MAT_plants, "100010_cocoa_beans", 100010);
	ranchCrop_100011 = LordNew BlockNewCrops(1271, BM_Material::BM_MAT_plants, "100011_rubber_wood", 100011);
	ranchCrop_100012 = LordNew BlockNewCrops(1272, BM_Material::BM_MAT_plants, "100012_silk", 100012);
	ranchCrop_100013 = LordNew BlockNewCrops(1273, BM_Material::BM_MAT_plants, "100013_chili", 100013);
	ranchCrop_100014 = LordNew BlockNewCrops(1274, BM_Material::BM_MAT_plants, "100014_rice", 100014);
	ranchCrop_100015 = LordNew BlockNewCrops(1275, BM_Material::BM_MAT_plants, "100015_the_rose", 100015);
	ranchCrop_100016 = LordNew BlockNewCrops(1276, BM_Material::BM_MAT_plants, "100016_jasmine", 100016);
	ranchCrop_100017 = LordNew BlockNewCrops(1277, BM_Material::BM_MAT_plants, "100017_coffee_beans", 100017);

	nest_01 = LordNew BlockNest(1291, BM_Material::BM_MAT_rock, "nest_01"); nest_01->setHardness(3.0f); nest_01->setResistance(15.f);
	nest_02 = LordNew BlockNest(1292, BM_Material::BM_MAT_rock, "nest_02"); nest_02->setHardness(3.0f); nest_02->setResistance(15.f);

	fruits_01 = LordNew BlockFruits(1301, BM_Material::BM_MAT_plants, "fruits_01"); fruits_01->setHardness(0.5F);
	fruits_02 = LordNew BlockFruits(1302, BM_Material::BM_MAT_plants, "fruits_02"); fruits_02->setHardness(0.5F);
	fruits_03 = LordNew BlockFruits(1303, BM_Material::BM_MAT_plants, "fruits_03"); fruits_03->setHardness(0.5F);
	fruits_04 = LordNew BlockFruits(1304, BM_Material::BM_MAT_plants, "fruits_04"); fruits_04->setHardness(0.5F);
	fruits_05 = LordNew BlockFruits(1305, BM_Material::BM_MAT_plants, "fruits_05"); fruits_05->setHardness(0.5F);
	fruits_06 = LordNew BlockFruits(1306, BM_Material::BM_MAT_plants, "fruits_06"); fruits_06->setHardness(0.5F);
	fruits_07 = LordNew BlockFruits(1307, BM_Material::BM_MAT_plants, "fruits_07"); fruits_07->setHardness(0.5F);
	fruits_08 = LordNew BlockFruits(1308, BM_Material::BM_MAT_plants, "fruits_08"); fruits_08->setHardness(0.5F);
	fruits_09 = LordNew BlockFruits(1309, BM_Material::BM_MAT_plants, "fruits_09"); fruits_09->setHardness(0.5F);
	fruits_10 = LordNew BlockFruits(1310, BM_Material::BM_MAT_plants, "fruits_10"); fruits_10->setHardness(0.5F);
	fruits_11 = LordNew BlockFruits(1311, BM_Material::BM_MAT_plants, "fruits_11"); fruits_11->setHardness(0.5F);
	fruits_12 = LordNew BlockFruits(1312, BM_Material::BM_MAT_plants, "fruits_12"); fruits_12->setHardness(0.5F);

	banner_00 = LordNew BlockBanner(900, BM_Material::BM_MAT_anvil, "banner_00"); banner_00->setHardness(0.5F); banner_00->setResistance(2000.0F);
	banner_01 = LordNew BlockBanner(901, BM_Material::BM_MAT_anvil, "banner_01"); banner_01->setHardness(0.5F); banner_01->setResistance(2000.0F);
	banner_02 = LordNew BlockBanner(902, BM_Material::BM_MAT_anvil, "banner_02"); banner_02->setHardness(0.5F); banner_02->setResistance(2000.0F);
	banner_03 = LordNew BlockBanner(903, BM_Material::BM_MAT_anvil, "banner_03"); banner_03->setHardness(0.5F); banner_03->setResistance(2000.0F);
	banner_04 = LordNew BlockBanner(904, BM_Material::BM_MAT_anvil, "banner_04"); banner_04->setHardness(0.5F); banner_04->setResistance(2000.0F);
	banner_05 = LordNew BlockBanner(905, BM_Material::BM_MAT_anvil, "banner_05"); banner_05->setHardness(0.5F); banner_05->setResistance(2000.0F);
	banner_06 = LordNew BlockBanner(906, BM_Material::BM_MAT_anvil, "banner_06"); banner_06->setHardness(0.5F); banner_06->setResistance(2000.0F);
	banner_07 = LordNew BlockBanner(907, BM_Material::BM_MAT_anvil, "banner_07"); banner_07->setHardness(0.5F); banner_07->setResistance(2000.0F);
	banner_08 = LordNew BlockBanner(908, BM_Material::BM_MAT_anvil, "banner_08"); banner_08->setHardness(0.5F); banner_08->setResistance(2000.0F);
	banner_09 = LordNew BlockBanner(909, BM_Material::BM_MAT_anvil, "banner_09"); banner_09->setHardness(0.5F); banner_09->setResistance(2000.0F);
	banner_10 = LordNew BlockBanner(910, BM_Material::BM_MAT_anvil, "banner_10"); banner_10->setHardness(0.5F); banner_10->setResistance(2000.0F);
	banner_11 = LordNew BlockBanner(911, BM_Material::BM_MAT_anvil, "banner_11"); banner_11->setHardness(0.5F); banner_11->setResistance(2000.0F);
	banner_12 = LordNew BlockBanner(912, BM_Material::BM_MAT_anvil, "banner_12"); banner_12->setHardness(0.5F); banner_12->setResistance(2000.0F);
	banner_13 = LordNew BlockBanner(913, BM_Material::BM_MAT_anvil, "banner_13"); banner_13->setHardness(0.5F); banner_13->setResistance(2000.0F);
	banner_14 = LordNew BlockBanner(914, BM_Material::BM_MAT_anvil, "banner_14"); banner_14->setHardness(0.5F); banner_14->setResistance(2000.0F);
	banner_15 = LordNew BlockBanner(915, BM_Material::BM_MAT_anvil, "banner_15"); banner_15->setHardness(0.5F); banner_15->setResistance(2000.0F);

	transparency = LordNew BlockTransparency(916, BM_Material::BM_MAT_glass, "transparency"); transparency->setHardness(-1.0F); transparency->setLightOpacity(0); transparency->setResistance(6000000.0F);

	sCanBlockGrass[0] = true;

	// registBlockItems();
}

void BlockManager::registBlockItems()
{
	Item::itemsList[cloth->getItemBlockId()] = LordNew ItemCloth(cloth->getItemBlockId() - 256, "cloth");
	Item::itemsList[clayHardenedStained->getItemBlockId()] = LordNew ItemCloth(clayHardenedStained->getItemBlockId() - 256, "clayHardenedStained");
	Item::itemsList[woolCarpet->getItemBlockId()] = LordNew ItemCloth(woolCarpet->getItemBlockId() - 256, "woolCarpet");
	//Item::itemsList[roadCarpet->getItemBlockId()] = LordNew ItemCloth(roadCarpet->getItemBlockId() - 256, "roadCarpet");
	Item::itemsList[wood->getItemBlockId()] = LordNew ItemMultiTextureTile(wood->getItemBlockId() - 256, "log", BlockLog1::woodType);
	Item::itemsList[planks->getItemBlockId()] = LordNew ItemMultiTextureTile(planks->getItemBlockId() - 256, "wood", BlockWood::woodType);
	Item::itemsList[silverfish->getItemBlockId()] = LordNew ItemMultiTextureTile(silverfish->getItemBlockId() - 256, "monsterStoneEgg", BlockSilverfish::silverfishStoneTypes);
	Item::itemsList[stoneBrick->getItemBlockId()] = LordNew ItemMultiTextureTile(stoneBrick->getItemBlockId() - 256, "stonebricksmooth", BlockStoneBrick::STONE_BRICK_TYPES);
	Item::itemsList[sandStone->getItemBlockId()] = LordNew ItemMultiTextureTile(sandStone->getItemBlockId() - 256, "sandStone", BlockSandStone::SAND_STONE_TYPES);
	Item::itemsList[blockNetherQuartz->getItemBlockId()] = LordNew ItemMultiTextureTile(blockNetherQuartz->getItemBlockId() - 256, "quartzBlock", BlockQuartz::quartzBlockTypes);
	Item::itemsList[stoneSingleSlab->getItemBlockId()] = LordNew ItemSlab(stoneSingleSlab->getItemBlockId() - 256, "stoneSlab", stoneSingleSlab, stoneDoubleSlab, false);
	Item::itemsList[stoneDoubleSlab->getItemBlockId()] = LordNew ItemSlab(stoneDoubleSlab->getItemBlockId() - 256, "stoneSlab", stoneSingleSlab, stoneDoubleSlab, true);
	Item::itemsList[woodSingleSlab->getItemBlockId()] = LordNew ItemSlab(woodSingleSlab->getItemBlockId() - 256, "woodSlab", woodSingleSlab, woodDoubleSlab, false);
	Item::itemsList[woodDoubleSlab->getItemBlockId()] = LordNew ItemSlab(woodDoubleSlab->getItemBlockId() - 256, "woodSlab", woodSingleSlab, woodDoubleSlab, true);
	Item::itemsList[sapling->getItemBlockId()] = LordNew ItemMultiTextureTile(sapling->getItemBlockId() - 256, "sapling", BlockSapling::WOOD_TYPES);
	Item::itemsList[leaves->getItemBlockId()] = LordNew ItemLeaves(leaves->getItemBlockId() - 256, "leaves");
	Item::itemsList[vine->getItemBlockId()] = LordNew ItemColored(vine->getItemBlockId() - 256, false);
	Item::itemsList[tallGrass->getItemBlockId()] = LordNew ItemColored(tallGrass->getItemBlockId() - 256, true, StringArray({ "shrub", "grass", "fern" }));
	Item::itemsList[snow->getItemBlockId()] = LordNew ItemSnow(snow->getItemBlockId() - 256); //, snow);
	Item::itemsList[waterlily->getItemBlockId()] = LordNew ItemLilyPad(waterlily->getItemBlockId() - 256);
	Item::itemsList[pistonBase->getItemBlockId()] = LordNew ItemPiston(pistonBase->getItemBlockId() - 256);
	Item::itemsList[pistonStickyBase->getItemBlockId()] = LordNew ItemPiston(pistonStickyBase->getItemBlockId() - 256);
	Item::itemsList[cobblestoneWall->getItemBlockId()] = LordNew ItemMultiTextureTile(cobblestoneWall->getItemBlockId() - 256, "cobbleWall", BlockWall::types);
	Item::itemsList[anvil->getItemBlockId()] = LordNew ItemAnvilBlock(anvil->getItemBlockId() - 256, "anvil");
	Item::itemsList[wood2->getItemBlockId()] = LordNew ItemMultiTextureTile(wood2->getItemBlockId() - 256, "log", BlockLog2::woodType);
	Item::itemsList[tnt->getItemBlockId()] = LordNew ItemTnt(tnt->getItemBlockId() - 256, "tnt");
	Item::itemsList[concrete->getItemBlockId()] = LordNew ItemCloth(concrete->getItemBlockId() - 256, "concrete");
	Item::itemsList[concrete_powder->getItemBlockId()] = LordNew ItemCloth(concrete_powder->getItemBlockId() - 256, "concrete_powder");
	Item::itemsList[redSandstoneSingleSlab->getItemBlockId()] = LordNew ItemSlab(redSandstoneSingleSlab->getItemBlockId() - 256, "redSandstoneSlab", redSandstoneSingleSlab, redSandstoneDoubleSlab, false);
	Item::itemsList[redSandstoneDoubleSlab->getItemBlockId()] = LordNew ItemSlab(redSandstoneDoubleSlab->getItemBlockId() - 256, "redSandstoneSlab", redSandstoneSingleSlab, redSandstoneDoubleSlab, true);
	Item::itemsList[stained_glass->getItemBlockId()] = LordNew ItemCloth(stained_glass->getItemBlockId() - 256, "glass");

	Item::itemsList[banner_00->getItemBlockId()] = LordNew ItemBanner(banner_00->getItemBlockId() - 256, "banner_00");
	Item::itemsList[banner_01->getItemBlockId()] = LordNew ItemBanner(banner_01->getItemBlockId() - 256, "banner_01");
	Item::itemsList[banner_02->getItemBlockId()] = LordNew ItemBanner(banner_02->getItemBlockId() - 256, "banner_02");
	Item::itemsList[banner_03->getItemBlockId()] = LordNew ItemBanner(banner_03->getItemBlockId() - 256, "banner_03");
	Item::itemsList[banner_04->getItemBlockId()] = LordNew ItemBanner(banner_04->getItemBlockId() - 256, "banner_04");
	Item::itemsList[banner_05->getItemBlockId()] = LordNew ItemBanner(banner_05->getItemBlockId() - 256, "banner_05");
	Item::itemsList[banner_06->getItemBlockId()] = LordNew ItemBanner(banner_06->getItemBlockId() - 256, "banner_06");
	Item::itemsList[banner_07->getItemBlockId()] = LordNew ItemBanner(banner_07->getItemBlockId() - 256, "banner_07");
	Item::itemsList[banner_08->getItemBlockId()] = LordNew ItemBanner(banner_08->getItemBlockId() - 256, "banner_08");
	Item::itemsList[banner_09->getItemBlockId()] = LordNew ItemBanner(banner_09->getItemBlockId() - 256, "banner_09");
	Item::itemsList[banner_10->getItemBlockId()] = LordNew ItemBanner(banner_10->getItemBlockId() - 256, "banner_10");
	Item::itemsList[banner_11->getItemBlockId()] = LordNew ItemBanner(banner_11->getItemBlockId() - 256, "banner_11");
	Item::itemsList[banner_12->getItemBlockId()] = LordNew ItemBanner(banner_12->getItemBlockId() - 256, "banner_12");
	Item::itemsList[banner_13->getItemBlockId()] = LordNew ItemBanner(banner_13->getItemBlockId() - 256, "banner_13");
	Item::itemsList[banner_14->getItemBlockId()] = LordNew ItemBanner(banner_14->getItemBlockId() - 256, "banner_14");
	Item::itemsList[banner_15->getItemBlockId()] = LordNew ItemBanner(banner_15->getItemBlockId() - 256, "banner_15");

	for (int i = 0; i < MAX_BLOCK_COUNT; ++i)
	{
		Block* pBlock = sBlocks[i];
		if(!pBlock)
			continue;

		int itemId = pBlock->getItemBlockId();
		if (Item::itemsList[itemId] == NULL)
		{
			Item::itemsList[itemId] = LordNew ItemBlock(itemId - 256);
			pBlock->initializeBlock();
		}

		bool useNeighborBrightness = false;

		if (i == BLOCK_ID_STAIRS_COBBLE_STONE ||
			i == BLOCK_ID_STAIRS_NETHER_BRICK ||
			i == BLOCK_ID_STAIRS_SAND_STONE ||
			i == BLOCK_ID_STAIRS_STONE_BRICK ||
			i == BLOCK_ID_STAIRS_WOOD_OAK ||
			i == BLOCK_ID_STAIRS_WOOD_BIRCH ||
			i == BLOCK_ID_STAIRS_WOOD_JUNGLE ||
			i == BLOCK_ID_STAIRS_WOOD_SPURCE ||
			i == BLOCK_ID_STAIRS_BRICK ||
			i == BLOCK_ID_STAIRS_NETHER_QUARTZ)
		{
			useNeighborBrightness = true;
		}

		BlockHalfSlab* pslab = dynamic_cast<BlockHalfSlab*>(pBlock);
		if (i > 0 && pslab)
		{
			useNeighborBrightness = true;
		}

		if (i == tilledField->getBlockID())
		{
			useNeighborBrightness = true;
		}

		if (sCanBlockGrass[i])
		{
			useNeighborBrightness = true;
		}

		if (sLightOpacity[i] == 0)
		{
			useNeighborBrightness = true;
		}

		sUseNeighborBrightness[i] = useNeighborBrightness;
	}
}


void BlockManager::onBlockCreated(Block* pBlock)
{
	LordAssert(sBlocks);
	int blockID = pBlock->getBlockID();
	LordAssertX(blockID >= 0 && blockID < MAX_BLOCK_COUNT, "Out of array bounds!");
	LordAssertX(sBlocks[blockID] == NULL, "ReCreate Block!");
	sBlocks[blockID] = pBlock;
	// modify by zhouyou 
	// m_opaqueCubeLookup and  m_lightOpacity is not correctly. for the Block::isOpaqueCube is virtual function.
	// and currently the derived class of Block is not cunstruction.
	sOpaqueCubeLookup[blockID] = pBlock->isOpaqueCube(); 
	sLightOpacity[blockID] = pBlock->isOpaqueCube() ? 255 : 0;
	sCanBlockGrass[blockID] = pBlock->getMaterial().canBlockGrass();
}


void BlockManager::setLightOpacity(int blockID, int lightOpacity)
{
	LordAssert(sLightOpacity);
	LordAssertX(blockID >= 0 && blockID < MAX_BLOCK_COUNT, "Out of array bound");

	sLightOpacity[blockID] = lightOpacity;
}

void BlockManager::setLightValue(int blockID, float lightValue)
{
	LordAssert(sLightValue);
	LordAssertX(blockID >= 0 && blockID < MAX_BLOCK_COUNT, "Out of array bound");

	sLightValue[blockID] = (int)(15.0 * lightValue);;
}

bool BlockManager::isNormalCube(int blockID)
{
	Block* pBlock = sBlocks[blockID];
	if (!pBlock)
		return false;

	return
		pBlock->m_blockMaterial.isOpaque() &&
		pBlock->renderAsNormalBlock() &&
		!pBlock->canProvidePower();
}

bool BlockManager::isAssociatedBlockID(int blockID1, int blockID2)
{
	if (blockID2 == blockID2)
		return true;

	if (blockID1 != 0 &&
		blockID2 != 0 &&
		sBlocks[blockID1] != NULL &&
		sBlocks[blockID2] != NULL)
	{
		return sBlocks[blockID1]->isAssociatedBlockID(blockID2);
	}

	return false;
}

Block* BlockManager::getBlockById(int blockId)
{
	if (blockId < 0 || blockId >= MAX_BLOCK_COUNT)
		return NULL;

	return sBlocks[blockId];
}

bool BlockManager::isStairsBlock(int blockId)
{
	return blockId == 53
		|| blockId == 67
		|| blockId == 108
		|| blockId == 109
		|| blockId == 114
		|| blockId == 128
		|| blockId == 134
		|| blockId == 135
		|| blockId == 136
		|| blockId == 156;
}

ui8 BlockManager::StairsBlockRotate(ui8 meta, bool xImage, bool zImage)
{
	switch (meta)
	{
	case 0:
		if (xImage && zImage) return 1;
		if (!xImage && zImage) return 3;
		if (xImage && !zImage) return 2;
		break;
	case 1:
		if (xImage && zImage) return 0;
		if (!xImage && zImage) return 2;
		if (xImage && !zImage) return 3;
		break;
	case 2:
		if (xImage && zImage) return 3;
		if (!xImage && zImage) return 0;
		if (xImage && !zImage) return 1;
		break;
	case 3:
		if (xImage && zImage) return 2;
		if (!xImage && zImage) return 1;
		if (xImage && !zImage) return 0;
		break;
	case 4:
		if (xImage && zImage) return 5;
		if (!xImage && zImage) return 7;
		if (xImage && !zImage) return 6;
		break;
	case 5:
		if (xImage && zImage) return 4;
		if (!xImage && zImage) return 6;
		if (xImage && !zImage) return 7;
		break;
	case 6:
		if (xImage && zImage) return 7;
		if (!xImage && zImage) return 4;
		if (xImage && !zImage) return 5;
		break;
	case 7:
		if (xImage && zImage) return 6;
		if (!xImage && zImage) return 5;
		if (xImage && !zImage) return 4;
		break;
	default:
		return meta;
	}
	return meta;
}

bool BlockManager::isAttachmentBlock(int blockId)
{
	if (blockId < 0 || blockId >= MAX_BLOCK_COUNT)
		return false;

	return blockId == BLOCK_ID_LADDER // ladder
		|| blockId == BLOCK_ID_TORCH_WOOD // torchWood
		|| blockId == BLOCK_ID_DOOR_WOOD // doorwood
		|| blockId == BLOCK_ID_DOOR_IRON; // dooriron
}

bool BlockManager::isDoorBlock(int blockId)
{
	if (blockId < 0 || blockId >= MAX_BLOCK_COUNT)
		return false;

	return blockId == BLOCK_ID_DOOR_WOOD
		|| blockId == BLOCK_ID_DOOR_IRON;
}

}