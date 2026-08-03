#include "GuiActorWindow.h"

#include "GUI/GuiDef.h"
#include "cWorld/Blockman.h"
#include "Entity/EntityPlayer.h"
#include "Inventory/InventoryPlayer.h"
#include "cItem/cItem.h"
#include "Item/Item.h"
#include "Item/Items.h"

#include "Actor/ActorObject.h"
#include "Actor/ActorManager.h"
#include "Effect/EffectSystemManager.h"
#include "UI/GUIWindowManager.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIRenderManager.h"
#include "Object/Root.h"

namespace BLOCKMAN
{

	GuiActorWindow::GuiActorWindow(GUIWindowType nType, const GUIString& strName)
		: GUIWindow(nType, strName)
		, m_pActor(NULL)
		, m_fScaleFactor(1.0f)
		, m_fRotateX(0.0f)
		, m_fRotateY(0.0f)
		, m_fRotateZ(0.0f)
		, m_fScaleClientSet(1.0f)
		, m_fParmaScale(1.f)
	{
	}

	GuiActorWindow::~GuiActorWindow()
	{
		if (m_pActor)
		{
			ActorManager::Instance()->DestroyActor(m_pActor);
			m_pActor = NULL;
		}
	}

	void GuiActorWindow::DrawSelf()
	{
		//GUIRenderManager::Instance()->EndRender();

		Matrix4 viewprojMat = SceneManager::Instance()->getGUICamera()->getViewProjMatrix();
		if (m_pActor)
		{
			GUIRenderManager::Instance()->SetDepth(GUIRenderManager::Instance()->GetDepth() - GetPixelSize().x);

			UpdateActorArea();

			m_pActor->UpdateForUI(Root::Instance()->getFrameTime(), viewprojMat);
			//m_pActor->Update(Root::Instance()->getFrameTime());
		}

		// draw effect and actor
		/*ui8 nStartIndex = SceneManager::Instance()->getRenderQueueIndex("SkinedMeshForActorGPU");
		ui8 nEndIndex = SceneManager::Instance()->getRenderQueueIndex("StaticMeshForActorHigh");

		for (size_t i = nStartIndex; i <= nEndIndex; ++i)
		{
		RenderQueue* renderQueue = SceneManager::Instance()->getRenderQueueByIndex(i);
		LordAssert(renderQueue);
		renderQueue->renderQueue();
		renderQueue->beginRender();
		}*/

		// render effect
		EffectSystemManager::Instance()->endRender(true, &viewprojMat);
		EffectSystemManager::Instance()->beginRender();

		// restore ui render state
		GUIRenderManager::Instance()->BeginRender();
	}

	void GuiActorWindow::UpdateSelf(ui32 nTimeElapse)
	{
		if (m_pActor)
		{
			UpdateActorArea();

			Invalidate();
		}
	}

	void GuiActorWindow::UpdateActorArea()
	{
		if (m_pActor)
		{
			// ----update scene node
			// position
			Vector3 position;
			ERect rtOuterRect = GetRenderArea();
			position.x = rtOuterRect.left + rtOuterRect.getWidth() * 0.5f;
			position.y = rtOuterRect.bottom;
			//Camera* pCamera = SceneManager::Instance()->getGUICamera();
			position.z = GUIRenderManager::Instance()->GetDepth();
			m_pActor->SetPosition(position);

			// orientation
			Quaternion qx(Vector3::UNIT_X, Math::PI + Math::DEG2RAD * m_fRotateX);
			Quaternion qy(Vector3::UNIT_Y, Math::DEG2RAD * m_fRotateY);
			Quaternion qz(Vector3::UNIT_Z, Math::DEG2RAD * m_fRotateZ);
			Quaternion q = qx*qy*qz;
			m_pActor->SetOrientation(q);

			// size
			// 这里需要一个actor的实际高度来进行缩放
			float fScale = m_fScaleClientSet * rtOuterRect.getWidth();
			m_pActor->SetScale(Vector3(fScale * m_fParmaScale, fScale * m_fParmaScale, fScale * m_fParmaScale));
		}
	}

	void GuiActorWindow::SetActor(ActorObject* actor)
	{
		if (actor != m_pActor)
		{
			m_pActor = actor;
			if (m_pActor)
			{
				m_pActor->SetScale(Vector3::ONE);
				Box actorAABB = m_pActor->getActorAABB();
				m_fScaleFactor = 1.f / actorAABB.getDX();
			}

			UpdateActorArea();
		}
	}

	void GuiActorWindow::SetActor(const GUIString& strActorName, const GUIString& strSkillName, float Rotate)
	{
		SetRotateY(Rotate);

		if (strActorName != m_strActorName)
		{
			if (m_pActor)
			{
				ActorManager::Instance()->DestroyActor(m_pActor);
				m_pActor = NULL;
			}

			if (!strActorName.empty())
			{
				m_pActor = ActorManager::Instance()->CreateActor(strActorName.c_str(), strSkillName.c_str(), true);

				if (m_pActor)
				{
					m_pActor->SetScale(Vector3::ONE);
					Box actorAABB = m_pActor->getActorAABB();
					m_fScaleFactor = 1.f / actorAABB.getDX();

					m_strActorName = strActorName;
					m_strSkillName = strSkillName;
				}

				UpdateActorArea();
			}
		}

		if (strActorName == m_strActorName && strSkillName != m_strSkillName)
		{
			if (m_pActor)
			{
				m_pActor->PlaySkill(strSkillName.c_str());

				m_strSkillName = strSkillName;
			}
		}
	}

	void GuiActorWindow::OnBeforeDraw()
	{
		GUIRenderManager::Instance()->EndRender();
	}

	void GuiActorWindow::OnEndDraw()
	{
		GUIRenderManager::Instance()->BeginRender();
	}

	bool GuiActorWindow::SetProperty(const GUIString& strName, const GUIString& strValue)
	{
		if (GUIWindow::SetProperty(strName, strValue))
		{
			return true;
		}

		if (strName == GWP_ACTOR_WINDOW_ACTORNAME)
		{
			SetActor(strValue.c_str());
			return true;
		}
		else if (strName == GWP_ACTOR_WINDOW_SKILLNAME)
		{
			SetSkillName(strValue.c_str());
			return true;
		}
		else if (strName == GWP_ACTOR_WINDOW_ROTATEX)
		{
			m_fRotateX = StringUtil::ParseFloat(strValue.c_str());
			return true;
		}
		else if (strName == GWP_ACTOR_WINDOW_ROTATEY)
		{
			m_fRotateY = StringUtil::ParseFloat(strValue.c_str());
			return true;
		}
		else if (strName == GWP_ACTOR_WINDOW_ROTATEZ)
		{
			m_fRotateZ = StringUtil::ParseFloat(strValue.c_str());
			return true;
		}
		else if (strName == GWP_ACTOR_WINDOW_SCALE)
		{
			m_fScaleClientSet = StringUtil::ParseFloat(strValue.c_str());
			return true;
		}

		return false;
	}

	bool GuiActorWindow::GetProperty(const GUIString& strName, GUIString& strValue)
	{
		if (GUIWindow::GetProperty(strName, strValue))
		{
			return true;
		}

		if (strName == GWP_ACTOR_WINDOW_ACTORNAME)
		{
			strValue = m_strActorName;
			return true;
		}
		else if (strName == GWP_ACTOR_WINDOW_SKILLNAME)
		{
			strValue = m_strSkillName;
			return true;
		}
		else if (strName == GWP_ACTOR_WINDOW_ROTATEX)
		{
			strValue = StringUtil::ToString(m_fRotateX).c_str();
			return true;
		}
		else if (strName == GWP_ACTOR_WINDOW_ROTATEY)
		{
			strValue = StringUtil::ToString(m_fRotateY).c_str();
			return true;
		}
		else if (strName == GWP_ACTOR_WINDOW_ROTATEZ)
		{
			strValue = StringUtil::ToString(m_fRotateZ).c_str();
			return true;
		}
		else if (strName == GWP_ACTOR_WINDOW_SCALE)
		{
			strValue = StringUtil::ToString(m_fScaleClientSet).c_str();
			return true;
		}

		return false;
	}

	void GuiActorWindow::GetPropertyList(GUIPropertyList& properlist)
	{
		GUIWindow::GetPropertyList(properlist);
		properlist.push_back(GWP_ACTOR_WINDOW_ACTORNAME);
		properlist.push_back(GWP_ACTOR_WINDOW_SKILLNAME);
		properlist.push_back(GWP_ACTOR_WINDOW_ROTATEX);
		properlist.push_back(GWP_ACTOR_WINDOW_ROTATEY);
		properlist.push_back(GWP_ACTOR_WINDOW_ROTATEZ);
		properlist.push_back(GWP_ACTOR_WINDOW_SCALE);
	}

	bool GuiActorWindow::GetPropertyType(const GUIString& strName, GUIPropertyType& type)
	{
		if (GUIWindow::GetPropertyType(strName, type))
		{
			return true;
		}

		if (strName == GWP_ACTOR_WINDOW_ACTORNAME)
		{
			type = GPT_STRING;
			return true;
		}
		else if (strName == GWP_ACTOR_WINDOW_SKILLNAME)
		{
			type = GPT_STRING;
			return true;
		}
		else if (strName == GWP_ACTOR_WINDOW_ROTATEX ||
			strName == GWP_ACTOR_WINDOW_ROTATEY ||
			strName == GWP_ACTOR_WINDOW_ROTATEZ ||
			strName == GWP_ACTOR_WINDOW_SCALE)
		{
			type = GPT_FLOAT;
			return true;
		}

		return false;
	}

	int GuiActorWindow::SetSkillName(const GUIString& strSkillName)
	{
		if (strSkillName != m_strSkillName)
		{
			m_strSkillName = strSkillName;

			if (m_pActor && !m_strSkillName.empty())
			{
				return	m_pActor->PlaySkill(strSkillName.c_str());
			}
		}

		return 0;
	}

	bool GuiActorWindow::TouchDown(const Vector2& position)
	{
		m_lastXPosition = position.x;

		return 	GUIWindow::TouchDown(position);
	}

	bool GuiActorWindow::TouchMove(const Vector2& position)
	{
		float delta = position.x - m_lastXPosition;
		m_lastXPosition = position.x;

		float old = GetRotateY();
		SetRotateY(old + delta);

		return 	GUIWindow::TouchMove(position);
	}

	void GuiActorWindow::reflreshArmor(EntityPlayer* pPlayer)
	{
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
		m_pActor->EnableAllBodyParts(false);

		int faceID = pPlayer->m_faceID;
		int hairID = pPlayer->m_hairID;
		int topsID = pPlayer->m_topsID;
		int pantsID = pPlayer->m_pantsID;
		int shoesID = pPlayer->m_shoesID;
		int glassesID = pPlayer->m_glassesId;
		bool viewClothes = pPlayer->m_showClothes;

		// face, body, helmat, leg, feet.
		m_pActor->UseBodyPart("custom_face", StringUtil::ToString(faceID)); // face will always show.
		if (glassesID > 0) {
			m_pActor->UseBodyPart("custom_glasses", StringUtil::ToString(glassesID));
		}

		if (pPlayer->m_wingId >= 0) {
			m_pActor->UseBodyPart("custom_wing", StringUtil::ToString(pPlayer->m_wingId));
		}

		if (pPlayer->m_hatId > 0) {
			m_pActor->AttachSlave("custom_hat", StringUtil::ToString(pPlayer->m_hatId));
		}

		if (pPlayer->m_decorate_hatId >= 0) {
			m_pActor->AttachSlave("custom_decorate_hat", StringUtil::ToString(pPlayer->m_decorate_hatId));
		}

		if (pPlayer->m_armId >= 0) {
			m_pActor->AttachSlave("custom_hand", StringUtil::ToString(pPlayer->m_armId));
		}
		
		if (pPlayer->m_tailId >= 0) {
			m_pActor->AttachSlave("custom_tail", StringUtil::ToString(pPlayer->m_tailId));
		}

		if (pPlayer->m_extra_wingId >= 0) {
			m_pActor->AttachSlave("custom_wing_flag", StringUtil::ToString(pPlayer->m_extra_wingId));
		}

		/*if (pPlayer->m_footHaloId > 0) {
			m_pActor->DetachSelectEffect();
			String effectName = "custom_foot_halo_" + StringUtil::ToString(pPlayer->m_footHaloId) + ".effect";
			m_pActor->AttachSelectEffect(effectName);
		}*/

		m_pActor->SetCustomColor(pPlayer->m_skinColor);

		if (viewClothes)
		{
			// will hide the helmet and  shoes
			m_pActor->UseBodyPart("custom_hair", StringUtil::ToString(hairID)); // always show the hair, and hide the helmet
			m_pActor->UseBodyPart("clothes_tops", StringUtil::ToString(topsID > 0 ? topsID : 1));
			m_pActor->UseBodyPart("clothes_pants", StringUtil::ToString(pantsID > 0 ? pantsID : 1));
			m_pActor->UseBodyPart("custom_shoes", StringUtil::ToString(shoesID > 0 ? shoesID : 1));
			if (pPlayer->m_scarfId > 0) {
				m_pActor->UseBodyPart("custom_scarf", StringUtil::ToString(pPlayer->m_scarfId));
			}
		}
		else
		{
			String armorNamePrefix[] = { "clothes", "leather", "chain", "iron", "diamond", "gold" };
			if (equipments[0] > 0)
				m_pActor->UseBodyPart("armor_head", armorNamePrefix[equipments[0]]);
			else
				m_pActor->UseBodyPart("custom_hair", StringUtil::ToString(hairID));

			if (equipments[1] > 0)
				m_pActor->UseBodyPart("armor_chest", armorNamePrefix[equipments[1]]);
			else {
				m_pActor->UseBodyPart("clothes_tops", StringUtil::ToString(topsID > 0 ? topsID : 1));
				if (pPlayer->m_scarfId > 0) {
					m_pActor->UseBodyPart("custom_scarf", StringUtil::ToString(pPlayer->m_scarfId));
				}
			}

			if (equipments[2] > 0)
				m_pActor->UseBodyPart("armor_thigh", armorNamePrefix[equipments[2]]);
			else
				m_pActor->UseBodyPart("clothes_pants", StringUtil::ToString(pantsID > 0 ? pantsID : 1));

			if (equipments[3] > 0)
				m_pActor->UseBodyPart("armor_foot", armorNamePrefix[equipments[3]]);
			else
				m_pActor->UseBodyPart("custom_shoes", StringUtil::ToString(shoesID > 0 ? shoesID : 1));
		}
	}

	void GuiActorWindow::UseBodyPart(const String & masterName, const String & slaveName)
	{
		if (m_pActor)
		{
			m_pActor->UseBodyPart(masterName, slaveName);
		}
	}

	void GuiActorWindow::UnloadBodyPart(const String & masterName)
	{
		if (m_pActor)
		{
			m_pActor->UnloadBodyPart(masterName);
		}
	}

	void GuiActorWindow::SetCustomColor(const Color & color)
	{
		if (m_pActor)
		{
			m_pActor->SetCustomColor(color);
		}
	}

	void GuiActorWindow::PlaySkill(const String & skillName)
	{
		if (m_pActor)
		{
			m_pActor->PlaySkill(skillName);
		}
	}

	void GuiActorWindow::SetHoldItem(EntityPlayer* pPlayer, int itemId)
	{
		if (!m_pActor)
			return;

		if (itemId >= Item::MAX_ITEM_COUNT)
			return;

		cItem* item = dynamic_cast<cItem*>(Item::itemsList[itemId]);
		if (item == nullptr)
			return;

		if (item->getRenderType() == ItemRenderType::MESH && item->getMeshName() != StringUtil::BLANK)
		{
			UseBodyPart("gun", item->getMeshName());
		}
		else
		{
			UnloadBodyPart("gun");
		}
	}

}
