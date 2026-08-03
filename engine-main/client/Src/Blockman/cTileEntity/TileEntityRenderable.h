/********************************************************************
filename: 	TileEntityRenderable.h
file path:	dev\client\Src\Blockman\cTileEntity

version:	1
author:		ajohn
company:	supernano
date:		2017-08-09
*********************************************************************/
#ifndef __TILE_ENTITY_RENDERABLE_HEADER__
#define __TILE_ENTITY_RENDERABLE_HEADER__

#include "Core.h"
using namespace LORD;

namespace BLOCKMAN
{

class TileEntityRender;
class EntityLivingBase;
class ModelBox;
class TextureAtlas;
class TileEntity;
class ModelBase;

class TileEntityRenderable : public ObjectAlloc
{
protected:
	// the render param for main model
	SkinedRenderable*	m_skin = nullptr;
	Texture*			m_texture = nullptr;
	Vector2i			m_texSize;
	ModelBase*			m_model = nullptr;
	Vector4*			m_boneMatrix = nullptr;
	int					m_boneCount = 0;
	int					m_brightness = 0;

	TileEntity*			m_tileEntity = nullptr;
	TileEntityRender*	m_renderer = nullptr;  // the texture,model maybe not equal the renderer's texture,model.
	Matrix4				m_worldMat;
	

protected:
	void compileGeometry();
	void compileTexture();
	void release();

public:
	TileEntityRenderable(TileEntity* pTileEntity);
	virtual ~TileEntityRenderable();

	void setTileEntity(TileEntity* pTileEntity);

	void checkTexutreAndModel(Texture* tex, ModelBase* model);
	void setWorldMatrix(const Matrix4& mat) { m_worldMat = mat; }
	void setBrightness(const Color& b);
	void setColor(const Color& c);
	void updateAnimation();
	
	void updateRender();
	void invalid();
};

}

#endif
