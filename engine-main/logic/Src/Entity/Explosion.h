/********************************************************************
filename: 	Explosion.h
file path:	dev\client\Src\Blockman\Entity

version:	1
author:		ajohn
company:	supernano
date:		2017-2-8
*********************************************************************/
#ifndef __EXPLOSION_HEADER__
#define __EXPLOSION_HEADER__

#include "BM_Container_def.h"

namespace BLOCKMAN
{

class Random;
class World;
class Entity;
class EntityLivingBase;

typedef map<Entity*, Vector3>::type	PlayerKnockMap;

class Explosion : public ObjectAlloc
{
protected:
	Random* rand = nullptr;
	World* worldObj = nullptr;
	PlayerKnockMap playerKnockbackMap; 
	EntityLivingBase* explosivePlacedBy;
	// Map field_77288_k = new HashMap();

public:
	/** whether or not the explosion sets fire to blocks around it */
	bool isFlaming = false;

	/** whether or not this explosion spawns smoke particles */
	bool isSmoking = true; // = true;
	Vector3 explosionPos;
	Entity* exploder = nullptr;
	float explosionSize = 0.f;

	/** A list of ChunkPositions of blocks affected by this explosion */
	BlockPosArr affectedBlocksPos;
	

public:
	Explosion(World* pWorld, Entity* exploder, EntityLivingBase* explosivePlacedBy, const Vector3& pos, float size);
	Explosion(World* pWorld, Entity* exploder, EntityLivingBase* explosivePlacedBy, const Vector3& pos, float size , BlockPosArr blocksPos);

	~Explosion();

	/** Does the first part of the explosion (destroy blocks) */
	void doExplosionA(bool isBreakBlock, bool isHurt, bool isHurtEntityItem, float custom_explosionSize = 1.0f);
	
	/** Does the second part of the explosion (sound, particles, drop spawn) */
	void doExplosionB(bool spawnParticles, float custom_explosionSize = 1.0f);
	
	PlayerKnockMap& getPlayerKnockbackMap() {	return playerKnockbackMap; }

	EntityLivingBase* getExplosivePlacedBy()
	{
		return explosivePlacedBy;
	}
};

}
#endif