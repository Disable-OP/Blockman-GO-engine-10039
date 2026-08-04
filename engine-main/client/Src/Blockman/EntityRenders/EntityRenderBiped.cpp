#include "EntityRenderBiped.h"
#include "EntityRenderManager.h"

#include "cItem/cItem.h"

#include "Entity/EntityLivingBase.h"
#include "Model/ModelBiped.h"
#include "Model/ModelRenderer.h"
#include "Item/Item.h"
#include "Item/Items.h"
#include "Item/ItemStack.h"
#include "Entity/EntityLiving.h"
#include "Render/RenderBlocks.h"
#include "Block/Block.h"
#include "Block/BlockManager.h"
#include "Render/ItemRender.h"

namespace BLOCKMAN
{
	
EntityRenderBiped::LoadedTextureMap EntityRenderBiped::armorTextureMap;
String EntityRenderBiped::bipedArmorFilenamePrefix[ARMOR_COUNT] = { "leather", "chainmail", "iron", "diamond", "gold" };

EntityRenderBiped::EntityRenderBiped(ModelBiped* pModelBiped, float size)
	: EntityRenderLiving(pModelBiped, size)
{
	modelBipedMain = pModelBiped;
	createModels();
}

Texture* EntityRenderBiped::getArmorTexture(cItemArmor* pArmor, int par1, const String& par2Str)
{
	String suffix = StringUtil::BLANK;
	if(!par2Str.empty())
		suffix = StringUtil::Format("_%s", par2Str.c_str());
	
	String fileName = bipedArmorFilenamePrefix[pArmor->renderIndex];
	fileName += "_layer_";
	int layer = par1 == 2 ? 2 : 1;
	fileName += StringUtil::ToString(layer);
	fileName += suffix;
	fileName += ".png";
	
	LoadedTextureMap::iterator it = armorTextureMap.find(fileName);
	if (it == armorTextureMap.end())
	{
		Texture* pTex = TextureManager::Instance()->createTexture(fileName);
		pTex->prepare();
		pTex->load();

		armorTextureMap.insert(std::make_pair(fileName, pTex));
		return pTex;
	}

	return it->second;
}

void EntityRenderBiped::createModels()
{
	// modelElytra = LordNew ModelBiped(1.0F);
	// modelHoldItem = LordNew ModelBiped(0.5F);
}

void EntityRenderBiped::doRenderLiving(EntityLiving* pLiving, const Vector3& pos, float yaw, float ticks)
{
}

void EntityRenderBiped::setTranslation()
{
}

void EntityRenderBiped::preRenderArmor(EntityLivingBase* pLivingBase, int armorSlot, float ticks)
{
	
}

int EntityRenderBiped::shouldRenderPass(EntityLivingBase* pLivingBase, int armorSlot, float ticks)
{
	return -1;
}

void EntityRenderBiped::renderEquippedItems(EntityLivingBase* pLivingBase, float ticks)
{
	
}

void EntityRenderBiped::renderPlayer(EntityLivingBase* pLiving, const Vector3& pos, float yaw, float ticks)
{
	doRenderLiving((EntityLiving*)pLiving, pos, yaw, ticks);
}

void EntityRenderBiped::doRender(Entity* pEntity, const Vector3& pos, float yaw, float ticks)
{
	doRenderLiving((EntityLiving*)pEntity, pos, yaw, ticks);
}

bool EntityRenderBiped::compileSkinMesh(SkinedRenderable* pRenderable, const Vector2i& pos, const Vector2i& size)
{
	// todo.
	return false;
}

void EntityRenderBiped::updateBones(EntityLivingBase* pLiving, float rdt, Vector4* outMatrices)
{
	// todo.
}

}