/********************************************************************
filename: 	GUIProgressBar.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/

#ifndef __GUI_PROGRESS_BAR_H__
#define __GUI_PROGRESS_BAR_H__

#include "Core.h"
#include "GUIWindow.h"
#include "GUIRenderImage.h"

namespace LORD
{
	class LORD_CORE_API GUIProgressBar: public GUIWindow
	{
	public:
		GUIProgressBar(GUIWindowType nType, const GUIString& strName);
		virtual ~GUIProgressBar();

		virtual bool	SetProperty(const GUIString& strName, const GUIString& strValue);
		virtual bool	GetProperty(const GUIString& strName, GUIString& strValue);	
		virtual void	GetPropertyList(GUIPropertyList& properlist);
		virtual bool	GetPropertyType(const GUIString& strName, GUIPropertyType& type);

		virtual void	DrawSelf();

		void			SetBackImage(const GUIString& strBackImage);
		void			SetProgressImage(const GUIString& strProgressImage);
		void			SetHeaderImage(const GUIString& strHeaderImage);
		void			SetVertical(bool isVertical) { m_isVertical = isVertical; }

		void			SetProgress(float fProgress);
		float			GetProgress() { return m_fProgress; }

	protected:

		float			m_fProgress;
		bool			m_isVertical;

		// renderData
		GUIRenderImage	m_BackImage;//背景图
		GUIRenderImage	m_ProgressImage;//当前进度的图
		GUIRenderImage	m_HeaderImage;//进度条末端图片
	};
}

#endif