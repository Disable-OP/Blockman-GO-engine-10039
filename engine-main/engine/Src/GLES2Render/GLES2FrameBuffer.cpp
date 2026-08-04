#include "GLES2RenderStd.h"
#include "GLES2Renderer.h"
#include "GLES2FrameBuffer.h"
#include "GLES2RenderState.h"

namespace LORD
{
	GLES2FrameBuffer::GLES2FrameBuffer()
		: m_Color(Color::INVALID)
		, m_Depth(-1.0f)
		, m_Stencil(-1)
	{
		glGenFramebuffers(1, &m_hFBO);
	}
	
	GLES2FrameBuffer::GLES2FrameBuffer(GLuint hFBO)
		: m_hFBO(hFBO)
		, m_Color(Color::INVALID)
		, m_Depth(-1.0f)
		, m_Stencil(-1)
	{
	}

	GLES2FrameBuffer::~GLES2FrameBuffer()
	{
		if(m_hFBO > 0)
		{
			glDeleteFramebuffers(1, &m_hFBO);
		}
	}

	void GLES2FrameBuffer::clear(Dword clearFlags, const Color& color, float depth, int stencil)
	{
		GLbitfield mask = 0;
		
		if(clearFlags & CM_COLOR)
		{
			if (m_Color != color)
			{
				glClearColor(color.r, color.g, color.b, color.a);
				m_Color = color;
			}
			mask |= GL_COLOR_BUFFER_BIT;
		}
	
		//if(m_pDSV)
		{
			if((clearFlags & CM_DEPTH))
			{
				if (m_Depth != depth)
				{
					glClearDepthf(depth);
					m_Depth = depth;
				}
				mask |= GL_DEPTH_BUFFER_BIT;
			}

			if((clearFlags & CM_STENCIL))
			{
				if (m_Stencil != stencil)
				{
					glClearStencil(stencil);
					m_Stencil = stencil;
				}
				mask |= GL_STENCIL_BUFFER_BIT;
			}
		}

		if(mask != 0)
		{
			DepthStencilState::DepthStencilDesc* pDSState = Renderer::Instance()->getDepthStencilState();
			bool needRevertDepthWrite = false;
			if(pDSState)
			{
				needRevertDepthWrite = !(pDSState->bWriteDepth);
				if(needRevertDepthWrite)
					glDepthMask(GL_TRUE);
			}

			glClear(mask);
			if(needRevertDepthWrite)
			{
				glDepthMask(GL_FALSE);
			}
		}
	}

	void GLES2FrameBuffer::bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_hFBO);
	}

	GLuint GLES2FrameBuffer::getFBOHandle() const
	{
		return m_hFBO;
	}

	inline void GLES2FrameBuffer::swap()
	{
#if (LORD_PLATFORM == LORD_PLATFORM_WINDOWS)
		if (glDiscardFramebufferEXT != NULL)
		{
			const GLint numAttachments=3;
			GLenum attachments[numAttachments];
			GLint currentAttachment=0;
			
			attachments[currentAttachment] = GL_COLOR_EXT;
			currentAttachment++;
			
			attachments[currentAttachment] = GL_DEPTH_EXT;
			currentAttachment++;
	
			attachments[currentAttachment] = GL_STENCIL_EXT;
			currentAttachment++;

			glDiscardFramebufferEXT(GL_FRAMEBUFFER, currentAttachment, attachments);
		}

		if( GLES2Renderer::Instance() )
		{
			eglSwapBuffers( static_cast<EGLDisplay>(static_cast<GLES2Renderer *>(GLES2Renderer::Instance())->getDisplay()), static_cast<EGLSurface>(static_cast<GLES2Renderer *>(GLES2Renderer::Instance())->getSurface()) );

#if _DEBUG
			EGLint no_erro = eglGetError();

			if (no_erro != GL_NO_ERROR && no_erro != EGL_SUCCESS)
			{
				LordLogError("GLES2FrameBuffer: Swap() Failed !");
				return;
			}
#endif
		}
#endif
	}

}
