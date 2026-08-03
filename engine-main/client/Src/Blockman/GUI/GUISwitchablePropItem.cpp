#include "GUISwitchablePropItem.h"
#include "UI/GUIWindowManager.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"
#include "Util/ClientEvents.h"
#include "cEntity/EntityPlayerSP.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "game.h"
#include "Setting/GameTypeSetting.h"
#include "Util/UICommon.h"

namespace BLOCKMAN
{
	GUISwitchablePropItem::GUISwitchablePropItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("SwitchablePropItem.json"));
		renameComponents(m_window);
		m_name = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(0));
		m_icon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(1));
		m_price = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(2));
		m_priceIcon = dynamic_cast<GUIStaticImage*>(m_price->GetChildByIndex(0));
		m_priceNum = dynamic_cast<GUIStaticText*>(m_price->GetChildByIndex(1));
		m_buy = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(3));
		auto prop = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(4));
		prop->subscribeEvent(EventButtonClick, std::bind(&GUISwitchablePropItem::onPropClick, this, std::placeholders::_1));
		m_buy->subscribeEvent(EventButtonClick, std::bind(&GUISwitchablePropItem::onBuyClick, this, std::placeholders::_1));
		AddWindowToDrawList(*m_window);
		SetTouchable(false);
	}

	void GUISwitchablePropItem::removeComponents()
	{
		if (m_window)
		{
			RemoveWindowFromDrawList(*m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	void GUISwitchablePropItem::updateData()
	{	
		switch (m_status)
		{
		case BLOCKMAN::SwitchablePropStatus::DontBuy:
			m_buy->SetNormalImage("set:app_shop.json image:app_shop_buy");
			m_buy->SetPushedImage("set:app_shop.json image:app_shop_buy");
			m_buy->SetEnabled(false);
			if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::HASHall)
				m_buy->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_TIGER_LOTTERY_LOCK).c_str());
			else
				m_buy->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_SHOP_DONT_BUY).c_str());
			break;
		case BLOCKMAN::SwitchablePropStatus::CanBuy:
			m_buy->SetNormalImage("set:app_shop.json image:app_shop_buy");
			m_buy->SetPushedImage("set:app_shop.json image:app_shop_buy");
			m_buy->SetEnabled(true);
			m_buy->SetText(LanguageManager::Instance()->getString(LanguageKey::SHOP_BTN_BUY).c_str());
			break;
		case BLOCKMAN::SwitchablePropStatus::HasGet:
			m_buy->SetNormalImage("set:app_shop.json image:app_shop_has_get");
			m_buy->SetPushedImage("set:app_shop.json image:app_shop_has_get");
			m_buy->SetEnabled(true);
			if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::HASHall)
				m_buy->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_CLICK_USE).c_str());
			else
				m_buy->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_SHOP_HAS_GET).c_str());
			break;
		case BLOCKMAN::SwitchablePropStatus::HasUse:
			m_buy->SetNormalImage("set:app_shop.json image:app_shop_has_use");
			m_buy->SetPushedImage("set:app_shop.json image:app_shop_has_use");
			m_buy->SetEnabled(true);
			m_buy->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_SHOP_HAS_USE).c_str());
			break;
		default:
			break;
		}
	}

	void GUISwitchablePropItem::setSwitchableProp(SwitchableProp prop)
	{
		m_prop = prop;
		String name = LanguageManager::Instance()->getString(prop.m_name);
		m_name->SetText(name.c_str());
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
		float left = (width - (35.0f + 12 * digits)) / 2;
		m_priceIcon->SetXPosition(UDim(0, left));
		m_priceNum->SetXPosition(UDim(0, 35.0f + left));

		m_priceIcon->SetImage(UICommon::getCurrencyIcon().c_str());
		m_buy->SetVisible(price >= 0);

		m_status = prop.m_status;
		m_window->SetBackImage(m_prop.m_rare == 0 ? "set:app_shop.json image:app_shop_item_bg" : "set:hideandseek.json image:rare_bg");
	}

	void GUISwitchablePropItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());
		updateData();
		BaseClass::UpdateSelf(nTimeElapse);
	}

	bool GUISwitchablePropItem::onBuyClick(const EventArgs & events)
	{
		switch (m_prop.m_status)
		{
		case SwitchablePropStatus::CanBuy:
			if (Blockman::Instance()->m_pPlayer->m_wallet)
			{
				ui64 money = Blockman::Instance()->m_pPlayer->m_wallet->getCurrency();
				if (money < m_prop.m_price)
					PersonalShopTipEvent::emit(LanguageManager::Instance()->getString(LanguageKey::LACK_OF_MONEY).c_str());
				else
					GameClient::CGame::Instance()->getNetwork()->getSender()->sendBuySwitchableProp(m_prop.m_uniqueId);
			}
			break;
		case SwitchablePropStatus::HasGet:
			GameClient::CGame::Instance()->getNetwork()->getSender()->sendBuySwitchableProp(m_prop.m_uniqueId);
			break;
		default:
			break;
		}
		return true;
	}

	bool GUISwitchablePropItem::onPropClick(const EventArgs & events)
	{
		ShowSwitchablePropEvent::emit(m_prop.m_name, m_prop.m_image, m_prop.m_desc, m_prop.m_rare);
		return true;
	}

	void GUISwitchablePropItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	vector<GUIWindow*>::type GUISwitchablePropItem::getPrivateChildren()
	{
		return { m_window };
	}

}
