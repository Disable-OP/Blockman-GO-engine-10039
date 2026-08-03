#include "Core.h"
#include "RenderView.h"
#include "Renderer.h"

namespace LORD
{
	RenderView::RenderView(RenderViewType type, PixelFormat pixFmt)
		: m_type(type)
		, m_pixFmt(pixFmt)
	{
	}

	RenderView::~RenderView()
	{
	}

	RenderView::RenderViewType RenderView::getRenderViewType() const
	{
		return m_type;
	}

	PixelFormat RenderView::getPixelFormat() const
	{
		return m_pixFmt;
	}

}