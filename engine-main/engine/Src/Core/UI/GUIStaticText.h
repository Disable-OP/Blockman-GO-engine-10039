/********************************************************************
filename: 	GUIStaticText.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/

#ifndef __GUI_STATIC_TEXT_H__
#define __GUI_STATIC_TEXT_H__

#include "Core.h"
#include "GUIWindow.h"
#include "GUIRenderText.h"

namespace LORD
{
	class LORD_CORE_API GUIStaticText: public GUIWindow
	{
	public:
		GUIStaticText(GUIWindowType nType, const GUIString& strName);
		virtual ~GUIStaticText();

		virtual bool	SetProperty(const GUIString& strName, const GUIString& strValue);
		virtual bool	GetProperty(const GUIString& strName, GUIString& strValue);	
		virtual void	GetPropertyList(GUIPropertyList& properlist);
		virtual bool	GetPropertyType(const GUIString& strName, GUIPropertyType& type);

		virtual void	DrawSelf();

		void			SetTextLineExtraSpace(float lineExtraSpace);
		void			SetTextColor(const Color& cColor);
		const Color&	GetTextColor() { return m_RenderText.GetTextColor(); }

		void			SetTextVertAlign(VerticalAlignment vertAlign);
		void			SetTextHorzAlign(HorizontalAlignment horzAlign);
		void			SetWordWrap(bool bWordWrap);
		void            AllShowOneLine(bool showOneLine);

		virtual void	OnTextChanged();
		virtual void	OnSized();
		virtual void	OnFontChanged();

		void            SetSelfAdaptionArea(bool selfAdapt);

		void			SetTextScale(float fScale) { m_fScale = fScale; }

		void			SetTextBoader(const Color& cColor);

	protected:

		GUIRenderText		m_RenderText;
		bool                m_SelfAdaptHigh;
		bool                m_showOneLine;//所有文字一行显示
		float				m_fScale;
	};
}

#endif