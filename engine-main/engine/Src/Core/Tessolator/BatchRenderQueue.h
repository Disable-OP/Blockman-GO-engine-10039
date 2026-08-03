/********************************************************************
filename: 	BatchRenderQueue.h
file path:	dev\engine\Src\Core\Tessolator\

version:	1
author:		ajohn
company:	supernano
datetime:   2017-07-04
*********************************************************************/

#ifndef __LORD_STANDARD_RENDERQUEUE_H__
#define __LORD_STANDARD_RENDERQUEUE_H__

#include "Core.h"
#include "Tessolator/TessHeader.h"
#include "Render/RenderInput.h"
#include "Resource/Resource.h"

namespace LORD
{

class BatchRenderable;
typedef list<BatchRenderable*>::type BatchRenderList;

class BatchRenderQueue : public ObjectAlloc
{
public:
	BatchRenderQueue(BATCH_RENDER_PASS rg);
	~BatchRenderQueue();

	void setShaderProgram(ShaderProgram* s);
	void setRenderInput(const RenderInput::VertexElementList& elements);
	SamplerState* getSamplerState() { return m_samplerState; }
	void setSamplerState(const SamplerState::SamplerDesc& desc);
	void setBlendState(const BlendState::BlendDesc& desc);
	void setDepthState(const DepthStencilState::DepthStencilDesc& desc);
	void setRasterizerState(const RasterizerState::RasterizerDesc& desc);

	void beginRender() { m_renderables.clear(); }
	void endRender(bool isLine, const Matrix4* viewproj = NULL);

	void addRenderable(BatchRenderable* renderable);
	BATCH_RENDER_PASS getRenderGroup() const { return m_renderGroup; }

protected:
	void _render(Renderer* pRender, BatchRenderable* pRenderable, bool isLine);

protected:
	RenderInput*		m_renderInput;
	ShaderProgram*		m_shaderProgram;
	SamplerState*		m_samplerState;
	BlendState*			m_blendState;
	DepthStencilState*	m_depthState;
	RasterizerState*	m_rasterizerState;
	Texture*			m_renderTexture;
	int					m_sPMatMVPIndex;
	int					m_spFogParamIndex;
	int					m_spCameraPosIndex;
	int					m_spFogColorIndex;
	int					m_sPTexTextureIndex;

	BATCH_RENDER_PASS m_renderGroup;
	BatchRenderList	m_renderables;
};

}
#endif