#pragma once
#ifndef __GUI_APP_SHOP_HEADER__
#define __GUI_APP_SHOP_HEADER__
#include "Core.h"
#include "gui_layout.h"
#include "UI/GUILayout.h"
#include "UI/GUIGridView.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIRadioButton.h"
#include "GUI/RootGuiLayout.h"
#include "World/Shop.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_appShop : public  gui_layout
	{
	private:
		GUILayout* m_tabLayout = nullptr;
		GUILayout* m_itemListLayout = nullptr;
		GUIGridView* m_itemGridView = nullptr;
		GUIStaticText* m_textTip = nullptr;
		GUIRadioButton* m_selectTab = nullptr;
		SubscriptionGuard m_subscriptionGuard;

		ui32 m_showTipTime = 0;
		String m_tipMessage = "";

	public:
		gui_appShop();
		~gui_appShop();
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool onRadioChange(const EventArgs & events, GoodsGroup group, GUIRadioButton* radio);
		bool onButtonClick(const EventArgs & events);
		bool onLackOfMoney(const EventArgs & events);
		bool onShopUpdate();
		bool onBuyGoodsResult(const String& msg);

	private:
		void addTabView( GoodsGroup group, int index);

		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}
	};

}
#endif