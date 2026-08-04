/********************************************************************
filename: 	GUIRadioButton.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/

#ifndef __GUI_RADIO_BUTTON_H__
#define __GUI_RADIO_BUTTON_H__

#include "Core.h"
#include "GUIButton.h"

namespace LORD
{
	class LORD_CORE_API GUIRadioButton: public GUIButton
	{
	public:

		GUIRadioButton(GUIWindowType nType, const GUIString& strName);
		virtual ~GUIRadioButton();

		virtual bool	SetProperty(const GUIString& strName, const GUIString& strValue);
		virtual bool	GetProperty(const GUIString& strName, GUIString& strValue);	
		virtual void	GetPropertyList(GUIPropertyList& properlist);
		virtual bool	GetPropertyType(const GUIString& strName, GUIPropertyType& type);
		
		virtual bool	TouchDown(const Vector2& position);
		virtual bool	TouchUp(const Vector2& position);
		virtual bool	MotionRelease();

		void			SetSelected(bool bSelected);
		bool			IsSelected() { return m_bSelected; }

		void			SetGroupID(uint nGroupID);
		uint			GetGroupID() { return m_nGroupID; }

	protected:
		void			DeselectOtherButtonInGroup();

	protected:

		bool			m_bSelected;
		uint			m_nGroupID;
	};
}

#endif