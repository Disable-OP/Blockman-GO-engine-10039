#include "GLES2RenderStd.h"
#include "GLES2Mapping.h"
#include "GLES2RenderTarget.h"
#include "GLES2Texture.h"

namespace LORD
{

	GLES2RenderTarget::GLES2RenderTarget( RenderTargetID _id, RenderTargetCreateFlags _cf, float _width, float _height, PixelFormat _pixelFormat, bool _hasDepth /* = false */, bool _msaa /* = false */, bool _multi_resolution )
		: RenderTarget( _id, _cf, _width, _height, _pixelFormat, _hasDepth, _msaa, _multi_resolution )
		, m_fbo( Invalid_Value )
		, m_rbo( Invalid_Value )
        , mBackGroundColor(Color::INVALID)
        , mDepthValue(-1.0f)
        , mStencilValue(10)
	{
		m_bindTexture = LordNew GLES2Texture( "rt_" + StringUtil::ToString(_id), RESOURCE_TYPE_MANUAL);
	} 

	GLES2RenderTarget::~GLES2RenderTarget()
	{
		if( m_fbo != Invalid_Value )
		{
			glDeleteFramebuffers( 1, &m_fbo );
		}

		if( m_rbo != Invalid_Value )
		{
			glDeleteRenderbuffers( 1, &m_rbo );
		}
		LordSafeDelete(m_bindTexture);
	}

	bool GLES2RenderTarget::doCreate()
	{

		LordAssert( m_bindTexture );

		bool no_error = false;

		glGenTextures( 1, &static_cast<GLES2Texture*>(m_bindTexture)->m_hTexture );

		glBindTexture( GL_TEXTURE_2D, static_cast<GLES2Texture*>(m_bindTexture)->m_hTexture );

		glTexImage2D( GL_TEXTURE_2D, 0, GLES2Mapping::MapFormat(m_pixelFormat), m_width, m_height, 0, GLES2Mapping::MapFormat( m_pixelFormat ), GL_UNSIGNED_BYTE, (GLvoid*)0 );

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		if( m_bHasDepth )
		{
			glGenRenderbuffers( 1, &m_rbo );
			glBindRenderbuffer( GL_RENDERBUFFER, m_rbo );
			glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, m_width, m_height );
		}

		glGenFramebuffers( 1, &m_fbo );

		glBindFramebuffer( GL_FRAMEBUFFER, m_fbo );

		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, static_cast<GLES2Texture*>(m_bindTexture)->m_hTexture, 0 );

		if( m_bHasDepth )
		{
			glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_rbo );
		}


		GLuint uStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		if(uStatus == GL_FRAMEBUFFER_COMPLETE)
		{
			no_error = true;
		}
		else
		{
			LordLogError( "Create RenderTarget Failed !" );
			no_error = false;
		}

#if( LORD_PLATFORM == LORD_PLATFORM_WINDOWS || LORD_PLATFORM == LORD_PLATFORM_ANDROID )
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
#endif
		return no_error;
	} 

	bool GLES2RenderTarget::doBeginRender( bool _clearColor, const Color& _backgroundColor,  bool _clearDepth, float _depthValue, bool _clearStencil, ui8 _stencilValue )
	{
		LordAssert( m_id == RenderTargetManager::Instance()->getInUsingRenderTargetID() );

		if( m_bFrameBufferChange )
		{
			glBindFramebuffer( GL_FRAMEBUFFER, m_fbo );
		}
		//GLint fbo, rbo;
		//glGetIntegerv( GL_FRAMEBUFFER_BINDING,  &fbo );
		//glGetIntegerv( GL_RENDERBUFFER_BINDING, &rbo );

		if( m_bViewportChange )
		{
			glViewport( 0, 0, m_width, m_height );
		}

		doClear( _clearColor, _backgroundColor, _clearDepth, _depthValue, _clearStencil, _stencilValue );

		return true;
	}

	bool GLES2RenderTarget::doEndRender()
	{
		//		RenderTargetManager::Instance()->beginRenderTarget( RTI_DefaultBackBuffer, true, Renderer::BGCOLOR, true, 1.0f, false, 0 );
		return true;
	}

	void GLES2RenderTarget::doClear( bool _clearColor, const Color& _backgroundColor, bool _clearDepth, float _depthValue, bool _clearStencil, ui8 _stencilValue )
	{
		GLbitfield mask = 0;

		if( _clearColor)
		{
            if (mBackGroundColor!=_backgroundColor)
            {
                mBackGroundColor = _backgroundColor;
                glClearColor(_backgroundColor.r, _backgroundColor.g, _backgroundColor.b, _backgroundColor.a);
            }

			mask |= GL_COLOR_BUFFER_BIT;
		}

		if( _clearDepth)
		{
            if (mDepthValue!=_depthValue)
            {
                mDepthValue = _depthValue;
                glClearDepthf( _depthValue );
            }

			mask |= GL_DEPTH_BUFFER_BIT;
		}

		glDepthMask( _clearDepth );

		if( _clearStencil)
		{
            if(mStencilValue!=_stencilValue)
            {
                mStencilValue = _stencilValue;
                glClearStencil( _stencilValue );
            }

			mask |= GL_STENCIL_BUFFER_BIT;
		}

		glStencilMask(_clearStencil);

        if (mask != 0)
            glClear( mask );

		Renderer::Instance()->setDepthStencilState(Renderer::Instance()->getDefaultDepthStencilState());

	}

	void GLES2RenderTarget::doOnResize( ui32 _width, ui32 _height )
	{
		if( m_id == RTI_DefaultBackBuffer )
		{
			m_width = _width;
			m_height = _height;

			return ;
		}

		switch( m_rtcf )
		{
		case RTCF_Fixed: return;

		case RTCF_Scale:
			m_width = RenderTargetManager::Instance()->getEnableLogicResolution() ? static_cast<ui32>( _width * RenderTargetManager::Instance()->logicWidthScale() * m_rtcf_width ) : static_cast<ui32>( _width * m_rtcf_width );
			
			if( m_width == 0 ) m_width = 1;
			
			m_height = RenderTargetManager::Instance()->getEnableLogicResolution() ? static_cast<ui32>( _height * RenderTargetManager::Instance()->logicHeightScale() * m_rtcf_height ) : static_cast<ui32>(_height * m_rtcf_height);
			
			if( m_height == 0 ) m_height = 1;

			break;
		}

		LordAssert( m_fbo != Invalid_Value );

		glDeleteFramebuffers( 1, &m_fbo );

		LordAssert( m_rbo != Invalid_Value );
		
		glDeleteRenderbuffers( 1, &m_rbo );

		LordAssert( m_bindTexture );
		
		LordSafeDelete( m_bindTexture );

		m_bindTexture = LordNew GLES2Texture("rt_" + StringUtil::ToString(m_id), RESOURCE_TYPE_MANUAL);

		create();
		//doCreate();
	}

	GLES2RenderTargetManager::GLES2RenderTargetManager()
	{
	}

	GLES2RenderTargetManager::~GLES2RenderTargetManager()
	{
		//TODO
	}

	RenderTarget* GLES2RenderTargetManager::doCreateRenderTarget( RenderTargetID _id, RenderTargetCreateFlags _cf, float _width, float _height, PixelFormat _pixelFormat, bool _hasDepth /* = false */, bool _msaa /* = false */, bool _multi_resolution )
	{

		RenderTarget* pNewRenderTarget = LordNew GLES2RenderTarget(_id, _cf, _width, _height, _pixelFormat, _hasDepth, _msaa, _multi_resolution);

		if( !pNewRenderTarget )
		{
			LordLogError( "Allocate GLES2RenderTarget Failed !" );
			return NULL;
		}

		if( !pNewRenderTarget->create() )
		{
			LordLogError( "GLES2RenderTarget::create Failed !" );
			LordSafeDelete( pNewRenderTarget );
			return NULL;
		}

		m_mapRenderTargets.insert( RenderTargetMap::value_type( _id, pNewRenderTarget ) );

		return pNewRenderTarget;
	}

	bool GLES2RenderTargetManager::doStoreDefaultRenderTarget()
	{

		RenderTarget* pDefaultRT = LordNew GLES2RenderTarget(RTI_DefaultBackBuffer, RTCF_Scale, 1.0f, 1.0f, Renderer::Instance()->getBackBufferPixelFormat(), true, false);

		if( !pDefaultRT )
		{
			LordLogError( "LordNew RTI_DefaultBackBuffer Failed !" );
			return false;
		}

		glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*)&(static_cast<GLES2RenderTarget*>(pDefaultRT)->m_fbo));

		m_mapRenderTargets.insert(RenderTargetMap::value_type( RTI_DefaultBackBuffer, pDefaultRT ));

		return true;
	}

	bool GLES2RenderTargetManager::doRestoreDefaultRenderTarget()
	{
		GLES2RenderTarget* pDefaultRT = static_cast<GLES2RenderTarget*>(getRenderTargetByID(RTI_DefaultBackBuffer));

		if( !pDefaultRT )
		{
			LordLogError( "Could not found RenderTarget[RTI_DefaultBackBuffer]" );

			return false;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, pDefaultRT->m_fbo);

		return true;
	}

	bool GLES2RenderTargetManager::doBeginRenderTarget( RenderTargetID _id, bool _clearColor, const Color& _backgroundColor, bool _clearDepth, float _depthValue, bool _clearStencil, ui8 stencilValue )
	{
		return getRenderTargetByID( _id )->beginRender( _clearColor, _backgroundColor, _clearDepth, _depthValue, _clearStencil, stencilValue );
	}

	bool GLES2RenderTargetManager::doEndRenderTarget( RenderTargetID _id )
	{
		//		LordAssert( m_inUsingRenderTarget = RTI_End );

		return getRenderTargetByID( _id )->endRender();
	}

}
