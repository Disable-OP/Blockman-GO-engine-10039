#include "EntityRenderLiving.h"
#include "EntityRenderManager.h"
#include "Model/ModelBase.h"
#include "Model/ModelRenderer.h"
#include "Model/ModelBox.h"

#include "Entity/EntityLiving.h"

namespace BLOCKMAN
{

EntityRenderLiving::EntityRenderLiving(ModelBase* pmodel, float shadowsize)
	: EntityRendererLivingBase(pmodel, shadowsize)
{}

EntityRenderLiving::~EntityRenderLiving()
{}

bool EntityRenderLiving::canRenderName(EntityLivingBase* pLivingBase)
{
	if (!pLivingBase->isClass(ENTITY_CLASS_LIVING))
		return false;

	EntityLiving* pLiving = static_cast<EntityLiving*>(pLivingBase);
	return EntityRendererLivingBase::canRenderName(pLiving) && (pLiving->getAlwaysRenderNameTagForRender() || pLiving->hasCustomNameTag() && pLiving == renderManager->pointedEntity);
}

void EntityRenderLiving::doRender(Entity* pEntity, const Vector3& pos, float yaw, float ticks)
{
	doRenderLiving((EntityLiving*)pEntity, pos, yaw, ticks);
}

void EntityRenderLiving::updateBones(EntityLivingBase* pLiving, float rdt, Vector4* outMatrices)
{
	EntityRendererLivingBase::updateBones(pLiving, rdt, outMatrices);
}

void EntityRenderLiving::doRenderLiving(EntityLiving* pLiving, const Vector3& pos, float yaw, float ticks)
{
	EntityRendererLivingBase::renderLivingBase(pLiving, pos, yaw, ticks);
	renderLeash(pLiving, pos, yaw, ticks);
}

void EntityRenderLiving::renderPlayer(EntityLivingBase* pLiving, const Vector3& pos, float yaw, float ticks)
{
	doRenderLiving((EntityLiving*)pLiving, pos, yaw, ticks);
}

void EntityRenderLiving::renderLeash(EntityLiving* pLiving, const Vector3& pos, float yaw, float ticks)
{
	
}

}