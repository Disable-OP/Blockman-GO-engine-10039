/********************************************************************
filename: 	FxRenderQueue.h
file path:	dev\engine\Src\Core\Tessolator\

version:	1
author:		ajohn
company:	supernano
datetime:   2017-08-01
*********************************************************************/

#ifndef __LORD_FX_RENDERQUEUE_H__
#define __LORD_FX_RENDERQUEUE_H__

#include "Core.h"
#include "Tessolator/TessHeader.h"
#include "Render/RenderInput.h"
#include "Resource/Resource.h"

namespace LORD
{

class FxRenderable;
typedef list<FxRenderable*>::type FxRenderList;

class FxRenderQueue : public ObjectAlloc
{
public:
	FxRenderQueue(FX_RENDER_PASS rg);
	~FxRenderQueue();

	void setShaderProgram(ShaderProgram* s);
	void setRenderInput(const RenderInput::VertexElementList& elements);
	SamplerState* getSamplerState() { return m_samplerState; }
	void setSamplerState(const SamplerState::SamplerDesc& desc);
	void setSamplerState_lightmap(const SamplerState::SamplerDesc& desc);
	void setBlendState(const BlendState::BlendDesc& desc);
	void setDepthState(const DepthStencilState::DepthStencilDesc& desc);
	void setRasterizerState(const RasterizerState::RasterizerDesc& desc);

	void beginRender() { m_renderables.clear(); }
	void endRender(bool isLine, const Matrix4* viewproj = NULL);

	void addRenderable(FxRenderable* renderable);
	FX_RENDER_PASS getRenderGroup() const { return m_renderGroup; }

protected:
	void _render(Renderer* pRender, FxRenderable* pRenderable, bool isLine);

protected:
	RenderInput*		m_renderInput;
	ShaderProgram*		m_shaderProgram;
	SamplerState*		m_samplerState;
	SamplerState*		m_samplerState_lightmap;
	BlendState*			m_blendState;
	DepthStencilState*	m_depthState;
	RasterizerState*	m_rasterizerState;
	Texture*			m_renderTexture;
	
	int					m_sPMatMVPIndex;
	int					m_spVecFogIndex;
	int					m_spFogColorIndex;
	int					m_spCameraPosIndex;
	int					m_spMainLightColorIndex;
	int					m_spSubLightColorIndex;
	int					m_sPTexTextureIndex;


	FX_RENDER_PASS		m_renderGroup;
	FxRenderList		m_renderables;
};

}
#endif
