/********************************************************************
filename: 	GUIEditBox.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/

#ifndef __GUI_EDIT_BOX_H__
#define __GUI_EDIT_BOX_H__

#include "Core.h"
#include "GUIWindow.h"
#include "GUIRenderImage.h"
#include "GUIRenderText.h"

namespace LORD
{
	class GUIImageset;
	class GUIEditBoxImpl;

	class LORD_CORE_API GUIEditBox: public GUIWindow
	{
	public:

		GUIEditBox(GUIWindowType nType, const GUIString& strName);
		virtual ~GUIEditBox();

		virtual bool	SetProperty(const GUIString& strName, const GUIString& strValue);
		virtual bool	GetProperty(const GUIString& strName, GUIString& strValue);	
		virtual void	GetPropertyList(GUIPropertyList& properlist);
		virtual bool	GetPropertyType(const GUIString& strName, GUIPropertyType& type);

		virtual void	DrawSelf();
		virtual void	UpdateSelf(ui32 nTimeElapse);
		virtual void	OnTextChanged();
		virtual void	OnSized();
		virtual void	OnFontChanged();

		// 激活输入框
		virtual bool	TouchUp(const Vector2& position);

		void			OnInputText(const GUIString& strText);

		//bool			IsActive();

		void			SetBackImage(const GUIString& strBackImage);

		void			SetCaratImage(const GUIString& strCaratImage);

		void SetTextVertAlign( VerticalAlignment vertAlign );

		void SetTextHorzAlign( HorizontalAlignment horzAlign );

		void SetMaxLength(LORD::i32 maxLength) { m_nMaxTextLength = maxLength; }

		void SetBlinkshow(bool isBlinkshow) { m_bBlinkshow = isBlinkshow; }

		void SetCaratOffset(float offset) { m_fCaratOffset = offset; }

		LORD::i32 GetMaxLength() { return m_nMaxTextLength; }
		GUIEditBoxImpl*	getEditBoxImpl() { return m_pImpl; }
        
        void            OpenKeyboard();
        
		void            CloseKeyboard();
        
	protected:

		float			m_fCaratOffset;//闪烁光栅的位置偏移量
		GUIRenderImage	m_BackImage;
		GUIRenderImage	m_CaratImage; //闪烁光栅Image
		bool			m_bBlinkshow;//是否闪烁
		bool			m_bIsPassword;
		GUIRenderText	m_RenderText;
		float			m_fTextOffset;
		int				m_nMaxTextLength;
		// 如果超出了，要处理裁剪的偏移
		float			m_fClipOffset;
		GUIEditBoxImpl*	m_pImpl;
	};
}

#endif
