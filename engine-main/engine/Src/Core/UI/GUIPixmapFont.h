/********************************************************************
filename: 	GUIPixmapFont.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/

#ifndef __GUI_PIXEL_FONT_H__
#define __GUI_PIXEL_FONT_H__

#include "GUIFont.h"

namespace LORD
{

	class GUIPixmapFont : public GUIFont
	{
	public:
		GUIPixmapFont(const GUIString& strFontName, const GUIString& strFileName);

		virtual ~GUIPixmapFont();

		void DefineMapping(const utf16 cCodePoint, const GUIString& strImageName, 
						   const float fHorzAdvance);

		virtual const GUIFontGlyph* GetGlyphData(utf16 codepoint);

	protected:
		void	InitPixelFont();

	protected:

		vector<GUIImageset*>::type 	m_pGlyphImages;
	};

}


#endif