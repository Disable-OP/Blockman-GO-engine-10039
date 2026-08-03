#pragma once
#ifndef __GUI_SHOP_HEADER__
#define __GUI_SHOP_HEADER__
#include "Core.h"
#include "gui_layout.h"
#include "UI/GUIWindow.h"
#include "RootGuiLayout.h"
#include "UI/GUIListBox.h"
#include "UI/GUILayout.h"
#include "Util/SubscriptionGuard.h"
#include "Entity/EntityMerchant.h"
#include "UI/GUILayout.h"
#include "GUI/GUIShopGridView.h"
#include "GUI/GuiSlotTable.h"
#include "UI/GUIRadioButton.h"
#include "UI/GUIStaticText.h"


using namespace LORD;

namespace BLOCKMAN
{

	class gui_shop : public gui_layout
	{

	public:
		gui_shop();
		~gui_shop();
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool onShopOpen(EntityMerchant* merchant);//Lack of money
		bool onLackOfMoney(const EventArgs & events);
		bool onBuyCommodityResult(const String& msg);
		void onUpdateUnlockedState();

	private:
		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}

		void addTabView(CommodityGroup group  , int index);
		bool onRadioChange(const EventArgs & events, const CommodityGroup group , GUIRadioButton* radio);

		bool onClick(const EventArgs & events);
		void updateGridView(CommodityGroup group);
		void clearGridView();
		GUILayout* findCurrencViewByCoinId(int coinId, int index);
		bool onShowShopItemTip(int itemId, int damage, const String& image, int coinItemId, const String& tipDesc, const String& name, int price);
		bool onClickCloseShopItemTipBtn(const EventArgs &);

	private:
		int m_merchantId = 0;
		ui32 m_showTipTime = 0;
		String m_tipMessage = "";
		SubscriptionGuard m_subscriptionGuard;
		GUILayout* m_tabLayout = nullptr;
		GUILayout* m_goodsLayout = nullptr;
		GUILayout* m_currencyLayout = nullptr;
		GUIStaticText* m_textTip = nullptr;
		GuiShopGridView* m_goodsGridView = nullptr;
		GuiSlotTable* m_slotTable = nullptr;
		GUIRadioButton* m_currentSelectionTab = nullptr;
		map<int, GUILayout*>::type m_currencyMap;
		Vector3 m_showMerchantPosition;

		//ShopItemTips
		GUILayout* m_shopItemTip = nullptr;
		GUIButton* m_closeTipBtn = nullptr;
		GUIStaticImage* m_goodsIcon = nullptr;
		GUIStaticText* m_shopItemName = nullptr;
		GUILayout* m_shopItemBuyLayout = nullptr;
		GUIStaticImage* m_shopItemBuyIcon = nullptr;
		GUIStaticText* m_shopItemBuyCount = nullptr;
		GUILayout* m_shopItemTipDesc = nullptr;
	};

}

#endif

