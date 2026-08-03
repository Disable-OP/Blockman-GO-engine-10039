/********************************************************************
filename: 	GLES2RenderTarget.h
file path:	dev\engine\Src\GLES2Render\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef LORD_GLES2RENDERTAGER
#define LORD_GLES2RENDERTAGER

#include "Core.h"
#include "Render/RenderTarget.h"

namespace LORD
{

	class LORD_GLES2RENDER_API GLES2RenderTarget : public RenderTarget
	{

		friend class GLES2RenderTargetManager;

	public:
/*
		GLES2RenderTarget( RenderTargetID _id, RenderTargetCreateFlags _cf, float _width, float _height, PixelFormat _pixelFormat, bool _hasDepth = false, bool _msaa  = false );
		virtual ~GLES2RenderTarget();*/


	protected:
		virtual bool doCreate();
		virtual bool doBeginRender( bool _clearColor, const Color& _backgroundColor,  bool _clearDepth, float _depthValue, bool _clearStencil, ui8 stencilValue );
		virtual bool doEndRender();
        virtual void doClear( bool _clearColor, const Color& _backgroundColor,  bool _clearDepth, float _depthValue, bool _clearStencil, ui8 stencilValue );
		virtual void doOnResize( ui32 _width, ui32 _height );
	protected:
		GLuint m_fbo;
		GLuint m_rbo;
		GLuint m_textureHandle;
        Color mBackGroundColor;
        float mDepthValue;
        ui8 mStencilValue;
	protected:
		GLES2RenderTarget( RenderTargetID _id, RenderTargetCreateFlags _cf, float _width, float _height, PixelFormat _pixelFormat, bool _hasDepth = false, bool _msaa  = false, bool _multi_resolution = false  );
		virtual ~GLES2RenderTarget();
	};

	class LORD_GLES2RENDER_API GLES2RenderTargetManager : public RenderTargetManager
	{
	public:
		GLES2RenderTargetManager();
		virtual ~GLES2RenderTargetManager();

	protected:
		virtual bool doBeginRenderTarget( RenderTargetID _id, bool _clearColor, const Color& _backgroundColor,  bool _clearDepth, float _depthValue, bool _clearStencil, ui8 stencilValue );

		virtual bool doEndRenderTarget( RenderTargetID _id );

		virtual RenderTarget* doCreateRenderTarget( RenderTargetID _id, RenderTargetCreateFlags _cf, float _width, float _height, PixelFormat _pixelFormat, bool _hasDepth = false, bool _msaa = false, bool _multi_resolution = false );

		virtual bool doStoreDefaultRenderTarget();

		virtual bool doRestoreDefaultRenderTarget();
	};


}


#endif
