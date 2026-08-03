#include "Core.h"
#include "FxRenderable.h"
#include "TessMaterial.h"
#include "Render/Renderer.h"
#include "Render/GPUBuffer.h"


namespace LORD
{

FxRenderable::FxRenderable()
	: m_VertexSysMemory(nullptr)
	, m_VertexBuffer(nullptr)
	, m_quadPoolSize(0)
	, m_quadNum(0)
	, m_IndexBuffer(nullptr)
	, m_Texture(nullptr)
	, m_RenderGroup(FRP_DEFAULT)
	, m_created(false)
	, m_viewSpace(false)
{
}

FxRenderable::~FxRenderable()
{
	destroyVertexPool();
}

void FxRenderable::setQuadPoolSize(ui16 quadSize)
{
	LordAssert(!m_created);
	m_quadPoolSize = quadSize;
	m_quadNum = 0;
}

void FxRenderable::destroyVertexPool()
{
	LordSafeFree(m_VertexSysMemory);
	LordSafeDelete(m_VertexBuffer);
	m_quadNum = 0;
	m_quadPoolSize = 0;
	m_created = false;
}

void FxRenderable::beginPolygon()
{
	if (!m_created)
	{
		LordAssert(m_VertexSysMemory == nullptr);
		LordAssert(m_VertexBuffer == nullptr);
		m_VertexSysMemory = (float*)(LordMalloc(m_quadPoolSize * 4 * sizeof(FxVertexFmt)));
		Buffer vertBuff(m_quadPoolSize * 4 * sizeof(FxVertexFmt), m_VertexSysMemory);
		m_VertexBuffer = Renderer::Instance()->createVertexBuffer(GPUBuffer::GBU_GPU_READ | GPUBuffer::GBU_CPU_WRITE, vertBuff);
		m_created = true;
	}
	
	m_quadNum = 0;
}

void FxRenderable::endPolygon()
{
	LordAssert(m_VertexSysMemory);
	LordAssert(m_VertexBuffer);
	
	Buffer vertBuff(m_quadNum * 4 * sizeof(FxVertexFmt), m_VertexSysMemory);
	m_VertexBuffer->updateSubData(0, vertBuff);
}

void FxRenderable::setMaterialParam(Texture* texture, FX_RENDER_PASS group)
{
	m_Texture = texture;
	m_RenderGroup = group;
}

}
