#include "GLES2RenderStd.h"
#include "GLES2Renderer.h"
#include "GLES2Mapping.h"
#include "GLES2TextureManager.h"
#include "GLES2RenderTarget.h"
#include "GLES2Loader.h"
#include "GLES2FrameBuffer.h"
#include "GLES2Texture.h"
#include "GLES2ShaderProgram.h"
#include "GLES2ShaderProgramManager.h"
#include "GLES2RenderInput.h"
#include "Render/Viewport.h"
#include "Object/Root.h"
#include "Object/Exception.h"

#include "Render/RenderStage.h"

namespace LORD
{
	GLES2Renderer::GLES2Renderer()
#if ( LORD_PLATFORM == LORD_PLATFORM_WINDOWS )
		: m_eglConfig( 0 )
		, m_eglDisplay( 0 )
		, m_eglContext( 0 )
		, m_eglSurface( 0 )
		, m_iConfig( 0 )
		, m_hDC( 0 )
		, m_hWnd( 0 )
    
#elif (LORD_PLATFORM == LORD_PLATFORM_MAC_IOS)
        : m_hRBO(0)
#endif
	{ 

	}
	
	GLES2Renderer::~GLES2Renderer()
	{
		//destroy();
	}
	
	bool GLES2Renderer::initialize(const RenderCfg& config)
	{
#if ( LORD_PLATFORM == LORD_PLATFORM_WINDOWS )
		//if( Root::Instance()->isEditorMode() )
		//{
			// create render context
			if( !createRenderContext( config ) )
			{
				LordLogDebug("createRenderContext failed.");
				return false;
			}
		// }
#endif

	#if defined(LORD_LOAD_GLES_EXT)
	
		LordLogDebug("Load GLES extensions ...");
	
		LordNew GLES2Loader;
		if(!GLES2Loader::Instance()->initialize())
		{
			LordLogDebug("Load GLES extensions failed.");
			return false;
		}
	
		LordLogDebug("Load GLES extensions successed");
	
	#endif
	

	#if (LORD_PLATFORM == LORD_PLATFORM_MAC_IOS)
		glGenRenderbuffers(1, &m_hRBO);
		glBindRenderbuffer(GL_RENDERBUFFER, m_hRBO);
	
		if(!m_hRBO)
		{
			LordLogError("Create default RBO failed.");
			return false;
		}
	#endif


#if( LORD_PLATFORM != LORD_PLATFORM_WINDOWS )
		m_screenWidth = config.screenWidth;
		m_screenHeight = config.screenHeight;
#endif

		checkOpenGLExtensions();

		delayInit();

		return true;
	}
	
	void GLES2Renderer::checkOpenGLExtensions()
	{
		String GLExtensions = " ";
		const char* strExt = (const char*)glGetString(GL_EXTENSIONS);
		if (strExt)
			GLExtensions += strExt;

		Root::Instance()->deviceFeatures().checkTextureFormat( GLExtensions );
	
		GLExtensions += " ";
		if(GLExtensions.size() > 512)
			GLExtensions[500] = 0;
		LordLogDebug("%s", GLExtensions.c_str());

// 		if (!Root::Instance()->deviceFeatures().supportATITC()
// 			&& !Root::Instance()->deviceFeatures().supportPVR()
// 			&& !Root::Instance()->deviceFeatures().supportDXT1()
// 			&& Root::Instance()->deviceFeatures().supportETC1())
// 		{
// 			Root::Instance()->deviceFeatures().setSeperateSampleAlpha(true);
// 			LordLogDebug("GLES2Renderer:  setSeperateSampleAlpha true!");
// 		}

		GLExtensions += " ";
		if(GLExtensions.size() > 512)
			GLExtensions[500] = 0;
		LordLogDebug("%s", GLExtensions.c_str());

		if (!glGetString(GL_VERSION))
		{
			LordLogError("OpenGL checkOpenGLExtensions glGetString(GL_VERSION) is NULL");
		}

		GLExtensions = " ";
		GLExtensions += String((const char*)glGetString(GL_VERSION));
		GLExtensions += " ";
		LordLogDebug("%s", GLExtensions.c_str());

		Root::Instance()->deviceFeatures().glesVersion() = GLExtensions;


		GLExtensions = " ";
		GLExtensions += String((const char*)glGetString(GL_VENDOR));
		GLExtensions += " ";
		LordLogDebug("%s", GLExtensions.c_str());

		Root::Instance()->deviceFeatures().vendor() = GLExtensions;

		GLExtensions = " ";
		GLExtensions += String((const char*)glGetString(GL_RENDERER));
		GLExtensions += " ";
		LordLogDebug("%s", GLExtensions.c_str());
		
		Root::Instance()->deviceFeatures().rendererName() = GLExtensions;

		GLExtensions = " ";
		GLExtensions += String((const char*)glGetString( GL_SHADING_LANGUAGE_VERSION ));
		GLExtensions += " ";
		LordLogDebug("%s", GLExtensions.c_str());

		Root::Instance()->deviceFeatures().shadingLangVersion() = GLExtensions;

	}

	bool GLES2Renderer::delayInit()
	{
		// create render factory instance
		LordNew GLES2TextureManager;

		LordNew GLES2ShaderProgramManager;

		// Initialize GLES2RenderTargetManager
#ifdef LORD_USE_RENDERTARGET
		LordNew GLES2RenderTargetManager;

#if( LORD_PLATFORM == LORD_PLATFORM_WINDOWS || LORD_PLATFORM == LORD_PLATFORM_ANDROID )

		if(!GLES2RenderTargetManager::Instance()->initialize())
		{
			LordLogError( "GLES2RenderTargetManager::initialize Falied !" );

			return false;
		}
#endif

#else

#if (LORD_PLATFORM == LORD_PLATFORM_MAC_IOS)
		/*
		// Extract width and height.
		GLint screenWidth, screenHeight;
		glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &screenWidth);
		glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &screenHeight);
		*/
		// create default RTV
		GLES2RenderTargetView* pRTV = LordNew GLES2RenderTargetView(m_hRBO, m_screenWidth, m_screenHeight);

		// create default DSV
		DepthStencilView* pDSV = createDepthStencilView(PF_D16_UNORM, m_screenWidth, m_screenHeight);

		// create frame buffer
		m_pFrameBuffer = createFrameBuffer();
		m_pFrameBuffer->bind();
		m_pFrameBuffer->attachRTV(0, pRTV);
		m_pFrameBuffer->attachDSV(pDSV);

		// bind color RBO
		glBindRenderbuffer(GL_RENDERBUFFER, m_hRBO);

		if (!m_pFrameBuffer)
		{
			LordLogError("Create frame buffer failed.");
			return false;
		}
#else
		// EGL created default framebuffer is Handle 0
		m_pFrameBuffer = LordNew GLES2FrameBuffer(0);
#endif

#endif // LORD_USE_RENDERTARGET

		m_bFullscreen = true;
		m_bVSync = false;
		
		// set default render states
		RasterizerState::RasterizerDesc rsDesc; 
		m_pDefaultRasterizerState = createRasterizerState(rsDesc);

		DepthStencilState::DepthStencilDesc dssDesc;
		m_pDefaultDepthStencilState = createDepthStencilState(dssDesc);

		BlendState::BlendDesc bsDesc;
		m_pDefaultBlendState = createBlendState(bsDesc);

		setRasterizerState(m_pDefaultRasterizerState);
		setDepthStencilState(m_pDefaultDepthStencilState);
		setBlendState(m_pDefaultBlendState);

		// set view port
		m_pMainViewport = LordNew Viewport(0, 0, m_screenWidth, m_screenHeight);
		return true;
	}
	
	void GLES2Renderer::destroy()
	{ 
		LordSafeDelete(m_pDefaultRasterizerState);
		LordSafeDelete(m_pDefaultDepthStencilState);
		LordSafeDelete(m_pDefaultBlendState);
		LordSafeDelete(m_pMainViewport);
	#if defined(LORD_LOAD_GLES_EXT)
		LordDelete GLES2Loader::Instance();
	#endif
#ifdef LORD_USE_RENDERTARGET
		if( GLES2RenderTargetManager::Instance() )
		{
			LordDelete GLES2RenderTargetManager::Instance();
		}
#endif
		LordDelete GLES2TextureManager::Instance();

		LordDelete GLES2ShaderProgramManager::Instance();

		LordSafeDelete(m_pFrameBuffer);

#if ( LORD_PLATFORM == LORD_PLATFORM_WINDOWS )
		destroyRenderContext();
#endif

	}

	void GLES2Renderer::setViewport(Viewport* pViewport)
	{
		LordAssert(pViewport)
		glViewport(pViewport->getLeft(), pViewport->getTop(), pViewport->getWidth(), pViewport->getHeight());
	}
    
    void GLES2Renderer::setViewport(ui32 left, ui32 top, ui32 width, ui32 height)
    {
        glViewport(left, top, width, height);
    }
	uint GLES2Renderer::getMaxStageNum() const
	{ 
		return 32;  
	}
	
	void GLES2Renderer::scissor(uint left, uint top, uint width, uint height)
	{
		glEnable(GL_SCISSOR_TEST);
		
		uint realtop = m_pMainViewport->getHeight() - top - height;

		if (Root::Instance()->isFixedAspect())
		{
			realtop = Root::Instance()->getRealHeight() - top - height;
		}
		glScissor(left, realtop, width, height);
	}
	
	void GLES2Renderer::endScissor()
	{
		glDisable(GL_SCISSOR_TEST);
	}

	void GLES2Renderer::setTexture(uint index, Texture* pTex)
	{
		if (pTex)
		{
			GLuint hTexture = ((GLES2Texture*)pTex)->getTextureHandle();
			if (m_Textures[index] != hTexture)
			{
				m_Textures[index] = hTexture;
				GLenum glTarget = GLES2Mapping::MapTextureType(pTex->getType());
				m_TextureType[index] = int(glTarget);

				glActiveTexture(GL_TEXTURE0 + index);
				glBindTexture(glTarget, hTexture);
			}
		}
		else
		{
			if (m_Textures[index] != 0)
			{
				m_Textures[index] = 0;
				GLenum glTarget = m_TextureType[index];
				m_TextureType[index] = 0;
				glActiveTexture(GL_TEXTURE0 + index);
				glBindTexture(glTarget, 0);
			}
		}
	}

	void GLES2Renderer::render(RenderInput* pInput)
	{
		LordAssert(pInput);

		pInput->bind();

		// set the type of primitive that should be rendered from this vertex buffer
		RenderInput::TopologyType topologyType = pInput->getTopologyType();
		GLenum glTopologyType = GLES2Mapping::MapPrimitiveTopology(topologyType);

		GPUBuffer* pVerBuff = pInput->getVertexBuffer();
		LordAssert(pVerBuff);

		//set the index buffer to active in the input assembler
		GPUBuffer* pIdxBuff = pInput->getIndexBuffer();
		if (pIdxBuff)
		{
			GLenum idxType;
			uint idxStride;

			//opengles does not support GL_UNSIGNED_INT
			/*if(pInput->getIndexStride() == sizeof(Dword))
			{
			idxType = GL_UNSIGNED_INT;
			idxStride = sizeof(Dword);
			}
			else*/
			{
				idxType = GL_UNSIGNED_SHORT;
				idxStride = sizeof(Word);
			}

			uint idxCount;

			if (pInput->getIndexCount() > 0)
				idxCount = pInput->getIndexCount();
			else
				idxCount = pIdxBuff->getSize() / idxStride;

			//Byte* idxOffset = 0;
			uint idxOffset = pInput->getStartIndex() * idxStride;
			glDrawElements(glTopologyType, idxCount, idxType, (void*)idxOffset);

			if (Root::Instance()->getEnableFrameProfile())
			{
				Root::Instance()->frameState().incrDrawCallTimes(1);
				Root::Instance()->frameState().incrTriangleNum(idxCount / 3);
			}
		}
		else	// no using index buffer 
		{
			uint vertStride = pInput->getVertexStride();
			uint vertCount;
			if (pInput->getVertexCount() > 0)
				vertCount = pInput->getVertexCount();
			else
				vertCount = pVerBuff->getSize() / vertStride;

			uint startVert = pInput->getStartVertex();
			glDrawArrays(glTopologyType, startVert, vertCount);
			
			if (Root::Instance()->getEnableFrameProfile())
			{
				Root::Instance()->frameState().incrDrawCallTimes(1);
				Root::Instance()->frameState().incrTriangleNum(vertCount / 3);
			}
		}

		pInput->unbind();
	}

	void GLES2Renderer::render(RenderInput* pInput, ui16* pIndices, ui32 indicesNum)
	{
		LordAssert(pInput);
		LordAssert(pIndices);

		// set the type of primitive that should be rendered from this vertex buffer
		RenderInput::TopologyType topologyType = pInput->getTopologyType();
		GLenum glTopologyType = GLES2Mapping::MapPrimitiveTopology(topologyType);
		GLenum idxType = GL_UNSIGNED_SHORT;
		
		glDrawElements(glTopologyType, indicesNum, idxType, (void*)pIndices);

		if (Root::Instance()->getEnableFrameProfile())
		{
			Root::Instance()->frameState().incrDrawCallTimes(1);
			Root::Instance()->frameState().incrTriangleNum(indicesNum / 3);
		}
	}

	void GLES2Renderer::getDepthRange(Vector2& vec)
	{
		vec.x = -1.0f;
		vec.y = 1.0f;
	}

	void GLES2Renderer::convertMatOrho(Matrix4& mat, const Matrix4& matOrth, Real zn, Real zf)
	{
		mat.m00 = matOrth.m00;	mat.m01 = matOrth.m01;	mat.m02 = matOrth.m02;		mat.m03 = matOrth.m03;
		mat.m10 = matOrth.m10;	mat.m11 = matOrth.m11;	mat.m12 = matOrth.m12;		mat.m13 = matOrth.m13;
		mat.m20 = matOrth.m20;	mat.m21 = matOrth.m21;	mat.m22 = 2*matOrth.m22;	mat.m23 = matOrth.m23;
		mat.m30 = matOrth.m30;	mat.m31 = matOrth.m31;	mat.m32 = (zn+zf)/(zn-zf);	mat.m33 = matOrth.m33;
	}

	void GLES2Renderer::convertMatProj(Matrix4& mat, const Matrix4& matProj)
	{
		mat.m00 = matProj.m00;	mat.m01 = matProj.m01;	mat.m02 = matProj.m02;		mat.m03 = matProj.m03;
		mat.m10 = matProj.m10;	mat.m11 = matProj.m11;	mat.m12 = matProj.m12;		mat.m13 = matProj.m13;
		mat.m20 = matProj.m20;	mat.m21 = matProj.m21;	mat.m22 = 2*matProj.m22+1;	mat.m23 = matProj.m23;
		mat.m30 = matProj.m30;	mat.m31 = matProj.m31;	mat.m32 = 2*matProj.m32;	mat.m33 = matProj.m33;
	}

	int GLES2Renderer::getRenderError() const
	{
		GLES2Error err = GLES2Mapping::MapErrorCode(glGetError());
		return int(err);
	}

	void GLES2Renderer::DebugGLError()
	{
		GLES2Error err = GLES2Mapping::MapErrorCode(glGetError());
		switch (err)
		{
		case GLES2ERR_NO_ERROR:
			{
				LordLogDebug("No error.");
			} break;
		case GLES2ERR_INVALID_ENUM:
			{
				LordLogError("Invalid enum.");
			} break;
		case GLES2ERR_INVALID_VALUE:
			{
				LordLogError("Invalid value.");
			} break;
		case GLES2ERR_INVALID_OPERATION:
			{
				LordLogError("Invalid operation.");
			} break;
		case GLES2ERR_STACK_OVERFLOW:
			{
				LordLogError("Stack overflow.");
			} break;
		case GLES2ERR_STACK_UNDERFLOW:
			{
				LordLogError("Stack underflow.");
			} break;
		case GLES2ERR_OUT_OF_MEMORY:
			{
				LordLogError("Out of memory.");
			} break;
		default: break;
		}
	}

	GPUBuffer* GLES2Renderer::createVertexBuffer(Dword usage, const Buffer& buff)
	{
		GPUBuffer* pGPUBuffer = NULL;
		try
		{
			pGPUBuffer = LordNew GLES2GPUBuffer(GPUBuffer::GBT_VERTEX, usage, buff);
		}
		catch (Exception e)
		{
			LordLogError("%s", e.getMessage().c_str());
		}

		return pGPUBuffer;
	}

	GPUBuffer* GLES2Renderer::createIndexBuffer(Dword usage, const Buffer& buff)
	{
		GPUBuffer* pGPUBuffer = NULL;
		try
		{
			pGPUBuffer = LordNew GLES2GPUBuffer(GPUBuffer::GBT_INDEX, usage, buff);
		}
		catch (Exception e)
		{
			LordLogError("%s", e.getMessage().c_str());
		}

		return pGPUBuffer;
	}

	Texture* GLES2Renderer::createTexture2D(PixelFormat pixFmt, Dword usage, uint width, uint height, uint numMipmaps, const Buffer& buff)
	{
		Texture* pTexture2D = NULL;
		try
		{
			pTexture2D = LordNew GLES2Texture(Texture::TT_2D, pixFmt, usage, width, height, 1, numMipmaps, buff);
		}
		catch (Exception e)
		{
			LordLogError("%s", e.getMessage().c_str());
		}

		return pTexture2D;
	}

	ShaderProgram* GLES2Renderer::createShaderProgram(ui32 uuid)
	{
		ShaderProgram* pShaderProgram = NULL;
		try
		{
			pShaderProgram = LordNew GLES2ShaderProgram(uuid);
		}
		catch (Exception e)
		{
			LordLogError("%s", e.getMessage().c_str());
		}
		
		return pShaderProgram;
	}

	Shader* GLES2Renderer::createShader(Shader::ShaderType type, const Shader::ShaderDesc& desc, const String& filename, String& errMsg)
	{
		Shader* pShader = NULL;
		try
		{
			pShader = LordNew GLES2Shader(type, desc, filename, errMsg);
		}
		catch (Exception e)
		{
			LordLogError("%s", e.getMessage().c_str());
		}

		return pShader;
	}

	RenderInput* GLES2Renderer::createRenderInput(const RenderInput::VertexElementList& vertElms, ShaderProgram* pProgram)
	{
		RenderInput* pRenderInput = NULL;
		try
		{
			pRenderInput = LordNew GLES2RenderInput(vertElms, pProgram);
		}
		catch (Exception e)
		{
			LordLogError("%s", e.getMessage().c_str());
		}

		return pRenderInput;
	}

	FrameBuffer* GLES2Renderer::createFrameBuffer()
	{
		FrameBuffer* pFrameBuffer = NULL;
		try
		{
			pFrameBuffer = LordNew GLES2FrameBuffer;
		}
		catch (Exception e)
		{
			LordLogError("%s", e.getMessage().c_str());
		}

		return pFrameBuffer;
	}

	RenderTargetView* GLES2Renderer::createRenderTargetView(PixelFormat fmt, uint width, uint height)
	{
		RenderTargetView* pRTV = NULL;
		try
		{
			pRTV = LordNew GLES2RenderTargetView(fmt, width, height);
		}
		catch (Exception e)
		{
			LordLogError("%s", e.getMessage().c_str());
		}

		return pRTV;
	}

	DepthStencilView* GLES2Renderer::createDepthStencilView(PixelFormat fmt, uint width, uint height)
	{
		DepthStencilView* pDSV = NULL;
		try
		{
			pDSV = LordNew GLES2DepthStencilView(fmt, width, height);
		}
		catch (Exception e)
		{
			LordLogError("%s", e.getMessage().c_str());
		}

		return pDSV;
	}

	RasterizerState* GLES2Renderer::createRasterizerState(const RasterizerState::RasterizerDesc& desc)
	{
		RasterizerState* pState = NULL;
		try
		{
			pState = LordNew GLES2RasterizerState(desc);
		}
		catch (Exception e)
		{
			LordLogError("%s", e.getMessage().c_str());
		}

		return pState;
	}

	DepthStencilState* GLES2Renderer::createDepthStencilState(const DepthStencilState::DepthStencilDesc& desc)
	{
		DepthStencilState* pState = NULL;
		try
		{
			pState = LordNew GLES2DepthStencilState(desc);
		}
		catch (Exception e)
		{
			LordLogError("%s", e.getMessage().c_str());
		}

		return pState;
	}

	BlendState* GLES2Renderer::createBlendState(const BlendState::BlendDesc& desc)
	{
		BlendState* pState = NULL;
		try
		{
			pState = LordNew GLES2BlendState(desc);
		}
		catch (Exception e)
		{
			LordLogError("%s", e.getMessage().c_str());
		}

		return pState;
	}

	SamplerState* GLES2Renderer::createSamplerState(const SamplerState::SamplerDesc& desc)
	{
		SamplerState* pState = NULL;
		try
		{
			pState = LordNew GLES2SamplerState(desc);
		}
		catch (Exception e)
		{
			LordLogError("%s", e.getMessage().c_str());
		}

		return pState;
	}

#if ( LORD_PLATFORM == LORD_PLATFORM_WINDOWS )

	bool GLES2Renderer::createRenderContext( const RenderCfg& config )
	{
		m_hWnd = (HWND)config.windowHandle;
		m_hDC = GetDC(m_hWnd);

		RECT rect;
		GetClientRect(m_hWnd, &rect );

		m_screenWidth = rect.right - rect.left;
		m_screenHeight = rect.bottom - rect.top;

		

		m_eglDisplay = eglGetDisplay(m_hDC);

		if( m_eglDisplay == EGL_NO_DISPLAY )
		{
			return false;
		}

		GLint majorVersion, minorVersion;
		if( ! eglInitialize( m_eglDisplay, &majorVersion, &minorVersion ) )
		{
			return false;
		}

		if( !eglBindAPI(EGL_OPENGL_ES_API) )
		{
			return false;
		}

		int i = 0;
		EGLint	m_attri_list[34];

		m_attri_list[i++] = EGL_BUFFER_SIZE;
		m_attri_list[i++] = 0;

		m_attri_list[i++] = EGL_DEPTH_SIZE;
		m_attri_list[i++] = 24;

		m_attri_list[i++] = EGL_STENCIL_SIZE;
		m_attri_list[i++] = 8;

		m_attri_list[i++] = EGL_SURFACE_TYPE;
		m_attri_list[i++] = EGL_WINDOW_BIT;

		m_attri_list[i++] = EGL_RENDERABLE_TYPE;
		m_attri_list[i++] = EGL_OPENGL_ES2_BIT;

		m_attri_list[i++] = EGL_SAMPLE_BUFFERS;
		m_attri_list[i++] = 0;

		m_attri_list[i] = EGL_NONE;


		EGLint num_configs = 0;

		if( !eglChooseConfig(m_eglDisplay, m_attri_list, &m_eglConfig, 1, &num_configs) )
		{
			return false;
		}

		if(!eglGetConfigAttrib(m_eglDisplay, m_eglConfig, EGL_CONFIG_ID, &m_iConfig))
		{
			return false;
		}

		// create render context
		if( m_eglContext )
		{
			eglDestroyContext(m_eglDisplay, m_eglContext);
		}

		GLint context_attri_list[32];

		int j = 0;
		context_attri_list[j++] = EGL_CONTEXT_CLIENT_VERSION;
		context_attri_list[j++] = 2;

		context_attri_list[j] = EGL_NONE;

		m_eglContext = eglCreateContext(m_eglDisplay, m_eglConfig, NULL, context_attri_list	);

		if( m_eglConfig == EGL_NO_CONTEXT )
		{
			return false;
		}

		GLint surface_attri_list[16];

		surface_attri_list[0] = EGL_NONE;

		m_eglSurface = eglCreateWindowSurface(m_eglDisplay, m_eglConfig, m_hWnd, surface_attri_list );

		if( m_eglSurface == EGL_NO_SURFACE )
		{
			return false;
		}

		if( !eglMakeCurrent(m_eglDisplay, m_eglSurface, m_eglSurface, m_eglContext) )
		{
			return false;
		}

		//Get the gl extension string
		const char* strExtensions = (const char*)glGetString(GL_EXTENSIONS);
		if (strExtensions)
		{
			//Get the length of the string we're searching for
			const size_t strLength = strlen("GL_EXT_discard_framebuffer");

			//Get the string position
			const char* position = strstr(strExtensions, "GL_EXT_discard_framebuffer");

			//Loop through until we find the actual extension, avoiding substrings.
			while (position != NULL && position[strLength] != '\0' && position[strLength] != ' ')
			{
				position = strstr(position + strLength, "GL_EXT_discard_framebuffer");
			}

			//Initialise the extension if it's found.
			if (position != NULL)
			{
				glDiscardFramebufferEXT = (PFNGLDISCARDFRAMEBUFFEREXTPROC)eglGetProcAddress("glDiscardFramebufferEXT");
			}
			else
			{
				glDiscardFramebufferEXT = NULL;
			}
		}
		else
		{
			glDiscardFramebufferEXT = NULL;
		}

		eglSwapInterval(m_eglDisplay, 0);

		return true;
	}

	void GLES2Renderer::destroyRenderContext()
	{
		eglSwapBuffers(m_eglDisplay, m_eglSurface);
		eglMakeCurrent(m_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		eglDestroyContext(m_eglDisplay, m_eglContext);
		eglDestroySurface(m_eglDisplay, m_eglSurface);
		eglTerminate(m_eglDisplay);
		ReleaseDC(m_hWnd, m_hDC);
	}
	
#endif
}
