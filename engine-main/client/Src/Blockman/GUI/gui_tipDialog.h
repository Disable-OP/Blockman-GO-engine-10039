#pragma once
#ifndef __GUI_TIP_DIALOG_HEADER__
#define __GUI_TIP_DIALOG_HEADER__
#include "Core.h"
#include "GUI/gui_layout.h"
#include "GUI/RootGuiLayout.h"
#include "Util/TipType.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIButton.h"
#include "Inventory/Wallet.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_tipDialog : public gui_layout
	{
		enum class VIewId
		{
			BTN_SURE,
			BTN_CANCEL
		};


	public:
		gui_tipDialog();
		~gui_tipDialog();

		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool onBtnClick(const EventArgs, VIewId viewId);
		bool onBtnClose(const EventArgs);
		bool showVehicleTipDialog(int vehicleId);
		bool showUnlockCommodityTipDialog(int merchantId, int groupIndex, int commodityIndex, int commodityId, int unlockCost, const String& commodityName);
		bool showLackOfMoneyGoAppShop();
		bool showRevive(int moneyType, int moneyCost, int countDown, int uniqueId, int index);
		void closeRevive();
		void closeKeepItemTip();
		TipType getShowTipType();
		bool showGoNpcMerchantTipDialog(int merchantId, float x, float y, float z, float yaw);
		bool showPickUpItemTipDialog(i32 itemEntityId, i32 itemId, i32 price, CurrencyType moneyType);
		bool showCustomTipMsg(const String&  customContentLangName, const String&  tipMsg);
		void refreshCustomTipUI();
		void SyncCustomTipResult(bool bResult);
		bool showResultTipDialog(ResultCode code, String resultMsg = "");
		bool showGetOrSellManor();
		bool showGetManor();
		bool showSellManor();
		bool showSureSellManor(i32 price);
		bool showLackOfMoney(i32 difference, CurrencyType type);
		bool showManorBuildHouse();
		bool showBuildWarGuessTip(int guessRoomId, String name);
		bool showConsumeCoinTipMsg(const String&  message, int coinId, int price, const String& extra);
		void sendConsumeCoinTipResult(bool isSure);
		bool showRanchExBeginTip();
		bool showRanchExTaskFinish();
		bool showKeepItemTip(int coinType, int coin, int time);
		bool showEnchantmentQuickTip(int diamond, int equipId, int effectIndex);
		bool showPixelGunHallNotOpen(int type);
		bool showPixelGunHallLvNotEnough(int lv, int type);
		bool showPixelGunHallMapLockDiamond(int num, int mapId);
		

	private:
		void pickUpItem();
		bool showChangePlayerActortTip(int changeCount, int needMoneyCount, const String& actortName);
		void onShow();
		bool showUpgradeResourceTip(int costItemId, int costItemNum, int resourceId, const String& tipStr);
		void refreshUpgradeResourceUI();
		void upgradeResource();
		

	private:
		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}

	private:
		void refreshUI();

		SubscriptionGuard m_subscriptionGuard;
		TipType m_showTipType = TipType::OTHER_TIP;
		CurrencyType m_currencyType = CurrencyType::UNKNOWN;
		String m_tipMessage = "";
		String m_itemName = "";
		i32 m_vehiclePrice = 0;
		i32 m_vehicleId = 0;

		i32 m_pickUpItemId = 0;
		i32 m_pickUpItemPrice = 0;
		i32 m_pickUpItemEntityId = 0;

		String ActortName = "";
		int needMoneyCount = 0;
		int m_coinId = 0;
		float m_countDown = 0;
		int m_uniqueId = 0;
		int m_index = 0;
		String m_tipExtra = "";

		int m_merchantId = 0;
		float m_npc_x = 0;
		float m_npc_y = 0;
		float m_npc_z = 0;
		float m_player_yaw = 0;

		int resourceId = 0;
		int m_buildWarGuessId = 0;

		int m_groupIndex = 0;
		int m_commodityIndex = 0;
		int m_commodityId = 0;
		int m_unlockCost = 0;

		int m_enchantment_equip_id = 0;
		int m_enchantment_effect_index = 0;
		int m_pixelhall_unlock_map_num = 0;
		int m_pixelhall_map_id = 0;

		GUIStaticText*		m_titleText = nullptr;
		GUIStaticText*		m_messageText = nullptr;
		GUIButton*			m_cancelBtn = nullptr;
		GUIButton*			m_sureBtn = nullptr;
		GUIStaticImage*		m_iconImage = nullptr;
		GUIStaticText*		m_valueText = nullptr;
		GUIStaticText*		m_otherMsgText = nullptr;
		GUIWindow*			m_contentWindow = nullptr;
		GUIWindow*			m_otherContentWindow = nullptr;
		GUIWindow*			m_otherLackOfMoneyWindow = nullptr;
		GUIStaticText*		m_lackOfMoneyTip = nullptr;
		GUIStaticImage*		m_lackOfMoneyIcon = nullptr;
		GUIStaticText*		m_lackOfMoneyValue = nullptr;
		GUIStaticText*		m_lackOfMoneyMsg = nullptr;
	};
}
#endif
