/********************************************************************
filename: 	GUIFreeTypeFont.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/

#ifndef __GUI_FREE_TYPE_FONT_H__
#define __GUI_FREE_TYPE_FONT_H__

#include "Core.h"
#include "GUIFont.h"
#include "ft2build.h"
#include FT_FREETYPE_H

namespace LORD
{
	class GUIImageset;
	class GUIImage;
	class LORD_CORE_API GUIFreeTypeFont: public GUIFont
	{
	public:
		GUIFreeTypeFont(const GUIString& strName, const GUIString& strFileName, const float fSize);
		virtual ~GUIFreeTypeFont(void);

		virtual const GUIFontGlyph* GetGlyphData(utf16 codepoint);

		GUIImageset* GetCurrentImageset() { return m_pCurrentImageset; }

		FT_Face getFTFace() { return m_hFontFace; }
	protected:
		void		InitFontSize();

		GUIImage* 	CreateGlyphImage(utf16 nCodePoint);
		
		float	m_fSize;
		FT_Face m_hFontFace;
		typedef vector<GUIImageset*>::type ImagesetVector;
		mutable ImagesetVector m_GlyphImages;

		uint			m_nCurrentX;
		uint			m_nCurrentY;
		uint			m_nCurrentYBottom;

		GUIImageset*	m_pCurrentImageset;
		uint			m_nBufferSize;
	};
}


#endif
