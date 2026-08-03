#include "GUISuperPropItem.h"
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
	GUISuperPropItem::GUISuperPropItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("SuperPropItem.json"));
		renameComponents(m_window);
		m_name = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(1));
		m_icon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(2));
		m_content = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(3));
		m_price = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(4));
		m_priceIcon = dynamic_cast<GUIStaticImage*>(m_price->GetChildByIndex(0));
		m_priceNum = dynamic_cast<GUIStaticText*>(m_price->GetChildByIndex(1));
		m_buy = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(5));
		m_descShow = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(6));
		m_descShow->subscribeEvent(EventButtonClick, std::bind(&GUISuperPropItem::onPropClick, this, std::placeholders::_1));
		m_buy->subscribeEvent(EventButtonClick, std::bind(&GUISuperPropItem::onBuyClick, this, std::placeholders::_1));
		AddWindowToDrawList(*m_window);
		SetTouchable(false);
	}

	void GUISuperPropItem::removeComponents()
	{
		if (m_window)
		{
			RemoveWindowFromDrawList(*m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	void GUISuperPropItem::updateData()
	{
		switch (m_status)
		{
		case BLOCKMAN::SuperPropStatus::DontBuy:
			m_buy->SetNormalImage("set:app_shop.json image:app_shop_buy");
			m_buy->SetPushedImage("set:app_shop.json image:app_shop_buy");
			m_buy->SetEnabled(false);
			m_buy->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_SHOP_DONT_BUY).c_str());
			break;
		case BLOCKMAN::SuperPropStatus::CanBuy:
			m_buy->SetNormalImage("set:app_shop.json image:app_shop_buy");
			m_buy->SetPushedImage("set:app_shop.json image:app_shop_buy");
			m_buy->SetEnabled(true);
			m_buy->SetText(LanguageManager::Instance()->getString(LanguageKey::SHOP_BTN_BUY).c_str());
			break;
		case BLOCKMAN::SuperPropStatus::Unlock:
			m_buy->SetNormalImage("set:app_shop.json image:app_shop_buy");
			m_buy->SetPushedImage("set:app_shop.json image:app_shop_buy");
			m_buy->SetEnabled(true);
			m_buy->SetText(LanguageManager::Instance()->getString(LanguageKey::SHOP_BTN_UNLOCK).c_str());
			break;
		case BLOCKMAN::SuperPropStatus::Switch:
			m_buy->SetNormalImage("set:app_shop.json image:app_shop_has_get");
			m_buy->SetPushedImage("set:app_shop.json image:app_shop_has_get");
			m_buy->SetEnabled(true);
			m_buy->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_SHOP_SWITCH).c_str());
			break;
		case BLOCKMAN::SuperPropStatus::Upgrade:
			m_buy->SetNormalImage("set:app_shop.json image:app_shop_has_use");
			m_buy->SetPushedImage("set:app_shop.json image:app_shop_has_use");
			m_buy->SetEnabled(true);
			m_buy->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_SHOP_TAB_UPGRADE).c_str());
			break;
		case BLOCKMAN::SuperPropStatus::HasGet:
			m_buy->SetNormalImage("set:app_shop.json image:app_shop_has_get");
			m_buy->SetPushedImage("set:app_shop.json image:app_shop_has_get");
			m_buy->SetEnabled(true);
			m_buy->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_SHOP_HAS_GET).c_str());
			break;
		case BLOCKMAN::SuperPropStatus::HasUse:
			m_buy->SetNormalImage("set:app_shop.json image:app_shop_has_use");
			m_buy->SetPushedImage("set:app_shop.json image:app_shop_has_use");
			m_buy->SetEnabled(true);
			m_buy->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_SHOP_HAS_USE).c_str());
			break;
		case BLOCKMAN::SuperPropStatus::MaxLv:
			m_buy->SetNormalImage("set:app_shop.json image:app_shop_buy");
			m_buy->SetPushedImage("set:app_shop.json image:app_shop_buy");
			m_buy->SetEnabled(false);
			m_buy->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_SHOP_TIP_MAX_LEVEL).c_str());
			break;
		default:
			break;
		}
	}

	void GUISuperPropItem::setSuperProp(SuperProp prop)
	{
		m_prop = prop;
		String name = LanguageManager::Instance()->getString(prop.m_name);
		m_name->SetText(name.c_str());
		String content = LanguageManager::Instance()->getString(prop.m_content);
		if (StringUtil::Find(content, "%d"))
		{
			m_content->SetText(StringUtil::Format(content.c_str(), prop.m_value).c_str());
		}
		else
		{
			m_content->SetText(content.c_str());
		}
		m_content->SetText(content.c_str());
		m_content->SetTextLineExtraSpace(10.0f);
		m_icon->SetImage(prop.m_image.c_str());

		int price = prop.m_price;
		m_priceNum->SetText(StringUtil::ToString(price).c_str());
		m_price->SetVisible(price > 0);

		float width = m_price->GetPixelSize().x;
		int digits = 0;
		while (price > 10)
		{
			digits++;
			price /= 10;
		}
		float left = (width - (40.0f + 12 * digits)) / 2;
		m_priceIcon->SetXPosition(UDim(0, left));
		m_priceNum->SetXPosition(UDim(0, 40.0f + left));

		m_priceIcon->SetImage(UICommon::getCurrencyIcon().c_str());
		m_buy->SetVisible(price >= 0);

		m_descShow->SetVisible(m_prop.m_desc.length() > size_t(0));

		m_status = prop.m_status;
		updateData();
	}

	void GUISuperPropItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());
		BaseClass::UpdateSelf(nTimeElapse);
	}

	bool GUISuperPropItem::onBuyClick(const EventArgs & events)
	{
		switch (m_prop.m_status)
		{
		case SuperPropStatus::CanBuy:
		case SuperPropStatus::Unlock:
		case SuperPropStatus::Switch:
		case SuperPropStatus::Upgrade:
			if (Blockman::Instance()->m_pPlayer->m_wallet)
			{
				ui64 money = Blockman::Instance()->m_pPlayer->m_wallet->getCurrency();
				if (money < m_prop.m_price)
					SuperShopTipEvent::emit(LanguageManager::Instance()->getString(LanguageKey::LACK_OF_MONEY).c_str());
				else
					GameClient::CGame::Instance()->getNetwork()->getSender()->sendBuySuperProp(m_prop.m_uniqueId);
			}
			break;
		case SuperPropStatus::HasGet:
			GameClient::CGame::Instance()->getNetwork()->getSender()->sendBuySuperProp(m_prop.m_uniqueId);
			break;
		default:
			break;
		}
		return true;
	}

	bool GUISuperPropItem::onPropClick(const EventArgs & events)
	{
		ShowSuperPropEvent::emit(m_prop.m_name, m_prop.m_image, m_prop.m_desc);
		return true;
	}

	void GUISuperPropItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	vector<GUIWindow*>::type GUISuperPropItem::getPrivateChildren()
	{
		return { m_window };
	}

}
