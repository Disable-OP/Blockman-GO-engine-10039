/********************************************************************
filename: 	GUIScrolledContainer.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/

#ifndef __GUI_SCROLLED_CONTAINER_H__
#define __GUI_SCROLLED_CONTAINER_H__

#include "Core.h"
#include "GUIWindow.h"

namespace LORD
{
	class LORD_CORE_API GUIScrolledContainer: public GUIWindow
	{
	public:

		GUIScrolledContainer(GUIWindowType nType, const GUIString& strName);
		virtual ~GUIScrolledContainer();

		virtual bool	SetProperty(const GUIString& strName, const GUIString& strValue);
		virtual bool	GetProperty(const GUIString& strName, GUIString& strValue);	
		virtual void	GetPropertyList(GUIPropertyList& properlist);
		virtual bool	GetPropertyType(const GUIString& strName, GUIPropertyType& type);		

		virtual void	OnChildAdd();
		virtual void	OnChildRemove();

		void			OnResizeContainer();

	protected:
		
	};
}

#endif