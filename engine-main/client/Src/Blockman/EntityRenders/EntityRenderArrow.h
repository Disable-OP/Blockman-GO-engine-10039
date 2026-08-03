/********************************************************************
filename: 	EntityRenderArrow.h
file path:	dev\client\Src\Blockman\RenderEntity

version:	1
author:		ajohn
company:	supernano
date:		2017-7-3
*********************************************************************/
#ifndef __ENTITY_RENDER_ARROW_HEADER__
#define __ENTITY_RENDER_ARROW_HEADER__

#include "EntityRender.h"

namespace BLOCKMAN
{
class EntityArrow;

class EntityRenderArrow : public EntityRender
{
public:
	EntityRenderArrow();
	virtual ~EntityRenderArrow();

	void doRenderArrow(EntityArrow* pEntityItem, const Vector3& pos, float yaw, float rdt);

	/** implement override functions for EntityRender */
	virtual void doRender(Entity* pEntity, const Vector3& pos, float yaw, float rdt);

protected:
	void initArrowModel();

protected:
	Texture*			m_texArrow = nullptr;
	SkinedVertexFmt*	m_model = nullptr;
};

}

#endif