/********************************************************************
filename: 	GUIAnimateWindow.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/

#ifndef __GUI_ANIMATE_WINDOW_H__
#define __GUI_ANIMATE_WINDOW_H__

#include "Core.h"
#include "GUIWindow.h"
#include "GUIRenderImage.h"

namespace LORD
{
	class LORD_CORE_API GUIAnimateWindow: public GUIWindow
	{
	public:

		GUIAnimateWindow(GUIWindowType nType, const GUIString& strName);
		virtual ~GUIAnimateWindow();

		virtual bool	SetProperty(const GUIString& strName, const GUIString& strValue);
		virtual bool	GetProperty(const GUIString& strName, GUIString& strValue);	
		virtual void	GetPropertyList(GUIPropertyList& properlist);
		virtual bool	GetPropertyType(const GUIString& strName, GUIPropertyType& type);		

		virtual void	DrawSelf();

		void			SetAnimateID(i32 nID);
		i32				GetAnimateID() { return m_nAnimateID; }

		void			play();

		void			pause(){ m_play = false; }

	protected:

		i32				m_nAnimateID;

		ui32			m_hasRenderTime;
		int				m_index;
		bool			m_play;

		GUIRenderImage	m_renderImage;
	};
}

#endif