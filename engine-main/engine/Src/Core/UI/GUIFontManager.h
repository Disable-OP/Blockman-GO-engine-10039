/********************************************************************
filename: 	GUIFontManager.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/

#ifndef __GUI_FONT_MANAGER_H__
#define __GUI_FONT_MANAGER_H__

#include "Core.h"
#include "Object/Singleton.h"
#include "GUIString.h"

namespace LORD
{
	class GUIFont;
	class LORD_CORE_API GUIFontManager : public ObjectAlloc, public Singleton<GUIFontManager>
	{
	public:
		typedef map<GUIString, GUIFont*, GUIString::FastLessCompare>::type FontRegistry;

		GUIFontManager();
		~GUIFontManager();

		static GUIFontManager* Instance();

		GUIFont* CreateFreeTypeFont(const GUIString& strName, 
			const float fSize, const GUIString& strFilename);

		GUIFont* CreatePixmapFont(const GUIString& strName, const GUIString& strFilename);

		void	DestroyAllFont();

		GUIFont*	GetDefaultFont();

		GUIFont*	GetFont(const GUIString& strName);

	private:

		FontRegistry	m_Fonts;
	};
}


#endif