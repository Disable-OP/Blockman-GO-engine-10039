#include "gui_ranchTip.h"
#include "Util/LanguageKey.h"
#include "UI/GUIButton.h"
#include "GUI/RootGuiLayout.h"
#include "Util/ClientEvents.h"
#include "GUI/GuiDef.h"
#include "rapidjson/document.h"
#include "GUI/GuiRanchItem.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "game.h"
#include "ShellInterface.h"
#include "Util/UICommon.h"
#include "Setting/RanchSetting.h"

using namespace LORD;

namespace BLOCKMAN
{
	gui_ranchTip::gui_ranchTip()
		: gui_layout("RanchTip.json")
	{
	}

	gui_ranchTip::~gui_ranchTip()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_ranchTip::onLoad()
	{

		m_btnSure = getWindow<GUIButton>("RanchTip-Btn-Sure");
		m_tvTitle = getWindow<GUIStaticText>("RanchTip-Title-Name");
		m_tvPanelMsg = getWindow<GUIStaticText>("RanchTip-Panel-Msg");
		m_tvItemsMsg = getWindow<GUIStaticText>("RanchTip-Items-Msg");
		m_tvPrice = getWindow<GUIStaticText>("RanchTip-Currency-Value");
		m_listView = getWindow<GUIListHorizontalBox>("RanchTip-Items-List");
		m_gridView = getWindow<GUIGridView>("RanchTip-Items-Grid");
		m_currencyIcon = getWindow<GUIStaticImage>("RanchTip-Currency-Icon");

		m_listLayout = getWindow<GUILayout>("RanchTip-Items");
		m_panelLayout = getWindow<GUILayout>("RanchTip-Panel");
		m_lackMoneyLayout = getWindow<GUILayout>("RanchTip-Lack-Money");

		m_listView->SetInterval(10.f);
		m_gridView->InitConfig(40.f, 10.f, 4);
		getWindow<GUIButton>("RanchTip-Btn-Close")->subscribeEvent(EventButtonClick, std::bind(&gui_ranchTip::onClick, this, std::placeholders::_1, ViewId::CLOSE));
		getWindow<GUIButton>("RanchTip-Btn-Sure")->subscribeEvent(EventButtonClick, std::bind(&gui_ranchTip::onClick, this, std::placeholders::_1, ViewId::SURE));
		m_subscriptionGuard.add(ShowRanchUnlockItemsTipEvent::subscribe(std::bind(&gui_ranchTip::showUnlockItems, this, std::placeholders::_1)));
		m_subscriptionGuard.add(ShowRanchCommonTipEvent::subscribe(std::bind(&gui_ranchTip::showCommonTip, this, std::placeholders::_1)));
		m_subscriptionGuard.add(ShowRanchLockItemsEvent::subscribe(std::bind(&gui_ranchTip::showLackItems, this, std::placeholders::_1)));
		m_subscriptionGuard.add(ShowRanchLockOfMoneyEvent::subscribe(std::bind(&gui_ranchTip::showLockOfMoneyTip, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(ShowRanchQueueUnlockEvent::subscribe(std::bind(&gui_ranchTip::showQueueUnlock, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5)));
		m_subscriptionGuard.add(ShowRanchOrderOperateLockItemsEvent::subscribe(std::bind(&gui_ranchTip::showRanchOrderOperateLockItems, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		m_subscriptionGuard.add(ShowRanchLockItemsByBuildOperationEvent::subscribe(std::bind(&gui_ranchTip::showRanchLockItemsByBuildOperation, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		m_subscriptionGuard.add(ShowRanchStorageFullTipEvent::subscribe(std::bind(&gui_ranchTip::showRanchStorageFull, this, std::placeholders::_1)));
		m_subscriptionGuard.add(ShowRanchLockItemsByUpgradeStorageEvent::subscribe(std::bind(&gui_ranchTip::showRanchLockItemsByUpgradeStorage, this, std::placeholders::_1)));
		m_subscriptionGuard.add(ShowRanchLockItemsByOrderHelpEvent::subscribe(std::bind(&gui_ranchTip::showRanchLockItemsByOrderHelp, this, std::placeholders::_1, std::placeholders::_2)));
	}

	void gui_ranchTip::onUpdate(ui32 nTimeElapse)
	{
		m_btnSure->SetRenderOffset(m_currencyIcon->IsVisible() ? Vector2(10, 0) : Vector2(0, 0));
	}

	bool gui_ranchTip::onClick(const EventArgs & events, ViewId viewId)
	{
		switch (viewId) {
		case ViewId::CLOSE:
			RootGuiLayout::Instance()->hideRanchTip();
			break;
		case ViewId::SURE:

			btnSure();
			break;
		}
		return false;
	}

	void gui_ranchTip::btnSure()
	{
		switch (m_tipType)
		{
		case RanchTipType::GO_APP_SHOP:
			RootGuiLayout::Instance()->showAppShop();
			break;
		case RanchTipType::BACK_APP:
			GameClient::CGame::Instance()->getShellInterface()->onRecharge(1);
			break;
		case RanchTipType::BUILD_QUEUE_UNLOCK:
			//1 production 2 speed up 3 unlockQueue
			if (UICommon::checkMoney(m_price, m_currencyType))
			{
				ClientNetwork::Instance()->getSender()->sendRanchBuildQueueOperation(3, m_entityId, m_queueId, 0);
				RootGuiLayout::Instance()->hideRanchTip();
			}
			break;
		case RanchTipType::LACK_ITEMS:
			if (UICommon::checkRanchStorageCapacity(m_items))
			{
				if (UICommon::checkMoney(m_price, m_currencyType))
				{
					ClientNetwork::Instance()->getSender()->sendRanchBuyItems(m_items);
					RootGuiLayout::Instance()->hideRanchTip();
				}
			}
			break;
		case RanchTipType::ORDER_OPERATE_LOCK_ITEMS:
			if (UICommon::checkMoney(m_price, m_currencyType))
			{
				ClientNetwork::Instance()->getSender()->sendUseCubeFinshOrder(m_orderId, m_orderIndex);
				RootGuiLayout::Instance()->hideRanchTip();
			}
			break; 
		case RanchTipType::LOCK_ITEMS_BY_BUILD_OPERATION:
			if (UICommon::checkMoney(m_price, m_currencyType))
			{
				ClientNetwork::Instance()->getSender()->sendRanchBuildQueueOperation(4, m_entityId, 0, m_productId);
				RootGuiLayout::Instance()->hideRanchTip();
			}
			break;
		case RanchTipType::RANCH_STORAGE_FULL:
			RootGuiLayout::Instance()->hideRanchTip();
			ShowRanchStorageUpgradeTipEvent::emit();
			break;
		case RanchTipType::LOCK_ITEMS_BY_UPGRADE_STORAGE:
			if (UICommon::checkMoney(m_price, m_currencyType))
			{
				ClientNetwork::Instance()->getSender()->sendRanchOperation(RanchOperationType::USE_CUBE_UPGRADE_STORAGE);
				RootGuiLayout::Instance()->hideRanchTip();
			}
			break; 
		case RanchTipType::LOCK_ITEMS_BY_ORDER_HELP:
			if (UICommon::checkMoney(m_price, m_currencyType))
			{
				ClientNetwork::Instance()->getSender()->sendRanchHelpFinish(m_orderHelpId, 2);
				RootGuiLayout::Instance()->hideRanchTip();
			}
			break;
		case RanchTipType::COMMON:
			RootGuiLayout::Instance()->hideRanchTip();
			break;
		case RanchTipType::UNLOCK_ITEMS:
		default:
			RootGuiLayout::Instance()->hideRanchTip();
			break;
		}
	}

	bool gui_ranchTip::showUnlockItems(const vector<i32>::type & recipe)
	{
		m_tipType = RanchTipType::UNLOCK_ITEMS;
		m_listLayout->SetVisible(true);
		m_currencyIcon->SetVisible(false);
		m_panelLayout->SetVisible(false);
		m_lackMoneyLayout->SetVisible(false);
		m_btnSure->SetRenderOffset(Vector2(0, 0));
		setText(m_tvItemsMsg, "gui_ranch_upgrade_unlock_items");
		setText(m_tvTitle, LanguageKey::GUI_DIALOG_TIP_TITLE_TIP);
		setText(m_btnSure, LanguageKey::GUI_DIALOG_TIP_BTN_SURE);
		initUnlockItem(recipe);
		RootGuiLayout::Instance()->showRanchTip();
		return true;
	}

	bool gui_ranchTip::showCommonTip(const String & msg)
	{
		m_tipType = RanchTipType::COMMON;
		m_listLayout->SetVisible(false);
		m_panelLayout->SetVisible(true);
		m_lackMoneyLayout->SetVisible(false);
		m_tvPanelMsg->SetText(getString(msg));
		m_tvTitle->SetText(getString(LanguageKey::GUI_DIALOG_TIP_TITLE_TIP));
		m_btnSure->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_SURE));
		m_btnSure->SetRenderOffset(Vector2(0, 0));
		m_currencyIcon->SetVisible(false);
		RootGuiLayout::Instance()->showRanchTip();
		return true;
	}

	bool gui_ranchTip::showQueueUnlock(i32 entityId, i32 queueId, i32 price, i32 currencyType, const String & msg)
	{
		m_tipType = RanchTipType::BUILD_QUEUE_UNLOCK;
		m_currencyType = (CurrencyType)currencyType;
		m_price = price;
		m_entityId = entityId;
		m_queueId = queueId;
		m_listLayout->SetVisible(false);
		m_panelLayout->SetVisible(true);
		m_currencyIcon->SetVisible(true);
		m_lackMoneyLayout->SetVisible(false);
		m_tvPanelMsg->SetText(getString(msg));
		m_tvTitle->SetText(getString("gui_ranch_tip_title_queue"));
		m_btnSure->SetText(getString(LanguageKey::SHOP_BTN_BUY));
		m_tvPrice->SetText(StringUtil::ToString(price).c_str());
		m_btnSure->SetRenderOffset(Vector2(10.f, 0));
		m_currencyIcon->SetImage(UICommon::getCurrencyIconByType(m_currencyType).c_str());
		RootGuiLayout::Instance()->showRanchTip();
		return true;
	}

	bool gui_ranchTip::showLockOfMoneyTip(i32 difference, i32 currencyType)
	{
		m_currencyType = (CurrencyType)currencyType;

		m_listLayout->SetVisible(false);
		m_panelLayout->SetVisible(false);
		m_currencyIcon->SetVisible(false);
		m_lackMoneyLayout->SetVisible(true);
		setText(m_tvTitle, LanguageKey::LACK_OF_MONEY);
		setText(m_lackMoneyLayout->GetChildByIndex(0), "gui_tip_need_currency");
		setText(m_lackMoneyLayout->GetChildByIndex(0)->GetChildByIndex(1), StringUtil::ToString(difference));
		GUIStaticImage* currencyIcon = getWindow<GUIStaticImage>("RanchTip-Panel-Lack-Money-Icon");
		currencyIcon->SetImage(UICommon::getCurrencyIconByType(m_currencyType).c_str());
		m_btnSure->SetRenderOffset(Vector2(0.f, 0));
		if (m_currencyType == CurrencyType::PLATFORM_MONEY)
		{
			showGotoApp();
		}
		else
		{
			showGotoShop();
		}
		RootGuiLayout::Instance()->showRanchTip();
		return true;
	}

	bool gui_ranchTip::showLackItems(const vector<RanchCommon>::type & recipe)
	{
		m_items = recipe;
		m_tipType = RanchTipType::LACK_ITEMS;
		m_currencyType = CurrencyType::PLATFORM_MONEY;
		m_listLayout->SetVisible(true);
		m_currencyIcon->SetVisible(true);
		m_panelLayout->SetVisible(false);
		m_lackMoneyLayout->SetVisible(false);
		m_btnSure->SetRenderOffset(Vector2(20.f, 0));
		setText(m_tvTitle, "gui_ranch_lack_items");
		setText(m_tvItemsMsg, "gui_use_platform_money_buy_items");
		setText(m_btnSure, "gui_ranch_btn_do_go");
		i32 total = 0;
		for (auto item : recipe)
		{
			total += RanchSetting::getRanchItemCubePrice(item.itemId) * item.num;
		}
		m_price = total;
		setText(m_tvPrice, StringUtil::ToString(total));
		m_currencyIcon->SetImage(UICommon::getCurrencyIconByType(m_currencyType).c_str());
		initRecipeItem(recipe);
		RootGuiLayout::Instance()->showRanchTip();
		return true;
	}

	bool gui_ranchTip::showRanchOrderOperateLockItems(const vector<RanchCommon>::type & recipe, i32 orderId, i32 index)
	{
		showLackItems(recipe);
		m_orderId = orderId;
		m_orderIndex = index;
		m_tipType = RanchTipType::ORDER_OPERATE_LOCK_ITEMS;
		setText(m_tvItemsMsg, "gui_use_platform_money_buy_items_for_task");
		return false;
	}

	bool gui_ranchTip::showRanchLockItemsByBuildOperation(const vector<RanchCommon>::type & recipe, i32 entityId, i32 productId)
	{
		showLackItems(recipe);
		m_entityId = entityId;
		m_productId = productId;
		m_tipType = RanchTipType::LOCK_ITEMS_BY_BUILD_OPERATION;
		setText(m_tvItemsMsg, "gui_use_platform_money_buy_items_for_production");
		return false;
	}

	bool gui_ranchTip::showRanchLockItemsByUpgradeStorage(const vector<RanchCommon>::type & recipe)
	{
		showLackItems(recipe);
		m_tipType = RanchTipType::LOCK_ITEMS_BY_UPGRADE_STORAGE;
		setText(m_tvItemsMsg, "gui_use_platform_money_buy_items_for_upgrade_storage");
		return true;
	}

	bool gui_ranchTip::showRanchLockItemsByOrderHelp(const vector<RanchCommon>::type & recipe, i64 helpId)
	{
		showLackItems(recipe);
		m_orderHelpId = helpId;
		m_tipType = RanchTipType::LOCK_ITEMS_BY_ORDER_HELP;
		setText(m_tvItemsMsg, "gui_use_platform_money_buy_items_for_task");
		return true;
	}

	bool gui_ranchTip::showRanchStorageFull(const String & msg)
	{
		showCommonTip(msg);
		m_tipType = RanchTipType::RANCH_STORAGE_FULL;
		return true;
	}

	bool gui_ranchTip::showGotoShop()
	{
		m_tipType = RanchTipType::GO_APP_SHOP;
		setText(m_btnSure, LanguageKey::GUI_DIALOG_TIP_BTN_SURE);
		setText(m_lackMoneyLayout->GetChildByIndex(1), "gui_ranch_tip_lack_of_money_go_app_shop");
		return false;
	}

	bool gui_ranchTip::showGotoApp()
	{
		m_tipType = RanchTipType::BACK_APP;
		setText(m_btnSure, LanguageKey::GUI_BACK_PLATFORM_TOP_UP);
		setText(m_lackMoneyLayout->GetChildByIndex(1), LanguageKey::GUI_NEED_BACK_PLATFORM_TOP_UP);
		return false;
	}

	void gui_ranchTip::initRecipeItem(const vector<RanchCommon>::type& items)
	{
		m_listView->SetVisible(true);
		m_gridView->SetVisible(false);
		m_listView->ClearAllItem();
		for (auto item : items)
		{
			static unsigned count = 0;
			count++;
			String itemLayout = StringUtil::Format("RanchTip-Items-List-Item-%d", count).c_str();
			if (GUIWindowManager::Instance()->GetGUIWindow(itemLayout.c_str()))
			{
				return;
			}
			GuiRanchItem* ranchItemView = (GuiRanchItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_RANCH_ITEM, itemLayout.c_str());
			ranchItemView->setNum(StringUtil::Format("+%d", item.num).c_str());
			auto ranchItem = RanchSetting::getRanchItem(item.itemId);
			ranchItemView->setIcon(ranchItem ? ranchItem->icon.c_str() : "");
			ranchItemView->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(0.0f, 80.f), UDim(0.0f, 110.f));
			m_listView->AddItem(ranchItemView, false);
		}
		m_listView->setContainerAlignment();
	}

	void gui_ranchTip::initUnlockItem(const vector<i32>::type & items)
	{
		m_listView->SetVisible(false);
		m_gridView->SetVisible(true);
		m_gridView->RemoveAllItems();
		for (auto itemId : items)
		{
			static unsigned count = 0;
			count++;
			String itemLayout = StringUtil::Format("RanchTip-Items-Grid-Item-%d", count).c_str();
			if (GUIWindowManager::Instance()->GetGUIWindow(itemLayout.c_str()))
			{
				return;
			}
			float y = (m_gridView->GetPixelSize().y - 10.f) / 2;
			GuiRanchItem* ranchItemView = (GuiRanchItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_RANCH_ITEM, itemLayout.c_str());
			ranchItemView->setTextScale(0.8f);
			ranchItemView->setIcon(RanchSetting::getRanchItemIcon(itemId).c_str());
			ranchItemView->setNum(getString(RanchSetting::getRanchItemName(itemId)).c_str());
			ranchItemView->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(0.0f, y - 30.f), UDim(0.0f, y));
			m_gridView->AddItem(ranchItemView);
		}
		float x = ((m_gridView->GetPixelSize().y - 10.f) / 2 - 30) * 4.f+ 3 * 40.f;
		m_gridView->SetWidth(UDim(0.f, x));
	}

}
