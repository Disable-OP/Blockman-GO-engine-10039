/********************************************************************
filename: 	Renderer.h
file path:	dev\engine\Src\Core\Render\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_RENDERER_H__
#define __LORD_RENDERER_H__

#include "Core.h"
#include "Object/Singleton.h"
#include "Image/Color.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Matrix4.h"
#include "RenderInput.h"
#include "RenderState.h"
#include "FrameBuffer.h"
#include "Object/Buffer.h"
#include "Shader.h"
#include "Texture.h"
#include "Material.h"

namespace LORD
{
	class GUIString;
	struct FontBuffer;
	class GUIFont;

	class LORD_CORE_API Renderer: public ObjectAlloc, public Singleton<Renderer>
	{
	public:
		static inline Renderer* Instance()
		{
			return ms_pSingleton;
		}

	public:

		struct RenderCfg
		{
			uint	screenWidth;
			uint	screenHeight;
			float	nearClip;
			float	farClip;
			bool	bFullscreen;
			bool	bVSync;
			// for Windows Editor.
			unsigned int windowHandle;

			RenderCfg()
				: screenWidth(800)
				, screenHeight(600)
				, nearClip(0.1f)
				, farClip(10000.0f)
				, bFullscreen(false)
				, bVSync(false)
				, windowHandle(0)
			{}
		};

		static Color BGCOLOR;

		typedef map<uint, SamplerState*>::type	StageSamplerMap;

	public:
		Renderer();
		virtual ~Renderer();

	public:
		virtual bool				initialize(const RenderCfg& config) = 0;
		virtual void				destroy() = 0;
		virtual void				checkOpenGLExtensions() = 0;

        virtual void				setViewport(Viewport* pViewport) = 0;
        virtual void				setViewport(ui32 left, ui32 top, ui32 width, ui32 height) = 0;
		virtual void				setRasterizerState(RasterizerState* pState);
		virtual void				setDepthStencilState(DepthStencilState* pState);
		virtual void				setBlendState(BlendState* pState);
		virtual void				setTexture(uint index, Texture* pTex) = 0;
		virtual bool				isFullscreen() const;
		virtual bool				isVSync() const;
		virtual uint				getMaxStageNum() const = 0;
		FrameBuffer*				getFrameBuffer() const { return m_pFrameBuffer; }
		virtual SamplerState*		getSamplerState(uint stage) const;

		virtual int					getRenderError() const;

		Viewport*					getMainViewport() const
		{
			return m_pMainViewport;
		}
		RasterizerState* getDefaultRasterizerState() const
		{
			return m_pDefaultRasterizerState;
		}

		DepthStencilState* getDefaultDepthStencilState() const
		{
			return m_pDefaultDepthStencilState;
		}

		BlendState* getDefaultBlendState() const
		{
			return m_pDefaultBlendState;
		}

		RasterizerState::RasterizerDesc* getRasterizerState() const
		{
			return m_pRasterizerState;
		}

		DepthStencilState::DepthStencilDesc* getDepthStencilState() const
		{
			return m_pDepthStencilState;
		}

		BlendState::BlendDesc* getBlendState() const
		{
			return m_pBlendState;
		}

		//virtual void				beginScene(Dword clearFlags = FrameBuffer::CM_COLOR | FrameBuffer::CM_DEPTH, const Color& color = BGCOLOR, float depth = 1.0f, int stencil = 0);
		//virtual void				endScene();

		virtual void				scissor(uint left, uint top, uint width, uint height) = 0;
		virtual void				endScissor() = 0;

		virtual void				render(RenderInput* pInput) = 0;
		virtual void				render(RenderInput* pInput, ui16* pIndices, ui32 indicesNum) = 0;

#ifdef LORD_INCLUDE_GUI
		virtual void				renderText(const GUIString& strText, const Vector2& pos, GUIFont* pFont,
										const Color& color = Color::WHITE, const Vector2& scale = Vector2::ONE, bool isLogicPosition = false, vector<FontBuffer>::type* fontBuffer = NULL);
#endif
		virtual void				project(Vector3& screenPos, const Vector3& worldPos, const Matrix4& matVP, Viewport* pViewport = NULL);
		virtual Vector3				project(const Vector3& worldPos);

		virtual void				unproject(Vector3& worldPos, const Vector3& screenPos, const Matrix4& matVP, Viewport* pViewport = NULL);
		virtual void				getDepthRange(Vector2& vec) = 0;
		virtual void				convertMatOrho(Matrix4& mat, const Matrix4& matOrth, Real zn, Real zf) = 0;
		virtual void				convertMatProj(Matrix4& mat, const Matrix4& matProj) = 0;

		virtual GPUBuffer*			createVertexBuffer(Dword usage, const Buffer& buff) = 0;
		virtual GPUBuffer*			createIndexBuffer(Dword usage, const Buffer& buff) = 0;
		virtual Texture*			createTexture2D(PixelFormat pixFmt, Dword usage, uint width, uint height, uint numMipmaps, const Buffer& buff) = 0;
		virtual Texture*			createTextureFromImage(Image* pImage, Dword usage = Texture::TU_DEFAULT);
		virtual ShaderProgram*		createShaderProgram(ui32 uuid) = 0;
		virtual Shader*				createShader(Shader::ShaderType type, const Shader::ShaderDesc& desc, const String& filename, String& errMsg) = 0;
		virtual RenderInput*		createRenderInput(const RenderInput::VertexElementList& vertElms, ShaderProgram* pProgram) = 0;
		virtual FrameBuffer*		createFrameBuffer() = 0;
		virtual RenderTargetView*	createRenderTargetView(PixelFormat fmt, uint width, uint height) = 0;
		virtual DepthStencilView*	createDepthStencilView(PixelFormat fmt, uint width, uint height) = 0;
		virtual RasterizerState*	createRasterizerState(const RasterizerState::RasterizerDesc& desc) = 0;
		virtual DepthStencilState*	createDepthStencilState(const DepthStencilState::DepthStencilDesc& desc) = 0;
		virtual BlendState*			createBlendState(const BlendState::BlendDesc& desc) = 0;
		virtual SamplerState*		createSamplerState(const SamplerState::SamplerDesc& desc) = 0;

		ui32						getScreenWidth() { return m_screenWidth; }
		ui32						getScreenHeight() { return m_screenHeight; }

		PixelFormat					getBackBufferPixelFormat() const
		{
			return m_backBufferFormat;
		}

		bool						present()
		{
			return doPresent();
		}

		bool						renderScene();

		void						clearCacheTexture()
		{
			for (int i = 0; i < MAX_TEXTURE_SAMPLER; ++i)
			{
				m_Textures[i] = 0;
				m_TextureType[i] = 0;
			}
		}

	protected:

		virtual bool doPresent() = 0;

		bool				m_bFullscreen;
		bool				m_bVSync;
		bool				m_bSupportsDXT;
		bool				m_bSupportsPVRTC;
		bool				m_bSupportsATITC;
		bool				m_bSupportsDepthTextures;
		bool				m_bSupportsAnisotropy;

		ui16				m_bBind2RGBTexture;
		ui16				m_bBind2RGBATexture;
		PixelFormat			m_backBufferFormat;
		ui32				m_backBufferBlueBits;
		ui32				m_backBufferRedBits;
		ui32				m_backBufferGreenBits;
		ui32				m_backBufferAlphaBits;
		ui32				m_backBufferBits;
		ui32				m_depthBufferBits;
		ui32				m_screenWidth;
		ui32				m_screenHeight;

		Viewport*			m_pMainViewport;
		RasterizerState*	m_pDefaultRasterizerState;
		DepthStencilState*	m_pDefaultDepthStencilState;
		BlendState*			m_pDefaultBlendState;
		RasterizerState::RasterizerDesc*	m_pRasterizerState;
		DepthStencilState::DepthStencilDesc*	m_pDepthStencilState;
		BlendState::BlendDesc*	m_pBlendState;
		FrameBuffer*		m_pFrameBuffer;
		StageSamplerMap		m_stageSamplerMap;


		int			    m_Textures[MAX_TEXTURE_SAMPLER];
		int				m_TextureType[MAX_TEXTURE_SAMPLER];
	};
}

#endif
