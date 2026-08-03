/********************************************************************
filename: 	CoinManager.h
file path:	dev\logic\Src\Inventory

version:	1
author:		ajohn
company:	supernano
date:		2017-12-12
*********************************************************************/

#ifndef __COIN_MANAGER_HEADER__
#define __COIN_MANAGER_HEADER__

#include "BM_TypeDef.h"

namespace BLOCKMAN
{
	class CoinManager : public Singleton<CoinManager>, public ObjectAlloc
	{
	public:
		/** coin mapping */
		LORD::map<int, int>::type m_coinMapping;
	public:

		CoinManager();
		~CoinManager();

		void clearCoinMapping() { m_coinMapping.clear(); }
		void addCoinMapping(int coinId, int itemId);
		int itemIdToCoinId(int itemId);
		int coinIdToItemId(int coinId);
	};
}
	
#endif