#include "Core.h"
#include "BatchRenderQueue.h"
#include "BatchRenderable.h"
#include "TessManager.h"

namespace LORD
{

BatchRenderQueue::BatchRenderQueue(BATCH_RENDER_PASS rg)
: m_renderInput(NULL)
, m_shaderProgram(NULL)
, m_samplerState(NULL)
, m_blendState(NULL)
, m_depthState(NULL)
, m_rasterizerState(NULL)
, m_sPMatMVPIndex(0)
, m_spFogParamIndex(0)
, m_spCameraPosIndex(0)
, m_spFogColorIndex(0)
, m_sPTexTextureIndex(0)
, m_renderGroup(rg)
, m_renderTexture(NULL)
{ }

BatchRenderQueue::~BatchRenderQueue()
{
	if (m_renderInput)
		m_renderInput->unbindGPUBuffers();
	LordSafeDelete(m_renderInput);
	LordSafeDelete(m_samplerState);
	LordSafeDelete(m_blendState);
	LordSafeDelete(m_depthState);
	LordSafeDelete(m_rasterizerState);
}

void BatchRenderQueue::setShaderProgram(ShaderProgram* s)
{
	LordAssert(s);
	m_shaderProgram = s;
	m_sPMatMVPIndex = m_shaderProgram->getParamPhysicsIndex("matWVP");
	m_spFogParamIndex = m_shaderProgram->getParamPhysicsIndex("fogParam");
	m_spCameraPosIndex = m_shaderProgram->getParamPhysicsIndex("cameraPos");
	m_spFogColorIndex = m_shaderProgram->getParamPhysicsIndex("fogColor");
	m_sPTexTextureIndex = m_shaderProgram->getParamPhysicsIndex("texSampler");
}

void BatchRenderQueue::setSamplerState(const SamplerState::SamplerDesc& desc)
{
	Renderer* pRenderer = Renderer::Instance();
	LordAssert(pRenderer);

	m_samplerState = pRenderer->createSamplerState(desc);
}

void BatchRenderQueue::setBlendState(const BlendState::BlendDesc& desc)
{
	Renderer* pRenderer = Renderer::Instance();
	LordAssert(pRenderer);

	m_blendState = pRenderer->createBlendState(desc);
}

void BatchRenderQueue::setDepthState(const DepthStencilState::DepthStencilDesc& desc)
{
	Renderer* pRenderer = Renderer::Instance();
	LordAssert(pRenderer);

	m_depthState = pRenderer->createDepthStencilState(desc);
}

void BatchRenderQueue::setRasterizerState(const RasterizerState::RasterizerDesc& desc)
{
	Renderer* pRenderer = Renderer::Instance();
	LordAssert(pRenderer);

	m_rasterizerState = pRenderer->createRasterizerState(desc);
}

void BatchRenderQueue::setRenderInput(const RenderInput::VertexElementList& elements)
{
	Renderer* pRenderer = Renderer::Instance();
	LordAssert(pRenderer);
	LordAssert(m_shaderProgram);

	m_renderInput = pRenderer->createRenderInput(elements, m_shaderProgram);
}

void BatchRenderQueue::addRenderable(BatchRenderable* renderable)
{
	LordAssert(renderable);
	m_renderables.push_back(renderable);
}

void BatchRenderQueue::endRender(bool isLine, const Matrix4* viewproj)
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
	Matrix4 viewProjMat = viewproj ? *viewproj : camera->getViewProjMatrix();
	const Vector4& fog_params = SceneManager::Instance()->getFogParam();
	const Color& fog_color = SceneManager::Instance()->getFogColor();
	const Vector3* camerapos = camera->getPositionPtr();

	m_shaderProgram->setUniform(m_sPMatMVPIndex, &viewProjMat, SPT_MAT4, 1);
	m_shaderProgram->setUniform(m_spFogParamIndex, &fog_params, SPT_VEC4, 1);
	m_shaderProgram->setUniform(m_spFogColorIndex, &fog_color, SPT_VEC4, 1);
	m_shaderProgram->setUniform(m_spCameraPosIndex, (void*)camerapos, SPT_VEC3, 1);
	m_shaderProgram->setUniform(m_sPTexTextureIndex, (void*)&TEXTURE_UNIT0, SPT_TEXTURE, 1);
	m_shaderProgram->bindUniforms();

	BatchRenderable* pRenderable = *(m_renderables.begin());
	LordAssert(pRenderable);
	Texture* texture = pRenderable->getTexture();
	pRender->setTexture(0, texture);
	texture->setSamplerState(m_samplerState);

	pRender->setDepthStencilState(m_depthState);
	pRender->setRasterizerState(m_rasterizerState);
	pRender->setBlendState(m_blendState);

	for (BatchRenderList::iterator it = m_renderables.begin(); it != m_renderables.end(); ++it)
	{
		pRenderable = *it;
		LordAssert(pRenderable);
		_render(pRender, pRenderable, isLine);
	}

	// need delete all the renders
	for (auto it : m_renderables)
	{
		TessManager::Instance()->destroyBatchRenderable(it);
	}
	pRender->setTexture(0, NULL);
}

void BatchRenderQueue::_render(Renderer* pRender, BatchRenderable* pRenderable, bool isLine)
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
	m_renderInput->bindGPUBuffers(pRenderable->getVertexBuffer(), sizeof(BatchVertexFmt), pRenderable->getIndexBuffer(), sizeof(ui16));
	m_renderInput->setVertexCount(quadNum * 4);
	m_renderInput->setIndexCount(quadNum * (isLine ? 8 : 6));
	m_renderInput->setTopologyType(isLine ? RenderInput::TT_LINELIST : RenderInput::TT_TRIANGLELIST);
	pRender->render(m_renderInput);
}


}