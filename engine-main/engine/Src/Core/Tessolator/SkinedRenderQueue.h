/********************************************************************
filename: 	EntityRenderQueue.h
file path:	dev\engine\Src\Core\Tessolator\

version:	1
author:		ajohn
company:	supernano
datetime:   2017-03-09
*********************************************************************/

#ifndef __LORD_ENTITY_RENDERQUEUE_H__
#define __LORD_ENTITY_RENDERQUEUE_H__

#include "Core.h"
#include "Tessolator/TessHeader.h"
#include "Render/RenderInput.h"
#include "Resource/Resource.h"

namespace LORD
{
class SkinedRenderable;
class BatchRenderable;
typedef list<SkinedRenderable*>::type EntityRenderList;
typedef map<Texture*, EntityRenderList>::type EntityRenderBatch;

class SkinedRenderQueue : public ObjectAlloc
{
public:
	SkinedRenderQueue(SKINED_RENDER_PASS rg);
	virtual ~SkinedRenderQueue();

	virtual void setShaderProgram(ShaderProgram* s);
	void setRenderInput(const RenderInput::VertexElementList& elements);
	SamplerState* getSamplerState() { return m_samplerState; }

	void setSamplerState(const SamplerState::SamplerDesc& desc);
	void setBlendState(const BlendState::BlendDesc& desc);
	void setDepthState(const DepthStencilState::DepthStencilDesc& desc);
	void setRasterizerState(const RasterizerState::RasterizerDesc& desc);

	void beginRender() { m_renderables.clear(); }
	virtual void endRender(bool isLine, const Matrix4* viewproj = NULL);
	
	void addRenderable(SkinedRenderable* renderable);
	SKINED_RENDER_PASS getRenderGroup() const { return m_renderGroup; }

protected:
	virtual void _render(Renderer* pRender, SkinedRenderable* pRenderable, bool isLine);
	void batch();
	BatchRenderable* batchPass(Texture* tex);

protected:
	RenderInput*		m_renderInput;
	ShaderProgram*		m_shaderProgram;
	SamplerState*		m_samplerState;
	BlendState*			m_blendState;
	DepthStencilState*	m_depthState;
	RasterizerState*	m_rasterizerState;
	Texture*			m_renderTexture;
	int					m_sPMatMVPIndex;
	int					m_spMatWorldIndex;
	int					m_spVecFogIndex;
	int					m_spFogColorIndex;
	int					m_spCameraPosIndex;
	int					m_spExtraColorIndex;
	int					m_spBoneMatricesIndex;
	int					m_sPTexTextureIndex;

	SKINED_RENDER_PASS	m_renderGroup;
	EntityRenderList	m_renderables;
};

}
#endif