#pragma once
#ifndef __GUI_TEAM_RESOURCES_ITEM_HEADER__
#define __GUI_TEAM_RESOURCES_ITEM_HEADER__
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUILayout.h"
#include "UI/GUIProgressBar.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GuiTeamResourcesItem : public CustomGuiWindow
	{
	public:
		struct TeamResourcesItem
		{
			i32 m_teamId = 0;
			i32 m_value = 0;
			i32 m_maxValue = 0;
			String m_teamName = "";
			String m_teamIcon = "";
			String m_progressNor = "";
			String m_progressPre = "";

			TeamResourcesItem(){}

			TeamResourcesItem(i32 teamId, i32 value, i32 maxValue, String teamName, String teamIcon, String progressNor, String progressPre)
				: m_teamId(teamId)
				, m_value(value)
				, m_maxValue(maxValue)
				, m_teamName(teamName)
				, m_teamIcon(teamIcon)
				, m_progressNor(progressNor)
				, m_progressPre(progressPre)
			{

			}

		};

	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;
		GUIProgressBar* m_progressBar = nullptr;
		GUIStaticImage* m_teamIcon = nullptr;
		GUIStaticText* m_progressValue = nullptr;
		GUIString m_backImage = "";

	private:
		TeamResourcesItem m_teamResources;

	public:
		void setTeamResourcesItem(TeamResourcesItem item);
		void refreshUI();
		i32 getTeamId() { return m_teamResources.m_teamId; }

	private:
		void removeComponents();

	public:
		GuiTeamResourcesItem(const GUIWindowType& nType, const GUIString& strName);
		~GuiTeamResourcesItem() = default;
		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;

		void setBackImage(const GUIString& strBackImage);
		bool SetProperty(const GUIString& strName, const GUIString& strValue) override;
		bool GetProperty(const GUIString& strName, GUIString& strValue) override;
		void GetPropertyList(GUIPropertyList& properlist) override;
		bool GetPropertyType(const GUIString& strName, GUIPropertyType& type) override;

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;
	};

}
#endif
