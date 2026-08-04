#pragma once
#ifndef __GUI_RANCH_ORDER_HEADER__
#define __GUI_RANCH_ORDER_HEADER__

#include "gui_layout.h"
#include "UI/GUIWindow.h"
#include "UI/GUILayout.h"
#include "GUI/gui_ranch.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIGridView.h"
#include "UI/GUIButton.h"
#include "UI/GUIRadioButton.h"
#include "World/Ranch.h"
#include "GUI/GuiRanchOrderOperate.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_ranchOrder : public  gui_layout
	{
	private:
		enum class ViewId {
			BTN_MAIN,
			BTN_CLOSE,
			BTN_RECEIVE,
			BTN_REFRESH
		};
	public:
		gui_ranchOrder();
		virtual ~gui_ranchOrder();
		void onLoad() override;
		void onHide() override;
		void onShow() override;
		void onUpdate(ui32 nTimeElapse) override;
		gui_ranch* getParent()
		{
			return dynamic_cast<gui_ranch*>(m_parentLayout);
		}
		bool onClick(const EventArgs & events, ViewId viewId);
		bool onItemClick(const UVector2& pos, const RanchOrderInfo& info);
		bool onRanchOrderChange();
		bool onRadioChange(const EventArgs& args, GUIRadioButton* tab, i32 index);
		bool hideOrderOperate();

	private:
		void initTabs();
		void initTabBack(i32 status, GUIRadioButton* tab);
		void initOrderInfo();
		void initShipments();
		void initOrderReward();
		void initRefreshTime();

	private:
		bool m_dataChange = false;

		i32 m_selectIndex = 0;
		RanchOrder m_order;
		
		SubscriptionGuard m_subscriptionGuard;
		GUIStaticText* m_tvMsg = nullptr;
		GUIStaticText* m_tvShipmentsLeftTime = nullptr;
		GUIStaticText* m_tvRefreshLeftTime = nullptr;
		GUIGridView* m_gvOrderInfoGrid = nullptr;
		GUIGridView* m_gvOrderRewardGrid = nullptr;

		GuiRanchOrderOperate* m_orderOperate = nullptr;

		GUILayout* m_layoutTabs = nullptr;
		GUILayout* m_layoutOrderInfo = nullptr;
		GUILayout* m_layoutOrderShipments = nullptr;
		GUILayout* m_layoutOrderReward = nullptr;

		GUIButton* m_btnRefresh = nullptr;

		vector<UVector2>::type m_tabPositions;
		map<i32, GUIRadioButton*>::type m_tabMap;


	};
}

#endif