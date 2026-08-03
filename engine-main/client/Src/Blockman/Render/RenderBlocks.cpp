#include "RenderBlocks.h"
#include "BM_TypeDef.h"
#include "SectionRenderer.h"
#include "TextureAtlas.h"
#include "cBlock/cBlock.h"
#include "cBlock/cBlockManager.h"
#include "cTileEntity/TileEntityRenderMgr.h"

#include "Block/Blocks.h"
#include "Block/BlockManager.h"
#include "TileEntity/TileEntityInventory.h"
#include "TileEntity/TileEntitys.h"
#include "World/IBlockAccess.h"

#include "Tessolator/TessManager.h"
#include "Tessolator/TessRenderable.h"
#include "RenderEntity.h"

namespace BLOCKMAN
{

bool RenderBlocks::fancyGrass = true;

const float anti_gap = 0.000000f;

RenderBlocks::RenderBlocks(IBlockAccess* pBlockAccess, SectionRenderer* pSectionRenderer, const Vector3& sectionCenter)
{
	init();
	blockAccess = pBlockAccess;
	sectionRenderer = pSectionRenderer;
	m_sectionCenter = sectionCenter;
	m_sectionPos = sectionCenter - 8.f;
}

RenderBlocks::RenderBlocks(IBlockAccess* pBlockAccess)
{
	init();
	blockAccess = pBlockAccess;
	m_sectionCenter = Vector3(8.f, 8.f, 8.f);
	m_sectionPos = Vector3::ZERO;
}

RenderBlocks::~RenderBlocks()
{
	LordSafeDelete(m_chest);
	LordSafeDelete(m_Trapped);
	LordSafeDelete(m_enderChest);
}

void RenderBlocks::init()
{
	useInventoryTint = true;
	sectionRenderer = NULL;
	overrideBlockTexture = NULL;
	flipTexture = false;
	renderAllFaces = false;
	renderMinX = 0.0;
	renderMaxX = 1.0;
	renderMinY = 0.0;
	renderMaxY = 1.0;
	renderMinZ = 0.0;
	renderMaxZ = 1.0;
	lockBlockBounds = false;
	partialRenderBounds = false;
	uvRotateEast = 0;
	uvRotateWest = 0;
	uvRotateSouth = 0;
	uvRotateNorth = 0;
	uvRotateTop = 0;
	uvRotateBottom = 0;
	m_transform = Vector3::ZERO;
	enableAO = false;
	aoLightValueScratchXYZNNN = 0.f;
	aoLightValueScratchXYNN = 0.f;
	aoLightValueScratchXYZNNP = 0.f;
	aoLightValueScratchYZNN = 0.f;
	aoLightValueScratchYZNP = 0.f;
	aoLightValueScratchXYZPNN = 0.f;
	aoLightValueScratchXYPN = 0.f;
	aoLightValueScratchXYZPNP = 0.f;
	aoLightValueScratchXYZNPN = 0.f;
	aoLightValueScratchXYNP = 0.f;
	aoLightValueScratchXYZNPP = 0.f;
	aoLightValueScratchYZPN = 0.f;
	aoLightValueScratchXYZPPN = 0.f;
	aoLightValueScratchXYPP = 0.f;
	aoLightValueScratchYZPP = 0.f;
	aoLightValueScratchXYZPPP = 0.f;
	aoLightValueScratchXZNN = 0.f;
	aoLightValueScratchXZPN = 0.f;
	aoLightValueScratchXZNP = 0.f;
	aoLightValueScratchXZPP = 0.f;
	aoBrightnessXYZNNN = 0;
	aoBrightnessXYNN = 0;
	aoBrightnessXYZNNP = 0;
	aoBrightnessYZNN = 0;
	aoBrightnessYZNP = 0;
	aoBrightnessXYZPNN = 0;
	aoBrightnessXYPN = 0;
	aoBrightnessXYZPNP = 0;
	aoBrightnessXYZNPN = 0;
	aoBrightnessXYNP = 0;
	aoBrightnessXYZNPP = 0;
	aoBrightnessYZPN = 0;
	aoBrightnessXYZPPN = 0;
	aoBrightnessXYPP = 0;
	aoBrightnessYZPP = 0;
	aoBrightnessXYZPPP = 0;
	aoBrightnessXZNN = 0;
	aoBrightnessXZPN = 0;
	aoBrightnessXZNP = 0;
	aoBrightnessXZPP = 0;
	brightnessTopLeft = 0;
	brightnessBottomLeft = 0;
	brightnessBottomRight = 0;
	brightnessTopRight = 0;
	colorTopLeft = Color(0.8f, 0.8f, 0.8f, 1.f);
	colorBottomLeft = Color(0.8f, 0.8f, 0.8f, 1.f);
	colorBottomRight = Color(0.8f, 0.8f, 0.8f, 1.f);
	colorTopRight = Color(0.8f, 0.8f, 0.8f, 1.f);
	m_sectionCenter = Vector3::ZERO;
	specialRenderable = NULL;
	m_chest = LordNew TileEntityChest();
	m_Trapped = LordNew TileEntityChest(1);
	m_enderChest = LordNew TileEntityEnderChest();
}

void RenderBlocks::setRenderBounds(float minx, float miny, float minz, float maxx, float maxy, float maxz)
{
	if (!lockBlockBounds)
	{
		renderMinX = minx;
		renderMaxX = maxx;
		renderMinY = miny;
		renderMaxY = maxy;
		renderMinZ = minz;
		renderMaxZ = maxz;
		partialRenderBounds = /*blockmanRB.gameSettings.ambientOcclusion >= 2 &&*/
			(renderMinX > 0.0 ||
				renderMaxX < 1.0 ||
				renderMinY > 0.0 ||
				renderMaxY < 1.0 ||
				renderMinZ > 0.0 ||
				renderMaxZ < 1.0);
	}
}

/** Like setRenderBounds, but automatically pulling the bounds from the given Block. */
void RenderBlocks::setRenderBoundsFromBlock(Block* pBlock)
{
	if (!lockBlockBounds)
	{
		renderMinX = (float)pBlock->getBlockBoundsMinX();
		renderMaxX = (float)pBlock->getBlockBoundsMaxX();
		renderMinY = (float)pBlock->getBlockBoundsMinY();
		renderMaxY = (float)pBlock->getBlockBoundsMaxY();
		renderMinZ = (float)pBlock->getBlockBoundsMinZ();
		renderMaxZ = (float)pBlock->getBlockBoundsMaxZ();
		partialRenderBounds = /*blockmanRB.gameSettings.ambientOcclusion >= 2 &&*/
			(renderMinX > 0.0 ||
				renderMaxX < 1.0 ||
				renderMinY > 0.0 ||
				renderMaxY < 1.0 ||
				renderMinZ > 0.0 ||
				renderMaxZ < 1.0);
	}
}

/** Like setRenderBounds, but locks the values so that RenderBlocks won't change them.  If you use this, you must
* call unlockBlockBounds after you finish rendering! */
void RenderBlocks::overrideBlockBounds(float minx, float miny, float minz, float maxx, float  maxy, float maxz)
{
	renderMinX = minx;
	renderMaxX = maxx;
	renderMinY = miny;
	renderMaxY =  maxy;
	renderMinZ = minz;
	renderMaxZ = maxz;
	lockBlockBounds = true;
	partialRenderBounds = /*blockmanRB.gameSettings.ambientOcclusion >= 2 &&*/
		(renderMinX > 0.0 ||
			renderMaxX < 1.0 ||
			renderMinY > 0.0 ||
			renderMaxY < 1.0 ||
			renderMinZ > 0.0 ||
			renderMaxZ < 1.0);
}

void RenderBlocks::renderBlockUsingTexture(Block* pBlock, const BlockPos& pos, AtlasSprite* sprite)
{
	setOverrideBlockTexture(sprite);
	renderBlockByRenderType(pBlock, pos);
	clearOverrideBlockTexture();
}

/** Render all faces of a block */
void RenderBlocks::renderBlockAllFaces(Block* pBlock, const BlockPos& pos)
{
	renderAllFaces = true;
	renderBlockByRenderType(pBlock, pos);
	renderAllFaces = false;
}

/** Renders the block at the given coordinates using the block's rendering type */
bool RenderBlocks::renderBlockByRenderType(Block* pBlock, const BlockPos& pos, bool select)
{
	cBlock* pcBLock = cBlockManager::scBlocks[pBlock->getBlockID()];
	LordAssert(pcBLock);

	int renderType = pcBLock->getRenderType();

	bool bResult = false;
	if (renderType == -1)
		return bResult;

	pBlock->setBlockBoundsBasedOnState(blockAccess, pos);
	setRenderBoundsFromBlock(pBlock);
	
	switch (renderType)
	{
	case RENDER_TYPE_TRAPDOOR:		bResult = renderStandardBlock(pBlock, pcBLock, pos);		break;
	case RENDER_TYPE_FLOWER:		bResult = renderCrossedSquares(pBlock, pcBLock, pos);		break;
	case RENDER_TYPE_TORCH:			bResult = renderBlockTorch(pBlock, pcBLock, pos);			break;
	case RENDER_TYPE_FIRE:			bResult = renderBlockFire(pBlock, pcBLock, pos);			break;
	case RENDER_TYPE_FLUID:			bResult = renderBlockFluids(pBlock, pcBLock, pos);			break;
	case RENDER_TYPE_CROPS:			bResult = renderBlockCrops(pBlock, pcBLock, pos);			break;
	case RENDER_TYPE_DOOR:			bResult = renderBlockDoor(pBlock, pcBLock, pos);			break;
	case RENDER_TYPE_LADDER:		bResult = renderBlockLadder(pBlock, pcBLock, pos);			break;
	case RENDER_TYPE_RAIL_BASE:		bResult = renderBlockMinecartTrack(pBlock, pcBLock, pos);	break;
	case RENDER_TYPE_STAIRS:		bResult = renderBlockStairs(pBlock, pcBLock, pos);			break;
	case RENDER_TYPE_FENCE:			bResult = renderBlockFence(pBlock, pcBLock, pos);			break;
	case RENDER_TYPE_LEVER:			bResult = renderBlockLever(pBlock, pcBLock, pos);			break;
	case RENDER_TYPE_CACTUS:		bResult = renderBlockCactus(pBlock, pcBLock, pos);			break;
	case RENDER_TYPE_BED:			bResult = renderBlockBed(pBlock, pcBLock, pos);				break;
	case RENDER_TYPE_PISTON_BASE:	bResult = renderPistonBase(pBlock, pcBLock, pos, false);	break;
	case RENDER_TYPE_PISTON_EXT:	bResult = renderPistonExtension(pBlock, pcBLock, pos, true);break;
	case RENDER_TYPE_PANE:			bResult = renderBlockPane(pBlock, pcBLock, pos);			break;
	case RENDER_TYPE_STEM:			bResult = renderBlockStem(pBlock, pcBLock, pos);			break;
	case RENDER_TYPE_VINE:			bResult = renderBlockVine(pBlock, pcBLock, pos);			break;
	case RENDER_TYPE_FENCE_GATE:	bResult = renderBlockFenceGate(pBlock, pcBLock, pos);		break;
	case RENDER_TYPE_LILYPAD:		bResult = renderBlockLilyPad(pBlock, pcBLock, pos);			break;
	case RENDER_TYPE_CAULDRON:		bResult = renderBlockCauldron(pBlock, pcBLock, pos);		break;
	case RENDER_TYPE_BREWING_STAND:	bResult = renderBlockBrewingStand(pBlock, pcBLock, pos);	break;
	case RENDER_TYPE_PORTAL_FRAME:	bResult = renderBlockEndPortalFrame(pBlock, pcBLock, pos);	break;
	case RENDER_TYPE_DRAGON_EGG:	bResult = renderBlockDragonEgg(pBlock, pcBLock, pos);		break;
	case RENDER_TYPE_COCOA:			bResult = renderBlockCocoa(pBlock, pcBLock, pos);			break;
	case RENDER_TYPE_TRIPWIRE_SOURCE:bResult = renderBlockTripWireSource(pBlock, pcBLock, pos);	break;
	case RENDER_TYPE_TRIPWIRE:		bResult = renderBlockTripWire(pBlock, pcBLock, pos);		break;
	case RENDER_TYPE_HAY:			bResult = renderBlockLog(pBlock, pcBLock, pos);				break;
	case RENDER_TYPE_WALL:			bResult = renderBlockWall(pBlock, pcBLock, pos);			break;
	case RENDER_TYPE_FLOWER_POT:	bResult = renderBlockFlowerpot(pBlock, pcBLock, pos);		break;
	case RENDER_TYPE_BEACON:		bResult = renderBlockBeacon(pBlock, pcBLock, pos);			break;
	case RENDER_TYPE_ANVIL:			bResult = renderBlockAnvil(pBlock, pcBLock, pos);			break;
	case RENDER_TYPE_HOPPER:		bResult = renderBlockHopper(pBlock,pcBLock, pos);			break;
	case RENDER_TYPE_QUARTZ:		bResult = renderBlockQuartz(pBlock, pcBLock, pos);			break;
	case RENDER_TYPE_REDSTONE_REPEATER:bResult = renderBlockRepeater(pBlock, pcBLock, pos);		break;
	case RENDER_TYPE_COMPARATOR:	bResult = renderBlockComparator(pBlock, pcBLock, pos);		break;
	case RENDER_TYPE_REDSTONE_LOGIC:bResult = renderBlockRedstoneLogic(pBlock, pcBLock, pos);	break;
	case RENDER_TYPE_REDSTONE_WIRE:	bResult = renderBlockRedstoneWire(pBlock, pcBLock, pos);	break;
	case RENDER_TYPE_EFFECT_CHEST:	bResult = renderBlockEffectChest(pBlock, pcBLock, pos);		break;
	case RENDER_TYPE_LEAVES:		bResult = renderCrossedSquares(pBlock, pcBLock, pos);		break;
	case RENDER_TYPE_AIR:			bResult = renderAirBlock(pBlock, pcBLock, pos, select);		break;
	case RENDER_TYPE_BANNER:		bResult = renderBlockBanner(pBlock, pcBLock, pos);			break;
	case RENDER_TYPE_FRUIT:			bResult = renderBlockFruit(pBlock, pcBLock, pos);			break;
	//case RENDER_TYPE_CHEST:			bResult = renderBlcokChest(pBlock, pos); break;
	//																	
	//
	
	default:	break;
	}
	return bResult;
}

bool RenderBlocks::renderStandardBlock(Block* pBlock, cBlock* pcBlock, const BlockPos& pos)
{
	ui32 rgb = pcBlock->colorMultiplier(blockAccess, pos);
	Color _color;
	_color.setARGB(rgb);

	return /*Blockman.isAmbientOcclusionEnabled() && */		
		BlockManager::sLightValue[pBlock->getBlockID()] == 0 ?
		(partialRenderBounds ? renderBlockWithAmbientOcclusion(pBlock, pcBlock, pos, _color) :
			renderStandardBlockWithAmbientOcclusion(pBlock, pcBlock, pos, _color)) :
			renderStandardBlockWithColorMultiplier(pBlock, pcBlock, pos, _color);
}

bool RenderBlocks::renderAirBlock(Block * pBlock, cBlock * pcBlock, const BlockPos & pos, bool select)
{
	if (!select) return false;

	ui32 rgb = pcBlock->colorMultiplier(blockAccess, pos);
	Color _color;
	_color.setARGB(rgb);

	return /*Blockman.isAmbientOcclusionEnabled() && */
		BlockManager::sLightValue[pBlock->getBlockID()] == 0 ?
		(partialRenderBounds ? renderBlockWithAmbientOcclusion(pBlock, pcBlock, pos, _color) :
			renderStandardBlockWithAmbientOcclusion(pBlock, pcBlock, pos, _color)) :
		renderStandardBlockWithColorMultiplier(pBlock, pcBlock, pos, _color);
}

bool RenderBlocks::renderCrossedSquares(Block* pBlock, cBlock* pcBlock, const BlockPos& pos)
{
	// Tessellator var5 = Tessellator.instance;
	// var5.setBrightness(pBlock->getMixedBrightnessForBlock(blockAccess, pos));
	int bright = pcBlock->getMixedBrightnessForBlock(blockAccess, pos);
	ui32 color = pcBlock->colorMultiplier(blockAccess, pos);
	Vector3 rpos = pos;

	if (pBlock->getBlockID() == BLOCK_ID_TALL_GRASS)
	{
		long rn = (long)(pos.x * 3129871) ^ (long)pos.z * 116129781L ^ (long)pos.y;
		rn = rn * rn * 42317861L + rn * 11L;
		rpos.x += (((float)(rn >> 16 & 15L) / 15.0F) - 0.5f) * 0.5f;
		rpos.y += (((float)(rn >> 20 & 15L) / 15.0F) - 1.0f) * 0.2f;
		rpos.z += (((float)(rn >> 24 & 15L) / 15.0F) - 0.5f) * 0.5f;
	}

	drawCrossedSquares(pBlock, pcBlock, blockAccess->getBlockMeta(pos), rpos, 1.0F, color, bright);
	return true;
}

bool RenderBlocks::renderBlockLeaves(Block* pBlock, cBlock* pcBlock, const BlockPos& pos)
{
	// Tessellator var5 = Tessellator.instance;
	// var5.setBrightness(pBlock->getMixedBrightnessForBlock(blockAccess, pos));
	int bright = pcBlock->getMixedBrightnessForBlock(blockAccess, pos);
	ui32 color = pcBlock->colorMultiplier(blockAccess, pos);
	Vector3 rpos = pos;

	if (pBlock->getBlockID() == BLOCK_ID_TALL_GRASS)
	{
		long rn = (long)(pos.x * 3129871) ^ (long)pos.z * 116129781L ^ (long)pos.y;
		rn = rn * rn * 42317861L + rn * 11L;
		rpos.x += (((float)(rn >> 16 & 15L) / 15.0F) - 0.5f) * 0.5f;
		rpos.y += (((float)(rn >> 20 & 15L) / 15.0F) - 1.0f) * 0.2f;
		rpos.z += (((float)(rn >> 24 & 15L) / 15.0F) - 0.5f) * 0.5f;
	}

	drawCrossedSquaresEx(pBlock, pcBlock, blockAccess->getBlockMeta(pos), rpos, 1.0F, color, bright);
	return true;
}

bool RenderBlocks::renderBlockCactus(Block* pBlock, cBlock* pcBlock, const BlockPos& pos)
{
	ui32 color = pcBlock->colorMultiplier(blockAccess, pos);
	float par5 = (float)(color >> 16 & 255) / 255.0F;
	float par6 = (float)(color >> 8 & 255) / 255.0F;
	float par7 = (float)(color & 255) / 255.0F;

	bool var9 = false;
	float var10 = 0.5F;
	float var11 = 1.0F;
	float var12 = 0.8F;
	float var13 = 0.6F;
	float var14 = var10 * par5;
	float var15 = var11 * par5;
	float var16 = var12 * par5;
	float var17 = var13 * par5;
	float var18 = var10 * par6;
	float var19 = var11 * par6;
	float var20 = var12 * par6;
	float var21 = var13 * par6;
	float var22 = var10 * par7;
	float var23 = var11 * par7;
	float var24 = var12 * par7;
	float var25 = var13 * par7;
	float var26 = 0.0626F;
	int var27 = pcBlock->getMixedBrightnessForBlock(blockAccess, pos);

	if (renderAllFaces || pBlock->shouldSideBeRendered(blockAccess, pos.getNegY(), 0))
	{
		setBrightness(renderMinY > 0.0f ? var27 : pcBlock->getMixedBrightnessForBlock(blockAccess, pos.getNegY()));
		setColor(var14, var18, var22);
		renderFaceYNeg(pcBlock, pos, getBlockIcon(pcBlock, blockAccess, pos, 0));
	}

	if (renderAllFaces || pBlock->shouldSideBeRendered(blockAccess, pos.getPosY(), 1))
	{
		setBrightness(renderMaxY < 1.0f ? var27 : pcBlock->getMixedBrightnessForBlock(blockAccess, pos.getPosY()));
		setColor(var15, var19, var23);
		renderFaceYPos(pcBlock, pos, getBlockIcon(pcBlock, blockAccess, pos, 1));
	}
	Vector3 trans_bak = m_transform;
	m_transform = trans_bak + Vector3(0.0F, 0.0F, var26);
	renderFaceZNeg(pcBlock, pos, getBlockIcon(pcBlock, blockAccess, pos, 2));
	m_transform = trans_bak + Vector3(0.0F, 0.0F, -var26);
	renderFaceZPos(pcBlock, pos, getBlockIcon(pcBlock, blockAccess, pos, 3));
	m_transform = trans_bak + Vector3(var26, 0.0F, 0.0F);
	renderFaceXNeg(pcBlock, pos, getBlockIcon(pcBlock, blockAccess, pos, 4));
	m_transform = trans_bak + Vector3(-var26, 0.0F, 0.0F);
	renderFaceXPos(pcBlock, pos, getBlockIcon(pcBlock, blockAccess, pos, 5));
	m_transform = trans_bak;
	return true;
}

bool RenderBlocks::renderBlockEffectChest(Block* pBlock, cBlock* pcBlock, const BlockPos& pos)
{
	cBlockWithEffect* pChestEffect = (cBlockWithEffect*)(pcBlock);
	int bright = pChestEffect->getMixedBrightnessForBlock(blockAccess, pos);
	int color = Color(1.0F, 1.0F, 1.0F).getARGB();
	int metadata = blockAccess->getBlockMeta(pos);
	int direction = BlockCocoa::getDirection(metadata);

	AtlasSprite* spriteTop = pChestEffect->getIcon(0, direction);
	AtlasSprite* spriteFront = pChestEffect->getIcon(2, direction);
	AtlasSprite* spriteSide = pChestEffect->getIcon(3, direction);
	
	float offl = 4.f / 64.f;
	float offr = 60.f / 64.f;
	float offt = 20.f / 64.f;
	float offb = 64.f / 64.f;
	float offh = 44.f / 64.f;
	float topminu = spriteTop->getInterpolatedU(offl);
	float topminv = spriteTop->getInterpolatedV(offl);
	float topmaxu = spriteTop->getInterpolatedU(offr);
	float topmaxv = spriteTop->getInterpolatedV(offr);

	float frontminu = spriteFront->getInterpolatedU(offl);
	float frontminv = spriteFront->getInterpolatedV(offt);
	float frontmaxu = spriteFront->getInterpolatedU(offr);
	float frontmaxv = spriteFront->getInterpolatedV(offb);

	float sideminu = spriteSide->getInterpolatedU(offl);
	float sideminv = spriteSide->getInterpolatedV(offt);
	float sidemaxu = spriteSide->getInterpolatedU(offr);
	float sidemaxv = spriteSide->getInterpolatedV(offb);

	int pass = pChestEffect->getRenderBlockPass();
	TessRenderable* renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(spriteTop->getTexture(), (TESS_RENDER_PASS)pass, 6);
	TessVertexFmt* pVetLockPtr = NULL;
	if (!renderable->getLockPtr(6, pVetLockPtr))
		return false;

	float x = pos.x + 0.f;
	float y = pos.y + 0.f;
	float z = pos.z + 0.f;
	// buttom
	pVetLockPtr[0].setEx(m_sectionPos, x + offl, y, z + offl, color, bright, topminu, topminv);
	pVetLockPtr[1].setEx(m_sectionPos, x + offr, y, z + offl, color, bright, topmaxu, topminv);
	pVetLockPtr[2].setEx(m_sectionPos, x + offr, y, z + offr, color, bright, topmaxu, topmaxv);
	pVetLockPtr[3].setEx(m_sectionPos, x + offl, y, z + offr, color, bright, topminu, topmaxv);
	//AutoCaculateQuadNormal(pVetLockPtr + 0);

	// top
	pVetLockPtr[4].setEx(m_sectionPos, x + offl, y + offh, z + offl, color, bright, topminu, topminv);
	pVetLockPtr[7].setEx(m_sectionPos, x + offr, y + offh, z + offl, color, bright, topmaxu, topminv);
	pVetLockPtr[6].setEx(m_sectionPos, x + offr, y + offh, z + offr, color, bright, topmaxu, topmaxv);
	pVetLockPtr[5].setEx(m_sectionPos, x + offl, y + offh, z + offr, color, bright, topminu, topmaxv);
	//AutoCaculateQuadNormal(pVetLockPtr + 4);

	// fornt
	pVetLockPtr[ 8].setEx(m_sectionPos, x + offl, y + 0.0f, z + offl, color, bright, frontminu, frontmaxv);
	pVetLockPtr[11].setEx(m_sectionPos, x + offr, y + 0.0f, z + offl, color, bright, frontmaxu, frontmaxv);
	pVetLockPtr[10].setEx(m_sectionPos, x + offr, y + offh, z + offl, color, bright, frontmaxu, frontminv);
	pVetLockPtr[ 9].setEx(m_sectionPos, x + offl, y + offh, z + offl, color, bright, frontminu, frontminv);
	//AutoCaculateQuadNormal(pVetLockPtr + 8);

	// side1
	pVetLockPtr[12].setEx(m_sectionPos, x + offl, y + 0.0f, z + offl, color, bright, sideminu, sidemaxv);
	pVetLockPtr[13].setEx(m_sectionPos, x + offl, y + 0.0f, z + offr, color, bright, sidemaxu, sidemaxv);
	pVetLockPtr[14].setEx(m_sectionPos, x + offl, y + offh, z + offr, color, bright, sidemaxu, sideminv);
	pVetLockPtr[15].setEx(m_sectionPos, x + offl, y + offh, z + offl, color, bright, sideminu, sideminv);
	//AutoCaculateQuadNormal(pVetLockPtr + 12);

	// side2
	pVetLockPtr[16].setEx(m_sectionPos, x + offr, y + 0.0f, z + offl, color, bright, sideminu, sidemaxv);
	pVetLockPtr[19].setEx(m_sectionPos, x + offr, y + 0.0f, z + offr, color, bright, sidemaxu, sidemaxv);
	pVetLockPtr[18].setEx(m_sectionPos, x + offr, y + offh, z + offr, color, bright, sidemaxu, sideminv);
	pVetLockPtr[17].setEx(m_sectionPos, x + offr, y + offh, z + offl, color, bright, sideminu, sideminv);
	//AutoCaculateQuadNormal(pVetLockPtr + 16);

	// side3
	pVetLockPtr[20].setEx(m_sectionPos, x + offl, y + 0.0f, z + offr, color, bright, sideminu, sidemaxv);
	pVetLockPtr[21].setEx(m_sectionPos, x + offr, y + 0.0f, z + offr, color, bright, sidemaxu, sidemaxv);
	pVetLockPtr[22].setEx(m_sectionPos, x + offr, y + offh, z + offr, color, bright, sidemaxu, sideminv);
	pVetLockPtr[23].setEx(m_sectionPos, x + offl, y + offh, z + offr, color, bright, sideminu, sideminv);
	//AutoCaculateQuadNormal(pVetLockPtr + 20);

	return true;
}

bool RenderBlocks::renderBlockBanner(Block * pBlock, cBlock * pcBlock, const BlockPos & pos)
{
	int metadata = blockAccess->getBlockMeta(pos);

	return renderBlockBannerOrient(pBlock, pcBlock, pos, metadata);
}

bool RenderBlocks::renderBlockFruit(Block* pBlock, cBlock* pcBlock, const BlockPos& pos)
{
	int bright = pcBlock->getMixedBrightnessForBlock(blockAccess, pos);

	int meta = blockAccess->getBlockMeta(pos);

	renderBlockFruitImpl(pcBlock, bright, meta, Vector3(pos));

	return true;
}

bool RenderBlocks::renderBlockLog(Block* pBlock, cBlock* pcBlock, const BlockPos& pos)
{
	int metadata = blockAccess->getBlockMeta(pos);
	int var6 = metadata & 12;

	if (var6 == 4)
	{
		uvRotateEast = 1;
		uvRotateWest = 1;
		uvRotateTop = 1;
		uvRotateBottom = 1;
	}
	else if (var6 == 8)
	{
		uvRotateSouth = 1;
		uvRotateNorth = 1;
	}

	bool var7 = renderStandardBlock(pBlock, pcBlock, pos);
	uvRotateSouth = 0;
	uvRotateEast = 0;
	uvRotateWest = 0;
	uvRotateNorth = 0;
	uvRotateTop = 0;
	uvRotateBottom = 0;
	return var7;
}

bool RenderBlocks::renderBlockVine(Block* pBlock, cBlock* pcBlock, const BlockPos& pos)
{
	AtlasSprite* sprite = getBlockIconFromSide(pcBlock, 0);

	if (hasOverrideBlockTexture())
	{
		sprite = overrideBlockTexture;
	}

	float var7 = 1.0F;
	int bright = pcBlock->getMixedBrightnessForBlock(blockAccess, pos);
	// var5.setBrightness();
	ui32 color = pcBlock->colorMultiplier(blockAccess, pos);
	// float var9 = (float)(color >> 16 & 255) / 255.0F;
	// float var10 = (float)(color >> 8 & 255) / 255.0F;
	// float var11 = (float)(color & 255) / 255.0F;
	// var5.setColorOpaque_F(var7 * var9, var7 * var10, var7 * var11);
	float var19 = sprite->getMinU();
	float var20 = sprite->getMinV();
	float var12 = sprite->getMaxU();
	float var14 = sprite->getMaxV();
	float xoff = 0.05000000f;
	int var18 = blockAccess->getBlockMeta(pos);
	int pass = pcBlock->getRenderBlockPass();

	if ((var18 & 2) != 0)
	{
		TessRenderable* renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite->getTexture(), (TESS_RENDER_PASS)pass, 2);
		TessVertexFmt* pVetex = NULL;
		if (renderable->getLockPtr(2, pVetex))
		{
			float x = pos.x + xoff;
			float y0 = pos.y + 0.f;
			float y1 = pos.y + 1.f;
			float z0 = pos.z + 0.f;
			float z1 = pos.z + 1.f;
			pVetex[0].setEx(m_sectionPos, x, y1, z1, color, bright, var19, var20);
			pVetex[1].setEx(m_sectionPos, x, y0, z1, color, bright, var19, var14);
			pVetex[2].setEx(m_sectionPos, x, y0, z0, color, bright, var12, var14);
			pVetex[3].setEx(m_sectionPos, x, y1, z0, color, bright, var12, var20);
			//AutoCaculateQuadNormal(pVetex + 0);
			
			pVetex[4].setEx(m_sectionPos, x, y1, z0, color, bright, var12, var20);
			pVetex[5].setEx(m_sectionPos, x, y0, z0, color, bright, var12, var14);
			pVetex[6].setEx(m_sectionPos, x, y0, z1, color, bright, var19, var14);
			pVetex[7].setEx(m_sectionPos, x, y1, z1, color, bright, var19, var20);
			//AutoCaculateQuadNormal(pVetex + 4);
		}
	}

	if ((var18 & 8) != 0)
	{
		TessRenderable* renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite->getTexture(), (TESS_RENDER_PASS)pass, 2);
		TessVertexFmt* pVetex = NULL;
		if (renderable->getLockPtr(2, pVetex))
		{
			float x = pos.x + 1.f - xoff;
			float y0 = pos.y + 0.f;
			float y1 = pos.y + 1.f;
			float z0 = pos.z + 0.f;
			float z1 = pos.z + 1.f;
			pVetex[0].setEx(m_sectionPos, x, y0, z1, color, bright, var12, var14);
			pVetex[1].setEx(m_sectionPos, x, y1, z1, color, bright, var12, var20);
			pVetex[2].setEx(m_sectionPos, x, y1, z0, color, bright, var19, var20);
			pVetex[3].setEx(m_sectionPos, x, y0, z0, color, bright, var19, var14);
			//AutoCaculateQuadNormal(pVetex + 0);

			pVetex[4].setEx(m_sectionPos, x, y0, z0, color, bright, var19, var14);
			pVetex[5].setEx(m_sectionPos, x, y1, z0, color, bright, var19, var20);
			pVetex[6].setEx(m_sectionPos, x, y1, z1, color, bright, var12, var20);
			pVetex[7].setEx(m_sectionPos, x, y0, z1, color, bright, var12, var14);
			//AutoCaculateQuadNormal(pVetex + 4);
		}
	}

	if ((var18 & 4) != 0)
	{
		TessRenderable* renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite->getTexture(), (TESS_RENDER_PASS)pass, 2);
		TessVertexFmt* pVetex = NULL;
		if (renderable->getLockPtr(2, pVetex))
		{
			float x0 = pos.x + 0.f;
			float x1 = pos.x + 1.f;
			float y0 = pos.y + 0.f;
			float y1 = pos.y + 1.f;
			float z = pos.z + xoff;
			pVetex[0].setEx(m_sectionPos, x1, y0, z, color, bright, var12, var14);
			pVetex[1].setEx(m_sectionPos, x1, y1, z, color, bright, var12, var20);
			pVetex[2].setEx(m_sectionPos, x0, y1, z, color, bright, var19, var20);
			pVetex[3].setEx(m_sectionPos, x0, y0, z, color, bright, var19, var14);
			//AutoCaculateQuadNormal(pVetex + 0);

			pVetex[4].setEx(m_sectionPos, x0, y0, z, color, bright, var19, var14);
			pVetex[5].setEx(m_sectionPos, x0, y1, z, color, bright, var19, var20);
			pVetex[6].setEx(m_sectionPos, x1, y1, z, color, bright, var12, var20);
			pVetex[7].setEx(m_sectionPos, x1, y0, z, color, bright, var12, var14);
			//AutoCaculateQuadNormal(pVetex + 4);
		}
	}

	if ((var18 & 1) != 0)
	{
		TessRenderable* renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite->getTexture(), (TESS_RENDER_PASS)pass, 2);
		TessVertexFmt* pVetex = NULL;
		if (renderable->getLockPtr(2, pVetex))
		{
			float x0 = pos.x + 0.f;
			float x1 = pos.x + 1.f;
			float y0 = pos.y + 0.f;
			float y1 = pos.y + 1.f;
			float z = pos.z + 1.f - xoff;
			pVetex[0].setEx(m_sectionPos, x1, y1, z, color, bright, var19, var20);
			pVetex[1].setEx(m_sectionPos, x1, y0, z, color, bright, var19, var14);
			pVetex[2].setEx(m_sectionPos, x0, y0, z, color, bright, var12, var14);
			pVetex[3].setEx(m_sectionPos, x0, y1, z, color, bright, var12, var20);
			//AutoCaculateQuadNormal(pVetex + 0);

			pVetex[4].setEx(m_sectionPos, x0, y1, z, color, bright, var12, var20);
			pVetex[5].setEx(m_sectionPos, x0, y0, z, color, bright, var12, var14);
			pVetex[6].setEx(m_sectionPos, x1, y0, z, color, bright, var19, var14);
			pVetex[7].setEx(m_sectionPos, x1, y1, z, color, bright, var19, var20);
			//AutoCaculateQuadNormal(pVetex + 4);
		}

	}

	if (blockAccess->isBlockNormalCube(pos.getPosY()))
	{
		TessRenderable* renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite->getTexture(), (TESS_RENDER_PASS)pass, 1);
		TessVertexFmt* pVetex = NULL;
		if (renderable->getLockPtr(1, pVetex))
		{
			float x0 = pos.x + 0.f;
			float x1 = pos.x + 1.f;
			float z0 = pos.z + 0.f;
			float z1 = pos.z + 1.f;
			float y = pos.y + 1.f - xoff;
			pVetex[0].setEx(m_sectionPos, x1, y, z0, color, bright, var19, var20);
			pVetex[1].setEx(m_sectionPos, x1, y, z1, color, bright, var19, var14);
			pVetex[2].setEx(m_sectionPos, x0, y, z1, color, bright, var12, var14);
			pVetex[3].setEx(m_sectionPos, x0, y, z0, color, bright, var12, var20);
			//AutoCaculateQuadNormal(pVetex + 0);
		}
	}

	return true;
}

bool RenderBlocks::renderBlockLilyPad(Block* pBlock, cBlock* pcBlock, const BlockPos& pos)
{
	// Tessellator var5 = Tessellator.instance;
	AtlasSprite* sprite = getBlockIconFromSide(pcBlock, 1);

	if (hasOverrideBlockTexture())
	{
		sprite = overrideBlockTexture;
	}

	float var7 = 0.015625F;
	float u_min = sprite->getMinU();
	float v_min = sprite->getMinV();
	float u_max = sprite->getMaxU();
	float v_max = sprite->getMaxV();
	i64 var16 = (i64)(pos.x * 3129871) ^ (i64)pos.z * 116129781L ^ (i64)pos.y;
	var16 = var16 * var16 * 42317861L + var16 * 11L;
	int var18 = (int)(var16 >> 16 & 3L);
	
	int bright = pcBlock->getMixedBrightnessForBlock(blockAccess, pos);
	float var19 = pos.x + 0.5F;
	float var20 = pos.z + 0.5F;
	float var21 = (float)(var18 & 1) * 0.5F * (float)(1 - var18 / 2 % 2 * 2);
	float var22 = (float)(var18 + 1 & 1) * 0.5F * (float)(1 - (var18 + 1) / 2 % 2 * 2);

	int color = pcBlock->getBlockColor();
	int color2 = (color & 16711422) >> 1;
	// var5.setColorOpaque_I(pBlock->getBlockColor());
	int pass = pcBlock->getRenderBlockPass();
	TessRenderable* renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite->getTexture(), (TESS_RENDER_PASS)pass, 2);
	TessVertexFmt* pVetex = NULL;
	if (renderable->getLockPtr(2, pVetex))
	{
		pVetex[0].setEx(m_sectionPos, (var19 + var21 - var22), pos.y + var7, (var20 + var21 + var22), color, bright, u_min, v_min);
		pVetex[1].setEx(m_sectionPos, (var19 + var21 + var22), pos.y + var7, (var20 - var21 + var22), color, bright, u_max, v_min);
		pVetex[2].setEx(m_sectionPos, (var19 - var21 + var22), pos.y + var7, (var20 - var21 - var22), color, bright, u_max, v_max);
		pVetex[3].setEx(m_sectionPos, (var19 - var21 - var22), pos.y + var7, (var20 + var21 - var22), color, bright, u_min, v_max);
		//AutoCaculateQuadNormal(pVetex + 0);

		pVetex[4].setEx(m_sectionPos, (var19 - var21 - var22), pos.y + var7, (var20 + var21 - var22), color2, bright, u_min, v_max);
		pVetex[5].setEx(m_sectionPos, (var19 - var21 + var22), pos.y + var7, (var20 - var21 - var22), color2, bright, u_max, v_max);
		pVetex[6].setEx(m_sectionPos, (var19 + var21 + var22), pos.y + var7, (var20 - var21 + var22), color2, bright, u_max, v_min);
		pVetex[7].setEx(m_sectionPos, (var19 + var21 - var22), pos.y + var7, (var20 + var21 + var22), color2, bright, u_min, v_min);
		//AutoCaculateQuadNormal(pVetex + 4);
	}

	return true;
}

bool RenderBlocks::renderBlockFluids(Block* pBlock, cBlock* pcBlock, const BlockPos& pos)
{
	ui32 color = pcBlock->colorMultiplier(blockAccess, pos);
	
	float var7 = (float)(color >> 16 & 255) / 255.0F;
	float var8 = (float)(color >> 8 & 255) / 255.0F;
	float var9 = (float)(color & 255) / 255.0F;
	bool render_top = pBlock->shouldSideBeRendered(blockAccess, pos.getPosY(), 1);
	bool render_bottom = pBlock->shouldSideBeRendered(blockAccess, pos.getNegY(), 0);
	bool render_side[4];
	render_side[0] = pBlock->shouldSideBeRendered(blockAccess, pos.getNegZ(), 2);
	render_side[1] = pBlock->shouldSideBeRendered(blockAccess, pos.getPosZ(), 3);
	render_side[2] = pBlock->shouldSideBeRendered(blockAccess, pos.getNegX(), 4);
	render_side[3] = pBlock->shouldSideBeRendered(blockAccess, pos.getPosX(), 5);

	if (!render_top && !render_bottom && !render_side[0] && !render_side[1] && !render_side[2] && !render_side[3])
	{
		return false;
	}
	else
	{
		bool var13 = false;
		float var14 = 0.5F;
		float var15 = 1.0F;
		float var16 = 0.8F;
		float var17 = 0.6F;
		float var18 = 0.0f;
		float var20 = 1.0f;
		const BM_Material& mat = pBlock->getMaterial();
		int metadata = blockAccess->getBlockMeta(pos);
		float var24 = getFluidHeight(pos, mat);
		float var26 = getFluidHeight(pos.getPosZ(), mat);
		float var28 = getFluidHeight(pos.getSE(), mat);
		float var30 = getFluidHeight(pos.getPosX(), mat);
		float var32 = 0.001f;
		float var52;
		float var53;
		float var54;

		if (renderAllFaces || render_top)
		{
			var13 = true;
			AtlasSprite* sprite = getBlockIconFromSideAndMetadata(pcBlock, 1, metadata);
			float var35 = (float)BlockFluid::getFlowDirection(blockAccess, pos, mat);

			if (var35 > -999.0F)
			{
				sprite = getBlockIconFromSideAndMetadata(pcBlock, 2, metadata);
			}

			var24 -= var32;
			var26 -= var32;
			var28 -= var32;
			var30 -= var32;
			float tx_min;
			float tx_max;
			float ty_min;
			float ty_max;
			float tu_min;
			float tu_max;
			float tv_max;
			float tv_min;

			if (var35 < -999.0F)
			{
				tx_min = sprite->getInterpolatedU(0.0);
				ty_min = sprite->getInterpolatedV(0.0);
				tu_min = tx_min;
				ty_max = sprite->getInterpolatedV(1.0);
				tx_max = sprite->getInterpolatedU(1.0);
				tv_max = ty_max;
				tu_max = tx_max;
				tv_min = ty_min;
			}
			else
			{
				var52 = Math::Sin_tab(var35) * 0.25F;
				var53 = Math::Cos_tab(var35) * 0.25F;
				tx_min = sprite->getInterpolatedU((0.5f + (-var53 - var52) ));
				ty_min = sprite->getInterpolatedV((0.5F + (-var53 + var52) ));
				tu_min = sprite->getInterpolatedU((0.5F + (-var53 + var52) ));
				ty_max = sprite->getInterpolatedV((0.5F + (var53 + var52) ));
				tx_max = sprite->getInterpolatedU((0.5F + (var53 + var52) ));
				tv_max = sprite->getInterpolatedV((0.5F + (var53 - var52) ));
				tu_max = sprite->getInterpolatedU((0.5F + (var53 - var52) ));
				tv_min = sprite->getInterpolatedV((0.5F + (-var53 - var52) ));
			}

			int bright = pcBlock->getMixedBrightnessForBlock(blockAccess, pos);
			
			// todo. lava.
			int pass = pcBlock->getRenderBlockPass();
			if (pass == TRP_TRANSPARENT_UV && !(var24 == var26 && var26 == var28 && var28 == var30))
			{
				pass = TRP_TRANSPARENT_3DUV;
			}

			TessRenderable* renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite->getTexture(), (TESS_RENDER_PASS)pass, 1);
			TessVertexFmt* pVetLockPtr = NULL;
			if (renderable->getLockPtr(1, pVetLockPtr))
			{
				pVetLockPtr[0].setEx(m_sectionPos, pos.x + 0.f, pos.y + var24, pos.z + 0.f, color, bright, tx_min, ty_min);
				pVetLockPtr[1].setEx(m_sectionPos, pos.x + 0.f, pos.y + var26, pos.z + 1.f, color, bright, tu_min, ty_max);
				pVetLockPtr[2].setEx(m_sectionPos, pos.x + 1.f, pos.y + var28, pos.z + 1.f, color, bright, tx_max, tv_max);
				pVetLockPtr[3].setEx(m_sectionPos, pos.x + 1.f, pos.y + var30, pos.z + 0.f, color, bright, tu_max, tv_min);
				//AutoCaculateQuadNormal(pVetLockPtr + 0);
			}
		}

		if (renderAllFaces || render_bottom)
		{
			int bright = pcBlock->getMixedBrightnessForBlock(blockAccess, pos.getNegY());
			float var57 = 1.0F;
			// var5.setColorOpaque_F(var14 * var57, var14 * var57, var14 * var57);
			renderFaceYNeg(pcBlock, Vector3(pos) + Vector3(0.f, var32, 0.f), getBlockIconFromSide(pcBlock, 0));
			var13 = true;
		}

		for (int var59 = 0; var59 < 4; ++var59)
		{
			int var58 = pos.x;
			int var37 = pos.z;

			if (var59 == 0)
			{
				var37 = pos.z - 1;
			}

			if (var59 == 1)
			{
				++var37;
			}

			if (var59 == 2)
			{
				var58 = pos.x - 1;
			}

			if (var59 == 3)
			{
				++var58;
			}

			AtlasSprite* sprite2 = getBlockIconFromSideAndMetadata(pcBlock, var59 + 2, metadata);

			if (renderAllFaces || render_side[var59])
			{
				float var39;
				float var41;
				float var43;
				float var45;
				float var47;
				float var49;

				if (var59 == 0)
				{
					var39 = var24;
					var41 = var30;
					var43 = (float)pos.x;
					var47 = (float)(pos.x + 1);
					var45 = (float)pos.z + var32;
					var49 = (float)pos.z + var32;
				}
				else if (var59 == 1)
				{
					var39 = var28;
					var41 = var26;
					var43 = (float)(pos.x + 1);
					var47 = (float)pos.x;
					var45 = (float)(pos.z + 1) - var32;
					var49 = (float)(pos.z + 1) - var32;
				}
				else if (var59 == 2)
				{
					var39 = var26;
					var41 = var24;
					var43 = (float)pos.x + var32;
					var47 = (float)pos.x + var32;
					var45 = (float)(pos.z + 1);
					var49 = (float)pos.z;
				}
				else
				{
					var39 = var30;
					var41 = var28;
					var43 = (float)(pos.x + 1) - var32;
					var47 = (float)(pos.x + 1) - var32;
					var45 = (float)pos.z;
					var49 = (float)(pos.z + 1);
				}

				var13 = true;
				float var51 = sprite2->getInterpolatedU(0.0);
				var52 = sprite2->getInterpolatedU(0.5f);
				var53 = sprite2->getInterpolatedV((1.0f - var39) * 0.5f);
				var54 = sprite2->getInterpolatedV((1.0f - var41) * 0.5f);
				float var55 = sprite2->getInterpolatedV(0.5f);
				float var56 = 1.0F;

				if (var59 < 2)
				{
					var56 *= var16;
				}
				else
				{
					var56 *= var17;
				}

				int bright = pcBlock->getMixedBrightnessForBlock(blockAccess, BlockPos(var58, pos.y, var37));
				int colorOpaque = Color(var15 * var56 * var7, var15 * var56 * var8, var15 * var56 * var9).getARGB();
				int pass = pcBlock->getRenderBlockPass();
				if (pass == TRP_TRANSPARENT_UV)
				{
					pass = TRP_TRANSPARENT_3DUV;
				}
				TessRenderable* renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite2->getTexture(), (TESS_RENDER_PASS)pass, 1);
				TessVertexFmt* pVetLockPtr = NULL;
				if (renderable->getLockPtr(1, pVetLockPtr))
				{
					pVetLockPtr[0].setEx(m_sectionPos, var43, pos.y + var39, var45, colorOpaque, bright, var51, var53);
					pVetLockPtr[1].setEx(m_sectionPos, var47, pos.y + var41, var49, colorOpaque, bright, var52, var54);
					pVetLockPtr[2].setEx(m_sectionPos, var47, pos.y + 0.f  , var49, colorOpaque, bright, var52, var55);
					pVetLockPtr[3].setEx(m_sectionPos, var43, pos.y + 0.f  , var45, colorOpaque, bright, var51, var55);
					//AutoCaculateQuadNormal(pVetLockPtr + 0);
				}
			}
		}

		renderMinY = var18;
		renderMaxY = var20;
		return var13;
	}
}

bool RenderBlocks::renderBlockStairs(Block* pBlock, cBlock* pcBlock, const BlockPos& pos)
{
	BlockStairs* pBlockStairs = (BlockStairs*)(pBlock);
	pBlockStairs->getCollSlabBouding(blockAccess, pos);
	setRenderBoundsFromBlock(pBlockStairs);
	renderStandardBlock(pBlockStairs, pcBlock, pos);
	bool var5 = pBlockStairs->getCollQuarterBouding(blockAccess, pos);
	setRenderBoundsFromBlock(pBlockStairs);
	renderStandardBlock(pBlockStairs, pcBlock, pos);

	if (var5 && pBlockStairs->getCollEighthBouding(blockAccess, pos))
	{
		setRenderBoundsFromBlock(pBlockStairs);
		renderStandardBlock(pBlockStairs, pcBlock, pos);
	}

	return true;
}

bool RenderBlocks::renderBlockCrops(Block* pBlock, cBlock* pcBlock, const BlockPos& pos)
{
	int bright = pcBlock->getMixedBrightnessForBlock(blockAccess, pos);

	int meta = blockAccess->getBlockMeta(pos);

	renderBlockCropsImpl(pcBlock, bright, meta, Vector3(pos));

	return true;
}

void RenderBlocks::renderBlockCropsImpl(cBlock* pcBlock, int bright, int meta, const Vector3& pos)
{
	int color = 0xFFFFFFFF;
	AtlasSprite* sprite = getBlockIconFromSideAndMetadata(pcBlock, 0, meta);

	if (hasOverrideBlockTexture())
	{
		sprite = overrideBlockTexture;
	}

	float min_u = sprite->getMinU();
	float min_v = sprite->getMinV();
	float max_u = sprite->getMaxU();
	float max_v = sprite->getMaxV();

	int pass = pcBlock->getRenderBlockPass();
	TessRenderable* renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite->getTexture(), (TESS_RENDER_PASS)pass, 8);
	TessVertexFmt* pVetLockPtr = NULL;
	if (!renderable->getLockPtr(8, pVetLockPtr))
		return;


	float min_x = pos.x + 0.5F - 0.25F;
	float max_x = pos.x + 0.5F + 0.25F;
	float min_z = pos.z + 0.5F - 0.5F;
	float max_z = pos.z + 0.5F + 0.5F;

	//negative and positive
	float y = pos.y;
	pVetLockPtr[0].setEx(m_sectionPos, min_x, y + 1.0F, min_z, color, bright, min_u, min_v);
	pVetLockPtr[1].setEx(m_sectionPos, min_x, y + 0.0F, min_z, color, bright, min_u, max_v);
	pVetLockPtr[2].setEx(m_sectionPos, min_x, y + 0.0F, max_z, color, bright, max_u, max_v);
	pVetLockPtr[3].setEx(m_sectionPos, min_x, y + 1.0F, max_z, color, bright, max_u, min_v);
	//AutoCaculateQuadNormal(pVetLockPtr + 0);

	pVetLockPtr[4].setEx(m_sectionPos, min_x, y + 1.0F, max_z, color, bright, min_u, min_v);
	pVetLockPtr[5].setEx(m_sectionPos, min_x, y + 0.0F, max_z, color, bright, min_u, max_v);
	pVetLockPtr[6].setEx(m_sectionPos, min_x, y + 0.0F, min_z, color, bright, max_u, max_v);
	pVetLockPtr[7].setEx(m_sectionPos, min_x, y + 1.0F, min_z, color, bright, max_u, min_v);
	//AutoCaculateQuadNormal(pVetLockPtr + 4);

	pVetLockPtr[8].setEx(m_sectionPos, max_x, y + 1.0F, max_z, color, bright, min_u, min_v);
	pVetLockPtr[9].setEx(m_sectionPos, max_x, y + 0.0F, max_z, color, bright, min_u, max_v);
	pVetLockPtr[10].setEx(m_sectionPos, max_x, y + 0.0F, min_z, color, bright, max_u, max_v);
	pVetLockPtr[11].setEx(m_sectionPos, max_x, y + 1.0F, min_z, color, bright, max_u, min_v);
	//AutoCaculateQuadNormal(pVetLockPtr + 8);

	pVetLockPtr[12].setEx(m_sectionPos, max_x, y + 1.0F, min_z, color, bright, min_u, min_v);
	pVetLockPtr[13].setEx(m_sectionPos, max_x, y + 0.0F, min_z, color, bright, min_u, max_v);
	pVetLockPtr[14].setEx(m_sectionPos, max_x, y + 0.0F, max_z, color, bright, max_u, max_v);
	pVetLockPtr[15].setEx(m_sectionPos, max_x, y + 1.0F, max_z, color, bright, max_u, min_v);
	//AutoCaculateQuadNormal(pVetLockPtr + 12);

	min_x = pos.x + 0.5F - 0.5F;
	max_x = pos.x + 0.5F + 0.5F;
	min_z = pos.z + 0.5F - 0.25F;
	max_z = pos.z + 0.5F + 0.25F;

	pVetLockPtr[16].setEx(m_sectionPos, min_x, y + 1.0F, min_z, color, bright, min_u, min_v);
	pVetLockPtr[17].setEx(m_sectionPos, min_x, y + 0.0F, min_z, color, bright, min_u, max_v);
	pVetLockPtr[18].setEx(m_sectionPos, max_x, y + 0.0F, min_z, color, bright, max_u, max_v);
	pVetLockPtr[19].setEx(m_sectionPos, max_x, y + 1.0F, min_z, color, bright, max_u, min_v);
	//AutoCaculateQuadNormal(pVetLockPtr + 16);

	pVetLockPtr[20].setEx(m_sectionPos, max_x, y + 1.0F, min_z, color, bright, min_u, min_v);
	pVetLockPtr[21].setEx(m_sectionPos, max_x, y + 0.0F, min_z, color, bright, min_u, max_v);
	pVetLockPtr[22].setEx(m_sectionPos, min_x, y + 0.0F, min_z, color, bright, max_u, max_v);
	pVetLockPtr[23].setEx(m_sectionPos, min_x, y + 1.0F, min_z, color, bright, max_u, min_v);
	//AutoCaculateQuadNormal(pVetLockPtr + 20);

	pVetLockPtr[24].setEx(m_sectionPos, max_x, y + 1.0F, max_z, color, bright, min_u, min_v);
	pVetLockPtr[25].setEx(m_sectionPos, max_x, y + 0.0F, max_z, color, bright, min_u, max_v);
	pVetLockPtr[26].setEx(m_sectionPos, min_x, y + 0.0F, max_z, color, bright, max_u, max_v);
	pVetLockPtr[27].setEx(m_sectionPos, min_x, y + 1.0F, max_z, color, bright, max_u, min_v);
	//AutoCaculateQuadNormal(pVetLockPtr + 24);

	pVetLockPtr[28].setEx(m_sectionPos, min_x, y + 1.0F, max_z, color, bright, min_u, min_v);
	pVetLockPtr[29].setEx(m_sectionPos, min_x, y + 0.0F, max_z, color, bright, min_u, max_v);
	pVetLockPtr[30].setEx(m_sectionPos, max_x, y + 0.0F, max_z, color, bright, max_u, max_v);
	pVetLockPtr[31].setEx(m_sectionPos, max_x, y + 1.0F, max_z, color, bright, max_u, min_v);
	//AutoCaculateQuadNormal(pVetLockPtr + 28);
	
	return;
}

void RenderBlocks::renderChest(Block* pBlock, int meta, const Vector3& pos)
{
	if (specialRenderable == NULL)
		return;

	if (pBlock->getBlockID() == BLOCK_ID_ENDER_CHEST || pBlock->getBlockID() == BLOCK_ID_ENDER_CHEST_NEW)
	{
		TileEntityRenderMgr::Instance()->renderTileEntityToTessRender(m_enderChest, pos, 0xFFFFFFFF, specialRenderable);
	}
	else if (pBlock->getBlockID() == BLOCK_ID_CHEST_TRAPPED || pBlock->getBlockID() == BLOCK_ID_CHEST_TRAPPED_NEW)
	{
		TileEntityRenderMgr::Instance()->renderTileEntityToTessRender(m_Trapped, pos, 0xFFFFFFFF, specialRenderable);
	}
	else
		TileEntityRenderMgr::Instance()->renderTileEntityToTessRender(m_chest, pos, 0xFFFFFFFF, specialRenderable);
}

void RenderBlocks::renderBlockFruitImpl(cBlock* pcBlock, int bright, int meta, const Vector3& pos)
{
	int color = 0xFFFFFFFF;
	AtlasSprite* spriteTop = getBlockIconFromSideAndMetadata(pcBlock, 1, meta);
	AtlasSprite* sprite = getBlockIconFromSideAndMetadata(pcBlock, 0, meta);
	float maxMeta = (float) pcBlock->getMaxMeta();
	float scale = Math::Clamp(maxMeta - meta, 0.0F, maxMeta);

	if (hasOverrideBlockTexture())
	{
		sprite = overrideBlockTexture;
	}

	float min_u = sprite->getMinU();
	float min_v = sprite->getMinV();
	float max_u = sprite->getMaxU();
	float max_v = sprite->getMaxV();
	float max_v_scale = scale / maxMeta * max_v + (1.0F - scale / maxMeta) * min_v;

	if (max_v_scale != min_v)
	{
		int pass = pcBlock->getRenderBlockPass();
		TessRenderable* renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite->getTexture(), (TESS_RENDER_PASS)pass, 5);
		TessVertexFmt* pVetLockPtr = NULL;
		if (!renderable->getLockPtr(5, pVetLockPtr))
			return;

		float min_x = pos.x + 0.5F - 0.5F;
		float max_x = pos.x + 0.5F + 0.5F;
		float min_z = pos.z + 0.5F - 0.5F;
		float max_z = pos.z + 0.5F + 0.5F;
		float min_y = pos.y;
		float max_y = min_y + 0.72F / maxMeta * scale;

		// side
		pVetLockPtr[0].setEx(m_sectionPos, min_x, max_y, min_z, color, bright, min_u, min_v);
		pVetLockPtr[1].setEx(m_sectionPos, min_x, min_y, min_z, color, bright, min_u, max_v_scale);
		pVetLockPtr[2].setEx(m_sectionPos, min_x, min_y, max_z, color, bright, max_u, max_v_scale);
		pVetLockPtr[3].setEx(m_sectionPos, min_x, max_y, max_z, color, bright, max_u, min_v);

		pVetLockPtr[4].setEx(m_sectionPos, max_x, max_y, max_z, color, bright, min_u, min_v);
		pVetLockPtr[5].setEx(m_sectionPos, max_x, min_y, max_z, color, bright, min_u, max_v_scale);
		pVetLockPtr[6].setEx(m_sectionPos, max_x, min_y, min_z, color, bright, max_u, max_v_scale);
		pVetLockPtr[7].setEx(m_sectionPos, max_x, max_y, min_z, color, bright, max_u, min_v);

		pVetLockPtr[8].setEx(m_sectionPos, max_x, max_y, min_z, color, bright, min_u, min_v);
		pVetLockPtr[9].setEx(m_sectionPos, max_x, min_y, min_z, color, bright, min_u, max_v_scale);
		pVetLockPtr[10].setEx(m_sectionPos, min_x, min_y, min_z, color, bright, max_u, max_v_scale);
		pVetLockPtr[11].setEx(m_sectionPos, min_x, max_y, min_z, color, bright, max_u, min_v);

		pVetLockPtr[12].setEx(m_sectionPos, min_x, max_y, max_z, color, bright, min_u, min_v);
		pVetLockPtr[13].setEx(m_sectionPos, min_x, min_y, max_z, color, bright, min_u, max_v_scale);
		pVetLockPtr[14].setEx(m_sectionPos, max_x, min_y, max_z, color, bright, max_u, max_v_scale);
		pVetLockPtr[15].setEx(m_sectionPos, max_x, max_y, max_z, color, bright, max_u, min_v);

		// bottom
		pVetLockPtr[16].setEx(m_sectionPos, min_x, min_y, max_z, color, bright, min_u, max_v);
		pVetLockPtr[17].setEx(m_sectionPos, min_x, min_y, min_z, color, bright, min_u, min_v);
		pVetLockPtr[18].setEx(m_sectionPos, max_x, min_y, min_z, color, bright, max_u, min_v);
		pVetLockPtr[19].setEx(m_sectionPos, max_x, min_y, max_z, color, bright, max_u, max_v);

		min_u = spriteTop->getMinU();
		min_v = spriteTop->getMinV();
		max_u = spriteTop->getMaxU();
		max_v = spriteTop->getMaxV();
		pass = pcBlock->getRenderBlockPass();
		renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(spriteTop->getTexture(), (TESS_RENDER_PASS)pass, 1);
		pVetLockPtr = NULL;
		if (!renderable->getLockPtr(1, pVetLockPtr))
			return;

		// top
		pVetLockPtr[0].setEx(m_sectionPos, min_x, max_y, min_z, color, bright, min_u, min_v);
		pVetLockPtr[1].setEx(m_sectionPos, min_x, max_y, max_z, color, bright, min_u, max_v);
		pVetLockPtr[2].setEx(m_sectionPos, max_x, max_y, max_z, color, bright, max_u, max_v);
		pVetLockPtr[3].setEx(m_sectionPos, max_x, max_y, min_z, color, bright, max_u, min_v);
	}

	return;
}

bool RenderBlocks::renderBlockStem(Block* pBlock, cBlock* pcBlock, const BlockPos& pos)
{	
	cBlockStem* pBlockStem = (cBlockStem*)(pBlock);
	int bright = pBlockStem->getMixedBrightnessForBlock(blockAccess, pos);
	float opaque = 1.0F;
	ui32 rgb = pBlockStem->colorMultiplier(blockAccess, pos);
	float r = (float)(rgb >> 16 & 255) / 255.0F;
	float g = (float)(rgb >> 8 & 255) / 255.0F;
	float b = (float)(rgb & 255) / 255.0F;

	//if (EntityRenderer.anaglyphEnable	
	{
		float tr = (r * 30.0F + g * 59.0F + b * 11.0F) / 100.0F;
		float tg = (r * 30.0F + g * 70.0F) / 100.0F;
		float tb = (r * 30.0F + b * 70.0F) / 100.0F;
		r = tr;
		g = tg;
		b = tb;
	}
	
	int color = Color(r, g, b).getARGB();
	pBlockStem->setBlockBoundsBasedOnState(blockAccess, pos);
	int state = pBlockStem->getState(blockAccess, pos);
	Vector3 yoff(0.f, 0.0625f, 0.f);

	if (state < 0)
	{
		renderBlockStemSmall(pBlockStem, pcBlock, blockAccess->getBlockMeta(pos), renderMaxY, Vector3(pos) - yoff, color, bright);
	}
	else
	{
		renderBlockStemSmall(pBlockStem, pcBlock, blockAccess->getBlockMeta(pos), 0.5f, Vector3(pos) - yoff, color, bright);
		renderBlockStemBig(pBlockStem, pcBlock, blockAccess->getBlockMeta(pos), state, renderMaxY, Vector3(pos) - yoff, color, bright);
	}

	return true;
}

bool RenderBlocks::renderBlockDoor(Block* pBlock, cBlock* pcBlock, const BlockPos& pos)
{	
	int metadata = blockAccess->getBlockMeta(pos);

	if ((metadata & 8) != 0)
	{
		if (blockAccess->getBlockId(pos.getNegY()) != pBlock->getBlockID())
		{
			//return false;
		}
	}
	else if (blockAccess->getBlockId(pos.getPosY()) != pBlock->getBlockID())
	{
		//return false;
	}

	bool var7 = false;
	float var8 = 0.5F;
	float var9 = 1.0F;
	float var10 = 0.8F;
	float var11 = 0.6F;
	int bright = pcBlock->getMixedBrightnessForBlock(blockAccess, pos);
	AtlasSprite* sprite = getBlockIcon(pcBlock, blockAccess, pos, 0);
	setBrightness(renderMinY > 0.0 ? bright : pcBlock->getMixedBrightnessForBlock(blockAccess, pos.getNegY()));
	setColor(var8, var8, var8)	;
	renderFaceYNeg(pcBlock, Vector3(pos), sprite);
	var7 = true;
	setBrightness(renderMaxY < 1.0 ? bright : pcBlock->getMixedBrightnessForBlock(blockAccess, pos.getPosY()));
	setColor(var9, var9, var9);
	sprite = getBlockIcon(pcBlock, blockAccess, pos, 1);
	renderFaceYPos(pcBlock, Vector3(pos), sprite);

	var7 = true;
	setBrightness(renderMinZ > 0.0 ? bright : pcBlock->getMixedBrightnessForBlock(blockAccess, pos.getNegZ()));
	setColor(var10, var10, var10);
	sprite= getBlockIcon(pcBlock, blockAccess, pos, 2);
	renderFaceZNeg(pcBlock, Vector3(pos), sprite);
	var7 = true;
	flipTexture = false;
	setBrightness(renderMaxZ < 1.0 ? bright : pcBlock->getMixedBrightnessForBlock(blockAccess, pos.getPosZ()));
	setColor(var10, var10, var10);
	sprite = getBlockIcon(pcBlock, blockAccess, pos, 3);
	renderFaceZPos(pcBlock, Vector3(pos), sprite);

	var7 = true;
	flipTexture = false;
	setBrightness(renderMinX > 0.0 ? bright : pcBlock->getMixedBrightnessForBlock(blockAccess, pos.getNegX()));
	setColor(var11, var11, var11);
	sprite = getBlockIcon(pcBlock, blockAccess, pos, 4);
	renderFaceXNeg(pcBlock, Vector3(pos), sprite);
	var7 = true;
	flipTexture = false;
	setBrightness(renderMaxX < 1.0 ? bright : pcBlock->getMixedBrightnessForBlock(blockAccess, pos.getPosX()));
	setColor(var11, var11, var11);
	sprite = getBlockIcon(pcBlock, blockAccess, pos, 5);
	renderFaceXPos(pcBlock, Vector3(pos), sprite);
	var7 = true;
	flipTexture = false;
	return var7;
}

bool RenderBlocks::renderBlockLadder(Block* pBlock, cBlock* pcBlock, const BlockPos& pos)
{
	
	AtlasSprite* sprite = getBlockIconFromSide(pcBlock, 0);

	if (hasOverrideBlockTexture())
	{
		sprite = overrideBlockTexture;
	}

	int bright = pcBlock->getMixedBrightnessForBlock(blockAccess, pos);
	float var7 = 1.0F;
	int color = Color(var7, var7, var7).getARGB();
	float min_u = sprite->getMinU();
	float min_v = sprite->getMinV();
	float max_u = sprite->getMaxU();
	float max_v = sprite->getMaxV();
	int metadata = blockAccess->getBlockMeta(pos);
	float var16 = 0.0F;
	float var18 = 0.05000000074505806F;

	int pass = pcBlock->getRenderBlockPass();
	TessRenderable* renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite->getTexture(), (TESS_RENDER_PASS)pass, 1);
	TessVertexFmt* pVetLockPtr = NULL;
	if (!renderable->getLockPtr(1, pVetLockPtr))
		return false;

	float y0 = pos.y - var16;
	float y1 = pos.y + 1.f + var16;
	float z0 = pos.z - var16;
	float z1 = pos.z + 1.f + var16;
	float x0 = pos.x - var16;
	float x1 = pos.x + 1.f + var16;

	if (metadata == 5)
	{
		float x = pos.x + var18;
		pVetLockPtr[0].setEx(m_sectionPos, x, y1, z1, color, bright, min_u, min_v);
		pVetLockPtr[1].setEx(m_sectionPos, x, y0, z1, color, bright, min_u, max_v);
		pVetLockPtr[2].setEx(m_sectionPos, x, y0, z0, color, bright, max_u, max_v);
		pVetLockPtr[3].setEx(m_sectionPos, x, y1, z0, color, bright, max_u, min_v);
		//AutoCaculateQuadNormal(pVetLockPtr + 0);
	}
	else if (metadata == 4)
	{		
		float x = pos.x  + 1.f - var18;
		pVetLockPtr[0].setEx(m_sectionPos, x, y0, z1, color, bright, max_u, max_v);
		pVetLockPtr[1].setEx(m_sectionPos, x, y1, z1, color, bright, max_u, min_v);
		pVetLockPtr[2].setEx(m_sectionPos, x, y1, z0, color, bright, min_u, min_v);
		pVetLockPtr[3].setEx(m_sectionPos, x, y0, z0, color, bright, min_u, max_v);
		//AutoCaculateQuadNormal(pVetLockPtr + 0);
	}
	else if (metadata == 3)
	{
		float z = pos.z + var18;
		pVetLockPtr[0].setEx(m_sectionPos, x1, y0, z, color, bright, max_u, max_v);
		pVetLockPtr[1].setEx(m_sectionPos, x1, y1, z, color, bright, max_u, min_v);
		pVetLockPtr[2].setEx(m_sectionPos, x0, y1, z, color, bright, min_u, min_v);
		pVetLockPtr[3].setEx(m_sectionPos, x0, y0, z, color, bright, min_u, max_v);
		//AutoCaculateQuadNormal(pVetLockPtr + 0);
	}
	else if (metadata == 0)
	{
		float z = pos.z +1.f - var18;
		pVetLockPtr[0].setEx(m_sectionPos, x1, y1, z, color, bright, min_u, min_v);
		pVetLockPtr[1].setEx(m_sectionPos, x1, y0, z, color, bright, min_u, max_v);
		pVetLockPtr[2].setEx(m_sectionPos, x0, y0, z, color, bright, max_u, max_v);
		pVetLockPtr[3].setEx(m_sectionPos, x0, y1, z, color, bright, max_u, min_v);
		//AutoCaculateQuadNormal(pVetLockPtr + 0);
	}

	return true;
}

bool RenderBlocks::renderBlockCocoa(Block* pBlock, cBlock* pcBlock, const BlockPos& pos)
{
	
	cBlockCocoa* pBlockCocoa = (cBlockCocoa*)(pBlock);
	int bright = pBlockCocoa->getMixedBrightnessForBlock(blockAccess, pos);
	int color = Color(1.0F, 1.0F, 1.0F).getARGB();
	int metadata = blockAccess->getBlockMeta(pos);
	int direction = BlockCocoa::getDirection(metadata);
	int age = BlockCocoa::getAgeFromMeta(metadata);
	AtlasSprite* sprite = pBlockCocoa->getItonByAge(age);
	int var10 = 4 + age * 2;
	int var11 = 5 + age * 2;
	float var12 = 15.0F - var10;
	float var14 = 15.0F;
	float var16 = 4.0F;
	float var18 = 4.0F + var11;
	float var20 = sprite->getInterpolatedU(var12 / 16.0F);
	float var22 = sprite->getInterpolatedU(var14 / 16.0F);
	float var24 = sprite->getInterpolatedV(var16 / 16.0F);
	float var26 = sprite->getInterpolatedV(var18 / 16.0F);
	float var28 = 0.0;
	float var30 = 0.0;

	switch (direction)
	{
	case 0:
		var28 = 8.0F - (var10 / 2);
		var30 = 15.0F - var10;
		break;

	case 1:
		var28 = 1.0F;
		var30 = 8.0F - (var10 / 2);
		break;

	case 2:
		var28 = 8.0F - (var10 / 2);
		var30 = 1.0F;
		break;

	case 3:
		var28 = 15.0F - var10;
		var30 = 8.0F - (var10 / 2);
	}

	float var32 = pos.x + var28 / 16.0F;
	float var34 = pos.x + (var28 + var10) / 16.0F;
	float var36 = pos.y + (12.0F - var11) / 16.0F;
	float var38 = pos.y + 0.75F;
	float var40 = pos.z + var30 / 16.0F;
	float var42 = pos.z + (var30 + var10) / 16.0F;

	int pass = pBlockCocoa->getRenderBlockPass();
	TessRenderable* renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite->getTexture(), (TESS_RENDER_PASS)pass, 4);
	TessVertexFmt* pVetLockPtr = NULL;
	if(!renderable->getLockPtr(4, pVetLockPtr))
		return false;

	

	pVetLockPtr[0].setEx(m_sectionPos, var32, var36, var40, color, bright, var20, var26);
	pVetLockPtr[1].setEx(m_sectionPos, var32, var36, var42, color, bright, var22, var26);
	pVetLockPtr[2].setEx(m_sectionPos, var32, var38, var42, color, bright, var22, var24);
	pVetLockPtr[3].setEx(m_sectionPos, var32, var38, var40, color, bright, var20, var24);
	//AutoCaculateQuadNormal(pVetLockPtr + 0);

	pVetLockPtr[4].setEx(m_sectionPos, var34, var36, var42, color, bright, var20, var26);
	pVetLockPtr[5].setEx(m_sectionPos, var34, var36, var40, color, bright, var22, var26);
	pVetLockPtr[6].setEx(m_sectionPos, var34, var38, var40, color, bright, var22, var24);
	pVetLockPtr[7].setEx(m_sectionPos, var34, var38, var42, color, bright, var20, var24);
	//AutoCaculateQuadNormal(pVetLockPtr + 4);

	pVetLockPtr[8].setEx(m_sectionPos, var34, var36, var40, color, bright, var20, var26);
	pVetLockPtr[9].setEx(m_sectionPos, var32, var36, var40, color, bright, var22, var26);
	pVetLockPtr[10].setEx(m_sectionPos, var32, var38, var40, color, bright, var22, var24);
	pVetLockPtr[11].setEx(m_sectionPos, var34, var38, var40, color, bright, var20, var24);
	//AutoCaculateQuadNormal(pVetLockPtr + 8);

	pVetLockPtr[12].setEx(m_sectionPos, var32, var36, var42, color, bright, var20, var26);
	pVetLockPtr[13].setEx(m_sectionPos, var34, var36, var42, color, bright, var22, var26);
	pVetLockPtr[14].setEx(m_sectionPos, var34, var38, var42, color, bright, var22, var24);
	pVetLockPtr[15].setEx(m_sectionPos, var32, var38, var42, color, bright, var20, var24);
	//AutoCaculateQuadNormal(pVetLockPtr + 12);
	int var44 = var10;

	if (age >= 2)
	{
		var44 = var10 - 1;
	}

	var20 = sprite->getMinU();
	var22 = sprite->getInterpolatedU(var44 / 16.0F);
	var24 = sprite->getMinV();
	var26 = sprite->getInterpolatedV(var44 / 16.0F);

	renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite->getTexture(), (TESS_RENDER_PASS)pass, 2);
	pVetLockPtr = NULL;
	if(!renderable->getLockPtr(2, pVetLockPtr))
		return false;

	pVetLockPtr[0].setEx(m_sectionPos, var32, var38, var42, color, bright, var20, var26);
	pVetLockPtr[1].setEx(m_sectionPos, var34, var38, var42, color, bright, var22, var26);
	pVetLockPtr[2].setEx(m_sectionPos, var34, var38, var40, color, bright, var22, var24);
	pVetLockPtr[3].setEx(m_sectionPos, var32, var38, var40, color, bright, var20, var24);
	//AutoCaculateQuadNormal(pVetLockPtr + 0);

	pVetLockPtr[4].setEx(m_sectionPos, var32, var36, var40, color, bright, var20, var24);
	pVetLockPtr[5].setEx(m_sectionPos, var34, var36, var40, color, bright, var22, var24);
	pVetLockPtr[6].setEx(m_sectionPos, var34, var36, var42, color, bright, var22, var26);
	pVetLockPtr[7].setEx(m_sectionPos, var32, var36, var42, color, bright, var20, var26);
	//AutoCaculateQuadNormal(pVetLockPtr + 4);

	var20 = sprite->getInterpolatedU(12.0F / 16.0F);
	var22 = sprite->getMaxU();
	var24 = sprite->getMinV();
	var26 = sprite->getInterpolatedV(4.0F / 16.0F);
	var28 = 8.0;
	var30 = 0.0;
	float var45;

	switch (direction)
	{
	case 0:
		var28 = 8.0;
		var30 = 12.0;
		var45 = var20;
		var20 = var22;
		var22 = var45;
		break;

	case 1:
		var28 = 0.0;
		var30 = 8.0;
		break;

	case 2:
		var28 = 8.0;
		var30 = 0.0;
		break;

	case 3:
		var28 = 12.0;
		var30 = 8.0;
		var45 = var20;
		var20 = var22;
		var22 = var45;
	}

	var32 = pos.x + var28 / 16.0F;
	var34 = pos.x + (var28 + 4.0F) / 16.0F;
	var36 = pos.y + 0.75F;
	var38 = pos.y + 1.0F;
	var40 = pos.z + var30 / 16.0F;
	var42 = pos.z + (var30 + 4.0F) / 16.0F;

	if (direction != 2 && direction != 0)
	{
		if (direction == 1 || direction == 3)
		{
			renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite->getTexture(), (TESS_RENDER_PASS)pass, 2);
			pVetLockPtr = NULL;
			if (!renderable->getLockPtr(2, pVetLockPtr))
				return false;

			pVetLockPtr[0].setEx(m_sectionPos, var34, var36, var40, color, bright, var20, var26);
			pVetLockPtr[1].setEx(m_sectionPos, var32, var36, var40, color, bright, var22, var26);
			pVetLockPtr[2].setEx(m_sectionPos, var32, var38, var40, color, bright, var22, var24);
			pVetLockPtr[3].setEx(m_sectionPos, var34, var38, var40, color, bright, var20, var24);
			//AutoCaculateQuadNormal(pVetLockPtr + 0);

			pVetLockPtr[4].setEx(m_sectionPos, var32, var36, var40, color, bright, var22, var26);
			pVetLockPtr[5].setEx(m_sectionPos, var34, var36, var40, color, bright, var20, var26);
			pVetLockPtr[6].setEx(m_sectionPos, var34, var38, var40, color, bright, var20, var24);
			pVetLockPtr[7].setEx(m_sectionPos, var32, var38, var40, color, bright, var22, var24);
			//AutoCaculateQuadNormal(pVetLockPtr + 4);
		}
	}
	else
	{
		renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite->getTexture(), (TESS_RENDER_PASS)pass, 2);
		pVetLockPtr = NULL;
		if (!renderable->getLockPtr(2, pVetLockPtr))
			return false;

		pVetLockPtr[0].setEx(m_sectionPos, var32, var36, var40, color, bright, var22, var26);
		pVetLockPtr[1].setEx(m_sectionPos, var32, var36, var42, color, bright, var20, var26);
		pVetLockPtr[2].setEx(m_sectionPos, var32, var38, var42, color, bright, var20, var24);
		pVetLockPtr[3].setEx(m_sectionPos, var32, var38, var40, color, bright, var22, var24);
		//AutoCaculateQuadNormal(pVetLockPtr + 0);

		pVetLockPtr[4].setEx(m_sectionPos, var32, var36, var42, color, bright, var20, var26);
		pVetLockPtr[5].setEx(m_sectionPos, var32, var36, var40, color, bright, var22, var26);
		pVetLockPtr[6].setEx(m_sectionPos, var32, var38, var40, color, bright, var22, var24);
		pVetLockPtr[7].setEx(m_sectionPos, var32, var38, var42, color, bright, var20, var24);
		//AutoCaculateQuadNormal(pVetLockPtr + 4);
	}

	return true;
}

bool RenderBlocks::renderBlockFence(Block* pBlock, cBlock* pcBlock, const BlockPos& pos)
{	
	BlockFence* pBlockFence = (BlockFence*)(pBlock);
	bool var5 = false;
	float var6 = 0.375F;
	float var7 = 0.625F;
	setRenderBounds(var6, 0.0F, var6, var7, 1.0F, var7);
	renderStandardBlock(pBlockFence, pcBlock, pos);
	var5 = true;
	bool var8 = false;
	bool var9 = false;

	if (pBlockFence->canConnectFenceTo(blockAccess, pos.getNegX()) || pBlockFence->canConnectFenceTo(blockAccess, pos.getPosX()))
	{
		var8 = true;
	}

	if (pBlockFence->canConnectFenceTo(blockAccess, pos.getNegZ()) || pBlockFence->canConnectFenceTo(blockAccess, pos.getPosZ()))
	{
		var9 = true;
	}

	bool var10 = pBlockFence->canConnectFenceTo(blockAccess, pos.getNegX());
	bool var11 = pBlockFence->canConnectFenceTo(blockAccess, pos.getPosX());
	bool var12 = pBlockFence->canConnectFenceTo(blockAccess, pos.getNegZ());
	bool var13 = pBlockFence->canConnectFenceTo(blockAccess, pos.getPosZ());

	if (!var8 && !var9)
	{
		var8 = true;
	}

	var6 = 0.4375F;
	var7 = 0.5625F;
	float var14 = 0.75F;
	float var15 = 0.9375F;
	float var16 = var10 ? 0.0F : var6;
	float var17 = var11 ? 1.0F : var7;
	float var18 = var12 ? 0.0F : var6;
	float var19 = var13 ? 1.0F : var7;

	if (var8)
	{
		setRenderBounds(var16, var14, var6, var17, var15, var7);
		renderStandardBlock(pBlockFence, pcBlock, pos);
		var5 = true;
	}

	if (var9)
	{
		setRenderBounds(var6, var14, var18, var7, var15, var19);
		renderStandardBlock(pBlockFence, pcBlock, pos);
		var5 = true;
	}

	var14 = 0.375F;
	var15 = 0.5625F;

	if (var8)
	{
		setRenderBounds(var16, var14, var6, var17, var15, var7);
		renderStandardBlock(pBlockFence, pcBlock, pos);
		var5 = true;
	}

	if (var9)
	{
		setRenderBounds(var6, var14, var18, var7, var15, var19);
		renderStandardBlock(pBlockFence, pcBlock, pos);
		var5 = true;
	}

	pBlockFence->setBlockBoundsBasedOnState(blockAccess, pos);
	return var5;
}

bool RenderBlocks::renderBlockPane(Block* pBlock, cBlock* pcBlock, const BlockPos& pos)
{
	cBlockPane* pBlockPane = (cBlockPane*)(pBlock);
	int height = blockAccess->getHeight();
	
	int bright = pBlockPane->getMixedBrightnessForBlock(blockAccess, pos);
	float var7 = 1.0F;
	ui32 var8 = pBlockPane->colorMultiplier(blockAccess, pos);
	float var9 = (float)(var8 >> 16 & 255) / 255.0F;
	float var10 = (float)(var8 >> 8 & 255) / 255.0F;
	float var11 = (float)(var8 & 255) / 255.0F;

	//if (EntityRenderer.anaglyphEnable)
	{
		float var12 = (var9 * 30.0F + var10 * 59.0F + var11 * 11.0F) / 100.0F;
		float var13 = (var9 * 30.0F + var10 * 70.0F) / 100.0F;
		float var14 = (var9 * 30.0F + var11 * 70.0F) / 100.0F;
		var9 = var12;
		var10 = var13;
		var11 = var14;
	}

	int color = Color(var7 * var9, var7 * var10, var7 * var11).getARGB();
	AtlasSprite* sprite1;
	AtlasSprite* sprite2;

	if (hasOverrideBlockTexture())
	{
		sprite1 = overrideBlockTexture;
		sprite2 = overrideBlockTexture;
	}
	else
	{
		int var66 = blockAccess->getBlockMeta(pos);
		sprite1 = getBlockIconFromSideAndMetadata(pBlockPane, 0, var66);
		sprite2 = getBlockIconFromSideAndMetadata(pBlockPane, 1, var66);
	}

	float min_u1 = sprite1->getMinU();
	float var16 = sprite1->getInterpolatedU(8.0F / 16.0F);
	float max_u1 = sprite1->getMaxU();
	float min_v1 = sprite1->getMinV();
	float max_v1 = sprite1->getMaxV();
	float var24 = sprite2->getInterpolatedU(7.0F / 16.0F);
	float var26 = sprite2->getInterpolatedU(9.0F / 16.0F);
	float min_v2 = sprite2->getMinV();
	float var30 = sprite2->getInterpolatedV(8.0F / 16.0F);
	float max_v2 = sprite2->getMaxV();
	float var34 = pos.x + 0.0F;
	float var36 = pos.x + 0.5F;
	float var38 = pos.x + 1.0F;
	float var40 = pos.z + 0.0F;
	float var42 = pos.z + 0.5F;
	float var44 = pos.z + 1.0F;
	float var46 = pos.x + 0.5F - 0.0625F;
	float var48 = pos.x + 0.5F + 0.0625F;
	float var50 = pos.z + 0.5F - 0.0625F;
	float var52 = pos.z + 0.5F + 0.0625F;
	bool canConnectZNeg = pBlockPane->canThisPaneConnectToThisBlockID(blockAccess->getBlockId(pos.getNegZ()));
	bool canConnectZPos = pBlockPane->canThisPaneConnectToThisBlockID(blockAccess->getBlockId(pos.getPosZ()));
	bool canConnectXNeg = pBlockPane->canThisPaneConnectToThisBlockID(blockAccess->getBlockId(pos.getNegX()));
	bool canConnectXPos = pBlockPane->canThisPaneConnectToThisBlockID(blockAccess->getBlockId(pos.getPosX()));
	bool shouldRenderYPos = pBlockPane->shouldSideBeRendered(blockAccess, pos.getPosY(), 1);
	bool shouldRenderYNeg = pBlockPane->shouldSideBeRendered(blockAccess, pos.getNegY(), 0);
	float var60 = 0.01F;
	float var62 = 0.005F;
	float y0 = pos.y + 0.f;
	float y1 = pos.y + 1.f;

	if ((!canConnectXNeg || !canConnectXPos) && (canConnectXNeg || canConnectXPos || canConnectZNeg || canConnectZPos))
	{
		if (canConnectXNeg && !canConnectXPos)
		{
			int pass = pBlockPane->getRenderBlockPass();
			TessRenderable* renderrable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite1->getTexture(), (TESS_RENDER_PASS)pass, 2);
			TessVertexFmt* pVetLockPtr = NULL;
			if(!renderrable->getLockPtr(2, pVetLockPtr))
				return false;
			
			pVetLockPtr[0].setEx(m_sectionPos, var34, y1, var42, color, bright, min_u1, min_v1);
			pVetLockPtr[1].setEx(m_sectionPos, var34, y0, var42, color, bright, min_u1, max_v1);
			pVetLockPtr[2].setEx(m_sectionPos, var36, y0, var42, color, bright, var16, max_v1);
			pVetLockPtr[3].setEx(m_sectionPos, var36, y1, var42, color, bright, var16, min_v1);
			//AutoCaculateQuadNormal(pVetLockPtr + 0);

			pVetLockPtr[4].setEx(m_sectionPos, var36, y1, var42, color, bright, min_u1, min_v1);
			pVetLockPtr[5].setEx(m_sectionPos, var36, y0, var42, color, bright, min_u1, max_v1);
			pVetLockPtr[6].setEx(m_sectionPos, var34, y0, var42, color, bright, var16, max_v1);
			pVetLockPtr[7].setEx(m_sectionPos, var34, y1, var42, color, bright, var16, min_v1);
			//AutoCaculateQuadNormal(pVetLockPtr + 4);

			if (!canConnectZPos && !canConnectZNeg)
			{
				renderrable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite2->getTexture(), (TESS_RENDER_PASS)pass, 2);
				if(!renderrable->getLockPtr(2, pVetLockPtr))
					return false;

				pVetLockPtr[0].setEx(m_sectionPos, var36, y1, var52, color, bright, var24, min_v2);
				pVetLockPtr[1].setEx(m_sectionPos, var36, y0, var52, color, bright, var24, max_v2);
				pVetLockPtr[2].setEx(m_sectionPos, var36, y0, var50, color, bright, var26, max_v2);
				pVetLockPtr[3].setEx(m_sectionPos, var36, y1, var50, color, bright, var26, min_v2);
				//AutoCaculateQuadNormal(pVetLockPtr + 0);

				pVetLockPtr[4].setEx(m_sectionPos, var36, y1, var50, color, bright, var24, min_v2);
				pVetLockPtr[5].setEx(m_sectionPos, var36, y0, var50, color, bright, var24, max_v2);
				pVetLockPtr[6].setEx(m_sectionPos, var36, y0, var52, color, bright, var26, max_v2);
				pVetLockPtr[7].setEx(m_sectionPos, var36, y1, var52, color, bright, var26, min_v2);
				//AutoCaculateQuadNormal(pVetLockPtr + 4);
			}

			if (shouldRenderYPos || pos.y < height - 1 && blockAccess->isAirBlock(pos.getUW()))
			{
				renderrable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite2->getTexture(), (TESS_RENDER_PASS)pass, 2);
				if (!renderrable->getLockPtr(2, pVetLockPtr))
					return false;

				float y = pos.y + 1.f - 0.01f;
				pVetLockPtr[0].setEx(m_sectionPos, var34, y, var52, color, bright, var26, var30);
				pVetLockPtr[1].setEx(m_sectionPos, var36, y, var52, color, bright, var26, max_v2);
				pVetLockPtr[2].setEx(m_sectionPos, var36, y, var50, color, bright, var24, max_v2);
				pVetLockPtr[3].setEx(m_sectionPos, var34, y, var50, color, bright, var24, var30);
				//AutoCaculateQuadNormal(pVetLockPtr + 0);

				pVetLockPtr[4].setEx(m_sectionPos, var36, y, var52, color, bright, var26, var30);
				pVetLockPtr[5].setEx(m_sectionPos, var34, y, var52, color, bright, var26, max_v2);
				pVetLockPtr[6].setEx(m_sectionPos, var34, y, var50, color, bright, var24, max_v2);
				pVetLockPtr[7].setEx(m_sectionPos, var36, y, var50, color, bright, var24, var30);
				//AutoCaculateQuadNormal(pVetLockPtr + 4);
			}

			if (shouldRenderYNeg || pos.y > 1 && blockAccess->isAirBlock(pos.getDW()))
			{
				renderrable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite2->getTexture(), (TESS_RENDER_PASS)pass, 2);
				if (!renderrable->getLockPtr(2, pVetLockPtr))
					return false;

				float y = pos.y - 0.01f;
				pVetLockPtr[0].setEx(m_sectionPos, var34, y, var52, color, bright, var26, var30);
				pVetLockPtr[1].setEx(m_sectionPos, var36, y, var52, color, bright, var26, max_v2);
				pVetLockPtr[2].setEx(m_sectionPos, var36, y, var50, color, bright, var24, max_v2);
				pVetLockPtr[3].setEx(m_sectionPos, var34, y, var50, color, bright, var24, var30);
				//AutoCaculateQuadNormal(pVetLockPtr + 0);

				pVetLockPtr[4].setEx(m_sectionPos, var36, y, var52, color, bright, var26, var30);
				pVetLockPtr[5].setEx(m_sectionPos, var34, y, var52, color, bright, var26, max_v2);
				pVetLockPtr[6].setEx(m_sectionPos, var34, y, var50, color, bright, var24, max_v2);
				pVetLockPtr[7].setEx(m_sectionPos, var36, y, var50, color, bright, var24, var30);
				//AutoCaculateQuadNormal(pVetLockPtr + 4);
			}
		}
		else if (!canConnectXNeg && canConnectXPos)
		{
			int pass = pBlockPane->getRenderBlockPass();
			TessRenderable* renderrable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite1->getTexture(), (TESS_RENDER_PASS)pass, 2);
			TessVertexFmt* pVetLockPtr = NULL;
			if (!renderrable->getLockPtr(2, pVetLockPtr))
				return false;

			pVetLockPtr[0].setEx(m_sectionPos, var36, y1, var42, color, bright, var16, min_v1);
			pVetLockPtr[1].setEx(m_sectionPos, var36, y0, var42, color, bright, var16, max_v1);
			pVetLockPtr[2].setEx(m_sectionPos, var38, y0, var42, color, bright, max_u1, max_v1);
			pVetLockPtr[3].setEx(m_sectionPos, var38, y1, var42, color, bright, max_u1, min_v1);
			//AutoCaculateQuadNormal(pVetLockPtr + 0);

			pVetLockPtr[4].setEx(m_sectionPos, var38, y1, var42, color, bright, var16, min_v1);
			pVetLockPtr[5].setEx(m_sectionPos, var38, y0, var42, color, bright, var16, max_v1);
			pVetLockPtr[6].setEx(m_sectionPos, var36, y0, var42, color, bright, max_u1, max_v1);
			pVetLockPtr[7].setEx(m_sectionPos, var36, y1, var42, color, bright, max_u1, min_v1);
			//AutoCaculateQuadNormal(pVetLockPtr + 4);

			if (!canConnectZPos && !canConnectZNeg)
			{
				renderrable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite2->getTexture(), (TESS_RENDER_PASS)pass, 2);
				if (!renderrable->getLockPtr(2, pVetLockPtr))
					return false;

				pVetLockPtr[0].setEx(m_sectionPos, var36, y1, var50, color, bright, var24, min_v2);
				pVetLockPtr[1].setEx(m_sectionPos, var36, y0, var50, color, bright, var24, max_v2);
				pVetLockPtr[2].setEx(m_sectionPos, var36, y0, var52, color, bright, var26, max_v2);
				pVetLockPtr[3].setEx(m_sectionPos, var36, y1, var52, color, bright, var26, min_v2);
				//AutoCaculateQuadNormal(pVetLockPtr + 0);

				pVetLockPtr[4].setEx(m_sectionPos, var36, y1, var52, color, bright, var24, min_v2);
				pVetLockPtr[5].setEx(m_sectionPos, var36, y0, var52, color, bright, var24, max_v2);
				pVetLockPtr[6].setEx(m_sectionPos, var36, y0, var50, color, bright, var26, max_v2);
				pVetLockPtr[7].setEx(m_sectionPos, var36, y1, var50, color, bright, var26, min_v2);
				//AutoCaculateQuadNormal(pVetLockPtr + 4);
			}

			if (shouldRenderYPos || pos.y < height - 1 && blockAccess->isAirBlock(pos.getUE()))
			{
				renderrable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite2->getTexture(), (TESS_RENDER_PASS)pass, 2);
				if (!renderrable->getLockPtr(2, pVetLockPtr))
					return false;

				float y = pos.y + 1.01f;
				pVetLockPtr[0].setEx(m_sectionPos, var36, y, var52, color, bright, var26, min_v2);
				pVetLockPtr[1].setEx(m_sectionPos, var38, y, var52, color, bright, var26, var30);
				pVetLockPtr[2].setEx(m_sectionPos, var38, y, var50, color, bright, var24, var30);
				pVetLockPtr[3].setEx(m_sectionPos, var36, y, var50, color, bright, var24, min_v2);
				//AutoCaculateQuadNormal(pVetLockPtr + 0);

				pVetLockPtr[4].setEx(m_sectionPos, var38, y, var52, color, bright, var26, min_v2);
				pVetLockPtr[5].setEx(m_sectionPos, var36, y, var52, color, bright, var26, var30);
				pVetLockPtr[6].setEx(m_sectionPos, var36, y, var50, color, bright, var24, var30);
				pVetLockPtr[7].setEx(m_sectionPos, var38, y, var50, color, bright, var24, min_v2);
				//AutoCaculateQuadNormal(pVetLockPtr + 4);
			}

			if (shouldRenderYNeg || pos.y > 1 && blockAccess->isAirBlock(pos.getDE()))
			{
				renderrable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite2->getTexture(), (TESS_RENDER_PASS)pass, 2);
				if (!renderrable->getLockPtr(2, pVetLockPtr))
					return false;

				float y = pos.y - 0.01f;
				pVetLockPtr[0].setEx(m_sectionPos, var36, y, var52, color, bright, var26, min_v2);
				pVetLockPtr[1].setEx(m_sectionPos, var38, y, var52, color, bright, var26, var30);
				pVetLockPtr[2].setEx(m_sectionPos, var38, y, var50, color, bright, var24, var30);
				pVetLockPtr[3].setEx(m_sectionPos, var36, y, var50, color, bright, var24, min_v2);
				//AutoCaculateQuadNormal(pVetLockPtr + 0);

				pVetLockPtr[4].setEx(m_sectionPos, var38, y, var52, color, bright, var26, min_v2);
				pVetLockPtr[5].setEx(m_sectionPos, var36, y, var52, color, bright, var26, var30);
				pVetLockPtr[6].setEx(m_sectionPos, var36, y, var50, color, bright, var24, var30);
				pVetLockPtr[7].setEx(m_sectionPos, var38, y, var50, color, bright, var24, min_v2);
				//AutoCaculateQuadNormal(pVetLockPtr + 4);
			}
		}
	}
	else
	{

		int pass = pBlockPane->getRenderBlockPass();
		TessRenderable* renderrable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite1->getTexture(), (TESS_RENDER_PASS)pass, 2);
		TessVertexFmt* pVetLockPtr = NULL;
		if (!renderrable->getLockPtr(2, pVetLockPtr))
			return false;

		pVetLockPtr[0].setEx(m_sectionPos, var34, y1, var42, color, bright, min_u1, min_v1);
		pVetLockPtr[1].setEx(m_sectionPos, var34, y0, var42, color, bright, min_u1, max_v1);
		pVetLockPtr[2].setEx(m_sectionPos, var38, y0, var42, color, bright, max_u1, max_v1);
		pVetLockPtr[3].setEx(m_sectionPos, var38, y1, var42, color, bright, max_u1, min_v1);
		//AutoCaculateQuadNormal(pVetLockPtr + 0);

		pVetLockPtr[4].setEx(m_sectionPos, var38, y1, var42, color, bright, min_u1, min_v1);
		pVetLockPtr[5].setEx(m_sectionPos, var38, y0, var42, color, bright, min_u1, max_v1);
		pVetLockPtr[6].setEx(m_sectionPos, var34, y0, var42, color, bright, max_u1, max_v1);
		pVetLockPtr[7].setEx(m_sectionPos, var34, y1, var42, color, bright, max_u1, min_v1);
		//AutoCaculateQuadNormal(pVetLockPtr + 4);

		if (shouldRenderYPos)
		{
			renderrable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite2->getTexture(), (TESS_RENDER_PASS)pass, 2);
			if (!renderrable->getLockPtr(2, pVetLockPtr))
				return false;

			float y = pos.y + 1.01f;
			pVetLockPtr[0].setEx(m_sectionPos, var34, y, var52, color, bright, var26, max_v2);
			pVetLockPtr[1].setEx(m_sectionPos, var38, y, var52, color, bright, var26, min_v2);
			pVetLockPtr[2].setEx(m_sectionPos, var38, y, var50, color, bright, var24, min_v2);
			pVetLockPtr[3].setEx(m_sectionPos, var34, y, var50, color, bright, var24, max_v2);
			//AutoCaculateQuadNormal(pVetLockPtr + 0);

			pVetLockPtr[4].setEx(m_sectionPos, var38, y, var52, color, bright, var26, max_v2);
			pVetLockPtr[5].setEx(m_sectionPos, var34, y, var52, color, bright, var26, min_v2);
			pVetLockPtr[6].setEx(m_sectionPos, var34, y, var50, color, bright, var24, min_v2);
			pVetLockPtr[7].setEx(m_sectionPos, var38, y, var50, color, bright, var24, max_v2);
			//AutoCaculateQuadNormal(pVetLockPtr + 4);
		}
		else
		{
			if (pos.y < height - 1 && blockAccess->isAirBlock(pos.getUW()))
			{
				renderrable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite2->getTexture(), (TESS_RENDER_PASS)pass, 2);
				if (!renderrable->getLockPtr(2, pVetLockPtr))
					return false;

				float y = pos.y + 1.01f;
				pVetLockPtr[0].setEx(m_sectionPos, var34, y, var52, color, bright, var26, var30);
				pVetLockPtr[1].setEx(m_sectionPos, var36, y, var52, color, bright, var26, max_v2);
				pVetLockPtr[2].setEx(m_sectionPos, var36, y, var50, color, bright, var24, max_v2);
				pVetLockPtr[3].setEx(m_sectionPos, var34, y, var50, color, bright, var24, var30);
				//AutoCaculateQuadNormal(pVetLockPtr + 0);

				pVetLockPtr[4].setEx(m_sectionPos, var36, y, var52, color, bright, var26, var30);
				pVetLockPtr[5].setEx(m_sectionPos, var34, y, var52, color, bright, var26, max_v2);
				pVetLockPtr[6].setEx(m_sectionPos, var34, y, var50, color, bright, var24, max_v2);
				pVetLockPtr[7].setEx(m_sectionPos, var36, y, var50, color, bright, var24, var30);
				//AutoCaculateQuadNormal(pVetLockPtr + 4);
			}

			if (pos.y < height - 1 && blockAccess->isAirBlock(pos.getUE()))
			{
				renderrable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite2->getTexture(), (TESS_RENDER_PASS)pass, 2);
				if (!renderrable->getLockPtr(2, pVetLockPtr))
					return false;

				float y = pos.y + 1.01f;
				pVetLockPtr[0].setEx(m_sectionPos, var36, y, var52, color, bright, var26, min_v2);
				pVetLockPtr[1].setEx(m_sectionPos, var38, y, var52, color, bright, var26, var30);
				pVetLockPtr[2].setEx(m_sectionPos, var38, y, var50, color, bright, var24, var30);
				pVetLockPtr[3].setEx(m_sectionPos, var36, y, var50, color, bright, var24, min_v2);
				//AutoCaculateQuadNormal(pVetLockPtr + 0);

				pVetLockPtr[4].setEx(m_sectionPos, var38, y, var52, color, bright, var26, min_v2);
				pVetLockPtr[5].setEx(m_sectionPos, var36, y, var52, color, bright, var26, var30);
				pVetLockPtr[6].setEx(m_sectionPos, var36, y, var50, color, bright, var24, var30);
				pVetLockPtr[7].setEx(m_sectionPos, var38, y, var50, color, bright, var24, min_v2);
				//AutoCaculateQuadNormal(pVetLockPtr + 4);
			}
		}

		if (shouldRenderYNeg)
		{
			renderrable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite2->getTexture(), (TESS_RENDER_PASS)pass, 2);
			if (!renderrable->getLockPtr(2, pVetLockPtr))
				return false;

			float y = pos.y - 0.01f;
			pVetLockPtr[0].setEx(m_sectionPos, var34,y, var52, color, bright, var26, max_v2);
			pVetLockPtr[1].setEx(m_sectionPos, var38,y, var52, color, bright, var26, min_v2);
			pVetLockPtr[2].setEx(m_sectionPos, var38,y, var50, color, bright, var24, min_v2);
			pVetLockPtr[3].setEx(m_sectionPos, var34,y, var50, color, bright, var24, max_v2);
			//AutoCaculateQuadNormal(pVetLockPtr + 0);

			pVetLockPtr[4].setEx(m_sectionPos, var38,y, var52, color, bright, var26, max_v2);
			pVetLockPtr[5].setEx(m_sectionPos, var34,y, var52, color, bright, var26, min_v2);
			pVetLockPtr[6].setEx(m_sectionPos, var34,y, var50, color, bright, var24, min_v2);
			pVetLockPtr[7].setEx(m_sectionPos, var38,y, var50, color, bright, var24, max_v2);
			//AutoCaculateQuadNormal(pVetLockPtr + 4);
		}
		else
		{
			if (pos.y > 1 && blockAccess->isAirBlock(pos.getDW()))
			{
				renderrable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite2->getTexture(), (TESS_RENDER_PASS)pass, 2);
				if (!renderrable->getLockPtr(2, pVetLockPtr))
					return false;

				float y = pos.y - 0.01f;
				pVetLockPtr[0].setEx(m_sectionPos, var34, y, var52, color, bright, var26, var30);
				pVetLockPtr[1].setEx(m_sectionPos, var36, y, var52, color, bright, var26, max_v2);
				pVetLockPtr[2].setEx(m_sectionPos, var36, y, var50, color, bright, var24, max_v2);
				pVetLockPtr[3].setEx(m_sectionPos, var34, y, var50, color, bright, var24, var30);
				//AutoCaculateQuadNormal(pVetLockPtr + 0);

				pVetLockPtr[4].setEx(m_sectionPos, var36, y, var52, color, bright, var26, var30);
				pVetLockPtr[5].setEx(m_sectionPos, var34, y, var52, color, bright, var26, max_v2);
				pVetLockPtr[6].setEx(m_sectionPos, var34, y, var50, color, bright, var24, max_v2);
				pVetLockPtr[7].setEx(m_sectionPos, var36, y, var50, color, bright, var24, var30);
				//AutoCaculateQuadNormal(pVetLockPtr + 4);
			}

			if (pos.y > 1 && blockAccess->isAirBlock(pos.getDE()))
			{
				renderrable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite2->getTexture(), (TESS_RENDER_PASS)pass, 2);
				if (!renderrable->getLockPtr(2, pVetLockPtr))
					return false;

				float y = pos.y - 0.01f;
				pVetLockPtr[0].setEx(m_sectionPos, var36, y, var52, color, bright, var26, min_v2);
				pVetLockPtr[1].setEx(m_sectionPos, var38, y, var52, color, bright, var26, var30);
				pVetLockPtr[2].setEx(m_sectionPos, var38, y, var50, color, bright, var24, var30);
				pVetLockPtr[3].setEx(m_sectionPos, var36, y, var50, color, bright, var24, min_v2);
				//AutoCaculateQuadNormal(pVetLockPtr + 0);

				pVetLockPtr[4].setEx(m_sectionPos, var38, y, var52, color, bright, var26, min_v2);
				pVetLockPtr[5].setEx(m_sectionPos, var36, y, var52, color, bright, var26, var30);
				pVetLockPtr[6].setEx(m_sectionPos, var36, y, var50, color, bright, var24, var30);
				pVetLockPtr[7].setEx(m_sectionPos, var38, y, var50, color, bright, var24, min_v2);
				//AutoCaculateQuadNormal(pVetLockPtr + 4);
			}
		}
	}

	if ((!canConnectZNeg || !canConnectZPos) && (canConnectXNeg || canConnectXPos || canConnectZNeg || canConnectZPos))
	{
		if (canConnectZNeg && !canConnectZPos)
		{

			int pass = pBlockPane->getRenderBlockPass();
			TessRenderable* renderrable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite1->getTexture(), (TESS_RENDER_PASS)pass, 2);
			TessVertexFmt* pVetLockPtr = NULL;
			if (!renderrable->getLockPtr(2, pVetLockPtr))
				return false;

			pVetLockPtr[0].setEx(m_sectionPos, var36, y1, var40, color, bright, min_u1, min_v1);
			pVetLockPtr[1].setEx(m_sectionPos, var36, y0, var40, color, bright, min_u1, max_v1);
			pVetLockPtr[2].setEx(m_sectionPos, var36, y0, var42, color, bright, var16, max_v1);
			pVetLockPtr[3].setEx(m_sectionPos, var36, y1, var42, color, bright, var16, min_v1);
			//AutoCaculateQuadNormal(pVetLockPtr + 0);

			pVetLockPtr[4].setEx(m_sectionPos, var36, y1, var42, color, bright, min_u1, min_v1);
			pVetLockPtr[5].setEx(m_sectionPos, var36, y0, var42, color, bright, min_u1, max_v1);
			pVetLockPtr[6].setEx(m_sectionPos, var36, y0, var40, color, bright, var16, max_v1);
			pVetLockPtr[7].setEx(m_sectionPos, var36, y1, var40, color, bright, var16, min_v1);
			//AutoCaculateQuadNormal(pVetLockPtr + 4);

			if (!canConnectXPos && !canConnectXNeg)
			{
				renderrable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite2->getTexture(), (TESS_RENDER_PASS)pass, 2);
				if (!renderrable->getLockPtr(2, pVetLockPtr))
					return false;

				pVetLockPtr[0].setEx(m_sectionPos, var46, y1, var42, color, bright, var24, min_v2);
				pVetLockPtr[1].setEx(m_sectionPos, var46, y0, var42, color, bright, var24, max_v2);
				pVetLockPtr[2].setEx(m_sectionPos, var48, y0, var42, color, bright, var26, max_v2);
				pVetLockPtr[3].setEx(m_sectionPos, var48, y1, var42, color, bright, var26, min_v2);
				//AutoCaculateQuadNormal(pVetLockPtr + 0);

				pVetLockPtr[4].setEx(m_sectionPos, var48, y1, var42, color, bright, var24, min_v2);
				pVetLockPtr[5].setEx(m_sectionPos, var48, y0, var42, color, bright, var24, max_v2);
				pVetLockPtr[6].setEx(m_sectionPos, var46, y0, var42, color, bright, var26, max_v2);
				pVetLockPtr[7].setEx(m_sectionPos, var46, y1, var42, color, bright, var26, min_v2);
				//AutoCaculateQuadNormal(pVetLockPtr + 4);
			}

			if (shouldRenderYPos || pos.y < height - 1 && blockAccess->isAirBlock(pos.getUN()))
			{
				renderrable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite2->getTexture(), (TESS_RENDER_PASS)pass, 2);
				if (!renderrable->getLockPtr(2, pVetLockPtr))
					return false;

				float y = pos.y + 1.005f;
				pVetLockPtr[0].setEx(m_sectionPos, var46, y, var40, color, bright, var26, min_v2);
				pVetLockPtr[1].setEx(m_sectionPos, var46, y, var42, color, bright, var26, var30);
				pVetLockPtr[2].setEx(m_sectionPos, var48, y, var42, color, bright, var24, var30);
				pVetLockPtr[3].setEx(m_sectionPos, var48, y, var40, color, bright, var24, min_v2);
				//AutoCaculateQuadNormal(pVetLockPtr + 0);

				pVetLockPtr[4].setEx(m_sectionPos, var46, y, var42, color, bright, var26, min_v2);
				pVetLockPtr[5].setEx(m_sectionPos, var46, y, var40, color, bright, var26, var30);
				pVetLockPtr[6].setEx(m_sectionPos, var48, y, var40, color, bright, var24, var30);
				pVetLockPtr[7].setEx(m_sectionPos, var48, y, var42, color, bright, var24, min_v2);
				//AutoCaculateQuadNormal(pVetLockPtr + 4);
			}

			if (shouldRenderYNeg || pos.y > 1 && blockAccess->isAirBlock(pos.getDN()))
			{
				renderrable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite2->getTexture(), (TESS_RENDER_PASS)pass, 2);
				if (!renderrable->getLockPtr(2, pVetLockPtr))
					return false;

				float y = pos.y - 0.005f;
				pVetLockPtr[0].setEx(m_sectionPos, var46, y, var40, color, bright, var26, min_v2);
				pVetLockPtr[1].setEx(m_sectionPos, var46, y, var42, color, bright, var26, var30);
				pVetLockPtr[2].setEx(m_sectionPos, var48, y, var42, color, bright, var24, var30);
				pVetLockPtr[3].setEx(m_sectionPos, var48, y, var40, color, bright, var24, min_v2);
				//AutoCaculateQuadNormal(pVetLockPtr + 0);

				pVetLockPtr[4].setEx(m_sectionPos, var46, y, var42, color, bright, var26, min_v2);
				pVetLockPtr[5].setEx(m_sectionPos, var46, y, var40, color, bright, var26, var30);
				pVetLockPtr[6].setEx(m_sectionPos, var48, y, var40, color, bright, var24, var30);
				pVetLockPtr[7].setEx(m_sectionPos, var48, y, var42, color, bright, var24, min_v2);
				//AutoCaculateQuadNormal(pVetLockPtr + 4);
			}
		}
		else if (!canConnectZNeg && canConnectZPos)
		{

			int pass = pBlockPane->getRenderBlockPass();
			TessRenderable* renderrable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite1->getTexture(), (TESS_RENDER_PASS)pass, 2);
			TessVertexFmt* pVetLockPtr = NULL;
			if (!renderrable->getLockPtr(2, pVetLockPtr))
				return false;

			pVetLockPtr[0].setEx(m_sectionPos, var36, y1, var42, color, bright, var16, min_v1);
			pVetLockPtr[1].setEx(m_sectionPos, var36, y0, var42, color, bright, var16, max_v1);
			pVetLockPtr[2].setEx(m_sectionPos, var36, y0, var44, color, bright, max_u1, max_v1);
			pVetLockPtr[3].setEx(m_sectionPos, var36, y1, var44, color, bright, max_u1, min_v1);
			//AutoCaculateQuadNormal(pVetLockPtr + 0);

			pVetLockPtr[4].setEx(m_sectionPos, var36, y1, var44, color, bright, var16, min_v1);
			pVetLockPtr[5].setEx(m_sectionPos, var36, y0, var44, color, bright, var16, max_v1);
			pVetLockPtr[6].setEx(m_sectionPos, var36, y0, var42, color, bright, max_u1, max_v1);
			pVetLockPtr[7].setEx(m_sectionPos, var36, y1, var42, color, bright, max_u1, min_v1);
			//AutoCaculateQuadNormal(pVetLockPtr + 4);

			if (!canConnectXPos && !canConnectXNeg)
			{
				renderrable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite2->getTexture(), (TESS_RENDER_PASS)pass, 2);
				if (!renderrable->getLockPtr(2, pVetLockPtr))
					return false;

				pVetLockPtr[0].setEx(m_sectionPos, var48, y1, var42, color, bright, var24, min_v2);
				pVetLockPtr[1].setEx(m_sectionPos, var48, y0, var42, color, bright, var24, max_v2);
				pVetLockPtr[2].setEx(m_sectionPos, var46, y0, var42, color, bright, var26, max_v2);
				pVetLockPtr[3].setEx(m_sectionPos, var46, y1, var42, color, bright, var26, min_v2);
				//AutoCaculateQuadNormal(pVetLockPtr + 0);

				pVetLockPtr[4].setEx(m_sectionPos, var46, y1, var42, color, bright, var24, min_v2);
				pVetLockPtr[5].setEx(m_sectionPos, var46, y0, var42, color, bright, var24, max_v2);
				pVetLockPtr[6].setEx(m_sectionPos, var48, y0, var42, color, bright, var26, max_v2);
				pVetLockPtr[7].setEx(m_sectionPos, var48, y1, var42, color, bright, var26, min_v2);
				//AutoCaculateQuadNormal(pVetLockPtr + 4);
			}

			if (shouldRenderYPos || pos.y < height - 1 && blockAccess->isAirBlock(pos.getUS()))
			{
				renderrable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite2->getTexture(), (TESS_RENDER_PASS)pass, 2);
				if (!renderrable->getLockPtr(2, pVetLockPtr))
					return false;

				float y = pos.y + 1.005f;
				pVetLockPtr[0].setEx(m_sectionPos, var46, y, var42, color, bright, var24, var30);
				pVetLockPtr[1].setEx(m_sectionPos, var46, y, var44, color, bright, var24, max_v2);
				pVetLockPtr[2].setEx(m_sectionPos, var48, y, var44, color, bright, var26, max_v2);
				pVetLockPtr[3].setEx(m_sectionPos, var48, y, var42, color, bright, var26, var30);
				//AutoCaculateQuadNormal(pVetLockPtr + 0);

				pVetLockPtr[4].setEx(m_sectionPos, var46, y, var44, color, bright, var24, var30);
				pVetLockPtr[5].setEx(m_sectionPos, var46, y, var42, color, bright, var24, max_v2);
				pVetLockPtr[6].setEx(m_sectionPos, var48, y, var42, color, bright, var26, max_v2);
				pVetLockPtr[7].setEx(m_sectionPos, var48, y, var44, color, bright, var26, var30);
				//AutoCaculateQuadNormal(pVetLockPtr + 4);
			}

			if (shouldRenderYNeg || pos.y > 1 && blockAccess->isAirBlock(pos.getDS()))
			{
				renderrable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite2->getTexture(), (TESS_RENDER_PASS)pass, 2);
				if (!renderrable->getLockPtr(2, pVetLockPtr))
					return false;

				float y = pos.y - 0.005f;
				pVetLockPtr[0].setEx(m_sectionPos, var46, y, var42, color, bright, var24, var30);
				pVetLockPtr[1].setEx(m_sectionPos, var46, y, var44, color, bright, var24, max_v2);
				pVetLockPtr[2].setEx(m_sectionPos, var48, y, var44, color, bright, var26, max_v2);
				pVetLockPtr[3].setEx(m_sectionPos, var48, y, var42, color, bright, var26, var30);
				//AutoCaculateQuadNormal(pVetLockPtr + 0);

				pVetLockPtr[4].setEx(m_sectionPos, var46, y, var44, color, bright, var24, var30);
				pVetLockPtr[5].setEx(m_sectionPos, var46, y, var42, color, bright, var24, max_v2);
				pVetLockPtr[6].setEx(m_sectionPos, var48, y, var42, color, bright, var26, max_v2);
				pVetLockPtr[7].setEx(m_sectionPos, var48, y, var44, color, bright, var26, var30);
				//AutoCaculateQuadNormal(pVetLockPtr + 4);
			}
		}
	}
	else
	{

		int pass = pBlockPane->getRenderBlockPass();
		TessRenderable* renderrable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite1->getTexture(), (TESS_RENDER_PASS)pass, 2);
		TessVertexFmt* pVetLockPtr = NULL;
		if (!renderrable->getLockPtr(2, pVetLockPtr))
			return false;

		pVetLockPtr[0].setEx(m_sectionPos, var36, y1, var44, color, bright, min_u1, min_v1);
		pVetLockPtr[1].setEx(m_sectionPos, var36, y0, var44, color, bright, min_u1, max_v1);
		pVetLockPtr[2].setEx(m_sectionPos, var36, y0, var40, color, bright, max_u1, max_v1);
		pVetLockPtr[3].setEx(m_sectionPos, var36, y1, var40, color, bright, max_u1, min_v1);
		//AutoCaculateQuadNormal(pVetLockPtr + 0);

		pVetLockPtr[4].setEx(m_sectionPos, var36, y1, var40, color, bright, min_u1, min_v1);
		pVetLockPtr[5].setEx(m_sectionPos, var36, y0, var40, color, bright, min_u1, max_v1);
		pVetLockPtr[6].setEx(m_sectionPos, var36, y0, var44, color, bright, max_u1, max_v1);
		pVetLockPtr[7].setEx(m_sectionPos, var36, y1, var44, color, bright, max_u1, min_v1);
		//AutoCaculateQuadNormal(pVetLockPtr + 4);

		if (shouldRenderYPos)
		{
			renderrable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite2->getTexture(), (TESS_RENDER_PASS)pass, 2);
			if (!renderrable->getLockPtr(2, pVetLockPtr))
				return false;

			float y = pos.y + 1.005f;
			pVetLockPtr[0].setEx(m_sectionPos, var48, y, var44, color, bright, var26, max_v2);
			pVetLockPtr[1].setEx(m_sectionPos, var48, y, var40, color, bright, var26, min_v2);
			pVetLockPtr[2].setEx(m_sectionPos, var46, y, var40, color, bright, var24, min_v2);
			pVetLockPtr[3].setEx(m_sectionPos, var46, y, var44, color, bright, var24, max_v2);
			//AutoCaculateQuadNormal(pVetLockPtr + 0);

			pVetLockPtr[4].setEx(m_sectionPos, var48, y, var40, color, bright, var26, max_v2);
			pVetLockPtr[5].setEx(m_sectionPos, var48, y, var44, color, bright, var26, min_v2);
			pVetLockPtr[6].setEx(m_sectionPos, var46, y, var44, color, bright, var24, min_v2);
			pVetLockPtr[7].setEx(m_sectionPos, var46, y, var40, color, bright, var24, max_v2);
			//AutoCaculateQuadNormal(pVetLockPtr + 4);
		}
		else
		{
			if (pos.y < height - 1 && blockAccess->isAirBlock(pos.getUN()))
			{
				renderrable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite2->getTexture(), (TESS_RENDER_PASS)pass, 2);
				if (!renderrable->getLockPtr(2, pVetLockPtr))
					return false;

				float y = pos.y + 1.005f;
				pVetLockPtr[0].setEx(m_sectionPos, var46, y, var40, color, bright, var26, min_v2);
				pVetLockPtr[1].setEx(m_sectionPos, var46, y, var42, color, bright, var26, var30);
				pVetLockPtr[2].setEx(m_sectionPos, var48, y, var42, color, bright, var24, var30);
				pVetLockPtr[3].setEx(m_sectionPos, var48, y, var40, color, bright, var24, min_v2);
				//AutoCaculateQuadNormal(pVetLockPtr + 0);

				pVetLockPtr[4].setEx(m_sectionPos, var46, y, var42, color, bright, var26, min_v2);
				pVetLockPtr[5].setEx(m_sectionPos, var46, y, var40, color, bright, var26, var30);
				pVetLockPtr[6].setEx(m_sectionPos, var48, y, var40, color, bright, var24, var30);
				pVetLockPtr[7].setEx(m_sectionPos, var48, y, var42, color, bright, var24, min_v2);
				//AutoCaculateQuadNormal(pVetLockPtr + 4);
			}

			if (pos.y < height - 1 && blockAccess->isAirBlock(pos.getUS()))
			{
				renderrable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite2->getTexture(), (TESS_RENDER_PASS)pass, 2);
				if (!renderrable->getLockPtr(2, pVetLockPtr))
					return false;

				float y = pos.y + 1.005f;
				pVetLockPtr[0].setEx(m_sectionPos, var46, y, var42, color, bright, var24, var30);
				pVetLockPtr[1].setEx(m_sectionPos, var46, y, var44, color, bright, var24, max_v2);
				pVetLockPtr[2].setEx(m_sectionPos, var48, y, var44, color, bright, var26, max_v2);
				pVetLockPtr[3].setEx(m_sectionPos, var48, y, var42, color, bright, var26, var30);
				//AutoCaculateQuadNormal(pVetLockPtr + 0);

				pVetLockPtr[4].setEx(m_sectionPos, var46, y, var44, color, bright, var24, var30);
				pVetLockPtr[5].setEx(m_sectionPos, var46, y, var42, color, bright, var24, max_v2);
				pVetLockPtr[6].setEx(m_sectionPos, var48, y, var42, color, bright, var26, max_v2);
				pVetLockPtr[7].setEx(m_sectionPos, var48, y, var44, color, bright, var26, var30);
				//AutoCaculateQuadNormal(pVetLockPtr + 4);
			}
		}

		if (shouldRenderYNeg)
		{
			renderrable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite2->getTexture(), (TESS_RENDER_PASS)pass, 2);
			if (!renderrable->getLockPtr(2, pVetLockPtr))
				return false;

			float y = pos.y - 0.005f;
			pVetLockPtr[0].setEx(m_sectionPos, var48, y, var44, color, bright, var26, max_v2);
			pVetLockPtr[1].setEx(m_sectionPos, var48, y, var40, color, bright, var26, min_v2);
			pVetLockPtr[2].setEx(m_sectionPos, var46, y, var40, color, bright, var24, min_v2);
			pVetLockPtr[3].setEx(m_sectionPos, var46, y, var44, color, bright, var24, max_v2);
			//AutoCaculateQuadNormal(pVetLockPtr + 0);

			pVetLockPtr[4].setEx(m_sectionPos, var48, y, var40, color, bright, var26, max_v2);
			pVetLockPtr[5].setEx(m_sectionPos, var48, y, var44, color, bright, var26, min_v2);
			pVetLockPtr[6].setEx(m_sectionPos, var46, y, var44, color, bright, var24, min_v2);
			pVetLockPtr[7].setEx(m_sectionPos, var46, y, var40, color, bright, var24, max_v2);
			//AutoCaculateQuadNormal(pVetLockPtr + 4);
		}
		else
		{
			if (pos.y > 1 && blockAccess->isAirBlock(pos.getDN()))
			{
				renderrable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite2->getTexture(), (TESS_RENDER_PASS)pass, 2);
				if (!renderrable->getLockPtr(2, pVetLockPtr))
					return false;

				float y = pos.y - 0.005f;
				pVetLockPtr[0].setEx(m_sectionPos, var46, y, var40, color, bright, var26, min_v2);
				pVetLockPtr[1].setEx(m_sectionPos, var46, y, var42, color, bright, var26, var30);
				pVetLockPtr[2].setEx(m_sectionPos, var48, y, var42, color, bright, var24, var30);
				pVetLockPtr[3].setEx(m_sectionPos, var48, y, var40, color, bright, var24, min_v2);
				//AutoCaculateQuadNormal(pVetLockPtr + 0);

				pVetLockPtr[4].setEx(m_sectionPos, var46, y, var42, color, bright, var26, min_v2);
				pVetLockPtr[5].setEx(m_sectionPos, var46, y, var40, color, bright, var26, var30);
				pVetLockPtr[6].setEx(m_sectionPos, var48, y, var40, color, bright, var24, var30);
				pVetLockPtr[7].setEx(m_sectionPos, var48, y, var42, color, bright, var24, min_v2);
				//AutoCaculateQuadNormal(pVetLockPtr + 4);
			}

			if (pos.y > 1 && blockAccess->isAirBlock(pos.getDS()))
			{
				renderrable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite2->getTexture(), (TESS_RENDER_PASS)pass, 2);
				if (!renderrable->getLockPtr(2, pVetLockPtr))
					return false;

				float y = pos.y - 0.005f;
				pVetLockPtr[0].setEx(m_sectionPos, var46, y, var42, color, bright, var24, var30);
				pVetLockPtr[1].setEx(m_sectionPos, var46, y, var44, color, bright, var24, max_v2);
				pVetLockPtr[2].setEx(m_sectionPos, var48, y, var44, color, bright, var26, max_v2);
				pVetLockPtr[3].setEx(m_sectionPos, var48, y, var42, color, bright, var26, var30);
				//AutoCaculateQuadNormal(pVetLockPtr + 0);

				pVetLockPtr[4].setEx(m_sectionPos, var46, y, var44, color, bright, var24, var30);
				pVetLockPtr[5].setEx(m_sectionPos, var46, y, var42, color, bright, var24, max_v2);
				pVetLockPtr[6].setEx(m_sectionPos, var48, y, var42, color, bright, var26, max_v2);
				pVetLockPtr[7].setEx(m_sectionPos, var48, y, var44, color, bright, var26, var30);
				//AutoCaculateQuadNormal(pVetLockPtr + 4);
			}
		}
	}

	return true;
}

bool RenderBlocks::renderBlockFenceGate(Block* pBlock, cBlock* pcBlock, const BlockPos& pos)
{
	BlockFenceGate* pBlockFenceGate = (BlockFenceGate*)(pBlock);
	bool var5 = true;
	int metadata = blockAccess->getBlockMeta(pos);
	bool isOpen = BlockFenceGate::isFenceGateOpen(metadata);
	int direction = BlockFenceGate::getDirection(metadata);
	float var9 = 0.375F;
	float var10 = 0.5625F;
	float var11 = 0.75F;
	float var12 = 0.9375F;
	float var13 = 0.3125F;
	float var14 = 1.0F;

	if ((direction == 2 || direction == 0) &&
		blockAccess->getBlockId(pos.getNegX()) == BLOCK_ID_COBBLE_STONE_WALL &&
		blockAccess->getBlockId(pos.getPosX()) == BLOCK_ID_COBBLE_STONE_WALL ||
		(direction == 3 || direction == 1) &&
		blockAccess->getBlockId(pos.getNegZ()) == BLOCK_ID_COBBLE_STONE_WALL && 
		blockAccess->getBlockId(pos.getPosZ()) == BLOCK_ID_COBBLE_STONE_WALL)
	{
		var9 -= 0.1875F;
		var10 -= 0.1875F;
		var11 -= 0.1875F;
		var12 -= 0.1875F;
		var13 -= 0.1875F;
		var14 -= 0.1875F;
	}

	renderAllFaces = true;
	float var15;
	float var16;
	float var17;
	float var18;

	if (direction != 3 && direction != 1)
	{
		var15 = 0.0F;
		var16 = 0.125F;
		var17 = 0.4375F;
		var18 = 0.5625F;
		setRenderBounds(var15, var13, var17, var16, var14, var18);
		renderStandardBlock(pBlockFenceGate, pcBlock, pos);
		var15 = 0.875F;
		var16 = 1.0F;
		setRenderBounds(var15, var13, var17, var16, var14, var18);
		renderStandardBlock(pBlockFenceGate, pcBlock, pos);
	}
	else
	{
		uvRotateTop = 1;
		var15 = 0.4375F;
		var16 = 0.5625F;
		var17 = 0.0F;
		var18 = 0.125F;
		setRenderBounds(var15, var13, var17, var16, var14, var18);
		renderStandardBlock(pBlockFenceGate, pcBlock, pos);
		var17 = 0.875F;
		var18 = 1.0F;
		setRenderBounds(var15, var13, var17, var16, var14, var18);
		renderStandardBlock(pBlockFenceGate, pcBlock, pos);
		uvRotateTop = 0;
	}

	if (isOpen)
	{
		if (direction == 2 || direction == 0)
		{
			uvRotateTop = 1;
		}

		float var19;
		float var20;
		float var21;

		if (direction == 3)
		{
			var15 = 0.0F;
			var16 = 0.125F;
			var17 = 0.875F;
			var18 = 1.0F;
			var19 = 0.5625F;
			var20 = 0.8125F;
			var21 = 0.9375F;
			setRenderBounds(0.8125F, var9, 0.0F, 0.9375F, var12, 0.125F);
			renderStandardBlock(pBlockFenceGate, pcBlock, pos);
			setRenderBounds(0.8125F, var9, 0.875F, 0.9375F, var12, 1.0F);
			renderStandardBlock(pBlockFenceGate, pcBlock, pos);
			setRenderBounds(0.5625F, var9, 0.0F, 0.8125F, var10, 0.125F);
			renderStandardBlock(pBlockFenceGate, pcBlock, pos);
			setRenderBounds(0.5625F, var9, 0.875F, 0.8125F, var10, 1.0F);
			renderStandardBlock(pBlockFenceGate, pcBlock, pos);
			setRenderBounds(0.5625F, var11, 0.0F, 0.8125F, var12, 0.125F);
			renderStandardBlock(pBlockFenceGate, pcBlock, pos);
			setRenderBounds(0.5625F, var11, 0.875F, 0.8125F, var12, 1.0F);
			renderStandardBlock(pBlockFenceGate, pcBlock, pos);
		}
		else if (direction == 1)
		{
			var15 = 0.0F;
			var16 = 0.125F;
			var17 = 0.875F;
			var18 = 1.0F;
			var19 = 0.0625F;
			var20 = 0.1875F;
			var21 = 0.4375F;
			setRenderBounds(0.0625F, var9, 0.0F, 0.1875F, var12, 0.125F);
			renderStandardBlock(pBlockFenceGate, pcBlock, pos);
			setRenderBounds(0.0625F, var9, 0.875F, 0.1875F, var12, 1.0F);
			renderStandardBlock(pBlockFenceGate, pcBlock, pos);
			setRenderBounds(0.1875F, var9, 0.0F, 0.4375F, var10, 0.125F);
			renderStandardBlock(pBlockFenceGate, pcBlock, pos);
			setRenderBounds(0.1875F, var9, 0.875F, 0.4375F, var10, 1.0F);
			renderStandardBlock(pBlockFenceGate, pcBlock, pos);
			setRenderBounds(0.1875F, var11, 0.0F, 0.4375F, var12, 0.125F);
			renderStandardBlock(pBlockFenceGate, pcBlock, pos);
			setRenderBounds(0.1875F, var11, 0.875F, 0.4375F, var12, 1.0F);
			renderStandardBlock(pBlockFenceGate, pcBlock, pos);
		}
		else if (direction == 0)
		{
			var15 = 0.0F;
			var16 = 0.125F;
			var17 = 0.875F;
			var18 = 1.0F;
			var19 = 0.5625F;
			var20 = 0.8125F;
			var21 = 0.9375F;
			setRenderBounds(0.0F, var9, 0.8125F, 0.125F, var12, 0.9375F);
			renderStandardBlock(pBlockFenceGate, pcBlock, pos);
			setRenderBounds(0.875F, var9, 0.8125F, 1.0F, var12, 0.9375F);
			renderStandardBlock(pBlockFenceGate, pcBlock, pos);
			setRenderBounds(0.0F, var9, 0.5625F, 0.125F, var10, 0.8125F);
			renderStandardBlock(pBlockFenceGate, pcBlock, pos);
			setRenderBounds(0.875F, var9, 0.5625F, 1.0F, var10, 0.8125F);
			renderStandardBlock(pBlockFenceGate, pcBlock, pos);
			setRenderBounds(0.0F, var11, 0.5625F, 0.125F, var12, 0.8125F);
			renderStandardBlock(pBlockFenceGate, pcBlock, pos);
			setRenderBounds(0.875F, var11, 0.5625F, 1.0F, var12, 0.8125F);
			renderStandardBlock(pBlockFenceGate, pcBlock, pos);
		}
		else if (direction == 2)
		{
			var15 = 0.0F;
			var16 = 0.125F;
			var17 = 0.875F;
			var18 = 1.0F;
			var19 = 0.0625F;
			var20 = 0.1875F;
			var21 = 0.4375F;
			setRenderBounds(0.0F, var9, 0.0625F, 0.125F, var12, 0.1875F);
			renderStandardBlock(pBlockFenceGate, pcBlock, pos);
			setRenderBounds(0.875F, var9, 0.0625F, 1.0F, var12, 0.1875F);
			renderStandardBlock(pBlockFenceGate, pcBlock, pos);
			setRenderBounds(0.0F, var9, 0.1875F, 0.125F, var10, 0.4375F);
			renderStandardBlock(pBlockFenceGate, pcBlock, pos);
			setRenderBounds(0.875F, var9, 0.1875F, 1.0F, var10, 0.4375F);
			renderStandardBlock(pBlockFenceGate, pcBlock, pos);
			setRenderBounds(0.0F, var11, 0.1875F, 0.125F, var12, 0.4375F);
			renderStandardBlock(pBlockFenceGate, pcBlock, pos);
			setRenderBounds(0.875F, var11, 0.1875F, 1.0F, var12, 0.4375F);
			renderStandardBlock(pBlockFenceGate, pcBlock, pos);
		}
	}
	else if (direction != 3 && direction != 1)
	{
		var15 = 0.375F;
		var16 = 0.5F;
		var17 = 0.4375F;
		var18 = 0.5625F;
		setRenderBounds(var15, var9, var17, var16, var12, var18);
		renderStandardBlock(pBlockFenceGate, pcBlock, pos);
		var15 = 0.5F;
		var16 = 0.625F;
		setRenderBounds(var15, var9, var17, var16, var12, var18);
		renderStandardBlock(pBlockFenceGate, pcBlock, pos);
		var15 = 0.625F;
		var16 = 0.875F;
		setRenderBounds(var15, var9, var17, var16, var10, var18);
		renderStandardBlock(pBlockFenceGate, pcBlock, pos);
		setRenderBounds(var15, var11, var17, var16, var12, var18);
		renderStandardBlock(pBlockFenceGate, pcBlock, pos);
		var15 = 0.125F;
		var16 = 0.375F;
		setRenderBounds(var15, var9, var17, var16, var10, var18);
		renderStandardBlock(pBlockFenceGate, pcBlock, pos);
		setRenderBounds(var15, var11, var17, var16, var12, var18);
		renderStandardBlock(pBlockFenceGate, pcBlock, pos);
	}
	else
	{
		uvRotateTop = 1;
		var15 = 0.4375F;
		var16 = 0.5625F;
		var17 = 0.375F;
		var18 = 0.5F;
		setRenderBounds(var15, var9, var17, var16, var12, var18);
		renderStandardBlock(pBlockFenceGate, pcBlock, pos);
		var17 = 0.5F;
		var18 = 0.625F;
		setRenderBounds(var15, var9, var17, var16, var12, var18);
		renderStandardBlock(pBlockFenceGate, pcBlock, pos);
		var17 = 0.625F;
		var18 = 0.875F;
		setRenderBounds(var15, var9, var17, var16, var10, var18);
		renderStandardBlock(pBlockFenceGate, pcBlock, pos);
		setRenderBounds(var15, var11, var17, var16, var12, var18);
		renderStandardBlock(pBlockFenceGate, pcBlock, pos);
		var17 = 0.125F;
		var18 = 0.375F;
		setRenderBounds(var15, var9, var17, var16, var10, var18);
		renderStandardBlock(pBlockFenceGate, pcBlock, pos);
		setRenderBounds(var15, var11, var17, var16, var12, var18);
		renderStandardBlock(pBlockFenceGate, pcBlock, pos);
	}

	renderAllFaces = false;
	uvRotateTop = 0;
	setRenderBounds(0.0F, 0.0F, 0.0F, 1.0F, 1.0F, 1.0F);
	return var5;
}

bool RenderBlocks::renderBlockWall(Block* pBlock, cBlock* pcBlock, const BlockPos& pos)
{
	BlockWall* pBlockWall = (BlockWall*)(pBlock);
	bool canConnectXNeg = pBlockWall->canConnectWallTo(blockAccess, pos.getNegX());
	bool canConnectXPos = pBlockWall->canConnectWallTo(blockAccess, pos.getPosX());
	bool canConnectZNeg = pBlockWall->canConnectWallTo(blockAccess, pos.getNegZ());
	bool canConnectZPos = pBlockWall->canConnectWallTo(blockAccess, pos.getPosZ());
	bool onlyConnectZAxis = canConnectZNeg && canConnectZPos && !canConnectXNeg && !canConnectXPos;
	bool onlyConnectXAxis = !canConnectZNeg && !canConnectZPos && canConnectXNeg && canConnectXPos;
	bool topIsAir = blockAccess->isAirBlock(pos.getPosY());

	if ((onlyConnectZAxis || onlyConnectXAxis) && topIsAir)
	{
		if (onlyConnectZAxis)
		{
			setRenderBounds(0.3125F, 0.0F, 0.0F, 0.6875F, 0.8125F, 1.0F);
			renderStandardBlock(pBlockWall, pcBlock, pos);
		}
		else
		{
			setRenderBounds(0.0F, 0.0F, 0.3125F, 1.0F, 0.8125F, 0.6875F);
			renderStandardBlock(pBlockWall, pcBlock, pos);
		}
	}
	else
	{
		setRenderBounds(0.25F, 0.0F, 0.25F, 0.75F, 1.0F, 0.75F);
		renderStandardBlock(pBlockWall, pcBlock, pos);

		if (canConnectXNeg)
		{
			setRenderBounds(0.0F, 0.0F, 0.3125F, 0.25F, 0.8125F, 0.6875F);
			renderStandardBlock(pBlockWall, pcBlock, pos);
		}

		if (canConnectXPos)
		{
			setRenderBounds(0.75F, 0.0F, 0.3125F, 1.0F, 0.8125F, 0.6875F);
			renderStandardBlock(pBlockWall, pcBlock, pos);
		}

		if (canConnectZNeg)
		{
			setRenderBounds(0.3125F, 0.0F, 0.0F, 0.6875F, 0.8125F, 0.25F);
			renderStandardBlock(pBlockWall, pcBlock, pos);
		}

		if (canConnectZPos)
		{
			setRenderBounds(0.3125F, 0.0F, 0.75F, 0.6875F, 0.8125F, 1.0F);
			renderStandardBlock(pBlockWall, pcBlock, pos);
		}
	}

	pBlockWall->setBlockBoundsBasedOnState(blockAccess, pos);
	return true;
}

bool RenderBlocks::renderBlockQuartz(Block* pBlock, cBlock* pcBlock, const BlockPos& pos)
{
	int metadata = blockAccess->getBlockMeta(pos);

	if (metadata == 3)
	{
		uvRotateEast = 1;
		uvRotateWest = 1;
		uvRotateTop = 1;
		uvRotateBottom = 1;
	}
	else if (metadata == 4)
	{
		uvRotateSouth = 1;
		uvRotateNorth = 1;
	}

	bool var6 = renderStandardBlock(pBlock, pcBlock, pos);
	uvRotateSouth = 0;
	uvRotateEast = 0;
	uvRotateWest = 0;
	uvRotateNorth = 0;
	uvRotateTop = 0;
	uvRotateBottom = 0;
	return var6;
}

bool RenderBlocks::renderBlockBeacon(Block* pBlock, cBlock* pcBlock, const BlockPos& pos)
{
	float var5 = 0.1875F;
	setOverrideBlockTexture(getBlockIcon(cBlockManager::scBlocks[BLOCK_ID_GLASS]));
	setRenderBounds(0.0F, 0.0F, 0.0F, 1.0F, 1.0F, 1.0F);
	renderStandardBlock(pBlock, pcBlock, pos);
	renderAllFaces = true;
	setOverrideBlockTexture(getBlockIcon(cBlockManager::scBlocks[BLOCK_ID_OBSIDIAN]));
	setRenderBounds(0.125F, 0.0062500000931322575F, 0.125F, 0.875F, var5, 0.875F);
	renderStandardBlock(pBlock, pcBlock, pos);
	setOverrideBlockTexture(getBlockIcon(cBlockManager::scBlocks[BLOCK_ID_BEACON]));
	setRenderBounds(0.1875F, var5, 0.1875F, 0.8125F, 0.875F, 0.8125F);
	renderStandardBlock(pBlock, pcBlock, pos);
	renderAllFaces = false;
	clearOverrideBlockTexture();
	return true;
}

bool RenderBlocks::renderBlockAnvil(Block* pBlock, cBlock* pcBlock, const BlockPos& pos)
{
	int metadata = blockAccess->getBlockMeta(pos);
	int bright = pcBlock->getMixedBrightnessForBlock(blockAccess, pos);
	ui32 color = pcBlock->colorMultiplier(blockAccess, pos);
	float r = (color >> 16 & 255) / 255.0F;
	float g = (color >> 8 & 255) / 255.0F;
	float b = (color & 255) / 255.0F;

	//if(EntityRenderer.anaglyphEnable)
	{
		float tr = (r * 30.0F + g * 59.0F +  b * 11.0F) / 100.0F;
		float tg = (r * 30.0F + g * 70.0F) / 100.0F;
		float tb = (r * 30.0F + b * 70.0F) / 100.0F;
		r = tr;
		g = tg;
		b = tb;
	}	
	
	setBrightness(bright);
	setColor(r, g, b);

	return renderBlockAnvilOrient(pBlock, pcBlock, pos, metadata, false);

}

bool RenderBlocks::renderBlockFlowerpot(Block* pBlock, cBlock* pcBlock, const BlockPos& pos)
{
	cBlockFlowerPot* pBlockFlowerPot = (cBlockFlowerPot*)(pBlock);
	renderStandardBlock(pBlockFlowerPot, pcBlock, pos);
	
	int bright = pBlockFlowerPot->getMixedBrightnessForBlock(blockAccess, pos);
	setBrightness(bright);
	float var6 = 1.0F;
	ui32 color = pBlockFlowerPot->colorMultiplier(blockAccess, pos);
	AtlasSprite* sprite = getBlockIconFromSide(pBlockFlowerPot, 0);
	float r = (float)(color >> 16 & 255) / 255.0F;
	float g = (float)(color >> 8 & 255) / 255.0F;
	float b = (float)(color & 255) / 255.0F;
	

	//if (EntityRenderer.anaglyphEnable)
	{
		float tr = (r * 30.0F + g * 59.0F + b * 11.0F) / 100.0F;
		float tg = (r * 30.0F + g * 70.0F) / 100.0F;
		float tb = (r * 30.0F + b * 70.0F) / 100.0F;
		r = tr;
		g = tg;
		b = tb;
	}
	color = Color(r, g, b).getARGB();
	setColor(var6 * r, var6 * g, var6 * b);
	float var12 = 0.1865F;
	Vector3 vpos(pos);
	renderFaceXPos(pBlockFlowerPot, vpos + Vector3(var12 - 0.5f, 0.f, 0.f), sprite);
	renderFaceXNeg(pBlockFlowerPot, vpos + Vector3(0.5f - var12, 0.f, 0.f), sprite);
	renderFaceZPos(pBlockFlowerPot, vpos + Vector3(0.f, 0.f, var12 - 0.5f), sprite);
	renderFaceZNeg(pBlockFlowerPot, vpos + Vector3(0.f, 0.f, 0.5f - var12), sprite);
	renderFaceYPos(pBlockFlowerPot, vpos + Vector3(0.f, var12 + 0.1875f - 0.5f, 0.f), getBlockIcon(cBlockManager::scBlocks[BLOCK_ID_DIRT]));
	int metadata = blockAccess->getBlockMeta(pos);

	if (metadata != 0)
	{
		float var14 = 0.0F;
		float var15 = 4.0F;
		float var16 = 0.0F;
		Block* pBlockFlower = NULL;

		switch (metadata)
		{
		case 1:
			pBlockFlower = BlockManager::plantRed;
			break;

		case 2:
			pBlockFlower = BlockManager::plantYellow;

		case 3:
		case 4:
		case 5:
		case 6:
		default:
			break;

		case 7:
			pBlockFlower = BlockManager::mushroomRed;
			break;

		case 8:
			pBlockFlower = BlockManager::mushroomBrown;
		}

		//var5.addTranslation(var14 / 16.0F, var15 / 16.0F, var16 / 16.0F);

		if (pBlockFlower != NULL)
		{
			renderBlockByRenderType(pBlockFlower, pos);
		}
		else if (metadata == 9)
		{
			renderAllFaces = true;
			float var18 = 0.125F;
			setRenderBounds((0.5F - var18), 0.0F, (0.5F - var18),(0.5F + var18), 0.25F, (0.5F + var18));
			renderStandardBlock(BlockManager::cactus, pcBlock, pos);
			setRenderBounds((0.5F - var18), 0.25F, (0.5F - var18), (0.5F + var18), 0.5F, (0.5F + var18));
			renderStandardBlock(BlockManager::cactus, pcBlock, pos);
			setRenderBounds((0.5F - var18), 0.5F, (0.5F - var18), (0.5F + var18), 0.75F, (0.5F + var18));
			renderStandardBlock(BlockManager::cactus, pcBlock, pos);
			renderAllFaces = false;
			setRenderBounds(0.0F, 0.0F, 0.0F, 1.0F, 1.0F, 1.0F);
		}
		else if (metadata == 3)
		{
			drawCrossedSquares(BlockManager::sapling, cBlockManager::scBlocks[BLOCK_ID_SAPLING], 0, Vector3(pos), 0.75F, color, bright);
		}
		else if (metadata == 5)
		{
			drawCrossedSquares(BlockManager::sapling, cBlockManager::scBlocks[BLOCK_ID_SAPLING], 2, Vector3(pos), 0.75F, color, bright);
		}
		else if (metadata == 4)
		{
			drawCrossedSquares(BlockManager::sapling, cBlockManager::scBlocks[BLOCK_ID_SAPLING], 1, Vector3(pos), 0.75F, color, bright);
		}
		else if (metadata == 6)
		{
			drawCrossedSquares(BlockManager::sapling, cBlockManager::scBlocks[BLOCK_ID_SAPLING], 3, Vector3(pos), 0.75F, color, bright);
		}
		else if (metadata == 11)
		{
			color = cBlockManager::scBlocks[BLOCK_ID_TALL_GRASS]->colorMultiplier(blockAccess, pos);
			r = (float)(color >> 16 & 255) / 255.0F;
			g = (float)(color >> 8 & 255) / 255.0F;
			b = (float)(color & 255) / 255.0F;
			//var5.setColorOpaque_F(var6 * var9, var6 * var10, var6 * var11);
			color = Color(r, g, b).getARGB();
			drawCrossedSquares(BlockManager::tallGrass, cBlockManager::scBlocks[BLOCK_ID_TALL_GRASS], 2, Vector3(pos), 0.75F, color, bright);
		}
		else if (metadata == 10)
		{
			drawCrossedSquares(BlockManager::deadBush, cBlockManager::scBlocks[BLOCK_ID_DEADBUSH], 2, Vector3(pos), 0.75F, color, bright);
		}
	}

	return true;
}

bool RenderBlocks::renderBlockBed(Block* pBlock, cBlock* pcBlock, const BlockPos& pos)
{	
	int metadata = blockAccess->getBlockMeta(pos);
	int direction = BlockBed::getDirection(metadata);
	bool isHeadOfBed = BlockBed::isBlockHeadOfBed(metadata);
	float var9 = 0.5F;
	float var10 = 1.0F;
	float var11 = 0.8F;
	float var12 = 0.6F;
	int bright = pcBlock->getMixedBrightnessForBlock(blockAccess, pos);
	int color = Color(var9, var9, var9).getARGB();	
	AtlasSprite* sprite = getBlockIcon(pcBlock, blockAccess, pos, 0);

	float min_u = sprite->getMinU();
	float max_u = sprite->getMaxU();
	float min_v = sprite->getMinV();
	float max_v = sprite->getMaxV();
	float var36 = pos.x + renderMinX;
	float var38 = pos.x + renderMaxX;
	float var40 = pos.y + renderMinY + 0.1875F;
	float var42 = pos.z + renderMinZ;
	float var44 = pos.z + renderMaxZ;

	int pass = pcBlock->getRenderBlockPass();
	TessRenderable* renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite->getTexture(), (TESS_RENDER_PASS)pass, 1);
	TessVertexFmt* pVetLockPtr = NULL;
	if(!renderable->getLockPtr(1, pVetLockPtr))
		return false;

	pVetLockPtr[0].setEx(m_sectionPos, var36, var40, var44, color, bright, min_u, max_v);
	pVetLockPtr[1].setEx(m_sectionPos, var36, var40, var42, color, bright, min_u, min_v);
	pVetLockPtr[2].setEx(m_sectionPos, var38, var40, var42, color, bright, max_u, min_v);
	pVetLockPtr[3].setEx(m_sectionPos, var38, var40, var44, color, bright, max_u, max_v);
	//AutoCaculateQuadNormal(pVetLockPtr + 0);

	bright = pcBlock->getMixedBrightnessForBlock(blockAccess, pos.getPosY());
	color = Color(var10, var10, var10).getARGB();
	sprite = getBlockIcon(pcBlock, blockAccess, pos, 1);
	min_u = (float)sprite->getMinU();
	max_u = (float)sprite->getMaxU();
	min_v = (float)sprite->getMinV();
	max_v = (float)sprite->getMaxV();

	var36 = min_u;
	var38 = max_u;
	var40 = min_v;
	var42 = min_v;
	var44 = min_u;
	float var46 = max_u;
	float var48 = max_v;
	float var50 = max_v;

	if (direction == 0)
	{
		var38 = min_u;
		var40 = max_v;
		var44 = max_u;
		var50 = min_v;
	}
	else if (direction == 2)
	{
		var36 = max_u;
		var42 = max_v;
		var46 = min_u;
		var48 = min_v;
	}
	else if (direction == 3)
	{
		var36 = max_u;
		var42 = max_v;
		var46 = min_u;
		var48 = min_v;
		var38 = min_u;
		var40 = max_v;
		var44 = max_u;
		var50 = min_v;
	}

	float var52 = pos.x + renderMinX;
	float var54 = pos.x + renderMaxX;
	float var56 = pos.y + renderMaxY;
	float var58 = pos.z + renderMinZ;
	float var60 = pos.z + renderMaxZ;

	renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite->getTexture(), (TESS_RENDER_PASS)pass, 1);
	pVetLockPtr = NULL;
	if(!renderable->getLockPtr(1, pVetLockPtr))
		return false;

	pVetLockPtr[0].setEx(m_sectionPos, var54, var56, var60, color, bright, var44, var48);
	pVetLockPtr[1].setEx(m_sectionPos, var54, var56, var58, color, bright, var36, var40);
	pVetLockPtr[2].setEx(m_sectionPos, var52, var56, var58, color, bright, var38, var42);
	pVetLockPtr[3].setEx(m_sectionPos, var52, var56, var60, color, bright, var46, var50);
	//AutoCaculateQuadNormal(pVetLockPtr + 0);

	int var62 = dir_directionToFacing[direction];

	if (isHeadOfBed)
	{
		var62 = dir_directionToFacing[dir_rotateOpposite[direction]];
	}

	int var63 = 4;

	switch (direction)
	{
	case 0:
		var63 = 5;
		break;
	case 1:
		var63 = 3;
		break;
	case 2:
		var63 = 4;
		break;
	case 3:
		var63 = 2;
	default:
		break;
	}

	if (var62 != 2 && (renderAllFaces || pBlock->shouldSideBeRendered(blockAccess, pos.getNegZ(), 2)))
	{
		setBrightness(renderMinZ > 0.0F ? bright : pcBlock->getMixedBrightnessForBlock(blockAccess, pos.getNegZ()));
		setColor(var11, var11, var11);
		flipTexture = var63 == 2;
		renderFaceZNeg(pcBlock, Vector3(pos), getBlockIcon(pcBlock, blockAccess, pos, 2));
	}

	if (var62 != 3 && (renderAllFaces || pBlock->shouldSideBeRendered(blockAccess, pos.getPosZ(), 3)))
	{
		setBrightness(renderMaxZ < 1.0F ? bright : pcBlock->getMixedBrightnessForBlock(blockAccess, pos.getPosZ()));
		setColor(var11, var11, var11);
		flipTexture = var63 == 3;
		renderFaceZPos(pcBlock, Vector3(pos), getBlockIcon(pcBlock, blockAccess, pos, 3));
	}

	if (var62 != 4 && (renderAllFaces || pBlock->shouldSideBeRendered(blockAccess, pos.getNegX(), 4)))
	{
		setBrightness(renderMinZ > 0.0F ? bright : pcBlock->getMixedBrightnessForBlock(blockAccess, pos.getNegX()));
		setColor(var12, var12, var12);
		flipTexture = var63 == 4;
		renderFaceXNeg(pcBlock, Vector3(pos), getBlockIcon(pcBlock, blockAccess, pos, 4));
	}

	if (var62 != 5 && (renderAllFaces || pBlock->shouldSideBeRendered(blockAccess, pos.getPosX(), 5)))
	{
		setBrightness(renderMaxZ < 1.0F ? bright : pcBlock->getMixedBrightnessForBlock(blockAccess, pos.getPosX()));
		setColor(var12, var12, var12);
		flipTexture = var63 == 5;
		renderFaceXPos(pcBlock, Vector3(pos), getBlockIcon(pcBlock, blockAccess, pos, 5));
	}

	flipTexture = false;
	return true;
}

bool RenderBlocks::renderBlockMinecartTrack(Block* pBlock, cBlock* pcBlock, const BlockPos& pos)
{
	//Tessellator var5 = Tessellator.instance;
	BlockRailBase* pBlockRailBase = (BlockRailBase*)(pBlock);
	int metadata = blockAccess->getBlockMeta(pos);
	AtlasSprite* sprite = getBlockIconFromSideAndMetadata(cBlockManager::scBlocks[pBlock->getBlockID()], 0, metadata);

	if (hasOverrideBlockTexture())
	{
		sprite = overrideBlockTexture;
	}

	if (pBlockRailBase->isPowered())
	{
		metadata &= 7;
	}

	//var5.setBrightness(par1BlockRailBase.getMixedBrightnessForBlock(this.blockAccess, pos));
	int bright = pcBlock->getMixedBrightnessForBlock(blockAccess, pos);
	//var5.setColorOpaque_F(1.0F, 1.0F, 1.0F);
	int color = Color(1.0F, 1.0F, 1.0F).getARGB();
	float min_u = sprite->getMinU();
	float min_v = sprite->getMinV();
	float max_u = sprite->getMaxU();
	float max_v = sprite->getMaxV();
	float var16 = 0.0625F;
	float var18 = pos.x + 1.f;
	float var20 = pos.x + 1.f;
	float var22 = pos.x + 0.f;
	float var24 = pos.x + 0.f;
	float var26 = pos.z + 0.f;
	float var28 = pos.z + 1.f;
	float var30 = pos.z + 1.f;
	float var32 = pos.z + 0.f;
	float var34 = pos.y + var16;
	float var36 = pos.y + var16;
	float var38 = pos.y + var16;
	float var40 = pos.y + var16;

	if (metadata != 1 && metadata != 2 && metadata != 3 && metadata != 7)
	{
		if (metadata == 8)
		{
			var18 = var20 = pos.x + 0.f;
			var22 = var24 = pos.x + 1.f;
			var26 = var32 = pos.z + 1.f;
			var28 = var30 = pos.z + 0.f;
		}
		else if (metadata == 9)
		{
			var18 = var24 = pos.x + 0.f;
			var20 = var22 = pos.x + 1.f;
			var26 = var28 = pos.z + 0.f;
			var30 = var32 = pos.z + 1.f;
		}
	}
	else
	{
		var18 = var24 = pos.x + 1.f;
		var20 = var22 = pos.x + 0.f;
		var26 = var28 = pos.z + 1.f;
		var30 = var32 = pos.z + 0.f;
	}

	if (metadata != 2 && metadata != 4)
	{
		if (metadata == 3 || metadata == 5)
		{
			++var36;
			++var38;
		}
	}
	else
	{
		++var34;
		++var40;
	}

	int pass = pcBlock->getRenderBlockPass();
	TessRenderable* renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite->getTexture(), (TESS_RENDER_PASS)pass, 2);
	TessVertexFmt* pVetLockPtr = NULL;
	if (!renderable->getLockPtr(2, pVetLockPtr))
		return false;

	pVetLockPtr[0].setEx(m_sectionPos, var18, var34, var26, color, bright, max_u, min_v);
	pVetLockPtr[1].setEx(m_sectionPos, var20, var36, var28, color, bright, max_u, max_v);
	pVetLockPtr[2].setEx(m_sectionPos, var22, var38, var30, color, bright, min_u, max_v);
	pVetLockPtr[3].setEx(m_sectionPos, var24, var40, var32, color, bright, min_u, min_v);
	//AutoCaculateQuadNormal(pVetLockPtr + 0);

	pVetLockPtr[4].setEx(m_sectionPos, var24, var40, var32, color, bright, min_u, min_v);
	pVetLockPtr[5].setEx(m_sectionPos, var22, var38, var30, color, bright, min_u, max_v);
	pVetLockPtr[6].setEx(m_sectionPos, var20, var36, var28, color, bright, max_u, max_v);
	pVetLockPtr[7].setEx(m_sectionPos, var18, var34, var26, color, bright, max_u, min_v);
	//AutoCaculateQuadNormal(pVetLockPtr + 4);
	return true;
}

bool RenderBlocks::renderPistonBase(Block* pBlock, cBlock* pcBlock, const BlockPos& pos, bool par5)
{	
	BlockPistonBase* pBlockPistonBase = (BlockPistonBase*)(pBlock);
	int metadata = blockAccess->getBlockMeta(pos);
	bool isExtended = par5 || (metadata & 8) != 0;
	int d = BlockPistonBase::getOrientation(metadata);	

	if (isExtended)
	{
		switch (d)
		{
		case 0:
			uvRotateEast = 3;
			uvRotateWest = 3;
			uvRotateSouth = 3;
			uvRotateNorth = 3;
			setRenderBounds(0.0F, 0.25F, 0.0F, 1.0F, 1.0F, 1.0F);
			break;

		case 1:
			setRenderBounds(0.0F, 0.0F, 0.0F, 1.0F, 0.75F, 1.0F);
			break;

		case 2:
			uvRotateSouth = 1;
			uvRotateNorth = 2;
			setRenderBounds(0.0F, 0.0F, 0.25F, 1.0F, 1.0F, 1.0F);
			break;

		case 3:
			uvRotateSouth = 2;
			uvRotateNorth = 1;
			uvRotateTop = 3;
			uvRotateBottom = 3;
			setRenderBounds(0.0F, 0.0F, 0.0F, 1.0F, 1.0F, 0.75F);
			break;

		case 4:
			uvRotateEast = 1;
			uvRotateWest = 2;
			uvRotateTop = 2;
			uvRotateBottom = 1;
			setRenderBounds(0.25F, 0.0F, 0.0F, 1.0F, 1.0F, 1.0F);
			break;

		case 5:
			uvRotateEast = 2;
			uvRotateWest = 1;
			uvRotateTop = 1;
			uvRotateBottom = 2;
			setRenderBounds(0.0F, 0.0F, 0.0F, 0.75F, 1.0F, 1.0F);
		}

		pBlockPistonBase->setBlockBounds(renderMinX, renderMinY, renderMinZ, renderMaxX, renderMaxY, renderMaxZ);
		renderStandardBlock(pBlockPistonBase, pcBlock, pos);
		uvRotateEast = 0;
		uvRotateWest = 0;
		uvRotateSouth = 0;
		uvRotateNorth = 0;
		uvRotateTop = 0;
		uvRotateBottom = 0;
		setRenderBounds(0.0F, 0.0F, 0.0F, 1.0F, 1.0F, 1.0F);
		pBlockPistonBase->setBlockBounds(renderMinX, renderMinY, renderMinZ, renderMaxX, renderMaxY, renderMaxZ);
	}
	else
	{
		switch (d)
		{
		case 0:
			uvRotateEast = 3;
			uvRotateWest = 3;
			uvRotateSouth = 3;
			uvRotateNorth = 3;

		case 1:
		default:
			break;

		case 2:
			uvRotateSouth = 1;
			uvRotateNorth = 2;
			break;

		case 3:
			uvRotateSouth = 2;
			uvRotateNorth = 1;
			uvRotateTop = 3;
			uvRotateBottom = 3;
			break;

		case 4:
			uvRotateEast = 1;
			uvRotateWest = 2;
			uvRotateTop = 2;
			uvRotateBottom = 1;
			break;

		case 5:
			uvRotateEast = 2;
			uvRotateWest = 1;
			uvRotateTop = 1;
			uvRotateBottom = 2;
		}

		renderStandardBlock(pBlockPistonBase, pcBlock, pos);
		uvRotateEast = 0;
		uvRotateWest = 0;
		uvRotateSouth = 0;
		uvRotateNorth = 0;
		uvRotateTop = 0;
		uvRotateBottom = 0;
	}

	return true;
}

bool RenderBlocks::renderPistonExtension(Block* pBlock, cBlock* pcBlock, const BlockPos& pos, bool par5)
{
	int metadata = blockAccess->getBlockMeta(pos);
	int d = BlockPistonExtension::getDirectionMeta(metadata);
	float var8 = 0.25F;
	float var9 = 0.375F;
	float var10 = 0.625F;
	float bright = pcBlock->getBlockBrightness(blockAccess, pos);
	float var12 = par5 ? 1.0F : 0.5F;
	float var13 = par5 ? 16.0F : 8.0F;

	float x1 = pos.x + 0.625F;
	float x0 = pos.x + 0.375F;
	float z1 = pos.z + 0.625f;
	float z0 = pos.z + 0.375f;
	float y1 = pos.y + 0.625f;
	float y0 = pos.y + 0.375f;
	float f0, f1;

	switch (d)
	{
	case 0:
		uvRotateEast = 3;
		uvRotateWest = 3;
		uvRotateSouth = 3;
		uvRotateNorth = 3;
		setRenderBounds(0.0F, 0.0F, 0.0F, 1.0F, 0.25F, 1.0F);
		renderStandardBlock(pBlock, pcBlock, pos);

		f0 = pos.y + 0.25f;
		f1 = pos.y + 0.25f + var12;
		renderPistonRodUD(x0, x1, f0, f1, z1, z1, bright * 0.8F, var13);
		renderPistonRodUD(x1, x0, f0, f1, z0, z0, bright * 0.8F, var13);
		renderPistonRodUD(x0, x0, f0, f1, z0, z1, bright * 0.6F, var13);
		renderPistonRodUD(x1, x1, f0, f1, z1, z0, bright * 0.6F, var13);
		break;

	case 1:
		setRenderBounds(0.0F, 0.75F, 0.0F, 1.0F, 1.0F, 1.0F);
		renderStandardBlock(pBlock, pcBlock, pos);

		f0 = pos.y - 0.25f + 1.f - var12;
		f1 = pos.y - 0.25f + 1.f;
		renderPistonRodUD(x0, x1, f0, f1, z1, z1, bright * 0.8F, var13);
		renderPistonRodUD(x1, x0, f0, f1, z0, z0, bright * 0.8F, var13);
		renderPistonRodUD(x0, x0, f0, f1, z0, z1, bright * 0.6F, var13);
		renderPistonRodUD(x1, x1, f0, f1, z1, z0, bright * 0.6F, var13);
		break;

	case 2:
		uvRotateSouth = 1;
		uvRotateNorth = 2;
		setRenderBounds(0.0F, 0.0F, 0.0F, 1.0F, 1.0F, 0.25F);
		renderStandardBlock(pBlock, pcBlock, pos);

		f0 = pos.z + 0.25f;
		f1 = pos.z + 0.25f + var12;
		renderPistonRodSN(x0, x0, y1, y0, f0, f1, bright * 0.6F, var13);
		renderPistonRodSN(x1, x1, y0, y1, f0, f1, bright * 0.6F, var13);
		renderPistonRodSN(x0, x1, y0, y0, f0, f1, bright * 0.5F, var13);
		renderPistonRodSN(x1, x0, y1, y1, f0, f1, bright, var13);
		break;

	case 3:
		uvRotateSouth = 2;
		uvRotateNorth = 1;
		uvRotateTop = 3;
		uvRotateBottom = 3;
		setRenderBounds(0.0F, 0.0F, 0.75F, 1.0F, 1.0F, 1.0F);
		renderStandardBlock(pBlock, pcBlock, pos);

		f0 = pos.z - 0.25f + 1.f - var12;
		f1 = pos.z - 0.25f + 1.f;
		renderPistonRodSN(x0, x0, y1, y0, f0, f1, bright * 0.6F, var13);
		renderPistonRodSN(x1, x1, y0, y1, f0, f1, bright * 0.6F, var13);
		renderPistonRodSN(x0, x1, y0, y0, f0, f1, bright * 0.5F, var13);
		renderPistonRodSN(x1, x0, y1, y1, f0, f1, bright, var13);
		break;

	case 4:
		uvRotateEast = 1;
		uvRotateWest = 2;
		uvRotateTop = 2;
		uvRotateBottom = 1;
		setRenderBounds(0.0F, 0.0F, 0.0F, 0.25F, 1.0F, 1.0F);
		renderStandardBlock(pBlock, pcBlock, pos);

		f0 = pos.x + 0.25f;
		f1 = pos.x + 0.25f + var12;
		renderPistonRodEW(f0, f1, y0, y0, z1, z0, bright * 0.5F, var13);
		renderPistonRodEW(f0, f1, y1, y1, z0, z1, bright, var13);
		renderPistonRodEW(f0, f1, y0, y1, z0, z0, bright * 0.6F, var13);
		renderPistonRodEW(f0, f1, y1, y0, z1, z1, bright * 0.6F, var13);
		break;

	case 5:
		uvRotateEast = 2;
		uvRotateWest = 1;
		uvRotateTop = 1;
		uvRotateBottom = 2;
		setRenderBounds(0.75F, 0.0F, 0.0F, 1.0F, 1.0F, 1.0F);
		renderStandardBlock(pBlock, pcBlock, pos);

		f0 = pos.x - 0.25f + 1.f - var12;
		f1 = pos.x - 0.25f + 1.f;
		renderPistonRodEW(f0, f1, y0, y0, z1, z0, bright * 0.5F, var13);
		renderPistonRodEW(f0, f1, y1, y1, z0, z1, bright, var13);
		renderPistonRodEW(f0, f1, y0, y1, z0, z0, bright * 0.6F, var13);
		renderPistonRodEW(f0, f1, y1, y0, z1, z1, bright * 0.6F, var13);
	}

	uvRotateEast = 0;
	uvRotateWest = 0;
	uvRotateSouth = 0;
	uvRotateNorth = 0;
	uvRotateTop = 0;
	uvRotateBottom = 0;
	setRenderBounds(0.0F, 0.0F, 0.0F, 1.0F, 1.0F, 1.0F);
	return true;
}


bool RenderBlocks::renderBlockTorch(Block* pBlock, cBlock* pcBlock, const BlockPos& pos)
{
	int metadata = blockAccess->getBlockMeta(pos);
	
	setOABrightness(pcBlock->getMixedBrightnessForBlock(blockAccess, pos));
	setOAColor(1.0F, 1.0F, 1.0F);
	float var7 = 0.4000000059604645;
	float var9 = 0.5F - var7;
	float var11 = 0.20000000298023224;

	if (metadata == 1)
	{
		renderTorchAtAngle(pBlock, pcBlock, Vector3(pos) + Vector3(-var9, var11, 0.f), -var7, 0.0F, 0);
	}
	else if (metadata == 2)
	{
		renderTorchAtAngle(pBlock, pcBlock, Vector3(pos) + Vector3(var9, var11, 0.f), var7, 0.0F, 0);
	}
	else if (metadata == 3)
	{
		renderTorchAtAngle(pBlock, pcBlock, Vector3(pos) + Vector3(0.f, var11, -var9), 0.0F, -var7, 0);
	}
	else if (metadata == 4)
	{
		renderTorchAtAngle(pBlock, pcBlock, Vector3(pos) + Vector3(0.f, var11, var9), 0.0F, var7, 0);
	}
	else
	{
		renderTorchAtAngle(pBlock, pcBlock, Vector3(pos), 0.0F, 0.0F, 0);
	}

	return true;
}

bool RenderBlocks::renderBlockFire(Block* pBlock, cBlock* pcBlock, const BlockPos& pos)
{	
	cBlockFire* pBlockFire = (cBlockFire*)(pBlock);
	AtlasSprite* sprite_0 = getBlockIconFromSide(pBlockFire, 0);
	AtlasSprite* sprite_1 = getBlockIconFromSide(pBlockFire, 1);
	AtlasSprite* pSprite = sprite_0;

	if (hasOverrideBlockTexture())
	{
		pSprite = overrideBlockTexture;
	}

	int color = Color(1.0F, 1.0F, 1.0F).getARGB();	
	int bright = pcBlock->getMixedBrightnessForBlock(blockAccess, pos);
		
	float min_u = pSprite->getMinU();
	float min_v = pSprite->getMinV();
	float max_u = pSprite->getMaxU();
	float max_v = pSprite->getMaxV();
	float var17 = 1.4F;
	float var20;
	float var22;
	float var24;
	float var26;
	float var28;
	float var30;
	float var32;

	TESS_RENDER_PASS pass = TRP_SOLID_UV;

	if (!blockAccess->doesBlockHaveSolidTopSurface(pos.getNegY()) && !pBlockFire->canBlockCatchFire(blockAccess, pos.getNegY()))
	{
		float var36 = 0.2F;
		float var19 = 0.0625F;

		if ((pos.x + pos.y + pos.z & 1) == 1)
		{
			min_u = sprite_1->getMinU();
			min_v = sprite_1->getMinV();
			max_u = sprite_1->getMaxU();
			max_v = sprite_1->getMaxV();
			pSprite = sprite_1;
		}

		if ((pos.x / 2 + pos.y / 2 + pos.z / 2 & 1) == 1)
		{
			var20 = max_u;
			max_u = min_u;
			min_u = var20;
		}

		

		if (pBlockFire->canBlockCatchFire(blockAccess, pos.getNegX()))
		{
			TessRenderable* renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(pSprite->getTexture(), pass, 2);
			TessVertexFmt* pVetLockPtr = NULL;
			if(!renderable->getLockPtr(2, pVetLockPtr))
				return false;

			float x0 = pos.x + 0.f;
			float x1 = pos.x + var36;
			float y0 = pos.y + var19;
			float y1 = pos.y + var17 + var19;
			float z0 = pos.z + 0.f;
			float z1 = pos.z + 1.f;

			pVetLockPtr[0].setEx(m_sectionPos, x1, y1, z1, color, bright, max_u, min_v);
			pVetLockPtr[1].setEx(m_sectionPos, x0, y0, z1, color, bright, max_u, max_v);
			pVetLockPtr[2].setEx(m_sectionPos, x0, y0, z0, color, bright, min_u, max_v);
			pVetLockPtr[3].setEx(m_sectionPos, x1, y1, z0, color, bright, min_u, min_v);
			//AutoCaculateQuadNormal(pVetLockPtr + 0);

			pVetLockPtr[4].setEx(m_sectionPos, x1, y1, z0, color, bright, min_u, min_v);
			pVetLockPtr[5].setEx(m_sectionPos, x0, y0, z0, color, bright, min_u, max_v);
			pVetLockPtr[6].setEx(m_sectionPos, x0, y0, z1, color, bright, max_u, max_v);
			pVetLockPtr[7].setEx(m_sectionPos, x1, y1, z1, color, bright, max_u, min_v);
			//AutoCaculateQuadNormal(pVetLockPtr + 4);
		}

		if (pBlockFire->canBlockCatchFire(blockAccess, pos.getPosX()))
		{
			TessRenderable* renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(pSprite->getTexture(), pass, 2);
			TessVertexFmt* pVetLockPtr = NULL;
			if (!renderable->getLockPtr(2, pVetLockPtr))
				return false;

			float x0 = pos.x + 1.f;
			float x1 = pos.x + 1.f - var36;
			float y0 = pos.y + var19;
			float y1 = pos.y + var17 + var19;
			float z0 = pos.z + 0.f;
			float z1 = pos.z + 1.f;

			pVetLockPtr[0].setEx(m_sectionPos, x1, y1, z0, color, bright, min_u, min_v);
			pVetLockPtr[1].setEx(m_sectionPos, x0, y0, z0, color, bright, min_u, max_v);
			pVetLockPtr[2].setEx(m_sectionPos, x0, y0, z1, color, bright, max_u, max_v);
			pVetLockPtr[3].setEx(m_sectionPos, x1, y1, z1, color, bright, max_u, min_v);
			//AutoCaculateQuadNormal(pVetLockPtr + 0);

			pVetLockPtr[4].setEx(m_sectionPos, x1, y1, z1, color, bright, max_u, min_v);
			pVetLockPtr[5].setEx(m_sectionPos, x0, y0, z1, color, bright, max_u, max_v);
			pVetLockPtr[6].setEx(m_sectionPos, x0, y0, z0, color, bright, min_u, max_v);
			pVetLockPtr[7].setEx(m_sectionPos, x1, y1, z0, color, bright, min_u, min_v);
			//AutoCaculateQuadNormal(pVetLockPtr + 4);
		}

		if (pBlockFire->canBlockCatchFire(blockAccess, pos.getNegZ()))
		{
			TessRenderable* renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(pSprite->getTexture(), pass, 2);
			TessVertexFmt* pVetLockPtr = NULL;
			if (!renderable->getLockPtr(2, pVetLockPtr))
				return false;

			float x0 = pos.x + 0.f;
			float x1 = pos.x + 1.f;
			float y0 = pos.y + var19;
			float y1 = pos.y + var17 + var19;
			float z0 = pos.z + 0.f;
			float z1 = pos.z + var36;

			pVetLockPtr[0].setEx(m_sectionPos, x0, y1, z1, color, bright, max_u, min_v);
			pVetLockPtr[1].setEx(m_sectionPos, x0, y0, z0, color, bright, max_u, max_v);
			pVetLockPtr[2].setEx(m_sectionPos, x1, y0, z0, color, bright, min_u, max_v);
			pVetLockPtr[3].setEx(m_sectionPos, x1, y1, z1, color, bright, min_u, min_v);
			//AutoCaculateQuadNormal(pVetLockPtr + 0);

			pVetLockPtr[4].setEx(m_sectionPos, x1, y1, z1, color, bright, min_u, min_v);
			pVetLockPtr[5].setEx(m_sectionPos, x1, y0, z0, color, bright, min_u, max_v);
			pVetLockPtr[6].setEx(m_sectionPos, x0, y0, z0, color, bright, max_u, max_v);
			pVetLockPtr[7].setEx(m_sectionPos, x0, y1, z1, color, bright, max_u, min_v);
			//AutoCaculateQuadNormal(pVetLockPtr + 4);
		}

		if (pBlockFire->canBlockCatchFire(blockAccess, pos.getPosZ()))
		{
			TessRenderable* renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(pSprite->getTexture(), pass, 2);
			TessVertexFmt* pVetLockPtr = NULL;
			if (!renderable->getLockPtr(2, pVetLockPtr))
				return false;

			float x0 = pos.x + 0.f;
			float x1 = pos.x + 1.f;
			float y0 = pos.y + var19;
			float y1 = pos.y + var17 + var19;
			float z0 = pos.z + 1.f;
			float z1 = pos.z + 1.f - var36;

			pVetLockPtr[0].setEx(m_sectionPos, x1, y1, z1, color, bright, min_u, min_v);
			pVetLockPtr[1].setEx(m_sectionPos, x1, y0, z0, color, bright, min_u, max_v);
			pVetLockPtr[2].setEx(m_sectionPos, x0, y0, z0, color, bright, max_u, max_v);
			pVetLockPtr[3].setEx(m_sectionPos, x0, y1, z1, color, bright, max_u, min_v);
			//AutoCaculateQuadNormal(pVetLockPtr + 0);

			pVetLockPtr[4].setEx(m_sectionPos, x0, y1, z1, color, bright, max_u, min_v);
			pVetLockPtr[5].setEx(m_sectionPos, x0, y0, z0, color, bright, max_u, max_v);
			pVetLockPtr[6].setEx(m_sectionPos, x1, y0, z0, color, bright, min_u, max_v);
			pVetLockPtr[7].setEx(m_sectionPos, x1, y1, z1, color, bright, min_u, min_v);
			//AutoCaculateQuadNormal(pVetLockPtr + 4);
		}

		if (pBlockFire->canBlockCatchFire(blockAccess, pos.getPosY()))
		{
			var20 = pos.x + 0.5F + 0.5F;
			var22 = pos.x + 0.5F - 0.5F;
			var24 = pos.z + 0.5F + 0.5F;
			var26 = pos.z + 0.5F - 0.5F;
			var28 = pos.x + 0.5F - 0.5F;
			var30 = pos.x + 0.5F + 0.5F;
			var32 = pos.z + 0.5F - 0.5F;
			float var34 = pos.z + 0.5F + 0.5F;
			min_u = sprite_0->getMinU();
			min_v = sprite_0->getMinV();
			max_u = sprite_0->getMaxU();
			max_v = sprite_0->getMaxV();
			BlockPos pos1 = pos.getPosY();
			var17 = -0.2F;

			TessRenderable* renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite_0->getTexture(), pass, 1);
			TessVertexFmt* pVetLockPtr = NULL;
			if (!renderable->getLockPtr(1, pVetLockPtr))
				return false;

			float y0 = pos1.y + 0.f;
			float y1 = pos1.y + var17;
			float z0 = pos1.z + 0.f;
			float z1 = pos1.z + 1.f;
			float x0 = pos1.x + 0.f;
			float x1 = pos1.x + 1.f;
			if ((pos1.x + pos1.y + pos1.z & 1) == 0)
			{
				pVetLockPtr[0].setEx(m_sectionPos, var28, y1, z0, color, bright, max_u, min_v);
				pVetLockPtr[1].setEx(m_sectionPos, var20, y0, z0, color, bright, max_u, max_v);
				pVetLockPtr[2].setEx(m_sectionPos, var20, y0, z1, color, bright, min_u, max_v);
				pVetLockPtr[3].setEx(m_sectionPos, var28, y1, z1, color, bright, min_u, min_v);
				//AutoCaculateQuadNormal(pVetLockPtr + 0);

				min_u = sprite_1->getMinU();
				min_v = sprite_1->getMinV();
				max_u = sprite_1->getMaxU();
				max_v = sprite_1->getMaxV();

				renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite_1->getTexture(), pass, 1);
				pVetLockPtr = NULL;
				if (!renderable->getLockPtr(1, pVetLockPtr))
					return false;

				pVetLockPtr[0].setEx(m_sectionPos, var30, y1, z1, color, bright, max_u, min_v);
				pVetLockPtr[1].setEx(m_sectionPos, var22, y0, z1, color, bright, max_u, max_v);
				pVetLockPtr[2].setEx(m_sectionPos, var22, y0, z0, color, bright, min_u, max_v);
				pVetLockPtr[3].setEx(m_sectionPos, var30, y1, z0, color, bright, min_u, min_v);
				//AutoCaculateQuadNormal(pVetLockPtr + 0);
			}
			else
			{
				pVetLockPtr[0].setEx(m_sectionPos, x0, y1, var34, color, bright, max_u, min_v);
				pVetLockPtr[1].setEx(m_sectionPos, x0, y0, var26, color, bright, max_u, max_v);
				pVetLockPtr[2].setEx(m_sectionPos, x1, y0, var26, color, bright, min_u, max_v);
				pVetLockPtr[3].setEx(m_sectionPos, x1, y1, var34, color, bright, min_u, min_v);
				//AutoCaculateQuadNormal(pVetLockPtr + 0);

				min_u = sprite_1->getMinU();
				min_v = sprite_1->getMinV();
				max_u = sprite_1->getMaxU();
				max_v = sprite_1->getMaxV();

				renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite_1->getTexture(), pass, 1);
				pVetLockPtr = NULL;
				if (!renderable->getLockPtr(1, pVetLockPtr))
					return false;
				
				pVetLockPtr[0].setEx(m_sectionPos, x1, y1, var32, color, bright, max_u, min_v);
				pVetLockPtr[1].setEx(m_sectionPos, x1, y0, var24, color, bright, max_u, max_v);
				pVetLockPtr[2].setEx(m_sectionPos, x0, y0, var24, color, bright, min_u, max_v);
				pVetLockPtr[3].setEx(m_sectionPos, x0, y1, var32, color, bright, min_u, min_v);
				//AutoCaculateQuadNormal(pVetLockPtr + 0);
			}
		}
	}
	else
	{
		float var18 = pos.x + 0.5F + 0.2F;
		var20 = pos.x + 0.5F - 0.2F;
		var22 = pos.z + 0.5F + 0.2F;
		var24 = pos.z + 0.5F - 0.2F;
		var26 = pos.x + 0.5F - 0.3F;
		var28 = pos.x + 0.5F + 0.3F;
		var30 = pos.z + 0.5F - 0.3F;
		var32 = pos.z + 0.5F + 0.3F;
		
		TessRenderable* renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(pSprite->getTexture(), pass, 2);
		TessVertexFmt* pVetLockPtr = NULL;
		if (!renderable->getLockPtr(2, pVetLockPtr))
			return false;

		float y0 = pos.y + 0.f;
		float y1 = pos.y + var17;
		float x0 = pos.x + 0.f;
		float x1 = pos.x + 1.f;
		float z0 = pos.z + 0.f;
		float z1 = pos.z + 1.f;

		pVetLockPtr[0].setEx(m_sectionPos, var26, y1, z1, color, bright, max_u, min_v);
		pVetLockPtr[1].setEx(m_sectionPos, var18, y0, z1, color, bright, max_u, max_v);
		pVetLockPtr[2].setEx(m_sectionPos, var18, y0, z0, color, bright, min_u, max_v);
		pVetLockPtr[3].setEx(m_sectionPos, var26, y1, z0, color, bright, min_u, min_v);
		//AutoCaculateQuadNormal(pVetLockPtr + 0);

		pVetLockPtr[4].setEx(m_sectionPos, var28, y1, z0, color, bright, max_u, min_v);
		pVetLockPtr[5].setEx(m_sectionPos, var20, y0, z0, color, bright, max_u, max_v);
		pVetLockPtr[6].setEx(m_sectionPos, var20, y0, z1, color, bright, min_u, max_v);
		pVetLockPtr[7].setEx(m_sectionPos, var28, y1, z1, color, bright, min_u, min_v);
		//AutoCaculateQuadNormal(pVetLockPtr + 4);

		//
		min_u = sprite_1->getMinU();
		min_v = sprite_1->getMinV();
		max_u = sprite_1->getMaxU();
		max_v = sprite_1->getMaxV();
		pSprite = sprite_1;

		renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(pSprite->getTexture(), pass, 4);
		pVetLockPtr = NULL;
		if (!renderable->getLockPtr(4, pVetLockPtr))
			return false;

		pVetLockPtr[0].setEx(m_sectionPos, x1, y1, var32, color, bright, max_u, min_v);
		pVetLockPtr[1].setEx(m_sectionPos, x1, y0, var24, color, bright, max_u, max_v);
		pVetLockPtr[2].setEx(m_sectionPos, x0, y0, var24, color, bright, min_u, max_v);
		pVetLockPtr[3].setEx(m_sectionPos, x0, y1, var32, color, bright, min_u, min_v);
		//AutoCaculateQuadNormal(pVetLockPtr + 0);

		pVetLockPtr[4].setEx(m_sectionPos, x0, y1, var30, color, bright, max_u, min_v);
		pVetLockPtr[5].setEx(m_sectionPos, x0, y0, var22, color, bright, max_u, max_v);
		pVetLockPtr[6].setEx(m_sectionPos, x1, y0, var22, color, bright, min_u, max_v);
		pVetLockPtr[7].setEx(m_sectionPos, x1, y1, var30, color, bright, min_u, min_v);
		//AutoCaculateQuadNormal(pVetLockPtr + 4);
		var18 = pos.x + 0.5F - 0.5F;
		var20 = pos.x + 0.5F + 0.5F;
		var22 = pos.z + 0.5F - 0.5F;
		var24 = pos.z + 0.5F + 0.5F;
		var26 = pos.x + 0.5F - 0.4F;
		var28 = pos.x + 0.5F + 0.4F;
		var30 = pos.z + 0.5F - 0.4F;
		var32 = pos.z + 0.5F + 0.4F;
		pVetLockPtr[8].setEx(m_sectionPos, var26, y1, z0, color, bright, min_u, min_v);
		pVetLockPtr[9].setEx(m_sectionPos, var18, y0, z0, color, bright, min_u, max_v);
		pVetLockPtr[10].setEx(m_sectionPos, var18, y0, z1, color, bright, max_u, max_v);
		pVetLockPtr[11].setEx(m_sectionPos, var26, y1, z1, color, bright, max_u, min_v);
		//AutoCaculateQuadNormal(pVetLockPtr + 8);

		pVetLockPtr[12].setEx(m_sectionPos, var28, y1, z1, color, bright, min_u, min_v);
		pVetLockPtr[13].setEx(m_sectionPos, var20, y0, z1, color, bright, min_u, max_v);
		pVetLockPtr[14].setEx(m_sectionPos, var20, y0, z0, color, bright, max_u, max_v);
		pVetLockPtr[15].setEx(m_sectionPos, var28, y1, z0, color, bright, max_u, min_v);
		//AutoCaculateQuadNormal(pVetLockPtr + 12);

		//
		min_u = sprite_0->getMinU();
		min_v = sprite_0->getMinV();
		max_u = sprite_0->getMaxU();
		max_v = sprite_0->getMaxV();
		pSprite = sprite_0;

		renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(pSprite->getTexture(), pass, 2);
		pVetLockPtr = NULL;
		if (!renderable->getLockPtr(2, pVetLockPtr))
			return false;

		pVetLockPtr[0].setEx(m_sectionPos, x0, y1, var32, color, bright, min_u, min_v);
		pVetLockPtr[1].setEx(m_sectionPos, x0, y0, var24, color, bright, min_u, max_v);
		pVetLockPtr[2].setEx(m_sectionPos, x1, y0, var24, color, bright, max_u, max_v);
		pVetLockPtr[3].setEx(m_sectionPos, x1, y1, var32, color, bright, max_u, min_v);
		//AutoCaculateQuadNormal(pVetLockPtr + 0);

		pVetLockPtr[4].setEx(m_sectionPos, x1, y1, var30, color, bright, min_u, min_v);
		pVetLockPtr[5].setEx(m_sectionPos, x1, y0, var22, color, bright, min_u, max_v);
		pVetLockPtr[6].setEx(m_sectionPos, x0, y0, var22, color, bright, max_u, max_v);
		pVetLockPtr[7].setEx(m_sectionPos, x0, y1, var30, color, bright, max_u, min_v);
		//AutoCaculateQuadNormal(pVetLockPtr + 4);
	}

	return true;
}

bool RenderBlocks::renderBlockLever(Block* pBlock, cBlock* pcBlock, const BlockPos& pos)
{
	int metadata = blockAccess->getBlockMeta(pos);
	int d = metadata & 7;
	bool var7 = (metadata & 8) > 0;
	bool var9 = hasOverrideBlockTexture();

	if (!var9)
	{
		setOverrideBlockTexture(getBlockIcon(cBlockManager::scBlocks[BLOCK_ID_COBBLE_STONE]));
	}

	float var10 = 0.25F;
	float var11 = 0.1875F;
	float var12 = 0.1875F;

	if (d == 5)
	{
		setRenderBounds((0.5F - var11), 0.0F, (0.5F - var10), (0.5F + var11), var12, (0.5F + var10));
	}
	else if (d == 6)
	{
		setRenderBounds((0.5F - var10), 0.0F, (0.5F - var11), (0.5F + var10), var12, (0.5F + var11));
	}
	else if (d == 4)
	{
		setRenderBounds((0.5F - var11), (0.5F - var10), (1.0F - var12), (0.5F + var11), (0.5F + var10), 1.0F);
	}
	else if (d == 3)
	{
		setRenderBounds((0.5F - var11), (0.5F - var10), 0.0F, (0.5F + var11), (0.5F + var10), var12);
	}
	else if (d == 2)
	{
		setRenderBounds((1.0F - var12), (0.5F - var10), (0.5F - var11), 1.0F, (0.5F + var10), (0.5F + var11));
	}
	else if (d == 1)
	{
		setRenderBounds(0.0F, (0.5F - var10), (0.5F - var11), var12, (0.5F + var10), (0.5F + var11));
	}
	else if (d == 0)
	{
		setRenderBounds((0.5F - var10), (1.0F - var12), (0.5F - var11), (0.5F + var10), 1.0F, (0.5F + var11));
	}
	else if (d == 7)
	{
		setRenderBounds((0.5F - var11), (1.0F - var12), (0.5F - var10), (0.5F + var11), 1.0F, (0.5F + var10));
	}

	renderStandardBlock(pBlock, pcBlock, pos);

	if (!var9)
	{
		clearOverrideBlockTexture();
	}

	int bright = pcBlock->getMixedBrightnessForBlock(blockAccess, pos);
	float var13 = 1.0F;

	if (BlockManager::sLightValue[pBlock->getBlockID()] > 0)
	{
		var13 = 1.0F;
	}

	int color = Color(var13, var13, var13).getARGB();
	
	AtlasSprite* sprite = getBlockIconFromSide(pcBlock, 0);

	if (hasOverrideBlockTexture())
	{
		sprite = overrideBlockTexture;
	}

	float min_u = sprite->getMinU();
	float min_v = sprite->getMinV();
	float max_u = sprite->getMaxU();
	float max_v = sprite->getMaxV();	
	float var24 = 0.0625F;
	float var25 = 0.0625F;
	float var26 = 0.625F;

	Vector3 vec[8];
	vec[0] = Vector3((-var24), 0.0F, (-var25));
	vec[1] = Vector3(var24, 0.0F, (-var25));
	vec[2] = Vector3(var24, 0.0F, var25);
	vec[3] = Vector3((-var24), 0.0F, var25);
	vec[4] = Vector3((-var24), var26, (-var25));
	vec[5] = Vector3(var24, var26, (-var25));
	vec[6] = Vector3(var24, var26, var25);
	vec[7] = Vector3((-var24), var26, var25);

	for (int i = 0; i < 8; ++i)
	{
		if (var7)
		{
			vec[i].z -= 0.0625F;
			vec[i].rotateAroundX(Math::PI_2 / 9.0F);
		}
		else
		{
			vec[i].z += 0.0625F;
			vec[i].rotateAroundX(-Math::PI_2 / 9.0F);
		}

		if (d == 0 || d == 7)
		{
			vec[i].rotateAroundZ(Math::PI);
		}

		if (d == 6 || d == 0)
		{
			vec[i].rotateAroundY(Math::PI_DIV2);
		}

		if (d > 0 && d < 5)
		{
			vec[i].y -= 0.375F;						
			vec[i].rotateAroundX(Math::PI_DIV2);

			if (d == 4)
			{				
				vec[i].rotateAroundY(0.f);				
			}

			if (d == 3)
			{
				vec[i].rotateAroundY(Math::PI);
			}

			if (d == 2)
			{
				vec[i].rotateAroundY(Math::PI_DIV2);
			}

			if (d == 1)
			{
				vec[i].rotateAroundY(-Math::PI_DIV2);
			}

			vec[i] += Vector3(pos) + Vector3::HALF;
		}
		else if (d != 0 && d != 7)
		{
			vec[i] += Vector3(pos) + Vector3(0.5f, 0.125f, 0.5f);
		}
		else
		{
			vec[i] += Vector3(pos) + Vector3(0.5f, 0.875F, 0.5f);
		}
	}

	Vector3 var32;
	Vector3 var28;
	Vector3 var29;
	Vector3 var30;

	for (int i = 0; i < 6; ++i)
	{
		if (i == 0)
		{
			min_u = sprite->getInterpolatedU(7.0F / 16.0F);
			min_v = sprite->getInterpolatedV(6.0F / 16.0F);
			max_u = sprite->getInterpolatedU(9.0F / 16.0F);
			max_v = sprite->getInterpolatedV(8.0F / 16.0F);
		}
		else if (i == 2)
		{
			min_u = sprite->getInterpolatedU(7.0F / 16.0F);
			min_v = sprite->getInterpolatedV(6.0F / 16.0F);
			max_u = sprite->getInterpolatedU(9.0F / 16.0F);
			max_v = sprite->getMaxV();
		}

		if (i == 0)
		{
			var32 = vec[0];
			var28 = vec[1];
			var29 = vec[2];
			var30 = vec[3];
		}
		else if (i == 1)
		{
			var32 = vec[7];
			var28 = vec[6];
			var29 = vec[5];
			var30 = vec[4];
		}
		else if (i == 2)
		{
			var32 = vec[1];
			var28 = vec[0];
			var29 = vec[4];
			var30 = vec[5];
		}
		else if (i == 3)
		{
			var32 = vec[2];
			var28 = vec[1];
			var29 = vec[5];
			var30 = vec[6];
		}
		else if (i == 4)
		{
			var32 = vec[3];
			var28 = vec[2];
			var29 = vec[6];
			var30 = vec[7];
		}
		else if (i == 5)
		{
			var32 = vec[0];
			var28 = vec[3];
			var29 = vec[7];
			var30 = vec[4];
		}

		int pass = pcBlock->getRenderBlockPass();
		TessRenderable* renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite->getTexture(), (TESS_RENDER_PASS)pass, 1);
		TessVertexFmt* pVetLockPtr = NULL;
		if(!renderable->getLockPtr(1, pVetLockPtr))
			return false;

		pVetLockPtr[0].setEx(m_sectionPos, var32.x, var32.y, var32.z, color, bright, min_u, max_v);
		pVetLockPtr[1].setEx(m_sectionPos, var28.x, var28.y, var28.z, color, bright, max_u, max_v);
		pVetLockPtr[2].setEx(m_sectionPos, var29.x, var29.y, var29.z, color, bright, max_u, min_v);
		pVetLockPtr[3].setEx(m_sectionPos, var30.x, var30.y, var30.z, color, bright, min_u, min_v);
		//AutoCaculateQuadNormal(pVetLockPtr + 0);
	}

	return true;
}

bool RenderBlocks::renderBlockBrewingStand(Block* pBlock, cBlock* pcBlock, const BlockPos& pos)
{
	setRenderBounds(0.4375F, 0.0F, 0.4375F, 0.5625F, 0.875F, 0.5625F);
	renderStandardBlock(pBlock, pcBlock, pos);
	setOverrideBlockTexture(getBlockIconFromSideAndMetadata(pcBlock, 1, 0));
	renderAllFaces = true;
	setRenderBounds(0.5625F, 0.0F, 0.3125F, 0.9375F, 0.125F, 0.6875F);
	renderStandardBlock(pBlock, pcBlock, pos);
	setRenderBounds(0.125F, 0.0F, 0.0625F, 0.5F, 0.125F, 0.4375F);
	renderStandardBlock(pBlock, pcBlock, pos);
	setRenderBounds(0.125F, 0.0F, 0.5625F, 0.5F, 0.125F, 0.9375F);
	renderStandardBlock(pBlock, pcBlock, pos);
	renderAllFaces = false;
	clearOverrideBlockTexture();	
	int bright = pcBlock->getMixedBrightnessForBlock(blockAccess, pos);
	float var6 = 1.0F;
	ui32 color = pcBlock->colorMultiplier(blockAccess, pos);
	float r = (float)(color >> 16 & 255) / 255.0F;
	float g = (float)(color >> 8 & 255) / 255.0F;
	float b = (float)(color & 255) / 255.0F;

	if (RenderEntity::anaglyphEnable)
	{
		float var11 = (r * 30.0F + g * 59.0F + b * 11.0F) / 100.0F;
		float var12 = (r * 30.0F + g * 70.0F) / 100.0F;
		float var13 = (r * 30.0F + b * 70.0F) / 100.0F;
		r = var11;
		g = var12;
		b = var13;
	}

	//var5.setColorOpaque_F(var6 * r, var6 * g, var6 * b);
	AtlasSprite* sprite = getBlockIconFromSideAndMetadata(pcBlock, 0, 0);

	if (hasOverrideBlockTexture())
	{
		sprite = overrideBlockTexture;
	}

	float min_v = sprite->getMinV();
	float max_v = sprite->getMaxV();
	int metadata = blockAccess->getBlockMeta(pos);

	for (int i = 0; i < 3; ++i)
	{
		float var18 = (float)i * Math::PI * 2.0F / 3.0F + (Math::PI / 2.0F);
		float var20 = sprite->getInterpolatedU(8.0F / 16.0F);
		float max_u = sprite->getMaxU();

		if ((metadata & 1 << i) != 0)
		{
			max_u = (float)sprite->getMinU();
		}
		
		float var24 = pos.x + 0.5F;
		float var26 = pos.x + 0.5F + Math::Sin(var18) * 8.0F / 16.0F;
		float var28 = pos.z + 0.5F;
		float var30 = pos.z + 0.5F + Math::Cos(var18) * 8.0F / 16.0F;

		int pass = pcBlock->getRenderBlockPass();
		TessRenderable* renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite->getTexture(), (TESS_RENDER_PASS)pass, 2);
		TessVertexFmt* pVetLockPtr = NULL;
		if(!renderable->getLockPtr(2, pVetLockPtr))
			return false;

		pVetLockPtr[0].setEx(m_sectionPos, var24, pos.y + 1.f, var28, color, bright, var20, min_v);
		pVetLockPtr[1].setEx(m_sectionPos, var24, pos.y + 0.f, var28, color, bright, var20, max_v);
		pVetLockPtr[2].setEx(m_sectionPos, var26, pos.y + 0.f, var30, color, bright, max_u, max_v);
		pVetLockPtr[3].setEx(m_sectionPos, var26, pos.y + 1.f, var30, color, bright, max_u, min_v);
		//AutoCaculateQuadNormal(pVetLockPtr + 0);

		pVetLockPtr[4].setEx(m_sectionPos, var26, pos.y + 1.f, var30, color, bright, max_u, min_v);
		pVetLockPtr[5].setEx(m_sectionPos, var26, pos.y + 0.f, var30, color, bright, max_u, max_v);
		pVetLockPtr[6].setEx(m_sectionPos, var24, pos.y + 0.f, var28, color, bright, var20, max_v);
		pVetLockPtr[7].setEx(m_sectionPos, var24, pos.y + 1.f, var28, color, bright, var20, min_v);
		//AutoCaculateQuadNormal(pVetLockPtr + 4);
	}

	pBlock->setBlockBoundsForItemRender();
	return true;
}

bool RenderBlocks::renderBlockCauldron(Block* pBlock, cBlock* pcBlock, const BlockPos& pos)
{
	renderStandardBlock(pBlock, pcBlock, pos);
	
	int bright = pcBlock->getMixedBrightnessForBlock(blockAccess, pos);
	float var6 = 1.0F;
	ui32 color = pcBlock->colorMultiplier(blockAccess, pos);
	float r = (float)(color >> 16 & 255) / 255.0F;
	float g = (float)(color >> 8 & 255) / 255.0F;
	float b = (float)(color & 255) / 255.0F;
	float var12;

	if (RenderEntity::anaglyphEnable)
	{
		float var11 = (r * 30.0F + g * 59.0F + b * 11.0F) / 100.0F;
		var12 = (r * 30.0F + g * 70.0F) / 100.0F;
		float var13 = (r * 30.0F + b * 70.0F) / 100.0F;
		r = var11;
		g = var12;
		b = var13;
	}

	//var5.setColorOpaque_F(var6 * r, var6 * g, var6 * b);

	setColor(r, g, b);
	setBrightness(bright);

	AtlasSprite* var16 = pcBlock->getSpriteFromSide(2);
	var12 = 0.125F;
	Vector3 vpos(pos);
	renderFaceXPos(pcBlock, vpos - Vector3(1.f - var12, 0.f, 0.f), var16);
	renderFaceXNeg(pcBlock, vpos + Vector3(1.f - var12, 0.f, 0.f), var16);
	renderFaceZPos(pcBlock, vpos - Vector3(0.f, 0.f, 1.f - var12), var16);
	renderFaceZNeg(pcBlock, vpos + Vector3(0.f, 0.f, 1.f - var12), var16);
	AtlasSprite* var17 = getBlockIconFromSideAndMetadata(pcBlock, 0, 1);
	renderFaceYPos(pcBlock, vpos + Vector3(0.f, -1.f + 0.25f, 0.f), var17);
	renderFaceYNeg(pcBlock, vpos + Vector3(0.f, 1.f - 0.75f, 0.f), var17);
	int metadata = blockAccess->getBlockMeta(pos);

	if (metadata > 0)
	{
		AtlasSprite* var15 = getBlockIconFromSide(cBlockManager::scBlocks[BLOCK_ID_WATERMOVING], 0);
		if (metadata > 3)
			metadata = 3;
		pcBlock->getRenderBlockPass();
		auto pCauldron = dynamic_cast<cBlockCauldron*>(pcBlock);
		LordAssert(pCauldron);
		pCauldron->setRenderBlockPass(TRP_TRANSPARENT_UV);
		renderFaceYPos(pcBlock, vpos + Vector3(0.f, -1.f + (6.0F + (float)metadata * 3.0F) / 16.0F, 0.f), var15);
		pCauldron->setRenderBlockPass(TRP_SOLID);
	}

	return true;
}

bool RenderBlocks::renderBlockDragonEgg(Block* pBlock, cBlock* pcBlock, const BlockPos& pos)
{
	bool var5 = false;
	int var6 = 0;

	for (int i = 0; i < 8; ++i)
	{
		int var8 = 0;
		int var9 = 1;

		if (i == 0)
		{
			var8 = 2;
		}

		if (i == 1)
		{
			var8 = 3;
		}

		if (i == 2)
		{
			var8 = 4;
		}

		if (i == 3)
		{
			var8 = 5;
			var9 = 2;
		}

		if (i == 4)
		{
			var8 = 6;
			var9 = 3;
		}

		if (i == 5)
		{
			var8 = 7;
			var9 = 5;
		}

		if (i == 6)
		{
			var8 = 6;
			var9 = 2;
		}

		if (i == 7)
		{
			var8 = 3;
		}

		float var10 = (float)var8 / 16.0F;
		float var11 = 1.0F - (float)var6 / 16.0F;
		float var12 = 1.0F - (float)(var6 + var9) / 16.0F;
		var6 += var9;
		setRenderBounds((float)(0.5F - var10), (float)var12, (float)(0.5F - var10), (float)(0.5F + var10), (float)var11, (float)(0.5F + var10));
		renderStandardBlock(pBlock, pcBlock, pos);
	}

	var5 = true;
	setRenderBounds(0.0F, 0.0F, 0.0F, 1.0F, 1.0F, 1.0F);
	return var5;
}

bool RenderBlocks::renderBlockTripWire(Block* pBlock, cBlock* pcBlock, const BlockPos& pos)
{	
	AtlasSprite* sprite = getBlockIconFromSide(pcBlock, 0);
	int metadata = blockAccess->getBlockMeta(pos);
	bool var8 = (metadata & 4) == 4;
	bool var9 = (metadata & 2) == 2;

	if (hasOverrideBlockTexture())
	{
		sprite = overrideBlockTexture;
	}

	int bright = pcBlock->getMixedBrightnessForBlock(blockAccess, pos);
	float var10 = pcBlock->getBlockBrightness(blockAccess, pos) * 0.75F;
	int color = Color(var10, var10, var10).getARGB();
	float min_u = sprite->getMinU();
	float var13 = sprite->getInterpolatedV((var8 ? 2.0F : 0.0F) / 16.0F);
	float max_u = sprite->getMaxU();
	float var17 = sprite->getInterpolatedV((var8 ? 4.0F : 2.0F) / 16.0F);
	float var19 = (var9 ? 3.5F : 1.5F) / 16.0F;
	bool var21 = BlockTripWire::isConnectedTo(blockAccess, pos, metadata, 1);
	bool var22 = BlockTripWire::isConnectedTo(blockAccess, pos, metadata, 3);
	bool var23 = BlockTripWire::isConnectedTo(blockAccess, pos, metadata, 2);
	bool var24 = BlockTripWire::isConnectedTo(blockAccess, pos, metadata, 0);
	float var25 = 0.03125F;
	float var26 = 0.5F - var25 / 2.0F;
	float var27 = var26 + var25;

	float x0 = pos.x + var26;
	float x1 = pos.x + var27;
	float z0 = pos.z + var26;
	float z1 = pos.z + var27;
	float y0 = pos.y + var19;
	float f0, f1;

	if (!var23 && !var22 && !var24 && !var21)
	{
		var23 = true;
		var24 = true;
	}

	if (var23)
	{
		int pass = pcBlock->getRenderBlockPass();
		TessRenderable* renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite->getTexture(), (TESS_RENDER_PASS)pass, 2);
		TessVertexFmt* pVetLockPtr = NULL;
		if(!renderable->getLockPtr(2, pVetLockPtr))
			return false;

		f0 = pos.z + 0.f;
		f1 = pos.z + 0.25f;
		pVetLockPtr[0].setEx(m_sectionPos, x0, y0, f1, color, bright, min_u, var13);
		pVetLockPtr[1].setEx(m_sectionPos, x1, y0, f1, color, bright, min_u, var17);
		pVetLockPtr[2].setEx(m_sectionPos, x1, y0, f0, color, bright, max_u, var17);
		pVetLockPtr[3].setEx(m_sectionPos, x0, y0, f0, color, bright, max_u, var13);
		//AutoCaculateQuadNormal(pVetLockPtr + 0);

		pVetLockPtr[4].setEx(m_sectionPos, x0, y0, f0, color, bright, max_u, var13);
		pVetLockPtr[5].setEx(m_sectionPos, x1, y0, f0, color, bright, max_u, var17);
		pVetLockPtr[6].setEx(m_sectionPos, x1, y0, f1, color, bright, min_u, var17);
		pVetLockPtr[7].setEx(m_sectionPos, x0, y0, f1, color, bright, min_u, var13);
		//AutoCaculateQuadNormal(pVetLockPtr + 4);
	}

	if (var23 || var24 && !var22 && !var21)
	{
		int pass = pcBlock->getRenderBlockPass();
		TessRenderable* renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite->getTexture(), (TESS_RENDER_PASS)pass, 2);
		TessVertexFmt* pVetLockPtr = NULL;
		if (!renderable->getLockPtr(2, pVetLockPtr))
			return false;

		f0 = pos.z + 0.25f;
		f1 = pos.z + 0.5f;
		pVetLockPtr[0].setEx(m_sectionPos, x0, y0, f1, color, bright, min_u, var13);
		pVetLockPtr[1].setEx(m_sectionPos, x1, y0, f1, color, bright, min_u, var17);
		pVetLockPtr[2].setEx(m_sectionPos, x1, y0, f0, color, bright, max_u, var17);
		pVetLockPtr[3].setEx(m_sectionPos, x0, y0, f0, color, bright, max_u, var13);
		//AutoCaculateQuadNormal(pVetLockPtr + 0);

		pVetLockPtr[4].setEx(m_sectionPos, x0, y0, f0, color, bright, max_u, var13);
		pVetLockPtr[5].setEx(m_sectionPos, x1, y0, f0, color, bright, max_u, var17);
		pVetLockPtr[6].setEx(m_sectionPos, x1, y0, f1, color, bright, min_u, var17);
		pVetLockPtr[7].setEx(m_sectionPos, x0, y0, f1, color, bright, min_u, var13);
		//AutoCaculateQuadNormal(pVetLockPtr + 4);
	}

	if (var24 || var23 && !var22 && !var21)
	{
		int pass = pcBlock->getRenderBlockPass();
		TessRenderable* renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite->getTexture(), (TESS_RENDER_PASS)pass, 2);
		TessVertexFmt* pVetLockPtr = NULL;
		if (!renderable->getLockPtr(2, pVetLockPtr))
			return false;

		f0 = pos.z + 0.5f;
		f1 = pos.z + 0.75f;
		pVetLockPtr[0].setEx(m_sectionPos, x0, y0, f1, color, bright, min_u, var13);
		pVetLockPtr[1].setEx(m_sectionPos, x1, y0, f1, color, bright, min_u, var17);
		pVetLockPtr[2].setEx(m_sectionPos, x1, y0, f0, color, bright, max_u, var17);
		pVetLockPtr[3].setEx(m_sectionPos, x0, y0, f0, color, bright, max_u, var13);
		//AutoCaculateQuadNormal(pVetLockPtr + 0);

		pVetLockPtr[4].setEx(m_sectionPos, x0, y0, f0, color, bright, max_u, var13);
		pVetLockPtr[5].setEx(m_sectionPos, x1, y0, f0, color, bright, max_u, var17);
		pVetLockPtr[6].setEx(m_sectionPos, x1, y0, f1, color, bright, min_u, var17);
		pVetLockPtr[7].setEx(m_sectionPos, x0, y0, f1, color, bright, min_u, var13);
		//AutoCaculateQuadNormal(pVetLockPtr + 4);
	}

	if (var24)
	{
		int pass = pcBlock->getRenderBlockPass();
		TessRenderable* renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite->getTexture(), (TESS_RENDER_PASS)pass, 2);
		TessVertexFmt* pVetLockPtr = NULL;
		if (!renderable->getLockPtr(2, pVetLockPtr))
			return false;

		f0 = pos.z + 0.75f;
		f1 = pos.z + 1.f;
		pVetLockPtr[0].setEx(m_sectionPos, x0, y0, f1, color, bright, min_u, var13);
		pVetLockPtr[1].setEx(m_sectionPos, x1, y0, f1, color, bright, min_u, var17);
		pVetLockPtr[2].setEx(m_sectionPos, x1, y0, f0, color, bright, max_u, var17);
		pVetLockPtr[3].setEx(m_sectionPos, x0, y0, f0, color, bright, max_u, var13);
		//AutoCaculateQuadNormal(pVetLockPtr + 0);

		pVetLockPtr[4].setEx(m_sectionPos, x0, y0, f0, color, bright, max_u, var13);
		pVetLockPtr[5].setEx(m_sectionPos, x1, y0, f0, color, bright, max_u, var17);
		pVetLockPtr[6].setEx(m_sectionPos, x1, y0, f1, color, bright, min_u, var17);
		pVetLockPtr[7].setEx(m_sectionPos, x0, y0, f1, color, bright, min_u, var13);
		//AutoCaculateQuadNormal(pVetLockPtr + 4);
	}

	if (var21)
	{
		int pass = pcBlock->getRenderBlockPass();
		TessRenderable* renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite->getTexture(), (TESS_RENDER_PASS)pass, 2);
		TessVertexFmt* pVetLockPtr = NULL;
		if (!renderable->getLockPtr(2, pVetLockPtr))
			return false;

		f0 = pos.x + 0.f;
		f1 = pos.x + 0.25f;
		pVetLockPtr[0].setEx(m_sectionPos, f0, y0, z1, color, bright, min_u, var17);
		pVetLockPtr[1].setEx(m_sectionPos, f1, y0, z1, color, bright, max_u, var17);
		pVetLockPtr[2].setEx(m_sectionPos, f1, y0, z0, color, bright, max_u, var13);
		pVetLockPtr[3].setEx(m_sectionPos, f0, y0, z0, color, bright, min_u, var13);
		//AutoCaculateQuadNormal(pVetLockPtr + 0);

		pVetLockPtr[4].setEx(m_sectionPos, f0, y0, z0, color, bright, min_u, var13);
		pVetLockPtr[5].setEx(m_sectionPos, f1, y0, z0, color, bright, max_u, var13);
		pVetLockPtr[6].setEx(m_sectionPos, f1, y0, z1, color, bright, max_u, var17);
		pVetLockPtr[7].setEx(m_sectionPos, f0, y0, z1, color, bright, min_u, var17);
		//AutoCaculateQuadNormal(pVetLockPtr + 4);
	}

	if (var21 || var22 && !var23 && !var24)
	{
		int pass = pcBlock->getRenderBlockPass();
		TessRenderable* renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite->getTexture(), (TESS_RENDER_PASS)pass, 2);
		TessVertexFmt* pVetLockPtr = NULL;
		if (!renderable->getLockPtr(2, pVetLockPtr))
			return false;

		f0 = pos.x + 0.25f;
		f1 = pos.x + 0.5f;
		pVetLockPtr[0].setEx(m_sectionPos, f0, y0, z1, color, bright, min_u, var17);
		pVetLockPtr[1].setEx(m_sectionPos, f1, y0, z1, color, bright, max_u, var17);
		pVetLockPtr[2].setEx(m_sectionPos, f1, y0, z0, color, bright, max_u, var13);
		pVetLockPtr[3].setEx(m_sectionPos, f0, y0, z0, color, bright, min_u, var13);
		//AutoCaculateQuadNormal(pVetLockPtr + 0);

		pVetLockPtr[4].setEx(m_sectionPos, f0, y0, z0, color, bright, min_u, var13);
		pVetLockPtr[5].setEx(m_sectionPos, f1, y0, z0, color, bright, max_u, var13);
		pVetLockPtr[6].setEx(m_sectionPos, f1, y0, z1, color, bright, max_u, var17);
		pVetLockPtr[7].setEx(m_sectionPos, f0, y0, z1, color, bright, min_u, var17);
		//AutoCaculateQuadNormal(pVetLockPtr + 4);
	}

	if (var22 || var21 && !var23 && !var24)
	{
		int pass = pcBlock->getRenderBlockPass();
		TessRenderable* renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite->getTexture(), (TESS_RENDER_PASS)pass, 2);
		TessVertexFmt* pVetLockPtr = NULL;
		if (!renderable->getLockPtr(2, pVetLockPtr))
			return false;

		f0 = pos.x + 0.5f;
		f1 = pos.x + 0.75f;
		pVetLockPtr[0].setEx(m_sectionPos, f0, y0, z1, color, bright, min_u, var17);
		pVetLockPtr[1].setEx(m_sectionPos, f1, y0, z1, color, bright, max_u, var17);
		pVetLockPtr[2].setEx(m_sectionPos, f1, y0, z0, color, bright, max_u, var13);
		pVetLockPtr[3].setEx(m_sectionPos, f0, y0, z0, color, bright, min_u, var13);
		//AutoCaculateQuadNormal(pVetLockPtr + 0);

		pVetLockPtr[4].setEx(m_sectionPos, f0, y0, z0, color, bright, min_u, var13);
		pVetLockPtr[5].setEx(m_sectionPos, f1, y0, z0, color, bright, max_u, var13);
		pVetLockPtr[6].setEx(m_sectionPos, f1, y0, z1, color, bright, max_u, var17);
		pVetLockPtr[7].setEx(m_sectionPos, f0, y0, z1, color, bright, min_u, var17);
		//AutoCaculateQuadNormal(pVetLockPtr + 4);
	}

	if (var22)
	{
		int pass = pcBlock->getRenderBlockPass();
		TessRenderable* renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite->getTexture(), (TESS_RENDER_PASS)pass, 2);
		TessVertexFmt* pVetLockPtr = NULL;
		if (!renderable->getLockPtr(2, pVetLockPtr))
			return false;

		f0 = pos.x + 0.75f;
		f1 = pos.x + 1.f;
		pVetLockPtr[0].setEx(m_sectionPos, f0, y0, z1, color, bright, min_u, var17);
		pVetLockPtr[1].setEx(m_sectionPos, f1, y0, z1, color, bright, max_u, var17);
		pVetLockPtr[2].setEx(m_sectionPos, f1, y0, z0, color, bright, max_u, var13);
		pVetLockPtr[3].setEx(m_sectionPos, f0, y0, z0, color, bright, min_u, var13);
		//AutoCaculateQuadNormal(pVetLockPtr + 0);

		pVetLockPtr[4].setEx(m_sectionPos, f0, y0, z0, color, bright, min_u, var13);
		pVetLockPtr[5].setEx(m_sectionPos, f1, y0, z0, color, bright, max_u, var13);
		pVetLockPtr[6].setEx(m_sectionPos, f1, y0, z1, color, bright, max_u, var17);
		pVetLockPtr[7].setEx(m_sectionPos, f0, y0, z1, color, bright, min_u, var17);
		//AutoCaculateQuadNormal(pVetLockPtr + 4);
	}

	return true;
}

bool RenderBlocks::renderBlockTripWireSource(Block* pBlock, cBlock* pcBlock, const BlockPos& pos)
{
	
	int metadata = blockAccess->getBlockMeta(pos);
	int d = metadata & 3;
	bool var8 = (metadata & 4) == 4;
	bool var9 = (metadata & 8) == 8;
	bool var10 = !blockAccess->doesBlockHaveSolidTopSurface(pos.getNegY());
	bool var11 = hasOverrideBlockTexture();

	if (!var11)
	{
		setOverrideBlockTexture(getBlockIcon(cBlockManager::scBlocks[BLOCK_ID_PLANKS]));
	}

	float var12 = 0.25F;
	float var13 = 0.125F;
	float var14 = 0.125F;
	float var15 = 0.3F - var12;
	float var16 = 0.3F + var12;

	if (d == 2)
	{
		setRenderBounds((float)(0.5F - var13), (float)var15, (float)(1.0F - var14), (float)(0.5F + var13), (float)var16, 1.0F);
	}
	else if (d == 0)
	{
		setRenderBounds((float)(0.5F - var13), (float)var15, 0.0F, (float)(0.5F + var13), (float)var16, (float)var14);
	}
	else if (d == 1)
	{
		setRenderBounds((float)(1.0F - var14), (float)var15, (float)(0.5F - var13), 1.0F, (float)var16, (float)(0.5F + var13));
	}
	else if (d == 3)
	{
		setRenderBounds(0.0F, (float)var15, (float)(0.5F - var13), (float)var14, (float)var16, (float)(0.5F + var13));
	}

	//render plank
	renderStandardBlock(pBlock, pcBlock, pos);

	if (!var11)
	{
		clearOverrideBlockTexture();
	}

	
	int bright = pcBlock->getMixedBrightnessForBlock(blockAccess, pos);
	float var17 = 1.0F;

	if (BlockManager::sLightValue[pBlock->getBlockID()] > 0)
	{
		var17 = 1.0F;
	}

	int color = Color(var17, var17, var17).getARGB();	
	AtlasSprite* sprite = getBlockIconFromSide(pcBlock, 0);

	if (hasOverrideBlockTexture())
	{
		sprite = overrideBlockTexture;
	}

	float min_u = sprite->getMinU();
	float min_v = sprite->getMinV();
	float max_u = sprite->getMaxU();
	float max_v = sprite->getMaxV();
	Vector3 vec[8];
	float vx = 0.046875F;
	float vz = 0.046875F;
	float vy = 0.3125F;
	vec[0] = Vector3((-vx), 0.0F, (-vz));
	vec[1] = Vector3(vx, 0.0F, (-vz));
	vec[2] = Vector3(vx, 0.0F, vz);
	vec[3] = Vector3((-vx), 0.0F, vz);
	vec[4] = Vector3((-vx), vy, (-vz));
	vec[5] = Vector3(vx, vy, (-vz));
	vec[6] = Vector3(vx, vy, vz);
	vec[7] = Vector3((-vx), vy, vz);

	for (int i = 0; i < 8; ++i)
	{
		vec[i].z += 0.0625F;

		if (var9)
		{
			vec[i].rotateAroundX(0.5235988F);
			vec[i].y -= 0.4375F;
		}
		else if (var8)
		{
			vec[i].rotateAroundX(0.08726647F);
			vec[i].y -= 0.4375F;
		}
		else
		{	
			vec[i].rotateAroundX(-Math::PI_2 / 9.0F);
			vec[i].y -= 0.375F;
		}

		vec[i].rotateAroundX(Math::PI_DIV2);

		if (d == 2)
		{
			vec[i].rotateAroundY(0.f);			
		}

		if (d == 0)
		{
			vec[i].rotateAroundY(Math::PI);
		}

		if (d == 1)
		{
			vec[i].rotateAroundY(Math::PI_DIV2);
		}

		if (d == 3)
		{
			vec[i].rotateAroundY(-Math::PI_DIV2);
		}

		vec[i] += Vector3(pos) + Vector3(0.5f, 0.3125f, 0.5f);
	}

	Vector3 vec_1;
	Vector3 vec_2;
	Vector3 vec_3;
	Vector3 vec_4;
	int var35 = 7;
	int var36 = 9;
	int var37 = 9;
	int var38 = 16;

	for (int i = 0; i < 6; ++i)
	{
		if (i == 0)
		{
			vec_1 = vec[0];
			vec_2 = vec[1];
			vec_3 = vec[2];
			vec_4 = vec[3];
			min_u = sprite->getInterpolatedU(var35 / 16.0F);
			min_v = sprite->getInterpolatedV(var37 / 16.0F);
			max_u = sprite->getInterpolatedU(var36 / 16.0F);
			max_v = sprite->getInterpolatedV((var37 + 2) / 16.0F);
		}
		else if (i == 1)
		{
			vec_1 = vec[7];
			vec_2 = vec[6];
			vec_3 = vec[5];
			vec_4 = vec[4];
		}
		else if (i == 2)
		{
			vec_1 = vec[1];
			vec_2 = vec[0];
			vec_3 = vec[4];
			vec_4 = vec[5];
			min_u = sprite->getInterpolatedU(var35 / 16.0F);
			min_v = sprite->getInterpolatedV(var37 / 16.0F);
			max_u = sprite->getInterpolatedU(var36 / 16.0F);
			max_v = sprite->getInterpolatedV(var38 / 16.0F);
		}
		else if (i == 3)
		{
			vec_1 = vec[2];
			vec_2 = vec[1];
			vec_3 = vec[5];
			vec_4 = vec[6];
		}
		else if (i == 4)
		{
			vec_1 = vec[3];
			vec_2 = vec[2];
			vec_3 = vec[6];
			vec_4 = vec[7];
		}
		else if (i == 5)
		{
			vec_1 = vec[0];
			vec_2 = vec[3];
			vec_3 = vec[7];
			vec_4 = vec[4];
		}

		int pass = pcBlock->getRenderBlockPass();
		TessRenderable* renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite->getTexture(), (TESS_RENDER_PASS)pass, 1);
		TessVertexFmt* pVetLockPtr = NULL;
		if(!renderable->getLockPtr(1, pVetLockPtr))
			return false;
		
		//render crabstick	
		pVetLockPtr[0].setEx(m_sectionPos, vec_1.x, vec_1.y, vec_1.z, color, bright, min_u, max_v);
		pVetLockPtr[1].setEx(m_sectionPos, vec_2.x, vec_2.y, vec_2.z, color, bright, max_u, max_v);
		pVetLockPtr[2].setEx(m_sectionPos, vec_3.x, vec_3.y, vec_3.z, color, bright, max_u, min_v);
		pVetLockPtr[3].setEx(m_sectionPos, vec_4.x, vec_4.y, vec_4.z, color, bright, min_u, min_v);
		//AutoCaculateQuadNormal(pVetLockPtr + 0);
	}

	float var63 = 0.09375F;
	float var40 = 0.09375F;
	float var41 = 0.03125F;
	vec[0] = Vector3((-var63), 0.0F, (-var40));
	vec[1] = Vector3(var63, 0.0F, (-var40));
	vec[2] = Vector3(var63, 0.0F, var40);
	vec[3] = Vector3((-var63), 0.0F, var40);
	vec[4] = Vector3((-var63), var41, (-var40));
	vec[5] = Vector3(var63, var41, (-var40));
	vec[6] = Vector3(var63, var41, var40);
	vec[7] = Vector3((-var63), var41, var40);

	for (int i = 0; i < 8; ++i)
	{
		vec[i].z += 0.21875F;

		if (var9)
		{
			vec[i].y -= 0.09375F;
			vec[i].z -= 0.1625F;
			vec[i].rotateAroundX(0.f);			
		}
		else if (var8)
		{
			vec[i].y += 0.015625F;
			vec[i].z -= 0.171875F;
			vec[i].rotateAroundX(0.17453294F);
		}
		else
		{
			vec[i].rotateAroundX(0.87266463F);
		}


		if (d == 2)
		{
			vec[i].rotateAroundY(0.f);			
		}

		if (d == 0)
		{
			vec[i].rotateAroundY(Math::PI);
		}

		if (d == 1)
		{
			vec[i].rotateAroundY(Math::PI_DIV2);
		}

		if (d == 3)
		{
			vec[i].rotateAroundY(-Math::PI_DIV2);
		}

		vec[i] += Vector3(pos) + Vector3(0.5f, 0.3125f, 0.5f);
	}

	int var64 = 5;
	int var43 = 11;
	int var44 = 3;
	int var45 = 9;

	for (int var46 = 0; var46 < 6; ++var46)
	{
		if (var46 == 0)
		{
			vec_1 = vec[0];
			vec_2 = vec[1];
			vec_3 = vec[2];
			vec_4 = vec[3];
			min_u = sprite->getInterpolatedU(var64 / 16.0F);
			min_v = sprite->getInterpolatedV(var44 / 16.0F);
			max_u = sprite->getInterpolatedU(var43 / 16.0F);
			max_v = sprite->getInterpolatedV(var45 / 16.0F);
		}
		else if (var46 == 1)
		{
			vec_1 = vec[7];
			vec_2 = vec[6];
			vec_3 = vec[5];
			vec_4 = vec[4];
		}
		else if (var46 == 2)
		{
			vec_1 = vec[1];
			vec_2 = vec[0];
			vec_3 = vec[4];
			vec_4 = vec[5];
			min_u = sprite->getInterpolatedU(var64 / 16.0F);
			min_v = sprite->getInterpolatedV(var44 / 16.0F);
			max_u = sprite->getInterpolatedU(var43 / 16.0F);
			max_v = sprite->getInterpolatedV((var44 + 2) / 16.0F);
		}
		else if (var46 == 3)
		{
			vec_1 = vec[2];
			vec_2 = vec[1];
			vec_3 = vec[5];
			vec_4 = vec[6];
		}
		else if (var46 == 4)
		{
			vec_1 = vec[3];
			vec_2 = vec[2];
			vec_3 = vec[6];
			vec_4 = vec[7];
		}
		else if (var46 == 5)
		{
			vec_1 = vec[0];
			vec_2 = vec[3];
			vec_3 = vec[7];
			vec_4 = vec[4];
		}

		int pass = pcBlock->getRenderBlockPass();
		TessRenderable* renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite->getTexture(), (TESS_RENDER_PASS)pass, 1);
		TessVertexFmt* pVetLockPtr = NULL;
		if (!renderable->getLockPtr(1, pVetLockPtr))
			return false;
		
		//render iron
		pVetLockPtr[0].setEx(m_sectionPos, vec_1.x, vec_1.y, vec_1.z, color, bright, min_u, max_v);
		pVetLockPtr[1].setEx(m_sectionPos, vec_2.x, vec_2.y, vec_2.z, color, bright, max_u, max_v);
		pVetLockPtr[2].setEx(m_sectionPos, vec_3.x, vec_3.y, vec_3.z, color, bright, max_u, min_v);
		pVetLockPtr[3].setEx(m_sectionPos, vec_4.x, vec_4.y, vec_4.z, color, bright, min_u, min_v);
		//AutoCaculateQuadNormal(pVetLockPtr + 0);
	}

	if (var8)
	{
		float var65 = vec[0].y;
		float var48 = 0.03125F;
		float var49 = 0.5F - var48 / 2.0F;
		float var50 = var49 + var48;
		//AtlasSprite* var51 = getBlockIcon(BlockManager::tripWire);
		float var52 = sprite->getMinU();
		float var54 = sprite->getInterpolatedV((var8 ? 2.0F : 0.0F) / 16.0F);
		float var56 = sprite->getMaxU();
		float var58 = sprite->getInterpolatedV((var8 ? 4.0F : 2.0F) / 16.0F);
		float var60 = (var10 ? 3.5F : 1.5F) / 16.0F;
		float var17 = pcBlock->getBlockBrightness(blockAccess, pos) * 0.75F;
		color = Color(var17, var17, var17).getARGB();		

		int pass = pcBlock->getRenderBlockPass();
		TessRenderable* renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite->getTexture(), (TESS_RENDER_PASS)pass, 2);
		TessVertexFmt* pVetLockPtr = NULL;
		if (!renderable->getLockPtr(2, pVetLockPtr))
			return false;

		float x0 = pos.x + var49;
		float x1 = pos.x + var50;
		float z0 = pos.z + var49;
		float z1 = pos.z + var50;
		float y0 = pos.y + var60;
		float f0, f1, f2;
		if (d == 2)
		{
			f0 = pos.z + 0.f;
			f1 = pos.z + 0.25f;
			f2 = pos.z + 0.5f;
			pVetLockPtr[0].setEx(m_sectionPos, x0, y0, f1, color, bright, var52, var54);
			pVetLockPtr[1].setEx(m_sectionPos, x1, y0, f1, color, bright, var52, var58);
			pVetLockPtr[2].setEx(m_sectionPos, x1, y0, f0, color, bright, var56, var58);
			pVetLockPtr[3].setEx(m_sectionPos, x0, y0, f0, color, bright, var56, var54);
			//AutoCaculateQuadNormal(pVetLockPtr + 0);

			pVetLockPtr[4].setEx(m_sectionPos, x0, var65, f2, color, bright, var52, var54);
			pVetLockPtr[5].setEx(m_sectionPos, x1, var65, f2, color, bright, var52, var58);
			pVetLockPtr[6].setEx(m_sectionPos, x1, y0, f1, color, bright, var56, var58);
			pVetLockPtr[7].setEx(m_sectionPos, x0, y0, f1, color, bright, var56, var54);
			//AutoCaculateQuadNormal(pVetLockPtr + 4);
		}
		else if (d == 0)
		{
			f0 = pos.z + 0.5f;
			f1 = pos.z + 0.75f;
			f2 = pos.z + 1.f;
			pVetLockPtr[0].setEx(m_sectionPos, x0, y0, f1, color, bright, var52, var54);
			pVetLockPtr[1].setEx(m_sectionPos, x1, y0, f1, color, bright, var52, var58);
			pVetLockPtr[2].setEx(m_sectionPos, x1, var65, f0, color, bright, var56, var58);
			pVetLockPtr[3].setEx(m_sectionPos, x0, var65, f0, color, bright, var56, var54);
			//AutoCaculateQuadNormal(pVetLockPtr + 0);

			pVetLockPtr[4].setEx(m_sectionPos, x0, y0, f2, color, bright, var52, var54);
			pVetLockPtr[5].setEx(m_sectionPos, x1, y0, f2, color, bright, var52, var58);
			pVetLockPtr[6].setEx(m_sectionPos, x1, y0, f1, color, bright, var56, var58);
			pVetLockPtr[7].setEx(m_sectionPos, x0, y0, f1, color, bright, var56, var54);
			//AutoCaculateQuadNormal(pVetLockPtr + 4);
		}
		else if (d == 1)
		{
			f0 = pos.x + 0.f;
			f1 = pos.x + 0.25f;
			f2 = pos.x + 0.5f;
			pVetLockPtr[0].setEx(m_sectionPos, f0, y0, z1, color, bright, var52, var58);
			pVetLockPtr[1].setEx(m_sectionPos, f1, y0, z1, color, bright, var56, var58);
			pVetLockPtr[2].setEx(m_sectionPos, f1, y0, z0, color, bright, var56, var54);
			pVetLockPtr[3].setEx(m_sectionPos, f0, y0, z0, color, bright, var52, var54);
			//AutoCaculateQuadNormal(pVetLockPtr + 0);

			pVetLockPtr[4].setEx(m_sectionPos, f1, y0, z1, color, bright, var52, var58);
			pVetLockPtr[5].setEx(m_sectionPos, f2, var65, z1, color, bright, var56, var58);
			pVetLockPtr[6].setEx(m_sectionPos, f2, var65, z0, color, bright, var56, var54);
			pVetLockPtr[7].setEx(m_sectionPos, f1, y0, z0, color, bright, var52, var54);
			//AutoCaculateQuadNormal(pVetLockPtr + 4);
		}
		else
		{
			f0 = pos.x + 0.5f;
			f1 = pos.x + 0.75f;
			f2 = pos.x + 1.f;
			pVetLockPtr[0].setEx(m_sectionPos, f0, var65, z1, color, bright, var52, var58);
			pVetLockPtr[1].setEx(m_sectionPos, f1, y0, z1, color, bright, var56, var58);
			pVetLockPtr[2].setEx(m_sectionPos, f1, y0, z0, color, bright, var56, var54);
			pVetLockPtr[3].setEx(m_sectionPos, f0, var65, z0, color, bright, var52, var54);
			//AutoCaculateQuadNormal(pVetLockPtr + 0);

			pVetLockPtr[4].setEx(m_sectionPos, f1, y0, z1, color, bright, var52, var58);
			pVetLockPtr[5].setEx(m_sectionPos, f2, y0, z1, color, bright, var56, var58);
			pVetLockPtr[6].setEx(m_sectionPos, f2, y0, z0, color, bright, var56, var54);
			pVetLockPtr[7].setEx(m_sectionPos, f1, y0, z0, color, bright, var52, var54);
			//AutoCaculateQuadNormal(pVetLockPtr + 4);
		}
	}

	return true;
}

bool RenderBlocks::renderBlockHopper(Block* pBlock, cBlock* pcBlock, const BlockPos& pos)
{	
	int bright = pcBlock->getMixedBrightnessForBlock(blockAccess, pos);
	float var6 = 1.0F;
	ui32 color = pcBlock->colorMultiplier(blockAccess, pos);
	float r = (float)(color >> 16 & 255) / 255.0F;
	float g = (float)(color >> 8 & 255) / 255.0F;
	float b = (float)(color & 255) / 255.0F;

	if (RenderEntity::anaglyphEnable)
	{
		float var11 = (r * 30.0F + g * 59.0F + b * 11.0F) / 100.0F;
		float var12 = (r * 30.0F + g * 70.0F) / 100.0F;
		float var13 = (r * 30.0F + b * 70.0F) / 100.0F;
		r = var11;
		g = var12;
		b = var13;
	}

	//var5.setColorOpaque_F(var6 * r, var6 * g, var6 * b);
	return renderBlockHopperMetadata(pBlock, pcBlock, pos, blockAccess->getBlockMeta(pos), false);
}

bool RenderBlocks::renderBlockEndPortalFrame(Block* pBlock, cBlock* pcBlock, const BlockPos& pos)
{	
	int metadata = blockAccess->getBlockMeta(pos);
	int var6 = metadata & 3;

	if (var6 == 0)
	{
		uvRotateTop = 3;
	}
	else if (var6 == 3)
	{
		uvRotateTop = 1;
	}
	else if (var6 == 1)
	{
		uvRotateTop = 2;
	}

	if (!BlockEndPortalFrame::isEnderEyeInserted(metadata))
	{
		setRenderBounds(0.0F, 0.0F, 0.0F, 1.0F, 0.8125F, 1.0F);
		renderStandardBlock(pBlock, pcBlock, pos);
		uvRotateTop = 0;
		return true;
	}
	else
	{
		renderAllFaces = true;
		setRenderBounds(0.0F, 0.0F, 0.0F, 1.0F, 0.8125F, 1.0F);
		renderStandardBlock(pBlock, pcBlock, pos);
		setOverrideBlockTexture(((cBlockEndPortalFrame*)pBlock)->getSpriteEye());
		setRenderBounds(0.25F, 0.8125F, 0.25F, 0.75F, 1.0F, 0.75F);
		renderStandardBlock(pBlock, pcBlock, pos);
		renderAllFaces = false;
		clearOverrideBlockTexture();
		uvRotateTop = 0;
		return true;
	}
}

bool RenderBlocks::renderBlockRedstoneLogic(Block* pBlock, cBlock* pcBlock, const BlockPos& pos)
{
	int meta = blockAccess->getBlockMeta(pos);
	meta = meta & 3;
	return renderBlockRedStoneLogicMeta(pBlock, pcBlock, pos, meta);
}

bool RenderBlocks::renderBlockRedStoneLogicMeta(Block* pBlock, cBlock* pcBlock, const BlockPos& pos, int meta)
{
	renderStandardBlock(pBlock, pcBlock, pos);
	int bright = pcBlock->getMixedBrightnessForBlock(blockAccess, pos);
	int color = Color(1.0F, 1.0F, 1.0F).getARGB();
	int meta2 = blockAccess->getBlockMeta(pos);
	AtlasSprite* sprite = getBlockIconFromSideAndMetadata(pcBlock, 1, meta2);
	float minu = sprite->getMinU();
	float maxu = sprite->getMaxU();
	float minv = sprite->getMinV();
	float maxv = sprite->getMaxV();
	float var19 = pos.x + 1.f;
	float var21 = pos.x + 1.f;
	float var23 = pos.x + 0.f;
	float var25 = pos.x + 0.f;
	float var27 = pos.z + 0.f;
	float var29 = pos.z + 1.f;
	float var31 = pos.z + 1.f;
	float var33 = pos.z + 0.f;
	float var35 = pos.y + 0.125f;

	if (meta == 2)
	{
		var19 = var21 = pos.x + 0.f;
		var23 = var25 = pos.x + 1.f;
		var27 = var33 = pos.z + 1.f;
		var29 = var31 = pos.z + 0.f;
	}
	else if (meta == 3)
	{
		var19 = var25 = pos.x + 0.f;
		var21 = var23 = pos.x + 1.f;
		var27 = var29 = pos.z + 0.f;
		var31 = var33 = pos.z + 1.f;
	}
	else if (meta == 1)
	{
		var19 = var25 = pos.x + 1.f;
		var21 = var23 = pos.x + 0.f;
		var27 = var29 = pos.z + 1.f;
		var31 = var33 = pos.z + 0.f;
	}

	int pass = pcBlock->getRenderBlockPass();
	TessRenderable* renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite->getTexture(), (TESS_RENDER_PASS)pass, 1);
	TessVertexFmt* pVetLockPtr = nullptr;
	if (!renderable->getLockPtr(1, pVetLockPtr))
		return false;
	pVetLockPtr[0].setEx(m_sectionPos, var25, var35, var33, color, bright, minu, minv);
	pVetLockPtr[1].setEx(m_sectionPos, var23, var35, var31, color, bright, minu, maxv);
	pVetLockPtr[2].setEx(m_sectionPos, var21, var35, var29, color, bright, maxu, maxv);
	pVetLockPtr[3].setEx(m_sectionPos, var19, var35, var27, color, bright, maxu, minv);
	//AutoCaculateQuadNormal(pVetLockPtr + 0);
	return true;
}

bool RenderBlocks::renderBlockRedstoneWire(Block* pBlock, cBlock* pcBlock, const BlockPos& pos)
{
	int meta = blockAccess->getBlockMeta(pos);
	AtlasSprite* sprite_cross = cBlockRedstoneWire::getSpriteByName("cross");
	AtlasSprite* sprite_line = cBlockRedstoneWire::getSpriteByName("line");
	AtlasSprite* sprite_cross_overlay = cBlockRedstoneWire::getSpriteByName("cross_overlay");
	AtlasSprite* sprite_line_overlay = cBlockRedstoneWire::getSpriteByName("line_overlay");
	
	int bright = pcBlock->getMixedBrightnessForBlock(blockAccess, pos);

	float strength = (float)meta / 15.0F;
	float r = strength * 0.6F + 0.4F;
	if (meta == 0)
		r = 0.3F;
	float g = strength * strength * 0.7F - 0.5F;
	float b = strength * strength * 0.6F - 0.7F;
	if (g < 0.0F)
		g = 0.0F;
	if (b < 0.0F)
		b = 0.0F;

	int color = Color(r, g, b).getARGB();
	float var16 = 0.015625f;
	float var18 = 0.015625f;
	bool pWest  = BlockRedstoneWire::isPowerProviderOrWire(blockAccess, pos.getNegX(), 1) || !blockAccess->isBlockNormalCube(pos.getNegX()) && BlockRedstoneWire::isPowerProviderOrWire(blockAccess, pos.getDW(), -1);
	bool pEast  = BlockRedstoneWire::isPowerProviderOrWire(blockAccess, pos.getPosX(), 3) || !blockAccess->isBlockNormalCube(pos.getPosX()) && BlockRedstoneWire::isPowerProviderOrWire(blockAccess, pos.getDE(), -1);
	bool pNorth = BlockRedstoneWire::isPowerProviderOrWire(blockAccess, pos.getNegZ(), 2) || !blockAccess->isBlockNormalCube(pos.getNegZ()) && BlockRedstoneWire::isPowerProviderOrWire(blockAccess, pos.getDN(), -1);
	bool pSouth = BlockRedstoneWire::isPowerProviderOrWire(blockAccess, pos.getPosZ(), 0) || !blockAccess->isBlockNormalCube(pos.getPosZ()) && BlockRedstoneWire::isPowerProviderOrWire(blockAccess, pos.getDS(), -1);

	if (!blockAccess->isBlockNormalCube(pos.getPosY()))
	{
		if (blockAccess->isBlockNormalCube(pos.getNegX()) && BlockRedstoneWire::isPowerProviderOrWire(blockAccess, pos.getUW(), -1))
		{
			pWest = true;
		}

		if (blockAccess->isBlockNormalCube(pos.getPosX()) && BlockRedstoneWire::isPowerProviderOrWire(blockAccess, pos.getUE(), -1))
		{
			pEast = true;
		}

		if (blockAccess->isBlockNormalCube(pos.getNegZ()) && BlockRedstoneWire::isPowerProviderOrWire(blockAccess, pos.getUN(), -1))
		{
			pNorth = true;
		}

		if (blockAccess->isBlockNormalCube(pos.getPosZ()) && BlockRedstoneWire::isPowerProviderOrWire(blockAccess, pos.getUS(), -1))
		{
			pSouth = true;
		}
	}

	float x0 = pos.x + 0.f;
	float x1 = pos.x + 1.f;
	float z0 = pos.z + 0.f;
	float z1 = pos.z + 1.f;
	float y0 = pos.y + 0.015625f;
	int var28 = 0;

	int pass = pcBlock->getRenderBlockPass();

	if ((pWest || pEast) && !pNorth && !pSouth)
	{
		var28 = 1;
	}

	if ((pNorth || pSouth) && !pEast && !pWest)
	{
		var28 = 2;
	}

	TessRenderable* renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite_cross->getTexture(), (TESS_RENDER_PASS)pass, 2);
	TessVertexFmt* pVetLockPtr = nullptr;
	if (!renderable->getLockPtr(2, pVetLockPtr))
		return false;

	if (var28 == 0)
	{
		float var29 = 0.f;
		float var30 = 0.f;
		float var31 = 1.f;
		float var32 = 1.f;
		bool var33 = true;

		if (!pWest)
			x0 += 0.3125F;

		if (!pWest)
			var29 += 5.f / 16.f;

		if (!pEast)
			x1 -= 0.3125F;

		if (!pEast)
			var31 -= 5.5 / 16.f;

		if (!pNorth)
			z0 += 0.3125F;

		if (!pNorth)
			var30 += 5.f / 16.f;

		if (!pSouth)
			z1 -= 0.3125F;

		if (!pSouth)
			var32 -= 5.5 / 16.f;

		pVetLockPtr[0].setEx(m_sectionPos, x1, y0, z1, color, bright, sprite_cross->getInterpolatedU(var31), sprite_cross->getInterpolatedV(var32));
		pVetLockPtr[1].setEx(m_sectionPos, x1, y0, z0, color, bright, sprite_cross->getInterpolatedU(var31), sprite_cross->getInterpolatedV(var30));
		pVetLockPtr[2].setEx(m_sectionPos, x0, y0, z0, color, bright, sprite_cross->getInterpolatedU(var29), sprite_cross->getInterpolatedV(var30));
		pVetLockPtr[3].setEx(m_sectionPos, x0, y0, z1, color, bright, sprite_cross->getInterpolatedU(var29), sprite_cross->getInterpolatedV(var32));
		//AutoCaculateQuadNormal(pVetLockPtr + 0);

		color = Color(1.f, 1.f, 1.f).getARGB();
		pVetLockPtr[4].setEx(m_sectionPos, x1, y0, z1, color, bright, sprite_cross_overlay->getInterpolatedU(var31), sprite_cross_overlay->getInterpolatedV(var32));
		pVetLockPtr[5].setEx(m_sectionPos, x1, y0, z0, color, bright, sprite_cross_overlay->getInterpolatedU(var31), sprite_cross_overlay->getInterpolatedV(var30));
		pVetLockPtr[6].setEx(m_sectionPos, x0, y0, z0, color, bright, sprite_cross_overlay->getInterpolatedU(var29), sprite_cross_overlay->getInterpolatedV(var30));
		pVetLockPtr[7].setEx(m_sectionPos, x0, y0, z1, color, bright, sprite_cross_overlay->getInterpolatedU(var29), sprite_cross_overlay->getInterpolatedV(var32));
		//AutoCaculateQuadNormal(pVetLockPtr + 4);
	}
	else if (var28 == 1)
	{
		pVetLockPtr[0].setEx(m_sectionPos, x1, y0, z1, color, bright, sprite_line->getMaxU(), sprite_line->getMaxV());
		pVetLockPtr[1].setEx(m_sectionPos, x1, y0, z0, color, bright, sprite_line->getMaxU(), sprite_line->getMinV());
		pVetLockPtr[2].setEx(m_sectionPos, x0, y0, z0, color, bright, sprite_line->getMinU(), sprite_line->getMinV());
		pVetLockPtr[3].setEx(m_sectionPos, x0, y0, z1, color, bright, sprite_line->getMinU(), sprite_line->getMaxV());
		//AutoCaculateQuadNormal(pVetLockPtr + 0);

		color = Color(1.f, 1.f, 1.f).getARGB();
		pVetLockPtr[4].setEx(m_sectionPos, x1, y0, z1, color, bright, sprite_line_overlay->getMaxU(), sprite_line_overlay->getMaxV());
		pVetLockPtr[5].setEx(m_sectionPos, x1, y0, z0, color, bright, sprite_line_overlay->getMaxU(), sprite_line_overlay->getMinV());
		pVetLockPtr[6].setEx(m_sectionPos, x0, y0, z0, color, bright, sprite_line_overlay->getMinU(), sprite_line_overlay->getMinV());
		pVetLockPtr[7].setEx(m_sectionPos, x0, y0, z1, color, bright, sprite_line_overlay->getMinU(), sprite_line_overlay->getMaxV());
		//AutoCaculateQuadNormal(pVetLockPtr + 4);
	}
	else
	{
		pVetLockPtr[0].setEx(m_sectionPos, x1, y0, z1, color, bright, sprite_line->getMaxU(), sprite_line->getMaxV());
		pVetLockPtr[1].setEx(m_sectionPos, x1, y0, z0, color, bright, sprite_line->getMinU(), sprite_line->getMaxV());
		pVetLockPtr[2].setEx(m_sectionPos, x0, y0, z0, color, bright, sprite_line->getMinU(), sprite_line->getMinV());
		pVetLockPtr[3].setEx(m_sectionPos, x0, y0, z1, color, bright, sprite_line->getMaxU(), sprite_line->getMinV());
		//AutoCaculateQuadNormal(pVetLockPtr + 0);

		color = Color(1.f, 1.f, 1.f).getARGB();
		pVetLockPtr[4].setEx(m_sectionPos, x1, y0, z1, color, bright, sprite_line_overlay->getMaxU(), sprite_line_overlay->getMaxV());
		pVetLockPtr[5].setEx(m_sectionPos, x1, y0, z0, color, bright, sprite_line_overlay->getMinU(), sprite_line_overlay->getMaxV());
		pVetLockPtr[6].setEx(m_sectionPos, x0, y0, z0, color, bright, sprite_line_overlay->getMinU(), sprite_line_overlay->getMinV());
		pVetLockPtr[7].setEx(m_sectionPos, x0, y0, z1, color, bright, sprite_line_overlay->getMaxU(), sprite_line_overlay->getMinV());
		//AutoCaculateQuadNormal(pVetLockPtr + 4);
	}

	if (!blockAccess->isBlockNormalCube(pos.getPosY()))
	{
		float var34 = 0.021875F;
		float y1 = pos.y + 1.f + +0.021875F;
		float y2 = pos.y + 0.f;

		if (blockAccess->isBlockNormalCube(pos.getNegX()) && blockAccess->getBlockId(pos.getUW()) == BLOCK_ID_REDSTONE_WIRE)
		{
			if (!renderable->getLockPtr(2, pVetLockPtr))
				return false;
			
			float x2 = pos.x + 0.015625f;
			color = Color(r, g, b).getARGB();
			pVetLockPtr[0].setEx(m_sectionPos, x2, y1, z1, color, bright, sprite_line->getMaxU(), sprite_line->getMinV());
			pVetLockPtr[1].setEx(m_sectionPos, x2, y2, z1, color, bright, sprite_line->getMinU(), sprite_line->getMinV());
			pVetLockPtr[2].setEx(m_sectionPos, x2, y2, z0, color, bright, sprite_line->getMinU(), sprite_line->getMaxV());
			pVetLockPtr[3].setEx(m_sectionPos, x2, y1, z0, color, bright, sprite_line->getMaxU(), sprite_line->getMaxV());
			//AutoCaculateQuadNormal(pVetLockPtr + 0);

			color = Color(1.f, 1.f, 1.f).getARGB();
			pVetLockPtr[4].setEx(m_sectionPos, x2, y1, z1, color, bright, sprite_line_overlay->getMaxU(), sprite_line_overlay->getMinV());
			pVetLockPtr[5].setEx(m_sectionPos, x2, y2, z1, color, bright, sprite_line_overlay->getMinU(), sprite_line_overlay->getMinV());
			pVetLockPtr[6].setEx(m_sectionPos, x2, y2, z0, color, bright, sprite_line_overlay->getMinU(), sprite_line_overlay->getMaxV());
			pVetLockPtr[7].setEx(m_sectionPos, x2, y1, z0, color, bright, sprite_line_overlay->getMaxU(), sprite_line_overlay->getMaxV());
			//AutoCaculateQuadNormal(pVetLockPtr + 4);
		}

		if (blockAccess->isBlockNormalCube(pos.getPosX()) && blockAccess->getBlockId(pos.getUE()) == BLOCK_ID_REDSTONE_WIRE)
		{
			if (!renderable->getLockPtr(2, pVetLockPtr))
				return false;

			float x2 = pos.x + 1.f - 0.015625f;
			color = Color(r, g, b).getARGB();
			pVetLockPtr[0].setEx(m_sectionPos, x2, y2, z1, color, bright, sprite_line->getMinU(), sprite_line->getMaxV());
			pVetLockPtr[1].setEx(m_sectionPos, x2, y1, z1, color, bright, sprite_line->getMaxU(), sprite_line->getMaxV());
			pVetLockPtr[2].setEx(m_sectionPos, x2, y1, z0, color, bright, sprite_line->getMaxU(), sprite_line->getMinV());
			pVetLockPtr[3].setEx(m_sectionPos, x2, y2, z0, color, bright, sprite_line->getMinU(), sprite_line->getMinV());
			//AutoCaculateQuadNormal(pVetLockPtr + 0);

			color = Color(1.f, 1.f, 1.f).getARGB();
			pVetLockPtr[4].setEx(m_sectionPos, x2, y2, z1, color, bright, sprite_line_overlay->getMinU(), sprite_line_overlay->getMaxV());
			pVetLockPtr[5].setEx(m_sectionPos, x2, y1, z1, color, bright, sprite_line_overlay->getMaxU(), sprite_line_overlay->getMaxV());
			pVetLockPtr[6].setEx(m_sectionPos, x2, y1, z0, color, bright, sprite_line_overlay->getMaxU(), sprite_line_overlay->getMinV());
			pVetLockPtr[7].setEx(m_sectionPos, x2, y2, z0, color, bright, sprite_line_overlay->getMinU(), sprite_line_overlay->getMinV());
			//AutoCaculateQuadNormal(pVetLockPtr + 4);
		}

		if (blockAccess->isBlockNormalCube(pos.getNegZ()) && blockAccess->getBlockId(pos.getUN()) == BLOCK_ID_REDSTONE_WIRE)
		{
			if (!renderable->getLockPtr(2, pVetLockPtr))
				return false;

			float z2 = pos.z + 0.015625f;
			color = Color(r, g, b).getARGB();
			pVetLockPtr[0].setEx(m_sectionPos, x1, y2, z2, color, bright, sprite_line->getMinU(), sprite_line->getMaxV());
			pVetLockPtr[1].setEx(m_sectionPos, x1, y1, z2, color, bright, sprite_line->getMaxU(), sprite_line->getMaxV());
			pVetLockPtr[2].setEx(m_sectionPos, x0, y1, z2, color, bright, sprite_line->getMaxU(), sprite_line->getMinV());
			pVetLockPtr[3].setEx(m_sectionPos, x0, y2, z2, color, bright, sprite_line->getMinU(), sprite_line->getMinV());
			//AutoCaculateQuadNormal(pVetLockPtr + 0);

			color = Color(1.f, 1.f, 1.f).getARGB();
			pVetLockPtr[4].setEx(m_sectionPos, x1, y2, z2, color, bright, sprite_line_overlay->getMinU(), sprite_line_overlay->getMaxV());
			pVetLockPtr[5].setEx(m_sectionPos, x1, y1, z2, color, bright, sprite_line_overlay->getMaxU(), sprite_line_overlay->getMaxV());
			pVetLockPtr[6].setEx(m_sectionPos, x0, y1, z2, color, bright, sprite_line_overlay->getMaxU(), sprite_line_overlay->getMinV());
			pVetLockPtr[7].setEx(m_sectionPos, x0, y2, z2, color, bright, sprite_line_overlay->getMinU(), sprite_line_overlay->getMinV());
			//AutoCaculateQuadNormal(pVetLockPtr + 4);
		}

		if (blockAccess->isBlockNormalCube(pos.getPosZ()) && blockAccess->getBlockId(pos.getUS()) == BLOCK_ID_REDSTONE_WIRE)
		{
			if (!renderable->getLockPtr(2, pVetLockPtr))
				return false;

			float z2 = pos.z + 1.f - 0.015625f;
			color = Color(r, g, b).getARGB();
			pVetLockPtr[0].setEx(m_sectionPos, x1, y1, z2, color, bright, sprite_line->getMaxU(), sprite_line->getMinV());
			pVetLockPtr[1].setEx(m_sectionPos, x1, y2, z2, color, bright, sprite_line->getMinU(), sprite_line->getMinV());
			pVetLockPtr[2].setEx(m_sectionPos, x0, y2, z2, color, bright, sprite_line->getMinU(), sprite_line->getMaxV());
			pVetLockPtr[3].setEx(m_sectionPos, x0, y1, z2, color, bright, sprite_line->getMaxU(), sprite_line->getMaxV());
			//AutoCaculateQuadNormal(pVetLockPtr + 0);

			color = Color(1.f, 1.f, 1.f).getARGB();
			pVetLockPtr[4].setEx(m_sectionPos, x1, y1, z2, color, bright, sprite_line_overlay->getMaxU(), sprite_line_overlay->getMinV());
			pVetLockPtr[5].setEx(m_sectionPos, x1, y2, z2, color, bright, sprite_line_overlay->getMinU(), sprite_line_overlay->getMinV());
			pVetLockPtr[6].setEx(m_sectionPos, x0, y2, z2, color, bright, sprite_line_overlay->getMinU(), sprite_line_overlay->getMaxV());
			pVetLockPtr[7].setEx(m_sectionPos, x0, y1, z2, color, bright, sprite_line_overlay->getMaxU(), sprite_line_overlay->getMaxV());
			//AutoCaculateQuadNormal(pVetLockPtr + 4);
		}
	}

	return true;
}

bool RenderBlocks::renderBlockRepeater(Block* pBlock, cBlock* pcBlock, const BlockPos& pos)
{
	int meta = blockAccess->getBlockMeta(pos);
	int var6 = meta & 3;
	int var7 = (meta & 12) >> 2;

	cBlockRedstoneRepeater* pRepeater = dynamic_cast<cBlockRedstoneRepeater*>(pcBlock);
	int bright = pRepeater->getMixedBrightnessForBlock(blockAccess, pos);
	int color = Color(1.f, 1.f, 1.f).getARGB();
	Vector3 torchPos;
	torchPos.y = -0.1875f;
	bool var11 = pRepeater->isLocked(blockAccess, pos, meta);
	torchPos.x = 0.0f;
	torchPos.z = 0.0f;
	float var16 = 0.0f;
	float var18 = 0.0f;

	switch (var6)
	{
	case 0:
		var18 = -0.3125f;
		torchPos.z = cBlockRedstoneRepeater::repeaterTorchOffset[var7];
		break;

	case 1:
		var16 = 0.3125f;
		torchPos.x = -cBlockRedstoneRepeater::repeaterTorchOffset[var7];
		break;

	case 2:
		var18 = 0.3125f;
		torchPos.z = -cBlockRedstoneRepeater::repeaterTorchOffset[var7];
		break;

	case 3:
		var16 = -0.3125f;
		torchPos.x = cBlockRedstoneRepeater::repeaterTorchOffset[var7];
	}

	if (!var11)
	{
		renderTorchAtAngle(pBlock, pRepeater, Vector3(pos) + torchPos, 0.f, 0.f, 0);
	}
	else
	{
		AtlasSprite* sprite_bedrock = getBlockIcon(cBlockManager::scBlocks[BLOCK_ID_BEDROCK]);
		setOverrideBlockTexture(sprite_bedrock);
		float var21 = 2.0F;
		float var22 = 14.0F;
		float var23 = 7.0F;
		float var24 = 9.0F;

		switch (var6)
		{
		case 1:
		case 3:
			var21 = 7.0F;
			var22 = 9.0F;
			var23 = 2.0F;
			var24 = 14.0F;

		case 0:
		case 2:
		default:
			setRenderBounds(var21 / 16.0F + torchPos.x, 0.125f, var23 / 16.0F + torchPos.z, var22 / 16.0F + torchPos.x, 0.25f, var24 / 16.0F + torchPos.z);
			float var25 = sprite_bedrock->getInterpolatedU(var21);
			float var27 = sprite_bedrock->getInterpolatedV(var23);
			float var29 = sprite_bedrock->getInterpolatedU(var22);
			float var31 = sprite_bedrock->getInterpolatedV(var24);
			int pass = pRepeater->getRenderBlockPass();
			TessRenderable* renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite_bedrock->getTexture(), (TESS_RENDER_PASS)pass, 1);
			TessVertexFmt* pVetLockPtr = nullptr;
			if (!renderable->getLockPtr(1, pVetLockPtr))
				return false;
			pVetLockPtr[0].setEx(m_sectionPos, (pos.x + var21 / 16.F) + torchPos.x, pos.y + 0.25F, (pos.z + var23 / 16.0F) + torchPos.z, color, bright, var25, var27);
			pVetLockPtr[0].setEx(m_sectionPos, (pos.x + var21 / 16.F) + torchPos.x, pos.y + 0.25F, (pos.z + var24 / 16.0F) + torchPos.z, color, bright, var25, var31);
			pVetLockPtr[0].setEx(m_sectionPos, (pos.x + var22 / 16.F) + torchPos.x, pos.y + 0.25F, (pos.z + var24 / 16.0F) + torchPos.z, color, bright, var29, var31);
			pVetLockPtr[0].setEx(m_sectionPos, (pos.x + var22 / 16.F) + torchPos.x, pos.y + 0.25F, (pos.z + var23 / 16.0F) + torchPos.z, color, bright, var29, var27);
			//AutoCaculateQuadNormal(pVetLockPtr + 0);
			renderStandardBlock(pBlock, pcBlock, pos);
			setRenderBounds(0.0f, 0.0f, 0.0f, 1.0f, 0.125f, 1.0f);
			clearOverrideBlockTexture();
		}
	}

	setOABrightness(pRepeater->getMixedBrightnessForBlock(blockAccess, pos));
	setOAColor(1.0F, 1.0F, 1.0F);
	renderTorchAtAngle(pBlock, pRepeater, (Vector3)pos + Vector3(var16, torchPos.y, var18), 0.0f, 0.0f, 0);
	renderBlockRedstoneLogic(pBlock, pRepeater, pos);
	return true;
}

bool RenderBlocks::renderBlockComparator(Block* pBlock, cBlock* pcBlock, const BlockPos& pos)
{
	setOABrightness(pcBlock->getMixedBrightnessForBlock(blockAccess, pos));
	setOAColor(1.0F, 1.0F, 1.0F);
	int meta = blockAccess->getBlockMeta(pos);
	int dir = meta & 3;
	float var8 = 0.0f;
	float var10 = -0.1875f;
	float var12 = 0.0f;
	float var14 = 0.0f;
	float var16 = 0.0f;
	AtlasSprite* pSprite = nullptr;

	cBlockComparator* pComparator = dynamic_cast<cBlockComparator*>(pcBlock);
	if (pComparator->isComparatorActived(meta))
	{
		pSprite = cBlockManager::scBlocks[BLOCK_ID_TORCH_REDSTONE_ACTIVE]->getSpriteFromSide(0);
	}
	else
	{
		var10 -= 0.1875f;
		pSprite = cBlockManager::scBlocks[BLOCK_ID_TORCH_REDSTONE_IDLE]->getSpriteFromSide(0);
	}

	switch (dir)
	{
	case 0:
		var12 = -0.3125f;
		var16 = 1.0f;
		break;

	case 1:
		var8 = 0.3125f;
		var14 = -1.0f;
		break;

	case 2:
		var12 = 0.3125f;
		var16 = -1.0f;
		break;

	case 3:
		var8 = -0.3125f;
		var14 = 1.0f;
	}

	renderTorchAtAngle(pBlock, pComparator, Vector3(pos.x + 0.25f * var14 + 0.1875f * var16, pos.y - 0.1875F, pos.z + 0.25f * var16 + 0.1875f * var14), 0.0f, 0.0f, meta);
	renderTorchAtAngle(pBlock, pComparator, Vector3(pos.x + 0.25f * var14 - 0.1875f * var16, pos.y - 0.1875F, pos.z + 0.25f * var16 - 0.1875f * var14), 0.0f, 0.0f, meta);
	setOverrideBlockTexture(pSprite);
	renderTorchAtAngle(pBlock, pComparator, Vector3(pos.x + var8, pos.y + var10, pos.z + var12), 0.0f, 0.0f, meta);
	clearOverrideBlockTexture();
	renderBlockRedStoneLogicMeta(pBlock, pComparator, pos, dir);
	return true;
}

float RenderBlocks::getFluidHeight(const BlockPos& pos, const BM_Material& mat)
{
	int var5 = 0;
	float var6 = 0.0F;

	for (int dir = 0; dir < 4; ++dir)
	{
		int ix = pos.x - (dir & 1);
		int iz = pos.z - (dir >> 1 & 1);
		BlockPos pos1(ix, pos.y, iz);

		if (blockAccess->getBlockMaterial(pos1.getPosY()) == mat)
		{
			return 1.0F;
		}

		const BM_Material& mat_ne = blockAccess->getBlockMaterial(pos1);

		if (mat_ne == mat)
		{
			int meta = blockAccess->getBlockMeta(pos1);

			if (meta >= 8 || meta == 0)
			{
				var6 += BlockFluid::getFluidHeightPercent(meta) * 10.0F;
				var5 += 10;
			}

			var6 += BlockFluid::getFluidHeightPercent(meta);
			++var5;
		}
		else if (!mat_ne.isSolid())
		{
			++var6;
			++var5;
		}
	}

	return 1.0F - var6 / (float)var5;
}

void RenderBlocks::drawCrossedSquares(Block* pBlock, cBlock* pcBlock, int metadata, const Vector3& pos, float par9, int color, int bright)
{
	// Tessellator var10 = Tessellator.instance;
	AtlasSprite* sprite = getBlockIconFromSideAndMetadata(pcBlock, 0, metadata);

	if (hasOverrideBlockTexture())
	{
		sprite = overrideBlockTexture;
	}

	float var12 = sprite->getMinU();
	float var14 = sprite->getMinV();
	float var16 = sprite->getMaxU();
	float var18 = sprite->getMaxV();
	float var20 = 0.45f * par9;
	float var22 = pos.x + 0.5f - var20;
	float var24 = pos.x + 0.5f + var20;
	float var26 = pos.z + 0.5f - var20;
	float var28 = pos.z + 0.5f + var20;

	int pass = pcBlock->getRenderBlockPass();
	TessRenderable* renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite->getTexture(), (TESS_RENDER_PASS)pass, 4);
	TessVertexFmt* pVetex = NULL;
	if (!renderable->getLockPtr(4, pVetex))
		return;

	float y0 = pos.y + 0.f;
	float y1 = pos.y + par9;
	// fill the vertex and index of the VBO,IBO.
	pVetex[0].setEx(m_sectionPos, var22, y1, var26, color, bright, var12, var14);
	pVetex[1].setEx(m_sectionPos, var22, y0, var26, color, bright, var12, var18);
	pVetex[2].setEx(m_sectionPos, var24, y0, var28, color, bright, var16, var18);
	pVetex[3].setEx(m_sectionPos, var24, y1, var28, color, bright, var16, var14);
	//AutoCaculateQuadNormal(pVetex + 0);

	pVetex[4].setEx(m_sectionPos, var24, y1, var28, color, bright, var12, var14);
	pVetex[5].setEx(m_sectionPos, var24, y0, var28, color, bright, var12, var18);
	pVetex[6].setEx(m_sectionPos, var22, y0, var26, color, bright, var16, var18);
	pVetex[7].setEx(m_sectionPos, var22, y1, var26, color, bright, var16, var14);
	//AutoCaculateQuadNormal(pVetex + 4);
	
	pVetex[8].setEx(m_sectionPos, var22, y1, var28, color, bright, var12, var14);
	pVetex[9].setEx(m_sectionPos, var22, y0, var28, color, bright, var12, var18);
	pVetex[10].setEx(m_sectionPos, var24, y0, var26, color, bright, var16, var18);
	pVetex[11].setEx(m_sectionPos, var24, y1, var26, color, bright, var16, var14);
	//AutoCaculateQuadNormal(pVetex + 8);

	pVetex[12].setEx(m_sectionPos, var24, y1, var26, color, bright, var12, var14);
	pVetex[13].setEx(m_sectionPos, var24, y0, var26, color, bright, var12, var18);
	pVetex[14].setEx(m_sectionPos, var22, y0, var28, color, bright, var16, var18);
	pVetex[15].setEx(m_sectionPos, var22, y1, var28, color, bright, var16, var14);
	//AutoCaculateQuadNormal(pVetex + 12);
}


void RenderBlocks::drawCrossedSquaresEx(Block* pBlock, cBlock* pcBlock, int metadata, const Vector3& pos, float par9, int color, int bright)
{
	// Tessellator var10 = Tessellator.instance;
	AtlasSprite* sprite = getBlockIconFromSideAndMetadata(pcBlock, 0, metadata);

	if (hasOverrideBlockTexture())
	{
		sprite = overrideBlockTexture;
	}

	float minU = sprite->getMinU();
	float minV = sprite->getMinV();
	float maxU = sprite->getMaxU();
	float maxV = sprite->getMaxV();
	

	int pass = pcBlock->getRenderBlockPass();
	TessRenderable* renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite->getTexture(), (TESS_RENDER_PASS)pass, 12);
	TessVertexFmt* pVetex = NULL;
	if (!renderable->getLockPtr(12, pVetex))
		return;

	// face1
	Vector3 top1 = Vector3(-0.36f, 1.0f, 0.36f) + pos + Vector3(0.5f, 0.0f, 0.5f);
	Vector3 top2 = Vector3(0.49f, 1.0f, -0.13f) + pos + Vector3(0.5f, 0.0f, 0.5f);
	Vector3 btn1 = Vector3(0.36f, 0.0f, -0.36f) + pos + Vector3(0.5f, 0.0f, 0.5f);
	Vector3 btn2 = Vector3(-0.49f, 0.0f, 0.13f) + pos + Vector3(0.5f, 0.0f, 0.5f);

	int idx = 0;
	pVetex[idx++].setEx(m_sectionPos, top2.x, top2.y, top2.z, color, bright, maxU, minV);
	pVetex[idx++].setEx(m_sectionPos, top1.x, top1.y, top1.z, color, bright, minU, minV);
	pVetex[idx++].setEx(m_sectionPos, btn2.x, btn2.y, btn2.z, color, bright, minU, maxV);
	pVetex[idx++].setEx(m_sectionPos, btn1.x, btn1.y, btn1.z, color, bright, maxU, maxV);
	//AutoCaculateQuadNormal(pVetex + idx - 4);

	pVetex[idx++].setEx(m_sectionPos, top2.x, top2.y, top2.z, color, bright, maxU, minV);
	pVetex[idx++].setEx(m_sectionPos, btn1.x, btn1.y, btn1.z, color, bright, maxU, maxV);
	pVetex[idx++].setEx(m_sectionPos, btn2.x, btn2.y, btn2.z, color, bright, minU, maxV);
	pVetex[idx++].setEx(m_sectionPos, top1.x, top1.y, top1.z, color, bright, minU, minV);
	//AutoCaculateQuadNormal(pVetex + idx - 4);

	// face2
	top1 = Vector3(-0.13f, 1.0f, -0.49f) + pos + Vector3(0.5f, 0.0f, 0.5f);
	top2 = Vector3(0.36f, 1.0f, 0.36f) + pos + Vector3(0.5f, 0.0f, 0.5f);
	btn2 = Vector3(-0.36f, 0.0f, -0.36f) + pos + Vector3(0.5f, 0.0f, 0.5f);
	btn1 = Vector3(0.13f, 0.0f, 0.49f) + pos + Vector3(0.5f, 0.0f, 0.5f);

	pVetex[idx++].setEx(m_sectionPos, top2.x, top2.y, top2.z, color, bright, maxU, minV);
	pVetex[idx++].setEx(m_sectionPos, top1.x, top1.y, top1.z, color, bright, minU, minV);
	pVetex[idx++].setEx(m_sectionPos, btn2.x, btn2.y, btn2.z, color, bright, minU, maxV);
	pVetex[idx++].setEx(m_sectionPos, btn1.x, btn1.y, btn1.z, color, bright, maxU, maxV);
	//AutoCaculateQuadNormal(pVetex + idx - 4);

	pVetex[idx++].setEx(m_sectionPos, top2.x, top2.y, top2.z, color, bright, maxU, minV);
	pVetex[idx++].setEx(m_sectionPos, btn1.x, btn1.y, btn1.z, color, bright, maxU, maxV);
	pVetex[idx++].setEx(m_sectionPos, btn2.x, btn2.y, btn2.z, color, bright, minU, maxV);
	pVetex[idx++].setEx(m_sectionPos, top1.x, top1.y, top1.z, color, bright, minU, minV);
	//AutoCaculateQuadNormal(pVetex + idx - 4);

	// face3
	top1 = Vector3(-0.5f, 0.64f, -0.5f) + pos + Vector3(0.5f, 0.0f, 0.5f);
	top2 = Vector3(0.5f, 0.64f, -0.5f) + pos + Vector3(0.5f, 0.0f, 0.5f);
	btn2 = Vector3(-0.5f, 0.0f, 0.26f) + pos + Vector3(0.5f, 0.0f, 0.5f);
	btn1 = Vector3(0.5f, 0.0f, 0.26f) + pos + Vector3(0.5f, 0.0f, 0.5f);

	pVetex[idx++].setEx(m_sectionPos, top2.x, top2.y, top2.z, color, bright, maxU, minV);
	pVetex[idx++].setEx(m_sectionPos, top1.x, top1.y, top1.z, color, bright, minU, minV);
	pVetex[idx++].setEx(m_sectionPos, btn2.x, btn2.y, btn2.z, color, bright, minU, maxV);
	pVetex[idx++].setEx(m_sectionPos, btn1.x, btn1.y, btn1.z, color, bright, maxU, maxV);
	//AutoCaculateQuadNormal(pVetex + idx - 4);

	pVetex[idx++].setEx(m_sectionPos, top2.x, top2.y, top2.z, color, bright, maxU, minV);
	pVetex[idx++].setEx(m_sectionPos, btn1.x, btn1.y, btn1.z, color, bright, maxU, maxV);
	pVetex[idx++].setEx(m_sectionPos, btn2.x, btn2.y, btn2.z, color, bright, minU, maxV);
	pVetex[idx++].setEx(m_sectionPos, top1.x, top1.y, top1.z, color, bright, minU, minV);
	//AutoCaculateQuadNormal(pVetex + idx - 4);

	// face4
	top1 = Vector3(-0.5f, 0.64f, 0.5f) + pos + Vector3(0.5f, 0.0f, 0.5f);
	top2 = Vector3(0.5f, 0.64f, 0.5f) + pos + Vector3(0.5f, 0.0f, 0.5f);
	btn2 = Vector3(-0.5f, 0.0f, -0.26f) + pos + Vector3(0.5f, 0.0f, 0.5f);
	btn1 = Vector3(0.5f, 0.0f, -0.26f) + pos + Vector3(0.5f, 0.0f, 0.5f);

	pVetex[idx++].setEx(m_sectionPos, top2.x, top2.y, top2.z, color, bright, maxU, minV);
	pVetex[idx++].setEx(m_sectionPos, top1.x, top1.y, top1.z, color, bright, minU, minV);
	pVetex[idx++].setEx(m_sectionPos, btn2.x, btn2.y, btn2.z, color, bright, minU, maxV);
	pVetex[idx++].setEx(m_sectionPos, btn1.x, btn1.y, btn1.z, color, bright, maxU, maxV);
	//AutoCaculateQuadNormal(pVetex + idx - 4);

	pVetex[idx++].setEx(m_sectionPos, top2.x, top2.y, top2.z, color, bright, maxU, minV);
	pVetex[idx++].setEx(m_sectionPos, btn1.x, btn1.y, btn1.z, color, bright, maxU, maxV);
	pVetex[idx++].setEx(m_sectionPos, btn2.x, btn2.y, btn2.z, color, bright, minU, maxV);
	pVetex[idx++].setEx(m_sectionPos, top1.x, top1.y, top1.z, color, bright, minU, minV);
	//AutoCaculateQuadNormal(pVetex + idx - 4);

	// face5
	top1 = Vector3(-0.5f, 0.64f, -0.5f) + pos + Vector3(0.5f, 0.0f, 0.5f);
	top2 = Vector3(-0.5f, 0.64f, 0.5f) + pos + Vector3(0.5f, 0.0f, 0.5f);
	btn2 = Vector3(0.26f, 0.0f, -0.5f) + pos + Vector3(0.5f, 0.0f, 0.5f);
	btn1 = Vector3(0.26f, 0.0f, 0.5f) + pos + Vector3(0.5f, 0.0f, 0.5f);

	pVetex[idx++].setEx(m_sectionPos, top2.x, top2.y, top2.z, color, bright, maxU, minV);
	pVetex[idx++].setEx(m_sectionPos, top1.x, top1.y, top1.z, color, bright, minU, minV);
	pVetex[idx++].setEx(m_sectionPos, btn2.x, btn2.y, btn2.z, color, bright, minU, maxV);
	pVetex[idx++].setEx(m_sectionPos, btn1.x, btn1.y, btn1.z, color, bright, maxU, maxV);
	//AutoCaculateQuadNormal(pVetex + idx - 4);

	pVetex[idx++].setEx(m_sectionPos, top2.x, top2.y, top2.z, color, bright, maxU, minV);
	pVetex[idx++].setEx(m_sectionPos, btn1.x, btn1.y, btn1.z, color, bright, maxU, maxV);
	pVetex[idx++].setEx(m_sectionPos, btn2.x, btn2.y, btn2.z, color, bright, minU, maxV);
	pVetex[idx++].setEx(m_sectionPos, top1.x, top1.y, top1.z, color, bright, minU, minV);
	//AutoCaculateQuadNormal(pVetex + idx - 4);

	// face6
	top1 = Vector3(0.5f, 0.64f, -0.5f) + pos + Vector3(0.5f, 0.0f, 0.5f);
	top2 = Vector3(0.5f, 0.64f, 0.5f) + pos + Vector3(0.5f, 0.0f, 0.5f);
	btn2 = Vector3(-0.26f, 0.0f, -0.5f) + pos + Vector3(0.5f, 0.0f, 0.5f);
	btn1 = Vector3(-0.26f, 0.0f, 0.5f) + pos + Vector3(0.5f, 0.0f, 0.5f);

	pVetex[idx++].setEx(m_sectionPos, top2.x, top2.y, top2.z, color, bright, maxU, minV);
	pVetex[idx++].setEx(m_sectionPos, top1.x, top1.y, top1.z, color, bright, minU, minV);
	pVetex[idx++].setEx(m_sectionPos, btn2.x, btn2.y, btn2.z, color, bright, minU, maxV);
	pVetex[idx++].setEx(m_sectionPos, btn1.x, btn1.y, btn1.z, color, bright, maxU, maxV);
	//AutoCaculateQuadNormal(pVetex + idx - 4);

	pVetex[idx++].setEx(m_sectionPos, top2.x, top2.y, top2.z, color, bright, maxU, minV);
	pVetex[idx++].setEx(m_sectionPos, btn1.x, btn1.y, btn1.z, color, bright, maxU, maxV);
	pVetex[idx++].setEx(m_sectionPos, btn2.x, btn2.y, btn2.z, color, bright, minU, maxV);
	pVetex[idx++].setEx(m_sectionPos, top1.x, top1.y, top1.z, color, bright, minU, minV);
	//AutoCaculateQuadNormal(pVetex + idx - 4);
}

bool RenderBlocks::renderStandardBlockWithAmbientOcclusion(Block* pBlock, cBlock* pcBlock, const BlockPos& pos, const Color& _color)
{
	enableAO = true;
	bool var8 = false;
	bool hasVertexColor = true;
	BlockPos ipos = pos;
	int var14 = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos);
	
	if (getBlockIcon(pcBlock)->getSpriteName() == "grass_top")
		hasVertexColor = false;
	else if (hasOverrideBlockTexture())
		hasVertexColor = false;

	bool var16;
	bool var17;
	bool var18;
	bool var19;
	int var20;
	float var21;

	if (renderAllFaces || pBlock->shouldSideBeRendered(blockAccess, ipos.getNegY(), BM_FACE_DOWN))
	{
		if (renderMinY <= 0.0)
		{
			--ipos.y;
		}

		aoBrightnessXYNN = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getNegX());
		aoBrightnessYZNN = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getNegZ());
		aoBrightnessYZNP = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getPosZ());
		aoBrightnessXYPN = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getPosX());
		aoLightValueScratchXYNN = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getNegX());
		aoLightValueScratchYZNN = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getNegZ());
		aoLightValueScratchYZNP = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getPosZ());
		aoLightValueScratchXYPN = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getPosX());
		var16 = BlockManager::sCanBlockGrass[blockAccess->getBlockId(ipos.getDE())] != 0;
		var17 = BlockManager::sCanBlockGrass[blockAccess->getBlockId(ipos.getDW())] != 0;
		var18 = BlockManager::sCanBlockGrass[blockAccess->getBlockId(ipos.getDS())] != 0;
		var19 = BlockManager::sCanBlockGrass[blockAccess->getBlockId(ipos.getDN())] != 0;

		if (!var19 && !var17)
		{
			aoLightValueScratchXYZNNN = aoLightValueScratchXYNN;
			aoBrightnessXYZNNN = aoBrightnessXYNN;
		}
		else
		{
			aoLightValueScratchXYZNNN = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getNW());
			aoBrightnessXYZNNN = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getNW());
		}

		if (!var18 && !var17)
		{
			aoLightValueScratchXYZNNP = aoLightValueScratchXYNN;
			aoBrightnessXYZNNP = aoBrightnessXYNN;
		}
		else
		{
			aoLightValueScratchXYZNNP = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getSW());
			aoBrightnessXYZNNP = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getSW());
		}

		if (!var19 && !var16)
		{
			aoLightValueScratchXYZPNN = aoLightValueScratchXYPN;
			aoBrightnessXYZPNN = aoBrightnessXYPN;
		}
		else
		{
			aoLightValueScratchXYZPNN = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getNE());
			aoBrightnessXYZPNN = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getNE());
		}

		if (!var18 && !var16)
		{
			aoLightValueScratchXYZPNP = aoLightValueScratchXYPN;
			aoBrightnessXYZPNP = aoBrightnessXYPN;
		}
		else
		{
			aoLightValueScratchXYZPNP = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getSE());
			aoBrightnessXYZPNP = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getSE());
		}

		if (renderMinY <= 0.0)
		{
			++ipos.y;
		}

		var20 = var14;

		if (renderMinY <= 0.0 || !blockAccess->isBlockOpaqueCube(ipos.getNegY()))
		{
			var20 = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getNegY());
		}

		var21 = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getNegY());
		aoTopLeft = (aoLightValueScratchXYZNNP + aoLightValueScratchXYNN + aoLightValueScratchYZNP + var21) / 4.0F;
		aoTopRgith = (aoLightValueScratchYZNP + var21 + aoLightValueScratchXYZPNP + aoLightValueScratchXYPN) / 4.0F;
		aoBottomRight = (var21 + aoLightValueScratchYZNN + aoLightValueScratchXYPN + aoLightValueScratchXYZPNN) / 4.0F;
		aoBottomLeft = (aoLightValueScratchXYNN + aoLightValueScratchXYZNNN + var21 + aoLightValueScratchYZNN) / 4.0F;

		brightnessTopLeft = getAoBrightness(aoBrightnessXYZNNP, aoBrightnessXYNN, aoBrightnessYZNP, var20);
		brightnessTopRight = getAoBrightness(aoBrightnessYZNP, aoBrightnessXYZPNP, aoBrightnessXYPN, var20);
		brightnessBottomRight = getAoBrightness(aoBrightnessYZNN, aoBrightnessXYPN, aoBrightnessXYZPNN, var20);
		brightnessBottomLeft = getAoBrightness(aoBrightnessXYNN, aoBrightnessXYZNNN, aoBrightnessYZNN, var20);

		if (hasVertexColor)
			colorTopLeft = colorBottomLeft = colorBottomRight = colorTopRight = _color * 0.5f;
		else
			colorTopLeft = colorBottomLeft = colorBottomRight = colorTopRight = Color(0.5f, 0.5f, 0.5f, 1.0f);
		renderFaceYNeg(pcBlock, Vector3(ipos), getBlockIcon(pcBlock, blockAccess, ipos, 0));
		var8 = true;
	}

	if (renderAllFaces || pBlock->shouldSideBeRendered(blockAccess, ipos.getPosY(), BM_FACE_UP))
	{
		if (renderMaxY >= 1.0)
		{
			++ipos.y;
		}

		aoBrightnessXYNP = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getNegX());
		aoBrightnessXYPP = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getPosX());
		aoBrightnessYZPN = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getNegZ());
		aoBrightnessYZPP = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getPosZ());
		aoLightValueScratchXYNP = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getNegX());
		aoLightValueScratchXYPP = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getPosX());
		aoLightValueScratchYZPN = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getNegZ());
		aoLightValueScratchYZPP = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getPosZ());
		var16 = BlockManager::sCanBlockGrass[blockAccess->getBlockId(ipos.getUE())] != 0;
		var17 = BlockManager::sCanBlockGrass[blockAccess->getBlockId(ipos.getUW())] != 0;
		var18 = BlockManager::sCanBlockGrass[blockAccess->getBlockId(ipos.getUS())] != 0;
		var19 = BlockManager::sCanBlockGrass[blockAccess->getBlockId(ipos.getUN())] != 0;

		if (!var19 && !var17)
		{
			aoLightValueScratchXYZNPN = aoLightValueScratchXYNP;
			aoBrightnessXYZNPN = aoBrightnessXYNP;
		}
		else
		{
			aoLightValueScratchXYZNPN = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getNW());
			aoBrightnessXYZNPN = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getNW());
		}

		if (!var19 && !var16)
		{
			aoLightValueScratchXYZPPN = aoLightValueScratchXYPP;
			aoBrightnessXYZPPN = aoBrightnessXYPP;
		}
		else
		{
			aoLightValueScratchXYZPPN = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getNE());
			aoBrightnessXYZPPN = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getNE());
		}

		if (!var18 && !var17)
		{
			aoLightValueScratchXYZNPP = aoLightValueScratchXYNP;
			aoBrightnessXYZNPP = aoBrightnessXYNP;
		}
		else
		{
			aoLightValueScratchXYZNPP = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getSW());
			aoBrightnessXYZNPP = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getSW());
		}

		if (!var18 && !var16)
		{
			aoLightValueScratchXYZPPP = aoLightValueScratchXYPP;
			aoBrightnessXYZPPP = aoBrightnessXYPP;
		}
		else
		{
			aoLightValueScratchXYZPPP = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getSE());
			aoBrightnessXYZPPP = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getSE());
		}

		if (renderMaxY >= 1.0)
		{
			--ipos.y;
		}

		var20 = var14;

		if (renderMaxY >= 1.0 || !blockAccess->isBlockOpaqueCube(ipos.getPosY()))
		{
			var20 = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getPosY());
		}

		var21 = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getPosY());
		aoTopRgith = (aoLightValueScratchXYZNPP + aoLightValueScratchXYNP + aoLightValueScratchYZPP + var21) / 4.0F;
		aoTopLeft = (aoLightValueScratchYZPP + var21 + aoLightValueScratchXYZPPP + aoLightValueScratchXYPP) / 4.0F;
		aoBottomLeft = (var21 + aoLightValueScratchYZPN + aoLightValueScratchXYPP + aoLightValueScratchXYZPPN) / 4.0F;
		aoBottomRight = (aoLightValueScratchXYNP + aoLightValueScratchXYZNPN + var21 + aoLightValueScratchYZPN) / 4.0F;

		brightnessTopRight = getAoBrightness(aoBrightnessXYZNPP, aoBrightnessXYNP, aoBrightnessYZPP, var20);
		brightnessTopLeft = getAoBrightness(aoBrightnessYZPP, aoBrightnessXYZPPP, aoBrightnessXYPP, var20);
		brightnessBottomLeft = getAoBrightness(aoBrightnessYZPN, aoBrightnessXYPP, aoBrightnessXYZPPN, var20);
		brightnessBottomRight = getAoBrightness(aoBrightnessXYNP, aoBrightnessXYZNPN, aoBrightnessYZPN, var20);

		colorTopLeft = colorBottomLeft = colorBottomRight = colorTopRight = _color;

		renderFaceYPos(pcBlock, Vector3(ipos), getBlockIcon(pcBlock, blockAccess, ipos, 1));
		var8 = true;
	}

	AtlasSprite* pSprite = NULL;

	if (renderAllFaces || pBlock->shouldSideBeRendered(blockAccess, ipos.getNegZ(), BM_FACE_NORTH))
	{
		if (renderMinZ <= 0.0)
		{
			--ipos.z;
		}

		aoLightValueScratchXZNN = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getNegX());
		aoLightValueScratchYZNN = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getNegY());
		aoLightValueScratchYZPN = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getPosY());
		aoLightValueScratchXZPN = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getPosX());
		aoBrightnessXZNN = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getNegX());
		aoBrightnessYZNN = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getNegY());
		aoBrightnessYZPN = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getPosY());
		aoBrightnessXZPN = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getPosX());
		var16 = BlockManager::sCanBlockGrass[blockAccess->getBlockId(ipos.getNE())] != 0;
		var17 = BlockManager::sCanBlockGrass[blockAccess->getBlockId(ipos.getNW())] != 0;
		var18 = BlockManager::sCanBlockGrass[blockAccess->getBlockId(ipos.getUN())] != 0;
		var19 = BlockManager::sCanBlockGrass[blockAccess->getBlockId(ipos.getDN())] != 0;

		if (!var17 && !var19)
		{
			aoLightValueScratchXYZNNN = aoLightValueScratchXZNN;
			aoBrightnessXYZNNN = aoBrightnessXZNN;
		}
		else
		{
			aoLightValueScratchXYZNNN = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getDW());
			aoBrightnessXYZNNN = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getDW());
		}

		if (!var17 && !var18)
		{
			aoLightValueScratchXYZNPN = aoLightValueScratchXZNN;
			aoBrightnessXYZNPN = aoBrightnessXZNN;
		}
		else
		{
			aoLightValueScratchXYZNPN = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getUW());
			aoBrightnessXYZNPN = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getUW());
		}

		if (!var16 && !var19)
		{
			aoLightValueScratchXYZPNN = aoLightValueScratchXZPN;
			aoBrightnessXYZPNN = aoBrightnessXZPN;
		}
		else
		{
			aoLightValueScratchXYZPNN = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getDE());
			aoBrightnessXYZPNN = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getDE());
		}

		if (!var16 && !var18)
		{
			aoLightValueScratchXYZPPN = aoLightValueScratchXZPN;
			aoBrightnessXYZPPN = aoBrightnessXZPN;
		}
		else
		{
			aoLightValueScratchXYZPPN = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getUE());
			aoBrightnessXYZPPN = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getUE());
		}

		if (renderMinZ <= 0.0)
		{
			++ipos.z;
		}

		var20 = var14;

		if (renderMinZ <= 0.0 || !blockAccess->isBlockOpaqueCube(ipos.getNegZ()))
		{
			var20 = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getNegZ());
		}

		var21 = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getNegZ());
		aoTopLeft = (aoLightValueScratchXZNN + aoLightValueScratchXYZNPN + var21 + aoLightValueScratchYZPN) / 4.0F;
		aoBottomLeft = (var21 + aoLightValueScratchYZPN + aoLightValueScratchXZPN + aoLightValueScratchXYZPPN) / 4.0F;
		aoBottomRight = (aoLightValueScratchYZNN + var21 + aoLightValueScratchXYZPNN + aoLightValueScratchXZPN) / 4.0F;
		aoTopRgith = (aoLightValueScratchXYZNNN + aoLightValueScratchXZNN + aoLightValueScratchYZNN + var21) / 4.0F;

		brightnessTopLeft = getAoBrightness(aoBrightnessXZNN, aoBrightnessXYZNPN, aoBrightnessYZPN, var20);
		brightnessBottomLeft = getAoBrightness(aoBrightnessYZPN, aoBrightnessXZPN, aoBrightnessXYZPPN, var20);
		brightnessBottomRight = getAoBrightness(aoBrightnessYZNN, aoBrightnessXYZPNN, aoBrightnessXZPN, var20);
		brightnessTopRight = getAoBrightness(aoBrightnessXYZNNN, aoBrightnessXZNN, aoBrightnessYZNN, var20);

		if (hasVertexColor)
			colorTopLeft = colorBottomLeft = colorBottomRight = colorTopRight = _color * 0.8f;
		else
			colorTopLeft = colorBottomLeft = colorBottomRight = colorTopRight = Color(0.8f, 0.8f, 0.8f, 1.0f);
		pSprite = getBlockIcon(pcBlock, blockAccess, ipos, 2);
		
		if (fancyGrass && pSprite->getSpriteName() == "grass_side" && !hasOverrideBlockTexture())
		{
			Color _topleft = colorTopLeft;
			Color _bottomLeft = colorBottomLeft;
			Color _bottomRight = colorBottomRight;
			Color _topRight = colorTopRight;
			colorTopLeft *= _color;
			colorBottomLeft *= _color;
			colorBottomRight *= _color;
			colorTopRight *= _color;
			renderFaceZNeg(pcBlock, Vector3(ipos), cBlockGrass::getIconSideOverlay());
			colorTopLeft = _topleft;
			colorBottomLeft = _bottomLeft;
			colorBottomRight = _bottomRight;
			colorTopRight = _topRight;
		}

		renderFaceZNeg(pcBlock, Vector3(ipos), pSprite);

		var8 = true;
	}

	if (renderAllFaces || pBlock->shouldSideBeRendered(blockAccess, ipos.getPosZ(), BM_FACE_SOUTH))
	{
		if (renderMaxZ >= 1.0)
		{
			++ipos.z;
		}

		aoLightValueScratchXZNP = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getNegX());
		aoLightValueScratchXZPP = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getPosX());
		aoLightValueScratchYZNP = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getNegY());
		aoLightValueScratchYZPP = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getPosY());
		aoBrightnessXZNP = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getNegX());
		aoBrightnessXZPP = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getPosX());
		aoBrightnessYZNP = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getNegY());
		aoBrightnessYZPP = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getPosY());
		var16 = BlockManager::sCanBlockGrass[blockAccess->getBlockId(ipos.getSE())] != 0;
		var17 = BlockManager::sCanBlockGrass[blockAccess->getBlockId(ipos.getSW())] != 0;
		var18 = BlockManager::sCanBlockGrass[blockAccess->getBlockId(ipos.getUS())] != 0;
		var19 = BlockManager::sCanBlockGrass[blockAccess->getBlockId(ipos.getDS())] != 0;

		if (!var17 && !var19)
		{
			aoLightValueScratchXYZNNP = aoLightValueScratchXZNP;
			aoBrightnessXYZNNP = aoBrightnessXZNP;
		}
		else
		{
			aoLightValueScratchXYZNNP = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getDW());
			aoBrightnessXYZNNP = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getDW());
		}

		if (!var17 && !var18)
		{
			aoLightValueScratchXYZNPP = aoLightValueScratchXZNP;
			aoBrightnessXYZNPP = aoBrightnessXZNP;
		}
		else
		{
			aoLightValueScratchXYZNPP = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getUW());
			aoBrightnessXYZNPP = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getUW());
		}

		if (!var16 && !var19)
		{
			aoLightValueScratchXYZPNP = aoLightValueScratchXZPP;
			aoBrightnessXYZPNP = aoBrightnessXZPP;
		}
		else
		{
			aoLightValueScratchXYZPNP = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getDE());
			aoBrightnessXYZPNP = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getDE());
		}

		if (!var16 && !var18)
		{
			aoLightValueScratchXYZPPP = aoLightValueScratchXZPP;
			aoBrightnessXYZPPP = aoBrightnessXZPP;
		}
		else
		{
			aoLightValueScratchXYZPPP = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getUE());
			aoBrightnessXYZPPP = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getUE());
		}

		if (renderMaxZ >= 1.0)
		{
			--ipos.z;
		}

		var20 = var14;

		if (renderMaxZ >= 1.0 || !blockAccess->isBlockOpaqueCube(ipos.getPosZ()))
		{
			var20 = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getPosZ());
		}

		var21 = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getPosZ());
		aoTopLeft = (aoLightValueScratchXZNP + aoLightValueScratchXYZNPP + var21 + aoLightValueScratchYZPP) / 4.0F;
		aoTopRgith = (var21 + aoLightValueScratchYZPP + aoLightValueScratchXZPP + aoLightValueScratchXYZPPP) / 4.0F;
		aoBottomRight = (aoLightValueScratchYZNP + var21 + aoLightValueScratchXYZPNP + aoLightValueScratchXZPP) / 4.0F;
		aoBottomLeft = (aoLightValueScratchXYZNNP + aoLightValueScratchXZNP + aoLightValueScratchYZNP + var21) / 4.0F;
		brightnessTopLeft = getAoBrightness(aoBrightnessXZNP, aoBrightnessXYZNPP, aoBrightnessYZPP, var20);
		brightnessTopRight = getAoBrightness(aoBrightnessYZPP, aoBrightnessXZPP, aoBrightnessXYZPPP, var20);
		brightnessBottomRight = getAoBrightness(aoBrightnessYZNP, aoBrightnessXYZPNP, aoBrightnessXZPP, var20);
		brightnessBottomLeft = getAoBrightness(aoBrightnessXYZNNP, aoBrightnessXZNP, aoBrightnessYZNP, var20);

		if (hasVertexColor)
			colorTopLeft = colorBottomLeft = colorBottomRight = colorTopRight = _color * 0.8f;
		else
			colorTopLeft = colorBottomLeft = colorBottomRight = colorTopRight = Color(0.8f, 0.8f, 0.8f, 1.0f);
		pSprite = getBlockIcon(pcBlock, blockAccess, ipos, 3);
		
		if (fancyGrass && pSprite->getSpriteName() == "grass_side" && !hasOverrideBlockTexture())
		{
			Color _topleft = colorTopLeft;
			Color _bottomLeft = colorBottomLeft;
			Color _bottomRight = colorBottomRight;
			Color _topRight = colorTopRight;
			colorTopLeft *= _color;
			colorBottomLeft *= _color;
			colorBottomRight *= _color;
			colorTopRight *= _color;
			renderFaceZPos(pcBlock, Vector3(ipos), cBlockGrass::getIconSideOverlay());
			colorTopLeft = _topleft;
			colorBottomLeft = _bottomLeft;
			colorBottomRight = _bottomRight;
			colorTopRight = _topRight;
		}
		renderFaceZPos(pcBlock, Vector3(ipos), getBlockIcon(pcBlock, blockAccess, ipos, 3));

		var8 = true;
	}

	if (renderAllFaces || pBlock->shouldSideBeRendered(blockAccess, ipos.getNegX(), BM_FACE_WEST))
	{
		if (renderMinX <= 0.0)
		{
			--ipos.x;
		}

		aoLightValueScratchXYNN = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getNegY());
		aoLightValueScratchXZNN = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getNegZ());
		aoLightValueScratchXZNP = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getPosZ());
		aoLightValueScratchXYNP = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getPosY());
		aoBrightnessXYNN = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getNegY());
		aoBrightnessXZNN = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getNegZ());
		aoBrightnessXZNP = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getPosZ());
		aoBrightnessXYNP = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getPosY());
		var16 = BlockManager::sCanBlockGrass[blockAccess->getBlockId(ipos.getUW())] != 0;
		var17 = BlockManager::sCanBlockGrass[blockAccess->getBlockId(ipos.getDW())] != 0;
		var18 = BlockManager::sCanBlockGrass[blockAccess->getBlockId(ipos.getNW())] != 0;
		var19 = BlockManager::sCanBlockGrass[blockAccess->getBlockId(ipos.getSW())] != 0;

		if (!var18 && !var17)
		{
			aoLightValueScratchXYZNNN = aoLightValueScratchXZNN;
			aoBrightnessXYZNNN = aoBrightnessXZNN;
		}
		else
		{
			aoLightValueScratchXYZNNN = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getDN());
			aoBrightnessXYZNNN = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getDN());
		}

		if (!var19 && !var17)
		{
			aoLightValueScratchXYZNNP = aoLightValueScratchXZNP;
			aoBrightnessXYZNNP = aoBrightnessXZNP;
		}
		else
		{
			aoLightValueScratchXYZNNP = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getDS());
			aoBrightnessXYZNNP = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getDS());
		}

		if (!var18 && !var16)
		{
			aoLightValueScratchXYZNPN = aoLightValueScratchXZNN;
			aoBrightnessXYZNPN = aoBrightnessXZNN;
		}
		else
		{
			aoLightValueScratchXYZNPN = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getUN());
			aoBrightnessXYZNPN = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getUN());
		}

		if (!var19 && !var16)
		{
			aoLightValueScratchXYZNPP = aoLightValueScratchXZNP;
			aoBrightnessXYZNPP = aoBrightnessXZNP;
		}
		else
		{
			aoLightValueScratchXYZNPP = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getUS());
			aoBrightnessXYZNPP = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getUS());
		}

		if (renderMinX <= 0.0)
		{
			++ipos.x;
		}

		var20 = var14;

		if (renderMinX <= 0.0 || !blockAccess->isBlockOpaqueCube(ipos.getNegX()))
		{
			var20 = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getNegX());
		}

		var21 = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getNegX());
		aoTopRgith = (aoLightValueScratchXYNN + aoLightValueScratchXYZNNP + var21 + aoLightValueScratchXZNP) / 4.0F;
		aoTopLeft = (var21 + aoLightValueScratchXZNP + aoLightValueScratchXYNP + aoLightValueScratchXYZNPP) / 4.0F;
		aoBottomLeft = (aoLightValueScratchXZNN + var21 + aoLightValueScratchXYZNPN + aoLightValueScratchXYNP) / 4.0F;
		aoBottomRight = (aoLightValueScratchXYZNNN + aoLightValueScratchXYNN + aoLightValueScratchXZNN + var21) / 4.0F;
		brightnessTopRight = getAoBrightness(aoBrightnessXYNN, aoBrightnessXYZNNP, aoBrightnessXZNP, var20);
		brightnessTopLeft = getAoBrightness(aoBrightnessXZNP, aoBrightnessXYNP, aoBrightnessXYZNPP, var20);
		brightnessBottomLeft = getAoBrightness(aoBrightnessXZNN, aoBrightnessXYZNPN, aoBrightnessXYNP, var20);
		brightnessBottomRight = getAoBrightness(aoBrightnessXYZNNN, aoBrightnessXYNN, aoBrightnessXZNN, var20);

		if (hasVertexColor)
			colorTopLeft = colorBottomLeft = colorBottomRight = colorTopRight = _color * 0.8f;
		else
			colorTopLeft = colorBottomLeft = colorBottomRight = colorTopRight = Color(0.8f, 0.8f, 0.8f, 1.0f);
		pSprite = getBlockIcon(pcBlock, blockAccess, ipos, 4);
		
		if (fancyGrass && pSprite->getSpriteName() == "grass_side" && !hasOverrideBlockTexture())
		{
			Color _topleft = colorTopLeft;
			Color _bottomLeft = colorBottomLeft;
			Color _bottomRight = colorBottomRight;
			Color _topRight = colorTopRight;
			colorTopLeft *= _color;
			colorBottomLeft *= _color;
			colorBottomRight *= _color;
			colorTopRight *= _color;
			renderFaceXNeg(pcBlock, Vector3(ipos), cBlockGrass::getIconSideOverlay());
			colorTopLeft = _topleft;
			colorBottomLeft = _bottomLeft;
			colorBottomRight = _bottomRight;
			colorTopRight = _topRight;
		}
		renderFaceXNeg(pcBlock, Vector3(ipos), pSprite);

		var8 = true;
	}

	if (renderAllFaces || pBlock->shouldSideBeRendered(blockAccess, ipos.getPosX(), BM_FACE_EAST))
	{
		if (renderMaxX >= 1.0)
		{
			++ipos.x;
		}

		aoLightValueScratchXYPN = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getNegY());
		aoLightValueScratchXZPN = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getNegZ());
		aoLightValueScratchXZPP = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getPosZ());
		aoLightValueScratchXYPP = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getPosY());
		aoBrightnessXYPN = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getNegY());
		aoBrightnessXZPN = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getNegZ());
		aoBrightnessXZPP = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getPosZ());
		aoBrightnessXYPP = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getPosY());
		var16 = BlockManager::sCanBlockGrass[blockAccess->getBlockId(ipos.getUE())] != 0;
		var17 = BlockManager::sCanBlockGrass[blockAccess->getBlockId(ipos.getDE())] != 0;
		var18 = BlockManager::sCanBlockGrass[blockAccess->getBlockId(ipos.getSE())] != 0;
		var19 = BlockManager::sCanBlockGrass[blockAccess->getBlockId(ipos.getNE())] != 0;

		if (!var17 && !var19)
		{
			aoLightValueScratchXYZPNN = aoLightValueScratchXZPN;
			aoBrightnessXYZPNN = aoBrightnessXZPN;
		}
		else
		{
			aoLightValueScratchXYZPNN = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getDN());
			aoBrightnessXYZPNN = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getDN());
		}

		if (!var17 && !var18)
		{
			aoLightValueScratchXYZPNP = aoLightValueScratchXZPP;
			aoBrightnessXYZPNP = aoBrightnessXZPP;
		}
		else
		{
			aoLightValueScratchXYZPNP = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getDS());
			aoBrightnessXYZPNP = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getDS());
		}

		if (!var16 && !var19)
		{
			aoLightValueScratchXYZPPN = aoLightValueScratchXZPN;
			aoBrightnessXYZPPN = aoBrightnessXZPN;
		}
		else
		{
			aoLightValueScratchXYZPPN = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getUN());
			aoBrightnessXYZPPN = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getUN());
		}

		if (!var16 && !var18)
		{
			aoLightValueScratchXYZPPP = aoLightValueScratchXZPP;
			aoBrightnessXYZPPP = aoBrightnessXZPP;
		}
		else
		{
			aoLightValueScratchXYZPPP = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getUS());
			aoBrightnessXYZPPP = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getUS());
		}

		if (renderMaxX >= 1.0)
		{
			--ipos.x;
		}

		var20 = var14;

		if (renderMaxX >= 1.0 || !blockAccess->isBlockOpaqueCube(ipos.getPosX()))
		{
			var20 = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getPosX());
		}

		var21 = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getPosX());
		aoTopLeft = (aoLightValueScratchXYPN + aoLightValueScratchXYZPNP + var21 + aoLightValueScratchXZPP) / 4.0F;
		aoBottomLeft = (aoLightValueScratchXYZPNN + aoLightValueScratchXYPN + aoLightValueScratchXZPN + var21) / 4.0F;
		aoBottomRight = (aoLightValueScratchXZPN + var21 + aoLightValueScratchXYZPPN + aoLightValueScratchXYPP) / 4.0F;
		aoTopRgith = (var21 + aoLightValueScratchXZPP + aoLightValueScratchXYPP + aoLightValueScratchXYZPPP) / 4.0F;
		brightnessTopLeft = getAoBrightness(aoBrightnessXYPN, aoBrightnessXYZPNP, aoBrightnessXZPP, var20);
		brightnessTopRight = getAoBrightness(aoBrightnessXZPP, aoBrightnessXYPP, aoBrightnessXYZPPP, var20);
		brightnessBottomRight = getAoBrightness(aoBrightnessXZPN, aoBrightnessXYZPPN, aoBrightnessXYPP, var20);
		brightnessBottomLeft = getAoBrightness(aoBrightnessXYZPNN, aoBrightnessXYPN, aoBrightnessXZPN, var20);

		if (hasVertexColor)
			colorTopLeft = colorBottomLeft = colorBottomRight = colorTopRight = _color * 0.8f;
		else
			colorTopLeft = colorBottomLeft = colorBottomRight = colorTopRight = Color(0.8f, 0.8f, 0.8f, 1.0f);

		pSprite = getBlockIcon(pcBlock, blockAccess, ipos, 5);
		
		if (fancyGrass && pSprite->getSpriteName() == "grass_side" && !hasOverrideBlockTexture())
		{
			Color _topleft = colorTopLeft;
			Color _bottomLeft = colorBottomLeft;
			Color _bottomRight = colorBottomRight;
			Color _topRight = colorTopRight;
			colorTopLeft *= _color;
			colorBottomLeft *= _color;
			colorBottomRight *= _color;
			colorTopRight *= _color;
			renderFaceXPos(pcBlock, Vector3(ipos), cBlockGrass::getIconSideOverlay());
			colorTopLeft = _topleft;
			colorBottomLeft = _bottomLeft;
			colorBottomRight = _bottomRight;
			colorTopRight = _topRight;
		}

		renderFaceXPos(pcBlock, Vector3(ipos), pSprite);
		var8 = true;
	}

	enableAO = false;
	return var8;
}

bool RenderBlocks::renderBlockWithAmbientOcclusion(Block* pBlock, cBlock* pcBlock, const BlockPos& pos, const Color& _color)
{
	enableAO = true;
	bool var8 = false;
	bool hasVertexColor = true;
	BlockPos ipos = pos;
	int var14 = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos);
	
	if (getBlockIcon(pcBlock)->getSpriteName() == "grass_top")
	{
		hasVertexColor = false;
	}
	else if (hasOverrideBlockTexture())
	{
		hasVertexColor = false;
	}

	bool var16;
	bool var17;
	bool var18;
	bool var19;
	int var20;
	float var21;

	i8* canBlockGrass = BlockManager::sCanBlockGrass;

	if (renderAllFaces || pBlock->shouldSideBeRendered(blockAccess, ipos.getNegY(), 0))
	{
		if (renderMinY <= 0.0)
		{
			--ipos.y;
		}

		aoBrightnessXYNN = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getNegX());
		aoBrightnessYZNN = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getNegZ());
		aoBrightnessYZNP = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getPosZ());
		aoBrightnessXYPN = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getPosX());
		aoLightValueScratchXYNN = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getNegX());
		aoLightValueScratchYZNN = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getNegZ());
		aoLightValueScratchYZNP = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getPosZ());
		aoLightValueScratchXYPN = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getPosX());
		var16 = canBlockGrass[blockAccess->getBlockId(ipos.getDE())] != 0;
		var17 = canBlockGrass[blockAccess->getBlockId(ipos.getDW())] != 0;
		var18 = canBlockGrass[blockAccess->getBlockId(ipos.getDS())] != 0;
		var19 = canBlockGrass[blockAccess->getBlockId(ipos.getDN())] != 0;

		if (!var19 && !var17)
		{
			aoLightValueScratchXYZNNN = aoLightValueScratchXYNN;
			aoBrightnessXYZNNN = aoBrightnessXYNN;
		}
		else
		{
			aoLightValueScratchXYZNNN = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getNW());
			aoBrightnessXYZNNN = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getNW());
		}

		if (!var18 && !var17)
		{
			aoLightValueScratchXYZNNP = aoLightValueScratchXYNN;
			aoBrightnessXYZNNP = aoBrightnessXYNN;
		}
		else
		{
			aoLightValueScratchXYZNNP = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getSW());
			aoBrightnessXYZNNP = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getSW());
		}

		if (!var19 && !var16)
		{
			aoLightValueScratchXYZPNN = aoLightValueScratchXYPN;
			aoBrightnessXYZPNN = aoBrightnessXYPN;
		}
		else
		{
			aoLightValueScratchXYZPNN = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getNE());
			aoBrightnessXYZPNN = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getNE());
		}

		if (!var18 && !var16)
		{
			aoLightValueScratchXYZPNP = aoLightValueScratchXYPN;
			aoBrightnessXYZPNP = aoBrightnessXYPN;
		}
		else
		{
			aoLightValueScratchXYZPNP = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getSE());
			aoBrightnessXYZPNP = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getSE());
		}

		if (renderMinY <= 0.0)
		{
			++ipos.y;
		}

		var20 = var14;

		if (renderMinY <= 0.0 || !blockAccess->isBlockOpaqueCube(ipos.getNegY()))
		{
			var20 = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getNegY());
		}

		var21 = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getNegY());
		aoTopLeft = (aoLightValueScratchXYZNNP + aoLightValueScratchXYNN + aoLightValueScratchYZNP + var21) / 4.0F;
		aoTopRgith = (aoLightValueScratchYZNP + var21 + aoLightValueScratchXYZPNP + aoLightValueScratchXYPN) / 4.0F;
		aoBottomRight = (var21 + aoLightValueScratchYZNN + aoLightValueScratchXYPN + aoLightValueScratchXYZPNN) / 4.0F;
		aoBottomLeft = (aoLightValueScratchXYNN + aoLightValueScratchXYZNNN + var21 + aoLightValueScratchYZNN) / 4.0F;
		brightnessTopLeft = getAoBrightness(aoBrightnessXYZNNP, aoBrightnessXYNN, aoBrightnessYZNP, var20);
		brightnessTopRight = getAoBrightness(aoBrightnessYZNP, aoBrightnessXYZPNP, aoBrightnessXYPN, var20);
		brightnessBottomRight = getAoBrightness(aoBrightnessYZNN, aoBrightnessXYPN, aoBrightnessXYZPNN, var20);
		brightnessBottomLeft = getAoBrightness(aoBrightnessXYNN, aoBrightnessXYZNNN, aoBrightnessYZNN, var20);

		if (hasVertexColor)
			colorTopLeft = colorBottomLeft = colorBottomRight = colorTopRight = _color * 0.5f;
		else
			colorTopLeft = colorBottomLeft = colorBottomRight = colorTopRight = Color(0.5f, 0.5f, 0.5f, 1.0f);
		renderFaceYNeg(pcBlock, Vector3(ipos), getBlockIcon(pcBlock, blockAccess, ipos, 0));
		var8 = true;
	}

	if (renderAllFaces || pBlock->shouldSideBeRendered(blockAccess, ipos.getPosY(), 1))
	{
		if (renderMaxY >= 1.0)
		{
			++ipos.y;
		}

		aoBrightnessXYNP = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getNegX());
		aoBrightnessXYPP = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getPosX());
		aoBrightnessYZPN = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getNegZ());
		aoBrightnessYZPP = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getPosZ());
		aoLightValueScratchXYNP = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getNegX());
		aoLightValueScratchXYPP = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getPosX());
		aoLightValueScratchYZPN = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getNegZ());
		aoLightValueScratchYZPP = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getPosZ());
		var16 = canBlockGrass[blockAccess->getBlockId(ipos.getUE())] != 0;
		var17 = canBlockGrass[blockAccess->getBlockId(ipos.getUW())] != 0;
		var18 = canBlockGrass[blockAccess->getBlockId(ipos.getUS())] != 0;
		var19 = canBlockGrass[blockAccess->getBlockId(ipos.getUN())] != 0;

		if (!var19 && !var17)
		{
			aoLightValueScratchXYZNPN = aoLightValueScratchXYNP;
			aoBrightnessXYZNPN = aoBrightnessXYNP;
		}
		else
		{
			aoLightValueScratchXYZNPN = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getNW());
			aoBrightnessXYZNPN = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getNW());
		}

		if (!var19 && !var16)
		{
			aoLightValueScratchXYZPPN = aoLightValueScratchXYPP;
			aoBrightnessXYZPPN = aoBrightnessXYPP;
		}
		else
		{
			aoLightValueScratchXYZPPN = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getNE());
			aoBrightnessXYZPPN = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getNE());
		}

		if (!var18 && !var17)
		{
			aoLightValueScratchXYZNPP = aoLightValueScratchXYNP;
			aoBrightnessXYZNPP = aoBrightnessXYNP;
		}
		else
		{
			aoLightValueScratchXYZNPP = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getSW());
			aoBrightnessXYZNPP = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getSW());
		}

		if (!var18 && !var16)
		{
			aoLightValueScratchXYZPPP = aoLightValueScratchXYPP;
			aoBrightnessXYZPPP = aoBrightnessXYPP;
		}
		else
		{
			aoLightValueScratchXYZPPP = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getSE());
			aoBrightnessXYZPPP = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getSE());
		}

		if (renderMaxY >= 1.0)
		{
			--ipos.y;
		}

		var20 = var14;

		if (renderMaxY >= 1.0 || !blockAccess->isBlockOpaqueCube(ipos.getPosY()))
		{
			var20 = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getPosY());
		}

		var21 = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getPosY());
		aoTopRgith = (aoLightValueScratchXYZNPP + aoLightValueScratchXYNP + aoLightValueScratchYZPP + var21) / 4.0F;
		aoTopLeft = (aoLightValueScratchYZPP + var21 + aoLightValueScratchXYZPPP + aoLightValueScratchXYPP) / 4.0F;
		aoBottomLeft = (var21 + aoLightValueScratchYZPN + aoLightValueScratchXYPP + aoLightValueScratchXYZPPN) / 4.0F;
		aoBottomRight = (aoLightValueScratchXYNP + aoLightValueScratchXYZNPN + var21 + aoLightValueScratchYZPN) / 4.0F;
		brightnessTopRight = getAoBrightness(aoBrightnessXYZNPP, aoBrightnessXYNP, aoBrightnessYZPP, var20);
		brightnessTopLeft = getAoBrightness(aoBrightnessYZPP, aoBrightnessXYZPPP, aoBrightnessXYPP, var20);
		brightnessBottomLeft = getAoBrightness(aoBrightnessYZPN, aoBrightnessXYPP, aoBrightnessXYZPPN, var20);
		brightnessBottomRight = getAoBrightness(aoBrightnessXYNP, aoBrightnessXYZNPN, aoBrightnessYZPN, var20);

		colorTopLeft = colorTopRight = colorBottomLeft = colorBottomRight = _color;
		renderFaceYPos(pcBlock, Vector3(ipos), getBlockIcon(pcBlock, blockAccess, ipos, 1));
		var8 = true;
	}

	float var22;
	float var23;
	float var24;
	float var25;
	int var26;
	int var27;
	int var28;
	int var29;
	AtlasSprite* pSprite = NULL;

	if (renderAllFaces || pBlock->shouldSideBeRendered(blockAccess, ipos.getNegZ(), 2))
	{
		if (renderMinZ <= 0.0)
		{
			--ipos.z;
		}

		aoLightValueScratchXZNN = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getNegX());
		aoLightValueScratchYZNN = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getNegY());
		aoLightValueScratchYZPN = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getPosY());
		aoLightValueScratchXZPN = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getPosX());
		aoBrightnessXZNN = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getNegX());
		aoBrightnessYZNN = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getNegY());
		aoBrightnessYZPN = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getPosY());
		aoBrightnessXZPN = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getPosX());
		var16 = canBlockGrass[blockAccess->getBlockId(ipos.getNE())] != 0;
		var17 = canBlockGrass[blockAccess->getBlockId(ipos.getNW())] != 0;
		var18 = canBlockGrass[blockAccess->getBlockId(ipos.getUN())] != 0;
		var19 = canBlockGrass[blockAccess->getBlockId(ipos.getDN())] != 0;

		if (!var17 && !var19)
		{
			aoLightValueScratchXYZNNN = aoLightValueScratchXZNN;
			aoBrightnessXYZNNN = aoBrightnessXZNN;
		}
		else
		{
			aoLightValueScratchXYZNNN = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getDW());
			aoBrightnessXYZNNN = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getDW());
		}

		if (!var17 && !var18)
		{
			aoLightValueScratchXYZNPN = aoLightValueScratchXZNN;
			aoBrightnessXYZNPN = aoBrightnessXZNN;
		}
		else
		{
			aoLightValueScratchXYZNPN = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getUW());
			aoBrightnessXYZNPN = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getUW());
		}

		if (!var16 && !var19)
		{
			aoLightValueScratchXYZPNN = aoLightValueScratchXZPN;
			aoBrightnessXYZPNN = aoBrightnessXZPN;
		}
		else
		{
			aoLightValueScratchXYZPNN = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getDE());
			aoBrightnessXYZPNN = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getDE());
		}

		if (!var16 && !var18)
		{
			aoLightValueScratchXYZPPN = aoLightValueScratchXZPN;
			aoBrightnessXYZPPN = aoBrightnessXZPN;
		}
		else
		{
			aoLightValueScratchXYZPPN = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getUE());
			aoBrightnessXYZPPN = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getUE());
		}

		if (renderMinZ <= 0.0)
		{
			++ipos.z;
		}

		var20 = var14;

		if (renderMinZ <= 0.0 || !blockAccess->isBlockOpaqueCube(ipos.getNegZ()))
		{
			var20 = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getNegZ());
		}

		var21 = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getNegZ());
		var22 = (aoLightValueScratchXZNN + aoLightValueScratchXYZNPN + var21 + aoLightValueScratchYZPN) / 4.0F;
		var23 = (var21 + aoLightValueScratchYZPN + aoLightValueScratchXZPN + aoLightValueScratchXYZPPN) / 4.0F;
		var24 = (aoLightValueScratchYZNN + var21 + aoLightValueScratchXYZPNN + aoLightValueScratchXZPN) / 4.0F;
		var25 = (aoLightValueScratchXYZNNN + aoLightValueScratchXZNN + aoLightValueScratchYZNN + var21) / 4.0F;
		aoTopLeft = (float)((float)var22 * renderMaxY * (1.0 - renderMinX) + (float)var23 * renderMinY * renderMinX + (float)var24 * (1.0 - renderMaxY) * renderMinX + (float)var25 * (1.0 - renderMaxY) * (1.0 - renderMinX));
		aoBottomLeft = (float)((float)var22 * renderMaxY * (1.0 - renderMaxX) + (float)var23 * renderMaxY * renderMaxX + (float)var24 * (1.0 - renderMaxY) * renderMaxX + (float)var25 * (1.0 - renderMaxY) * (1.0 - renderMaxX));
		aoBottomRight = (float)((float)var22 * renderMinY * (1.0 - renderMaxX) + (float)var23 * renderMinY * renderMaxX + (float)var24 * (1.0 - renderMinY) * renderMaxX + (float)var25 * (1.0 - renderMinY) * (1.0 - renderMaxX));
		aoTopRgith = (float)((float)var22 * renderMinY * (1.0 - renderMinX) + (float)var23 * renderMinY * renderMinX + (float)var24 * (1.0 - renderMinY) * renderMinX + (float)var25 * (1.0 - renderMinY) * (1.0 - renderMinX));
		var26 = getAoBrightness(aoBrightnessXZNN, aoBrightnessXYZNPN, aoBrightnessYZPN, var20);
		var27 = getAoBrightness(aoBrightnessYZPN, aoBrightnessXZPN, aoBrightnessXYZPPN, var20);
		var28 = getAoBrightness(aoBrightnessYZNN, aoBrightnessXYZPNN, aoBrightnessXZPN, var20);
		var29 = getAoBrightness(aoBrightnessXYZNNN, aoBrightnessXZNN, aoBrightnessYZNN, var20);
		brightnessTopLeft = mixAoBrightness(var26, var27, var28, var29, renderMaxY * (1.0f - renderMinX), renderMaxY * renderMinX, (1.0f - renderMaxY) * renderMinX, (1.0f - renderMaxY) * (1.0f - renderMinX));
		brightnessBottomLeft = mixAoBrightness(var26, var27, var28, var29, renderMaxY * (1.0f - renderMaxX), renderMaxY * renderMaxX, (1.0f - renderMaxY) * renderMaxX, (1.0f - renderMaxY) * (1.0f - renderMaxX));
		brightnessBottomRight = mixAoBrightness(var26, var27, var28, var29, renderMinY * (1.0f - renderMaxX), renderMinY * renderMaxX, (1.0f - renderMinY) * renderMaxX, (1.0f - renderMinY) * (1.0f - renderMaxX));
		brightnessTopRight = mixAoBrightness(var26, var27, var28, var29, renderMinY * (1.0f - renderMinX), renderMinY * renderMinX, (1.0f - renderMinY) * renderMinX, (1.0f - renderMinY) * (1.0f - renderMinX));

		if (hasVertexColor)
			colorTopLeft = colorBottomLeft = colorBottomRight = colorTopRight = _color * 0.8f;
		else
			colorTopLeft = colorBottomLeft = colorBottomRight = colorTopRight = Color(0.8f, 0.8f, 0.8f, 1.0f);
		pSprite = getBlockIcon(pcBlock, blockAccess, ipos, 2);
		renderFaceZNeg(pcBlock, Vector3(ipos), pSprite);

		if (fancyGrass && pSprite->getSpriteName() == "grass_side" && !hasOverrideBlockTexture())
		{
			colorTopLeft *= _color;
			colorBottomLeft *= _color;
			colorBottomRight *= _color;
			colorTopRight *= _color;
			renderFaceZNeg(pcBlock, Vector3(ipos), cBlockGrass::getIconSideOverlay());
		}

		var8 = true;
	}

	if (renderAllFaces || pBlock->shouldSideBeRendered(blockAccess, ipos.getPosZ(), 3))
	{
		if (renderMaxZ >= 1.0)
		{
			++ipos.z;
		}

		aoLightValueScratchXZNP = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getNegX());
		aoLightValueScratchXZPP = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getPosX());
		aoLightValueScratchYZNP = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getNegY());
		aoLightValueScratchYZPP = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getPosY());
		aoBrightnessXZNP = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getNegX());
		aoBrightnessXZPP = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getPosX());
		aoBrightnessYZNP = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getNegY());
		aoBrightnessYZPP = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getPosY());
		var16 = canBlockGrass[blockAccess->getBlockId(ipos.getSE())] != 0;
		var17 = canBlockGrass[blockAccess->getBlockId(ipos.getSW())] != 0;
		var18 = canBlockGrass[blockAccess->getBlockId(ipos.getUS())] != 0;
		var19 = canBlockGrass[blockAccess->getBlockId(ipos.getDS())] != 0;

		if (!var17 && !var19)
		{
			aoLightValueScratchXYZNNP = aoLightValueScratchXZNP;
			aoBrightnessXYZNNP = aoBrightnessXZNP;
		}
		else
		{
			aoLightValueScratchXYZNNP = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getDW());
			aoBrightnessXYZNNP = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getDW());
		}

		if (!var17 && !var18)
		{
			aoLightValueScratchXYZNPP = aoLightValueScratchXZNP;
			aoBrightnessXYZNPP = aoBrightnessXZNP;
		}
		else
		{
			aoLightValueScratchXYZNPP = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getUW());
			aoBrightnessXYZNPP = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getUW());
		}

		if (!var16 && !var19)
		{
			aoLightValueScratchXYZPNP = aoLightValueScratchXZPP;
			aoBrightnessXYZPNP = aoBrightnessXZPP;
		}
		else
		{
			aoLightValueScratchXYZPNP = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getDE());
			aoBrightnessXYZPNP = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getDE());
		}

		if (!var16 && !var18)
		{
			aoLightValueScratchXYZPPP = aoLightValueScratchXZPP;
			aoBrightnessXYZPPP = aoBrightnessXZPP;
		}
		else
		{
			aoLightValueScratchXYZPPP = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getUE());
			aoBrightnessXYZPPP = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getUE());
		}

		if (renderMaxZ >= 1.0)
		{
			--ipos.z;
		}

		var20 = var14;

		if (renderMaxZ >= 1.0 || !blockAccess->isBlockOpaqueCube(ipos.getPosZ()))
		{
			var20 = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getPosZ());
		}

		var21 = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getPosZ());
		var22 = (aoLightValueScratchXZNP + aoLightValueScratchXYZNPP + var21 + aoLightValueScratchYZPP) / 4.0F;
		var23 = (var21 + aoLightValueScratchYZPP + aoLightValueScratchXZPP + aoLightValueScratchXYZPPP) / 4.0F;
		var24 = (aoLightValueScratchYZNP + var21 + aoLightValueScratchXYZPNP + aoLightValueScratchXZPP) / 4.0F;
		var25 = (aoLightValueScratchXYZNNP + aoLightValueScratchXZNP + aoLightValueScratchYZNP + var21) / 4.0F;
		aoTopLeft = (float)((float)var22 * renderMaxY * (1.0 - renderMinX) + (float)var23 * renderMaxY * renderMinX + (float)var24 * (1.0 - renderMaxY) * renderMinX + (float)var25 * (1.0 - renderMaxY) * (1.0 - renderMinX));
		aoBottomLeft = (float)((float)var22 * renderMinY * (1.0 - renderMinX) + (float)var23 * renderMinY * renderMinX + (float)var24 * (1.0 - renderMinY) * renderMinX + (float)var25 * (1.0 - renderMinY) * (1.0 - renderMinX));
		aoBottomRight = (float)((float)var22 * renderMinY * (1.0 - renderMaxX) + (float)var23 * renderMinY * renderMaxX + (float)var24 * (1.0 - renderMinY) * renderMaxX + (float)var25 * (1.0 - renderMinY) * (1.0 - renderMaxX));
		aoTopRgith = (float)((float)var22 * renderMaxY * (1.0 - renderMaxX) + (float)var23 * renderMaxY * renderMaxX + (float)var24 * (1.0 - renderMaxY) * renderMaxX + (float)var25 * (1.0 - renderMaxY) * (1.0 - renderMaxX));
		var26 = getAoBrightness(aoBrightnessXZNP, aoBrightnessXYZNPP, aoBrightnessYZPP, var20);
		var27 = getAoBrightness(aoBrightnessYZPP, aoBrightnessXZPP, aoBrightnessXYZPPP, var20);
		var28 = getAoBrightness(aoBrightnessYZNP, aoBrightnessXYZPNP, aoBrightnessXZPP, var20);
		var29 = getAoBrightness(aoBrightnessXYZNNP, aoBrightnessXZNP, aoBrightnessYZNP, var20);
		brightnessTopLeft = mixAoBrightness(var26, var29, var28, var27, renderMaxY * (1.0f - renderMinX), (1.0f - renderMaxY) * (1.0f - renderMinX), (1.0f - renderMaxY) * renderMinX, renderMaxY * renderMinX);
		brightnessBottomLeft = mixAoBrightness(var26, var29, var28, var27, renderMinY * (1.0f - renderMinX), (1.0f - renderMinY) * (1.0f - renderMinX), (1.0f - renderMinY) * renderMinX, renderMinY * renderMinX);
		brightnessBottomRight = mixAoBrightness(var26, var29, var28, var27, renderMinY * (1.0f - renderMaxX), (1.0f - renderMinY) * (1.0f - renderMaxX), (1.0f - renderMinY) * renderMaxX, renderMinY * renderMaxX);
		brightnessTopRight = mixAoBrightness(var26, var29, var28, var27, renderMaxY * (1.0f - renderMaxX), (1.0f - renderMaxY) * (1.0f - renderMaxX), (1.0f - renderMaxY) * renderMaxX, renderMaxY * renderMaxX);

		if (hasVertexColor)
			colorTopLeft = colorBottomLeft = colorBottomRight = colorTopRight = _color * 0.8f;
		else
			colorTopLeft = colorBottomLeft = colorBottomRight = colorTopRight = Color(0.8f, 0.8f, 0.8f, 1.0f);
		pSprite = getBlockIcon(pcBlock, blockAccess, ipos, 3);
		renderFaceZPos(pcBlock, Vector3(ipos), getBlockIcon(pcBlock, blockAccess, ipos, 3));

		if (fancyGrass && pSprite->getSpriteName() == "grass_side" && !hasOverrideBlockTexture())
		{
			colorTopLeft *= _color;
			colorBottomLeft *= _color;
			colorBottomRight *= _color;
			colorTopRight *= _color;
			renderFaceZPos(pcBlock, Vector3(ipos), cBlockGrass::getIconSideOverlay());
		}

		var8 = true;
	}

	if (renderAllFaces || pBlock->shouldSideBeRendered(blockAccess, ipos.getNegX(), 4))
	{
		if (renderMinX <= 0.0)
		{
			--ipos.x;
		}

		aoLightValueScratchXYNN = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getNegY());
		aoLightValueScratchXZNN = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getNegZ());
		aoLightValueScratchXZNP = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getPosZ());
		aoLightValueScratchXYNP = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getPosY());
		aoBrightnessXYNN = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getNegY());
		aoBrightnessXZNN = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getNegZ());
		aoBrightnessXZNP = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getPosZ());
		aoBrightnessXYNP = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getPosY());
		var16 = canBlockGrass[blockAccess->getBlockId(ipos.getUW())] != 0;
		var17 = canBlockGrass[blockAccess->getBlockId(ipos.getDW())] != 0;
		var18 = canBlockGrass[blockAccess->getBlockId(ipos.getNW())] != 0;
		var19 = canBlockGrass[blockAccess->getBlockId(ipos.getSW())] != 0;

		if (!var18 && !var17)
		{
			aoLightValueScratchXYZNNN = aoLightValueScratchXZNN;
			aoBrightnessXYZNNN = aoBrightnessXZNN;
		}
		else
		{
			aoLightValueScratchXYZNNN = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getDN());
			aoBrightnessXYZNNN = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getDN());
		}

		if (!var19 && !var17)
		{
			aoLightValueScratchXYZNNP = aoLightValueScratchXZNP;
			aoBrightnessXYZNNP = aoBrightnessXZNP;
		}
		else
		{
			aoLightValueScratchXYZNNP = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getDS());
			aoBrightnessXYZNNP = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getDS());
		}

		if (!var18 && !var16)
		{
			aoLightValueScratchXYZNPN = aoLightValueScratchXZNN;
			aoBrightnessXYZNPN = aoBrightnessXZNN;
		}
		else
		{
			aoLightValueScratchXYZNPN = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getUN());
			aoBrightnessXYZNPN = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getUN());
		}

		if (!var19 && !var16)
		{
			aoLightValueScratchXYZNPP = aoLightValueScratchXZNP;
			aoBrightnessXYZNPP = aoBrightnessXZNP;
		}
		else
		{
			aoLightValueScratchXYZNPP = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getUS());
			aoBrightnessXYZNPP = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getUS());
		}

		if (renderMinX <= 0.0)
		{
			++ipos.x;
		}

		var20 = var14;

		if (renderMinX <= 0.0 || !blockAccess->isBlockOpaqueCube(ipos.getNegX()))
		{
			var20 = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getNegX());
		}

		var21 = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getNegX());
		var22 = (aoLightValueScratchXYNN + aoLightValueScratchXYZNNP + var21 + aoLightValueScratchXZNP) / 4.0F;
		var23 = (var21 + aoLightValueScratchXZNP + aoLightValueScratchXYNP + aoLightValueScratchXYZNPP) / 4.0F;
		var24 = (aoLightValueScratchXZNN + var21 + aoLightValueScratchXYZNPN + aoLightValueScratchXYNP) / 4.0F;
		var25 = (aoLightValueScratchXYZNNN + aoLightValueScratchXYNN + aoLightValueScratchXZNN + var21) / 4.0F;
		aoTopLeft = (float)((float)var23 * renderMaxY * renderMaxZ + (float)var24 * renderMaxY * (1.0 - renderMaxZ) + (float)var25 * (1.0 - renderMaxY) * (1.0 - renderMaxZ) + (float)var22 * (1.0 - renderMaxY) * renderMaxZ);
		aoBottomLeft = (float)((float)var23 * renderMaxY * renderMinZ + (float)var24 * renderMaxY * (1.0 - renderMinZ) + (float)var25 * (1.0 - renderMaxY) * (1.0 - renderMinZ) + (float)var22 * (1.0 - renderMaxY) * renderMinZ);
		aoBottomRight = (float)((float)var23 * renderMinY * renderMinZ + (float)var24 * renderMinY * (1.0 - renderMinZ) + (float)var25 * (1.0 - renderMinY) * (1.0 - renderMinZ) + (float)var22 * (1.0 - renderMinY) * renderMinZ);
		aoTopRgith = (float)((float)var23 * renderMinY * renderMaxZ + (float)var24 * renderMinY * (1.0 - renderMaxZ) + (float)var25 * (1.0 - renderMinY) * (1.0 - renderMaxZ) + (float)var22 * (1.0 - renderMinY) * renderMaxZ);
		var26 = getAoBrightness(aoBrightnessXYNN, aoBrightnessXYZNNP, aoBrightnessXZNP, var20);
		var27 = getAoBrightness(aoBrightnessXZNP, aoBrightnessXYNP, aoBrightnessXYZNPP, var20);
		var28 = getAoBrightness(aoBrightnessXZNN, aoBrightnessXYZNPN, aoBrightnessXYNP, var20);
		var29 = getAoBrightness(aoBrightnessXYZNNN, aoBrightnessXYNN, aoBrightnessXZNN, var20);
		brightnessTopLeft = mixAoBrightness(var27, var28, var29, var26, renderMaxY * renderMaxZ, renderMaxY * (1.0f - renderMaxZ), (1.0f - renderMaxY) * (1.0f - renderMaxZ), (1.0f - renderMaxY) * renderMaxZ);
		brightnessBottomLeft = mixAoBrightness(var27, var28, var29, var26, renderMaxY * renderMinZ, renderMaxY * (1.0f - renderMinZ), (1.0f - renderMaxY) * (1.0f - renderMinZ), (1.0f - renderMaxY) * renderMinZ);
		brightnessBottomRight = mixAoBrightness(var27, var28, var29, var26, renderMinY * renderMinZ, renderMinY * (1.0f - renderMinZ), (1.0f - renderMinY) * (1.0f - renderMinZ), (1.0f - renderMinY) * renderMinZ);
		brightnessTopRight = mixAoBrightness(var27, var28, var29, var26, renderMinY * renderMaxZ, renderMinY * (1.0f - renderMaxZ), (1.0f - renderMinY) * (1.0f - renderMaxZ), (1.0f - renderMinY) * renderMaxZ);

		if (hasVertexColor)
			colorTopLeft = colorBottomLeft = colorBottomRight = colorTopRight = _color * 0.6f;
		else
			colorTopLeft = colorBottomLeft = colorBottomRight = colorTopRight = Color(0.6f, 0.6f, 0.6f, 1.0f);


		pSprite = getBlockIcon(pcBlock, blockAccess, ipos, 4);
		renderFaceXNeg(pcBlock, Vector3(ipos), pSprite);

		if (fancyGrass && pSprite->getSpriteName() == "grass_side" && !hasOverrideBlockTexture())
		{
			colorTopLeft *= _color;
			colorBottomLeft *= _color;
			colorBottomRight *= _color;
			colorTopRight *= _color;
			renderFaceXNeg(pcBlock, Vector3(ipos), cBlockGrass::getIconSideOverlay());
		}

		var8 = true;
	}

	if (renderAllFaces || pBlock->shouldSideBeRendered(blockAccess, ipos.getPosX(), 5))
	{
		if (renderMaxX >= 1.0)
		{
			++ipos.x;
		}

		aoLightValueScratchXYPN = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getNegY());
		aoLightValueScratchXZPN = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getNegZ());
		aoLightValueScratchXZPP = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getPosZ());
		aoLightValueScratchXYPP = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getPosY());
		aoBrightnessXYPN = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getNegY());
		aoBrightnessXZPN = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getNegZ());
		aoBrightnessXZPP = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getPosZ());
		aoBrightnessXYPP = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getPosY());
		var16 = canBlockGrass[blockAccess->getBlockId(ipos.getUE())] != 0;
		var17 = canBlockGrass[blockAccess->getBlockId(ipos.getDE())] != 0;
		var18 = canBlockGrass[blockAccess->getBlockId(ipos.getSE())] != 0;
		var19 = canBlockGrass[blockAccess->getBlockId(ipos.getNE())] != 0;

		if (!var17 && !var19)
		{
			aoLightValueScratchXYZPNN = aoLightValueScratchXZPN;
			aoBrightnessXYZPNN = aoBrightnessXZPN;
		}
		else
		{
			aoLightValueScratchXYZPNN = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getDN());
			aoBrightnessXYZPNN = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getDN());
		}

		if (!var17 && !var18)
		{
			aoLightValueScratchXYZPNP = aoLightValueScratchXZPP;
			aoBrightnessXYZPNP = aoBrightnessXZPP;
		}
		else
		{
			aoLightValueScratchXYZPNP = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getDS());
			aoBrightnessXYZPNP = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getDS());
		}

		if (!var16 && !var19)
		{
			aoLightValueScratchXYZPPN = aoLightValueScratchXZPN;
			aoBrightnessXYZPPN = aoBrightnessXZPN;
		}
		else
		{
			aoLightValueScratchXYZPPN = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getUN());
			aoBrightnessXYZPPN = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getUN());
		}

		if (!var16 && !var18)
		{
			aoLightValueScratchXYZPPP = aoLightValueScratchXZPP;
			aoBrightnessXYZPPP = aoBrightnessXZPP;
		}
		else
		{
			aoLightValueScratchXYZPPP = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getUS());
			aoBrightnessXYZPPP = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getUS());
		}

		if (renderMaxX >= 1.0)
		{
			--ipos.x;
		}

		var20 = var14;

		if (renderMaxX >= 1.0 || !blockAccess->isBlockOpaqueCube(ipos.getPosX()))
		{
			var20 = pcBlock->getMixedBrightnessForBlock(blockAccess, ipos.getPosX());
		}

		var21 = pcBlock->getAmbientOcclusionLightValue(blockAccess, ipos.getPosX());
		var22 = (aoLightValueScratchXYPN + aoLightValueScratchXYZPNP + var21 + aoLightValueScratchXZPP) / 4.0F;
		var23 = (aoLightValueScratchXYZPNN + aoLightValueScratchXYPN + aoLightValueScratchXZPN + var21) / 4.0F;
		var24 = (aoLightValueScratchXZPN + var21 + aoLightValueScratchXYZPPN + aoLightValueScratchXYPP) / 4.0F;
		var25 = (var21 + aoLightValueScratchXZPP + aoLightValueScratchXYPP + aoLightValueScratchXYZPPP) / 4.0F;
		aoTopLeft = (float)((float)var22 * (1.0 - renderMinY) * renderMaxZ + (float)var23 * (1.0 - renderMinY) * (1.0 - renderMaxZ) + (float)var24 * renderMinY * (1.0 - renderMaxZ) + (float)var25 * renderMinY * renderMaxZ);
		aoBottomLeft = (float)((float)var22 * (1.0 - renderMinY) * renderMinZ + (float)var23 * (1.0 - renderMinY) * (1.0 - renderMinZ) + (float)var24 * renderMinY * (1.0 - renderMinZ) + (float)var25 * renderMinY * renderMinZ);
		aoBottomRight = (float)((float)var22 * (1.0 - renderMaxY) * renderMinZ + (float)var23 * (1.0 - renderMaxY) * (1.0 - renderMinZ) + (float)var24 * renderMaxY * (1.0 - renderMinZ) + (float)var25 * renderMaxY * renderMinZ);
		aoTopRgith = (float)((float)var22 * (1.0 - renderMaxY) * renderMaxZ + (float)var23 * (1.0 - renderMaxY) * (1.0 - renderMaxZ) + (float)var24 * renderMaxY * (1.0 - renderMaxZ) + (float)var25 * renderMaxY * renderMaxZ);
		var26 = getAoBrightness(aoBrightnessXYPN, aoBrightnessXYZPNP, aoBrightnessXZPP, var20);
		var27 = getAoBrightness(aoBrightnessXZPP, aoBrightnessXYPP, aoBrightnessXYZPPP, var20);
		var28 = getAoBrightness(aoBrightnessXZPN, aoBrightnessXYZPPN, aoBrightnessXYPP, var20);
		var29 = getAoBrightness(aoBrightnessXYZPNN, aoBrightnessXYPN, aoBrightnessXZPN, var20);
		brightnessTopLeft = mixAoBrightness(var26, var29, var28, var27, (1.0f - renderMinY) * renderMaxZ, (1.0f - renderMinY) * (1.0f - renderMaxZ), renderMinY * (1.0f - renderMaxZ), renderMinY * renderMaxZ);
		brightnessBottomLeft = mixAoBrightness(var26, var29, var28, var27, (1.0f - renderMinY) * renderMinZ, (1.0f - renderMinY) * (1.0f - renderMinZ), renderMinY * (1.0f - renderMinZ), renderMinY * renderMinZ);
		brightnessBottomRight = mixAoBrightness(var26, var29, var28, var27, (1.0f - renderMaxY) * renderMinZ, (1.0f - renderMaxY) * (1.0f - renderMinZ), renderMaxY * (1.0f - renderMinZ), renderMaxY * renderMinZ);
		brightnessTopRight = mixAoBrightness(var26, var29, var28, var27, (1.0f - renderMaxY) * renderMaxZ, (1.0f - renderMaxY) * (1.0f - renderMaxZ), renderMaxY * (1.0f - renderMaxZ), renderMaxY * renderMaxZ);

		if (hasVertexColor)
			colorTopLeft = colorBottomLeft = colorBottomRight = colorTopRight = _color * 0.6f;
		else
			colorTopLeft = colorBottomLeft = colorBottomRight = colorTopRight = Color(0.6f, 0.6f, 0.6f, 1.0f);
		pSprite = getBlockIcon(pcBlock, blockAccess, ipos, 5);
		renderFaceXPos(pcBlock, Vector3(ipos), pSprite);

		if (fancyGrass && pSprite->getSpriteName() == "grass_side" && !hasOverrideBlockTexture())
		{
			colorTopLeft *= _color;
			colorBottomLeft *= _color;
			colorBottomRight *= _color;
			colorTopRight *= _color;
			renderFaceXPos(pcBlock, Vector3(ipos), cBlockGrass::getIconSideOverlay());
		}

		var8 = true;
	}

	enableAO = false;
	return var8;
}

int RenderBlocks::getAoBrightness(int par1, int par2, int par3, int par4)
{
	if (par1 == 0)
	{
		par1 = par4;
	}

	if (par2 == 0)
	{
		par2 = par4;
	}

	if (par3 == 0)
	{
		par3 = par4;
	}

	return (par1 + par2 + par3 + par4) >> 2 & 0xFF00FF;
}

int RenderBlocks::mixAoBrightness(int par1, int par2, int par3, int par4, float par5, float par7, float par9, float par11)
{
	int var13 = (int)((float)(par1 >> 16 & 255) * par5 + (float)(par2 >> 16 & 255) * par7 + (float)(par3 >> 16 & 255) * par9 + (float)(par4 >> 16 & 255) * par11) & 255;
	int var14 = (int)((float)(par1 & 255) * par5 + (float)(par2 & 255) * par7 + (float)(par3 & 255) * par9 + (float)(par4 & 255) * par11) & 255;
	return var13 << 16 | var14;
}

bool RenderBlocks::renderStandardBlockWithColorMultiplier(Block* pBlock, cBlock* pcBlock, const BlockPos& pos, const Color& _color)
{
	enableAO = false;

	bool bResult = false;
	Color _c = Color(1.0f, 1.0f, 1.0f, 1.0f);
	Color _c1 = Color(0.5f, 0.5f, 0.5f, 1.0f);
	Color _c2 = Color(0.8f, 0.8f, 0.8f, 1.0f);
	Color _c3 = Color(0.6f, 0.6f, 0.6f, 1.0f);

	if (pBlock != BlockManager::grass)
	{
		_c1 = _c1 * _color;
		_c2 = _c2 * _color;
		_c3 = _c3 * _color;
	}

	int bright = pcBlock->getMixedBrightnessForBlock(blockAccess, pos);

	if (renderAllFaces || pBlock->shouldSideBeRendered(blockAccess, pos.getNegY(), 0))
	{
		setBrightness(renderMinY > 0.0f ? bright : pcBlock->getMixedBrightnessForBlock(blockAccess, pos.getNegY()));
		setColor(_c1);
		renderFaceYNeg(pcBlock, Vector3(pos), getBlockIcon(pcBlock, blockAccess, pos, 0));
		bResult = true;
	}

	if (renderAllFaces || pBlock->shouldSideBeRendered(blockAccess, pos.getPosY(), 1))
	{
		setBrightness(renderMaxY < 1.0f ? bright : pcBlock->getMixedBrightnessForBlock(blockAccess, pos.getPosY()));
		setColor(_c);
		renderFaceYPos(pcBlock, Vector3(pos), getBlockIcon(pcBlock, blockAccess, pos, 1));
		bResult = true;
	}

	AtlasSprite* pSprite = NULL;

	if (renderAllFaces || pBlock->shouldSideBeRendered(blockAccess, pos.getNegZ(), 2))
	{
		setBrightness(renderMinZ > 0.0f ? bright : pcBlock->getMixedBrightnessForBlock(blockAccess, pos.getNegZ()));
		setColor(_c2);
		pSprite = getBlockIcon(pcBlock, blockAccess, pos, 2);
		renderFaceZNeg(pcBlock, Vector3(pos), pSprite);

		if (fancyGrass && pSprite->getSpriteName() == "grass_side" && !hasOverrideBlockTexture())
		{
			setColor(_c2 * _color);
			renderFaceZNeg(pcBlock, Vector3(pos), cBlockGrass::getIconSideOverlay());
		}

		bResult = true;
	}

	if (renderAllFaces || pBlock->shouldSideBeRendered(blockAccess, pos.getPosZ(), 3))
	{
		setBrightness(renderMaxZ < 1.0f ? bright : pcBlock->getMixedBrightnessForBlock(blockAccess, pos.getPosZ()));
		setColor(_c2);
		pSprite = getBlockIcon(pcBlock, blockAccess, pos, 3);
		renderFaceZPos(pcBlock, Vector3(pos), pSprite);

		if (fancyGrass && pSprite->getSpriteName() == "grass_side" && !hasOverrideBlockTexture())
		{
			setColor(_c2 * _color);
			renderFaceZPos(pcBlock, Vector3(pos), cBlockGrass::getIconSideOverlay());
		}

		bResult = true;
	}

	if (renderAllFaces || pBlock->shouldSideBeRendered(blockAccess, pos.getNegX(), 4))
	{
		setBrightness(renderMinX > 0.0f ? bright : pcBlock->getMixedBrightnessForBlock(blockAccess, pos.getNegX()));
		setColor(_c3);
		pSprite = getBlockIcon(pcBlock, blockAccess, pos, 4);
		renderFaceXNeg(pcBlock, Vector3(pos), pSprite);

		if (fancyGrass && pSprite->getSpriteName() == "grass_side" && !hasOverrideBlockTexture())
		{
			setColor(_c3 * _color);
			renderFaceXNeg(pcBlock, Vector3(pos), cBlockGrass::getIconSideOverlay());
		}

		bResult = true;
	}

	if (renderAllFaces || pBlock->shouldSideBeRendered(blockAccess, pos.getPosX(), 5))
	{
		setBrightness(renderMaxX < 1.0f ? bright : pcBlock->getMixedBrightnessForBlock(blockAccess, pos.getPosX()));
		setColor(_c3);
		pSprite = getBlockIcon(pcBlock, blockAccess, pos, 5);
		renderFaceXPos(pcBlock, Vector3(pos), pSprite);

		if (fancyGrass && pSprite->getSpriteName() == "grass_side" && !hasOverrideBlockTexture())
		{
			setColor(_c3 * _color);
			renderFaceXPos(pcBlock, Vector3(pos), cBlockGrass::getIconSideOverlay());
		}

		bResult = true;
	}

	return bResult;
}

bool RenderBlocks::renderBlockHopperMetadata(Block* pBlock, cBlock* pcBlock, const BlockPos& pos, int metadata, bool par6)
{
	//Tessellator var7 = Tessellator.instance;
	int d = BlockHopper::getDirectionFromMetadata(metadata);
	float var9 = 0.625F;
	setRenderBounds(0.0F, var9, 0.0F, 1.0F, 1.0F, 1.0F);

	if (par6)
	{
		//var7.startDrawingQuads();
		//var7.setNormal(0.0F, -1.0F, 0.0F);
		renderFaceYNeg(pcBlock, Vector3::ZERO, getBlockIconFromSideAndMetadata(pcBlock, 0, metadata));
		/*var7.draw();
		var7.startDrawingQuads();
		var7.setNormal(0.0F, 1.0F, 0.0F);*/
		renderFaceYPos(pcBlock, Vector3::ZERO, getBlockIconFromSideAndMetadata(pcBlock, 1, metadata));
		/*var7.draw();
		var7.startDrawingQuads();
		var7.setNormal(0.0F, 0.0F, -1.0F);*/
		renderFaceZNeg(pcBlock, Vector3::ZERO, getBlockIconFromSideAndMetadata(pcBlock, 2, metadata));
		/*var7.draw();
		var7.startDrawingQuads();
		var7.setNormal(0.0F, 0.0F, 1.0F);*/
		renderFaceZPos(pcBlock, Vector3::ZERO, getBlockIconFromSideAndMetadata(pcBlock, 3, metadata));
		/*var7.draw();
		var7.startDrawingQuads();
		var7.setNormal(-1.0F, 0.0F, 0.0F);*/
		renderFaceXNeg(pcBlock, Vector3::ZERO, getBlockIconFromSideAndMetadata(pcBlock, 4, metadata));
		/*var7.draw();
		var7.startDrawingQuads();
		var7.setNormal(1.0F, 0.0F, 0.0F);*/
		renderFaceXPos(pcBlock, Vector3::ZERO, getBlockIconFromSideAndMetadata(pcBlock, 5, metadata));
		//var7.draw();
	}
	else
	{
		renderStandardBlock(pBlock, pcBlock, pos);
	}

	float var13;

	if (!par6)
	{
		//var7.setBrightness(pBlock.getMixedBrightnessForBlock(blockAccess, pos));
		float var11 = 1.0F;
		ui32 var12 = pcBlock->colorMultiplier(blockAccess, pos);
		var13 = (float)(var12 >> 16 & 255) / 255.0F;
		float var14 = (float)(var12 >> 8 & 255) / 255.0F;
		float var15 = (float)(var12 & 255) / 255.0F;

		if (RenderEntity::anaglyphEnable)
		{
			float var16 = (var13 * 30.0F + var14 * 59.0F + var15 * 11.0F) / 100.0F;
			float var17 = (var13 * 30.0F + var14 * 70.0F) / 100.0F;
			float var18 = (var13 * 30.0F + var15 * 70.0F) / 100.0F;
			var13 = var16;
			var14 = var17;
			var15 = var18;
		}

		//var7.setColorOpaque_F(var11 * var13, var11 * var14, var11 * var15);
	}

	cBlockHopper* pBlockHopper = (cBlockHopper*)(pBlock);
	AtlasSprite* spriteOutside = pBlockHopper->getHopperIcon("hopper_outside");
	AtlasSprite* spriteInside = pBlockHopper->getHopperIcon("hopper_inside");
	var13 = 0.125F;

	if (par6)
	{
		/*var7.startDrawingQuads();
		var7.setNormal(1.0F, 0.0F, 0.0F);*/
		renderFaceXPos(pcBlock, Vector3(-1.0F + var13, 0.F, 0.F), spriteOutside);
		/*var7.draw();
		var7.startDrawingQuads();
		var7.setNormal(-1.0F, 0.0F, 0.0F);*/
		renderFaceXNeg(pcBlock, Vector3(1.0F - var13, 0.F, 0.F), spriteOutside);
		/*var7.draw();
		var7.startDrawingQuads();
		var7.setNormal(0.0F, 0.0F, 1.0F);*/
		renderFaceZPos(pcBlock, Vector3(0.F, 0.F, -1.0F + var13), spriteOutside);
		/*var7.draw();
		var7.startDrawingQuads();
		var7.setNormal(0.0F, 0.0F, -1.0F);*/
		renderFaceZNeg(pcBlock, Vector3(0.F, 0.F, 1.0F - var13), spriteOutside);
		/*var7.draw();
		var7.startDrawingQuads();
		var7.setNormal(0.0F, 1.0F, 0.0F);*/
		renderFaceYPos(pcBlock, Vector3(0.F, -1.F + var9, 0.0F), spriteInside);
		//var7.draw();
	}
	else
	{
		Vector3 vpos(pos);
		renderFaceXPos(pcBlock, vpos - Vector3(1.f - var13, 0.f, 0.f), spriteOutside);
		renderFaceXNeg(pcBlock, vpos + Vector3(1.f - var13, 0.f, 0.f), spriteOutside);
		renderFaceZPos(pcBlock, vpos - Vector3(0.f, 0.f, 1.f - var13), spriteOutside);
		renderFaceZNeg(pcBlock, vpos + Vector3(0.f, 0.f, 1.f - var13), spriteOutside);
		renderFaceYPos(pcBlock, vpos - Vector3(0.f, 1.F - var9, 0.f), spriteInside);
	}

	setOverrideBlockTexture(spriteOutside);
	float var26 = 0.25F;
	float var27 = 0.25F;
	setRenderBounds(var26, var27, var26, 1.0F - var26, var9 - 0.002F, 1.0F - var26);

	if (par6)
	{
		/*var7.startDrawingQuads();
		var7.setNormal(1.0F, 0.0F, 0.0F);*/
		renderFaceXPos(pcBlock, Vector3::ZERO, spriteOutside);
		/*var7.draw();
		var7.startDrawingQuads();
		var7.setNormal(-1.0F, 0.0F, 0.0F);*/
		renderFaceXNeg(pcBlock, Vector3::ZERO, spriteOutside);
		/*var7.draw();
		var7.startDrawingQuads();
		var7.setNormal(0.0F, 0.0F, 1.0F);*/
		renderFaceZPos(pcBlock, Vector3::ZERO, spriteOutside);
		/*var7.draw();
		var7.startDrawingQuads();
		var7.setNormal(0.0F, 0.0F, -1.0F);*/
		renderFaceZNeg(pcBlock, Vector3::ZERO, spriteOutside);
		/*var7.draw();
		var7.startDrawingQuads();
		var7.setNormal(0.0F, 1.0F, 0.0F);*/
		renderFaceYPos(pcBlock, Vector3::ZERO, spriteOutside);
		/*var7.draw();
		var7.startDrawingQuads();
		var7.setNormal(0.0F, -1.0F, 0.0F);*/
		renderFaceYNeg(pcBlock, Vector3::ZERO, spriteOutside);
		//var7.draw();
	}
	else
	{
		renderStandardBlock(pBlock, pcBlock, pos);
	}

	if (!par6)
	{
		float var20 = 0.375F;
		float var22 = 0.25F;
		setOverrideBlockTexture(spriteOutside);

		if (d == 0)
		{
			setRenderBounds(var20, 0.0F, var20, 1.0F - var20, 0.25F, 1.0F - var20);
			renderStandardBlock(pBlock, pcBlock, pos);
		}

		if (d == 2)
		{
			setRenderBounds(var20, var27, 0.0F, 1.0F - var20, var27 + var22, var26);
			renderStandardBlock(pBlock, pcBlock, pos);
		}

		if (d == 3)
		{
			setRenderBounds(var20, var27, 1.0F - var26, 1.0F - var20, var27 + var22, 1.0F);
			renderStandardBlock(pBlock, pcBlock, pos);
		}

		if (d == 4)
		{
			setRenderBounds(0.0F, var27, var20, var26, var27 + var22, 1.0F - var20);
			renderStandardBlock(pBlock, pcBlock, pos);
		}

		if (d == 5)
		{
			setRenderBounds(1.0F - var26, var27, var20, 1.0F, var27 + var22, 1.0F - var20);
			renderStandardBlock(pBlock, pcBlock, pos);
		}
	}

	clearOverrideBlockTexture();
	return true;
}

void RenderBlocks::renderFaceYNeg(cBlock* pcBlock, const Vector3& pos, AtlasSprite* sprite)
{
	if (hasOverrideBlockTexture())
	{
		sprite = overrideBlockTexture;
	}

	float tx_min = sprite->getInterpolatedU(renderMinX);
	float tx_max = sprite->getInterpolatedU(renderMaxX);
	float ty_min = sprite->getInterpolatedV(renderMinZ);
	float ty_max = sprite->getInterpolatedV(renderMaxZ);

	if (renderMinX < 0.0f || renderMaxX > 1.0f)
	{
		tx_min = sprite->getMinU();
		tx_max = sprite->getMaxU();
	}

	if (renderMinZ < 0.0f || renderMaxZ > 1.0f)
	{
		ty_min = sprite->getMinV();
		ty_max = sprite->getMaxV();
	}

	float tu_min = tx_min;
	float tu_max = tx_max;
	float tv_min = ty_min;
	float tv_max = ty_max;

	if (uvRotateBottom == 2)
	{
		tx_min = sprite->getInterpolatedU(renderMinZ);
		ty_min = sprite->getInterpolatedV(1.0f - renderMaxX);
		tx_max = sprite->getInterpolatedU(renderMaxZ);
		ty_max = sprite->getInterpolatedV(1.0f - renderMinX);
		tv_min = ty_min;
		tv_max = ty_max;
		tu_max = tx_min;
		tu_min = tx_max;
		ty_min = ty_max;
		ty_max = tv_min;
	}
	else if (uvRotateBottom == 1)
	{
		tx_min = sprite->getInterpolatedU(1.0f - renderMaxZ);
		ty_min = sprite->getInterpolatedV(renderMinX);
		tx_max = sprite->getInterpolatedU(1.0f - renderMinZ);
		ty_max = sprite->getInterpolatedV(renderMaxX);
		tu_max = tx_max;
		tu_min = tx_min;
		tx_min = tx_max;
		tx_max = tu_min;
		tv_min = ty_max;
		tv_max = ty_min;
	}
	else if (uvRotateBottom == 3)
	{
		tx_min = sprite->getInterpolatedU(1.0f - renderMinX);
		tx_max = sprite->getInterpolatedU(1.0f - renderMaxX);
		ty_min = sprite->getInterpolatedV(1.0f - renderMinZ);
		ty_max = sprite->getInterpolatedV(1.0f - renderMaxZ);
		tu_max = tx_max;
		tu_min = tx_min;
		tv_min = ty_min;
		tv_max = ty_max;
	}

	float posx_min = pos.x + renderMinX + m_transform.x;
	float posx_max = pos.x + renderMaxX + m_transform.x;
	float posy = pos.y + renderMinY + m_transform.y;
	float posz_min = pos.z + renderMinZ + m_transform.z;
	float posz_max = pos.z + renderMaxZ + m_transform.z;

	int pass = pcBlock->getRenderBlockPass();
	TessRenderable* renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite->getTexture(), (TESS_RENDER_PASS)pass, 1);
	TessVertexFmt* pVetLockPtr = NULL;
	if (!renderable->getLockPtr(1, pVetLockPtr))
		return;

	if (enableAO)
	{
		int color_tl = (colorTopLeft * aoTopLeft).getARGB();
		int color_bl = (colorBottomLeft * aoBottomLeft).getARGB();
		int color_br = (colorBottomRight * aoBottomRight).getARGB();
		int color_tr = (colorTopRight * aoTopRgith).getARGB();
		// fill the vertex and index of the VBO,IBO.
		pVetLockPtr[0].setEx(m_sectionPos, posx_min - anti_gap, posy, posz_max + anti_gap, color_tl, brightnessTopLeft, tu_min, tv_max);
		pVetLockPtr[1].setEx(m_sectionPos, posx_min - anti_gap, posy, posz_min - anti_gap, color_bl, brightnessBottomLeft, tx_min, ty_min);
		pVetLockPtr[2].setEx(m_sectionPos, posx_max + anti_gap, posy, posz_min - anti_gap, color_br, brightnessBottomRight, tu_max, tv_min);
		pVetLockPtr[3].setEx(m_sectionPos, posx_max + anti_gap, posy, posz_max + anti_gap, color_tr, brightnessTopRight, tx_max, ty_max);
		//AutoCaculateQuadNormal(pVetLockPtr + 0);
	}
	else
	{
		pVetLockPtr[0].setEx(m_sectionPos, posx_min - anti_gap, posy, posz_max + anti_gap, m_color, m_bright, tu_min, tv_max);
		pVetLockPtr[1].setEx(m_sectionPos, posx_min - anti_gap, posy, posz_min - anti_gap, m_color, m_bright, tx_min, ty_min);
		pVetLockPtr[2].setEx(m_sectionPos, posx_max + anti_gap, posy, posz_min - anti_gap, m_color, m_bright, tu_max, tv_min);
		pVetLockPtr[3].setEx(m_sectionPos, posx_max + anti_gap, posy, posz_max + anti_gap, m_color, m_bright, tx_max, ty_max);
		//AutoCaculateQuadNormal(pVetLockPtr + 0);
	}
}

void RenderBlocks::renderFaceYPos(cBlock* pcBlock, const Vector3& pos, AtlasSprite* sprite)
{
	if (hasOverrideBlockTexture())
	{
		sprite = overrideBlockTexture;
	}

	float tx_min = sprite->getInterpolatedU(renderMinX);
	float tx_max = sprite->getInterpolatedU(renderMaxX);
	float ty_min = sprite->getInterpolatedV(renderMinZ);
	float ty_max = sprite->getInterpolatedV(renderMaxZ);

	if (renderMinX < 0.0f || renderMaxX > 1.0f)
	{
		tx_min = sprite->getMinU();
		tx_max = sprite->getMaxU();
	}

	if (renderMinZ < 0.0f || renderMaxZ > 1.0f)
	{
		ty_min = sprite->getMinV();
		ty_max = sprite->getMaxV();
	}

	float tu_min = tx_min;
	float tu_max = tx_max;
	float tv_min = ty_min;
	float tv_max = ty_max;

	if (uvRotateTop == 1)
	{
		tx_min = sprite->getInterpolatedU(renderMinZ);
		ty_min = sprite->getInterpolatedV(1.0f - renderMaxX);
		tx_max = sprite->getInterpolatedU(renderMaxZ);
		ty_max = sprite->getInterpolatedV(1.0f - renderMinX);
		tv_min = ty_min;
		tv_max = ty_max;
		tu_max = tx_min;
		tu_min = tx_max;
		ty_min = ty_max;
		ty_max = tv_min;
	}
	else if (uvRotateTop == 2)
	{
		tx_min = sprite->getInterpolatedU(1.0f - renderMaxZ);
		ty_min = sprite->getInterpolatedV(renderMinX);
		tx_max = sprite->getInterpolatedU(1.0f - renderMinZ);
		ty_max = sprite->getInterpolatedV(renderMaxX);
		tu_max = tx_max;
		tu_min = tx_min;
		tx_min = tx_max;
		tx_max = tu_min;
		tv_min = ty_max;
		tv_max = ty_min;
	}
	else if (uvRotateTop == 3)
	{
		tx_min = sprite->getInterpolatedU(1.0f - renderMinX);
		tx_max = sprite->getInterpolatedU(1.0f - renderMaxX);
		ty_min = sprite->getInterpolatedV(1.0f - renderMinZ);
		ty_max = sprite->getInterpolatedV(1.0f - renderMaxZ);
		tu_max = tx_max;
		tu_min = tx_min;
		tv_min = ty_min;
		tv_max = ty_max;
	}
		
	float posx_min = pos.x + renderMinX + m_transform.x;
	float posx_max = pos.x + renderMaxX + m_transform.x;
	float posy = pos.y + renderMaxY + m_transform.y;
	float posz_min = pos.z + renderMinZ + m_transform.z;
	float posz_max = pos.z + renderMaxZ + m_transform.z;

	int pass = pcBlock->getRenderBlockPass();
	TessRenderable* renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite->getTexture(), (TESS_RENDER_PASS)pass, 1);
	TessVertexFmt* pVetLockPtr = NULL;
	if (!renderable->getLockPtr(1, pVetLockPtr))
		return;

	if (enableAO)
	{
		int color_tl = (colorTopLeft * aoTopLeft).getARGB();
		int color_bl = (colorBottomLeft * aoBottomLeft).getARGB();
		int color_br = (colorBottomRight * aoBottomRight).getARGB();
		int color_tr = (colorTopRight * aoTopRgith).getARGB();
		pVetLockPtr[0].setEx(m_sectionPos, posx_max + anti_gap, posy, posz_max + anti_gap, color_tl, brightnessTopLeft, tx_max, ty_max);
		pVetLockPtr[1].setEx(m_sectionPos, posx_max + anti_gap, posy, posz_min - anti_gap, color_bl, brightnessBottomLeft, tu_max, tv_min);
		pVetLockPtr[2].setEx(m_sectionPos, posx_min - anti_gap, posy, posz_min - anti_gap, color_br, brightnessBottomRight, tx_min, ty_min);
		pVetLockPtr[3].setEx(m_sectionPos, posx_min - anti_gap, posy, posz_max + anti_gap, color_tr, brightnessTopRight, tu_min, tv_max);
		//AutoCaculateQuadNormal(pVetLockPtr + 0);
	}
	else
	{
		pVetLockPtr[0].setEx(m_sectionPos, posx_max + anti_gap, posy, posz_max + anti_gap, m_color, m_bright, tx_max, ty_max);
		pVetLockPtr[1].setEx(m_sectionPos, posx_max + anti_gap, posy, posz_min - anti_gap, m_color, m_bright, tu_max, tv_min);
		pVetLockPtr[2].setEx(m_sectionPos, posx_min - anti_gap, posy, posz_min - anti_gap, m_color, m_bright, tx_min, ty_min);
		pVetLockPtr[3].setEx(m_sectionPos, posx_min - anti_gap, posy, posz_max + anti_gap, m_color, m_bright, tu_min, tv_max);
		//AutoCaculateQuadNormal(pVetLockPtr + 0);
	}
}

void RenderBlocks::renderFaceZNeg(cBlock* pcBlock, const Vector3& pos, AtlasSprite* sprite)
{
	if (hasOverrideBlockTexture())
	{
		sprite = overrideBlockTexture;
	}

	float tx_min = sprite->getInterpolatedU(renderMinX);
	float tx_max = sprite->getInterpolatedU(renderMaxX);
	float ty_min = sprite->getInterpolatedV(1.0f - renderMaxY);
	float ty_max = sprite->getInterpolatedV(1.0f - renderMinY);
	float tu_min;

	if (flipTexture)
	{
		tu_min = tx_min;
		tx_min = tx_max;
		tx_max = tu_min;
	}

	if (renderMinX < 0.0f || renderMaxX > 1.0f)
	{
		tx_min = sprite->getMinU();
		tx_max = sprite->getMaxU();
	}

	if (renderMinY < 0.0f || renderMaxY > 1.0f)
	{
		ty_min = sprite->getMinV();
		ty_max = sprite->getMaxV();
	}

	tu_min = tx_max;
	float tu_max = tx_min;
	float tv_min = ty_min;
	float tv_max = ty_max;

	if (uvRotateEast == 2)
	{
		tx_min = sprite->getInterpolatedU(renderMinY);
		ty_min = sprite->getInterpolatedV(1.0f - renderMinX);
		tx_max = sprite->getInterpolatedU(renderMaxY);
		ty_max = sprite->getInterpolatedV(1.0f - renderMaxX);
		tv_min = ty_min;
		tv_max = ty_max;
		tu_min = tx_min;
		tu_max = tx_max;
		ty_min = ty_max;
		ty_max = tv_min;
	}
	else if (uvRotateEast == 1)
	{
		tx_min = sprite->getInterpolatedU(1.0f - renderMaxY);
		ty_min = sprite->getInterpolatedV(renderMaxX);
		tx_max = sprite->getInterpolatedU(1.0f - renderMinY);
		ty_max = sprite->getInterpolatedV(renderMinX);
		tu_min = tx_max;
		tu_max = tx_min;
		tx_min = tx_max;
		tx_max = tu_max;
		tv_min = ty_max;
		tv_max = ty_min;
	}
	else if (uvRotateEast == 3)
	{
		tx_min = sprite->getInterpolatedU(1.0f - renderMinX);
		tx_max = sprite->getInterpolatedU(1.0f - renderMaxX);
		ty_min = sprite->getInterpolatedV(renderMaxY);
		ty_max = sprite->getInterpolatedV(renderMinY);
		tu_min = tx_max;
		tu_max = tx_min;
		tv_min = ty_min;
		tv_max = ty_max;
	}

	float posx_min = pos.x + renderMinX + m_transform.x;
	float posx_max = pos.x + renderMaxX + m_transform.x;
	float posy_min = pos.y + renderMinY + m_transform.y;
	float posy_max = pos.y + renderMaxY + m_transform.y;
	float posz = pos.z + renderMinZ + m_transform.z;

	int pass = pcBlock->getRenderBlockPass();
	TessRenderable* renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite->getTexture(), (TESS_RENDER_PASS)pass, 1);
	TessVertexFmt* pVetLockPtr = NULL;
	if (!renderable->getLockPtr(1, pVetLockPtr))
		return;

	if (enableAO)
	{
		int color_tl = (colorTopLeft * aoTopLeft).getARGB();
		int color_bl = (colorBottomLeft * aoBottomLeft).getARGB();
		int color_br = (colorBottomRight * aoBottomRight).getARGB();
		int color_tr = (colorTopRight * aoTopRgith).getARGB();
		pVetLockPtr[0].setEx(m_sectionPos, posx_min - anti_gap, posy_max + anti_gap, posz, color_tl, brightnessTopLeft, tu_min, tv_min);
		pVetLockPtr[1].setEx(m_sectionPos, posx_max + anti_gap, posy_max + anti_gap, posz, color_bl, brightnessBottomLeft, tx_min, ty_min);
		pVetLockPtr[2].setEx(m_sectionPos, posx_max + anti_gap, posy_min - anti_gap, posz, color_br, brightnessBottomRight, tu_max, tv_max);
		pVetLockPtr[3].setEx(m_sectionPos, posx_min - anti_gap, posy_min - anti_gap, posz, color_tr, brightnessTopRight, tx_max, ty_max);
		//AutoCaculateQuadNormal(pVetLockPtr + 0);
	}
	else
	{
		pVetLockPtr[0].setEx(m_sectionPos, posx_min - anti_gap, posy_max + anti_gap, posz, m_color, m_bright, tu_min, tv_min);
		pVetLockPtr[1].setEx(m_sectionPos, posx_max + anti_gap, posy_max + anti_gap, posz, m_color, m_bright, tx_min, ty_min);
		pVetLockPtr[2].setEx(m_sectionPos, posx_max + anti_gap, posy_min - anti_gap, posz, m_color, m_bright, tu_max, tv_max);
		pVetLockPtr[3].setEx(m_sectionPos, posx_min - anti_gap, posy_min - anti_gap, posz, m_color, m_bright, tx_max, ty_max);
		//AutoCaculateQuadNormal(pVetLockPtr + 0);
	}
}

void RenderBlocks::renderFaceZPos(cBlock* pcBlock, const Vector3& pos, AtlasSprite* sprite)
{
	if (hasOverrideBlockTexture())
	{
		sprite = overrideBlockTexture;
	}

	if (hasOverrideBlockTexture())
	{
		sprite = overrideBlockTexture;
	}

	float tx_min = sprite->getInterpolatedU(renderMinX);
	float tx_max = sprite->getInterpolatedU(renderMaxX);
	float ty_min = sprite->getInterpolatedV(1.0f - renderMaxY);
	float ty_max = sprite->getInterpolatedV(1.0f - renderMinY);
	float tu_max;

	if (flipTexture)
	{
		tu_max = tx_min;
		tx_min = tx_max;
		tx_max = tu_max;
	}

	if (renderMinX < 0.0f || renderMaxX > 1.0f)
	{
		tx_min = sprite->getMinU();
		tx_max = sprite->getMaxU();
	}

	if (renderMinY < 0.0f || renderMaxY > 1.0f)
	{
		ty_min = sprite->getMinV();
		ty_max = sprite->getMaxV();
	}

	tu_max = tx_max;
	float tu_min = tx_min;
	float tv_min = ty_min;
	float tv_max = ty_max;

	if (uvRotateWest == 1)
	{
		tx_min = sprite->getInterpolatedU(renderMinY);
		ty_max = sprite->getInterpolatedV(1.0f - renderMinX);
		tx_max = sprite->getInterpolatedU(renderMaxY);
		ty_min = sprite->getInterpolatedV(1.0f - renderMaxX);
		tv_min = ty_min;
		tv_max = ty_max;
		tu_max = tx_min;
		tu_min = tx_max;
		ty_min = ty_max;
		ty_max = tv_min;
	}
	else if (uvRotateWest == 2)
	{
		tx_min = sprite->getInterpolatedU(1.0f - renderMaxY);
		ty_min = sprite->getInterpolatedV(renderMinX);
		tx_max = sprite->getInterpolatedU(1.0f - renderMinY);
		ty_max = sprite->getInterpolatedV(renderMaxX);
		tu_max = tx_max;
		tu_min = tx_min;
		tx_min = tx_max;
		tx_max = tu_min;
		tv_min = ty_max;
		tv_max = ty_min;
	}
	else if (uvRotateWest == 3)
	{
		tx_min = sprite->getInterpolatedU(1.0f - renderMinX);
		tx_max = sprite->getInterpolatedU(1.0f - renderMaxX);
		ty_min = sprite->getInterpolatedV(renderMaxY);
		ty_max = sprite->getInterpolatedV(renderMinY);
		tu_max = tx_max;
		tu_min = tx_min;
		tv_min = ty_min;
		tv_max = ty_max;
	}

	float posx_min = pos.x + renderMinX + m_transform.x;
	float posx_max = pos.x + renderMaxX + m_transform.x;
	float posy_min = pos.y + renderMinY + m_transform.y;
	float posy_max = pos.y + renderMaxY + m_transform.y;
	float posz = pos.z + renderMaxZ + m_transform.z;

	int pass = pcBlock->getRenderBlockPass();
	TessRenderable* renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite->getTexture(), (TESS_RENDER_PASS)pass, 1);
	TessVertexFmt* pVetLockPtr = NULL;
	if (!renderable->getLockPtr(1, pVetLockPtr))
		return;

	if (enableAO)
	{
		int color_tl = (colorTopLeft * aoTopLeft).getARGB();
		int color_bl = (colorBottomLeft * aoBottomLeft).getARGB();
		int color_br = (colorBottomRight * aoBottomRight).getARGB();
		int color_tr = (colorTopRight * aoTopRgith).getARGB();
		pVetLockPtr[0].setEx(m_sectionPos, posx_min - anti_gap, posy_max + anti_gap, posz, color_tl, brightnessTopLeft, tx_min, ty_min);
		pVetLockPtr[1].setEx(m_sectionPos, posx_min - anti_gap, posy_min - anti_gap, posz, color_bl, brightnessBottomLeft, tu_min, tv_max);
		pVetLockPtr[2].setEx(m_sectionPos, posx_max + anti_gap, posy_min - anti_gap, posz, color_br, brightnessBottomRight, tx_max, ty_max);
		pVetLockPtr[3].setEx(m_sectionPos, posx_max + anti_gap, posy_max + anti_gap, posz, color_tr, brightnessTopRight, tu_max, tv_min);
		//AutoCaculateQuadNormal(pVetLockPtr + 0);
	}
	else
	{
		pVetLockPtr[0].setEx(m_sectionPos, posx_min - anti_gap, posy_max + anti_gap, posz, m_color, m_bright, tx_min, ty_min);
		pVetLockPtr[1].setEx(m_sectionPos, posx_min - anti_gap, posy_min - anti_gap, posz, m_color, m_bright, tu_min, tv_max);
		pVetLockPtr[2].setEx(m_sectionPos, posx_max + anti_gap, posy_min - anti_gap, posz, m_color, m_bright, tx_max, ty_max);
		pVetLockPtr[3].setEx(m_sectionPos, posx_max + anti_gap, posy_max + anti_gap, posz, m_color, m_bright, tu_max, tv_min);
		//AutoCaculateQuadNormal(pVetLockPtr + 0);
	}
}

void RenderBlocks::renderFaceXNeg(cBlock* pcBlock, const Vector3& pos, AtlasSprite* sprite)
{
	if (hasOverrideBlockTexture())
	{
		sprite = overrideBlockTexture;
	}

	float tx_min = sprite->getInterpolatedU(renderMinZ);
	float tx_max = sprite->getInterpolatedU(renderMaxZ);
	float ty_min = sprite->getInterpolatedV(1.0f - renderMaxY);
	float ty_max = sprite->getInterpolatedV(1.0f - renderMinY);
	float tu_max;

	if (flipTexture)
	{
		tu_max = tx_min;
		tx_min = tx_max;
		tx_max = tu_max;
	}

	if (renderMinZ < 0.0 || renderMaxZ > 1.0)
	{
		tx_min = sprite->getMinU();
		tx_max = sprite->getMaxU();
	}

	if (renderMinY < 0.0 || renderMaxY > 1.0)
	{
		ty_min = sprite->getMinV();
		ty_max = sprite->getMaxV();
	}

	tu_max = tx_max;
	float tu_min = tx_min;
	float tv_min = ty_min;
	float tv_max = ty_max;

	if (uvRotateNorth == 1)
	{
		tx_min = sprite->getInterpolatedU(renderMinY);
		ty_min = sprite->getInterpolatedV(1.0f - renderMaxZ);
		tx_max = sprite->getInterpolatedU(renderMaxY);
		ty_max = sprite->getInterpolatedV(1.0f - renderMinZ);
		tv_min = ty_min;
		tv_max = ty_max;
		tu_max = tx_min;
		tu_min = tx_max;
		ty_min = ty_max;
		ty_max = tv_min;
	}
	else if (uvRotateNorth == 2)
	{
		tx_min = sprite->getInterpolatedU(1.0f - renderMaxY);
		ty_min = sprite->getInterpolatedV(renderMinZ);
		tx_max = sprite->getInterpolatedU(1.0f - renderMinY);
		ty_max = sprite->getInterpolatedV(renderMaxZ);
		tu_max = tx_max;
		tu_min = tx_min;
		tx_min = tx_max;
		tx_max = tu_min;
		tv_min = ty_max;
		tv_max = ty_min;
	}
	else if (uvRotateNorth == 3)
	{
		tx_min = sprite->getInterpolatedU(1.0f - renderMinZ);
		tx_max = sprite->getInterpolatedU(1.0f - renderMaxZ);
		ty_min = sprite->getInterpolatedV(renderMaxY);
		ty_max = sprite->getInterpolatedV(renderMinY);
		tu_max = tx_max;
		tu_min = tx_min;
		tv_min = ty_min;
		tv_max = ty_max;
	}

	float posx = pos.x + renderMinX + m_transform.x;
	float posy_min = pos.y + renderMinY + m_transform.y;
	float posy_max = pos.y + renderMaxY + m_transform.y;
	float posz_min = pos.z + renderMinZ + m_transform.z;
	float posz_max = pos.z + renderMaxZ + m_transform.z;
	
	int pass = pcBlock->getRenderBlockPass();
	TessRenderable* renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite->getTexture(), (TESS_RENDER_PASS)pass, 1);
	TessVertexFmt* pVetLockPtr = NULL;
	if (!renderable->getLockPtr(1, pVetLockPtr))
		return;

	if (enableAO)
	{
		int color_tl = (colorTopLeft * aoTopLeft).getARGB();
		int color_bl = (colorBottomLeft * aoBottomLeft).getARGB();
		int color_br = (colorBottomRight * aoBottomRight).getARGB();
		int color_tr = (colorTopRight * aoTopRgith).getARGB();
		pVetLockPtr[0].setEx(m_sectionPos, posx, posy_max + anti_gap, posz_max + anti_gap, color_tl, brightnessTopLeft, tu_max, tv_min);
		pVetLockPtr[1].setEx(m_sectionPos, posx, posy_max + anti_gap, posz_min - anti_gap, color_bl, brightnessBottomLeft, tx_min, ty_min);
		pVetLockPtr[2].setEx(m_sectionPos, posx, posy_min - anti_gap, posz_min - anti_gap, color_br, brightnessBottomRight, tu_min, tv_max);
		pVetLockPtr[3].setEx(m_sectionPos, posx, posy_min - anti_gap, posz_max + anti_gap, color_tr, brightnessTopRight, tx_max, ty_max);
		//AutoCaculateQuadNormal(pVetLockPtr + 0);
	}
	else
	{
		pVetLockPtr[0].setEx(m_sectionPos, posx, posy_max + anti_gap, posz_max + anti_gap, m_color, m_bright, tu_max, tv_min);
		pVetLockPtr[1].setEx(m_sectionPos, posx, posy_max + anti_gap, posz_min - anti_gap, m_color, m_bright, tx_min, ty_min);
		pVetLockPtr[2].setEx(m_sectionPos, posx, posy_min - anti_gap, posz_min - anti_gap, m_color, m_bright, tu_min, tv_max);
		pVetLockPtr[3].setEx(m_sectionPos, posx, posy_min - anti_gap, posz_max + anti_gap, m_color, m_bright, tx_max, ty_max);
		//AutoCaculateQuadNormal(pVetLockPtr + 0);
	}
}

void RenderBlocks::renderFaceXPos(cBlock* pcBlock, const Vector3& pos, AtlasSprite* sprite)
{
	if (hasOverrideBlockTexture())
	{
		sprite = overrideBlockTexture;
	}

	float tx_min = sprite->getInterpolatedU(renderMinZ);
	float tx_max = sprite->getInterpolatedU(renderMaxZ);
	float ty_min = sprite->getInterpolatedV(1.0f - renderMaxY);
	float ty_max = sprite->getInterpolatedV(1.0f - renderMinY);
	float tv_max;

	if (flipTexture)
	{
		tv_max = tx_min;
		tx_min = tx_max;
		tx_max = tv_max;
	}

	if (renderMinZ < 0.0 || renderMaxZ > 1.0)
	{
		tx_min = sprite->getMinU();
		tx_max = sprite->getMaxU();
	}

	if (renderMinY < 0.0 || renderMaxY > 1.0)
	{
		ty_min = sprite->getMinV();
		ty_max = sprite->getMaxV();
	}

	tv_max = tx_max;
	float tv_min = tx_min;
	float tu_min = ty_min;
	float tu_max = ty_max;

	if (uvRotateSouth == 2)
	{
		tx_min = sprite->getInterpolatedU(renderMinY);
		ty_min = sprite->getInterpolatedV(1.0f - renderMinZ);
		tx_max = sprite->getInterpolatedU(renderMaxY);
		ty_max = sprite->getInterpolatedV(1.0f - renderMaxZ);
		tu_min = ty_min;
		tu_max = ty_max;
		tv_max = tx_min;
		tv_min = tx_max;
		ty_min = ty_max;
		ty_max = tu_min;
	}
	else if (uvRotateSouth == 1)
	{
		tx_min = sprite->getInterpolatedU(1.0f - renderMaxY);
		ty_min = sprite->getInterpolatedV(renderMaxZ);
		tx_max = sprite->getInterpolatedU(1.0f - renderMinY);
		ty_max = sprite->getInterpolatedV(renderMinZ);
		tv_max = tx_max;
		tv_min = tx_min;
		tx_min = tx_max;
		tx_max = tv_min;
		tu_min = ty_max;
		tu_max = ty_min;
	}
	else if (uvRotateSouth == 3)
	{
		tx_min = sprite->getInterpolatedU(1.0f - renderMinZ);
		tx_max = sprite->getInterpolatedU(1.0f - renderMaxZ);
		ty_min = sprite->getInterpolatedV(renderMaxY);
		ty_max = sprite->getInterpolatedV(renderMinY);
		tv_max = tx_max;
		tv_min = tx_min;
		tu_min = ty_min;
		tu_max = ty_max;
	}

	float posx = pos.x + renderMaxX + m_transform.x;
	float posy_min = pos.y + renderMinY + m_transform.y;
	float posy_max = pos.y + renderMaxY + m_transform.y;
	float posz_min = pos.z + renderMinZ + m_transform.z;
	float posz_max = pos.z + renderMaxZ + m_transform.z;

	int pass = pcBlock->getRenderBlockPass();
	TessRenderable* renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite->getTexture(), (TESS_RENDER_PASS)pass, 1);
	TessVertexFmt* pVetLockPtr = NULL;
	if (!renderable->getLockPtr(1, pVetLockPtr))
		return;

	if (enableAO)
	{
		int color_tl = (colorTopLeft * aoTopLeft).getARGB();
		int color_bl = (colorBottomLeft * aoBottomLeft).getARGB();
		int color_br = (colorBottomRight * aoBottomRight).getARGB();
		int color_tr = (colorTopRight * aoTopRgith).getARGB();
		pVetLockPtr[0].setEx(m_sectionPos, posx, posy_min - anti_gap, posz_max + anti_gap, color_tl, brightnessTopLeft, tv_min, tu_max);
		pVetLockPtr[1].setEx(m_sectionPos, posx, posy_min - anti_gap, posz_min - anti_gap, color_bl, brightnessBottomLeft, tx_max, ty_max);
		pVetLockPtr[2].setEx(m_sectionPos, posx, posy_max + anti_gap, posz_min - anti_gap, color_br, brightnessBottomRight, tv_max, tu_min);
		pVetLockPtr[3].setEx(m_sectionPos, posx, posy_max + anti_gap, posz_max + anti_gap, color_tr, brightnessTopRight, tx_min, ty_min);
		//AutoCaculateQuadNormal(pVetLockPtr + 0);
	}
	else
	{
		pVetLockPtr[0].setEx(m_sectionPos, posx, posy_min - anti_gap, posz_max + anti_gap, m_color, m_bright, tv_min, tu_max);
		pVetLockPtr[1].setEx(m_sectionPos, posx, posy_min - anti_gap, posz_min - anti_gap, m_color, m_bright, tx_max, ty_max);
		pVetLockPtr[2].setEx(m_sectionPos, posx, posy_max + anti_gap, posz_min - anti_gap, m_color, m_bright, tv_max, tu_min);
		pVetLockPtr[3].setEx(m_sectionPos, posx, posy_max + anti_gap, posz_max + anti_gap, m_color, m_bright, tx_min, ty_min);
		//AutoCaculateQuadNormal(pVetLockPtr + 0);
	}
}

void RenderBlocks::renderBlockAsItem(Block* pBlock, cBlock* pcBlock, int meta)
{
	int blockID = pBlock->getBlockID();
	bool isGrass = blockID == BLOCK_ID_GRASS;

	if (blockID == BLOCK_ID_DISPENSER || blockID == BLOCK_ID_DROPPER || blockID == BLOCK_ID_FURNACE_IDLE)
		meta = 3;
	
	if (useInventoryTint)
	{
		m_color = pcBlock->getRenderColor(meta);
		if (isGrass)
			m_color = 0xFFFFFF;
	}

	int renderType = pcBlock->getRenderType();
	setRenderBoundsFromBlock(pBlock);
	
	if (renderType != RENDER_TYPE_TRAPDOOR &&
		renderType != RENDER_TYPE_HAY &&
		renderType != RENDER_TYPE_QUARTZ &&
		renderType != RENDER_TYPE_PISTON_BASE &&
		renderType != RENDER_TYPE_PORTAL_FRAME)
	{
		if (renderType == RENDER_TYPE_FLOWER)
		{
			drawCrossedSquares(pBlock, pcBlock, meta, Vector3::ZERO, 1.0F, m_color, 0);
		}
		else if (renderType == RENDER_TYPE_STEM)
		{
			pBlock->setBlockBoundsForItemRender();
			renderBlockStemSmall(pBlock, pcBlock, meta, renderMaxY, Vector3(0.f, 0.f, 0.5f), m_color, 0);
		}
		else if (renderType == RENDER_TYPE_LILYPAD)
		{
			pBlock->setBlockBoundsForItemRender();
		}
		else if (renderType == RENDER_TYPE_CACTUS)
		{
			pBlock->setBlockBoundsForItemRender();
			float trans = 0.0625f;
			renderFaceYNeg(pcBlock, Vector3::ZERO, getBlockIconFromSide(pcBlock, 0));
			renderFaceYPos(pcBlock, Vector3::ZERO, getBlockIconFromSide(pcBlock, 1));
			Vector3 bak_trans = m_transform;
			m_transform = bak_trans + Vector3(0.0f, 0.f, trans);
			renderFaceZNeg(pcBlock, Vector3::ZERO, getBlockIconFromSide(pcBlock, 2));
			m_transform = bak_trans + Vector3(0.0f, 0.f, -trans);
			renderFaceZPos(pcBlock, Vector3::ZERO, getBlockIconFromSide(pcBlock, 3));
			m_transform = bak_trans + Vector3(trans, 0.f, 0.f);
			renderFaceXNeg(pcBlock, Vector3::ZERO, getBlockIconFromSide(pcBlock, 4));
			m_transform = bak_trans + Vector3(-trans, 0.f, 0.f);
			renderFaceXPos(pcBlock, Vector3::ZERO, getBlockIconFromSide(pcBlock, 5));
			m_transform = bak_trans;
		}
		else if (renderType == RENDER_TYPE_CHEST)
		{
			renderChest(pBlock, meta, Vector3(0.5f, 0.f, 0.5f));
		}
		else if (renderType == RENDER_TYPE_CROPS)
		{
			renderBlockCropsImpl(pcBlock, 0xFFFFFFFF, meta, -Vector3::HALF);
		}
		else if (renderType == RENDER_TYPE_TORCH)
		{
			renderTorchAtAngle(pBlock, pcBlock, Vector3::ZERO, 0.f, 0.f, 0);
		}
		else if (renderType == RENDER_TYPE_STAIRS)
		{
			for (int i = 0; i < 2; ++i)
			{
				if (i == 0)
					setRenderBounds(0.f, 0.f, 0.f, 1.f, 1.f, 0.5f);
				if (i == 1)
					setRenderBounds(0.f, 0.f, 0.5f, 1.f, 0.5f, 1.f);

				renderFaceYNeg(pcBlock, Vector3::ZERO, getBlockIconFromSide(pcBlock, 0));
				renderFaceYPos(pcBlock, Vector3::ZERO, getBlockIconFromSide(pcBlock, 1));
				renderFaceZNeg(pcBlock, Vector3::ZERO, getBlockIconFromSide(pcBlock, 2));
				renderFaceZPos(pcBlock, Vector3::ZERO, getBlockIconFromSide(pcBlock, 3));
				renderFaceXNeg(pcBlock, Vector3::ZERO, getBlockIconFromSide(pcBlock, 4));
				renderFaceXPos(pcBlock, Vector3::ZERO, getBlockIconFromSide(pcBlock, 5));
			}
		}
		else if (renderType == RENDER_TYPE_DRAGON_EGG)
		{
			int var14 = 0;

			for (int i = 0; i < 8; ++i)
			{
				int var16 = 0;
				int var17 = 1;

				switch (i)
				{
				case 0: var16 = 2; break;
				case 1: var16 = 3; break;
				case 2: var16 = 4; break;
				case 3: var16 = 5; var17 = 2; break;
				case 4: var16 = 6; var17 = 3; break;
				case 5: var16 = 7; var17 = 5; break;
				case 6: var16 = 6; var17 = 2; break;
				case 7: var16 = 3;
				}
			
				float var11 = (float)var16 / 16.0F;
				float var12 = 1.0F - (float)var14 / 16.0F;
				float var13 = 1.0F - (float)(var14 + var17) / 16.0F;
				var14 += var17;
				setRenderBounds(0.5F - var11, var13, 0.5F - var11, 0.5F + var11, var12, 0.5F + var11);
				renderFaceYNeg(pcBlock, Vector3::ZERO, getBlockIconFromSide(pcBlock, 0));
				renderFaceYPos(pcBlock, Vector3::ZERO, getBlockIconFromSide(pcBlock, 1));
				renderFaceZNeg(pcBlock, Vector3::ZERO, getBlockIconFromSide(pcBlock, 2));
				renderFaceZPos(pcBlock, Vector3::ZERO, getBlockIconFromSide(pcBlock, 3));
				renderFaceXNeg(pcBlock, Vector3::ZERO, getBlockIconFromSide(pcBlock, 4));
				renderFaceXPos(pcBlock, Vector3::ZERO, getBlockIconFromSide(pcBlock, 5));
			}

			setRenderBounds(0.f, 0.f, 0.f, 1.f, 1.f, 1.f);
		}
		else if (renderType == RENDER_TYPE_FENCE)
		{
			for (int i = 0; i < 4; ++i)
			{
				float trans = 0.125F;

				if (i == 0)
					setRenderBounds(0.5f - trans, 0.f, 0.f, 0.5F + trans, 1.f, trans * 2.f);
				if (i == 1)
					setRenderBounds(0.5f - trans, 0.f, 1.f - trans * 2.f, 0.5F + trans, 1.f, 1.f);

				trans = 0.0625F;
				if (i == 2)
					setRenderBounds(0.5f - trans, 1.f - trans * 3.f, -trans * 2.f, 0.5f + trans, 1.f - trans, 1.f + trans * 2.f);
				if (i == 3)
					setRenderBounds(0.5f - trans, 0.5f - trans * 3.f, -trans * 2.f, 0.5f + trans, 0.5f - trans, 1.f + trans * 2.f);
				
				renderFaceYNeg(pcBlock, Vector3::ZERO, getBlockIconFromSide(pcBlock, 0));
				renderFaceYPos(pcBlock, Vector3::ZERO, getBlockIconFromSide(pcBlock, 1));
				renderFaceZNeg(pcBlock, Vector3::ZERO, getBlockIconFromSide(pcBlock, 2));
				renderFaceZPos(pcBlock, Vector3::ZERO, getBlockIconFromSide(pcBlock, 3));
				renderFaceXNeg(pcBlock, Vector3::ZERO, getBlockIconFromSide(pcBlock, 4));
				renderFaceXPos(pcBlock, Vector3::ZERO, getBlockIconFromSide(pcBlock, 5));
			}

			setRenderBounds(0.f, 0.f, 0.f, 1.f, 1.f, 1.f);
		}
		else if (renderType == RENDER_TYPE_FENCE_GATE)
		{
			for (int i = 0; i < 3; ++i)
			{
				float trans = 0.0625F;

				if (i == 0)
					setRenderBounds(0.5f - trans, 0.3f, 0.0f, 0.5F + trans, 1.f, trans * 2.f);
				if (i == 1)
					setRenderBounds(0.5f - trans, 0.3f, 1.f - trans * 2.f, 0.5f + trans, 1.f, 1.f);
				if (i == 2)
					setRenderBounds(0.5f - trans, 0.5f, 0.f, 0.5f + trans, 1.0f - trans, 1.f);

				renderFaceYNeg(pcBlock, Vector3::ZERO, getBlockIconFromSide(pcBlock, 0));
				renderFaceYPos(pcBlock, Vector3::ZERO, getBlockIconFromSide(pcBlock, 1));
				renderFaceZNeg(pcBlock, Vector3::ZERO, getBlockIconFromSide(pcBlock, 2));
				renderFaceZPos(pcBlock, Vector3::ZERO, getBlockIconFromSide(pcBlock, 3));
				renderFaceXNeg(pcBlock, Vector3::ZERO, getBlockIconFromSide(pcBlock, 4));
				renderFaceXPos(pcBlock, Vector3::ZERO, getBlockIconFromSide(pcBlock, 5));
			}
		}
		else if (renderType == RENDER_TYPE_WALL)
		{
			for (int i = 0; i < 2; ++i)
			{
				if (i == 0)
					setRenderBounds(0.f, 0.f, 0.3125f, 1.f, 0.8125f, 0.6875f);
				if (i == 1)
					setRenderBounds(0.25f, 0.f, 0.25f, 0.75f, 1.f, 0.75f);

				renderFaceYNeg(pcBlock, Vector3::ZERO, getBlockIconFromSideAndMetadata(pcBlock, 0, meta));
				renderFaceYPos(pcBlock, Vector3::ZERO, getBlockIconFromSideAndMetadata(pcBlock, 1, meta));
				renderFaceZNeg(pcBlock, Vector3::ZERO, getBlockIconFromSideAndMetadata(pcBlock, 2, meta));
				renderFaceZPos(pcBlock, Vector3::ZERO, getBlockIconFromSideAndMetadata(pcBlock, 3, meta));
				renderFaceXNeg(pcBlock, Vector3::ZERO, getBlockIconFromSideAndMetadata(pcBlock, 4, meta));
				renderFaceXPos(pcBlock, Vector3::ZERO, getBlockIconFromSideAndMetadata(pcBlock, 5, meta));
			}
			setRenderBounds(0.f, 0.f, 0.f, 1.f, 1.f, 1.f);
		}
		else if (renderType == RENDER_TYPE_ANVIL)
		{
			renderBlockAnvilOrient(pBlock, pcBlock, BlockPos::ZERO, meta << 2, true);
		}
		else if (renderType == RENDER_TYPE_BEACON)
		{
			for (int i = 0; i < 3; ++i)
			{
				if (i == 0)
				{
					setRenderBounds(0.125f, 0.f, 0.125f, 0.875f, 0.1875f, 0.875f);
					setOverrideBlockTexture(getBlockIcon(cBlockManager::scBlocks[BLOCK_ID_OBSIDIAN]));
				}
				else if (i == 1)
				{
					setRenderBounds(0.1875f, 0.1875f, 0.1875f, 0.8125f, 0.875f, 0.8125f);
					setOverrideBlockTexture(getBlockIcon(cBlockManager::scBlocks[BLOCK_ID_BEACON]));
				}
				else if (i == 2)
				{
					setRenderBounds(0.f, 0.f, 0.f, 1.f, 1.f, 1.f);
					setOverrideBlockTexture(getBlockIcon(cBlockManager::scBlocks[BLOCK_ID_GLASS]));
				}

				renderFaceYNeg(pcBlock, Vector3::ZERO, getBlockIconFromSideAndMetadata(pcBlock, 0, meta));
				renderFaceYPos(pcBlock, Vector3::ZERO, getBlockIconFromSideAndMetadata(pcBlock, 1, meta));
				renderFaceZNeg(pcBlock, Vector3::ZERO, getBlockIconFromSideAndMetadata(pcBlock, 2, meta));
				renderFaceZPos(pcBlock, Vector3::ZERO, getBlockIconFromSideAndMetadata(pcBlock, 3, meta));
				renderFaceXNeg(pcBlock, Vector3::ZERO, getBlockIconFromSideAndMetadata(pcBlock, 4, meta));
				renderFaceXPos(pcBlock, Vector3::ZERO, getBlockIconFromSideAndMetadata(pcBlock, 5, meta));
			}

			setRenderBounds(0.f, 0.f, 0.f, 1.f, 1.f, 1.f);
			clearOverrideBlockTexture();
		}
		else if (renderType == RENDER_TYPE_FLOWER_POT)
		{
			renderBlockHopperMetadata(pBlock, pcBlock, BlockPos::ZERO, 0, true);
		}
		else if (renderType == RENDER_TYPE_FRUIT)
		{
			renderBlockFruitImpl(pcBlock, 0xFFFFFFFF, 0, -Vector3::HALF);
		}
	}
	else
	{
		if (renderType == RENDER_TYPE_PISTON_BASE)
		{
			meta = 1;
		}

		// rotateY(Math::PI_DIV2)
		pBlock->setBlockBoundsForItemRender();
		setRenderBoundsFromBlock(pBlock);
		renderFaceYNeg(pcBlock, Vector3::ZERO, getBlockIconFromSideAndMetadata(pcBlock, 0, meta));

		if (isGrass && useInventoryTint)
			m_color = pcBlock->getRenderColor(meta);

		renderFaceYPos(pcBlock, Vector3::ZERO, getBlockIconFromSideAndMetadata(pcBlock, 1, meta));

		if (isGrass && useInventoryTint)
			m_color = 0xFFFFFFFF;

		renderFaceZNeg(pcBlock, Vector3::ZERO, getBlockIconFromSideAndMetadata(pcBlock, 2, meta));
		renderFaceZPos(pcBlock, Vector3::ZERO, getBlockIconFromSideAndMetadata(pcBlock, 3, meta));
		renderFaceXNeg(pcBlock, Vector3::ZERO, getBlockIconFromSideAndMetadata(pcBlock, 4, meta));
		renderFaceXPos(pcBlock, Vector3::ZERO, getBlockIconFromSideAndMetadata(pcBlock, 5, meta));
	}
}

void RenderBlocks::renderBlockStemSmall(Block* pBlock, cBlock* pcBlock, int metadata, float renderMaxY, const Vector3& pos, int color, int bright)
{
	AtlasSprite* sprite = getBlockIconFromSideAndMetadata(pcBlock, 0, metadata);

	if (hasOverrideBlockTexture())
	{
		sprite = overrideBlockTexture;
	}

	float min_u = sprite->getMinU();
	float min_v = sprite->getMinV();
	float max_u = sprite->getMaxU();
	float max_v = sprite->getInterpolatedV(renderMaxY);
	//float max_v = sprite->getMaxV();
	float min_x = pos.x + 0.5F - 0.45F;
	float max_x = pos.x + 0.5F + 0.45F;
	float min_z = pos.z + 0.5F - 0.45F;
	float max_z = pos.z + 0.5F + 0.45F;

	int pass = pcBlock->getRenderBlockPass();
	TessRenderable* renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite->getTexture(), (TESS_RENDER_PASS)pass, 4);
	TessVertexFmt* pVetLockPtr = NULL;
	if(!renderable->getLockPtr(4, pVetLockPtr))
		return;

	pVetLockPtr[0].setEx(m_sectionPos, min_x, pos.y + renderMaxY, min_z, color, bright, min_u, min_v);
	pVetLockPtr[1].setEx(m_sectionPos, min_x, pos.y + 0.0F, min_z, color, bright, min_u, max_v);
	pVetLockPtr[2].setEx(m_sectionPos, max_x, pos.y + 0.0F, max_z, color, bright, max_u, max_v);
	pVetLockPtr[3].setEx(m_sectionPos, max_x, pos.y + renderMaxY, max_z, color, bright, max_u, min_v);
	//AutoCaculateQuadNormal(pVetLockPtr + 0);

	pVetLockPtr[4].setEx(m_sectionPos, max_x, pos.y + renderMaxY, max_z, color, bright, min_u, min_v);
	pVetLockPtr[5].setEx(m_sectionPos, max_x, pos.y + 0.0F, max_z, color, bright, min_u, max_v);
	pVetLockPtr[6].setEx(m_sectionPos, min_x, pos.y + 0.0F, min_z, color, bright, max_u, max_v);
	pVetLockPtr[7].setEx(m_sectionPos, min_x, pos.y + renderMaxY, min_z, color, bright, max_u, min_v);
	//AutoCaculateQuadNormal(pVetLockPtr + 4);

	pVetLockPtr[8].setEx(m_sectionPos, min_x, pos.y + renderMaxY, max_z, color, bright, min_u, min_v);
	pVetLockPtr[9].setEx(m_sectionPos, min_x, pos.y + 0.0F, max_z, color, bright, min_u, max_v);
	pVetLockPtr[10].setEx(m_sectionPos, max_x, pos.y + 0.0F, min_z, color, bright, max_u, max_v);
	pVetLockPtr[11].setEx(m_sectionPos, max_x, pos.y + renderMaxY, min_z, color, bright, max_u, min_v);
	//AutoCaculateQuadNormal(pVetLockPtr + 8);

	pVetLockPtr[12].setEx(m_sectionPos, max_x, pos.y + renderMaxY, min_z, color, bright, min_u, min_v);
	pVetLockPtr[13].setEx(m_sectionPos, max_x, pos.y + 0.0F, min_z, color, bright, min_u, max_v);
	pVetLockPtr[14].setEx(m_sectionPos, min_x, pos.y + 0.0F, max_z, color, bright, max_u, max_v);
	pVetLockPtr[15].setEx(m_sectionPos, min_x, pos.y + renderMaxY, max_z, color, bright, max_u, min_v);
	//AutoCaculateQuadNormal(pVetLockPtr + 12);
}

void RenderBlocks::renderBlockStemBig(Block* pBlock, cBlock* pcBlock, int metadata, int state, float renderMaxY, const Vector3& pos, int color, int bright)
{		
	AtlasSprite* sprite = getBlockIconFromSideAndMetadata(pcBlock, 1, metadata);

	if (hasOverrideBlockTexture())
	{
		sprite = overrideBlockTexture;
	}

	float min_u = sprite->getMinU();
	float min_v = sprite->getMinV();
	float max_u = sprite->getMaxU();
	float max_v = sprite->getMaxV();
	float min_x = pos.x + 0.5F - 0.5F;
	float max_x = pos.x + 0.5F + 0.5F;
	float min_z = pos.z + 0.5F - 0.5F;
	float max_z = pos.z + 0.5F + 0.5F;
	float mid_x = pos.x + 0.5F;
	float mid_z = pos.z + 0.5F;

	if ((state + 1) / 2 % 2 == 1)
	{
		float var34 = max_u;
		max_u = min_u;
		min_u = var34;
	}

	int pass = pcBlock->getRenderBlockPass();
	TessRenderable* renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite->getTexture(), (TESS_RENDER_PASS)pass, 2);
	TessVertexFmt* pVetLockPtr = NULL;
	if (!renderable->getLockPtr(2, pVetLockPtr))
		return;

	if (state < 2)
	{
		pVetLockPtr[0].setEx(m_sectionPos, min_x, pos.y + renderMaxY, mid_z, color, bright, min_u, min_v);
		pVetLockPtr[1].setEx(m_sectionPos, min_x, pos.y + 0.0F, mid_z, color, bright, min_u, max_v);
		pVetLockPtr[2].setEx(m_sectionPos, max_x, pos.y + 0.0F, mid_z, color, bright, max_u, max_v);
		pVetLockPtr[3].setEx(m_sectionPos, max_x, pos.y + renderMaxY, mid_z, color, bright, max_u, min_v);
		//AutoCaculateQuadNormal(pVetLockPtr + 0);

		pVetLockPtr[4].setEx(m_sectionPos, max_x, pos.y + renderMaxY, mid_z, color, bright, max_u, min_v);
		pVetLockPtr[5].setEx(m_sectionPos, max_x, pos.y + 0.0F, mid_z, color, bright, max_u, max_v);
		pVetLockPtr[6].setEx(m_sectionPos, min_x, pos.y + 0.0F, mid_z, color, bright, min_u, max_v);
		pVetLockPtr[7].setEx(m_sectionPos, min_x, pos.y + renderMaxY, mid_z, color, bright, min_u, min_v);
		//AutoCaculateQuadNormal(pVetLockPtr + 4);
	}
	else
	{
		pVetLockPtr[0].setEx(m_sectionPos, mid_x, pos.y + renderMaxY, max_z, color, bright, min_u, min_v);
		pVetLockPtr[1].setEx(m_sectionPos, mid_x, pos.y + 0.0F, max_z, color, bright, min_u, max_v);
		pVetLockPtr[2].setEx(m_sectionPos, mid_x, pos.y + 0.0F, min_z, color, bright, max_u, max_v);
		pVetLockPtr[3].setEx(m_sectionPos, mid_x, pos.y + renderMaxY, min_z, color, bright, max_u, min_v);
		//AutoCaculateQuadNormal(pVetLockPtr + 0);

		pVetLockPtr[4].setEx(m_sectionPos, mid_x, pos.y + renderMaxY, min_z, color, bright, max_u, min_v);
		pVetLockPtr[5].setEx(m_sectionPos, mid_x, pos.y + 0.0F, min_z, color, bright, max_u, max_v);
		pVetLockPtr[6].setEx(m_sectionPos, mid_x, pos.y + 0.0F, max_z, color, bright, min_u, max_v);
		pVetLockPtr[7].setEx(m_sectionPos, mid_x, pos.y + renderMaxY, max_z, color, bright, min_u, min_v);
		//AutoCaculateQuadNormal(pVetLockPtr + 4);
	}
}

bool RenderBlocks::renderBlockAnvilOrient(Block* pBlock, cBlock* pcBlock, const BlockPos& pos, int metadata, bool par6)
{
	int state = par6 ? 0 : metadata & 3;
	bool flag = false;
	float var9 = 0.0F;

	switch (state)
	{
	case 0:
		uvRotateSouth = 2;
		uvRotateNorth = 1;
		uvRotateTop = 3;
		uvRotateBottom = 3;
		break;
	case 1:
		uvRotateEast = 1;
		uvRotateWest = 2;
		uvRotateTop = 2;
		uvRotateBottom = 1;
		flag = true;
		break;
	case 2:
		uvRotateSouth = 1;
		uvRotateNorth = 2;
		break;
	case 3:
		uvRotateEast = 2;
		uvRotateWest = 1;
		uvRotateTop = 1;
		uvRotateBottom = 2;
		flag = true;
		break;
	default:
		break;
	}

	var9 = renderBlockAnvilRotate(pBlock, pcBlock, pos, 0, var9, 0.75F, 0.25F, 0.75F, flag, par6, metadata);
	var9 = renderBlockAnvilRotate(pBlock, pcBlock, pos, 1, var9, 0.5F, 0.0625F, 0.625F, flag, par6, metadata);
	var9 = renderBlockAnvilRotate(pBlock, pcBlock, pos, 2, var9, 0.25F, 0.3125F, 0.5F, flag, par6, metadata);
	renderBlockAnvilRotate(pBlock, pcBlock, pos, 3, var9, 0.625F, 0.375F, 1.0F, flag, par6, metadata);
	setRenderBounds(0.0F, 0.0F, 0.0F, 1.0F, 1.0F, 1.0F);
	uvRotateEast = 0;
	uvRotateWest = 0;
	uvRotateSouth = 0;
	uvRotateNorth = 0;
	uvRotateTop = 0;
	uvRotateBottom = 0;
	return true;
}

float RenderBlocks::renderBlockAnvilRotate(Block* pBlock, cBlock* pcBlock, const BlockPos& pos, int state, float par6, float par7, float par8, float par9, bool isFlip, bool par11, int metadata)
{
	cBlockAnvil* pBlockAnvil = (cBlockAnvil*)(pBlock);

	if (isFlip)
	{
		float t = par7;
		par7 = par9;
		par9 = t;
	}

	par7 /= 2.0F;
	par9 /= 2.0F;

	pBlockAnvil->m_anvilRenderState = state;
	setRenderBounds(0.5F - par7, par6, 0.5F - par9, 0.5F + par7, par6 + par8, 0.5F + par9);

	if (par11)
	{
		//Tessellator var14 = Tessellator.instance;
		//var14.setNormal(0.0F, -1.0F, 0.0F);
		renderFaceYNeg(pcBlock, Vector3::ZERO, getBlockIconFromSideAndMetadata(pcBlock, 0, metadata));
		//var14.setNormal(0.0F, 1.0F, 0.0F);
		renderFaceYPos(pcBlock, Vector3::ZERO, getBlockIconFromSideAndMetadata(pcBlock, 1, metadata));
		//var14.setNormal(0.0F, 0.0F, -1.0F);
		renderFaceZNeg(pcBlock, Vector3::ZERO, getBlockIconFromSideAndMetadata(pcBlock, 2, metadata));
		//var14.setNormal(0.0F, 0.0F, 1.0F);
		renderFaceZPos(pcBlock, Vector3::ZERO, getBlockIconFromSideAndMetadata(pcBlock, 3, metadata));
		//var14.setNormal(-1.0F, 0.0F, 0.0F);
		renderFaceXNeg(pcBlock, Vector3::ZERO, getBlockIconFromSideAndMetadata(pcBlock, 4, metadata));
		//var14.setNormal(1.0F, 0.0F, 0.0F);
		renderFaceXPos(pcBlock, Vector3::ZERO, getBlockIconFromSideAndMetadata(pcBlock, 5, metadata));
	}
	else
	{
		renderStandardBlock(pBlockAnvil, pcBlock, pos);
	}

	return par6 + par8;
}

bool RenderBlocks::renderBlockBannerOrient(Block* pBlock, cBlock* pcBlock, const BlockPos& pos, int metadata)
{
	uvRotateEast = 0;
	uvRotateWest = 0;
	uvRotateSouth = 0;
	uvRotateNorth = 0;
	uvRotateTop = 0;
	uvRotateBottom = 0;

	int color = pcBlock->getRenderColor(0);
	int rad = metadata;

	AtlasSprite* sprite = getBlockIcon(pcBlock, blockAccess, pos, 2);
	TessRenderable* renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite->getTexture(), TRP_SOLID, 0);
	TessVertexFmt* pVertBegin = NULL;
	renderable->getLockPtr(0, pVertBegin);

	if (pcBlock)
	{
		color = pcBlock->getRenderColor(color);
	}

	renderBlockBannerRotate(pBlock, pcBlock, pos, 0.45F, 0.0F, 0.45F, 0.55F, 1.85F, 0.55F, false, 0, false);
	renderBlockBannerRotate(pBlock, pcBlock, pos, 0.0F, 1.85F, 0.45F, 1.0F, 2.0F, 0.55F, false, 0, false);
	renderBlockBannerRotate(pBlock, pcBlock, pos, 0.0F, 0.2F, 0.38F, 1.0F, 2.0F, 0.445F, true, color, true);

	TessVertexFmt* pVertEnd = NULL;
	renderable->getLockPtr(0, pVertEnd);

	int vertCnt = ((uintptr_t)pVertEnd - (uintptr_t)pVertBegin) / sizeof(TessVertexFmt);

	Quaternion q(Vector3::UNIT_Y, Math::PI_DIV8 * rad); // pi / 8.

// 	Vector3 offset;
// 	offset.x = pos.x + 0.5f;
// 	offset.z = pos.z + 0.5f;
// 	offset.y = 0;

	Vector3 offset = Vector3(pos) - m_sectionPos + Vector3::HALF;

	for (int i = 0; i < vertCnt; ++i)
	{
		Vector3 vpos = pVertBegin[i].GetPosition();  // unpack
		vpos -= offset;
		vpos = q * vpos;
		vpos += offset;
		pVertBegin[i].SetPosition(vpos.x, vpos.y, vpos.z); // pack
	}

	//通过直接修改内存来修正角度！！！！hack方式
	setRenderBounds(0.0F, 0.0F, 0.0F, 1.0F, 1.0F, 1.0F);
	uvRotateEast = 0;
	uvRotateWest = 0;
	uvRotateSouth = 0;
	uvRotateNorth = 0;
	uvRotateTop = 0;
	uvRotateBottom = 0;
	return true;
}

void RenderBlocks::renderBlockBannerRotate(Block* pBlock, cBlock* pcBlock, const BlockPos& pos, float minX, float minY, float minZ, float maxX, float maxY, float maxZ, bool is_color, int color, bool is_slate)
{
	cBlockBanner* pBlockBanner = (cBlockBanner*)(pBlock);

	pBlockBanner->m_is_slate = is_slate;
	setRenderBounds(minX, minY, minZ, maxX, maxY, maxZ);

	if (color)
	{
		float r = (float)(color >> 16 & 255) / 255.0F;
		float g = (float)(color >> 8 & 255) / 255.0F;
		float b = (float)(color & 255) / 255.0F;
		Color _color = Color(r, g, b);
		renderStandardBlockWithColorMultiplier(pBlock, pcBlock, pos, _color);
	}
	else
	{
		renderStandardBlock(pBlockBanner, pcBlock, pos);
	}
}

void RenderBlocks::renderPistonRodUD(float minX, float maxX, float minY, float maxY, float minZ, float maxZ, float par13, float par14)
{
	AtlasSprite* sprite = cBlockPistonBase::getIconByName("piston_side");

	if (hasOverrideBlockTexture())
	{
		sprite = overrideBlockTexture;
	}
	
	float min_u = (float)sprite->getMinU();
	float min_v = (float)sprite->getMinV();
	float var22 = (float)sprite->getInterpolatedU(par14 / 16.0F);
	float var24 = (float)sprite->getInterpolatedV(4.0F / 16.0F);
	
	int color = Color(par13, par13, par13).getARGB();		

	TessRenderable* renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite->getTexture(), TRP_SOLID, 1);
	TessVertexFmt* pVetLockPtr = NULL;
	if(!renderable->getLockPtr(1, pVetLockPtr))
		return;

	pVetLockPtr[0].setEx(m_sectionPos, minX, maxY, minZ, color, brightnessTopLeft, var22, min_v);
	pVetLockPtr[1].setEx(m_sectionPos, minX, minY, minZ, color, brightnessBottomLeft, min_u, min_v);
	pVetLockPtr[2].setEx(m_sectionPos, maxX, minY, maxZ, color, brightnessBottomRight, min_u, var24);
	pVetLockPtr[3].setEx(m_sectionPos, maxX, maxY, maxZ, color, brightnessTopRight, var22, var24);
	//AutoCaculateQuadNormal(pVetLockPtr + 0);
}

void RenderBlocks::renderPistonRodSN(float minX, float maxX, float minY, float maxY, float minZ, float maxZ, float par13, float par14)
{
	AtlasSprite* sprite = cBlockPistonBase::getIconByName("piston_side");

	if (hasOverrideBlockTexture())
	{
		sprite = overrideBlockTexture;
	}

	float min_u = (float)sprite->getMinU();
	float min_v = (float)sprite->getMinV();
	float var22 = (float)sprite->getInterpolatedU(par14 / 16.0F);
	float var24 = (float)sprite->getInterpolatedV(4.0F / 16.0F);

	int color = Color(par13, par13, par13).getARGB();	

	TessRenderable* renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite->getTexture(), TRP_SOLID, 1);
	TessVertexFmt* pVetLockPtr = NULL;
	if (!renderable->getLockPtr(1, pVetLockPtr))
		return;

	pVetLockPtr[0].setEx(m_sectionPos, minX, minY, maxZ, color, brightnessTopLeft, var22, min_v);
	pVetLockPtr[1].setEx(m_sectionPos, minX, minY, minZ, color, brightnessBottomLeft, min_u, min_v);
	pVetLockPtr[2].setEx(m_sectionPos, maxX, maxY, minZ, color, brightnessBottomRight, min_u, var24);
	pVetLockPtr[3].setEx(m_sectionPos, maxX, maxY, maxZ, color, brightnessTopRight, var22, var24);
	//AutoCaculateQuadNormal(pVetLockPtr + 0);
}

void RenderBlocks::renderPistonRodEW(float minX, float maxX, float minY, float maxY, float minZ, float maxZ, float par13, float par14)
{
	AtlasSprite* sprite = cBlockPistonBase::getIconByName("piston_side");

	if (hasOverrideBlockTexture())
	{
		sprite = overrideBlockTexture;
	}

	float min_u = (float)sprite->getMinU();
	float min_v = (float)sprite->getMinV();
	float var22 = (float)sprite->getInterpolatedU(par14 / 16.0F);
	float var24 = (float)sprite->getInterpolatedV(4.0F / 16.0F);

	int color = Color(par13, par13, par13).getARGB();	

	TessRenderable* renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite->getTexture(), TRP_SOLID, 1);
	TessVertexFmt* pVetLockPtr = NULL;
	if (!renderable->getLockPtr(1, pVetLockPtr))
		return;

	pVetLockPtr[0].setEx(m_sectionPos, maxX, minY, minZ, color, brightnessTopLeft, var22, min_v);
	pVetLockPtr[1].setEx(m_sectionPos, minX, minY, minZ, color, brightnessBottomLeft, min_u, min_v);
	pVetLockPtr[2].setEx(m_sectionPos, minX, maxY, maxZ, color, brightnessBottomRight, min_u, var24);
	pVetLockPtr[3].setEx(m_sectionPos, maxX, maxY, maxZ, color, brightnessTopRight, var22, var24);
	//AutoCaculateQuadNormal(pVetLockPtr + 0);
}

void RenderBlocks::renderTorchAtAngle(Block* pBlock, cBlock* pcBlock, const Vector3& pos, float par8, float par10, int par12)
{
	AtlasSprite* sprite = getBlockIconFromSideAndMetadata(pcBlock, 0, par12);

	if (hasOverrideBlockTexture())
	{
		sprite = overrideBlockTexture;
	}


	float var15 = sprite->getMinU();
	float var17 = sprite->getMinV();
	float var19 = sprite->getMaxU();
	float var21 = sprite->getMaxV();
	float var23 = sprite->getInterpolatedU(7.0F / 16.0F);
	float var25 = sprite->getInterpolatedV(6.0F / 16.0F);
	float var27 = sprite->getInterpolatedU(9.0F / 16.0F);
	float var29 = sprite->getInterpolatedV(8.0F / 16.0F);
	float var31 = sprite->getInterpolatedU(7.0F / 16.0F);
	float var33 = sprite->getInterpolatedV(13.0F / 16.0F);
	float var35 = sprite->getInterpolatedU(9.0F / 16.0F);
	float var37 = sprite->getInterpolatedV(15.0F / 16.0F);
	float x = pos.x;
	float z = pos.z;
	float y = pos.y;
	x += 0.5F;
	z += 0.5F;
	float var39 = x - 0.5F;
	float var41 = x + 0.5F;
	float var43 = z - 0.5F;
	float var45 = z + 0.5F;
	float var47 = 0.0625F;
	float var49 = 0.625F;

	int pass = pcBlock->getRenderBlockPass();
	TessRenderable* renderable = specialRenderable ? specialRenderable : sectionRenderer->getRenderable(sprite->getTexture(), (TESS_RENDER_PASS)(pass), 6);
	TessVertexFmt* pVetLockPtr = NULL;
	if (!renderable->getLockPtr(6, pVetLockPtr))
		return;
	
	int color_tl = (colorTopLeft * aoTopLeft).getARGB();
	int color_bl = (colorBottomLeft * aoTopRgith).getARGB();
	int color_br = (colorBottomRight * aoBottomLeft).getARGB();
	int color_tr = (colorTopRight * aoBottomRight).getARGB();

	pVetLockPtr[0].setEx(m_sectionPos, x + par8 * (1.0F - var49) - var47, y + var49, z + par10 * (1.0F - var49) - var47, color_tl, brightnessTopLeft, var23, var25);
	pVetLockPtr[1].setEx(m_sectionPos, x + par8 * (1.0F - var49) - var47, y + var49, z + par10 * (1.0F - var49) + var47, color_bl, brightnessBottomLeft, var23, var29);
	pVetLockPtr[2].setEx(m_sectionPos, x + par8 * (1.0F - var49) + var47, y + var49, z + par10 * (1.0F - var49) + var47, color_br, brightnessBottomRight, var27, var29);
	pVetLockPtr[3].setEx(m_sectionPos, x + par8 * (1.0F - var49) + var47, y + var49, z + par10 * (1.0F - var49) - var47, color_tr, brightnessTopRight, var27, var25);
	//AutoCaculateQuadNormal(pVetLockPtr + 0);

	pVetLockPtr[4].setEx(m_sectionPos, x + var47 + par8, y, z - var47 + par10, color_tl, brightnessTopLeft, var35, var33);
	pVetLockPtr[5].setEx(m_sectionPos, x + var47 + par8, y, z + var47 + par10, color_bl, brightnessBottomLeft, var35, var37);
	pVetLockPtr[6].setEx(m_sectionPos, x - var47 + par8, y, z + var47 + par10, color_br, brightnessBottomRight, var31, var37);
	pVetLockPtr[7].setEx(m_sectionPos, x - var47 + par8, y, z - var47 + par10, color_tr, brightnessTopRight, var31, var33);
	//AutoCaculateQuadNormal(pVetLockPtr + 4);

	pVetLockPtr[8].setEx(m_sectionPos, x - var47, y + 1.0F, var43, color_tl, brightnessTopLeft, var15, var17);
	pVetLockPtr[9].setEx(m_sectionPos, x - var47 + par8, y + 0.0F, var43 + par10, color_bl, brightnessBottomLeft, var15, var21);
	pVetLockPtr[10].setEx(m_sectionPos, x - var47 + par8, y + 0.0F, var45 + par10, color_br, brightnessBottomRight, var19, var21);
	pVetLockPtr[11].setEx(m_sectionPos, x - var47, y + 1.0F, var45, color_tr, brightnessTopRight, var19, var17);
	//AutoCaculateQuadNormal(pVetLockPtr + 8);

	pVetLockPtr[12].setEx(m_sectionPos, x + var47, y + 1.0F, var45, color_tl, brightnessTopLeft, var15, var17);
	pVetLockPtr[13].setEx(m_sectionPos, x + par8 + var47, y + 0.0F, var45 + par10, color_bl, brightnessBottomLeft, var15, var21);
	pVetLockPtr[14].setEx(m_sectionPos, x + par8 + var47, y + 0.0F, var43 + par10, color_br, brightnessBottomRight, var19, var21);
	pVetLockPtr[15].setEx(m_sectionPos, x + var47, y + 1.0F, var43, color_tr, brightnessTopRight, var19, var17);
	//AutoCaculateQuadNormal(pVetLockPtr + 12);

	pVetLockPtr[16].setEx(m_sectionPos, var39, y + 1.0F, z + var47, color_tl, brightnessTopLeft, var15, var17);
	pVetLockPtr[17].setEx(m_sectionPos, var39 + par8, y + 0.0F, z + var47 + par10, color_bl, brightnessBottomLeft, var15, var21);
	pVetLockPtr[18].setEx(m_sectionPos, var41 + par8, y + 0.0F, z + var47 + par10, color_br, brightnessBottomRight, var19, var21);
	pVetLockPtr[19].setEx(m_sectionPos, var41, y + 1.0F, z + var47, color_tr, brightnessTopRight, var19, var17);
	//AutoCaculateQuadNormal(pVetLockPtr + 16);

	pVetLockPtr[20].setEx(m_sectionPos, var41, y + 1.0F, z - var47, color_tl, brightnessTopLeft, var15, var17);
	pVetLockPtr[21].setEx(m_sectionPos, var41 + par8, y + 0.0F, z - var47 + par10, color_bl, brightnessBottomLeft, var15, var21);
	pVetLockPtr[22].setEx(m_sectionPos, var39 + par8, y + 0.0F, z - var47 + par10, color_br, brightnessBottomRight, var19, var21);
	pVetLockPtr[23].setEx(m_sectionPos, var39, y + 1.0F, z - var47, color_tr, brightnessTopRight, var19, var17);
	//AutoCaculateQuadNormal(pVetLockPtr + 20);
}

AtlasSprite* RenderBlocks::getBlockIcon(cBlock* pcBlock, IBlockAccess* pBlockAccess, const BlockPos& pos, int side)
{
	return getIconSafe(pcBlock->getBlockSprite(pBlockAccess, pos, side));
}

AtlasSprite* RenderBlocks::getBlockIconFromSideAndMetadata(cBlock* pcBlock, int side, int metedata)
{
	return getIconSafe(pcBlock->getIcon(side, metedata));
}

AtlasSprite* RenderBlocks::getBlockIconFromSide(cBlock* pcBlock, int side)
{
	return getIconSafe(pcBlock->getSpriteFromSide(side));
}

AtlasSprite* RenderBlocks::getBlockIcon(cBlock* pcBlock)
{
	return getIconSafe(pcBlock->getSpriteFromSide(1));
}

AtlasSprite* RenderBlocks::getIconSafe(AtlasSprite* pSprite)
{
	if (pSprite == NULL)
	{
		pSprite = TextureAtlasRegister::Instance()->getAtlasSprite("empty");
	}

	return pSprite;
}

bool RenderBlocks::renderItemIn3d(int rendertype)
{
	if (rendertype == RENDER_TYPE_TRAPDOOR ||
		rendertype == RENDER_TYPE_HAY ||
		rendertype == RENDER_TYPE_QUARTZ ||
		rendertype == RENDER_TYPE_CACTUS ||
		rendertype == RENDER_TYPE_STAIRS ||
		rendertype == RENDER_TYPE_FENCE ||
		rendertype == RENDER_TYPE_DRAGON_EGG ||
		rendertype == RENDER_TYPE_CHEST ||
		rendertype == RENDER_TYPE_FENCE_GATE ||
		rendertype == RENDER_TYPE_PISTON_BASE ||
		rendertype == RENDER_TYPE_PORTAL_FRAME ||
		rendertype == RENDER_TYPE_WALL ||
		rendertype == RENDER_TYPE_BEACON ||
		rendertype == RENDER_TYPE_ANVIL ||
		rendertype == RENDER_TYPE_AIR ||
		rendertype == RENDER_TYPE_BANNER ||
		rendertype == RENDER_TYPE_TRANSPARENCY)
	{
		return true;
	}

	return false;
}


void RenderBlocks::setOABrightness(int brightness)
{
	brightnessBottomLeft = brightnessBottomRight = brightnessTopLeft = brightnessTopRight = brightness;
}

void RenderBlocks::setOAColor(float r, float g, float b)
{
	colorTopLeft = Color(r, g, b, 1.0f);
	colorBottomLeft = Color(r, g, b, 1.0f);
	colorBottomRight = Color(r, g, b, 1.0f);
	colorTopRight = Color(r, g, b, 1.0f);
}

void RenderBlocks::setColor(float r, float g, float b)
{
	Color c(r, g, b, 1.f);
	m_color = c.getARGB();
}

void RenderBlocks::setColor(const Color& _color)
{
	m_color = _color.getARGB();
}

}
