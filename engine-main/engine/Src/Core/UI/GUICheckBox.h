/********************************************************************
filename: 	GUICheckBox.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/

#ifndef __GUI_CHECK_BOX_H__
#define __GUI_CHECK_BOX_H__

#include "Core.h"
#include "GUIButton.h"
#include "GUIRenderImage.h"
#include "GUICommonDef.h"

namespace LORD
{
	class LORD_CORE_API GUICheckBox: public GUIButton
	{
	public:

		GUICheckBox(GUIWindowType nType, const GUIString& strName);
		virtual ~GUICheckBox();
		
		virtual bool	TouchDown(const Vector2& position);

		virtual bool	TouchUp(const Vector2& position);

		virtual bool	MotionRelease();

		void			SetChecked(bool bChecked);
		void			SetCheckedNoEvent(bool bChecked);

		bool			GetChecked() { return m_bChecked; }

	protected:

		bool			m_bChecked;
	};
}

#endif