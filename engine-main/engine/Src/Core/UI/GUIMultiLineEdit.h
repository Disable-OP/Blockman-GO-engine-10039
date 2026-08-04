/********************************************************************
filename: 	GUIMultiLineEdit.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/

#ifndef __GUI_MULTILINE_EDIT_H__
#define __GUI_MULTILINE_EDIT_H__

#include "Core.h"
#include "GUIEditBox.h"
#include "GUIRenderImage.h"
#include "GUIRenderText.h"

namespace LORD
{
	class GUIImageset;
	class LORD_CORE_API GUIMultiLineEdit: public GUIEditBox
	{
	public:
		GUIMultiLineEdit(GUIWindowType nType, const GUIString& strName);
		virtual ~GUIMultiLineEdit();

		virtual bool	SetProperty(const GUIString& strName, const GUIString& strValue);
		virtual bool	GetProperty(const GUIString& strName, GUIString& strValue);	
		virtual void	GetPropertyList(GUIPropertyList& properlist);
		virtual bool	GetPropertyType(const GUIString& strName, GUIPropertyType& type);

		virtual void	DrawSelf();
		virtual void	OnTextChanged();
		virtual void	OnFontChanged();
		virtual void	OnSized();

	protected:
		void UpdateCaratOffset();
		
	};
}

#endif
