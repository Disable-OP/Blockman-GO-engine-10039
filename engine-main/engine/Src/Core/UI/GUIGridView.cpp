#include "GUIGridView.h"

#include <cfloat>
#include "GUIWindowManager.h"
#include "Audio/SoundSystem.h"

namespace LORD
{
	GUIGridView::GUIGridView( GUIWindowType nType, const GUIString& strName )
		: GUIWindow(nType, strName)
		, m_fHeight(0.0f)
		, m_fYOffset(0.0f)
		, m_fHInterval(0.0f)
		, m_fVInterval(0.0f)
		, m_rowSize(1)
		, m_bQuickFind(true)
		, m_moveAble(true)
		, m_container(nullptr)
		, m_itemAlignment(HA_LEFT)
	{
		SetClipChild(true);
	}

	GUIGridView::~GUIGridView()
	{
	}

	bool GUIGridView::SetProperty( const GUIString& strName, const GUIString& strValue )
	{
		if (GUIWindow::SetProperty(strName, strValue))
		{
			return true;
		}

		if (strName == GWP_GRID_VIEW_ITEM_ALIGNMENT)
		{
			HorizontalAlignment alignment;
			if (strValue == "Centre")
			{
				alignment = HA_CENTRE;
			}
			else if (strValue == "Right")
			{
				alignment = HA_RIGHT;
			}
			else
			{
				alignment = HA_LEFT;
			}
			SetItemAlignment(alignment);
			return true;
		}

		return false;
	}

	bool GUIGridView::GetProperty( const GUIString& strName, GUIString& strValue )
	{
		if (GUIWindow::GetProperty(strName, strValue))
		{
			return true;
		}

		if (strName == GWP_GRID_VIEW_ITEM_ALIGNMENT)
		{
			switch (m_itemAlignment)
			{
			case HA_CENTRE:
				strValue = "Centre";
				break;

			case HA_RIGHT:
				strValue = "Right";
				break;

			default:
				strValue = "Left";
			}
			return true;
		}

		return false;
	}

	void GUIGridView::GetPropertyList( GUIPropertyList& properlist )
	{
		GUIWindow::GetPropertyList(properlist);
		properlist.push_back(GWP_GRID_VIEW_ITEM_ALIGNMENT);
	}

	bool GUIGridView::GetPropertyType( const GUIString& strName, GUIPropertyType& type )
	{
		if (GUIWindow::GetPropertyType(strName, type))
		{
			return true;
		}

		if (strName == GWP_GRID_VIEW_ITEM_ALIGNMENT)
		{
			type = GPT_HA;
			return true;
		}

		return false;
	}

	bool GUIGridView::TouchDown( const Vector2& position )
	{
		//GUIWindow::TouchDown( position );

		m_vertScroll.TouchDown(position, m_fHeight, GetPixelSize().y);
		MouseEventArgs args(this);
		args.position = position;
		fireEvent(EventWindowTouchDown, args);
		return true;
	}

	bool GUIGridView::TouchMove( const Vector2& position )
	{
		if (!m_moveAble)
			return false;
		m_fYOffset = m_vertScroll.TouchMove(position, GetPixelSize().y - m_fHeight);
		LayoutItems();
		RestChildVisible();
		MouseEventArgs args(this);
		args.position = position;
		fireEvent(EventWindowTouchMove, args);
		//SoundSystem::Instance()->playEffect( "slide.mp3", false );
		return true;
	}

	bool GUIGridView::TouchUp( const Vector2& position )
	{
		//GUIWindow::TouchUp( position );
		MouseEventArgs args(this);
		args.position = position;
		fireEvent(EventWindowTouchUp, args);
		m_vertScroll.TouchUp(position, GetPixelSize().y - m_fHeight);

		return true;
	}

	bool GUIGridView::MotionRelease()
	{
		m_vertScroll.MotionRelease(GetPixelSize().y - m_fHeight);

		return true;
	}

	void GUIGridView::UpdateSelf( ui32 nTimeElapse )
	{
		m_fYOffset = m_vertScroll.Update(nTimeElapse, GetPixelSize().y - m_fHeight);
		LayoutItems();
	}

	void GUIGridView::InitConfig(float hInterval,float vInterval,i32 rowSize)
	{
		if (hInterval<0 || vInterval <0 || rowSize<1)
			return;

		m_fHInterval = hInterval;
		m_fVInterval = vInterval;
		m_rowSize    = rowSize;

		LayoutItems();
	}
	void GUIGridView::AddItem(GUIWindow* pItem)
	{
		if (!m_container)
		{
			m_container = dynamic_cast<GUIMutualNonoverlappingGroup*>(GUIWindowManager::Instance()->CreateGUIWindow(GWT_MUTUAL_NONOVERLAPPING_GROUP, GetName() + "-Container"));
			m_container->SetArea({ 0, 0 }, { 0, 0 }, { 1, 0 }, { 1, 0 });
			m_container->SetTouchable(false);
			AddChildWindow(m_container);
		}

		if (!pItem)
			return;

		m_container->AddChildWindow(pItem);

		if(m_bQuickFind)//没有Item会hidden，可快速决定该pItem的位置
		{
			size_t size=m_Children.size();	
			float  xPosition = (size % m_rowSize) * (m_fHInterval + pItem->GetPixelSize().x) ;
			float  yPosition = (size / m_rowSize) * (m_fVInterval + pItem->GetPixelSize().y) ;
			pItem->SetPosition(UVector2(UDim(0,xPosition),UDim(0,yPosition)));
		}
		else
		{
			LayoutItems();
		}
		RestChildVisible();
	}

	void GUIGridView::RemoveItem( GUIWindow* pItem )
	{
		if ( !pItem || !m_container)
		{
			return;
		}

		m_container->RemoveChildWindow(pItem);
		LayoutItems();
		RestChildVisible();
	}

	void GUIGridView::LayoutItems()
	{
		if (!m_container)
		{
			return;
		}
		float xPosition = 0.0f , yPosition = m_fYOffset;
		size_t size = m_container->GetChildCount();
		m_fHeight=0.0f;
		float xOffset = FLT_MAX;
		for (size_t i=0; i<size; ++i)
		{
			auto child = m_container->GetChildByIndex(i);
			child->SetPosition(UVector2(UDim(0,xPosition),UDim(0,yPosition)));
			m_fHeight = yPosition + child->GetPixelSize().y;
			xPosition += m_fHInterval + child->GetPixelSize().x;
			if (0 == (i+1) % m_rowSize || i == size - 1)
			{
				float space;
				if (i == size - 1)
				{
					space = GetPixelSize().x - m_rowSize *  child->GetPixelSize().x;
				}
				else
				{
					space = GetPixelSize().x - xPosition;
				}
				using std::min;
				switch (m_itemAlignment)
				{
				case LORD::HA_LEFT:
					xOffset = 0.0f;
					break;
				case LORD::HA_CENTRE:
					xOffset = min(xOffset, space / 2);
					break;
				case LORD::HA_RIGHT:
					xOffset = min(xOffset, space);
					break;
				default:
					// TODO: Log error
					xOffset = 0.0f;
					break;
				}
				yPosition += m_fVInterval + child->GetPixelSize().y;
				xPosition = 0.0f;
			}		
		}
		if (xOffset)
		{
			for (size_t i = 0; i<size; ++i)
			{
				auto child = m_container->GetChildByIndex(i);
				auto position = child->GetPosition();
				auto x = position.d_x;
				auto newX = x + UDim(0, xOffset);
				child->SetPosition({ newX, position.d_y });
			}
		}
		for (size_t i = 0; i<size; ++i)
		{
			auto child = m_container->GetChildByIndex(i);
			auto position = child->GetPosition().asAbsolute(m_container->GetPixelSize());
			if (position.y + child->GetPixelSize().y <= 0 || position.y >= m_container->GetPixelSize().y)
			{
				child->SetVisible(false);
			}
			else
			{
				child->SetVisible(true);
			}
		}
		m_fHeight -= m_fYOffset;
	}

	void GUIGridView::ResetPos( )
	{
		m_vertScroll.SetOffset( 0 );
		m_fYOffset = 0;
		LayoutItems( );
	}

	void LORD::GUIGridView::SetItemAlignment(HorizontalAlignment align)
	{
		if (align != m_itemAlignment)
		{
			m_itemAlignment = align;
			NotifyScreenAreaChanged();
		}
	}

	void GUIGridView::OnEndDraw()
	{
		m_vertScroll.DrawThumb(GetRenderArea());

		GUIWindow::OnEndDraw();
	}

	int GUIGridView::GetChildIndex(const GUIString & strName)
	{
		if (m_container)
		{
			return m_container->GetChildIndex(strName);
		}
		return -1;
	}

	void GUIGridView::RestChildVisible()
	{
		if (m_container)
		{
			for (size_t i = 0; i < m_container->GetChildCount(); i++)
			{
				GUIWindow* pItem = m_container->GetChildByIndex(i);
				float itemTop = pItem->GetUnclippedOuterRect().top;
				float itemBottom = pItem->GetUnclippedOuterRect().bottom;
				pItem->SetVisible(itemBottom >= m_OuterUnclippedRect.top && itemTop <= m_OuterUnclippedRect.bottom);
			}
		}
	}

	float GUIGridView::GetScrollOffset()
	{
		return m_vertScroll.GetOffset();
	}

	void GUIGridView::SetScrollOffset(float offSet)
	{
		m_vertScroll.SetOffset(offSet);
	}

}