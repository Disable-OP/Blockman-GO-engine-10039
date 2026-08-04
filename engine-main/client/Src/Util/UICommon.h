#pragma once

#include "Core.h"
#include "Inventory/Wallet.h"
#include "World/Ranch.h"
using namespace LORD;

namespace BLOCKMAN {

	class BirdScope;

	class UICommon
	{

	public: 
		static bool checkMoney(i32 price, i32 type);
		static bool checkMoney(i32 price, CurrencyType type);
		static bool checkRanchItems(const std::vector<RanchCommon>& items);
		static bool checkRanchItems(const vector<RanchCommon>::type& items);
		static bool checkRanchStorageCapacity(const vector<RanchCommon>::type& items);
		static bool checkRanchLevel(i32 needLevel);
		static bool isRanchIconVisible();
		static bool checkOpenFlying();

		static vector<RanchCommon>::type getLackRanchItems(const std::vector<RanchCommon>& items);
		static vector<RanchCommon>::type getLackRanchItems(const vector<RanchCommon>::type& items);

		static bool checkBirdMiniTip(const BirdScope& scope);
		static bool checkBirdConvert();
		static String getBirdMiniTipText(const BirdScope& scope);

	public:

		static void onMiniTipClick();

	public:
		static bool checkPlayerParam();
		static bool checkWalletParam();
		static bool checkRanchParam();
		static bool checkBirdParam();
		static bool checkBirdBagParam();

	public:
		static i64 getOtherCurrency();
		static String getCurrencyIcon();
		static String getOtherCurrencyIcon();
		static String getCurrencyIconByType(i32 currencyType);
		static String getCurrencyIconByType(CurrencyType currencyType);
		static String timeFormat(i32 time);
		static bool isMultiplayer();
	};

}
