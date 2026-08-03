/********************************************************************
filename: 	EntitySmokeFx.h
file path:	dev\client\Src\Blockman\Effects

version:	1
author:		ajohn
company:	supernano
date:		2017-10-11
*********************************************************************/
#ifndef __ENTITY_SMOKE_FX_HEADER__
#define __ENTITY_SMOKE_FX_HEADER__

#include "EntityFx.h"

namespace BLOCKMAN
{

class EntitySmokeFX : public EntityFx
{
public:
	EntitySmokeFX(World* pWorld, const Vector3& pos, const Vector3& pos2, float scale = 1.f);
	
	/** implement override functions from EntityFx. */
	virtual void renderParticle(FxVertexFmt* vertices, int brightness, float rdt, float rx, float rxz, float rz, float ryz, float rxy) override;
	virtual void onUpdate() override;

protected:
	float smokeParticleScale = 0.f;
};

}

#endif