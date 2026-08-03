#include "WallTextRenderer.h"
#include "UI/GUIString.h"
#include "Render/ActiveRenderInfo.h"
#include "UI/GUIFontManager.h"
#include "UI/GUIImageset.h"
#include "UI/GUIFontGlyph.h"
#include "UI/GUIFont.h"
#include "cTileEntity/TileEntityRenderMgr.h"
#include "Tessolator/FontRenderable.h"
#include "Blockman/cWorld/Blockman.h"
#include "Blockman/cEntity/EntityPlayerSPMP.h"

using namespace LORD;

namespace BLOCKMAN
{
	void WallTextRenderer::renderWallText(const LORD::String& txt, LORD::Vector3 pos, bool canBeOccluded, float scale, float yaw, float pitch, float r, float g, float b, float a)
	{
		struct HeadTextInfo
		{
			GUIString text;
			Color	  color;
		};
		struct CharacterInfo
		{
			Vector2  pos;
			Vector2  size;
			uint     color;
			ERect	 uv;
			Texture* tex;
		};

		vector<HeadTextInfo>::type txt_lines;
		Color defaultColor = Color(r, g, b, a);
		StringArray txts = StringUtil::Split(txt, "\n");
		for (auto& line : txts)
		{
			if (line.find("[#") == 0)
			{
				String colorstr = line.substr(2, 6);
				String txtstr = line.substr(9, line.length() - 9);
				unsigned int nColor = 0;
				sscanf(colorstr.c_str(), "%X", &nColor);
				Color color = nColor;
				color.a = 1.f;
				txt_lines.push_back({ GUIString(txtstr.c_str()), color });
			}
			else
			{
				txt_lines.push_back({ GUIString(line.c_str()), defaultColor });
			}
		}

		float rx = ActiveRenderInfo::rotationX;
		float rz = ActiveRenderInfo::rotationZ;
		float ryz = ActiveRenderInfo::rotationYZ;
		float rxy = ActiveRenderInfo::rotationXY;
		float rxz = ActiveRenderInfo::rotationXZ;

		float line_height = 0.3f * scale;
		float line_space = 0.12f * scale;
		float word_space = 0.02f * scale;
		int line_count = txt_lines.size();
		int line_num = 0;
		GUIFont* pFont = GUIFontManager::Instance()->GetFont("HT20");
		float heightScale = line_height / pFont->GetFontHeight();
		vector<CharacterInfo>::type buffer;

		for (auto& txt_line : txt_lines)
		{
			utf16 cCode;
			float totalWidth = 0.f;
			vector<CharacterInfo>::type buffer_line;
			auto color = txt_line.color;
			for (size_t c = 0; c < txt_line.text.length(); ++c)
			{
				cCode = txt_line.text[c];
				switch (cCode)
				{
				case 0x25A2:
				{
					auto colorstr = txt_line.text.substr(c + 1, 8);
					c += 8;
					unsigned int nColor = 0;
					sscanf(colorstr.c_str(), "%X", &nColor);
					color = nColor;
					continue;
				}
				case 0x25A5:
					color = txt_line.color;
					continue;
				default:
					break;
				}

				const GUIFontGlyph* glyph = pFont->GetGlyphData(cCode);
				if (!glyph)
					continue;

				float charWidth = glyph->GetWidth() * heightScale;
				float charHeight = glyph->GetHeight() * heightScale;

				CharacterInfo info;
				const GUIImage* const img = glyph->GetImage();
				info.tex = img->GetImageset()->GetTexture();
				info.uv = glyph->GetImage()->GetUV();
				Vector2 offset = img->GetOffset() * heightScale;
				info.pos.x = totalWidth + offset.x + word_space;
				info.pos.y = line_height * (line_count - line_num - 1) - offset.y - charHeight;
				info.size.x = charWidth;
				info.size.y = charHeight;
				info.color = color.getABGR();
				buffer_line.push_back(info);

				totalWidth += charWidth + word_space;
			}
			// adjust all the xposition of all the characters.
			for (auto& it : buffer_line)
				it.pos.x -= totalWidth / 2.f;
			buffer.insert(buffer.end(), buffer_line.begin(), buffer_line.end());
			line_num++;
		}

		pitch = -pitch * Math::DEG2RAD;
		yaw = -yaw * Math::DEG2RAD + Math::PI;

		Matrix4 matText, matTemp;
		matText.identity(); matText.translate(pos);
		matTemp.identity(); matTemp.rotateY(yaw);
		matText.joint(matTemp);
		matTemp.identity(); matTemp.rotateX(pitch);
		matText.joint(matTemp);

		FontRenderable* renderable = NULL;
		FontVertexFmt* pVertex = NULL;
		TileEntityRenderMgr* mgr = TileEntityRenderMgr::Instance();
		for (auto& it : buffer)
		{
			renderable = mgr->getFontRenderable(it.tex, canBeOccluded);
			if (!renderable->getLockPtr(1, pVertex))
				continue;

			float z = 0.072f;
			Vector3 v1(it.pos.x, it.pos.y, z);
			Vector3 v2(it.pos.x, it.pos.y + it.size.y, z);
			Vector3 v3(it.pos.x + it.size.x, it.pos.y + it.size.y, z);
			Vector3 v4(it.pos.x + it.size.x, it.pos.y, z);
			pVertex[0].set(v1 * matText, it.color, it.uv.left, it.uv.bottom);
			pVertex[3].set(v2 * matText, it.color, it.uv.left, it.uv.top);
			pVertex[2].set(v3 * matText, it.color, it.uv.right, it.uv.top);
			pVertex[1].set(v4 * matText, it.color, it.uv.right, it.uv.bottom);
		}
	}

}
