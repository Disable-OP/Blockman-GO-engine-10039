#ifndef __WALLET_HEADER__
#define __WALLET_HEADER__

#include "Core.h"
#include "Util/SubscriptionGuard.h"
using namespace LORD;

namespace BLOCKMAN
{
	class InventoryPlayer;

	enum CurrencyType
	{
		UNKNOWN,
		PLATFORM_MONEY,
		PLATFORM_GOLD,
		GAME_MONEY,
		OTHER_MONEY
	};

	class Wallet : public ObjectAlloc
	{

	public:
		

	private:

		int m_coins[21] = {0};
		InventoryPlayer* m_inventory = nullptr;
		bool m_inventoryChanged = true;
		SubscriptionGuard m_subscriptionGuard;
		i64 m_diamonds = -1;
		i64 m_golds = -1;
		i64 m_currency = -1;

	public:
		Wallet(InventoryPlayer* inventory);
		virtual ~Wallet() {};

		int getCoinNum(int coindId)
		{
			return m_coins[coindId];
		}

		void consumeCoin(int coinId, int num);
		void onUpdate();

		void setDiamonds(i64 diamonds) { m_diamonds = diamonds; }
		void setGolds(i64 golds) { m_golds = golds; }
		void setCurrency(i64 currency) { m_currency = currency; }
		void subCurrency(i32 currency) { m_currency = m_currency - currency >= 0 ? m_currency - currency : 0; }
		void addCurrency(i32 currency) { m_currency += currency; }
		i64 getDiamonds() { return m_diamonds; }
		i64 getGolds() { return m_golds; }
		i64 getCurrency() { return m_currency; }

		i64 getMoneyByCoinId(i32 coinId)
		{
			switch (coinId)
			{
			case 1:
				return m_diamonds;
			case 2:
				return m_golds;
			case 3:
				return m_currency;
			default:
				return 0;
			}
		}
	};
}

#endif // !__WALLET_HEADER__
