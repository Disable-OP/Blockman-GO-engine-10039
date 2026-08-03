#include "Core.h"
#include "FontRenderQueue.h"
#include "FontRenderable.h"
#include "TessManager.h"

namespace LORD
{

FontRenderQueue::FontRenderQueue()
	: m_renderInput(NULL)
	, m_shaderProgram(NULL)
	, m_samplerState(NULL)
	, m_samplerState_lightmap(NULL)
	, m_blendState(NULL)
	, m_depthStateWithZTest(NULL)
	, m_depthStateWithoutZTest(NULL)
	, m_rasterizerState(NULL)
	, m_sPMatMVPIndex(0)
	, m_sPTexTextureIndex(0)
	, m_renderTexture(NULL)
{ }

FontRenderQueue::~FontRenderQueue()
{
	if (m_renderInput)
		m_renderInput->unbindGPUBuffers();
	LordSafeDelete(m_renderInput);
	LordSafeDelete(m_samplerState);
	LordSafeDelete(m_blendState);
	LordSafeDelete(m_depthStateWithZTest);
	LordSafeDelete(m_depthStateWithoutZTest);
	LordSafeDelete(m_rasterizerState);
}

void FontRenderQueue::setShaderProgram(ShaderProgram* s)
{
	LordAssert(s);
	m_shaderProgram = s;
	m_sPMatMVPIndex = m_shaderProgram->getParamPhysicsIndex("matWVP");
	m_sPTexTextureIndex = m_shaderProgram->getParamPhysicsIndex("texSampler_texture");
}

void FontRenderQueue::setSamplerState(const SamplerState::SamplerDesc& desc)
{
	Renderer* pRenderer = Renderer::Instance();
	LordAssert(pRenderer);

	m_samplerState = pRenderer->createSamplerState(desc);
}

void FontRenderQueue::setBlendState(const BlendState::BlendDesc& desc)
{
	Renderer* pRenderer = Renderer::Instance();
	LordAssert(pRenderer);

	m_blendState = pRenderer->createBlendState(desc);
}

void FontRenderQueue::setDepthState(DepthStencilState::DepthStencilDesc desc)
{
	Renderer* pRenderer = Renderer::Instance();
	LordAssert(pRenderer);

	desc.bDepthEnable = true;
	m_depthStateWithZTest = pRenderer->createDepthStencilState(desc);
	desc.bDepthEnable = false;
	m_depthStateWithoutZTest = pRenderer->createDepthStencilState(desc);
}

void FontRenderQueue::setRasterizerState(const RasterizerState::RasterizerDesc& desc)
{
	Renderer* pRenderer = Renderer::Instance();
	LordAssert(pRenderer);

	m_rasterizerState = pRenderer->createRasterizerState(desc);
}

void FontRenderQueue::setRenderInput(const RenderInput::VertexElementList& elements)
{
	Renderer* pRenderer = Renderer::Instance();
	LordAssert(pRenderer);
	LordAssert(m_shaderProgram);

	m_renderInput = pRenderer->createRenderInput(elements, m_shaderProgram);
}

void FontRenderQueue::addRenderable(FontRenderable* renderable)
{
	LordAssert(renderable);
	m_renderables.push_back(renderable);
}

void FontRenderQueue::endRender(bool isLine, const Matrix4* viewproj)
{
	//flush all renderalbes to draw.
	if (m_renderables.empty())
		return;

	LordAssert(m_shaderProgram);
	LordAssert(m_renderInput);
	m_renderTexture = NULL;
	Renderer* pRender = Renderer::Instance();
	m_shaderProgram->bind();
	
	Camera* camera = SceneManager::Instance()->getMainCamera();
	Matrix4 projMat = camera->getViewProjMatrix();
	m_shaderProgram->setUniform(m_sPMatMVPIndex, &projMat, SPT_MAT4, 1);
	m_shaderProgram->setUniform(m_sPTexTextureIndex, (void*)&TEXTURE_UNIT0, SPT_TEXTURE, 1);
	m_shaderProgram->bindUniforms();

	FontRenderable* pRenderable = *(m_renderables.begin());
	LordAssert(pRenderable);

	Texture* texture = pRenderable->getTexture();
	if (texture == NULL)
	{
		return;
	}
	pRender->setTexture(0, texture);
	texture->setSamplerState(m_samplerState);

	pRender->setRasterizerState(m_rasterizerState);
	pRender->setBlendState(m_blendState);

	for (auto it : m_renderables)
	{
		FontRenderable* pRenderable = it;
		LordAssert(pRenderable);
		_render(pRender, pRenderable, isLine);
	}

	pRender->setTexture(0, NULL);
}

void FontRenderQueue::_render(Renderer* pRender, FontRenderable* pRenderable, bool isLine)
{
	LordAssert(pRender);
	LordAssert(pRenderable);

	ui16 charCount = pRenderable->getCharCount();
	if (charCount == 0)
		return;

	Texture* texture = pRenderable->getTexture();
	if (texture != m_renderTexture && texture != NULL)
	{
		pRender->setTexture(0, texture);
		m_renderTexture = texture;
		texture->setSamplerState(m_samplerState);
	}

	if (pRenderable->isZTestEnabled())
	{
		pRender->setDepthStencilState(m_depthStateWithZTest);
	}
	else
	{
		pRender->setDepthStencilState(m_depthStateWithoutZTest);
	}

	m_renderInput->unbindGPUBuffers();
	m_renderInput->bindGPUBuffers(pRenderable->getVertexBuffer(), sizeof(FontVertexFmt), pRenderable->getIndexBuffer(), sizeof(ui16));
	m_renderInput->setVertexCount(charCount * 4);
	m_renderInput->setIndexCount(charCount * (isLine ? 8 : 6));
	m_renderInput->setTopologyType(isLine ? RenderInput::TT_LINELIST : RenderInput::TT_TRIANGLELIST);
	pRender->render(m_renderInput);
}


}