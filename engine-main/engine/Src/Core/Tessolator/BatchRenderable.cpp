#include "Core.h"
#include "BatchRenderable.h"
#include "Render/Renderer.h"
#include "Render/GPUBuffer.h"


namespace LORD
{

BatchRenderable::BatchRenderable()
: m_VertexSysMemory(NULL)
, m_VertexBuffer(NULL)
, m_quadPoolSize(0)
, m_quadNum(0)
, m_IndexBuffer(NULL)
, m_Texture(NULL)
, m_RenderGroup(STRP_SOLID)
{
}

BatchRenderable::~BatchRenderable()
{
	destroyVertexPool();
}

void BatchRenderable::setQuadPoolSize(ui16 quadSize)
{
	m_quadPoolSize = quadSize;
	m_quadNum = 0;
}

void BatchRenderable::destroyVertexPool()
{
	LordSafeFree(m_VertexSysMemory);
	LordSafeDelete(m_VertexBuffer);
	m_quadNum = 0;
	m_quadPoolSize = 0;
}

void BatchRenderable::beginPolygon()
{
	LordAssert(!m_VertexSysMemory);

	m_VertexSysMemory = (float*)(LordMalloc(m_quadPoolSize * 4 * sizeof(BatchVertexFmt)));
	LordAssert(m_VertexSysMemory);

	LordSafeDelete(m_VertexBuffer);

	m_quadNum = 0;
}

void BatchRenderable::endPolygon()
{
	LordAssert(m_VertexSysMemory);
	LordAssert(!m_VertexBuffer);

	Buffer vertBuff(m_quadNum * 4 * sizeof(BatchVertexFmt), m_VertexSysMemory);
	m_VertexBuffer = Renderer::Instance()->createVertexBuffer(GPUBuffer::GBU_GPU_READ | GPUBuffer::GBU_CPU_WRITE, vertBuff);

	LordSafeFree(m_VertexSysMemory);
}

void BatchRenderable::setMaterialParam(Texture* texture, BATCH_RENDER_PASS group)
{
	m_Texture = texture;
	m_RenderGroup = group;
}

}