#include "Core.h"
#include "LineRenderable.h"
#include "Render/Renderer.h"
#include "Render/GPUBuffer.h"


namespace LORD
{

LineRenderable::LineRenderable(LINE_RENDER_PASS pass)
	: m_VertexSysMemory(NULL)
	, m_VertexBuffer(NULL)
	, m_pointPoolSize(0)
	, m_pointNum(0)
	, m_lineSegNum(0)
	, m_IndexBuffer(NULL)
	, m_bInitialize(false)
	, m_IndexSysMemory(NULL)
	, m_RenderGroup(pass)
{
}

LineRenderable::~LineRenderable()
{
	destroyVertexPool();
}

void LineRenderable::setPointPoolSize(ui16 pointSize)
{
	m_pointPoolSize = pointSize;
	m_pointNum = 0;
	m_lineSegNum = 0;
}

void LineRenderable::destroyVertexPool()
{
	LordSafeFree(m_VertexSysMemory);
	LordSafeFree(m_IndexSysMemory);
	LordSafeDelete(m_VertexBuffer);
	LordSafeDelete(m_IndexBuffer);
	m_pointNum = 0;
	m_pointPoolSize = 0;
}

void LineRenderable::beginPolygon()
{
	if (!m_VertexSysMemory)
	{
		m_VertexSysMemory = (float*)(LordMalloc(m_pointPoolSize * sizeof(LineVertexFmt)));
		m_IndexSysMemory = (i16*)(LordMalloc(m_pointPoolSize * 2 * sizeof(i16)));
	}

	m_pointNum = 0;
	m_lineSegNum = 0;
}

void LineRenderable::endPolygon()
{
	LordAssert(m_VertexSysMemory);
	LordAssert(m_IndexSysMemory);

	if (m_pointNum == 0)
		return;

	Buffer vertBuff(m_pointNum * sizeof(LineVertexFmt), m_VertexSysMemory);
	Buffer indexBuff(m_lineSegNum * 2 * sizeof(i16), m_IndexSysMemory);

	if (!m_bInitialize)
	{
		m_VertexBuffer = Renderer::Instance()->createVertexBuffer(GPUBuffer::GBU_GPU_READ | GPUBuffer::GBU_CPU_WRITE, vertBuff);
		m_IndexBuffer = Renderer::Instance()->createIndexBuffer(GPUBuffer::GBU_GPU_READ | GPUBuffer::GBU_CPU_WRITE, indexBuff);
		m_bInitialize = true;
	}
	else
	{
		m_VertexBuffer->updateSubData(0, vertBuff);
		m_IndexBuffer->updateSubData(0, indexBuff);
	}
}

bool LineRenderable::getLockPtr_line(ui16 pointNum, LineVertexFmt*& vertexAddr)
{
	if (m_pointNum + pointNum <= m_pointPoolSize)
	{
		vertexAddr = (LineVertexFmt*)(m_VertexSysMemory + m_pointNum * sizeof(LineVertexFmt) / sizeof(float));
		int segCount = pointNum - 1;
		for (int i = 0; i < segCount; ++i)
		{
			m_IndexSysMemory[m_lineSegNum * 2 + i * 2 + 0] = m_pointNum + i;
			m_IndexSysMemory[m_lineSegNum * 2 + i * 2 + 1] = m_pointNum + i + 1;
		}
		
		m_pointNum += pointNum;
		m_lineSegNum += segCount;
	}
	else
	{
		return false;
	}

	return true;
}

bool LineRenderable::getLockPtr_seg(ui16 pointNum, LineVertexFmt*& vertexAddr)
{
	if (m_pointNum + pointNum <= m_pointPoolSize)
	{
		vertexAddr = (LineVertexFmt*)(m_VertexSysMemory + m_pointNum * sizeof(LineVertexFmt) / sizeof(float));
		int segCount = pointNum / 2;
		for (int i = 0; i < segCount; ++i)
		{
			m_IndexSysMemory[m_lineSegNum * 2 + i * 2 + 0] = m_pointNum + i * 2 + 0;
			m_IndexSysMemory[m_lineSegNum * 2 + i * 2 + 1] = m_pointNum + i * 2 + 1;
		}

		m_pointNum += pointNum;
		m_lineSegNum += segCount;
	}
	else
	{
		return false;
	}

	return true;
}

}