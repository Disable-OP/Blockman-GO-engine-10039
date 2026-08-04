#include "ItemRender.h"
#include "RenderBlocks.h"
#include "TextureAtlas.h"

#include "cBlock/cBlock.h"
#include "cBlock/cBlockManager.h"
#include "cItem/cItem.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Model/ItemModelMgr.h"
#include "EntityRenders/EntityRenderManager.h"
#include "EntityRenders/EntityRenderPlayer.h"
#include "EntityRenders/EntityRenderable.h"
#include "Render/RenderEntity.h"
#include "cWorld/Blockman.h"

#include "Block/Block.h"
#include "Block/BlockManager.h"
#include "Item/Items.h"
#include "Item/ItemStack.h"
#include "Inventory/InventoryPlayer.h"
#include "Entity/EntityLivingBase.h"
#include "World/World.h"

#include "Tessolator/ItemRenderable.h"
#include "Tessolator/TessManager.h"
#include "Scene/SceneManager.h"
#include "Scene/SceneNode.h"
#include "Model/MeshManager.h"
#include "Model/Mesh.h"
#include "Scene/ModelEntity.h"
#include "Actor/ActorManager.h"


namespace BLOCKMAN
{

Texture* ItemRenderer::s_texEnchantment = NULL;
//Texture* ItemRenderer::field_110931_c = NULL;
//Texture* ItemRenderer::field_110929_d = NULL;
Color	ItemRenderer::s_colorEnch_block(0.12f, 0.06f, 0.2f, 1.f);
Color	ItemRenderer::s_colorEnch_equip(0.38f, 0.19f, 0.6f, 1.f);
Color	ItemRenderer::s_colorEnch_item(0.38f, 0.19f, 0.6f, 1.f);

void ItemRenderer::initialize()
{
	s_texEnchantment = TextureManager::Instance()->createTexture("enchanted_item_glint.png");
	s_texEnchantment->load();
	//field_110931_c = TextureManager::Instance()->createTexture("map_background.png");
	//field_110931_c->load();
	//field_110929_d = TextureManager::Instance()->createTexture("underwater.png");
	//field_110929_d->load();
}

void ItemRenderer::unInitialize()
{
	TextureManager::Instance()->releaseResource(s_texEnchantment);
	//TextureManager::Instance()->releaseResource(field_110931_c);
	//TextureManager::Instance()->releaseResource(field_110929_d);
}

ItemRenderer::ItemRenderer(Blockman* par1Blockman)
	: bm(par1Blockman)
	, itemToRender(NULL)
	, equippedProgress(0.f)
	, prevEquippedProgress(0.f)
	, equippedItemSlot(-1)
	, m_rightArmRenderable(NULL)
	, m_rightBlockRenderable(NULL)
	, m_rightBlockEnchantRenderable(NULL)
	, m_rightItemRenderable(NULL)
	, m_rightItemMesh(NULL)
	, m_rightItemEnchantRenderable(NULL)
	, m_fireEffectsRenderable(NULL)
{
	// mapItemRenderer = new MapItemRenderer(par1Blockman.gameSettings, par1Blockman.func_110434_K());
	renderBlocksInstance = LordNew RenderBlocks();
	renderManager = EntityRenderManager::Instance();
	m_color = 0xFFFFFFFF;
	m_lightUV = Vector2(0.f, 0.f);
}

ItemRenderer::~ItemRenderer()
{
	destroyRightItemMesh();
	LordSafeDelete(renderBlocksInstance);
	LordSafeDelete(m_rightArmRenderable);
	LordSafeDelete(m_rightBlockRenderable);
	LordSafeDelete(m_rightBlockEnchantRenderable);
	LordSafeDelete(m_rightItemRenderable);
	LordSafeDelete(m_rightItemEnchantRenderable);
	LordSafeDelete(m_fireEffectsRenderable);
}

void ItemRenderer::renderItem(EntityLivingBase* pLiving, ItemStackPtr pStack, int par3)
{
	
}

void ItemRenderer::renderItemIn2D(float tu_min, float tu_max, float tv_min, float tv_max, int sizeU, int sizeV, float anti_gap)
{
	
}

void ItemRenderer::renderItemInFirstPerson(float rdt)
{
	float equipProgress = prevEquippedProgress + (equippedProgress - prevEquippedProgress) * rdt;
	EntityPlayerSP* pPlayer = bm->m_pPlayer;

	ItemStackPtr pStack = pPlayer->getArmStack();

	// render the hold item.
	if (!pStack)
	{
		pStack = itemToRender;
	}

	BlockPos pos = pPlayer->position.getFloor();

	int brightUV = pPlayer->getBrightnessForRender(0.f);
	int brightness = Blockman::Instance()->m_entityRender->getLightMapColor(brightUV);
			
	if (pStack != NULL && ItemClient::citemsList[pStack->itemID])
	{
		m_color = ItemClient::citemsList[pStack->itemID]->getColorFromItemStack(pStack, 0);
	}
	
	float var14;
	float var15;
	float var16;
	float var20;
	EntityRender* var26 = NULL;
	EntityRenderPlayer* var28 = NULL;

	if (pStack != NULL && pStack->itemID == 102+256 /*Item::imap->itemID*/) // todo. the Item::imap is not valid
	{
		/*GL11.glPushMatrix();
		var20 = 0.8F;
		float swing = pPlayer->getSwingProgress(rdt);
		float b = Math::Sin(swing * Math::PI);
		float r = Math::Sin(Math::Sqrt(swing) * Math::PI);
		GL11.glTranslatef(-r * 0.4F, Math::Sin(Math::Sqrt(swing) * Math::PI_2) * 0.2F, -b * 0.2F);
		g = 1.0F - pitch / 45.0F + 0.1F;

		if (g < 0.0F)
		{
			g = 0.0F;
		}

		if (g > 1.0F)
		{
			g = 1.0F;
		}

		g = -MathHelper.cos(g * (float)Math.PI) * 0.5F + 0.5F;
		GL11.glTranslatef(0.0F, 0.0F * var20 - (1.0F - equipProgress) * 1.2F - g * 0.5F + 0.04F, -0.9F * var20);
		GL11.glRotatef(90.0F, 0.0F, 1.0F, 0.0F);
		GL11.glRotatef(g * -85.0F, 0.0F, 0.0F, 1.0F);
		GL11.glEnable(GL12.GL_RESCALE_NORMAL);
		bm.func_110434_K().func_110577_a(pPlayer.func_110306_p());

		for (light_v = 0; light_v < 2; ++light_v)
		{
			int var24 = light_v * 2 - 1;
			GL11.glPushMatrix();
			GL11.glTranslatef(-0.0F, -0.6F, 1.1F * (float)var24);
			GL11.glRotatef((float)(-45 * var24), 1.0F, 0.0F, 0.0F);
			GL11.glRotatef(-90.0F, 0.0F, 0.0F, 1.0F);
			GL11.glRotatef(59.0F, 0.0F, 0.0F, 1.0F);
			GL11.glRotatef((float)(-65 * var24), 0.0F, 1.0F, 0.0F);
			var26 = RenderManager.instance.getEntityRenderObject(bm.thePlayer);
			var28 = (RenderPlayer)var26;
			var16 = 1.0F;
			GL11.glScalef(var16, var16, var16);
			var28.renderFirstPersonArm(bm.thePlayer);
			GL11.glPopMatrix();
		}

		b = pPlayer.getSwingProgress(rdt);
		r = MathHelper.sin(b * b * (float)Math.PI);
		var14 = MathHelper.sin(MathHelper.sqrt_float(b) * (float)Math.PI);
		GL11.glRotatef(-r * 20.0F, 0.0F, 1.0F, 0.0F);
		GL11.glRotatef(-var14 * 20.0F, 0.0F, 0.0F, 1.0F);
		GL11.glRotatef(-var14 * 80.0F, 1.0F, 0.0F, 0.0F);
		var15 = 0.38F;
		GL11.glScalef(var15, var15, var15);
		GL11.glRotatef(90.0F, 0.0F, 1.0F, 0.0F);
		GL11.glRotatef(180.0F, 0.0F, 0.0F, 1.0F);
		GL11.glTranslatef(-1.0F, -1.0F, 0.0F);
		var16 = 0.015625F;
		GL11.glScalef(var16, var16, var16);
		bm.func_110434_K().func_110577_a(field_110931_c);
		Tessellator var29 = Tessellator.instance;
		GL11.glNormal3f(0.0F, 0.0F, -1.0F);
		var29.startDrawingQuads();
		byte var30 = 7;
		var29.addVertexWithUV((double)(0 - var30), (double)(128 + var30), 0.0D, 0.0D, 1.0D);
		var29.addVertexWithUV((double)(128 + var30), (double)(128 + var30), 0.0D, 1.0D, 1.0D);
		var29.addVertexWithUV((double)(128 + var30), (double)(0 - var30), 0.0D, 1.0D, 0.0D);
		var29.addVertexWithUV((double)(0 - var30), (double)(0 - var30), 0.0D, 0.0D, 0.0D);
		var29.draw();
		MapData var19 = Item.map.getMapData(item, bm.theWorld);

		if (var19 != null)
		{
			mapItemRenderer.renderMap(bm.thePlayer, bm.func_110434_K(), var19);
		}

		GL11.glPopMatrix();*/
	}
	else if (pStack != NULL && pStack->isFirstRender())
	{
		var20 = 0.8F;
		Matrix4 matItem = Matrix4::IDENTITY;
		Matrix4 temp = Matrix4::IDENTITY;
		Item* pItem = pStack->getItem();

		bool isMeshShove1 = false;
		bool isMeshShove2 = false;
		if (pItem && ((pItem->itemID >= Item::digShovelIron->itemID && pItem->itemID <= Item::shovelBigScooper->itemID) || (pItem->itemID == Item::shovelEngineerGold->itemID)))
			isMeshShove1 = true;
		if (pItem && pItem->itemID >= Item::shovelJackHammer->itemID && pItem->itemID < Item::shovelEngineerGold->itemID)
			isMeshShove2 = true;

		if(pItem && pStack->isGunItem() && pPlayer->m_isReloadBullet && pPlayer->m_reloadPregress > 0.f)
		{
			float r = pPlayer->getReloadProcess(rdt);
			float b = 1.0f - r;
			var15 = 1.0f - b * b * b * b * b;

			matItem.translate(0.0F, Math::Abs(Math::Cos(r * 8 / 4.0F * Math::PI) * 0.1F) * (r > 0.2f ? 1 : 0), 0.0F);
			temp.identity(); temp.translate(var15 * 0.6F, -var15 * 0.5F, 0.0F);
			matItem.joint(temp);
			temp.identity(); temp.rotateY(var15 * 90.f * Math::DEG2RAD);
			matItem.joint(temp);
			temp.identity(); temp.rotateX(var15 * 10.f * Math::DEG2RAD);
			matItem.joint(temp);
			temp.identity(); temp.rotateZ(var15 * 30.f * Math::DEG2RAD);
			matItem.joint(temp);
		}
		else if (pPlayer->getItemInUseCount() > 0)
		{
			ACTION_TYPE actionType = pStack->getItemUseAction();

			if (actionType == ACTION_TYPE_EAT || actionType == ACTION_TYPE_DRINK)
			{
				float b = (float)pPlayer->getItemInUseCount() - rdt + 1.0F;
				float r = 1.0F - b / (float)pStack->getMaxItemUseDuration();
				var14 = 1.0F - r;
				var14 = var14 * var14 * var14;
				var14 = var14 * var14 * var14;
				var14 = var14 * var14 * var14;
				var15 = 1.0F - var14;

				matItem.translate(0.0F, Math::Abs(Math::Cos(b / 4.0F * Math::PI) * 0.1F) * (r > 0.2f ? 1 : 0), 0.0F);
				temp.identity(); temp.translate(var15 * 0.6F, -var15 * 0.5F, 0.0F);
				matItem.joint(temp);
				temp.identity(); temp.rotateY(var15 * 90.f * Math::DEG2RAD);
				matItem.joint(temp);
				temp.identity(); temp.rotateX(var15 * 10.f * Math::DEG2RAD);
				matItem.joint(temp);
				temp.identity(); temp.rotateZ(var15 * 30.f * Math::DEG2RAD);
				matItem.joint(temp);
			}
		}
		else
		{
			float swings = pPlayer->getSwingProgress(rdt);
			if (pPlayer->isHoldGun)
			{
				float f1 = Math::Sin(swings * swings * Math::PI_DIV2);
				matItem.translate(0.f, 0.f, f1 * 0.04f);
			}
			else if (isMeshShove1)
			{
				float f1 = Math::Sin(swings * Math::PI);
				float f2 = Math::Sin(Math::Sqrt(swings) * Math::PI);
				matItem.translate(-f2 * 0.8F, Math::Sin(Math::Sqrt(swings) * Math::PI_2) * 0.6F, -f1 * 0.4F);
			}
			else if (isMeshShove2)
			{
				float f1 = Math::Sin(swings * swings * Math::PI_2*2);
				matItem.translate(0.f, 0.f, -f1 * 0.1f);
			}
			else
			{
				float f1 = Math::Sin(swings * Math::PI);
				float f2 = Math::Sin(Math::Sqrt(swings) * Math::PI);
				matItem.translate(-f2 * 0.4F, Math::Sin(Math::Sqrt(swings) * Math::PI_2) * 0.2F, -f1 * 0.2F);
			}
		}

		temp.identity(); temp.translate(0.7F * var20, -0.65F * var20 - (1.0F - equipProgress) * 0.6F, -0.9F * var20);
		matItem.joint(temp);
		temp.identity();
		if (pStack->itemID == BLOCK_ID_CHEST || pStack->itemID == BLOCK_ID_CHEST_NEW)
			temp.rotateY(Math::PI_DIV4 + Math::PI_DIV2); // add by zhouyou. the chest's snap is render when the render is no-depth test.
		else
			temp.rotateY(Math::PI_DIV4);
		matItem.joint(temp);

		float swings = pPlayer->getSwingProgress(rdt);
		float f1 = Math::Sin(swings * swings * Math::PI);
		if (pPlayer->isHoldGun)
		{
			temp.identity();
			temp.rotateX(f1 * 10.f * Math::DEG2RAD);
			matItem.joint(temp);
		}
		else if(isMeshShove1)
		{
			float f2 = Math::Sin(Math::Sqrt(swings) * Math::PI);
			Matrix4::RotateYawPitchRoll(temp, -f1 * 20.f * Math::DEG2RAD, f2 * 80.f * Math::DEG2RAD, -f2 * 20.f * Math::DEG2RAD);
			matItem.joint(temp);
		}
		else if (isMeshShove2)
		{

		}
		else
		{
			float f2 = Math::Sin(Math::Sqrt(swings) * Math::PI);
			Matrix4::RotateYawPitchRoll(temp, -f1 * 20.f * Math::DEG2RAD, -f2 * 80.f * Math::DEG2RAD, -f2 * 20.f * Math::DEG2RAD);
			matItem.joint(temp);
		}

		var14 = 0.4F;
		temp.identity(); temp.scale(var14, var14, var14);
		matItem.joint(temp);

		float var17;
		float var18;

		if (pPlayer->getItemInUseCount() > 0)
		{
			ACTION_TYPE actionType = pStack->getItemUseAction();

			if (actionType == ACTION_TYPE_BLOCK)
			{
				temp.identity(); temp.translate(-0.5F, 0.2F, 0.0F);
				matItem.joint(temp);

				//Matrix4::RotateYawPitchRoll(temp, Math::PI_DIV6, 70 * Math::DEG2RAD, Math::PI_DIV3);
				//matItem.joint(temp);
			}
			else if (actionType == ACTION_TYPE_BOW)
			{
				temp.identity(); temp.rotateZ(-18.f * Math::DEG2RAD);
				matItem.joint(temp);
				temp.identity(); temp.rotateY(-12.f * Math::DEG2RAD);
				matItem.joint(temp);
				temp.identity(); temp.rotateX(-8.f * Math::DEG2RAD);
				matItem.joint(temp);
				temp.identity(); temp.translate(-0.9F, 0.2F, 0.0F);
				matItem.joint(temp);

				var16 = (float)pStack->getMaxItemUseDuration() - ((float)pPlayer->getItemInUseCount() - rdt + 1.0F);
				var17 = var16 / 20.0F;
				var17 = (var17 * var17 + var17 * 2.0F) / 3.0F;

				if (var17 > 1.0F)
				{
					var17 = 1.0F;
				}

				if (var17 > 0.1F)
				{
					temp.identity(); temp.translate(0.0F, Math::Sin((var16 - 0.1F) * 1.3F) * 0.01F * (var17 - 0.1F), 0.0F);
					matItem.joint(temp);
				}


				temp.identity(); temp.translate(0.0F, 0.0F, var17 * 0.1F);
				matItem.joint(temp);
				temp.identity(); temp.rotateZ(-335.f * Math::DEG2RAD);
				matItem.joint(temp);
				temp.identity(); temp.rotateY(-50.f * Math::DEG2RAD);
				matItem.joint(temp);
				temp.identity(); temp.translate(0.0f, 0.5f, 0.0f);
				matItem.joint(temp);
				var18 = 1.0F + var17 * 0.2F;
				temp.identity(); temp.scale(1.f, 1.f, var18);
				matItem.joint(temp);
				temp.identity(); temp.translate(0.0f, -.5f, 0.0f);
				matItem.joint(temp);
				temp.identity(); temp.rotateY(50.f * Math::DEG2RAD);
				matItem.joint(temp);
				temp.identity(); temp.rotateZ(335.f * Math::DEG2RAD);
				matItem.joint(temp);
			}
		}

		int itemid = pStack->getItem()->itemID;
		if (ItemClient::citemsList[itemid]->shouldRotateAroundWhenRendering())
		{
			temp.identity(); temp.rotateY(Math::PI);
			matItem.joint(temp);
		}

		bool isItemBlock = false;
		int blockId = -1;
		if (auto itemBlock = dynamic_cast<ItemBlock*>(pStack->getItem()))
		{
			blockId = itemBlock->getBlockID();
			isItemBlock = (ItemClient::getRenderType(pStack) == ItemRenderType::BLOCK &&
				blockId < BlockManager::MAX_BLOCK_COUNT &&
				cBlockManager::scBlocks[blockId] != NULL &&
				RenderBlocks::renderItemIn3d(cBlockManager::scBlocks[blockId]->getRenderType()));
		}
		
		if (ItemClient::getRenderType(pStack) == ItemRenderType::MESH)
		{
			checkRightItemMesh(pStack);
			LordAssert(m_rightItemMesh);
			
			if (isMeshShove1)
			{
				temp.identity(); temp.translate(1.3f, -1.2f, 2.2f);
				matItem.joint(temp);
				temp.identity(); temp.scale(1.8f, 1.8f, 1.8f);
				matItem.joint(temp);
				temp.identity(); temp.rotateX(-150 * Math::DEG2RAD);
				matItem.joint(temp);
				temp.identity(); temp.rotateZ(50 * Math::DEG2RAD);
				matItem.joint(temp);
			}
			else if (isMeshShove2)
			{
				temp.identity(); temp.translate(-1.3f, 0.7f, -0.3f);
				matItem.joint(temp);
				temp.identity(); temp.scale(1.8f, 1.8f, 1.8f);
				matItem.joint(temp);
				temp.identity(); temp.rotateX(-120 * Math::DEG2RAD);
				matItem.joint(temp);
			}
			else
			{
				temp.identity(); temp.translate(-1.1f, 0.7f, 0.2f);
				matItem.joint(temp);
				temp.identity(); temp.scale(1.8f, 1.8f, 1.8f);
				matItem.joint(temp);
				temp.identity(); temp.rotateX(-Math::PI_DIV2);
				matItem.joint(temp);
				temp.identity(); temp.rotateZ(50 * Math::DEG2RAD);
				matItem.joint(temp);
			}

			SceneNode* pNode = m_rightItemMesh->getSceneNode();
			pNode->setWorldMatrix(matItem);
			m_rightItemMesh->setBrightness(brightness);
			if (!bm->m_pPlayer->m_bIsInAimSight)
			{
				m_rightItemMesh->update2();
				m_rightItemMesh->updateRenderQueue(ActorManager::m_ActorRenderQueue);
			}
		}
		else if (isItemBlock)
		{
			checkRightBlockRenderable(blockId, pStack->getItemDamage());
			ItemBlockMesh* pBlockMesh = ItemModelManager::Instance()->getItemBlockMesh(blockId, pStack->getItemDamage());
			const Box& ab = pBlockMesh->getAABB();
			temp.identity(); temp.translate(0.f, 0.5f-ab.vMax.y, 0.f);
			matItem.joint(temp);

			m_rightBlockRenderable->beginPolygon();
			m_rightBlockRenderable->addItemBlock(pBlockMesh, brightness, matItem);
			m_rightBlockRenderable->endPolygon();
			m_rightBlockRenderable->updateRenderQueue();

			if (pStack->hasEffect())
			{
				checkRightBlockEnchantRenderable(blockId, pStack->getItemDamage());
				m_rightBlockEnchantRenderable->beginPolygon();
				cBlock* pcBlock = cBlockManager::scBlocks[blockId];
				AtlasSprite* pAtlas = pcBlock->getSprite();
				Vector4 vAtlas = pAtlas->getAtlasVec4();
				m_rightBlockEnchantRenderable->setTextureAtlas(vAtlas);
				Color c(brightness);
				c *= s_colorEnch_block;
				m_rightBlockEnchantRenderable->addItemBlock(pBlockMesh, c.getABGR(), matItem);
				m_rightBlockEnchantRenderable->endPolygon();
				m_rightBlockEnchantRenderable->updateRenderQueue();
			}
		}
		else
		{
			checkRgihtItemRenderable(pStack);

			temp.identity(); temp.translate(-0.f, -0.3f, 0.f);
			matItem.joint(temp);
			float scale = 1.5F;
			temp.identity(); temp.scale(scale, scale, scale);
			matItem.joint(temp);
			temp.identity(); temp.rotateY(50.f * Math::DEG2RAD);
			matItem.joint(temp);
			temp.identity(); temp.rotateZ(335.f * Math::DEG2RAD);
			matItem.joint(temp);
			temp.identity(); temp.translate(-0.4375F, -0.0625F, 0.0F);
			matItem.joint(temp);
			temp.identity(); temp.rotateY(Math::PI);
			matItem.joint(temp);

			m_rightItemRenderable->setWorldMatrix(matItem);
			Blockman::Instance()->m_firstPersonItemMat = matItem;
			m_rightItemRenderable->setBrightness(brightness);
			TessManager::Instance()->addItemRenderalbe(m_rightItemRenderable);

			if (pStack->hasEffect())
			{
				checkRightItemEnchantRenderable(pStack);
				m_rightItemEnchantRenderable->setWorldMatrix(matItem);
				m_rightItemEnchantRenderable->setBrightness(Color(brightness) * s_colorEnch_item);
				TessManager::Instance()->addEnchantRenderable(m_rightItemEnchantRenderable);
			}
		}

	
		/*if (item->getItem()->requiresMultipleRenderPasses())
		{
			renderItem(pPlayer, item, 0);
			int var27 = Item::itemsList[item->itemID]->getColorFromItemStack(item, 1);
			r = (float)(var27 >> 16 & 255) / 255.0F;
			g = (float)(var27 >> 8 & 255) / 255.0F;
			b = (float)(var27 & 255) / 255.0F;
			Color color(r, g, b, 1.f);
			m_color = color.getARGB();
			renderItem(pPlayer, item, 1);
		}
		else
		{
			renderItem(pPlayer, item, 0);
		}*/
	}
	else if (!pPlayer->isInvisible())  // render the arm.
	{
		if (!m_rightArmRenderable)
			m_rightArmRenderable = LordNew RightArmRenderable();
		
		bool iszombie = pPlayer->isZombie();
		if (m_rightArmRenderable->getisZombie() != iszombie)
			m_rightArmRenderable->setZombie(iszombie);

		m_rightArmRenderable->setPlayer(bm->m_pPlayer);
		m_rightArmRenderable->update(equipProgress, rdt);
		m_rightArmRenderable->updateRender();
	}
}

void ItemRenderer::renderOverlays(float rdt)
{
	if (bm->m_pPlayer->isBurning())
	{
		renderFireInFirstPerson(rdt);
	}

	/*
	if (bm.thePlayer.isEntityInsideOpaqueBlock())
	{
		int var2 = MathHelper.floor_double(bm.thePlayer.posX);
		int var3 = MathHelper.floor_double(bm.thePlayer.posY);
		int var4 = MathHelper.floor_double(bm.thePlayer.posZ);
		int var5 = bm.theWorld.getBlockId(var2, var3, var4);

		if (bm.theWorld.isBlockNormalCube(var2, var3, var4))
		{
			renderInsideOfBlock(ticks, Block.blocksList[var5].getBlockTextureFromSide(2));
		}
		else
		{
			for (int var6 = 0; var6 < 8; ++var6)
			{
				float var7 = ((float)((var6 >> 0) % 2) - 0.5F) * bm.thePlayer.width * 0.9F;
				float var8 = ((float)((var6 >> 1) % 2) - 0.5F) * bm.thePlayer.height * 0.2F;
				float var9 = ((float)((var6 >> 2) % 2) - 0.5F) * bm.thePlayer.width * 0.9F;
				int var10 = MathHelper.floor_float((float)var2 + var7);
				int var11 = MathHelper.floor_float((float)var3 + var8);
				int var12 = MathHelper.floor_float((float)var4 + var9);

				if (bm.theWorld.isBlockNormalCube(var10, var11, var12))
				{
					var5 = bm.theWorld.getBlockId(var10, var11, var12);
				}
			}
		}

		if (Block.blocksList[var5] != null)
		{
			renderInsideOfBlock(ticks, Block.blocksList[var5].getBlockTextureFromSide(2));
		}
	}

	if (bm.thePlayer.isInsideOfMaterial(Material.water))
	{
		renderWarpedTextureOverlay(ticks);
	}

	GL11.glEnable(GL11.GL_ALPHA_TEST);
	*/
}

void ItemRenderer::renderInsideOfBlock(float rdt, AtlasSprite* sprite)
{
	/*bm.func_110434_K().func_110577_a(TextureMap.field_110575_b);
	Tessellator var3 = Tessellator.instance;
	float var4 = 0.1F;
	GL11.glColor4f(var4, var4, var4, 0.5F);
	GL11.glPushMatrix();
	float var5 = -1.0F;
	float var6 = 1.0F;
	float var7 = -1.0F;
	float var8 = 1.0F;
	float var9 = -0.5F;
	float var10 = sprite.getMinU();
	float var11 = sprite.getMaxU();
	float var12 = sprite.getMinV();
	float var13 = sprite.getMaxV();
	var3.startDrawingQuads();
	var3.addVertexWithUV((double)var5, (double)var7, (double)var9, (double)var11, (double)var13);
	var3.addVertexWithUV((double)var6, (double)var7, (double)var9, (double)var10, (double)var13);
	var3.addVertexWithUV((double)var6, (double)var8, (double)var9, (double)var10, (double)var12);
	var3.addVertexWithUV((double)var5, (double)var8, (double)var9, (double)var11, (double)var12);
	var3.draw();
	GL11.glPopMatrix();
	GL11.glColor4f(1.0F, 1.0F, 1.0F, 1.0F);*/
}

void ItemRenderer::renderWarpedTextureOverlay(float rdt)
{
	/*bm.func_110434_K().func_110577_a(field_110929_d);
	Tessellator var2 = Tessellator.instance;
	float var3 = bm.thePlayer.getBrightness(ticks);
	GL11.glColor4f(var3, var3, var3, 0.5F);
	GL11.glEnable(GL11.GL_BLEND);
	GL11.glBlendFunc(GL11.GL_SRC_ALPHA, GL11.GL_ONE_MINUS_SRC_ALPHA);
	GL11.glPushMatrix();
	float var4 = 4.0F;
	float var5 = -1.0F;
	float var6 = 1.0F;
	float var7 = -1.0F;
	float var8 = 1.0F;
	float var9 = -0.5F;
	float var10 = -bm.thePlayer.rotationYaw / 64.0F;
	float var11 = bm.thePlayer.rotationPitch / 64.0F;
	var2.startDrawingQuads();
	var2.addVertexWithUV((double)var5, (double)var7, (double)var9, (double)(var4 + var10), (double)(var4 + var11));
	var2.addVertexWithUV((double)var6, (double)var7, (double)var9, (double)(0.0F + var10), (double)(var4 + var11));
	var2.addVertexWithUV((double)var6, (double)var8, (double)var9, (double)(0.0F + var10), (double)(0.0F + var11));
	var2.addVertexWithUV((double)var5, (double)var8, (double)var9, (double)(var4 + var10), (double)(0.0F + var11));
	var2.draw();
	GL11.glPopMatrix();
	GL11.glColor4f(1.0F, 1.0F, 1.0F, 1.0F);
	GL11.glDisable(GL11.GL_BLEND);*/
}

void ItemRenderer::checkRightBlockRenderable(int blockID, int meta)
{
	int blockKey = (blockID << 8) | meta;

	if (!m_rightBlockRenderable)
	{
		m_rightBlockRenderable = LordNew PackMassRenderable(EntityRenderManager::Instance()->m_BlockTex, SRP_SOLID_NODEPTH);
		m_rightBlockRenderable->setViewSpace(true);
	}

	if (m_rightBlockRenderable->getLastBlockKey() == blockKey &&
		m_rightBlockRenderable->getBoneCount() == 1)
	{
		// reuse the item.
		return;
	}
	
	ItemBlockMesh* pBlockMesh = ItemModelManager::Instance()->getItemBlockMesh(blockID, meta);
	m_rightBlockRenderable->setMaterialParam(pBlockMesh->getTexture(), SRP_SOLID_NODEPTH);
	m_rightBlockRenderable->beginPolygon();
	m_rightBlockRenderable->addItemBlock(pBlockMesh, 0xFFFFFFFF, Matrix4::IDENTITY);
	m_rightBlockRenderable->endPolygon();
}

void ItemRenderer::checkRightBlockEnchantRenderable(int blockID, int meta)
{
	int blockKey = (blockID << 8) | meta;

	if (!m_rightBlockEnchantRenderable)
	{
		m_rightBlockEnchantRenderable = LordNew PackMassRenderable(s_texEnchantment, SRP_ENCHANT_NODEPTH);
		m_rightBlockEnchantRenderable->setViewSpace(true);
	}

	if (m_rightBlockEnchantRenderable->getLastBlockKey() == blockKey &&
		m_rightBlockEnchantRenderable->getBoneCount() == 1)
	{
		// reuse the item.
		return;
	}

	ItemBlockMesh* pBlockMesh = ItemModelManager::Instance()->getItemBlockMesh(blockID, meta);
	m_rightBlockEnchantRenderable->setMaterialParam(s_texEnchantment, SRP_ENCHANT_NODEPTH);
	m_rightBlockEnchantRenderable->beginPolygon();
	m_rightBlockEnchantRenderable->addItemBlock(pBlockMesh, 0xFFFFFFFF, Matrix4::IDENTITY);
	m_rightBlockEnchantRenderable->endPolygon();
}

void ItemRenderer::checkRgihtItemRenderable(ItemStackPtr pStack)
{
	LordAssert(pStack);
	if (!m_rightItemRenderable)
	{
		m_rightItemRenderable = LordNew ItemRenderable(IRP_SOLID_NODEPTH);
		m_rightItemRenderable->setViewSpace(true);
	}

	int itemKey = pStack->itemID;
	if (itemKey == Item::bow->itemID)
	{
		EntityPlayer* pPlayer = bm->m_pPlayer;
		cItemBow* pBow = (cItemBow*)Item::bow;
		if (pPlayer->itemInUse)
		{
			// if is the bow. the itemkey is  5 / 201 / 202 for different duration.
			itemKey = pBow->getItemKey(pPlayer->getItemInUseCount());
		}
	}
	else if (itemKey == Item::bow_red->itemID)
	{
		EntityPlayer* pPlayer = bm->m_pPlayer;
		cItemBow* pBow = (cItemBow*)Item::bow_red;
		if (pPlayer->itemInUse)
		{
			// if is the bow. the itemkey is  5 / 201 / 202 for different duration.
			itemKey = pBow->getItemKey(pPlayer->getItemInUseCount());
		}
	}
	else if (itemKey == Item::bow_black->itemID)
	{
		EntityPlayer* pPlayer = bm->m_pPlayer;
		cItemBow* pBow = (cItemBow*)Item::bow_black;
		if (pPlayer->itemInUse)
		{
			// if is the bow. the itemkey is  5 / 201 / 202 for different duration.
			itemKey = pBow->getItemKey(pPlayer->getItemInUseCount());
		}
	}

	if (m_rightItemRenderable->getItemKey() == pStack->itemID)
		return;

	ItemRenderMesh* pItemMesh = ItemModelManager::Instance()->getItemTesselation(pStack, bm->m_pPlayer);
	m_rightItemRenderable->setMesh(pItemMesh);

}

void ItemRenderer::destroyRightItemMesh()
{
	if (!m_rightItemMesh)
		return;

	LORD::SceneNode* node = m_rightItemMesh->getSceneNode();
	LORD::Mesh* mesh = m_rightItemMesh->getMesh();
	LORD::SceneManager::Instance()->getRootNode()->destroyChild(node);
	LORD::MeshManager::Instance()->releaseResource(mesh);
	LordSafeDelete(m_rightItemMesh);
}

void ItemRenderer::checkRightItemMesh(ItemStackPtr pStack)
{
	LordAssert(pStack);
	if (pStack->itemID == m_lastItemMeshID)
		return;

	destroyRightItemMesh();

	cItem* pcitem = dynamic_cast<cItem*>(pStack->getItem());
	LordAssert(pcitem);
	LordAssert(pcitem->getRenderType() == ItemRenderType::MESH)
	LordAssert(pcitem->getMeshName() != StringUtil::BLANK)
	LORD::Mesh* pMesh = MeshManager::Instance()->createMesh(pcitem->getMeshName());
	pMesh->load();
	m_rightItemMesh = LordNew LORD::ModelEntity;
	LORD::SceneNode* node = LORD::SceneManager::Instance()->getRootNode()->createChild();
	m_rightItemMesh->setMesh(pMesh);
	m_rightItemMesh->attachTo(node);
	m_rightItemMesh->setMaterial(ActorManager::Instance()->GetStaticMaterial());
	m_rightItemMesh->createRenderable(true);
	m_rightItemMesh->prepareTexture(true);

	m_lastItemMeshID = pStack->itemID;
}

void ItemRenderer::checkRightItemEnchantRenderable(ItemStackPtr pStack)
{
	LordAssert(pStack);
	if (!pStack->hasEffect())
		return;

	if (!m_rightItemEnchantRenderable)
	{
		m_rightItemEnchantRenderable = LordNew ItemRenderable();
		m_rightItemEnchantRenderable->setViewSpace(true);
	}

	int itemKey = pStack->itemID;
	if (itemKey == Item::bow->itemID)
	{
		EntityPlayer* pPlayer = bm->m_pPlayer;
		cItemBow* pBow = (cItemBow*)Item::bow;
		if (pPlayer->itemInUse)
		{
			// if is the bow. the itemkey is  5 / 201 / 202 for different duration.
			itemKey = pBow->getItemKey(pPlayer->getItemInUseCount());
		}
	}
	else if (itemKey == Item::bow_red->itemID)
	{
		EntityPlayer* pPlayer = bm->m_pPlayer;
		cItemBow* pBow = (cItemBow*)Item::bow_red;
		if (pPlayer->itemInUse)
		{
			// if is the bow. the itemkey is  5 / 201 / 202 for different duration.
			itemKey = pBow->getItemKey(pPlayer->getItemInUseCount());
		}
	}
	else if (itemKey == Item::bow_black->itemID)
	{
		EntityPlayer* pPlayer = bm->m_pPlayer;
		cItemBow* pBow = (cItemBow*)Item::bow_black;
		if (pPlayer->itemInUse)
		{
			// if is the bow. the itemkey is  5 / 201 / 202 for different duration.
			itemKey = pBow->getItemKey(pPlayer->getItemInUseCount());
		}
	}

	if (m_rightItemEnchantRenderable->getItemKey() == pStack->itemID)
		return;

	ItemRenderMesh* pItemMesh = ItemModelManager::Instance()->getItemTesselation(pStack, bm->m_pPlayer);
	m_rightItemEnchantRenderable->setMesh(pItemMesh);
	m_rightItemEnchantRenderable->setMaterialParam(s_texEnchantment, IRP_ENCHANT_NODEPTH);

}

void ItemRenderer::renderFireInFirstPerson(float rdt)
{
	AtlasSprite* sprite = cBlockManager::scBlocks[BLOCK_ID_FIRE]->getSpriteFromSide(1);
	float bound = 1.0F;
	int frames = TessManager::Instance()->getFrameCount();
	// fire is 16 * 512.
	int frame = frames % 32;

	if (!m_fireEffectsRenderable)
	{
		m_fireEffectsRenderable = LordNew PackMassRenderable(sprite->getTexture(), SRP_TRANSPARENT);
		m_fireEffectsRenderable->setViewSpace(true);
	}
	m_fireEffectsRenderable->beginPolygon();

	for (int i = 0; i < 2; ++i)
	{
		float minu = sprite->getMinU();
		float maxu = sprite->getMaxU();
		float minv = sprite->getMinV();
		float maxv = sprite->getMaxV();
		float vVal = maxv - minv;
		minv += vVal * frame;
		maxv += vVal * frame;

		float x0 = (0.0F - bound) / 2.0F;
		float x1 = x0 + bound;
		float y0 = 0.0F - bound / 2.0F;
		float y1 = y0 + bound;
		float z = -0.5F;

		Matrix4 matfire = Matrix4::IDENTITY;
		matfire.translate(-(i * 2 - 1) * 0.24F, -0.3F, 0.0F);
		Matrix4 temp = Matrix4::IDENTITY;
		temp.rotateY((i * 2 - 1) * 10.0F * Math::DEG2RAD);
		matfire.joint(temp);

		SkinedVertexFmt verts[4];
		verts[0].setEx2(Vector3(x0, y0, z), 0xFFFFFFFF, maxu, maxv, 0);
		verts[1].setEx2(Vector3(x1, y0, z), 0xFFFFFFFF, minu, maxv, 0);
		verts[2].setEx2(Vector3(x1, y1, z), 0xFFFFFFFF, minu, minv, 0);
		verts[3].setEx2(Vector3(x0, y1, z), 0xFFFFFFFF, maxu, minv, 0);

		m_fireEffectsRenderable->addPackMass(verts, 1, 0xBFFFFFFF, matfire);
	}

	m_fireEffectsRenderable->endPolygon();
	m_fireEffectsRenderable->updateRenderQueue();
}

void ItemRenderer::updateEquippedItem()
{
	prevEquippedProgress = equippedProgress;
	EntityPlayerSP* pPlayer = bm->m_pPlayer;
	ItemStackPtr pStack = pPlayer->inventory->getCurrentItem();
	bool flag = equippedItemSlot == pPlayer->inventory->currentItemIndex && pStack == itemToRender;

	if (!itemToRender && !pStack)
		flag = true;

	if (pStack && itemToRender && pStack != itemToRender && pStack->itemID == itemToRender->itemID && pStack->getItemDamage() == itemToRender->getItemDamage())
	{
		itemToRender = pStack;
		flag = true;
	}

	float var4 = 0.4F;
	float var5 = flag ? 1.0F : 0.0F;
	float var6 = var5 - equippedProgress;

	if (var6 < -var4)
	{
		var6 = -var4;
	}

	if (var6 > var4)
	{
		var6 = var4;
	}

	equippedProgress += var6;

	if (equippedProgress < 0.1F)
	{
		itemToRender = pStack;
		equippedItemSlot = pPlayer->inventory->currentItemIndex;
	}
}

}
