#include "Core.h"
#include "TessRenderable.h"
#include "TessMaterial.h"
#include "Render/Renderer.h"
#include "Render/GPUBuffer.h"


namespace LORD
{
TessRenderable::TessRenderable()
	: m_VertexSysMemory(NULL)
	, m_VertexBuffer(NULL)
	, m_quadPoolSize(0)
	, m_quadNum(0)
	, m_IndexBuffer(NULL)
	, m_Texture(NULL)
	, m_RenderGroup(TRP_SOLID)
	, m_position(Vector3::ZERO)
{
}

TessRenderable::~TessRenderable()
{
	destroyVertexPool();
}

void TessRenderable::setQuadPoolSize(ui16 quadSize)
{
	m_quadPoolSize = quadSize;
	m_quadNum = 0;
}

void TessRenderable::destroyVertexPool()
{
	LordSafeFree(m_VertexSysMemory);
	LordSafeDelete(m_VertexBuffer);
	m_quadNum = 0;
	m_quadPoolSize = 0;
}

void TessRenderable::beginPolygon()
{
	LordAssert(!m_VertexSysMemory);
	
	m_VertexSysMemory = (float*)(LordMalloc(m_quadPoolSize * 4 * sizeof(TessVertexFmt)));
	LordAssert(m_VertexSysMemory);

	LordSafeDelete(m_VertexBuffer);

	m_quadNum = 0;
}

void TessRenderable::setVertexAlpha(float alpha)
{
	LordAssert(m_VertexSysMemory);
	LordAssert(!m_VertexBuffer);

	TessVertexFmt* vertices = (TessVertexFmt*)(m_VertexSysMemory);
	for (int i = 0; i < m_quadNum * 4; ++i)
	{
		Color color = vertices[i].GetLightColor();
		color.a = alpha;
		vertices[i].SetLightColor(color.getARGB());
	}
}

void TessRenderable::endPolygon()
{
	LordAssert(m_VertexSysMemory);
	LordAssert(!m_VertexBuffer);
	
	Buffer vertBuff(m_quadNum * 4 * sizeof(TessVertexFmt), m_VertexSysMemory);
	m_VertexBuffer = Renderer::Instance()->createVertexBuffer(GPUBuffer::GBU_GPU_READ | GPUBuffer::GBU_CPU_WRITE, vertBuff);

	LordSafeFree(m_VertexSysMemory);	
}

void TessRenderable::setMaterialParam(Texture* texture, TESS_RENDER_PASS group)
{
	m_Texture = texture;
	m_RenderGroup = group;
}

Vector4 TessRenderable::updateUVParam(int frameCount)
{
	// all the animation texture is vertical arrangement!
	Vector4 uvAnimParam = Vector4::ZERO;
	if (m_Texture)
	{
		int w = m_Texture->getWidth();
		int h = m_Texture->getHeight();
		uvAnimParam.y = float(w) / h;
		int frames = h / w;
		int dir = (frameCount / frames) % 2;
		int current = frameCount % frames;
		uvAnimParam.w = float(current);
		if (frames == 20 && dir == 1) // 20 is for lava still  - -!
		{
			uvAnimParam.w = float(19 - current);
		}
	}
	return uvAnimParam;
}

}
