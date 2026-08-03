#include "EntityRenderLivingBase.h"
#include "EntityRenderManager.h"

#include "Model/ModelBase.h"
#include "Model/ModelBox.h"
#include "Model/ModelBiped.h"
#include "Model/ModelRenderer.h"
#include "Entity/EntityLivingBase.h"
#include "Entity/EntityPlayer.h"
#include "Entity/EntityArrow.h"
#include "Util/Random.h"
#include "cWorld/Blockman.h"

namespace BLOCKMAN
{

Texture* EntityRendererLivingBase::TEXTURE_BRIGHTNESS = NULL;

void EntityRendererLivingBase::initialize()
{
	TEXTURE_BRIGHTNESS = TextureManager::Instance()->createTexture("enchanted_item_glint.png");
	TEXTURE_BRIGHTNESS->load();
}

void EntityRendererLivingBase::unInitialize()
{
	TextureManager::Instance()->releaseResource(TEXTURE_BRIGHTNESS);
	TEXTURE_BRIGHTNESS = NULL;
}

EntityRendererLivingBase::EntityRendererLivingBase(ModelBase* pModel, float shadowsize)
	: mainModel(pModel)
{
	shadowSize = shadowsize;
}

EntityRendererLivingBase::~EntityRendererLivingBase()
{
	LordSafeDelete(mainModel);
}

float EntityRendererLivingBase::interpolateRotation(float angle1, float angle2, float interval)
{
	float angle;

	for (angle = angle2 - angle1; angle < -180.0F; angle += 360.0F)
		;

	while (angle >= 180.0F)
		angle -= 360.0F;

	return angle1 + interval * angle;
}

void EntityRendererLivingBase::renderLivingBase(EntityLivingBase* pLiving, const Vector3& pos, float yaw, float ticks)
{
	
}

void EntityRendererLivingBase::renderModel(EntityLivingBase* pLiving, float limbSwing, float swingAmount, float ticks, float headYaw, float headPitch)
{
	
}

void EntityRendererLivingBase::renderLivingAt(EntityLivingBase* pLiving, const Vector3& pos)
{

}

void EntityRendererLivingBase::rotateCorpse(EntityLivingBase* pLiving, float pitch, float yaw, float ticks)
{	
	
}

float EntityRendererLivingBase::renderSwingProgress(EntityLivingBase* pLiving, float rdt)
{
	return pLiving->getSwingProgress(rdt);
}

float EntityRendererLivingBase::handleRotationFloat(EntityLivingBase* pLiving, float ticks)
{
	return pLiving->ticksExisted + ticks;
}

void EntityRendererLivingBase::renderArrowsStuckInEntity(EntityLivingBase* pLiving, float ticks)
{
	
}

void EntityRendererLivingBase::passSpecialRender(EntityLivingBase* pLiving, const Vector3& pos)
{
	if (!canRenderName(pLiving))
		return;
	
	float var8 = 1.6F;
	float scale = 0.016666668F * var8;
	float disSqr = pLiving->getDistanceSqToEntity(renderManager->livingPlayer);
	float viewDis = pLiving->isSneaking() ? 32.0F : 64.0F;

	if (disSqr < (double)(viewDis * viewDis))
	{
		String name = pLiving->getTranslatedEntityName();

		if (pLiving->isSneaking())
		{
			// todo.
			/*FontRenderer var14 = getFontRendererFromRenderManager();
			GL11.glPushMatrix();
			GL11.glTranslatef((float)par2 + 0.0F, (float)par4 + pLiving.height + 0.5F, (float)par6);
			GL11.glNormal3f(0.0F, 1.0F, 0.0F);
			GL11.glRotatef(-renderManager.playerViewY, 0.0F, 1.0F, 0.0F);
			GL11.glRotatef(renderManager.playerViewX, 1.0F, 0.0F, 0.0F);
			GL11.glScalef(-var9, -var9, var9);
			GL11.glDisable(GL11.GL_LIGHTING);
			GL11.glTranslatef(0.0F, 0.25F / var9, 0.0F);
			GL11.glDepthMask(false);
			GL11.glEnable(GL11.GL_BLEND);
			GL11.glBlendFunc(GL11.GL_SRC_ALPHA, GL11.GL_ONE_MINUS_SRC_ALPHA);
			Tessellator var15 = Tessellator.instance;
			GL11.glDisable(GL11.GL_TEXTURE_2D);
			var15.startDrawingQuads();
			int var16 = var14.getStringWidth(var13) / 2;
			var15.setColorRGBA_F(0.0F, 0.0F, 0.0F, 0.25F);
			var15.addVertex((double)(-var16 - 1), -1.0D, 0.0D);
			var15.addVertex((double)(-var16 - 1), 8.0D, 0.0D);
			var15.addVertex((double)(var16 + 1), 8.0D, 0.0D);
			var15.addVertex((double)(var16 + 1), -1.0D, 0.0D);
			var15.draw();
			GL11.glEnable(GL11.GL_TEXTURE_2D);
			GL11.glDepthMask(true);
			var14.drawString(var13, -var14.getStringWidth(var13) / 2, 0, 553648127);
			GL11.glEnable(GL11.GL_LIGHTING);
			GL11.glDisable(GL11.GL_BLEND);
			GL11.glColor4f(1.0F, 1.0F, 1.0F, 1.0F);
			GL11.glPopMatrix();*/
		}
		else
		{
			renderLabel(pLiving, pos, name, scale, disSqr);
		}
	}
}

bool EntityRendererLivingBase::canRenderName(EntityLivingBase* pLiving)
{
	// todo.
	// return Blockman.isGuiEnabled() && pLiving != renderManager.livingPlayer && !pLiving.func_98034_c(Blockman.getBlockman().thePlayer) && pLiving.riddenByEntity == null;
	return false;
}

void EntityRendererLivingBase::renderLabel(EntityLivingBase* pLiving, const Vector3& pos, const String& label, float scale, float dissqr)
{
	if (pLiving->isPlayerSleeping())
	{
		Vector3 pos1 = pos;
		pos1.y -= 1.5f;
		renderLivingLabel(pLiving, label, pos1, 64);
	}
	else
	{
		renderLivingLabel(pLiving, label, pos, 64);
	}
}

void EntityRendererLivingBase::renderLivingLabel(EntityLivingBase* pLiving, const String& label, const Vector3& pos, int mindis)
{
	float disSqr = pLiving->getDistanceSqToEntity(renderManager->livingPlayer);

	if (disSqr <= (double)(mindis * mindis))
	{
		// todo.
		/*FontRenderer var12 = getFontRendererFromRenderManager();
		float var13 = 1.6F;
		float var14 = 0.016666668F * var13;
		GL11.glPushMatrix();
		GL11.glTranslatef((float)par3 + 0.0F, (float)par5 + pLiving.height + 0.5F, (float)par7);
		GL11.glNormal3f(0.0F, 1.0F, 0.0F);
		GL11.glRotatef(-renderManager.playerViewY, 0.0F, 1.0F, 0.0F);
		GL11.glRotatef(renderManager.playerViewX, 1.0F, 0.0F, 0.0F);
		GL11.glScalef(-var14, -var14, var14);
		GL11.glDisable(GL11.GL_LIGHTING);
		GL11.glDepthMask(false);
		GL11.glDisable(GL11.GL_DEPTH_TEST);
		GL11.glEnable(GL11.GL_BLEND);
		GL11.glBlendFunc(GL11.GL_SRC_ALPHA, GL11.GL_ONE_MINUS_SRC_ALPHA);
		Tessellator var15 = Tessellator.instance;
		byte var16 = 0;

		if (label.equals("deadmau5"))
		{
			var16 = -10;
		}

		GL11.glDisable(GL11.GL_TEXTURE_2D);
		var15.startDrawingQuads();
		int var17 = var12.getStringWidth(label) / 2;
		var15.setColorRGBA_F(0.0F, 0.0F, 0.0F, 0.25F);
		var15.addVertex((double)(-var17 - 1), (double)(-1 + var16), 0.0D);
		var15.addVertex((double)(-var17 - 1), (double)(8 + var16), 0.0D);
		var15.addVertex((double)(var17 + 1), (double)(8 + var16), 0.0D);
		var15.addVertex((double)(var17 + 1), (double)(-1 + var16), 0.0D);
		var15.draw();
		GL11.glEnable(GL11.GL_TEXTURE_2D);
		var12.drawString(label, -var12.getStringWidth(label) / 2, var16, 553648127);
		GL11.glEnable(GL11.GL_DEPTH_TEST);
		GL11.glDepthMask(true);
		var12.drawString(label, -var12.getStringWidth(label) / 2, var16, -1);
		GL11.glEnable(GL11.GL_LIGHTING);
		GL11.glDisable(GL11.GL_BLEND);
		GL11.glColor4f(1.0F, 1.0F, 1.0F, 1.0F);
		GL11.glPopMatrix();*/
	}
}

void EntityRendererLivingBase::doRender(Entity* pEnitity, const Vector3& pos, float yaw, float ticks)
{
	renderLivingBase((EntityLivingBase*)pEnitity, pos, yaw, ticks);
}

void EntityRendererLivingBase::updateBones(EntityLivingBase* pLiving, float rdt, Vector4* outMatrices)
{
	mainModel->swingProgress = renderSwingProgress(pLiving, rdt);

	float limbYaw = pLiving->prevLimbYaw + (pLiving->limbYaw - pLiving->prevLimbYaw) * rdt;
	float limbSwing = pLiving->limbSwing - pLiving->limbYaw * (1.0F - rdt);
	float headPitch = pLiving->prevRotationPitch + (pLiving->rotationPitch - pLiving->prevRotationPitch) * rdt;
	float yawOffset = interpolateRotation(pLiving->prevRenderYawOffset, pLiving->renderYawOffset, rdt);
	float yawHead = interpolateRotation(pLiving->prevRotationYawHead, pLiving->rotationYawHead, rdt);
	float yawDelta = handleRotationFloat(pLiving, rdt);
	
	ModelBiped* pBiped = (ModelBiped*)(mainModel);
	pBiped->setRotationAngles(limbSwing, limbYaw, yawDelta, yawHead - yawOffset, headPitch, pLiving);
	pBiped->updateBones(outMatrices);
}

}