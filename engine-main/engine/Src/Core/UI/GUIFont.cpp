#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-register"
#include "GUIFont.h"
#pragma clang diagnostic pop

#include "GUIHyperlinkSystem.h"
#include "GUIAnimatMgr.h"
#include "GUIAnimate.h"
#include "GUISystem.h"

namespace LORD
{
	static utf16 cHyperlinkStart = '{';
	static utf16 cHyperlinkEnd = '}';

	GUIFont::GUIFont( const GUIString& strName, const GUIString& strFileName )
		: m_strName(strName)
		, m_strFileName(strFileName)
		, m_bIsColor(false)
		, m_DrawTextColor(Color::WHITE)
		, m_bIsDrawColorRect(false)
		, m_OldColor(Color::WHITE)
		, m_bIsHyperlink(false)
		, m_bIsHyperlinkInfo(false)
		, m_bIsGUIAnimate(false)
		, m_bRenderTxt(false)
		, m_fAnimateHigh(0.0f)
	{

	}

	GUIFont::~GUIFont( void )
	{

	}

	float GUIFont::GetTextHigh(const GUIString & strText, float scale)
	{		
		utf16 cCode;
		for (size_t c = 0; c < strText.length(); ++c)
		{
			cCode = strText[c];

			ParseCode(cCode);
		}		
		float result = scale * Math::Max(m_fAnimateHigh, m_fHeight + m_fLineExtraSpace);
		ResetParse();
		return result;
	}

	float GUIFont::GetTextExtent(const GUIString& strText, float scale)
	{
		const GUIFontGlyph* pGlyph;
		float cur_extent = 0, adv_extent = 0, width;
		m_fXPosition=0;//辅助计算长度		
		utf16 cCode;
		for (size_t c = 0; c < strText.length(); ++c)
		{
			cCode = strText[c];

			if (ParseCode(cCode))
			{
				continue;
			}

			pGlyph = GetGlyphData(cCode);

			if (pGlyph)
			{
				width = pGlyph->GetRenderedAdvance() * scale;

				if (adv_extent + width > cur_extent)
					cur_extent = adv_extent + width;

				adv_extent += pGlyph->GetAdvance() * scale;
			}
		}

		// reset
		ResetParse();

		return m_fXPosition + Math::Max(adv_extent, cur_extent);
	}

	void GUIFont::RenderText_Impl( const GUIString& strText, const Vector2& position, const ERect& rtClip,
									bool bShadow, const Color& cShaderColor
									, bool bBorder, const Color& cBorderColor
									, const Vector2& vScale, GUIWindow* pWindow, FontBufferListType* fontBuffer)
	{
		m_UnderLineImage.SetWholeImageName(GUISystem::Instance()->GetUnderLineImageName());

		m_bRenderTxt=true;
		const float base_y = position.y + GetBaseline();
		Vector2 glyph_pos(position);
		m_RenderPosition = position;
		m_fXPosition = position.x;
		utf16 cCode;
		for (size_t c = 0; c < strText.length(); ++c)
		{
			cCode = strText[c];

			if (ParseCode(cCode, pWindow))//处理非文字信息
			{
				continue;
			}

			const GUIFontGlyph* glyph;
			if ((glyph = GetGlyphData(cCode))) // NB: assignment
			{
				const GUIImage* const img = glyph->GetImage();
				glyph_pos.y = base_y;
				glyph_pos.x = m_fXPosition;//需要跳过图片的位置
				if(bShadow)
				{
					img->DrawFontImage(cCode, ERect(glyph_pos.x + 1, glyph_pos.y + 1,
						glyph_pos.x + glyph->GetWidth() + 1, glyph_pos.y + glyph->GetHeight() + 1), &rtClip, ColorRect(cShaderColor, cShaderColor, cShaderColor, cShaderColor), fontBuffer, vScale);
				}

				if (bBorder)
				{
					for(register int ii=-1; ii<=1; ii++)
					{
						for(register int jj=-1; jj<=1; jj++)
						{
							if(ii==0 && jj==0) continue;

							Vector2 vBorderPos = glyph_pos;
							vBorderPos.x += jj;
							vBorderPos.y += ii;
							img->DrawFontImage(cCode, ERect(vBorderPos.x, vBorderPos.y,
								vBorderPos.x + glyph->GetWidth() + 1, vBorderPos.y + glyph->GetHeight() + 1), &rtClip,
								ColorRect(cBorderColor, cBorderColor, cBorderColor, cBorderColor), fontBuffer, vScale);
						}
					}
				}

				Color drawColor = m_DrawTextColor;
				if (GUIHyperlinkManager::Instance()->GetClickArea().isPointInRect(glyph_pos) 
					&& GUIHyperlinkManager::Instance()->IsClickedLink())
				{
					drawColor = GUISystem::Instance()->GetHyperLinkClickColor();
				}

				ColorRect colorRectTmp;
				if (m_bIsDrawColorRect)
				{
					colorRectTmp = m_DrawColorRect;
				} 
				else
				{
					colorRectTmp = ColorRect(drawColor, drawColor, drawColor, drawColor);
				}
				
				img->DrawFontImage(cCode, ERect(glyph_pos.x, glyph_pos.y,
					glyph_pos.x + glyph->GetWidth(), glyph_pos.y + glyph->GetHeight()), &rtClip, colorRectTmp, fontBuffer, vScale);

				glyph_pos.x += glyph->GetAdvance()*vScale.x;
				// apply extra spacing to space chars
				if (strText[c] == ' ')
					glyph_pos.x += 0.0f;
			}

			// 超链接需要记录位置
			m_fXPosition = glyph_pos.x;
		}

		// reset
		ResetParse();
	}

	void GUIFont::RenderText( const GUIString& strText, const Vector2& position, const ERect& rtClip, const Color& color,
							bool bShadow, const Color& cShaderColor
							, bool bBorder, const Color& cBorderColor
							, const Vector2& vScale, GUIWindow* pWindow, FontBufferListType* fontBuffer)
	{
		m_DrawTextColor = color;
		
		m_OldColor = color;
		m_bIsDrawColorRect = false;

		RenderText_Impl(strText, position, rtClip, bShadow, cShaderColor, bBorder, cBorderColor, vScale, pWindow, fontBuffer);

	}

	void GUIFont::RenderText( const GUIString& strText, const Vector2& position, const ERect& rtClip, const ColorRect& color,
							bool bShadow, const Color& cShaderColor
							, bool bBorder, const Color& cBorderColor
							, const Vector2& vScale, GUIWindow* pWindow, FontBufferListType* fontBuffer)
	{
		m_DrawColorRect = color;
		m_bIsDrawColorRect = true;

		RenderText_Impl(strText, position, rtClip, bShadow, cShaderColor, bBorder, cBorderColor, vScale, pWindow, fontBuffer);
	}

	bool GUIFont::ParseCode( utf16 cCode, GUIWindow* pWindow )
	{
		bool bShouldContinue = false;

		switch(cCode)
		{
		case 0x25A2:// color
			{				
				m_bIsColor = true;
				bShouldContinue = true;
			}
			break;
		case 0x25A5:// color end
			{
				m_DrawTextColor = m_OldColor;				
				bShouldContinue = true;
			}
			break;
		case 0x25A3:// hyper link start
			{				
				m_bIsHyperlink = true;
				
				if (pWindow)
				{
					m_rtHyperlinkArea.left = m_fXPosition;
					m_rtHyperlinkArea.top = m_RenderPosition.y;
				}				

				bShouldContinue = true;
			}
			break;
		case 0x25A4:// hyper link end
			{				
				m_bIsHyperlink = false;

				if (pWindow)
				{
					m_rtHyperlinkArea.right = m_fXPosition;
					m_rtHyperlinkArea.bottom = m_RenderPosition.y + GetFontHeight();
					GUIHyperlinkManager::Instance()->AddHyperLink(m_rtHyperlinkArea, m_strHyperlinkInfo, pWindow);

					// draw underline
					ERect rtUnderline = m_rtHyperlinkArea;
					rtUnderline.top = rtUnderline.bottom-1;

					Color drawColor = m_DrawTextColor;
					if (GUIHyperlinkManager::Instance()->GetClickArea().isPointInRect(Vector2(m_rtHyperlinkArea.left+1, m_rtHyperlinkArea.top+1))
						&& GUIHyperlinkManager::Instance()->IsClickedLink())
					{
						drawColor = GUISystem::Instance()->GetHyperLinkClickColor();
					}

					m_UnderLineImage.DrawImage(pWindow, rtUnderline, drawColor.getABGR(), 0.0f);
				}

				m_strHyperlinkInfo.clear();
				bShouldContinue = true;
			}
			break;
		case 0x25A6://GUIAnimate begin
			{				
				m_bIsGUIAnimate = true;
				bShouldContinue = true;
				m_strAnimateId.clear();
			}
			break;
		case 0x25A7://GUIAnimate end
			{				
				m_bIsGUIAnimate = false;
				i32 id= StringUtil::ParseI32(m_strAnimateId.c_str());
				GUIAnimate* pAnimate=GUIAnimatMgr::Instance()->GetAnimatById(id);
				if (pAnimate!=NULL)
				{					
					Vector2 imageSize=pAnimate->getCurrentImageVect2();
					if (m_bRenderTxt)//被RenderText调用的	
					{
						float left=m_fXPosition;
						float top=m_RenderPosition.y+GetFontHeight()-imageSize.y;
						float right=left+imageSize.x;
						float bottom=top+imageSize.y;
						pAnimate->doPlay(ERect(left,top,right,bottom), pWindow);
					}
					else//被GetTextHigh或GetTextExtent或SplitStringToMultiLine调用的
						m_fAnimateHigh=m_fAnimateHigh > imageSize.y ? m_fAnimateHigh:imageSize.y;
					m_fXPosition+=imageSize.x;					
				}				
				bShouldContinue = true;
			}
			break;
		default:
			{
				// 处理颜色
				if (m_bIsColor)
				{
					m_strColorString += cCode;

					if (m_strColorString.length()==8)
					{
						unsigned int nColor = 0;
						sscanf(m_strColorString.c_str(),"%X",&nColor);
						m_DrawTextColor.setARGB(nColor);
						m_strColorString.clear();
						m_bIsColor = false;
					}

					bShouldContinue = true;
				}
				// 处理超链接
				if(m_bIsHyperlink)
				{
					if (cCode == cHyperlinkStart)
					{
						m_bIsHyperlinkInfo = true;
						bShouldContinue = true;
					}
					else if (cCode == cHyperlinkEnd)
					{
						m_bIsHyperlinkInfo = false;
						bShouldContinue = true;
					}
					else if (m_bIsHyperlinkInfo)
					{
						m_strHyperlinkInfo += cCode;
						bShouldContinue = true;
					}
				}
				//处理表情动画
				if(m_bIsGUIAnimate)
				{
					m_strAnimateId += cCode;
					bShouldContinue = true;
				}
			}
			break;
		}

		return bShouldContinue;
	}

	void GUIFont::ResetParse()
	{
		// reset
		m_bIsColor = false;
		m_bIsHyperlink = false;
		m_bIsHyperlinkInfo = false;
		m_bIsGUIAnimate = false;
		m_bRenderTxt=false;
		m_strColorString.clear();
		m_strHyperlinkInfo.clear();
		m_strAnimateId.clear();
		m_OldColor = Color::WHITE;
		m_fAnimateHigh=0.0f;
	}

	void GUIFont::SplitStringToMultiLine( float fWidth, const Color& cDefaultColor, const GUIString& strInput,
											TextStringList& outList ,vector<float>::type & outHighList)
	{
		const GUIFontGlyph* pGlyph;
		float fCharWidth = 0.0f;
		float fCurrentWidth = 0.0f;
		m_DrawTextColor = cDefaultColor;

		outList.clear();
		outHighList.clear();

		GUIString strLine;
		bool curLineHasImage = false;
		float lastImageHigh = 0.0f;

		float cur_word_width = 0.0f;
		utf16 word_node[21] = {
			' ', '\n', '-', '_' , ',' , '.' , '*' , '/' , ':' , ';',
			'=', '?', '&', '%' , '$' , '#' , '@' , '!' , '~', '(', 
			')'
		};

		for (size_t c = 0; c < strInput.length(); ++c)
		{
			utf16 cCode = strInput[c];
			m_fXPosition=0;

			if (ParseCode(cCode))
			{
				if (cCode==0x25A6)//动画开始标志
					continue;
				if (m_bIsGUIAnimate)
					continue;
				if (cCode==0x25A7)//动画结束标志
				{
					if(fCurrentWidth + m_fXPosition > fWidth)//需换行显示
					{						
						outList.push_back(strLine);
						strLine.clear();
						fCurrentWidth=0.0f;	
						if (curLineHasImage)//当前行前面有动画
							outHighList.push_back(Math::Max(lastImageHigh, m_fHeight + m_fLineExtraSpace));
						else						
							outHighList.push_back(m_fHeight + m_fLineExtraSpace);
						lastImageHigh=m_fAnimateHigh + m_fLineExtraSpace;
					}
					else
						lastImageHigh=Math::Max(lastImageHigh, m_fAnimateHigh + m_fLineExtraSpace);
					m_fAnimateHigh=0.0f;
					curLineHasImage=true;
					fCurrentWidth+=m_fXPosition;					
					strLine += 0x25A6; strLine += m_strAnimateId; strLine += 0x25A7;
					continue;
				}				
				strLine += cCode;
				continue;
			}

			size_t word_i = c;
			float word_width = 0.0f;
			bool interrupt = false;

			while (!interrupt)
			{
				if (word_i < strInput.length())
				{
					utf16 code = strInput[word_i];
					bool isNode = code >= 128;
					for (utf16 node : word_node)
					{
						if (node == code)
						{
							isNode = true;
							break;
						}
					}
					pGlyph = GetGlyphData(code);
					if (!isNode)
					{
						if (pGlyph)
						{
							word_width += pGlyph->GetAdvance();
						}
						word_i++;
					}
					else
					{
						interrupt = true;
					}
				}
				else
				{
					interrupt = true;
				}
			}

			if (cur_word_width < 0.1f)
			{
				cur_word_width = word_width;
			}

			if (cCode >= 128)
			{
				cur_word_width = 0;
			}
			else
			{
				for (utf16 node : word_node)
				{
					if (node == cCode)
					{
						cur_word_width = 0;
						break;
					}
				}
			}

			pGlyph = GetGlyphData(cCode);

			if (pGlyph)
			{
				fCharWidth = pGlyph->GetAdvance();

				if ((fCurrentWidth + fCharWidth > fWidth || cCode == '\n' || fCurrentWidth + word_width > fWidth) && cur_word_width <= fWidth)
				{
					cur_word_width = 0.0f;
					// 这一行结束加个超链接的结束符
					if (m_bIsHyperlink)
					{
						strLine += 0x25A4;
					}	
					
					outList.push_back(strLine);
					strLine.clear();
					if (curLineHasImage)//当前行前面有动画
						outHighList.push_back(Math::Max(lastImageHigh, m_fHeight + m_fLineExtraSpace));
					else						
						outHighList.push_back(m_fHeight + m_fLineExtraSpace);
					lastImageHigh=0.0f;
					curLineHasImage=false;
					// 下一行开始要把超链接的信息加进去
					if (m_bIsHyperlink)
					{
						strLine += 0x25A3;
						strLine += cHyperlinkStart;
						strLine += m_strHyperlinkInfo;
						strLine += cHyperlinkEnd;
					}
					// 加上颜色的解析
					strLine += 0x25A2;

					char buff[16];
					sprintf(buff, "%X", m_DrawTextColor.getARGB());

					strLine += buff;


					fCurrentWidth = 0.0f;
				}

				if (cCode != '\n')
				{
					fCurrentWidth += fCharWidth;
					strLine += cCode;
				}	
			}
		}

		if (!strLine.empty())
		{
			outList.push_back(strLine);
			if (curLineHasImage)//当前行前面有动画
				outHighList.push_back(Math::Max(lastImageHigh,m_fHeight + m_fLineExtraSpace));
			else						
				outHighList.push_back(m_fHeight + m_fLineExtraSpace);
			// lastImageHigh = 0.0f;
			// curLineHasImage = false;
		}
		
		// reset
		ResetParse();
	}
}
