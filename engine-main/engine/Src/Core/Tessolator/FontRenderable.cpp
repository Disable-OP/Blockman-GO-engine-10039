#include "Core.h"
#include "FontRenderable.h"
#include "TessMaterial.h"
#include "Render/Renderer.h"
#include "Render/GPUBuffer.h"


namespace LORD
{

FontRenderable::FontRenderable()
	: m_VertexSysMemory(nullptr)
	, m_VertexBuffer(nullptr)
	, m_charPoolSize(0)
	, m_charCount(0)
	, m_IndexBuffer(nullptr)
	, m_Texture(nullptr)
	, m_created(false)
{
}

FontRenderable::~FontRenderable()
{
	destroyVertexPool();
}

void FontRenderable::setQuadPoolSize(ui16 charCount)
{
	LordAssert(!m_created);
	m_charPoolSize = charCount;
	m_charCount = 0;
}

void FontRenderable::destroyVertexPool()
{
	LordSafeFree(m_VertexSysMemory);
	LordSafeDelete(m_VertexBuffer);
	m_charCount = 0;
	m_charPoolSize = 0;
	m_created = false;
}

void FontRenderable::beginPolygon()
{
	if (!m_created)
	{
		LordAssert(m_VertexSysMemory == nullptr);
		LordAssert(m_VertexBuffer == nullptr);
		m_VertexSysMemory = (float*)(LordMalloc(m_charPoolSize * 4 * sizeof(FontVertexFmt)));
		Buffer vertBuff(m_charPoolSize * 4 * sizeof(FontVertexFmt), m_VertexSysMemory);
		m_VertexBuffer = Renderer::Instance()->createVertexBuffer(GPUBuffer::GBU_GPU_READ | GPUBuffer::GBU_CPU_WRITE, vertBuff);
		m_created = true;
	}
	
	m_charCount = 0;
}

void FontRenderable::endPolygon()
{
	LordAssert(m_VertexSysMemory);
	LordAssert(m_VertexBuffer);
	
	Buffer vertBuff(m_charCount * 4 * sizeof(FontVertexFmt), m_VertexSysMemory);
	m_VertexBuffer->updateSubData(0, vertBuff);
}

void FontRenderable::setMaterialParam(Texture* texture)
{
	m_Texture = texture;
}



}
