#include "GuiRanchSellItem.h"
#include "UI/GUIWindowManager.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Network/ClientNetwork.h"
#include "Util/ClientEvents.h"
#include "Util/UICommon.h"

namespace BLOCKMAN
{

	void GuiRanchSellItem::removeComponents()
	{
		if (m_window)
		{
			RemoveWindowFromDrawList(*m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	vector<GUIWindow*>::type GuiRanchSellItem::getPrivateChildren()
	{
		return { m_window };
	}

	GuiRanchSellItem::GuiRanchSellItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("RanchSellItem.json"));
		renameComponents(m_window);
		m_name = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(0)->GetChildByIndex(0));
		m_desc = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(0)->GetChildByIndex(1));
		GUILayout * info = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(0)->GetChildByIndex(2));
		m_icon = dynamic_cast<GUIStaticImage*>(info->GetChildByIndex(1));
		m_tvNum = dynamic_cast<GUIStaticText*>(info->GetChildByIndex(2));
		m_btnSub = dynamic_cast<GUIButton*>(info->GetChildByIndex(0));
		m_btnAdd = dynamic_cast<GUIButton*>(info->GetChildByIndex(3));

		m_btnSub->subscribeEvent(EventButtonClick, std::bind(&GuiRanchSellItem::onClick, this, std::placeholders::_1, VIewId::BTN_SUB));
		m_btnAdd->subscribeEvent(EventButtonClick, std::bind(&GuiRanchSellItem::onClick, this, std::placeholders::_1, VIewId::BTN_ADD));

		m_tvPrice = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(0)->GetChildByIndex(3)->GetChildByIndex(1));
		m_window->GetChildByIndex(0)->GetChildByIndex(4)->subscribeEvent(EventButtonClick, std::bind(&GuiRanchSellItem::onClick, this, std::placeholders::_1, VIewId::BTN_SURE));

		if (LanguageManager::Instance())
		{
			m_window->GetChildByIndex(0)->GetChildByIndex(4)->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_SHOP_TAB_SELL).c_str());
		}

		AddWindowToDrawList(*m_window);
	}

	void GuiRanchSellItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());
		BaseClass::UpdateSelf(nTimeElapse);

		if (!UICommon::checkPlayerParam() || !Blockman::Instance()->m_pPlayer->m_ranch->getStorage() || !LanguageManager::Instance() || !IsVisible())
		{
			return;
		}

		RanchItem* ranchItem = RanchSetting::getRanchItem(m_commonItem.itemId);
		auto storage = Blockman::Instance()->m_pPlayer->m_ranch->getStorage();

		m_btnSub->SetEnabled(m_num > 1);
		m_btnAdd->SetEnabled(m_num < storage->getItemNum(m_commonItem.itemId));
		m_window->GetChildByIndex(0)->GetChildByIndex(4)->SetEnabled(m_num > 0);
		if (ranchItem)
		{
			m_icon->SetImage(ranchItem->icon.c_str());
			m_name->SetText(LanguageManager::Instance()->getString(ranchItem->name).c_str());
			m_desc->SetText(LanguageManager::Instance()->getString(ranchItem->desc).c_str());
			m_tvNum->SetText(StringUtil::Format("X %d", m_num).c_str());
			int price = m_num * m_commonItem.price;
			m_tvPrice->SetText( StringUtil::ToString(price).c_str());
		}
	}


	void GuiRanchSellItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GuiRanchSellItem::setItem(const RanchCommon & item)
	{
		m_commonItem = item;
		
		if (UICommon::checkRanchParam() && Blockman::Instance()->m_pPlayer->m_ranch->getStorage())
		{
			RanchStorage* storage = Blockman::Instance()->m_pPlayer->m_ranch->getStorage();
			i32 itemNum = storage->getItemNum(m_commonItem.itemId);
			m_num = itemNum > 1 ? itemNum / 2 : itemNum;
		}
	}


	bool GuiRanchSellItem::onClick(const EventArgs & args, VIewId viewId)
	{
		switch (viewId)
		{
		case VIewId::BTN_SUB:
			m_num -= 1;
			break;
		case VIewId::BTN_ADD:
			m_num += 1;
			break;
		case VIewId::BTN_SURE:
			if (UICommon::checkRanchParam() && Blockman::Instance()->m_pPlayer->m_ranch->getStorage())
			{
				RanchStorage* storage = Blockman::Instance()->m_pPlayer->m_ranch->getStorage();
				if (m_num > 0 && storage->checkInventory(m_commonItem.itemId, m_num))
				{
					SetVisible(false);
					ClientNetwork::Instance()->getSender()->sendRanchSellItem(m_commonItem.itemId, m_num);
					return true;
				}
			}

			ShowRanchCommonTipEvent::emit("gui_ranch_sell_failure_lack_items");
			break;
		}
		return true;
	}

	void GuiRanchSellItem::setBack(bool isLeft, bool isTop)
	{
		m_window->SetBackImage(isLeft ? "set:ranch_main.json image:ranch_sell_item_left_bg" : "set:ranch_main.json image:ranch_sell_item_right_bg");
		m_window->GetChildByIndex(0)->SetPosition(UVector2(UDim(0, isLeft ?  20.f : 0.f), UDim(0, 0)));

		String str = StringUtil::Format("%d %d %d %d", isLeft ? 34 : 14, isLeft ? 14 : 34, isTop ? 95 : 70, isTop ? 14 : 55);
		//m_window->SetProperty(GWP_WINDOW_STRETCH_OFFSET, isLeft ? "34 14 70 55" : "14 34 70 55");
		m_window->SetProperty(GWP_WINDOW_STRETCH_OFFSET, str.c_str());
	}


}