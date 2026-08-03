/********************************************************************
filename: 	EntityRenderBlockmanEmpty.h
file path:	dev\client\Src\Blockman\RenderEntity

version:	1
author:		ajohn
company:	supernano
date:		2018-01-29
*********************************************************************/
#ifndef __ENTITY_RENDER_BLOCKMAN_EMPTY_HEADER__
#define __ENTITY_RENDER_BLOCKMAN_EMPTY_HEADER__

#include "EntityRender.h"

namespace BLOCKMAN
{
	
class EntityBlockmanEmpty;

class EntityRenderBlockmanEmpty: public EntityRender
{
public:
	EntityRenderBlockmanEmpty();
	virtual ~EntityRenderBlockmanEmpty();

	/** implement override functions for EntityRender */
	virtual void doRender(Entity* pEntity, const Vector3& pos, float yaw, float rdt);
};

}

#endif