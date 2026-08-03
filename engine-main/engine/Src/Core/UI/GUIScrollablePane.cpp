#include "GUIScrollablePane.h"
#include "GUIWindowManager.h"
#include "GUIScrolledContainer.h"
#include "GUIEventArgs.h"
#include "Object/Root.h"

namespace LORD
{
	GUIScrollablePane::GUIScrollablePane( GUIWindowType nType, const GUIString& strName )
		: GUIWindow(nType, strName)
		, m_pContainer(NULL)
		, m_horiScrollEnable(true)
		, m_vertScrollEnable(true)
	{
		SetClipChild(true);
		m_horiScroll.SetScrollType(GUIScroll::HORIZON_SCROLL);
		m_vertScroll.SetScrollType(GUIScroll::VERTICAL_SCROLL);
	}

	GUIScrollablePane::~GUIScrollablePane()
	{
		m_pContainer = NULL;
	}

	bool GUIScrollablePane::SetProperty( const GUIString& strName, const GUIString& strValue )
	{
		if (GUIWindow::SetProperty(strName, strValue))
		{
			return true;
		}

		if (strName == GWP_SCROLL_PANE_HORI_SCROLL)
		{
			SetHoriScrollEnable(StringUtil::ParseBool(strValue.c_str()));			
			return true;
		}
		else if (strName == GWP_SCROLL_PANE_VERT_SCROLL)
		{
			SetVertScrollEnable(StringUtil::ParseBool(strValue.c_str()));			
			return true;
		}

		return false;
	}

	bool GUIScrollablePane::GetProperty( const GUIString& strName, GUIString& strValue )
	{
		if (GUIWindow::GetProperty(strName, strValue))
		{
			return true;
		}

		if (strName == GWP_SCROLL_PANE_HORI_SCROLL)
		{
			strValue = StringUtil::ToString(m_horiScrollEnable).c_str();
			return true;
		}
		else if (strName == GWP_SCROLL_PANE_VERT_SCROLL)
		{
			strValue = StringUtil::ToString(m_vertScrollEnable).c_str();
			return true;
		}

		return false;
	}

	void GUIScrollablePane::GetPropertyList( GUIPropertyList& properlist )
	{
		GUIWindow::GetPropertyList(properlist);
		properlist.push_back(GWP_SCROLL_PANE_HORI_SCROLL);
		properlist.push_back(GWP_SCROLL_PANE_VERT_SCROLL);

	}

	bool GUIScrollablePane::GetPropertyType( const GUIString& strName, GUIPropertyType& type )
	{
		if (GUIWindow::GetPropertyType(strName, type))
		{
			return true;
		}

		if (strName == GWP_SCROLL_PANE_HORI_SCROLL)
		{
			type = GPT_BOOL;
			return true;
		}
		else if (strName == GWP_SCROLL_PANE_VERT_SCROLL)
		{
			type = GPT_BOOL;
			return true;
		}

		return false;
	}

	void GUIScrollablePane::InitializeContainer()
	{
		if (!m_pContainer)
		{
			m_pContainer = (GUIScrolledContainer*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_SCROLLED_CONTAINER, GetName() + "_auto_container");
			AddChildWindow(m_pContainer);
		}

		m_pContainer->SetTouchable(false);
		InitializePos();
	}

	void GUIScrollablePane::InitializePos()
	{
		m_vertScroll.SetOffset(0.0f);
		m_horiScroll.SetOffset(0.0f);
	}

	bool GUIScrollablePane::TouchDown( const Vector2& position )
	{
		GUIWindow::TouchDown( position );

		if (m_horiScrollEnable && m_pContainer)
		{
			m_horiScroll.TouchDown(position, m_pContainer->GetPixelSize().x, GetPixelSize().x);
		}

		if (m_vertScrollEnable && m_pContainer)
		{
			m_vertScroll.TouchDown(position, m_pContainer->GetPixelSize().y, GetPixelSize().y);
		}

		return true;
	}

	bool GUIScrollablePane::TouchMove( const Vector2& position )
	{
		if (!m_pContainer)
		{
			return true;
		}

		if (m_vertScrollEnable)
		{
			// vert
			float fStartSlowMinOffset = GetPixelSize().y - m_pContainer->GetPixelSize().y;
			float yOffset = m_vertScroll.TouchMove(position, fStartSlowMinOffset);
			m_pContainer->SetYPosition(UDim(0, yOffset));
		}

		if (m_horiScrollEnable)
		{
			// hori
			float fStartSlowMinOffsetX = GetPixelSize().x - m_pContainer->GetPixelSize().x;
			float xOffset = m_horiScroll.TouchMove(position, fStartSlowMinOffsetX);
			m_pContainer->SetXPosition(UDim(0, xOffset));
		}

		WindowEventArgs args(this);
		fireEvent(EventScrollLabelPaneContainerMove, args);

		return true;
	}

	bool GUIScrollablePane::TouchUp( const Vector2& position )
	{
		if (m_vertScrollEnable && m_pContainer)
		{
			// vert
			float fStartSlowMinOffset = GetPixelSize().y - m_pContainer->GetPixelSize().y;
			m_vertScroll.TouchUp(position, fStartSlowMinOffset);
		}

		if (m_horiScrollEnable && m_pContainer)
		{
			// hori
			float fStartSlowMinOffsetX = GetPixelSize().x - m_pContainer->GetPixelSize().x;
			m_horiScroll.TouchUp(position, fStartSlowMinOffsetX);
		}

		return true;
	}

	bool GUIScrollablePane::MotionRelease()
	{
        if (!m_pContainer) {
            return false;
        }
        
		if (m_vertScrollEnable)
		{
			// vert
			float fStartSlowMinOffset = GetPixelSize().y - m_pContainer->GetPixelSize().y;
			m_vertScroll.MotionRelease(fStartSlowMinOffset);
		}

		if (m_horiScrollEnable)
		{
			// hori
			float fStartSlowMinOffsetX = GetPixelSize().x - m_pContainer->GetPixelSize().x;
			m_horiScroll.MotionRelease(fStartSlowMinOffsetX);
		}

		return true;
	}

	void GUIScrollablePane::UpdateSelf( ui32 nTimeElapse )
	{
		if (m_pContainer)
		{
			if (m_vertScrollEnable)
			{
				// vert
				float fStartSlowMinOffset = GetPixelSize().y - m_pContainer->GetPixelSize().y;
				float fYOffset = m_vertScroll.Update(nTimeElapse, fStartSlowMinOffset);

				m_pContainer->SetYPosition(UDim(0, fYOffset));
			}

			if (m_horiScrollEnable)
			{

				// hori
				float fStartSlowMinOffsetX = GetPixelSize().x - m_pContainer->GetPixelSize().x;
				float fXOffset = m_horiScroll.Update(nTimeElapse, fStartSlowMinOffsetX);
				m_pContainer->SetXPosition(UDim(0, fXOffset));
			}
		}
	}

	void GUIScrollablePane::OnEndDraw()
	{
		if (m_vertScrollEnable)
		{
			// vert
			m_vertScroll.DrawThumb(GetRenderArea());
		}

		if (m_horiScrollEnable)
		{
			// hori
			m_horiScroll.DrawThumb(GetRenderArea());
		}

		GUIWindow::OnEndDraw();
	}

	void GUIScrollablePane::AddItem(GUIWindow* window)
	{
		if (window && m_pContainer)
		{
			m_pContainer->AddChildWindow(window);
			window->setChildPlayAction(isPlayingAction());
		}
	}

	void GUIScrollablePane::ClearAllItem()
	{
		InitializePos();
		if (m_pContainer)
		{
			m_pContainer->CleanupChildren();
		}
	}

	void GUIScrollablePane::SetVertScrollOffset(float value)
	{
		if (!m_pContainer)
		{
			return;
		}

		float fStartSlowMinOffset = GetPixelSize().y - m_pContainer->GetPixelSize().y;
		fStartSlowMinOffset = fStartSlowMinOffset > 0 ? 0 : fStartSlowMinOffset;

		value = value > 0 ? 0 : value;
		value = value < fStartSlowMinOffset ? fStartSlowMinOffset : value;

		m_vertScroll.SetOffset(value);
	}

	void GUIScrollablePane::SetHoriScrollOffset(float value)
	{
		if (!m_pContainer)
		{
			return;
		}

		float fStartSlowMinOffsetX = GetPixelSize().x - m_pContainer->GetPixelSize().x;
		fStartSlowMinOffsetX = fStartSlowMinOffsetX > 0 ? 0 : fStartSlowMinOffsetX;

		value = value > 0 ? 0 : value;
		value = value < fStartSlowMinOffsetX ? fStartSlowMinOffsetX : value;

		m_horiScroll.SetOffset(value);
	}

}
