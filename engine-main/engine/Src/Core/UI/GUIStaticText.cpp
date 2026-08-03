#include "GUIStaticText.h"
#include "GUIFont.h"
#include "GUISystem.h"
#include "GUIWindowManager.h"

namespace LORD
{
	GUIStaticText::GUIStaticText( GUIWindowType nType, const GUIString& strName )
		: GUIWindow(nType, strName)
		, m_SelfAdaptHigh(false)
		, m_showOneLine(false)
		, m_fScale(1.0f)
	{
		m_RenderText.SetFont(GetFont());
		m_TextOriColor=m_RenderText.GetTextColor();
	}

	GUIStaticText::~GUIStaticText()
	{
	}

	void GUIStaticText::SetTextLineExtraSpace(float lineExtraSpace)
	{
		m_RenderText.SetLineExtraSpace(lineExtraSpace);
	}

	void GUIStaticText::SetTextColor( const Color& cColor )
	{
		m_RenderText.SetTextColor(cColor);
		m_TextOriColor=m_RenderText.GetTextColor();
	}

	void GUIStaticText::SetTextBoader(const Color& cColor)
	{
		m_RenderText.SetBordered(true);
		m_RenderText.SetBorderColor(cColor);
	}

	bool GUIStaticText::SetProperty( const GUIString& strName, const GUIString& strValue )
	{
		if (GUIWindow::SetProperty(strName, strValue))
		{
			return true;
		}

		if (strName == GWP_TEXT_COLOR)
		{
			SetTextColor(StringUtil::ParseColor(strValue.c_str()));
			return true;
		}
		else if (strName == GWP_TEXT_WORDWRAP)
		{
			SetWordWrap(StringUtil::ParseBool(strValue.c_str()));
			return true;
		}
		else if (strName == GWP_TEXT_SELFADAPT_HIGH)
		{
			SetSelfAdaptionArea(StringUtil::ParseBool(strValue.c_str()));
			return true;
		}
		else if (strName == GWP_TEXT_HORZ_ALIGN)
		{
			HorizontalAlignment align;
			if (strValue == "Centre")
			{
				align = HA_CENTRE;
			}
			else if (strValue == "Right")
			{
				align = HA_RIGHT;
			}
			else
			{
				align = HA_LEFT;
			}
			SetTextHorzAlign(align);
			return true;
		}
		else if (strName == GWP_TEXT_VERT_ALIGN)
		{
			VerticalAlignment align;
			if (strValue == "Centre")
			{
				align = VA_CENTRE;
			}
			else if (strValue == "Bottom")
			{
				align = VA_BOTTOM;
			}
			else
			{
				align = VA_TOP;
			}
			SetTextVertAlign(align);
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
		else if (strName == GWP_TEXT_ALLSHOW_ONELINE)
		{
			AllShowOneLine(StringUtil::ParseBool(strValue.c_str()));
			return true;
		}
		else if (strName == GWP_TEXT_SCALE)
		{
			SetTextScale(StringUtil::ParseFloat(strValue.c_str()));
			return true;
		}
		else if (strName == GWP_TEXT_LINE_EXTRA_SPACE)
		{
			SetTextLineExtraSpace(StringUtil::ParseFloat(strValue.c_str()));
			return true;
		}

		return false;
	}

	bool GUIStaticText::GetProperty( const GUIString& strName, GUIString& strValue )
	{
		if (GUIWindow::GetProperty(strName, strValue))
		{
			return true;
		}

		if (strName == GWP_TEXT_COLOR)
		{
			strValue = StringUtil::ToString(m_RenderText.GetTextColor()).c_str();
			return true;
		}
		else if (strName == GWP_TEXT_WORDWRAP)
		{
			strValue = StringUtil::ToString(m_RenderText.IsWordWrap()).c_str();
			return true;
		}
		else if (strName == GWP_TEXT_SELFADAPT_HIGH)
		{
			strValue = StringUtil::ToString(m_SelfAdaptHigh).c_str();
			return true;
		}
		else if (strName == GWP_TEXT_HORZ_ALIGN)
		{
			switch(m_RenderText.GetTextHorzAlign())
			{
			case HA_CENTRE:
				strValue = "Centre";
				break;

			case HA_RIGHT:
				strValue = "Right";
				break;

			default:
				strValue = "Left";
			}
			return true;
		}
		else if (strName == GWP_TEXT_VERT_ALIGN)
		{
			switch(m_RenderText.GetTextVertAlign())
			{
			case VA_CENTRE:
				strValue = "Centre";
				break;

			case VA_BOTTOM:
				strValue = "Bottom";
				break;

			default:
				strValue = "Top";
			}
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
		else if (strName == GWP_TEXT_ALLSHOW_ONELINE)
		{
			strValue= StringUtil::ToString(m_showOneLine).c_str();
			return true;
		}
		else if (strName == GWP_TEXT_SCALE)
		{
			strValue = StringUtil::ToString(m_fScale).c_str();
			return true;
		}
		else if (strName == GWP_TEXT_LINE_EXTRA_SPACE)
		{
			strValue = StringUtil::ToString(m_RenderText.GetLineExtraSpace()).c_str();
			return true;
		}

		return false;
	}

	void GUIStaticText::GetPropertyList( GUIPropertyList& properlist )
	{
		GUIWindow::GetPropertyList(properlist);
		properlist.push_back(GWP_TEXT_COLOR);
		properlist.push_back(GWP_TEXT_HORZ_ALIGN);
		properlist.push_back(GWP_TEXT_VERT_ALIGN);
		properlist.push_back(GWP_TEXT_WORDWRAP);
		properlist.push_back(GWP_TEXT_SELFADAPT_HIGH);
		properlist.push_back(GWP_TEXT_SHADOW);
		properlist.push_back(GWP_TEXT_SHADOW_COLOR);
		properlist.push_back(GWP_TEXT_BORDER);
		properlist.push_back(GWP_TEXT_BORDER_COLOR);
		properlist.push_back(GWP_TEXT_ALLSHOW_ONELINE);
		properlist.push_back(GWP_TEXT_SCALE);
		properlist.push_back(GWP_TEXT_LINE_EXTRA_SPACE);
	}

	bool GUIStaticText::GetPropertyType( const GUIString& strName, GUIPropertyType& type )
	{
		if (GUIWindow::GetPropertyType(strName, type))
		{
			return true;
		}

		if (strName == GWP_TEXT_COLOR)
		{
			type = GPT_COLOR;
			return true;
		}
		else if (strName == GWP_TEXT_WORDWRAP)
		{
			type = GPT_BOOL;
			return true;
		}
		else if (strName == GWP_TEXT_SELFADAPT_HIGH)
		{
			type = GPT_BOOL;
			return true;
		}
		else if (strName == GWP_TEXT_HORZ_ALIGN)
		{
			type = GPT_HA;
			return true;
		}
		else if (strName == GWP_TEXT_VERT_ALIGN)
		{
			type = GPT_VA;
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
		else if (strName == GWP_TEXT_ALLSHOW_ONELINE)
		{
			type = GPT_BOOL;
			return true;
		}
		else if (strName == GWP_TEXT_SCALE)
		{
			type = GPT_FLOAT;
			return true;
		}
		return false;
	}

	void GUIStaticText::DrawSelf()
	{
// 		if (!IsEnabled())
// 			m_RenderText.SetTextColor(Color(m_drawColor));
// 		else
// 			m_RenderText.SetTextColor(m_TextOriColor);

		m_RenderText.SetTextColor(getTextRenderColor());
		m_RenderText.SetTextScale(m_fScale);
		m_RenderText.DrawGUIText(GetRenderArea(), this, m_bEnableColorRect, m_ColorRect);
	}

	void GUIStaticText::SetTextVertAlign( VerticalAlignment vertAlign )
	{
		m_RenderText.SetTextVertAlign(vertAlign);
	}

	void GUIStaticText::SetTextHorzAlign( HorizontalAlignment horzAlign )
	{
		m_RenderText.SetTextHorzAlign(horzAlign);
	}

	void GUIStaticText::OnTextChanged()
	{
		if (GUIWindowManager::Instance()->IsEnterBack())
		{
			setNeedRenderText();
			return;
		}
		m_RenderText.SetTextScale(m_fScale);
		m_RenderText.SetText(GetText());
		SetSelfAdaptionArea(m_SelfAdaptHigh);	

		//GUISystem::Instance()->GetEventCallback()(GET_STATICTEXT_TEXTCHANGED, GetOwnerDialogName(), GetName(), 0, 0);
	}

	void GUIStaticText::SetWordWrap( bool bWordWrap )
	{
		if (bWordWrap)
			m_showOneLine=false;
		m_RenderText.SetTextScale(m_fScale);
		m_RenderText.SetWordWrap(bWordWrap);
		SetSelfAdaptionArea(m_SelfAdaptHigh);		
	}

	void GUIStaticText::SetSelfAdaptionArea(bool selfAdapt)
	{
		m_SelfAdaptHigh=selfAdapt;
		if(GetText().length() == 0)//ÄÚÈÝ¿Õ
			return;
		if (m_SelfAdaptHigh)
		{	
			if (m_showOneLine)
				SetHeight(UDim(0,m_RenderText.GetFont()->GetTextHigh(GetText())));
			else
			{
				m_RenderText.LayoutWordWrapText();				
				SetHeight(UDim(0,m_RenderText.GetTextStringHigh()));					
			}					
		}
	}

	void GUIStaticText::AllShowOneLine(bool showOneLine)
	{
		m_showOneLine= showOneLine;
		if (showOneLine)
		{
			SetWordWrap(false);			
			float txtWidth=m_RenderText.GetFont()->GetTextExtent(GetText());
			SetWidth(UDim(0,txtWidth));
			SetHeight(UDim(0,m_RenderText.GetFont()->GetTextHigh(GetText())));
			m_RenderText.SetTextVertAlign(VA_BOTTOM);
		}
	}

	void GUIStaticText::OnSized()
	{
		GUIWindow::OnSized();		
		m_RenderText.SetWidth(GetPixelSize().x);
		SetSelfAdaptionArea(m_SelfAdaptHigh);
	}

	void GUIStaticText::OnFontChanged()
	{
		m_RenderText.SetFont(GetFont());		
		SetSelfAdaptionArea(m_SelfAdaptHigh);
	}
}

