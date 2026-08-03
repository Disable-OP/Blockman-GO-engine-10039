#include "SectionRenderer.h"
#include "VisualGraph.h"
#include "RenderBlocks.h"
#include "Common.h"
#include "TessMeshCollection.h"
#include "GenSectionLOD.h"
#include "Rasterizer.h"

#include "Block/BlockManager.h"
#include "Block/Block.h"
#include "World/World.h"
#include "Chunk/Chunk.h"
#include "cChunk/ChunkClient.h"
#include "World/ChunkCollection.h"
#include "cTileEntity/TileEntityRenderMgr.h"

#include "Scene/Camera.h"
#include "Tessolator/TessRenderable.h"
#include "Tessolator/TessManager.h"
#include "Tessolator/RasterizerHelpRender.h"
#include "VisualTessThread.h"
#include "cWorld/Blockman.h"

namespace BLOCKMAN
{
int SectionRenderer::chunksUpdated = 0;

SectionRenderer::SectionRenderer(World* par1World, TileEntitySet& entities, const BlockPos& pos)
	: m_entities_world(entities)
{
	worldObj = par1World;
	// this.tileEntities = par2List;
	blockPos.x = -999;
	chunkIndex = 0;
	isChunkLit = false;
	setPosition(pos);
	needsUpdate = false;
	m_visibilityCache = NULL;
	m_chunkPos = Vector2i::INVALID;
	m_frameIndex = -1;
#ifdef SHOW_RASTERIZER_RENDER
	rasterRenderable = nullptr;
#endif
}

SectionRenderer::~SectionRenderer()
{
	releaseRenderalbes();

#ifdef SHOW_RASTERIZER_RENDER
	if (rasterRenderable)
	{
		TessManager::Instance()->destroyRasterizerRenderalbe(rasterRenderable);
		rasterRenderable = nullptr;
	}
#endif

	LordSafeDelete(m_rasterizerMesh);
}

void SectionRenderer::setPosition(const BlockPos& pos)
{
	if (pos != blockPos)
	{
		blockPos = pos;
		posPlus = pos + 8;
		posClip.x = pos.x & 1023;
		posClip.y = pos.y;
		posClip.z = pos.z & 1023;
		posMinus = pos - posClip;
		Vector3 bound = Vector3::ZERO;
		FailCnt = 0;
		rendererBoundingBox.setExtents(Vector3(pos) - bound, Vector3(pos) + 16 + bound);
		markDirty();
	}
}

float SectionRenderer::distanceToEntitySquared(Camera* pCamera) const
{
	Vector3 cameraPos = pCamera->getPosition();

	Vector3 disVec = cameraPos - posPlus;
	return disVec.lenSqr();
}

bool SectionRenderer::setFrameIndex(int frameIdx)
{
	if (frameIdx == m_frameIndex)
		return false;
	m_frameIndex = frameIdx;
	return true;
}

void SectionRenderer::checkRebuild()
{
	if (!needsUpdate && m_chunkPos != Vector2i::INVALID)
	{
		if (worldObj->chunkExists(blockPos.x >> 4, blockPos.z >> 4))
		{
			auto pChunk = std::static_pointer_cast<ChunkClient>(worldObj->getChunkFromChunkCoords(blockPos.x >> 4, blockPos.z >> 4));
			// check if the provider is loaded the correct chunk for prepare rendering.
			// when the player teleport in world.
			if (pChunk->m_posX != m_chunkPos.x || pChunk->m_posZ != m_chunkPos.y)
			{
				LordLogError("Error in SectionRenderer::checkRebuild the cacehd chunk has woring position! expect pos [%d, %d], reality pos[%d, %d]",
					m_chunkPos.x, m_chunkPos.y, pChunk->m_posX, pChunk->m_posZ);
				return;
			}

			if (!pChunk->m_isPrepared || pChunk->gapLightingUpdated() || pChunk->needRebuildAll())
			{
				m_chunkPos = Vector2i::INVALID;
				m_visibilityCache = NULL;
				needsUpdate = true;
			}
		}
		else
		{
			LordLogError("Error in SectionRenderer::checkRebuild the cacehd chunk is gone away! chunk pos [%d, %d]", m_chunkPos.x, m_chunkPos.y);
		}
	}
}

void SectionRenderer::markDirty()
{
	needsUpdate = true;
	m_chunkPos = Vector2i::INVALID;
	m_visibilityCache = NULL;
}

void SectionRenderer::exchangeRenderables()
{
	if (renderBlockCnt == 0)
	{
		releaseRenderalbes();
		return;
	}
	if (logic_tessLst.empty())
		return;

	int already = render_tessLst.size();
	int beforeQuadNum = 0;
	if (already > 0)
		beforeQuadNum = render_tessLst.front()->getQuadNum();

	releaseRenderalbes();
	render_tessLst.insert(render_tessLst.end(), logic_tessLst.begin(), logic_tessLst.end());
	logic_tessLst.clear();
	finishUpdateRender();

	if (already > 0)
	{
		int afterQuadNum = 0;
		if (!render_tessLst.empty())
			afterQuadNum = render_tessLst.front()->getQuadNum();
		//LordLogInfo("SectionRender exchanged [%d, %d, %d] (%d ===> %d)", blockPos.x >> 4, blockPos.y >> 4, blockPos.z >> 4, beforeQuadNum, afterQuadNum);
	}
}	

void SectionRenderer::releaseRenderalbes()
{
	for (TessRenderableLst::iterator it = render_tessLst.begin(); it != render_tessLst.end(); ++it)
	{
		TessManager::Instance()->destroyTessRenderable(*it);
	}
	render_tessLst.clear();
}

bool SectionRenderer::finishUpdateRender()
{
	bool result = false;
	for (TessRenderableLst::iterator it = render_tessLst.begin(); it != render_tessLst.end(); ++it)
	{
		(*it)->endPolygon();
		result = true;
	}
	return result;
}

bool SectionRenderer::updateRenderer()
{
	bool nResult = false;
	TessMeshCollection* pChunkCollection = NULL;

	do
	{
		ERROR_BREAK(needsUpdate);

		BlockPos startPos = blockPos;
		BlockPos endPos = startPos + 16;
		int extend = 1;

		TessMeshTest test((WorldClient*)worldObj);
		int retReSet = test.reSetSection(blockPos, FailCnt++);

		if (test.m_isInAsync)
			break;

		if (test.isEmpty)
		{
			nResult = true;
			break;
		}

		ChunkPtr pChunk = worldObj->getChunkFromBlockCoords(blockPos.x, blockPos.z);
		if (!pChunk->m_isPrepared || pChunk->gapLightingUpdated() || pChunk->needRebuildAll())
		{
			Blockman::Instance()->m_lightTransferTrhead->addTask(pChunk);
			//ChunkRebuilder::Instance()->rebuildChunk(blockPos.x >> 4, blockPos.z >> 4, worldObj->getChunkService());
			break;
		}
		
		pChunkCollection = LordNew TessMeshCollection((WorldClient*)worldObj);
		ERROR_BREAK(pChunkCollection);
		retReSet = pChunkCollection->reSetSection(blockPos);
		ERROR_BREAK(retReSet);
		
		TileEntitySet tileEntities_old = m_entities_section;  // 1,[ABC]  2,[ABCD]
		m_entities_section.clear();
		// releaseRenderalbes();
		++chunksUpdated;
		needsUpdate = false;
		RenderBlocks* pRenderBlocks = LordNew RenderBlocks(pChunkCollection, this, Vector3(blockPos) + 8.f);
		bytesDrawn = 0;
		renderBlockCnt = 0;
		for (int iy = startPos.y; iy < endPos.y; ++iy)
		{
			for (int iz = startPos.z; iz < endPos.z; ++iz)
			{
				for (int ix = startPos.x; ix < endPos.x; ++ix)
				{
					BlockPos pos(ix, iy, iz);
					int blockID = pChunkCollection->getBlockId(pos);

					if (blockID <= 0)
						continue;

					Block* pBlock = BlockManager::sBlocks[blockID];
					if (!pBlock)
						continue;

					pBlock->isOpaqueCube();

					if (pBlock->hasTileEntity())
					{
						TileEntity* ptileEntity = pChunkCollection->getBlockTileEntity(pos);

						if (TileEntityRenderMgr::Instance()->hasSpecialRenderer(ptileEntity))
						{
							m_entities_section.insert(ptileEntity);
						}
					}

					pRenderBlocks->renderBlockByRenderType(pBlock, pos);
					renderBlockCnt++;
				}
			}
		}
		
		// caculate the rasterize mesh.
		if (!m_rasterizerMesh)
			m_rasterizerMesh = LordNew RasterizerMesh();
		GenSectionLod genLod(m_rasterizerMesh, blockPos/16);
		genLod.generate(pChunkCollection->blocks);

		// debug show the rasterizerMesh.
#ifdef SHOW_RASTERIZER_RENDER
		if (!rasterRenderable)
		{
			rasterRenderable = TessManager::Instance()->createRasterizerRenderable();
		}
		int quadCount = m_rasterizerMesh->m_rasterVertex.size() / 4;
		rasterRenderable->beginPolygon();
		LineVertexFmt* pVertex;
		rasterRenderable->getLockPtr(quadCount, pVertex);
		for (int i = 0; i < quadCount * 4; ++i)
		{
			Vector3 position = m_rasterizerMesh->m_rasterVertex[i].GetPosition();
			pVertex[i].pos = position + blockPos;
			pVertex[i].color = 0x7FFF007F;
		}
		rasterRenderable->endPolygon();
#endif


		TileEntitySet tileEntities_new = m_entities_section;	// 1,[ABCD]  2,[AB]
		for (auto it : tileEntities_old)
			tileEntities_new.erase(it);							// 1,[CD]  2[ ]
		m_entities_world.insert(tileEntities_new.begin(), tileEntities_new.end());
		for (auto it : m_entities_section)
			tileEntities_old.erase(it);
		for (auto it : tileEntities_old)
			m_entities_world.erase(it);


		LordSafeDelete(pRenderBlocks);
		isInitialized = true;
		// finishUpdateRender();
		nResult = true;
	} while (0);

	LordSafeDelete(pChunkCollection);

	return nResult;
}

bool SectionRenderer::updateSpecifyPos(const BlockPos& pos, TessRenderable* render)
{
	bool nResult = false;
	ChunkCollection* pChunkCollection = NULL;

	do
	{
		BlockPos startPos = blockPos;
		BlockPos endPos = startPos + 16;
		int extend = 1;
		pChunkCollection = LordNew ChunkCollection(worldObj, startPos - extend, endPos + extend, extend);

		ERROR_BREAK(pChunkCollection);

		if (pChunkCollection->isInAsync())
		{
			break;
		}

		if (pChunkCollection->extendedLevelsInChunkCollection())
		{
			nResult = true;
			break;
		}

		ChunkPtr pChunk = worldObj->getChunkFromBlockCoords(blockPos.x, blockPos.z);
		if (!pChunk->m_isPrepared || pChunk->gapLightingUpdated() || pChunk->needRebuildAll())
		{
			Blockman::Instance()->m_lightTransferTrhead->addTask(pChunk);
			//ChunkRebuilder::Instance()->rebuildChunk(blockPos.x >> 4, blockPos.z >> 4, worldObj->getChunkService());
			break;
		}

		RenderBlocks* pRenderBlocks = LordNew RenderBlocks(pChunkCollection, this, Vector3(blockPos) + 8.f);
		render->beginPolygon();
		pRenderBlocks->setSpecialRenderable(render);

		int blockID = pChunkCollection->getBlockId(pos);
		ERROR_BREAK(blockID > 0);

		Block* pBlock = BlockManager::sBlocks[blockID];
		ERROR_BREAK(pBlock);

		pRenderBlocks->renderBlockByRenderType(pBlock, pos, true);
		render->setVertexAlpha(0.4f);
		render->endPolygon();

		LordSafeDelete(pRenderBlocks);
		nResult = true;
	} while (0);

	LordSafeDelete(pChunkCollection);

	return nResult;
}

void SectionRenderer::render(const Vector3& eyePos)
{
	TessManager* tessManager = TessManager::Instance();
	for (TessRenderableLst::iterator it = render_tessLst.begin(); it != render_tessLst.end(); ++it)
	{
		TessRenderable* renderable = *it;
		float dx = eyePos.x - blockPos.x;
		float dz = eyePos.z - blockPos.z;
		float lensqr = dx * dx + dz * dz;
		if (renderable->getpass() == TRP_SOLID_NOMIP &&	lensqr > 64.f * 64.f)
			continue;

		tessManager->addTessRenderable(renderable);
	}
}

TessRenderable* SectionRenderer::getRenderable(Texture* tex, TESS_RENDER_PASS pass, int quadNum)
{
	TessRenderable* pResult = NULL;
	for (TessRenderableLst::iterator it = logic_tessLst.begin(); it != logic_tessLst.end(); ++it)
	{
		if ((*it)->getTexture() != tex)
			continue;
		if ((*it)->getRenderGroup() != pass)
			continue;
		if ((*it)->getFreeQuadNum() < quadNum)
			continue;

		pResult = *it;
	}

	if (!pResult)
	{
		pResult = TessManager::Instance()->createTessRenderable(tex, pass);
		pResult->beginPolygon();
		logic_tessLst.push_back(pResult);
	}
	pResult->setPosition(blockPos);
	return pResult;
}

bool SectionRenderer::needUpdateSkyLight()
{
	ChunkPtr pChunk = worldObj->getChunkFromChunkCoords(blockPos.x >> 4, blockPos.z>> 4);
	if (pChunk && pChunk->gapLightingUpdated())
		return true;
	return false;
}

bool SectionRenderer::updateSkyLight()
{
	auto pChunk = std::static_pointer_cast<ChunkClient>(worldObj->getChunkFromChunkCoords(blockPos.x >> 4, blockPos.z >> 4));
	if (pChunk)
	{
		pChunk->updateSkylight();
		pChunk->updateBlocklight();
		return !pChunk->gapLightingUpdated();
	}
	return false;
}

SetVisibility* SectionRenderer::getSectionVisibility()
{
	if (m_visibilityCache)
		return m_visibilityCache;

	if (!worldObj->chunkExists(blockPos.x >> 4, blockPos.z >> 4))
	{
		return m_visibilityCache;
	}
	auto pChunk = std::static_pointer_cast<ChunkClient>(worldObj->getChunkFromChunkCoords(blockPos.x >> 4, blockPos.z >> 4));
	//if (pChunk && pChunk->m_posX == m_chunkPos.x && pChunk->m_posZ == m_chunkPos.y)
	if (pChunk && (pChunk->m_posX != m_chunkPos.x || pChunk->m_posZ != m_chunkPos.y))
	{
		m_chunkPos = Vector2i(pChunk->m_posX, pChunk->m_posZ);
		m_visibilityCache = pChunk->getSetVisibility(blockPos.y >> 4);
	}
	return m_visibilityCache;
}

}
