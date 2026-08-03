/********************************************************************
filename: 	RenderLiving.h
file path:	dev\client\Src\Blockman\RenderEntity

version:	1
author:		ajohn
company:	supernano
date:		2017-3-13
*********************************************************************/
#ifndef __ENTITY_RENDER_LIVING_HEADER_
#define __ENTITY_RENDER_LIVING_HEADER_

#include "EntityRenderLivingBase.h"

namespace BLOCKMAN
{
	
class EntityLiving;
	
class EntityRenderLiving : public EntityRendererLivingBase
{
protected:
	float interpolate(float f1, float f2, float interval) { return f1 + (f2 - f1) * interval; }
	void renderLeash(EntityLiving* pLiving, const Vector3& pos, float yaw, float ticks);
	
	/** implement override functions from RenderLivingBase */
	virtual bool canRenderName(EntityLivingBase* pLiving);
	
public:
	EntityRenderLiving(ModelBase* pmodel, float shadowsize);
	virtual ~EntityRenderLiving();
	
	/** so this is the origin of all the render entry */
	virtual void doRenderLiving(EntityLiving* pLiving, const Vector3& pos, float yaw, float ticks);
	virtual void renderPlayer(EntityLivingBase* pLiving, const Vector3& pos, float yaw, float ticks);

	virtual bool compileSkinMesh(SkinedRenderable* pRenderable, const Vector2i& pos, const Vector2i& size) = 0;

	/** implement override functions from RenderLivingBase */
	virtual void doRender(Entity* pEntity, const Vector3& pos, float yaw, float ticks);
	virtual void updateBones(EntityLivingBase* pLiving, float rdt, Vector4* outMatrices);
};

}

#endif