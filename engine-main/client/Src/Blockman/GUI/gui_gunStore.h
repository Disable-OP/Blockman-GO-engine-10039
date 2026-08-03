#pragma once
#ifndef __GUI_GUN_STORE_HEADER__
#define __GUI_GUN_STORE_HEADER__

#include "Core.h"
#include "gui_layout.h"
#include "Util/SubscriptionGuard.h"
#include "GUIGunStoreTab.h"
#include "GUIGunStorePanel.h"
#include "GUIGunStoreModel.h"
#include "GUIGunStoreInfos.h"
#include "GUIStoreGunDetail.h"
#include "GUIStorePropDetail.h"
#include "GUI/RootGuiLayout.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_gunStore : public gui_layout
	{
	private:
		bool m_init = false;
		int m_cur_gunId = 0;
		int m_cur_propId = 0;

		list<GUIGunStoreTab*>::type m_store_tabs;
		GUIGunStorePanel* m_store_panel = NULL;
		GUIGunStoreModel* m_store_model = NULL;
		GUIGunStoreInfos* m_store_infos = NULL;
		GUIStoreGunDetail* m_store_gun_detail = NULL;
		GUIStorePropDetail* m_store_prop_detail = NULL;

		SubscriptionGuard m_subscriptionGuard;

	public:
		gui_gunStore();
		~gui_gunStore();

		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		void onShow() override;

	private:
		void initTabsAndPanels();
		void initPanelsChilds();
		void updateTabNewCount(int tabType);

		bool onCloseClick(const EventArgs & events);
		bool onTabChange(int type);
		bool onGunItemClick(int gunId);
		bool onPropItemClick(int propId);
		bool onShowGunDetail(int gunId);
		bool onShowPropDetail(int propId);

		bool onUpdateStoreGunData(const String& data);
		bool onUpdateStorePropData(const String& data);

		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}
	};

}
#endif