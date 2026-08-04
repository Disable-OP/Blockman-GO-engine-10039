/********************************************************************
filename: 	EntityDiggingFx.h
file path:	dev\client\Src\Blockman\Effects

version:	1
author:		ajohn
company:	supernano
date:		2017-07-31
*********************************************************************/
#ifndef __ENTITY_DIGGING_FX_HEADER__
#define __ENTITY_DIGGING_FX_HEADER__

#include "EntityFx.h"

namespace BLOCKMAN
{

class cBlock;

class EntityDiggingFX : public EntityFx
{
protected:
	cBlock* blockInstance = nullptr;

public:
	EntityDiggingFX(World* pWorld, const Vector3& pos1, const Vector3& pos2, Block* pBlock, int side);
	
	/** If the block has a colour multiplier, copies it to this particle and returns this particle. */
	void applyColourMultiplier(const BlockPos& pos);
	
	/** Creates a new EntityDiggingFX with the block render color applied to the base particle color */
	void applyRenderColor(int meta);
	
	/** implement override functions from EntityFx. */
	virtual int getFXLayer() { return 1; }
	virtual void renderParticle(FxVertexFmt* vertices, int brightness, float rdt, float rx, float rxz, float rz, float ryz, float rxy);
};

}

#endif
