/********************************************************************
filename: 	ItemRenderQueue.h
file path:	dev\engine\Src\Core\Tessolator\

version:	1
author:		ajohn
company:	supernano
datetime:   2017-05-04
*********************************************************************/

#ifndef __LORD_ITEM_RENDERQUEUE_H__
#define __LORD_ITEM_RENDERQUEUE_H__

#include "Core.h"
#include "Tessolator/TessHeader.h"
#include "Render/RenderInput.h"
#include "Resource/Resource.h"

namespace LORD
{

class ItemRenderable;
typedef list<ItemRenderable*>::type ItemRenderList;

class ItemRenderQueue : public ObjectAlloc
{
public:
	ItemRenderQueue(ITEM_RENDER_PASS rg);
	virtual ~ItemRenderQueue();

	virtual void setShaderProgram(ShaderProgram* s);
	void setRenderInput(const RenderInput::VertexElementList& elements);
	// SamplerState* getSamplerState() { return m_samplerState; }

	// void setSamplerState(const SamplerState::SamplerDesc& desc);
	void setBlendState(const BlendState::BlendDesc& desc);
	void setDepthState(const DepthStencilState::DepthStencilDesc& desc);
	void setRasterizerState(const RasterizerState::RasterizerDesc& desc);

	void beginRender() { m_renderables.clear(); }
	virtual void endRender(bool isLine, const Matrix4* viewproj = NULL);

	void addRenderable(ItemRenderable* renderable);
	ITEM_RENDER_PASS getRenderGroup() const { return m_renderGroup; }

protected:
	virtual void _render(Renderer* pRender, ItemRenderMesh* pItemMesh, bool isLine);

protected:
	RenderInput*		m_renderInput;
	ShaderProgram*		m_shaderProgram;
	// SamplerState*		m_samplerState;
	BlendState*			m_blendState;
	DepthStencilState*	m_depthState;
	RasterizerState*	m_rasterizerState;

	int					m_sPMatMVPIndex;
	int					m_spMatWorldIndex;
	int					m_spVecFogIndex;
	int					m_spCameraPosIndex;
	int					m_spExtraColorIndex;
	int					m_spFogColorIndex;

	ITEM_RENDER_PASS	m_renderGroup;
	ItemRenderList		m_renderables;
};

}
#endif