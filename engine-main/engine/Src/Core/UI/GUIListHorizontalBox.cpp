#include "GUIListHorizontalBox.h"
#include "GUIWindowManager.h"
#include "GUIStaticImage.h"
#include "GUIRenderManager.h"
#include "Object/Root.h"
#include "GUISystem.h"
#include "Audio/SoundSystem.h"

namespace LORD
{
	//const float MAX_SCROLL_OUT_RANGE = 100.0f;
	//const float DAMP_PARAM = 0.8f;
	//const float GO_BACK_STEP = 20.0f;
	//const ui32	MAX_FLY_TIME = 500;

	GUIListHorizontalBox::GUIListHorizontalBox(GUIWindowType nType, const GUIString& strName)
		: GUIWindow(nType, strName)
		, m_pContainer(NULL)
		//, m_fYOffset(0.0f)
		//, m_bIsDragGoBack(false)
		//, m_fWholeGoBackDistance(false)
		, m_fItemWidth(30.0f)
		, m_nSelectIndex(-1)
		//, m_nStartFlyTime(0)
		//, m_fFlyInitSpeed(0.0f)
		//, m_bIsFlying(false)
		//, m_nFlyTime(0)
		, m_bIsBlinkSelect(false)
		, m_allowScroll(true)
		, m_fInteval(0.0f)
		, m_bCanMove(true)
	{
		m_scroll.SetScrollType(GUIScroll::HORIZON_SCROLL);
		SetClipChild(true);
	}

	GUIListHorizontalBox::~GUIListHorizontalBox()
	{
	}

	bool GUIListHorizontalBox::TouchDown(const Vector2& position)
	{
		MouseEventArgs args(this);
		args.position = position;
		fireEvent(EventWindowTouchDown, args);

		if (!m_pContainer)
		{
			return true;
		}

		if (m_allowScroll)
		{
			m_scroll.TouchDown(position, m_pContainer->GetPixelSize().x, GetPixelSize().x);
		}

		if (m_pContainer)
		{
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

	bool GUIListHorizontalBox::TouchMove(const Vector2& position)
	{

		if (!m_bCanMove)
		{
			return false;
		}

		if (!m_allowScroll)
		{
			return false;
		}

		if (m_pContainer)
		{
			float fStartSlowMinOffset = GetPixelSize().x - m_pContainer->GetPixelSize().x;
			float xOffset = m_scroll.TouchMove(position, fStartSlowMinOffset);
			m_pContainer->SetXPosition(UDim(0, xOffset));
			RestChildVisible();
			UnSelectAll();

			//加入滑动音效
			//SoundSystem::Instance()->playEffect( "slide.mp3", false );
			// m_DragPosition = position;
		}
		return true;
	}

	bool GUIListHorizontalBox::TouchUp(const Vector2& position)
	{
		if (m_pContainer)
		{
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

			}
			else
			{

			}

			float fStartSlowMinOffset = GetPixelSize().x - m_pContainer->GetPixelSize().x;

			m_scroll.TouchUp(position, fStartSlowMinOffset);
		}

		if (m_bIsBlinkSelect)
		{
			UnSelectAll();
		}
		return true;
	}

	bool GUIListHorizontalBox::MotionRelease()
	{
		if (!m_allowScroll)
		{
			return true;
		}

		if (m_pContainer)
		{
			float fStartSlowMinOffset = GetPixelSize().x - m_pContainer->GetPixelSize().x;
			m_scroll.MotionRelease(fStartSlowMinOffset);
		}

		return true;
	}

	void GUIListHorizontalBox::AddItem(GUIWindow* pItem, bool defaultWidth)
	{
		float itemWidth = 0;
		if (defaultWidth)
			itemWidth = m_fItemWidth;
		else
			itemWidth = pItem->GetPixelSize().x;

		if (!m_pContainer)
		{
			m_pContainer = GUIWindowManager::Instance()->CreateGUIWindow(GWT_DEFAULT_WINDOW, GetName() + "_Container");
			AddChildWindow(m_pContainer);
			m_pContainer->SetPosition(UVector2(UDim(0, 0), UDim(0, 0)));
			//m_pContainer->SetSize(UVector2(UDim(1.0f, 0), UDim(0, GetUnclippedOuterRect().getHeight())));
			m_pContainer->SetSize(UVector2(UDim(1.f,0), UDim(1.0f, 0)));
			m_pContainer->SetTouchable(false);
		}

		static unsigned count = 0;
		String strBackImageName = StringUtil::Format("%s_Item_Back_%d", GetName().c_str(), count++);
		GUIStaticImage* pBackImage = (GUIStaticImage*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, strBackImageName.c_str());
		pBackImage->SetTouchable(false);
		m_pContainer->AddChildWindow(pBackImage);
		pBackImage->SetPosition(UVector2(UDim(0, 0), UDim(0, 0)));
		pBackImage->SetSize(UVector2(UDim(0, itemWidth), UDim(1.0f,0)));
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

	void GUIListHorizontalBox::ClearAllItem()
	{
		if (m_pContainer)
		{
			while (m_pContainer->GetChildCount() > 0)
			{
				GUIWindow* pChild = m_pContainer->GetChildByIndex(0);
				GUIWindowManager::Instance()->DestroyGUIWindow(pChild);
			}
			m_nSelectIndex = -1;
		}

		m_scroll.SetOffset(0.0f);
	}

	void GUIListHorizontalBox::LayoutChild()
	{
		if (m_pContainer)
		{
			//float nHeight = 0.0f;
			float nWidth = 0.f;
			size_t nCount = m_pContainer->GetChildCount();
			for (size_t i = 0; i < nCount; ++i)
			{
				GUIWindow* pItem = m_pContainer->GetChildByIndex(i);
				pItem->SetXPosition(UDim(0, nWidth));
				pItem->SetYPosition(UDim(0, 0));

				nWidth += pItem->GetPixelSize().x + m_fInteval;
			}

			m_pContainer->SetWidth(UDim(0, nWidth));
		}
	}

	bool GUIListHorizontalBox::SetProperty(const GUIString& strName, const GUIString& strValue)
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
			SetItemWidth(StringUtil::ParseFloat(strValue.c_str()));
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

	bool GUIListHorizontalBox::GetProperty(const GUIString& strName, GUIString& strValue)
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
			strValue = StringUtil::ToString(m_fItemWidth).c_str();
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

	void GUIListHorizontalBox::GetPropertyList(GUIPropertyList& properlist)
	{
		GUIWindow::GetPropertyList(properlist);
		properlist.push_back(GWP_LIST_NORMAL_IMAGE_BACK);
		properlist.push_back(GWP_LIST_SELECT_IMAGE_BACK);
		properlist.push_back(GWP_LIST_ITEM_HEIGHT);
		properlist.push_back(GWP_LIST_BLINK_SELECT);
		properlist.push_back(GWP_LIST_ALLOW_SCROLL);
		properlist.push_back(GWP_LIST_BETWEEN_DISTANCE);
	}

	bool GUIListHorizontalBox::GetPropertyType(const GUIString& strName, GUIPropertyType& type)
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

	void GUIListHorizontalBox::SetItemNormalImage(const GUIString& strName)
	{
		if (m_strItemNormalImageName != strName)
		{
			m_strItemNormalImageName = strName;

			OnImageChanged();
		}
	}

	void GUIListHorizontalBox::SetItemSelectImage(const GUIString& strName)
	{
		if (m_strItemSelectImageName != strName)
		{
			m_strItemSelectImageName = strName;

			OnImageChanged();
		}
	}

	void GUIListHorizontalBox::OnImageChanged()
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
					if (m_nSelectIndex == i)
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

	void GUIListHorizontalBox::SetItemWidth(float fWidth)
	{
		if (fWidth != m_fItemWidth)
		{
			m_fItemWidth = fWidth;
			LayoutChild();
		}
	}

	void GUIListHorizontalBox::SetInterval(float interval)
	{
		m_fInteval = interval;
		LayoutChild();
	}

	void GUIListHorizontalBox::SetSelectItem(size_t nIndex)
	{
		if (m_pContainer && nIndex < m_pContainer->GetChildCount())
		{
			if (nIndex != m_nSelectIndex)
			{
				m_nSelectIndex = (int)nIndex;

				OnImageChanged();
			}
		}
	}

	void GUIListHorizontalBox::UnSelectAll()
	{
		if (m_pContainer)
		{
			m_nSelectIndex = -1;
			OnImageChanged();
		}
	}

	void GUIListHorizontalBox::UpdateSelf(ui32 nTimeElapse)
	{

		if (m_pContainer)
		{
			float fStartSlowMinOffset = GetPixelSize().x - m_pContainer->GetPixelSize().x;
			float fXOffset = m_scroll.Update(nTimeElapse, fStartSlowMinOffset);
			m_pContainer->SetXPosition(UDim(0, fXOffset));
		}
	}

	void GUIListHorizontalBox::DeleteItem(size_t nIndex)
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

	void GUIListHorizontalBox::ResetScroll()
	{
		m_scroll.SetOffset(0.f);
	}

	float GUIListHorizontalBox::GetScrollOffset()
	{
		return m_scroll.GetOffset();
	}

	void GUIListHorizontalBox::SetScrollOffset(float offset)
	{
		m_scroll.SetOffset(offset);
	}

	void GUIListHorizontalBox::OnEndDraw()
	{
		m_scroll.DrawThumb(GetRenderArea());

		GUIWindow::OnEndDraw();
	}

	void GUIListHorizontalBox::setContainerAlignment()
	{
		if (m_pContainer)
		{
			float containerX = m_pContainer->GetPixelSize().x;
			float listX = GetPixelSize().x;
			m_pContainer->SetHorizontalAlignment(listX > containerX ? HA_CENTRE : HA_LEFT);
			m_pContainer->SetVerticalAlignment(VA_CENTRE);
		}
		RestChildVisible();
	}

	void GUIListHorizontalBox::RestChildVisible()
	{
		if (m_pContainer)
		{
			for (size_t i = 0; i < m_pContainer->GetChildCount(); i++)
			{
				GUIWindow* pItem = m_pContainer->GetChildByIndex(i);
				float itemLeft = pItem->GetUnclippedOuterRect().left;
				float itemRight = pItem->GetUnclippedOuterRect().right;
				pItem->SetVisible(itemRight >= m_OuterUnclippedRect.left - m_fItemWidth && itemLeft <= m_OuterUnclippedRect.right + m_fItemWidth);
			}
		}
	}

}
