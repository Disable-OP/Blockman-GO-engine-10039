/********************************************************************
filename: 	EntityRenderVehicle.h
file path:	dev\client\Src\Blockman\RenderEntity

version:	1
author:		ajohn
company:	supernano
date:		2018-01-29
*********************************************************************/
#ifndef __ENTITY_RENDER_VEHICLE_HEADER__
#define __ENTITY_RENDER_VEHICLE_HEADER__

#include "EntityRender.h"

namespace BLOCKMAN
{
	
class EntityVehicle;

class EntityRenderVehicle: public EntityRender
{
public:
	EntityRenderVehicle();
	virtual ~EntityRenderVehicle();

	/** implement override functions for EntityRender */
	virtual void doRender(Entity* pEntity, const Vector3& pos, float yaw, float rdt);
};

}

#endif