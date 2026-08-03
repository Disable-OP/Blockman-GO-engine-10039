/********************************************************************
filename: 	WorldServerListener.h
file path:	client\blockmango-client\dev\server\src\World

version:	1
author:		qutianxiang
company:    sandboxol
date:		2017/07/13
*********************************************************************/
#ifndef __WORLD_SERVER_LISTENER_H__
#define __WORLD_SERVER_LISTENER_H__

#include "World/IWorldEventListener.h"


namespace BLOCKMAN
{

class ServerWorld;

class WorldServerListener : public IWorldEventListener, public ObjectAlloc
{
protected:

	/** The WorldServer object. */
	ServerWorld* theWorldServer;

public:
	WorldServerListener(ServerWorld* pWorld);
	
	/** implement override functions from IWorldEventListener */
	virtual void spawnParticle(const String& particleType, const Vector3& pos, const Vector3& vel, EntityPlayer* emmiter = nullptr) {}
	virtual void onEntityCreate(Entity* pEntity);
	virtual void onEntityDestroy(Entity* pEntity);
	virtual void playSound(const Vector3& pos, SoundType soundType);
	virtual void playSoundToNearExcept(EntityPlayer* exceptPlayer, const Vector3& pos, SoundType soundType);
	virtual void markBlockRangeForRenderUpdate(int minx, int miny, int minz, int maxx, int maxy, int maxz) {}
	virtual void markBlockForUpdate(const BlockPos& pos);
	virtual void markBlockForRenderUpdate(const BlockPos& pos) {}
	virtual void playRecord(const String& recordName, const BlockPos& pos) {}
	virtual void playAuxSFX(EntityPlayer* player, int type, const BlockPos& pos, int data);
	virtual void broadcastSound(int soundID, const BlockPos& pos, int data);
	virtual void destroyBlockPartially(int breakerID, const BlockPos& pos, int progress);
	virtual void onPlayerChangeActor(EntityPlayer* pPlayer);
	virtual void onEntityChangeActor(Entity* pEntity, const String& actorName);
	virtual void onPlayerRestoreActor(Entity* pEntity);
};

}

#endif