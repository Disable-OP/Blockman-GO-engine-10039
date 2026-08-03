#pragma once
#ifndef __GUI_TEAM_RESOURCES_HEADER__
#define __GUI_TEAM_RESOURCES_HEADER__
#include "GUI/gui_layout.h"
#include "GUI/RootGuiLayout.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIButton.h"
#include "UI/GUILayout.h"
#include "GUI/GuiTeamResourcesItem.h"
#include <map>

using namespace LORD;

namespace BLOCKMAN
{
	class gui_teamResources : public gui_layout
	{

	public:
		gui_teamResources();
		~gui_teamResources();

		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;


	private:
		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}
		bool onTeamResourcesUpdate(const String& data);
		GuiTeamResourcesItem::TeamResourcesItem initTeamResources(i32 teamId, i32 value, i32 maxValue);
		void setMaxResourcesTeamInfo();
		void setTeamResourcesItem(GuiTeamResourcesItem::TeamResourcesItem item);
		bool showNotification(const String& message, i32 keepTime);

	private:
		SubscriptionGuard m_subscriptionGuard;
		vector<GuiTeamResourcesItem *>::type m_teamResourcesView = vector<GuiTeamResourcesItem *>::type();
		vector<GuiTeamResourcesItem::TeamResourcesItem>::type m_teamResources = vector<GuiTeamResourcesItem::TeamResourcesItem>::type();
		i32 m_keepShowNotificationTime = 0;
		float m_recordPosY = 0.f;
		bool m_isResourceModify = false;

	private:
		GUILayout* m_maxResourceTeam = nullptr;
		GUILayout* m_resouceContainer = nullptr;
		GUILayout* m_teamResourceAll = nullptr;
		GUIStaticText*	m_notificationText = nullptr;
		GUIStaticText*	m_maxResourcesTeamText = nullptr;
		GUIStaticText*	m_myTeamResourceText = nullptr;
		GUIStaticText*	m_otherTeamResourceText = nullptr;
		GuiTeamResourcesItem * m_maxResourcesTeamInfo = nullptr;
		GuiTeamResourcesItem * m_myTeamResourcesInfo = nullptr;

	};
}
#endif
