#include "gui_tipDialog.h"
#include "game.h"
#include "Util/LanguageKey.h"
#include "Setting/CarSetting.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "UI/GUIButton.h"
#include "Entity/EntityMerchant.h"
#include "Entity/EntityItem.h"
#include "Util/LanguageManager.h"
#include "Item/Item.h"
#include "Item/ItemStack.h"
#include "GuiItemStack.h"
#include "Inventory/CoinManager.h"
#include "ShellInterface.h"
#include "Object/Root.h"
#include "Util/UICommon.h"
#include "Inventory/InventoryPlayer.h"
#include "Setting/GameTypeSetting.h"

using namespace LORD;

namespace BLOCKMAN
{

	gui_tipDialog::gui_tipDialog() :
		gui_layout("TipDialog.json")
	{

	}


	gui_tipDialog::~gui_tipDialog()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_tipDialog::onLoad()
	{
		switch (GameClient::CGame::Instance()->GetGameType())
		{
		case ClientGameType::PixelGunHall:
		case ClientGameType::PixelGunGameTeam:
		case ClientGameType::PixelGunGamePerson:
		case ClientGameType::PixelGunGame1v1:
			getWindow("TipDialog-Panel")->SetVisible(false);
			m_titleText = getWindow<GUIStaticText>("TipDialog-Pixel-Gun-Title-Name");
			m_messageText = getWindow<GUIStaticText>("TipDialog-Pixel-Gun-Content-Vehicle-Message");
			m_valueText = getWindow<GUIStaticText>("TipDialog-Pixel-Gun-Content-Currency-Value");
			m_otherMsgText = getWindow<GUIStaticText>("TipDialog-Pixel-Gun-Content-Other-Message");
			m_iconImage = getWindow<GUIStaticImage>("TipDialog-Pixel-Gun-Content-Currency-Icon");
			m_cancelBtn = getWindow<GUIButton>("TipDialog-Pixel-Gun-Btn-Cancel");
			m_sureBtn = getWindow<GUIButton>("TipDialog-Pixel-Gun-Btn-Sure");
			m_contentWindow = getWindow("TipDialog-Pixel-Gun-Content-Vehicle");
			m_otherContentWindow = getWindow("TipDialog-Pixel-Gun-Content-Other");
			m_otherLackOfMoneyWindow = getWindow("TipDialog-Pixel-Gun-Content-LackOfMoney");
			m_lackOfMoneyTip = getWindow<GUIStaticText>("TipDialog-Pixel-Gun-Content-LackOfMoney-Currency-Tip");
			m_lackOfMoneyIcon = getWindow<GUIStaticImage>("TipDialog-Pixel-Gun-Content-LackOfMoney-Currency-Icon");
			m_lackOfMoneyValue = getWindow<GUIStaticText>("TipDialog-Pixel-Gun-Content-LackOfMoney-Currency-Value");
			m_lackOfMoneyMsg = getWindow<GUIStaticText>("TipDialog-Pixel-Gun-Content-LackOfMoney-Message");
			getWindow<GUIButton>("TipDialog-Pixel-Gun-Title-Btn-Close")->SetVisible(false);
			getWindow<GUIButton>("TipDialog-Close")->subscribeEvent(EventButtonClick, std::bind(&gui_tipDialog::onBtnClose, this, std::placeholders::_1));
			break;
		default:
			getWindow("TipDialog-Pixel-Gun-Panel")->SetVisible(false);
			m_titleText = getWindow<GUIStaticText>("TipDialog-Title-Name");
			m_messageText = getWindow<GUIStaticText>("TipDialog-Content-Vehicle-Message");
			m_valueText = getWindow<GUIStaticText>("TipDialog-Content-Currency-Value");
			m_otherMsgText = getWindow<GUIStaticText>("TipDialog-Content-Other-Message");
			m_iconImage = getWindow<GUIStaticImage>("TipDialog-Content-Currency-Icon");
			m_cancelBtn = getWindow<GUIButton>("TipDialog-Btn-Cancel");
			m_sureBtn = getWindow<GUIButton>("TipDialog-Btn-Sure");
			m_contentWindow = getWindow("TipDialog-Content-Vehicle");
			m_otherContentWindow = getWindow("TipDialog-Content-Other");
			m_otherLackOfMoneyWindow = getWindow("TipDialog-Content-LackOfMoney");
			m_lackOfMoneyTip = getWindow<GUIStaticText>("TipDialog-Content-LackOfMoney-Currency-Tip");
			m_lackOfMoneyIcon = getWindow<GUIStaticImage>("TipDialog-Content-LackOfMoney-Currency-Icon");
			m_lackOfMoneyValue = getWindow<GUIStaticText>("TipDialog-Content-LackOfMoney-Currency-Value");
			m_lackOfMoneyMsg = getWindow<GUIStaticText>("TipDialog-Content-LackOfMoney-Message");
			auto btnClose = getWindow<GUIButton>("TipDialog-Title-Btn-Close");
			btnClose->subscribeEvent(EventButtonClick, std::bind(&gui_tipDialog::onBtnClose, this, std::placeholders::_1));
			break;
		}

		m_cancelBtn->subscribeEvent(EventButtonClick, std::bind(&gui_tipDialog::onBtnClick, this, std::placeholders::_1, VIewId::BTN_CANCEL));
		m_sureBtn->subscribeEvent(EventButtonClick, std::bind(&gui_tipDialog::onBtnClick, this, std::placeholders::_1, VIewId::BTN_SURE));

		m_subscriptionGuard.add(ShowVehicleTipDialogEvent::subscribe(std::bind(&gui_tipDialog::showVehicleTipDialog, this, std::placeholders::_1)));
		m_subscriptionGuard.add(showUnlockCommodityTipDialogEvent::subscribe(std::bind(&gui_tipDialog::showUnlockCommodityTipDialog, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6)));
		m_subscriptionGuard.add(ShowGoAppShopTipDialogEvent::subscribe(std::bind(&gui_tipDialog::showLackOfMoneyGoAppShop, this)));
		m_subscriptionGuard.add(ShowReviveEvent::subscribe(std::bind(&gui_tipDialog::showRevive, this,
			std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5)));
		m_subscriptionGuard.add(ShowGoNpcMerchantTipDialogEvent::subscribe(std::bind(&gui_tipDialog::showGoNpcMerchantTipDialog, this,
			std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5)));
		m_subscriptionGuard.add(SyncChangePlayerActortEvent::subscribe(std::bind(&gui_tipDialog::showChangePlayerActortTip, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		m_subscriptionGuard.add(ShowPickUpItemTipDialogEvent::subscribe(std::bind(&gui_tipDialog::showPickUpItemTipDialog, this, 
			std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)));
		m_subscriptionGuard.add(ShowResultTipDialogEvent::subscribe(std::bind(&gui_tipDialog::showResultTipDialog, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(ShowGetSellManorTipDialogEvent::subscribe(std::bind(&gui_tipDialog::showGetOrSellManor, this)));
		m_subscriptionGuard.add(ShowGetManorTipDialogEvent::subscribe(std::bind(&gui_tipDialog::showGetManor, this)));
		m_subscriptionGuard.add(ShowSellManorTipDialogEvent::subscribe(std::bind(&gui_tipDialog::showSellManor, this)));
		m_subscriptionGuard.add(ShowLackOfMoneyTipDialogEvent::subscribe(std::bind(&gui_tipDialog::showLackOfMoney, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(ShowUpgradeResourceEvent::subscribe(std::bind(&gui_tipDialog::showUpgradeResourceTip, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)));
		m_subscriptionGuard.add(ShowCustomTipMsgEvent::subscribe(std::bind(&gui_tipDialog::showCustomTipMsg, this, std::placeholders::_1,std::placeholders::_2)));
		m_subscriptionGuard.add(ShowManorBuildTipDialogEvent::subscribe(std::bind(&gui_tipDialog::showManorBuildHouse, this)));
		m_subscriptionGuard.add(ShowBuildWarGuessTipDialogEvent::subscribe(std::bind(&gui_tipDialog::showBuildWarGuessTip, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(ShowConsumeCoinTipEvent::subscribe(std::bind(&gui_tipDialog::showConsumeCoinTipMsg, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)));
		m_subscriptionGuard.add(ShowRanchExBeginTipDialogEvent::subscribe(std::bind(&gui_tipDialog::showRanchExBeginTip, this)));
		m_subscriptionGuard.add(ShowRanchExTaskFinishTipDialogEvent::subscribe(std::bind(&gui_tipDialog::showRanchExTaskFinish, this)));
		m_subscriptionGuard.add(ShowKeepItemTipEvent::subscribe(std::bind(&gui_tipDialog::showKeepItemTip, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		m_subscriptionGuard.add(ShowEnchantMentQuickEvent::subscribe(std::bind(&gui_tipDialog::showEnchantmentQuickTip, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		m_subscriptionGuard.add(ShowPixelGunHallNotOpenEvent::subscribe(std::bind(&gui_tipDialog::showPixelGunHallNotOpen, this, std::placeholders::_1)));
		m_subscriptionGuard.add(ShowPixelGunHallLvNotEnoughEvent::subscribe(std::bind(&gui_tipDialog::showPixelGunHallLvNotEnough, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(ShowPixelGunHallLvMapLockDiamondEvent::subscribe(std::bind(&gui_tipDialog::showPixelGunHallMapLockDiamond, this, std::placeholders::_1, std::placeholders::_2)));
	}

	void gui_tipDialog::onShow()
	{

	}

	void gui_tipDialog::refreshUpgradeResourceUI()
	{
		ItemStack* itemStack = NULL;
		ItemStackPtr m_itemStack = NULL;
		m_titleText->SetText(getString(LanguageKey::GUI_SHOP_TAB_UPGRADE).c_str());
		m_messageText->SetText(m_tipMessage.c_str());

		if (m_vehicleId < 0)
		{
			m_contentWindow->SetVisible(false);
			m_otherContentWindow->SetVisible(true);
			m_cancelBtn->SetVisible(false);
			m_sureBtn->SetVisible(false);
			m_otherMsgText->SetText(m_tipMessage.c_str());
			return;
		}

		m_contentWindow->SetVisible(true);
		m_otherContentWindow->SetVisible(false);
		m_cancelBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_CANCEL));
		m_valueText->SetText(StringUtil::ToString(m_vehiclePrice).c_str());
		itemStack = LordNew ItemStack(m_vehicleId, 1, 0);
		m_itemStack = itemStack->copy();
		GuiItemStack::showItemIcon(m_iconImage, m_itemStack, m_vehicleId, 0);
		LordDelete itemStack;
		m_contentWindow->SetVisible(true);
		m_otherContentWindow->SetVisible(false);
		m_sureBtn->SetEnabled(false);
		m_sureBtn->SetText(getString(LanguageKey::LACK_OF_MONEY));
		if (Blockman::Instance() && Blockman::Instance()->m_pPlayer)
		{
			Wallet * wallet = Blockman::Instance()->m_pPlayer->m_wallet;
			int coinId = CoinManager::Instance()->itemIdToCoinId(m_vehicleId);
			if (wallet && wallet->getCoinNum(coinId) >= m_vehiclePrice)
			{
				m_sureBtn->SetEnabled(true);
				m_sureBtn->SetText(getString(LanguageKey::GUI_SHOP_TAB_UPGRADE));
			}
		}
	}

	void gui_tipDialog::upgradeResource()
	{
		ClientNetwork::Instance()->getSender()->sendUpgradeResource(resourceId);
	}

	void gui_tipDialog::refreshUI()
	{
		
		String msg;

		m_cancelBtn->SetVisible(true);
		m_sureBtn->SetVisible(true);
		switch (m_showTipType)
		{
		case TipType::VEHICLE_TIP:
			m_titleText->SetText(getString(LanguageKey::GUI_DIALOG_TIP_TITLE_VEHICLE));
			m_cancelBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_CANCEL));
			m_sureBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_UNLOCK));
			m_valueText->SetText(StringUtil::ToString(m_vehiclePrice).c_str());
			m_messageText->SetText(m_tipMessage.c_str());
			m_contentWindow->SetVisible(true);
			m_otherContentWindow->SetVisible(false);
			m_otherLackOfMoneyWindow->SetVisible(false);
			m_sureBtn->SetEnabled(true);
			m_iconImage->SetImage(UICommon::getCurrencyIcon().c_str());
			if (Blockman::Instance() && Blockman::Instance()->m_pPlayer)
			{
				Wallet * wallet = Blockman::Instance()->m_pPlayer->m_wallet;
				if (wallet && wallet->getCurrency() >= m_vehiclePrice)
				{
					m_sureBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_UNLOCK));
				}
				else
				{
					m_sureBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_GET_CURRENCY));
				}
			}
			break;

		case TipType::UNLOCK_COMMODITY:
			m_titleText->SetText(getString(LanguageKey::GUI_DIALOG_TIP_TITLE_TIP));
			m_cancelBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_CANCEL));
			m_sureBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_SURE));
			m_cancelBtn->SetEnabled(true);
			m_sureBtn->SetEnabled(true);
			m_messageText->SetText(m_tipMessage.c_str());
			m_contentWindow->SetVisible(true);
			m_otherContentWindow->SetVisible(false);
			m_otherLackOfMoneyWindow->SetVisible(false);
			m_valueText->SetText(StringUtil::ToString(m_unlockCost).c_str());
			m_iconImage->SetImage(UICommon::getCurrencyIcon().c_str());
			break;

		case TipType::GO_APP_SHOP:
			m_titleText->SetText(getString(LanguageKey::GUI_DIALOG_TIP_TITLE_TIP));
			m_cancelBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_CANCEL));
			m_sureBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_SURE));
			m_sureBtn->SetEnabled(true);
			m_otherMsgText->SetText(m_tipMessage.c_str());
			m_contentWindow->SetVisible(false);
			m_otherContentWindow->SetVisible(true);
			m_otherLackOfMoneyWindow->SetVisible(false);
			break;

		case TipType::REVIVE:
			m_titleText->SetText(getString(LanguageKey::GUI_DIALOG_TIP_TITLE_TIP));
			m_cancelBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_REVIVE_CANCEL));
			m_sureBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_REVIVE_SURE));
			msg = StringUtil::Format(getString(LanguageKey::GUI_DIALOG_TIP_REVIVE_MSG).c_str(), (int)m_countDown);
			m_messageText->SetText(msg.c_str());
			m_contentWindow->SetVisible(true);
			m_otherContentWindow->SetVisible(false);
			m_otherLackOfMoneyWindow->SetVisible(false);
			break;
		case TipType::GO_NPC_MERCHANT:
			m_titleText->SetText(getString(LanguageKey::GUI_DIALOG_TIP_TITLE_TIP));
			m_cancelBtn->SetText(getString(LanguageKey::GUI_DIALOG_BTN_UPGRADE_BAG));
			m_sureBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_SURE));
			m_sureBtn->SetEnabled(true);
			m_otherMsgText->SetText(m_tipMessage.c_str());
			m_contentWindow->SetVisible(false);
			m_otherContentWindow->SetVisible(true);
			m_otherLackOfMoneyWindow->SetVisible(false);
			break;
		case TipType::CHANGE_ACTORT_TIP:
			m_titleText->SetText(getString(LanguageKey::GUI_DIALOG_TIP_TITLE_CHANGE_ACTORT));
			m_cancelBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_CANCEL));
			m_sureBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_CHANGE_ACTORT));
			m_valueText->SetText(StringUtil::ToString(needMoneyCount).c_str());
			m_messageText->SetText(m_tipMessage.c_str());
			m_contentWindow->SetVisible(true);
			m_otherContentWindow->SetVisible(false);
			m_otherLackOfMoneyWindow->SetVisible(false);
			m_sureBtn->SetEnabled(true);
			//m_iconImage->SetImage("set:app_shop.json image:app_shop_diamonds");
			m_iconImage->SetImage(GameClient::CGame::Instance()->isMultiplayer() ? "set:app_shop.json image:app_shop_diamonds" : "set:app_shop.json image:app_shop_diamonds_mods");
			if (Blockman::Instance() && Blockman::Instance()->m_pPlayer)
			{
				Wallet * wallet = Blockman::Instance()->m_pPlayer->m_wallet;
				if (wallet && wallet->getDiamonds() >= needMoneyCount)
				{
					m_sureBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_CHANGE_ACTORT));
				}
				else
				{
					m_sureBtn->SetEnabled(false);
					m_sureBtn->SetText(getString(LanguageKey::LACK_OF_MONEY));
				}
			}
			break;

		case TipType::PICK_UP_ITEM:
			m_titleText->SetText(StringUtil::Format(getString(LanguageKey::GUI_TIP_PICK_UP_GOODS_TITLE).c_str(), m_itemName.c_str()).c_str());
			m_messageText->SetText(m_tipMessage.c_str());
			m_cancelBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_CANCEL));
			m_sureBtn->SetText(getString(LanguageKey::GUI_TIP_PICK_UP_GOODS_BTN_SURE));
			m_valueText->SetText(StringUtil::ToString(m_pickUpItemPrice).c_str());
			m_iconImage->SetImage(UICommon::getCurrencyIconByType(m_currencyType).c_str());
			m_contentWindow->SetVisible(true);
			m_otherContentWindow->SetVisible(false);
			m_otherLackOfMoneyWindow->SetVisible(false);
			break;
		case TipType::GET_MANOR:
			m_titleText->SetText(getString(LanguageKey::GUI_DIALOG_TIP_TITLE_TIP));
			m_cancelBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_CANCEL));
			m_sureBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_SURE_GET_MANOR));
			m_sureBtn->SetEnabled(true);
			m_otherMsgText->SetText(m_tipMessage.c_str());
			m_contentWindow->SetVisible(false);
			m_otherContentWindow->SetVisible(true);
			m_otherLackOfMoneyWindow->SetVisible(false);
			break;
		case  TipType::LACK_OF_PLATFORM_MONEY:
			m_titleText->SetText(getString(LanguageKey::LACK_OF_MONEY));
			m_cancelBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_CANCEL));
			m_sureBtn->SetText(getString(LanguageKey::GUI_BACK_PLATFORM_TOP_UP));

			m_lackOfMoneyTip->SetText(getString(LanguageKey::GUI_TIP_NEED_CURRENCY));
			m_lackOfMoneyMsg->SetText(getString(LanguageKey::GUI_NEED_BACK_PLATFORM_TOP_UP));
			m_lackOfMoneyValue->SetText(StringUtil::ToString(needMoneyCount).c_str());
			m_lackOfMoneyIcon->SetImage(UICommon::getCurrencyIconByType(m_currencyType).c_str());;
			
			m_contentWindow->SetVisible(false);
			m_otherContentWindow->SetVisible(false);
			m_otherLackOfMoneyWindow->SetVisible(true);
			m_sureBtn->SetEnabled(true);

			break;

		case TipType::UPGRADE_RESOURCE:
			refreshUpgradeResourceUI();
			m_otherLackOfMoneyWindow->SetVisible(false);
			break;
		case TipType::MANOR_BUILD_HOUSE:
			m_titleText->SetText(getString(LanguageKey::GUI_DIALOG_TIP_TITLE_TIP));
			m_cancelBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_CANCEL));
			m_sureBtn->SetText(getString(LanguageKey::GUI_MANOR_HOUSE_BTN_BUILD));
			m_sureBtn->SetEnabled(true);
			m_otherMsgText->SetText(m_tipMessage.c_str());
			m_contentWindow->SetVisible(false);
			m_otherContentWindow->SetVisible(true);
			m_otherLackOfMoneyWindow->SetVisible(false);
			break;
		case TipType::CUSTOM_TIP:
			refreshCustomTipUI();
			break;
		case TipType::BUILD_WAR_GUESS_TIP:
			m_titleText->SetText(getString(LanguageKey::GUI_DIALOG_TIP_TITLE_TIP));
			m_cancelBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_CANCEL));
			m_sureBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_SURE));
			m_sureBtn->SetEnabled(true);
			m_otherMsgText->SetText(m_tipMessage.c_str());
			m_contentWindow->SetVisible(false);
			m_otherContentWindow->SetVisible(true);
			m_otherLackOfMoneyWindow->SetVisible(false);
			break;
		case TipType::CONSUME_COIN_TIP:
			m_titleText->SetText(getString(LanguageKey::GUI_DIALOG_TIP_TITLE_TIP));
			m_cancelBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_CANCEL));
			m_sureBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_SURE));
			m_messageText->SetText(m_tipMessage.c_str());
			m_valueText->SetText(StringUtil::ToString(needMoneyCount).c_str());
			m_contentWindow->SetVisible(true);
			m_otherLackOfMoneyWindow->SetVisible(false);
			m_otherContentWindow->SetVisible(false);
			break;
		case TipType::KEEP_ITEM:
			m_titleText->SetText(getString(LanguageKey::GUI_DIALOG_TIP_TITLE_TIP));
			m_cancelBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_KEEP_ITEM_CANCEL));
			m_sureBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_KEEP_ITEM_SURE));
			msg = StringUtil::Format(getString(LanguageKey::GUI_DIALOG_TIP_KEEP_ITEM_MSG).c_str(), (int)m_countDown);
			m_messageText->SetText(msg.c_str());
			m_contentWindow->SetVisible(true);
			m_otherContentWindow->SetVisible(false);
			m_otherLackOfMoneyWindow->SetVisible(false);
			break;
		case TipType::ENCHANTMENT_QUICK:
			m_titleText->SetText(getString(LanguageKey::GUI_DIALOG_TIP_TITLE_TIP));
			m_cancelBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_CANCEL));
			m_sureBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_SURE));
			m_messageText->SetText(m_tipMessage.c_str());
			m_contentWindow->SetVisible(true);
			m_otherContentWindow->SetVisible(false);
			m_otherLackOfMoneyWindow->SetVisible(false);
			break;
		case TipType::PIXEL_GUNHALL_LOCK_MAP_DIAMOND:
			m_titleText->SetText(getString(LanguageKey::GUI_PIXEL_GUNHALL_MAP_DIAMOND_LOCK_TIP_TITLE));
			m_cancelBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_CANCEL));
			m_sureBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_SURE));
			m_messageText->SetText(m_tipMessage.c_str());
			m_contentWindow->SetVisible(true);
			m_otherContentWindow->SetVisible(false);
			m_otherLackOfMoneyWindow->SetVisible(false);
			m_iconImage->SetVisible(true);
			m_valueText->SetVisible(true);
			break;
		case TipType::PIXEL_GUNHALL_LOCK_MODE:
			m_cancelBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_CANCEL));
			m_sureBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_SURE));
			m_messageText->SetText(m_tipMessage.c_str());
			m_contentWindow->SetVisible(true);
			m_otherContentWindow->SetVisible(false);
			m_otherLackOfMoneyWindow->SetVisible(false);
			m_iconImage->SetVisible(false);
			m_valueText->SetVisible(false);
			m_cancelBtn->SetVisible(false);
			m_sureBtn->SetVisible(false);
			break;
		case TipType::PIXEL_GUNHALL_MODE_NOT_OPEN:
			m_cancelBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_CANCEL));
			m_sureBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_SURE));
			m_messageText->SetText(m_tipMessage.c_str());
			m_contentWindow->SetVisible(true);
			m_otherContentWindow->SetVisible(false);
			m_otherLackOfMoneyWindow->SetVisible(false);
			m_iconImage->SetVisible(false);
			m_valueText->SetVisible(false);
			m_cancelBtn->SetVisible(false);
			m_sureBtn->SetVisible(false);
			break;
		default:
			m_titleText->SetText(getString(LanguageKey::GUI_DIALOG_TIP_TITLE_TIP));
			m_cancelBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_CANCEL));
			m_sureBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_SURE));
			m_sureBtn->SetEnabled(true);
			m_cancelBtn->SetVisible(true);
			m_sureBtn->SetVisible(true);
			m_otherMsgText->SetText(m_tipMessage.c_str());
			m_contentWindow->SetVisible(false);
			m_otherContentWindow->SetVisible(true);
			m_otherLackOfMoneyWindow->SetVisible(false);
			break;
		}
	}

	void gui_tipDialog::onUpdate(ui32 nTimeElapse)
	{
		if (m_showTipType == TipType::REVIVE)
		{
			if (m_countDown > 0)
			{
				m_countDown -= (float)nTimeElapse / 1000;
				String msg = StringUtil::Format(getString(LanguageKey::GUI_DIALOG_TIP_REVIVE_MSG).c_str(), (int)m_countDown);
				m_messageText->SetText(msg.c_str());
				if (m_countDown < 0)
				{
					closeRevive();
				}
			}
		}
		else if (m_showTipType == TipType::KEEP_ITEM)
		{
			if (m_countDown > 0)
			{
				m_countDown -= (float)nTimeElapse / 1000;
				String msg = StringUtil::Format(getString(LanguageKey::GUI_DIALOG_TIP_KEEP_ITEM_MSG).c_str(), (int)m_countDown);
				m_messageText->SetText(msg.c_str());
				if (m_countDown < 0)
				{
					closeKeepItemTip();
				}
			}

		}
	}

	bool gui_tipDialog::onBtnClick(const EventArgs, VIewId viewId)
	{
		switch (viewId)
		{
		case gui_tipDialog::VIewId::BTN_SURE:
			switch (m_showTipType) {
			case TipType::VEHICLE_TIP:
				if (Blockman::Instance() && Blockman::Instance()->m_pPlayer)
				{
					Wallet * wallet = Blockman::Instance()->m_pPlayer->m_wallet;
					if (wallet && wallet->getCurrency() >= m_vehiclePrice)
					{
						GameClient::CGame::Instance()->getNetwork()->getSender()->sendBuyVehicle(m_vehicleId);
						getParent()->hideTipDialog();
					}
					else
					{
						getParent()->showAppShop();
					}
				}
				break;
			case TipType::UNLOCK_COMMODITY:
				if (Blockman::Instance() && Blockman::Instance()->m_pPlayer)
				{
					Wallet* wallet = Blockman::Instance()->m_pPlayer->m_wallet;
					if (wallet && wallet->getCurrency() >= m_unlockCost)
					{
						GameClient::CGame::Instance()->getNetwork()->getSender()->sendUnlockCommodity(m_merchantId, m_groupIndex, m_commodityIndex, m_commodityId);
						getParent()->hideTipDialog();
					}
					else
					{
						getParent()->showAppShop();
					}
				}
				break;
			case TipType::GO_NPC_MERCHANT:
				getParent()->hideTipDialog();
				GameClient::CGame::Instance()->getNetwork()->getSender()->sendGoNpcMerchant(m_npc_x, m_npc_y, m_npc_z, m_player_yaw);
				break;
			case TipType::GO_APP_SHOP:
				getParent()->showAppShop();
				break;
			case TipType::REVIVE:
				m_countDown = 0.0f;
				getParent()->hideTipDialog();
				GameClient::CGame::Instance()->getNetwork()->getSender()->sendBuyRespawn(true, m_uniqueId, m_index);
				break;
			case TipType::CHANGE_ACTORT_TIP:
				if (Blockman::Instance() && Blockman::Instance()->m_pPlayer)
				{
					Wallet * wallet = Blockman::Instance()->m_pPlayer->m_wallet;
					if (wallet && wallet->getDiamonds() >= needMoneyCount)
					{
						GameClient::CGame::Instance()->getNetwork()->getSender()->sendChangePlayerActor();
						getParent()->hideTipDialog();
					}
				}
				break;
			case TipType::PICK_UP_ITEM:
				pickUpItem();
				break;
			case TipType::GET_MANOR:
				//TODO
				ClientNetwork::Instance()->getSender()->sendGetManor();
				getParent()->hideTipDialog();
				break;
			case TipType::SELL_MANOR:
				if (Blockman::Instance()->m_pPlayer->m_manor->getInfo() && Blockman::Instance()->m_pPlayer->m_manor->getInfo()->manorId.length() > 0)
				{
					showSureSellManor(Blockman::Instance()->m_pPlayer->m_manor->getInfo()->value);
				}
				break;
			case TipType::SURE_SELL_MANOR:
				//TODO
				ClientNetwork::Instance()->getSender()->sendSellManor();
				getParent()->hideTipDialog();
				break;
			case TipType::LACK_OF_PLATFORM_MONEY:
				GameClient::CGame::Instance()->getShellInterface()->onRecharge(1);
				break;
			case TipType::UPGRADE_RESOURCE:
				upgradeResource();
				getParent()->hideTipDialog();
				break;
			case TipType::CUSTOM_TIP:
				getParent()->hideTipDialog();
				SyncCustomTipResult(true);
				break;
			case TipType::MANOR_BUILD_HOUSE:
				SureBuildManorHouseEvent::emit();
				getParent()->hideTipDialog();
				break;
			case TipType::BUILD_WAR_GUESS_TIP:
				SoundSystem::Instance()->playEffectByType(ST_Click);
				ClientNetwork::Instance()->getSender()->sendBuildWarGuessSuc(m_buildWarGuessId);
				getParent()->hideTipDialog();
				break;
			case TipType::CONSUME_COIN_TIP:
				sendConsumeCoinTipResult(true);
				break;
			case TipType::RANCHERS_EXPLORE_BEGIN:
				ClientNetwork::Instance()->getSender()->sendRanchExBegin();
				getParent()->hideTipDialog();
				break;
			case TipType::RANCHERS_TASK_FINISH:
				if (Blockman::Instance() 
					&& Blockman::Instance()->m_pPlayer 
					&& Blockman::Instance()->m_pPlayer->inventory
					&& Blockman::Instance()->m_pPlayer->inventory->getCurrentItem())
				{
					ClientNetwork::Instance()->getSender()->sendRanchExTaskFinish(Blockman::Instance()->m_pPlayer->inventory->getCurrentItem()->itemID, 
						Blockman::Instance()->m_pPlayer->inventory->getCurrentItem()->getItemStackInfo().num);
				}
				
				getParent()->hideTipDialog();
				break;
			case TipType::KEEP_ITEM:
				m_countDown = 0.0f;
				getParent()->hideTipDialog();
				GameClient::CGame::Instance()->getNetwork()->getSender()->sendKeepItem(true);
				break;
			case TipType::ENCHANTMENT_QUICK:
				getParent()->hideTipDialog();
				GameClient::CGame::Instance()->getNetwork()->getSender()->sendEnchantmentQuick(m_enchantment_equip_id, m_enchantment_effect_index);
				break;
			case TipType::PIXEL_GUNHALL_LOCK_MAP_DIAMOND:
				GameClient::CGame::Instance()->getNetwork()->getSender()->sendPixelHallUnlockMap(m_pixelhall_unlock_map_num, m_pixelhall_map_id);
				getParent()->hideTipDialog();
				break;
			case TipType::PIXEL_GUNHALL_LOCK_MODE:
				getParent()->hideTipDialog();
				break;
			case TipType::PIXEL_GUNHALL_MODE_NOT_OPEN:
				getParent()->hideTipDialog();
				break;
			default:
				getParent()->hideTipDialog();
				break;
			}
			break;
		case gui_tipDialog::VIewId::BTN_CANCEL:
			switch (m_showTipType) {
			case TipType::GO_NPC_MERCHANT:
			{
				getParent()->hideTipDialog();
				GameClient::CGame::Instance()->getNetwork()->getSender()->sendGoNpcMerchant(m_npc_x, m_npc_y, m_npc_z, m_player_yaw);
				auto dataCache = ClientNetwork::Instance()->getDataCache();
				auto entityId = dataCache->getClientId(m_merchantId);
				auto merchant = dynamic_cast<EntityMerchant*>(Blockman::Instance()->getWorld()->getEntity(entityId));
				if (merchant)
				{
					if (!merchant->getHasUpdated())
					{
						merchant->setHasUpdated(true);
						ClientNetwork::Instance()->getSender()->sendNeedUpdateMerchantCommodities(m_merchantId);
					}
					MerchantActivateEvent::emit(merchant);
				}
			}
			break;
			case TipType::CUSTOM_TIP:
				SyncCustomTipResult(false);
				getParent()->hideTipDialog();
				break;
			case TipType::CONSUME_COIN_TIP:
				sendConsumeCoinTipResult(false);
				break;
			case TipType::KEEP_ITEM:
				closeKeepItemTip();
				break;
			default:
				getParent()->hideTipDialog();
				if (m_showTipType == TipType::REVIVE)
					closeRevive();
				break;
			}
			break;
		}
		return true;
	}

	bool gui_tipDialog::onBtnClose(const EventArgs)
	{
		getParent()->hideTipDialog();
		if (m_showTipType == TipType::REVIVE)
			closeRevive();
		else if (m_showTipType == TipType::KEEP_ITEM)
		{
			closeKeepItemTip();
		}
		return true;
	}

	bool gui_tipDialog::showVehicleTipDialog(int vehicleId)
	{
		this->m_showTipType = TipType::VEHICLE_TIP;
		CarSetting* carSetting = CarSetting::getCarSetting(vehicleId);
		if (!CarSetting::m_sbIsCarFree && carSetting)
		{
			getParent()->showTipDialog();
			m_vehicleId = carSetting->carId;
			m_tipMessage = StringUtil::Format(getString(LanguageKey::GUI_DIALOG_TIP_CONTENT_UNLOCK_VEHICLE).c_str(), getString(carSetting->name).c_str());
			m_vehiclePrice = carSetting->price;
			refreshUI();
		}
		return true;
	}

	bool gui_tipDialog::showUnlockCommodityTipDialog(int merchantId, int groupIndex, int commodityIndex, int commodityId, int unlockCost, const String & commodityName)
	{
		this->m_showTipType = TipType::UNLOCK_COMMODITY;
		m_merchantId = merchantId;
		m_groupIndex = groupIndex;
		m_commodityIndex = commodityIndex;
		m_commodityId = commodityId;
		m_unlockCost = unlockCost;
		m_tipMessage = StringUtil::Format(getString(LanguageKey::GUI_DIALOG_TIP_CONTENT_UNLOCK_COMMODITY).c_str(), getString(commodityName).c_str());
		getParent()->showTipDialog();
		refreshUI();
		return true;
	}

	bool gui_tipDialog::showLackOfMoneyGoAppShop()
	{
		this->m_showTipType = TipType::GO_APP_SHOP;
		m_tipMessage = getString(LanguageKey::GUI_DIALOG_TIP_CONTENT_GO_APP_SHOP).c_str();
		getParent()->showTipDialog();
		refreshUI();
		return false;
	}

	bool gui_tipDialog::showGoNpcMerchantTipDialog(int merchantId, float x, float y, float z, float yaw)
	{
		this->m_merchantId = merchantId;
		this->m_npc_x = x;
		this->m_npc_y = y;
		this->m_npc_z = z;
		this->m_player_yaw = yaw;
		this->m_showTipType = TipType::GO_NPC_MERCHANT;
		m_tipMessage = getString(LanguageKey::GUI_DIALOG_TIP_CONTENT_GO_NPC_MERCHANT).c_str();
		getParent()->showTipDialog();
		refreshUI();
		return true;
	}

	bool gui_tipDialog::showChangePlayerActortTip(int changeCount, int needMoneyCount, const String& actortName)
	{
		this->m_showTipType = TipType::CHANGE_ACTORT_TIP;
		getParent()->showTipDialog();
		ActortName = actortName;
		this->needMoneyCount = needMoneyCount;
		m_tipMessage = StringUtil::Format(getString(LanguageKey::GUI_DIALOG_TIP_CONTENT_CHANGE_ACTORT).c_str(), StringUtil::ToString(changeCount).c_str());
		m_vehiclePrice = needMoneyCount;
		refreshUI();
		return true;
	}

	bool gui_tipDialog::showUpgradeResourceTip(int costItemId, int costItemNum, int resId,const String & tipStr)
	{
		this->m_showTipType = TipType::UPGRADE_RESOURCE;
		getParent()->showTipDialog();
		m_vehicleId = costItemId;
		m_tipMessage = tipStr;
		m_vehiclePrice = costItemNum;
		resourceId = resId;
		refreshUI();
		return true;
	}

	bool gui_tipDialog::showPickUpItemTipDialog(i32 itemEntityId, i32 itemId, i32 price, CurrencyType moneyType)
	{

		if (isShown() && itemEntityId == m_pickUpItemId)
		{
			return false;
		}

		ItemStackPtr itemStack = LORD::make_shared<ItemStack>(itemId, 1, 0);
		this->m_showTipType = TipType::PICK_UP_ITEM;
		getParent()->showTipDialog();
		this->m_pickUpItemId = itemId;
		this->m_pickUpItemPrice = price;
		this->m_itemName = LanguageManager::Instance()->getItemName(itemStack->getItemName()).c_str();
		this->m_pickUpItemEntityId = itemEntityId;
		this->m_currencyType = moneyType;
		m_tipMessage = StringUtil::Format(getString(LanguageKey::GUI_TIP_PICK_UP_GOODS_CONTENT).c_str(), m_itemName.c_str());
		refreshUI();
		return true;
	}

	bool gui_tipDialog::showResultTipDialog(ResultCode code, String resultMsg)
	{
		this->m_showTipType = TipType::OTHER_TIP;
		switch (code)
		{
		case ResultCode::ITEM_NOT_EXIST:
			m_tipMessage = getString(LanguageKey::GUI_TIP_PICK_UP_GOODS_NOT_EXIST).c_str();
			
			break;
		case ResultCode::LACK_MONEY:
			m_tipMessage = getString(LanguageKey::LACK_OF_MONEY).c_str();
			break;
		case ResultCode::INVENTORY_FULL:
			m_tipMessage = getString(LanguageKey::GUI_STR_APP_SHOP_INVENTORY_IS_FULL).c_str();
			break;
		case ResultCode::PAY_FAILURE:
			m_tipMessage = getString(LanguageKey::GUI_TIP_PICK_UP_GOODS_PAY_FAILURE).c_str();
			break;
		default:
			m_tipMessage = getString(resultMsg).c_str();
			break;
		}
		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::Rancher)
		{
			ShowRanchCommonTipEvent::emit(m_tipMessage);
			return false;
		}
		
		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::BirdSimulator)
		{
			ShowBirdCommonTipEvent::emit(m_tipMessage);
			return false;
		}
		getParent()->showTipDialog();
		refreshUI();
		return true;
	}

	bool gui_tipDialog::showGetOrSellManor()
	{
		auto manor = Blockman::Instance()->m_pPlayer->m_manor->getInfo();

		if (manor && manor->manorId.length() > 0)
		{
			showSellManor();
		}
		else
		{
			showGetManor();
		}

		return true;
	}

	bool gui_tipDialog::showGetManor()
	{
		auto manor = Blockman::Instance()->m_pPlayer->m_manor->getInfo();

		if (manor && manor->manorId.length() > 0)
		{
			showResultTipDialog(ResultCode::COMMON, LanguageKey::GUI_MANOR_GET_ALREADY_EXISTS);
		}
		else
		{
			this->m_showTipType = TipType::GET_MANOR;
			m_tipMessage = getString(LanguageKey::GUI_DIALOG_TIP_CONTENT_GET_MANOR).c_str();
			m_tipMessage = StringUtil::Format(m_tipMessage.c_str(), Blockman::Instance()->m_pPlayer->getEntityName().c_str());
			getParent()->showTipDialog();
			refreshUI();
		}
		return true;
	}

	bool gui_tipDialog::showSellManor()
	{
		auto manor = Blockman::Instance()->m_pPlayer->m_manor->getInfo();

		if (manor && manor->manorId.length() > 0)
		{
			if (manor->isMaster)
			{
				this->m_showTipType = TipType::SELL_MANOR;
				m_tipMessage = getString(LanguageKey::GUI_DIALOG_TIP_CONTENT_SELL_MANOR).c_str();
				m_tipMessage = StringUtil::Format(m_tipMessage.c_str(), Blockman::Instance()->m_pPlayer->getEntityName().c_str(), manor->value);
				getParent()->showTipDialog();
				refreshUI();
			}
			else
			{
				showResultTipDialog(ResultCode::COMMON, LanguageKey::GUI_MANOR_IS_NOT_LOAD);
			}
		}
		else
		{
			showResultTipDialog(ResultCode::COMMON, LanguageKey::GUI_MANOR_SELL_NOT_EXISTS);
		}
		return true;
	}

	bool gui_tipDialog::showSureSellManor(i32 price)
	{
		this->m_showTipType = TipType::SURE_SELL_MANOR;
		m_tipMessage = getString(LanguageKey::GUI_DIALOG_TIP_CONTENT_SURE_SELL_MANOR).c_str();
		m_tipMessage = StringUtil::Format(m_tipMessage.c_str(), price);
		getParent()->showTipDialog();
		refreshUI();
		return true;
	}

	bool gui_tipDialog::showLackOfMoney(i32 difference, CurrencyType type)
	{
		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::Rancher)
		{
			ShowRanchLockOfMoneyEvent::emit(difference, (int)type);
			return false;
		}

		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::BirdSimulator)
		{
			ShowBirdLockOfMoneyEvent::emit(difference, (int)type);
			return false;
		}
		if (type == CurrencyType::PLATFORM_MONEY)
		{
			this->m_showTipType = TipType::LACK_OF_PLATFORM_MONEY;
			this->m_currencyType = type;
			this->needMoneyCount = difference;
			getParent()->showTipDialog();
			refreshUI();
		}
		else
		{
			showLackOfMoneyGoAppShop();
		}
		return true;
	}

	bool gui_tipDialog::showManorBuildHouse()
	{
		this->m_showTipType = TipType::MANOR_BUILD_HOUSE;
		this->m_tipMessage = getString(LanguageKey::GUI_MANOR_BUILD_HOUSE_TIP).c_str();
		getParent()->showTipDialog();
		refreshUI();
		return true;
	}

	bool gui_tipDialog::showBuildWarGuessTip(int guessRoomId, String name)
	{
		m_buildWarGuessId = guessRoomId;
		m_showTipType = TipType::BUILD_WAR_GUESS_TIP;
		m_tipMessage = getString(LanguageKey::GUI_BUILD_WAR_GUESS_TIP).c_str();
		m_tipMessage = StringUtil::Format(m_tipMessage.c_str(), name.c_str());
		getParent()->showTipDialog();
		refreshUI();
		return true;
	}

	bool gui_tipDialog::showConsumeCoinTipMsg(const String & message, int coinId, int price, const String & extra)
	{
		m_showTipType = TipType::CONSUME_COIN_TIP;
		m_tipExtra = extra;
		m_tipMessage = getString(message).c_str();
		m_coinId = coinId;
		needMoneyCount = price;
		if (StringUtil::Find(m_tipMessage, "%d"))
			m_tipMessage = StringUtil::Format(m_tipMessage.c_str(), price);
		m_iconImage->SetImage(UICommon::getCurrencyIconByType((CurrencyType)coinId).c_str());
		getParent()->showTipDialog();
		refreshUI();
		return true;
	}

	void gui_tipDialog::sendConsumeCoinTipResult(bool isSure)
	{
		getParent()->hideTipDialog();
		if (!isSure)
		{
			ClientNetwork::Instance()->getSender()->sendConsumeTipResult(this->m_tipExtra, isSure);
			return;
		}
		if (Blockman::Instance() && Blockman::Instance()->m_pPlayer)
		{
			Wallet * wallet = Blockman::Instance()->m_pPlayer->m_wallet;
			if (wallet)
			{
				i64 money = wallet->getMoneyByCoinId(m_coinId);
				if (money < needMoneyCount)
					showLackOfMoney(needMoneyCount, (CurrencyType)m_coinId);
				else
					ClientNetwork::Instance()->getSender()->sendConsumeTipResult(this->m_tipExtra, isSure);
			}
		}
	}

	bool gui_tipDialog::showRanchExBeginTip()
	{
		this->m_showTipType = TipType::RANCHERS_EXPLORE_BEGIN;
		m_tipMessage = getString(LanguageKey::GUI_DIALOG_TIP_RANCHER_EXPLORE_BEGIN).c_str();
		getParent()->showTipDialog();
		refreshUI();

		return true;
	}

	bool gui_tipDialog::showRanchExTaskFinish()
	{
		this->m_showTipType = TipType::RANCHERS_TASK_FINISH;
		m_tipMessage = getString(LanguageKey::GUI_DIALOG_TIP_RANCHER_EXPLORE_TASK_FINISH).c_str();
		getParent()->showTipDialog();
		refreshUI();

		return true;
	}

	void gui_tipDialog::pickUpItem()
	{
		auto dataCache = ClientNetwork::Instance()->getDataCache();
		auto entityId = dataCache->getClientId(m_pickUpItemEntityId);
		auto entityItem = dynamic_cast<EntityItem*>(Blockman::Instance()->getWorld()->getEntity(entityId));
		if (!entityItem)
		{
			LordLogInfo("Pick up item do not exist");
			showResultTipDialog(ResultCode::ITEM_NOT_EXIST, "");
			return;
		}

		if (Blockman::Instance() && Blockman::Instance()->m_pPlayer)
		{
			if (!Blockman::Instance()->m_pPlayer->inventory->isCanAddItem(m_pickUpItemId, entityItem->getEntityItem()->stackSize, entityItem->getEntityItem()->getItem()->getMetadata(entityItem->getEntityItem()->getItemDamage())))
			{
				LordLogInfo("pick up item inventory full");
				showResultTipDialog(ResultCode::INVENTORY_FULL, "");
				return;
			}
			Wallet * wallet = Blockman::Instance()->m_pPlayer->m_wallet;

			if (wallet && wallet->getMoneyByCoinId((int) m_currencyType) >= m_pickUpItemPrice)
			{
				ClientNetwork::Instance()->getSender()->sendPickUpItemPay(m_pickUpItemEntityId , m_pickUpItemId);
				getParent()->hideTipDialog();
			}
			else
			{
				showResultTipDialog(ResultCode::LACK_MONEY, "");
			}
		}
	}

	bool gui_tipDialog::showRevive(int moneyType, int moneyCost, int countDown, int uniqueId, int index)
	{
		m_countDown = (float)countDown;
		m_uniqueId = uniqueId;
		m_index = index;
		this->m_showTipType = TipType::REVIVE;
		getParent()->showTipDialog();
		refreshUI();
		switch (moneyType)
		{
		case 1:
			m_iconImage->SetImage(GameClient::CGame::Instance()->isMultiplayer() ? "set:app_shop.json image:app_shop_diamonds" : "set:app_shop.json image:app_shop_diamonds_mods");
			break;
		case 2:
			m_iconImage->SetImage("set:app_shop.json image:app_shop_gold");
			break;
		case 3:
			m_iconImage->SetImage(UICommon::getCurrencyIcon().c_str());
			break;
		}
		m_valueText->SetText(StringUtil::ToString(moneyCost).c_str());
		Wallet *wallet = Blockman::Instance()->m_pPlayer->m_wallet;
		m_iconImage->SetImage(UICommon::getCurrencyIconByType((CurrencyType)moneyType).c_str());
		if (wallet)
		{
			ui64 money = wallet->getMoneyByCoinId(moneyType);
			m_sureBtn->SetEnabled(money >= moneyCost);
			m_sureBtn->SetText(getString(money >= moneyCost ? LanguageKey::GUI_DIALOG_TIP_REVIVE_SURE : LanguageKey::LACK_OF_MONEY));
		}
		else
		{
			m_sureBtn->SetEnabled(false);
		}

		return true;
	}

	void gui_tipDialog::closeRevive()
	{
		m_countDown = 0.0f;
		getParent()->hideTipDialog();
		GameClient::CGame::Instance()->getNetwork()->getSender()->sendBuyRespawn(false, -1, -1);
		LordLogInfo("C2SPacketBuyRespawn:%d", Root::Instance()->getCurrentTime());
	}

	void gui_tipDialog::closeKeepItemTip()
	{
		m_countDown = 0.0f;
		getParent()->hideTipDialog();
		GameClient::CGame::Instance()->getNetwork()->getSender()->sendKeepItem(false);
	}

	TipType gui_tipDialog::getShowTipType()
	{
		return m_showTipType;
	}

	bool gui_tipDialog::showCustomTipMsg(const String&  messageLang, const String&  extra)
	{
		this->m_showTipType = TipType::CUSTOM_TIP;
		this->m_tipExtra = extra;
		m_tipMessage = getString(messageLang).c_str();
		getParent()->showTipDialog();
		refreshUI();
		return true;
	}

	bool gui_tipDialog::showKeepItemTip(int coinType, int coin, int time)
	{
		m_countDown = (float)time;
		this->m_showTipType = TipType::KEEP_ITEM;
		getParent()->showTipDialog();
		refreshUI();
		switch (coinType)
		{
		case 1:
			m_iconImage->SetImage(GameClient::CGame::Instance()->isMultiplayer() ? "set:app_shop.json image:app_shop_diamonds" : "set:app_shop.json image:app_shop_diamonds_mods");
			break;
		case 2:
			m_iconImage->SetImage("set:app_shop.json image:app_shop_gold");
			break;
		case 3:
			m_iconImage->SetImage(UICommon::getCurrencyIcon().c_str());
			break;
		}
		m_valueText->SetText(StringUtil::ToString(coin).c_str());
		Wallet *wallet = Blockman::Instance()->m_pPlayer->m_wallet;
		m_iconImage->SetImage(UICommon::getCurrencyIconByType((CurrencyType)coinType).c_str());
		if (wallet)
		{
			ui64 money = wallet->getMoneyByCoinId(coinType);
			m_sureBtn->SetEnabled(money >= coin);
			m_sureBtn->SetText(getString(money >= coin ? LanguageKey::GUI_DIALOG_TIP_KEEP_ITEM_SURE : LanguageKey::LACK_OF_MONEY));
		}
		else
		{
			m_sureBtn->SetEnabled(false);
		}

		return true;
	}

	bool gui_tipDialog::showEnchantmentQuickTip(int diamond, int equipId, int effectIndex)
	{
		this->m_showTipType = TipType::ENCHANTMENT_QUICK;
		m_iconImage->SetImage(GameClient::CGame::Instance()->isMultiplayer() ? "set:app_shop.json image:app_shop_diamonds" : "set:app_shop.json image:app_shop_diamonds_mods");
		m_valueText->SetText(StringUtil::ToString(diamond).c_str());
		m_enchantment_equip_id = equipId;
		m_enchantment_effect_index = effectIndex;
		m_tipMessage = StringUtil::Format(getString(LanguageKey::GUI_DIALOG_TIP_ENCHANTMENT_QUICK).c_str());
		getParent()->showTipDialog();
		refreshUI();
		return true;
	}

	bool gui_tipDialog::showPixelGunHallNotOpen(int type)
	{
		this->m_showTipType = TipType::PIXEL_GUNHALL_MODE_NOT_OPEN;
		m_tipMessage = getString(LanguageKey::GUI_PIXEL_GUNHALL_MODE_NOT_OPEN_TIP_TXT).c_str();

		if (type == 0)
		{
			m_titleText->SetText(getString(LanguageKey::GUI_PIXEL_GUNHALL_MODE_NOT_OPEN_TIP_TITLE));
		}
		else if (type == 1)
		{
			m_titleText->SetText(getString(LanguageKey::GUI_PIXEL_GUNHALL_MAP_NOT_OPEN_TIP_TITLE));
		}
		
		getParent()->showTipDialog();
		refreshUI();
		return true;
	}

	bool gui_tipDialog::showPixelGunHallLvNotEnough(int lv, int type)
	{
		this->m_showTipType = TipType::PIXEL_GUNHALL_LOCK_MODE;
		m_tipMessage = StringUtil::Format(getString(LanguageKey::GUI_PIXEL_GUNHALL_MODE_LOCK_TIP_TXT).c_str(), lv);

		if (type == 0)
		{
			m_titleText->SetText(getString(LanguageKey::GUI_PIXEL_GUNHALL_MODE_LOCK_TIP_TITLE));
		}
		else
		{
			m_titleText->SetText(getString(LanguageKey::GUI_PIXEL_GUNHALL_MAP_LOCK_TIP_TITLE));
		}

		getParent()->showTipDialog();
		refreshUI();
		return true;
	}

	bool gui_tipDialog::showPixelGunHallMapLockDiamond(int num, int mapId)
	{
		this->m_showTipType = TipType::PIXEL_GUNHALL_LOCK_MAP_DIAMOND;
		m_pixelhall_unlock_map_num = num;
		m_pixelhall_map_id = mapId;
		m_iconImage->SetImage("set:app_shop.json image:app_shop_diamonds_mods");
		m_valueText->SetText(StringUtil::ToString(num).c_str());
		m_tipMessage = StringUtil::Format(getString(LanguageKey::GUI_PIXEL_GUNHALL_MAP_DIAMOND_LOCK_TIP_TXT).c_str());
		getParent()->showTipDialog();
		refreshUI();
		return true;
	}

	void  gui_tipDialog::refreshCustomTipUI()
	{
		m_titleText->SetText(getString(""));
		m_cancelBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_CANCEL));
		m_sureBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_SURE));
		m_sureBtn->SetEnabled(true);
		m_otherMsgText->SetText(m_tipMessage.c_str());
		m_contentWindow->SetVisible(false);
		m_otherContentWindow->SetVisible(true);
		m_otherLackOfMoneyWindow->SetVisible(false);
	}

	void gui_tipDialog::SyncCustomTipResult(bool bResult)
	{
		ClientNetwork::Instance()->getSender()->sendCustomTipResult(this->m_tipExtra, bResult);
	}
}
