/********************************************************************
filename: 	WorldGenerator.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-11-12
*********************************************************************/
#ifndef __WORLD_GENERATOR_HEADER__
#define __WORLD_GENERATOR_HEADER__

#include "Util/Random.h"
#include "BM_TypeDef.h"

namespace BLOCKMAN
{

class World;
class WeightedRandomChestContent;

class WorldGenerator : public ObjectAlloc
{
public:
	WorldGenerator();
	virtual ~WorldGenerator();

	WorldGenerator(bool par1);

	virtual bool generate(World* pWorld, Random& rand, int x, int y, int z) = 0;

	/** Rescales the generator settings, only used in WorldGenBigTree */
	virtual  void setScale(double xScale, double yScale, double zScale) {}

	/** Sets the block without metadata in the world, notifying neighbors if enabled. */
	void setBlock(World* pWorld, int x, int y, int z, int blockID);

	/** Sets the block in the world, notifying neighbors if enabled. */
	void setBlockAndMetadata(World* pWorld, int x, int y, int z, int blockID, int metadata);

protected:
	/** Sets wither or not the generator should notify blocks of blocks it changes. When the world is first generated,
	* this is false, when saplings grow, this is true. */
	bool doBlockNotify = false;
};

class WorldGenBigMushroom : public WorldGenerator
{
public:
	WorldGenBigMushroom(int mushroomType);
	WorldGenBigMushroom();
	virtual bool generate(World* pWorld, Random& rand, int x, int y, int z);

protected:
	/** The mushroom type. 0 for brown, 1 for red. */
	int mushroomType = 0;
};

class WorldGenBigTree : public WorldGenerator
{

public:
	WorldGenBigTree(bool par1);
	virtual ~WorldGenBigTree();

	/** Generates a list of leaf nodes for the tree, to be populated by generateLeaves.	*/
	void generateLeafNodeList();

	void genTreeLayer(int x, int y, int z, float size, i8 index, int blockID);

	/** Gets the rough size of a layer of the tree. */
	float layerSize(int par1);

	float leafSize(int height);

	/** Generates the leaves surrounding an individual entry in the leafNodes list. */
	void generateLeafNode(int par1, int par2, int par3);

	/** Places a line of the specified block ID into the world from the first coordinate triplet to the second. */
	void placeBlockLine(int* pBottom, int* pTop, int blockID);

	/** Generates the leaf portion of the tree as specified by the leafNodes list. */
	void generateLeaves();

	/** Indicates whether or not a leaf node requires additional wood to be added to preserve integrity. */
	bool leafNodeNeedsBase(int height) { return (double)height >= (double)heightLimit * 0.2; }

	/** Places the trunk for the big tree that is being generated. Able to generate double-sized trunks by changing a field that is always 1 to 2. */
	void generateTrunk();

	/** Generates additional wood blocks to fill out the bases of different leaf nodes that would otherwise degrade. */
	void generateLeafNodeBases();

	void generateFinish();

	/** Checks a line of blocks in the world from the first coordinate to triplet to the second, returning the distance
	* (in blocks) before a non-air, non-leaf block is encountered and/or the end is encountered. */
	int checkBlockLine(int* pBottom, int* pTop);

	/** Returns a boolean indicating whether or not the current location for the tree, spanning basePos to to the height limit, is valid. */
	bool validTreeLocation();

	/** Rescales the generator settings, only used in WorldGenBigTree */
	virtual void setScale(double par1, double par3, double par5);
	virtual bool generate(World* pWorld, Random& rand, int x, int y, int z);

protected:
	/** Contains three sets of two values that provide complimentary indices for a given 'major' index - 1 and 2 for 0, 0
	* and 2 for 1, and 0 and 1 for 2. */
	static i8 otherCoordPairs[6];// = new byte[]{ (byte)2, (byte)0, (byte)0, (byte)1, (byte)2, (byte)1 };

	/** random seed for GenBigTree */
	Random m_Rand;

	/** Reference to the World object. */
	World* worldObj = nullptr;
	int basePos[3] = {0};
	int heightLimit = 0;
	int height = 0;
	double heightAttenuation = 0.0;
	double branchDensity = 0.0;
	double branchSlope = 0.0;
	double scaleWidth = 0.0;
	double leafDensity = 0.0;

	/** Currently always 1, can be set to 2 in the class constructor to generate a double-sized tree trunk for big trees. */
	int trunkSize = 0;

	/** Sets the limit of the random value used to initialize the height limit. */
	int heightLimitLimit = 0;

	/** Sets the distance limit for how far away the generator will populate leaves from the base leaf node. */
	int leafDistanceLimit = 0;

	/** Contains a list of a points at which to generate groups of leaves. */
	int* leafNodes = nullptr;
	int  leafNodePitch = 0;
};

class WorldGenCactus : public WorldGenerator
{
public:
	virtual bool generate(World* pWorld, Random& rand, int x, int y, int z);
};

class WorldGenClay : public WorldGenerator
{
public:
	WorldGenClay(int blockNum);
	virtual bool generate(World* pWorld, Random& rand, int x, int y, int z);
	
protected:
	/** The block ID for clay. */
	int clayBlockId = 0;

	/** The number of blocks to generate. */
	int numberOfBlocks = 0;

};

class WorldGenDeadBush : public WorldGenerator
{
public:
	WorldGenDeadBush(int deadBushID);
	virtual bool generate(World* pWorld, Random& rand, int x, int y, int z);

protected:
	/** stores the ID for WorldGenDeadBush */
	int deadBushID = 0;
};

class WorldGenDesertWells : public WorldGenerator
{
public:
	virtual bool generate(World* pWorld, Random& rand, int x, int y, int z);
};

class WorldGenDungeons : public WorldGenerator
{
public :
	virtual bool generate(World* pWorld, Random& rand, int x, int y, int z);

	static WeightedRandomChestContent** field_111189_a; // = new WeightedRandomChestContent[]{ new WeightedRandomChestContent(Item.saddle.itemID, 0, 1, 1, 10), new WeightedRandomChestContent(Item.ingotIron.itemID, 0, 1, 4, 10), new WeightedRandomChestContent(Item.bread.itemID, 0, 1, 1, 10), new WeightedRandomChestContent(Item.wheat.itemID, 0, 1, 4, 10), new WeightedRandomChestContent(Item.gunpowder.itemID, 0, 1, 4, 10), new WeightedRandomChestContent(Item.silk.itemID, 0, 1, 4, 10), new WeightedRandomChestContent(Item.bucketEmpty.itemID, 0, 1, 1, 10), new WeightedRandomChestContent(Item.appleGold.itemID, 0, 1, 1, 1), new WeightedRandomChestContent(Item.redstone.itemID, 0, 1, 4, 10), new WeightedRandomChestContent(Item.record13.itemID, 0, 1, 1, 10), new WeightedRandomChestContent(Item.recordCat.itemID, 0, 1, 1, 10), new WeightedRandomChestContent(Item.field_111212_ci.itemID, 0, 1, 1, 10), new WeightedRandomChestContent(Item.field_111216_cf.itemID, 0, 1, 1, 2), new WeightedRandomChestContent(Item.field_111215_ce.itemID, 0, 1, 1, 5), new WeightedRandomChestContent(Item.field_111213_cg.itemID, 0, 1, 1, 1) };
	static int randomChestCount;

	static void initialize();
	static void uninitialize();
	
	void simulateCreateChest(World* pWorld, Random& rand, int x, int y, int z);

	//add by maxicheng
	void simulateCreateMob(Random& rand);

protected:
	/** Randomly decides which spawner to use in a dungeon*/
	String pickMobSpawner(Random& rand)
	{
		int mobType = rand.nextInt(4);
		return mobType == 0 ? "Skeleton" : (mobType == 1 ? "Zombie" : (mobType == 2 ? "Zombie" : (mobType == 3 ? "Spider" : "")));
	}
};

class WorldGenFire : public WorldGenerator
{
public:
	virtual bool generate(World* pWorld, Random& rand, int x, int y, int z);
};

class WorldGenFlowers : public WorldGenerator
{
public:
	WorldGenFlowers(int plantBlockId);
	virtual bool generate(World* pWorld, Random& rand, int x, int y, int z);

protected:
	/** The ID of the plant block used in this plant generator. */
	int plantBlockId = 0;
};

class WorldGenForest : public WorldGenerator
{
public:
	WorldGenForest(bool par1);
	virtual bool generate(World* pWorld, Random& rand, int x, int y, int z);
};

class WorldGenGlowStone1 : public WorldGenerator
{
public:
	virtual bool generate(World* pWorld, Random& rand, int x, int y, int z);	
};


class WorldGenGlowStone2 : public WorldGenerator
{
public:
	virtual bool generate(World* pWorld, Random& rand, int x, int y, int z);
};

class WorldGenHellLava : public WorldGenerator
{
protected:
	/** Stores the ID for WorldGenHellLava */
	int hellLavaID = 0;
	bool field_94524_b = false;

public:
	WorldGenHellLava(int par1, bool par2);

	virtual bool generate(World* pWorld, Random& rand, int x, int y, int z);
};

class WorldGenHugeTrees : public WorldGenerator
{
public:
	WorldGenHugeTrees(bool par1, int baseHeight, int woodMetadata, int leavesMetadata);
	void setParam(int baseHeight, int woodMetadata, int leavesMetadata);
	virtual bool generate(World* pWorld, Random& rand, int x, int y, int z);
	void growLeaves(World* pWorld, int x, int z, int y, int par5, Random& rand);

protected:
	/** The base height of the tree */
	int baseHeight = 0;
	/** Sets the metadata for the wood blocks used */
	int woodMetadata = 0;
	/** Sets the metadata for the leaves used in huge trees */
	int leavesMetadata = 0;

};

class WorldGenLakes : public WorldGenerator
{
public:
	WorldGenLakes(int blockIndex);
	virtual bool generate(World* pWorld, Random& rand, int x, int y, int z);

protected:
	int blockIndex = 0;
};

class WorldGenLiquids : public WorldGenerator
{
public:
	WorldGenLiquids(int liquidBlockId);
	virtual bool generate(World* pWorld, Random& rand, int x, int y, int z);

protected:
	/** The ID of the liquid block used in this liquid generator. */
	int liquidBlockId = 0;
};

class WorldGenMinable : public WorldGenerator
{
protected:
	/** The block ID of the ore to be placed using this generator. */
	int minableBlockId = 0;

	/** The number of blocks to generate. */
	int numberOfBlocks = 0;
	int replaceId = 0;

public:
	WorldGenMinable(int minableBlockId, int numberOfBlocks);
	WorldGenMinable(int minableBlockId, int numberOfBlocks, int replaceId);

	virtual bool generate(World* pWorld, Random& rand, int x, int y, int z);
};

class WorldGenPumpkin : public WorldGenerator
{
public:
	virtual bool generate(World* pWorld, Random& rand, int x, int y, int z);
};

class WorldGenReed : public WorldGenerator
{
public:
	virtual bool generate(World* pWorld, Random& rand, int x, int y, int z);
};

class WorldGenSand : public WorldGenerator
{
protected:
	/** Stores ID for WorldGenSand */
	int sandID = 0;

	/** The maximum radius used when generating a patch of blocks. */
	int radius = 0;

public:
	WorldGenSand(int radius, int sandID);
	virtual bool generate(World* pWorld, Random& rand, int x, int y, int z);
};

class WorldGenShrub : public WorldGenerator
{
public:
	WorldGenShrub(int metaWood, int metaLeaves);
	virtual bool generate(World* pWorld, Random& rand, int x, int y, int z);

protected:
	int metaLeaves = 0;
	int metaWood = 0;
};

class WorldGenSpikes : public WorldGenerator
{
protected:
	/** The Block ID that the generator is allowed to replace while generating the terrain. */
	int replaceID = 0;

public:
	WorldGenSpikes(int replaceID);
	virtual bool generate(World* pWorld, Random& rand, int x, int y, int z);
};

class WorldGenSwamp : public WorldGenerator
{
public:
	virtual bool generate(World* pWorld, Random& rand, int x, int t, int z);
	
protected:
	/** Generates vines at the given position until it hits a block. */
	void generateVines(World* pWorld, int x, int y, int z, int metadata);
};

class WorldGenTaiga1 : public WorldGenerator
{
public:
	virtual bool generate(World* pWorld, Random& rand, int x, int y, int z);
};

class WorldGenTaiga2 : public WorldGenerator
{
public:
	WorldGenTaiga2(bool par1);
	virtual bool generate(World* pWorld, Random& rand, int x, int y, int z);
};

class WorldGenTallGrass : public WorldGenerator
{
protected:
	/** Stores ID for WorldGenTallGrass */
	int tallGrassID = 0;
	int tallGrassMetadata = 0;

public:
	WorldGenTallGrass(int tallGrassID, int tallGrassMetaData);
	virtual bool generate(World* pWorld, Random& rand, int x, int y, int z);
};


class WorldGenTrees : public WorldGenerator
{
protected:
	/** The minimum height of a generated tree. */
	int minTreeHeight = 0;

	/** True if this tree should grow Vines. */
	bool vinesGrow = false;

	/** The metadata value of the wood to use in tree generation. */
	int metaWood = 0;

	/** The metadata value of the leaves to use in tree generation. */
	int metaLeaves = 0;

public:
	WorldGenTrees(bool par1);
	WorldGenTrees(bool par1, int minTreeHeight, int metaWood, int metaLeaves, bool vinesGrow);
	void setParam(int minTreeHeight, int metaWood, int metaLeaves, bool vinesGrow);
	virtual bool generate(World* pWorld, Random& rand, int x, int y, int z);
	
protected:
	/** Grows vines downward from the given block for a given length. Args: World, x, starty, z, vine-length */
	void growVines(World* pWorld, int x, int y, int z, int metadata);
};

class WorldGenVines : public WorldGenerator
{
public:
	virtual bool generate(World* pWorld, Random& rand, int x, int y, int z);
};

class WorldGenWaterlily : public WorldGenerator
{
public:
	virtual bool generate(World* pWorld, Random& rand, int x, int y, int z);
};

class WorldGeneratorBonusChest : public WorldGenerator
{
public:
	WorldGeneratorBonusChest(WeightedRandomChestContent** par1ArrayOfWeightedRandomChestContent, int par2);
	virtual bool generate(World* pWorld, Random& rand, int x, int y, int z);

protected:
	/** Instance of WeightedRandomChestContent what will randomly generate items into the Bonus Chest. */
	WeightedRandomChestContent** theBonusChestGenerator;

	/** Value of this int will determine how much items gonna generate in Bonus Chest. */
	int itemsToGenerateInBonusChest = 0;
};

}

#endif