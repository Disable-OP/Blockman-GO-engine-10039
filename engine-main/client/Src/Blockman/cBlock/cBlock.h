/********************************************************************
filename: 	Block.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-6-21
*********************************************************************/
#ifndef __CBLOCK_CLIENT_HEADER__
#define __CBLOCK_CLIENT_HEADER__

#include "bm_clientDef.h"
#include "Block/Blocks.h"
#include "Render/Colorizer.h"
#include "Audio/SoundDef.h"

namespace BLOCKMAN
{

class IBlockAccess;
class World;
class AtlasSprite;
class TextureAtlasRegister;

struct StepSound
{
	SoundType   placeSound;
	SoundType	stepSound;
	SoundType   breakSound;

	StepSound(SoundType ps, SoundType ss, SoundType bs)
		: placeSound(ps)
		, stepSound(ss)
		, breakSound(bs)
	{}
};

class cBlock : public ObjectAlloc
{
public:
	cBlock(const String& atlas);
	virtual ~cBlock();

public:
	virtual int getBlockColor()  { return 0xFFFFFFFF; }

	// Returns the color this block should be rendered. Used by leaves.
	virtual int getRenderColor(int meta)  { return 0xFFFFFFFF; }

	/** Returns a integer with hex for 0xrrggbb with this color multiplied against the blocks color. */
	virtual ui32 colorMultiplier(IBlockAccess* piBlockAccess, const BlockPos& pos)  { return 0xFFFFFFFF; }

	// How bright to render this block based on the light its receiving. Args: iBlockAccess, x, y, z
	virtual float getBlockBrightness(IBlockAccess* pBlockAccess, const BlockPos& pos);

	// Goes straight to getLightBrightnessForSkyBlocks for Blocks, does some fancy computing for Fluids
	virtual int getMixedBrightnessForBlock(IBlockAccess* piBlockAccess, const BlockPos& pos);

	// Returns the default ambient occlusion value based on block opacity
	float getAmbientOcclusionLightValue(IBlockAccess* pBlockAccess, const BlockPos& pos);

	// A randomly called display update to be able to add particles or other items for display
	virtual void randomDisplayTick(World* pWorld, const BlockPos& pos, Random& rand) {}

	// Returns which pass should this block be rendered on. 0 for solids and 1 for alpha
	virtual int getRenderBlockPass() { return TRP_SOLID; }

	// The type of render function that is called for this block
	virtual int getRenderType() const { return RENDER_TYPE_TRAPDOOR; }

	virtual bool hasUVAnimation() { return false; }

	AtlasSprite* getSprite() { return m_sprite; }

	// From the specified side and block meta retrieves the blocks texture. Args: side, meta
	virtual AtlasSprite* getIcon(int side, int meta);

	// Returns the block texture based on the side being looked at.  Args: side
	virtual AtlasSprite* getSpriteFromSide(int side);

	// Retrieves the block texture to use based on the display side. Args: iBlockAccess, x, y, z, side
	virtual AtlasSprite* getBlockSprite(IBlockAccess* piBlockAccess, const BlockPos& pos, int side);

	virtual void registerIcons(TextureAtlasRegister* pAtlas);

	void setSpriteName(const String& name) { m_spriteName = name; }

	virtual const String& getSpriteName() const { return m_spriteName; }

	const String& getAtlasName() const { return m_atlasName; }

	virtual String getItemSpriteName() const { return StringUtil::BLANK; }

	void setStepSound(StepSound* sound) { m_stepSound = sound; }
	const StepSound& getStepSound() const { LordAssert(m_stepSound); return *m_stepSound; }

	virtual int getMaxMeta() { return 15; }

public:
	static StepSound soundPowderFootstep;	// = new StepSound("stone", 1.0F, 1.0F);
	static StepSound soundWoodFootstep;		// = new StepSound("wood", 1.0F, 1.0F);
	static StepSound soundGravelFootstep;	// = new StepSound("gravel", 1.0F, 1.0F);
	static StepSound soundGrassFootstep;	// = new StepSound("grass", 1.0F, 1.0F);
	static StepSound soundStoneFootstep;	// = new StepSound("stone", 1.0F, 1.0F);
	static StepSound soundMetalFootstep;	// = new StepSound("stone", 1.0F, 1.5F);
	static StepSound soundGlassFootstep;	// = new StepSoundStone("stone", 1.0F, 1.0F);
	static StepSound soundClothFootstep;	// = new StepSound("cloth", 1.0F, 1.0F);
	static StepSound soundSandFootstep;		// = new StepSound("sand", 1.0F, 1.0F);
	static StepSound soundSnowFootstep;		// = new StepSound("snow", 1.0F, 1.0F);
	static StepSound soundLadderFootstep;	// = new StepSoundSand("ladder", 1.0F, 1.0F);
	static StepSound soundAnvilFootstep;	// = new StepSoundAnvil("anvil", 0.3F, 1.0F);

protected:
	ui32 colorFromBiome(COLORIZER_TYPE colorType, IBlockAccess* piBlockAccess, const BlockPos& pos);

protected:
	String			m_atlasName = "Blocks.json";
	String			m_spriteName;
	AtlasSprite*	m_sprite = nullptr;
	StepSound*		m_stepSound = nullptr;
};

class BlockClient : public Block, public cBlock
{
public:
	BlockClient(int id, const BM_Material& mat, const String& name, const String& atlas);
};

class cBlockFlower : public cBlock
{
public:
	cBlockFlower(const String& name);

	/** implement override function from BlockClient. */
	virtual int getRenderBlockPass() { return TRP_SOLID_NOMIP; }
	virtual int getRenderType() const { return RENDER_TYPE_FLOWER; }
};

class cFlower : public BlockFlower, public cBlockFlower
{
public:
	cFlower(int id, const BM_Material& mat, const String& name, const String& atlas);
};

class cBlockTallGrass : public  BlockTallGrass, public cBlockFlower
{
public:
	cBlockTallGrass(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override function from BlockClient. */
	virtual int getBlockColor();
	virtual int getRenderColor(int pass) { return  pass == 0 ? 0xFFFFFF : 0x48B518; }
	virtual ui32 colorMultiplier(IBlockAccess* pBlockAccess, const BlockPos& pos);
	virtual AtlasSprite* getIcon(int side, int meta);
	virtual void registerIcons(TextureAtlasRegister* pAtlas);

protected:
	static const int grassTypeCount = 3;
	static String grassTypes[grassTypeCount];
	AtlasSprite* sprites[grassTypeCount] = { nullptr };
};


class cBlockDeadBush : public BlockDeadBush, public cBlockFlower
{
public:
	cBlockDeadBush(int id, const BM_Material& mat, const String& name, const String& atlas);
};


class cBlockFluid : public cBlock
{
public:
	cBlockFluid(const String& atlas);

	/** implement override function from BlockClient. */
	virtual int getBlockColor() { return 0xFFFFFF; }
	virtual ui32 colorMultiplier(IBlockAccess* pBlockAccess, const BlockPos& pos);
	virtual float getBlockBrightness(IBlockAccess* pBlockAccess, const BlockPos& pos);
	virtual int getMixedBrightnessForBlock(IBlockAccess* pBlockAccess, const BlockPos& pos);
	virtual void randomDisplayTick(World* pWorld, const BlockPos& pos, Random& rand);
	virtual int getRenderBlockPass();
	virtual int getRenderType() const { return RENDER_TYPE_FLUID; }
	virtual bool hasUVAnimation() { return true; }
	virtual void registerIcons(TextureAtlasRegister* pAtlas);
	virtual AtlasSprite* getIcon(int side, int meta);

	virtual bool isWater() = 0;

protected:
	AtlasSprite* sprites[2] = { nullptr };
};

class cBlockStationary : public BlockStationary, public cBlockFluid
{
public:
	cBlockStationary(int id, const BM_Material& mat, const String& name, const String& atlas);
	
	/** implement override function from BlockClient. */
	virtual bool isWater();

	/** implement override function from Block */
	virtual bool shouldSideBeRendered(IBlockAccess* pBlockAccess, const BlockPos& pos, int side);
};

class cBlockFlowing : public BlockFlowing, public cBlockFluid
{
public:
	cBlockFlowing(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override function from BlockClient. */
	virtual bool isWater();

	/** implement override function from Block */
	virtual bool shouldSideBeRendered(IBlockAccess* pBlockAccess, const BlockPos& pos, int side);
};

class cBlockMushroom : public BlockMushroom, public cBlockFlower
{
public:
	cBlockMushroom(int id, const BM_Material& mat, const String& name, const String& atlas);
};


class cBlockCactus : public BlockCactus, public cBlock
{
public:
	cBlockCactus(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override function from Block */
	virtual Box getSelectedBoundingBox(World* pWorld, const BlockPos& pos);

	/** implement override function from BlockClient */
	virtual int getRenderType() const { return RENDER_TYPE_CACTUS; }
	virtual AtlasSprite* getIcon(int side, int meta);
	virtual void registerIcons(TextureAtlasRegister* pAtlas);

protected:
	AtlasSprite* cactusTopIcon = nullptr;
	AtlasSprite* cactusBottomIcon = nullptr;
};

class cBlockRotatedPillar : public cBlock
{
public:
	cBlockRotatedPillar(const String& atlas);

	/** implement override function from BlockClient */
	virtual int getRenderType() const { return RENDER_TYPE_HAY; }
	virtual AtlasSprite* getIcon(int side, int meta);

protected:
	virtual AtlasSprite* getSpriteSide(int meta) = 0;
	virtual AtlasSprite* getSpriteTop(int meta) { return spriteTop; }

protected:
	AtlasSprite* spriteTop = nullptr;
};

class cBlockLogBase : public cBlockRotatedPillar
{
public:
	cBlockLogBase(const String& atlas);

	/** implement override function from BlockClient */
	virtual void registerIcons(TextureAtlasRegister* pAtlas);

protected:
	/** implement override function from BlockRotatedPillarClient */
	virtual AtlasSprite* getSpriteSide(int meta) { return wood_side[meta]; }
	virtual AtlasSprite* getSpriteTop(int meta) { return wood_top[meta]; }

private:
	virtual StringArray getWoodTypes() = 0;

protected:
	AtlasSprite* wood_side[4] = { nullptr };
	AtlasSprite* wood_top[4] = { nullptr };
};

class cBlockLog1 : public BlockLog1, public cBlockLogBase
{
public:
	cBlockLog1(int id, const BM_Material& mat, const String& name, const String& atlas)
		: BlockLog1(id, mat, name)
		, cBlockLogBase(atlas) {}

private:
	virtual StringArray getWoodTypes() override { return woodType; };
};

class cBlockLog2 : public BlockLog2, public cBlockLogBase
{
public:
	cBlockLog2(int id, const BM_Material& mat, const String& name, const String& atlas)
		: BlockLog2(id, mat, name)
		, cBlockLogBase(atlas) {}

private:
	virtual StringArray getWoodTypes() override { return woodType; };
};

class cBlockHay : public BlockHay, public cBlockRotatedPillar
{
public:
	cBlockHay(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override function from BlockClient */
	virtual void registerIcons(TextureAtlasRegister* atlas);

protected:
	/** implement override function from BlockRotatedPillarClient */
	virtual AtlasSprite* getSpriteSide(int side) { return m_sprite; }
};

class cBlockLeaves : public BlockLeaves, public cBlock
{
public:
	cBlockLeaves(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override function from Block */
	virtual bool shouldSideBeRendered(IBlockAccess* pBlockAccess, const BlockPos& pos, int side);

	/** implement override function from BlockClient */
	virtual int getRenderType() const;
	virtual int getBlockColor();
	virtual int getRenderColor(int meta);
	virtual ui32 colorMultiplier(IBlockAccess* pBlockAccess, const BlockPos& pos);
	virtual void randomDisplayTick(World* pWorld, const BlockPos& pos, Random& rand);
	virtual AtlasSprite* getIcon(int side, int meta);
	virtual void registerIcons(TextureAtlasRegister* pAtlas);

	/** Pass false to draw this block using fancy graphics, or true for fast graphics. */
	void setOpaque(bool opaque);
	
public:
	static const int leavesTypeCount = 6;
	static String leavesTypes[leavesTypeCount];
	static String leavesSpriteNames[2][leavesTypeCount];
	
protected:
	int isOpaque = 0;
	AtlasSprite* sprites[2][leavesTypeCount] = { { nullptr }, { nullptr } };
};

class cBlockSnow : public BlockSnow, public cBlock
{
public:
	cBlockSnow(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override function from Block */
	virtual bool shouldSideBeRendered(IBlockAccess* pBlockAccess, const BlockPos& pos, int side);
};

class cBlockSand : public BlockSand, public cBlock
{
public:
	cBlockSand(int id, const BM_Material& mat, const String& name, const String& atlas);
};


class cBlockVine : public BlockVine, public cBlock
{
public:
	cBlockVine(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override function from BlockClient */
	virtual int getRenderType() const	{ return RENDER_TYPE_VINE; }
	virtual int getBlockColor() { return 0x48B518; }
	virtual int getRenderColor(int meta) { return 0x48B518; }
	virtual ui32 colorMultiplier(IBlockAccess* pBlockAccess, const BlockPos& pos);
};


class cBlockCocoa : public BlockCocoa, public cBlock
{
public:
	cBlockCocoa(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override function from BlockClient */
	virtual int getRenderType() const { return RENDER_TYPE_COCOA; }
	virtual AtlasSprite* getIcon(int side, int meta) { return sprites[2]; }
	virtual void registerIcons(TextureAtlasRegister* pAtlas);

	AtlasSprite* getItonByAge(int side);

protected:
	AtlasSprite* sprites[3] = { nullptr };
};

class cBlockGrass : public BlockGrass, public cBlock
{
public:
	cBlockGrass(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override function from BlockClient */
	virtual int getRenderBlockPass() { return TRP_SOLID; }
	virtual AtlasSprite* getIcon(int side, int meta);
	virtual AtlasSprite* getBlockSprite(IBlockAccess* pBlockAccess, const BlockPos& pos, int side);
	virtual void registerIcons(TextureAtlasRegister* pAtlas);
	virtual int getBlockColor();
	virtual int getRenderColor(int meta) { return getBlockColor(); }
	virtual ui32 colorMultiplier(IBlockAccess* pBlockAccess, const BlockPos& pos);

	static AtlasSprite* getIconSideOverlay();

protected:
	AtlasSprite* iconGrassTop = nullptr;
	AtlasSprite* iconSnowSide = nullptr;
	AtlasSprite* iconGrassSideOverlay = nullptr;
};

class cBlockWood : public BlockWood, public cBlock
{
public:
	cBlockWood(int id, const BM_Material& mat, const String& name, const String& atlas);
	
	/** implement override function from BlockClient */
	virtual AtlasSprite* getIcon(int side, int meta);
	virtual void registerIcons(TextureAtlasRegister* pAtlas);

public:
	static const int woolTypeCount = 6;
	static String woodType[woolTypeCount]; // { "oak", "spruce", "birch", "jungle", "acacia", "dark_oak" };

protected:
	AtlasSprite* sprites[woolTypeCount] = { nullptr };
};


class cBlockLilyPad : public BlockLilyPad, public cBlockFlower
{
public:
	cBlockLilyPad(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override function from BlockClient */
	virtual int getRenderType() const { return RENDER_TYPE_LILYPAD; }
	virtual int getBlockColor() { return 0x208030; }
	virtual int getRenderColor(int pass) { return 0x208030; }
	virtual ui32 colorMultiplier(IBlockAccess* pBlockAccess, const BlockPos& pos) { return 0xFF208030; }
};


class cBlockReed : public BlockReed, public cBlock
{
public:
	cBlockReed(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override function from BlockClient */
	virtual int getRenderType() const { return RENDER_TYPE_FLOWER; }
};


class cBlockSandStone : public BlockSandStone, public cBlock
{
public:
	cBlockSandStone(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override function from BlockClient */
	virtual AtlasSprite* getIcon(int side, int meta);
	virtual void registerIcons(TextureAtlasRegister* pAtlas);

public:
	static const int SAND_TYPE_COUNT = 3;
	static String SAND_TYPE_NAME[SAND_TYPE_COUNT];		// = new String[]{ "normal", "carved", "smooth" };

protected:
	AtlasSprite* m_icon_side[SAND_TYPE_COUNT] = { nullptr };
	AtlasSprite* m_icon_top = nullptr;
	AtlasSprite* m_icon_bottom = nullptr;
};


class cBlockMycelium : public BlockMycelium, public cBlock
{
public:
	cBlockMycelium(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override function from BlockClient */
	virtual AtlasSprite* getIcon(int side, int meta);
	virtual AtlasSprite* getBlockSprite(IBlockAccess* piBlockAccess, const BlockPos& pos, int side);
	virtual void registerIcons(TextureAtlasRegister* atlas);
	virtual void randomDisplayTick(World* pWorld, const BlockPos& pos, Random& rand);

protected:
	AtlasSprite* m_icon_top = nullptr;
	AtlasSprite* m_icon_side = nullptr;
};

class cBlockChest : public BlockChest, public cBlock
{
public:
	cBlockChest(int id, const BM_Material& mat, const String& name, const String& atlas, int chest_type);

	virtual void randomDisplayTick(World* pWorld, const BlockPos& pos, Random& rand);
	virtual int getRenderType() const { return RENDER_TYPE_CHEST; }
	virtual void registerIcons(TextureAtlasRegister* pAtlas);
	virtual bool onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit);
};


class cBlockBed : public BlockBed, public cBlock
{
protected:
	AtlasSprite* bedEndIcons[2] = { nullptr };
	AtlasSprite* bedSideIcons[2] = { nullptr };
	AtlasSprite* bedTopIcons[2] = { nullptr };

public:
	cBlockBed(int id, const String& name, const String& atlas);

	/** implement override functions from BlockClient */
	virtual AtlasSprite* getIcon(int side, int meta);
	virtual void registerIcons(TextureAtlasRegister* atlas);
	virtual int getRenderType() const { return RENDER_TYPE_BED; }
};


class cBlockSilverfish : public BlockSilverfish, public cBlock
{
public:
	cBlockSilverfish(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override functions from BlockClient */
	virtual AtlasSprite* getIcon(int side, int meta);
};

class cBlockFarmland : public BlockFarmland, public cBlock
{
public:
	cBlockFarmland(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override functions from BlockClient */
	virtual AtlasSprite* getIcon(int side, int meta);
	virtual void registerIcons(TextureAtlasRegister* atlas);

protected:
	AtlasSprite* m_spriteWet = nullptr;
	AtlasSprite* m_spriteDry = nullptr;
};

class cBlockDoor : public BlockDoor, public cBlock
{
public:
	cBlockDoor(int id, const BM_Material& mat, const String& name, const String& atlas);
	~cBlockDoor();
	
	/** implement override functions from Block */
	virtual Box getSelectedBoundingBox(World* pWorld, const BlockPos& pos);

	/** implement override functions from BlockClient */
	virtual AtlasSprite* getIcon(int side, int meta);
	virtual void registerIcons(TextureAtlasRegister* atlas);
	virtual AtlasSprite* getBlockSprite(IBlockAccess* pBlockAccess, const BlockPos& pos, int face);
	virtual int getRenderType() const { return RENDER_TYPE_DOOR; }
	virtual bool checkDoor(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer);
	virtual bool onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit);
	
protected:
	AtlasSprite* m_spriteUpper[2] = { nullptr };
	AtlasSprite* m_spriteLower[2] = { nullptr };
};

class cBlockSnowBlock : public BlockSnowBlock, public cBlock
{
public:
	cBlockSnowBlock(int id, const BM_Material& mat, const String& name, const String& atlas);
};

class cBlockCake : public BlockCake, public cBlock
{
public:
	cBlockCake(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override functions from Block */
	virtual Box getSelectedBoundingBox(World* pWorld, const BlockPos& pos);

	/** implement override functions from BlockClient */
	virtual AtlasSprite* getIcon(int side, int meta);
	virtual void registerIcons(TextureAtlasRegister* atlas);

protected:
	AtlasSprite* m_spriteCakeTop = nullptr;
	AtlasSprite* m_spriteCakeBottom = nullptr;
	AtlasSprite* m_spriteCakeInner = nullptr;
};

class cBlockTrapDoor : public BlockTrapDoor, public cBlock
{
public:
	cBlockTrapDoor(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override functions from Block */
	virtual Box getSelectedBoundingBox(World* pWorld, const BlockPos& pos);

	/** implement override functions from BlockClient */
	virtual int getRenderType() const { return RENDER_TYPE_TRAPDOOR; }
};

class cBlockStoneBrick : public BlockStoneBrick, public cBlock
{
public:
	cBlockStoneBrick(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override functions from BlockClient */
	virtual AtlasSprite* getIcon(int side, int meta);
	virtual void registerIcons(TextureAtlasRegister* atlas);

public:
	static const int stoneBrickTypeCount = 4;
	static String stoneTypes[stoneBrickTypeCount];
	static String stonePostfix[stoneBrickTypeCount];

protected:
	AtlasSprite* m_sprites[4] = { nullptr };
};

class cBlockMushroomCap : public BlockMushroomCap, public cBlock
{
public:
	cBlockMushroomCap(int id, const BM_Material& mat, const String& name, const String& atlas, int mushroomType);

	/** implement override functions from BlockClient */
	virtual AtlasSprite* getIcon(int side, int meta);
	virtual void registerIcons(TextureAtlasRegister* atlas);

public:
	static const int mushroomCapTypeCount = 2;
	static String mushroomCapTypes[mushroomCapTypeCount];

protected:
	AtlasSprite* m_sprites[mushroomCapTypeCount] = { nullptr };
	AtlasSprite* m_spriteInside = nullptr;
	AtlasSprite* m_spriteSkinStem = nullptr;
};

class cBlockMelon : public BlockMelon, public cBlock
{
public:
	cBlockMelon(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override functions from BlockClient */
	virtual AtlasSprite* getIcon(int side, int meta);
	virtual void registerIcons(TextureAtlasRegister* atlas);

protected:
	AtlasSprite* m_spriteTop = nullptr;
};


class cBlockOre : public BlockOre, public cBlock
{
public:
	cBlockOre(int id, const BM_Material& mat, const String& name, const String& atlas);
};

class cBlockOreStorage : public BlockOreStorage, public cBlock
{
public:
	cBlockOreStorage(int id, const BM_Material& mat, const String& name, const String& atlas);
};


class cBlockPumpkin : public BlockPumpkin, public cBlock
{
public:
	cBlockPumpkin(int id, const BM_Material& mat, const String& name, const String& atlas, bool type);

	/** implement override functions from BlockClient */
	virtual AtlasSprite* getIcon(int side, int meta);
	virtual void registerIcons(TextureAtlasRegister* atlas);

protected:
	AtlasSprite* m_spriteTop = nullptr;
	AtlasSprite* m_spriteFace = nullptr;
};


class cBlockCommandBlock : public BlockCommandBlock, public cBlock
{
public:
	cBlockCommandBlock(int id, const BM_Material& mat, const String& name, const String& atlas);
};


class cBlockDaylightDetector : public BlockDaylightDetector, public cBlock
{
public:
	cBlockDaylightDetector(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override functions from BlockClient */
	virtual AtlasSprite* getIcon(int side, int meta);
	virtual void registerIcons(TextureAtlasRegister* atlas);

private:
	AtlasSprite* m_sprites[2] = { nullptr };
};

class cBlockDispenser : public BlockDispenser, public cBlock
{
public:
	cBlockDispenser(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override functions from BlockClient */
	virtual AtlasSprite* getIcon(int side, int meta);
	virtual void registerIcons(TextureAtlasRegister* atlas);

protected:
	AtlasSprite* m_spriteFurnaceTop = nullptr;
	AtlasSprite* m_spriteFurnaceFrontHorizontal = nullptr;
	AtlasSprite* m_spriteFurnaceFrontVertical = nullptr;
};

class cBlockEnchantmentTable : public BlockEnchantmentTable, public cBlock
{
public:
	cBlockEnchantmentTable(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override functions from BlockClient */
	virtual AtlasSprite* getIcon(int side, int meta);
	virtual void registerIcons(TextureAtlasRegister* atlas);
	virtual void randomDisplayTick(World* pWorld, const BlockPos& pos, Random& rand);

protected:
	AtlasSprite* m_spriteTop = nullptr;
	AtlasSprite* m_spriteBottom = nullptr;
};

class cBlockFurnace : public BlockFurnace, public cBlock
{
public:
	cBlockFurnace(int id, const BM_Material& mat, const String& name, const String& atlas, bool isActive);

	/** implement override functions from BlockClient */
	virtual AtlasSprite* getIcon(int side, int meta);
	virtual void registerIcons(TextureAtlasRegister* atlas);

protected:
	AtlasSprite* m_spriteFurnaceTop = nullptr;
	AtlasSprite* m_spriteFurnaceFront = nullptr;
};

class cBlockJukeBox : public BlockJukeBox, public cBlock
{
public:
	cBlockJukeBox(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override functions from BlockClient */
	virtual AtlasSprite* getIcon(int side, int meta);
	virtual void registerIcons(TextureAtlasRegister* atlas);
	
protected:
	AtlasSprite* m_spriteTop = nullptr;
};

class cBlockMobSpawner : public BlockMobSpawner, public cBlock
{
public:
	cBlockMobSpawner(int id, const BM_Material& mat, const String& name, const String& atlas);
};

class cBlockNote : public BlockNote, public cBlock
{
public:
	cBlockNote(int id, const BM_Material& mat, const String& name, const String& atlas);
};

class cBlockStairs : public BlockStairs, public cBlock
{
public:
	cBlockStairs(int id, const BM_Material& mat, const String& name, const String& atlas, Block* pBlock, int meta);

	/** implement override functions from Block */
	virtual Box getSelectedBoundingBox(World* pWorld, const BlockPos& pos);

	/** implement override functions from BlockClient */
	virtual int getMixedBrightnessForBlock(IBlockAccess* pBlockAccess, const BlockPos& pos);
	virtual float getBlockBrightness(IBlockAccess* pBlockAccess, const BlockPos& pos);
	virtual AtlasSprite* getIcon(int side, int meta);
	virtual int getRenderType() const { return RENDER_TYPE_STAIRS; }
	virtual void randomDisplayTick(World* pWorld, const BlockPos& pos, Random& rand);
	virtual int getRenderBlockPass();

protected:
	cBlock* m_pBlockClient = nullptr;
};

class cBlockStep : public BlockStep, public cBlock
{
public:
	cBlockStep(int id, const BM_Material& mat, const String& name, const String& atlas, bool isDoubleSlab);

	/** implement override functions from Block */
	virtual bool shouldSideBeRendered(IBlockAccess* pBlockAccess, const BlockPos& pos, int side);

	/** implement override functions from BlockClient */
	virtual AtlasSprite* getIcon(int side, int meta);
	virtual void registerIcons(TextureAtlasRegister* atlas);

public:
	static const int stepTypeCount = 8;
	static String stepTypes[stepTypeCount];

protected:
	AtlasSprite* m_spriteSide = nullptr;
};


class cBlockWoodSlab : public BlockWoodSlab, public cBlock
{
public:
	cBlockWoodSlab(int id, const BM_Material& mat, const String& name, const String& atlas, bool isDoubleSlab);

	/** implement override functions from Block */
	virtual bool shouldSideBeRendered(IBlockAccess* pBlockAccess, const BlockPos& pos, int side);

	/** implement override functions from BlockClient */
	AtlasSprite* getIcon(int side, int meta);

public:
	static const int slabTypeCount = 4;
	static String slabTypes[slabTypeCount];
};

class cBlockRedStoneSlab : public BlockRedStoneSlab, public cBlock
{
public:
	cBlockRedStoneSlab(int id, const BM_Material& mat, const String& name, const String& atlas, bool isDoubleSlab);

	/** implement override functions from Block */
	virtual bool shouldSideBeRendered(IBlockAccess* pBlockAccess, const BlockPos& pos, int side);

	/** implement override functions from BlockClient */
	virtual AtlasSprite* getIcon(int side, int meta);
	virtual void registerIcons(TextureAtlasRegister* atlas);


protected:
	AtlasSprite* m_spriteSide = nullptr;
};

class cBlockCrops : public BlockCrops, public cBlockFlower
{
public:
	cBlockCrops(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override functions from BlockClient */
	virtual AtlasSprite* getIcon(int side, int meta);
	virtual void registerIcons(TextureAtlasRegister* atlas);
	virtual int getRenderType() const { return RENDER_TYPE_CROPS; }

protected:
	AtlasSprite* m_sprites[8] = { nullptr };
};


class cBlockCarrot : public cBlockCrops
{
public:
	cBlockCarrot(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override functions from BlockCropsClient */
	virtual AtlasSprite* getIcon(int side, int meta);
	virtual void registerIcons(TextureAtlasRegister* atlas);

protected:
	AtlasSprite* m_sprites[4] = { nullptr };
};

class cBlockPotato : public cBlockCrops
{
public:
	cBlockPotato(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override functions from BlockCropsClient */
	virtual AtlasSprite* getIcon(int side, int meta);
	virtual void registerIcons(TextureAtlasRegister* atlas);

protected:
	AtlasSprite* m_sprites[4] = { nullptr };
};

class cBlockStem : public BlockStem, public cBlockFlower
{
public:
	cBlockStem(int id, const BM_Material& mat, const String& name, const String& atlas, Block* fruit);

	/** implement override functions from BlockCropsClient */
	virtual AtlasSprite* getIcon(int side, int meta);
	virtual void registerIcons(TextureAtlasRegister* atlas);
	virtual int getRenderType() const { return RENDER_TYPE_STEM; }
	virtual ui32 colorMultiplier(IBlockAccess* pBlockAccess, const BlockPos& pos);
	virtual int getRenderColor(int meta);

protected:
	AtlasSprite* m_spriteConnected = nullptr;
};

class cBlockButtonWood : public BlockButtonWood, public cBlock
{
public:
	cBlockButtonWood(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override functions from BlockClient */
	virtual AtlasSprite* getIcon(int side, int meta);
};

class cBlockButtonStone : public BlockButtonStone, public cBlock
{
public:
	cBlockButtonStone(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override functions from BlockClient */
	virtual AtlasSprite* getIcon(int side, int meta);
};


class cBlockLadder : public BlockLadder, public cBlock
{
public:
	cBlockLadder(int id, const BM_Material& mat, const String& name, const String& atlas);
		
	/** implement override functions from Block */
	virtual Box getSelectedBoundingBox(World* pWorld, const BlockPos& pos);

	/** implement override functions from BlockClient */
	virtual int getRenderType() const { return RENDER_TYPE_LADDER; }

	virtual int getRenderBlockPass() override { return TRP_SOLID_NOMIP; }
};


class cBlockFence : public BlockFence, public cBlock
{
public:
	cBlockFence(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override functions from Block */
	virtual bool shouldSideBeRendered(IBlockAccess* piBlockAccess, const BlockPos& pos, int side) { return true; }

	/** implement override functions from BlockClient */
	virtual int getRenderType() const { return RENDER_TYPE_FENCE; }
};

class cBlockPane : public BlockPane, public cBlock
{
public:
	cBlockPane(int id, const BM_Material& mat, const String& name, const String& atlas, const String& atlasTop, bool canDropItSelf);

	/** implement override functions from Block */
	virtual bool shouldSideBeRendered(IBlockAccess *pBlockAccess, const BlockPos& pos, int side);

	/** implement override functions from BlockClient */
	virtual void registerIcons(TextureAtlasRegister* atlas);
	virtual AtlasSprite* getIcon(int side, int meta) { return side != 0 ? m_spriteTop : m_sprite; }
	virtual int getRenderType() const { return RENDER_TYPE_PANE; }

protected:
	String m_spriteTopName;
	AtlasSprite* m_spriteTop = nullptr;
};

class cBlockFenceGate : public BlockFenceGate, public cBlock
{
public:
	cBlockFenceGate(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override functions from Block */
	virtual bool shouldSideBeRendered(IBlockAccess* pBlockAccess, const BlockPos& pos, int side) { return true; }

	/** implement override functions from BlockClient */
	virtual void registerIcons(TextureAtlasRegister* atlas) {}
	virtual AtlasSprite* getIcon(int side, int meta);
	virtual int getRenderType() const { return RENDER_TYPE_FENCE_GATE; }
};

class cBlockWall : public BlockWall, public cBlock
{
public:
	cBlockWall(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override functions from Block */
	virtual bool shouldSideBeRendered(IBlockAccess* pBlockAccess, const BlockPos& pos, int side);

	/** implement override functions from BlockClient */
	virtual void registerIcons(TextureAtlasRegister* atlas) {}
	virtual AtlasSprite* getIcon(int side, int meta);
	virtual int getRenderType() const { return RENDER_TYPE_WALL; }
};

class cBlockGlass : public BlockGlass, public cBlock
{
public:
	cBlockGlass(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override functions from Block */
	virtual bool shouldSideBeRendered(IBlockAccess* pBlockAccess, const BlockPos& pos, int side);
};

class cBlockAirWall : public BlockAirWall, public cBlock
{
public:
	cBlockAirWall(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override functions from Block */
	virtual int getRenderType() const { return RENDER_TYPE_AIR; }
};

class cBlockIce : public BlockIce, public cBlock
{
public:
	cBlockIce(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override functions from Block */
	virtual bool shouldSideBeRendered(IBlockAccess* pBlockAccess, const BlockPos& pos, int side);

	/** implement override functions from BlockClient */
	virtual int getRenderBlockPass() { return TRP_TRANSPARENT_3D; }
};

class cBlockPackedIce : public BlockPackedIce, public cBlock
{
public:
	cBlockPackedIce(int id, const BM_Material& mat, const String& name, const String& atlas);
};

class cBlockPortal : public BlockPortal, public cBlock
{
public:
	cBlockPortal(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override functions from Block */
	virtual bool shouldSideBeRendered(IBlockAccess* pBlockAccess, const BlockPos& pos, int side);

	/** implement override functions from BlockClient */
	virtual int getRenderBlockPass() { return TRP_TRANSPARENT_2D; }
	virtual void randomDisplayTick(World* pWorld, const BlockPos& pos, Random& rand);
};


class cBlockQuartz : public BlockQuartz, public cBlock
{
public:
	cBlockQuartz(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override functions from BlockClient */
	virtual void registerIcons(TextureAtlasRegister* atlas);
	virtual AtlasSprite* getIcon(int side, int meta);
	virtual int getRenderType() const { return RENDER_TYPE_QUARTZ; }


public:
	static const int quartzTypeCount = 5;
	static String quartzTextureTypes[quartzTypeCount];

protected:
	AtlasSprite* quartzblockIcons[quartzTypeCount] = { nullptr };
	AtlasSprite* quartzblock_chiseled_top = nullptr;
	AtlasSprite* quartzblock_lines_top = nullptr;
	AtlasSprite* quartzblock_top = nullptr;
	AtlasSprite* quartzblock_bottom = nullptr;
};

class cBlockBeacon : public BlockBeacon, public cBlock
{
public:
	cBlockBeacon(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override functions from BlockClient */
	virtual int getRenderType() const { return RENDER_TYPE_BEACON; }
};


class cBlockAnvil : public BlockAnvil, public cBlock
{
public:
	cBlockAnvil(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override functions from Block */
	virtual bool shouldSideBeRendered(IBlockAccess* pBlockAccess, const BlockPos& pos, int side) { return true; }

	/** implement override functions from BlockClient */
	virtual void registerIcons(TextureAtlasRegister* atlas);
	virtual AtlasSprite* getIcon(int side, int meta);
	virtual int getRenderType() const { return RENDER_TYPE_ANVIL; }
	

public:
	static const int anvilTypeCount = 3;
	static String statuses[anvilTypeCount];
	int m_anvilRenderState = 0;

private:
	static String anvilSpriteNames[anvilTypeCount];
	AtlasSprite* m_sprites[anvilTypeCount] = { nullptr };
};

class cBlockBanner : public BlockBanner, public cBlock
{
public:
	cBlockBanner(int id, const BM_Material& mat, const String& name, const String& atlas, int color, bool m_is_slate = true);

	/** implement override functions from Block */
	virtual bool shouldSideBeRendered(IBlockAccess* pBlockAccess, const BlockPos& pos, int side);
	virtual void registerIcons(TextureAtlasRegister* atlas);
	virtual AtlasSprite* getIcon(int side, int meta);
	virtual int getRenderType() const { return RENDER_TYPE_BANNER; }
	virtual int getRenderColor(int meta);
	bool m_is_slate = true;
private:
	AtlasSprite* m_sprite = nullptr;
	AtlasSprite* m_sprite_slate = nullptr;
	int color = 0xFFFFFFFF;
};

class cBlockPoweredOre : public BlockPoweredOre, public cBlock
{
public:
	cBlockPoweredOre(int id, const BM_Material& mat, const String& name, const String& atlas);
};

class cBlockFlowerPot : public BlockFlowerPot, public cBlock
{
public:
	cBlockFlowerPot(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override functions from BlockClient */
	virtual int getRenderType() const { return RENDER_TYPE_FLOWER_POT; }
};


class cBlockSapling : public BlockSapling, public cBlockFlower
{
public:
	cBlockSapling(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override functions from BlockClient */
	virtual void registerIcons(TextureAtlasRegister* atlas);
	virtual AtlasSprite* getIcon(int side, int meta);

public:
	static const int saplingTypeCount = 6;

protected:
	AtlasSprite* m_sprites[saplingTypeCount] = { nullptr };
};


class cBlockRedstoneLight : public BlockRedstoneLight, public cBlock
{
public:
	cBlockRedstoneLight(int id, const BM_Material& mat, const String& name, const String& atlas, bool powered);
};


class cBlockColored : public BlockColored, public cBlock
{
public:
	cBlockColored(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override functions from BlockClient */
	virtual void registerIcons(TextureAtlasRegister* atlas);
	virtual AtlasSprite* getIcon(int side, int meta);

protected:
	AtlasSprite* m_sprites[colorType] = { nullptr };
};


class cBlockCarpet : public BlockCarpet, public cBlock
{
public:
	cBlockCarpet(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override functions from Block */
	virtual bool shouldSideBeRendered(IBlockAccess* pBlockAccess, const BlockPos& pos, int side);

	/** implement override functions from BlockClient */
	virtual AtlasSprite* getIcon(int side, int meta);
};

class cBlockRoad : public BlockRoad, public cBlock
{
public:
	cBlockRoad(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override functions from Block */
	virtual bool shouldSideBeRendered(IBlockAccess* pBlockAccess, const BlockPos& pos, int side);

	/** implement override functions from BlockClient */
	virtual AtlasSprite* getIcon(int side, int meta);
};

class cBlockWeb : public BlockWeb, public cBlock
{
public:
	cBlockWeb(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override functions from BlockClient */
	virtual int getRenderType() const { return RENDER_TYPE_FLOWER; }
};


class cBlockTNT : public BlockTNT, public cBlock
{
public:
	cBlockTNT(int id, const BM_Material& mat, const String& name, const String& atlas);
	/** implement override functions from BlockClient */
	virtual void registerIcons(TextureAtlasRegister* atlas);
	virtual AtlasSprite* getIcon(int side, int meta);

protected:
	AtlasSprite* m_spriteTop = nullptr;
	AtlasSprite* m_spriteBottom = nullptr;
};

class cBlockBookshelf : public BlockBookshelf, public cBlock
{
public:
	cBlockBookshelf(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override functions from BlockClient */
	virtual AtlasSprite* getIcon(int side, int meta);
};

class cBlockWorkbench : public BlockWorkbench, public cBlock
{
public:
	cBlockWorkbench(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override functions from BlockClient */
	virtual void registerIcons(TextureAtlasRegister* atlas);
	virtual AtlasSprite* getIcon(int side, int meta);
	virtual bool onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit);
protected:
	AtlasSprite* m_spriteTop = nullptr;
	AtlasSprite* m_spriteFront = nullptr;
};

class cBlockClay : public BlockClay, public cBlock
{
public:
	cBlockClay(int id, const BM_Material& mat, const String& name, const String& atlas);
};


class cBlockRedstoneOre : public BlockRedstoneOre, public cBlock
{
public:
	cBlockRedstoneOre(int id, const BM_Material& mat, const String& name, const String& atlas, bool glowing);

	/** implement override functions from BlockClient */
	virtual void randomDisplayTick(World* pWorld, const BlockPos& pos, Random& rand);

protected:
	/** implement override functions from BlockRedstoneOre */
	virtual void sparkle(World* pWorld, const BlockPos& pos);
};


class cBlockRailPowered : public BlockRailPowered, public cBlock
{
public:
	cBlockRailPowered(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override functions from BlockClient */
	virtual int getRenderType() const { return RENDER_TYPE_RAIL_BASE; }
	virtual void registerIcons(TextureAtlasRegister* atlas);
	virtual AtlasSprite* getIcon(int side, int meta);
	
protected:
	AtlasSprite* m_spritePowered = nullptr;
};

class cBlockRail : public BlockRail, public cBlock
{
public:
	cBlockRail(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override functions from BlockClient */
	virtual int getRenderType() const { return RENDER_TYPE_RAIL_BASE; }
	virtual void registerIcons(TextureAtlasRegister* atlas);
	virtual AtlasSprite* getIcon(int side, int meta);
	
protected:
	AtlasSprite* m_spriteTurned = nullptr;
};

class cBlockDetectorRail : public BlockDetectorRail, public cBlock
{
public:
	cBlockDetectorRail(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override functions from BlockClient */
	virtual int getRenderType() const { return RENDER_TYPE_RAIL_BASE; }
	virtual void registerIcons(TextureAtlasRegister* atlas);
	virtual AtlasSprite* getIcon(int side, int meta);
	
protected:
	AtlasSprite* m_spritePowered = nullptr;
};


class cBlockPistonBase : public BlockPistonBase, public cBlock
{
public:
	cBlockPistonBase(int id, const BM_Material& mat, const String& name, const String& atlas, bool isSticky);

	/** implement override functions from BlockClient */
	virtual void registerIcons(TextureAtlasRegister* atlas);
	virtual AtlasSprite* getIcon(int side, int meta);
	virtual int getRenderType() const { return RENDER_TYPE_PISTON_BASE; }

	AtlasSprite* getPistonExtensionTexture() { return m_spriteTop; }
	static AtlasSprite* getIconByName(const String& name);

private:
	AtlasSprite* m_spriteTop = nullptr;
	AtlasSprite* m_spriteBottom = nullptr;
	AtlasSprite* m_spriteInner = nullptr;
};


class cBlockPistonExtension : public BlockPistonExtension, public cBlock
{
public:
	cBlockPistonExtension(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override functions from BlockClient */
	virtual AtlasSprite* getIcon(int side, int meta);
	virtual int getRenderType() const { return RENDER_TYPE_PISTON_EXT; }
	virtual void registerIcons(TextureAtlasRegister* atlas) {}

	void setHeadTexture(AtlasSprite* sprite) { m_spriteHead = sprite; }
	void clearHeadTexture() { m_spriteHead = NULL; }

private:
	AtlasSprite* m_spriteHead = nullptr;
};


class cBlockPistonMoving : public BlockPistonMoving, public cBlock
{
public:
	cBlockPistonMoving(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override functions from BlockClient */
	virtual int getRenderType() const { return -1; }
};

class cBlockTorch : public BlockTorch, public cBlock
{
public:
	cBlockTorch(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override functions from BlockClient */
	virtual int getRenderType() const { return RENDER_TYPE_TORCH; }
	virtual void randomDisplayTick(World* pWorld, const BlockPos& pos, Random& rand);
};


class cBlockRedstoneTorch : public BlockRedstoneTorch, public cBlock
{
public:
	cBlockRedstoneTorch(int id, const BM_Material& mat, const String& name, const String& atlas, bool isActive);

	/** implement override functions from BlockClient */
	virtual int getRenderType() const { return RENDER_TYPE_TORCH; }
	virtual void randomDisplayTick(World* pWorld, const BlockPos& pos, Random& rand);
};


class cBlockFire : public BlockFire, public cBlock
{
public:
	cBlockFire(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override functions from BlockClient */
	virtual void registerIcons(TextureAtlasRegister* atlas);
	virtual AtlasSprite* getIcon(int side, int meta);
	virtual int getRenderType() const { return RENDER_TYPE_FIRE; }
	virtual void randomDisplayTick(World* pWorld, const BlockPos& pos, Random& rand);

private:
	AtlasSprite* m_sprites[2] = { nullptr };
};


class cBlockLever : public BlockLever, public cBlock
{
public:
	cBlockLever(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override functions from BlockClient */
	virtual int getRenderType() const { return RENDER_TYPE_LEVER; }
};

class cBlockNetherStalk : public BlockNetherStalk, public cBlockFlower
{
public:
	cBlockNetherStalk(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override functions from BlockClient */
	virtual void registerIcons(TextureAtlasRegister* atlas);
	virtual AtlasSprite* getIcon(int side, int meta);
	virtual int getRenderType() const { return RENDER_TYPE_CROPS; }

private:
	AtlasSprite* m_sprites[3] = { nullptr };
};


class cBlockBrewingStand : public BlockBrewingStand, public cBlock
{
public:
	cBlockBrewingStand(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override functions from BlockClient */
	virtual void registerIcons(TextureAtlasRegister* atlas);
	virtual AtlasSprite* getIcon(int side, int meta);
	virtual int getRenderType() const { return RENDER_TYPE_BREWING_STAND; }
	virtual void randomDisplayTick(World* pWorld, const BlockPos& pos, Random& rand);

private:
	AtlasSprite* m_spriteBase = nullptr;
};

class cBlockCauldron : public BlockCauldron, public cBlock
{
public:
	cBlockCauldron(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override functions from BlockClient */
	virtual void registerIcons(TextureAtlasRegister* atlas);
	virtual AtlasSprite* getIcon(int side, int meta);
	virtual int getRenderType() const { return RENDER_TYPE_CAULDRON; }
	virtual int getRenderBlockPass() override { return m_pass; }
	
	void setRenderBlockPass(int pass) { m_pass = pass; }

protected:
	int			 m_pass = 0;
	AtlasSprite* m_spriteInner = nullptr;
	AtlasSprite* m_spriteTop = nullptr;
	AtlasSprite* m_spriteBottom = nullptr;
};


class cBlockDragonEgg : public BlockDragonEgg, public cBlock
{
public:
	cBlockDragonEgg(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override functions from Block */
	virtual bool shouldSideBeRendered(IBlockAccess* pIBlockAccess, const BlockPos& pos, int side) { return true; }

	/** implement override functions from BlockClient */
	virtual int getRenderType() const { return RENDER_TYPE_DRAGON_EGG; }
};


class cBlockTripWire : public BlockTripWire, public cBlock
{
public:
	cBlockTripWire(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override functions from BlockClient */
	virtual	int getRenderType() const { return RENDER_TYPE_TRIPWIRE; }
	virtual int getRenderBlockPass() { return TRP_TRANSPARENT_2D; }
};


class cBlockTripWireSource : public BlockTripWireSource, public cBlock
{
public:
	cBlockTripWireSource(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override functions from BlockClient */
	virtual int getRenderType() const { return RENDER_TYPE_TRIPWIRE_SOURCE; }
};


class cBlockHopper : public BlockHopper, public cBlock
{
public:
	cBlockHopper(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override functions from Block */
	virtual bool shouldSideBeRendered(IBlockAccess* pBlockAccess, const BlockPos& pos, int side) { return true; }

	/** implement override functions from BlockClient */
	virtual void registerIcons(TextureAtlasRegister* atlas);
	virtual AtlasSprite* getIcon(int side, int meta);
	virtual int getRenderType() const { return RENDER_TYPE_HOPPER; }
	virtual String getItemSpriteName() const { return "hopper"; }

	AtlasSprite* getHopperIcon(const String& name);

private:
	AtlasSprite* m_spriteOutside = nullptr;
	AtlasSprite* m_spriteTop = nullptr;
	AtlasSprite* m_spriteInside = nullptr;
};


class cBlockDropper : public BlockDropper, public cBlock
{
public:
	cBlockDropper(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override functions from BlockClient */
	virtual AtlasSprite* getIcon(int side, int meta);
	virtual void registerIcons(TextureAtlasRegister* atlas);

protected:
	AtlasSprite* m_spriteFurnaceTop = nullptr;
	AtlasSprite* m_spriteFurnaceFrontHorizontal = nullptr;
	AtlasSprite* m_spriteFurnaceFrontVertical = nullptr;
};


class cBlockBasePressurePlate : public BlockBasePressurePlate, public cBlock
{
public:
	cBlockBasePressurePlate(int id, const BM_Material& mat, const String& name, const String& atlas);
};


class cBlockPressurePlate : public BlockPressurePlate, public cBlock
{
public:
	cBlockPressurePlate(int id, const BM_Material& mat, const String& name, const String& atlas, MOB_TYPE mobType);
};

class cBlockPressurePlateWeighted : public BlockPressurePlateWeighted, public cBlock
{
public:
	cBlockPressurePlateWeighted(int id, const BM_Material& mat, const String& name, const String& atlas, int maxItemsWeighted, MOB_TYPE mobType);
};

class cBlockSoulSand : public BlockSoulSand, public cBlock
{
public:
	cBlockSoulSand(int id, const BM_Material& mat, const String& name, const String& atlas);
};

class cBlockGlowStone : public BlockGlowStone, public cBlock
{
public:
	cBlockGlowStone(int id, const BM_Material& mat, const String& name, const String& atlas);
};

class cBlockEndPortalFrame : public BlockEndPortalFrame, public cBlock
{
public:
	cBlockEndPortalFrame(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override functions from BlockClient */
	virtual void registerIcons(TextureAtlasRegister* atlas);
	virtual AtlasSprite* getIcon(int side, int meta);
	AtlasSprite* getSpriteEye() { return m_spriteEye; }
	virtual int getRenderType() const { return RENDER_TYPE_PORTAL_FRAME; }

private:
	AtlasSprite* m_spriteTop = nullptr;
	AtlasSprite* m_spriteEye = nullptr;
};

class cBlockSign : public BlockSign, public cBlock
{
public:
	cBlockSign(int id, const BM_Material& mat, const String& name, const String& atlas, int signClass, bool freestanding);

	/** implement override functions from Block */
	virtual Box getSelectedBoundingBox(World* pWorld, const BlockPos& pos);

	/** implement override functions from cBlock */
	virtual int getRenderType() const { return -1; }
	virtual AtlasSprite* getIcon(int side, int meta);
	virtual void registerIcons(TextureAtlasRegister* atlas) {}
};

class cBlockComparator : public BlockComparator, public cBlock
{
public:
	cBlockComparator(int id, const BM_Material& mat, const String& name, const String& atlas, bool powered);

	/** implement override functions from cBlock */
	virtual int getRenderType() const { return RENDER_TYPE_COMPARATOR; }
	virtual AtlasSprite* getIcon(int side, int meta);

	/** implement override functions from Block */
	virtual bool shouldSideBeRendered(IBlockAccess* piBlockAccess, const BlockPos& pos, int side) { return side != 0 && side != 1; }
};

class cBlockRedstoneWire : public BlockRedstoneWire, public cBlock
{
public:
	cBlockRedstoneWire(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override functions from cBlock */
	virtual int getRenderType() const override { return RENDER_TYPE_REDSTONE_WIRE; }
	virtual ui32 colorMultiplier(IBlockAccess* pBlockAccess, const BlockPos& pos) override { return 0xFF800000; }
	virtual void randomDisplayTick(World* pWorld, const BlockPos& pos, Random& rand) override;
	virtual void registerIcons(TextureAtlasRegister* atlas) override;

	static AtlasSprite* getSpriteByName(const String& name);

protected:
	AtlasSprite* m_sprite_cross = nullptr;
	AtlasSprite* m_sprite_line = nullptr;
	AtlasSprite* m_sprite_corss_overlay = nullptr;
	AtlasSprite* m_sprite_line_overlay = nullptr;
};

class cBlockRedstoneRepeater : public BlockRedstoneRepeater, public cBlock
{
public:
	static float repeaterTorchOffset[4]; // = { -0.0625f, 0.0625f, 0.1875f, 0.3125f};

public:
	cBlockRedstoneRepeater(int id, const BM_Material& mat, const String& name, const String& atlas, bool powered);

	/** implement override functions from Block */
	virtual bool shouldSideBeRendered(IBlockAccess* piBlockAccess, const BlockPos& pos, int side) { return side != 0 && side != 1; }

	/** implement override functions from cBlock */
	virtual int getRenderType() const { return RENDER_TYPE_REDSTONE_REPEATER; }
	virtual AtlasSprite* getIcon(int side, int meta);
	virtual void randomDisplayTick(World* pWorld, const BlockPos& pos, Random& rand);
};

class cBlockEndPortal : public BlockEndPortal, public cBlock
{
public:
	cBlockEndPortal(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override functions from Block */
	virtual bool shouldSideBeRendered(IBlockAccess* piBlockAccess, const BlockPos& pos, int side);

	/** implement override functions from cBlock */
	virtual int getRenderType() const { return RENDER_TYPE_END_PORTAL; }
	virtual void randomDisplayTick(World* pWorld, const BlockPos& pos, Random& rand);
};

class cBlockEnderChest : public BlockEnderChest, public cBlock
{
public:
	cBlockEnderChest(int id, const BM_Material& mat, const String& name, const String& atlas, int block_ender_chest_type);

	/** implement override functions from cBlock */
	virtual int getRenderType() const { return RENDER_TYPE_CHEST; }
	virtual void randomDisplayTick(World* pWorld, const BlockPos& pos, Random& rand);
	virtual bool onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit);
};

class cBlockSkull : public BlockSkull, public cBlock
{
public:
	cBlockSkull(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override functions from cBlock */
	virtual int getRenderType() const { return -1; }
	virtual void registerIcons(TextureAtlasRegister* atlas) {}
	virtual AtlasSprite* getIcon(int side, int meta);
	virtual String getItemSpriteName() const;
};

class cBlockWithEffect : public BlockWithEffect, public cBlock
{
public:
	cBlockWithEffect(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override functions from cBlock */
	virtual int getRenderType() const { return RENDER_TYPE_EFFECT_CHEST; }
	virtual void randomDisplayTick(World* pWorld, const BlockPos& pos, Random& rand);
	virtual void registerIcons(TextureAtlasRegister* atlas);
	virtual AtlasSprite* getIcon(int side, int meta);
private:
	AtlasSprite*	m_front = nullptr;
	AtlasSprite*	m_side = nullptr;
};

class cBlockDirt : public BlockDirt, public cBlock
{
public:
	cBlockDirt(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override functions from BlockClient */
	virtual AtlasSprite* getIcon(int side, int meta);
	virtual void registerIcons(TextureAtlasRegister* atlas);

protected:
	AtlasSprite* m_spriteTop = nullptr;
	AtlasSprite* m_spriteOther = nullptr;
};

class cBlockNewCrops : public BlockNewCrops, public cBlockFlower
{
public:
	cBlockNewCrops(int id, const BM_Material& mat, const String& name, const String& atlas, int seedsId, int renderType = RENDER_TYPE_FLOWER);

	/** implement override functions from BlockClient */
	virtual AtlasSprite* getIcon(int side, int meta);
	virtual void registerIcons(TextureAtlasRegister* atlas);
	virtual int getRenderType() const { return m_renderType; }

protected:
	AtlasSprite * m_sprites[6] = { nullptr };
	int m_renderType = RENDER_TYPE_FLOWER;
};


class cBlockSlime : public BlockSlime, public cBlock
{
public:
	cBlockSlime(int id, const BM_Material& mat, const String& name, const String& atlas);

};

class cBlockFruits : public BlockFruits, public cBlock
{
public:
	cBlockFruits(int id, const BM_Material& mat, const String& name, const String& atlas, int renderType = RENDER_TYPE_FRUIT);

	/** implement override functions from BlockClient */
	virtual int getRenderBlockPass() { return TRP_SOLID_NOMIP; }
	virtual int getRenderType() const { return m_renderType; }
	virtual AtlasSprite* getIcon(int side, int meta);
	virtual void registerIcons(TextureAtlasRegister* pAtlas);
	int getMaxMeta();

protected:
	int m_renderType = RENDER_TYPE_FRUIT;
	AtlasSprite* m_spriteTop = nullptr;
};

class cBlockNest : public BlockNest, public cBlock
{
public:
	cBlockNest(int id, const BM_Material& mat, const String& name, const String& atlas);
	virtual bool shouldSideBeRendered(IBlockAccess* pBlockAccess, const BlockPos& pos, int side);
	virtual AtlasSprite* getIcon(int side, int meta);
};

class cBlockStainedGlass : public BlockStainedGlass, public cBlock
{
public:
	cBlockStainedGlass(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override functions from Block */
	virtual bool shouldSideBeRendered(IBlockAccess* pBlockAccess, const BlockPos& pos, int side);

	/** implement override functions from BlockClient */
	virtual void registerIcons(TextureAtlasRegister* atlas);
	virtual AtlasSprite* getIcon(int side, int meta);

	virtual ui32 colorMultiplier(IBlockAccess* pBlockAccess, const BlockPos& pos);
	virtual int getRenderBlockPass();

protected:
	AtlasSprite* m_sprites[colorType] = { nullptr };
};

class cBlockTransparency : public BlockTransparency, public cBlock
{
public:
	cBlockTransparency(int id, const BM_Material& mat, const String& name, const String& atlas);

	/** implement override functions from Block */
	virtual int getRenderType() const { return RENDER_TYPE_TRANSPARENCY; }
};

}
#endif
