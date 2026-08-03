/********************************************************************
filename: 	GLES2Renderer.h
file path:	dev\engine\Src\GLES2Render\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_GLES2RENDERER_H__
#define __LORD_GLES2RENDERER_H__

#include "GLES2GPUBuffer.h"
#include "Render/Renderer.h"

namespace LORD
{
	class LORD_GLES2RENDER_API GLES2Renderer: public Renderer
	{
	public:
		GLES2Renderer();
		~GLES2Renderer();
	
		typedef vector<GLuint>::type			TexUintList;
		typedef vector<SamplerState*>::type		SamplerList;

	public:
		bool				initialize(const RenderCfg& config);
		bool				delayInit();
		void				destroy();
		void 				checkOpenGLExtensions();

        void				setViewport(Viewport* pViewport);
        void				setViewport(ui32 left, ui32 top, ui32 width, ui32 height);

		uint				getMaxStageNum() const;

		void				scissor(uint left, uint top, uint width, uint height);
		void				endScissor();

		void				setTexture(uint index, Texture* pTex);

		void				render(RenderInput* pInput);
		void				render(RenderInput* pInput, ui16* pIndices, ui32 indicesNum);

		void				getDepthRange(Vector2& vec);
		void				convertMatOrho(Matrix4& mat, const Matrix4& matOrth, Real zn, Real zf);
		void				convertMatProj(Matrix4& mat, const Matrix4& matProj);

		int					getRenderError() const;
		static void			DebugGLError();
		

		GPUBuffer*				createVertexBuffer(Dword usage, const Buffer& buff);
		GPUBuffer*				createIndexBuffer(Dword usage, const Buffer& buff);
		Texture*				createTexture2D(PixelFormat pixFmt, Dword usage, uint width, uint height, uint numMipmaps, const Buffer& buff);
		ShaderProgram*			createShaderProgram(ui32 uuid);
		Shader*					createShader(Shader::ShaderType type, const Shader::ShaderDesc& desc, const String& filename, String& errMsg);
		RenderInput*			createRenderInput(const RenderInput::VertexElementList& vertElms, ShaderProgram* pProgram);
		FrameBuffer*			createFrameBuffer();
		RenderTargetView*		createRenderTargetView(PixelFormat fmt, uint width, uint height);
		DepthStencilView*		createDepthStencilView(PixelFormat fmt, uint width, uint height);
		RasterizerState*		createRasterizerState(const RasterizerState::RasterizerDesc& desc);
		DepthStencilState*		createDepthStencilState(const DepthStencilState::DepthStencilDesc& desc);
		BlendState*				createBlendState(const BlendState::BlendDesc& desc);
		SamplerState*			createSamplerState(const SamplerState::SamplerDesc& desc);
	
	protected:
		virtual bool doPresent()
		{
#if (LORD_PLATFORM == LORD_PLATFORM_WINDOWS)
			if (glDiscardFramebufferEXT)
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
					LordLogError("GLES2Renderer: Present() Failed !");
					return false;
				}
#endif
			}

			return true;
#endif
			return true;
		}

	private:
	#if (LORD_PLATFORM == LORD_PLATFORM_MAC_IOS)
		GLuint				m_hRBO;
	#endif
		String				m_gpuDesc;

#if ( LORD_PLATFORM == LORD_PLATFORM_WINDOWS )

	public:
		void* getDisplay() const
		{
			return m_eglDisplay;
		}

		void* getSurface() const
		{
			return m_eglSurface;
		}

		bool createRenderContext( const RenderCfg& config );

		void destroyRenderContext();
	protected:
		EGLConfig	m_eglConfig;
		EGLDisplay	m_eglDisplay;
		EGLContext	m_eglContext;
		EGLSurface	m_eglSurface;
		EGLint		m_iConfig;
		HDC			m_hDC;
		HWND		m_hWnd;
		typedef void (GL_APIENTRY * PFNGLDISCARDFRAMEBUFFEREXT)(GLenum target, GLsizei numAttachments, const GLenum *attachments);
		PFNGLDISCARDFRAMEBUFFEREXT			glDiscardFramebufferEXT;
#endif

	};
}

#endif
