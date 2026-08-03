#include "RenderGlobal.h"
#include "CloudSerializer.h"
#include "VisualGraph.h"
#include "SectionRenderer.h"
#include "RenderBlocks.h"
#include "TextureAtlas.h"
#include "Rasterizer.h"
#include "VisualTessThread.h"

#include "Block/Blocks.h"
#include "Block/BlockManager.h"
#include "cBlock/cBlock.h"
#include "cBlock/cBlockManager.h"
#include "Entity/EntityItem.h"
#include "World/World.h"
#include "World/WorldProvider.h"
#include "cWorld/Blockman.h"
#include "World/GameSettings.h"
#include "Chunk/Chunk.h"
#include "cWorld/WorldClient.h"
#include "Entity/EntityLiving.h"
#include "Entity/EntityLivingBase.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "World/RayTracyResult.h"
#include "EntityRenders/EntityRenderManager.h"
#include "Item/Item.h"
#include "Item/Items.h"
#include "cItem/cItem.h"
#include "cEffects/EffectRenderer.h"
#include "cEffects/EntityDiggingFx.h"
#include "cEffects/EntitySpellFx.h"
#include "cEffects/EntityLavaFx.h"
#include "cEffects/EntityBubbleFx.h"
#include "cEffects/EntitySmokeFx.h"
#include "cEffects/EntityBreakingFx.h"
#include "cEffects/EntityExplodeFx.h"
#include "cEffects/EntityGunFlameFx.h"
#include "cEffects/EntityChestFx.h"
#include "cTileEntity/TileEntityRenderMgr.h"

#include "Scene/Camera.h"
#include "Tessolator/TessManager.h"
#include "Tessolator/TessObject.h"
#include "Tessolator/TessRenderable.h"
#include "Tessolator/LineRenderable.h"
#include "Audio/SoundSystem.h"
#include "Chunk/ChunkService.h"
#include "Render/TextureAtlas.h"
#include "cChunk/ChunkClient.h"
#include "Render/WallTextRenderer.h"

namespace BLOCKMAN
{

const int RASTER_WIDTH = 64;
const int RASTER_HEIGHT = 48;

LocalRenderInfo::LocalRenderInfo(SectionRenderer* sectionRender, Facing* facing, int counter)
	: m_sectionRender(sectionRender)
	, m_facing(facing)
	, m_setFacing(0)
	, m_counter(counter)
{}

void LocalRenderInfo::setDirection(i8 dir, Facing* facing)
{
	m_setFacing = (i8)(m_setFacing | dir | 1 << facing->getIndex());
}

bool LocalRenderInfo::hasDirection(Facing* facing)
{
	return (m_setFacing & 1 << facing->getIndex()) > 0;
}

RenderGlobal::RenderGlobal(Blockman* pmc)
{
	bm = pmc;
	theWorld = NULL;
	sectionRenderer = NULL;
	sectionRendererLen = 0;
	m_selectFrame = NULL;
	m_selectBlock = NULL;
	m_visual_collisions = NULL;
	m_selectPos = BlockPos::INVALID;
	m_selectID = -1;
	m_selectMeta = -1;

	renderChunksWide = 0;
	renderChunksTall = 0;
	renderChunksDeep = 0;
	minBlockX = 0;
	minBlockY = 0;
	minBlockZ = 0;
	maxBlockX = 0;
	maxBlockY = 0;
	maxBlockZ = 0;
	renderersBeingRendered = 0;
	renderersUpdateCount = 0;
	renderersRasterCullCnt = 0;
	cloudTickCounter = 0;
	globalRenderBlocks = NULL;
    breakRenderable = NULL;
	for (int i = 0; i < DAMAGE_COUNT; ++i)
		destroyBlockIcons[i] = 0;

	worldRenderersCheckIndex = 0;

	countEntitiesTotal = 0;
	countEntitiesRendered = 0;
	countEntitiesHidden = 0;

	prevSort.set(-9999.0, -9999.0, -9999.0);
	frustumUpdatePosChunk.set(Math::MIN_INT, Math::MIN_INT, Math::MIN_INT);

	frustumCheckOffset = 0;
	skyLightCheckOffset = 0;
	initAllRenders();
	initSkyRender();
	initSunRender();
	initDebugRaster();
	initPoisonRender();
	initCloudRender();

	m_rasterizer = LordNew Rasterizer(this, RASTER_WIDTH, RASTER_HEIGHT);
}

RenderGlobal::~RenderGlobal()
{
	LordSafeDelete(m_rasterizer);
#ifdef SHOW_RASTERIZER_MAP
	LordSafeFree(m_rasterSysMem);
	TextureManager::Instance()->releaseResource(m_rasterTex);
	TessManager::Instance()->destroyPostObject(m_rasterObj);
#endif
	TessManager::Instance()->destroyPrevObject(m_skyUpperObj);
	TessManager::Instance()->destroyPrevObject(m_skyUnderObj);
	TessManager::Instance()->destroyPrevObject(m_sunObj);
	TessManager::Instance()->destroyPostObject(m_markObj);
	TessManager::Instance()->destroyPrevObject(m_moonObj);
	TessManager::Instance()->destroyPrevObject(m_cloudObj);
	TessManager::Instance()->destroyPrevObject(m_glowObj);
	TessManager::Instance()->destroyPostObject(m_poisonObj);
	// TessManager::Instance()->destroyLineRenderable(m_selectFrame);
	TextureManager::Instance()->releaseResource(m_selectBlock->getTexture());
	TessManager::Instance()->destroyTessRenderable(m_selectBlock);
	TessManager::Instance()->destroyLineRenderable(m_visual_collisions);
	SceneManager::Instance()->getRootNode()->destroyChild(m_skyUpperNode);
	SceneManager::Instance()->getRootNode()->destroyChild(m_skyUnderNode);
	SceneManager::Instance()->getRootNode()->destroyChild(m_sunNode);
	SceneManager::Instance()->getRootNode()->destroyChild(m_markNode);
	SceneManager::Instance()->getRootNode()->destroyChild(m_rasterNode);
	SceneManager::Instance()->getRootNode()->destroyChild(m_moonNode);
	SceneManager::Instance()->getRootNode()->destroyChild(m_cloudNode);
	SceneManager::Instance()->getRootNode()->destroyChild(m_glowNode);
	SceneManager::Instance()->getRootNode()->destroyChild(m_poisonNode);
    if (breakRenderable) {
		TessManager::Instance()->destroyTessRenderable(breakRenderable);
    }

	LordSafeDelete(m_cloudSerializer);

	SectionRenderer* pSectionRender = NULL;
	for (int i = 0; i < sectionRendererLen; ++i)
	{
		pSectionRender = sectionRenderer[i];
		if (!pSectionRender)
			continue;
		LordSafeDelete(pSectionRender);
	}
	LordSafeFree(sectionRenderer);
	sectionRenderersToUpdate.clear();
	LordSafeDelete(globalRenderBlocks);

	for (auto it : damagedBlocks)
	{
		LordSafeDelete (it.second);
	}

	for (auto it : m_visualHelps)
	{
		TessManager::Instance()->destroyLineRenderable(it);
	}
}

void RenderGlobal::initAllRenders()
{
	// as the render sort to create all renders.
	m_skyUpperObj = TessManager::Instance()->createTessObject_Sky();
	m_glowObj = TessManager::Instance()->createTessObject_Glow();
	m_sunObj = TessManager::Instance()->createTessObject_Sun();
	m_moonObj = TessManager::Instance()->createTessObject_Sun();
	m_markObj = TessManager::Instance()->createTessObject_Mark();  // post render, not prev render.
	m_skyUnderObj = TessManager::Instance()->createTessObject_Sky();
	m_cloudObj = TessManager::Instance()->createTessObject_Cloud();
	m_poisonObj = TessManager::Instance()->createTessObject_PoisionCircle();
#ifdef SHOW_RASTERIZER_MAP
	m_rasterObj = TessManager::Instance()->createTessObject_Raster();
#endif
	// m_selectFrame = TessManager::Instance()->createLineRenderable(LRP_SELECTED);
	Texture* blockTex = TextureManager::Instance()->createTexture("Blocks.png");
	m_selectBlock = TessManager::Instance()->createTessRenderable(blockTex, TRP_TRANSPARENT_HL);
	m_visual_collisions = TessManager::Instance()->createLineRenderable(LRP_VIRSUALHELP);
}

void RenderGlobal::initCloudRender()
{
	SceneManager* sceneMgr = SceneManager::Instance();
	m_cloudSerializer = LordNew CloudSerializer("clouds.png");
	m_cloudSerializer->setLoadSize(Vector2i(64, 64));
	m_cloudSerializer->setCloudSpeed(0.5f);
	m_cloudSerializer->setCloudShift(Vector2(0.0f, 0.0f));
		
	m_cloudNode = sceneMgr->getRootNode()->createChild();
	m_cloudNode->setLocalScaling(Vector3(12.f, 1.f, 12.f));
	m_cloudObj->attachTo(m_cloudNode);
}

void RenderGlobal::initPoisonRender()
{
	SceneManager* sceneMgr = SceneManager::Instance();

	TessObject_PoisonCircle* pPoisonObj = dynamic_cast<TessObject_PoisonCircle*>(m_poisonObj);
	pPoisonObj->setTexture("poisionCircle.png");
	
	pPoisonObj->setHeight(16.f, 128.f);
	pPoisonObj->setUVLength(4.f);
	pPoisonObj->setUVAnimation({ 0.02f, 0.02f });

	// call setPoints and reflesh every frame in game play.
	Vector2 points[4] = { { 0.f,0.f },{ 0.f,0.f },{ 0.f,0.f },{ 0.f,0.f } };
	pPoisonObj->setPoints(points);
	pPoisonObj->reflesh(0.f);

	// no need to move poisonNode
	m_poisonNode = sceneMgr->getRootNode()->createChild();
	pPoisonObj->attachTo(m_poisonNode);
}

void RenderGlobal::initDebugRaster()
{
#ifdef SHOW_RASTERIZER_MAP
	SceneManager* sceneMgr = SceneManager::Instance();
	TessObject_Sun* prasterObj = dynamic_cast<TessObject_Sun*>(m_rasterObj);
	
	int nBufferSize = RASTER_WIDTH * RASTER_HEIGHT * sizeof(int);
	m_rasterSysMem = (ui32*)LordMalloc(nBufferSize);
	memset(m_rasterSysMem, 1, nBufferSize);
	Buffer buff(nBufferSize, m_rasterSysMem);

	m_rasterTex = TextureManager::Instance()->createManual("rasterTex", Texture::TT_2D, PF_RGBA8_UNORM,
		Texture::TU_STATIC, RASTER_WIDTH, RASTER_HEIGHT, 1, 1, buff);

	prasterObj->setTexturePtr(m_rasterTex);
	
	prasterObj->beginPolygon();
	prasterObj->setQuadPos(Vector3(-6.4f, 4.8f, 0.f), Vector3(-6.4f, -4.8f, 0.f), Vector3(6.4f, -4.8f, 0.f), Vector3(6.4f, 4.8f, 0.f));
	prasterObj->setQuadUV(Vector2(0.f, 1.f), Vector2(0.f, 0.f), Vector2(1.f, 0.f), Vector2(1.f, 1.f));
	prasterObj->endPolygon();
	m_rasterNode = sceneMgr->getRootNode()->createChild();
	prasterObj->attachTo(m_rasterNode);
#endif
}

void RenderGlobal::initSunRender()
{
	SceneManager* sceneMgr = SceneManager::Instance();

	TessObject_Sun* psunObj = dynamic_cast<TessObject_Sun*>(m_sunObj);
	psunObj->setTexture("sun.png");
	float x = 30.f;
	float y = 100.f;
	psunObj->beginPolygon();
	psunObj->setQuadPos(Vector3(-x, y, -x), Vector3(x, y, -x), Vector3(x, y, x), Vector3(-x, y, x));
	psunObj->setQuadUV(Vector2(0.f, 0.f), Vector2(1.f, 0.f), Vector2(1.f, 1.f), Vector2(0.f, 1.f));
	psunObj->endPolygon();

	x = 20.f;
	int phase = 0;
	int ubase = phase % 4;
	int vbase = phase / 4 % 2;
	float u_min = (float)(ubase + 0) / 4.0F;
	float v_min = (float)(vbase + 0) / 2.0F;
	float u_max = (float)(ubase + 1) / 4.0F;
	float v_max = (float)(vbase + 1) / 2.0F;
	TessObject_Sun* pmoonObj = dynamic_cast<TessObject_Sun*>(m_moonObj);
	pmoonObj->setTexture("moon_phases.png");
	pmoonObj->beginPolygon();
	pmoonObj->setQuadPos(Vector3(-x, -y, x), Vector3(x, -y, x), Vector3(x, -y, -x), Vector3(-x, -y, -x));
	pmoonObj->setQuadUV(Vector2(u_max, v_max), Vector2(u_min, v_max), Vector2(u_min, v_min), Vector2(u_max, v_min));
	pmoonObj->endPolygon();

	TessObject_Sun* pmarkObj = dynamic_cast<TessObject_Sun*>(m_markObj);
	pmarkObj->setTexture2("mask.png");  // change your mark texture!
	pmarkObj->setColor(Color(1.f, 0.5f, 0.3f, 0.3f));
	pmarkObj->beginPolygon();
	pmarkObj->setQuadPos(Vector3(0, 0, 0), Vector3(1, 0, 0), Vector3(1, 0, 1), Vector3(0, 0, 1));
	pmarkObj->setQuadUV(Vector2(0.f, 0.f), Vector2(1.f, 0.f), Vector2(1.f, 1.f), Vector2(0.f, 1.f));
	pmarkObj->endPolygon();

	m_glowNode = sceneMgr->getRootNode()->createChild();
	m_glowObj->attachTo(m_glowNode);

	m_sunNode = sceneMgr->getRootNode()->createChild();
	m_sunObj->attachTo(m_sunNode);

	m_moonNode = sceneMgr->getRootNode()->createChild();
	m_moonObj->attachTo(m_moonNode);

	m_markNode = sceneMgr->getRootNode()->createChild();
	m_markObj->attachTo(m_markNode);
	m_markNode->setVisible(false); // default 
}

void RenderGlobal::enableMarkRender(bool enble)
{
	m_markNode->setVisible(enble);
	//LordLogInfo("=======> enableMarkRender [%d]", enble ? 1 : 0);
}

void RenderGlobal::setMarkRenderArea(Vector3 beginPos, Vector2 size, Color color)
{
	m_markNode->setLocalPosition(beginPos + Vector3(0,0.01f,0));
	m_markNode->setLocalScaling(Vector3(size.x, 1, size.y));
	m_markNode->update();
	m_markObj->setColor(color);
	//LordLogInfo("=======> setMarkRenderArea pos[%.1f, %.1f, %.1f] scale[%.1f, 1.0, %.1f]", beginPos.x, beginPos.y, beginPos.z, size.x, size.y);
}

void RenderGlobal::initSkyRender()
{
	SceneManager* sceneMgr = SceneManager::Instance();

	m_skyUpperObj->beginPolygon();
	int segWidth = 64;
	int segCount = 256 / segWidth + 2;
	float height = 16.0F;
	int i;
	int j;
	for (i = -segWidth * segCount; i <= segWidth * segCount; i += segWidth)
	{
		for (j = -segWidth * segCount; j <= segWidth * segCount; j += segWidth)
		{
			m_skyUpperObj->setQuadPos(
				Vector3((float)(i + 0), height, (float)(j + 0)),
				Vector3((float)(i + segWidth), height, (float)(j + 0)),
				Vector3((float)(i + segWidth), height, (float)(j + segWidth)),
				Vector3((float)(i + 0), height, (float)(j + segWidth)));
		}
	}
	m_skyUpperObj->endPolygon();
	m_skyUpperObj->setTransparent(true);
	m_skyUpperObj->setTestDepth(false);
	m_skyUpperObj->setWriteDepth(false);
	m_skyUpperNode = sceneMgr->getRootNode()->createChild();
	m_skyUpperObj->attachTo(m_skyUpperNode);

	m_skyUnderObj->beginPolygon();
	height = -16.0F;
	for (i = -segWidth * segCount; i <= segWidth * segCount; i += segWidth)
	{
		for (j = -segWidth * segCount; j <= segWidth * segCount; j += segWidth)
		{
			m_skyUnderObj->setQuadPos(
				Vector3((float)(i + segWidth), height, (float)(j + 0)),
				Vector3((float)(i + 0), height, (float)(j + 0)),
				Vector3((float)(i + 0), height, (float)(j + segWidth)),
				Vector3((float)(i + segWidth), height, (float)(j + segWidth)));
		}
	}
	m_skyUnderObj->endPolygon();
	m_skyUnderObj->setTransparent(true);
	m_skyUnderObj->setTestDepth(false);
	m_skyUnderObj->setWriteDepth(false);
	m_skyUnderNode = sceneMgr->getRootNode()->createChild();
	m_skyUnderObj->attachTo(m_skyUnderNode);
}

void RenderGlobal::setWorldAndLoadRenderers(World* pWorld)
{
	if (theWorld != NULL)
	{
		// theWorld->removeWorldAccess(this);
	}

	prevSort.set(-9999.0, -9999.0, -9999.0);
	theWorld = pWorld;
	globalRenderBlocks = LordNew RenderBlocks(pWorld);
	Texture* blockTex = (Texture*)(TextureManager::Instance()->getByName("Blocks.png"));
	LordAssert(blockTex);
	breakRenderable = TessManager::Instance()->createTessRenderable(blockTex, TRP_TRANSPARENT_2D);
	globalRenderBlocks->setSpecialRenderable(breakRenderable);

	if (pWorld != NULL)
	{
		pWorld->addWorldListener(this);
		loadRenderers();
	}
}

void RenderGlobal::loadRenderers()
{
	if (!theWorld)
		return;

	cBlockLeaves* leaves = (cBlockLeaves*)BlockManager::leaves;
	leaves->setOpaque(bm->m_gameSettings->fancyGraphics);
	renderChunksWide = renderChunksDeep = bm->m_gameSettings->getRenderChunksRange();
	renderChunksTall = 16;

	sectionRendererLen = renderChunksWide *  renderChunksTall * renderChunksDeep;
	sectionRenderer = (SectionRenderer**)LordMalloc(sizeof(SectionRenderer*) * sectionRendererLen);
	memset(sectionRenderer, 0, sizeof(SectionRenderer*) * sectionRendererLen);

	int var2 = 0;
	int var3 = 0;
	minBlockX = 0;
	minBlockY = 0;
	minBlockZ = 0;
	maxBlockX = renderChunksWide;
	maxBlockY = renderChunksTall;
	maxBlockZ = renderChunksDeep;
	int x;
	SectionRenderer* pSectionRender = NULL;

	for (x = 0; x < renderChunksWide; ++x)
	{
		for (int y = 0; y < renderChunksTall; ++y)
		{
			for (int z = 0; z < renderChunksDeep; ++z)
			{
				int idx = (z * renderChunksTall + y) * renderChunksWide + x;
				pSectionRender = LordNew SectionRenderer(theWorld, m_tileEntities, BlockPos(x, y, z) * 16);

				pSectionRender->chunkIndex = var3++;
				pSectionRender->markDirty();

				sectionRenderer[idx] = pSectionRender;
				var2 += 3;
			}
		}
	}

	EntityLivingBase* pViewer = bm->renderViewEntity;
	if (pViewer)
	{
		markRenderersForNewPosition(pViewer->getPosition().getFloor());
	}
}

void RenderGlobal::markRenderersForNewPosition(const BlockPos& pos)
{
	BlockPos pos1 = pos - 8;
	minBlockX = Math::MAX_INT;
	minBlockY = Math::MAX_INT;
	minBlockZ = Math::MAX_INT;
	maxBlockX = Math::MIN_INT;
	maxBlockY = Math::MIN_INT;
	maxBlockZ = Math::MIN_INT;
	int renderBlocksWide = renderChunksWide * 16;
	int renderBlocksHalfWide = renderBlocksWide / 2;

	for (int chunkX = 0; chunkX < renderChunksWide; ++chunkX)
	{
		int worldX = chunkX * 16;
		int var8 = worldX + renderBlocksHalfWide - pos1.x;

		if (var8 < 0)
			var8 -= renderBlocksWide - 1;

		var8 /= renderBlocksWide;
		worldX -= var8 * renderBlocksWide;

		if (worldX < minBlockX)
			minBlockX = worldX;

		if (worldX > maxBlockX)
			maxBlockX = worldX;

		for (int chunkZ = 0; chunkZ < renderChunksDeep; ++chunkZ)
		{
			int worldZ = chunkZ * 16;
			int var11 = worldZ + renderBlocksHalfWide - pos1.z;

			if (var11 < 0)
				var11 -= renderBlocksWide - 1;

			var11 /= renderBlocksWide;
			worldZ -= var11 * renderBlocksWide;

			if (worldZ < minBlockZ)
				minBlockZ = worldZ;

			if (worldZ > maxBlockZ)
				maxBlockZ = worldZ;

			for (int chunkY = 0; chunkY < renderChunksTall; ++chunkY)
			{
				int worldY = chunkY * 16;

				if (worldY < minBlockY)
					minBlockY = worldY;

				if (worldY > maxBlockY)
					maxBlockY = worldY;

				SectionRenderer* pSectionRender = sectionRenderer[(chunkZ * renderChunksTall + chunkY) * renderChunksWide + chunkX];
				pSectionRender->setPosition(BlockPos(worldX, worldY, worldZ));
			}
		}
	}
}

void RenderGlobal::setupTerrain(float rdt)
{
	m_lightCalFrame = 1;
	frameIndex++;
	renderersBeingRendered = 0;
	renderersRasterCullCnt = 0;
	m_mainCamera = SceneManager::Instance()->getMainCamera();

	// rasterizer
	m_rasterizer->ClearBuffer();
	m_rasterizer->SetViewProject(m_mainCamera->getViewProjMatrix());
	m_rasterizer->SetNearFar(m_mainCamera->getNearClip(), m_mainCamera->getFarClip());

	markRenderersForNewPositionIfUpdated();
	EntityLivingBase* viewer = bm->renderViewEntity;
	Vector3 currentPos = viewer->getPosition(rdt);
	Vector3i blockpos(int(Math::Floor(currentPos.x)), int(Math::Floor(currentPos.y + viewer->getEyeHeight())), int(Math::Floor(currentPos.z)));
	//reCheckNearby(blockpos);

	sectionRenderersToRender.clear();
	// from the camera position find all the visible section by  breadth-first search (BFS)
	LocalRenderInfoQueue queue;
	SectionRenderer* pBeginSection = getSectionRender(blockpos);
	if (pBeginSection)
	{
		LocalRenderInfo info(pBeginSection, NULL, 0);
		queue.push_back(info);
	}
	else
	{
		int y = viewer->position.y >= 1.0f ? 248 : 8;
		int halfRenderRangeWide = renderChunksWide / 2;
		int halfRenderRangeDeep = renderChunksDeep / 2;
		for (int x = -halfRenderRangeWide; x <= halfRenderRangeWide; ++x)
		{
			for (int z = -halfRenderRangeDeep; z <= halfRenderRangeDeep; ++z)
			{
				SectionRenderer* pRenderChunk = getSectionRender(Vector3i((x << 4) + 8, y, (z << 4) + 8));
				if (pRenderChunk != NULL && m_mainCamera->getFrustum().intersect(pRenderChunk->rendererBoundingBox))
				{
					queue.push_back(LocalRenderInfo(pRenderChunk, NULL, 0));
				}
			}
		}
	}

	while (!queue.empty())
	{
		LocalRenderInfo info = queue.front();
		queue.pop_front();
		SectionRenderer* pSectionRender = info.m_sectionRender;
		Facing* facing = info.m_facing;
		sectionRenderersToRender.push_back(pSectionRender);
		SetVisibility* pSetVisibility = pSectionRender->getSectionVisibility();
		
		if (info.m_counter <= 3)
		{
			m_rasterizer->PerformRaterization(pSectionRender->blockPos / 16);

#ifdef SHOW_RASTERIZER_RENDER
			RasterizerRenderable* showRaster = pSectionRender->getRasterRenderable();
			if(showRaster)
				TessManager::Instance()->addRasterizerRenderable(showRaster);
#endif
		}

		if (!pSetVisibility)
		{
			pSetVisibility = SetVisibility::s_Empty;
		}

		for (int i = 0; i < 6; ++i)
		{
			Facing* facingIter = Facing::VALUES[i];
			SectionRenderer* pDetectSection = getRenderChunkOffset(blockpos, pSectionRender, facingIter);
			if(!pDetectSection)
				continue;

			Vector3i sectionID = pDetectSection->blockPos / 16;
			if ((!info.hasDirection(facingIter->getOpposite())) &&
				(facing == NULL || (pSetVisibility->getVisibility(facing->getOpposite(), facingIter))) &&
				pDetectSection != NULL && pDetectSection->setFrameIndex(frameIndex) &&
				m_mainCamera->getFrustum().intersect(pDetectSection->rendererBoundingBox))
			{
				if (info.m_counter > 1)
				{
					if (!m_rasterizer->TestAABBVisible(sectionID))
					{
						renderersRasterCullCnt++;
						continue;
					}
				}

				LocalRenderInfo detect(pDetectSection, facingIter, info.m_counter + 1);
				detect.setDirection(info.m_setFacing, facingIter);
				queue.push_back(detect);
			}
		}
	}

	sectionRenderersToUpdate.clear();

	for (SectionRenderList::iterator it = sectionRenderersToRender.begin(); it != sectionRenderersToRender.end(); ++it)
	{
		SectionRenderer* pSectionRender = *it;

		pSectionRender->checkRebuild();

		if (pSectionRender->needsUpdate)
		{
			bool isNear = (pSectionRender->blockPos + 8 - blockpos).lenSqr() < 768;

			if (!isNear)
			{
				sectionRenderersToUpdate.push_back(pSectionRender);
			}
			else
			{
				if (pSectionRender->updateRenderer())
				{
					++renderersUpdateCount;
					pSectionRender->needsUpdate = false;
				}
			}
		}
	}
}

void RenderGlobal::updateChunks()
{
	theWorld->getChunkService()->cancelAsyncGets();
	for (SectionRenderList::iterator it = sectionRenderersToUpdate.begin(); it != sectionRenderersToUpdate.end(); ++it)
	{
		SectionRenderer* pSectionRender = *it;
		
		if (pSectionRender->updateRenderer())
		{
			++renderersUpdateCount;
			pSectionRender->needsUpdate = false;
			break;
		}	
	}
}

void RenderGlobal::updateRenders(const Vector3& eyePos)
{
	for (SectionRenderList::iterator it = sectionRenderersToRender.begin(); it != sectionRenderersToRender.end(); ++it)
	{
		SectionRenderer* pSectionRender = *it;
		
		pSectionRender->exchangeRenderables();
		++renderersBeingRendered;
		pSectionRender->render(eyePos);
	}
}

void RenderGlobal::markBlocksForUpdate(const BlockPos& imin, const BlockPos& imax)
{
	markBlocksForUpdate(imin.x, imin.y, imin.z, imax.x, imax.y, imax.z);
}

void RenderGlobal::markBlocksForUpdate(int minX, int minY, int minZ, int maxX, int maxY, int maxZ)
{
	if(!sectionRenderer)
    {
        LordLogError("RenderGlobal::markBlocksForUpdate sectionRenderer is null");
		return;
	}

	int min_x = Math::bucketInt(minX, 16);
	int min_y = Math::bucketInt(minY, 16);
	int min_z = Math::bucketInt(minZ, 16);
	int max_x = Math::bucketInt(maxX, 16);
	int max_y = Math::bucketInt(maxY, 16);
	int max_z = Math::bucketInt(maxZ, 16);

	for (int chunk_x = min_x; chunk_x <= max_x; ++chunk_x)
	{
		int var14 = chunk_x % renderChunksWide;

		if (var14 < 0)
		{
			var14 += renderChunksWide;
		}

		for (int chunk_y = min_y; chunk_y <= max_y; ++chunk_y)
		{
			int var16 = chunk_y % renderChunksTall;

			if (var16 < 0)
			{
				var16 += renderChunksTall;
			}

			for (int chunk_z = min_z; chunk_z <= max_z; ++chunk_z)
			{
				int var18 = chunk_z % renderChunksDeep;

				if (var18 < 0)
				{
					var18 += renderChunksDeep;
				}

				int idx = (var18 * renderChunksTall + var16) * renderChunksWide + var14;
				SectionRenderer* pSectionRender = sectionRenderer[idx];
				pSectionRender->needsUpdate = true;
			}
		}
	}
}

SectionRenderer* RenderGlobal::getSectionRender(const BlockPos& pos)
{
	int x = pos.x >> 4;
	int y = pos.y >> 4;
	int z = pos.z >> 4;

	if (y < 0 || y >= renderChunksTall)
	{
		return nullptr;
	}

	x = x % renderChunksWide;

	if (x < 0)
	{
		x += renderChunksWide;
	}

	z = z % renderChunksDeep;

	if (z < 0)
	{
		z += renderChunksDeep;
	}

	int idx = (z * renderChunksTall + y) * renderChunksWide + x;
	return sectionRenderer[idx];
}

SectionRenderer* RenderGlobal::getRenderChunkOffset(const Vector3i& camPos, SectionRenderer* from, Facing* facing)
{
	Vector3i blockPos;
	blockPos = from->blockPos + facing->getFrontOffset() * 16;

	if (Math::Abs(camPos.x - blockPos.x) > renderChunksWide * 16)
		return NULL;
	if (blockPos.y < 0 || blockPos.y >= 256)
		return NULL;
	if (Math::Abs(camPos.z - blockPos.z) > renderChunksDeep * 16)
		return NULL;

	return getSectionRender(blockPos);
}

FacingArr RenderGlobal::getVisibleFacing(const Vector3i& pos)
{
	VisualGraph visualGraph;

	ChunkPtr pChunk = theWorld->getChunkFromBlockCoords(pos.x, pos.z);
	LordAssert(pChunk);
	int yBegin = (pos.y >> 4) << 4;
	int blockID = 0;
	Block** pBlocks = BlockManager::sBlocks;
	for (int x = 0; x < 16; ++x)
	{
		for (int z = 0; z < 16; ++z)
		{
			for (int y = 0; y < 16; ++y)
			{
				BlockPos pos1(x, yBegin + y, z);
				blockID = pChunk->getBlockID(pos1);
				if (blockID > 0 && pBlocks[blockID]->isOpaqueCube())
				{
					visualGraph.setOpaque(BlockPos(x, y ,z));
				}
			}
		}
	}
	return visualGraph.getVisibleFacings(BlockPos(pos.x & 15, pos.y & 15, pos.z & 15));
}

void RenderGlobal::renderSky(float rdt)
{
	Color sky_color = ((WorldClient*)theWorld)->getSkyColor(bm->renderViewEntity, rdt);
	float skycolor_r = sky_color.r;
	float skycolor_g = sky_color.g;
	float skycolor_b = sky_color.b;

	// float* sunsetColors = theWorld->provider->calcSunriseSunsetColors(theWorld->getCelestialAngle(dtTime), dtTime);
	Vector3 playerPos = bm->m_pPlayer->getPosition(rdt);
	
	TessObject_Sky* pUpperSky = dynamic_cast<TessObject_Sky*>(m_skyUpperObj);
	pUpperSky->setColor(sky_color);
	m_skyUpperNode->setLocalPosition(playerPos);
	m_skyUpperNode->update();

	float horizonY = playerPos.y - theWorld->getHorizon();
	playerPos.y = playerPos.y - (horizonY - 16.f);
	if (playerPos.y > -1.f)
		playerPos.y = -1.f;
	TessObject_Sky* pUnderSky = dynamic_cast<TessObject_Sky*>(m_skyUnderObj);
	pUnderSky->setColor(Color(skycolor_r * 0.2F + 0.04F, skycolor_g * 0.2F + 0.04F, skycolor_b * 0.6F + 0.1F));
	m_skyUnderNode->setLocalPosition(playerPos);
	m_skyUnderNode->update();
}

void RenderGlobal::renderDebugRaster()
{
#ifdef SHOW_RASTERIZER_MAP
	if (m_mainCamera == nullptr)
		return;
	// first update the texture.
	ui8* rasterBuf = m_rasterizer->m_depthBuffer;
	ui8* texBuf = (ui8*)m_rasterSysMem;
	for (int i = 0; i < RASTER_HEIGHT * RASTER_WIDTH; ++i)
	{
			texBuf[i * 4 + 0] = rasterBuf[i];
			texBuf[i * 4 + 1] = rasterBuf[i];
			texBuf[i * 4 + 2] = rasterBuf[i];
			texBuf[i * 4 + 3] = rasterBuf[i];
	}
	ERect rt(0, 0, RASTER_WIDTH, RASTER_HEIGHT);
	m_rasterTex->updateSubTex2D(0, rt, m_rasterSysMem);

	Vector3 pos = m_mainCamera->getPosition() + m_mainCamera->getDirection() * 16.5f + m_mainCamera->getRight()*12.f;
	m_rasterNode->setLocalPosition(pos);
	m_rasterNode->setLocalOrientation(m_mainCamera->getOritation());
	m_rasterNode->update();
#endif
}

void RenderGlobal::renderSun(float rdt)
{
	Vector3 playerPos = bm->m_pPlayer->getPosition(rdt);
	float celestial = theWorld->getCelestialAngle(rdt);

	Quaternion rotateY(Vector3::UNIT_Y, -Math::PI_DIV3);
	Quaternion rotateX(Vector3::UNIT_X, celestial * Math::PI_2);
	Quaternion orientation = rotateY * rotateX;
	m_sunNode->setLocalOrientation(orientation);
	m_sunNode->setLocalPosition(playerPos);
	m_sunNode->update();

	m_markNode->update();

	m_moonNode->setLocalOrientation(orientation);
	m_moonNode->setLocalPosition(playerPos);
	m_moonNode->update();
}

void RenderGlobal::renderPoisonCircle(float rdt, i64 currentTime)
{
	TessObject_PoisonCircle* pPoisonObj = dynamic_cast<TessObject_PoisonCircle*>(m_poisonObj);
	pPoisonObj->onUpdate(rdt, currentTime);
}

void RenderGlobal::renderGlow(float rdt)
{
	Vector3 playerPos = bm->m_pPlayer->getPosition(rdt);
	float celestial = theWorld->getCelestialAngle(rdt);

	float* sunRise_sunSet = theWorld->m_provider->calcSunriseSunsetColors(celestial, rdt);
	if (!sunRise_sunSet)
	{
		m_glowNode->setVisible(false);
		return;
	}

	m_glowNode->setVisible(true);

	Quaternion rotateX(Vector3::UNIT_X, Math::PI_DIV2);
	Quaternion rotateY(Vector3::UNIT_Z, (Math::Sin(theWorld->getCelestialAngleRadians(rdt)) < 0.f) ? Math::PI : 0);
	Quaternion rotateZ(Vector3::UNIT_Z, Math::PI_DIV2);
	Quaternion orientation = rotateX* rotateY * rotateZ;
	m_glowNode->setLocalOrientation(orientation);
	m_glowNode->setLocalPosition(playerPos);
	m_glowNode->update();

	ui32 colorCenter = Color(sunRise_sunSet).getABGR();
	ui32 colorEdge = Color(sunRise_sunSet[0], sunRise_sunSet[1], sunRise_sunSet[2], 0.f).getABGR();

	TessObject_Glow* glowObj = dynamic_cast<TessObject_Glow*>(m_glowObj);
	glowObj->beginPolygon();
	glowObj->addVertex(0.f, 100.f, 0.f, colorCenter);
	int segment = 16;
	float angle = 0.f;
	for (int i = 0; i <= segment; ++i)
	{
		angle = (float)i * (float)Math::PI * 2.0F / (float)segment;
		float x = Math::Sin(angle);
		float z = Math::Cos(angle);
		glowObj->addVertex(x * 120.0F, z * 120.0F, -z * 40.0F * sunRise_sunSet[3], colorEdge);
	}
	glowObj->endPolygon();
}

void RenderGlobal::updateClouds()
{
	++cloudTickCounter;
}

void RenderGlobal::renderClouds(float dtTime)
{
	Camera* pCamera = SceneManager::Instance()->getMainCamera();
	Vector3 curPos = pCamera->getPosition();

	static bool isFirst = true;

	if (isFirst)
	{
		m_cloudSerializer->setLoadCoordinate(Vector2i((int)curPos.x, (int)curPos.z));
		m_cloudSerializer->setCameraPrevPosition(pCamera->getPosition());
	}

	Vector2 shift = m_cloudSerializer->getCloudShift();
	Vector2i coordinate = m_cloudSerializer->getLoadCoordinate();
	Vector3 prevPos = m_cloudSerializer->getCameraPrevPosition();

	float autoShiftSpeed = m_cloudSerializer->getCloudSpeed() * dtTime;
	shift += Vector2(autoShiftSpeed, autoShiftSpeed);
	shift.x -= curPos.x - prevPos.x;
	shift.y -= curPos.z - prevPos.z;
	m_cloudSerializer->setCameraPrevPosition(curPos);
	

	bool isChanged = false;

	const float length = m_cloudNode->getLocalScaling().x;
	if (shift.x > length) { shift.x -= length; --coordinate.x; isChanged = true; }
	if (shift.x < -length) { shift.x += length; ++coordinate.x; isChanged = true; }
	if (shift.y > length) { shift.y -= length; --coordinate.y; isChanged = true; }
	if (shift.y < -length) { shift.y += length; ++coordinate.y; isChanged = true; }
	m_cloudSerializer->setCloudShift(shift);

	Color coloudColor = ((WorldClient*)theWorld)->getCloudColour(0.f);
	TessObject_Cloud* pCloudObj = dynamic_cast<TessObject_Cloud*>(m_cloudObj);
	pCloudObj->setAmbient(coloudColor);

	m_cloudNode->setLocalPosition(Vector3(curPos.x + shift.x, m_cloudNode->getLocalPosition().y, curPos.z + shift.y));
	m_cloudNode->update();
	
	if (isChanged || isFirst)
	{
		const int cloudWidth = m_cloudSerializer->getLoadSize().x;
		isFirst = false;

		m_cloudSerializer->setLoadCoordinate(coordinate);
		m_cloudSerializer->generate();

		//fill vertex quad to cloudObj.	
		pCloudObj->beginPolygon();

		const float widthPerPixel = 2.f;
		const int width = m_cloudSerializer->getWidth();
		const int height = m_cloudSerializer->getHeight();
		const float offsetX = -cloudWidth / 2 * widthPerPixel;
		const float offsetZ = -cloudWidth / 2 * widthPerPixel;
		const float heightTop = 128.f + 4.f;
		const float heightBtm = 128.f;

		ui32 colorButtom = (Color::WHITE * 0.9f).getABGR();
		ui32 colorTop = (Color::WHITE * 1.f).getABGR();
		ui32 colorSide = (Color::WHITE * 0.6f).getABGR();

		const CloudSerializer::CloudQuadArr& quadArr = m_cloudSerializer->getQuads();
		for (CloudSerializer::CloudQuadArr::const_iterator it = quadArr.begin(); it != quadArr.end(); ++it)
		{
			const Vector4i& quad = *it;
			float x1 = quad.x * widthPerPixel + offsetX;
			float z1 = quad.y * widthPerPixel + offsetZ;
			float x2 = (quad.x + quad.z) * widthPerPixel + offsetX;
			float z2 = (quad.y + quad.w) * widthPerPixel + offsetZ;

			pCloudObj->setQuadPos(
				Vector3(x1, heightBtm, z1), 
				Vector3(x2, heightBtm, z1),
				Vector3(x2, heightBtm, z2),
				Vector3(x1, heightBtm, z2));
				
			//pCloudObj->setQuadColor(colorButtom, colorButtom, colorButtom, colorButtom);
			pCloudObj->setQuadColor(colorSide, colorSide, colorSide, colorSide);
			
			pCloudObj->setQuadPos(
				Vector3(x1, heightTop, z1),
				Vector3(x1, heightTop, z2),
				Vector3(x2, heightTop, z2),
				Vector3(x2, heightTop, z1));
			pCloudObj->setQuadColor(colorTop, colorTop, colorTop, colorTop);

		}

		const CloudSerializer::ClosureEdges& edges = m_cloudSerializer->getEdges();
		for (CloudSerializer::ClosureEdges::const_iterator it = edges.begin(); it != edges.end(); ++it)
		{
			const Vector4i& edge = *it;

			float x1 = edge.x * widthPerPixel + offsetX;
			float z1 = edge.y * widthPerPixel + offsetZ;
			float x2 = edge.z * widthPerPixel + offsetX;
			float z2 = edge.w * widthPerPixel + offsetZ;

			pCloudObj->setQuadPos(
				Vector3(x1, heightTop, z1),
				Vector3(x1, heightBtm, z1),
				Vector3(x2, heightBtm, z2),
				Vector3(x2, heightTop, z2));
			//pCloudObj->setQuadColor(colorSide, colorSide, colorSide, colorSide);			
			pCloudObj->setQuadColor(colorButtom, colorButtom, colorButtom, colorButtom);
		}

		pCloudObj->endPolygon();
	}
}

void RenderGlobal::destroyBlockPartially(int entiyID, const BlockPos& pos, int damage)
{
	if (damage >= 0 && damage < 10)
	{
		DestroyBlockProgress* pDamageBlock = NULL;
		DamageBlocks::iterator it = damagedBlocks.find(entiyID);
		if (it != damagedBlocks.end())
			pDamageBlock = it->second;
		if (pDamageBlock == NULL)
		{
			pDamageBlock = LordNew DestroyBlockProgress(entiyID, pos);
			damagedBlocks.insert(std::make_pair(entiyID, pDamageBlock));
		}
		
		pDamageBlock->setBlockPos(pos);
		pDamageBlock->setPartialBlockDamage(damage);
		pDamageBlock->setCloudUpdateTick(cloudTickCounter);
	}
	else
	{
		// delete the DestroyBlockProgress
		DamageBlocks::iterator it = damagedBlocks.find(entiyID);
		if (it != damagedBlocks.end())
		{
			LordSafeDelete(it->second);
			damagedBlocks.erase(entiyID);
		}
	}
}

void RenderGlobal::registerDestroyBlockIcons(TextureAtlasRegister* atlas)
{
	for (int i = 0; i < DAMAGE_COUNT; ++i)
	{
		destroyBlockIcons[i] = atlas->getAtlasSprite("destroy_stage_" + StringUtil::ToString(i));
	}
}

void RenderGlobal::drawBlockDamageTexture(EntityPlayer* pPlayer, float rdt)
{
	if (damagedBlocks.empty())
		return;

	breakRenderable->beginPolygon();
	Vector3 playerPos = pPlayer->lastTickPos + (pPlayer->position - pPlayer->lastTickPos) * rdt;
	for (DamageBlocks::iterator it = damagedBlocks.begin(); it != damagedBlocks.end(); )
	{
		DestroyBlockProgress* pDamageBlock = it->second;
		BlockPos pos = pDamageBlock->getBlockPos();
		Vector3 delVec = playerPos - Vector3(pos);

		BlockPos sectionPos = pos;
		sectionPos.x <<= 4;
		sectionPos.y <<= 4;
		sectionPos.z <<= 4;
		sectionPos.x >>= 4;
		sectionPos.y >>= 4;
		sectionPos.z >>= 4;

		breakRenderable->setPosition(sectionPos);
		globalRenderBlocks->setSpecialPosition(sectionPos);

		if (delVec.lenSqr() > 1024.0f)
		{
			LordSafeDelete(it->second);
			DamageBlocks::iterator jt = it++;
			damagedBlocks.erase(jt);
		}
		else
		{
			int blockID = theWorld->getBlockId(pos);
			Block* pBlock = blockID > 0 ? BlockManager::sBlocks[blockID] : NULL;

			if (pBlock == NULL)
			{
				pBlock = BlockManager::stone;
			}

			globalRenderBlocks->renderBlockUsingTexture(pBlock, pos, destroyBlockIcons[pDamageBlock->getPartialBlockDamage()]);
			++it;
		}
	}
	breakRenderable->endPolygon();
	TessManager::Instance()->addTessRenderable(breakRenderable);
}

void RenderGlobal::setPoisonCircleRangeAndSpeed(vector<Vector2>::type range, float speed)
{
	TessObject_PoisonCircle* pPoisonObj = dynamic_cast<TessObject_PoisonCircle*>(m_poisonObj);
	if (pPoisonObj)
	{
		pPoisonObj->setPoisonCircleRange(range);
		pPoisonObj->setPoisonSpeed(speed);
	}
}

bool RenderGlobal::isStart()
{
	TessObject_PoisonCircle* pPoisonObj = dynamic_cast<TessObject_PoisonCircle*>(m_poisonObj);
	if (pPoisonObj)
	{
		return pPoisonObj->isStart();
	}
	return false;
}

void RenderGlobal::renderWallText()
{
	for (auto iter : m_wallTextList)
	{
		WallTextRenderer::renderWallText(iter.wallText, iter.textPos, true, iter.scale, iter.yaw, iter.pitch, iter.r, iter.g, iter.b, iter.a);
	}
}

void RenderGlobal::addWallText(String txt, Vector3 pos, float scale, float yaw, float pitch, float r, float g, float b, float a)
{
	m_wallTextList.push_back({ txt, pos, scale, yaw, pitch, r, g, b, a });
}

void RenderGlobal::deleteWallText(Vector3 pos)
{
	for (auto iter = m_wallTextList.begin(); iter != m_wallTextList.end();)
	{
		float distance = iter->textPos.distanceTo(pos);
		if (distance <= 1.0f)
		{
			iter = m_wallTextList.erase(iter);
		}
		else
		{
			iter++;
		}
	}
}

Vector3 RenderGlobal::getPoisonCircleStartPosition()
{
	TessObject_PoisonCircle* pPoisonObj = dynamic_cast<TessObject_PoisonCircle*>(m_poisonObj);
	if (pPoisonObj)
	{
		return pPoisonObj->getPoisonCircleStartPosition();
	}
	return Vector3();
}

Vector3 RenderGlobal::getPoisonCircleEndPosition()
{
	TessObject_PoisonCircle* pPoisonObj = dynamic_cast<TessObject_PoisonCircle*>(m_poisonObj);
	if (pPoisonObj)
	{
		return pPoisonObj->getPoisonCircleEndPosition();
	}
	return Vector3();
}

Vector3 RenderGlobal::getSafeCircleStartPosition()
{
	TessObject_PoisonCircle* pPoisonObj = dynamic_cast<TessObject_PoisonCircle*>(m_poisonObj);
	if (pPoisonObj)
	{
		return pPoisonObj->getSafeCircleStartPosition();
	}
	return Vector3();
}

Vector3 RenderGlobal::getSafeCircleEndPosition()
{
	TessObject_PoisonCircle* pPoisonObj = dynamic_cast<TessObject_PoisonCircle*>(m_poisonObj);
	if (pPoisonObj)
	{
		return pPoisonObj->getSafeCircleEndPosition();
	}
	return Vector3();
}


void RenderGlobal::clearSelection()
{
	m_selectID = -1;
	m_selectMeta = -1;
	m_selectPos = BlockPos::INVALID;
}

void RenderGlobal::drawSelectionBox(const RayTraceResult& tracy, int itemstack, float rdt)
{
	if (itemstack == 0 && tracy.typeOfHit == RAYTRACE_TYPE_BLOCK)
	{
		BlockPos pos = tracy.blockPos;
		int blockID = theWorld->getBlockId(pos);
		float expandSize = 0.002f;
		if (blockID > 0)
		{
			// draw line frame.
			int meta = theWorld->getBlockMeta(pos);
			BlockManager::sBlocks[blockID]->setBlockBoundsBasedOnState(theWorld, pos);
			// drawOutlinedBoundingBox(BlockManager::sBlocks[blockID]->getSelectedBoundingBox(theWorld, pos).expand(expandSize));

			// draw high lighting block.
			if (blockID != m_selectID || meta != m_selectMeta || pos != m_selectPos)
			{
				SectionRenderer* sr = getSectionRender(pos);
				const auto& atlasName = cBlockManager::scBlocks[blockID]->getAtlasName();
				auto texture = TextureAtlasRegister::Instance()->getTextureAtlas(atlasName)->getTexture();
				m_selectBlock->setPosition(sr->blockPos);
				LordAssert(texture);
				if (m_selectBlock->getTexture() != texture)
				{
					m_selectBlock->setMaterialParam(texture, TRP_TRANSPARENT_HL);
				}
				if (sr && sr->updateSpecifyPos(pos, m_selectBlock))
				{
					m_selectID = blockID;
					m_selectMeta = meta;
					m_selectPos = pos;
				}
				else
					clearSelection();
			}
		}
		else
			clearSelection();
	}
	else
		clearSelection();
}

void RenderGlobal::beginFrame()
{
	// m_selectFrame->beginPolygon();
	for (auto it : m_visualHelps)
	{
		it->beginPolygon();
	}
	// m_selectBlock->beginPolygon();
	// add another renderding things.
}

void RenderGlobal::endFrame()
{
	/*
	m_selectFrame->endPolygon();
	if (m_selectFrame->getPointNum() > 0)
		TessManager::Instance()->addLineRenderable(m_selectFrame);
		*/

	if (m_visual_collisions->getPointNum() > 0)
		TessManager::Instance()->addLineRenderable(m_visual_collisions);

	for (auto it : m_visualHelps)
	{
		it->endPolygon();
		if (it->getPointNum() > 0)
			TessManager::Instance()->addLineRenderable(it);
	}

	if (m_selectID > 0 && m_selectPos != BlockPos::INVALID && m_selectBlock->getQuadNum() > 0)
		TessManager::Instance()->addTessRenderable(m_selectBlock);
	// add another renderding things.
}

LineRenderable* RenderGlobal::getLineRenderable(int points)
{
	LineRenderable* pResult = nullptr;

	for (auto it : m_visualHelps)
	{
		if (it->getFreePointNum() < points)
			continue;
		pResult = it;
		break;
	}

	if (!pResult)
	{
		pResult = TessManager::Instance()->createLineRenderable(LRP_VIRSUALHELP);
		pResult->beginPolygon();
		m_visualHelps.push_back(pResult);
	}

	return pResult;
}

void RenderGlobal::drawVisualHelpBox()
{
	m_visual_collisions->beginPolygon();
	for (auto it : visualHelps)
	{
		const Box& aabb = it.first;
		ui32 color = it.second;

		LineVertexFmt* points = NULL;
		m_visual_collisions->getLockPtr_line(5, points);
		points[0].set(aabb.vMin.x, aabb.vMin.y, aabb.vMin.z, color);
		points[1].set(aabb.vMax.x, aabb.vMin.y, aabb.vMin.z, color);
		points[2].set(aabb.vMax.x, aabb.vMin.y, aabb.vMax.z, color);
		points[3].set(aabb.vMin.x, aabb.vMin.y, aabb.vMax.z, color);
		points[4].set(aabb.vMin.x, aabb.vMin.y, aabb.vMin.z, color);

		m_visual_collisions->getLockPtr_line(5, points);
		points[0].set(aabb.vMin.x, aabb.vMax.y, aabb.vMin.z, color);
		points[1].set(aabb.vMax.x, aabb.vMax.y, aabb.vMin.z, color);
		points[2].set(aabb.vMax.x, aabb.vMax.y, aabb.vMax.z, color);
		points[3].set(aabb.vMin.x, aabb.vMax.y, aabb.vMax.z, color);
		points[4].set(aabb.vMin.x, aabb.vMax.y, aabb.vMin.z, color);

		m_visual_collisions->getLockPtr_seg(8, points);
		points[0].set(aabb.vMin.x, aabb.vMin.y, aabb.vMin.z, color);
		points[1].set(aabb.vMin.x, aabb.vMax.y, aabb.vMin.z, color);
		points[2].set(aabb.vMax.x, aabb.vMin.y, aabb.vMin.z, color);
		points[3].set(aabb.vMax.x, aabb.vMax.y, aabb.vMin.z, color);
		points[4].set(aabb.vMax.x, aabb.vMin.y, aabb.vMax.z, color);
		points[5].set(aabb.vMax.x, aabb.vMax.y, aabb.vMax.z, color);
		points[6].set(aabb.vMin.x, aabb.vMin.y, aabb.vMax.z, color);
		points[7].set(aabb.vMin.x, aabb.vMax.y, aabb.vMax.z, color);
	}
	m_visual_collisions->endPolygon();
}

void RenderGlobal::drawOutlinedBoundingBox(const Box& aabb)
{
	/*
	LineVertexFmt* points = NULL;
	m_selectFrame->getLockPtr_line(5, points);
	points[0].set(aabb.vMin.x, aabb.vMin.y, aabb.vMin.z, 0xFF000000);
	points[1].set(aabb.vMax.x, aabb.vMin.y, aabb.vMin.z, 0xFF000000);
	points[2].set(aabb.vMax.x, aabb.vMin.y, aabb.vMax.z, 0xFF000000);
	points[3].set(aabb.vMin.x, aabb.vMin.y, aabb.vMax.z, 0xFF000000);
	points[4].set(aabb.vMin.x, aabb.vMin.y, aabb.vMin.z, 0xFF000000);
	
	m_selectFrame->getLockPtr_line(5, points);
	points[0].set(aabb.vMin.x, aabb.vMax.y, aabb.vMin.z, 0xFF000000);
	points[1].set(aabb.vMax.x, aabb.vMax.y, aabb.vMin.z, 0xFF000000);
	points[2].set(aabb.vMax.x, aabb.vMax.y, aabb.vMax.z, 0xFF000000);
	points[3].set(aabb.vMin.x, aabb.vMax.y, aabb.vMax.z, 0xFF000000);
	points[4].set(aabb.vMin.x, aabb.vMax.y, aabb.vMin.z, 0xFF000000);

	m_selectFrame->getLockPtr_seg(8, points);
	points[0].set(aabb.vMin.x, aabb.vMin.y, aabb.vMin.z, 0xFF000000);
	points[1].set(aabb.vMin.x, aabb.vMax.y, aabb.vMin.z, 0xFF000000);
	points[2].set(aabb.vMax.x, aabb.vMin.y, aabb.vMin.z, 0xFF000000);
	points[3].set(aabb.vMax.x, aabb.vMax.y, aabb.vMin.z, 0xFF000000);
	points[4].set(aabb.vMax.x, aabb.vMin.y, aabb.vMax.z, 0xFF000000);
	points[5].set(aabb.vMax.x, aabb.vMax.y, aabb.vMax.z, 0xFF000000);
	points[6].set(aabb.vMin.x, aabb.vMin.y, aabb.vMax.z, 0xFF000000);
	points[7].set(aabb.vMin.x, aabb.vMax.y, aabb.vMax.z, 0xFF000000);
	*/
}

void RenderGlobal::markBlockForUpdate(const BlockPos& pos)
{
	markBlocksForUpdate(pos -1, pos + 1);
}

void RenderGlobal::markBlockForRenderUpdate(const BlockPos& pos)
{
	markBlocksForUpdate(pos -1, pos + 1);
}

void RenderGlobal::markBlockRangeForRenderUpdate(int minx, int miny, int minz, int maxx, int maxy, int maxz)
{
	markBlocksForUpdate(minx - 1, miny - 1, minz - 1, maxx + 1, maxy + 1, maxz + 1);
}

void RenderGlobal::spawnParticle(const String& particleType, const Vector3& pos, const Vector3& vel, EntityPlayer* emmiter)
{
	spawnParticle_impl(particleType, pos, vel, emmiter);
}

// add by zhouyou.
void RenderGlobal::onEntityCreate(Entity* pEntity)
{
	EntityRenderManager* pEntityRM = EntityRenderManager::Instance();
	if (pEntity->isClass(ENTITY_CLASS_PLAYER))
	{
		bool ret = pEntityRM->createEntityRenderable((EntityLivingBase*)pEntity);
		if (!ret)
			LordLogError("create EntityRenderable Failed!");
		ret = pEntityRM->createEntityActor(pEntity);
		if (!ret)
			LordLogError("create EntityActor Failed!");
	}
	else if (pEntity->isClass(ENTITY_CLASS_MERCHANT))
	{
		if (!pEntityRM->createEntityActor(pEntity))
			LordLogError("create EntityActor Failed!");
	}
	else if (pEntity->isClass(ENTITY_CLASS_VEHICLE))
	{
		if (!pEntityRM->createEntityActor(pEntity))
			LordLogError("create Vehicle EntityActor Failed!");
	}
	else if (pEntity->isClass(ENTITY_CLASS_ITEM))
	{
		EntityItem* pEntityItem = dynamic_cast<EntityItem*>(pEntity);
		if (!EntityRenderManager::Instance()->isItemMeshRender(pEntityItem))
			return;
		if (!pEntityRM->createMeshItem(pEntityItem))
			LordLogError("create MeshItem Failed!");
	}
	else if (pEntity->isClass(ENTITY_CLASS_RANK_NPC))
	{
		if (!pEntityRM->createEntityActor(pEntity))
			LordLogError("create EntityActor rank Failed!");
	}
	else if (pEntity->isClass(ENTITY_CLASS_AIRCRAFT))
	{
		if (!pEntityRM->createEntityActor(pEntity))
			LordLogError("create EntityActor aircraft Failed!");
	}
	else if (pEntity->isClass(ENTITY_CLASS_ACTOR_NPC))
	{
		if (!pEntityRM->createEntityActor(pEntity))
			LordLogError("create EntityActor actornpc Failed!");
	}
	else if (pEntity->isClass(ENTITY_CLASS_SESSION_NPC)) {
		if (!pEntityRM->createEntityActor(pEntity))
			LordLogError("create EntitySession actornpc Failed!");
	}
	else if (pEntity->isClass(ENTITY_CLASS_CREATURE))
	{
		if (!pEntityRM->createEntityActor(pEntity))
			LordLogError("create EntityActor Creature Failed!");
	}
	else if (pEntity->isClass(ENTITY_CLASS_BLOCKMAN_EMPTY))
	{
		if (!pEntityRM->createEntityActor(pEntity))
			LordLogError("create EntityBlockmanEmpty Creature Failed!");
	}
	else if (pEntity->isClass(ENTITY_CLASS_BUILD_NPC))
	{
		if (!pEntityRM->createEntityActor(pEntity))
			LordLogError("create EntityBuildNpc Creature Failed!");
	}
	else if (pEntity->isClass(ENTITY_CLASS_LAND_NPC)) {
		if (!pEntityRM->createEntityActor(pEntity))
			LordLogError("create EntityLand actornpc Failed!");
	}
	else if (pEntity->isClass(ENTITY_CLASS_ACTOR_CANNON_CLIENT))
	{
		if (!pEntityRM->createEntityActor(pEntity))
			LordLogError("create EntityActor actorCannon Failed!");
	}
	else if (pEntity->isClass(ENTITY_CLASS_BULLETIN)) {
		if (!pEntityRM->createEntityActor(pEntity))
			LordLogError("create EntityBulletin actornpc Failed!");
	}
	else if (pEntity->isClass(ENTITY_CLASS_BIRD)) {
		if (!pEntityRM->createEntityActor(pEntity))
			LordLogError("create EntityBird actornpc Failed!");
	}
}

void RenderGlobal::onEntityDestroy(Entity* pEntity)
{
	EntityRenderManager* pEntityRM = EntityRenderManager::Instance();
	if (pEntity->isClass(ENTITY_CLASS_PLAYER))
	{
		bool ret = pEntityRM->destroyEntityRenderable((EntityLivingBase*)pEntity);
		if (!ret)
			LordLogError("destroy EntityRenderable Failed!");
		ret = pEntityRM->destroyEntityActor(pEntity);
		if (!ret)
			LordLogError("destroy EntityActor Failed!");
	}
	else if (pEntity->isClass(ENTITY_CLASS_MERCHANT))
	{
		if (!pEntityRM->destroyEntityActor(pEntity))
			LordLogError("destroy EntityActor Failed!");
	}
	else if (pEntity->isClass(ENTITY_CLASS_ITEM))
	{
		EntityItem* pEntityItem = dynamic_cast<EntityItem*>(pEntity);
		if (!EntityRenderManager::Instance()->isItemMeshRender(pEntityItem))
			return;
		if (!pEntityRM->destroyMeshItem(pEntityItem))
			LordLogError("destroy MeshItem Failed!");
	}
	else if (pEntity->isClass(ENTITY_CLASS_VEHICLE))
	{
		if (!pEntityRM->destroyEntityActor(pEntity))
			LordLogError("destroy EntityActor of vehicle Failed!");
	}
	else if (pEntity->isClass(ENTITY_CLASS_RANK_NPC))
	{
		if (!pEntityRM->destroyEntityActor(pEntity))
			LordLogError("destroy EntityActor of rank Failed!");
	}
	else if (pEntity->isClass(ENTITY_CLASS_AIRCRAFT))
	{
		if (!pEntityRM->destroyEntityActor(pEntity))
			LordLogError("destroy EntityActor of aircraft Failed!");
	}
	else if (pEntity->isClass(ENTITY_CLASS_ACTOR_NPC))
	{
		if(!pEntityRM->destroyEntityActor(pEntity))
			LordLogError("destroy EntityActor of actorNpc Failed!");

	}
	else if (pEntity->isClass(ENTITY_CLASS_CREATURE)) 
	{
		if (!pEntityRM->destroyEntityActor(pEntity))
			LordLogError("destroy EntityActor of EntityCreature Failed!");
	}
	else if (pEntity->isClass(ENTITY_CLASS_SESSION_NPC)) {
		if (!pEntityRM->destroyEntityActor(pEntity))
			LordLogError("destroy EntitySession of actorNpc Failed!");
	}
	else if (pEntity->isClass(ENTITY_CLASS_BLOCKMAN_EMPTY)) {
		if (!pEntityRM->destroyEntityActor(pEntity))
			LordLogError("destroy EntityBlockmanEmpty of blockmanEmpty Failed!");
	}
	else if (pEntity->isClass(ENTITY_CLASS_BUILD_NPC)) {
		if (!pEntityRM->destroyEntityActor(pEntity))
			LordLogError("destroy EntityBuildNpc of buildNpc Failed!");
	}
	else if (pEntity->isClass(ENTITY_CLASS_LAND_NPC)) {
		if (!pEntityRM->destroyEntityActor(pEntity))
			LordLogError("destroy EntityLandNpc ofLandNpc Failed!");
	}
	else if (pEntity->isClass(ENTITY_CLASS_ACTOR_CANNON_CLIENT))
	{
		if (!pEntityRM->destroyEntityActor(pEntity))
			LordLogError("destroy EntityActor of actorCannon Failed!");
	}
	else if (pEntity->isClass(ENTITY_CLASS_BULLETIN)) {
		if (!pEntityRM->destroyEntityActor(pEntity))
			LordLogError("destroy EntityBulletin of Bulletin Failed!");
	}
	else if (pEntity->isClass(ENTITY_CLASS_BIRD)) {
		if (!pEntityRM->destroyEntityActor(pEntity))
			LordLogError("destroy EntityBird of Bird Failed!");
	}
}

void RenderGlobal::broadcastSound(int soundID, const BlockPos& pos, int data)
{
	Random& prand = theWorld->m_Rand;

	switch (soundID)
	{
	case 1013:
	case 1018:
		if (bm->renderViewEntity)
		{
			Vector3 viewPos = bm->renderViewEntity->getPosition();
			Vector3 distance = Vector3(pos) - viewPos;
			float len = distance.len();
			
			if (len > 0.0f)
			{
				viewPos += distance / len * 2.f;
			}

			if (soundID == 1013)
			{
				theWorld->playSoundByType(viewPos, ST_MobWitherSpawn);
			}
			else if (soundID == 1018)
			{
				theWorld->playSoundByType(viewPos, ST_MobEndDragon);
			}
		}

	default: 
		break;
	}
}

void RenderGlobal::playAuxSFX(EntityPlayer* player, int type, const BlockPos& pos, int data)
{
	Random& rand = theWorld->m_Rand;
	switch (type)
	{
	case 1000:
		//theWorld->playSound(Vector3(pos), "random.click", 1.0F, 1.0F, false);	break;
		theWorld->playSoundByType(Vector3(pos), ST_Click); break;

	case 1001:
		//theWorld->playSound(Vector3(pos), "random.click", 1.0F, 1.2F, false);	break;
		theWorld->playSoundByType(Vector3(pos), ST_Click); break;

	case 1002:
		//theWorld->playSound(Vector3(pos), "random.bow", 1.0F, 1.2F, false);		break;
		theWorld->playSoundByType(Vector3(pos), ST_PullTheBow); break;

	case 1003:
		if (rand.nextFloat() < 0.5f)
			//theWorld->playSound(Vector3(pos) + Vector3::HALF, "random.door_open", 1.0F, rand.nextFloat() * 0.1F + 0.9F, false);
			theWorld->playSoundByType(Vector3(pos) + Vector3::HALF, ST_OpenDoor);
		else
			//theWorld->playSound(Vector3(pos) + Vector3::HALF, "random.door_close", 1.0F, rand.nextFloat() * 0.1F + 0.9F, false);
			theWorld->playSoundByType(Vector3(pos) + Vector3::HALF, ST_CloseDoor);
		break;

	case 1004:
		//theWorld->playSound(Vector3(pos) + Vector3::HALF, "random.fizz", 0.5F, 2.6F + (rand.nextFloat() - rand.nextFloat()) * 0.8F, false);	break;
		theWorld->playSoundByType(Vector3(pos) + Vector3::HALF, ST_Fizz); break;
	case 1005:
		if (dynamic_cast<ItemRecord*>(Item::itemsList[data]))
			theWorld->playRecord(((ItemRecord*)Item::itemsList[data])->recordName, pos);
		break;

	case 1007:
		//theWorld->playSound(Vector3(pos) + Vector3::HALF, "mob.ghast.charge", 10.0F, (rand.nextFloat() - rand.nextFloat()) * 0.2F + 1.0F, false); break;
		theWorld->playSoundByType(Vector3(pos) + Vector3::HALF, ST_MobGhastCharge);  break;
	case 1008:
		//theWorld->playSound(Vector3(pos) + Vector3::HALF, "mob.ghast.fireball", 10.0F, (rand.nextFloat() - rand.nextFloat()) * 0.2F + 1.0F, false); break;
		theWorld->playSoundByType(Vector3(pos) + Vector3::HALF, ST_MobGhastFireball);  break; 

	case 1009:
		//theWorld->playSound(Vector3(pos) + Vector3::HALF, "mob.ghast.fireball", 2.0F, (rand.nextFloat() - rand.nextFloat()) * 0.2F + 1.0F, false); break;
		theWorld->playSoundByType(Vector3(pos) + Vector3::HALF, ST_MobGhastFireball);  break; 

	case 1010:
		//theWorld->playSound(Vector3(pos) + Vector3::HALF, "mob.zombie.wood", 2.0F, (rand.nextFloat() - rand.nextFloat()) * 0.2F + 1.0F, false);	break;
		theWorld->playSoundByType(Vector3(pos) + Vector3::HALF, ST_MobZombieWood);  break;

	case 1011:
		//theWorld->playSound(Vector3(pos) + Vector3::HALF, "mob.zombie.metal", 2.0F, (rand.nextFloat() - rand.nextFloat()) * 0.2F + 1.0F, false); break;
		theWorld->playSoundByType(Vector3(pos) + Vector3::HALF, ST_MobZombieMetal);  break;

	case 1012:
		//theWorld->playSound(Vector3(pos) + Vector3::HALF, "mob.zombie.woodbreak", 2.0F, (rand.nextFloat() - rand.nextFloat()) * 0.2F + 1.0F, false); break;
		theWorld->playSoundByType(Vector3(pos) + Vector3::HALF, ST_MobZombieWoodbreak);  break; 

	case 1014:
		//theWorld->playSound(Vector3(pos) + Vector3::HALF, "mob.wither.shoot", 2.0F, (rand.nextFloat() - rand.nextFloat()) * 0.2F + 1.0F, false); break;
		theWorld->playSoundByType(Vector3(pos) + Vector3::HALF, ST_MobWitherShoot);  break;

	case 1015:
		//theWorld->playSound(Vector3(pos) + Vector3::HALF, "mob.bat.takeoff", 0.05F, (rand.nextFloat() - rand.nextFloat()) * 0.2F + 1.0F, false); break;
		theWorld->playSoundByType(Vector3(pos) + Vector3::HALF, ST_MobBatTakeoff);  break; 

	case 1016:
		//theWorld->playSound(Vector3(pos) + Vector3::HALF, "mob.zombie.infect", 2.0F, (rand.nextFloat() - rand.nextFloat()) * 0.2F + 1.0F, false); break;
		theWorld->playSoundByType(Vector3(pos) + Vector3::HALF, ST_MobZombieInfect);  break; 

	case 1017:
		//theWorld->playSound(Vector3(pos) + Vector3::HALF, "mob.zombie.unfect", 2.0F, (rand.nextFloat() - rand.nextFloat()) * 0.2F + 1.0F, false); break;
		theWorld->playSoundByType(Vector3(pos) + Vector3::HALF, ST_MobZombieUnfect);  break; 

	case 1020:
		//theWorld->playSound(Vector3(pos) + Vector3::HALF, "random.anvil_break", 1.0F, rand.nextFloat() * 0.1F + 0.9F, false); break;
		theWorld->playSoundByType(Vector3(pos) + Vector3::HALF, ST_BreakAnvil);  break; 

	case 1021:
		//theWorld->playSound(Vector3(pos) + Vector3::HALF, "random.anvil_use", 1.0F, rand.nextFloat() * 0.1F + 0.9F, false);	break;
		theWorld->playSoundByType(Vector3(pos) + Vector3::HALF, ST_UseAnvil);  break;

	case 1022:
		//theWorld->playSound(Vector3(pos) + Vector3::HALF, "random.anvil_land", 0.3F, rand.nextFloat() * 0.1F + 0.9F, false); break;
		theWorld->playSoundByType(Vector3(pos) + Vector3::HALF, ST_LandAnvil);  break;

	case 2000:
	{
		int ex = data % 3 - 1;
		int ez = data / 3 % 3 - 1;
		Vector3 vec = Vector3(pos) + Vector3(ex * 0.5f, 0.f, ez * 0.6f) + Vector3::HALF;

		for (int i = 0; i < 10; ++i)
		{
			float velxz = rand.nextFloat() * 0.2f + 0.01f;
			Vector3 emitPos, emitVel;
			emitPos.x = vec.x + ex * 0.01f + (rand.nextFloat() - 0.5f) * ez * 0.5f;
			emitPos.y = vec.y + (rand.nextFloat() - 0.5f) * 0.5f;
			emitPos.z = vec.z + ez * 0.01f + (rand.nextFloat() - 0.5f) * ex * 0.5f;
			emitVel.x = ex * velxz + rand.nextFloat() * 0.01f;
			emitVel.y = -0.03f + rand.nextFloat() * 0.01f;
			emitVel.z = ez * velxz + rand.nextFloat() * 0.01f;
			spawnParticle("smoke", emitPos, emitVel);
		}
	}
	break;

	case 2001:
	{
		int id = data & 4095;

		if (id > 0)
		{
			cBlock* pcBlock = cBlockManager::scBlocks[id];
			// todo. break sound.
			SoundType breakSound = pcBlock->getStepSound().placeSound;
			SoundSystem::Instance()->playEffectByType(breakSound);

			//bm->getWorld()->playSound(Vector3(pos) + Vector3::HALF, pcBlock->getStepSound().getBreakSound(), (pcBlock->getStepSound().volume + 1.0F) / 2.0F, pcBlock->getStepSound().pitch * 0.8F, false);
			bm->getWorld()->playSoundByType(Vector3(pos) + Vector3::HALF, pcBlock->getStepSound().breakSound);
		}

		EffectRenderer::Instance()->addBlockDestroyEffects(pos, data & 4095, (data >> 12) & 255);
	}
	break;

	case 2002:
	{
		Vector3 vec = Vector3(pos);
		String effectName = StringUtil::Format("iconcrack_%d,%d" + Item::potion->itemID, data);

		for (int i = 0; i< 8; ++i)
		{
			spawnParticle(effectName, vec, rand.nextVector() * 0.15f);  // vel.y * 0.2f. ingore.
		}

		int color = Item::potion->getColorFromDamage(data);
		float r = (float)(color >> 16 & 255) / 255.0F;
		float g = (float)(color >> 8 & 255) / 255.0F;
		float b = (float)(color >> 0 & 255) / 255.0F;
		effectName = "spell";

		if (Item::potion->isEffectInstant(data))
			effectName = "instantSpell";

		for (int i = 0; i < 100; ++i)
		{
			Vector3 emitVel, emitpos;
			float velxz = rand.nextFloat() * 4.0f;
			float angle = rand.nextFloat() * Math::PI_2;
			emitVel.x = Math::Cos(angle) * velxz;
			emitVel.y = 0.01f + rand.nextFloat() * 0.5f;
			emitVel.z = Math::Sin(angle) * velxz;
			emitpos.x = vec.x + emitVel.x * 0.1f;
			emitpos.y = vec.y + 0.3f;
			emitpos.z = vec.z + emitVel.z * 0.1f;
			EntityFx* pFx = spawnParticle_impl(effectName, emitpos, emitVel);

			if (pFx)
			{
				float var32 = 0.75F + rand.nextFloat() * 0.25F;
				pFx->setColor(r * var32, g * var32, b * var32);
				pFx->multiplyVelocity(velxz);
			}
		}

		//theWorld->playSound(Vector3(pos) + Vector3::HALF, "random.glass", 1.0F, rand.nextFloat() * 0.1F + 0.9F, false);
		theWorld->playSoundByType(Vector3(pos) + Vector3::HALF, ST_Glass);
	}
	break;

	case 2003:
	{
		Vector3 vec = Vector3(pos) + Vector3(0.5f, 0.f, 0.5f);
		String effectName = StringUtil::Format("iconcrack_%d", Item::eyeOfEnder->itemID);

		for (int i = 0; i < 8; ++i)
		{
			spawnParticle(effectName, vec, rand.nextVector() * 0.15f); // vel.y * 0.2f. ingore.
		}

		for (int i = 0; i < 40; ++i)
		{
			float angle = i * Math::PI_2 / 20;
			float fx = Math::Cos(angle);
			float fz = Math::Sin(angle);
			spawnParticle("portal", vec + Vector3(fx*5.f, -0.4f, fz * 5.f), Vector3(fx * -5.f, 0.f, fz * -5.f));
			spawnParticle("portal", vec + Vector3(fx*5.f, -0.4f, fz * 5.f), Vector3(fx * -7.f, 0.f, fz * -7.f));
		}
	}
	break;

	case 2004:
		for (int i = 0; i < 20; ++i)
		{
			Vector3 vec = Vector3(pos) + Vector3::HALF + (rand.nextVector() - Vector3::HALF) * 2.f;
			theWorld->spawnParticle("smoke", vec, Vector3::ZERO);
			theWorld->spawnParticle("flame", vec, Vector3::ZERO);
		}
		break;

	case 2005:
		ItemDye::spawnBonemealParticles(theWorld, pos, data);
		break;
	}
}

EntityFx* RenderGlobal::spawnParticle_impl(const String& name, const Vector3& pos, const Vector3& vel, EntityPlayer* emmiter)
{
	if (bm == nullptr || bm->renderViewEntity == nullptr)
		return nullptr;

	EffectRenderer* fxRM = EffectRenderer::Instance();
	int particleSetting = bm->m_gameSettings->particleSetting;

	if (particleSetting == 1 && theWorld->m_Rand.nextInt(3) == 0)
		particleSetting = 2;

	Vector3 vec = bm->renderViewEntity->getPosition() - pos;
	EntityFx* pFx = nullptr;


	if (name == "hugeexplosion")
	{
		pFx = LordNew EntityHugeExplodeFX(theWorld, pos, vel);
	}
	else if (name == "largeexplode")
	{
		pFx = LordNew EntityLargeExplodeFX(theWorld, pos, vel);
	}
	else if (name == "fireworksSpark")
	{
		// pFx = new EntityFireworkSparkFX(this.theWorld, par2, par4, par6, par8, par10, par12, this.bm.effectRenderer);
	}

	if (pFx != nullptr)
	{
		fxRM->addEffect(pFx);
		return pFx;
	}

	float minDis = 16.f;

	if (vel.lenSqr() > minDis * minDis)
		return nullptr;
	else if (particleSetting > 1)
		return nullptr;

	if (name == "bubble")
	{
		pFx = LordNew EntityBubbleFX(theWorld, pos, vel);
	}
	else if (name == "suspended")
	{
		// pFx = new EntitySuspendFX(this.theWorld, par2, par4, par6, par8, par10, par12);
	}
	else if (name == "depthsuspend")
	{
		// pFx = new EntityAuraFX(this.theWorld, par2, par4, par6, par8, par10, par12);
	}
	else if (name == "townaura")
	{
		// pFx = new EntityAuraFX(this.theWorld, par2, par4, par6, par8, par10, par12);
	}
	else if (name == "crit")
	{
		// pFx = new EntityCritFX(this.theWorld, par2, par4, par6, par8, par10, par12);
	}
	else if (name == "magicCrit")
	{
		/*pFx = new EntityCritFX(this.theWorld, par2, par4, par6, par8, par10, par12);
		((EntityFX)pFx).setRBGColorF(((EntityFX)pFx).getRedColorF() * 0.3F, ((EntityFX)pFx).getGreenColorF() * 0.8F, ((EntityFX)pFx).getBlueColorF());
		((EntityFX)pFx).nextTextureIndexX();*/
	}
	else if (name == "smoke")
	{
		pFx = LordNew EntitySmokeFX(theWorld, pos, vel);
	}
	else if (name == "chestFx")
	{
		pFx = LordNew EntityChestFX(theWorld, pos, vel);
	}
	else if (name == "mobSpell")
	{
		pFx = LordNew EntitySpellFX(theWorld, pos, Vector3::ZERO);
		pFx->setColor(vel.x, vel.y, vel.z);
	}
	else if (name == "mobSpellAmbient")
	{
		pFx = LordNew EntitySpellFX(theWorld, pos, Vector3::ZERO);
		pFx->setAlpha(0.15f);
		pFx->setColor(vel.x, vel.y, vel.z);
	}
	else if (name == "spell")
	{
		pFx = LordNew EntitySpellFX(theWorld, pos, vel);
	}
	else if (name == "gunFlame")
	{
		pFx = LordNew EntityGunFlameFX(theWorld, pos, emmiter);
	}
	else if (name == "instantSpell")
	{
		pFx = LordNew EntitySpellFX(theWorld, pos, vel);
		((EntitySpellFX*)pFx)->setBaseSpellTextureIndex(144);
	}
	else if (name == "witchMagic")
	{
		pFx = LordNew EntitySpellFX(theWorld, pos, vel);
		((EntitySpellFX*)pFx)->setBaseSpellTextureIndex(144);
		float c = theWorld->m_Rand.nextFloat() * 0.5F + 0.35F;
		((EntitySpellFX*)pFx)->setColor(c, c, c);
	}
	else if (name == "note")
	{
		// pFx = new EntityNoteFX(this.theWorld, par2, par4, par6, par8, par10, par12);
	}
	else if (name == "portal")
	{
		// pFx = new EntityPortalFX(this.theWorld, par2, par4, par6, par8, par10, par12);
	}
	else if (name == "enchantmenttable")
	{
		// pFx = new EntityEnchantmentTableParticleFX(this.theWorld, par2, par4, par6, par8, par10, par12);
	}
	else if (name == "explode")
	{
		// pFx = new EntityExplodeFX(this.theWorld, par2, par4, par6, par8, par10, par12);
	}
	else if (name == "flame")
	{
		// pFx = new EntityFlameFX(this.theWorld, par2, par4, par6, par8, par10, par12);
	}
	else if (name == "lava")
	{
		pFx = LordNew EntityLavaFX(theWorld, pos);
	}
	else if (name == "footstep")
	{
		// pFx = new EntityFootStepFX(this.renderEngine, this.theWorld, par2, par4, par6);
	}
	else if (name == "splash")
	{
		// pFx = new EntitySplashFX(this.theWorld, par2, par4, par6, par8, par10, par12);
	}
	else if (name == "largesmoke")
	{
		// pFx = new EntitySmokeFX(this.theWorld, par2, par4, par6, par8, par10, par12, 2.5F);
	}
	else if (name == "cloud")
	{
		// pFx = new EntityCloudFX(this.theWorld, par2, par4, par6, par8, par10, par12);
	}
	else if (name == "reddust")
	{
		// pFx = new EntityReddustFX(this.theWorld, par2, par4, par6, (float)par8, (float)par10, (float)par12);
	}
	else if (name == "snowballpoof")
	{
		pFx = LordNew EntityBreakingFX(theWorld, pos, ItemClient::citemsList[ITEM_ID_SNOW_BALL]);
	}
	else if (name == "dripWater")
	{
		// pFx = new EntityDropParticleFX(this.theWorld, par2, par4, par6, Material.water);
	}
	else if (name == "dripLava")
	{
		// pFx = new EntityDropParticleFX(this.theWorld, par2, par4, par6, Material.lava);
	}
	else if (name == "snowshovel")
	{
		// pFx = new EntitySnowShovelFX(this.theWorld, par2, par4, par6, par8, par10, par12);
	}
	else if (name == "slime")
	{
		// pFx = new EntityBreakingFX(this.theWorld, par2, par4, par6, Item.slimeBall);
	}
	else if (name == "heart")
	{
		// pFx = new EntityHeartFX(this.theWorld, par2, par4, par6, par8, par10, par12);
	}
	else if (name == "angryVillager")
	{
		// pFx = new EntityHeartFX(this.theWorld, par2, par4 + 0.5D, par6, par8, par10, par12);
		// ((EntityFX)pFx).setParticleTextureIndex(81);
		// ((EntityFX)pFx).setRBGColorF(1.0F, 1.0F, 1.0F);
	}
	else if (name == "happyVillager")
	{
		// pFx = new EntityAuraFX(this.theWorld, par2, par4, par6, par8, par10, par12);
		// ((EntityFX)pFx).setParticleTextureIndex(82);
		// ((EntityFX)pFx).setRBGColorF(1.0F, 1.0F, 1.0F);
	}
	else
	{
		int blockID;
		int side;
		StringArray strArr;

		if (name.find("iconcrack_") == 0)
		{
			strArr = StringUtil::Split(name, "_", 3);
			if (strArr.size() > 2)
			{
				blockID = StringUtil::ParseInt(strArr[1]);
				side = StringUtil::ParseInt(strArr[2]);
				pFx = LordNew EntityBreakingFX(theWorld, pos, vel, ItemClient::citemsList[blockID], side);
			}
			else
			{
				blockID = StringUtil::ParseInt(strArr[1]);
				pFx = LordNew EntityBreakingFX(theWorld, pos, vel, ItemClient::citemsList[blockID], 0);
			}
		}
		else if (name.find("tilecrack_") == 0)
		{
			strArr = StringUtil::Split(name, "_", 3);
			blockID = StringUtil::ParseInt(strArr[1]);
			side = StringUtil::ParseInt(strArr[2]);
			EntityDiggingFX* diggingFx = LordNew EntityDiggingFX(theWorld, pos, vel, BlockManager::sBlocks[blockID], side);
			diggingFx->applyRenderColor(side);
			pFx = (EntityFx*)diggingFx;
		}
	}

	if (pFx != nullptr)
	{
		fxRM->addEffect(pFx);
	}

	return pFx;
}

void RenderGlobal::markRenderersForNewPositionIfUpdated()
{
	EntityLivingBase* viewer = bm->renderViewEntity;
	const Vector3& exceptPos = viewer->getPosition();
	if (frustumUpdatePosChunk != viewer->chunkCoord || exceptPos.squareDistanceTo(prevSort) > 16.0)
	{
		prevSort = exceptPos;
		frustumUpdatePosChunk = viewer->chunkCoord;
		markRenderersForNewPosition(exceptPos.getFloor());
	}
}

void RenderGlobal::renderEntities(const Vector3& viewerPos, const Frustum& frustum, float rdt)
{
	static int renderEntitiesStartupCounter = 10;
	if (renderEntitiesStartupCounter > 0)
	{
		--renderEntitiesStartupCounter;
		return;
	}

	// RenderManager.instance.cacheActiveRenderInfo(theWorld, bm.func_110434_K(), bm.fontRenderer, bm.renderViewEntity, bm.pointedEntityLiving, bm.gameSettings, rdt);
	TileEntityRenderMgr::Instance()->cacheActiveRenderInfo(theWorld, bm->m_pPlayer, rdt);
	countEntitiesTotal = 0;
	countEntitiesRendered = 0;
	countEntitiesHidden = 0;
	EntityLivingBase* pPlayer = bm->renderViewEntity;
	EntityRenderManager* pEntityRM = EntityRenderManager::Instance();
	pEntityRM->cacheActiveRenderInfo(theWorld, bm->renderViewEntity, bm->pointedEntityLiving, bm->m_gameSettings, rdt);
	pEntityRM->renderPos = pPlayer->getPosition(rdt);
	pEntityRM->beginRender();
	TileEntityRenderMgr::Instance()->beginRender();

	EntityMap* loadedEntityMap = theWorld->getLoadedEntityMap();
	countEntitiesTotal = loadedEntityMap->size();
	Entity* pEntity;
	
	for (EntityMap::iterator it = loadedEntityMap->begin(); it != loadedEntityMap->end(); ++it)
	{
		pEntity = it->second;
 		bool visible = pEntity->isInRangeToRenderVec3D(viewerPos) &&
			(pEntity->ignoreFrustumCheck || frustum.intersect(pEntity->boundingBox.expand(1.f)));

		if (!visible && pEntity->isClass(ENTITY_CLASS_LIVING))
		{
			EntityLiving* pLiving = (EntityLiving*)pEntity;

			if (pLiving->getLeashed() && pLiving->getLeashedToEntity())
			{
				const Entity* leashedEntity = pLiving->getLeashedToEntity();
				visible = frustum.intersect(leashedEntity->boundingBox);
			}
		}

		if ((visible || pEntity->isClass(ENTITY_CLASS_FISHHOOK_CLIENT) || pEntity->isClass(ENTITY_CLASS_ACTOR_CANNON_CLIENT) || pEntity->isClass(ENTITY_CLASS_GUN_FLAME))
			&& (pEntity != bm->renderViewEntity || bm->m_gameSettings->getPersonView() != GameSettings::SPVT_FIRST || bm->m_pPlayer->m_isCameraLocked || bm->renderViewEntity->isPlayerSleeping())
			&& theWorld->blockExists(BlockPos(int(Math::Floor(pEntity->position.x)), 0, int(Math::Floor(pEntity->position.z)))))
		{
 			++countEntitiesRendered;
			pEntityRM->renderEntity(pEntity, rdt);
		}
	}

	/*** render all the tile-entities*/
	for (auto it : m_tileEntities)
	{
		TileEntityRenderMgr::Instance()->renderTileEntity(it, rdt);
	}

	renderWallText();

	pEntityRM->endRender();
	TileEntityRenderMgr::Instance()->endRender();
}

void RenderGlobal::onPlayerChangeActor(EntityPlayer* pEntityPlayer)
{
	EntityRenderManager::Instance()->refreshEntityActor(pEntityPlayer, pEntityPlayer->m_actorName);
}

void RenderGlobal::onEntityChangeActor(Entity * pEntity, const String & actorName)
{
	EntityRenderManager::Instance()->refreshEntityActor(pEntity, actorName);
}


void RenderGlobal::onPlayerRestoreActor(Entity * pEntity)
{
	EntityRenderManager::Instance()->restorePlayerActor(pEntity);
}

}
