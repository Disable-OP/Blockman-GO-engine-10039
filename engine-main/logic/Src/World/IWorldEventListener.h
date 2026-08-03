/********************************************************************
filename: 	IWorldEventListener.h
file path:	dev\client\Src\Blockman\World

version:	1
author:		ajohn
company:	supernano
date:		2016-2-8
*********************************************************************/
#ifndef __WORLD_ACCESS_HEADER__
#define __WORLD_ACCESS_HEADER__

#include "BM_TypeDef.h"
#include "Audio/SoundDef.h"

namespace BLOCKMAN
{

class Entity;
class EntityPlayer;

class IWorldEventListener
{
public:
	/** On the client, re-renders the block. On the server, sends the block to the client (which will re-render it), */
	virtual void markBlockForUpdate(const BlockPos& pos) = 0;

	/** On the client, re-renders this block. On the server, does nothing. Used for lighting updates. */
	virtual void markBlockForRenderUpdate(const BlockPos& pos) = 0;

	/** On the client, re-renders all blocks in this range, inclusive. On the server, does nothing. */
	virtual void markBlockRangeForRenderUpdate(int minx, int miny, int minz, int maxx, int maxy, int maxz) = 0;

	/** Plays the specified sound. Arg: soundName, x, y, z, volume, pitch */
	virtual void playSound(const Vector3& pos, SoundType soundType) = 0;

	/** Plays sound to all near players except the player reference given */
	virtual void playSoundToNearExcept(EntityPlayer* exceptPlayer, const Vector3& pos, SoundType soundType) = 0;

	/** Spawns a particle. Arg: particleType, x, y, z, velX, velY, velZ	*/
	virtual void spawnParticle(const String& particleType, const Vector3& pos, const Vector3& vel, EntityPlayer* emmiter = nullptr) = 0;

	/** Called on all IWorldAccesses when an entity is created or loaded. On client worlds. */
	virtual void onEntityCreate(Entity* pEntity) = 0;

	/** Called on all IWorldAccesses when an entity is unloaded or destroyed. On client worlds. */
	virtual void onEntityDestroy(Entity* pEntity) = 0;

	/** Plays the specified record. Arg: recordName, x, y, z */
	virtual void playRecord(const String& recordName, const BlockPos& pos) = 0;

	virtual void broadcastSound(int soundID, const BlockPos& pos, int data) = 0;

	/** Plays a pre-canned sound effect along with potentially auxiliary data-driven one-shot behaviour (particles, etc). */
	virtual void playAuxSFX(EntityPlayer* player, int type, const BlockPos& pos, int data) = 0;

	/** Starts (or continues) destroying a block with given ID at the given coordinates for the given partially destroyed value	*/
	virtual void destroyBlockPartially(int breakerID, const BlockPos& pos, int progress) = 0;

	// change player actor
	virtual void onPlayerChangeActor(EntityPlayer* pEntityPlayer) = 0;

	// change entity actor
	virtual void onEntityChangeActor(Entity* pEntity, const String& actorName) = 0;

	// restore player actor
	virtual void onPlayerRestoreActor(Entity* pEntity) = 0;
};
}
#endif