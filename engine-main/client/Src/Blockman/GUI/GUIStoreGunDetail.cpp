#include "GUIStoreGunDetail.h"
#include "GUI/GUIStorePropertyItem.h"
#include "GUI/GuiDef.h"
#include "Util/LanguageKey.h"
#include "Util/ClientEvents.h"

namespace BLOCKMAN
{
	GUIStoreGunDetail::GUIStoreGunDetail() :
		gui_layout("StoreGunDetail.json")
	{
	}

	GUIStoreGunDetail::~GUIStoreGunDetail()
	{
	}

	void GUIStoreGunDetail::onLoad()
	{
		m_name = getWindow<GUIStaticText>("StoreGunDetail-Name");
		m_tag = getWindow<GUIStaticText>("StoreGunDetail-Tag");
		m_actor = getWindow<GuiActorWindow>("StoreGunDetail-Actor");
		m_damage_text = getWindow<GUIStaticText>("StoreGunDetail-Damage-Progress-Text");
		m_damage_max = getWindow<GUIButton>("StoreGunDetail-Damage-Max");
		m_damage_default = getWindow<GUILayout>("StoreGunDetail-Damage-Progress-Default");
		m_damage_value = getWindow<GUIProgressBar>("StoreGunDetail-Damage-Progress-Value");
		m_attribute = getWindow<GUILayout>("StoreGunDetail-Attribute");
		m_shootspeed = getWindow<GUIStaticText>("StoreGunDetail-ShootSpeed-Value");
		m_bullet = getWindow<GUIStaticText>("StoreGunDetail-Bullet-Value");
		m_property = getWindow<GUILayout>("StoreGunDetail-Property");
		m_countdown = getWindow<GUILayout>("StoreGunDetail-Countdown");
		m_countdown_value = getWindow<GUIStaticText>("StoreGunDetail-Countdown-Value");
		m_use = getWindow<GUIButton>("StoreGunDetail-Action-Use");
		m_buy = getWindow<GUIButton>("StoreGunDetail-Action-Buy");
		m_upgrade_action = getWindow<GUILayout>("StoreGunDetail-Upgrade-Action");
		m_damage_add = getWindow<GUILayout>("StoreGunDetail-Damage-Add");
		m_damage_add_text = getWindow<GUIStaticText>("StoreGunDetail-Damage-Add-Value");
		m_price = getWindow<GUILayout>("StoreGunDetail-Price");
		m_price_type = getWindow<GUIStaticImage>("StoreGunDetail-Price-Icon");
		m_price_text = getWindow<GUIStaticText>("StoreGunDetail-Price-Value");
		m_upgrade = getWindow<GUIButton>("StoreGunDetail-Action-Upgrade");

		auto Close = getWindow<GUIButton>("StoreGunDetail-Close");
		Close->subscribeEvent(EventButtonClick, std::bind(&GUIStoreGunDetail::onCloseClick, this, std::placeholders::_1));

		auto ShootSpeedText = getWindow<GUIStaticText>("StoreGunDetail-ShootSpeed-Text");
		ShootSpeedText->SetText(getString(LanguageKey::GUI_STR_GUN_FIRING_RATE));
		auto BulletText = getWindow<GUIStaticText>("StoreGunDetail-Bullet-Text");
		BulletText->SetText(getString(LanguageKey::GUI_STR_GUN_BULLET));
		m_upgrade_action->SetVisible(false);
		m_buy->SetText(getString(LanguageKey::SHOP_BTN_BUY));

		m_actor->subscribeEvent(EventWindowTouchDown, std::bind(&GUIStoreGunDetail::onActorTouchDown, this, std::placeholders::_1));
		m_actor->subscribeEvent(EventWindowTouchUp, std::bind(&GUIStoreGunDetail::onActorTouchUp, this, std::placeholders::_1));

		m_use->subscribeEvent(EventButtonClick, std::bind(&GUIStoreGunDetail::onUseClick, this, std::placeholders::_1));
		m_buy->subscribeEvent(EventButtonClick, std::bind(&GUIStoreGunDetail::onBuyClick, this, std::placeholders::_1));
		m_upgrade->subscribeEvent(EventButtonClick, std::bind(&GUIStoreGunDetail::onUpgradeClick, this, std::placeholders::_1));
	}

	void GUIStoreGunDetail::onUpdate(ui32 nTimeElapse)
	{
		if (!isShown())
			return;
		m_actor->SetRotateY(m_actor->GetRotateY() + m_rotate);
		if (m_data == NULL)
			return;
		if (m_data->UpgradeTime > 0)
		{
			int seconds = m_data->UpgradeTime / 1000;
			int second = seconds % 60;
			int minute = (seconds % 3600) / 60;
			int hour = seconds / 3600;
			m_countdown_value->SetText(StringUtil::Format("%s:%s:%s",
				hour < 10 ? StringUtil::Format("0%d", hour).c_str() : StringUtil::ToString(hour).c_str(),
				minute < 10 ? StringUtil::Format("0%d", minute).c_str() : StringUtil::ToString(minute).c_str(),
				second < 10 ? StringUtil::Format("0%d", second).c_str() : StringUtil::ToString(second).c_str()).c_str());
		}
	}

	void GUIStoreGunDetail::SetStoreGun(StoreGun * gun)
	{
		m_data = gun;
		m_rotate = 1.0f;
		m_name->SetText(getString(gun->Name));
		m_tag->SetText(getString(gun->Tag));
		m_actor->SetActor(gun->Actor.c_str(), "idle", 90.0f);
		m_damage_max->SetText(StringUtil::Format("%0.f", gun->MaxDamage).c_str());
		m_countdown->SetVisible(m_data->UpgradeTime > 0);
		m_shootspeed->SetText(StringUtil::ToString(gun->ShootSpeed).c_str());
		m_bullet->SetText(StringUtil::Format("%d/%d", gun->Bullet, gun->SpareBullet).c_str());
		m_property->CleanupChildren();
		m_buy->SetEnabled(true);
		m_upgrade->SetEnabled(true);

		if (gun->Type == StoreGunType::GUN_TYPE_GUN)
		{
			m_attribute->SetVisible(true);
			m_property->SetXPosition({ 0.5f, 5.0f });
		}
		else
		{
			m_attribute->SetVisible(false);
			m_property->SetXPosition({ 0.0f, 10.0f });
		}

		auto properties = GunStoreSetting::findStorePropertyByIds(gun->PropertyIds);
		static unsigned gun_property_index = 0;
		int index = 0;
		float PropertyHeight = 34.0f;
		for (auto property : properties)
		{
			index++;
			gun_property_index++;
			GUIString PropertyName = StringUtil::Format("GUIStoreGunDetail-Property-%d", gun_property_index).c_str();
			GUIStorePropertyItem* pPropertyItem = (GUIStorePropertyItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STORE_PROPERTY_ITEM, PropertyName);
			pPropertyItem->SetWidth({ 1.0f, 0.0f });
			pPropertyItem->SetHeight({ 0.0f, PropertyHeight });
			
			if (gun->Type == StoreGunType::GUN_TYPE_GUN)
			{
				pPropertyItem->SetYPosition({ 0.0f, (index - 1) * PropertyHeight });
			}
			else
			{
				if (index % 2 != 0)
				{
					pPropertyItem->SetXPosition({ 0.0f, 0.0f });
					pPropertyItem->SetYPosition({ 0.0f, (index / 2) * PropertyHeight });
				}
				else
				{
					pPropertyItem->SetXPosition({ 1.0f, 10.0f });
					pPropertyItem->SetYPosition({ 0.0f, (index / 2 - 1) * PropertyHeight });
				}
			}

			pPropertyItem->SetStoreProperty(property, true);
			m_property->AddChildWindow(pPropertyItem);
		}
		StoreGunLevel* level = GunStoreSetting::findStoreGunLevelByGunIdAndLevel(gun->Id, gun->Level);
		if (!level)
		{
			m_use->SetVisible(false);
			m_buy->SetVisible(false);
			m_damage_add->SetVisible(false);
			m_price->SetVisible(false);
			m_upgrade_action->SetVisible(false);
			return;
		}
		float MaxAdd = Math::Max(gun->MaxDamage - gun->Damage, 1.0f);
		m_damage_text->SetText(StringUtil::Format("%0.f", (gun->Damage + level->CurDamage)).c_str());
		m_damage_default->SetArea({ 0.0f, 0.0f }, { 0.0f, 0.0f }, { gun->Damage / gun->MaxDamage + 0.01f, 0.0f }, { 0.0f, 25.5f });
		m_damage_value->SetArea({ 0.0f, 0.0f }, { 0.0f, 0.0f }, { (1.0f - gun->Damage / gun->MaxDamage) + 0.01f, 0.0f }, { 0.0f, 25.5f });
		m_damage_value->SetProgress(level->CurDamage / MaxAdd);
		switch (gun->Status)
		{
		case STATUS_NEW:
		case STATUS_NOT_GET:
			m_use->SetVisible(false);
			m_buy->SetVisible(true);
			m_damage_add->SetVisible(false);
			m_upgrade_action->SetVisible(false);
			m_price->SetVisible(true);
			m_price_type->SetImage(GunStoreSetting::getPriceIcon(gun->PriceType).c_str());
			m_price_text->SetText(GunStoreSetting::getPriceText(gun->PriceType, gun->Chip, gun->Price).c_str());
			break;
		case STATUS_HAS_GET:
		case STATUS_HAS_USE:
			m_use->SetVisible(true);
			m_use->SetText(getString(gun->Status == STATUS_HAS_USE ? LanguageKey::GUI_BTN_EQUIPPED : LanguageKey::INVENTORY_PANEL_EQUIPMENT));
			m_use->SetEnabled(gun->Status == STATUS_HAS_GET);
			m_buy->SetVisible(false);
			if (gun->Level < gun->MaxLevel)
			{
				m_damage_add->SetVisible(true);
				m_price->SetVisible(true);
				m_upgrade_action->SetVisible(true);
				m_damage_add_text->SetText(StringUtil::Format("+%0.f", level->AddDamage).c_str());
				if (gun->UpgradeTime > 0)
				{
					m_price_type->SetImage(GunStoreSetting::getPriceIcon(level->QuickUpgradePriceType).c_str());
					m_price_text->SetText(GunStoreSetting::getPriceText(level->QuickUpgradePriceType, gun->Chip, level->QuickUpgradePrice).c_str());
					m_upgrade->SetText(getString(LanguageKey::GUI_GUN_STORE_QUICK_UPGRADE));
				}
				else
				{
					m_price_type->SetImage(GunStoreSetting::getPriceIcon(level->UpgradePriceType).c_str());
					m_price_text->SetText(GunStoreSetting::getPriceText(level->UpgradePriceType, gun->Chip, level->UpgradePrice).c_str());
					m_upgrade->SetText(getString(LanguageKey::GUI_SHOP_TAB_UPGRADE));
				}
			}
			else
			{
				m_damage_add->SetVisible(false);
				m_price->SetVisible(false);
				m_upgrade_action->SetVisible(false);
			}
			break;
		default:
			break;
		}
		
	}

	bool GUIStoreGunDetail::onActorTouchDown(const EventArgs & events)
	{
		m_rotate = 0.0f;
		return true;
	}

	bool GUIStoreGunDetail::onActorTouchUp(const EventArgs & events)
	{
		m_rotate = 1.0f;
		return true;
	}

	bool GUIStoreGunDetail::onCloseClick(const EventArgs & events)
	{
		hide();
		return true;
	}

	bool GUIStoreGunDetail::onUseClick(const EventArgs & events)
	{
		playSoundByType(ST_Click);
		if (m_data == NULL)
			return false;
		if (m_data->Status != STATUS_HAS_GET)
			return false;
		GunStoreSetting::useGunStoreFunc(StoreAction::USE_GUN, m_data->Id);
		return true;
	}

	bool GUIStoreGunDetail::onBuyClick(const EventArgs & events)
	{
		playSoundByType(ST_Click);
		if (m_data == NULL)
			return false;
		if (m_data->Status == STATUS_LOCK)
			return false;
		bool hasMoney = GunStoreSetting::checkHasMoney(m_data->PriceType, m_data->Chip, m_data->Price);
		if (!hasMoney)
		{
			ShowLackOfMoneyTipDialogEvent::emit(m_data->Price, m_data->PriceType);
			return false;
		}
		m_buy->SetEnabled(false);
		GunStoreSetting::useGunStoreFunc(StoreAction::BUY_GUN, m_data->Id);
		return true;
	}

	bool GUIStoreGunDetail::onUpgradeClick(const EventArgs & events)
	{
		playSoundByType(ST_Click);
		if (m_data == NULL)
			return false;
		if (m_data->Status != STATUS_HAS_GET && m_data->Status != STATUS_HAS_USE)
			return false;
		if (m_data->Level >= m_data->MaxLevel)
			return false;
		StoreGunLevel* level = GunStoreSetting::findStoreGunLevelByGunIdAndLevel(m_data->Id, m_data->Level);
		if (!level)
			return false;
		CurrencyType PriceType = UNKNOWN;
		int Price = 0;
		if (m_data->UpgradeTime > 0)
		{
			PriceType = level->QuickUpgradePriceType;
			Price = level->QuickUpgradePrice;
		}
		else
		{
			PriceType = level->UpgradePriceType;
			Price = level->UpgradePrice;
		}
		bool hasMoney = GunStoreSetting::checkHasMoney(PriceType, m_data->Chip, Price);
		if (!hasMoney)
		{
			ShowLackOfMoneyTipDialogEvent::emit(Price,PriceType);
			return false;
		}

		m_upgrade->SetEnabled(false);
		if (m_data->UpgradeTime > 0)
			GunStoreSetting::useGunStoreFunc(StoreAction::QUICK_UPGRADE_GUN, m_data->Id);
		else
			GunStoreSetting::useGunStoreFunc(StoreAction::UPGRADE_GUN, m_data->Id);
		return true;
	}

}
