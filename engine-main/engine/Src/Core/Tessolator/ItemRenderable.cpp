#include "Core.h"
#include "ItemRenderable.h"
#include "TessMaterial.h"

#include "Render/Renderer.h"
#include "Render/GPUBuffer.h"


namespace LORD
{

ItemRenderMesh::ItemRenderMesh()
	: m_VertexSysMemory(NULL)
	, m_VertexBuffer(NULL)
	, m_quadPoolSize(0)
	, m_quadNum(0)
	, m_IndexBuffer(NULL)
{
}

ItemRenderMesh::~ItemRenderMesh()
{
	destroyVertexPool();
}

ItemRenderable::ItemRenderable(ITEM_RENDER_PASS pass)
	: m_worldMat(Matrix4::IDENTITY)
	, m_brightness(1.f, 1.f, 0.5f, 1.f)
	, m_viewSpace(false)
	, m_RenderGroup(pass)
	, m_texture(NULL)
{
}

void ItemRenderMesh::setQuadPoolSize(ui16 quadSize)
{
	m_quadPoolSize = quadSize;
	m_quadNum = 0;
}

void ItemRenderMesh::destroyVertexPool()
{
	LordSafeFree(m_VertexSysMemory);
	LordSafeDelete(m_VertexBuffer);
	m_quadNum = 0;
	m_quadPoolSize = 0;
}

void ItemRenderMesh::beginPolygon()
{
	LordAssert(!m_VertexSysMemory);

	m_VertexSysMemory = (float*)(LordMalloc(m_quadPoolSize * 4 * sizeof(ItemVertexFmt)));
	LordAssert(m_VertexSysMemory);

	LordSafeDelete(m_VertexBuffer);

	m_quadNum = 0;
}

void ItemRenderMesh::endPolygon()
{
	LordAssert(m_VertexSysMemory);
	LordAssert(!m_VertexBuffer);

	Buffer vertBuff(m_quadNum * 4 * sizeof(ItemVertexFmt), m_VertexSysMemory);
	m_VertexBuffer = Renderer::Instance()->createVertexBuffer(GPUBuffer::GBU_GPU_READ | GPUBuffer::GBU_CPU_WRITE, vertBuff);

	LordSafeFree(m_VertexSysMemory);
}

ItemRenderable::~ItemRenderable()
{
}

}