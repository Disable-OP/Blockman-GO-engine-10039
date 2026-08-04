/********************************************************************
filename: 	RenderTarget.h
file path:	dev\engine\Src\Core\Render\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef LORD_RENDERTAGER
#define LORD_RENDERTAGER

#include "Core.h"
#include "Object/Singleton.h"
#include "Object/Root.h"

namespace LORD
{
	#define MAX_GUI_RT_COUNT		12

	enum RenderTargetCreateFlags
	{
		RTCF_Scale,
		RTCF_Fixed
	};

	enum RenderTargetID
	{
		RTI_DefaultBackBuffer,
		RTI_EditorBackGroundMap,
		RTI_EditorBackGroundMapBlended,
		RTI_SceneColorMap,
		RTI_LDRSceneColorMap,
		RTI_LDRSceneColorMap_Half,
		// ...
		RTI_HorizonBlurringMap_Quater,
		RTI_VerticalBlurringMap_Quater,

		// GUI RENDER TARGET BEGIN
		RTI_GUI_BEGIN,
		RTI_GUI_END = RTI_GUI_BEGIN + MAX_GUI_RT_COUNT,
		// GUI RENDER TARGET END
		RTI_End
	};

	class LORD_CORE_API RenderTarget : public ObjectAlloc
	{
		friend class RenderTargetManager;
		friend class GLES2RenderTargetManager;
	public:

		enum
		{
			Invalid_Value = 0xffffffff
		};

	public:

		ui32& width(){ return m_width; }

		const ui32& width() const { return m_width; }

		ui32& height(){ return m_height; }

		const ui32& height() const { return m_height; }

		bool hasDepth() const { return m_bHasDepth; }

		bool hasMSAA() const { return m_bHasMSAA; }

		RenderTargetID id() const { return m_id; }

		PixelFormat pixelFormat() const { return m_pixelFormat; }

		ui32	getRenderFrameCount() const
		{
			return m_RenderFrameCount;
		}

		Texture* getBindTexture()
		{
			return m_bindTexture;
		}

		bool	checkValidInCurFrame() const
		{
			return m_RenderFrameCount == Root::Instance()->getCurFrameCount();
		}

		bool create()
		{
			LordAssert( m_bindTexture );

			m_bindTexture->m_width = m_width;
			m_bindTexture->m_height = m_height;
			m_bindTexture->m_depth = 1;
			m_bindTexture->m_texType = Texture::TT_2D;
			m_bindTexture->m_internalFormat = m_pixelFormat;
			m_bindTexture->m_pixFmt = m_pixelFormat;
			m_bindTexture->m_bCompressed = false;
			m_bindTexture->m_compressType = Texture::CompressType_Unknown;

			return doCreate();
		}

		bool beginRender( bool _clearColor, const Color& _backgroundColor,  bool _clearDepth, float _depthValue, bool _clearStencil, ui8 _stencilValue )
		{
			m_RenderFrameCount = Root::Instance()->getCurFrameCount();
			return doBeginRender( _clearColor, _backgroundColor, _clearDepth, _depthValue, _clearStencil, _stencilValue );
		}

		bool endRender()
		{
			return doEndRender();
		}

		void clear( bool _clearColor, const Color& _backgroundColor,  bool _clearDepth, float _depthValue, bool _clearStencil, ui8 _stencilValue )
		{
			return doClear( _clearColor, _backgroundColor, _clearDepth, _depthValue, _clearStencil, _stencilValue );
		}

		void onResize( ui32 _width, ui32 _height )
		{
			return doOnResize(_width, _height);
		}

	protected:

		virtual bool doCreate() = 0;

		virtual bool doBeginRender( bool _clearColor, const Color& _backgroundColor,  bool _clearDepth, float _depthValue, bool _clearStencil, ui8 stencilValue ) = 0;

		virtual bool doEndRender() = 0;

		virtual void doClear( bool _clearColor, const Color& _backgroundColor,  bool _clearDepth, float _depthValue, bool _clearStencil, ui8 _stencilValue ) = 0;

		virtual void doOnResize( ui32 _width, ui32 _height ) = 0;

	protected:
		PixelFormat				m_pixelFormat;
		bool						m_bHasMSAA;
		RenderTargetID				m_id;

		bool						m_bHasDepth;
		ui32						m_width;
		ui32						m_height;
		ui32						m_clearFlags;
		bool						m_bFrameBufferChange;
		bool						m_bViewportChange;
		class Texture*				m_bindTexture;
		ui32						m_RenderFrameCount;

		RenderTargetCreateFlags		m_rtcf;
		float						m_rtcf_width;
		float						m_rtcf_height;
		bool						m_bLogicResolution;
	protected:
		RenderTarget( RenderTargetID _id, RenderTargetCreateFlags _rtcf, float _width, float _height, PixelFormat _format, bool _hasDepth = false, bool _msaa = false,bool _multi_resolution = false );
		virtual ~RenderTarget();
	};


	// usage :
	// SceneManager::update();
	// RenderTargetManager::Instance()::beginRenderTarget(RTI_xxx)
	// SceneManager::render();
	// RenderTargetManager::Instance()::endRenderTarget(RTI_xxx)

	typedef map<ui32, RenderTarget*>::type RenderTargetMap;

	class LORD_CORE_API RenderTargetManager : public Singleton<RenderTargetManager>, public ObjectAlloc
	{


	public:

		static inline RenderTargetManager* Instance()
		{
			return ms_pSingleton;
		}

	public:

		RenderTargetManager();

		virtual ~RenderTargetManager();

		bool initialize();

		// !!!note!!! : do NOT use the RTI_DefaultBackBuffer as _srcID !
		bool updateRenderTarget( RenderTargetID _srcID, RenderTargetID _dstID, bool _clear_color =false, bool _clear_depth = false, bool _clear_stencil =false );
		bool updateRenderTargetWithFilter( RenderTargetID _srcID, RenderTargetID _dstID, bool _clear_color =false, bool _clear_depth = false, bool _clear_stencil =false );

		void changeFilterBlendmapName(const String& texName);

		bool beginRenderTarget( RenderTargetID _id,bool _clearColor = true, const Color& _backgroundColor = Renderer::BGCOLOR,  bool _clearDepth = true, float _depthValue = 1.0f, bool _clearStencil = false, ui8 stencilValue = 0, ui32 rbo = RenderTarget::Invalid_Value )
		{
			LordAssert( _id != RTI_End );

			RenderTarget* pRT = getRenderTargetByID( _id );

			RenderTarget* pUsingRT = 0;
			
			if(m_inUsingRenderTarget != RTI_End)
			{
				pUsingRT = getRenderTargetByID( m_inUsingRenderTarget );
			}

			LordAssert( pRT );

			if( m_inUsingRenderTarget != _id )
			{
				pRT->m_bFrameBufferChange = true;
			}
			else
			{
				pRT->m_bFrameBufferChange = false;
			}

			if (_id == RTI_LDRSceneColorMap)
			{
				pRT->m_bViewportChange = false;
			}
			else
			{
				pRT->m_bViewportChange = true;
			}

			m_inUsingRenderTarget = _id;

			bool ret = doBeginRenderTarget( _id, _clearColor, _backgroundColor, _clearDepth, _depthValue, _clearStencil, _clearStencil );

			if (!pRT->m_bViewportChange)
			{
				if (m_bEnableMultiResolution)
				{
					uint targetheight;
					Root::Instance()->fixedAspect(pRT->m_width, targetheight);
					Renderer::Instance()->setViewport(0, Math::Max((int)pRT->m_height - (int)targetheight, 0) / 2, pRT->m_width, targetheight);
				}
				else
				{
					Renderer::Instance()->setViewport(Renderer::Instance()->getMainViewport());
				}
			}

			return ret;
		}


		bool endRenderTarget( RenderTargetID _id )
		{
			LordAssert( m_inUsingRenderTarget == _id );

			return doEndRenderTarget( _id );
		}


		RenderTarget* createRenderTarget( RenderTargetID _id, RenderTargetCreateFlags _cf, float _width, float _height, PixelFormat _pixelFormat, bool _hasDepth = false, bool _msaa  = false, bool _multi_resolution = false )
		{
			return doCreateRenderTarget( _id, _cf, _width, _height, _pixelFormat, _hasDepth, _msaa, _multi_resolution );
		}

		void destroyRenderTarget(RenderTargetID _id);

		bool storeDefaultRenderTarget()
		{
			return doStoreDefaultRenderTarget();
		}

		bool restoreDefaultRenderTarget()
		{
			return doRestoreDefaultRenderTarget();
		}

		RenderTarget* getRenderTargetByID( RenderTargetID _id )
		{

			RenderTargetMap::iterator fit = m_mapRenderTargets.find( _id );

			if( fit == m_mapRenderTargets.end() )
			{
				LordLogError( "Could not found RenderTarget[%d]", _id );

				return NULL;
			}

			return fit->second;
		}

		RenderTargetID getInUsingRenderTargetID() const
		{
			return m_inUsingRenderTarget;
		}
		
		bool checkRenderTargetIDValid( RenderTargetID _id )
		{
			if( _id == RTI_End) return false;

			if( NULL != getRenderTargetByID(_id) )
			{
				return true;
			}

			return false;
		}

		Material* getBaseMaterial() const
		{
			return m_pMaterialBaseUpdate;
		}

		Material* getFilterMaterial() const
		{
			return m_pMaterialFilterUpdate;
		}

		void onResize( ui32 _width, ui32 _height )
		{

			if( m_bEnableMultiResolution )
			{
				AdaptSize(_width, _height, m_logicWidth, m_logicHeight);

				m_logicWidthScale = static_cast<float>(m_logicWidth) / static_cast<float>(_width);
				m_logicHeightScale = static_cast<float>(m_logicHeight) / static_cast<float>(_height);
			}

			RenderTargetMap::iterator bit = m_mapRenderTargets.begin();

			for( ; bit != m_mapRenderTargets.end(); ++bit )
			{
				bit->second->onResize( _width, _height );
			}
		}

		const ui32& logicWidth() const
		{
			return m_logicWidth;
		}

		const ui32& logicHeight() const
		{
			return m_logicHeight;
		}

		const ui32& hardwareWidth() const
		{
			return m_hardwareWidth;
		}

		const ui32& hardwareHeight() const
		{
			return m_hardwareHeight;
		}

		bool getEnableLogicResolution() const
		{
			return m_bEnableMultiResolution;
		}

		const float& logicWidthScale() const
		{
			return m_logicWidthScale;
		}

		const float& logicHeightScale() const
		{
			return m_logicHeightScale;
		}

		static void setAutoSizeMinWidth( ui32 asminW )
		{
			m_autoSizeMinWidth = asminW;
		}

		static ui32 getAutoSizeMinWidth()
		{
			return m_autoSizeMinWidth;
		}

		static void setAutoSizeMinHeight( ui32 asminH )
		{
			m_autoSizeMinHeight = asminH;
		}

		static ui32 getAutoSizeMinHeight()
		{
			return m_autoSizeMinHeight;
		}

		static void setAutoSizeMaxWidth( ui32 asmaxW )
		{
			m_autoSizeMaxWidth = asmaxW;
		}

		static ui32 getAutoSizeMaxWidth()
		{
			return m_autoSizeMaxWidth;
		}

		static void setAutoSizeMaxHeight( ui32 asmaxH )
		{
			m_autoSizeMaxHeight = asmaxH;
		}

		static ui32 getAutoSizeMaxHeight()
		{
			return m_autoSizeMaxHeight;
		}

		const RenderInput::VertexElementList& getVertexElement() const { return m_VertexElement; }
	protected:

		static ui32 m_autoSizeMinWidth ;//= 960;
		static ui32 m_autoSizeMinHeight ;//= 640;

		static ui32 m_autoSizeMaxWidth ;//= 1280;
		static ui32 m_autoSizeMaxHeight ;//= 800;

		void AdaptSize( uint nWidth, uint nHeight, uint& outWidth, uint& outHeight )
		{
			if (nWidth < m_autoSizeMinWidth || nHeight < m_autoSizeMinHeight)
			{
				float fWidthScale = m_autoSizeMinWidth * 1.0f / nWidth;
				float fHeightScale = m_autoSizeMinHeight * 1.0f / nHeight;
				if (fWidthScale > fHeightScale)
				{
					outWidth = (uint)(nWidth*fWidthScale);
					outHeight = (uint)(nHeight*fWidthScale);
				}
				else
				{
					outWidth = (uint)(nWidth*fHeightScale);
					outHeight = (uint)(nHeight*fHeightScale);
				}
			}
			else if (nWidth > m_autoSizeMaxWidth || nHeight > m_autoSizeMaxHeight)
			{
				float fWidthScale = m_autoSizeMaxWidth * 1.0f / nWidth;
				float fHeightScale = m_autoSizeMaxHeight * 1.0f / nHeight;
				if (fWidthScale < fHeightScale)
				{
					outWidth = (uint)(nWidth*fWidthScale);
					outHeight = (uint)(nHeight*fWidthScale);
				} 
				else
				{
					outWidth = (uint)(nWidth*fHeightScale);
					outHeight = (uint)(nHeight*fHeightScale);
				}
			}
			else
			{
				// ÆÁÄ»³ß´ç¾ÍÊÇ×îÖÕ³ß´ç
				outWidth = nWidth;
				outHeight = nHeight;
			}
		}

		virtual bool doBeginRenderTarget( RenderTargetID _id, bool _clearColor, const Color& _backgroundColor,  bool _clearDepth, float _depthValue, bool _clearStencil, ui8 stencilValue ) = 0;
		virtual bool doEndRenderTarget( RenderTargetID _id ) = 0;
		virtual RenderTarget* doCreateRenderTarget( RenderTargetID _id, RenderTargetCreateFlags _cf, float _width, float _height, PixelFormat _pixelFormat, bool _hasDepth = false, bool _msaa  = false, bool _multi_resolution = false ) = 0;
		virtual bool doStoreDefaultRenderTarget() = 0;
		virtual bool doRestoreDefaultRenderTarget() = 0;

		bool createScreenAlignedQuad_ext();
	protected:
		bool	m_bEnableMultiResolution;
		bool	m_bEnableFilter;// add by ajohn for filter additional
		RenderTargetID m_inUsingRenderTarget;
		ui32	m_logicWidth;
		ui32	m_logicHeight;
		ui32	m_hardwareWidth;
		ui32	m_hardwareHeight;
		float	m_logicWidthScale;
		float	m_logicHeightScale;

		Renderable* m_pScreenAlignedQuad;
		Renderable* m_pScreenAlignedQuadFilter;
		Material* m_pMaterialBaseUpdate;
		Material* m_pMaterialFilterUpdate;
		Texture* m_pFilterBlendmap;

		String	 m_pFilterBlendmapName;
		RenderTargetMap m_mapRenderTargets;

		RenderInput::VertexElementList m_VertexElement;
	};


#define RENDER_PASS_COMMAND( rtid, render_code ) \
	{ RenderTargetManager::Instance()->beginRenderTarget(rtid);\
		render_code\
	RenderTargetManager::Instance()->endRenderTarget(rtid);}

}


#endif
