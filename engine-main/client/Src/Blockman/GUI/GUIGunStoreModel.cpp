#include "GUIGunStoreModel.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Setting/ActionStateSetting.h"
#include "Setting/GunSetting.h"
#include "Actor/ActorObject.h"

namespace BLOCKMAN
{
	GUIGunStoreModel::GUIGunStoreModel() :
		gui_layout("GunStoreModel.json")
	{
	}

	GUIGunStoreModel::~GUIGunStoreModel()
	{
	}

	void GUIGunStoreModel::onLoad()
	{
		m_model = getWindow<GuiActorWindow>("GunStoreModel-Model");
		EntityPlayerSPMP* pPlayer = Blockman::Instance()->m_pPlayer;
		m_model->SetActor(pPlayer->m_sex <= 1 ? "boy.actor" : "girl.actor", "idle", -45.0f);
		m_model->SetCustomColor(pPlayer->m_skinColor);
		m_model->reflreshArmor(pPlayer);
		auto Shoot = getWindow("GunStoreModel-Shoot");
		Shoot->subscribeEvent(EventButtonClick, std::bind(&GUIGunStoreModel::tryGunShoot, this, std::placeholders::_1));
		Shoot->SetVisible(false);
	}

	void GUIGunStoreModel::onUpdate(ui32 nTimeElapse)
	{
	}

	void GUIGunStoreModel::SetStoreGun(StoreGun * gun)
	{
		m_cur_gun_item_id = gun->ItemId;
		EntityPlayerSPMP* pPlayer = Blockman::Instance()->m_pPlayer;
		m_model->SetHoldItem(pPlayer, gun->ItemId);
		String SkillName = ActionStateSetting::getSkillName((int)AS_IDLE, gun->ItemId, 0);
		if (!SkillName.empty())
		{
			m_model->PlaySkill(SkillName);
		}
	}

	void GUIGunStoreModel::SetStoreProp(StoreProp * prop)
	{
		m_cur_gun_item_id = 0;
		EntityPlayerSPMP* pPlayer = Blockman::Instance()->m_pPlayer;
		m_model->SetHoldItem(pPlayer, prop->ItemId);
		String SkillName = ActionStateSetting::getSkillName((int)AS_IDLE, prop->ItemId, 0);
		if (!SkillName.empty())
		{
			m_model->PlaySkill(SkillName);
		}
	}

	bool GUIGunStoreModel::tryGunShoot(const EventArgs & events)
	{
		if (m_cur_gun_item_id == 0)
			return false;
		String SkillName = ActionStateSetting::getSkillName((int)AS_GUN_FIRE, m_cur_gun_item_id, 0);
		if (!SkillName.empty())
		{
			m_model->PlaySkill(SkillName);
		}
		ActorObject* actor = m_model->GetActor();
		GunSetting* setting = GunSetting::getGunSetting(m_cur_gun_item_id);
		if (!setting)
		{
			return false;
		}
		if (setting->flameEffect.length() == 0)
		{
			return false;
		}
		if (actor != NULL)
		{
			Vector3 pos = Vector3(1.0f, 0.55f, 0.19f);
			Quaternion q = actor->GetOrientation();
			actor->AddPluginEffect("FlameEffect", "bip01", setting->flameEffect, 1.0f, 1.0f, 1, pos, q);
		}
		return true;
	}

}
