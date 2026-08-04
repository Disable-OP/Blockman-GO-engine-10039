/********************************************************************
filename: 	EnchantRenderQueue.h
file path:	dev\engine\Src\Core\Tessolator\

version:	1
author:		ajohn
company:	supernano
datetime:   2017-08-22
*********************************************************************/

#ifndef __LORD_ENCHANT_RENDERQUEUE_H__
#define __LORD_ENCHANT_RENDERQUEUE_H__

#include "ItemRenderQueue.h"
#include "SkinedRenderQueue.h"
#include "SkinedRenderable.h"

namespace LORD
{

class EnchantItemRenderQueue : public ItemRenderQueue
{
public:
	EnchantItemRenderQueue();
	~EnchantItemRenderQueue();

	SamplerState* getSamplerState() { return m_samplerState; }
	void setSamplerState(const SamplerState::SamplerDesc& desc);
	void setDepthState_noDepth(const DepthStencilState::DepthStencilDesc& desc);
	
	/** implement override functions from ItemRenderQuque. */
	virtual void setShaderProgram(ShaderProgram* s);
	virtual void endRender(bool isLine, const Matrix4* viewproj = NULL) override;

protected:
	void _render(Renderer* pRender, ItemRenderable* pRenderable, bool isLine);

protected:
	int					m_spMatTextureIndex1;
	int					m_spMatTextureIndex2;
	int					m_spTexTextureIndex;
	SamplerState*		m_samplerState;
	DepthStencilState*	m_depthState_noDepth;
	Texture*			m_renderTexture;
};

class EnchantSkinedRenderable : public SkinedRenderable
{
public:
	EnchantSkinedRenderable() {}
	~EnchantSkinedRenderable() {}

	void setTextureAtlas(const Vector4& atlas) { m_texAtlas = atlas; }
	const Vector4& getTextureAtlas() const { return m_texAtlas; }

protected:
	Vector4 m_texAtlas;  // for map atlas uv to [0, 1].
};

class EnchantSkinedRenderQueue : public SkinedRenderQueue
{
public:
	EnchantSkinedRenderQueue();
	~EnchantSkinedRenderQueue();

	/** implement override functions from ItemRenderQuque. */
	virtual void setShaderProgram(ShaderProgram* s);
	virtual void endRender(bool isLine, const Matrix4* viewproj = NULL) override;
	void setDepthState_noDepth(const DepthStencilState::DepthStencilDesc& desc);
	
protected:
	int					m_spMatTextureIndex1;
	int					m_spMatTextureIndex2;
	int					m_spVecTextureAtlas;
	DepthStencilState*	m_depthState_noDepth;
};

}
#endif