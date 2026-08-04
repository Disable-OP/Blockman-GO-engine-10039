/********************************************************************
filename: 	EntityRainFx.h
file path:	dev\client\Src\Blockman\Effects

version:	1
author:		ajohn
company:	supernano
date:		2017-12-28
*********************************************************************/
#ifndef __ENTITY_RAIN_FX_HEADER__
#define __ENTITY_RAIN_FX_HEADER__

#include "EntityFx.h"

namespace BLOCKMAN
{

class EntityRainFX : public EntityFx
{
public:
	EntityRainFX(World* pWorld, const Vector3& pos);
	
	/** implement override functions from EntityFx. */
	virtual void onUpdate() override;
};

}

#endif