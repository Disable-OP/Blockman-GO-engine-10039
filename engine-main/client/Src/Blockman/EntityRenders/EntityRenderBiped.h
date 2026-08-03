/********************************************************************
filename: 	RenderBiped.h
file path:	dev\client\Src\Blockman\RenderEntity

version:	1
author:		ajohn
company:	supernano
date:		2017-3-13
*********************************************************************/
#ifndef __ENTITY_RENDER_BIPED_HEADER__
#define __ENTITY_RENDER_BIPED_HEADER__

#include "EntityRenderLiving.h"

namespace BLOCKMAN
{

class ModelBiped;
class cItemArmor;
class EntityLiving;
class ItemStack;

class EntityRenderBiped : public EntityRenderLiving
{
protected:
	ModelBiped* modelBipedMain = nullptr;

	typedef map<String, Texture*>::type LoadedTextureMap;
	static LoadedTextureMap armorTextureMap;

	static const int ARMOR_COUNT = 5;
	/** List of armor texture filenames. */
	static String bipedArmorFilenamePrefix[ARMOR_COUNT]; // = new String[]{ "leather", "chainmail", "iron", "diamond", "gold" };

protected:
	void createModels();	// func_82421_b
	void setTranslation();	// func_82422_c
	
	virtual int shouldRenderPass(EntityLivingBase* pLiving, int armorSlot, float ticks);
	virtual void preRenderArmor(EntityLivingBase* pLiving, int armorSlot, float ticks);
	virtual void renderEquippedItems(EntityLivingBase* pLiving, float ticks);

public:
	EntityRenderBiped(ModelBiped* pModelBiped, float size);
	
	static Texture* getArmorTexture(cItemArmor* pArmor, int par1, const String& external=StringUtil::BLANK);

	virtual bool compileSkinMesh(SkinedRenderable* pRenderable, const Vector2i& pos, const Vector2i& size) override;
	virtual void updateBones(EntityLivingBase* pLiving, float rdt, Vector4* outMatrices) override;

	/** implement override functions from RenderLiving */
	virtual void doRenderLiving(EntityLiving* pLiving, const Vector3& pos, float yaw, float ticks) override;
	virtual void renderPlayer(EntityLivingBase* pLiving, const Vector3& pos, float yaw, float ticks) override;
	virtual void doRender(Entity* pEntity, const Vector3& pos, float yaw, float ticks) override;
};

}

#endif