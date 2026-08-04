/********************************************************************
filename: 	GUIWindowManager.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/

#ifndef __GUI_WINDOW_MANAGER_H__
#define __GUI_WINDOW_MANAGER_H__

#include "Core.h"
#include "Object/Singleton.h"
#include "GUICommonDef.h"
#include "GUIString.h"

namespace LORD
{
	class GUIWindow;
	class LORD_CORE_API GUIWindowManager : public ObjectAlloc, public Singleton<GUIWindowManager>
	{
	public:
		GUIWindowManager();
		~GUIWindowManager();

		static GUIWindowManager* Instance();

		GUIWindow* CreateGUIWindow(GUIWindowType nType, const GUIString& strName);
		GUIWindow* CreateGUIWindow(const GUIString& strType, const GUIString& strName);

		void	DestroyGUIWindow(GUIWindow* window);

		void	DestroyGUIWindow(const GUIString& strName);

		GUIWindow*	GetGUIWindow(const GUIString& strName) const;

		bool	IsGUIWindowPresent(const GUIString& strName) const;

		void	DestroyAllGUIWindows(void);

		void	CleanDeadPool(void);

		void	WriteXML(GUIWindow* pWindow, const GUIString& strFileName);
		
		GUIWindow* LoadWindowFromXML(const GUIString& strFileName);

		void	WriteJSON(GUIWindow* pWindow, const GUIString& strFileName);

		GUIWindow* LoadWindowFromJSON(const GUIString& strFileName);

		GUIWindow* CreateWindowFromTemplate(const GUIString& strNamePreffix, const GUIString& strTemplateFileName);

		GUIWindow* CloneWindow(const GUIString& strNamePreffix, GUIWindow* pWindow);

		void	RenameWindow(GUIWindow* pWindow, const GUIString& strNewName);

		typedef map<GUIString, GUIString>::type		PropertyDefaultValue;

		PropertyDefaultValue						m_defalutValue;

		void SetEnterBack(bool enterBackFlag) { m_enterBackFlag = enterBackFlag; }

		bool IsEnterBack() { return m_enterBackFlag; };

	private:

		typedef map<GUIString, GUIWindow*>::type	WindowRegistry;				
		typedef vector<GUIWindow*>::type				WindowVector;

		WindowRegistry	m_WindowRegistry;
		WindowVector    m_Deathrow;

		bool m_enterBackFlag = false;

	};
}


#endif