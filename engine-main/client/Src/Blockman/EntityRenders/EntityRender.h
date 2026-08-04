/********************************************************************
filename: 	Render.h
file path:	dev\client\Src\Blockman\RenderEntity

version:	1
author:		ajohn
company:	supernano
date:		2017-3-8
*********************************************************************/
#ifndef __ENTITY_RENDER_HEADER__
#define __ENTITY_RENDER_HEADER__

#include "Core.h"
using namespace LORD;

namespace BLOCKMAN
{

class Entity;
class RenderBlocks;
class EntityRenderManager;
class TextureAtlasRegister;
class World;
class Block;

class EntityRender : public ObjectAlloc
{
protected:
	static Texture* SHADOW_TEXTURES; // = new ResourceLocation("textures/misc/shadow.png");
	
	EntityRenderManager* renderManager = nullptr;
	float shadowSize = 0.f;
	/** Determines the darkness of the object's shadow. Higher value makes a darker shadow. */
	float shadowOpaque = 0.f;
	bool renderOutlines = false;


protected:
	/** Renders fire on top of the entity. Args: entity, x, y, z, partialTickTime */
	void renderEntityOnFire(Entity* pEntity, const Vector3& pos, float rdt);
	/** Renders the entity shadows at the position, shadow alpha and partialTickTime.*/
	void renderShadow(Entity* pEntity, const Vector3& pos, float alpha, float rdt);
	/** Returns the render manager's world object */
	World* getWorldFromRenderManager();
	/** Renders a shadow projected down onto the specified block. Brightness of the block plus how far away on the Y axis
	* determines the alpha of the shadow. */
	void renderShadowOnBlock(Block* pBlock, const Vector3& center, const Vector3i& blockPos, float alpha, float size, const Vector3& offset);

public:
	EntityRender();
	virtual ~EntityRender();
	
	virtual void doRender(Entity* pEntity, const Vector3& pos, float yaw, float rdt) = 0;
	virtual void doRender_gui(Entity* pEntity, const Matrix4& worldmat, const Matrix4& viewprojMat) {}
	virtual void updateIcons(TextureAtlasRegister* atlas) {}

	/** Renders a white box with the bounds of the AABB translated by the offset. Args: aabb, x, y, z */
	static void renderOffsetAABB(const Box& aabb, const Vector3& pos);
	/** Adds to the tesselator a box using the aabb for the bounds. Args: aabb */
	static void renderAABB(const Box& aabb);
	/** Sets the RenderManager. */
	void setRenderManager(EntityRenderManager* rm) { renderManager = rm; }
	/** Renders the entity's shadow and fire (if its on fire).*/
	void doRenderShadowAndFire(Entity* pEntity, const Vector3& pos, float yaw, float rdt);
	/** Returns the font renderer from the set render manager */
	// FontRenderer* getFontRendererFromRenderManager()

	/** Renders the entity's selected node (if its on selected).*/
	void doRenderSelectedNode(Entity* pEntity, const Vector3& pos, float yaw, float rdt);
};


}

#endif
