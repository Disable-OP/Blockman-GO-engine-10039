#include "GUIUpgradePropItem.h"
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
	GUIUpgradePropItem::GUIUpgradePropItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("UpgradePropItem.json"));
		renameComponents(m_window);
		m_name = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(1));
		m_icon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(2));
		auto m_descTip = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(3));
		m_descTip->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_SHOP_GOODS_TIP).c_str());
		m_desc = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(4));
		m_price = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(5));
		m_priceIcon = dynamic_cast<GUIStaticImage*>(m_price->GetChildByIndex(0));
		m_priceNum = dynamic_cast<GUIStaticText*>(m_price->GetChildByIndex(1));
		m_upgrade = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(6));
		m_upgrade->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_SHOP_TAB_UPGRADE).c_str());
		m_upgrade->subscribeEvent(EventButtonClick, std::bind(&GUIUpgradePropItem::onButtonClick, this, std::placeholders::_1));
		AddWindowToDrawList(*m_window);
		SetTouchable(false);
	}

	void GUIUpgradePropItem::removeComponents()
	{
		if (m_window)
		{
			RemoveWindowFromDrawList(*m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	void GUIUpgradePropItem::updateData()
	{	
		if (m_status == UpgradePropStatus::Default)
			return;
		switch (m_status)
		{
		case BLOCKMAN::UpgradePropStatus::DontBuy:
			m_upgrade->SetNormalImage("set:app_shop.json image:app_shop_buy");
			m_upgrade->SetPushedImage("set:app_shop.json image:app_shop_buy");
			m_upgrade->SetEnabled(false);
			m_upgrade->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_SHOP_DONT_BUY).c_str());
			break;
		case BLOCKMAN::UpgradePropStatus::CanBuy:
			m_upgrade->SetNormalImage("set:app_shop.json image:app_shop_buy");
			m_upgrade->SetPushedImage("set:app_shop.json image:app_shop_buy");
			m_upgrade->SetEnabled(true);
			m_upgrade->SetText(LanguageManager::Instance()->getString(LanguageKey::SHOP_BTN_BUY).c_str());
			break;
		case BLOCKMAN::UpgradePropStatus::Unlock:
			m_upgrade->SetNormalImage("set:app_shop.json image:app_shop_buy");
			m_upgrade->SetPushedImage("set:app_shop.json image:app_shop_buy");
			m_upgrade->SetEnabled(true);
			m_upgrade->SetText(LanguageManager::Instance()->getString(LanguageKey::SHOP_BTN_UNLOCK).c_str());
			break;
		case BLOCKMAN::UpgradePropStatus::Switch:
			m_upgrade->SetNormalImage("set:app_shop.json image:app_shop_has_get");
			m_upgrade->SetPushedImage("set:app_shop.json image:app_shop_has_get");
			m_upgrade->SetEnabled(true);
			m_upgrade->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_SHOP_SWITCH).c_str());
			break;
		case BLOCKMAN::UpgradePropStatus::Upgrade:
			m_upgrade->SetNormalImage("set:app_shop.json image:app_shop_has_use");
			m_upgrade->SetPushedImage("set:app_shop.json image:app_shop_has_use");
			m_upgrade->SetEnabled(true);
			m_upgrade->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_SHOP_TAB_UPGRADE).c_str());
			break;
		case BLOCKMAN::UpgradePropStatus::HasGet:
			m_upgrade->SetNormalImage("set:app_shop.json image:app_shop_has_get");
			m_upgrade->SetPushedImage("set:app_shop.json image:app_shop_has_get");
			m_upgrade->SetEnabled(true);
			m_upgrade->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_SHOP_HAS_GET).c_str());
			break;
		case BLOCKMAN::UpgradePropStatus::HasUse:
			m_upgrade->SetNormalImage("set:app_shop.json image:app_shop_has_use");
			m_upgrade->SetPushedImage("set:app_shop.json image:app_shop_has_use");
			m_upgrade->SetEnabled(true);
			m_upgrade->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_SHOP_HAS_USE).c_str());
			break;
		case BLOCKMAN::UpgradePropStatus::MaxLv:
			m_upgrade->SetNormalImage("set:app_shop.json image:app_shop_buy");
			m_upgrade->SetPushedImage("set:app_shop.json image:app_shop_buy");
			m_upgrade->SetEnabled(false);
			m_upgrade->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_SHOP_TIP_MAX_LEVEL).c_str());
			break;
		default:
			break;
		}
	}

	void GUIUpgradePropItem::setUpgradeProp(UpgradeProp prop)
	{
		m_prop = prop;
		String name = LanguageManager::Instance()->getString(prop.m_name);
		m_name->SetText(name.c_str());
		m_icon->SetImage(prop.m_image.c_str());

		String desc = LanguageManager::Instance()->getString(prop.m_desc);
		if (StringUtil::Find(desc, "%d"))
		{
			m_desc->SetText(StringUtil::Format(desc.c_str(), prop.m_level, prop.m_value, prop.m_nextValue).c_str());
		}
		else
		{
			m_desc->SetText(desc.c_str());
		}
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

		m_upgrade->SetVisible(prop.m_level >= 0 && price >= 0);
		if (price > 0)
		{
			m_upgrade->SetEnabled(true);
			m_upgrade->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_SHOP_TAB_UPGRADE).c_str());
		}
		else
		{
			m_upgrade->SetEnabled(false);
			m_upgrade->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_SHOP_TIP_MAX_LEVEL).c_str());
		}
		m_status = prop.m_status;
		updateData();
	}

	void GUIUpgradePropItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());
		updateData();
		BaseClass::UpdateSelf(nTimeElapse);
	}

	bool GUIUpgradePropItem::onButtonClick(const EventArgs & events)
	{
		if (Blockman::Instance()->m_pPlayer->m_wallet)
		{
			ui64 money = Blockman::Instance()->m_pPlayer->m_wallet->getCurrency();
			if (money < m_prop.m_price)
				PersonalShopTipEvent::emit(LanguageManager::Instance()->getString(LanguageKey::LACK_OF_MONEY).c_str());
			else
				GameClient::CGame::Instance()->getNetwork()->getSender()->sendBuyUpgradeProp(m_prop.m_uniqueId);
		}
		return true;
	}

	void GUIUpgradePropItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	vector<GUIWindow*>::type GUIUpgradePropItem::getPrivateChildren()
	{
		return { m_window };
	}

}
