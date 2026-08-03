/********************************************************************
filename: 	TileEntityRenders.h
file path:	dev\client\Src\Blockman\cTileEntity

version:	1
author:		ajohn
company:	supernano
date:		2017-08-08
*********************************************************************/
#ifndef __TILEENTITY_RENDERS_HEAD__
#define __TILEENTITY_RENDERS_HEAD__

#include "Core.h"
using namespace LORD;

namespace BLOCKMAN
{

class TileEntity;
class TileEntityRenderMgr;
class World;
class ModelChest;
class ModelBase;
class ModelSign;
class TileEntityChest;
class TileEntityEnderChest;
class TileEntitySign;
class TileEntityPiston;
class RenderBlocks;

class TileEntityRender : public ObjectAlloc
{
protected:
	/*** The TileEntityRenderer instance associated with this TileEntityRender */
	TileEntityRenderMgr* tileEntityRenderMgr;

public:
	virtual ~TileEntityRender() {}

	virtual void renderTileEntityAt(TileEntity* pEntity, const Vector3& pos, int bright, float rdt) = 0;
	virtual void renderTileEntityToTessRender(TileEntity* pEntity, const Vector3& pos, int bright, TessRenderable* pTarget) = 0;

	virtual Texture* getTexture() = 0;
	virtual ModelBase* getModel() = 0;
	/*** Associate a TileEntityRenderer with this TileEntitySpecialRenderer */
	void setTileEntityRenderer(TileEntityRenderMgr* pTileEntityRenderMgr) { tileEntityRenderMgr = pTileEntityRenderMgr; }
	
	/** Called when the ingame world being rendered changes */
	virtual void onWorldChange(World* pWorld) {}
};

class TEChestRender : public TileEntityRender
{
protected:
	Texture* m_texTrapped_double = nullptr; // = new ResourceLocation("textures/entity/chest/trapped_double.png");
	Texture* m_texChristmas_double = nullptr; // = new ResourceLocation("textures/entity/chest/christmas_double.png");
	Texture* m_texNormal_double = nullptr; // = new ResourceLocation("textures/entity/chest/normal_double.png");
	Texture* m_texTrapped = nullptr; // = new ResourceLocation("textures/entity/chest/trapped.png");
	Texture* m_texChristmas = nullptr; // = new ResourceLocation("textures/entity/chest/christmas.png");
	Texture* m_texNormal = nullptr; // = new ResourceLocation("textures/entity/chest/normal.png");

	Texture* m_texNormal_double_new = nullptr; // = new ResourceLocation("textures/entity/chest/normal_new_double.png");
	Texture* m_texNormal_new = nullptr; // = new ResourceLocation("textures/entity/chest/normal_new.png");
	Texture* m_texTrapped_double_new = nullptr; // = new ResourceLocation("textures/entity/chest/trapped_new_double.png");
	Texture* m_texTrapped_new = nullptr; // = new ResourceLocation("textures/entity/chest/trapped_new.png");

	Texture* m_texCurrent = nullptr;
	ModelChest* m_model = nullptr;

	/** The normal small chest model. */
	ModelChest* chestModel = nullptr; // = new ModelChest();

	/** The large double chest model. */
	ModelChest* largeChestModel = nullptr; // = new ModelLargeChest();

	/** If true, chests will be rendered with the Christmas present textures. */
	bool isChristmas = false;

public:
	TEChestRender();
	~TEChestRender();

	/** implement override functions from TileEntityRender. */
	virtual void renderTileEntityAt(TileEntity* pTileEntity, const Vector3& pos, int bright, float rdt) override;
	virtual void renderTileEntityToTessRender(TileEntity* pTileEntity, const Vector3& pos, int bright, TessRenderable* pTarget) override;
	virtual Texture* getTexture() override { return m_texCurrent; }
	virtual ModelBase* getModel() override { return (ModelBase*)m_model; }

	void renderTileEntityAt_impl(TileEntityChest* pChest, const Vector3& pos, int bright, float rdt);
};

class TEEnderChestRenderer : public TileEntityRender
{
protected:
	Texture* m_texture = nullptr;
	Texture* m_texture_new = nullptr;
	Texture* m_texCurrent = nullptr;
	ModelChest* m_model = nullptr;

public:
	TEEnderChestRenderer();
	~TEEnderChestRenderer();

	/** implement override functions from TileEntityRender. */
	virtual void renderTileEntityAt(TileEntity* pTileEntity, const Vector3& pos, int bright, float rdt) override;
	virtual void renderTileEntityToTessRender(TileEntity* pTileEntity, const Vector3& pos, int bright, TessRenderable* pTarget) override;
	virtual Texture* getTexture() override { return m_texCurrent; }
	virtual ModelBase* getModel() override { return (ModelBase*)m_model; }

	void renderTileEntityAt_impl(TileEntityEnderChest* pChest, const Vector3& pos, int bright, float rdt);
};

class TESignRenderer : public TileEntityRender
{
protected:
	Texture* m_texture = nullptr;
	ModelSign* m_modelPost = nullptr;
	ModelSign* m_modelWall = nullptr;
	ModelSign* m_model = nullptr;

public:
	TESignRenderer();
	~TESignRenderer();

	/** implement override functions from TileEntityRender. */
	virtual void renderTileEntityAt(TileEntity* pTileEntity, const Vector3& pos, int bright, float rdt) override;
	virtual void renderTileEntityToTessRender(TileEntity* pTileEntity, const Vector3& pos, int bright, TessRenderable* pTarget) override;
	virtual Texture* getTexture() override { return m_texture; }
	virtual ModelBase* getModel() override { return (ModelBase*)m_model; }

	void renderTileEntityAt_impl(TileEntitySign* pSign, const Vector3& pos, int bright, float rdt);
	void renderSignText(TileEntitySign* pSign, const Matrix4& matWorld);
};

class TEPistonRenderer : public TileEntityRender
{
protected:
	/** instance of RenderBlocks used to draw the piston base and extension. */
	RenderBlocks* m_blockRender = nullptr;
	Texture* m_texture = nullptr;

public:
	TEPistonRenderer();
	~TEPistonRenderer();

	/** implement override functions from TileEntityRender. */
	virtual void renderTileEntityAt(TileEntity* pTileEntity, const Vector3& pos, int bright, float rdt) override;
	virtual void renderTileEntityToTessRender(TileEntity* pTileEntity, const Vector3& pos, int bright, TessRenderable* pTarget) override {}
	virtual Texture* getTexture() override { return nullptr; }
	virtual ModelBase* getModel() override { return nullptr; }
	virtual void onWorldChange(World* pWorld) override;

	void renderTileEntityAt_impl(TileEntityPiston* piston, const Vector3& pos, int bright, float rdt);
};

}

#endif