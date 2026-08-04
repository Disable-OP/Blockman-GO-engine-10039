#include "Core.h"
#include "RenderTargetView.h"
#include "FrameBuffer.h"

namespace LORD
{
	RenderTargetView::RenderTargetView(PixelFormat pixFmt, uint width, uint height)
		: RenderView(RVT_RENDERTARGET, pixFmt)
		, m_attIdx(-1)
		, m_width(width)
		, m_height(height)
	{
	}

	RenderTargetView::~RenderTargetView()
	{
	}

	int RenderTargetView::getAttachedIndex() const
	{
		return m_attIdx;
	}

	uint RenderTargetView::getWidth() const
	{
		return m_width;
	}

	uint RenderTargetView::getHeight() const
	{
		return m_height;
	}
}