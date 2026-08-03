#include "gui_ranchOrder.h"
#include "Util/LanguageKey.h"
#include "Util/ClientEvents.h"
#include "Util/UICommon.h"
#include "UI/GUIRadioButton.h"
#include "UI/GUIStaticImage.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "GUI/GuiRanchOrderItem.h"
#include "GUI/GuiRanchItem.h"
#include "GUI/GuiDef.h"
#include "Network/ClientNetwork.h"
#include "Util/RedDotManager.h"

using namespace LORD;

namespace BLOCKMAN
{
	gui_ranchOrder::gui_ranchOrder()
		: gui_layout("RanchOrder.json")
	{
		m_tabPositions.push_back(UVector2(UDim(0, 0), UDim(0, 0)));
		m_tabPositions.push_back(UVector2(UDim(0, 176.f), UDim(0, 89.f)));
		m_tabPositions.push_back(UVector2(UDim(0, 280.f), UDim(0, 0.f)));
		m_tabPositions.push_back(UVector2(UDim(1.0f, -138.f), UDim(0, 70.f)));
		m_tabPositions.push_back(UVector2(UDim(1.f, -260.f), UDim(1.f, -206.f)));
		m_tabPositions.push_back(UVector2(UDim(0, 60.f), UDim(1.f, -206.f)));
	}

	gui_ranchOrder::~gui_ranchOrder()
	{
		m_subscriptionGuard.unsubscribeAll();
		m_tabMap.clear();
	}

	void gui_ranchOrder::onLoad()
	{

		setText("RanchOrder-Title-Name", "gui_ranch_main_order");
		setText("RanchOrder-Panel-OrderInfo-Title", "gui_ranch_order_info_title");
		setText("RanchOrder-Panel-OrderReward-Receive", "gui_ranch_order_receive");
		setText("RanchOrder-Panel-OrderReward-Title", "gui_ranch_order_reward_title");
		setText("RanchOrder-Panel-OrderShipments-Title", "gui_ranch_order_shipments_title");
		setText("RanchOrder-Panel-OrderShipments-Msg", "gui_ranch_order_shipments_msg");
		setText("RanchOrder-Refresh-Price", "0");

		
		getWindow("RanchOrder-Btn-Close")->subscribeEvent(EventButtonClick, std::bind(&gui_ranchOrder::onClick, this, std::placeholders::_1, ViewId::BTN_CLOSE));
		getWindow("RanchOrder-Panel-Order-Refresh")->subscribeEvent(EventButtonClick, std::bind(&gui_ranchOrder::onClick, this, std::placeholders::_1, ViewId::BTN_REFRESH));
		getWindow("RanchOrder-Panel-OrderReward-Receive")->subscribeEvent(EventButtonClick, std::bind(&gui_ranchOrder::onClick, this, std::placeholders::_1, ViewId::BTN_RECEIVE));

		m_btnRefresh = getWindow<GUIButton>("RanchOrder-Panel-Order-Refresh");
		m_tvMsg = getWindow<GUIStaticText>("RanchOrder-Panel-Order-Msg");
		m_tvRefreshLeftTime = getWindow<GUIStaticText>("RanchOrder-Refresh-LeftTime");
		m_tvShipmentsLeftTime = getWindow<GUIStaticText>("RanchOrder-Panel-OrderShipments-TimeLeft");
		m_gvOrderInfoGrid = getWindow<GUIGridView>("RanchOrder-Panel-OrderInfo-Grid");
		m_gvOrderRewardGrid = getWindow<GUIGridView>("RanchOrder-Panel-OrderReward-Grid");
		m_orderOperate = getWindow<GuiRanchOrderOperate>("RanchOrder-Panel-OrderInfo-Operate");
		m_orderOperate->SetVisible(false);

		m_gvOrderInfoGrid->InitConfig(13.f, 0.f, 2);
		m_gvOrderRewardGrid->InitConfig(13.f, 0.f, 2);

		m_layoutTabs = getWindow<GUILayout>("RanchOrder-Panel-OrderList-Tabs");
		m_layoutOrderInfo = getWindow<GUILayout>("RanchOrder-Panel-OrderInfo");
		m_layoutOrderReward = getWindow<GUILayout>("RanchOrder-Panel-OrderReward");
		m_layoutOrderShipments = getWindow<GUILayout>("RanchOrder-Panel-OrderShipments");
		m_subscriptionGuard.add(RanchOrderItemHideEvent::subscribe(std::bind(&gui_ranchOrder::hideOrderOperate, this)));
		m_subscriptionGuard.add(RanchOrderChangeEvent::subscribe(std::bind(&gui_ranchOrder::onRanchOrderChange, this)));
		m_subscriptionGuard.add(RanchOrderItemClickEvent::subscribe(std::bind(&gui_ranchOrder::onItemClick, this, std::placeholders::_1, std::placeholders::_2)));

		m_rootWindow->subscribeEvent(EventWindowTouchDown, std::bind(&gui_ranchOrder::onClick, this, std::placeholders::_1, ViewId::BTN_MAIN));
		m_gvOrderInfoGrid->subscribeEvent(EventWindowTouchDown, std::bind(&gui_ranchOrder::onClick, this, std::placeholders::_1, ViewId::BTN_MAIN));
	}

	void gui_ranchOrder::onHide()
	{
		hideOrderOperate();
	}

	void gui_ranchOrder::onShow()
	{
		initTabs();
	}

	void gui_ranchOrder::onUpdate(ui32 nTimeElapse)
	{
		if (!isShown())
		{
			hideOrderOperate();
			return;
		}

		for (auto view : m_tabMap)
		{
			const vector<RanchOrder>::type& orders = Blockman::Instance()->m_pPlayer->m_ranch->getOrders();
			if ((int)orders.size() > view.first )
			{
				view.second->GetChildByIndex(2)->SetVisible(RedDotManager::checkOrder(orders[view.first].id));
			}
		}

		if (m_order.status == 3 && m_order.timeLeft > 0)
		{
			m_order.timeLeft = (m_order.timeLeft - nTimeElapse > 0) ? (m_order.timeLeft - nTimeElapse) : 0;
			initShipments();
		}

		if (m_order.status == 1 && m_order.refreshTimeLeft > 0)
		{
			m_order.refreshTimeLeft = (m_order.refreshTimeLeft - nTimeElapse > 0) ? (m_order.refreshTimeLeft - nTimeElapse) : 0;
			initRefreshTime();
		}
	}

	bool gui_ranchOrder::onClick(const EventArgs & events, ViewId viewId)
	{
		switch (viewId)
		{
		case ViewId::BTN_CLOSE:
			RootGuiLayout::Instance()->showMainControl();
			break;
		case ViewId::BTN_RECEIVE:
			ClientNetwork::Instance()->getSender()->sendRanchOrderOperate(3, m_order.id, 0);
			break;
		case ViewId::BTN_REFRESH:
			if (UICommon::checkMoney(m_order.price, CurrencyType::PLATFORM_MONEY ))
			{
				ClientNetwork::Instance()->getSender()->sendRanchOrderOperate(4, m_order.id, 0);
			}
			break;
		}
		hideOrderOperate();
		return true;
	}

	bool gui_ranchOrder::onItemClick(const UVector2 & pos, const RanchOrderInfo & info)
	{
		if (!isShown())
		{
			return false;
		}
		m_orderOperate->SetVisible(true);
		m_orderOperate->setOrderInfo(info);
		m_orderOperate->setOrderId(m_order.id);
		UVector2 newPos = UVector2(UDim(-1.f, 20.f + pos.d_x.d_offset), UDim(pos.d_y.d_scale, pos.d_y.d_offset + 40.f - 61.5f));
		m_orderOperate->SetPosition(newPos);
		return true;
	}

	bool gui_ranchOrder::onRanchOrderChange()
	{
		m_dataChange = true;
		if (isShown())
		{
			initTabs();
		}
		return true;
	}

	bool gui_ranchOrder::onRadioChange(const EventArgs & args, GUIRadioButton* tab, i32 index)
	{
		if (!UICommon::checkRanchParam())
		{
			return false;
		}
		vector<RanchOrder>::type orders = Blockman::Instance()->m_pPlayer->m_ranch->getOrders();

		if (tab->IsSelected())
		{
			m_selectIndex = index;
			m_order = orders[index];
			switch (m_order.status)
			{
			case 1:
				initOrderInfo();
				initRefreshTime();
				m_btnRefresh->SetVisible(true);
				m_layoutOrderInfo->SetVisible(true);
				m_layoutOrderReward->SetVisible(false);
				m_layoutOrderShipments->SetVisible(false);
				setText("RanchOrder-Refresh-Price", StringUtil::ToString(m_order.price).c_str());
				break;
			case 2:
				initOrderInfo();
				m_btnRefresh->SetVisible(false);
				m_layoutOrderInfo->SetVisible(true);
				m_layoutOrderReward->SetVisible(false);
				m_layoutOrderShipments->SetVisible(false);
				setText("RanchOrder-Refresh-Price", StringUtil::ToString(m_order.price).c_str());
				break;
			case 3:
				initShipments();
				m_btnRefresh->SetVisible(false);
				m_layoutOrderInfo->SetVisible(false);
				m_layoutOrderReward->SetVisible(false);
				m_layoutOrderShipments->SetVisible(true);
				break;
			case 4:
				initOrderReward();
				m_btnRefresh->SetVisible(false);
				m_layoutOrderInfo->SetVisible(false);
				m_layoutOrderReward->SetVisible(true);
				m_layoutOrderShipments->SetVisible(false);
				break;
			}
			m_tvMsg->SetText(getString(m_order.msg));
			hideOrderOperate();
		}
		return true;
	}

	bool gui_ranchOrder::hideOrderOperate()
	{
		m_orderOperate->SetVisible(false);
		return true;
	}

	void gui_ranchOrder::initTabs()
	{

		if (!UICommon::checkRanchParam() || !m_dataChange)
		{
			return;
		}

		m_tabMap.clear();
		m_layoutTabs->CleanupChildren();

		vector<RanchOrder>::type orders = Blockman::Instance()->m_pPlayer->m_ranch->getOrders();
		
		for (int i = 0; i < (int)orders.size(); i++)
		{
			static unsigned count = 0;
			count++;
			String itemLayout = StringUtil::Format("RanchOrder-Panel-OrderList-Tabs-Item-%d", count).c_str();
			if (GUIWindowManager::Instance()->GetGUIWindow(itemLayout.c_str()))
			{
				return;
			}

			GUIRadioButton* tab = (GUIRadioButton*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_RADIO, itemLayout.c_str());
			tab->SetSize(UVector2(UDim(0, 138.f), UDim(0, 206.f)));
			tab->SetPosition(m_tabPositions[i]);
			initTabBack(orders[i].status, tab);
			m_layoutTabs->AddChildWindow(tab);

			String iconStr = itemLayout + "-Icon";
			GUIStaticImage* icon = (GUIStaticImage*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, iconStr.c_str());
			icon->SetArea(URect(UDim(0, 0), UDim(0, 0), UDim(1.f, 0), UDim(1.f, 0)));
			icon->SetImage(orders[i].guestIcon.c_str());
			icon->SetTouchable(false);
			tab->AddChildWindow(icon);

			String hotStr = itemLayout + "-Hot";
			GUIStaticImage* hot = (GUIStaticImage*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, hotStr.c_str());
			hot->SetSize(UVector2(UDim(0.f, 60.f), UDim(0.f, 39.f)));
			hot->SetPosition(UVector2(UDim(0, -10.f), UDim(0, 30.f)));
			hot->SetImage("set:ranch_order.json image:order_hot");
			hot->SetHorizontalAlignment(HA_RIGHT);
			hot->SetVerticalAlignment(VA_CENTRE);
			hot->SetTouchable(false);
			hot->SetVisible(orders[i].isHot);
			tab->AddChildWindow(hot);
			tab->subscribeEvent(EventRadioStateChanged, std::bind(&gui_ranchOrder::onRadioChange, this, std::placeholders::_1, tab, i));

			String redStr = itemLayout + "-RedDot";
			GUIStaticImage* redDot = (GUIStaticImage*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, redStr.c_str());
			redDot->SetSize(UVector2(UDim(0, 20.f), UDim(0, 20.f)));
			redDot->SetPosition(UVector2(UDim(0, -10.f), UDim(0, 20.f)));
			redDot->SetHorizontalAlignment(HA_RIGHT);
			redDot->SetTouchable(false);
			redDot->SetEffectName("reddot.effect");
			tab->AddChildWindow(redDot);
			m_tabMap[i] = tab;
		}

		if (m_tabMap[m_selectIndex])
		{
			m_tabMap[m_selectIndex]->SetSelected(true);
		}
		else if (m_tabMap[0])
		{
			m_tabMap[0]->SetSelected(true);
		}
		else
		{
			m_tabMap.clear();
			m_selectIndex = 0;
		}
		m_dataChange = false;
	}

	void gui_ranchOrder::initTabBack(i32 status, GUIRadioButton* tab)
	{
		switch (status)
		{
		case 1:
			tab->SetNormalImage("set:ranch_order.json image:order_unreceived_nor");
			tab->SetPushedImage("set:ranch_order.json image:order_unreceived_pre");
			break;
		case 2:
		case 3:
			tab->SetNormalImage("set:ranch_order.json image:order_doing_nor");
			tab->SetPushedImage("set:ranch_order.json image:order_doing_pre");
			break;
		case 4:
			tab->SetNormalImage("set:ranch_order.json image:order_done_nor");
			tab->SetPushedImage("set:ranch_order.json image:order_done_pre");
			break;
		}
	}

	void gui_ranchOrder::initOrderInfo()
	{
		m_gvOrderInfoGrid->RemoveAllItems();
		m_gvOrderInfoGrid->ResetPos();
		for (auto item : m_order.infos)
		{
			static unsigned count = 0;
			count++;
			String itemLayout = StringUtil::Format("RanchOrder-Panel-OrderInfo-Grid-Item-%d", count).c_str();
			if (GUIWindowManager::Instance()->GetGUIWindow(itemLayout.c_str()))
			{
				return;
			}
			GuiRanchOrderItem* orderItem = (GuiRanchOrderItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_RANCH_ORDER_ITEM, itemLayout.c_str());
			orderItem->setOrderInfo(item);
			orderItem->setOrderId(m_order.id);
			orderItem->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(0.0f, 102.f), UDim(0.0f, 90.f));
			m_gvOrderInfoGrid->AddItem(orderItem);
		}
	}

	void gui_ranchOrder::initShipments()
	{
		String h = StringUtil::ToString(m_order.timeLeft / 1000 / 60 / 60);
		h = (h.length() == 1 ? ("0" + h) : h);
		String m = StringUtil::ToString(m_order.timeLeft / 1000 / 60 % 60);
		m = (m.length() == 1 ? ("0" + m) : m);
		String s = StringUtil::ToString(m_order.timeLeft / 1000 % 60);
		s = (s.length() == 1 ? ("0" + s) : s);
		m_tvShipmentsLeftTime->SetText(StringUtil::Format("%s:%s:%s", h.c_str(), m.c_str(), s.c_str()).c_str());
	}

	void gui_ranchOrder::initOrderReward()
	{
		m_gvOrderRewardGrid->RemoveAllItems();
		m_gvOrderInfoGrid->ResetPos();
		for (auto item : m_order.rewards)
		{
			static unsigned count = 0;
			count++;
			String itemLayout = StringUtil::Format("RanchOrder-Panel-OrderReward-Grid-Item-%d", count).c_str();
			if (GUIWindowManager::Instance()->GetGUIWindow(itemLayout.c_str()))
			{
				return;
			}
			GuiRanchItem* ranchItemView = (GuiRanchItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_RANCH_ITEM, itemLayout.c_str());
			ranchItemView->initOrderReward();
			ranchItemView->setNum(StringUtil::Format("x%d", item.num).c_str());
			auto ranchItem = RanchSetting::getRanchItem(item.itemId);
			ranchItemView->setIcon(ranchItem ? ranchItem->icon.c_str() : "");
			ranchItemView->setHelper(item.userId);
			ranchItemView->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(0.0f, 102.f), UDim(0.0f, 60.f));
			m_gvOrderRewardGrid->AddItem(ranchItemView);
		}
	}

	void gui_ranchOrder::initRefreshTime()
	{
		m_order.price = Blockman::Instance()->m_pPlayer->m_ranch->getTimePrice(i32(m_order.refreshTimeLeft / 1000));
		if (m_order.refreshTimeLeft == 0)
		{
			m_tvRefreshLeftTime->SetText("");
		}
		else 
		{
			String h = StringUtil::ToString(m_order.refreshTimeLeft / 1000 / 60 / 60);
			h = (h.length() == 1 ? ("0" + h) : h);
			String m = StringUtil::ToString(m_order.refreshTimeLeft / 1000 / 60 % 60);
			m = (m.length() == 1 ? ("0" + m) : m);
			String s = StringUtil::ToString(m_order.refreshTimeLeft / 1000 % 60);
			s = (s.length() == 1 ? ("0" + s) : s);
			m_tvRefreshLeftTime->SetText(StringUtil::Format("%s:%s:%s", h.c_str(), m.c_str(), s.c_str()).c_str());
		}
	}

}
