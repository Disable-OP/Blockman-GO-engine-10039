#include "Core.h"
#include "LineRenderQueue.h"
#include "LineRenderable.h"
#include "TessManager.h"

namespace LORD
{

LineRenderQueue::LineRenderQueue()
	: m_renderInput(NULL)
	, m_shaderProgram(NULL)
	, m_blendState(NULL)
	, m_depthState(NULL)
	, m_rasterizerState(NULL)
	, m_sPMatMVPIndex(0)
{ }

LineRenderQueue::~LineRenderQueue()
{
	if (m_renderInput)
		m_renderInput->unbindGPUBuffers();
	LordSafeDelete(m_renderInput);
	LordSafeDelete(m_blendState);
	LordSafeDelete(m_depthState);
	LordSafeDelete(m_rasterizerState);
}

void LineRenderQueue::setShaderProgram(ShaderProgram* s)
{
	LordAssert(s);
	m_shaderProgram = s;
	m_sPMatMVPIndex = m_shaderProgram->getParamPhysicsIndex("matWVP");
}

void LineRenderQueue::setBlendState(const BlendState::BlendDesc& desc)
{
	Renderer* pRenderer = Renderer::Instance();
	LordAssert(pRenderer);

	m_blendState = pRenderer->createBlendState(desc);
}

void LineRenderQueue::setDepthState(const DepthStencilState::DepthStencilDesc& desc)
{
	Renderer* pRenderer = Renderer::Instance();
	LordAssert(pRenderer);

	m_depthState = pRenderer->createDepthStencilState(desc);
}

void LineRenderQueue::setRasterizerState(const RasterizerState::RasterizerDesc& desc)
{
	Renderer* pRenderer = Renderer::Instance();
	LordAssert(pRenderer);

	m_rasterizerState = pRenderer->createRasterizerState(desc);
}

void LineRenderQueue::setRenderInput(const RenderInput::VertexElementList& elements)
{
	Renderer* pRenderer = Renderer::Instance();
	LordAssert(pRenderer);
	LordAssert(m_shaderProgram);

	m_renderInput = pRenderer->createRenderInput(elements, m_shaderProgram);
}

void LineRenderQueue::addRenderable(LineRenderable* renderable)
{
	LordAssert(renderable);
	m_renderables.push_back(renderable);
}

void LineRenderQueue::endRender(const Matrix4* viewproj)
{
	//flush all renderalbes to draw.
	if (m_renderables.empty())
		return;

	LordAssert(m_shaderProgram);
	LordAssert(m_renderInput);
	
	Renderer* pRender = Renderer::Instance();
	m_shaderProgram->bind();

	Camera* camera = SceneManager::Instance()->getMainCamera();
	Matrix4 viewProjMat = viewproj ? *viewproj : camera->getViewProjMatrix();
	m_shaderProgram->setUniform(m_sPMatMVPIndex, &viewProjMat, SPT_MAT4, 1);
	m_shaderProgram->bindUniforms();

	pRender->setDepthStencilState(m_depthState);
	pRender->setRasterizerState(m_rasterizerState);
	pRender->setBlendState(m_blendState);

	for (LineRenderList::iterator it = m_renderables.begin(); it != m_renderables.end(); ++it)
	{
		LineRenderable* pRenderable = *it;
		_render(pRender, pRenderable);
	}
}

void LineRenderQueue::_render(Renderer* pRender, LineRenderable* pRenderable)
{
	LordAssert(pRender);
	LordAssert(pRenderable);

	ui16 pointNum = pRenderable->getPointNum();
	if (pointNum == 0)
		return;
	ui16 lineSegNum = pRenderable->getLineSegNum();
	if (lineSegNum == 0)
		return;

	m_renderInput->unbindGPUBuffers();
	m_renderInput->bindGPUBuffers(pRenderable->getVertexBuffer(), sizeof(LineVertexFmt), pRenderable->getIndexBuffer(), sizeof(ui16));
	m_renderInput->setVertexCount(pointNum);
	m_renderInput->setIndexCount(lineSegNum * 2);
	m_renderInput->setTopologyType(RenderInput::TT_LINELIST);
	pRender->render(m_renderInput);
}


}