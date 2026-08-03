/********************************************************************
filename: 	TileEntityRenderMgr.h
file path:	dev\client\Src\Blockman\cTileEntity

version:	1
author:		ajohn
company:	supernano
date:		2017-08-08
*********************************************************************/
#ifndef __TILEENTITY_RENDER_MANAGER_HEAD__
#define __TILEENTITY_RENDER_MANAGER_HEAD__

#include "Core.h"
using namespace LORD;

namespace BLOCKMAN
{

class TileEntity;
class TileEntityRender;
class World;
class EntityLivingBase;
class TileEntityRenderable;

class TileEntityRenderMgr : public Singleton<TileEntityRenderMgr>, public ObjectAlloc
{
	typedef map<int, TileEntityRender*>::type TileEntityRenderMap;
	typedef map<TileEntity*, TileEntityRenderable*>::type TileEntityRenderableMap;
	typedef list<TessRenderable*>::type TessRenderables;
	typedef list<FontRenderable*>::type FontRenderables;
protected:
	/** A mapping of TileEntitySpecialRenderers used for each TileEntity that has one */
	TileEntityRenderMap tileEntityRenderMap;
	TileEntityRenderMap tileEntityRenderSelector;

	TileEntityRenderableMap tileEntityRenderableMap;
	
	/** The FontRenderer instance used by the TileEntityRenderer */
	// FontRenderer fontRenderer;

	/** for block renders. */
	TessRenderables m_blockRender;
	TessRenderables m_freeBlockRender;
	TessRenderables m_fullBlockRender;

	/** for sign texts. */
	FontRenderables m_fontRender;
	FontRenderables m_freeFontRender;
	FontRenderables m_fullFontRender;

public:
	/** Reference to the World object. */
	World* worldObj = nullptr;
	EntityLivingBase* entityLivingPlayer = nullptr;
	float playerYaw = 0.f;
	float playerPitch = 0.f;

	/** The player's X position in this rendering context */
	Vector3 playerPos;
	
public:
	TileEntityRenderMgr();
	~TileEntityRenderMgr();
	
	/** Returns the TileEntitySpecialRenderer used to render this TileEntity class, or null if it has no special renderer */
	TileEntityRender* getSpecialRendererForClass(int type);

	TileEntityRenderable* getTileEntityRenderable(TileEntity* pEntity);

	/** Returns true if this TileEntity instance has a TileEntitySpecialRenderer associated with it, false otherwise. */
	bool hasSpecialRenderer(TileEntity* pTileEntity);

	/** Returns the TileEntitySpecialRenderer used to render this TileEntity instance, or null if it has no special renderer */
	TileEntityRender* getSpecialRendererForEntity(TileEntity* pTileEntity);

	/** Returns the FontRenderable by texture */
	FontRenderable* getFontRenderable(Texture* fontTex, bool zTestEnabled);

	/**  Caches several render-related references, including the active World, RenderEngine, FontRenderer, and the camera-
	* bound EntityLiving's interpolated pitch, yaw and position. */
	void cacheActiveRenderInfo(World* pWorld, EntityLivingBase* pLiving, float rdt);
	
	/** Render this TileEntity at its current position from the player */
	void renderTileEntity(TileEntity* pTileEntity, float rdt);
	
	/** Render this TileEntity at a given set of coordinates */
	void renderTileEntityAt(TileEntity* pTileEntity, const Vector3& pos, int bright, float rdt);

	/** Render this TileEntity to TessRender. */
	void renderTileEntityToTessRender(TileEntity* pTileEntity, const Vector3& pos, int bright, TessRenderable* pRenderable);

	/** Sets the world used by all TileEntitySpecialRender instances and notifies them of this change. */
	void setWorld(World* pWorld);

	/** get Tess renderable.*/
	TessRenderable* getTessRenderable(Texture* texture, TESS_RENDER_PASS pass, int quadNum);

	void beginRender();
	void endRender();
};

}

#endif