/********************************************************************
filename: 	EntityBubbleFx.h
file path:	dev\client\Src\Blockman\Effects

version:	1
author:		ajohn
company:	supernano
date:		2017-10-11
*********************************************************************/
#ifndef __ENTITY_BUBBLE_FX_HEADER__
#define __ENTITY_BUBBLE_FX_HEADER__

#include "EntityFx.h"

namespace BLOCKMAN
{

class EntityBubbleFX : public EntityFx
{
public:
	EntityBubbleFX(World* pWorld, const Vector3& pos, const Vector3& pos2);

	/** implement override functions from EntityFx. */
	virtual void onUpdate() override;

protected:
	float lavaParticleScale = 0.f;
};

}

#endif