/********************************************************************
filename: 	RenderPlayer.h
file path:	dev\client\Src\Blockman\RenderEntity

version:	1
author:		ajohn
company:	supernano
date:		2017-3-13
*********************************************************************/
#ifndef __ENTITY_RENDER_PLAYER_HEADER__
#define __ENTITY_RENDER_PLAYER_HEADER__

#include "EntityRenderLiving.h"

namespace BLOCKMAN
{

class ModelBiped;
class EntityPlayer;
class EntityPlayerSP;
class AtlasSprite;

class EntityRenderPlayer : public EntityRenderLiving
{
protected:

	static Texture* tex_default_player; // = new ResourceLocation("textures/entity/boy.png");
	ModelBiped* modelBipedMain = nullptr;
	ModelBiped* modelArmor = nullptr;
	ModelBiped* modelArmorPants = nullptr;
	AtlasSprite* bgBloodStrip = nullptr;
	AtlasSprite* pbBloodStrip = nullptr;
	
protected:
	/** render the player*/
	void renderPlayer_impl(EntityPlayer* pPlayer, const Vector3& pos, float yaw, float ticks);
	void renderBloodStrip(EntityPlayer* pPlayer, const Vector3& pos, float yaw, float rdt);

	/** implement override functions from RenderLivingBase */
	virtual void renderLabel(EntityLivingBase* pLiving, const Vector3& pos, const String& label, float scale, float dissqr);
	virtual void rotateCorpse(EntityLivingBase* pLiving, float pitch, float yaw, float ticks);
	virtual void preRenderCallback(EntityLivingBase* pLiving, float ticks);
	virtual void renderLivingAt(EntityLivingBase* pLiving, const Vector3& pos);
	virtual void preRenderArmor(EntityLivingBase* pLiving, int armorSlot, float ticks);
	virtual int shouldRenderPass(EntityLivingBase* pLiving, int armorSlot, float ticks);
	virtual void renderEquippedItems(EntityLivingBase* pLiving, float ticks);


public:
	EntityRenderPlayer();
	virtual ~EntityRenderPlayer();

    static void unInitialize();
	static void setDefaultTex(const String& filename);
	void renderFirstPersonArm(EntityPlayer* pPlayer, const Matrix4& mat);
	void renderHoldItem(EntityPlayer* pPlayer, const Matrix4& worldMat, const Matrix4& raMat, float rdt);
	void renderHoldItem_actor(EntityPlayer* pPlayer, const Matrix4& worldMat, float rdt);

	/** implement override functions from RenderLiving */
	virtual void doRender(Entity* pEntity, const Vector3& pos, float yaw, float ticks);
	virtual void doRender_gui(Entity* pEntity, const Matrix4& worldmat, const Matrix4& viewprojMat);
	virtual void renderPlayer(EntityLivingBase* pLiving, const Vector3& pos, float yaw, float ticks);
	virtual void updateBones(EntityLivingBase* pLiving, float rdt, Vector4* outMatrices);
	virtual bool compileSkinMesh(SkinedRenderable* pRenderable, const Vector2i& pos, const Vector2i& size);

	bool fillArmorSkinMesh(SkinedRenderable* pRenderable, int aromrSlot, const Vector2i& pos, const Vector2i& size, ui32 color = 0xFFFFFF);

	static Texture* getDefaultTexture() { return tex_default_player; }

private:
	void playAction(EntityPlayer* pPlayer, ActorObject* actor);
	void playBaseAction(EntityPlayer* pPlayer, ActorObject* actor, int holdItemId);
	void playUpperAction(EntityPlayer* pPlayer, ActorObject* actor, int holdItemId, bool bForce);
};

}

#endif
