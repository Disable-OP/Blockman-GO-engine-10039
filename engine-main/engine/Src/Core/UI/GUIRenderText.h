/********************************************************************
filename: 	GUIRenderText.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/

#ifndef __GUI_RENDER_TEXT_H__
#define __GUI_RENDER_TEXT_H__

#include "Core.h"
#include "GUIString.h"
#include "GUICommonDef.h"
#include "GUIImage.h"

namespace LORD
{
	class GUIFont;
	class GUIWindow;

	class LORD_CORE_API GUIRenderText: public ObjectAlloc
	{
	public:
		GUIRenderText();
		~GUIRenderText();

		void SetTextVertAlign(VerticalAlignment vertAlign);
		void SetTextHorzAlign(HorizontalAlignment horzAlign);
		void SetWordWrap(bool bSetting);
		void SetText(const GUIString& strText);
		void SetFont(GUIFont* pFont);
		GUIFont*  GetFont() {return m_pFont;}
		void SetWidth(float fWidth);
		void SetTextColor(const Color& color);
		void SetShadowed(bool bShadow);
		void SetShadowColor(const Color& color);
		void SetBordered(bool bBorder);
		void SetBorderColor(const Color& color);
		void SetLineExtraSpace(float lineExtraSpace);

		VerticalAlignment GetTextVertAlign() { return m_TextVertAlign; }
		HorizontalAlignment GetTextHorzAlign() { return m_TextHorzAlign; }
		bool IsWordWrap() { return m_bIsWordWrap; }
		const Color& GetTextColor() { return m_TextColor; }
		bool IsShadowed() { return m_bIsShadow; }
		const Color& GetShadowColor() { return m_ShadowColor; }
		bool IsBordered() { return m_bIsBorder; }
		const Color& GetBorderColor() { return m_BorderColor; }

		void LayoutWordWrapText();

		void DrawGUIText(const ERect& rtArea, GUIWindow* pWindow, bool bIsColorRect = false, const ColorRect& colorRect = ColorRect(Color::WHITE,Color::WHITE, Color::WHITE,Color::WHITE));

		const TextStringList& GetTextStringList(){return m_TextStringList;}	
		float GetTextStringHigh();
		float GetTextExtent();
		void SetTextScale(float fScale){ m_fScale = fScale; }

		void setRenderOffset(const Vector2& offset) { m_renderOffset = offset; }

		const Vector2 getRenderOffset() { return m_renderOffset; }

		static void DrawTextBuffer(GUIFont* font, FontBufferListType& buff);
		float GetLineExtraSpace() { return m_fLineExtraSpace; }
	private:

		bool isCharHex(utf16 code)
		{
			return (code >= '0' && code <= '9') || (code >= 'A' && code <= 'F');
		}

		void ParseEscapeChar();

		void invalidateFontBuffer() { m_bIsFontBufferDirty = true; }

		void drawCacheFont(GUIWindow* pWindow);

		VerticalAlignment	m_TextVertAlign;
		HorizontalAlignment	m_TextHorzAlign;
		bool				m_bIsWordWrap;
		GUIFont*			m_pFont;
		bool				m_bIsShadow;
		Color				m_ShadowColor;
		bool				m_bIsBorder;
		Color				m_BorderColor;

		// 单行
		GUIString			m_strText;		
		float               m_fTextWidth;
		// 处理多行换行的情况
		TextStringList		m_TextStringList;
		vector<float>::type m_TextHighList;//存放每行文本的高度
		// 用于计算换行的宽度
		float				m_fWidth;
		Color				m_TextColor;
		float				m_fScale;

		// 缓存
		//
		bool				m_bIsFontBufferDirty;
		FontBufferListType	m_fontBuffer;
		ERect				m_areaCache;
		bool				m_isColorRectCache;
		ColorRect			m_colorRectCache;

		// 增加偏移属性，因为可能button的阴影问题
		Vector2				m_renderOffset;

		float				m_fLineExtraSpace = 0.0f;

	};
}

#endif
