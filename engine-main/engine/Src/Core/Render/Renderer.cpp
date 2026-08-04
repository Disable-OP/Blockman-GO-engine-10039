#include "Core.h"
#include "Renderer.h"
#include "RenderTarget.h"
#include "RenderStage.h"
#include "Scene/SceneManager.h"
#include "Viewport.h"
#include "Image/PixelFormat.h"
#include "Image/Image.h"
#ifdef LORD_INCLUDE_GUI
#include "UI/GUIFont.h"
#include "UI/GUIFontManager.h"
#include "UI/GUIRenderManager.h"
#include "UI/GUISystem.h"
#endif

namespace LORD
{
	Color Renderer::BGCOLOR = Color::BLACK;

	//---------------------------------------------------------------------
	/*Renderer* Renderer::Instance()
	{
	return ms_pSingleton;
	}*/

	Renderer::Renderer()
		: m_screenWidth(0)
		, m_screenHeight(0)
		, m_bFullscreen(false)
		, m_bVSync(false)
		, m_pDefaultRasterizerState(NULL)
		, m_pDefaultDepthStencilState(NULL)
		, m_pDefaultBlendState(NULL)
		, m_pRasterizerState(NULL)
		, m_pDepthStencilState(NULL)
		, m_pBlendState(NULL)
		, m_pFrameBuffer(NULL)
	{
		memset(m_Textures, 0, MAX_TEXTURE_SAMPLER*sizeof(ui32));
		memset(m_TextureType, 0, MAX_TEXTURE_SAMPLER*sizeof(ui32));
	}

	Renderer::~Renderer()
	{
		LordSafeDelete(m_pDefaultRasterizerState);
		LordSafeDelete(m_pDefaultDepthStencilState);
		LordSafeDelete(m_pDefaultBlendState);
		LordSafeDelete(m_pMainViewport);
		LordSafeDelete(m_pRasterizerState);
		LordSafeDelete(m_pDepthStencilState);
		LordSafeDelete(m_pBlendState);
	}

	void Renderer::setRasterizerState(RasterizerState* pState)
	{
		LordAssert(pState);
		pState->active();

		if (!m_pRasterizerState)
		{
			m_pRasterizerState = LordNew RasterizerState::RasterizerDesc;
		}
		memcpy(m_pRasterizerState, &pState->getDesc(), sizeof(RasterizerState::RasterizerDesc));
	}

	void Renderer::setDepthStencilState(DepthStencilState* pState)
	{
		LordAssert(pState);
		pState->active();
		if (!m_pDepthStencilState)
		{
			m_pDepthStencilState = LordNew DepthStencilState::DepthStencilDesc;
		}
		memcpy(m_pDepthStencilState, &pState->getDesc(), sizeof(DepthStencilState::DepthStencilDesc));
	}

	void Renderer::setBlendState(BlendState* pState)
	{
		LordAssert(pState);
		pState->active();
		if (!m_pBlendState)
		{
			m_pBlendState = LordNew BlendState::BlendDesc;
		}
		memcpy(m_pBlendState, &pState->getDesc(), sizeof(BlendState::BlendDesc));
	}

	bool Renderer::isFullscreen() const
	{
		return m_bFullscreen;
	}

	bool Renderer::isVSync() const
	{
		return m_bVSync;
	}

	SamplerState* Renderer::getSamplerState(uint stage) const
	{
		StageSamplerMap::const_iterator it = m_stageSamplerMap.find(stage);
		if(it != m_stageSamplerMap.end())
			return it->second;
		else
			return NULL;
	}

	int Renderer::getRenderError() const
	{
		return -1;
	}
	
#ifdef LORD_INCLUDE_GUI
	void Renderer::renderText(const GUIString& strText, const Vector2& pos, GUIFont* pFont, const Color& color, const Vector2& scale, bool isLogicPosition, vector<FontBuffer>::type* fontBuffer)
	{
		LordAssert(pFont);
		Vector2 outPosition = pos;
		if (isLogicPosition == false)
		{
			GUISystem::Instance()->AdaptPositionEx(pos, outPosition);
		}

		// GUIRenderManager::Instance()->BeginRender();
		// 目前没用
		ERect rtClip;
		rtClip.left = 0;
		rtClip.top = 0;
		rtClip.right = (float)GUIRenderManager::Instance()->GetWidth();
		rtClip.bottom = (float)GUIRenderManager::Instance()->GetHeight();
		pFont->RenderText(strText, outPosition, rtClip, color, true, Color::BLACK, false, Color::BLACK, scale, NULL, fontBuffer);
		// GUIRenderManager::Instance()->EndRender();
	}

#endif

	void Renderer::project(Vector3& screenPos, const Vector3& worldPos, const Matrix4& matVP, Viewport* pViewport)
	{
		if (!pViewport)	pViewport = m_pMainViewport;

		if (pViewport->getWidth() == 0 ||
			pViewport->getHeight() == 0 )
		{
			return;
		}

		Vector4 vSSPos = Vector4(worldPos, 1.0);
		vSSPos = matVP.transform(vSSPos);

		Real invW = 1.0f / vSSPos.w;
		vSSPos.x *= invW;
		vSSPos.y *= invW;
		vSSPos.z *= invW;

		screenPos.x = (Real)pViewport->getLeft() + (1.0f + vSSPos.x) * (Real)pViewport->getWidth() / 2.0f;
		screenPos.y = (Real)pViewport->getTop() + (1.0f - vSSPos.y) * (Real)pViewport->getHeight() / 2.0f;
		screenPos.z = (1.0f + vSSPos.z) / 2.0f;
	}

	Vector3 Renderer::project(const Vector3& worldPos)
	{
		Vector3 screenPos = Vector3::ZERO;
		Camera* pMainCamera = SceneManager::Instance()->getMainCamera();
		Renderer::Instance()->project(screenPos, worldPos, pMainCamera->getViewProjMatrix());

		return screenPos;
	}

	void Renderer::unproject(Vector3& worldPos, const Vector3& screenPos, const Matrix4& matVP, Viewport* pViewport)
	{
		if (!pViewport)	pViewport = m_pMainViewport;

		Matrix4 matVPInv = matVP;
		matVPInv.detInverse();

		Vector4 vWSPos = Vector4(screenPos,1.0f);
		vWSPos.x = (screenPos.x - pViewport->getLeft())/(Real)pViewport->getWidth() * 2.0f - 1.0f;
		vWSPos.y = 1 - (screenPos.y - pViewport->getTop())/(Real)pViewport->getHeight() * 2.0f;

		Vector4 vWorld = vWSPos * matVPInv;
		vWorld /= vWorld.w;

		worldPos = (Vector3)vWorld;
	}

	Texture* Renderer::createTextureFromImage(Image* pImage, Dword usage)
	{
		LordAssert(pImage)

			PixelFormat pixFmt = pImage->getPixelFormat();

#ifdef LORD_ENDIAN_LITTLE
		switch(pixFmt)
		{
		case PF_BGR8_UNORM:		pixFmt = PF_RGB8_UNORM;		break;
		case PF_BGRA8_UNORM:	pixFmt = PF_RGBA8_UNORM;	break;
		default: ;
		}
#endif

		uint width = pImage->getWidth();
		uint height = pImage->getHeight();
		uint depth = pImage->getDepth();
		uint numMipmaps = pImage->getNumMipmaps();
		if(numMipmaps == 0)
			numMipmaps = 1;

		uint surSize = PixelUtil::CalcSurfaceSize(width, height, depth, numMipmaps, pixFmt);
		Buffer buff(surSize, pImage->getPixelBox().pData);

		/*
		// generate mipmap
		uint arraySize = images.size();
		Texture::TexArray texArr(arraySize);

		// note!! need to support cubemap, unimplement now.
		if(numMipmaps > Image::MAX_MINMAPS)
		{
		numMipmaps = Image::MAX_MINMAPS;
		}
		else if(numMipmaps == 0)
		{
		uint nSize = width > height ? width : height;
		int nMipmap = 1;
		uint mask = 0x01;
		for(uint i = 0; i < Image::MAX_MINMAPS; ++i, ++nMipmap, mask <<= 1)
		{
		if (nSize & mask)
		break;
		}
		numMipmaps = nMipmap;
		}

		for(uint i = 0; i < arraySize; ++i)
		{
		Texture::TexMipList &texMips = texArr[i];
		texMips.resize(numMipmaps);

		uint texWidth = width;
		uint texHeight = height;
		void *pData = images[i]->getPixelBox().pData;

		for(uint mip = 0; mip < numMipmaps; ++mip)
		{
		Texture::TexMipmap texMip;

		if(mip == 0)
		{
		texMip.pData = pData;
		}
		else
		{
		PixelBox srcPixBox(texWidth, texHeight, depth, pixFmt, pData);

		texWidth /= 2;
		texHeight /= 2;
		PixelBox dstPixBox(texWidth, texHeight, depth, pixFmt);
		dstPixBox.pData = LORD_ALLOC_T(Byte, dstPixBox.getConsecutiveSize());

		Image::Scale(srcPixBox, dstPixBox);
		texMip.pData = dstPixBox.pData;
		}

		texMip.size = PixelUtil::GetMemorySize(texWidth, texHeight, depth, pixFmt);
		texMip.pitch = texMip.size / texHeight;
		texMips[mip] = texMip;

		pData = texMip.pData;
		} // for
		} // for
		*/


		Texture* pTexture = NULL;

		if(pImage->getHeight() == 1)		// Texture1D
		{
		}
		else if(pImage->hasFlag(Image::IMGFLAG_3DTEX))		// Texture3D
		{
		}
		else if(pImage->hasFlag(Image::IMGFLAG_CUBEMAP))	// TextureCube
		{	
		}
		else
		{
			pTexture = createTexture2D(pixFmt, usage, width, height, numMipmaps, buff);
		}

		/*
		// free memory
		for(uint i = 0; i < arraySize; ++i)
		{
		Texture::TexMipList &texMips = texArr[i];
		for(uint mip = 0; mip < numMipmaps; ++mip)
		{
		if(mip > 0)
		{
		LordFree(texMips[mip].pData);
		}
		} // for
		} // for
		*/

		return pTexture;
	}

	bool Renderer::renderScene()
	{        
		Root::Instance()->TriggerCalcThread();

		clearCacheTexture();

#ifdef LORD_USE_RENDERTARGET
		LordAssert(RenderStageManager::Instance());
		RenderStageManager::Instance()->processRenderStages();
#else
		// set viewport
		Viewport* pViewport = Renderer::Instance()->getMainViewport();
		LordAssert(pViewport);
		Renderer::Instance()->setViewport(pViewport);
		m_pFrameBuffer->clear(FrameBuffer::CM_COLOR | FrameBuffer::CM_DEPTH, Renderer::BGCOLOR, 1.0f, 0);

		SceneManager::Instance()->render();
#endif
		return true;
	}
}
