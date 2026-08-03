#include "EntityRenderPlayer.h"
#include "EntityRenderBiped.h"
#include "EntityRenderManager.h"
#include "EntityRenderable.h"

#include "Entity/EntityVehicle.h"
#include "cBlock/cBlock.h"
#include "cBlock/cBlockManager.h"
#include "cItem/cItem.h"
#include "Model/ItemModelMgr.h"
#include "Model/ModelBiped.h"
#include "Model/ModelRenderer.h"
#include "Model/ModelBox.h"
#include "Model/ModelBiped.h"
#include "cEntity/EntityPlayerSP.h"
#include "Render/RenderBlocks.h"
#include "Render/ItemRender.h"
#include "Render/RenderEntity.h"
#include "Render/TextureAtlas.h"
#include "Render/HeadTextRender.h"
#include "cWorld/Blockman.h"
#include "Render/ActiveRenderInfo.h"
#include "cTileEntity/TileEntityRenderMgr.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"

#include "UI/GUIFontManager.h"
#include "UI/GUIImage.h"
#include "UI/GUIImageset.h"
#include "UI/GUIFontGlyph.h"
#include "UI/GUIFont.h"
#include "Block/Block.h"
#include "Block/BlockManager.h"
#include "Item/Item.h"
#include "Item/Items.h"
#include "Item/ItemStack.h"
#include "Inventory/InventoryPlayer.h"
#include "Setting/GunSetting.h"

#include "Tessolator/ItemRenderable.h"
#include "Tessolator/FontRenderable.h"
#include "Actor/ActorObject.h"
#include "Actor/ActorManager.h"
#include "Actor/ActorTemplateData.h"
#include "Actor/ActorMeshTexModify.h"
#include "Anim/AnimBlender.h"
#include "Anim/Skeleton.h"
#include "Anim/Bone.h"
#include "Render/HeadTextRenderer.h"
#include "Setting/LogicSetting.h"
#include "Setting/ActionStateSetting.h"
#include "Item/Potion.h"
#include "cWorld/WorldClient.h"

namespace BLOCKMAN
{

Texture* EntityRenderPlayer::tex_default_player = NULL;

EntityRenderPlayer::EntityRenderPlayer()
	: EntityRenderLiving(LordNew ModelBiped(0.f, 0.f), 0.5f)
{
	modelBipedMain = (ModelBiped*)mainModel;
	modelArmor = LordNew ModelBiped(1.f, 0.f);
	modelArmorPants = LordNew ModelBiped(0.5f, 0.f);
	//modelArmorChestplate = LordNew ModelBiped(1.0F);
	//modelArmor = LordNew ModelBiped(0.5F);
	bgBloodStrip = TextureAtlasRegister::Instance()->getAtlasSprite("blood_strip.json", "bg_blood_strip");
	pbBloodStrip = TextureAtlasRegister::Instance()->getAtlasSprite("blood_strip.json", "pb_blood_strip");
}

EntityRenderPlayer::~EntityRenderPlayer()
{
	LordSafeDelete(modelArmor);
}

    void EntityRenderPlayer::unInitialize()
    {
        TextureManager::Instance()->releaseResource(tex_default_player);
        tex_default_player = nullptr;
    }
    
void EntityRenderPlayer::setDefaultTex(const String& filename)
{
	if (tex_default_player)
		return;
	tex_default_player = TextureManager::Instance()->createTexture(filename);
	tex_default_player->load();
}

void EntityRenderPlayer::renderPlayer_impl(EntityPlayer* pPlayer, const Vector3& pos, float yaw, float rdt)
{
	/*EntityPlayerRenderable* pRenderable = EntityRenderManager::Instance()->getEntityRenderable(pPlayer);
	if (!pRenderable)
	return;*/

	ActorObject* actor = EntityRenderManager::Instance()->getEntityActor(pPlayer);
	if (!actor)
		return;

	//actor->Update(Root::Instance()->getFrameTime());

	if (!actor->getHasInited())
		return;

	actor->setBlockManVisible();

	bool needSetLooksChanged = false;

	if (pPlayer->m_isPeopleActor)
	{

		//if (pPlayer->m_handMeshChanged && actor->prepared() && actor->mergeFinished())
		if (pPlayer->m_handMeshChanged && actor->prepared())
		{
			pPlayer->m_handMeshChanged = false;
			pPlayer->m_outLooksChanged = true;
		}
		//if (pPlayer->m_outLooksChanged && actor->prepared() && actor->mergeFinished())
		if (pPlayer->m_outLooksChanged && actor->prepared())
		{
			bool lastFlag = actor->mbMergeFinished;
			actor->mbMergeFinished = false;

			StringArray bodyparts;
			int equipments[InventoryPlayer::ARMOR_INVENTORY_COUNT];
			int enchantments[InventoryPlayer::ARMOR_INVENTORY_COUNT];

			InventoryPlayer* inventory = pPlayer->inventory;
			for (int i = 0; i < InventoryPlayer::ARMOR_INVENTORY_COUNT; ++i)
			{
				equipments[i] = 0;
				enchantments[i] = 0;
				ItemStackPtr pStack = inventory->armorInventory[i];
				if (!pStack)
					continue;

				cItemArmor* pArmor = dynamic_cast<cItemArmor*>(pStack->getItem());
				LordAssert(pArmor);
				equipments[i] = pArmor->renderIndex + 1;
				if (pArmor->hasEffect(pStack))
					enchantments[i] = pArmor->renderIndex + 1;
			}

			ItemStackPtr pStack = pPlayer->inventory->getCurrentItem();
			// if the pStack is a mesh (gun)
			cItem* citem = nullptr;
			if (pStack) {
				citem = dynamic_cast<cItem*>(pStack->getItem());
			}
			if (citem && citem->getRenderType() == ItemRenderType::MESH && citem->getMeshName() != StringUtil::BLANK)
			{
				bodyparts.push_back(String("gun:") + citem->getMeshName());
			}

			int faceID = pPlayer->m_faceID;
			int hairID = pPlayer->m_hairID;
			int topsID = pPlayer->m_topsID;
			int pantsID = pPlayer->m_pantsID;
			int shoesID = pPlayer->m_shoesID;
			int glassesID = pPlayer->m_glassesId;
			bool viewClothes = pPlayer->m_showClothes;


			// face, body, helmat, leg, feet.
			bodyparts.push_back(String("custom_face:") + StringUtil::ToString(faceID));
			if (glassesID > 0) 
				bodyparts.push_back(String("custom_glasses:") + StringUtil::ToString(glassesID));
			if (pPlayer->m_wingId >= 0) 
				bodyparts.push_back(String("custom_wing:") + StringUtil::ToString(pPlayer->m_wingId));
			if (pPlayer->m_hatId > 0) 
				bodyparts.push_back(String("custom_hat:") + StringUtil::ToString(pPlayer->m_hatId));
			if (pPlayer->m_decorate_hatId >= 0) 
				bodyparts.push_back(String("custom_decorate_hat:") + StringUtil::ToString(pPlayer->m_decorate_hatId));
			if (pPlayer->m_armId >= 0) 
				bodyparts.push_back(String("custom_hand:") + StringUtil::ToString(pPlayer->m_armId));
			if (pPlayer->m_tailId >= 0) 
				bodyparts.push_back(String("custom_tail:") + StringUtil::ToString(pPlayer->m_tailId));
			if (pPlayer->m_extra_wingId >= 0) 
				bodyparts.push_back(String("custom_wing_flag:") + StringUtil::ToString(pPlayer->m_extra_wingId));
			if (pPlayer->m_crownId > 0)
				bodyparts.push_back(String("custom_crown:") + StringUtil::ToString(pPlayer->m_crownId));
			if (pPlayer->m_bagId > 0)
				bodyparts.push_back(String("custom_bag:")+ StringUtil::ToString(pPlayer->m_bagId));

			actor->SetCustomColor(pPlayer->m_skinColor);

			if (viewClothes)
			{
				// will hide the helmet and  shoes
				bodyparts.push_back(String("custom_hair:") + StringUtil::ToString(hairID));
				bodyparts.push_back(String("clothes_tops:") + StringUtil::ToString(topsID > 0 ? topsID : 1));
				bodyparts.push_back(String("clothes_pants:") + StringUtil::ToString(pantsID > 0 ? pantsID : 1));
				bodyparts.push_back(String("custom_shoes:") + StringUtil::ToString(shoesID > 0 ? shoesID : 1));
				if (pPlayer->m_scarfId > 0) {
					bodyparts.push_back(String("custom_scarf:") + StringUtil::ToString(pPlayer->m_scarfId));
				}
			}
			else
			{
				String armorNamePrefix[6] = { "clothes", "leather", "chain", "iron", "diamond", "gold" };
				if (equipments[0] > 0)
					bodyparts.push_back(String("armor_head:") + armorNamePrefix[equipments[0]]);
				else
					bodyparts.push_back(String("custom_hair:") + StringUtil::ToString(hairID));

				if (equipments[1] > 0)
					bodyparts.push_back(String("armor_chest:") + armorNamePrefix[equipments[1]]);
				else {
					bodyparts.push_back(String("clothes_tops:") + StringUtil::ToString(topsID > 0 ? topsID : 1));
					if (pPlayer->m_scarfId > 0) {
						bodyparts.push_back(String("custom_scarf:") + StringUtil::ToString(pPlayer->m_scarfId));
					}
				}

				if (equipments[2] > 0)
					bodyparts.push_back(String("armor_thigh:") + armorNamePrefix[equipments[2]]);
				else
					bodyparts.push_back(String("clothes_pants:") + StringUtil::ToString(pantsID > 0 ? pantsID : 1));

				if (equipments[3] > 0)
					bodyparts.push_back(String("armor_foot:") + armorNamePrefix[equipments[3]]);
				else
					bodyparts.push_back(String("custom_shoes:") + StringUtil::ToString(shoesID > 0 ? shoesID : 1));
			}

			// search all the bodyparts. connet to a string, compare to the current actor's string, if the same: return.
			String newBodyPartString;
			
			for (size_t i = 0; i < bodyparts.size(); ++i)
			{
				newBodyPartString += bodyparts[i];
				newBodyPartString += "|";
			}

			if (newBodyPartString == actor->bodyPartString)
			{
				actor->mbMergeFinished = lastFlag;
			}
			else
			{
				// begin change clothes.

				// draw off all the clothes
				actor->DelMeshTexModify();
				actor->EnableAllBodyParts(false);

				// Attach one by one
				for (size_t i = 0; i < bodyparts.size(); ++i)
				{
					String bodypart = bodyparts[i];
					StringArray parts = StringUtil::Split(bodypart, ":");
					if (parts.size() != 2)
						continue;
					actor->AttachSlave(parts[0], parts[1]);
				}
				// sign
				actor->bodyPartString = newBodyPartString;
				
				// cause the bug need operate texture, can not merge texture and  skin.
				if (pPlayer->m_bagId == 0)
				{
					//actor->requestMergeMeshesEvent();
				}
				else
				{
					actor->DelMeshTexModify();
					actor->AddMeshTexModify("custom_bag", StringUtil::ToString(pPlayer->m_bagId), Vector2i(1, 214), Vector2i(252, 40));  // create the static-mesh Texture Modifiler
					actor->mbMergeFinished = true;
				}
			}

			needSetLooksChanged = true;
		}

		// check the bug need update?
		if (pPlayer->m_bagId != 0)
		{
			ActorMeshTexModify* modifier = actor->getMeshTexModify();
			if (modifier && pPlayer->m_isBagChange)
			{
				pPlayer->m_isBagChange = false;
				float progress = (pPlayer->m_bagCapacity * 1.0f) / (pPlayer->m_bagMaxCapacity * 1.0f);
				String outputNum = StringUtil::Format("%d/%d", pPlayer->m_bagCapacity, pPlayer->m_bagMaxCapacity);
				modifier->updateString(progress, outputNum);
			}
		}
	}
	else
	{
		if (pPlayer->m_handMeshChanged && actor->prepared())
		{
			pPlayer->m_handMeshChanged = false;
			pPlayer->m_outLooksChanged = true;
		}
		if (pPlayer->m_outLooksChanged && actor->prepared())
		{
			//Cannot merge batch actor direct attach
			String bodyId = pPlayer->m_actorBodyId != "" ? pPlayer->m_actorBodyId : "1";
			actor->EnableAllBodyParts(false);
			actor->AttachSlave("body", bodyId);
			actor->AttachSlave("hair", StringUtil::ToString(1));
			//Weapons need attach
			ItemStackPtr pStack = pPlayer->inventory->getCurrentItem();
			cItem* citem = nullptr;
			if (pStack) 
				citem = dynamic_cast<cItem*>(pStack->getItem());
			if (citem && citem->getRenderType() == ItemRenderType::MESH && citem->getMeshName() != StringUtil::BLANK)
				actor->AttachSlave("gun", citem->getMeshName());

			needSetLooksChanged = true;
		}
	}

	if (pPlayer->m_outLooksChanged)
	{
		if (pPlayer->m_footHaloId > 0)
		{
			actor->DetachSelectEffect();
			String effectName = "custom_foot_halo_" + StringUtil::ToString(pPlayer->m_footHaloId) + ".effect";
			actor->AttachSelectEffect(effectName);
		}
		else
		{
			actor->DetachSelectEffect();
		}

		if (pPlayer->m_backEffectId > 0)
		{
			actor->DelPluginEffect("BackEffect");
			String effectName = "custom_back_effect_" + StringUtil::ToString(pPlayer->m_backEffectId) + ".effect";
			actor->AddPluginEffect("BackEffect", "back", effectName.c_str());
		}
		else
		{
			actor->DelPluginEffect("BackEffect");
		}

		if (pPlayer->m_headEffectId > 0)
		{
			actor->DelPluginEffect("HeadEffect");
			String effectName = "custom_head_effect_" + StringUtil::ToString(pPlayer->m_headEffectId) + ".effect";
			Vector3 position = Vector3(0.0f, pPlayer->getHeight() + 0.7f, 0.0f);
			actor->AddPluginEffect("HeadEffect", "head", effectName.c_str(), 1.0f, 1.0f, 1, position);
		}
		else
		{
			actor->DelPluginEffect("HeadEffect");
		}

		if (pPlayer->isFrozen())
		{
			actor->AddPluginEffect("FrozenEffect", "frozen", "frozen.effect");
		}
		else
		{
			actor->DelPluginEffect("FrozenEffect");
		}

		if (pPlayer->isColorful())
		{
			Vector3 scale = Vector3(pPlayer->getLength(), pPlayer->getHeight(), pPlayer->getWidth());
			actor->AddPluginEffect("ColorfulEffect", "colorful", "colorful.effect", 1.0f, 1.0f, -1, Vector3::ZERO, Quaternion::IDENTITY, scale);
		}	
		else
		{
			actor->DelPluginEffect("ColorfulEffect");
		}
	}

	if (needSetLooksChanged)
	{
		pPlayer->m_outLooksChanged = false;
	}

	// need use message 
	if (pPlayer->capabilities.isWatchMode)
	{
		actor->SetActorTranslateAlpha(0.45f);
	}
	else
	{
		actor->SetActorTranslateAlpha(pPlayer->m_opacity);
	}

	bool hideactor = false;
	bool isMyRole = pPlayer == Blockman::Instance()->m_pPlayer;
	bool vehicleHide = pPlayer->isOnVehicle() || pPlayer->isOnAircraft();
	bool isActorInvisible = pPlayer->isActorInvisible();
	if ((pPlayer->isInvisible() && !isMyRole) || vehicleHide || isActorInvisible)
		actor->setActorHide(true);
	else
		actor->setActorHide(false);

	Color color(1.f, 1.f, 1.f, 1.f);
	if (pPlayer->hurtTime > 0 || pPlayer->deathTime > 0 || pPlayer->isHurted())
	{
		color.set(1.f, 0.4f, 0.f, 1.f);
	}
	actor->SetNaturalColor(color, true);

	// set player's animation.
	playAction(pPlayer, actor);

	// pRenderable->updateRender();
	if (pPlayer->m_isNeedChangePostion) {
		Vector3 playerPos = pPlayer->getPosition(rdt, true);
		playerPos.y -= pPlayer->yOffset;
		actor->SetPosition(playerPos);
		pPlayer->m_isNeedChangePostion = false;
	}

	if (pPlayer->spYaw)
	{
		Vector3 playerPos = pPlayer->getPosition(rdt, true);
		playerPos.y -= pPlayer->yOffset;
		actor->SetPosition(playerPos);
		Quaternion q(Vector3::UNIT_Y, pPlayer->spYawRadian);
		actor->SetOrientation(q);
	}
	else
	{
		if (!pPlayer->m_isCameraLocked)
		{
			Vector3 playerPos = pPlayer->getPosition(rdt, true);
			playerPos.y -= pPlayer->yOffset;
			actor->SetPosition(playerPos);
			float py = (pPlayer->prevRenderYawOffset + (pPlayer->rotationPitch - pPlayer->prevRotationPitch) * rdt) * Math::DEG2RAD;
			Quaternion q(Vector3::UNIT_Y, -py);
			actor->SetOrientation(q);
		}
	}

	int brightUV = pPlayer->getBrightnessForRender(rdt);
	int brightness = Blockman::Instance()->m_entityRender->getLightMapColor(brightUV);
	actor->SetBrightness(Color(brightness));

	// caculate the head's rotation.
	if (!pPlayer->m_isCameraLocked)
	{
		float headPitch = pPlayer->prevRotationPitch + (pPlayer->rotationPitch - pPlayer->prevRotationPitch) * rdt;
		float yawOffset = interpolateRotation(pPlayer->prevRenderYawOffset, pPlayer->renderYawOffset, rdt);
		float yawHead = interpolateRotation(pPlayer->prevRotationYawHead, pPlayer->rotationYawHead, rdt);
		float y = -(yawHead - yawOffset) * Math::DEG2RAD;
		float p = headPitch * Math::DEG2RAD;
		Bone* bone = actor->GetSkeletonMaster()->getBone("b_head");
		if (bone)
		{
			int boneIdx = bone->getId();
			Quaternion q1(Vector3::UNIT_Y, y);
			Quaternion q2(Vector3::UNIT_X, p);
			actor->getAnimationBlender()->setExtRotation(boneIdx, q1 * q2);
		}
	}

	/* mc style
	Matrix4 raMat;
	pRenderable->getBoneMatrix(ModelBiped::BBI_ARM_R, raMat);
	renderHoldItem(pPlayer, pRenderable->getWorldMatrix(), raMat, rdt);
	*/
	if (!vehicleHide)
	{
		SceneNode* node = actor->getSocketSceneNode("s_hand_r");
		if (node)
		{
			renderHoldItem_actor(pPlayer, node->getWorldMatrix(), rdt);
			if (pPlayer->m_fishEntity)
			{
				pPlayer->m_fishEntity->m_thirdWorldMat = node->getWorldMatrix();
			}
		}
	}

	if (!pPlayer->isInvisible() && !vehicleHide)
	{

		// render the head text.
		Vector3 vpos = pPlayer->getPosition(rdt, true);
		vpos.y += pPlayer->getHeight() + 0.5F - pPlayer->yOffset;

		/*
		HeadTextRender headText;
		headText.setPosition(vpos);
		headText.setText(pPlayer->getShowName().c_str());
		HeadTextRenderManager::Instance()->addHeadText(headText);
		/*/
		bool canBeOccluded = (pPlayer->isSneaking() && !LogicSetting::Instance()->isSneakShowName() && !pPlayer->capabilities.isFlying);
		canBeOccluded = pPlayer->capabilities.isWatchMode || canBeOccluded;
		canBeOccluded = pPlayer->isNamePerspective() || canBeOccluded;
		HeadTextRenderer::renderHeadText(pPlayer->getShowName(), vpos, canBeOccluded);
		//*/
	}

	renderBloodStrip(pPlayer, pos, yaw, rdt);
}

void EntityRenderPlayer::renderBloodStrip(EntityPlayer * pPlayer, const Vector3 & pos, float yaw, float rdt)
{
	if (bgBloodStrip == NULL || pbBloodStrip == NULL)
		return;

	if (pPlayer->getHealth() <= 0)
		return;

	if (!pPlayer->isHurted())
		return;

	EntityPlayerSPMP* player = Blockman::Instance()->m_pPlayer;
	if (!player)
		return;
	float playerPitch = player->rotationPitch * Math::DEG2RAD;
	float playerYaw = player->rotationYaw * Math::DEG2RAD;
	Vector3 position = pPlayer->getPosition(rdt, true);
	position.y += pPlayer->getHeight() + 0.3F - pPlayer->yOffset;

	int skyBlockLight = pPlayer->getBrightnessForRender(0.f);
	int color = 0xFFFFFFFF;

	Matrix4 matBall, matTemp;
	matBall.identity(); 
	matBall.translate(position);
	matTemp.identity();
	matTemp.rotateY(-playerYaw + Math::PI);
	matBall.joint(matTemp);
	matTemp.identity();
	matTemp.rotateX(-playerPitch);
	matBall.joint(matTemp);
	
	int brightness = Blockman::Instance()->m_entityRender->getLightMapColor(skyBlockLight);
	Color c = Color(brightness) * Color(color);

	PackMassRenderable* pbPackRenderable = EntityRenderManager::Instance()->getPackRenderable(pbBloodStrip->getTexture(), SRP_SOLID, 1);
	float pbminu = pbBloodStrip->getMinU();
	float pbmaxu = pbBloodStrip->getMaxU();
	float pbminv = pbBloodStrip->getMinV();
	float pbmaxv = pbBloodStrip->getMaxV();

	float pbscaley = (float)(pbBloodStrip->getFrameSize().y) / (float)(pbBloodStrip->getFrameSize().x) / 2;
	float hp_percent = pPlayer->getHealth() / pPlayer->getMaxHealth();
	float subpbx = 1.0f - (pPlayer->getHealth() / pPlayer->getMaxHealth());
	SkinedVertexFmt pbvertices[4];
	pbvertices[0].setEx2(Vector3(-0.5f, -pbscaley, 0.f), brightness, pbminu, pbmaxv, 0);
	pbvertices[1].setEx2(Vector3(0.5f - subpbx, -pbscaley, 0.f), brightness, pbmaxu * hp_percent, pbmaxv, 0);
	pbvertices[2].setEx2(Vector3(0.5f - subpbx, pbscaley, 0.f), brightness, pbmaxu * hp_percent, pbminv, 0);
	pbvertices[3].setEx2(Vector3(-0.5f, pbscaley, 0.f), brightness, pbminu, pbminv, 0);
	pbPackRenderable->addPackMass(pbvertices, 1, c.getARGB(), matBall);

	PackMassRenderable* bgPackRenderable = EntityRenderManager::Instance()->getPackRenderable(bgBloodStrip->getTexture(), SRP_SOLID, 1);
	float bgminu = bgBloodStrip->getMinU();
	float bgmaxu = bgBloodStrip->getMaxU();
	float bgminv = bgBloodStrip->getMinV();
	float bgmaxv = bgBloodStrip->getMaxV();

	float bgscaley = (float)(bgBloodStrip->getFrameSize().y) / (float)(bgBloodStrip->getFrameSize().x) / 2;

	SkinedVertexFmt bgvertices[4];
	bgvertices[0].setEx2(Vector3(-0.5f, -bgscaley, 0.f), brightness, bgminu, bgmaxv, 0);
	bgvertices[1].setEx2(Vector3(0.5f, -bgscaley, 0.f), brightness, bgmaxu, bgmaxv, 0);
	bgvertices[2].setEx2(Vector3(0.5f, bgscaley, 0.f), brightness, bgmaxu, bgminv, 0);
	bgvertices[3].setEx2(Vector3(-0.5f, bgscaley, 0.f), brightness, bgminu, bgminv, 0);
	bgPackRenderable->addPackMass(bgvertices, 1, c.getARGB(), matBall);
}

void EntityRenderPlayer::renderFirstPersonArm(EntityPlayer* pPlayer, const Matrix4& mat)
{

}

void EntityRenderPlayer::renderLabel(EntityLivingBase* pLiving, const Vector3& pos, const String& label, float scale, float dissqr)
{
	if (dissqr < 100.0f)
	{
		// todo.
		/*Scoreboard var12 = pPlayer.getWorldScoreboard();
		ScoreObjective var13 = var12.func_96539_a(2);

		if (var13 != null)
		{
		Score var14 = var12.func_96529_a(pPlayer.getEntityName(), var13);

		if (pPlayer.isPlayerSleeping())
		{
		renderLivingLabel(pPlayer, var14.getScorePoints() + " " + var13.getDisplayName(), par2, par4 - 1.5D, par6, 64);
		}
		else
		{
		renderLivingLabel(pPlayer, var14.getScorePoints() + " " + var13.getDisplayName(), par2, par4, par6, 64);
		}

		par4 += (double)((float)getFontRendererFromRenderManager().FONT_HEIGHT * 1.15F * par9);
		}*/
	}

	EntityRenderLiving::renderLabel(pLiving, pos, label, scale, dissqr);
}

void EntityRenderPlayer::preRenderCallback(EntityLivingBase* pLiving, float ticks)
{

}

void EntityRenderPlayer::preRenderArmor(EntityLivingBase* pLiving, int armorslot, float par3)
{

}

int EntityRenderPlayer::shouldRenderPass(EntityLivingBase* pLiving, int armorSlot, float ticks)
{
	if (!pLiving->isClass(ENTITY_CLASS_PLAYER))
		return -1;


	return -1;
}

void EntityRenderPlayer::renderEquippedItems(EntityLivingBase* pLiving, float ticks)
{
	if (!pLiving->isClass(ENTITY_CLASS_PLAYER))
		return;
}

// copy form EntityRenderPlayer::renderHoldItem
void EntityRenderPlayer::renderHoldItem_actor(EntityPlayer* pPlayer, const Matrix4& worldMat, float rdt)
{
	bool isActorInvisible = pPlayer->isActorInvisible();
	if (isActorInvisible)
		return;

	// render the hold item.
	ItemStackPtr pStack = pPlayer->inventory->getCurrentItem();
	if (!pStack)
		return;

	Matrix4 matItem = Matrix4::IDENTITY;
	Matrix4 temp = Matrix4::IDENTITY;
	matItem = worldMat;
	//temp.translate(0.0625F, -0.4375F, 0.0625F);
	//matItem.joint(temp);

	int itemID = pStack->itemID;
	int blockId = -1;
	if (auto block = dynamic_cast<ItemBlock*>(Item::itemsList[itemID]))
	{
		blockId = block->getBlockID();
	}
	ACTION_TYPE action = ACTION_TYPE_INVALID;
	if (pPlayer->getItemInUseCount() > 0)
		action = pStack->getItemUseAction();

	float scale = 1.f;
	if (ItemClient::citemsList[itemID]->getRenderType() == ItemRenderType::BLOCK && blockId > 0 && RenderBlocks::renderItemIn3d(cBlockManager::scBlocks[blockId]->getRenderType()))
	{
		scale = 0.5F;
		temp.identity(); temp.translate(0.27F, -0.2F, 0.15F);
		matItem.joint(temp);
		scale *= 0.75F;
		temp.identity(); temp.rotateX(-20.f * Math::DEG2RAD);
		matItem.joint(temp);
		temp.identity(); temp.rotateY(Math::PI_DIV4);
		matItem.joint(temp);
		temp.identity(); temp.scale(scale, scale, scale);
		matItem.joint(temp);
	}
	else if (itemID == Item::bow->itemID || itemID == Item::bow_red->itemID || itemID == Item::bow_black->itemID)
	{
		scale = 1.f;
		temp.identity(); temp.translate(-0.2F, 0.F, -0.7125F);
		matItem.joint(temp);
		temp.identity(); temp.rotateY(Math::PI_DIV4);
		matItem.joint(temp);
		temp.identity(); temp.rotateZ(Math::PI);
		matItem.joint(temp);
		temp.identity(); temp.rotateX(Math::PI_DIV2);
		matItem.joint(temp);
		temp.identity(); temp.translate(0.5F, 0.0F, 0.0F);
		matItem.joint(temp);
	}
	else if (itemID == Item::fishingRod->itemID)
	{
		scale = 0.9375f;  // 0.625 * 1.5
		temp.identity(); temp.translate(0.05F, 0.05F, -0.2F);
		matItem.joint(temp);
		temp.identity(); temp.scale(scale, scale, scale);
		matItem.joint(temp);
		temp.identity(); temp.rotateY(Math::PI_DIV4);
		matItem.joint(temp);
		temp.identity(); temp.rotateZ(Math::PI);
		matItem.joint(temp);
		temp.identity(); temp.rotateX(Math::PI_DIV2);
		matItem.joint(temp);
		temp.identity(); temp.translate(0.5F, 0.0F, 0.0F);
		matItem.joint(temp);
		temp.identity(); temp.rotateX(Math::PI);
		matItem.joint(temp);
		temp.identity(); temp.rotateZ(-Math::PI_DIV2);
		matItem.joint(temp);
		temp.identity(); temp.translate(0.5F, -0.5F, 0.0F);
		matItem.joint(temp);
	}
	else if (ItemClient::citemsList[itemID]->isFull3D())  // sword, axe, etc.
	{
		scale = 0.9375f;  // 0.625 * 1.5
		temp.identity(); temp.translate(0.05F, 0.05F, -0.2F);
		matItem.joint(temp);
		temp.identity(); temp.scale(scale, scale, scale);
		matItem.joint(temp);
		temp.identity(); temp.rotateY(Math::PI_DIV4);
		matItem.joint(temp);
		temp.identity(); temp.rotateZ(Math::PI);
		matItem.joint(temp);
		temp.identity(); temp.rotateX(Math::PI_DIV2);
		matItem.joint(temp);
		temp.identity(); temp.translate(0.5F, 0.0F, 0.0F);
		matItem.joint(temp);
	}
	else
	{
		scale = 0.5625F;  // 0.375 * 1.5
		temp.identity(); temp.translate(0.05F, 0.05F, -0.0F);
		matItem.joint(temp);
		temp.identity(); temp.scale(scale, scale, scale);
		matItem.joint(temp);
		temp.identity(); temp.rotateZ(Math::PI_DIV3);
		matItem.joint(temp);
		temp.identity(); temp.rotateX(Math::PI_DIV2);
		matItem.joint(temp);
		temp.identity(); temp.rotateZ(20.0F * Math::DEG2RAD);
		matItem.joint(temp);
	}

	int brightUV = pPlayer->getBrightnessForRender(0.f);
	int brightness = Blockman::Instance()->m_entityRender->getLightMapColor(brightUV);


	// for ItemBlock
	if (ItemClient::getRenderType(pStack) == ItemRenderType::MESH)
	{
		
	}
	else if (ItemClient::getRenderType(pStack) == ItemRenderType::BLOCK &&
		blockId > 0 &&
		blockId < BlockManager::MAX_BLOCK_COUNT &&
		cBlockManager::scBlocks[blockId] != NULL &&
		RenderBlocks::renderItemIn3d(cBlockManager::scBlocks[blockId]->getRenderType()))
	{
		cBlock* pcBlock = cBlockManager::scBlocks[blockId];

		ItemBlockMesh* pBlockMesh = ItemModelManager::Instance()->getItemBlockMesh(blockId, pStack->getItemDamage());
		int quadNum = pBlockMesh->getQuadNum();
		PackMassRenderable* pPackRenderable = EntityRenderManager::Instance()->getPackRenderable(pBlockMesh->getTexture(), SRP_SOLID, quadNum);
		pPackRenderable->addItemBlock(pBlockMesh, brightness, matItem);

		if (pStack->hasEffect())
		{
			PackMassRenderable* pEnchant = EntityRenderManager::Instance()->getPackRenderable(ItemRenderer::s_texEnchantment, SRP_ENCHANT, quadNum);
			AtlasSprite* pAtlas = pcBlock->getSprite();
			Vector4 vAtlas = pAtlas->getAtlasVec4();
			pEnchant->setTextureAtlas(vAtlas);
			Color c(brightness);
			c *= ItemRenderer::s_colorEnch_block;
			pEnchant->addItemBlock(pBlockMesh, c.getABGR(), matItem);
		}
	}
	else
	{
		if (pStack->isThirdRender())
		{
			ItemRenderMesh* pItemRenderMesh = ItemModelManager::Instance()->getItemTesselation(pStack, pPlayer);
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

void EntityRenderPlayer::renderHoldItem(EntityPlayer* pPlayer, const Matrix4& worldMat, const Matrix4& raMat, float rdt)
{
	// render the hold item.
	ItemStackPtr pStack = pPlayer->inventory->getCurrentItem();
	if (!pStack)
		return;

	Matrix4 matItem = Matrix4::IDENTITY;
	Matrix4 temp = Matrix4::IDENTITY;
	matItem = worldMat;
	matItem.joint(raMat);
	temp.translate(0.0625F, -0.4375F, 0.0625F);
	matItem.joint(temp);

	int itemID = pStack->itemID;
	int blockId = -1;
	if (auto block = dynamic_cast<ItemBlock*>(Item::itemsList[itemID]))
	{
		blockId = block->getBlockID();
	}
	ACTION_TYPE action = ACTION_TYPE_INVALID;
	if (pPlayer->getItemInUseCount() > 0)
		action = pStack->getItemUseAction();

	float scale = 1.f;
	if (ItemClient::citemsList[itemID]->getRenderType() == ItemRenderType::BLOCK && blockId > 0 && RenderBlocks::renderItemIn3d(cBlockManager::scBlocks[blockId]->getRenderType()))
	{
		scale = 0.5F;
		temp.identity(); temp.translate(0.0F, -0.3875F, -0.3125F);
		matItem.joint(temp);
		scale *= 0.75F;
		temp.identity(); temp.rotateX(-20.f * Math::DEG2RAD);
		matItem.joint(temp);
		temp.identity(); temp.rotateY(Math::PI_DIV4);
		matItem.joint(temp);
		temp.identity(); temp.scale(scale, scale, scale);
		matItem.joint(temp);
	}
	else if (itemID == Item::bow->itemID || itemID == Item::bow_red->itemID || itemID == Item::bow_black->itemID)
	{
		scale = 0.725F;
		temp.identity(); temp.translate(0.0F, -0.125F, -0.0125F);
		matItem.joint(temp);
		// scale *= 0.75F;
		temp.identity(); temp.rotateY(20.f * Math::DEG2RAD);
		matItem.joint(temp);
		temp.identity(); temp.scale(scale, scale, scale);
		matItem.joint(temp);
		temp.identity(); temp.rotateX(-100.f * Math::DEG2RAD);
		matItem.joint(temp);
		temp.identity(); temp.rotateY(-Math::PI_DIV4);
		matItem.joint(temp);
	}
	else if (ItemClient::citemsList[itemID]->isFull3D())
	{
		scale = 0.625F;

		if (ItemClient::citemsList[itemID]->shouldRotateAroundWhenRendering())
		{
			// not tested. 
			temp.identity(); temp.rotateZ(Math::PI);
			matItem.joint(temp);
			temp.identity(); temp.translate(0.0F, -0.125F, 0.0F);
			matItem.joint(temp);
		}

		if (pPlayer->getItemInUseCount() > 0 && action == ACTION_TYPE_BLOCK)
		{
			// not tested.
			temp.identity(); temp.translate(0.05F, 0.0F, -0.1F);
			matItem.joint(temp);
			Matrix4::RotateYawPitchRoll(temp, 50 * Math::DEG2RAD, 10 * Math::DEG2RAD, -10 * Math::DEG2RAD);
			matItem.joint(temp);
		}

		temp.identity(); temp.translate(0.0F, -0.2275F, -0.29F);
		matItem.joint(temp);
		temp.identity(); temp.scale(scale, scale, scale);
		matItem.joint(temp);
		temp.identity(); temp.rotateX(-100.0F * Math::DEG2RAD);
		matItem.joint(temp);
		temp.identity(); temp.rotateY(-Math::PI_DIV4);
		matItem.joint(temp);
	}
	else
	{
		scale = 0.375F;
		temp.identity(); temp.translate(0.05F, -0.2875F, -0.1875F);
		matItem.joint(temp);
		temp.identity(); temp.scale(scale, scale, scale);
		matItem.joint(temp);
		temp.identity(); temp.rotateZ(Math::PI_DIV3);
		matItem.joint(temp);
		temp.identity(); temp.rotateX(-Math::PI_DIV2);
		matItem.joint(temp);
		temp.identity(); temp.rotateZ(20.0F * Math::DEG2RAD);
		matItem.joint(temp);
	}

	int brightUV = pPlayer->getBrightnessForRender(0.f);
	int brightness = Blockman::Instance()->m_entityRender->getLightMapColor(brightUV);
	
	if (ItemClient::getRenderType(pStack) == ItemRenderType::MESH)
	{

	}
	else if (ItemClient::getRenderType(pStack) == ItemRenderType::BLOCK &&
		blockId > 0 &&
		blockId < BlockManager::MAX_BLOCK_COUNT &&
		cBlockManager::scBlocks[blockId] != NULL &&
		RenderBlocks::renderItemIn3d(cBlockManager::scBlocks[blockId]->getRenderType()))
	{
		cBlock* pcBlock = cBlockManager::scBlocks[blockId];

		ItemBlockMesh* pBlockMesh = ItemModelManager::Instance()->getItemBlockMesh(blockId, pStack->getItemDamage());
		int quadNum = pBlockMesh->getQuadNum();
		PackMassRenderable* pPackRenderable = EntityRenderManager::Instance()->getPackRenderable(pBlockMesh->getTexture(), SRP_SOLID, quadNum);
		pPackRenderable->addItemBlock(pBlockMesh, brightness, matItem);

		if (pStack->hasEffect())
		{
			PackMassRenderable* pEnchant = EntityRenderManager::Instance()->getPackRenderable(ItemRenderer::s_texEnchantment, SRP_ENCHANT, quadNum);
			AtlasSprite* pAtlas = pcBlock->getSprite();
			Vector4 vAtlas = pAtlas->getAtlasVec4();
			pEnchant->setTextureAtlas(vAtlas);
			Color c(brightness);
			c *= ItemRenderer::s_colorEnch_block;
			pEnchant->addItemBlock(pBlockMesh, c.getABGR(), matItem);
		}
	}
	else
	{
		temp.identity(); temp.translate(-0.f, -0.8f, 0.f);
		matItem.joint(temp);
		float scale = 1.5F;
		temp.identity(); temp.scale(scale, scale, scale);
		matItem.joint(temp);
		temp.identity(); temp.rotateY(-50.f * Math::DEG2RAD);
		matItem.joint(temp);
		temp.identity(); temp.rotateZ(-335.f * Math::DEG2RAD);
		matItem.joint(temp);
		temp.identity(); temp.translate(0.4375F, -0.0625F, 0.0F);
		matItem.joint(temp);

		ItemRenderMesh* pItemRenderMesh = ItemModelManager::Instance()->getItemTesselation(pStack, pPlayer);
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

void EntityRenderPlayer::rotateCorpse(EntityLivingBase* pLiving, float pitch, float yaw, float ticks)
{
	if (!pLiving->isClass(ENTITY_CLASS_PLAYER))
		return;

}

void EntityRenderPlayer::renderLivingAt(EntityLivingBase* pLiving, const Vector3& pos)
{
	if (!pLiving->isClass(ENTITY_CLASS_PLAYER))
		return;

	EntityPlayer* pPlayer = static_cast<EntityPlayer*>(pLiving);
	if (pPlayer->isEntityAlive() && pPlayer->isPlayerSleeping())
	{
		EntityRenderLiving::renderLivingAt(pPlayer, pos + pPlayer->renderBedOffset);
	}
	else
	{
		EntityRenderLiving::renderLivingAt(pPlayer, pos);
	}
}

void EntityRenderPlayer::renderPlayer(EntityLivingBase* pLiving, const Vector3& pos, float yaw, float ticks)
{
	renderPlayer_impl((EntityPlayer*)pLiving, pos, yaw, ticks);
}

void EntityRenderPlayer::updateBones(EntityLivingBase* pLiving, float rdt, Vector4* outMatrices)
{
	mainModel->swingProgress = renderSwingProgress(pLiving, rdt);
	ModelBiped* pBiped = (ModelBiped*)(mainModel);

	if (pLiving->isClass(ENTITY_CLASS_PLAYER))
	{
		EntityPlayer* pPlayer = (EntityPlayer*)(pLiving);
		ItemStackPtr pStack = pPlayer->inventory->getCurrentItem();
		pBiped->heldItemRight = 0;
		pBiped->aimedBow = false;
		if (pStack && pPlayer->getItemInUseCount() > 0)
		{

			ACTION_TYPE action = pStack->getItemUseAction();
			if (action == ACTION_TYPE_BLOCK)
			{
				pBiped->heldItemRight = 3;
			}
			else if (action == ACTION_TYPE_BOW)
			{
				pBiped->aimedBow = true;;
			}
		}
		pBiped->isSneak = pPlayer->isSneaking();
	}

	float limbYaw = pLiving->prevLimbYaw + (pLiving->limbYaw - pLiving->prevLimbYaw) * rdt;
	float limbSwing = pLiving->limbSwing - pLiving->limbYaw * (1.0F - rdt);
	float headPitch = pLiving->prevRotationPitch + (pLiving->rotationPitch - pLiving->prevRotationPitch) * rdt;
	float yawOffset = interpolateRotation(pLiving->prevRenderYawOffset, pLiving->renderYawOffset, rdt);
	float yawHead = interpolateRotation(pLiving->prevRotationYawHead, pLiving->rotationYawHead, rdt);
	float yawDelta = handleRotationFloat(pLiving, rdt);

	pBiped->setRotationAngles(limbSwing, limbYaw, yawDelta, yawHead - yawOffset, headPitch, pLiving);
	pBiped->updateBones(outMatrices);
}

void EntityRenderPlayer::doRender(Entity* pEntity, const Vector3& pos, float yaw, float ticks)
{

	if (!Blockman::Instance()->m_closeup_isShow)
	{
		renderPlayer_impl((EntityPlayer*)pEntity, pos, yaw, ticks);
	}
}

void EntityRenderPlayer::doRender_gui(Entity* pEntity, const Matrix4& worldmat, const Matrix4& viewprojMat)
{
	EntityPlayer* pPlayer = dynamic_cast<EntityPlayer*>(pEntity);
	LordAssert(pPlayer);

	EntityPlayerRenderable* pRenderable = EntityRenderManager::Instance()->getEntityRenderable(pPlayer);
	if (!pRenderable)
		return;

	float rdt = Blockman::Instance()->m_render_dt;

	pRenderable->update(rdt);

	EntityRenderManager::Instance()->beginRender();
	Matrix4 raMat;
	pRenderable->getBoneMatrix(ModelBiped::BBI_ARM_R, raMat);
	renderHoldItem(pPlayer, worldmat, raMat, rdt);
	EntityRenderManager::Instance()->endRender();

	Color color(1.f, 1.f, 1.f, 1.f);
	if (pPlayer->hurtTime > 0 || pPlayer->deathTime > 0)
	{
		color.set(1.f, 0.4f, 0.f, 0.f);
	}
	pRenderable->setColor(color);
	pRenderable->updateRender_ui(worldmat, viewprojMat);
}

bool EntityRenderPlayer::compileSkinMesh(SkinedRenderable* pRenderable, const Vector2i& pos, const Vector2i& size)
{
	// currently only render mainModel.
	return mainModel->compileSkinMesh(pRenderable, pos, size);
}

bool EntityRenderPlayer::fillArmorSkinMesh(SkinedRenderable* pRenderable, int aromrSlot, const Vector2i& pos, const Vector2i& size, ui32 color)
{
	bool nResult = false;
	ModelBox* pBox = NULL;
	if (aromrSlot == 0)
	{
		pBox = modelArmor->getModelBox(ModelBiped::BBI_HEAD);
		pBox->render(pRenderable, pos, size, color);
		nResult = true;
	}
	else if (aromrSlot == 1)
	{
		pBox = modelArmor->getModelBox(ModelBiped::BBI_BODY);
		pBox->render(pRenderable, pos, size, color);
		pBox = modelArmor->getModelBox(ModelBiped::BBI_ARM_R);
		pBox->render(pRenderable, pos, size, color);
		pBox = modelArmor->getModelBox(ModelBiped::BBI_ARM_L);
		pBox->render(pRenderable, pos, size, color);
		nResult = true;
	}
	else if (aromrSlot == 2)
	{
		pBox = modelArmorPants->getModelBox(ModelBiped::BBI_BODY);
		pBox->render(pRenderable, pos, size, color);
		pBox = modelArmorPants->getModelBox(ModelBiped::BBI_LEG_R);
		pBox->render(pRenderable, pos, size, color);
		pBox = modelArmorPants->getModelBox(ModelBiped::BBI_LEG_L);
		pBox->render(pRenderable, pos, size, color);
		nResult = true;
	}
	else
	{
		pBox = modelArmor->getModelBox(ModelBiped::BBI_LEG_R);
		pBox->render(pRenderable, pos, size, color);
		pBox = modelArmor->getModelBox(ModelBiped::BBI_LEG_L);
		pBox->render(pRenderable, pos, size, color);
		nResult = true;
	}
	return nResult;
}

void EntityRenderPlayer::playAction(EntityPlayer* pPlayer, ActorObject* actor)
{
	int holdItemId = 0;
	ItemStackPtr holdItem = pPlayer->inventory->getCurrentItem();
	if (holdItem != nullptr)
	{
		holdItemId = holdItem->itemID;
	}
	
	playBaseAction(pPlayer, actor, holdItemId);
	playUpperAction(pPlayer, actor, holdItemId, false);
}

void EntityRenderPlayer::playBaseAction(EntityPlayer* pPlayer, ActorObject* actor, int holdItemId)
{
	ActionState baseAction = pPlayer->getBaseAction();
	ActionState prevBaseAction = pPlayer->getPrevBaseAction();
	if (prevBaseAction == baseAction)
		return;
		
	if (!pPlayer->canPlayAction())
		return;

	String skillName = ActionStateSetting::getSkillName((int)baseAction, holdItemId, pPlayer->getOccupation());
	if (!skillName.empty())
	{
		if (skillName == "idle")
		{
			skillName = actor->getDefaultIdle();
		}
		actor->PlaySkill(skillName);
	}

	//LordLogInfo("change base state %d ~ %d", (int)baseAction, (int)prevBaseAction);
	pPlayer->refreshBaseAction();
	playUpperAction(pPlayer, actor, holdItemId, true);
}

void EntityRenderPlayer::playUpperAction(EntityPlayer* pPlayer, ActorObject* actor, int holdItemId, bool bForce)
{
	ActionState prevUpperAction = pPlayer->getPrevUpperAction();
	ActionState upperAction = pPlayer->getUpperAction();
	ActionState baseAction = pPlayer->getBaseAction();
	if (prevUpperAction == upperAction && !pPlayer->m_needUpdateUpperAction && !bForce)
		return;
		
	if (!pPlayer->canPlayAction())
		return;

	pPlayer->m_needUpdateUpperAction = false;
	int animationTime = 0;
	String skillName = ActionStateSetting::getSkillName((int)upperAction, holdItemId, pPlayer->getOccupation());
	if (!skillName.empty())
	{
		if (skillName == "idle")
		{
			skillName = actor->getDefaultIdle();
		}

		animationTime = actor->GetSkillTimeLength(skillName);
		if (baseAction == AS_IDLE)
		{
			actor->PlaySkill(skillName);
		}
		else
		{
			actor->PlayUpperAnim(skillName);
		}
	}

	if (!bForce && pPlayer == Blockman::Instance()->m_pPlayer)
	{
		pPlayer->setSkillAnimationTime(animationTime);
	}

	//LordLogInfo("change upper state %d ~ %d", (int)upperAction, (int)prevUpperAction);
	pPlayer->refreshUpperAction();
}

}
