#include "Core.h"
#include "RasterizerHelpRender.h"
#include "Render/Renderer.h"
#include "Render/GPUBuffer.h"


namespace LORD
{
RasterizerRenderable::RasterizerRenderable()
	: m_VertexSysMemory(NULL)
	, m_VertexBuffer(NULL)
	, m_quadPoolSize(0)
	, m_quadNum(0)
	, m_IndexBuffer(NULL)
{
}

RasterizerRenderable::~RasterizerRenderable()
{
	destroyVertexPool();
}

void RasterizerRenderable::setQuadPoolSize(ui16 quadSize)
{
	m_quadPoolSize = quadSize;
	m_quadNum = 0;
}

void RasterizerRenderable::destroyVertexPool()
{
	LordSafeFree(m_VertexSysMemory);
	LordSafeDelete(m_VertexBuffer);
	m_quadNum = 0;
	m_quadPoolSize = 0;
}

void RasterizerRenderable::beginPolygon()
{
	LordAssert(!m_VertexSysMemory);
	
	m_VertexSysMemory = (float*)(LordMalloc(m_quadPoolSize * 4 * sizeof(LineVertexFmt)));
	LordAssert(m_VertexSysMemory);

	LordSafeDelete(m_VertexBuffer);

	m_quadNum = 0;
}

void RasterizerRenderable::endPolygon()
{
	LordAssert(m_VertexSysMemory);
	LordAssert(!m_VertexBuffer);
	
	Buffer vertBuff(m_quadNum * 4 * sizeof(LineVertexFmt), m_VertexSysMemory);
	m_VertexBuffer = Renderer::Instance()->createVertexBuffer(GPUBuffer::GBU_GPU_READ | GPUBuffer::GBU_CPU_WRITE, vertBuff);

	LordSafeFree(m_VertexSysMemory);	
}


RasterizerRenderQueue::RasterizerRenderQueue()
	: m_renderInput(NULL)
	, m_shaderProgram(NULL)
	, m_blendState(NULL)
	, m_depthState(NULL)
	, m_rasterizerState(NULL)
	, m_sPMatMVPIndex(0)
{ }

RasterizerRenderQueue::~RasterizerRenderQueue()
{
	if (m_renderInput)
		m_renderInput->unbindGPUBuffers();
	LordSafeDelete(m_renderInput);
	LordSafeDelete(m_blendState);
	LordSafeDelete(m_depthState);
	LordSafeDelete(m_rasterizerState);
}

void RasterizerRenderQueue::setShaderProgram(ShaderProgram* s)
{
	LordAssert(s);
	m_shaderProgram = s;
	m_sPMatMVPIndex = m_shaderProgram->getParamPhysicsIndex("matWVP");
}

void RasterizerRenderQueue::setBlendState(const BlendState::BlendDesc& desc)
{
	Renderer* pRenderer = Renderer::Instance();
	LordAssert(pRenderer);

	m_blendState = pRenderer->createBlendState(desc);
}

void RasterizerRenderQueue::setDepthState(const DepthStencilState::DepthStencilDesc& desc)
{
	Renderer* pRenderer = Renderer::Instance();
	LordAssert(pRenderer);

	m_depthState = pRenderer->createDepthStencilState(desc);
}

void RasterizerRenderQueue::setRasterizerState(const RasterizerState::RasterizerDesc& desc)
{
	Renderer* pRenderer = Renderer::Instance();
	LordAssert(pRenderer);

	m_rasterizerState = pRenderer->createRasterizerState(desc);
}

void RasterizerRenderQueue::setRenderInput(const RenderInput::VertexElementList& elements)
{
	Renderer* pRenderer = Renderer::Instance();
	LordAssert(pRenderer);
	LordAssert(m_shaderProgram);

	m_renderInput = pRenderer->createRenderInput(elements, m_shaderProgram);
}

void RasterizerRenderQueue::addRenderable(RasterizerRenderable* renderable)
{
	LordAssert(renderable);
	m_renderables.push_back(renderable);
}

void RasterizerRenderQueue::endRender(bool isLine, const Matrix4* viewproj)
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

	for (RasterizerRenderList::iterator it = m_renderables.begin(); it != m_renderables.end(); ++it)
	{
		RasterizerRenderable* pRenderable = *it;
		_render(pRender, pRenderable, isLine);
	}
}

void RasterizerRenderQueue::_render(Renderer* pRender, RasterizerRenderable* pRenderable, bool isLine)
{
	LordAssert(pRender);
	LordAssert(pRenderable);

	ui16 quadNum = pRenderable->getQuadNum();
	if (quadNum == 0)
		return;

	m_renderInput->unbindGPUBuffers();
	m_renderInput->bindGPUBuffers(pRenderable->getVertexBuffer(), sizeof(LineVertexFmt), pRenderable->getIndexBuffer(), sizeof(ui16));
	m_renderInput->setVertexCount(quadNum * 4);
	m_renderInput->setIndexCount(quadNum * (isLine ? 8 : 6));
	m_renderInput->setTopologyType(isLine ? RenderInput::TT_LINELIST : RenderInput::TT_TRIANGLELIST);
	pRender->render(m_renderInput);

}



}
