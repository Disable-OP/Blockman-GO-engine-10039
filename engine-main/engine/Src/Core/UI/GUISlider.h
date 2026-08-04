/********************************************************************
filename: 	GUISlider.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/

#ifndef __GUI_SLIDER_H__
#define __GUI_SLIDER_H__

#include "Core.h"
#include "GUIProgressBar.h"

namespace LORD
{
	class LORD_CORE_API GUISlider: public GUIProgressBar
	{
	public:
		GUISlider(GUIWindowType nType, const GUIString& strName);
		virtual ~GUISlider();

		virtual bool	SetProperty(const GUIString& strName, const GUIString& strValue);
		virtual bool	GetProperty(const GUIString& strName, GUIString& strValue);	
		virtual void	GetPropertyList(GUIPropertyList& properlist);
		virtual bool	GetPropertyType(const GUIString& strName, GUIPropertyType& type);

		virtual bool	TouchDown(const Vector2& position);

		virtual bool	TouchMove(const Vector2& position);

		virtual bool	TouchUp(const Vector2& position);

		virtual bool	MotionRelease();

	protected:

		bool			IsInDragArea(const Vector2& position);

	protected:
		bool			m_bIsDragging;		
		Vector2			m_DragPosition;

		bool            m_bQuickSlide;
	};
}

#endif