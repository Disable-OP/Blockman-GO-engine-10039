#include "Core.h"
#include "TessRenderQueue.h"
#include "TessRenderable.h"
#include "TessManager.h"

namespace LORD
{

TessRenderQueue::TessRenderQueue(TESS_RENDER_PASS rg)
	: m_renderInput(NULL)
	, m_shaderProgram(NULL)
	, m_samplerState(NULL)
	, m_samplerState_lightmap(NULL)
	, m_currentBlendState(NULL)
	, m_blendState(NULL)
	, m_blendState2(NULL)
	, m_currentDepthState(NULL)
	, m_depthState(NULL)
	, m_depthState2(NULL)
	, m_rasterizerState(NULL)
	, m_sPMatMVPIndex(0)
	, m_spSectionPos(0)
	, m_spVecFogParamIndex(0)
	, m_sPTexTextureIndex(0)
	, m_spCameraPos(0)
	, m_sPTexLightMapIndex(0)
	, m_sPUVTimeIdndex(0)
	, m_spFogColorIndex(0)
	, m_renderGroup(rg)
	, m_renderTexture(NULL)
{ }

TessRenderQueue::~TessRenderQueue()
{
	if (m_renderInput)
		m_renderInput->unbindGPUBuffers();
	LordSafeDelete(m_renderInput);
	LordSafeDelete(m_samplerState);
	LordSafeDelete(m_samplerState_lightmap);
	LordSafeDelete(m_blendState);
	LordSafeDelete(m_blendState2);
	LordSafeDelete(m_depthState);
	LordSafeDelete(m_depthState2);
	LordSafeDelete(m_rasterizerState);
}

void TessRenderQueue::setShaderProgram(ShaderProgram* s)
{
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

	LordAssert(s);
	m_shaderProgram = s;
	m_sPMatMVPIndex = m_shaderProgram->getParamPhysicsIndex("matWVP");
	m_spSectionPos = m_shaderProgram->getParamPhysicsIndex("sectionPos");
	m_spVecFogParamIndex = m_shaderProgram->getParamPhysicsIndex("fogParam");
	m_spCameraPos = m_shaderProgram->getParamPhysicsIndex("cameraPos");
	m_sPTexLightMapIndex = m_shaderProgram->getParamPhysicsIndex("texSampler_lightmap");
	m_sPTexTextureIndex = m_shaderProgram->getParamPhysicsIndex("texSampler_texture");
	m_spFogColorIndex = m_shaderProgram->getParamPhysicsIndex("fogColor");
	if (hasUVAnimation())
		m_sPUVTimeIdndex = m_shaderProgram->getParamPhysicsIndex("uvTime");
}

void TessRenderQueue::setSamplerState(const SamplerState::SamplerDesc& desc)
{
	Renderer* pRenderer = Renderer::Instance();
	LordAssert(pRenderer);

	m_samplerState = pRenderer->createSamplerState(desc);
}

void TessRenderQueue::setSamplerState_lightmap(const SamplerState::SamplerDesc& desc)
{
	Renderer* pRenderer = Renderer::Instance();
	LordAssert(pRenderer);

	m_samplerState_lightmap = pRenderer->createSamplerState(desc);
}

void TessRenderQueue::setBlendState(const BlendState::BlendDesc& desc)
{
	Renderer* pRenderer = Renderer::Instance();
	LordAssert(pRenderer);

	m_blendState = pRenderer->createBlendState(desc);
	m_currentBlendState = m_blendState;
}

void TessRenderQueue::setBlendState2(const BlendState::BlendDesc& desc)
{
	Renderer* pRenderer = Renderer::Instance();
	LordAssert(pRenderer);

	m_blendState2 = pRenderer->createBlendState(desc);
}

void TessRenderQueue::changeCurrentState(bool zfinished)
{
	if (m_blendState2 && m_depthState2 && zfinished)
	{
		m_currentBlendState = m_blendState2;
		m_currentDepthState = m_depthState2;
	}
	else
	{
		m_currentBlendState = m_blendState;
		m_currentDepthState = m_depthState;
	}
}

void TessRenderQueue::setDepthState(const DepthStencilState::DepthStencilDesc& desc)
{
	Renderer* pRenderer = Renderer::Instance();
	LordAssert(pRenderer);

	m_depthState = pRenderer->createDepthStencilState(desc);
	m_currentDepthState = m_depthState;
}

void TessRenderQueue::setDepthState2(const DepthStencilState::DepthStencilDesc& desc)
{
	Renderer* pRenderer = Renderer::Instance();
	LordAssert(pRenderer);

	m_depthState2 = pRenderer->createDepthStencilState(desc);
}

void TessRenderQueue::setRasterizerState(const RasterizerState::RasterizerDesc& desc)
{
	Renderer* pRenderer = Renderer::Instance();
	LordAssert(pRenderer);

	m_rasterizerState = pRenderer->createRasterizerState(desc);
}

void TessRenderQueue::setRenderInput(const RenderInput::VertexElementList& elements)
{
	Renderer* pRenderer = Renderer::Instance();
	LordAssert(pRenderer);
	LordAssert(m_shaderProgram);

	m_renderInput = pRenderer->createRenderInput(elements, m_shaderProgram);
}

void TessRenderQueue::addRenderable(TessRenderable* renderable)
{
	LordAssert(renderable);
	m_renderables.push_back(renderable);
}

void TessRenderQueue::endRender(bool isLine, const Matrix4* viewproj)
{
	//flush all renderalbes to draw.
	if (m_renderables.empty())
		return;

	LordAssert(m_shaderProgram);
	LordAssert(m_renderInput);
	LordAssert(m_lightMap);
	m_renderTexture = NULL;
	Renderer* pRender = Renderer::Instance();
	m_shaderProgram->bind();

	int frameCount = TessManager::Instance()->getFrameCount();
	Camera* camera = SceneManager::Instance()->getMainCamera();
	Matrix4 viewProjMat = viewproj ? *viewproj : camera->getViewProjMatrix();
	const Vector3* camerapos = camera->getPositionPtr();
	const Vector4& fog_params = SceneManager::Instance()->getFogParam();
	const Color& fog_color = SceneManager::Instance()->getFogColor();
	const Vector3& mainLightDir = SceneManager::Instance()->getMainLightDir();
	const Color& mainLightColor = SceneManager::Instance()->getMainLightColor();
	const Color& subLightColor = SceneManager::Instance()->getSubLightColor();
	Vector4 uv_params = Vector4::ZERO;
	Vector4 sectionPos = Vector4::ZERO;
	
	// vs
	m_shaderProgram->setUniform(m_sPMatMVPIndex, &viewProjMat, SPT_MAT4, 1);
	m_shaderProgram->setUniform(m_spSectionPos, &sectionPos, SPT_VEC4, 1);
	m_shaderProgram->setUniform(m_spVecFogParamIndex, &fog_params, SPT_VEC4, 1);
	if (hasUVAnimation())
		m_shaderProgram->setUniform(m_sPUVTimeIdndex, (void*)&uv_params, SPT_VEC4, 1);
	m_shaderProgram->setUniform(m_spCameraPos, (void*)camerapos, SPT_VEC3, 1);

	// ps
	m_shaderProgram->setUniform(m_sPTexTextureIndex, (void*)&TEXTURE_UNIT0, SPT_TEXTURE, 1);
	m_shaderProgram->setUniform(m_sPTexLightMapIndex, (void*)&TEXTURE_UNIT1, SPT_TEXTURE, 1);
	m_shaderProgram->setUniform(m_spFogColorIndex, &fog_color, SPT_VEC4, 1);

	m_shaderProgram->bindUniforms();

	TessRenderable* pRenderable = *(m_renderables.begin());
	LordAssert(pRenderable);
	Texture* texture = pRenderable->getTexture();
	pRender->setTexture(0, texture);
	texture->setSamplerState(m_samplerState);

	pRender->setDepthStencilState(m_currentDepthState);
	pRender->setRasterizerState(m_rasterizerState);
	pRender->setBlendState(m_currentBlendState);

	pRender->setTexture(1, m_lightMap);
	m_lightMap->setSamplerState(m_samplerState_lightmap);

	for (TessRenderList::iterator it = m_renderables.begin(); it != m_renderables.end(); ++it)
	{
		TessRenderable* pRenderable = *it;
		LordAssert(pRenderable);
		sectionPos = Vector4(pRenderable->getPosition(), 1.0f);
		m_shaderProgram->setUniform(m_spSectionPos, (void*)&sectionPos, SPT_VEC4, 1);
		if (hasUVAnimation())
		{
			uv_params = pRenderable->updateUVParam(frameCount);
			m_shaderProgram->setUniform(m_sPUVTimeIdndex, (void*)&uv_params, SPT_VEC4, 1);
			
		}
		m_shaderProgram->bindUniforms();
		_render(pRender, pRenderable, isLine);
	}

	pRender->setTexture(0, NULL);
	pRender->setTexture(1, NULL);
}

void TessRenderQueue::_render(Renderer* pRender, TessRenderable* pRenderable, bool isLine)
{
	LordAssert(pRender);
	LordAssert(pRenderable);

	ui16 quadNum = pRenderable->getQuadNum();
	if (quadNum == 0)
		return;

	Texture* texture = pRenderable->getTexture();
	if (texture != m_renderTexture)
	{
		pRender->setTexture(0, texture);
		m_renderTexture = texture;
		texture->setSamplerState(m_samplerState);
	}

	m_renderInput->unbindGPUBuffers();
	m_renderInput->bindGPUBuffers(pRenderable->getVertexBuffer(), sizeof(TessVertexFmt), pRenderable->getIndexBuffer(), sizeof(ui16));
	m_renderInput->setVertexCount(quadNum * 4);
	m_renderInput->setIndexCount(quadNum * (isLine ? 8 : 6));
	m_renderInput->setTopologyType(isLine ? RenderInput::TT_LINELIST : RenderInput::TT_TRIANGLELIST);
	pRender->render(m_renderInput);
}


}