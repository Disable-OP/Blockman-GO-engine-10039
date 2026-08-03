#pragma once
#ifndef __GUI_RANCH_HOUSE_PANEL_HEADER__
#define __GUI_RANCH_HOUSE_PANEL_HEADER__

#include "gui_layout.h"
#include "UI/GUIWindow.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticImage.h"
#include "GUI/gui_ranchHouse.h"
#include "UI/GUIStaticText.h"
#include "GUI/GuiRanchCommonItem.h"
#include "GUI/GuiRanchItem.h"

using namespace LORD;

namespace BLOCKMAN
{

	class gui_ranchHousePanel : public  gui_layout
	{
	private:
		enum class ViewId {
			MAIN,
			SURE
		};

	public:
		gui_ranchHousePanel();
		virtual ~gui_ranchHousePanel();
		void onLoad() override;
		void onHide() override;
		void onShow() override;
		void onUpdate(ui32 nTimeElapse) override;
		gui_ranchHouse* getParent()
		{
			return dynamic_cast<gui_ranchHouse*>(m_parentLayout);
		}
		bool onClick(const EventArgs & events, ViewId viewId);
		bool onItemClick(i32 itemId, const String & name);
		bool onRanchHouseChange();

	private:
		i32 m_leftTime = 0;

		SubscriptionGuard m_subscriptionGuard;
		GUIStaticText * m_tvNowHouseName = nullptr;
		GUIStaticImage* m_ivNowHouseIcon = nullptr;


		GUIStaticText * m_tvNextHouseName = nullptr;
		GUIStaticImage* m_ivNextHouseIcon = nullptr;

		GUIStaticText* m_tvPrice = nullptr;
		GUIStaticText* m_tvNeedLevel = nullptr;

		GuiRanchItem* m_ranchItemInfo = nullptr;

		GuiRanchCommonItem* m_panelItemNeed1 = nullptr;
		GuiRanchCommonItem* m_panelItemNeed2 = nullptr;
		GuiRanchCommonItem* m_panelItemNeed3 = nullptr;

		vector<GuiRanchCommonItem*>::type m_panelItemVector;


	};
}

#endif