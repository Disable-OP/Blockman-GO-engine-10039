#include "Core.h"
#include "FrameBuffer.h"
#include "Renderer.h"
#include "RenderTargetView.h"
#include "DepthStencilView.h"
#include "Viewport.h"

namespace LORD
{
	FrameBuffer::FrameBuffer()
		: m_pDSV(NULL)
		, m_pViewport(NULL)
	{
		for(uint i = 0; i < MAX_RENDERTARGETS; ++i)
		{
			m_pRTVs[i] = NULL;
		}
	}

	FrameBuffer::~FrameBuffer()
	{
		for(uint i = 0; i < MAX_RENDERTARGETS; ++i)
			LordSafeDelete(m_pRTVs[i]);

		LordSafeDelete(m_pDSV);
		LordSafeDelete(m_pViewport);
	}

	void FrameBuffer::attachRTV(uint idx, RenderTargetView* pRTV)
	{
		LordAssert(pRTV);

		detachRTV(idx);

		m_pRTVs[idx] = pRTV;
		pRTV->onAttached(idx);
	}

	void FrameBuffer::detachRTV(uint idx)
	{
		LordAssert(0 <= idx && idx < MAX_RENDERTARGETS);

		if(m_pRTVs[idx])
		{
			m_pRTVs[idx]->onDetached();
			LordSafeDelete(m_pRTVs[idx]);
		}
	}

	void FrameBuffer::attachDSV(DepthStencilView* pDSV)
	{
		detachDSV();
		m_pDSV = pDSV;
		m_pDSV->onAttached();
	}

	void FrameBuffer::detachDSV()
	{
		if(m_pDSV)
		{
			m_pDSV->onDetached();
			LordSafeDelete(m_pDSV);
		}
	}

	RenderTargetView* FrameBuffer::getRenderTargetView(uint idx) const
	{
		LordAssert(0 <= idx && idx < MAX_RENDERTARGETS);
		return m_pRTVs[idx];
	}

	DepthStencilView* FrameBuffer::getDepthStencilView() const
	{
		return m_pDSV;
	}

	void FrameBuffer::setViewport(Viewport* pViewport)
	{
		LordAssert(pViewport);
		LordSafeDelete(m_pViewport);
		m_pViewport = pViewport;
	}

	Viewport* FrameBuffer::getViewport() const
	{
		return m_pViewport;
	}
}