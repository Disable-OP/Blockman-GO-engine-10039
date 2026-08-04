#include "GLES2RenderStd.h"
#include "GLES2Renderer.h"
#include "GLES2RenderTargetView.h"
#include "GLES2Mapping.h"
#include "Object/Exception.h"

namespace LORD
{
	GLES2RenderTargetView::GLES2RenderTargetView(PixelFormat pixFmt, uint width, uint height)
		: RenderTargetView(pixFmt, width, height)
	{
		glGenRenderbuffers(1, &m_hRBO);
		if(!m_hRBO)
		{
			LordException("Create GLES2RenderTargetView failed.");
		}

		glBindRenderbuffer(GL_RENDERBUFFER, m_hRBO);

		GLenum internalFmt = GLES2Mapping::MapInternalFormat(pixFmt);
		glRenderbufferStorage(GL_RENDERBUFFER, internalFmt, width, height);

		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	GLES2RenderTargetView::GLES2RenderTargetView(uint hRBO, uint width, uint height)
		: RenderTargetView(PF_RGBA8_UNORM, width, height)
	{
		m_hRBO = hRBO;
	}

	GLES2RenderTargetView::~GLES2RenderTargetView()
	{
		glDeleteRenderbuffers(1, &m_hRBO);
	}
	
	void GLES2RenderTargetView::onAttached(uint idx)
	{
		LordAssertX(idx == 0, "GLES2 only support one render target.");
		m_attIdx = idx;
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_hRBO);
	}
	
	void GLES2RenderTargetView::onDetached()
	{
		m_attIdx = -1;
	}
	
	uint GLES2RenderTargetView::getRBOHandle() const
	{
		return m_hRBO;
	}
}
