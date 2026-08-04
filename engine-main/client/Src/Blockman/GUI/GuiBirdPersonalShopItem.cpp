#include "GuiBirdPersonalShopItem.h"
#include "UI/GUIWindowManager.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Util/ClientEvents.h"
#include "Util/UICommon.h"

namespace BLOCKMAN
{
	
	void GuiBirdPersonalShopItem::removeComponents()
	{
		if (m_window)
		{
			RemoveWindowFromDrawList(*m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	
	vector<GUIWindow*>::type GuiBirdPersonalShopItem::getPrivateChildren()
	{
		return { m_window };
	}

	GuiBirdPersonalShopItem::GuiBirdPersonalShopItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		removeComponents();
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("BirdPersonalShopItem.json"));
		renameComponents(m_window);

		m_name = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(0));
		m_image = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(2));
		m_money = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(1)->GetChildByIndex(1));
		m_moneyIcon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(1)->GetChildByIndex(0));
		m_alreadyBuy = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(1)->GetChildByIndex(2));
		m_discount = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(5));
		m_discountNum = dynamic_cast<GUIStaticText*>(m_discount->GetChildByIndex(0));
		m_topLeft = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(3));
		m_topRight = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(4));
		m_discount->SetVisible(false);
		m_alreadyBuy->SetVisible(false);

		m_window->subscribeEvent(EventWindowTouchUp, std::bind(&GuiBirdPersonalShopItem::onClick, this, std::placeholders::_1));	
	
		AddWindowToDrawList(*m_window);
	}

	void GuiBirdPersonalShopItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());
		BaseClass::UpdateSelf(nTimeElapse);

		m_image->SetImage(m_goods.icon.c_str());
		String name = LanguageManager::Instance()->getString(m_goods.name);
		m_name->SetText(name.c_str());
		m_money->SetText(StringUtil::ToString(m_goods.price).c_str());
		m_moneyIcon->SetImage(UICommon::getCurrencyIconByType(m_goods.currencyType).c_str());
		m_topLeft->SetImage(m_goods.topLeftIcon.c_str());
		m_topLeft->GetChildByIndex(0)->SetText(LanguageManager::Instance()->getString(m_goods.topLeftName).c_str());
		m_topRight->SetImage(m_goods.topRightIcon.c_str());
		m_topRight->GetChildByIndex(0)->SetText(LanguageManager::Instance()->getString(m_goods.topRightName).c_str());
		m_alreadyBuy->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_BIRD_PERSONAL_SHOP_ALREADY_BUY).c_str());
		if (m_goods.discount != "")
		{
			m_discountNum->SetText(m_goods.discount.c_str());
			m_discount->SetVisible(true);
		}
		if (m_goods.limit == 0)
		{
			m_money->SetVisible(false);
			m_moneyIcon->SetVisible(false);
			m_alreadyBuy->SetVisible(true);
		}
	}

	
	void GuiBirdPersonalShopItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	bool GuiBirdPersonalShopItem::onClick(const EventArgs& args)
	{
		UpdateBirdPersonalShopInfoEvent::emit(m_goods, m_tabId);
		return true;
	}

	void GuiBirdPersonalShopItem::setGoodInfo(BirdGiftBag goods, i32 tabId)
	{
		m_goods = goods;
		m_tabId = tabId;
	}

}