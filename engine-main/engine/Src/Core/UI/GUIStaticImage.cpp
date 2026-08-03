#include "GUIStaticImage.h"
#include "GUIImagesetManager.h"
#include "GUIImage.h"
#include "GUIImageset.h"
#include "GUIRenderManager.h"
#include "Render/Material.h"
#include "Object/Root.h"
#include "GUISystem.h"

namespace LORD
{


	GUIStaticImage::GUIStaticImage( GUIWindowType nType, const GUIString& strName )
		: GUIWindow(nType, strName)
// 		, m_bCoolDown(false)
// 		, m_fMaskPercent(0.0f)
// 		, m_nCoolDownTime(0)
// 		, m_nPassedTime(0)
// 		, m_bPlayCoolDown(false)
		, m_isImageFromFile(false)
		, m_imageTexture(NULL)
		, m_blurTexture(NULL)
		, m_rederAsBlock(false)
		, m_strOverlayFile("")
		, m_overlayColor(1.f, 1.f, 1.f, 1.f)
	{
	}

	GUIStaticImage::~GUIStaticImage()
	{
		// 清理文件的texture
		if (m_imageTexture)
		{
			TextureManager::Instance()->releaseResource(m_imageTexture);
			m_imageTexture = NULL;
		}
	}

	bool GUIStaticImage::SetProperty( const GUIString& strName, const GUIString& strValue )
	{
		if (GUIWindow::SetProperty(strName, strValue))
		{
			return true;
		}

		if (strName == GWP_SI_IMAGE_NAME)
		{
			SetImage(strValue);
			return true;
		}
// 		else if (strName == GWP_SI_SHOW_MASK)
// 		{
// 			//m_bCoolDown = StringUtil::ParseBool(strValue.c_str());
// 			return true;
// 		}
// 		else if (strName == GWP_SI_MASK_PERCENT)
// 		{
// 			//m_fMaskPercent = StringUtil::ParseReal(strValue.c_str());
// 			return true;
// 		}
		//add by maxicheng
		else if (strName == GWP_BTN_HEXAGON_TOP_IMG)
		{
			m_hexagon.SetImageTopName(strValue);
			return true;
		}
		else if (strName == GWP_BTN_HEXAGON_TOP_COLOR)
		{
			m_hexagon.setTopColor(StringUtil::ParseColor(strValue.c_str()));
			return true;
		}
		else if (strName == GWP_BTN_HEXAGON_SIDE_IMG)
		{
			m_hexagon.SetImageSideName(strValue);
			return true;
		}
		else if (strName == GWP_BTN_HEXAGON_SIDE_COLOR)
		{
			m_hexagon.setSideColor(StringUtil::ParseColor(strValue.c_str()));
			return true;
		}
		else if (strName == GWP_BTN_HEXAGON_OVER_IMG)
		{
			m_hexagon.SetImageOverlayerName(strValue);
			return true;
		}
		else if (strName == GWP_BTN_HEXAGON_OVER_COLOR)
		{
			m_hexagon.setOverlayerColor(StringUtil::ParseColor(strValue.c_str()));
			return true;
		}

		return false;
	}

	bool GUIStaticImage::GetProperty( const GUIString& strName, GUIString& strValue )
	{
		if (GUIWindow::GetProperty(strName, strValue))
		{
			return true;
		}

		if (strName == GWP_SI_IMAGE_NAME)
		{
			strValue = m_strTextureFile;
			return true;
		}
// 		else if (strName == GWP_SI_SHOW_MASK)
// 		{
// 			//strValue = StringUtil::ToString(m_bCoolDown).c_str();
// 			return true;
// 		}
// 		else if (strName == GWP_SI_MASK_PERCENT)
// 		{
// 			//strValue = StringUtil::ToString(m_fMaskPercent).c_str();
// 			return true;
// 		}
		//add by maxicheng
		else if (strName == GWP_BTN_HEXAGON_TOP_IMG)
		{
			strValue = m_hexagon.GetImageTopName();
			return true;
		}
		else if (strName == GWP_BTN_HEXAGON_TOP_COLOR)
		{
			strValue = StringUtil::ToString(m_hexagon.getTopColor()).c_str();
			return true;
		}
		else if (strName == GWP_BTN_HEXAGON_SIDE_IMG)
		{
			strValue = m_hexagon.GetImageSideName();
			return true;
		}
		else if (strName == GWP_BTN_HEXAGON_SIDE_COLOR)
		{
			strValue = StringUtil::ToString(m_hexagon.getSideColor()).c_str();
			return true;
		}
		else if (strName == GWP_BTN_HEXAGON_OVER_IMG)
		{
			strValue = m_hexagon.GetImageOverlayerName();
			return true;
		}
		else if (strName == GWP_BTN_HEXAGON_OVER_COLOR)
		{
			strValue = StringUtil::ToString(m_hexagon.getOverlayerColor()).c_str();
			return true;
		}
		return false;
	}

	void GUIStaticImage::GetPropertyList( GUIPropertyList& properlist )
	{
		GUIWindow::GetPropertyList(properlist);
		properlist.push_back(GWP_SI_IMAGE_NAME);
		//properlist.push_back(GWP_SI_SHOW_MASK);
		//properlist.push_back(GWP_SI_MASK_PERCENT);

		//add by maxicheng
		properlist.push_back(GWP_BTN_HEXAGON_TOP_IMG);
		properlist.push_back(GWP_BTN_HEXAGON_TOP_COLOR);
		properlist.push_back(GWP_BTN_HEXAGON_SIDE_IMG);
		properlist.push_back(GWP_BTN_HEXAGON_SIDE_COLOR);
		properlist.push_back(GWP_BTN_HEXAGON_OVER_IMG);
		properlist.push_back(GWP_BTN_HEXAGON_OVER_COLOR);

	}

	bool GUIStaticImage::GetPropertyType( const GUIString& strName, GUIPropertyType& type )
	{
		if (GUIWindow::GetPropertyType(strName, type))
		{
			return true;
		}

		if (strName == GWP_SI_IMAGE_NAME)
		{
			type = GPT_IMAGE;
			return true;
		}
// 		else if (strName == GWP_SI_SHOW_MASK)
// 		{
// 			type = GPT_BOOL;
// 			return true;
// 		}
// 		else if (strName == GWP_SI_MASK_PERCENT)
// 		{
// 			type = GPT_FLOAT;
// 			return true;
// 		}
		// add by maxicheng
		else if (strName == GWP_BTN_HEXAGON_TOP_IMG)
		{
			type = GPT_IMAGE;
			return true;
		}
		else if (strName == GWP_BTN_HEXAGON_TOP_COLOR)
		{
			type = GPT_COLOR;
			return true;
		}
		else if (strName == GWP_BTN_HEXAGON_SIDE_IMG)
		{
			type = GPT_IMAGE;
			return true;
		}
		else if (strName == GWP_BTN_HEXAGON_SIDE_COLOR)
		{
			type = GPT_COLOR;
			return true;
		}
		else if (strName == GWP_BTN_HEXAGON_OVER_IMG)
		{
			type = GPT_IMAGE;
			return true;
		}
		else if (strName == GWP_BTN_HEXAGON_OVER_COLOR)
		{
			type = GPT_COLOR;
			return true;
		}
		return false;
	}

	void GUIStaticImage::SetOverlayImage(const GUIString& strOverlayImage)
	{
		if (m_strOverlayFile != strOverlayImage)
		{
			m_strOverlayFile = strOverlayImage;

			if (!ResourceGroupManager::Instance()->resourceExistsInAnyGroup(m_strOverlayFile.c_str()))
			{
				// 如果是一个不存在的文件，说明不是文件，那么走原来的渲染流程
				// 否则的话认为是个文件，直接渲染文件
				m_overlayImage.SetWholeImageName(m_strOverlayFile);
			}
		}

		Invalidate();
	}

	void GUIStaticImage::SetImage( const GUIString& strImageName )
	{
		if (m_strTextureFile != strImageName)
		{
			m_strTextureFile = strImageName;

			if (!ResourceGroupManager::Instance()->resourceExistsInAnyGroup(m_strTextureFile.c_str()))
			{
				// 如果是一个不存在的文件，说明不是文件，那么走原来的渲染流程
				// 否则的话认为是个文件，直接渲染文件
				m_isImageFromFile = false;
				m_RenderImage.SetWholeImageName(m_strTextureFile);

				// 清理文件的texture
				if (m_imageTexture)
				{
					TextureManager::Instance()->releaseResource(m_imageTexture);
					m_imageTexture = NULL;
				}
			}
			else
			{
				m_isImageFromFile = true;

				if (m_imageTexture)
				{
					TextureManager::Instance()->releaseResource(m_imageTexture);
					m_imageTexture = NULL;
				}

				m_imageTexture = TextureManager::Instance()->createTexture(m_strTextureFile.c_str());
				m_imageTexture->load();
				SamplerState* pState = GUIRenderManager::Instance()->GetMaterial(GUIMaterialType_CULL_BACK)->getSamplerState(0);
				m_imageTexture->setSamplerState(pState);

				// 清理另一种渲染的数据
				m_RenderImage.SetWholeImageName("");

			}

		}

		Invalidate();
	}

	//add by maxicheng 
	void GUIStaticImage::SetHexagonImage(const GUIString& strTopImage, const GUIString& strSideImage)
	{
		m_hexagon.SetImageTopName(strTopImage);
		m_hexagon.SetImageSideName(strSideImage);
	}

	void GUIStaticImage::SetHexagonTopColor(const Color& color)
	{
		m_hexagon.setTopColor(color);
	}

	void GUIStaticImage::SetHexagonSideColor(const Color& color)
	{
		m_hexagon.setSideColor(color);
	}

	void GUIStaticImage::SetHexagonRenderHeight(float height)
	{
		m_hexagon.SetRenderHeight(height);
	}

	void GUIStaticImage::SetVertexData(ImageBufferListType &vertex)
	{
		m_hexagon.SetVertexData(vertex);
	}

	void GUIStaticImage::DrawSelf()
	{
		//add by maxicheng
		if (m_rederAsBlock)
		{
			//m_hexagon.DrawHexagon(this, GetRenderArea(), &GetClippedOuterRect());	
			m_hexagon.DrawHexagon2(this, GetRenderArea(), GetClippedOuterRect());
			return;		
		}
		
		
		if (m_blurTexture)
		{
			drawFromTexture(m_blurTexture, true);
			return;
		}

		if (m_isImageFromFile)
		{
			drawFromTexture(m_imageTexture);
			return;
		}
// 		if (m_bIsRenderTarget)
// 		{
// 			if (m_StretchType != ST_NONE)
// 			{
// 				m_RenderImage.DrawStretchImage(this, GetRenderArea(), m_drawColor, m_StretchType, m_StretchOffset, 0.0f, 0.0f, 0.0f, &GetClippedOuterRect());
// 			}
// 			else
// 			{
// 				m_RenderImage.DrawImage(this, GetRenderArea(), m_drawColor, 0.0f, m_fRotateY, m_fRotateX, &GetClippedOuterRect());
// 			}
// 		} 
// 		else
		{
			if (m_StretchType != ST_NONE)
			{
				m_RenderImage.DrawStretchImage(this, GetRenderArea(), getImageRenderColor(), m_StretchType, m_StretchOffset, m_fRotate, m_fRotateY, m_fRotateX, &GetClippedOuterRect());
				if (m_strOverlayFile != "")
					m_overlayImage.DrawStretchImage(this, GetRenderArea(), m_overlayColor, m_StretchType, m_StretchOffset, m_fRotate, m_fRotateY, m_fRotateX, &GetClippedOuterRect());
			}
			else
			{
				m_RenderImage.DrawImage(this, GetRenderArea(), getImageRenderColor(), m_fRotate, m_fRotateY, m_fRotateX, &GetClippedOuterRect());
				if (m_strOverlayFile != "")
					m_overlayImage.DrawImage(this, GetRenderArea(), m_overlayColor, m_fRotate, m_fRotateY, m_fRotateX, &GetClippedOuterRect());
			}
		}

// 		if (m_bCoolDown)
// 		{
// 			ERect rtArea = GetRenderArea();
// 			Vector2 position;
// 			position.x = rtArea.left + rtArea.getWidth() * 0.5f;
// 			position.y = rtArea.top + rtArea.getHeight() * 0.5f;
// 			GUIRenderManager::Instance()->AddCoolDown(position, 0.5f * rtArea.getWidth(), m_fMaskPercent);
// 		}
	}

	void GUIStaticImage::drawFromTexture(Texture* texture, bool isblur)
	{
		if (!texture)
		{
			//贴图不存在 字节return
			return;
		}

		ERect rtArea = GetRenderArea();
		// ERect rtDest = GetRenderArea();
		ERect clipRect = GetClippedOuterRect();

		if (isblur)
		{
			Vector2 lt = Vector2::ZERO;
			Vector2 rb = Vector2::ZERO;

			//计算一下新的屏幕坐标
			GUISystem::Instance()->AdaptPosition(Vector2::ZERO, lt);
			GUISystem::Instance()->AdaptPosition(Vector2((Real)Root::Instance()->getRealWidth(), (Real)Root::Instance()->getRealHeight()), rb);
			
			Vector3 lefttop = Vector3::ZERO;
			Vector3 rightbottom = Vector3::ZERO;
			Vector3 leftbottom = Vector3::ZERO;
			Vector3 righttop = Vector3::ZERO;

			lefttop.x = lt.x;
			lefttop.y = lt.y;

			rightbottom.x = rb.x;
			rightbottom.y = rb.y;

			leftbottom.x = lt.x;
			leftbottom.y = rb.y;

			righttop.x = rb.x;
			righttop.y = lt.y;

			ERect rtTexUV = ERect(0.0f, 1.0f, 1.0f, 0.0f);

			//GUIRenderManager::Instance()->EndRender();

 			//Renderer::Instance()->scissor(0, (uint)(0.5f * (Root::Instance()->getRealHeight() - GUISystem::Instance()->GetScreenHeight())), (uint)GUISystem::Instance()->GetScreenWidth(), (uint)GUISystem::Instance()->GetScreenHeight());
 			//GUIRenderManager::Instance()->BeginRender();

			GUIRenderManager::Instance()->AddQuadToBuffer(lefttop, rightbottom, leftbottom, righttop, rtTexUV, ColorRect(getImageRenderColor(), getImageRenderColor(), getImageRenderColor(), getImageRenderColor()),
				texture, getMaskTexture(), getMaterial());

			//GUIRenderManager::Instance()->EndRender();

			//Renderer::Instance()->endScissor();

			return;
		}

		ERect rtTexUV = ERect(0.0f, 0.0f, 1.0f, 1.0f);
		ERect rtOriUV = ERect(0.0f, 0.0f, 1.0f, 1.0f);

		// 如果有裁剪
		ERect rtDest = rtArea.intersect(clipRect);
		if (rtDest.getHeight() == 0 || rtDest.getWidth() == 0)
		{
			return;
		}

		float fInverseWidth = rtOriUV.getWidth() / rtArea.getWidth();
		float fInverseHeight = rtOriUV.getHeight() / rtArea.getHeight();

		rtTexUV.left = rtOriUV.left + (rtDest.left - rtArea.left) * fInverseWidth;
		rtTexUV.top = rtOriUV.top + (rtDest.top - rtArea.top) * fInverseHeight;
		rtTexUV.right = rtOriUV.left + (rtDest.right - rtArea.left) * fInverseWidth;
		rtTexUV.bottom = rtOriUV.top + (rtDest.bottom - rtArea.top) * fInverseHeight;

		float halfWidth = rtDest.getWidth() * 0.5f;
		float halfHeight = rtDest.getHeight() * 0.5f;
		Vector3 center = Vector3(rtDest.left + halfWidth, rtDest.top + halfHeight, 0.0f);

		Vector3 lt, rb, lb, rt;
		// 计算旋转
		GUIImage::CaculateRotate(rtDest, center, Vector3(m_fRotateX, m_fRotateY, m_fRotate), lt, rb, lb, rt);

		GUIRenderManager::Instance()->AddQuadToBuffer(lt*getActionMatrix(), rb*getActionMatrix(), lb*getActionMatrix(), rt*getActionMatrix(), rtTexUV, ColorRect(getImageRenderColor(), getImageRenderColor(), getImageRenderColor(), getImageRenderColor()),
			texture, getMaskTexture(), getMaterial());
	}

	void GUIStaticImage::setBlur(bool blur)
	{
		m_blurTexture = Root::Instance()->blurDefaultBuffer(blur);
	}

	void GUIStaticImage::changeTexture(const GUIString & strImageName)
	{
		if (m_strTextureFile != strImageName)
		{
			m_strTextureFile = strImageName;
			m_isImageFromFile = true;

			if (m_imageTexture)
			{
				TextureManager::Instance()->releaseResource(m_imageTexture);
				m_imageTexture = NULL;
			}

			m_imageTexture = TextureManager::Instance()->createTexture(m_strTextureFile.c_str(), Texture::TU_STATIC, RESOURCE_TYPE_OUTSIDE);
			m_imageTexture->load();
			SamplerState* pState = GUIRenderManager::Instance()->GetMaterial(GUIMaterialType_CULL_BACK)->getSamplerState(0);
			m_imageTexture->setSamplerState(pState);

			// 清理另一种渲染的数据
			m_RenderImage.SetWholeImageName("");
		}
	}

// 	void GUIStaticImage::SetCoolDown( float fCoolDownPercent )
// 	{
// 		m_fMaskPercent = fCoolDownPercent;
// 	}

// 	void GUIStaticImage::UpdateSelf( ui32 nTimeElapse )
// 	{
// 		GUIWindow::UpdateSelf(nTimeElapse);	
// 
// 		if (!m_bPlayCoolDown)
// 		{
// 			SetCoolDown(0.0f);
// 			return;
// 		}	
// 		
// 		if (m_bCoolDown)
// 		{
// 			if (m_nPassedTime < m_nCoolDownTime)
// 			{
// 				SetCoolDown(m_nPassedTime * 1.0f / m_nCoolDownTime);
// 
// 				m_nPassedTime += nTimeElapse;
// 			}
// 			else
// 			{
// 				// stop
// 				m_bCoolDown = false;
// 				m_nPassedTime = 0;
// 			}
// 		}
// 	}

// 	void GUIStaticImage::SetCoolDownTime( ui32 nPassTime, ui32 nCoolDownTime, bool bPlay)
// 	{
// 		if (nCoolDownTime == 0)
// 		{
// 			m_bCoolDown = false;
// 		}
// 		else
// 		{
// 			m_bCoolDown = true;
// 		}
// 
// 		m_bPlayCoolDown = bPlay;
// 		m_nCoolDownTime = nCoolDownTime;
// 		m_nPassedTime = nPassTime;
// 
// 	}

}
