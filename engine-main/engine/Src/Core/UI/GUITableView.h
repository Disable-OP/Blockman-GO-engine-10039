/********************************************************************
filename: 	GUITableView.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/

#ifndef __GUI_TABLE_VIEW_H__
#define __GUI_TABLE_VIEW_H__

#include "Core.h"
#include "GUIWindow.h"
#include "GUIScroll.h"

namespace LORD
{
	class GUICheckBox;
	class LORD_CORE_API GUITableView: public GUIWindow
	{
	public:

		struct SectionInfo
		{
			GUICheckBox*	pHeader;
			ChildList	cellList;
			bool		bIsOpen;
		};

		typedef vector<SectionInfo>::type	SectionList;

		GUITableView(GUIWindowType nType, const GUIString& strName);
		virtual ~GUITableView();

		virtual bool	SetProperty(const GUIString& strName, const GUIString& strValue);
		virtual bool	GetProperty(const GUIString& strName, GUIString& strValue);	
		virtual void	GetPropertyList(GUIPropertyList& properlist);
		virtual bool	GetPropertyType(const GUIString& strName, GUIPropertyType& type);

		virtual bool	TouchDown(const Vector2& position);
		virtual bool	TouchMove(const Vector2& position);
		virtual bool	TouchUp(const Vector2& position);
		virtual bool	MotionRelease();
		virtual void	UpdateSelf(ui32 nTimeElapse);

		size_t			AddHeader(GUICheckBox* pHeader);
		void			AddCell(size_t nHeaderIndex, GUIWindow* pCell);
		void			ClearAllSections();
		void			LayoutSections();
		size_t			GetSectionCount() { return m_sectionList.size(); }
		void			SetSectionOpen(size_t nSectionIndex, bool bOpen);
		void			SetIsRadio(bool isRadio) {m_isRadio = isRadio;}

		virtual void	OnEndDraw();

	protected:

		bool			handleHeaderClicked(const EventArgs& e);

		SectionList		m_sectionList;

		float			m_fHeight;
		float			m_fYOffset;

		GUIScroll		m_vertScroll;

		bool			m_isRadio;
	};
}


#endif