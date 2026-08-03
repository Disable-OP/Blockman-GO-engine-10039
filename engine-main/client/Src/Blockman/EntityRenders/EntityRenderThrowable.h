/********************************************************************
filename: 	EntityRenderThrowable.h
file path:	dev\client\Src\Blockman\RenderEntity

version:	1
author:		ajohn
company:	supernano
date:		2017-10-26
*********************************************************************/
#ifndef __ENTITY_RENDER_THROWABLE_HEADER__
#define __ENTITY_RENDER_THROWABLE_HEADER__

#include "EntityRender.h"

namespace BLOCKMAN
{

class cItem;
class AtlasSprite;

class EntityRenderThrowable : public EntityRender
{
public:
	EntityRenderThrowable(cItem* pItem, int subtype = 0);
	/** implement override functions for EntityRender */
	virtual void doRender(Entity* pEntity, const Vector3& pos, float yaw, float rdt);
	
protected:
	void render_impl(const Vector3& pos, int brightness, int color, AtlasSprite* sprite);

private:
	cItem* m_pItem = nullptr;
	int m_damage = 0;
};

}

#endif
