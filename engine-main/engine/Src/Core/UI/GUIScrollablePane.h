/********************************************************************
filename: 	GUIScrollablePane.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/

#ifndef __GUI_SCROLLABLE_PANE_H__
#define __GUI_SCROLLABLE_PANE_H__

#include "Core.h"
#include "GUIWindow.h"
#include "GUIScroll.h"

namespace LORD
{
	class GUIScrolledContainer;
	class LORD_CORE_API GUIScrollablePane: public GUIWindow
	{
	public:

		GUIScrollablePane(GUIWindowType nType, const GUIString& strName);
		virtual ~GUIScrollablePane();

		virtual bool	SetProperty(const GUIString& strName, const GUIString& strValue);
		virtual bool	GetProperty(const GUIString& strName, GUIString& strValue);	
		virtual void	GetPropertyList(GUIPropertyList& properlist);
		virtual bool	GetPropertyType(const GUIString& strName, GUIPropertyType& type);		

		void			InitializeContainer();

		void			InitializePos();

		virtual bool	TouchDown(const Vector2& position);
		virtual bool	TouchMove(const Vector2& position);
		virtual bool	TouchUp(const Vector2& position);
		virtual bool	MotionRelease();
		virtual void	UpdateSelf(ui32 nTimeElapse);

		virtual void	OnEndDraw();

		GUIScrolledContainer*	GetContainer() { return m_pContainer; }

		void			SetHoriScrollEnable(bool setting) { m_horiScrollEnable = setting; }
		void			SetVertScrollEnable(bool setting) { m_vertScrollEnable = setting; }

		void			SetHoriScrollOffset(float value);;
		float			GetHoriScrollOffset(){return m_horiScroll.GetOffset();};

		void			SetVertScrollOffset(float value);
		float			GetVertScrollOffset(){ return m_vertScroll.GetOffset(); };

		void			AddItem(GUIWindow* window);
		void			ClearAllItem();

	protected:

		GUIScrolledContainer*	m_pContainer;

		GUIScroll				m_horiScroll;		
		GUIScroll				m_vertScroll;
		bool					m_horiScrollEnable;
		bool					m_vertScrollEnable;
	};
}

#endif