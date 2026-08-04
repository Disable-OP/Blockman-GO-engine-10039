/********************************************************************
filename: 	SectionRenderer.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-12-2
*********************************************************************/
#ifndef __SECTION_RENDERER_HEADER__
#define __SECTION_RENDERER_HEADER__

#include "BM_TypeDef.h"
#include "BM_Container_def.h"

namespace BLOCKMAN
{

class World;
class SetVisibility;
class Chunk;
class RasterizerMesh;

typedef list<TessRenderable*>::type TessRenderableLst;

class SectionRenderer : public ObjectAlloc
{
public:
	/** Reference to the World object. */
	World* worldObj = nullptr;
	static int chunksUpdated;
	
	BlockPos blockPos;
	BlockPos posMinus;
	BlockPos posClip;
	BlockPos posPlus;

	/** Boolean for whether this renderer needs to be updated or not */
	bool needsUpdate = false;

	/** Axis aligned bounding box */
	Box rendererBoundingBox;

	/** Chunk index */
	int chunkIndex = 0;
	int FailCnt = 0;

	Vector2i m_chunkPos;
	SetVisibility* m_visibilityCache = nullptr;

	/** Is the chunk lit */
	bool isChunkLit = false;
	
	/** prevent BFS (breadth-first search) search this section repeat. */
	int m_frameIndex = 0;
	
	/** All the tile entities that have special rendering code for this chunk */
	TileEntitySet	m_entities_section;
	TileEntitySet&	m_entities_world;	// this set is for GlobalRender.

	/** the rasterizer mesh vertex data: */
	RasterizerMesh* m_rasterizerMesh = nullptr;

protected:

	// static Tessellator tessellator = Tessellator.instance;
	bool isInitialized = false;
	// List tileEntities;

	/** Bytes sent to the GPU */
	int bytesDrawn = 0;
	int renderBlockCnt = 0;

	TessRenderableLst logic_tessLst;
	TessRenderableLst render_tessLst;

#ifdef SHOW_RASTERIZER_RENDER
	RasterizerRenderable* rasterRenderable;
#endif

	void releaseRenderalbes();
	bool finishUpdateRender();
	

public:
	SectionRenderer(World* par1World, TileEntitySet& entities, const BlockPos& pos);
	~SectionRenderer();

	/** Sets a new position for the renderer and setting it up so it can be reloaded with the new data for that position */
	void setPosition(const BlockPos& pos);

	bool setFrameIndex(int frameIdx);

	/** Will update this chunk renderer */
	bool updateRenderer();

	/** will update specify position to render block. */
	bool updateSpecifyPos(const BlockPos& pos, TessRenderable* render);

	void render(const Vector3& eyePos);

	const TessRenderableLst& getRenderables() { return logic_tessLst; }

#ifdef SHOW_RASTERIZER_RENDER
	RasterizerRenderable* getRasterRenderable() { return rasterRenderable; }
#endif

	TessRenderable* getRenderable(Texture* tex, TESS_RENDER_PASS pass, int quadNum);

	/** Returns the distance of this chunk renderer to the entity without performing the final normalizing square root, for performance reasons.*/
	float distanceToEntitySquared(Camera* pCamera) const;

	void checkRebuild();

	/** Marks the current renderer data as dirty and needing to be updated. */
	void markDirty();

	bool needUpdateSkyLight();
	bool updateSkyLight();

	void exchangeRenderables();

	SetVisibility* getSectionVisibility();
};

}

#endif
