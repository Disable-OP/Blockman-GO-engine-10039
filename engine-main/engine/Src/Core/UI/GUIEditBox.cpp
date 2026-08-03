#include "GUIEditBox.h"
#include "GUIFont.h"
#include "GUISystem.h"
#include "GUIImagesetManager.h"
#include "GUIImageset.h"
#include "GUIImage.h"
#include "GUIRenderManager.h"
#include "GUIEditBoxImpl.h"
#include "GUIWindowManager.h"

namespace LORD
{

#if(LORD_PLATFORM == LORD_PLATFORM_LINUX)

    GUIEditBoxImpl* __createSystemEditBox(GUIEditBox* pEditBox)
    {
        //return new EditBoxImplWin(pEditBox);
        return NULL;
    }

    void __destroySystemEditBox(GUIEditBoxImpl* pEditBoxImpl)
    {

    }
#endif

	GUIEditBox::GUIEditBox( GUIWindowType nType, const GUIString& strName )
		: GUIWindow(nType, strName)
		, m_fCaratOffset(0.0f)
		, m_bBlinkshow(false)
		, m_bIsPassword(false)
		, m_fTextOffset(0.0f)
		, m_nMaxTextLength(10)
		, m_fClipOffset(0.0f)
		, m_pImpl(NULL)
	{
		m_RenderText.SetFont(GetFont());
		m_RenderText.SetTextVertAlign(VA_CENTRE);
		m_RenderText.SetTextHorzAlign(HA_LEFT);
		m_TextOriColor=m_RenderText.GetTextColor();
		SetClipChild(false);

		m_pImpl = __createSystemEditBox(this);
		LordAssert(m_pImpl);
		m_pImpl->setInputMode(IM_ANY);

	}

	GUIEditBox::~GUIEditBox()
	{
		if (m_pImpl)
		{
			__destroySystemEditBox(m_pImpl);
			m_pImpl = NULL;
		}
	}

	bool GUIEditBox::SetProperty( const GUIString& strName, const GUIString& strValue )
	{
		if (GUIWindow::SetProperty(strName, strValue))
		{
			return true;
		}

		if (strName == GWP_EDIT_BACK_IMAGE)
		{
			SetBackImage(strValue);
			return true;
		}
// 		else if (strName == GWP_EDIT_CARAT_IMAGE)
// 		{
// 			SetCaratImage(strValue);
// 			return true;
// 		}
		else if (strName == GWP_EDIT_PASSWORD)
		{
			m_bIsPassword = StringUtil::ParseBool(strValue.c_str());
			return true;
		}
		else if (strName == GWP_EDIT_TEXT_OFFSET)
		{
			m_fTextOffset = StringUtil::ParseFloat(strValue.c_str());
			return true;
		}
		else if (strName == GWP_EDIT_MAX_TEXT_LENGTH)
		{
			m_nMaxTextLength = StringUtil::ParseInt(strValue.c_str());
			return true;
		}

		return false;
	}

	bool GUIEditBox::GetProperty( const GUIString& strName, GUIString& strValue )
	{
		if (GUIWindow::GetProperty(strName, strValue))
		{
			return true;
		}

		if (strName == GWP_EDIT_BACK_IMAGE)
		{
			strValue = m_BackImage.GetWholeImageName();
			return true;
		}
// 		else if (strName == GWP_EDIT_CARAT_IMAGE)
// 		{
// 			strValue = m_CaratImage.GetWholeImageName();
// 			return true;
// 		}
		else if (strName == GWP_EDIT_PASSWORD)
		{
			strValue = StringUtil::ToString(m_bIsPassword).c_str();
			return true;
		}
		else if (strName == GWP_EDIT_TEXT_OFFSET)
		{
			strValue = StringUtil::ToString(m_fTextOffset).c_str();
			return true;
		}
		else if (strName == GWP_EDIT_MAX_TEXT_LENGTH)
		{
			strValue = StringUtil::ToString(m_nMaxTextLength).c_str();
			return true;
		}

		return false;
	}

	void GUIEditBox::GetPropertyList( GUIPropertyList& properlist )
	{
		GUIWindow::GetPropertyList(properlist);
		properlist.push_back(GWP_EDIT_BACK_IMAGE);
		//properlist.push_back(GWP_EDIT_CARAT_IMAGE);
		properlist.push_back(GWP_EDIT_PASSWORD);
		properlist.push_back(GWP_EDIT_TEXT_OFFSET);
		properlist.push_back(GWP_EDIT_MAX_TEXT_LENGTH);
	}

	bool GUIEditBox::GetPropertyType( const GUIString& strName, GUIPropertyType& type )
	{
		if (GUIWindow::GetPropertyType(strName, type))
		{
			return true;
		}

		if (strName == GWP_EDIT_BACK_IMAGE)
		{
			type = GPT_IMAGE;
			return true;
		}
// 		else if (strName == GWP_EDIT_CARAT_IMAGE)
// 		{
// 			type = GPT_IMAGE;
// 			return true;
// 		}
		else if (strName == GWP_EDIT_PASSWORD)
		{
			type = GPT_BOOL;
			return true;
		}
		else if (strName == GWP_EDIT_TEXT_OFFSET)
		{
			type = GPT_FLOAT;
			return true;
		}
		else if (strName == GWP_EDIT_MAX_TEXT_LENGTH)
		{
			type = GPT_INT;
			return true;
		}

		return false;
	}

	void GUIEditBox::DrawSelf()
	{
		ERect rtOuter = GetRenderArea();
		m_BackImage.DrawStretchImage(this, rtOuter, getImageRenderColor(), m_StretchType, m_StretchOffset, m_fRotate, m_fRotateY, m_fRotateX, &GetClippedOuterRect());

//		if (IsActive() && m_bBlinkshow)
//		{
//			ERect rtDest = rtOuter;
//			rtDest.left += m_fCaratOffset;
//			rtDest.right = rtDest.left + 2;
//			rtDest.top += 5;
//			rtDest.bottom -= 5;
//
//			m_CaratImage.DrawImage(rtDest, m_drawColor, m_fRotate);
//		}

// 		if (!IsEnabled())
// 			m_RenderText.SetTextColor(Color(m_drawColor));
// 		else
// 			m_RenderText.SetTextColor(m_TextOriColor);

		m_RenderText.SetTextColor(getTextRenderColor());

		ERect textRect = rtOuter;
		textRect.left += m_fTextOffset + m_fClipOffset;
		textRect.right += m_fTextOffset;
		// clip area
		GUIRenderManager::Instance()->DrawBuffer();
		ERect rtClipRect = GetScreenRenderArea();
		Renderer::Instance()->scissor((uint)PixelAligned(rtClipRect.left+m_fTextOffset), (uint)PixelAligned(rtClipRect.top), 
			(uint)PixelAligned(rtClipRect.getWidth()-2*m_fTextOffset), (uint)PixelAligned(rtClipRect.getHeight()));

		m_RenderText.DrawGUIText(textRect, this);

		// 画完了要设置回去
		GUIRenderManager::Instance()->DrawBuffer();
		Renderer::Instance()->endScissor();

	}

	bool GUIEditBox::TouchUp( const Vector2& position )
	{
		GUISystem::Instance()->SetActiveEditBox(this);

		if(m_pImpl)
		{
			m_pImpl->setText(GetText().c_str());
			m_pImpl->openKeyboard();
		}

		return true;
	}

//	bool GUIEditBox::IsActive()
//	{
//		return GUISystem::Instance()->GetActiveEditBox() == this;
//	}

	void GUIEditBox::OnInputText( const GUIString& strText )
	{
		GUIString strTemp = strText;
		size_t nLen = strTemp.getGBKByteCount();
		if ((int)nLen > m_nMaxTextLength)
		{
			strTemp = strText.cutGBKByByteCount(m_nMaxTextLength);
		}

		SetText(strTemp);

		WindowEventArgs args(this);
		fireEvent(EventEditTextInput, args);
	}

	void GUIEditBox::SetBackImage( const GUIString& strBackImage )
	{
		m_BackImage.SetWholeImageName(strBackImage);
	}

	void GUIEditBox::SetCaratImage( const GUIString& strCaratImage )
	{
		m_CaratImage.SetWholeImageName(strCaratImage);
	}

	void GUIEditBox::UpdateSelf( ui32 nTimeElapse )
	{
//		static i32 timePassed = 0;

//		if (IsActive())
//		{
//			timePassed += nTimeElapse;
//
//			if (timePassed > 330 )
//			{
//				m_bBlinkshow = !m_bBlinkshow;
//
//				timePassed = 0;
//				Invalidate();
//			}
//		}
	}

	void GUIEditBox::OnTextChanged()
	{
		if (GUIWindowManager::Instance()->IsEnterBack())
		{
			setNeedRenderText();
			return;
		}
		GUIWindow::OnTextChanged();

		if (GetFont())
		{
			float fTextLen = GetFont()->GetTextExtent(GetText());
			float fWidth = GetUnclippedOuterRect().getWidth();

			// 减去两边的边界
			if (fTextLen > (fWidth - m_fTextOffset*2))
			{
				m_fClipOffset = fWidth - fTextLen - m_fTextOffset*2;
				m_fCaratOffset = fWidth - m_fTextOffset;
			}
			else
			{
				m_fClipOffset = 0.0f;
				m_fCaratOffset = fTextLen;
			}

			if (m_bIsPassword)
			{
				GUIString strPassword;
				strPassword.assign( GetText().length(), '*');
				m_RenderText.SetText(strPassword);
			}
			else
			{
				m_RenderText.SetText(GetText());
			}
		}
	}

	void GUIEditBox::OnSized()
	{
		GUIWindow::OnSized();

		m_RenderText.SetWidth(GetPixelSize().x);
	}

	void GUIEditBox::OnFontChanged()
	{
		m_RenderText.SetFont(GetFont());
	}

	void GUIEditBox::SetTextVertAlign( VerticalAlignment vertAlign )
	{
		m_RenderText.SetTextVertAlign(vertAlign);
	}

	void GUIEditBox::SetTextHorzAlign( HorizontalAlignment horzAlign )
	{
		m_RenderText.SetTextHorzAlign(horzAlign);
	}
    
    void GUIEditBox::OpenKeyboard()
    {
        if (m_pImpl) {
            m_pImpl->openKeyboard();
        }
    }
    
    void GUIEditBox::CloseKeyboard()
    {
        if (m_pImpl) {
            m_pImpl->closeKeyboard();
        }
    }
}
