#include "Core.h"
#include "RenderInput.h"
#include "Object/Exception.h"
#include "Renderer.h"
#include "ShaderProgram.h"

namespace LORD
{
	RenderInput::RenderInput(const VertexElementList& vertElms, ShaderProgram* pProgram)
		: m_topologyType(TT_TRIANGLELIST)
		, m_startVert(0)
		, m_baseVert(0)
		, m_startIdx(0)
		, m_pVertBuff(NULL)
		, m_vertStride(0)
		, m_vertCount(0)
		, m_pIdxBuff(NULL)
		, m_idxStride(0)
		, m_idxCount(0)
	{
		if(!pProgram)
			LordException("Shader program couldn't be null, create RenderInput failed.");

		if(!pProgram->isLinked())
			LordException("The shader program has not been linked, create RenderInput failed.");
	}

	RenderInput::~RenderInput()
	{
		//LordSafeDelete(m_pVertBuff);
		//LordSafeDelete(m_pIdxBuff);
	}

	void RenderInput::bindGPUBuffers(GPUBuffer* pVertBuff, uint vertStride)
	{
		bindGPUBuffers(pVertBuff, vertStride, NULL, 0);
	}

	void RenderInput::bindGPUBuffers(GPUBuffer* pVertBuff, uint vertStride, GPUBuffer* pIdxBuff, uint idxStride)
	{
		LordAssert(pVertBuff)
		m_pVertBuff = pVertBuff;
		//m_vertStride = vertStride;

		if(pIdxBuff)
		{
			m_pIdxBuff = pIdxBuff;
			m_idxStride = idxStride;
		}
	}

	void RenderInput::unbindGPUBuffers()
	{
		//unbind();
		m_pVertBuff = NULL;
		//m_vertStride = 0;
		m_vertCount = 0;
		m_pIdxBuff = NULL;
		m_idxStride = 0;
		m_idxCount = 0;
	}
}