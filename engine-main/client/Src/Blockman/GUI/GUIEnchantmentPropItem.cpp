#include "GUIEnchantmentPropItem.h"
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
	GUIEnchantmentPropItem::GUIEnchantmentPropItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("EnchantmentPropItem.json"));
		renameComponents(m_window);
		m_name = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(1));
		m_icon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(3));
		auto m_descTip = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(4));
		m_descTip->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_SHOP_GOODS_TIP).c_str());
		m_desc = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(5));
		m_price = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(6));
		m_priceIcon = dynamic_cast<GUIStaticImage*>(m_price->GetChildByIndex(0));
		m_priceNum = dynamic_cast<GUIStaticText*>(m_price->GetChildByIndex(1));
		m_buy = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(7));
		m_buy->SetText(LanguageManager::Instance()->getString(LanguageKey::SHOP_BTN_BUY).c_str());
		m_buy->subscribeEvent(EventButtonClick, std::bind(&GUIEnchantmentPropItem::onButtonClick, this, std::placeholders::_1));
		AddWindowToDrawList(*m_window);
		SetTouchable(false);
	}

	void GUIEnchantmentPropItem::removeComponents()
	{
		if (m_window)
		{
			RemoveWindowFromDrawList(*m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	void GUIEnchantmentPropItem::updateData()
	{	
		
	}

	void GUIEnchantmentPropItem::setEnchantmentProp(EnchantmentProp prop)
	{
		m_prop = prop;
		String name = LanguageManager::Instance()->getString(prop.m_name);
		m_name->SetText(name.c_str());
		m_icon->SetImage(prop.m_image.c_str());

		String desc = LanguageManager::Instance()->getString(prop.m_desc);
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
		m_buy->SetVisible(true);
		m_buy->SetEnabled(!m_prop.m_isBuy);
		m_buy->SetText(LanguageManager::Instance()->getString(m_prop.m_isBuy ? LanguageKey::SHOP_BTN_HAVE_BOUGHT  : LanguageKey::SHOP_BTN_BUY ).c_str());

	}

	void GUIEnchantmentPropItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());
		updateData();
		BaseClass::UpdateSelf(nTimeElapse);
	}

	bool GUIEnchantmentPropItem::onButtonClick(const EventArgs & events)
	{
		if (Blockman::Instance()->m_pPlayer->m_wallet)
		{
			ui64 money = Blockman::Instance()->m_pPlayer->m_wallet->getCurrency();
			if (money < m_prop.m_price)
				PersonalShopTipEvent::emit(LanguageManager::Instance()->getString(LanguageKey::LACK_OF_MONEY).c_str());
			else
				GameClient::CGame::Instance()->getNetwork()->getSender()->sendBuyEnchantmentProp(m_prop.m_uniqueId);
		}
		return true;
	}

	void GUIEnchantmentPropItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	vector<GUIWindow*>::type GUIEnchantmentPropItem::getPrivateChildren()
	{
		return { m_window };
	}

}
