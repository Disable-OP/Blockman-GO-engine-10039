/********************************************************************
filename: 	GUIHyperlinkSystem.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/

#ifndef	__GUI_HYPERLINK_SYSTEM__H_
#define	__GUI_HYPERLINK_SYSTEM__H_

#include "Core.h"
#include "Object/Singleton.h"
#include "GUICommonDef.h"

namespace LORD
{
	class GUIString;
	class  LORD_CORE_API GUIHyperlinkManager: public ObjectAlloc, public Singleton<GUIHyperlinkManager>
	{
		
		public:
			GUIHyperlinkManager();
			~GUIHyperlinkManager();
			
			static GUIHyperlinkManager* Instance();

			void    AddHyperLink(const ERect& rtArea, const GUIString& strHyperLinkInfo, GUIWindow* pWindow);
			void    ClearAllHyperLink();
			bool    OnClickHyperLink(const Vector2& point, GUIWindow* pTargetWindow);
			void	SetClickLink(bool bClicked){ m_bIsClickedLink = bClicked; }
			bool	IsClickedLink() { return m_bIsClickedLink; }
			const ERect& GetClickArea() { return m_ClickArea; }

	protected:

			HyperlinkInfoList	m_HyperlinkInfoList;
			bool				m_bIsClickedLink;
			ERect				m_ClickArea;
	};


}

#endif		//--__GUI_HYPERLINK_SYSTEM__H_
