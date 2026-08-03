#include "GUICustomPropItem.h"
#include "UI/GUIWindowManager.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"
#include "Util/ClientEvents.h"
#include "cEntity/EntityPlayerSP.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "game.h"
#include "Util/UICommon.h"

namespace BLOCKMAN
{
	GUICustomPropItem::GUICustomPropItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("CustomPropItem.json"));
		renameComponents(m_window);
		m_name = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(1));
		m_icon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(2));
		auto m_descTip = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(3));
		m_descTip->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_SHOP_GOODS_TIP).c_str());
		m_desc = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(4));
		m_price = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(5));
		m_priceIcon = dynamic_cast<GUIStaticImage*>(m_price->GetChildByIndex(0));
		m_priceNum = dynamic_cast<GUIStaticText*>(m_price->GetChildByIndex(1));
		m_buy = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(6));
		m_buy->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_SHOP_TAB_UPGRADE).c_str());
		m_buy->subscribeEvent(EventButtonClick, std::bind(&GUICustomPropItem::onButtonClick, this, std::placeholders::_1));
		AddWindowToDrawList(*m_window);
		SetTouchable(false);
	}

	void GUICustomPropItem::removeComponents()
	{
		if (m_window)
		{
			RemoveWindowFromDrawList(*m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	void GUICustomPropItem::updateData()
	{
		m_buy->SetVisible(true);
		auto player = Blockman::Instance()->m_pPlayer;
		if (player && player->m_wallet)
		{
			ui64 money = player->m_wallet->getCurrency();
			if (money < m_prop.m_price)
			{
				m_buy->SetEnabled(false);
				m_buy->SetText(LanguageManager::Instance()->getString(LanguageKey::LACK_OF_MONEY).c_str());
				return;
			}
		}
		switch (m_status)
		{
		case BLOCKMAN::CustomPropStatus::Undefine:
			m_buy->SetVisible(false);
			break;
		case BLOCKMAN::CustomPropStatus::Default:
			m_buy->SetNormalImage("set:app_shop.json image:app_shop_buy");
			m_buy->SetPushedImage("set:app_shop.json image:app_shop_buy");
			m_buy->SetEnabled(true);
			m_buy->SetText(LanguageManager::Instance()->getString(LanguageKey::SHOP_BTN_BUY).c_str());
			break;
		case BLOCKMAN::CustomPropStatus::DontBuy:
			m_buy->SetNormalImage("set:app_shop.json image:app_shop_buy");
			m_buy->SetPushedImage("set:app_shop.json image:app_shop_buy");
			m_buy->SetEnabled(false);
			m_buy->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_SHOP_DONT_BUY).c_str());
			break;
		case BLOCKMAN::CustomPropStatus::CanBuy:
			m_buy->SetNormalImage("set:app_shop.json image:app_shop_buy");
			m_buy->SetPushedImage("set:app_shop.json image:app_shop_buy");
			m_buy->SetEnabled(true);
			m_buy->SetText(LanguageManager::Instance()->getString(LanguageKey::SHOP_BTN_BUY).c_str());
			break;
		case BLOCKMAN::CustomPropStatus::Unlock:
			m_buy->SetNormalImage("set:app_shop.json image:app_shop_buy");
			m_buy->SetPushedImage("set:app_shop.json image:app_shop_buy");
			m_buy->SetEnabled(true);
			m_buy->SetText(LanguageManager::Instance()->getString(LanguageKey::SHOP_BTN_UNLOCK).c_str());
			break;
		case BLOCKMAN::CustomPropStatus::Switch:
			m_buy->SetNormalImage("set:app_shop.json image:app_shop_has_get");
			m_buy->SetPushedImage("set:app_shop.json image:app_shop_has_get");
			m_buy->SetEnabled(true);
			m_buy->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_SHOP_SWITCH).c_str());
			break;
		case BLOCKMAN::CustomPropStatus::Upgrade:
			m_buy->SetNormalImage("set:app_shop.json image:app_shop_has_use");
			m_buy->SetPushedImage("set:app_shop.json image:app_shop_has_use");
			m_buy->SetEnabled(true);
			m_buy->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_SHOP_TAB_UPGRADE).c_str());
			break;
		case BLOCKMAN::CustomPropStatus::HasGet:
			m_buy->SetNormalImage("set:app_shop.json image:app_shop_has_get");
			m_buy->SetPushedImage("set:app_shop.json image:app_shop_has_get");
			m_buy->SetEnabled(true);
			m_buy->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_SHOP_HAS_GET).c_str());
			break;
		case BLOCKMAN::CustomPropStatus::HasUse:
			m_buy->SetNormalImage("set:app_shop.json image:app_shop_has_use");
			m_buy->SetPushedImage("set:app_shop.json image:app_shop_has_use");
			m_buy->SetEnabled(true);
			m_buy->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_SHOP_HAS_USE).c_str());
			break;
		case BLOCKMAN::CustomPropStatus::MaxLv:
			m_buy->SetNormalImage("set:app_shop.json image:app_shop_buy");
			m_buy->SetPushedImage("set:app_shop.json image:app_shop_buy");
			m_buy->SetEnabled(false);
			m_buy->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_SHOP_TIP_MAX_LEVEL).c_str());
			break;
		default:
			break;
		}
	}

	void GUICustomPropItem::setCustomProp(CustomProp prop)
	{
		m_prop = prop;
		String name = LanguageManager::Instance()->getString(prop.m_name);
		m_name->SetText(name.c_str());
		m_icon->SetImage(prop.m_image.c_str());

		String desc = LanguageManager::Instance()->getString(prop.m_desc);
		
		if (prop.m_values.length() > size_t(0))
		{
			StringArray values = StringUtil::Split(prop.m_values, "#");
			for (size_t i = 0; i < values.size(); i++)
			{
				String key = "%" + StringUtil::ToString(i + 1) + "s";
				if (StringUtil::Find(desc, key))
				{
					String value = values[i];
					desc = StringUtil::Replace(desc, key, value);
				}
			}
		}
		m_desc->SetText(desc.c_str());
		m_desc->SetTextLineExtraSpace(10.0f);

		int price = prop.m_price;
		m_price->SetVisible(price > 0);
		m_priceNum->SetText(StringUtil::ToString(price).c_str());

		float width = m_price->GetPixelSize().x;
		int digits = 0;
		while (price > 10)
		{
			digits++;
			price /= 10;
		}
		float left = (width - (35.0f + 12 * digits)) / 2;
		m_priceIcon->SetXPosition(UDim(0, left));
		m_priceNum->SetXPosition(UDim(0, 35.0f + left));

		m_priceIcon->SetImage(UICommon::getCurrencyIcon().c_str());
		m_status = prop.m_status;
		updateData();
	}

	void GUICustomPropItem::updateProp(CustomProp prop)
	{
		if (prop.m_name.length() > size_t(0))
		{
			String name = LanguageManager::Instance()->getString(prop.m_name);
			m_name->SetText(name.c_str());
			m_prop.m_name = prop.m_name;
		}
		if (prop.m_image.length() > size_t(0))
		{
			m_icon->SetImage(prop.m_image.c_str());
			m_prop.m_image = prop.m_image;
		}
		String desc;
		if (prop.m_desc.length() > size_t(0))
		{
			desc = LanguageManager::Instance()->getString(prop.m_desc);
			m_prop.m_desc = prop.m_desc;
		}
		else
		{
			desc = LanguageManager::Instance()->getString(m_prop.m_desc);
		}
		if (prop.m_values.length() > size_t(0))
		{
			StringArray values = StringUtil::Split(prop.m_values, "#");
			for (size_t i = 0; i < values.size(); i++)
			{
				String key = "%" + StringUtil::ToString(i + 1) + "s";
				if (StringUtil::Find(desc, key))
				{
					String value = values[i];
					desc = StringUtil::Replace(desc, key, value);
				}
			}
		}
		m_desc->SetText(desc.c_str());
		m_desc->SetTextLineExtraSpace(10.0f);

		if (prop.m_price != -1)
		{
			int price = prop.m_price;
			m_price->SetVisible(price > 0);
			m_priceNum->SetText(StringUtil::ToString(price).c_str());
			float width = m_price->GetPixelSize().x;
			int digits = 0;
			while (price > 10)
			{
				digits++;
				price /= 10;
			}
			float left = (width - (35.0f + 12 * digits)) / 2;
			m_priceIcon->SetXPosition(UDim(0, left));
			m_priceNum->SetXPosition(UDim(0, 35.0f + left));
			m_priceIcon->SetImage(UICommon::getCurrencyIcon().c_str());
			m_prop.m_price = prop.m_price;
		}
		if (prop.m_status != CustomPropStatus::Undefine)
		{
			m_status = prop.m_status;
			m_prop.m_status = prop.m_status;
		}
		updateData();
	}

	void GUICustomPropItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());
		updateData();
		BaseClass::UpdateSelf(nTimeElapse);
	}

	bool GUICustomPropItem::onButtonClick(const EventArgs & events)
	{
		if (Blockman::Instance()->m_pPlayer->m_wallet)
		{
			ui64 money = Blockman::Instance()->m_pPlayer->m_wallet->getCurrency();
			if (money < m_prop.m_price)
				PersonalShopTipEvent::emit(LanguageManager::Instance()->getString(LanguageKey::LACK_OF_MONEY).c_str());
			else
				GameClient::CGame::Instance()->getNetwork()->getSender()->sendBuyCustomProp(m_prop.m_uniqueId);
		}
		return true;
	}

	void GUICustomPropItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	vector<GUIWindow*>::type GUICustomPropItem::getPrivateChildren()
	{
		return { m_window };
	}

}
