/********************************************************************
filename: 	BlockManager.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-11-3
*********************************************************************/
#ifndef __BLOCK_MANAGER_HEADER__
#define __BLOCK_MANAGER_HEADER__

#include "Core.h"
using namespace LORD;

namespace BLOCKMAN
{

class Block;
class BlockPistonBase;
class BlockPistonExtension;
class TextureAtlasRegister;
class BlockPistonMoving;
class BlockTripWireSource;
class BlockRedstoneWire;
class BlockRedstoneRepeater;
class BlockComparator;
class BM_Material;
class BlockDoor;
class BlockWithEffect;
class BlockNewCrops;
class BlockFruits;
class BlockNest;

class BlockManager : public ObjectAlloc
{
public:
	static const int BASIC_BLOCK_ID_BEGIN = 0;
	static const int BASIC_BLOCK_ID_END = 256;
	static const int EXTENDED_BLOCK_ID_BEGIN = BASIC_BLOCK_ID_END;
	static const int EXTENDED_BLOCK_ID_END = 1000;
	static const int MAX_CUSTOM_BLOCK_COUNT = 1000;
	static const int CUSTOM_BLOCK_ID_BEGIN = EXTENDED_BLOCK_ID_END;
	static const int CUSTOM_BLOCK_ID_END = CUSTOM_BLOCK_ID_BEGIN + MAX_CUSTOM_BLOCK_COUNT;
	static const int MAX_BLOCK_COUNT = CUSTOM_BLOCK_ID_END;
	
	static void initialize();
	static void unInitialize();

	static void registBlockItems();
	
	static void onBlockCreated(Block* pBlock);
	
	static void setLightOpacity(int blockID, int lightOpacity);
	static void setLightValue(int blockID, float lightValue);
	static bool isNormalCube(int blockID);

	// Static version of isAssociatedBlockID.
	static bool isAssociatedBlockID(int blockID1, int blockID2);

	static bool isAttachmentBlock(int blockId);
	static bool isDoorBlock(int blockId);

	static Block* getBlockById(int blockId);

	static bool isStairsBlock(int blockId);
	static ui8 StairsBlockRotate(ui8 meta, bool xImage, bool zImage);

	/*
	Block**	blockList() { return m_blockList; }
	i8*	opaqueCubeLookup() { return m_opaqueCubeLookup; }
	int*	lightOpacity() { return m_lightOpacity; }
	i8*	canBlockGrass() { return m_canBlockGrass; }
	int*	lightValue() { return m_lightValue; }
	i8*	useNeighborBrightness() { return m_useNeighborBrightness; }
	*/

	static Block* stone;
	static Block* grass;
	static Block* dirt;
	static Block* cobblestone;
	static Block* planks;
	static Block* sapling;
	static Block* bedrock;
	static Block* waterMoving;
	static Block* waterStill;
	static Block* lavaMoving;
	static Block* lavaStill;
	static Block* sand;
	static Block* gravel;
	static Block* oreGold;
	static Block* oreIron;
	static Block* oreCoal;
	static Block* wood;
	static Block* leaves;
	static Block* sponge;
	static Block* glass;
	static Block* oreLapis;
	static Block* blockLapis;
	static Block* dispenser;
	static Block* sandStone;
	static Block* music;
	static Block* bed;
	static Block* railPowered;
	static Block* railDetector;
	static BlockPistonBase* pistonStickyBase;
	static Block* web;
	static Block* tallGrass;
	static Block* deadBush;
	static BlockPistonBase* pistonBase;
	static BlockPistonExtension* pistonExtension;
	static Block* cloth;
	static BlockPistonMoving* pistonMoving;
	static Block* plantYellow;
	static Block* plantRed;
	static Block* mushroomBrown;
	static Block* mushroomRed;
	static Block* blockGold;
	static Block* blockIron;
	static Block* stoneDoubleSlab;
	static Block* stoneSingleSlab;
	static Block* brick;
	static Block* tnt;
	static Block* bookShelf;
	static Block* cobblestoneMossy;
	static Block* obsidian;
	static Block* torchWood;
	static Block* fire;
	static Block* mobSpawner;
	static Block* stairsWoodOak;
	static Block* chest;
	static BlockRedstoneWire* redstoneWire;
	static Block* oreDiamond;
	static Block* blockDiamond;
	static Block* workbench;
	static Block* crops;
	static Block* tilledField;
	static Block* furnaceIdle;
	static Block* furnaceBurning;
	static Block* signPost;
	static Block* doorWood;
	static Block* ladder;
	static Block* rail;
	static Block* stairsCobblestone;
	static Block* signWall;
	static Block* lever;
	static Block* pressurePlateStone;
	static Block* doorIron;
	static Block* pressurePlatePlanks;
	static Block* oreRedstone;
	static Block* oreRedstoneGlowing;
	static Block* torchRedstoneIdle;
	static Block* torchRedstoneActive;
	static Block* stoneButton;
	static Block* snow;
	static Block* ice;
	static Block* blockSnow;
	static Block* cactus;
	static Block* blockClay;
	static Block* reed;
	static Block* jukebox;
	static Block* fence;
	static Block* pumpkin;
	static Block* netherrack;
	static Block* slowSand;
	static Block* glowStone;
	static Block* portal;
	static Block* pumpkinLantern;
	static Block* cake;
	static BlockRedstoneRepeater* redstoneRepeaterIdle;
	static BlockRedstoneRepeater* redstoneRepeaterActive;
	static Block* lockedChest;
	static Block* trapdoor;
	static Block* silverfish;
	static Block* stoneBrick;
	static Block* mushroomCapBrown;
	static Block* mushroomCapRed;
	static Block* fenceIron;
	static Block* thinGlass;
	static Block* melon;
	static Block* pumpkinStem;
	static Block* melonStem;
	static Block* vine;
	static Block* fenceGate;
	static Block* stairsBrick;
	static Block* stairsStoneBrick;
	static Block* mycelium;
	static Block* waterlily;
	static Block* netherBrick;
	static Block* netherFence;
	static Block* stairsNetherBrick;
	static Block* netherStalk;
	static Block* enchantmentTable;
	static Block* brewingStand;
	static Block* cauldron;
	static Block* endPortal;
	static Block* endPortalFrame;
	static Block* whiteStone;
	static Block* dragonEgg;
	static Block* redstoneLampIdle;
	static Block* redstoneLampActive;
	static Block* woodDoubleSlab;
	static Block* woodSingleSlab;
	static Block* cocoaPlant;
	static Block* stairsSandStone;
	static Block* oreEmerald;
	static Block* enderChest;
	static BlockTripWireSource* tripWireSource;
	static Block* tripWire;
	static Block* blockEmerald;
	static Block* stairsWoodSpruce;
	static Block* stairsWoodBirch;
	static Block* stairsWoodJungle;
	static Block* commandBlock;
	static Block* beacon;
	static Block* cobblestoneWall;
	static Block* flowerPot;
	static Block* carrot;
	static Block* potato;
	static Block* woodenButton;
	static Block* skull;
	static Block* anvil;
	static Block* chestTrapped;
	static Block* pressurePlateGold;
	static Block* pressurePlateIron;
	static BlockComparator* redstoneComparatorIdle;
	static BlockComparator* redstoneComparatorActive;
	static Block* daylightSensor;
	static Block* blockRedstone;
	static Block* oreNetherQuartz;
	static Block* hopperBlock;
	static Block* blockNetherQuartz;
	static Block* stairsNetherQuartz;
	static Block* railActivator;
	static Block* dropper;
	static Block* clayHardenedStained;
	static Block* wood2;
	static Block* stairsWoodDarkOak;
	static Block* slime;
	static Block* seaLantern;
	static Block* hayBlock;
	static Block* woolCarpet;
	static Block* clayHardened;
	static Block* blockCoal;
	static Block* packedIce;
	static Block* railReceive;
	static Block* redSandstoneDoubleSlab;
	static Block* redSandstoneSingleSlab;
	static Block* roadCarpet;
	static BlockDoor* keyDoor_01;
	static BlockDoor* keyDoor_02;
	static BlockDoor* keyDoor_03;
	static BlockDoor* keyDoor_04;
	static BlockDoor* keyDoor_05;
	static BlockDoor* keyDoor_06;
	static BlockDoor* keyDoor_07;
	static BlockDoor* keyDoor_08;
	static BlockDoor* keyDoor_09;
	static BlockDoor* keyDoor_10;
	static BlockWithEffect* blockWithEffect;
	static BlockWithEffect* blockWithEffect_02;
	static BlockWithEffect* blockWithEffect_03;
	static BlockWithEffect* blockWithEffect_04;
	static Block* pressurePlateWeightWood;
	static Block* chestNew;
	static Block* enderChestNew;
	static Block* chestTrappedNew;
	static BlockNewCrops* newCrops_01;
	static BlockNewCrops* newCrops_02;
	static BlockNewCrops* newCrops_03;
	static BlockNewCrops* newCrops_04;
	static BlockNewCrops* newCrops_05;
	static Block* concrete;
	static Block* concrete_powder;
	static Block* air_wall;
	static Block* stained_glass;

	static Block* banner_00;
	static Block* banner_01;
	static Block* banner_02;
	static Block* banner_03;
	static Block* banner_04;
	static Block* banner_05;
	static Block* banner_06;
	static Block* banner_07;
	static Block* banner_08;
	static Block* banner_09;
	static Block* banner_10;
	static Block* banner_11;
	static Block* banner_12;
	static Block* banner_13;
	static Block* banner_14;
	static Block* banner_15;

	static BlockNewCrops* ranchCrop_100001;
	static BlockNewCrops* ranchCrop_100002;
	static BlockNewCrops* ranchCrop_100003;
	static BlockNewCrops* ranchCrop_100004;
	static BlockNewCrops* ranchCrop_100005;
	static BlockNewCrops* ranchCrop_100006;
	static BlockNewCrops* ranchCrop_100007;
	static BlockNewCrops* ranchCrop_100008;
	static BlockNewCrops* ranchCrop_100009;
	static BlockNewCrops* ranchCrop_100010;
	static BlockNewCrops* ranchCrop_100011;
	static BlockNewCrops* ranchCrop_100012;
	static BlockNewCrops* ranchCrop_100013;
	static BlockNewCrops* ranchCrop_100014;
	static BlockNewCrops* ranchCrop_100015;
	static BlockNewCrops* ranchCrop_100016;
	static BlockNewCrops* ranchCrop_100017;

	static BlockNest* nest_01;
	static BlockNest* nest_02;

	static BlockFruits* fruits_01;
	static BlockFruits* fruits_02;
	static BlockFruits* fruits_03;
	static BlockFruits* fruits_04;
	static BlockFruits* fruits_05;
	static BlockFruits* fruits_06;
	static BlockFruits* fruits_07;
	static BlockFruits* fruits_08;
	static BlockFruits* fruits_09;
	static BlockFruits* fruits_10;
	static BlockFruits* fruits_11;
	static BlockFruits* fruits_12;

	static Block* transparency;

	// List of ly/ff (BlockType) containing the already registered blocks.
	static Block**	sBlocks;
	// An array of 4096 booleans corresponding to the result of the isOpaqueCube() method for each block ID
	static i8*		sOpaqueCubeLookup;
	// How much light is subtracted for going through this block
	static int*		sLightOpacity;
	// Array of booleans that tells if a block can grass
	static i8*		sCanBlockGrass;
	// Amount of light emitted
	static int*		sLightValue;
	//  Flag if block ID should use the brightest neighbor light value as its own
	static i8*		sUseNeighborBrightness;
};

}

#endif
