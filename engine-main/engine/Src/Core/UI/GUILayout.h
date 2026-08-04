/********************************************************************
filename: 	GUILayout.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/

#ifndef __GUI_LAYOUT_H__
#define __GUI_LAYOUT_H__

#include "Core.h"
#include "GUIWindow.h"
#include "GUIRenderImage.h"

namespace LORD
{
	class LORD_CORE_API GUILayout: public GUIWindow
	{
	public:
		GUILayout(GUIWindowType nType, const GUIString& strName);
		virtual ~GUILayout();
		
		virtual bool	SetProperty(const GUIString& strName, const GUIString& strValue);
		virtual bool	GetProperty(const GUIString& strName, GUIString& strValue);	
		virtual void	GetPropertyList(GUIPropertyList& properlist);
		virtual bool	GetPropertyType(const GUIString& strName, GUIPropertyType& type);
		void			SetBackImage(const GUIString& strBackImage);	

		virtual void	DrawSelf();

		void			LayoutChild();	

	protected:
		void			CalcuWndPixelSize(Vector2 & wndPixelSize);	

	protected:
		GUIRenderImage	m_BackImage;		
	};
}

#endif
