#include "GUIButton.h"
#include "GUIFont.h"
#include "GUIImageset.h"
#include "GUIImagesetManager.h"
#include "GUISystem.h"
#include "GUIEventArgs.h"
#include "GUIWindowManager.h"
#include "Audio/SoundSystem.h"

namespace LORD
{
	const ui32 nBiggerTime = 100;
	const ui32 nSmallerTime = 100;
	const ui32 nWholeTime = 200;

	GUIButton::GUIButton( GUIWindowType nType, const GUIString& strName )
		: GUIWindow(nType, strName)
		, m_eState(BS_NORMAL)
		, m_nPlayedTime(0)
		, m_ePlayState(PS_NONE)
		, m_nSoundType(0)
		, m_fScale(1.0f)
	{
		m_RenderText.SetFont(GetFont());
		m_RenderText.SetTextVertAlign(VA_CENTRE);
		m_RenderText.SetTextHorzAlign(HA_CENTRE);
		m_TextOriColor=m_RenderText.GetTextColor();
	}

	GUIButton::~GUIButton()
	{
	}

	void GUIButton::SetTextColor( const Color& cColor )
	{
		m_RenderText.SetTextColor(cColor);
		m_TextOriColor=cColor;
	}

	bool GUIButton::SetProperty( const GUIString& strName, const GUIString& strValue )
	{
		if (GUIWindow::SetProperty(strName, strValue))
		{
			return true;
		}

		if (strName == GWP_BTN_TEXT_COLOR)
		{
			SetTextColor(StringUtil::ParseColor(strValue.c_str()));			
			return true;
		}
		else if (strName == GWP_BTN_NORM_IMAGE)
		{
			SetNormalImage(strValue);
			return true;
		}
		else if (strName == GWP_BTN_PUSH_IMAGE)
		{
			SetPushedImage(strValue);
			return true;
		}
		else if (strName == GWP_BTN_SOUND_NAME)
		{
			m_nSoundType = StringUtil::ParseInt(strValue.c_str());
			return true;
		}
		else if (strName == GWP_TEXT_SHADOW)
		{
			m_RenderText.SetShadowed(StringUtil::ParseBool(strValue.c_str()));
			return true;
		}
		else if (strName == GWP_TEXT_SHADOW_COLOR)
		{
			m_RenderText.SetShadowColor(StringUtil::ParseColor(strValue.c_str()));
			return true;
		}
		else if (strName == GWP_TEXT_BORDER)
		{
			m_RenderText.SetBordered(StringUtil::ParseBool(strValue.c_str()));
			return true;
		}
		else if (strName == GWP_TEXT_BORDER_COLOR)
		{
			m_RenderText.SetBorderColor(StringUtil::ParseColor(strValue.c_str()));
			return true;
		}
		else if (strName == GWP_BTN_TEXT_RENDER_OFFSET)
		{
			m_RenderText.setRenderOffset(StringUtil::ParseVec2(strValue.c_str()));
			return true;
		}
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

	bool GUIButton::GetProperty( const GUIString& strName, GUIString& strValue )
	{
		if (GUIWindow::GetProperty(strName, strValue))
		{
			return true;
		}

		if (strName == GWP_BTN_TEXT_COLOR)
		{
			strValue = StringUtil::ToString(m_RenderText.GetTextColor()).c_str();
			return true;
		}
		else if (strName == GWP_BTN_NORM_IMAGE)
		{
			strValue = m_NormalImage.GetWholeImageName();
			return true;
		}
		else if (strName == GWP_BTN_PUSH_IMAGE)
		{
			strValue = m_PushedImage.GetWholeImageName();
			return true;
		}
		else if (strName == GWP_BTN_SOUND_NAME)
		{
			strValue = StringUtil::ToString(m_nSoundType).c_str();
			return true;
		}
		else if (strName == GWP_TEXT_SHADOW)
		{
			strValue = StringUtil::ToString(m_RenderText.IsShadowed()).c_str();
			return true;
		}
		else if (strName == GWP_TEXT_SHADOW_COLOR)
		{
			strValue = StringUtil::ToString(m_RenderText.GetShadowColor()).c_str();
			return true;
		}
		else if (strName == GWP_TEXT_BORDER)
		{
			strValue = StringUtil::ToString(m_RenderText.IsBordered()).c_str();
			return true;
		}
		else if (strName == GWP_TEXT_BORDER_COLOR)
		{
			strValue = StringUtil::ToString(m_RenderText.GetBorderColor()).c_str();
			return true;
		}
		else if (strName == GWP_BTN_TEXT_RENDER_OFFSET)
		{
			strValue = StringUtil::ToString(m_RenderText.getRenderOffset()).c_str();
			return true;
		}
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
		else if (strName == GWP_TEXT_SCALE)
		{
			strValue = StringUtil::ToString(m_fScale).c_str();
			return true;
		}

		return false;
	}

	void GUIButton::GetPropertyList( GUIPropertyList& properlist )
	{
		GUIWindow::GetPropertyList(properlist);
		properlist.push_back(GWP_BTN_TEXT_COLOR);
		properlist.push_back(GWP_TEXT_SHADOW);
		properlist.push_back(GWP_TEXT_SHADOW_COLOR);
		properlist.push_back(GWP_TEXT_BORDER);
		properlist.push_back(GWP_TEXT_BORDER_COLOR);
		properlist.push_back(GWP_BTN_NORM_IMAGE);
		properlist.push_back(GWP_BTN_PUSH_IMAGE);
		//properlist.push_back(GWP_BTN_STRECHED_IMAGE);
		//properlist.push_back(GWP_BTN_STRECHED_OFFSET);
		properlist.push_back(GWP_BTN_SOUND_NAME);
		properlist.push_back(GWP_BTN_TEXT_RENDER_OFFSET);
		properlist.push_back(GWP_BTN_HEXAGON_TOP_IMG);
		properlist.push_back(GWP_BTN_HEXAGON_TOP_COLOR);
		properlist.push_back(GWP_BTN_HEXAGON_SIDE_IMG);
		properlist.push_back(GWP_BTN_HEXAGON_SIDE_COLOR);
		properlist.push_back(GWP_BTN_HEXAGON_OVER_IMG);
		properlist.push_back(GWP_BTN_HEXAGON_OVER_COLOR);
	}

	bool GUIButton::GetPropertyType( const GUIString& strName, GUIPropertyType& type )
	{
		if (GUIWindow::GetPropertyType(strName, type))
		{
			return true;
		}

		if (strName == GWP_BTN_TEXT_COLOR)
		{
			type = GPT_COLOR;
			return true;
		}
		else if (strName == GWP_BTN_NORM_IMAGE)
		{
			type = GPT_IMAGE;
			return true;
		}
		else if (strName == GWP_BTN_PUSH_IMAGE)
		{
			type = GPT_IMAGE;
			return true;
		}
// 		else if (strName == GWP_BTN_STRECHED_IMAGE)
// 		{
// 			type = GPT_BOOL;
// 			return true;
// 		}
// 		else if (strName == GWP_BTN_STRECHED_OFFSET)
// 		{
// 			type = GPT_FLOAT;
// 			return true;
// 		}
		else if (strName == GWP_BTN_SOUND_NAME)
		{
			type = GPT_STRING;
			return true;
		}
		else if (strName == GWP_TEXT_SHADOW)
		{
			type = GPT_BOOL;
			return true;
		}
		else if (strName == GWP_TEXT_SHADOW_COLOR)
		{
			type = GPT_COLOR;
			return true;
		}
		else if (strName == GWP_TEXT_BORDER)
		{
			type = GPT_BOOL;
			return true;
		}
		else if (strName == GWP_TEXT_BORDER_COLOR)
		{
			type = GPT_COLOR;
			return true;
		}
		else if (strName == GWP_BTN_TEXT_RENDER_OFFSET)
		{
			type = GPT_VECTOR2;
			return true;
		}
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

	void GUIButton::DrawSelf()
	{
		if (m_hexagon.isActive())
		{
			m_hexagon.DrawHexagon(this, GetRenderArea(), &GetClippedOuterRect());
		}
		else if (m_StretchType != ST_NONE)
		{
			if (m_eState == BS_NORMAL)
			{
				m_NormalImage.DrawStretchImage(this, GetRenderArea(), getImageRenderColor(), m_StretchType, m_StretchOffset, m_fRotate, m_fRotateY, m_fRotateX, &GetClippedOuterRect());
			}
			else
			{
				m_PushedImage.DrawStretchImage(this, GetRenderArea(), getImageRenderColor(), m_StretchType, m_StretchOffset, m_fRotate, m_fRotateY, m_fRotateX, &GetClippedOuterRect());
			}
		}
		else
		{
			if (m_eState == BS_NORMAL)
			{
				m_NormalImage.DrawImage(this, GetRenderArea(), getImageRenderColor(), m_fRotate, m_fRotateY, m_fRotateX, &GetClippedOuterRect());
			}
			else
			{
				m_PushedImage.DrawImage(this, GetRenderArea(), getImageRenderColor(), m_fRotate, m_fRotateY, m_fRotateX, &GetClippedOuterRect());
			}
		}

// 		if (!IsEnabled())
// 			m_RenderText.SetTextColor(Color(m_drawColor));
// 		else
// 			m_RenderText.SetTextColor(m_TextOriColor);

		m_RenderText.SetTextColor(getTextRenderColor());
		m_RenderText.DrawGUIText(GetRenderArea(), this, m_bEnableColorRect, m_ColorRect);
		m_RenderText.SetTextScale(m_fScale);
	}

	void GUIButton::SetNormalImage( const GUIString& strNormalImage )
	{
		m_NormalImage.SetWholeImageName(strNormalImage);
	}

	void GUIButton::SetPushedImage( const GUIString& strPushedImage )
	{
		m_PushedImage.SetWholeImageName(strPushedImage);
	}

	void GUIButton::SetHexagonImage(const GUIString& strTopImage, const GUIString& strSideImage)
	{
		m_hexagon.SetImageTopName(strTopImage);
		m_hexagon.SetImageSideName(strSideImage);
	}

	void GUIButton::SetHexagonRenderHeight(float height)
	{
		m_hexagon.SetRenderHeight(height);
	}

	bool GUIButton::TouchDown( const Vector2& position )
	{
		m_eState = BS_PUSHED;

		if (m_ePlayState == PS_NONE)
		{
			m_nPlayedTime = 0;
			m_OrignRect = GetArea();
			m_ePlayState = PS_DOWN;
		}

		WindowEventArgs args(this);
		fireEvent(EventWindowTouchDown, args);

		return true;
	}

	bool GUIButton::TouchUp( const Vector2& position )
	{
		// send click message
		if (m_eState ==BS_PUSHED)
		{
			m_eState = BS_NORMAL;

			WindowEventArgs args(this);
			fireEvent(EventButtonClick, args);

			if (m_ePlayState == PS_NONE)
			{
				m_OrignRect = GetArea();
			}

			m_ePlayState = PS_UP;
			m_nPlayedTime = 0;

			//SoundSystem::Instance()->playEffectByType((SoundType)m_nSoundType);
		}
		return true;
	}

	bool GUIButton::MotionRelease()
	{
		// dont send click message
 		m_eState = BS_NORMAL;
 		
 		m_ePlayState = PS_UP;
 		m_nPlayedTime = 0;
		
		WindowEventArgs args(this);
		fireEvent(EventMotionRelease, args);

		return true;
	}

	void GUIButton::OnTextChanged()
	{
		if (GUIWindowManager::Instance()->IsEnterBack())
		{
			setNeedRenderText();
			return;
		}
		m_RenderText.SetTextScale(m_fScale);
		m_RenderText.SetText(GetText());
	}

	void GUIButton::OnSized()
	{
		GUIWindow::OnSized();

		m_RenderText.SetWidth(GetPixelSize().x);
	}

	void GUIButton::OnFontChanged()
	{
		m_RenderText.SetFont(GetFont());
	}

	void GUIButton::UpdateSelf( ui32 nTimeElapse )
	{
		GUIWindow::UpdateSelf(nTimeElapse);
		
		if (m_ePlayState == PS_DOWN)
		{
			if (m_nPlayedTime < nBiggerTime)
			{
				float fPercent = m_nPlayedTime * 1.0f / nBiggerTime;
				fPercent = 1.0f + fPercent * 0.05f;
				//SetScale(fPercent, m_OrignRect);
				SetActionMatrixScale(Vector3(fPercent, fPercent, fPercent));
				m_nPlayedTime += nTimeElapse;
			}
			else
			{
				m_ePlayState = PS_STAY;
				m_nPlayedTime = 0;
			}
		}
		else if (m_ePlayState == PS_UP)
		{
			if (m_nPlayedTime < nSmallerTime)
			{
				float fPercent = 1.0f - m_nPlayedTime * 1.0f / nSmallerTime;
				fPercent = 1.0f + fPercent * 0.05f;
				SetActionMatrixScale(Vector3(fPercent, fPercent, fPercent));
				m_nPlayedTime += nTimeElapse;
			}
			else
			{
				m_ePlayState = PS_NONE;
				m_nPlayedTime = 0;
				SetActionMatrixScale(Vector3(1.0f, 1.0f, 1.0f));
			}
		}
		else
		{

		}
	}

}
