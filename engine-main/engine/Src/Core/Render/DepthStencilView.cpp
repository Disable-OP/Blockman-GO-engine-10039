#include "Core.h"
#include "DepthStencilView.h"

namespace LORD
{
	DepthStencilView::DepthStencilView(PixelFormat pixFmt, uint width, uint height)
		: RenderView(RVT_DEPTHSTENCIL, pixFmt)
		, m_width(width)
		, m_height(height)
	{
	}

	DepthStencilView::~DepthStencilView()
	{
	}

	uint DepthStencilView::getWidth() const
	{
		return m_width;
	}

	uint DepthStencilView::getHeight() const
	{
		return m_height;
	}
}