#include "GUIFontManager.h"
#include "GUIFont.h"
#include "GUIFreeTypeFont.h"
#include "GUIPixmapFont.h"

#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include "rapidxml_print.hpp"

using namespace rapidxml;

namespace LORD
{
	//---------------------------------------------------------------------
	GUIFontManager* GUIFontManager::Instance()
	{
		return ms_pSingleton;
	}

	GUIFontManager::GUIFontManager()
	{

	}

	GUIFontManager::~GUIFontManager()
	{
		DestroyAllFont();
	}

	GUIFont* GUIFontManager::CreateFreeTypeFont( const GUIString& strName, const float fSize, const GUIString& strFilename )
	{
		GUIFont* pFont = LordNew GUIFreeTypeFont(strName, strFilename, fSize);
		LordAssert(pFont);
		m_Fonts[strName] = pFont;

		return pFont;
	}

	GUIFont* GUIFontManager::CreatePixmapFont( const GUIString& strName, const GUIString& strFilename )
	{
		GUIFont* pFont = LordNew GUIPixmapFont(strName, strFilename);
		LordAssert(pFont);
		m_Fonts[strName] = pFont;

		return pFont;
	}

	void GUIFontManager::DestroyAllFont()
	{
		FontRegistry::iterator itor = m_Fonts.begin();
		while(itor != m_Fonts.end())
		{
			LordSafeDelete(itor->second);
			++itor;
		}

		m_Fonts.clear();
	}

	GUIFont* GUIFontManager::GetDefaultFont()
	{
		LordAssert(m_Fonts.size());

		FontRegistry::iterator ht16 = m_Fonts.find("HT16");
		if (ht16 != m_Fonts.end() && ht16->second->GetFontType() == GUIFont::FT_FREE_TYPE)
		{
			return ht16->second;
		}

		FontRegistry::iterator itor = m_Fonts.begin();
		while(itor != m_Fonts.end())
		{
			if (itor->second->GetFontType() == GUIFont::FT_FREE_TYPE)
			{
				return itor->second;
			}

			++itor;
		}

		LordLogFatal("no default font has find");
		LordAssert(0);
		return NULL;
	}

	GUIFont* GUIFontManager::GetFont( const GUIString& strName )
	{
		FontRegistry::iterator itor =  m_Fonts.find(strName);
		if (itor != m_Fonts.end())
		{
			return itor->second;
		}
		else
		{
			return GetDefaultFont();
		}
	}

}