#include "gui_birdTip.h"
#include "Util/LanguageKey.h"
#include "GUI/RootGuiLayout.h"
#include "Util/ClientEvents.h"
#include "Util/UICommon.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "cWorld/Blockman.h"
#include "Network/ClientNetwork.h"
#include "World/BirdSimulator.h"
#include "game.h"
#include "ShellInterface.h"

using namespace LORD;

namespace BLOCKMAN
{
	gui_birdTip::gui_birdTip()
		: gui_layout("BirdTip.json")
	{
	}

	gui_birdTip::~gui_birdTip()
	{
	}

	void gui_birdTip::onLoad()
	{

		m_btnSure = getWindow<GUIButton>("BirdTip-Btn-Sure");
		m_tvTitle = getWindow<GUIStaticText>("BirdTip-Title-Name");
		m_tvPanelMsg = getWindow<GUIStaticText>("BirdTip-Panel-Msg");
		m_tvPrice = getWindow<GUIStaticText>("BirdTip-Currency-Value");
		m_tvTime = getWindow<GUIStaticText>("BirdTip-TimeTip-Time");
		m_btnTimePrice = getWindow<GUIButton>("BirdTip-TimeTip-Price");

		m_panelLayout = getWindow<GUILayout>("BirdTip-Panel");
		m_moneyTipLayout = getWindow<GUILayout>("BirdTip-MoneyTip");
		m_timeTipLayout = getWindow<GUILayout>("BirdTip-TimeTip");

		m_tvMoneyTipValue = getWindow<GUIStaticText>("BirdTip-MoneyTip-Value");

		m_btnSure->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_SURE));
		setText("BirdTip-Btn-Cancel", LanguageKey::GUI_DIALOG_TIP_BTN_CANCEL);

		getWindow<GUIButton>("BirdTip-Btn-Close")->subscribeEvent(EventButtonClick, std::bind(&gui_birdTip::onClick, this, std::placeholders::_1, ViewId::CLOSE));
		getWindow<GUIButton>("BirdTip-Btn-Cancel")->subscribeEvent(EventButtonClick, std::bind(&gui_birdTip::onClick, this, std::placeholders::_1, ViewId::CANCEL));
		getWindow<GUIButton>("BirdTip-Btn-Sure")->subscribeEvent(EventButtonClick, std::bind(&gui_birdTip::onClick, this, std::placeholders::_1, ViewId::SURE));
		m_subscriptionGuard.add(ShowBirdCommonTipEvent::subscribe(std::bind(&gui_birdTip::showCommonTip, this, std::placeholders::_1, BirdTipType::COMMON)));
		m_subscriptionGuard.add(BirdBuyPermissionEvent::subscribe(std::bind(&gui_birdTip::showCommonTip, this, std::placeholders::_1, BirdTipType::BIRD_BUY_PERMISSIO)));
		m_subscriptionGuard.add(ShowBirdLockOfMoneyEvent::subscribe(std::bind(&gui_birdTip::showLockOfMoneyTip, this, std::placeholders::_1, std::placeholders::_2 )));
		m_subscriptionGuard.add(ShowBirdUnlockNestTipEvent::subscribe(std::bind(&gui_birdTip::showExpenditureTip, this, std::placeholders::_1, std::placeholders::_2, BirdTipType::BIRD_UNLOCK_NEST)));
		m_subscriptionGuard.add(ShowBirdExpandCarryTipEvent::subscribe(std::bind(&gui_birdTip::showExpenditureTip, this, std::placeholders::_1, std::placeholders::_2, BirdTipType::BIRD_EXPAND_CARRY)));
		m_subscriptionGuard.add(ShowBirdCarryFullTipEvent::subscribe(std::bind(&gui_birdTip::showExpenditureTip, this, std::placeholders::_1, std::placeholders::_2, BirdTipType::BIRD_CARRY_FULL)));
		m_subscriptionGuard.add(ShowBirdCapacityFullTipEvent::subscribe(std::bind(&gui_birdTip::showExpenditureTip, this, std::placeholders::_1, std::placeholders::_2, BirdTipType::BIRD_CAPACITY_FULL)));
		m_subscriptionGuard.add(ShowBirdExpandCapacityTipEvent::subscribe(std::bind(&gui_birdTip::showExpenditureTip, this, std::placeholders::_1, std::placeholders::_2, BirdTipType::BIRD_EXPAND_CAPACITY)));
		m_subscriptionGuard.add(ShowBirdLotteryTipEvent::subscribe(std::bind(&gui_birdTip::showExpenditureTip, this, std::placeholders::_1, std::placeholders::_2, BirdTipType::BIRD_LOTTERY)));
		m_subscriptionGuard.add(ShowBirdOpenChestTipEvent::subscribe(std::bind(&gui_birdTip::showOpenChestTip, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		m_subscriptionGuard.add(ShowBirdDeleteTipEvent::subscribe(std::bind(&gui_birdTip::showDeleteTip, this, std::placeholders::_1, std::placeholders::_2)));
	
	}

	void gui_birdTip::onUpdate(ui32 nTimeElapse)
	{
		if (isShown() && m_tipType == BirdTipType::BIRD_OPEN_CHEST && m_leftTime > 0)
		{
			m_leftTime = (m_leftTime - nTimeElapse > 0) ? m_leftTime - nTimeElapse : 0;
			m_tvTime->SetText(UICommon::timeFormat(m_leftTime).c_str());
			m_price = UICommon::checkBirdParam() ? Blockman::Instance()->m_pPlayer->m_birdSimulator->getTimePrice(m_leftTime) : 0;
			m_btnTimePrice->SetText(StringUtil::ToString(m_price).c_str());
			if (m_leftTime == 0)
			{
				RootGuiLayout::Instance()->setBirdTipVisible(false);
			}
		}
	}

	bool gui_birdTip::onClick(const EventArgs & events, ViewId viewId)
	{
		switch (viewId) {
		case ViewId::CLOSE:
		case ViewId::CANCEL:
			RootGuiLayout::Instance()->setBirdTipVisible(false);
			break;
		case ViewId::SURE:
			btnSure();
			break;
		}
		return false;
	}

	void gui_birdTip::btnSure()
	{
		switch (m_tipType)
		{
		case BirdTipType::GO_APP_SHOP:
			ShowBirdPersonalShopEvent::emit(6);
			break;
		case BirdTipType::BIRD_BUY_PERMISSIO:
			ShowBirdPersonalShopEvent::emit(1);
			break;
		case BirdTipType::BACK_APP:
			GameClient::CGame::Instance()->getShellInterface()->onRecharge(1);
			RootGuiLayout::Instance()->setBirdTipVisible(false);
			break;
		case BirdTipType::BIRD_UNLOCK_NEST:
			RootGuiLayout::Instance()->setBirdTipVisible(false);
			break;

		case BirdTipType::BIRD_CARRY_FULL:
		case BirdTipType::BIRD_EXPAND_CARRY:
			if (UICommon::checkMoney(m_price, m_currencyType))
			{
				ClientNetwork::Instance()->getSender()->sendBirdBagOperation(0, 4);
				RootGuiLayout::Instance()->setBirdTipVisible(false);
			}
			break;
		case BirdTipType::BIRD_CAPACITY_FULL:
		case BirdTipType::BIRD_EXPAND_CAPACITY:
			if (UICommon::checkMoney(m_price, m_currencyType))
			{
				ClientNetwork::Instance()->getSender()->sendBirdBagOperation(0, 5);
				RootGuiLayout::Instance()->setBirdTipVisible(false);
			}
			break;
		case BirdTipType::BIRD_LOTTERY:
			RootGuiLayout::Instance()->setBirdTipVisible(false);
			break;
		case BirdTipType::BIRD_OPEN_CHEST:
			if (m_entityId > 0 && UICommon::checkMoney(m_price, m_currencyType))
			{
				ClientNetwork::Instance()->getSender()->sendBirdOpenTreasureChest(m_entityId);
				RootGuiLayout::Instance()->setBirdTipVisible(false);
			}
			break;
		case BirdTipType::BIRD_DELETE:
			if (m_birdId > 0)
			{
				ClientNetwork::Instance()->getSender()->sendBirdBagOperation(m_birdId, 3);
			}
			RootGuiLayout::Instance()->setBirdTipVisible(false);
			break;
		default:
			RootGuiLayout::Instance()->setBirdTipVisible(false);
			break;
		}
	}

	bool gui_birdTip::showCommonTip(const String & msg, BirdTipType tipType)
	{
		m_tipType = tipType;
		m_panelLayout->SetVisible(true);
		m_moneyTipLayout->SetVisible(false);
		m_timeTipLayout->SetVisible(false);
		m_tvPanelMsg->SetText(getString(msg));
		m_tvTitle->SetText(getString(LanguageKey::GUI_DIALOG_TIP_TITLE_TIP));
		m_btnSure->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_SURE));
		RootGuiLayout::Instance()->setBirdTipVisible(true);
		return true;
	}

	
	bool gui_birdTip::showLockOfMoneyTip(i32 difference, i32 currencyType)
	{
		m_currencyType = (CurrencyType)currencyType;
		m_panelLayout->SetVisible(false);
		m_timeTipLayout->SetVisible(false);
		m_moneyTipLayout->SetVisible(true);
		setText(m_tvTitle, LanguageKey::LACK_OF_MONEY);
		setText(m_moneyTipLayout->GetChildByIndex(0), "gui_tip_need_currency");
		m_tvMoneyTipValue->SetTextColor(Color::RED);
		setText(m_tvMoneyTipValue, StringUtil::ToString(difference));
		GUIStaticImage* currencyIcon = getWindow<GUIStaticImage>("BirdTip-MoneyTip-Icon");
		currencyIcon->SetImage(UICommon::getCurrencyIconByType(m_currencyType).c_str());
		if (m_currencyType == CurrencyType::PLATFORM_MONEY)
		{
			showGotoApp();
		}
		else
		{
			showGotoShop();
		}
		RootGuiLayout::Instance()->setBirdTipVisible(true);
		return true;
	}

	bool gui_birdTip::showExpenditureTip(i32 price, i32 currencyType, BirdTipType tipType)
	{
		m_tipType = tipType;
		m_price = price;
		m_currencyType = (CurrencyType)currencyType;
		m_panelLayout->SetVisible(false);
		m_timeTipLayout->SetVisible(false);
		m_moneyTipLayout->SetVisible(true);
		setText(m_moneyTipLayout->GetChildByIndex(0), "gui_bird_tip_expenditure");
		changeTitleAndMessage();
		m_tvMoneyTipValue->SetTextColor(Color::WHITE);
		setText(m_tvMoneyTipValue, StringUtil::ToString(price));
		setText(m_btnSure, LanguageKey::GUI_DIALOG_TIP_BTN_SURE);
		GUIStaticImage* currencyIcon = getWindow<GUIStaticImage>("BirdTip-MoneyTip-Icon");
		currencyIcon->SetImage(UICommon::getCurrencyIconByType(m_currencyType).c_str());
		RootGuiLayout::Instance()->setBirdTipVisible(true);
		return false;
	}

	bool gui_birdTip::showOpenChestTip(i32 entityId, i32 currencyType, i32 timeLeft)
	{
		m_currencyType = (CurrencyType)currencyType;
		m_leftTime = timeLeft;
		m_entityId = entityId;
		m_tipType = BirdTipType::BIRD_OPEN_CHEST;
		m_panelLayout->SetVisible(false);
		m_moneyTipLayout->SetVisible(false);
		m_timeTipLayout->SetVisible(true);
		m_btnTimePrice->SetNormalImage(UICommon::getCurrencyIconByType(m_currencyType).c_str());
		m_btnTimePrice->SetPushedImage(UICommon::getCurrencyIconByType(m_currencyType).c_str());
		setText("BirdTip-TimeTip-Msg", "gui_bird_tip_use_cube_open_chest");
		m_tvTitle->SetText(getString(LanguageKey::GUI_DIALOG_TIP_TITLE_TIP));
		m_btnSure->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_SURE));
		RootGuiLayout::Instance()->setBirdTipVisible(true);
		return false;
	}

	bool gui_birdTip::showDeleteTip(i64 birdId, const String& birdName)
	{
		m_birdId = birdId;
		m_tipType = BirdTipType::BIRD_DELETE;
		m_panelLayout->SetVisible(true);
		m_moneyTipLayout->SetVisible(false);
		m_timeTipLayout->SetVisible(false);
		m_tvPanelMsg->SetText(StringUtil::Format(getString("gui_bird_tip_delete_bird").c_str(), getString(birdName).c_str()).c_str());
		m_tvTitle->SetText(getString(LanguageKey::GUI_DIALOG_TIP_TITLE_TIP));
		m_btnSure->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_SURE));
		RootGuiLayout::Instance()->setBirdTipVisible(true);
		return true;
	}

	bool gui_birdTip::showGotoShop()
	{
		m_tipType = BirdTipType::GO_APP_SHOP;
		setText(m_btnSure, LanguageKey::GUI_DIALOG_TIP_BTN_SURE);
		setText(m_moneyTipLayout->GetChildByIndex(1), "gui_ranch_tip_lack_of_money_go_app_shop");
		return false;
	}

	bool gui_birdTip::showGotoApp()
	{
		m_tipType = BirdTipType::BACK_APP;
		setText(m_btnSure, LanguageKey::GUI_BACK_PLATFORM_TOP_UP);
		setText(m_moneyTipLayout->GetChildByIndex(1), LanguageKey::GUI_NEED_BACK_PLATFORM_TOP_UP);
		return false;
	}

	void gui_birdTip::changeTitleAndMessage()
	{
		switch (m_tipType)
		{
		case BirdTipType::BIRD_UNLOCK_NEST:
			setText(m_tvTitle, LanguageKey::GUI_DIALOG_TIP_TITLE_TIP);
			setText(m_moneyTipLayout->GetChildByIndex(1), "gui_bird_tip_nest");
			break;
		case BirdTipType::BIRD_EXPAND_CARRY:
			setText(m_tvTitle, LanguageKey::GUI_DIALOG_TIP_TITLE_TIP);
			setText(m_moneyTipLayout->GetChildByIndex(1), "gui_bird_tip_expand_carry");
			break;
		case BirdTipType::BIRD_EXPAND_CAPACITY:
			setText(m_tvTitle, LanguageKey::GUI_DIALOG_TIP_TITLE_TIP);
			setText(m_moneyTipLayout->GetChildByIndex(1), "gui_bird_tip_expand_capacity");
			break;
		case BirdTipType::BIRD_LOTTERY:
			setText(m_tvTitle, LanguageKey::GUI_DIALOG_TIP_TITLE_TIP);
			setText(m_moneyTipLayout->GetChildByIndex(1), "gui_bird_tip_lottery");
			break;
		case BirdTipType::BIRD_OPEN_CHEST:
			setText(m_tvTitle, LanguageKey::GUI_DIALOG_TIP_TITLE_TIP);
			setText(m_moneyTipLayout->GetChildByIndex(1), "gui_bird_tip_open_chest");
			break;

		case BirdTipType::BIRD_CARRY_FULL:
			setText(m_tvTitle, "gui_bird_carry_full_expenditure");
			setText(m_moneyTipLayout->GetChildByIndex(1), "gui_bird_carry_full");
			break;
		case BirdTipType::BIRD_CAPACITY_FULL:
			setText(m_tvTitle, "gui_bird_capacity_full_expenditure");
			setText(m_moneyTipLayout->GetChildByIndex(1), "gui_bird_capacity_full");
			break;
		}
	}

}
