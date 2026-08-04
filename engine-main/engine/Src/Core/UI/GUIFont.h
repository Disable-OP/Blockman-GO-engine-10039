/********************************************************************
filename: 	GUIFont.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/

#ifndef __GUI_FONT_H__
#define __GUI_FONT_H__

#include "Core.h"
#include "GUIString.h"
#include "GUIFontGlyph.h"
#include "GUICommonDef.h"
#include "GUIRenderImage.h"

namespace LORD
{
	class LORD_CORE_API GUIFont: public ObjectAlloc
	{
	public:

		enum FontType
		{
			FT_FREE_TYPE,
			FT_PIXEL_MAP,
		};

		virtual ~GUIFont(void);

		const GUIString& GetName() const { return m_strName; }

		bool IsCodepointAvailable(utf16 cp) const
		{ 
			//return (m_CodePointMap.find(cp) != m_CodePointMap.end());
			return m_CodepointArray[cp] != NULL;
		}

		void RenderText(const GUIString& strText, const Vector2& position, 
						const ERect& rtClip, const Color& color
						, bool bShadow = false, const Color& cShaderColor = Color::BLACK
						, bool bBorder = false, const Color& cBorderColor = Color::BLACK
						, const Vector2& vScale = Vector2::ONE
						, GUIWindow* pWindow = NULL, FontBufferListType* fontBuffer = NULL);

		void RenderText(const GUIString& strText, const Vector2& position, 
			const ERect& rtClip, const ColorRect& color
			, bool bShadow = false, const Color& cShaderColor = Color::BLACK
			, bool bBorder = false, const Color& cBorderColor = Color::BLACK
			, const Vector2& vScale = Vector2::ONE
			, GUIWindow* pWindow = NULL, FontBufferListType* fontBuffer = NULL);

		float GetLineSpacing() const { return m_fHeight; }

		float GetFontHeight() const
		{ return (m_fAscender - m_fDescender); }

		float GetBaseline() const
		{ return m_fAscender; }

		float GetTextExtent(const GUIString& strText, float scale = 1.0f);
		float GetTextHigh(const GUIString& strText, float scale = 1.0f);//文本含有图片时情景

		virtual const GUIFontGlyph* GetGlyphData(utf16 codepoint) = 0;

		void SplitStringToMultiLine(float fWidth, const Color& cDefaultColor, const GUIString& strInput, 
										TextStringList& outList,vector<float>::type & outHighList);

		FontType GetFontType() { return m_eType; }

		float GetLineExtraSpace() { return m_fLineExtraSpace; }
		void SetLineExtraSpace(float lineExtraSpace) { m_fLineExtraSpace = lineExtraSpace; }

	protected:

		void RenderText_Impl(const GUIString& strText, const Vector2& position, 
			const ERect& rtClip, bool bShadow = false, const Color& cShaderColor = Color::BLACK
			, bool bBorder = false, const Color& cBorderColor = Color::BLACK
			, const Vector2& vScale = Vector2::ONE
			, GUIWindow* pWindow = NULL, FontBufferListType* fontBuffer = NULL);

		GUIFont(const GUIString& strName, const GUIString& strFileName);

		void SetMaxCodepoint(utf16 codepoint) 
		{ 
			m_fMaxCodepoint = codepoint;
			m_CodepointArray.reserve(codepoint+1);
			m_CodepointArray.resize(codepoint+1);
		}

		bool ParseCode(utf16 cCode, GUIWindow* pWindow = NULL);

		void ResetParse();

		GUIString m_strName;

		GUIString m_strFileName;

		//! maximal font ascender (pixels above the baseline)
		float m_fAscender;
		//! maximal font descender (negative pixels below the baseline)
		float m_fDescender;
		//! (ascender - descender) + linegap
		float m_fHeight;
		// line space
		float m_fLineExtraSpace = 0.0f;

		utf16 m_fMaxCodepoint;

		//typedef map<utf16, GUIFontGlyph>::type CodepointMap;

		//CodepointMap m_CodePointMap;
		typedef vector<GUIFontGlyph*>::type CodepointArray;
		CodepointArray m_CodepointArray;

		// 解析字符串相关临时变量存储
		bool			m_bIsColor;
		Color			m_DrawTextColor;
		ColorRect		m_DrawColorRect;
		bool			m_bIsDrawColorRect;
		Color			m_OldColor;
		GUIString		m_strColorString;

		bool			m_bIsHyperlink;
		bool			m_bIsHyperlinkInfo;
		ERect			m_rtHyperlinkArea;
		GUIString		m_strHyperlinkInfo;
		Vector2			m_RenderPosition;
		float			m_fXPosition;

		bool            m_bIsGUIAnimate;
		bool            m_bRenderTxt;
		GUIString       m_strAnimateId;		
		float           m_fAnimateHigh;

		FontType		m_eType;

		GUIRenderImage	m_UnderLineImage;
	};
}


#endif
