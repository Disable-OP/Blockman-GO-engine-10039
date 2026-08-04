#include "ActorMeshTexModify.h"

#include "UI/GUIFreeTypeFont.h"
#include "UI/GUIFontManager.h"

namespace LORD
{

bool ActorMeshTexModify::s_fontInited = false;
ActorMeshTexModify::CharFontMem ActorMeshTexModify::s_characters[11];

ActorMeshTexModify::ActorMeshTexModify(const String& master, const String& slave, const Vector2i& pos, const Vector2i& size)
	: m_masterName(master)
	, m_slaveName(slave)
	, m_modifyier(pos, size, 3)
	, m_textColor(1.f, 1.f, 1.f)
	, m_ratio(0.f)
	, m_showNumber(StringUtil::BLANK)
{
	m_backGround = Color(0.7f, 0.3f, 0.3f).getARGB();
	m_processing = Color(0.3f, 0.8f, 0.3f).getARGB();
}

ActorMeshTexModify::~ActorMeshTexModify()
{
}


void ActorMeshTexModify::updateString(float ratio, const String& value)
{
	if (m_ratio == ratio && m_showNumber == value)
		return;

	m_ratio = ratio;
	m_showNumber = value;

	Vector2i size = m_modifyier.getSize();
	ui8* data = m_modifyier.getData();
	for (int iy = 0; iy < size.y; ++iy)
	{
		for (int ix = 0; ix < size.x; ++ix)
		{
			int idx = iy * size.x + ix;
			if (float(ix) / size.x < ratio)
			{
				data[idx * 3 + 0] = ui8(m_processing >> 16);
				data[idx * 3 + 1] = ui8(m_processing >> 8);
				data[idx * 3 + 2] = ui8(m_processing >> 0);
			}
			else
			{
				data[idx * 3 + 0] = ui8(m_backGround >> 16);
				data[idx * 3 + 1] = ui8(m_backGround >> 8);
				data[idx * 3 + 2] = ui8(m_backGround >> 0);
			}
		}
	}

	// setup text.
	if (!s_fontInited)
		setupCharacters();

	// render the characters.
	int beginH = 3;
	int beginW = 1;

	int valueWidth = 0;
	int valueHeight = 0;
	for (size_t i = 0; i < value.length(); ++i)
	{
		int charId = value[i] - '/';
		valueWidth += s_characters[charId].w;
		valueHeight = s_characters[charId].h;
	}

	if (valueWidth <= size.x)
	{
		beginW = (size.x - valueWidth) / 2;
	}
	beginH = (size.y - valueHeight) / 2;

	for (size_t i = 0; i < value.length(); ++i)
	{
		int charId = value[i] - '/';
		if (beginH + s_characters[charId].w >= size.x)
			break;

		for (int iy = 0; iy < s_characters[charId].h; iy++)
		{
			for (int ix = 0; ix < s_characters[charId].w; ix++)
			{
				int dstIdx = (iy + beginH) * size.x + ix + beginW;
				float fontGray = float(s_characters[charId].data[iy * s_characters[charId].w + ix]) / 255.f;
				float r = float(data[dstIdx * 3 + 0]) / 255.f;
				float g = float(data[dstIdx * 3 + 1]) / 255.f;
				float b = float(data[dstIdx * 3 + 2]) / 255.f;
				r = m_textColor.r * fontGray + r * (1.f - fontGray);
				g = m_textColor.g * fontGray + g * (1.f - fontGray);
				b = m_textColor.b * fontGray + b * (1.f - fontGray);
				data[dstIdx * 3 + 0] = ui8(r * 255.f);
				data[dstIdx * 3 + 1] = ui8(g * 255.f);
				data[dstIdx * 3 + 2] = ui8(b * 255.f);

			}
		}

		beginW += s_characters[charId].w + 1;
	}
	
	m_modifyier.updateKey();
	// ERect rect(m_modifyPos.x, m_modifyPos.y, m_modifyPos.x + m_modifySize.x, m_modifyPos.y + m_modifySize.y);
	// target->updateSubTex2D(0, rect, m_modifies);
}

void ActorMeshTexModify::setupCharacters()
{
	GUIFreeTypeFont* pFont = dynamic_cast<GUIFreeTypeFont*>(GUIFontManager::Instance()->GetFont("HT22"));
	FT_Face ftFace = pFont->getFTFace();
	GUIString s("/0123456789");
	utf16 code;
	if (pFont && ftFace)
	{
		for (size_t i = 0; i < s.size(); ++i)
		{
			code = s[i];
			if (FT_Load_Char(ftFace, code, FT_LOAD_RENDER | FT_LOAD_NO_AUTOHINT | FT_LOAD_TARGET_NORMAL | FT_LOAD_LINEAR_DESIGN))
			{
				LordLogError("EntityTextureModify::setupCharacters - Failed to load glyph for codepoint: %d", code);
			}
			else
			{
				uint glyph_w = ftFace->glyph->bitmap.width;
				uint glyph_h = ftFace->glyph->bitmap.rows;
				FT_Bitmap* glyph_bitmap = &ftFace->glyph->bitmap;
				s_characters[i].w = glyph_w;
				s_characters[i].h = glyph_h;
				
				for (uint h = 0; h < glyph_h; ++h)
				{
					ui8* pSrc = glyph_bitmap->buffer + (h * glyph_bitmap->pitch);
					ui8* pDst = s_characters[i].data + h * glyph_bitmap->width;
					memcpy(pDst, pSrc, glyph_bitmap->width);
				}
			}
		}
	}
	s_fontInited = true;

}

}