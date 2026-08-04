/********************************************************************
filename: 	FontRenderQueue.h
file path:	dev\engine\Src\Core\Tessolator\

version:	1
author:		ajohn
company:	supernano
datetime:   2017-11-01
*********************************************************************/

#ifndef __LORD_FONT_RENDERQUEUE_H__
#define __LORD_FONT_RENDERQUEUE_H__

#include "Core.h"
#include "Tessolator/TessHeader.h"
#include "Render/RenderInput.h"
#include "Resource/Resource.h"

namespace LORD
{

class FontRenderable;
typedef list<FontRenderable*>::type FontRenderList;

class FontRenderQueue : public ObjectAlloc
{
public:
	FontRenderQueue();
	~FontRenderQueue();

	void setShaderProgram(ShaderProgram* s);
	void setRenderInput(const RenderInput::VertexElementList& elements);
	SamplerState* getSamplerState() { return m_samplerState; }
	void setSamplerState(const SamplerState::SamplerDesc& desc);
	void setBlendState(const BlendState::BlendDesc& desc);
	void setDepthState(DepthStencilState::DepthStencilDesc desc);
	void setRasterizerState(const RasterizerState::RasterizerDesc& desc);

	void beginRender() { m_renderables.clear(); }
	void endRender(bool isLine, const Matrix4* viewproj = NULL);

	void addRenderable(FontRenderable* renderable);

protected:
	void _render(Renderer* pRender, FontRenderable* pRenderable, bool isLine);

protected:
	RenderInput*		m_renderInput;
	ShaderProgram*		m_shaderProgram;
	SamplerState*		m_samplerState;
	SamplerState*		m_samplerState_lightmap;
	BlendState*			m_blendState;
	DepthStencilState*	m_depthStateWithZTest;
	DepthStencilState*	m_depthStateWithoutZTest;
	RasterizerState*	m_rasterizerState;
	Texture*			m_renderTexture;
	int					m_sPMatMVPIndex;
	int					m_sPTexTextureIndex;

	FontRenderList		m_renderables;
};

}
#endif
