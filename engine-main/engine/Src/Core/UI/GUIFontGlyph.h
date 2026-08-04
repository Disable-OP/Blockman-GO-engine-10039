/********************************************************************
filename: 	GUIFontGlyph.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/

#ifndef __GUI_FONT_GLYPH__
#define __GUI_FONT_GLYPH__

#include "GUIImage.h"

namespace LORD
{

class GUIImageset;
class LORD_CORE_API GUIFontGlyph : public ObjectAlloc
{
public:
    GUIFontGlyph(float fAdvance = 0.0f, const GUIImage* pImage = 0) :
        m_pImage(pImage),
        m_fAdvance(fAdvance)
    {}

    const GUIImage* GetImage() const { return m_pImage; }

    const GUIImageset* GetImageset() const { return m_pImage->GetImageset(); }

    Vector2 GetSize() const { return Vector2(GetWidth(), GetHeight()); }

    float GetWidth() const
    { return m_pImage->GetWidth(); }

    float GetHeight() const
    { return m_pImage->GetHeight(); }

    float GetAdvance() const
    { return m_fAdvance; }

	float GetRenderedAdvance() const
	{ return (GetWidth() + m_pImage->GetOffset().x); }

    void SetAdvance(float fAdvance)
    { m_fAdvance = fAdvance; }

    void SetImage(const GUIImage* pImage)
    { m_pImage = pImage; }

private:
    const GUIImage*		m_pImage;
    float				m_fAdvance;
};

}

#endif
