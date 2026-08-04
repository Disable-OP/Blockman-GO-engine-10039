/********************************************************************
filename: 	EntityRenderItem.h
file path:	dev\client\Src\Blockman\RenderEntity

version:	1
author:		ajohn
company:	supernano
date:		2017-5-4
*********************************************************************/
#ifndef __ENTITY_RENDER_ITEM_HEADER__
#define __ENTITY_RENDER_ITEM_HEADER__

#include "EntityRender.h"

namespace BLOCKMAN
{

class Random;
class EntityItem;
class AtlasSprite;

class EntityRenderItem : public EntityRender
{
protected:
	static Texture* field_110798_h;		// = new ResourceLocation("textures/misc/enchanted_item_glint.png");
	Texture* texBlock = nullptr;
	Texture* texItem = nullptr;

	/** The RNG used in RenderItem (for bobbing itemStacks on the ground) */
	Random* random = nullptr;						// = new Random();

public:
	bool renderWithColor = true;				// = true;

	/** Defines the zLevel of rendering of item on GUI. */
	float zLevel = 0.f;
	static bool renderInFrame;

public:
	EntityRenderItem();
	virtual ~EntityRenderItem();

	void doRenderItem(EntityItem* pEntityItem, const Vector3& pos, float yaw, float rdt);
	
	/** implement override functions for EntityRender */
	virtual void doRender(Entity* pEntity, const Vector3& pos, float yaw, float rdt);
};

}

#endif