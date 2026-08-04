#include "GUIScrollCard.h"
#include "GUIEventArgs.h"
#include "GUISystem.h"
#include "GUIStaticText.h"
#include "Object/Root.h"

namespace LORD
{
	GUIScrollCard::GUIScrollCard( GUIWindowType nType, const GUIString& strName )
		: GUIWindow(nType, strName)
		, m_nSelectCardIndex(0)
		, m_nTargetCardIndex(0)
		, m_bIsScrolling(false)
		, m_nTimePassed(0)
        , m_bIsDragging(false)
        , m_nTouchDownTime(0)
        , m_fDragXPosition(0.0f)
        , m_nScrollSpeed(300)
	{
		SetClipChild(true);
	}

	GUIScrollCard::~GUIScrollCard()
	{
	}

	bool GUIScrollCard::SetProperty( const GUIString& strName, const GUIString& strValue )
	{
		if (GUIWindow::SetProperty(strName, strValue))
		{
			return true;
		}

		//if (strName == GWP_SCROLL_PANE_HORI_SCROLL)
		//{
		//	SetHoriScrollEnable(StringUtil::ParseBool(strValue.c_str()));			
		//	return true;
		//}
		//else if (strName == GWP_SCROLL_PANE_VERT_SCROLL)
		//{
		//	SetVertScrollEnable(StringUtil::ParseBool(strValue.c_str()));			
		//	return true;
		//}

		return false;
	}

	bool GUIScrollCard::GetProperty( const GUIString& strName, GUIString& strValue )
	{
		if (GUIWindow::GetProperty(strName, strValue))
		{
			return true;
		}

		//if (strName == GWP_SCROLL_PANE_HORI_SCROLL)
		//{
		//	strValue = StringUtil::ToString(m_horiScrollEnable).c_str();
		//	return true;
		//}
		//else if (strName == GWP_SCROLL_PANE_VERT_SCROLL)
		//{
		//	strValue = StringUtil::ToString(m_vertScrollEnable).c_str();
		//	return true;
		//}

		return false;
	}

	void GUIScrollCard::GetPropertyList( GUIPropertyList& properlist )
	{
		GUIWindow::GetPropertyList(properlist);
		//properlist.push_back(GWP_SCROLL_PANE_HORI_SCROLL);
		//properlist.push_back(GWP_SCROLL_PANE_VERT_SCROLL);

	}

	bool GUIScrollCard::GetPropertyType( const GUIString& strName, GUIPropertyType& type )
	{
		if (GUIWindow::GetPropertyType(strName, type))
		{
			return true;
		}

		//if (strName == GWP_SCROLL_PANE_HORI_SCROLL)
		//{
		//	type = GPT_BOOL;
		//	return true;
		//}
		//else if (strName == GWP_SCROLL_PANE_VERT_SCROLL)
		//{
		//	type = GPT_BOOL;
		//	return true;
		//}

		return false;
	}

	bool GUIScrollCard::TouchDown( const Vector2& position )
	{
		m_TouchDownPosition = position;
        m_nTouchDownTime = Root::Instance()->getCurrentTime();
        m_fDragXPosition = position.x;
		return true;
	}

	bool GUIScrollCard::TouchMove( const Vector2& position )
	{
//        ui32 nNowTime = Root::Instance()->getCurrentTime();
//        ui32 nTimePassed = nNowTime - m_nTouchDownTime;
//        if (!m_bIsDragging && nTimePassed < 200)
//        {
//            return true;
//        }
//        else
//        {
//            m_bIsDragging = true;
//        }
        
        //if (m_bIsDragging) {
            float fDeltaX = position.x - m_fDragXPosition;
            size_t nIndex = GetSelectCardIndex();
            if (fDeltaX > 15.0f) {
                ScrollToCard(nIndex-1, 100);
                m_fDragXPosition = position.x;
                m_bIsDragging = true;
            }
            else if( fDeltaX < -15.0f)
            {
                ScrollToCard(nIndex+1, 100);
                m_fDragXPosition = position.x;
                m_bIsDragging = true;
            }
        //}
        
		return true;
	}

	bool GUIScrollCard::TouchUp( const Vector2& position )
	{
//        if (m_bIsDragging) {
//            m_bIsDragging = false;
//            return true;
//        }
        m_bIsDragging = false;
        
        ui32 nNowTime = Root::Instance()->getCurrentTime();
        ui32 nTimePassed = nNowTime - m_nTouchDownTime;
        if (nTimePassed > 300) {
            return true;
        }
        
		float xOffset = position.x - m_TouchDownPosition.x;
		size_t nIndex = GetSelectCardIndex();
		size_t nCount = m_cardList.size();
        
        ui32 fSpeed = (int)fabs(2 * xOffset / nTimePassed);
        
		if (xOffset > 50.0f)
		{
			nIndex = nIndex >= fSpeed ? (nIndex-fSpeed) : 0;
			ScrollToCard(nIndex);
		}
		else if (xOffset < - 50.0f)
		{
			nIndex += fSpeed;
			nIndex = nIndex > nCount -1 ? nCount - 1 : nIndex;
			ScrollToCard(nIndex);
		}

		return true;
	}

	bool GUIScrollCard::MotionRelease()
	{
		return true;
	}

	void GUIScrollCard::UpdateSelf( ui32 nTimeElapse )
	{
		if (m_bIsScrolling)
		{
			if (m_nTimePassed < m_nScrollSpeed)
			{
				float fScale = m_nTimePassed * 1.0f / m_nScrollSpeed;

				size_t nCount = m_cardList.size();
				for (size_t i = 0; i < nCount; ++i)
				{
					m_cardList[i].pWindow->SetXPosition(UDim(0.0f, m_cardList[i].fStartPosition + fScale * (m_cardList[i].fEndPosition - m_cardList[i].fStartPosition)));
					m_cardList[i].pWindow->SetWidth(UDim(0.0f, m_cardList[i].fStartWidth + fScale * (m_cardList[i].fEndWidth - m_cardList[i].fStartWidth)));
					m_cardList[i].pWindow->SetHeight(UDim(0.0f, m_cardList[i].fStartHeight + fScale * (m_cardList[i].fEndHeight - m_cardList[i].fStartHeight)));
				
					float fTextScale = (m_cardList[i].fStartWidth + fScale * (m_cardList[i].fEndWidth - m_cardList[i].fStartWidth))/m_cardSize.x;
					ScaleChildText(m_cardList[i].pWindow, fTextScale);
				}

				m_nTimePassed += nTimeElapse;
			}
			else
			{
				m_bIsScrolling = false;

				size_t nCount = m_cardList.size();
				for (size_t i = 0; i < nCount; ++i)
				{
					m_cardList[i].pWindow->SetXPosition(UDim(0.0f, m_cardList[i].fEndPosition));
					m_cardList[i].pWindow->SetWidth(UDim(0.0f, m_cardList[i].fEndWidth));
					m_cardList[i].pWindow->SetHeight(UDim(0.0f, m_cardList[i].fEndHeight));
					ScaleChildText(m_cardList[i].pWindow, m_cardList[i].fEndWidth/m_cardSize.x);
				}
				m_nTimePassed = 0;
				m_nSelectCardIndex = m_nTargetCardIndex;
			}
		}
	}

	void GUIScrollCard::AddCard( GUIWindow* pWindow )
	{
		LordAssert(pWindow);
		pWindow->SetVerticalAlignment(VA_CENTRE);
		AddChildWindow(pWindow);

		pWindow->subscribeEvent(EventWindowTouchUp, GUIEvent::Subscriber(&GUIScrollCard::handleCardClicked, this));

		m_cardSize = pWindow->GetPixelSize();
		CardData temp;
		temp.pWindow = pWindow;
		m_cardList.insert(m_cardList.begin(), temp);
	}

	void GUIScrollCard::ClearAllCard()
	{
		CleanupChildren();
		m_cardList.clear();
	}

	void GUIScrollCard::ScrollToCard( size_t nCardIndex, ui32 nSpeed )
	{
		if (nCardIndex >= m_cardList.size())
		{
			return;
		}

        m_nScrollSpeed = nSpeed;
		m_nTargetCardIndex = nCardIndex;
		m_nTimePassed = 0;

		if (m_nTargetCardIndex == m_nSelectCardIndex)
		{
			ResetCardPos(m_nTargetCardIndex);
		}
		else
		{
			m_bIsScrolling = true;

			// 设置各个位置
			Vector2 pixelSize = GetPixelSize();
			GUIWindow* pWindow = m_cardList[m_nTargetCardIndex].pWindow;
			float fXLeft = 0.5f * (pixelSize.x - m_cardSize.x);
			float fXRight = fXLeft + m_cardSize.x;

			m_cardList[m_nTargetCardIndex].fStartPosition = pWindow->GetXPosition().d_offset;
			m_cardList[m_nTargetCardIndex].fStartWidth = pWindow->GetWidth().d_offset;
			m_cardList[m_nTargetCardIndex].fStartHeight = pWindow->GetHeight().d_offset;

			m_cardList[m_nTargetCardIndex].fEndPosition = fXLeft;
			m_cardList[m_nTargetCardIndex].fEndWidth = m_cardSize.x;
			m_cardList[m_nTargetCardIndex].fEndHeight = m_cardSize.y;

			pWindow->SetLevel(0);

			int nFrontIndex = (int)m_nTargetCardIndex-1;
			float fXPosition = fXLeft;
			float fScale = 1.0f;
			int nIndex = 1;
			float fScaleOffset = 0.08f;
			while( nFrontIndex >= 0)
			{
				fXPosition -= 40.0f;
				fScale -= fScaleOffset;
				fScale = fScale < 0.0f ? 0.0f : fScale;

				GUIWindow* pWindow = m_cardList[nFrontIndex].pWindow;
				m_cardList[nFrontIndex].fStartPosition = pWindow->GetXPosition().d_offset;
				m_cardList[nFrontIndex].fStartWidth = pWindow->GetWidth().d_offset;
				m_cardList[nFrontIndex].fStartHeight = pWindow->GetHeight().d_offset;

				m_cardList[nFrontIndex].fEndPosition = fXPosition;
				m_cardList[nFrontIndex].fEndWidth = m_cardSize.x*fScale;
				m_cardList[nFrontIndex].fEndHeight = m_cardSize.y*fScale;

				pWindow->SetLevel(nIndex);
				++nIndex;
				--nFrontIndex;

				fScaleOffset -= 0.01f;
				if (fScaleOffset < 0.05)
				{
					fScaleOffset = 0.05f;
				}
			}

			size_t nBackIndex = m_nTargetCardIndex+1;
			fXPosition = fXRight;
			fScale = 1.0f;
			nIndex = 1;
			fScaleOffset = 0.08f;
			while(nBackIndex < m_cardList.size())
			{
				fScale -= fScaleOffset;
				fScale = fScale < 0.0f ? 0.0f : fScale;

				GUIWindow* pWindow = m_cardList[nBackIndex].pWindow;

				m_cardList[nBackIndex].fStartPosition = pWindow->GetXPosition().d_offset;
				m_cardList[nBackIndex].fStartWidth = pWindow->GetWidth().d_offset;
				m_cardList[nBackIndex].fStartHeight = pWindow->GetHeight().d_offset;

				m_cardList[nBackIndex].fEndWidth = m_cardSize.x*fScale;
				m_cardList[nBackIndex].fEndHeight = m_cardSize.y*fScale;
				fXPosition = fXPosition + 40.0f - m_cardList[nBackIndex].fEndWidth;
				m_cardList[nBackIndex].fEndPosition = fXPosition;

				pWindow->SetLevel(nIndex);

				fXPosition += m_cardList[nBackIndex].fEndWidth;
				++nIndex;
				++nBackIndex;

				fScaleOffset -= 0.01f;
				if (fScaleOffset < 0.05)
				{
					fScaleOffset = 0.05f;
				}
			}
		}
	}

	bool GUIScrollCard::handleCardClicked( const EventArgs& e )
	{
        if(m_bIsDragging)
        {
            return true;
        }
        
		GUIWindow* pCard = ((const WindowEventArgs&)e).window;

		size_t nCount = m_cardList.size();
		for (size_t i = 0; i < nCount; ++i)
		{
			if (pCard == m_cardList[i].pWindow)
			{
				ScrollToCard(i);

				//GUISystem::Instance()->GetEventCallback()(GET_SCROLLCARD_CLICK, GetOwnerDialogName(), GetName(), i, 0);
				WindowEventArgs args(this);
				fireEvent(EventScrollCardClick, args);

				break;
			}
		}

		return true;
	}

	void GUIScrollCard::ResetCardPos(size_t nCardIndex)
	{
		if (nCardIndex >= m_cardList.size())
		{
			return;
		}

		m_nTargetCardIndex = nCardIndex;
		m_nSelectCardIndex = nCardIndex;

		// 不用滚动
		m_bIsScrolling = false;
		// 设置各个位置
		Vector2 pixelSize = GetPixelSize();
		GUIWindow* pWindow = m_cardList[m_nSelectCardIndex].pWindow;
		float fXLeft = 0.5f * (pixelSize.x - m_cardSize.x);
		float fXRight = fXLeft + m_cardSize.x;
		pWindow->SetXPosition(UDim(0.0f, fXLeft));
		pWindow->SetYPosition(UDim(0.0f, 0.0f));
		pWindow->SetWidth(UDim(0.0f, m_cardSize.x));
		pWindow->SetHeight(UDim(0.0f, m_cardSize.y));
		pWindow->SetLevel(0);
		ScaleChildText(pWindow, 1.0f);

		int nFrontIndex = (int)m_nSelectCardIndex-1;
		float fXPosition = fXLeft;
		float fScale = 1.0f;
		int nIndex = 1;
		float fScaleOffset = 0.08f;
		while( nFrontIndex >= 0)
		{
			fXPosition -= 40.0f;
			fScale -= fScaleOffset;
			fScale = fScale < 0.0f ? 0.0f : fScale;

			GUIWindow* pWindow = m_cardList[nFrontIndex].pWindow;
			pWindow->SetXPosition(UDim(0.0f, fXPosition));
			pWindow->SetWidth(UDim(0.0f, m_cardSize.x*fScale));
			pWindow->SetHeight(UDim(0.0f, m_cardSize.y*fScale));
			ScaleChildText(pWindow, fScale);
			pWindow->SetLevel(nIndex);
			++nIndex;
			--nFrontIndex;

			fScaleOffset -= 0.01f;
			if (fScaleOffset < 0.05)
			{
				fScaleOffset = 0.05f;
			}
		}

		size_t nBackIndex = m_nSelectCardIndex+1;
		fXPosition = fXRight;
		fScale = 1.0f;
		nIndex = 1;
		fScaleOffset = 0.08f;
		while(nBackIndex < m_cardList.size())
		{
			fScale -= fScaleOffset;
			fScale = fScale < 0.0f ? 0.0f : fScale;

			GUIWindow* pWindow = m_cardList[nBackIndex].pWindow;
			pWindow->SetWidth(UDim(0.0f, m_cardSize.x*fScale));
			pWindow->SetHeight(UDim(0.0f, m_cardSize.y*fScale));
			ScaleChildText(pWindow, fScale);

			fXPosition = fXPosition + 40.0f - pWindow->GetPixelSize().x;
			pWindow->SetXPosition(UDim(0.0f, fXPosition));
			pWindow->SetLevel(nIndex);

			fXPosition += pWindow->GetPixelSize().x;
			++nIndex;
			++nBackIndex;

			fScaleOffset -= 0.01f;
			if (fScaleOffset < 0.05)
			{
				fScaleOffset = 0.05f;
			}
		}
	}

	void GUIScrollCard::ScaleChildText( GUIWindow* pWindow, float fScale )
	{
		LordAssert(pWindow);
		size_t nCount = pWindow->GetChildCount();
		for (size_t i = 0; i < nCount; ++i)
		{
			GUIWindow* pChild = pWindow->GetChildByIndex(i);
			if (pChild->GetType() == GWT_STATIC_TEXT)
			{
				GUIStaticText* pText = (GUIStaticText*)pChild;
				pText->SetTextScale(fScale);
			}
		}
	}

}
