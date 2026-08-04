#include "cBlock.h"
#include "cBlockManager.h"

#include "cItem/cItem.h"

#include "BM_TypeDef.h"
#include "Block/BlockManager.h"
#include "Block/Block.h"
#include "Block/Blocks.h"
#include "Block/BM_Material.h"
#include "Item/Item.h"
#include "Item/Items.h"


namespace BLOCKMAN
{

cBlock** cBlockManager::scBlocks = NULL;

void cBlockManager::cUnInitialize()
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
	LordSafeFree(scBlocks);
}

void cBlockManager::cInitialize()
{
	LordAssert(!sBlocks);
	LordAssert(!sOpaqueCubeLookup);
	LordAssert(!sLightOpacity);
	LordAssert(!sCanBlockGrass);
	LordAssert(!sLightValue);
	LordAssert(!sUseNeighborBrightness);

	sBlocks = (Block**)LordMalloc(sizeof(Block*)* MAX_BLOCK_COUNT);
	scBlocks = (cBlock**)LordMalloc(sizeof(cBlock*)* MAX_BLOCK_COUNT);
	sOpaqueCubeLookup = (i8*)LordMalloc(sizeof(i8)* MAX_BLOCK_COUNT);
	sLightOpacity = (int*)LordMalloc(sizeof(int)* MAX_BLOCK_COUNT);
	sCanBlockGrass = (i8*)LordMalloc(sizeof(i8)* MAX_BLOCK_COUNT);
	sLightValue = (int*)LordMalloc(sizeof(int)* MAX_BLOCK_COUNT);
	sUseNeighborBrightness = (i8*)LordMalloc(sizeof(i8)* MAX_BLOCK_COUNT);
	
	memset(sBlocks, 0, sizeof(Block*)* MAX_BLOCK_COUNT);
	memset(scBlocks, 0, sizeof(cBlock*)* MAX_BLOCK_COUNT);
	memset(sOpaqueCubeLookup, 0, sizeof(i8)* MAX_BLOCK_COUNT);
	memset(sLightOpacity, 0, sizeof(int)* MAX_BLOCK_COUNT);
	memset(sCanBlockGrass, 0, sizeof(i8)* MAX_BLOCK_COUNT);
	memset(sLightValue, 0, sizeof(int)* MAX_BLOCK_COUNT);
	memset(sUseNeighborBrightness, 0, sizeof(i8)* MAX_BLOCK_COUNT);

	stone = LordNew BlockClient(1, BM_Material::BM_MAT_rock, "stone", "stone"); stone->setHardness(3.0f); stone->setResistance(15.f);
	grass = LordNew cBlockGrass(2, BM_Material::BM_MAT_grass, "grass", "grass"); grass->setHardness(0.6f);
	//dirt = LordNew BlockClient(3, BM_Material::BM_MAT_ground, "dirt", "dirt"); dirt->setHardness(0.5f);
	dirt = LordNew cBlockDirt(3, BM_Material::BM_MAT_ground, "dirt", "dirt"); dirt->setHardness(0.5f);
	cobblestone = LordNew BlockClient(4, BM_Material::BM_MAT_rock, "stonebrick", "cobblestone"); cobblestone->setHardness(2.0F);  cobblestone->setResistance(10.0F);
	planks = LordNew cBlockWood(5, BM_Material::BM_MAT_wood, "wood", "planks");  planks->setHardness(2.0F); planks->setResistance(5.0F);
	sapling = LordNew cBlockSapling(6, BM_Material::BM_MAT_plants, "sapling", "sapling"); sapling->setHardness(0.0F); sapling->setLightOpacity(0);
	bedrock = LordNew BlockClient(7, BM_Material::BM_MAT_rock, "bedrock", "bedrock"); bedrock->setHardness(-1.0f); bedrock->setResistance(6000000.0F); bedrock->disableStats();
	waterMoving = LordNew cBlockFlowing(8, BM_Material::BM_MAT_water, "water", "water_flow"); waterMoving->setHardness(100.0F); waterMoving->setLightOpacity(3);
	waterStill = LordNew cBlockStationary(9, BM_Material::BM_MAT_water, "water", "water_still"); waterStill->setHardness(100.0F); waterStill->setLightOpacity(3); waterStill->disableStats();
	lavaMoving = LordNew cBlockFlowing(10, BM_Material::BM_MAT_lava, "lava", "lava_flow"); lavaMoving->setHardness(0.0F); lavaMoving->setLightValue(1.0F); lavaMoving->disableStats(); lavaMoving->setLightOpacity(0);
	lavaStill = LordNew cBlockStationary(11, BM_Material::BM_MAT_lava, "lava", "lava_still"); lavaStill->setHardness(100.0F); lavaStill->setLightValue(1.0F); lavaStill->disableStats(); lavaStill->setLightOpacity(0);
	sand = LordNew cBlockSand(12, BM_Material::BM_MAT_sand, "sand", "sand"); sand->setHardness(0.5F);
	gravel = LordNew cBlockSand(13, BM_Material::BM_MAT_sand, "gravel", "gravel"); gravel->setHardness(0.6F);
	oreGold = LordNew cBlockOre(14, BM_Material::BM_MAT_rock, "oreGold", "gold_ore"); oreGold->setHardness(3.0F); oreGold->setResistance(5.0F);
	oreIron = LordNew cBlockOre(15, BM_Material::BM_MAT_rock, "oreIron", "iron_ore"); oreIron->setHardness(3.0F); oreIron->setResistance(5.0F);
	oreCoal = LordNew cBlockOre(16, BM_Material::BM_MAT_rock, "oreCoal", "coal_ore"); oreCoal->setHardness(3.0F); oreCoal->setResistance(5.0F);
	wood = LordNew cBlockLog1(17, BM_Material::BM_MAT_wood, "log", "log"); wood->setHardness(2.0F);
	leaves = LordNew cBlockLeaves(18, BM_Material::BM_MAT_leaves, "leaves", "leaves"); leaves->setHardness(0.2F); leaves->setLightOpacity(1);
	sponge = LordNew BlockClient(19, BM_Material::BM_MAT_sponge, "sponge", "sponge"); sponge->setHardness(0.6F);
	glass = LordNew cBlockGlass(20, BM_Material::BM_MAT_glass, "glass", "glass"); glass->setHardness(0.3F); glass->setLightOpacity(0);
	oreLapis = LordNew cBlockOre(21, BM_Material::BM_MAT_rock, "oreLapis", "lapis_ore"); oreLapis->setHardness(3.0F); oreLapis->setResistance(5.0F);
	blockLapis = LordNew BlockClient(22, BM_Material::BM_MAT_rock, "blockLapis", "lapis_block"); blockLapis->setHardness(3.0F); blockLapis->setResistance(5.0F);
	dispenser = LordNew cBlockDispenser(23, BM_Material::BM_MAT_rock, "dispenser", "dispenser"); dispenser->setHardness(3.5F);
	sandStone = LordNew cBlockSandStone(24, BM_Material::BM_MAT_rock, "sandstone", "sandstone"); sandStone->setHardness(0.8F);
	music = LordNew cBlockNote(25, BM_Material::BM_MAT_wood, "musicBlock", "noteblock"); music->setHardness(0.8F);
	bed = LordNew cBlockBed(26, "bed", "bed"); bed->setHardness(0.2F);
	railPowered = LordNew cBlockRailPowered(27, BM_Material::BM_MAT_circuits, "goldenRail", "rail_golden"); railPowered->setHardness(0.7F);
	railDetector = LordNew cBlockDetectorRail(28, BM_Material::BM_MAT_circuits, "detectorRail", "rail_detector"); railDetector->setHardness(0.7F);
	pistonStickyBase = LordNew cBlockPistonBase(29, BM_Material::BM_MAT_piston, "pistonStickyBase", "pistonStickyBase", true); pistonStickyBase->setHardness(0.5f);
	web = LordNew cBlockWeb(30, BM_Material::BM_MAT_web, "web", "web"); web->setLightOpacity(1); web->setHardness(4.0F);
	tallGrass = LordNew cBlockTallGrass(31, BM_Material::BM_MAT_vine, "tallgrass", "tallgrass"); tallGrass->setHardness(0.0F);
	deadBush = LordNew cBlockDeadBush(32, BM_Material::BM_MAT_vine, "deadbush", "deadbush"); deadBush->setHardness(0.0F);
	pistonBase = LordNew cBlockPistonBase(33, BM_Material::BM_MAT_piston, "pistonBase", "pistonBase", false); pistonBase->setHardness(0.5f);
	pistonExtension = LordNew cBlockPistonExtension(34, BM_Material::BM_MAT_piston, "pistonExtension", "pistonExtension"); pistonExtension->setHardness(0.5f);
	cloth = LordNew cBlockColored(35, BM_Material::BM_MAT_cloth, "cloth", "wool_colored"); cloth->setHardness(0.8F);
	pistonMoving = LordNew cBlockPistonMoving(36, BM_Material::BM_MAT_piston, "pistonMoving", "piston_top_normal");
	plantYellow = LordNew cFlower(37, BM_Material::BM_MAT_plants, "flower", "flower_dandelion"); plantYellow->setHardness(0.0F);
	plantRed = LordNew cFlower(38, BM_Material::BM_MAT_plants, "rose", "flower_rose"); plantRed->setHardness(0.0F);
	mushroomBrown = LordNew cBlockMushroom(39, BM_Material::BM_MAT_plants, "mushroom", "mushroom_brown"); mushroomBrown->setHardness(0.0F); mushroomBrown->setLightValue(0.125F);
	mushroomRed = LordNew cBlockMushroom(40, BM_Material::BM_MAT_plants, "mushroom", "mushroom_red"); mushroomRed->setHardness(0.0F);
	blockGold = LordNew cBlockOreStorage(41, BM_Material::BM_MAT_iron, "blockGold", "gold_block"); blockGold->setHardness(3.0F); blockGold->setResistance(10.0F);
	blockIron = LordNew cBlockOreStorage(42, BM_Material::BM_MAT_iron, "blockIron", "iron_block"); blockIron->setHardness(5.0F); blockIron->setResistance(10.0F);
	stoneDoubleSlab = LordNew cBlockStep(43, BM_Material::BM_MAT_rock, "stoneSlab", "slab_double", true); stoneDoubleSlab->setHardness(2.0F); stoneDoubleSlab->setResistance(10.0F);
	stoneSingleSlab = LordNew cBlockStep(44, BM_Material::BM_MAT_rock, "stoneSlab", "slab_single", false); stoneSingleSlab->setHardness(2.0F); stoneSingleSlab->setResistance(10.0F);
	brick = LordNew BlockClient(45, BM_Material::BM_MAT_rock, "brick", "brick"); brick->setHardness(2.0F); brick->setResistance(10.0F);
	tnt = LordNew cBlockTNT(46, BM_Material::BM_MAT_tnt, "tnt", "tnt"); tnt->setHardness(0.0F);
	bookShelf = LordNew cBlockBookshelf(47, BM_Material::BM_MAT_wood, "bookshelf", "bookshelf"); bookShelf->setHardness(1.5F);
	cobblestoneMossy = LordNew BlockClient(48, BM_Material::BM_MAT_rock, "stoneMoss", "cobblestone_mossy"); cobblestoneMossy->setHardness(2.0F); cobblestoneMossy->setResistance(10.0F);
	obsidian = LordNew BlockClient(49, BM_Material::BM_MAT_rock, "obsidian", "obsidian"); obsidian->setHardness(50.0F); obsidian->setResistance(2000.0F);
	torchWood = LordNew cBlockTorch(50, BM_Material::BM_MAT_circuits, "torch", "torch_on"); torchWood->setHardness(0.0F); torchWood->setLightValue(0.9375F);
	fire = LordNew cBlockFire(51, BM_Material::BM_MAT_fire, "fire", "fire"); fire->setHardness(0.0F); fire->setLightValue(1.0F);
	mobSpawner = LordNew cBlockMobSpawner(52, BM_Material::BM_MAT_rock, "mobSpawner", "mob_spawner");  mobSpawner->setHardness(5.0F); mobSpawner->setLightOpacity(0);
	stairsWoodOak = LordNew cBlockStairs(53, BM_Material::BM_MAT_wood, "stairsWood", "stairsWood", planks, 0); stairsWoodOak->setHardness(2.0f); stairsWoodOak->setResistance(5.0F);
	chest = LordNew cBlockChest(54, BM_Material::BM_MAT_wood, "chest", "chest", BlockChest::BTT_BLOCK_NOT_TRAPPED); chest->setHardness(2.5F);
	redstoneWire = LordNew cBlockRedstoneWire(55, BM_Material::BM_MAT_circuits, "redstoneDust", "redstone_dust"); redstoneWire->setHardness(0.0F);
	oreDiamond = LordNew cBlockOre(56, BM_Material::BM_MAT_rock, "oreDiamond", "diamond_ore"); oreDiamond->setHardness(3.0F);  oreDiamond->setResistance(5.0F);
	blockDiamond = LordNew cBlockOreStorage(57, BM_Material::BM_MAT_iron, "blockDiamond", "diamond_block"); blockDiamond->setHardness(5.0F); blockDiamond->setResistance(10.0F);
	workbench = LordNew cBlockWorkbench(58, BM_Material::BM_MAT_wood, "workbench", "crafting_table"); workbench->setHardness(2.5F);
	crops = LordNew cBlockCrops(59, BM_Material::BM_MAT_plants, "crops", "wheat"); crops->setHardness(0.f);
	tilledField = LordNew cBlockFarmland(60, BM_Material::BM_MAT_ground, "farmland", "farmland");//(new BlockFarmland(60)).setHardness(0.6F).setStepSound(soundGravelFootstep).setUnlocalizedName("farmland").func_111022_d("farmland");
	furnaceIdle = LordNew cBlockFurnace(61, BM_Material::BM_MAT_rock, "furnace", "furnace", false); furnaceIdle->setHardness(3.5F);
	furnaceBurning = LordNew cBlockFurnace(62, BM_Material::BM_MAT_rock, "furnace", "furnace", true); furnaceBurning->setHardness(3.5F); furnaceBurning->setLightValue(0.875F);
	signPost = LordNew cBlockSign(63, BM_Material::BM_MAT_wood, "sign", "sign", 0, true); signPost->setHardness(1.0F); signPost->disableStats();
	doorWood = LordNew cBlockDoor(64, BM_Material::BM_MAT_wood, "doorWood", "door_wood"); doorWood->setHardness(3.0F); doorWood->disableStats();
	ladder = LordNew cBlockLadder(65, BM_Material::BM_MAT_circuits, "ladder", "ladder"); ladder->setHardness(0.4F);
	rail = LordNew cBlockRail(66, BM_Material::BM_MAT_circuits, "rail", "rail_normal"); rail->setHardness(0.7F);
	stairsCobblestone = LordNew cBlockStairs(67, BM_Material::BM_MAT_rock, "stairsStone", "stairsStone", cobblestone, 0); stairsCobblestone->setHardness(2.0f); stairsCobblestone->setResistance(10.0f);//(new BlockStairs(67, cobblestone, 0)).setUnlocalizedName("stairsStone");
	signWall = LordNew cBlockSign(68, BM_Material::BM_MAT_wood, "sign", "sign", 1, false); signWall->setHardness(1.0F); signWall->disableStats();
	lever = LordNew cBlockLever(69, BM_Material::BM_MAT_circuits, "lever", "lever"); lever->setHardness(0.5F);
	pressurePlateStone = LordNew cBlockPressurePlate(70, BM_Material::BM_MAT_rock, "pressurePlate", "stone", MOB_TYPE_MOBS); pressurePlateStone->setHardness(0.5F);
	doorIron = LordNew cBlockDoor(71, BM_Material::BM_MAT_iron, "doorIron", "door_iron"); doorIron->setHardness(5.0F); doorIron->disableStats();
	pressurePlatePlanks = LordNew cBlockPressurePlate(72, BM_Material::BM_MAT_wood, "pressurePlate", "planks_oak", MOB_TYPE_EVERYTHING); pressurePlatePlanks->setHardness(0.5F);
	oreRedstone = LordNew cBlockRedstoneOre(73, BM_Material::BM_MAT_rock, "oreRedstone", "redstone_ore", false);  oreRedstone->setHardness(3.0F); oreRedstone->setResistance(5.0F);
	oreRedstoneGlowing = LordNew cBlockRedstoneOre(74, BM_Material::BM_MAT_rock, "oreRedstone", "redstone_ore", true); oreRedstoneGlowing->setHardness(3.0F); oreRedstoneGlowing->setResistance(5.0F); oreRedstoneGlowing->setLightValue(0.625F);
	torchRedstoneIdle = LordNew cBlockRedstoneTorch(75, BM_Material::BM_MAT_circuits, "notGate", "redstone_torch_off", false); torchRedstoneIdle->setHardness(0.0F);
	torchRedstoneActive = LordNew cBlockRedstoneTorch(76, BM_Material::BM_MAT_circuits, "notGate", "redstone_torch_on", true); torchRedstoneActive->setHardness(0.0F); torchRedstoneActive->setLightValue(0.5F);//torchRedstoneActive->setCreativeTab(CreativeTabs::tabRedstone);
	stoneButton = LordNew cBlockButtonStone(77, BM_Material::BM_MAT_circuits, "button", "button"); stoneButton->setHardness(0.5F);
	snow = LordNew cBlockSnow(78, BM_Material::BM_MAT_snow, "snow", "snow"); snow->setHardness(0.1F); snow->setLightOpacity(0);
	ice = LordNew cBlockIce(79, BM_Material::BM_MAT_ice, "ice", "ice"); ice->setHardness(0.5F); ice->setLightOpacity(3);
	blockSnow = LordNew cBlockSnowBlock(80, BM_Material::BM_MAT_craftedSnow, "snow", "snow"); blockSnow->setHardness(0.2F);//(new BlockSnowBlock(80)).setHardness(0.2F).setStepSound(soundSnowFootstep).setUnlocalizedName("snow").func_111022_d("snow");
	cactus = LordNew cBlockCactus(81, BM_Material::BM_MAT_cactus, "cactus", "cactus"); cactus->setHardness(0.4F);
	blockClay = LordNew cBlockClay(82, BM_Material::BM_MAT_clay, "clay", "clay"); blockClay->setHardness(0.6F);
	reed = LordNew cBlockReed(83, BM_Material::BM_MAT_plants, "reeds", "reeds"); reed->setHardness(0.0F);
	jukebox = LordNew cBlockJukeBox(84, BM_Material::BM_MAT_wood, "jukebox", "jukebox"); jukebox->setHardness(2.0F); jukebox->setResistance(10.0F);
	fence = LordNew cBlockFence(85, BM_Material::BM_MAT_wood, "fence", "planks_oak"); fence->setHardness(2.0F); fence->setResistance(5.0F);
	pumpkin = LordNew cBlockPumpkin(86, BM_Material::BM_MAT_pumpkin, "pumpkin", "pumpkin", false); pumpkin->setHardness(1.0F);
	netherrack = LordNew BlockClient(87, BM_Material::BM_MAT_rock, "hellrock", "netherrack"); netherrack->setHardness(0.4F);
	slowSand = LordNew cBlockSoulSand(88, BM_Material::BM_MAT_sand, "hellsand", "soul_sand"); slowSand->setHardness(0.5F);
	glowStone = LordNew cBlockGlowStone(89, BM_Material::BM_MAT_glass, "lightgem", "glowstone"); glowStone->setHardness(0.3F); glowStone->setLightValue(1.f);
	portal = LordNew cBlockPortal(90, BM_Material::BM_MAT_portal, "portal", "portal"); portal->setHardness(-1.0F); portal->setLightValue(0.75F);
	pumpkinLantern = LordNew cBlockPumpkin(91, BM_Material::BM_MAT_pumpkin, "litpumpkin", "pumpkin", true); pumpkinLantern->setHardness(1.0F); pumpkinLantern->setLightValue(1.0F);
	cake = LordNew cBlockCake(92, BM_Material::BM_MAT_cake, "cake", "cake"); cake->setHardness(0.5F);
	redstoneRepeaterIdle = LordNew cBlockRedstoneRepeater(93, BM_Material::BM_MAT_circuits, "diode", "repeater_off", false); redstoneRepeaterIdle->setHardness(0.0F); redstoneRepeaterIdle->disableStats();
	redstoneRepeaterActive = LordNew cBlockRedstoneRepeater(94, BM_Material::BM_MAT_circuits, "diode", "repeater_on", true); redstoneRepeaterActive->setHardness(0.0F); redstoneRepeaterActive->setLightValue(0.625F); redstoneRepeaterActive->disableStats();
	lockedChest = LordNew BlockClient(95, BM_Material::BM_MAT_wood, "lockedchest", "lockedchest"); lockedChest->setHardness(0.0F); lockedChest->setLightValue(1.0F); lockedChest->setTickRandomly(true);
	trapdoor = LordNew cBlockTrapDoor(96, BM_Material::BM_MAT_wood, "trapdoor", "trapdoor"); trapdoor->setHardness(3.0F);
	silverfish = LordNew cBlockSilverfish(97, BM_Material::BM_MAT_clay, "monsterStoneEgg", "monsterStoneEgg"); silverfish->setHardness(0.75F);
	stoneBrick = LordNew cBlockStoneBrick(98, BM_Material::BM_MAT_rock, "stonebricksmooth", "stonebrick"); stoneBrick->setHardness(1.5F); stoneBrick->setResistance(10.0F);
	mushroomCapBrown = LordNew cBlockMushroomCap(99, BM_Material::BM_MAT_wood, "mushroom", "mushroom_block", 0); mushroomCapBrown->setHardness(0.2F);
	mushroomCapRed = LordNew cBlockMushroomCap(100, BM_Material::BM_MAT_wood, "mushroom", "mushroom_block", 1); mushroomCapRed->setHardness(0.2F);
	fenceIron = LordNew cBlockPane(101, BM_Material::BM_MAT_iron, "fenceIron", "iron_bars", "iron_bars", true); fenceIron->setHardness(5.0F); fenceIron->setResistance(10.0F);
	thinGlass = LordNew cBlockPane(102, BM_Material::BM_MAT_glass, "thinGlass", "glass", "glass_pane_top", false);
	melon = LordNew cBlockMelon(103, BM_Material::BM_MAT_pumpkin, "melon", "melon"); melon->setHardness(1.0F);
	pumpkinStem = LordNew cBlockStem(104, BM_Material::BM_MAT_plants, "pumpkinStem", "pumpkin_stem", pumpkin); pumpkinStem->setHardness(0.0F);//(new BlockStem(104, pumpkin)).setHardness(0.0F).setStepSound(soundWoodFootstep).setUnlocalizedName("pumpkinStem").func_111022_d("pumpkin_stem");
	melonStem = LordNew cBlockStem(105, BM_Material::BM_MAT_plants, "melonStem", "melon_stem", melon); melonStem->setHardness(0.0F);//(new BlockStem(105, melon)).setHardness(0.0F).setStepSound(soundWoodFootstep).setUnlocalizedName("pumpkinStem").func_111022_d("melon_stem");
	vine = LordNew cBlockVine(106, BM_Material::BM_MAT_vine, "vine", "vine"); vine->setHardness(0.2F); vine->setLightOpacity(0);
	fenceGate = LordNew cBlockFenceGate(107, BM_Material::BM_MAT_wood, "fenceGate", "fenceGate"); fenceGate->setHardness(2.0F); fenceGate->setResistance(5.0F);
	stairsBrick = LordNew cBlockStairs(108, BM_Material::BM_MAT_rock, "stairsBrick", "stairsBrick", brick, 0); stairsBrick->setHardness(2.0f); stairsBrick->setResistance(10.0f);
	stairsStoneBrick = LordNew	cBlockStairs(109, BM_Material::BM_MAT_rock, "stairsStoneBrickSmooth", "stairsStoneBrickSmooth", stoneBrick, 0); stairsStoneBrick->setHardness(1.5f); stairsStoneBrick->setResistance(10.0f);
	mycelium = LordNew cBlockMycelium(110, BM_Material::BM_MAT_grass, "mycel", "mycelium"); mycelium->setHardness(0.6F);
	waterlily = LordNew cBlockLilyPad(111, BM_Material::BM_MAT_plants, "waterlily", "waterlily"); waterlily->setHardness(0.0F);
	netherBrick = LordNew BlockClient(112, BM_Material::BM_MAT_rock, "netherBrick", "nether_brick"); netherBrick->setHardness(2.0F); netherBrick->setResistance(10.0F);
	netherFence = LordNew cBlockFence(113, BM_Material::BM_MAT_rock, "netherFence", "nether_brick"); netherFence->setHardness(2.0F); netherFence->setResistance(10.0F);
	stairsNetherBrick = LordNew cBlockStairs(114, BM_Material::BM_MAT_rock, "stairsNetherBrick", "stairsNetherBrick", netherBrick, 0); stairsNetherBrick->setHardness(2.0F); stairsNetherBrick->setResistance(10.0F);
	netherStalk = LordNew cBlockNetherStalk(115, BM_Material::BM_MAT_plants, "netherStalk", "nether_wart");
	enchantmentTable = LordNew cBlockEnchantmentTable(116, BM_Material::BM_MAT_rock, "enchantmentTable", "enchanting_table"); enchantmentTable->setHardness(5.0F); enchantmentTable->setResistance(2000.0F);
	brewingStand = LordNew cBlockBrewingStand(117, BM_Material::BM_MAT_iron, "brewingStand", "brewing_stand"); brewingStand->setHardness(0.5F); brewingStand->setLightValue(0.125F);
	cauldron = LordNew cBlockCauldron(118, BM_Material::BM_MAT_iron, "cauldron", "cauldron"); cauldron->setHardness(2.0F);
	endPortal = LordNew cBlockEndPortal(119, BM_Material::BM_MAT_portal, "portal", "portal"); endPortal->setHardness(-1.0F); endPortal->setLightValue(1.f); endPortal->setResistance(6000000.0F);
	endPortalFrame = LordNew cBlockEndPortalFrame(120, BM_Material::BM_MAT_rock, "endPortalFrame", "endframe"); endPortalFrame->setHardness(-1.0F); endPortalFrame->setResistance(6000000.0F); endPortalFrame->setLightValue(0.125F); //setCreativeTab(CreativeTabs.tabDecorations).
	whiteStone = LordNew BlockClient(121, BM_Material::BM_MAT_rock, "whiteStone", "end_stone"); whiteStone->setHardness(3.0F); whiteStone->setResistance(15.0F);
	dragonEgg = LordNew cBlockDragonEgg(122, BM_Material::BM_MAT_dragonEgg, "dragonEgg", "dragon_egg"); dragonEgg->setHardness(3.0F); dragonEgg->setResistance(15.0F); dragonEgg->setLightValue(0.125F);
	redstoneLampIdle = LordNew cBlockRedstoneLight(123, BM_Material::BM_MAT_redstoneLight, "redstoneLight", "redstone_lamp_off", false); redstoneLampIdle->setHardness(0.3F);
	redstoneLampActive = LordNew cBlockRedstoneLight(124, BM_Material::BM_MAT_redstoneLight, "redstoneLight", "redstone_lamp_on", true); redstoneLampActive->setLightValue(1.f); redstoneLampActive->setHardness(0.3F);
	woodDoubleSlab = LordNew cBlockWoodSlab(125, BM_Material::BM_MAT_wood, "woodSlab", "woodSlab", true); woodDoubleSlab->setHardness(2.0F); woodDoubleSlab->setResistance(5.0F);//(BlockHalfSlab)(new BlockWoodSlab(125, true)).setHardness(2.0F).setResistance(5.0F).setStepSound(soundWoodFootstep).setUnlocalizedName("woodSlab");
	woodSingleSlab = LordNew cBlockWoodSlab(126, BM_Material::BM_MAT_wood, "woodslab", "woodslab", false); woodSingleSlab->setHardness(2.0F); woodSingleSlab->setResistance(5.0F);//(BlockHalfSlab)(new BlockWoodSlab(126, false)).setHardness(2.0F).setResistance(5.0F).setStepSound(soundWoodFootstep).setUnlocalizedName("woodSlab");
	cocoaPlant = LordNew cBlockCocoa(127, BM_Material::BM_MAT_plants, "cocoa", "cocoa"); cocoaPlant->setHardness(0.2F); cocoaPlant->setResistance(5.0F);
	stairsSandStone = LordNew cBlockStairs(128, BM_Material::BM_MAT_rock, "stairsSandStone", "stairsSandStone", sandStone, 0); stairsSandStone->setHardness(0.8f); //(new BlockStairs(128, sandStone, 0)).setUnlocalizedName("stairsSandStone");	
	oreEmerald = LordNew cBlockOre(129, BM_Material::BM_MAT_rock, "oreEmerald", "emerald_ore");  oreEmerald->setHardness(3.0F); oreEmerald->setResistance(5.0F);
	enderChest = LordNew cBlockEnderChest(130, BM_Material::BM_MAT_rock, "enderChest", "obsidian", BlockEnderChest::BECT_OLD); enderChest->setHardness(22.5F); enderChest->setResistance(1000.0F); enderChest->setLightValue(0.5F);
	tripWireSource = LordNew cBlockTripWireSource(131, BM_Material::BM_MAT_circuits, "tripWireSource", "trip_wire_source");//(BlockTripWireSource)(new BlockTripWireSource(131)).setUnlocalizedName("tripWireSource").func_111022_d("trip_wire_source");
	tripWire = LordNew cBlockTripWire(132, BM_Material::BM_MAT_circuits, "tripWire", "trip_wire");
	blockEmerald = LordNew cBlockOreStorage(133, BM_Material::BM_MAT_iron, "blockEmerald", "emerald_block"); blockEmerald->setHardness(5.0F); blockEmerald->setResistance(10.0F);
	stairsWoodSpruce = LordNew cBlockStairs(134, BM_Material::BM_MAT_wood, "stairsWoodSpruce", "stairsWoodSpruce", planks, 1); stairsWoodSpruce->setHardness(2.0f); stairsWoodSpruce->setResistance(5.f);//(new BlockStairs(134, planks, 1)).setUnlocalizedName("stairsWoodSpruce");
	stairsWoodBirch = LordNew cBlockStairs(135, BM_Material::BM_MAT_wood, "stairsWoodBirch", "stairsWoodBirch", planks, 2); stairsWoodBirch->setHardness(2.0f); stairsWoodBirch->setResistance(5.f);//(new BlockStairs(135, planks, 2)).setUnlocalizedName("stairsWoodBirch");
	stairsWoodJungle = LordNew cBlockStairs(136, BM_Material::BM_MAT_wood, "stairsWoodJungle", "stairsWoodJungle", planks, 3); stairsWoodJungle->setHardness(2.0f); stairsWoodJungle->setResistance(5.f);//(new BlockStairs(136, planks, 3)).setUnlocalizedName("stairsWoodJungle");
	commandBlock = LordNew cBlockCommandBlock(137, BM_Material::BM_MAT_iron, "commandBlock", "command_block"); commandBlock->setBlockUnbreakable(); commandBlock->setResistance(6000000.0F);
	beacon = LordNew cBlockBeacon(138, BM_Material::BM_MAT_glass, "beacon", "beacon"); beacon->setLightValue(1.0F);
	cobblestoneWall = LordNew cBlockWall(139, cobblestone->getMaterial(), "cobbleWall", "cobbleWall");  cobblestoneWall->setHardness(cobblestone->getHardness()); cobblestoneWall->setResistance(cobblestone->getResistance() / 3.0F);
	flowerPot = LordNew cBlockFlowerPot(140, BM_Material::BM_MAT_circuits, "flowerPot", "flower_pot"); flowerPot->setHardness(0.0F);
	carrot = LordNew cBlockCarrot(141, BM_Material::BM_MAT_plants, "carrots", "carrots");//(new BlockCarrot(141)).setUnlocalizedName("carrots").func_111022_d("carrots");
	potato = LordNew cBlockPotato(142, BM_Material::BM_MAT_plants, "potatoes", "potatoes");//(new BlockPotato(142)).setUnlocalizedName("potatoes").func_111022_d("potatoes");
	woodenButton = LordNew cBlockButtonWood(143, BM_Material::BM_MAT_circuits, "button", "button"); woodenButton->setHardness(0.5F);
	skull = LordNew cBlockSkull(144, BM_Material::BM_MAT_circuits, "skull", "skull"); skull->setHardness(1.0F);
	anvil = LordNew cBlockAnvil(145, BM_Material::BM_MAT_anvil, "anvil", "anvil"); anvil->setHardness(5.0F); anvil->setResistance(2000.0F);
	chestTrapped = LordNew cBlockChest(146, BM_Material::BM_MAT_wood, "chestTrap", "chestTrap", BlockChest::BTT_BLOCK_TRAPPED); chestTrapped->setHardness(2.5F);
	pressurePlateGold = LordNew cBlockPressurePlateWeighted(147, BM_Material::BM_MAT_iron, "weightedPlate_light", "gold_block", 64, MOB_TYPE_ITEM); pressurePlateGold->setHardness(0.5F);
	pressurePlateIron = LordNew cBlockPressurePlateWeighted(148, BM_Material::BM_MAT_iron, "weightedPlate_heavy", "iron_block", 640, MOB_TYPE_ITEM); pressurePlateIron->setHardness(0.5F);
	redstoneComparatorIdle = LordNew cBlockComparator(149, BM_Material::BM_MAT_circuits, "comparator", "comparator_off", false); redstoneComparatorIdle->setHardness(0.0F); redstoneComparatorIdle->disableStats();
	redstoneComparatorActive = LordNew cBlockComparator(150, BM_Material::BM_MAT_circuits, "comparator", "comparator_on", true); redstoneComparatorActive->setHardness(0.0F); redstoneComparatorActive->setLightValue(0.625F); redstoneComparatorActive->disableStats();
	daylightSensor = LordNew cBlockDaylightDetector(151, BM_Material::BM_MAT_wood, "daylightDetector", "daylight_detector"); daylightSensor->setHardness(0.2F);
	blockRedstone = LordNew cBlockPoweredOre(152, BM_Material::BM_MAT_iron, "blockRedstone", "redstone_block"); blockRedstone->setHardness(5.0F); blockRedstone->setResistance(10.0F);
	oreNetherQuartz = LordNew cBlockOre(153, BM_Material::BM_MAT_rock, "netherquartz", "quartz_ore"); oreNetherQuartz->setHardness(3.0F); oreNetherQuartz->setResistance(5.0F);
	hopperBlock = LordNew cBlockHopper(154, BM_Material::BM_MAT_iron, "hopper", "hopper"); hopperBlock->setHardness(3.0F); hopperBlock->setResistance(8.0F);
	blockNetherQuartz = LordNew cBlockQuartz(155, BM_Material::BM_MAT_rock, "quartzBlock", "quartz_block"); blockNetherQuartz->setHardness(0.8F);
	stairsNetherQuartz = LordNew cBlockStairs(156, BM_Material::BM_MAT_rock, "stairsQuartz", "stairsQuartz", blockNetherQuartz, 0); stairsNetherQuartz->setHardness(0.8f);
	railActivator = LordNew cBlockRailPowered(157, BM_Material::BM_MAT_circuits, "activatorRail", "rail_activator"); railActivator->setHardness(0.7F);
	dropper = LordNew cBlockDropper(158, BM_Material::BM_MAT_rock, "dropper", "dropper"); dropper->setHardness(3.5F);
	clayHardenedStained = LordNew cBlockColored(159, BM_Material::BM_MAT_rock, "clayHardenedStained", "hardened_clay_stained"); clayHardenedStained->setHardness(1.25F); clayHardenedStained->setResistance(7.0F);
	stained_glass = LordNew cBlockStainedGlass(160, BM_Material::BM_MAT_glass, "stained_glass", "glass"); stained_glass->setHardness(0.3F); stained_glass->setLightOpacity(0);
	wood2 = LordNew cBlockLog2(162, BM_Material::BM_MAT_wood, "log", "log"); wood2->setHardness(2.0F);
	stairsWoodDarkOak = LordNew cBlockStairs(164, BM_Material::BM_MAT_wood, "stairsWoodDarkOak", "stairsWoodDarkOak", planks, 5); stairsWoodDarkOak->setHardness(2.0f); stairsWoodDarkOak->setResistance(5.f);
	slime = LordNew cBlockSlime(165, BM_Material::BM_MAT_glass, "slime", "slime_face"); slime->setHardness(1.0F);
	seaLantern = LordNew BlockClient(168, BM_Material::BM_MAT_glass, "seaLantern", "sea_lantern"); seaLantern->setHardness(0.3F); seaLantern->setLightValue(1.f);
	blockWithEffect = LordNew cBlockWithEffect(169, BM_Material::BM_MAT_rock, "normal_chest", "normal_chest"); blockWithEffect->setHardness(3.0f); blockWithEffect->setResistance(15.f);
	hayBlock = LordNew cBlockHay(170, BM_Material::BM_MAT_grass, "hayBlock", "hay_block"); hayBlock->setHardness(0.5F);
	woolCarpet = LordNew cBlockCarpet(171, BM_Material::BM_MAT_carpet, "woolCarpet", "woolCarpet"); woolCarpet->setHardness(0.1F); woolCarpet->setLightOpacity(0);
	clayHardened = LordNew BlockClient(172, BM_Material::BM_MAT_rock, "clayHardened", "hardened_clay"); clayHardened->setHardness(1.25F); clayHardened->setResistance(7.0F);
	blockCoal = LordNew BlockClient(173, BM_Material::BM_MAT_rock, "blockCoal", "coal_block"); blockCoal->setHardness(5.0F); blockCoal->setResistance(10.0F);
	packedIce = LordNew cBlockPackedIce(174, BM_Material::BM_MAT_packedIce, "icePacked", "ice_packed"); packedIce->setHardness(0.5F);
	redSandstoneDoubleSlab = LordNew cBlockRedStoneSlab(181, BM_Material::BM_MAT_sand, "redSandstoneSlab", "slab_double", true); redSandstoneDoubleSlab->setHardness(2.0F); redSandstoneDoubleSlab->setResistance(10.0F);
	redSandstoneSingleSlab = LordNew cBlockRedStoneSlab(182, BM_Material::BM_MAT_sand, "redSandstoneSlab", "slab_single", false); redSandstoneSingleSlab->setHardness(2.0F); redSandstoneSingleSlab->setResistance(10.0F);

	blockWithEffect_02 = LordNew cBlockWithEffect(207, BM_Material::BM_MAT_rock, "iron_chest", "iron_chest"); blockWithEffect_02->setHardness(3.0f); blockWithEffect_02->setResistance(15.f);
	blockWithEffect_03 = LordNew cBlockWithEffect(210, BM_Material::BM_MAT_rock, "silver_chest", "silver_chest"); blockWithEffect_03->setHardness(3.0f); blockWithEffect_03->setResistance(15.f);
	blockWithEffect_04 = LordNew cBlockWithEffect(211, BM_Material::BM_MAT_rock, "gold_chest", "gold_chest"); blockWithEffect_04->setHardness(3.0f); blockWithEffect_04->setResistance(15.f);
	pressurePlateWeightWood = LordNew cBlockPressurePlateWeighted(217, BM_Material::BM_MAT_wood, "pressurePlateWeightWood", "planks_oak", 10, MOB_TYPE_PLAYERS); pressurePlateWeightWood->setHardness(0.5F);
	chestNew = LordNew cBlockChest(219, BM_Material::BM_MAT_wood, "chestNew", "chest", BlockChest::BTT_BLOCK_NOT_TRAPPED_NEW); chestNew->setHardness(2.5F);
	enderChestNew = LordNew cBlockEnderChest(220, BM_Material::BM_MAT_rock, "enderChestNew", "obsidian", BlockEnderChest::BECT_NEW); enderChestNew->setHardness(22.5F); enderChestNew->setResistance(1000.0F); enderChestNew->setLightValue(0.5F);
	chestTrappedNew = LordNew cBlockChest(221, BM_Material::BM_MAT_wood, "chestTrappedNew", "chestTrap", BlockChest::BTT_BLOCK_TRAPPED_NEW); chestTrappedNew->setHardness(2.5F);
	railReceive = LordNew cBlockRailPowered(240, BM_Material::BM_MAT_circuits, "receiveRail", "rail_golden"); railPowered->setHardness(0.7F);
	
	keyDoor_01 = LordNew cBlockDoor(241, BM_Material::BM_MAT_wood, "keyDoor_01", "door_iron_new"); keyDoor_01->setHardness(3.0F); keyDoor_01->disableStats();
	keyDoor_02 = LordNew cBlockDoor(242, BM_Material::BM_MAT_wood, "keyDoor_02", "door_iron_new"); keyDoor_02->setHardness(3.0F); keyDoor_02->disableStats();
	keyDoor_03 = LordNew cBlockDoor(243, BM_Material::BM_MAT_wood, "keyDoor_03", "door_iron_new"); keyDoor_03->setHardness(3.0F); keyDoor_03->disableStats();
	keyDoor_04 = LordNew cBlockDoor(244, BM_Material::BM_MAT_wood, "keyDoor_04", "door_iron_new"); keyDoor_04->setHardness(3.0F); keyDoor_04->disableStats();
	keyDoor_05 = LordNew cBlockDoor(245, BM_Material::BM_MAT_wood, "keyDoor_05", "door_iron_new"); keyDoor_05->setHardness(3.0F); keyDoor_05->disableStats();
	keyDoor_06 = LordNew cBlockDoor(246, BM_Material::BM_MAT_wood, "keyDoor_06", "door_iron_new"); keyDoor_06->setHardness(3.0F); keyDoor_06->disableStats();
	keyDoor_07 = LordNew cBlockDoor(247, BM_Material::BM_MAT_wood, "keyDoor_07", "door_iron_new"); keyDoor_07->setHardness(3.0F); keyDoor_07->disableStats();
	keyDoor_08 = LordNew cBlockDoor(248, BM_Material::BM_MAT_wood, "keyDoor_08", "door_iron_new"); keyDoor_08->setHardness(3.0F); keyDoor_08->disableStats();
	keyDoor_09 = LordNew cBlockDoor(249, BM_Material::BM_MAT_wood, "keyDoor_09", "door_iron_new"); keyDoor_09->setHardness(3.0F); keyDoor_09->disableStats();
	keyDoor_10 = LordNew cBlockDoor(250, BM_Material::BM_MAT_wood, "keyDoor_10", "door_iron_new"); keyDoor_10->setHardness(3.0F); keyDoor_10->disableStats();

	concrete = LordNew cBlockColored(251, BM_Material::BM_MAT_rock, "concrete", "concrete"); concrete->setHardness(1.25F); concrete->setResistance(7.0F);
	concrete_powder = LordNew cBlockColored(252, BM_Material::BM_MAT_sand, "concrete_powder", "concrete_powder"); concrete_powder->setHardness(0.5F);
	air_wall = LordNew cBlockAirWall(253, BM_Material::BM_MAT_glass, "air_wall", "air_wall"); air_wall->setHardness(-1.0F); air_wall->setLightOpacity(0); air_wall->setResistance(6000000.0F);

	roadCarpet  = LordNew cBlockRoad(255, BM_Material::BM_MAT_rock, "roadCarpet", "road_carpet"); roadCarpet->setHardness(0.5F); roadCarpet->setLightOpacity(0);

	/*extend block from 1256~1999*/
	/*Crops id 1256~1399 */
	newCrops_01 = LordNew cBlockNewCrops(1256, BM_Material::BM_MAT_plants, "newCrops_01", "seeds_wheat1", 900);
	newCrops_02 = LordNew cBlockNewCrops(1257, BM_Material::BM_MAT_plants, "newCrops_02", "seeds_carrot", 901);
	newCrops_03 = LordNew cBlockNewCrops(1258, BM_Material::BM_MAT_plants, "newCrops_03", "seeds_tomato", 902);
	newCrops_04 = LordNew cBlockNewCrops(1259, BM_Material::BM_MAT_plants, "newCrops_04", "seeds_cabbage", 903);
	newCrops_05 = LordNew cBlockNewCrops(1260, BM_Material::BM_MAT_plants, "newCrops_05", "seeds_pea", 904);

	ranchCrop_100001 = LordNew cBlockNewCrops(1261, BM_Material::BM_MAT_plants, "100001_wheat", "100001_wheat", 100001, RENDER_TYPE_CROPS);
	ranchCrop_100002 = LordNew cBlockNewCrops(1262, BM_Material::BM_MAT_plants, "100002_corn", "100002_corn", 100002, RENDER_TYPE_CROPS);
	ranchCrop_100003 = LordNew cBlockNewCrops(1263, BM_Material::BM_MAT_plants, "100003_carrots", "100003_carrots", 100003, RENDER_TYPE_CROPS);
	ranchCrop_100004 = LordNew cBlockNewCrops(1264, BM_Material::BM_MAT_plants, "100004_sugar_cane", "100004_sugar_cane", 100004, RENDER_TYPE_CROPS);
	ranchCrop_100005 = LordNew cBlockNewCrops(1265, BM_Material::BM_MAT_plants, "100005_cotton", "100005_cotton", 100005);
	ranchCrop_100006 = LordNew cBlockNewCrops(1266, BM_Material::BM_MAT_plants, "100006_strawberry", "100006_strawberry", 100006);
	ranchCrop_100007 = LordNew cBlockNewCrops(1267, BM_Material::BM_MAT_plants, "100007_tomato", "100007_tomato", 100007, RENDER_TYPE_CROPS);
	ranchCrop_100008 = LordNew cBlockNewCrops(1268, BM_Material::BM_MAT_plants, "100008_pine", "100008_pine", 100008);
	ranchCrop_100009 = LordNew cBlockNewCrops(1269, BM_Material::BM_MAT_plants, "100009_potatoes", "100009_potatoes", 100009);
	ranchCrop_100010 = LordNew cBlockNewCrops(1270, BM_Material::BM_MAT_plants, "100010_cocoa_beans", "100010_cocoa_beans", 100010);
	ranchCrop_100011 = LordNew cBlockNewCrops(1271, BM_Material::BM_MAT_plants, "100011_rubber_wood", "100011_rubber_wood", 100011);
	ranchCrop_100012 = LordNew cBlockNewCrops(1272, BM_Material::BM_MAT_plants, "100012_silk", "100012_silk", 100012);
	ranchCrop_100013 = LordNew cBlockNewCrops(1273, BM_Material::BM_MAT_plants, "100013_chili", "100013_chili", 100013);
	ranchCrop_100014 = LordNew cBlockNewCrops(1274, BM_Material::BM_MAT_plants, "100014_rice", "100014_rice", 100014);
	ranchCrop_100015 = LordNew cBlockNewCrops(1275, BM_Material::BM_MAT_plants, "100015_the_rose", "100015_the_rose", 100015);
	ranchCrop_100016 = LordNew cBlockNewCrops(1276, BM_Material::BM_MAT_plants, "100016_jasmine", "100016_jasmine", 100016);
	ranchCrop_100017 = LordNew cBlockNewCrops(1277, BM_Material::BM_MAT_plants, "100017_coffee_beans", "100017_coffee_beans", 100017);

	nest_01 = LordNew cBlockNest(1291, BM_Material::BM_MAT_rock, "nest_01", "nest_01"); nest_01->setHardness(3.0f); nest_01->setResistance(15.f);
	nest_02 = LordNew cBlockNest(1292, BM_Material::BM_MAT_rock, "nest_02", "nest_02"); nest_02->setHardness(3.0f); nest_02->setResistance(15.f);

	fruits_01 = LordNew cBlockFruits(1301, BM_Material::BM_MAT_plants, "fruits_01", "fruits_01", RENDER_TYPE_FRUIT); fruits_01->setHardness(0.5F);
	fruits_02 = LordNew cBlockFruits(1302, BM_Material::BM_MAT_plants, "fruits_02", "fruits_02", RENDER_TYPE_FRUIT); fruits_02->setHardness(0.5F);
	fruits_03 = LordNew cBlockFruits(1303, BM_Material::BM_MAT_plants, "fruits_03", "fruits_03", RENDER_TYPE_FRUIT); fruits_03->setHardness(0.5F);
	fruits_04 = LordNew cBlockFruits(1304, BM_Material::BM_MAT_plants, "fruits_04", "fruits_04", RENDER_TYPE_FRUIT); fruits_04->setHardness(0.5F);
	fruits_05 = LordNew cBlockFruits(1305, BM_Material::BM_MAT_plants, "fruits_05", "fruits_05", RENDER_TYPE_FRUIT); fruits_05->setHardness(0.5F);
	fruits_06 = LordNew cBlockFruits(1306, BM_Material::BM_MAT_plants, "fruits_06", "fruits_06", RENDER_TYPE_FRUIT); fruits_06->setHardness(0.5F);
	fruits_07 = LordNew cBlockFruits(1307, BM_Material::BM_MAT_plants, "fruits_07", "fruits_07", RENDER_TYPE_FRUIT); fruits_07->setHardness(0.5F);
	fruits_08 = LordNew cBlockFruits(1308, BM_Material::BM_MAT_plants, "fruits_08", "fruits_08", RENDER_TYPE_FRUIT); fruits_08->setHardness(0.5F);
	fruits_09 = LordNew cBlockFruits(1309, BM_Material::BM_MAT_plants, "fruits_09", "fruits_09", RENDER_TYPE_FRUIT); fruits_09->setHardness(0.5F);
	fruits_10 = LordNew cBlockFruits(1310, BM_Material::BM_MAT_plants, "fruits_10", "fruits_10", RENDER_TYPE_FRUIT); fruits_10->setHardness(0.5F);
	fruits_11 = LordNew cBlockFruits(1311, BM_Material::BM_MAT_plants, "fruits_11", "fruits_11", RENDER_TYPE_FRUIT); fruits_11->setHardness(0.5F);
	fruits_12 = LordNew cBlockFruits(1312, BM_Material::BM_MAT_plants, "fruits_12", "fruits_12", RENDER_TYPE_FRUIT); fruits_12->setHardness(0.5F);

	// 旗帜方块需要满足16个方向,16个颜色(256种状态),meta只支持 0-15,暂时方法只能有16个id区分颜色,meta区分旋转方向,若chunk,section信息能有方法扩展meta,则无需16个id
	banner_00 = LordNew cBlockBanner(900, BM_Material::BM_MAT_anvil, "banner_00", "banner_slate", 0x00FFFFFF); banner_00->setHardness(0.5F); banner_00->setResistance(2000.0F); // white
	banner_01 = LordNew cBlockBanner(901, BM_Material::BM_MAT_anvil, "banner_01", "banner_slate", 0x0000A5FF); banner_01->setHardness(0.5F); banner_01->setResistance(2000.0F); // orange
	banner_02 = LordNew cBlockBanner(902, BM_Material::BM_MAT_anvil, "banner_02", "banner_slate", 0x00FF00FF); banner_02->setHardness(0.5F); banner_02->setResistance(2000.0F); // megenta
	banner_03 = LordNew cBlockBanner(903, BM_Material::BM_MAT_anvil, "banner_03", "banner_slate", 0x00E6D8AD); banner_03->setHardness(0.5F); banner_03->setResistance(2000.0F); // light blue
	banner_04 = LordNew cBlockBanner(904, BM_Material::BM_MAT_anvil, "banner_04", "banner_slate", 0x0000FFFF); banner_04->setHardness(0.5F); banner_04->setResistance(2000.0F); // yellow
	banner_05 = LordNew cBlockBanner(905, BM_Material::BM_MAT_anvil, "banner_05", "banner_slate", 0x0032CD32); banner_05->setHardness(0.5F); banner_05->setResistance(2000.0F); // lime
	banner_06 = LordNew cBlockBanner(906, BM_Material::BM_MAT_anvil, "banner_06", "banner_slate", 0x00CBC0FF); banner_06->setHardness(0.5F); banner_06->setResistance(2000.0F); // pink
	banner_07 = LordNew cBlockBanner(907, BM_Material::BM_MAT_anvil, "banner_07", "banner_slate", 0x00696969); banner_07->setHardness(0.5F); banner_07->setResistance(2000.0F); // gray
	banner_08 = LordNew cBlockBanner(908, BM_Material::BM_MAT_anvil, "banner_08", "banner_slate", 0x009C9C9C); banner_08->setHardness(0.5F); banner_08->setResistance(2000.0F); // light gray
	banner_09 = LordNew cBlockBanner(909, BM_Material::BM_MAT_anvil, "banner_09", "banner_slate", 0x00FFFF00); banner_09->setHardness(0.5F); banner_09->setResistance(2000.0F); // cyan
	banner_10 = LordNew cBlockBanner(910, BM_Material::BM_MAT_anvil, "banner_10", "banner_slate", 0x00F020A0); banner_10->setHardness(0.5F); banner_10->setResistance(2000.0F); // purple
	banner_11 = LordNew cBlockBanner(911, BM_Material::BM_MAT_anvil, "banner_11", "banner_slate", 0x00FF0000); banner_11->setHardness(0.5F); banner_11->setResistance(2000.0F); // blue
	banner_12 = LordNew cBlockBanner(912, BM_Material::BM_MAT_anvil, "banner_12", "banner_slate", 0x0013458B); banner_12->setHardness(0.5F); banner_12->setResistance(2000.0F); // brown
	banner_13 = LordNew cBlockBanner(913, BM_Material::BM_MAT_anvil, "banner_13", "banner_slate", 0x0000FF00); banner_13->setHardness(0.5F); banner_13->setResistance(2000.0F); // green
	banner_14 = LordNew cBlockBanner(914, BM_Material::BM_MAT_anvil, "banner_14", "banner_slate", 0x000000FF); banner_14->setHardness(0.5F); banner_14->setResistance(2000.0F); // red
	banner_15 = LordNew cBlockBanner(915, BM_Material::BM_MAT_anvil, "banner_15", "banner_slate", 0x00010101); banner_15->setHardness(0.5F); banner_15->setResistance(2000.0F); // black

	transparency = LordNew cBlockTransparency(916, BM_Material::BM_MAT_glass, "transparency", "transparency"); transparency->setHardness(-1.0F); transparency->setLightOpacity(0); transparency->setResistance(6000000.0F);

	sCanBlockGrass[0] = true;

	for (int i = 0; i < MAX_BLOCK_COUNT; ++i)
	{
		if (!sBlocks[i])
			continue;
		scBlocks[i] = dynamic_cast<cBlock*>(sBlocks[i]);
	}

	// initialize sound config.
	scBlocks[BLOCK_ID_STONE]->setStepSound(&cBlock::soundStoneFootstep);
	scBlocks[BLOCK_ID_GRASS]->setStepSound(&cBlock::soundGrassFootstep);
	scBlocks[BLOCK_ID_DIRT]->setStepSound(&cBlock::soundGravelFootstep);
	scBlocks[BLOCK_ID_COBBLE_STONE]->setStepSound(&cBlock::soundStoneFootstep);
	scBlocks[BLOCK_ID_PLANKS]->setStepSound(&cBlock::soundWoodFootstep);
	scBlocks[BLOCK_ID_SAPLING]->setStepSound(&cBlock::soundWoodFootstep);
	scBlocks[BLOCK_ID_BEDROCK]->setStepSound(&cBlock::soundStoneFootstep);
	scBlocks[BLOCK_ID_SAND]->setStepSound(&cBlock::soundSandFootstep);
	scBlocks[BLOCK_ID_GRAVEL]->setStepSound(&cBlock::soundGravelFootstep);
	scBlocks[BLOCK_ID_ORE_GOLD]->setStepSound(&cBlock::soundStoneFootstep);
	scBlocks[BLOCK_ID_ORE_IRON]->setStepSound(&cBlock::soundStoneFootstep);
	scBlocks[BLOCK_ID_ORE_COAL]->setStepSound(&cBlock::soundStoneFootstep);
	scBlocks[BLOCK_ID_WOOD]->setStepSound(&cBlock::soundWoodFootstep);
	scBlocks[BLOCK_ID_LEAVES]->setStepSound(&cBlock::soundGrassFootstep);
	scBlocks[BLOCK_ID_SPONGE]->setStepSound(&cBlock::soundGrassFootstep);
	scBlocks[BLOCK_ID_GLASS]->setStepSound(&cBlock::soundGlassFootstep);
	scBlocks[BLOCK_ID_ORE_LAPIS]->setStepSound(&cBlock::soundStoneFootstep);
	scBlocks[BLOCK_ID_BLOCK_LAPIS]->setStepSound(&cBlock::soundStoneFootstep);
	scBlocks[BLOCK_ID_DISPENSER]->setStepSound(&cBlock::soundStoneFootstep);
	scBlocks[BLOCK_ID_SAND_STONE]->setStepSound(&cBlock::soundStoneFootstep);
	scBlocks[BLOCK_ID_RAIL_POWERED]->setStepSound(&cBlock::soundMetalFootstep);
	scBlocks[BLOCK_ID_RAIL_DETECTOR]->setStepSound(&cBlock::soundMetalFootstep);
	scBlocks[BLOCK_ID_TALL_GRASS]->setStepSound(&cBlock::soundGrassFootstep);
	scBlocks[BLOCK_ID_DEADBUSH]->setStepSound(&cBlock::soundGrassFootstep);
	scBlocks[BLOCK_ID_CLOTH]->setStepSound(&cBlock::soundClothFootstep);
	scBlocks[BLOCK_ID_PLANT_YELLOW]->setStepSound(&cBlock::soundGrassFootstep);
	scBlocks[BLOCK_ID_PLANT_RED]->setStepSound(&cBlock::soundGrassFootstep);
	scBlocks[BLOCK_ID_MUSHROOM_BROWN]->setStepSound(&cBlock::soundGrassFootstep);
	scBlocks[BLOCK_ID_MUSHROOM_RED]->setStepSound(&cBlock::soundGrassFootstep);
	scBlocks[BLOCK_ID_BLOCK_GOLD]->setStepSound(&cBlock::soundMetalFootstep);
	scBlocks[BLOCK_ID_BLOCK_IRON]->setStepSound(&cBlock::soundMetalFootstep);
	scBlocks[BLOCK_ID_STONE_DOUBLE_SLAB]->setStepSound(&cBlock::soundStoneFootstep);
	scBlocks[BLOCK_ID_STONE_SINGLE_SLAB]->setStepSound(&cBlock::soundStoneFootstep);
	scBlocks[BLOCK_ID_TNT]->setStepSound(&cBlock::soundGrassFootstep);
	scBlocks[BLOCK_ID_BOOKSHELF]->setStepSound(&cBlock::soundWoodFootstep);
	scBlocks[BLOCK_ID_COBBLE_STONE_MOSSY]->setStepSound(&cBlock::soundStoneFootstep);
	scBlocks[BLOCK_ID_OBSIDIAN]->setStepSound(&cBlock::soundStoneFootstep);
	scBlocks[BLOCK_ID_TORCH_WOOD]->setStepSound(&cBlock::soundWoodFootstep);
	scBlocks[BLOCK_ID_FIRE]->setStepSound(&cBlock::soundWoodFootstep);
	scBlocks[BLOCK_ID_MOB_SPAWNER]->setStepSound(&cBlock::soundMetalFootstep);
	scBlocks[BLOCK_ID_CHEST]->setStepSound(&cBlock::soundWoodFootstep);
	scBlocks[BLOCK_ID_REDSTONE_WIRE]->setStepSound(&cBlock::soundPowderFootstep);
	scBlocks[BLOCK_ID_ORE_DIAMOND]->setStepSound(&cBlock::soundStoneFootstep);
	scBlocks[BLOCK_ID_BLOCK_DIAMOND]->setStepSound(&cBlock::soundMetalFootstep);
	scBlocks[BLOCK_ID_WORK_BENCH]->setStepSound(&cBlock::soundWoodFootstep);
	scBlocks[BLOCK_ID_TILLED_FIELD]->setStepSound(&cBlock::soundGravelFootstep);
	scBlocks[BLOCK_ID_FURNACE_IDLE]->setStepSound(&cBlock::soundStoneFootstep);
	scBlocks[BLOCK_ID_FURNACE_BURNING]->setStepSound(&cBlock::soundStoneFootstep);
	scBlocks[BLOCK_ID_SIGN_POST]->setStepSound(&cBlock::soundWoodFootstep);
	scBlocks[BLOCK_ID_DOOR_WOOD]->setStepSound(&cBlock::soundWoodFootstep);
	scBlocks[BLOCK_ID_LADDER]->setStepSound(&cBlock::soundLadderFootstep);
	scBlocks[BLOCK_ID_RAIL]->setStepSound(&cBlock::soundMetalFootstep);
	scBlocks[BLOCK_ID_SIGN_WALL]->setStepSound(&cBlock::soundWoodFootstep);
	scBlocks[BLOCK_ID_LEVER]->setStepSound(&cBlock::soundWoodFootstep);
	scBlocks[BLOCK_ID_PRESSURE_PLATE_STONE]->setStepSound(&cBlock::soundStoneFootstep);
	scBlocks[BLOCK_ID_DOOR_IRON]->setStepSound(&cBlock::soundMetalFootstep);
	scBlocks[BLOCK_ID_PRESSURE_PLATE_PLANKS]->setStepSound(&cBlock::soundWoodFootstep);
	scBlocks[BLOCK_ID_ORE_REDSTONE]->setStepSound(&cBlock::soundStoneFootstep);
	scBlocks[BLOCK_ID_ORE_REDSTONE_GLOWING]->setStepSound(&cBlock::soundStoneFootstep);
	scBlocks[BLOCK_ID_TORCH_REDSTONE_IDLE]->setStepSound(&cBlock::soundWoodFootstep);
	scBlocks[BLOCK_ID_TORCH_REDSTONE_ACTIVE]->setStepSound(&cBlock::soundWoodFootstep);
	scBlocks[BLOCK_ID_STONE_BUTTON]->setStepSound(&cBlock::soundStoneFootstep);
	scBlocks[BLOCK_ID_SNOW]->setStepSound(&cBlock::soundSnowFootstep);
	scBlocks[BLOCK_ID_ICE]->setStepSound(&cBlock::soundGlassFootstep);
	scBlocks[BLOCK_ID_BLOCK_SNOW]->setStepSound(&cBlock::soundSnowFootstep);
	scBlocks[BLOCK_ID_CACTUS]->setStepSound(&cBlock::soundClothFootstep);
	scBlocks[BLOCK_ID_CLAY_BLOCK]->setStepSound(&cBlock::soundGravelFootstep);
	scBlocks[BLOCK_ID_REED]->setStepSound(&cBlock::soundGrassFootstep);
	scBlocks[BLOCK_ID_JUKE_BOX]->setStepSound(&cBlock::soundStoneFootstep);
	scBlocks[BLOCK_ID_FENCE]->setStepSound(&cBlock::soundWoodFootstep);
	scBlocks[BLOCK_ID_PUMPKIN]->setStepSound(&cBlock::soundWoodFootstep);
	scBlocks[BLOCK_ID_NETHERRACK]->setStepSound(&cBlock::soundStoneFootstep);
	scBlocks[BLOCK_ID_SLOW_SAND]->setStepSound(&cBlock::soundSandFootstep);
	scBlocks[BLOCK_ID_GLOW_STONE]->setStepSound(&cBlock::soundGlassFootstep);
	scBlocks[BLOCK_ID_PORTAL]->setStepSound(&cBlock::soundGlassFootstep);
	scBlocks[BLOCK_ID_PUMPKIN_LANTERN]->setStepSound(&cBlock::soundWoodFootstep);
	scBlocks[BLOCK_ID_CAKE]->setStepSound(&cBlock::soundClothFootstep);
	scBlocks[BLOCK_ID_REDSTONE_REPEATER_IDLE]->setStepSound(&cBlock::soundWoodFootstep);
	scBlocks[BLOCK_ID_REDSTONE_REPEATER_ACTIVE]->setStepSound(&cBlock::soundWoodFootstep);
	scBlocks[BLOCK_ID_LOCKED_CHEST]->setStepSound(&cBlock::soundWoodFootstep);
	scBlocks[BLOCK_ID_TRAP_DOOR]->setStepSound(&cBlock::soundWoodFootstep);
	scBlocks[BLOCK_ID_STONE_BRICK]->setStepSound(&cBlock::soundStoneFootstep);
	scBlocks[BLOCK_ID_MUSHROOMCAP_BROWN]->setStepSound(&cBlock::soundWoodFootstep);
	scBlocks[BLOCK_ID_MUSHROOMCAP_RED]->setStepSound(&cBlock::soundWoodFootstep);
	scBlocks[BLOCK_ID_FENCE_IRON]->setStepSound(&cBlock::soundMetalFootstep);
	scBlocks[BLOCK_ID_THIN_GLASS]->setStepSound(&cBlock::soundGlassFootstep);
	scBlocks[BLOCK_ID_MELON]->setStepSound(&cBlock::soundWoodFootstep);
	scBlocks[BLOCK_ID_PUMPKIN_STEM]->setStepSound(&cBlock::soundWoodFootstep);
	scBlocks[BLOCK_ID_MELON_STEM]->setStepSound(&cBlock::soundWoodFootstep);
	scBlocks[BLOCK_ID_VINE]->setStepSound(&cBlock::soundGrassFootstep);
	scBlocks[BLOCK_ID_FENCE_GATE]->setStepSound(&cBlock::soundWoodFootstep);
	scBlocks[BLOCK_ID_MYCELIUM]->setStepSound(&cBlock::soundGrassFootstep);
	scBlocks[BLOCK_ID_WATERLILY]->setStepSound(&cBlock::soundGrassFootstep);
	scBlocks[BLOCK_ID_NETHER_BRICK]->setStepSound(&cBlock::soundStoneFootstep);
	scBlocks[BLOCK_ID_NETHER_FENCE]->setStepSound(&cBlock::soundStoneFootstep);
	scBlocks[BLOCK_ID_END_PROTAL_FRAME]->setStepSound(&cBlock::soundGlassFootstep);
	scBlocks[BLOCK_ID_WHITE_STONE]->setStepSound(&cBlock::soundStoneFootstep);
	scBlocks[BLOCK_ID_DRAGON_EGG]->setStepSound(&cBlock::soundStoneFootstep);
	scBlocks[BLOCK_ID_REDSTONE_LAMP_IDLE]->setStepSound(&cBlock::soundGlassFootstep);
	scBlocks[BLOCK_ID_REDSTONE_LAMP_ACTIVE]->setStepSound(&cBlock::soundGlassFootstep);
	scBlocks[BLOCK_ID_WOOD_DOUBLE_SLAB]->setStepSound(&cBlock::soundWoodFootstep);
	scBlocks[BLOCK_ID_WOOD_SINGLE_SLAB]->setStepSound(&cBlock::soundWoodFootstep);
	scBlocks[BLOCK_ID_COCOAPLANT]->setStepSound(&cBlock::soundWoodFootstep);
	scBlocks[BLOCK_ID_ORE_EMERALD]->setStepSound(&cBlock::soundStoneFootstep);
	scBlocks[BLOCK_ID_ENDER_CHEST]->setStepSound(&cBlock::soundStoneFootstep);
	scBlocks[BLOCK_ID_BLOCK_EMERALD]->setStepSound(&cBlock::soundMetalFootstep);
	scBlocks[BLOCK_ID_FLOWER_POT]->setStepSound(&cBlock::soundPowderFootstep);
	scBlocks[BLOCK_ID_WOODEN_BUTTON]->setStepSound(&cBlock::soundWoodFootstep);
	scBlocks[BLOCK_ID_SKULL]->setStepSound(&cBlock::soundStoneFootstep);
	scBlocks[BLOCK_ID_ANVIL]->setStepSound(&cBlock::soundAnvilFootstep);
	scBlocks[BLOCK_ID_CHEST_TRAPPED]->setStepSound(&cBlock::soundWoodFootstep);
	scBlocks[BLOCK_ID_PRESSURE_PLATE_GOLD]->setStepSound(&cBlock::soundWoodFootstep);
	scBlocks[BLOCK_ID_PRESSURE_PLATE_IRON]->setStepSound(&cBlock::soundWoodFootstep);
	scBlocks[BLOCK_ID_REDSTONE_COMPARATOR_IDLE]->setStepSound(&cBlock::soundWoodFootstep);
	scBlocks[BLOCK_ID_REDSTONE_COMPARATOR_ACTIVE]->setStepSound(&cBlock::soundWoodFootstep);
	scBlocks[BLOCK_ID_DAYLIGHT_DETECTOR]->setStepSound(&cBlock::soundWoodFootstep);
	scBlocks[BLOCK_ID_BLOCK_REDSTONE]->setStepSound(&cBlock::soundMetalFootstep);
	scBlocks[BLOCK_ID_ORE_NETHER_QUARTZ]->setStepSound(&cBlock::soundStoneFootstep);
	scBlocks[BLOCK_ID_HOPPER]->setStepSound(&cBlock::soundWoodFootstep);
	scBlocks[BLOCK_ID_BLOCK_NETHER_QUARTZ]->setStepSound(&cBlock::soundStoneFootstep);
	scBlocks[BLOCK_ID_RAIL_ACTIVATOR]->setStepSound(&cBlock::soundMetalFootstep);
	scBlocks[BLOCK_ID_DROPPER]->setStepSound(&cBlock::soundStoneFootstep);
	scBlocks[BLOCK_ID_CLAY_HARDENED_STAINED]->setStepSound(&cBlock::soundStoneFootstep);
	scBlocks[BLOCK_ID_WOOD2]->setStepSound(&cBlock::soundWoodFootstep);
	scBlocks[BLOCK_ID_CLAY_BLOCK]->setStepSound(&cBlock::soundGrassFootstep);
	scBlocks[BLOCK_ID_WOOL_CARPET]->setStepSound(&cBlock::soundClothFootstep);
	scBlocks[BLOCK_ID_CLAY_HARDENED]->setStepSound(&cBlock::soundStoneFootstep);
	scBlocks[BLOCK_ID_BLOCK_COAL]->setStepSound(&cBlock::soundStoneFootstep);
	scBlocks[BLOCK_ID_BLOCK_PACKED_ICE]->setStepSound(&cBlock::soundGlassFootstep);
	scBlocks[BLOCK_ID_CHEST_NEW]->setStepSound(&cBlock::soundWoodFootstep);
	scBlocks[BLOCK_ID_ENDER_CHEST_NEW]->setStepSound(&cBlock::soundStoneFootstep);
	scBlocks[BLOCK_ID_CHEST_TRAPPED_NEW]->setStepSound(&cBlock::soundWoodFootstep);
	scBlocks[BLOCK_ID_SEA_LANTERN]->setStepSound(&cBlock::soundGlassFootstep);
	scBlocks[BLOCK_ID_REDSAND_STONE_DOUBLESLAB]->setStepSound(&cBlock::soundStoneFootstep);
	scBlocks[BLOCK_ID_REDSAND_STONE_SINGLESLAB]->setStepSound(&cBlock::soundStoneFootstep);
	scBlocks[BLOCK_ID_PRESSURE_PLATE_WEIGHT_WOOD]->setStepSound(&cBlock::soundWoodFootstep);
	scBlocks[BLOCK_ID_STAINED_GLASS]->setStepSound(&cBlock::soundGlassFootstep);
}

void cBlockManager::cRegistBlockItems()
{
	Item::itemsList[cloth->getItemBlockId()] = LordNew cItemCloth(cloth->getItemBlockId() - 256, "cloth");
	Item::itemsList[clayHardenedStained->getItemBlockId()] = LordNew cItemCloth(clayHardenedStained->getItemBlockId() - 256, "clayHardenedStained");
	Item::itemsList[woolCarpet->getItemBlockId()] = LordNew cItemCloth(woolCarpet->getItemBlockId() - 256, "woolCarpet");
	//Item::itemsList[roadCarpet->getItemBlockId()] = LordNew cItemCloth(roadCarpet->getItemBlockId() - 256, "roadCarpet");
	Item::itemsList[wood->getItemBlockId()] = LordNew cItemMultiTextureTile(wood->getItemBlockId() - 256, "log", BlockLog1::woodType, scBlocks[BLOCK_ID_WOOD]);
	Item::itemsList[planks->getItemBlockId()] = LordNew cItemMultiTextureTile(planks->getItemBlockId() - 256, "wood", BlockWood::woodType, scBlocks[BLOCK_ID_PLANKS]);
	Item::itemsList[silverfish->getItemBlockId()] = LordNew cItemMultiTextureTile(silverfish->getItemBlockId() - 256, "monsterStoneEgg", BlockSilverfish::silverfishStoneTypes, scBlocks[BLOCK_ID_SILVER_FISH]);
	Item::itemsList[stoneBrick->getItemBlockId()] = LordNew cItemMultiTextureTile(stoneBrick->getItemBlockId() - 256, "stonebricksmooth", BlockStoneBrick::STONE_BRICK_TYPES, scBlocks[BLOCK_ID_STONE_BRICK]);
	Item::itemsList[sandStone->getItemBlockId()] = LordNew cItemMultiTextureTile(sandStone->getItemBlockId() - 256, "sandStone", BlockSandStone::SAND_STONE_TYPES, scBlocks[BLOCK_ID_SAND_STONE]);
	Item::itemsList[blockNetherQuartz->getItemBlockId()] = LordNew cItemMultiTextureTile(blockNetherQuartz->getItemBlockId() - 256, "quartzBlock", BlockQuartz::quartzBlockTypes, scBlocks[BLOCK_ID_BLOCK_NETHER_QUARTZ]);
	Item::itemsList[stoneSingleSlab->getItemBlockId()] = LordNew cItemSlab(stoneSingleSlab->getItemBlockId() - 256, "stoneSlab", stoneSingleSlab, stoneDoubleSlab, false);
	Item::itemsList[stoneDoubleSlab->getItemBlockId()] = LordNew cItemSlab(stoneDoubleSlab->getItemBlockId() - 256, "stoneSlab", stoneSingleSlab, stoneDoubleSlab, true);
	Item::itemsList[woodSingleSlab->getItemBlockId()] = LordNew cItemSlab(woodSingleSlab->getItemBlockId() - 256, "woodSlab", woodSingleSlab, woodDoubleSlab, false);
	Item::itemsList[woodDoubleSlab->getItemBlockId()] = LordNew cItemSlab(woodDoubleSlab->getItemBlockId() - 256, "woodSlab", woodSingleSlab, woodDoubleSlab, true);
	Item::itemsList[sapling->getItemBlockId()] = LordNew cItemMultiTextureTile(sapling->getItemBlockId() - 256, "sapling", BlockSapling::WOOD_TYPES, scBlocks[BLOCK_ID_SAPLING]);
	Item::itemsList[leaves->getItemBlockId()] = LordNew cItemLeaves(leaves->getItemBlockId() - 256, "leaves");
	Item::itemsList[vine->getItemBlockId()] = LordNew cItemColoredImp(vine->getItemBlockId() - 256, false);
	Item::itemsList[tallGrass->getItemBlockId()] = LordNew cItemColoredImp(tallGrass->getItemBlockId() - 256, true); //setBlockNames(new String[]{ "shrub", "grass", "fern" });
	Item::itemsList[snow->getItemBlockId()] = LordNew cItemSnow(snow->getItemBlockId() - 256, scBlocks[BLOCK_ID_SNOW]); //, snow);
	Item::itemsList[waterlily->getItemBlockId()] = LordNew cItemLilyPad(waterlily->getItemBlockId() - 256);
	Item::itemsList[pistonBase->getItemBlockId()] = LordNew cItemPiston(pistonBase->getItemBlockId() - 256);
	Item::itemsList[pistonStickyBase->getItemBlockId()] = LordNew cItemPiston(pistonStickyBase->getItemBlockId() - 256);
	Item::itemsList[cobblestoneWall->getItemBlockId()] = LordNew cItemMultiTextureTile(cobblestoneWall->getItemBlockId() - 256, "cobbleWall", BlockWall::types, scBlocks[BLOCK_ID_COBBLE_STONE_WALL]);
	Item::itemsList[anvil->getItemBlockId()] = LordNew cItemAnvilBlock(anvil->getItemBlockId() - 256, "anvil", scBlocks[BLOCK_ID_ANVIL]);
	Item::itemsList[wood2->getItemBlockId()] = LordNew cItemMultiTextureTile(wood2->getItemBlockId() - 256, "log", BlockLog2::woodType, scBlocks[BLOCK_ID_WOOD2]);
	Item::itemsList[tnt->getItemBlockId()] = LordNew cItemTnt(tnt->getItemBlockId() - 256, "tnt");
	Item::itemsList[concrete->getItemBlockId()] = LordNew cItemCloth(concrete->getItemBlockId() - 256, "concrete");
	Item::itemsList[concrete_powder->getItemBlockId()] = LordNew cItemCloth(concrete_powder->getItemBlockId() - 256, "concrete_powder");
	Item::itemsList[redSandstoneSingleSlab->getItemBlockId()] = LordNew cItemSlab(redSandstoneSingleSlab->getItemBlockId() - 256, "redSandstoneSlab", redSandstoneSingleSlab, redSandstoneDoubleSlab, false);
	Item::itemsList[redSandstoneDoubleSlab->getItemBlockId()] = LordNew cItemSlab(redSandstoneDoubleSlab->getItemBlockId() - 256, "redSandstoneSlab", redSandstoneSingleSlab, redSandstoneDoubleSlab, true);
	Item::itemsList[stained_glass->getItemBlockId()] = LordNew cItemCloth(stained_glass->getItemBlockId() - 256, "glass");

	Item::itemsList[banner_00->getItemBlockId()] = LordNew cItemBanner(banner_00->getItemBlockId() - 256, "banner_00");
	Item::itemsList[banner_01->getItemBlockId()] = LordNew cItemBanner(banner_01->getItemBlockId() - 256, "banner_01");
	Item::itemsList[banner_02->getItemBlockId()] = LordNew cItemBanner(banner_02->getItemBlockId() - 256, "banner_02");
	Item::itemsList[banner_03->getItemBlockId()] = LordNew cItemBanner(banner_03->getItemBlockId() - 256, "banner_03");
	Item::itemsList[banner_04->getItemBlockId()] = LordNew cItemBanner(banner_04->getItemBlockId() - 256, "banner_04");
	Item::itemsList[banner_05->getItemBlockId()] = LordNew cItemBanner(banner_05->getItemBlockId() - 256, "banner_05");
	Item::itemsList[banner_06->getItemBlockId()] = LordNew cItemBanner(banner_06->getItemBlockId() - 256, "banner_06");
	Item::itemsList[banner_07->getItemBlockId()] = LordNew cItemBanner(banner_07->getItemBlockId() - 256, "banner_07");
	Item::itemsList[banner_08->getItemBlockId()] = LordNew cItemBanner(banner_08->getItemBlockId() - 256, "banner_08");
	Item::itemsList[banner_09->getItemBlockId()] = LordNew cItemBanner(banner_09->getItemBlockId() - 256, "banner_09");
	Item::itemsList[banner_10->getItemBlockId()] = LordNew cItemBanner(banner_10->getItemBlockId() - 256, "banner_10");
	Item::itemsList[banner_11->getItemBlockId()] = LordNew cItemBanner(banner_11->getItemBlockId() - 256, "banner_11");
	Item::itemsList[banner_12->getItemBlockId()] = LordNew cItemBanner(banner_12->getItemBlockId() - 256, "banner_12");
	Item::itemsList[banner_13->getItemBlockId()] = LordNew cItemBanner(banner_13->getItemBlockId() - 256, "banner_13");
	Item::itemsList[banner_14->getItemBlockId()] = LordNew cItemBanner(banner_14->getItemBlockId() - 256, "banner_14");
	Item::itemsList[banner_15->getItemBlockId()] = LordNew cItemBanner(banner_15->getItemBlockId() - 256, "banner_15");

	for (int i = 0; i < MAX_BLOCK_COUNT; ++i)
	{
		Block* pBlock = sBlocks[i];
		if (!pBlock)
			continue;

		int itemId = pBlock->getItemBlockId();
		if (Item::itemsList[itemId] == NULL)
		{
			Item::itemsList[itemId] = LordNew cItemBlockImp(itemId - 256);
			pBlock->initializeBlock();
		}

		bool useNeighborBrightness = false;

		if (i > 0 && scBlocks[i] && scBlocks[i]->getRenderType() == RENDER_TYPE_STAIRS)
		{
			useNeighborBrightness = true;
		}

		BlockHalfSlab* pslab = dynamic_cast<BlockHalfSlab*>(pBlock);
		if (i > 0 && pslab)
		{
			useNeighborBrightness = true;
		}

		if (i == BLOCK_ID_TILLED_FIELD)
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

void cBlockManager::registSprite(TextureAtlasRegister* regist)
{
	for (int i = 0; i < MAX_BLOCK_COUNT; ++i)
	{
		if (!scBlocks[i])
			continue;
		scBlocks[i]->registerIcons(regist);
	}
}

}
