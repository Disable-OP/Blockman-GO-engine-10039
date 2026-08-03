#include "GUIPixmapFont.h"
#include "GUIImagesetManager.h"
#include "GUIImageset.h"
#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include "rapidxml_print.hpp"
#include "rapidjson/document.h"


using namespace rapidxml;


namespace LORD
{
	GUIPixmapFont::GUIPixmapFont( const GUIString& strFontName, const GUIString& strFileName )
		: GUIFont(strFontName, strFileName)
	{
		m_eType = FT_PIXEL_MAP;
		InitPixelFont();
	}

	GUIPixmapFont::~GUIPixmapFont()
	{
		for (size_t i = 0; i < m_pGlyphImages.size(); ++i)
		{
			if (m_pGlyphImages[i])
			{
				GUIImagesetManager::Instance()->pushbackReleaseImageset(m_pGlyphImages[i]);
				m_pGlyphImages[i] = NULL;
			}
		}

		for (size_t i = 0; i < m_CodepointArray.size(); ++i)
		{
			GUIFontGlyph* point = m_CodepointArray[i];
			LordSafeDelete(point);
		}
		m_CodepointArray.clear();
		m_CodepointArray.resize(0);
		m_CodepointArray.reserve(0);
	}

	void GUIPixmapFont::DefineMapping( const utf16 cCodePoint, const GUIString& strImageName, const float fHorzAdvance )
	{
		GUIImage* pImage = NULL;

		for (size_t i = 0; i < m_pGlyphImages.size(); ++i)
		{
			pImage = m_pGlyphImages[i]->GetImage(strImageName);
			if (pImage)
			{
				break;
			}
		}

		LordAssert(pImage);

		if (cCodePoint > m_fMaxCodepoint)
		{
			m_fMaxCodepoint = cCodePoint;
			m_CodepointArray.reserve(cCodePoint+1);
			m_CodepointArray.resize(cCodePoint+1);
		}

		float adv = (fHorzAdvance == -1.0f) ?
			(float)(int)(pImage->GetWidth() + pImage->GetOffset().x) :
		fHorzAdvance;

		// create a new FontGlyph with given character code
		GUIFontGlyph* glyph = LordNew GUIFontGlyph(adv, pImage);

		if (pImage->GetOffset().y < -m_fAscender)
			m_fAscender = -pImage->GetOffset().y;
		if (pImage->GetHeight() + pImage->GetOffset().y > -m_fDescender)
			m_fDescender = -(pImage->GetHeight() + pImage->GetOffset().y);

		m_fHeight = m_fAscender - m_fDescender;

		m_CodepointArray[cCodePoint] = glyph;
	}

	const GUIFontGlyph* GUIPixmapFont::GetGlyphData( utf16 codepoint )
	{
		if (codepoint > m_fMaxCodepoint)
			return 0;
		
		return m_CodepointArray[codepoint];
	}

	void GUIPixmapFont::InitPixelFont()
	{
		m_fAscender = 0;
		m_fDescender = 0;
		m_fHeight = 0;
		m_fMaxCodepoint = 0;

		DataStream* pDataStream = ResourceGroupManager::Instance()->openResource(m_strFileName.c_str());		
		size_t nDataSize = pDataStream->size();
		char* pJSONData = (char*)LordMalloc(nDataSize + 1);
		pDataStream->read(pJSONData, nDataSize + 1);
		pJSONData[nDataSize] = 0;

		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(pJSONData);

		const rapidjson::Value::MemberIterator& itImagesetName = doc->MemberBegin();
		GUIString strImagesetname = itImagesetName->value.GetString();

		StringArray imagesets = StringUtil::Split(strImagesetname.c_str(), "|");
		for(size_t i = 0; i < imagesets.size(); ++i)
		{
			GUIImageset* pGlyphImages = GUIImagesetManager::Instance()->CreateFromFile(imagesets[i].c_str());
			LordAssert(pGlyphImages);

			m_pGlyphImages.push_back(pGlyphImages);
		}

		//
		rapidjson::Value::MemberIterator itMapping = itImagesetName->value.MemberBegin();
		
		for(; itMapping != itImagesetName->value.MemberEnd(); ++itMapping)
		{
			GUIString strCodePoint = itMapping->value["CodePoint"].GetString();
			GUIString strImageName = itMapping->value["ImageName"].GetString();
			float fHorzAdvance = -1.0f;
			if (itMapping->value.HasMember("HorzAdvance"))
			{
				fHorzAdvance = itMapping->value["HorzAdvance"].GetFloat();
			}
			utf16 cCodePoint = StringUtil::ParseUI16(strCodePoint.c_str());
			DefineMapping(cCodePoint, strImageName, fHorzAdvance);
		}


		delete doc;
		LordFree(pJSONData);
		LordDelete(pDataStream);

#if 0		
		DataStream* pDataStream = ResourceGroupManager::Instance()->openResource(m_strFileName.c_str());
		size_t nDataSize = pDataStream->size();
		char* pXMLData = (char*)LordMalloc(nDataSize+1);
		pDataStream->read(pXMLData, nDataSize);
		pXMLData[nDataSize] = 0;
		xml_document<>* doc = LordNewT(xml_document<>);
		doc->parse<0>(pXMLData);
		
		xml_node<>* pFontNode = doc->first_node();
		LordAssert(pFontNode);
		xml_attribute<>* pAttr = pFontNode->first_attribute();
		LordAssert(pAttr);
		GUIString strImagesetName = pAttr->value();

		StringArray imagesets = StringUtil::Split(strImagesetName.c_str(), "|");
		for (size_t i = 0; i < imagesets.size(); ++i)
		{
			// imageset
			GUIImageset* pGlyphImages = GUIImagesetManager::Instance()->CreateFromFile(imagesets[i].c_str());
			LordAssert(pGlyphImages);

			m_pGlyphImages.push_back(pGlyphImages);
		}

		xml_node<>* pMapping = pFontNode->first_node();
		while(pMapping)
		{
			xml_attribute<>* pMapAttr = pMapping->first_attribute();
			GUIString strCodePoint = pMapAttr->value();
			pMapAttr = pMapAttr->next_attribute();
			GUIString strImageName = pMapAttr->value();
			pMapAttr = pMapAttr->next_attribute();
			float fHorzAdvance = -1.0f;
			if (pMapAttr)
			{
				GUIString strHorzAdvance = pMapAttr->value();
				fHorzAdvance = StringUtil::ParseFloat(strHorzAdvance.c_str());
			}

			utf16 cCodePoint = StringUtil::ParseUI16(strCodePoint.c_str());
			DefineMapping(cCodePoint, strImageName, fHorzAdvance);

			pMapping = pMapping->next_sibling();
		}

		LordDeleteT(doc, xml_document<>);
		LordFree(pXMLData);
		LordSafeDelete(pDataStream);
#endif

	}

}
