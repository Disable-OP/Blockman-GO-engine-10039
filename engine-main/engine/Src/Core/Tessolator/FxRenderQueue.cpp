#include "Core.h"
#include "FxRenderQueue.h"
#include "FxRenderable.h"
#include "TessManager.h"

namespace LORD
{

FxRenderQueue::FxRenderQueue(FX_RENDER_PASS rg)
	: m_renderInput(NULL)
	, m_shaderProgram(NULL)
	, m_samplerState(NULL)
	, m_samplerState_lightmap(NULL)
	, m_blendState(NULL)
	, m_depthState(NULL)
	, m_rasterizerState(NULL)
	, m_sPMatMVPIndex(0)
	, m_spVecFogIndex(0)
	, m_spFogColorIndex(0)
	, m_spCameraPosIndex(0)
	, m_spMainLightColorIndex(0)
	, m_spSubLightColorIndex(0)
	, m_sPTexTextureIndex(0)
	, m_renderGroup(rg)
	, m_renderTexture(NULL)
{ }

FxRenderQueue::~FxRenderQueue()
{
	if (m_renderInput)
		m_renderInput->unbindGPUBuffers();
	LordSafeDelete(m_renderInput);
	LordSafeDelete(m_samplerState);
	LordSafeDelete(m_samplerState_lightmap);
	LordSafeDelete(m_blendState);
	LordSafeDelete(m_depthState);
	LordSafeDelete(m_rasterizerState);
}

void FxRenderQueue::setShaderProgram(ShaderProgram* s)
{
	LordAssert(s);
	m_shaderProgram = s;
	m_sPMatMVPIndex = m_shaderProgram->getParamPhysicsIndex("matWVP");
	m_spVecFogIndex = m_shaderProgram->getParamPhysicsIndex("fogParam");
	m_spFogColorIndex = m_shaderProgram->getParamPhysicsIndex("fogColor");
	m_spCameraPosIndex = m_shaderProgram->getParamPhysicsIndex("cameraPos");
	m_spMainLightColorIndex = m_shaderProgram->getParamPhysicsIndex("mainLightColor");
	m_spSubLightColorIndex = m_shaderProgram->getParamPhysicsIndex("subLightColor");
	m_sPTexTextureIndex = m_shaderProgram->getParamPhysicsIndex("texSampler_texture");
}

void FxRenderQueue::setSamplerState(const SamplerState::SamplerDesc& desc)
{
	Renderer* pRenderer = Renderer::Instance();
	LordAssert(pRenderer);

	m_samplerState = pRenderer->createSamplerState(desc);
}

void FxRenderQueue::setSamplerState_lightmap(const SamplerState::SamplerDesc& desc)
{
	Renderer* pRenderer = Renderer::Instance();
	LordAssert(pRenderer);

	m_samplerState_lightmap = pRenderer->createSamplerState(desc);
}

void FxRenderQueue::setBlendState(const BlendState::BlendDesc& desc)
{
	Renderer* pRenderer = Renderer::Instance();
	LordAssert(pRenderer);

	m_blendState = pRenderer->createBlendState(desc);
}

void FxRenderQueue::setDepthState(const DepthStencilState::DepthStencilDesc& desc)
{
	Renderer* pRenderer = Renderer::Instance();
	LordAssert(pRenderer);

	m_depthState = pRenderer->createDepthStencilState(desc);
}

void FxRenderQueue::setRasterizerState(const RasterizerState::RasterizerDesc& desc)
{
	Renderer* pRenderer = Renderer::Instance();
	LordAssert(pRenderer);

	m_rasterizerState = pRenderer->createRasterizerState(desc);
}

void FxRenderQueue::setRenderInput(const RenderInput::VertexElementList& elements)
{
	Renderer* pRenderer = Renderer::Instance();
	LordAssert(pRenderer);
	LordAssert(m_shaderProgram);

	m_renderInput = pRenderer->createRenderInput(elements, m_shaderProgram);
}

void FxRenderQueue::addRenderable(FxRenderable* renderable)
{
	LordAssert(renderable);
	m_renderables.push_back(renderable);
}

void FxRenderQueue::endRender(bool isLine, const Matrix4* viewproj)
{
	//flush all renderalbes to draw.
	if (m_renderables.empty())
		return;

	LordAssert(m_shaderProgram);
	LordAssert(m_renderInput);
	m_renderTexture = NULL;
	Renderer* pRender = Renderer::Instance();
	m_shaderProgram->bind();

	int frameCount = TessManager::Instance()->getFrameCount();
	Camera* camera = SceneManager::Instance()->getMainCamera();
	Matrix4 projMat = camera->getProjMatrix();
	Matrix4 viewMat = camera->getViewMatrix();
	// keep the viewMat the oritation and ignore the movement.
	viewMat.m30 = viewMat.m31 = viewMat.m32 = 0.f;
	projMat = viewMat * projMat;

	const Vector4 cameraFogParam = Vector4::ZERO;
	const Color& fog_color = SceneManager::Instance()->getFogColor();
	const Vector3* camerapos = camera->getPositionPtr();
	const Color& mainLightColor = SceneManager::Instance()->getMainLightColor();
	const Color& subLightColor = SceneManager::Instance()->getSubLightColor();

	m_shaderProgram->setUniform(m_sPMatMVPIndex, &projMat, SPT_MAT4, 1);
	m_shaderProgram->setUniform(m_spVecFogIndex, (void*)&cameraFogParam, SPT_VEC4, 1);
	m_shaderProgram->setUniform(m_spVecFogIndex, &cameraFogParam, SPT_VEC4, 1);
	m_shaderProgram->setUniform(m_spFogColorIndex, &fog_color, SPT_VEC4, 1);
	m_shaderProgram->setUniform(m_spCameraPosIndex, (void*)camerapos, SPT_VEC3, 1);
	m_shaderProgram->setUniform(m_spMainLightColorIndex, &mainLightColor, SPT_VEC4, 1);
	m_shaderProgram->setUniform(m_spSubLightColorIndex, &subLightColor, SPT_VEC4, 1);
	m_shaderProgram->setUniform(m_sPTexTextureIndex, (void*)&TEXTURE_UNIT0, SPT_TEXTURE, 1);
	m_shaderProgram->bindUniforms();

	FxRenderable* pRenderable = *(m_renderables.begin());
	LordAssert(pRenderable);
	Texture* texture = pRenderable->getTexture();
	pRender->setTexture(0, texture);
	texture->setSamplerState(m_samplerState);

	pRender->setDepthStencilState(m_depthState);
	pRender->setRasterizerState(m_rasterizerState);
	pRender->setBlendState(m_blendState);
		
	for (auto it : m_renderables)
	{
		FxRenderable* pRenderable = it;
		LordAssert(pRenderable);

		Matrix4 matProj;
		if (pRenderable->isViewSpace())
		{
			matProj = camera->getProjMatrix2();
		}
		else
		{
			matProj = camera->getProjMatrix();
		}
		matProj = viewMat * matProj;

		// set the unifroms.
		m_shaderProgram->setUniform(m_sPMatMVPIndex, &matProj, SPT_MAT4, 1);
		m_shaderProgram->bindUniforms();

		_render(pRender, pRenderable, isLine);
	}

	pRender->setTexture(0, NULL);
	pRender->setTexture(1, NULL);
}

void FxRenderQueue::_render(Renderer* pRender, FxRenderable* pRenderable, bool isLine)
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
	m_renderInput->bindGPUBuffers(pRenderable->getVertexBuffer(), sizeof(FxVertexFmt), pRenderable->getIndexBuffer(), sizeof(ui16));
	m_renderInput->setVertexCount(quadNum * 4);
	m_renderInput->setIndexCount(quadNum * (isLine ? 8 : 6));
	m_renderInput->setTopologyType(isLine ? RenderInput::TT_LINELIST : RenderInput::TT_TRIANGLELIST);
	pRender->render(m_renderInput);
}


}