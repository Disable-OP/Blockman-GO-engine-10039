#include "GUIMultiLineEdit.h"
#include "GUIFont.h"
#include "GUISystem.h"
#include "GUIImagesetManager.h"
#include "GUIImageset.h"
#include "GUIImage.h"
#include "GUIWindowManager.h"

namespace LORD
{
	GUIMultiLineEdit::GUIMultiLineEdit( GUIWindowType nType, const GUIString& strName )
		: GUIEditBox(nType, strName)		
	{
		m_RenderText.SetWordWrap(true);
	}

	GUIMultiLineEdit::~GUIMultiLineEdit()
	{
	}
	bool GUIMultiLineEdit::SetProperty( const GUIString& strName, const GUIString& strValue )
	{
		if (GUIEditBox::SetProperty(strName, strValue))
		{
			return true;
		}

// 		if (strName == GWP_EDIT_BACK_IMAGE)
// 		{
// 			SetBackImage(strValue);
// 			return true;
// 		}
		
		return false;
	}

	bool GUIMultiLineEdit::GetProperty( const GUIString& strName, GUIString& strValue )
	{
		if (GUIEditBox::GetProperty(strName, strValue))
		{
			return true;
		}

// 		if (strName == GWP_EDIT_BACK_IMAGE)
// 		{
// 			strValue = m_BackImage.GetWholeImageName();
// 			return true;
// 		}		
		return false;
	}

	void GUIMultiLineEdit::GetPropertyList( GUIPropertyList& properlist )
	{
		GUIEditBox::GetPropertyList(properlist);
		
	}

	bool GUIMultiLineEdit::GetPropertyType( const GUIString& strName, GUIPropertyType& type )
	{
		if (GUIEditBox::GetPropertyType(strName, type))
		{
			return true;
		}

// 		if (strName == GWP_EDIT_BACK_IMAGE)
// 		{
// 			type = GPT_IMAGE;
// 			return true;
// 		}		
		return false;
	}

	void GUIMultiLineEdit::DrawSelf()
	{
		m_BackImage.DrawStretchImage(this, GetRenderArea(), getImageRenderColor(), m_StretchType, m_StretchOffset, m_fRotate, m_fRotateY, m_fRotateX, &GetClippedOuterRect());

//		if (IsActive() && m_bBlinkshow)
//		{
//			size_t size= m_RenderText.GetTextStringList().size();
//			ERect rtDest   = GetRenderArea();
//			rtDest.left   += m_fCaratOffset;
//			rtDest.right  = rtDest.left + 2;
//			rtDest.top    +=(size-1)*GetFont()->GetLineSpacing() ;//+ 5;
//			rtDest.bottom = rtDest.top + GetFont()->GetLineSpacing();// - 5;
//
//			m_CaratImage.DrawImage(rtDest, m_drawColor, m_fRotate);
//		}

// 		if (!IsEnabled())
// 			m_RenderText.SetTextColor(Color(m_drawColor));
// 		else
// 			m_RenderText.SetTextColor(m_TextOriColor);
		m_RenderText.SetTextColor(getTextRenderColor());
		m_RenderText.DrawGUIText(GetRenderArea(), this);
	}

	void GUIMultiLineEdit::OnTextChanged()
	{
		if (GUIWindowManager::Instance()->IsEnterBack())
		{
			setNeedRenderText();
			return;
		}
		if (GetFont())
		{
			m_RenderText.SetText(GetText());
			UpdateCaratOffset();		
		}
	}
	void GUIMultiLineEdit::OnFontChanged()
	{
		m_RenderText.SetFont(GetFont());
		UpdateCaratOffset();
	}

	void GUIMultiLineEdit::OnSized()
	{
		GUIEditBox::OnSized();		
		UpdateCaratOffset();
	}
	void GUIMultiLineEdit::UpdateCaratOffset()
	{
		const TextStringList& tstrList=m_RenderText.GetTextStringList();
		size_t size=tstrList.size();
		if (0<size)
		{
			m_fCaratOffset = GetFont()->GetTextExtent(tstrList[size-1]);
		}	
	}
}
