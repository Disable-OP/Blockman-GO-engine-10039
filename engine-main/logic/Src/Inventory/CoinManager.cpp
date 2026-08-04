#include "CoinManager.h"

namespace BLOCKMAN
{
	CoinManager::CoinManager()
	{
		
	}

	CoinManager::~CoinManager()
	{

	}

	void CoinManager::addCoinMapping(int coinId, int itemId)
	{
		m_coinMapping[coinId] = itemId;
	}

	int CoinManager::itemIdToCoinId(int itemId)
	{
		for (LORD::map<int, int>::type::iterator it = m_coinMapping.begin(); it != m_coinMapping.end(); ++it)
		{
			if (it->second == itemId)
			{
				return it->first;
			}
		}
		return 0;
	}

	int CoinManager::coinIdToItemId(int coinId)
	{
		LORD::map<int, int>::type::iterator iter = m_coinMapping.find(coinId);
		if (iter != m_coinMapping.end())
		{
			return iter->second;
		}
		return 0;
	}
}
