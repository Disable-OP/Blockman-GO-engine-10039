#include "GUIListBox.h"
#include "GUIWindowManager.h"
#include "GUIStaticImage.h"
#include "GUIRenderManager.h"
#include "Object/Root.h"
#include "GUISystem.h"
#include "Audio/SoundSystem.h"

namespace LORD
{
	//const float MAX_SCROLL_OUT_RANGE = 100.0f;
	//// 阻尼系数 0 ～ 1
	//const float DAMP_PARAM = 0.8f;
	//const float GO_BACK_STEP = 20.0f;
	//const ui32	MAX_FLY_TIME = 500;

	GUIListBox::GUIListBox( GUIWindowType nType, const GUIString& strName )
		: GUIWindow(nType, strName)
		, m_pContainer(NULL)
		//, m_fYOffset(0.0f)
		//, m_bIsDragGoBack(false)
		//, m_fWholeGoBackDistance(false)
		, m_fItemHeight(30.0f)
		, m_nSelectIndex(-1)
		//, m_nStartFlyTime(0)
		//, m_fFlyInitSpeed(0.0f)
		//, m_bIsFlying(false)
		//, m_nFlyTime(0)
		, m_bIsBlinkSelect(false)
		, m_allowScroll(true)
		, m_fInteval(0.0f)
		, m_bCanMove( true )
	{
		SetClipChild(true);
	}

	GUIListBox::~GUIListBox()
	{
	}

	bool GUIListBox::TouchDown( const Vector2& position )
	{
// 		m_bIsDragging = true;
// 		m_DragPosition = position;
// 
// 		m_StartFlyPosition = position;
// 		m_nStartFlyTime = Root::Instance()->getCurrentTime();

		MouseEventArgs args(this);
		args.position = position;
		fireEvent(EventWindowTouchDown, args);

		if (!m_pContainer)
		{
			return true;
		}

		if (m_allowScroll)
		{
			m_scroll.TouchDown(position, m_pContainer->GetPixelSize().y, GetPixelSize().y);
		}

		if (m_pContainer)
		{
			// set select
			size_t i = 0;
			for (; i < m_pContainer->GetChildCount(); ++i)
			{
				GUIWindow* pItem = m_pContainer->GetChildByIndex(i);
				if (pItem->IsHit(position))
				{
					break;
				}
			}

			SetSelectItem(i);
		}
		return true;
	}

	bool GUIListBox::TouchMove( const Vector2& position )
	{
		//if (!m_allowScroll)
		//{
		//	m_DragPosition = position;
		//	m_StartFlyPosition = position;
		//	m_nStartFlyTime = Root::Instance()->getCurrentTime();
		//	return false;
		//}

		//if (Vector2(0,0)==(position-m_DragPosition))
		//	return false;

		if ( !m_bCanMove )
		{
			return false;
		}

		if (!m_allowScroll)
		{
			return false;
		}

		if (m_pContainer)
		{
// 			m_fYOffset += position.y - m_DragPosition.y;
// 
// 			//float fMaxOffset = m_pContainer->GetPixelSize().y - GetPixelSize().y;
// 			//fMaxOffset  = fMaxOffset < 0.0f ? 0.0f : fMaxOffset;
// 
// 			//m_fYOffset = m_fYOffset < -fMaxOffset ? -fMaxOffset : m_fYOffset;
// 
// 			//m_fYOffset = m_fYOffset > MAX_SCROLL_OUT_RANGE ? MAX_SCROLL_OUT_RANGE : m_fYOffset;
// 
// 			//m_pContainer->SetYPosition(UDim(0, m_fYOffset));
// 
// 			float fStartSlowMinOffset = GetPixelSize().y - m_pContainer->GetPixelSize().y;
// 			fStartSlowMinOffset = fStartSlowMinOffset > 0.0f ? 0.0f : fStartSlowMinOffset;
// 			
// 			float fStartSlowMaxOffset = 0.0f;
// 
// 			float fMinOffset = fStartSlowMinOffset - MAX_SCROLL_OUT_RANGE;
// 
// 			float fMaxOffset = MAX_SCROLL_OUT_RANGE;
// 
// 			if ( m_fYOffset > fStartSlowMaxOffset )
// 			{
// 				// 向下滑动变慢
// 				m_fYOffset -= DAMP_PARAM * (position.y - m_DragPosition.y);
// 			}
// 			else if (m_fYOffset < fStartSlowMinOffset )
// 			{
// 				// 向上滑动变慢
// 				m_fYOffset -= DAMP_PARAM * (position.y - m_DragPosition.y);
// 			}
// 
// 			m_fYOffset = m_fYOffset < fMinOffset ? fMinOffset : m_fYOffset;
// 			m_fYOffset = m_fYOffset > fMaxOffset ? fMaxOffset : m_fYOffset;

			float fStartSlowMinOffset = GetPixelSize().y - m_pContainer->GetPixelSize().y;
			float yOffset = m_scroll.TouchMove(position, fStartSlowMinOffset);
			m_pContainer->SetYPosition(UDim(0, yOffset));

			RestChildVisible();
			UnSelectAll();

			//加入滑动音效
			//SoundSystem::Instance()->playEffect( "slide.mp3", false );
			// m_DragPosition = position;
		}
		return true;
	}

	bool GUIListBox::TouchUp( const Vector2& position )
	{
		//m_bIsDragging = false;

		//float fFlyMoveDis = position.y - m_StartFlyPosition.y;
		//ui32 nElapsTime = Root::Instance()->getCurrentTime() - m_nStartFlyTime;

		//if (nElapsTime < 300)
		//{
		//	m_fFlyInitSpeed = fFlyMoveDis / nElapsTime;
		//	if (Math::Abs(m_fFlyInitSpeed) > 0)
		//	{
		//		m_bIsFlying = true;
		//		m_nFlyTime = 0;
		//	}
		//}
		
		if (m_pContainer)
		{
			// confirm select
			size_t i = 0;
			for (; i < m_pContainer->GetChildCount(); ++i)
			{
				GUIWindow* pItem = m_pContainer->GetChildByIndex(i);
				if (pItem->IsHit(position))
				{
					break;
				}
			}		

			if (i == m_nSelectIndex && i < m_pContainer->GetChildCount())
			{
				// fire event 
				// select changed
;
			}
			else
			{

			}

			float fStartSlowMinOffset = GetPixelSize().y - m_pContainer->GetPixelSize().y;

			m_scroll.TouchUp(position, fStartSlowMinOffset);
			
				//fStartSlowMinOffset = fStartSlowMinOffset > 0.0f ? 0.0f : fStartSlowMinOffset;

			//float fStartSlowMaxOffset = 0.0f;
			//if ( m_fYOffset > fStartSlowMaxOffset )
			//{
			//	// 向下滑动变慢
			//	m_bIsDragGoBack = true;
			//	m_fWholeGoBackDistance = m_fYOffset - fStartSlowMaxOffset;
			//}
			//else if (m_fYOffset < fStartSlowMinOffset )
			//{
			//	// 向上滑动变慢
			//	m_bIsDragGoBack = true;
			//	m_fWholeGoBackDistance = fStartSlowMinOffset - m_fYOffset;
			//}
		}

		if (m_bIsBlinkSelect)
		{
			UnSelectAll();
		}
		return true;
	}

	bool GUIListBox::MotionRelease()
	{
		if(!m_allowScroll)
		{
			return true;
		}

		if (m_pContainer)
		{
			float fStartSlowMinOffset = GetPixelSize().y - m_pContainer->GetPixelSize().y;
			m_scroll.MotionRelease(fStartSlowMinOffset);
		}

		//m_bIsDragging = false;

		//fStartSlowMinOffset = fStartSlowMinOffset > 0.0f ? 0.0f : fStartSlowMinOffset;

		//float fStartSlowMaxOffset = 0.0f;
		//if ( m_fYOffset > fStartSlowMaxOffset )
		//{
		//	// 向下滑动变慢
		//	m_bIsDragGoBack = true;
		//	m_fWholeGoBackDistance = m_fYOffset - fStartSlowMaxOffset;
		//}
		//else if (m_fYOffset < fStartSlowMinOffset )
		//{
		//	// 向上滑动变慢
		//	m_bIsDragGoBack = true;
		//	m_fWholeGoBackDistance = fStartSlowMinOffset - m_fYOffset;
		//}

		return true;
	}

	void GUIListBox::AddItem( GUIWindow* pItem ,bool defaultHigh)
	{
		float itemHigh=0;
		if (defaultHigh)
			itemHigh=m_fItemHeight;
		else
			itemHigh=pItem->GetPixelSize().y;

		if (!m_pContainer)
		{
			m_pContainer = GUIWindowManager::Instance()->CreateGUIWindow(GWT_DEFAULT_WINDOW, GetName()+"_Container");
			AddChildWindow(m_pContainer);
			m_pContainer->SetPosition(UVector2(UDim(0, 0), UDim(0, 0)));
			m_pContainer->SetSize(UVector2(UDim(1.0f, 0), UDim(0, GetUnclippedOuterRect().getHeight())));
			m_pContainer->SetTouchable(false);
		}

		static unsigned count = 0;
		String strBackImageName = StringUtil::Format("%s_Item_Back_%d", GetName().c_str(), count++);
		GUIStaticImage* pBackImage = (GUIStaticImage*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, strBackImageName.c_str());
		pBackImage->SetTouchable(false);
		m_pContainer->AddChildWindow(pBackImage);
		pBackImage->SetPosition(UVector2(UDim(0, 0), UDim(0, 0)));
		pBackImage->SetSize(UVector2(UDim(1.0f, 0), UDim(0, itemHigh)));
		pBackImage->SetImage(m_strItemNormalImageName);
		pBackImage->SetProperty(GWP_WINDOW_STRETCH_TYPE, "NineGrid");
		pBackImage->SetProperty(GWP_WINDOW_STRETCH_OFFSET, "4 4 4 4");
		//pBackImage->SetStretched(true);

		if (pItem)
		{
			pBackImage->AddChildWindow(pItem);
		}

		LayoutChild();
		RestChildVisible();
	}

	void GUIListBox::ClearAllItem()
	{
		if (m_pContainer)
		{
			while ( m_pContainer->GetChildCount() > 0 )
			{
				GUIWindow* pChild = m_pContainer->GetChildByIndex(0);
				GUIWindowManager::Instance()->DestroyGUIWindow(pChild);
			}
			m_nSelectIndex = -1;
		}

		m_scroll.SetOffset( 0.0f );
	}

	void GUIListBox::LayoutChild()
	{
		if (m_pContainer)
		{
			float nHeight = 0.0f;
			size_t nCount = m_pContainer->GetChildCount();
			for (size_t i = 0; i < nCount; ++i)
			{
				GUIWindow* pItem = m_pContainer->GetChildByIndex(i);
				pItem->SetXPosition(UDim(0, 0));
				pItem->SetYPosition(UDim(0, nHeight));

				nHeight += pItem->GetPixelSize().y + m_fInteval;
			}

			m_pContainer->SetHeight(UDim(0, nHeight));

			//if (nHeight < GetUnclippedOuterRect().getHeight())
			//{
			//	m_allowScroll = false;
			//}
			//else
			//{
			//	m_allowScroll = true;
			//}
		}
	}

	bool GUIListBox::SetProperty( const GUIString& strName, const GUIString& strValue )
	{
		if (GUIWindow::SetProperty(strName, strValue))
		{
			return true;
		}

		if (strName == GWP_LIST_NORMAL_IMAGE_BACK)
		{
			SetItemNormalImage(strValue);
			return true;
		}
		else if (strName == GWP_LIST_SELECT_IMAGE_BACK)
		{
			SetItemSelectImage(strValue);
			return true;
		}
		else if (strName == GWP_LIST_ITEM_HEIGHT)
		{
			SetItemHeight(StringUtil::ParseFloat(strValue.c_str()));
			return true;
		}
		else if (strName == GWP_LIST_BLINK_SELECT)
		{
			SetBlinkSelect(StringUtil::ParseBool(strValue.c_str()));
			return true;
		}
		else if (strName == GWP_LIST_ALLOW_SCROLL)
		{
			SetAllowScroll(StringUtil::ParseBool(strValue.c_str()));
			return true;
		}
		else if (strName == GWP_LIST_BETWEEN_DISTANCE)
		{
			m_fInteval = StringUtil::ParseFloat(strValue.c_str());
			return true;
		}

		return false;
	}

	bool GUIListBox::GetProperty( const GUIString& strName, GUIString& strValue )
	{
		if (GUIWindow::GetProperty(strName, strValue))
		{
			return true;
		}

		if (strName == GWP_LIST_NORMAL_IMAGE_BACK)
		{
			strValue = m_strItemNormalImageName;
			return true;
		}
		else if (strName == GWP_LIST_SELECT_IMAGE_BACK)
		{
			strValue = m_strItemSelectImageName;
			return true;
		}
		else if (strName == GWP_LIST_ITEM_HEIGHT)
		{
			strValue = StringUtil::ToString(m_fItemHeight).c_str();
			return true;
		}
		else if (strName == GWP_LIST_BLINK_SELECT)
		{
			strValue = StringUtil::ToString(m_bIsBlinkSelect).c_str();
			return true;
		}
		else if (strName == GWP_LIST_ALLOW_SCROLL)
		{
			strValue = StringUtil::ToString(m_allowScroll).c_str();
			return true;
		}
		else if (strName == GWP_LIST_BETWEEN_DISTANCE)
		{
			strValue = StringUtil::ToString(m_fInteval).c_str();
			return true;
		}

		return false;
	}

	void GUIListBox::GetPropertyList( GUIPropertyList& properlist )
	{
		GUIWindow::GetPropertyList(properlist);
		properlist.push_back(GWP_LIST_NORMAL_IMAGE_BACK);
		properlist.push_back(GWP_LIST_SELECT_IMAGE_BACK);
		properlist.push_back(GWP_LIST_ITEM_HEIGHT);
		properlist.push_back(GWP_LIST_BLINK_SELECT);
		properlist.push_back(GWP_LIST_ALLOW_SCROLL);
		properlist.push_back(GWP_LIST_BETWEEN_DISTANCE);
	}

	bool GUIListBox::GetPropertyType( const GUIString& strName, GUIPropertyType& type )
	{
		if (GUIWindow::GetPropertyType(strName, type))
		{
			return true;
		}

		if (strName == GWP_LIST_NORMAL_IMAGE_BACK)
		{
			type = GPT_IMAGE;
			return true;
		}
		else if (strName == GWP_LIST_SELECT_IMAGE_BACK)
		{
			type = GPT_IMAGE;
			return true;
		}
		else if (strName == GWP_LIST_ITEM_HEIGHT)
		{
			type = GPT_FLOAT;
			return true;
		}
		else if (strName == GWP_LIST_BLINK_SELECT)
		{
			type = GPT_BOOL;
			return true;
		}
		else if (strName == GWP_LIST_ALLOW_SCROLL)
		{
			type = GPT_BOOL;
			return true;
		}
		else if (strName == GWP_LIST_BETWEEN_DISTANCE)
		{
			type = GPT_FLOAT;
			return true;
		}

		return false;
	}

	void GUIListBox::SetItemNormalImage( const GUIString& strName )
	{
		if (m_strItemNormalImageName != strName)
		{
			m_strItemNormalImageName = strName;

			OnImageChanged();
		}
	}

	void GUIListBox::SetItemSelectImage( const GUIString& strName )
	{
		if (m_strItemSelectImageName != strName)
		{
			m_strItemSelectImageName = strName;

			OnImageChanged();
		}
	}

	void GUIListBox::OnImageChanged()
	{
		if (m_pContainer)
		{
			size_t nCount = m_pContainer->GetChildCount();
			for (size_t i = 0; i < nCount; ++i)
			{
				GUIWindow* pChild = m_pContainer->GetChildByIndex(i);
				if (pChild->GetType() == GWT_STATIC_IMAGE)
				{
					GUIStaticImage* pImage = (GUIStaticImage*)pChild;
					if ( m_nSelectIndex == i)
					{
						pImage->SetImage(m_strItemSelectImageName);
					}
					else
					{
						pImage->SetImage(m_strItemNormalImageName);
					}
				}
			}
		}
	}

	void GUIListBox::RestChildVisible()
	{
		if (m_pContainer)
		{
			for (size_t i = 0; i < m_pContainer->GetChildCount(); i++)
			{
				GUIWindow* pItem = m_pContainer->GetChildByIndex(i);
				float itemTop = pItem->GetUnclippedOuterRect().top;
				float itemBottom = pItem->GetUnclippedOuterRect().bottom;
				pItem->SetVisible(itemBottom >= m_OuterUnclippedRect.top - m_fItemHeight && itemTop <= m_OuterUnclippedRect.bottom + m_fItemHeight);
			}
		}
	}

	void GUIListBox::SetItemHeight( float fHeight )
	{
		if (fHeight != m_fItemHeight)
		{
			m_fItemHeight = fHeight;
			LayoutChild();
		}
	}

	void GUIListBox::SetSelectItem( size_t nIndex )
	{
		if (m_pContainer && nIndex < m_pContainer->GetChildCount())
		{
			if ( nIndex != m_nSelectIndex )
			{
				m_nSelectIndex = (int)nIndex;

				OnImageChanged();
			}
		}
	}

	void GUIListBox::UnSelectAll()
	{
		if (m_pContainer)
		{
			m_nSelectIndex = -1;
			OnImageChanged();
		}
	}

	void GUIListBox::UpdateSelf( ui32 nTimeElapse )
	{
		//static ui32 nElapse = 0;
		//if ( nElapse > 33 )
		//{
		//}
		//else
		//{
		//	nElapse += nTimeElapse;
		//	return;
		//}

		//if (m_pContainer)
		//{
		//	static float fMoveDistanse = 0.0f;

		//	float fStartSlowMinOffset = GetPixelSize().y - m_pContainer->GetPixelSize().y;
		//	fStartSlowMinOffset = fStartSlowMinOffset > 0.0f ? 0.0f : fStartSlowMinOffset;

		//	float fStartSlowMaxOffset = 0.0f;

		//	if (m_bIsDragGoBack)
		//	{
		//		float fParam = 1.0f;
		//		if ( m_fYOffset > fStartSlowMaxOffset )
		//		{
		//			// 向下滑动变慢
		//			fParam = -1.0f;
		//		}
		//		else if (m_fYOffset < fStartSlowMinOffset )
		//		{
		//			fParam = 1.0f;
		//			// 向上滑动变慢
		//		}

		//		m_fYOffset += fParam * GO_BACK_STEP;
		//		fMoveDistanse += GO_BACK_STEP;

		//		if (fMoveDistanse > m_fWholeGoBackDistance)
		//		{
		//			m_bIsDragGoBack = false;
		//			m_fYOffset -= fParam * (fMoveDistanse - m_fWholeGoBackDistance);
		//			fMoveDistanse = 0.0f;
		//		}
		//		m_pContainer->SetYPosition(UDim(0, m_fYOffset));
		//	}
		//	else if (m_bIsFlying)
		//	{
		//		m_fYOffset += m_fFlyInitSpeed * nElapse;

		//		m_nFlyTime += nElapse;

		//		if (m_fYOffset < fStartSlowMinOffset)
		//		{
		//			m_fYOffset = fStartSlowMinOffset;
		//			m_bIsFlying = false;
		//		}
		//		else if (m_fYOffset > fStartSlowMaxOffset)
		//		{
		//			m_fYOffset = fStartSlowMaxOffset;
		//			m_bIsFlying = false;
		//		}

		//		if (m_nFlyTime > MAX_FLY_TIME)
		//		{
		//			m_nFlyTime = 0;
		//			m_bIsFlying = false;
		//		}
		//		m_pContainer->SetYPosition(UDim(0, m_fYOffset));
		//	}
		//}

		//nElapse = 0;

		if (m_pContainer)
		{
			float fStartSlowMinOffset = GetPixelSize().y - m_pContainer->GetPixelSize().y;
			float fYOffset = m_scroll.Update(nTimeElapse, fStartSlowMinOffset);
			m_pContainer->SetYPosition(UDim(0, fYOffset));
		}
	}

	void GUIListBox::DeleteItem( size_t nIndex )
	{
		if (m_pContainer)
		{
			size_t nCount = m_pContainer->GetChildCount();
			if (nIndex < nCount)
			{
				GUIWindow* pItem = m_pContainer->GetChildByIndex(nIndex);
				GUIWindowManager::Instance()->DestroyGUIWindow(pItem);

				LayoutChild();
				RestChildVisible();
				m_nSelectIndex = -1;
			}
		}
	}

	void GUIListBox::ResetScroll()
	{
		m_scroll.SetOffset(0.f);
	}

	float GUIListBox::GetScrollOffset()
	{
		return m_scroll.GetOffset();
	}

	void GUIListBox::SetScrollOffset(float offset) 
	{
		m_scroll.SetOffset(offset);
	}

	void GUIListBox::OnEndDraw()
	{
		m_scroll.DrawThumb(GetRenderArea());

		GUIWindow::OnEndDraw();
	}

	int GUIListBox::GetChildIndex(const GUIString & strName)
	{
		if (m_pContainer)
		{
			return m_pContainer->GetChildIndex(strName);
		}
		return -1;
	}

}
