#include "EntityRenderItem.h"
#include "EntityRenderManager.h"
#include "bm_clientDef.h"

#include "cItem/cItem.h"
#include "cBlock/cBlock.h"
#include "cBlock/cBlockManager.h"
#include "Model/ItemModelMgr.h"
#include "Render/RenderBlocks.h"
#include "Render/TextureAtlas.h"
#include "Render/ItemRender.h"
#include "Render/RenderEntity.h"
#include "cWorld/Blockman.h"

#include "Block/Block.h"
#include "Block/BlockManager.h"
#include "Item/ItemStack.h"
#include "Item/Item.h"
#include "Entity/EntityItem.h"
#include "World/GameSettings.h"
#include "Util/Random.h"

#include "Tessolator/ItemRenderable.h"
#include "Tessolator/EnchantRenderQueue.h"
#include "Tessolator/TessManager.h"
#include "Scene/SceneNode.h"
#include "Scene/ModelEntity.h"
#include "Actor/ActorManager.h"

namespace BLOCKMAN
{

bool EntityRenderItem::renderInFrame = false;
Texture* EntityRenderItem::field_110798_h = NULL; // = new ResourceLocation("textures/misc/enchanted_item_glint.png");

EntityRenderItem::EntityRenderItem()
	: renderWithColor(true)
	, zLevel(0.f)
{
	shadowSize = 0.15F;
	shadowOpaque = 0.75F;
	random = LordNew Random();
	texBlock = TextureManager::Instance()->createTexture("Blocks.png");
	texItem = TextureManager::Instance()->createTexture("items.png");
	texBlock->load();
	texItem->load();
}

EntityRenderItem::~EntityRenderItem()
{
	TextureManager::Instance()->releaseResource(texBlock);
	TextureManager::Instance()->releaseResource(texItem);
	texBlock = NULL;
	texItem = NULL;
	LordSafeDelete(random);
}

void EntityRenderItem::doRenderItem(EntityItem* pEntityItem, const Vector3& pos, float yaw, float rdt)
{
	random->setSeed(187L);
	ItemStackPtr pStack = pEntityItem->getEntityItem();
	Matrix4 matItem = Matrix4::IDENTITY;
	Matrix4 matTemp = Matrix4::IDENTITY;

	if (pStack->getItem())
	{
		float f1 = Math::Sin_tab(((float)pEntityItem->age + rdt) / 10.0F + pEntityItem->hoverStart) * 0.1F + 0.1F;
		float selfRot = ((float)pEntityItem->age + rdt) / 20.0F + pEntityItem->hoverStart;
		int scount = 1;
		if (pStack->stackSize > 1)	scount = 2;
		if (pStack->stackSize > 5)	scount = 3;
		if (pStack->stackSize > 20)	scount = 4;
		if (pStack->stackSize > 40)	scount = 5;

		matItem.translate(pos.x, pos.y + f1, pos.z);
		
		/** get the brightness from light map. */
		int brightUV = pEntityItem->getBrightnessForRender(0.f);
		int brightness = Blockman::Instance()->m_entityRender->getLightMapColor(brightUV);

		int blockID = -1;
		if (auto itemBlock = dynamic_cast<ItemBlock*>(pStack->getItem()))
		{
			blockID = itemBlock->getBlockID();
		}
		
		if (ItemClient::getRenderType(pStack) == ItemRenderType::MESH)
		{
			LORD::ModelEntity* model = EntityRenderManager::Instance()->getMeshItem(pEntityItem);
			SceneNode* node = model->getSceneNode();
			float scale = 1.6f;
			matTemp.identity(); matTemp.scale(scale, scale, scale);
			matItem.joint(matTemp);
			matTemp.identity(); matTemp.rotateY(selfRot);
			matItem.joint(matTemp);
			matTemp.identity(); matTemp.rotateX(-Math::PI_DIV2);
			matItem.joint(matTemp);
			node->setWorldMatrix(matItem);
			model->setBrightness(brightness);
			model->update();
			model->updateRenderQueue(ActorManager::m_ActorRenderQueue);
		}
		else if (ItemClient::getRenderType(pStack) == ItemRenderType::BLOCK &&
			blockID > 0 &&
			blockID < BlockManager::MAX_BLOCK_COUNT &&
			cBlockManager::scBlocks[blockID] != NULL &&
			RenderBlocks::renderItemIn3d(cBlockManager::scBlocks[blockID]->getRenderType()))
		{
			cBlock* pcBlock = cBlockManager::scBlocks[blockID];

			matTemp.rotateY(selfRot);
			matItem.joint(matTemp);

			if (renderInFrame)
			{
				matTemp.identity(); matTemp.scale(1.25f, 1.25f, 1.25f);
				matItem.joint(matTemp);
				matTemp.identity(); matTemp.translate(0.f, 0.05f, 0.f);
				matItem.joint(matTemp);
				matTemp.identity(); matTemp.rotateY(-Math::PI_DIV2);
				matItem.joint(matTemp);
			}

			float scale = 0.25F;
			int renderType = pcBlock->getRenderType();

			if (renderType == RENDER_TYPE_FLOWER || 
				renderType == RENDER_TYPE_STEM ||
				renderType == RENDER_TYPE_LEVER ||
				renderType == RENDER_TYPE_TORCH)
			{
				scale = 0.5F;
			}

			matTemp.identity(); matTemp.scale(scale, scale, scale);
			matItem.joint(matTemp);

			for (int i = 0; i < scount; ++i)
			{
				if (i > 0)
				{
					float tx = (random->nextFloat() * 2.0F - 1.0F) * 0.2F / scale;
					float ty = (random->nextFloat() * 2.0F - 1.0F) * 0.2F / scale;
					float tz = (random->nextFloat() * 2.0F - 1.0F) * 0.2F / scale;
					matTemp.identity(); matTemp.translate(tx, ty, tz);
					matItem.joint(matTemp);
				}

				ItemBlockMesh* pBlockMesh = ItemModelManager::Instance()->getItemBlockMesh(blockID, pStack->getItemDamage());
				int quadNum = pBlockMesh->getQuadNum();
				PackMassRenderable* pPackRenderable = EntityRenderManager::Instance()->getPackRenderable(pBlockMesh->getTexture(), SRP_SOLID, quadNum);
				pPackRenderable->addItemBlock(pBlockMesh, brightness, matItem);

				if (pStack->hasEffect())
				{
					PackMassRenderable* pEnchant = EntityRenderManager::Instance()->getPackRenderable(ItemRenderer::s_texEnchantment, SRP_ENCHANT, quadNum);
					AtlasSprite* pAtlas = pcBlock->getSpriteFromSide(0);
					Vector4 vAtlas = pAtlas->getAtlasVec4();
					pEnchant->setTextureAtlas(vAtlas);
					Color c(brightness);
					c *= ItemRenderer::s_colorEnch_block;
					pEnchant->addItemBlock(pBlockMesh, c.getABGR(), matItem);
				}
			}
		}
		else
		{
			if (renderInFrame)
			{
				float scale = 0.5128205F;
				matTemp.identity(); matTemp.scale(scale, scale, scale);
				matItem.joint(matTemp);
				matTemp.identity(); matTemp.translate(0.f, -0.05f, 0.f);
				matItem.joint(matTemp);
			}
			else
			{
				float scale = 0.5f;
				matTemp.identity(); matTemp.scale(scale, scale, scale);
				matItem.joint(matTemp);
			}
			matTemp.identity(); matTemp.rotateY(selfRot);
			matItem.joint(matTemp);

			ItemRenderMesh* pItemRenderMesh = ItemModelManager::Instance()->getItemTesselation(pStack, NULL);
			ItemRenderable* pItemRenderable = EntityRenderManager::Instance()->getItemRenderable();
			pItemRenderable->setMaterialParam(NULL, IRP_SOLID);
			LordAssert(pItemRenderMesh);
			pItemRenderable->setMesh(pItemRenderMesh);
			pItemRenderable->setWorldMatrix(matItem);
			pItemRenderable->setBrightness(brightness);
			
			if (pStack->hasEffect())
			{
				ItemRenderable* pEnchant = EntityRenderManager::Instance()->getItemRenderable();
				LordAssert(pItemRenderMesh);
				pEnchant->setMesh(pItemRenderMesh);
				pEnchant->setMaterialParam(ItemRenderer::s_texEnchantment, IRP_ENCHANT);
				pEnchant->setWorldMatrix(matItem);
				pEnchant->setBrightness(Color(brightness) * ItemRenderer::s_colorEnch_item);
			}
		}
	}
}

void EntityRenderItem::doRender(Entity* pEntity, const Vector3& pos, float yaw, float rdt)
{
	doRenderItem((EntityItem*)pEntity, pos, yaw, rdt);
}

}