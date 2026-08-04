/********************************************************************
filename: 	RenderLivingBase.h
file path:	dev\client\Src\Blockman\RenderEntity

version:	1
author:		ajohn
company:	supernano
date:		2017-3-13
*********************************************************************/
#ifndef __ENTITY_RENDER_LIVING_BASE_HEADER__
#define __ENTITY_RENDER_LIVING_BASE_HEADER__

#include "EntityRender.h"

namespace BLOCKMAN
{

class ModelBase;
class EntityLivingBase;

class EntityRendererLivingBase : public EntityRender
{
protected:
	static Texture* TEXTURE_BRIGHTNESS; // textures/misc/enchanted_item_glint.png
	ModelBase* mainModel = nullptr;
	/** The model to be used during the render passes. */
	ModelBase* renderPassModel = nullptr;

protected:
	/** Returns a rotation angle that is inbetween two other rotation angles. par1 and par2 are the angles between which
	* to interpolate, par3 is probably a float between 0.0 and 1.0 that tells us where "between" the two angles we are.
	* Example: par1 = 30, par2 = 50, par3 = 0.5, then return = 40 */
	float interpolateRotation(float angle1, float angle2, float interval);
	/** Renders the model in RenderLiving */
	void renderModel(EntityLivingBase* pLiving, float limbSwing, float swingAmount, float ticks, float headYaw, float headPitch);
	float renderSwingProgress(EntityLivingBase* pLiving, float rdt);
	/** Defines what float the third param in setRotationAngles of ModelBase is */
	float handleRotationFloat(EntityLivingBase* pLiving, float ticks);
	/** renders arrows the Entity has been attacked with, attached to it */
	void renderArrowsStuckInEntity(EntityLivingBase* pLiving, float ticks);
	int inheritRenderPass(EntityLivingBase* pLiving, int par2, float par3) { return shouldRenderPass(pLiving, par2, par3); }
	/** Passes the specialRender and renders it */
	void passSpecialRender(EntityLivingBase* pLiving, const Vector3& pos);
	/** dorender render living base*/
	void renderLivingBase(EntityLivingBase* pLiving, const Vector3& pos, float yaw, float ticks);
	/** Draws the debug or playername text above a living */
	void renderLivingLabel(EntityLivingBase* pLiving, const String& label, const Vector3& pos, int mindis);

	/** Sets a simple glTranslate on a LivingEntity. */
	virtual void renderLivingAt(EntityLivingBase* pLiving, const Vector3& pos);
	virtual void rotateCorpse(EntityLivingBase* pLiving, float pitch, float yaw, float ticks);
	virtual void renderEquippedItems(EntityLivingBase* pLiving, float ticks) {}
	/** Queries whether should render the specified pass or not. */
	virtual int shouldRenderPass(EntityLivingBase* pLiving, int pass, float ticks) { return -1; }
	virtual void preRenderArmor(EntityLivingBase* pLiving, int par2, float par3) { }
	/** Returns an ARGB int color back. Args: entityLiving, lightBrightness, partialTickTime */
	virtual int getColorMultiplier(EntityLivingBase* pLiving, float par2, float par3) { return 0; }
	/** Allows the render to do any OpenGL state modifications necessary before the model is rendered. */
	virtual void preRenderCallback(EntityLivingBase* pLiving, float ticks) {}
	virtual bool canRenderName(EntityLivingBase* pLiving);
	/** render label */
	virtual void renderLabel(EntityLivingBase* pLiving, const Vector3& pos, const String& label, float scale, float dissqr);

public:
	EntityRendererLivingBase(ModelBase* pModel, float shadowsize);
	virtual ~EntityRendererLivingBase();

	ModelBase* getMainModel() { return mainModel; }
	
	/** Sets the model to be used in the current render pass (the first render pass is done after the primary model is rendered) */
	void setRenderPassModel(ModelBase* pModel) { renderPassModel = pModel; }
	
	/** implement override functions from Render */
	virtual void doRender(Entity* pEnitity, const Vector3& pos, float yaw, float ticks);

	virtual bool compileSkinMesh(SkinedRenderable* pRenderable, const Vector2i& pos, const Vector2i& size) = 0;
	virtual void updateBones(EntityLivingBase* pLiving, float rdt, Vector4* outMatrices);

	virtual float getDeathMaxRotation(EntityLivingBase* pLiving) { return 90.0F; }

	static void initialize();
	static void unInitialize();
};

}

#endif