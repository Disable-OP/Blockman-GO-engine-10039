/********************************************************************
filename: 	DataCache
file path:	H:\sandboxol\client\blockmango-client\dev\client\Src\Network\DataCache.h

version:	1
author:		qutianxiang
company:    sandboxol
date:		2017/07/25
*********************************************************************/
#ifndef __DATACACHE_H__
#define __DATACACHE_H__

#include "cWorld/Blockman.h"
#include "cWorld/WorldClient.h"
#include "Network/PacketHandlerRegistry.h"

#include "Core.h"

namespace BLOCKMAN
{
	class Entity;
	class WorldClient;
	class Blockman;
	class EntityPlayer;
}



class DataCache
{
public:
	typedef map<int, BLOCKMAN::Entity*>::type SyncEntities;
	typedef map<int, int>::type ClientToServerIdMap;
	typedef map<int, int>::type ServerToClientIdMap;

public:
	BLOCKMAN::Entity* getEntityByServerId(int serverId);
	BLOCKMAN::EntityPlayer* getPlayerByServerId(int serverId);

	void setBlockmanWorld(BLOCKMAN::Blockman* bm, BLOCKMAN::WorldClient* world);

	void setSelfServerId(int serverId) {
		m_selfServerId = serverId;
	}
	int getSelfServerId() { return m_selfServerId; }

	int getServerId(int clientId);
	int getClientId(int serverId);
	void addClientServerIdPair(int clientId, int serverId);
	void removeClientServerIdPair(int clientId);

	BLOCKMAN::Blockman* getBlockman() {
		return m_bm;
	}

	BLOCKMAN::WorldClient* getWorldClient() {
		return m_world;
	}

	void removeEntityByServerId(int serverId);

private:
	int   m_selfServerId = 0;

	BLOCKMAN::Blockman* m_bm = nullptr;
	BLOCKMAN::WorldClient* m_world = nullptr;

	SyncEntities m_syncEntities;
	ClientToServerIdMap m_clientToServerIdMap;
	ServerToClientIdMap m_serverToClientIdMap;
};
#endif
