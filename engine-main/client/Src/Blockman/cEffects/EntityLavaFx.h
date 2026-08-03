/********************************************************************
filename: 	EntityLavaFx.h
file path:	dev\client\Src\Blockman\Effects

version:	1
author:		ajohn
company:	supernano
date:		2017-10-11
*********************************************************************/
#ifndef __ENTITY_LAVA_FX_HEADER__
#define __ENTITY_LAVA_FX_HEADER__

#include "EntityFx.h"

namespace BLOCKMAN
{

class EntityLavaFX : public EntityFx
{
public:
	EntityLavaFX(World* pWorld, const Vector3& pos);
	
	/** implement override functions from Entity. */
	virtual int getBrightnessForRender(float rdt) override;
	virtual float getBrightness(float rdt) override { return 1.0F; }

	/** implement override functions from EntityFx. */
	virtual void renderParticle(FxVertexFmt* vertices, int brightness, float rdt, float rx, float rxz, float rz, float ryz, float rxy) override;
	virtual void onUpdate() override;

protected:
	float lavaParticleScale = 0.f;
};

}

#endif