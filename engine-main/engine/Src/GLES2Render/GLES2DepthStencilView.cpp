#include "GLES2RenderStd.h"
#include "GLES2Renderer.h"
#include "GLES2DepthStencilView.h"
#include "GLES2Mapping.h"
#include "Object/Exception.h"
#include "Image/PixelFormat.h"

namespace LORD
{
	GLES2DepthStencilView::GLES2DepthStencilView(PixelFormat pixFmt, uint width, uint height)
		: DepthStencilView(pixFmt, width, height)
	{
		glGenRenderbuffers(1, &m_hRBO);
		if(!m_hRBO)
		{
			LordException("Create GLES2DepthStencilView failed.");
		}

		glBindRenderbuffer(GL_RENDERBUFFER, m_hRBO);

		GLenum glPixFmt = GLES2Mapping::MapInternalFormat(pixFmt);
		glRenderbufferStorage(GL_RENDERBUFFER, glPixFmt, width, height);

		//glBindRenderbuffer(GL_RENDERBUFFER, NULL);
	}
	
	GLES2DepthStencilView::~GLES2DepthStencilView()
	{
		glDeleteRenderbuffers(1, &m_hRBO);
	}
	
	void GLES2DepthStencilView::onAttached()
	{
		if(PixelUtil::IsDepth(m_pixFmt))
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_hRBO);

		if(PixelUtil::IsStencil(m_pixFmt))
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_hRBO);
	}
	
	void GLES2DepthStencilView::onDetached()
	{
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, 0);
	}
	
	GLuint GLES2DepthStencilView::getRBOHandle() const
	{
		return m_hRBO;
	}

}
