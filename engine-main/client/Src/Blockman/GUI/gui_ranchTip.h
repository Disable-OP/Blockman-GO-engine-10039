#pragma once
#ifndef __GUI_RANCH_TIP_HEADER__
#define __GUI_RANCH_TIP_HEADER__

#include "gui_layout.h"
#include "Setting/RanchSetting.h"
#include "UI/GUIWindow.h"
#include "UI/GUILayout.h"
#include "UI/GUIListHorizontalBox.h"
#include "UI/GUIStaticText.h"
#include "Util/SubscriptionGuard.h"
#include "UI/GUIButton.h"
#include "UI/GUIGridView.h"
#include "UI/GUIStaticImage.h"
#include "Inventory/Wallet.h"
#include "Util/TipType.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_ranchTip : public  gui_layout
	{
	private:
		enum class ViewId {
			CLOSE,
			SURE
		};
	public:
		gui_ranchTip();
		virtual ~gui_ranchTip();
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool onClick(const EventArgs & events, ViewId viewId);
		bool showUnlockItems(const vector<i32>::type& recipe);
		bool showCommonTip(const String & msg);
		bool showQueueUnlock(i32 entityId, i32 queueId, i32 price , i32 currencyType, const String & msg);
		bool showLockOfMoneyTip(i32 difference, i32 currencyType);
		bool showLackItems(const vector<RanchCommon>::type& recipe);
		bool showRanchOrderOperateLockItems(const vector<RanchCommon>::type& recipe, i32 orderId, i32 index);
		bool showRanchLockItemsByBuildOperation(const vector<RanchCommon>::type& recipe, i32 entityId, i32 productId);
		bool showRanchLockItemsByUpgradeStorage(const vector<RanchCommon>::type& recipe);
		bool showRanchLockItemsByOrderHelp(const vector<RanchCommon>::type& recipe, i64 helpId);
		bool showRanchStorageFull(const String & msg);

	private:
		void btnSure();
		bool showGotoShop();
		bool showGotoApp();
		void initRecipeItem(const vector<RanchCommon>::type& items);
		void initUnlockItem(const vector<i32>::type& items);

	private:
		vector<RanchCommon>::type m_items = vector<RanchCommon>::type();
		i32 m_entityId = 0;
		i32 m_queueId = 0;

	private:
		RanchTipType m_tipType = RanchTipType::COMMON;

		GUIStaticText* m_tvTitle = nullptr;
		GUIStaticText* m_tvPrice = nullptr;
		GUIStaticText* m_tvPanelMsg = nullptr;
		GUIStaticText* m_tvItemsMsg = nullptr;
		GUIStaticImage* m_currencyIcon = nullptr;

		GUIButton* m_btnSure = nullptr;
		GUILayout* m_listLayout = nullptr;
		GUILayout* m_panelLayout = nullptr;
		GUILayout* m_lackMoneyLayout = nullptr;

		GUIListHorizontalBox* m_listView = nullptr;
		GUIGridView* m_gridView = nullptr;
		SubscriptionGuard m_subscriptionGuard;
		i32 m_landCode = 0;
		i32 m_price = 0;

		i32 m_orderId = 0;
		i32 m_orderIndex = 0;
		i32 m_productId = 0;
		i64 m_orderHelpId = 0;

		CurrencyType m_currencyType = CurrencyType::PLATFORM_MONEY;
	};
}

#endif