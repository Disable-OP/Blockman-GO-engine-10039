/********************************************************************
filename: 	TessRenderQueue.h
file path:	dev\engine\Src\Core\Tessolator\

version:	1
author:		ajohn
company:	supernano
datetime:   2016-12-14
*********************************************************************/

#ifndef __LORD_TESSOLATOR_RENDERQUEUE_H__
#define __LORD_TESSOLATOR_RENDERQUEUE_H__

#include "Core.h"
#include "Tessolator/TessHeader.h"
#include "Render/RenderInput.h"
#include "Resource/Resource.h"

namespace LORD
{
class TessRenderable;
typedef list<TessRenderable*>::type TessRenderList;

class TessRenderQueue : public ObjectAlloc
{
public:
	TessRenderQueue(TESS_RENDER_PASS rg);
	~TessRenderQueue();

	void setShaderProgram(ShaderProgram* s);
	void setRenderInput(const RenderInput::VertexElementList& elements);
	SamplerState* getSamplerState() { return m_samplerState; }
	void setSamplerState(const SamplerState::SamplerDesc& desc);
	void setSamplerState_lightmap(const SamplerState::SamplerDesc& desc);
	void setBlendState(const BlendState::BlendDesc& desc);
	void setBlendState2(const BlendState::BlendDesc& desc);
	void setDepthState(const DepthStencilState::DepthStencilDesc& desc);
	void setDepthState2(const DepthStencilState::DepthStencilDesc& desc);
	void setRasterizerState(const RasterizerState::RasterizerDesc& desc);
	void setLightmap(Texture* lightmap) { m_lightMap = lightmap; };
	void changeCurrentState(bool zfinished);

	void beginRender() { m_renderables.clear(); }
	void endRender(bool isLine, const Matrix4* viewproj = NULL);
	bool hasUVAnimation() const { return m_renderGroup == TRP_SOLID_UV || m_renderGroup == TRP_TRANSPARENT_UV || m_renderGroup == TRP_TRANSPARENT_3DUV; }

	void addRenderable(TessRenderable* renderable);
	TESS_RENDER_PASS getRenderGroup() const { return m_renderGroup; }
	
protected:
	void _render(Renderer* pRender, TessRenderable* pRenderable, bool isLine);

protected:
	RenderInput*		m_renderInput;
	ShaderProgram*		m_shaderProgram;
	SamplerState*		m_samplerState;
	SamplerState*		m_samplerState_lightmap;
	BlendState*			m_currentBlendState;
	BlendState*			m_blendState;
	BlendState*			m_blendState2;
	DepthStencilState*	m_currentDepthState;
	DepthStencilState*	m_depthState;
	DepthStencilState*	m_depthState2;
	RasterizerState*	m_rasterizerState;
	Texture*			m_renderTexture;
	Texture*			m_lightMap;
	
	/*
	uniform mat4 matWVP;
	uniform vec4 sectionPos;
	uniform vec4 fogParam[3];
	uniform vec3 cameraPos;
	uniform vec3 mainLightDir;
	uniform vec4 mainLightColor;
	uniform vec3 subLightDir;
	uniform vec4 subLightColor;
	uniform vec4 ambient;
	*/

	// vs
	int					m_sPMatMVPIndex;
	int					m_spSectionPos;
	int					m_spVecFogParamIndex;
	int					m_sPUVTimeIdndex;
	int					m_spCameraPos;

	// ps
	int					m_sPTexLightMapIndex;
	int					m_sPTexTextureIndex;
	int					m_spFogColorIndex;

	TESS_RENDER_PASS	m_renderGroup;
	TessRenderList		m_renderables;
};

}
#endif