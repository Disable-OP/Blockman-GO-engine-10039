#include "GUITableView.h"
#include "GUIWindowManager.h"
#include "GUIEventArgs.h"
#include "GUICheckBox.h"

namespace LORD
{
	GUITableView::GUITableView( GUIWindowType nType, const GUIString& strName )
		: GUIWindow(nType, strName)
		, m_fHeight(0.0f)
		, m_fYOffset(0.0f)
		, m_isRadio(false)
	{
		SetClipChild(true);
	}

	GUITableView::~GUITableView()
	{
	}

	bool GUITableView::SetProperty( const GUIString& strName, const GUIString& strValue )
	{
		if (GUIWindow::SetProperty(strName, strValue))
		{
			return true;
		}

		return false;
	}

	bool GUITableView::GetProperty( const GUIString& strName, GUIString& strValue )
	{
		if (GUIWindow::GetProperty(strName, strValue))
		{
			return true;
		}

		return false;
	}

	void GUITableView::GetPropertyList( GUIPropertyList& properlist )
	{
		GUIWindow::GetPropertyList(properlist);
	}

	bool GUITableView::GetPropertyType( const GUIString& strName, GUIPropertyType& type )
	{
		if (GUIWindow::GetPropertyType(strName, type))
		{
			return true;
		}

		return false;
	}

	size_t GUITableView::AddHeader( GUICheckBox* pHeader )
	{
		SectionInfo section;
		section.pHeader = pHeader;
		section.bIsOpen = !m_isRadio;//true;
		section.cellList.clear();

		m_sectionList.push_back(section);

		if (pHeader)
		{
			AddChildWindow(pHeader);

			pHeader->subscribeEvent(EventCheckStateChanged, GUIEvent::Subscriber(&GUITableView::handleHeaderClicked, this));
		}

		LayoutSections();

		return (m_sectionList.size()-1);
	}

	void GUITableView::AddCell( size_t nHeaderIndex, GUIWindow* pCell )
	{
		if (nHeaderIndex >= m_sectionList.size() || !pCell)
		{
			return;
		}

		m_sectionList[nHeaderIndex].cellList.push_back(pCell);

		AddChildWindow(pCell);

		LayoutSections();
	}

	void GUITableView::ClearAllSections()
	{
		for (size_t i = 0; i < m_sectionList.size(); ++i)
		{
			if (m_sectionList[i].pHeader)
			{
				GUIWindowManager::Instance()->DestroyGUIWindow(m_sectionList[i].pHeader);
			}

			size_t nCellCount = m_sectionList[i].cellList.size();
			for ( size_t j = 0; j < nCellCount; ++j)
			{
				if (m_sectionList[i].cellList[j])
				{
					GUIWindowManager::Instance()->DestroyGUIWindow(m_sectionList[i].cellList[j]);
				}
			}

			m_sectionList[i].cellList.clear();
		}

		m_sectionList.clear();
	}

	void GUITableView::LayoutSections()
	{
		m_fHeight = 0.0f;
		for (size_t i = 0; i < m_sectionList.size(); ++i)
		{
			if (m_sectionList[i].pHeader)
			{
				m_sectionList[i].pHeader->SetYPosition(UDim(0.0f, m_fYOffset));
				m_fHeight += m_sectionList[i].pHeader->GetPixelSize().y;
				m_fYOffset += m_sectionList[i].pHeader->GetPixelSize().y;
			}

			if (m_sectionList[i].bIsOpen)
			{
				size_t nCellCount = m_sectionList[i].cellList.size();
				for ( size_t j = 0; j < nCellCount; ++j)
				{
					if (m_sectionList[i].cellList[j])
					{
						m_sectionList[i].cellList[j]->SetVisible(true);
						m_sectionList[i].cellList[j]->SetYPosition(UDim(0.0f, m_fYOffset));
						m_fHeight += m_sectionList[i].cellList[j]->GetPixelSize().y;
						m_fYOffset += m_sectionList[i].cellList[j]->GetPixelSize().y;
					}
				}
			}
			else
			{
				size_t nCellCount = m_sectionList[i].cellList.size();
				for ( size_t j = 0; j < nCellCount; ++j)
				{
					if (m_sectionList[i].cellList[j])
					{
						m_sectionList[i].cellList[j]->SetVisible(false);
					}
				}
			}
		}
	}

	void GUITableView::SetSectionOpen( size_t nSectionIndex, bool bOpen )
	{
		if (nSectionIndex >= m_sectionList.size())
		{
			return;
		}

		m_sectionList[nSectionIndex].bIsOpen = bOpen;

		LayoutSections();
	}

	bool GUITableView::TouchDown( const Vector2& position )
	{
		m_vertScroll.TouchDown(position, m_fHeight, GetPixelSize().y);

		return true;
	}

	bool GUITableView::TouchMove( const Vector2& position )
	{
		m_fYOffset = m_vertScroll.TouchMove(position, GetPixelSize().y - m_fHeight);
		LayoutSections();

		return true;
	}

	bool GUITableView::TouchUp( const Vector2& position )
	{
		m_vertScroll.TouchUp(position, GetPixelSize().y - m_fHeight);

		return true;
	}

	bool GUITableView::MotionRelease()
	{
		m_vertScroll.MotionRelease(GetPixelSize().y - m_fHeight);

		return true;
	}

	void GUITableView::UpdateSelf( ui32 nTimeElapse )
	{
		m_fYOffset = m_vertScroll.Update(nTimeElapse, GetPixelSize().y - m_fHeight);
		LayoutSections();
	}

	bool GUITableView::handleHeaderClicked( const EventArgs& e )
	{
		GUIWindow* pHeader = ((const WindowEventArgs&)e).window;

		for (size_t i = 0; i < m_sectionList.size(); ++i)
		{
			if (!m_isRadio)
			{
				if (m_sectionList[i].pHeader == pHeader)
				{
					m_sectionList[i].bIsOpen = !m_sectionList[i].bIsOpen;
					break;
				}
			}
			else
			{
				if (m_sectionList[i].pHeader == pHeader)
				{
					m_sectionList[i].bIsOpen = !m_sectionList[i].bIsOpen;
				}
				else
				{
					m_sectionList[i].bIsOpen = false;
				}
			}
		}

		LayoutSections();
		return true;
	}

	void GUITableView::OnEndDraw()
	{
		m_vertScroll.DrawThumb(GetRenderArea());

		GUIWindow::OnEndDraw();
	}

}