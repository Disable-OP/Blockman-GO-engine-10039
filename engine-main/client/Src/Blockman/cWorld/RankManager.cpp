#include "RankManager.h"
#include "Core.h"
#include "Network/ClientNetwork.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

namespace GameClient
{
	RankManager::RankManager() 
	{

	}

	RankManager::~RankManager()
	{

	}

	void RankManager::setRankInfo(int entityId, String info)
	{
		m_RankInfoMap[entityId] = info;
	}

	String RankManager::getRankInfo(int entityId)
	{
		auto dataCache = ClientNetwork::Instance()->getDataCache();
		auto sEntityId = dataCache->getServerId(entityId);
		if (m_RankInfoMap.find(sEntityId) != m_RankInfoMap.end())
		{
			return m_RankInfoMap[sEntityId];
		}
		else
		{
			return "";
		}
	}
}