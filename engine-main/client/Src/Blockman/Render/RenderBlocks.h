/********************************************************************
filename: 	RenderBlocks.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-11-30
*********************************************************************/
#ifndef __RENDER_BLOCKS_HEADER__
#define __RENDER_BLOCKS_HEADER__

#include "BM_TypeDef.h"

namespace BLOCKMAN
{

class IBlockAccess;
class AtlasSprite;
class Block;
class cBlock;
class SectionRenderer;
class BM_Material;
class TileEntityChest;
class TileEntityEnderChest;

inline ui16 GETBLOCKID(ui32 bdata) { return bdata & 0xFF; }
inline ui8 GETBLOCKMETA(ui32 bdata) { return bdata >> 16 & 0xF; }
inline ui8 GETBLOCKSKY(ui32 bdata) { return bdata >> 24 & 0x7; }
inline ui8 GETBLOCKBLIHGT(ui32 bdata) { return bdata >> 28 & 0x7; }
inline ui32 SETBLOCKDATA(ui16 id, ui8 meta, ui8 sky, ui8 blight) { ui32 ret = id; ret |= ui32(meta) << 16; ret |= ui32(sky) << 24; ret |= ui32(blight) << 28;  return ret; }

class RenderBlocks : public ObjectAlloc
{
public:
	RenderBlocks(IBlockAccess* pBlockAccess, SectionRenderer* pSectionRenderer, const Vector3& sectionCenter);
	RenderBlocks(IBlockAccess* pBlockAccess = NULL);
	~RenderBlocks();
	void init();

	/** set sectionRender */
	void setSpecialRenderable(TessRenderable* special) { specialRenderable = special; }
	void setSpecialPosition(const Vector3& pos) { m_sectionPos = pos; m_sectionCenter = pos + Vector3(8.f, 8.f, 8.f); }
	/** Sets overrideBlockTexture */
	void setOverrideBlockTexture(AtlasSprite* sprite) {	overrideBlockTexture = sprite; }
	void clearOverrideBlockTexture() { overrideBlockTexture = NULL;	}
	bool hasOverrideBlockTexture() { return overrideBlockTexture != NULL; }

	/** Sets the bounding box for the block to draw in, e.g. 0.25-0.75 on all axes for a half-size, centered block. */
	void setRenderBounds(float minx, float miny, float minz, float maxx, float maxy, float maxz);

	/** Like setRenderBounds, but automatically pulling the bounds from the given Block. */
	void setRenderBoundsFromBlock(Block* pBlock);

	/** Like setRenderBounds, but locks the values so that RenderBlocks won't change them.  If you use this, you must
	* call unlockBlockBounds after you finish rendering! */
	void overrideBlockBounds(float minx, float miny, float minz, float maxx, float  maxy, float maxz);

	/** Unlocks the visual bounding box so that RenderBlocks can change it again.*/
	void unlockBlockBounds() { lockBlockBounds = false; }

	/** Renders a block using the given texture instead of the block's own default texture */
	void renderBlockUsingTexture(Block* pBlock, const BlockPos& pos, AtlasSprite* sprite);

	/** Render all faces of a block */
	void renderBlockAllFaces(Block* pBlock, const BlockPos& pos);

	/** Renders the block at the given coordinates using the block's rendering type */
	bool renderBlockByRenderType(Block* pBlock, const BlockPos& pos, bool select = false);
	
	/** Renders a standard cube block at the given coordinates*/
	bool renderStandardBlock(Block* pBlock, cBlock* pcBlock, const BlockPos& pos);
	bool renderAirBlock(Block* pBlock, cBlock* pcBlock, const BlockPos& pos, bool select = false);
	bool renderCrossedSquares(Block* pBlock, cBlock* pcBlock, const BlockPos& pos);
	bool renderBlockLeaves(Block* pBlock, cBlock* pcBlock, const BlockPos& pos);
	bool renderBlockFluids(Block* pBlock, cBlock* pcBlock, const BlockPos& pos);
	bool renderBlockCactus(Block* pBlock, cBlock* pcBlock, const BlockPos& pos);
	bool renderBlockLog(Block* pBlock, cBlock* pcBlock, const BlockPos& pos);
	bool renderBlockVine(Block* pBlock, cBlock* pcBlock, const BlockPos& pos);
	bool renderBlockLilyPad(Block* pBlock, cBlock* pcBlock, const BlockPos& pos);
	bool renderBlockStairs(Block* pBlock, cBlock* pcBlock, const BlockPos& pos);
	bool renderBlockCrops(Block* pBlock, cBlock* pcBlock, const BlockPos& pos);
	bool renderBlockStem(Block* pBlock, cBlock* pcBlock, const BlockPos& pos);
	bool renderBlockDoor(Block* pBlock, cBlock* pcBlock, const BlockPos& pos);
	bool renderBlockLadder(Block* pBlock, cBlock* pcBlock, const BlockPos& pos);
	bool renderBlockCocoa(Block* pBlock, cBlock* pcBlock, const BlockPos& pos);
	bool renderBlockFence(Block* pBlock, cBlock* pcBlock, const BlockPos& pos);
	bool renderBlockPane(Block* pBlock, cBlock* pcBlock, const BlockPos& pos);
	bool renderBlockFenceGate(Block* pBlock, cBlock* pcBlock, const BlockPos& pos);
	bool renderBlockWall(Block* pBlock, cBlock* pcBlock, const BlockPos& pos);
	bool renderBlockQuartz(Block* pBlock, cBlock* pcBlock, const BlockPos& pos);
	bool renderBlockBeacon(Block* pBlock, cBlock* pcBlock, const BlockPos& pos);
	bool renderBlockAnvil(Block* pBlock, cBlock* pcBlock, const BlockPos& pos);
	bool renderBlockFlowerpot(Block* pBlock, cBlock* pcBlock, const BlockPos& pos);
	bool renderBlockBed(Block* pBlock, cBlock* pcBlock, const BlockPos& pos);
	bool renderBlockMinecartTrack(Block* pBlock, cBlock* pcBlock, const BlockPos& pos);
	bool renderPistonBase(Block* pBlock, cBlock* pcBlock, const BlockPos& pos, bool par5);
	bool renderPistonExtension(Block* pBlock, cBlock* pcBlock, const BlockPos& pos, bool par5);
	bool renderBlockTorch(Block* pBlock, cBlock* pcBlock, const BlockPos& pos);
	bool renderBlockFire(Block* pBlock, cBlock* pcBlock, const BlockPos& pos);
	bool renderBlockLever(Block* pBlock, cBlock* pcBlock, const BlockPos& pos);
	bool renderBlockBrewingStand(Block* pBlock, cBlock* pcBlock, const BlockPos& pos);
	bool renderBlockCauldron(Block* pBlock, cBlock* pcBlock, const BlockPos& pos);
	bool renderBlockDragonEgg(Block* pBlock, cBlock* pcBlock, const BlockPos& pos);
	bool renderBlockTripWire(Block* pBlock, cBlock* pcBlock, const BlockPos& pos);
	bool renderBlockTripWireSource(Block* pBlock, cBlock* pcBlock, const BlockPos& pos);
	bool renderBlockHopper(Block* pBlock, cBlock* pcBlock, const BlockPos& pos);
	bool renderBlockEndPortalFrame(Block* pBlock, cBlock* pcBlock, const BlockPos& pos);
	bool renderBlockRedstoneLogic(Block* pBlock, cBlock* pcBlock, const BlockPos& pos);
	bool renderBlockRedStoneLogicMeta(Block* pBlock, cBlock* pcBlock, const BlockPos& pos, int meta);
	bool renderBlockRedstoneWire(Block* pBlock, cBlock* pcBlock, const BlockPos& pos);
	bool renderBlockRepeater(Block* pBlock, cBlock* pcBlock, const BlockPos& pos);
	bool renderBlockComparator(Block* pBlock, cBlock* pcBlock, const BlockPos& pos);
	bool renderBlockEffectChest(Block* pBlock, cBlock* pcBlock, const BlockPos& pos);
	bool renderBlockBanner(Block* pBlock, cBlock* pcBlock, const BlockPos& pos);
	bool renderBlockFruit(Block* pBlock, cBlock* pcBlock, const BlockPos& pos);

	void drawCrossedSquares(Block* pBlock, cBlock* pcBlock, int metadata, const Vector3& pos, float par9, int color, int bright);
	void drawCrossedSquaresEx(Block* pBlock, cBlock* pcBlock, int metadata, const Vector3& pos, float par9, int color, int bright);

	bool renderStandardBlockWithAmbientOcclusion(Block* pBlock, cBlock* pcBlock, const BlockPos& pos, const Color& _color);

	/** Renders non-full-cube block with ambient occusion.  Args: block, x, y, z, red, green, blue (lighting) */
	bool renderBlockWithAmbientOcclusion(Block* pBlock, cBlock* pcBlock, const BlockPos& pos, const Color& _color);

	/** Renders a standard cube block at the given coordinates, with a given color ratio.  Args: block, x, y, z, r, g, b */
	bool renderStandardBlockWithColorMultiplier(Block* pBlock, cBlock* pcBlock, const BlockPos& pos, const Color& _color);

	bool renderBlockHopperMetadata(Block* pBlock, cBlock* pcBlock, const BlockPos& pos, int metadata, bool par6);

	/** Renders the given texture to the face of the block. Args: block, x, y, z, texture */
	void renderFaceYNeg(cBlock* pcBlock, const Vector3& pos, AtlasSprite* sprite);
	void renderFaceYPos(cBlock* pcBlock, const Vector3& pos, AtlasSprite* sprite);
	void renderFaceZNeg(cBlock* pcBlock, const Vector3& pos, AtlasSprite* sprite);
	void renderFaceZPos(cBlock* pcBlock, const Vector3& pos, AtlasSprite* sprite);
	void renderFaceXNeg(cBlock* pcBlock, const Vector3& pos, AtlasSprite* sprite);
	void renderFaceXPos(cBlock* pcBlock, const Vector3& pos, AtlasSprite* sprite);

	void renderBlockStemSmall(Block* pBlock, cBlock* pcBlock, int metadata, float renderMaxY, const Vector3& pos, int color, int bright);
	void renderBlockStemBig(Block* pBlock, cBlock* pcBlock, int metadata, int state, float renderMaxY, const Vector3& pos, int color, int bright);
	bool renderBlockAnvilOrient(Block* pBlock, cBlock* pcBlock, const BlockPos& pos, int metadata, bool par6);
	float renderBlockAnvilRotate(Block* pBlock, cBlock* pcBlock, const BlockPos& pos, int state, float par6, float par7, float par8, float par9, bool par10, bool par11, int par12);

	bool renderBlockBannerOrient(Block* pBlock, cBlock* pcBlock, const BlockPos& pos, int metadata);
	void renderBlockBannerRotate(Block* pBlock, cBlock* pcBlock, const BlockPos& pos, float minX, float minY, float minZ, float maxX, float maxY, float maxZ, bool is_color, int color, bool is_slate);

	/** Render piston rod up/down */
	void renderPistonRodUD(float minX, float maxX, float minY, float maxY, float minZ, float maxZ, float par13, float par14);
	/** Render piston rod south/north */
	void renderPistonRodSN(float minX, float maxX, float minY, float maxY, float minZ, float maxZ, float par13, float par14);
	/** Render piston rod east/west */
	void renderPistonRodEW(float minX, float maxX, float minY, float maxY, float minZ, float maxZ, float par13, float par14);

	/** Renders a torch at the given coordinates, with the base slanting at the given delta */
	void renderTorchAtAngle(Block* pBlock, cBlock* pcBlock, const Vector3& pos, float par8, float par10, int par12);
	void renderBlockCropsImpl(cBlock* pcBlock, int bright, int meta, const Vector3& pos);
	void renderChest(Block* pBlock, int meta, const Vector3& pos);
	void renderBlockFruitImpl(cBlock* pcBlock, int bright, int meta, const Vector3& pos);


	AtlasSprite* getBlockIcon(cBlock* pcBlock, IBlockAccess* pBlockAccess, const BlockPos& pos, int side);
	AtlasSprite* getBlockIconFromSideAndMetadata(cBlock* pcBlock, int side, int metadata);
	AtlasSprite* getBlockIconFromSide(cBlock* pcBlock, int side);
	AtlasSprite* getBlockIcon(cBlock* pcBlock);
	AtlasSprite* getIconSafe(AtlasSprite* pSprite);

	static bool renderItemIn3d(int rendertype);
	void renderBlockAsItem(Block* pblock, cBlock* pcBlock, int meta);
	
	//add by maxicheng
	void setOABrightness(int brightness);
	void setOAColor(float r, float g, float b);
	void setBrightness(int brightness) { m_bright = brightness; }
	void setColor(float r, float g, float b);
	void setColor(const Color& _color);
	void setTranslate(const Vector3& trans) { m_transform = trans; }

protected:
	/** Get ambient occlusion brightness */
	int getAoBrightness(int par1, int par2, int par3, int par4);
	int mixAoBrightness(int par1, int par2, int par3, int par4, float par5, float par7, float par9, float par11);

	float getFluidHeight(const BlockPos& pos, const BM_Material& mat);

public:
	/** The IBlockAccess used by this instance of RenderBlocks */
	IBlockAccess* blockAccess = nullptr;
	SectionRenderer* sectionRenderer = nullptr;
	TessRenderable*	specialRenderable = nullptr;
	/** Fancy grass side matching biome */
	static bool fancyGrass;
	bool useInventoryTint = false;

protected:
	/** If set to >=0, all block faces will be rendered using this texture index */
	AtlasSprite* overrideBlockTexture = nullptr;
	/** this chest is for render item in hand. */
	TileEntityChest* m_chest = nullptr;
	TileEntityChest* m_Trapped = nullptr;
	TileEntityEnderChest* m_enderChest = nullptr;

	/** Set to true if the texture should be flipped horizontally during render*Face */
	bool flipTexture = false;

	/** If true, renders all faces on all blocks rather than using the logic in Block.shouldSideBeRendered.  Unused. */
	bool renderAllFaces = false;
	bool enableAO = false;

	int m_color = 0;
	int m_bright = 0;

	Vector3 m_transform;
	Vector3 m_sectionCenter;
	Vector3 m_sectionPos;

	/** The minimum  max X Y Z value for rendering . */
	float renderMinX = 0.f;
	float renderMaxX = 0.f;
	float renderMinY = 0.f;
	float renderMaxY = 0.f;
	float renderMinZ = 0.f;
	float renderMaxZ = 0.f;

	/** Set by overrideBlockBounds, to keep this class from changing the visual bounding box. */
	bool lockBlockBounds = false;
	bool partialRenderBounds = false;
	int uvRotateEast = 0;
	int uvRotateWest = 0;
	int uvRotateSouth = 0;
	int uvRotateNorth = 0;
	int uvRotateTop = 0;
	int uvRotateBottom = 0;

	/** Used as a scratch variable for ambient occlusion on the corner.*/
	float aoLightValueScratchXYZNNN = 0.f;
	float aoLightValueScratchXYNN = 0.f;
	float aoLightValueScratchXYZNNP = 0.f;
	float aoLightValueScratchYZNN = 0.f;
	float aoLightValueScratchYZNP = 0.f;
	float aoLightValueScratchXYZPNN = 0.f;
	float aoLightValueScratchXYPN = 0.f;
	float aoLightValueScratchXYZPNP = 0.f;
	float aoLightValueScratchXYZNPN = 0.f;
	float aoLightValueScratchXYNP = 0.f;
	float aoLightValueScratchXYZNPP = 0.f;
	float aoLightValueScratchYZPN = 0.f;
	float aoLightValueScratchXYZPPN = 0.f;
	float aoLightValueScratchXYPP = 0.f;
	float aoLightValueScratchYZPP = 0.f;
	float aoLightValueScratchXYZPPP = 0.f;
	float aoLightValueScratchXZNN = 0.f;
	float aoLightValueScratchXZPN = 0.f;
	float aoLightValueScratchXZNP = 0.f;
	float aoLightValueScratchXZPP = 0.f;

	/** Ambient occlusion brightness  */
	int aoBrightnessXYZNNN = 0;
	int aoBrightnessXYNN = 0;
	int aoBrightnessXYZNNP = 0;
	int aoBrightnessYZNN = 0;
	int aoBrightnessYZNP = 0;
	int aoBrightnessXYZPNN = 0;
	int aoBrightnessXYPN = 0;
	int aoBrightnessXYZPNP = 0;
	int aoBrightnessXYZNPN = 0;
	int aoBrightnessXYNP = 0;
	int aoBrightnessXYZNPP = 0;
	int aoBrightnessYZPN = 0;
	int aoBrightnessXYZPPN = 0;
	int aoBrightnessXYPP = 0;
	int aoBrightnessYZPP = 0;
	int aoBrightnessXYZPPP = 0;
	int aoBrightnessXZNN = 0;
	int aoBrightnessXZPN = 0;
	int aoBrightnessXZNP = 0;
	int aoBrightnessXZPP = 0;

	/** Brightness */
	int brightnessTopLeft = 0;
	int brightnessBottomLeft = 0;
	int brightnessBottomRight = 0;
	int brightnessTopRight = 0;

	/** ao */
	float aoTopLeft = 0;
	float aoBottomLeft = 0;
	float aoBottomRight = 0;
	float aoTopRgith = 0;

	/** Red color value for the corner */
	Color colorTopLeft;
	Color colorBottomLeft;
	Color colorBottomRight;
	Color colorTopRight;
};

}

#endif
