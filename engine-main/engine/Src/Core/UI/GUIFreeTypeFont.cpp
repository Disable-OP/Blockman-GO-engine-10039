#include "GUIFreeTypeFont.h"
#include "GUIImagesetManager.h"
#include "GUIImageset.h"
#include "GUIRenderManager.h"
#include "Render/Material.h"

namespace LORD
{

// Pixels to put between glyphs
#define INTER_GLYPH_PAD_SPACE 2
// A multiplication coefficient to convert FT_Pos values into normal floats
#define FT_POS_COEF  (1.0/64.0)

static int ft_usage_count = 0;
static FT_Library ft_lib;
const int FONT_TEXTURE_SIZE = 512;


GUIFreeTypeFont::GUIFreeTypeFont( const GUIString& strName, const GUIString& strFileName, const float fSize)
	: GUIFont(strName, strFileName)
	, m_fSize(fSize)
	, m_hFontFace(0)
	, m_nCurrentX(INTER_GLYPH_PAD_SPACE)
	, m_nCurrentY(INTER_GLYPH_PAD_SPACE)
	, m_nCurrentYBottom(INTER_GLYPH_PAD_SPACE)
	, m_pCurrentImageset(0)
	, m_nBufferSize(FONT_TEXTURE_SIZE*FONT_TEXTURE_SIZE*sizeof(Dword))
{
	m_eType = FT_FREE_TYPE;

	if (!ft_usage_count++)
		FT_Init_FreeType(&ft_lib);

	InitFontSize();
}

GUIFreeTypeFont::~GUIFreeTypeFont( void )
{
	if (!--ft_usage_count)
		FT_Done_FreeType(ft_lib);

	for (int i = 0; i < m_fMaxCodepoint; ++i)
	{
		GUIFontGlyph* point = m_CodepointArray[i];
		LordSafeDelete(point);
	}
	m_CodepointArray.clear();
	m_CodepointArray.resize(0);
	m_CodepointArray.reserve(0);
}

void GUIFreeTypeFont::InitFontSize()
{
	FT_Error error;

	String strFullPath = ResourceGroupManager::Instance()->getFileLocation(m_strFileName.c_str());

	if ((error = FT_New_Face(ft_lib, strFullPath.c_str(), 0,&m_hFontFace)) != 0)
	{
		LordLogFatal("GUIFreeTypeFont::InitCodeMap FT_New_Face error");
		LordAssert(0);
		return;
	}

	// check that default Unicode character map is available
	if (!m_hFontFace->charmap)
	{
		FT_Done_Face(m_hFontFace);
		m_hFontFace = 0;
		LordLogFatal("GUIFreeTypeFont::InitCodeMap font does not have unicode character map!");
		LordAssert(0);
		return;
	}

	uint horzdpi = 96;
	uint vertdpi = 96;

	float hps = m_fSize * 64;
	float vps = m_fSize * 64;

	if (FT_Set_Char_Size(m_hFontFace, FT_F26Dot6(hps), FT_F26Dot6(vps), horzdpi, vertdpi))
	{
		// For bitmap fonts we can render only at specific point sizes.
		// Try to find nearest point size and use it, if that is possible
		float ptSize_72 = (m_fSize * 72.0f) / vertdpi;
		float best_delta = 99999;
		float best_size = 0;
		for (int i = 0; i < m_hFontFace->num_fixed_sizes; i++)
		{
			float size = m_hFontFace->available_sizes [i].size * float(FT_POS_COEF);
			float delta = fabs(size - ptSize_72);
			if (delta < best_delta)
			{
				best_delta = delta;
				best_size = size;
			}
		}

		if ((best_size <= 0) ||
			FT_Set_Char_Size(m_hFontFace, 0, FT_F26Dot6(best_size * 64), 0, 0))
		{
			FT_Done_Face(m_hFontFace);
			m_hFontFace = 0;
			LordLogFatal("GUIFreeTypeFont::InitCodeMap font cannot be rasterised at a size give!");
			LordAssert(0);
		}
	}

	if (m_hFontFace->face_flags & FT_FACE_FLAG_SCALABLE)
	{
		//float x_scale = m_hFontFace->size->metrics.x_scale * FT_POS_COEF * (1.0/65536.0);
		float y_scale = m_hFontFace->size->metrics.y_scale * float(FT_POS_COEF) * (1.0f / 65536.0f);
		m_fAscender = m_hFontFace->ascender * y_scale;
		m_fDescender = m_hFontFace->descender * y_scale;
		m_fHeight = m_hFontFace->height * y_scale;
	}
	else
	{
		m_fAscender = m_hFontFace->size->metrics.ascender * float(FT_POS_COEF);
		m_fDescender = m_hFontFace->size->metrics.descender * float(FT_POS_COEF);
		m_fHeight = m_hFontFace->size->metrics.height * float(FT_POS_COEF);
	}

	SetMaxCodepoint(65535);
}

const GUIFontGlyph* GUIFreeTypeFont::GetGlyphData( utf16 codepoint )
{
	if (codepoint > m_fMaxCodepoint)
	{
		return NULL;
	}

	GUIFontGlyph* point = m_CodepointArray[codepoint];
	if (!point)
	{
		if (FT_Load_Char(m_hFontFace, codepoint,
			FT_LOAD_DEFAULT | FT_LOAD_NO_AUTOHINT | FT_LOAD_TARGET_NORMAL | FT_LOAD_LINEAR_DESIGN))
		{
			point = NULL;
		}
		else
		{
			GUIImage* pImage = CreateGlyphImage(codepoint);
			if (pImage)
			{
				float adv = m_hFontFace->glyph->metrics.horiAdvance * float(FT_POS_COEF);

				point = m_CodepointArray[codepoint] = LordNew GUIFontGlyph(adv, pImage);
			}
			else
			{
				point = NULL;
			}
		}		
	}
	return point;
}

GUIImage* GUIFreeTypeFont::CreateGlyphImage( utf16 nCodePoint )
{
	if (!m_pCurrentImageset)
	{
		String strTextureName = StringUtil::Format("FontTexture_%s_d%", m_strName.c_str(), nCodePoint);
		m_pCurrentImageset = GUIImagesetManager::Instance()->CreateManual(strTextureName.c_str(), FONT_TEXTURE_SIZE);
		if (m_pCurrentImageset)
		{
			m_GlyphImages.push_back(m_pCurrentImageset);
		}	
		else
		{
			return NULL;
		}
	}

	GUIImage* pImage = NULL;

	// Render the glyph
	if (FT_Load_Char(m_hFontFace, nCodePoint, FT_LOAD_RENDER | FT_LOAD_NO_AUTOHINT | FT_LOAD_TARGET_NORMAL | FT_LOAD_LINEAR_DESIGN))
	{
		LordLogError("Font::loadFreetypeGlyph - Failed to load glyph for codepoint: %d", nCodePoint);

		// Create a 'null' image for this glyph so we do not seg later
		ERect area(0, 0, 0, 0);
		Vector2 offset(0, 0);
		GUIString strName;
		strName += nCodePoint;
		pImage = m_pCurrentImageset->DefineImage(strName, area);
	}
	else
	{
		uint glyph_w = m_hFontFace->glyph->bitmap.width + INTER_GLYPH_PAD_SPACE;
		uint glyph_h = m_hFontFace->glyph->bitmap.rows + INTER_GLYPH_PAD_SPACE;

		// Check if glyph right margin does not exceed texture size
		uint x_next = m_nCurrentX + glyph_w;
		uint y_bot = m_nCurrentY + glyph_h;
		if (y_bot > m_nCurrentYBottom)
		{
			m_nCurrentYBottom = y_bot;
		}
		
		if (x_next > FONT_TEXTURE_SIZE)
		{
			m_nCurrentX = INTER_GLYPH_PAD_SPACE;

			if (m_nCurrentYBottom + glyph_h > FONT_TEXTURE_SIZE )
			{
				m_nCurrentY = INTER_GLYPH_PAD_SPACE;
				m_nCurrentYBottom = INTER_GLYPH_PAD_SPACE;
				// 不够了，创建一张新的
				String strTextureName = StringUtil::Format("FontTexture_%s_d%", m_strName.c_str(), nCodePoint);
				m_pCurrentImageset = GUIImagesetManager::Instance()->CreateManual(strTextureName.c_str(), FONT_TEXTURE_SIZE);
				m_GlyphImages.push_back(m_pCurrentImageset);

				LordLogWarning("Create new font texture, font: %s, texture count: %d", m_strName.c_str(), m_GlyphImages.size());
			}
			else
			{
				m_nCurrentY = m_nCurrentYBottom;
			}
		}

		// 把这个字画到纹理上
		FT_Bitmap *glyph_bitmap = &m_hFontFace->glyph->bitmap;

		if (glyph_bitmap->rows > 0 && glyph_bitmap->width > 0)
		{
			unsigned char* pDestBuffer = (unsigned char*)LordMalloc(glyph_bitmap->rows*glyph_bitmap->width);
			memset(pDestBuffer, 0, glyph_bitmap->rows*glyph_bitmap->width);
			unsigned char* pStartBuffer = pDestBuffer;

			for (int i = 0; i < glyph_bitmap->rows; ++i)
			{
				unsigned char *pSrc = glyph_bitmap->buffer + (i * glyph_bitmap->pitch);
				switch (glyph_bitmap->pixel_mode)
				{
				case FT_PIXEL_MODE_GRAY:
					{
						unsigned char* pDst = (unsigned char*)(pStartBuffer);
						memcpy(pDst, pSrc, glyph_bitmap->width);
						//for (int j = 0; j < glyph_bitmap->width; ++j)
						//{
						//	// Alpha
						//	*pDst++ = *pSrc++;
						//}
					}
					break;

				case FT_PIXEL_MODE_MONO:
					for (int j = 0; j < glyph_bitmap->width; ++j)
						pStartBuffer[j] = (pSrc [j / 8] & (0x80 >> (j & 7))) ? 0xFFFFFFFF : 0x00000000;
					break;


				default:
					LordException(("Font::drawGlyphToBuffer: "
						"The glyph could not be drawn because the pixel mode is "
						"unsupported."));
					break;
				}

				pStartBuffer += glyph_bitmap->width;
			}

			ERect rtArea((float)m_nCurrentX, (float)m_nCurrentY, 
				(float)(m_nCurrentX + glyph_bitmap->width), (float)(m_nCurrentY + glyph_bitmap->rows));

			if(m_pCurrentImageset && m_pCurrentImageset->GetTexture())
				m_pCurrentImageset->GetTexture()->updateSubTex2D(0, rtArea, pDestBuffer);

			//GUIRenderManager::Instance()->GetGUISamplerState()->setTexture(m_pCurrentImageset->GetTexture());
			//Renderer::Instance()->setSamplerState(0, GUIRenderManager::Instance()->GetGUISamplerState());

			LordFree(pDestBuffer);
			pDestBuffer = NULL;
		}

		// Create a new image in the imageset
		ERect area(static_cast<float>(m_nCurrentX),
			static_cast<float>(m_nCurrentY),
			static_cast<float>(m_nCurrentX + glyph_w - INTER_GLYPH_PAD_SPACE),
			static_cast<float>(m_nCurrentY + glyph_h - INTER_GLYPH_PAD_SPACE));

		Vector2 offset(m_hFontFace->glyph->metrics.horiBearingX * static_cast<float>(FT_POS_COEF),
			-m_hFontFace->glyph->metrics.horiBearingY * static_cast<float>(FT_POS_COEF));

		GUIString strName;
		strName += nCodePoint;

		pImage = m_pCurrentImageset->DefineImage(strName, area, offset);

		m_nCurrentX += glyph_w;
	}

	return pImage;
}

}
