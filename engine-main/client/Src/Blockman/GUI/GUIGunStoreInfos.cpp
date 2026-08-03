#include "GUIGunStoreInfos.h"
#include "Util/LanguageKey.h"
#include "Util/ClientEvents.h"
#include "GUI/GuiDef.h"
#include "GUI/GUIStorePropertyItem.h"
#include "GUI/GUIStoreAttributeItem.h"

namespace BLOCKMAN
{
	GUIGunStoreInfos::GUIGunStoreInfos() :
		gui_layout("GunStoreInfos.json")
	{
	}

	GUIGunStoreInfos::~GUIGunStoreInfos()
	{
	}

	void GUIGunStoreInfos::onLoad()
	{
		// Content
		m_content = getWindow<GUILayout>("GunStoreInfos");
		m_content->SetVisible(false);

		// Name
		m_name = getWindow<GUIStaticText>("GunStoreInfos-Name");

		// Gun Info
		m_gun_infos = getWindow<GUILayout>("GunStoreInfos-Gun-Infos");
		m_gun_damage_value = getWindow<GUIStaticText>("GunStoreInfos-Gun-Damage-Value");
		m_gun_shootspeed = getWindow<GUILayout>("GunStoreInfos-Gun-ShootSpeed");
		m_gun_shootspeed_value = getWindow<GUIStaticText>("GunStoreInfos-Gun-ShootSpeed-Value");
		m_gun_bullet = getWindow<GUILayout>("GunStoreInfos-Gun-Bullet");
		m_gun_bullet_value = getWindow<GUIStaticText>("GunStoreInfos-Gun-Bullet-Value");
		m_gun_property = getWindow<GUILayout>("GunStoreInfos-Gun-Property");
		auto GunDamageText = getWindow<GUIStaticText>("GunStoreInfos-Gun-Damage-Text");
		GunDamageText->SetText(getString(LanguageKey::GUI_STR_GUN_HARM));
		auto GunShootSpeedText = getWindow<GUIStaticText>("GunStoreInfos-Gun-ShootSpeed-Text");
		GunShootSpeedText->SetText(getString(LanguageKey::GUI_STR_GUN_FIRING_RATE));
		auto GunBulletText = getWindow<GUIStaticText>("GunStoreInfos-Gun-Bullet-Text");
		GunBulletText->SetText(getString(LanguageKey::GUI_STR_GUN_BULLET));

		// Prop Info
		m_prop_infos = getWindow<GUILayout>("GunStoreInfos-Prop-Infos");

		// Action
		m_action_use = getWindow<GUIButton>("GunStoreInfos-Action-Use");
		m_action_unload = getWindow<GUIButton>("GunStoreInfos-Action-Unload");
		m_action_buy = getWindow<GUIButton>("GunStoreInfos-Action-Buy");
		m_action_upgrade = getWindow<GUIButton>("GunStoreInfos-Action-Upgrade");
		m_action_use->SetText(getString(LanguageKey::INVENTORY_PANEL_EQUIPMENT));
		m_action_unload->SetText(getString(LanguageKey::GUI_BTN_UNLOAD));
		m_action_buy->SetText(getString(LanguageKey::SHOP_BTN_BUY));
		m_action_upgrade->SetText(getString(LanguageKey::GUI_SHOP_TAB_UPGRADE));
		m_action_use->SetVisible(false);
		m_action_unload->SetVisible(false);
		m_action_buy->SetVisible(false);
		m_action_upgrade->SetVisible(false);
		m_action_use->subscribeEvent(EventButtonClick, std::bind(&GUIGunStoreInfos::onUseClick, this, std::placeholders::_1));
		m_action_unload->subscribeEvent(EventButtonClick, std::bind(&GUIGunStoreInfos::onUnloadClick, this, std::placeholders::_1));
		m_action_buy->subscribeEvent(EventButtonClick, std::bind(&GUIGunStoreInfos::onBuyClick, this, std::placeholders::_1));
		m_action_upgrade->subscribeEvent(EventButtonClick, std::bind(&GUIGunStoreInfos::onUpgradeClick, this, std::placeholders::_1));
	}

	void GUIGunStoreInfos::onUpdate(ui32 nTimeElapse)
	{
		if (!isShown())
			return;
		onGunInfosUpdate();
		onPropInfosUpdate();
	}

	void GUIGunStoreInfos::onGunInfosUpdate()
	{
		if (m_display_type != DISPLAY_TYPE_GUN)
			return;
		m_action_unload->SetVisible(false);
		if (m_gun)
		{
			switch (m_gun->Status)
			{
			case STATUS_LOCK:
				m_action_use->SetVisible(false);
				m_action_buy->SetVisible(false);
				m_action_upgrade->SetVisible(false);
				break;
			case STATUS_NEW:
				m_action_use->SetVisible(false);
				m_action_buy->SetVisible(true);
				m_action_upgrade->SetVisible(false);
				break;
			case STATUS_NOT_GET:
				m_action_use->SetVisible(false);
				m_action_buy->SetVisible(true);
				m_action_upgrade->SetVisible(false);
				break;
			case STATUS_HAS_GET:
				m_action_use->SetVisible(true);
				m_action_buy->SetVisible(false);
				m_action_upgrade->SetVisible(true);
				break;
			case STATUS_HAS_USE:
				m_action_use->SetVisible(false);
				m_action_buy->SetVisible(false);
				m_action_upgrade->SetVisible(true);
				break;
			default:
				break;
			}
		}
	}

	void GUIGunStoreInfos::onPropInfosUpdate()
	{
		if (m_display_type != DISPLAY_TYPE_PROP)
			return;
		if (m_prop)
		{
			switch (m_prop->Status)
			{
			case STATUS_LOCK:
				m_action_use->SetVisible(false);
				m_action_unload->SetVisible(false);
				m_action_buy->SetVisible(false);
				m_action_upgrade->SetVisible(false);
				break;
			case STATUS_NEW:
				m_action_use->SetVisible(false);
				m_action_unload->SetVisible(false);
				m_action_buy->SetVisible(true);
				m_action_upgrade->SetVisible(false);
				break;
			case STATUS_NOT_GET:
				m_action_use->SetVisible(false);
				m_action_unload->SetVisible(false);
				m_action_buy->SetVisible(true);
				m_action_upgrade->SetVisible(false);
				break;
			case STATUS_HAS_GET:
				m_action_use->SetVisible(true);
				m_action_unload->SetVisible(false);
				m_action_buy->SetVisible(false);
				m_action_upgrade->SetVisible(true);
				break;
			case STATUS_HAS_USE:
				m_action_use->SetVisible(false);
				m_action_unload->SetVisible(m_prop->Type == StorePropType::PROP_TYPE_PROP);
				m_action_buy->SetVisible(false);
				m_action_upgrade->SetVisible(true);
				break;
			default:
				break;
			}
		}
	}

	bool GUIGunStoreInfos::onUseClick(const EventArgs & events)
	{
		playSoundByType(ST_Click);
		switch (m_display_type)
		{
		case BLOCKMAN::DISPLAY_TYPE_GUN:
			GunStoreSetting::useGunStoreFunc(StoreAction::USE_GUN, m_gun->Id);
			break;
		case BLOCKMAN::DISPLAY_TYPE_PROP:
			GunStoreSetting::useGunStoreFunc(StoreAction::USE_PROP, m_prop->Id);
			break;
		case BLOCKMAN::DISPLAY_TYPE_BLOCK:

			break;
		default:
			break;
		}
		return true;
	}

	bool GUIGunStoreInfos::onUnloadClick(const EventArgs & events)
	{
		playSoundByType(ST_Click);
		switch (m_display_type)
		{
		case BLOCKMAN::DISPLAY_TYPE_GUN:

			break;
		case BLOCKMAN::DISPLAY_TYPE_PROP:
			GunStoreSetting::useGunStoreFunc(StoreAction::UNLOAD_PROP, m_prop->Id);
			break;
		case BLOCKMAN::DISPLAY_TYPE_BLOCK:

			break;
		default:
			break;
		}
		return true;
	}

	bool GUIGunStoreInfos::onBuyClick(const EventArgs & events)
	{
		playSoundByType(ST_Click);
		switch (m_display_type)
		{
		case BLOCKMAN::DISPLAY_TYPE_GUN:
			ShowStoreGunDetailEvent::emit(m_gun->Id);
			break;
		case BLOCKMAN::DISPLAY_TYPE_PROP:
			ShowStorePropDetailEvent::emit(m_prop->Id);
			break;
		case BLOCKMAN::DISPLAY_TYPE_BLOCK:

			break;
		default:
			break;
		}
		return true;
	}

	bool GUIGunStoreInfos::onUpgradeClick(const EventArgs & events)
	{
		playSoundByType(ST_Click);
		switch (m_display_type)
		{
		case BLOCKMAN::DISPLAY_TYPE_GUN:
			ShowStoreGunDetailEvent::emit(m_gun->Id);
			break;
		case BLOCKMAN::DISPLAY_TYPE_PROP:
			ShowStorePropDetailEvent::emit(m_prop->Id);
			break;
		case BLOCKMAN::DISPLAY_TYPE_BLOCK:

			break;
		default:
			break;
		}
		return true;
	}

	void GUIGunStoreInfos::SetStoreGun(StoreGun * gun)
	{
		m_display_type = DISPLAY_TYPE_GUN;
		m_content->SetVisible(true);
		m_prop_infos->SetVisible(false);
		m_gun_infos->SetVisible(true);

		m_gun = gun;
		m_name->SetText(getString(gun->Name));
		StoreGunLevel* level = GunStoreSetting::findStoreGunLevelByGunIdAndLevel(gun->Id, gun->Level);
		if (level && level->CurDamage > 0)
			m_gun_damage_value->SetText(StringUtil::Format("%0.f▢FF45EA1C +%0.f", gun->Damage, level->CurDamage).c_str());
		else
			m_gun_damage_value->SetText(StringUtil::Format("%0.f", gun->Damage).c_str());

		if (gun->Type == StoreGunType::GUN_TYPE_GUN)
		{
			m_gun_shootspeed->SetVisible(true);
			m_gun_bullet->SetVisible(true);
			m_gun_shootspeed_value->SetText(StringUtil::ToString(gun->ShootSpeed).c_str());
			m_gun_bullet_value->SetText(StringUtil::Format("%d/%d", gun->Bullet, gun->SpareBullet).c_str());
			m_gun_property->SetYPosition({ 0, 96.0f });
		}
		else
		{
			m_gun_shootspeed->SetVisible(false);
			m_gun_bullet->SetVisible(false);
			m_gun_property->SetYPosition({ 0, 32.0f });
		}

		m_gun_property->CleanupChildren();
		auto properties = GunStoreSetting::findStorePropertyByIds(gun->PropertyIds);
		static unsigned gun_property_index = 0;
		int index = 0;
		float PropertyHeight = 32.0f;
		for (auto property : properties)
		{
			index++;
			gun_property_index++;
			GUIString PropertyName = StringUtil::Format("GUIGunStoreInfos-Gun-Property-%d", gun_property_index).c_str();
			GUIStorePropertyItem* pPropertyItem = (GUIStorePropertyItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STORE_PROPERTY_ITEM, PropertyName);
			pPropertyItem->SetWidth({ 1.0f, 0.0f });
			pPropertyItem->SetHeight({ 0.0f, PropertyHeight });
			pPropertyItem->SetYPosition({ 0.0f, (index - 1) * PropertyHeight });
			pPropertyItem->SetStoreProperty(property);
			m_gun_property->AddChildWindow(pPropertyItem);
		}
		if (gun->Level >= gun->MaxLevel)
			m_action_upgrade->SetText(getString(LanguageKey::GUI_MANOR_RADIO_INFO));
		else
			m_action_upgrade->SetText(getString(LanguageKey::GUI_SHOP_TAB_UPGRADE));
	}

	void GUIGunStoreInfos::SetStoreProp(StoreProp * prop)
	{
		m_display_type = DISPLAY_TYPE_PROP;
		m_content->SetVisible(true);
		m_gun_infos->SetVisible(false);
		m_prop_infos->SetVisible(true);

		m_prop = prop;
		m_name->SetText(getString(prop->Name));
		m_prop_infos->CleanupChildren();

		if (prop->Level >= prop->MaxLevel)
			m_action_upgrade->SetText(getString(LanguageKey::GUI_MANOR_RADIO_INFO));
		else
			m_action_upgrade->SetText(getString(LanguageKey::GUI_SHOP_TAB_UPGRADE));

		float AttributeHeight = 32.0f;
		float PropertyHeight = 32.0f;
		float CurYPosition = 0.0f;

		StorePropLevel* level = GunStoreSetting::findStorePropLevelByPropIdAndLevel(prop->Id, prop->Level);
		if (!level)
			return;

		auto attriutes = GunStoreSetting::findStoreAttributeByIds(prop->AttributeIds);
		static unsigned prop_attribute_index = 0;
		for (auto attriute : attriutes)
		{
			prop_attribute_index++;
			GUIString AttributeName = StringUtil::Format("GUIGunStoreInfos-Prop-Attribute-%d", prop_attribute_index).c_str();
			GUIStoreAttributeItem* pAttributeItem = (GUIStoreAttributeItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STORE_ATTRIBUTE_ITEM, AttributeName);
			pAttributeItem->SetWidth({ 1.0f, 0.0f });
			pAttributeItem->SetHeight({ 0.0f, AttributeHeight });
			pAttributeItem->SetYPosition({ 0.0f, CurYPosition });
			pAttributeItem->SetStoreAttribute(attriute, level->getCurValueById(attriute->Id));
			m_prop_infos->AddChildWindow(pAttributeItem);
			CurYPosition += AttributeHeight;
		}

		static unsigned prop_property_index = 0;
		auto properties = GunStoreSetting::findStorePropertyByIds(prop->PropertyIds);
		for (auto property : properties)
		{
			prop_property_index++;
			GUIString PropertyName = StringUtil::Format("GUIGunStoreInfos-Prop-Property-%d", prop_property_index).c_str();
			GUIStorePropertyItem* pPropertyItem = (GUIStorePropertyItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STORE_PROPERTY_ITEM, PropertyName);
			pPropertyItem->SetWidth({ 1.0f, 0.0f });
			pPropertyItem->SetHeight({ 0.0f, PropertyHeight });
			pPropertyItem->SetYPosition({ 0.0f, CurYPosition });
			pPropertyItem->SetStoreProperty(property);
			m_prop_infos->AddChildWindow(pPropertyItem);
			CurYPosition += PropertyHeight;
		}
	}

}
