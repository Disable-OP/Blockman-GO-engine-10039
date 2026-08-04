/********************************************************************
filename: 	EntityRenderTNTPrimed.h
file path:	dev\client\Src\Blockman\RenderEntity

version:	1
author:		ajohn
company:	supernano
date:		2017-10-27
*********************************************************************/
#ifndef __ENTITY_RENDER_TNTPRIMED_HEADER__
#define __ENTITY_RENDER_TNTPRIMED_HEADER__

#include "EntityRender.h"

namespace BLOCKMAN
{

class RenderBlocks;
class EntityTNTPrimed;

class EntityRenderTNTPrimed: public EntityRender
{
public:
	EntityRenderTNTPrimed();
	virtual ~EntityRenderTNTPrimed();

	/** implement override functions for EntityRender */
	virtual void doRender(Entity* pEntity, const Vector3& pos, float yaw, float rdt);

protected:
	void render_impl(EntityTNTPrimed* tnt, const Vector3& pos, float yaw, float rdt);
	bool createModel();

private:
	SkinedVertexFmt* m_model = nullptr;
	int m_quadNum = 0;
};

}

#endif