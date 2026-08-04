#include "GUIRenderText.h"
#include "GUIFont.h"
#include "GUIWindow.h"
#include "GUIImageset.h"

namespace LORD
{
	GUIRenderText::GUIRenderText()
		: m_TextVertAlign(VA_TOP)
		, m_TextHorzAlign(HA_LEFT)
		, m_bIsWordWrap(false)
		, m_pFont(NULL)
		, m_bIsShadow(false)
		, m_ShadowColor(Color::BLACK)
		, m_bIsBorder(false)
		, m_BorderColor(Color::BLACK)
		, m_fWidth(0.0f)
		, m_TextColor(Color::WHITE)
		, m_fScale(1.0f)
		, m_bIsFontBufferDirty(true)
		, m_isColorRectCache(false)
		, m_renderOffset(Vector2::ZERO)
	{
	}

	GUIRenderText::~GUIRenderText()
	{
		m_fontBuffer.clear();
	}

	void GUIRenderText::SetTextVertAlign( VerticalAlignment vertAlign )
	{
		if (vertAlign != m_TextVertAlign)
		{
			m_TextVertAlign = vertAlign;
			invalidateFontBuffer();
		}
	}

	void GUIRenderText::SetTextHorzAlign( HorizontalAlignment horzAlign )
	{
		if (horzAlign != m_TextHorzAlign)
		{
			m_TextHorzAlign = horzAlign;
			invalidateFontBuffer();
		}
	}

	void GUIRenderText::SetWordWrap( bool bSetting )
	{
		if (bSetting != m_bIsWordWrap)
		{
			m_bIsWordWrap = bSetting;
			LayoutWordWrapText();
			if (!m_bIsWordWrap)	//单行			
				m_fTextWidth = m_pFont->GetTextExtent(m_strText, m_fScale);

			invalidateFontBuffer();
		}		
	}

	float GUIRenderText::GetTextStringHigh()
	{
		float result=0.0f;
		if (m_bIsWordWrap)
		{
			for (size_t i=0;i<m_TextHighList.size();++i)			
				result+=m_TextHighList[i] + m_fLineExtraSpace;			
		}
		else		
			result=m_pFont->GetTextHigh(m_strText);
		return result;
	}

	void GUIRenderText::SetText(const GUIString& strText)
	{
		if (strText != m_strText)
		{
			m_strText = strText;
			ParseEscapeChar();
			LayoutWordWrapText();	
			if (!m_bIsWordWrap)	//单行			
				m_fTextWidth = m_pFont->GetTextExtent(m_strText, m_fScale);

			invalidateFontBuffer();
		}
	}

	void GUIRenderText::LayoutWordWrapText()
	{
		if (m_bIsWordWrap && m_pFont)
		{
			m_pFont->SplitStringToMultiLine(m_fWidth / m_fScale, m_TextColor, m_strText, m_TextStringList,m_TextHighList);
			invalidateFontBuffer();
		}		
	}

	void GUIRenderText::SetFont( GUIFont* pFont )
	{
		if (pFont != m_pFont)
		{
			m_pFont = pFont;
			LayoutWordWrapText();
			if (!m_bIsWordWrap)	//单行			
				m_fTextWidth = m_pFont->GetTextExtent(m_strText, m_fScale);
			invalidateFontBuffer();
		}
	}

	void GUIRenderText::SetWidth( float fWidth )
	{
		if (fWidth != m_fWidth)
		{
			m_fWidth = fWidth;
			LayoutWordWrapText();

			invalidateFontBuffer();
		}
	}

	bool renderTextCompare(const FontBuffer& data1, const FontBuffer& data2)
	{
		if ( data1.imageset && data2.imageset && data1.imageset->GetTexture() && data2.imageset->GetTexture())
		{
			return data1.imageset->GetTexture()->getHandle() < data2.imageset->GetTexture()->getHandle();
		}

		return false;
	}

	void GUIRenderText::DrawGUIText( const ERect& rtArea, GUIWindow* pWindow, bool bIsColorRect, const ColorRect& colorRect)
	{
		if (!m_pFont)
		{
			return;
		}

		ERect clipRect = rtArea;
		if (pWindow)
		{
			clipRect = pWindow->GetClippedOuterRect();
		}

		// 参数缓存起来，如果不一样需要刷新font buffer
		if (m_areaCache != rtArea || m_isColorRectCache != bIsColorRect || m_colorRectCache != colorRect)
		{
			invalidateFontBuffer();

			m_areaCache = rtArea;
			m_isColorRectCache = bIsColorRect;
			m_colorRectCache = colorRect;
		}

		if (m_bIsFontBufferDirty)
		{
			m_fontBuffer.clear();
			m_bIsFontBufferDirty = false;

			// 刷新font buffer
			if (pWindow && !pWindow->IsTouchable())
			{
				pWindow = pWindow->GetTouchableParent();
			}

			// get alpha
			float alpha = 1.0f * (m_TextColor >> 24) / 255;
			// set shadow and border
			m_BorderColor.a = alpha;
			m_ShadowColor.a = alpha;

			ERect rtOuterRect = rtArea;
			Vector2 position;
			if (m_bIsWordWrap)
			{
				position.x = rtOuterRect.left;
				float textHigh = GetTextStringHigh();
				switch (m_TextVertAlign)
				{
				case VA_CENTRE:
					position.y = rtOuterRect.top + 0.5f * (rtOuterRect.getHeight() - textHigh);
					break;

				case VA_BOTTOM:
					position.y = rtOuterRect.top + rtOuterRect.getHeight() - textHigh;
					break;
				default:
					position.y = rtOuterRect.top;
					break;
				}
				float   calculHigh = position.y;
				
				for (size_t i = 0; i < m_TextStringList.size(); ++i)
				{
					float textWidth = m_pFont->GetTextExtent(m_TextStringList[i], m_fScale);
					switch (m_TextHorzAlign)
					{
					case HA_CENTRE:
						position.x = rtOuterRect.left + 0.5f * (rtOuterRect.getWidth() - textWidth);
						break;
					case HA_RIGHT:
						position.x = rtOuterRect.left + rtOuterRect.getWidth() - textWidth;
						break;
					default:
						position.x = rtOuterRect.left;
						break;
					}
					position.x = PixelAligned(position.x);
					position.y = position.y + m_TextHighList[i] - m_pFont->GetFontHeight();
					position.y = PixelAligned(position.y);

					if (bIsColorRect)
					{
						m_pFont->RenderText(m_TextStringList[i], position + m_renderOffset, clipRect, colorRect, m_bIsShadow,
							m_ShadowColor, m_bIsBorder, m_BorderColor, Vector2(m_fScale, m_fScale), pWindow, &m_fontBuffer);
					}
					else
					{
						m_pFont->RenderText(m_TextStringList[i], position + m_renderOffset, clipRect, m_TextColor, m_bIsShadow,
							m_ShadowColor, m_bIsBorder, m_BorderColor, Vector2(m_fScale, m_fScale), pWindow, &m_fontBuffer);
					}

					calculHigh += m_TextHighList[i] * m_fScale;
					position.y = calculHigh;
					//if (rtOuterRect.bottom<(position.y+lineSpace))//out of border
					//	break;
				}
			}
			else
			{
				float fTextHeight = m_pFont->GetFontHeight();
				switch (m_TextVertAlign)
				{
				case VA_CENTRE:
				{
								  position.y = rtOuterRect.top + 0.5f * (rtOuterRect.getHeight() - fTextHeight);
				}
					break;

				case VA_BOTTOM:
				{
								  position.y = rtOuterRect.top + rtOuterRect.getHeight() - fTextHeight;
				}
					break;
				default:
				{
						   position.y = rtOuterRect.top;
				}
					break;
				}

				switch (m_TextHorzAlign)
				{
				case HA_CENTRE:
				{
								  position.x = rtOuterRect.left + 0.5f * (rtOuterRect.getWidth() - m_fTextWidth);
				}
					break;
				case HA_RIGHT:
				{
								 position.x = rtOuterRect.left + rtOuterRect.getWidth() - m_fTextWidth;
				}
					break;
				default:
				{
						   position.x = rtOuterRect.left;
				}
					break;
				}

				position.x = PixelAligned(position.x);
				position.y = PixelAligned(position.y);

				if (bIsColorRect)
				{
					m_pFont->RenderText(m_strText, position + m_renderOffset, clipRect, colorRect, m_bIsShadow,
						m_ShadowColor, m_bIsBorder, m_BorderColor, Vector2(m_fScale, m_fScale), pWindow, &m_fontBuffer);
				}
				else
				{
					m_pFont->RenderText(m_strText, position + m_renderOffset, clipRect, m_TextColor, m_bIsShadow,
						m_ShadowColor, m_bIsBorder, m_BorderColor, Vector2(m_fScale, m_fScale), pWindow, &m_fontBuffer);
				}


			}
			// to do sort buffer list

			std::stable_sort(m_fontBuffer.begin(), m_fontBuffer.end(), renderTextCompare);
		} 

		drawCacheFont(pWindow);

	}

	void GUIRenderText::SetTextColor( const Color& color )
	{
		if (color != m_TextColor)
		{
			m_TextColor = color;
			LayoutWordWrapText();

			invalidateFontBuffer();
		}
	}

	void GUIRenderText::SetShadowColor( const Color& color )
	{
		if (color != m_ShadowColor)
		{
			m_ShadowColor = color;
			LayoutWordWrapText();

			invalidateFontBuffer();
		}
	}

	void GUIRenderText::SetShadowed( bool bShadow )
	{
		if (bShadow != m_bIsShadow)
		{
			m_bIsShadow = bShadow;

			invalidateFontBuffer();
		}
	}

	void GUIRenderText::SetBorderColor( const Color& color )
	{
		if (color != m_BorderColor)
		{
			m_BorderColor = color;
			LayoutWordWrapText();

			invalidateFontBuffer();
		}
	}

	void GUIRenderText::SetLineExtraSpace(float lineExtraSpace)
	{
		if (m_pFont->GetLineExtraSpace() != lineExtraSpace)
		{
			m_fLineExtraSpace = lineExtraSpace;
			m_pFont->SetLineExtraSpace(lineExtraSpace);
			LayoutWordWrapText();
			invalidateFontBuffer();
		}
	}

	void GUIRenderText::SetBordered( bool bBorder )
	{
		if (bBorder != m_bIsBorder)
		{
			m_bIsBorder = bBorder;
			invalidateFontBuffer();
		}
	}

	float GUIRenderText::GetTextExtent()
	{
		if (m_pFont)
		{
			return m_pFont->GetTextExtent(m_strText, m_fScale);
		}

		return 0.0f;
	}



	void GUIRenderText::ParseEscapeChar()
	{
		GUIString result;
		result.clear();
		// 处理转义字符
		size_t i = 0;
		size_t length = m_strText.length();
		while (i < length)
		{
			if (m_strText[i] == '^')
			{
				if ((i + 1 < length) && isCharHex(m_strText[i + 1]) &&
					(i + 2 < length) && isCharHex(m_strText[i + 2]) && 
					(i + 3 < length) && isCharHex(m_strText[i + 3]) && 
					(i + 4 < length) && isCharHex(m_strText[i + 4]) && 
					(i + 5 < length) && isCharHex(m_strText[i + 5]) && 
					(i + 6 < length) && isCharHex(m_strText[i + 6]))
				{
					// color start
					result += 0x25A2;
					result += "FF";
					result += m_strText[i + 1];
					result += m_strText[i + 2];
					result += m_strText[i + 3];
					result += m_strText[i + 4];
					result += m_strText[i + 5];
					result += m_strText[i + 6];
					
					i += 7;
				}
				else
				{
					result += m_strText[i];
					++i;
				}
			}
			else if (m_strText[i] == '#')
			{
				if (((i + 1) < length) && m_strText[i + 1] == 'n')
				{
					result += '\n';
					i += 2;
				}
				else
				{
					result += m_strText[i];
					++i;
				}
			}
			else
			{
				result += m_strText[i];
				++i;
			}
		}

		m_strText = result;
	}

	void GUIRenderText::DrawTextBuffer(GUIFont* font, FontBufferListType& buff)
	{
		for (auto it : buff)
		{
			const GUIFontGlyph* pGlyph = font->GetGlyphData(it.code);
			if (!pGlyph)
				continue;

			const GUIImage* pImage = pGlyph->GetImage();
			if (!pImage)
				continue;

			GUIImageset* pImageset = const_cast<GUIImageset*>(pImage->GetImageset());
			if (!pImageset)
				continue;

			GUIMaterialType material;
			if (pImageset && pImageset->IsFreeTypeFontBuffer())
			{
				material = GUIMaterialType_FONT;
			}
			else
			{
				material = GUIMaterialType_CULL_BACK;
			}
			pImageset->DrawImage(it.lt, it.rb, it.lb, it.rt, it.uv, it.color, material);
		}
	}

	void GUIRenderText::drawCacheFont(GUIWindow* pWindow)
	{
		if (pWindow)
		{
			Matrix4 transMatrix = pWindow->getActionMatrix();
			bool isAction = pWindow->isPlayingAction();
			for (FontBufferListType::iterator iter = m_fontBuffer.begin(); iter != m_fontBuffer.end(); ++iter)
			{
				const GUIFontGlyph* pGlyph = m_pFont->GetGlyphData(iter->code);
				if (pGlyph)
				{
					const GUIImage* const pImage = pGlyph->GetImage();
					if (pImage && pImage->GetImageset())
					{
						GUIImageset* pImageset = const_cast<GUIImageset*>(pImage->GetImageset());

						GUIMaterialType material;
						if (pImageset && pImageset->IsFreeTypeFontBuffer())
						{
							material = GUIMaterialType_FONT;
						} 
						else
						{
							material = GUIMaterialType_CULL_BACK;
						}

						//if (isAction)
						{
							// 要乘以变幻矩阵
							pImageset->DrawImage(iter->lt * transMatrix, iter->rb * transMatrix,
								iter->lb * transMatrix, iter->rt * transMatrix, iter->uv, iter->color, material);
						}
// 						else
// 						{
// 							pImageset->DrawImage(iter->lt, iter->rb, iter->lb, iter->rt, iter->uv, iter->color, material);
// 						}
					}
				}
			}
		}
	}

}
