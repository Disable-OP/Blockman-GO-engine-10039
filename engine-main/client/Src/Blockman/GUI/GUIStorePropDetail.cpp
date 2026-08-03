#include "GUIStorePropDetail.h"
#include "GUI/GUIStoreAttributeItem.h"
#include "GUI/GUIStorePropertyItem.h"
#include "GUI/GuiDef.h"
#include "Util/LanguageKey.h"
#include "Util/ClientEvents.h"

namespace BLOCKMAN
{
	GUIStorePropDetail::GUIStorePropDetail() :
		gui_layout("StorePropDetail.json")
	{
	}

	GUIStorePropDetail::~GUIStorePropDetail()
	{
	}

	void GUIStorePropDetail::onLoad()
	{
		m_name = getWindow<GUIStaticText>("StorePropDetail-Name");
		m_tag = getWindow<GUIStaticText>("StorePropDetail-Tag");
		m_image = getWindow<GUIStaticImage>("StorePropDetail-Image");
		m_desc = getWindow<GUIStaticText>("StorePropDetail-Desc");
		m_level = getWindow<GUIGunStoreLevel>("StorePropDetail-Level");
		m_attribute = getWindow<GUILayout>("StorePropDetail-Attribute");
		m_property = getWindow<GUILayout>("StorePropDetail-Property");
		m_countdown = getWindow<GUILayout>("StorePropDetail-Countdown");
		m_countdown_value = getWindow<GUIStaticText>("StorePropDetail-Countdown-Value");
		m_use = getWindow<GUIButton>("StorePropDetail-Action-Use");
		m_buy = getWindow<GUIButton>("StorePropDetail-Action-Buy");
		m_upgrade_action = getWindow<GUILayout>("StorePropDetail-Upgrade-Action");
		m_price_type = getWindow<GUIStaticImage>("StorePropDetail-Price-Icon");
		m_price = getWindow<GUILayout>("StorePropDetail-Price");
		m_price_text = getWindow<GUIStaticText>("StorePropDetail-Price-Value");
		m_upgrade = getWindow<GUIButton>("StorePropDetail-Action-Upgrade");

		auto Close = getWindow<GUIButton>("StorePropDetail-Close");
		Close->subscribeEvent(EventButtonClick, std::bind(&GUIStorePropDetail::onCloseClick, this, std::placeholders::_1));

		m_upgrade_action->SetVisible(false);
		m_buy->SetText(getString(LanguageKey::SHOP_BTN_BUY));

		m_use->subscribeEvent(EventButtonClick, std::bind(&GUIStorePropDetail::onUseClick, this, std::placeholders::_1));
		m_buy->subscribeEvent(EventButtonClick, std::bind(&GUIStorePropDetail::onBuyClick, this, std::placeholders::_1));
		m_upgrade->subscribeEvent(EventButtonClick, std::bind(&GUIStorePropDetail::onUpgradeClick, this, std::placeholders::_1));
	}

	void GUIStorePropDetail::onUpdate(ui32 nTimeElapse)
	{
		if (!isShown())
			return;
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

	void GUIStorePropDetail::SetStoreProp(StoreProp * prop)
	{
		m_data = prop;
		m_name->SetText(getString(prop->Name));
		m_tag->SetText(getString(prop->Tag));
		m_image->SetImage(prop->Image.c_str());
		m_desc->SetText(getString(prop->Desc));
		m_level->InitLevel(prop->Level, prop->MaxLevel);
		m_countdown->SetVisible(m_data->UpgradeTime > 0);
		m_property->CleanupChildren();
		m_attribute->CleanupChildren();
		m_buy->SetEnabled(true);
		m_upgrade->SetEnabled(true);
		auto properties = GunStoreSetting::findStorePropertyByIds(prop->PropertyIds);
		static unsigned prop_property_index = 0;
		int index = 0;
		float PropertyHeight = 34.0f;
		for (auto property : properties)
		{
			index++;
			prop_property_index++;
			GUIString PropertyName = StringUtil::Format("GUIStorePropDetail-Property-%d", prop_property_index).c_str();
			GUIStorePropertyItem* pPropertyItem = (GUIStorePropertyItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STORE_PROPERTY_ITEM, PropertyName);
			pPropertyItem->SetWidth({ 1.0f, 0.0f });
			pPropertyItem->SetHeight({ 0.0f, PropertyHeight });
			pPropertyItem->SetYPosition({ 0.0f, (index - 1) * PropertyHeight });
			pPropertyItem->SetStoreProperty(property, true);
			m_property->AddChildWindow(pPropertyItem);
		}
		StorePropLevel* level = GunStoreSetting::findStorePropLevelByPropIdAndLevel(prop->Id, prop->Level);
		if (!level)
		{
			m_use->SetVisible(false);
			m_buy->SetVisible(false);
			m_price->SetVisible(false);
			m_upgrade_action->SetVisible(false);
			return;
		}
		float AttributeHeight = 34.0f;
		auto attriutes = GunStoreSetting::findStoreAttributeByIds(prop->AttributeIds);
		static unsigned prop_attribute_index = 0;
		index = 0;
		for (auto attriute : attriutes)
		{
			index++;
			prop_attribute_index++;
			GUIString AttributeName = StringUtil::Format("GUIStorePropDetail-Attribute-%d", prop_attribute_index).c_str();
			GUIStoreAttributeItem* pAttributeItem = (GUIStoreAttributeItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STORE_ATTRIBUTE_ITEM, AttributeName);
			pAttributeItem->SetWidth({ 1.0f, 0.0f });
			pAttributeItem->SetHeight({ 0.0f, AttributeHeight });
			pAttributeItem->SetYPosition({ 0.0f, (index - 1) * AttributeHeight });
			pAttributeItem->SetStoreAttribute(attriute, level->getCurValueById(attriute->Id), true);
			m_attribute->AddChildWindow(pAttributeItem);
		}
		switch (m_data->Status)
		{
		case STATUS_NEW:
		case STATUS_NOT_GET:
			m_use->SetVisible(false);
			m_buy->SetVisible(true);
			m_upgrade_action->SetVisible(false);
			m_price->SetVisible(true);
			m_price_type->SetImage(GunStoreSetting::getPriceIcon(prop->PriceType).c_str());
			m_price_text->SetText(GunStoreSetting::getPriceText(prop->PriceType, prop->Chip, prop->Price).c_str());
			break;
		case STATUS_HAS_GET:
		case STATUS_HAS_USE:
			m_use->SetVisible(true);
			m_buy->SetVisible(false);
			m_use->SetNormalImage(prop->Status == STATUS_HAS_USE ? "set:gun_store.json image:btn_purple" : "set:gun_store.json image:btn_green");
			m_use->SetPushedImage(prop->Status == STATUS_HAS_USE ? "set:gun_store.json image:btn_purple" : "set:gun_store.json image:btn_green");
			
			if (prop->Type == StorePropType::PROP_TYPE_PROP)
			{
				m_use->SetEnabled(true);
				m_use->SetText(getString(prop->Status == STATUS_HAS_USE ? LanguageKey::GUI_BTN_UNLOAD : LanguageKey::INVENTORY_PANEL_EQUIPMENT));
			}
			else
			{
				m_use->SetEnabled(prop->Status == STATUS_HAS_GET);
				m_use->SetText(getString(prop->Status == STATUS_HAS_USE ? LanguageKey::GUI_BTN_EQUIPPED : LanguageKey::INVENTORY_PANEL_EQUIPMENT));
			}	
			
			if (prop->Level < prop->MaxLevel)
			{
				m_price->SetVisible(true);
				m_upgrade_action->SetVisible(true);
				if (prop->UpgradeTime > 0)
				{
					m_price_type->SetImage(GunStoreSetting::getPriceIcon(level->QuickUpgradePriceType).c_str());
					m_price_text->SetText(GunStoreSetting::getPriceText(level->QuickUpgradePriceType, prop->Chip, level->QuickUpgradePrice).c_str());
					m_upgrade->SetText(getString(LanguageKey::GUI_GUN_STORE_QUICK_UPGRADE));
				}
				else
				{
					m_price_type->SetImage(GunStoreSetting::getPriceIcon(level->UpgradePriceType).c_str());
					m_price_text->SetText(GunStoreSetting::getPriceText(level->UpgradePriceType, prop->Chip, level->UpgradePrice).c_str());
					m_upgrade->SetText(getString(LanguageKey::GUI_SHOP_TAB_UPGRADE));
				}
			}
			else
			{
				m_price->SetVisible(false);
				m_upgrade_action->SetVisible(false);
			}
			break;
		default:
			break;
		}
	}

	bool GUIStorePropDetail::onCloseClick(const EventArgs & events)
	{
		hide();
		return true;
	}

	bool GUIStorePropDetail::onUseClick(const EventArgs & events)
	{
		playSoundByType(ST_Click);
		if (m_data == NULL)
			return false;
		if (m_data->Status != STATUS_HAS_GET && m_data->Status != STATUS_HAS_USE)
			return false;
		if (m_data->Status == STATUS_HAS_GET)
			GunStoreSetting::useGunStoreFunc(StoreAction::USE_PROP, m_data->Id);
		else
			GunStoreSetting::useGunStoreFunc(StoreAction::UNLOAD_PROP, m_data->Id);
		return true;
	}

	bool GUIStorePropDetail::onBuyClick(const EventArgs & events)
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
		GunStoreSetting::useGunStoreFunc(StoreAction::BUY_PROP, m_data->Id);
		return true;
	}

	bool GUIStorePropDetail::onUpgradeClick(const EventArgs & events)
	{
		playSoundByType(ST_Click);
		if (m_data == NULL)
			return false;
		if (m_data->Status != STATUS_HAS_GET && m_data->Status != STATUS_HAS_USE)
			return false;
		if (m_data->Level >= m_data->MaxLevel)
			return false;
		StorePropLevel* level = GunStoreSetting::findStorePropLevelByPropIdAndLevel(m_data->Id, m_data->Level);
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
			ShowLackOfMoneyTipDialogEvent::emit(Price, PriceType);
			return false;
		}

		m_upgrade->SetEnabled(false);
		if (m_data->UpgradeTime > 0)
			GunStoreSetting::useGunStoreFunc(StoreAction::QUICK_UPGRADE_PROP, m_data->Id);
		else
			GunStoreSetting::useGunStoreFunc(StoreAction::UPGRADE_PROP, m_data->Id);
		return true;
	}

}
