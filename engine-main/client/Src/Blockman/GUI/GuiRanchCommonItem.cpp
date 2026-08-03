#include "GuiRanchCommonItem.h"
#include "UI/GUIWindowManager.h"
#include "Util/LanguageKey.h"
#include "Util/LanguageManager.h"

#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Inventory/Wallet.h"
#include "Util/ClientEvents.h"
#include "Network/ClientNetwork.h"
#include "Util/UICommon.h"
#include "Setting/RanchSetting.h"

namespace BLOCKMAN
{

	void GuiRanchCommonItem::removeComponents()
	{
		if (m_window)
		{
			RemoveWindowFromDrawList(*m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	vector<GUIWindow*>::type GuiRanchCommonItem::getPrivateChildren()
	{
		return { m_window };
	}

	GuiRanchCommonItem::GuiRanchCommonItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		init("RanchHousePanelItem.json");
	}

	void GuiRanchCommonItem::init(const String & jsonName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON(jsonName.c_str()));
		renameComponents(m_window);
		m_icon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		m_tvNum = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(1));
		m_done = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(2));
		m_btnBuy = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(3));
		m_tvNotReach = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(4));
		if (LanguageManager::Instance())
		{
			m_btnBuy->GetChildByIndex(0)->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_BTN_BUY_ALL).c_str());
			m_tvNotReach->SetText(LanguageManager::Instance()->getString("gui_ranch_not_reach").c_str());
		}
		m_tvPrice = dynamic_cast<GUIStaticText*>(m_btnBuy->GetChildByIndex(2));
		m_btnBuy->subscribeEvent(EventButtonClick, std::bind(&GuiRanchCommonItem::onClick, this, std::placeholders::_1));
		m_icon->subscribeEvent(EventWindowTouchUp, std::bind(&GuiRanchCommonItem::iconClick, this, std::placeholders::_1));
		m_icon->SetTouchable(true);
		AddWindowToDrawList(*m_window);
	}

	void GuiRanchCommonItem::resetWindow(const String & jsonName)
	{
		removeComponents();
		init(jsonName);
	}

	void GuiRanchCommonItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());
		BaseClass::UpdateSelf(nTimeElapse);

		if (UICommon::checkRanchParam())
		{
			if (m_itemRecipe.itemId == 900001)
			{
				i64 gameCurrency = Blockman::Instance()->m_pPlayer->m_wallet->getCurrency();
				setNum((i32) gameCurrency, m_itemRecipe.num);
				m_btnBuy->SetVisible(false);
			} 
			else if (m_itemRecipe.itemId == 900002)
			{
				RanchInfo* info = Blockman::Instance()->m_pPlayer->m_ranch->getInfo();
				setNum(info->prosperity, m_itemRecipe.num);
				m_btnBuy->SetVisible(false);
			}
			else
			{
				RanchStorage* storage = Blockman::Instance()->m_pPlayer->m_ranch->getStorage();
				i32 itemInventory = storage ? storage->getItemNum(m_itemRecipe.itemId) : 0;
				i32 needBuyItemNum = m_itemRecipe.num - itemInventory > 0 ? m_itemRecipe.num - itemInventory : 0;
				setNum(itemInventory, m_itemRecipe.num);
				m_tvNotReach->SetVisible(false);
				m_tvPrice->SetText(StringUtil::ToString(needBuyItemNum * RanchSetting::getRanchItemCubePrice(m_itemRecipe.itemId)).c_str());
			}
			
		}
		else
		{
			setNum(0, m_itemRecipe.num);
		}
	}

	
	void GuiRanchCommonItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GuiRanchCommonItem::setItemRecipe(RanchCommon item)
	{
		m_itemRecipe = item;
		auto ranchItem = RanchSetting::getRanchItem(m_itemRecipe.itemId);
		setIcon(ranchItem ? ranchItem->icon.c_str() : "");
	}

	i32 GuiRanchCommonItem::getItemId()
	{
		return m_itemRecipe.itemId;
	}


	void GuiRanchCommonItem::setNum(i32 num, i32 needNum)
	{
		if (m_tvNum)
		{
			m_tvNum->SetText(getNumString(num, needNum).c_str());
			m_done->SetVisible(num >= needNum);
			m_btnBuy->SetVisible(num < needNum);
			m_tvNotReach->SetVisible(num < needNum);
		}
	}

	void GuiRanchCommonItem::setPrice(i32 price)
	{
		if (m_tvPrice)
		{
			m_tvPrice->SetText(StringUtil::ToString(price).c_str());
		}
	}

	void GuiRanchCommonItem::setIcon(const GUIString& strIconImage)
	{
		m_iconImage = strIconImage.c_str();
		if (m_icon)
		{
			m_icon->SetImage(m_iconImage);
		}
	}

	void GuiRanchCommonItem::setIconArea(const URect & area)
	{
		if (m_icon)
		{
			m_icon->SetArea(area);
		}
	}


	bool GuiRanchCommonItem::SetProperty(const GUIString& strName, const GUIString& strValue)
	{

		if (GUIWindow::SetProperty(strName, strValue))
		{
			return true;
		}

		if (strName == GWP_SI_IMAGE_NAME)
		{
			setIcon(strValue);
			return true;
		}

		return false;
	}

	bool GuiRanchCommonItem::GetProperty(const GUIString& strName, GUIString& strValue)
	{

		if (GUIWindow::GetProperty(strName, strValue))
		{
			return true;
		}

		if (strName == GWP_SI_IMAGE_NAME)
		{
			strValue = m_iconImage.c_str();
			return true;
		}
		return false;
	}

	void GuiRanchCommonItem::GetPropertyList(GUIPropertyList& properlist)
	{
		GUIWindow::GetPropertyList(properlist);
		properlist.push_back(GWP_SI_IMAGE_NAME);
	}

	bool GuiRanchCommonItem::GetPropertyType(const GUIString& strName, GUIPropertyType& type)
	{
		if (GUIWindow::GetPropertyType(strName, type))
		{
			return true;
		}
		if (strName == GWP_SI_IMAGE_NAME)
		{
			type = GPT_IMAGE;
			return true;
		}

		return false;
	}

	bool GuiRanchCommonItem::onClick(const EventArgs & events)
	{
		if ( Blockman::Instance()->m_pPlayer && Blockman::Instance()->m_pPlayer->m_ranch)
		{
			RanchStorage* storage = Blockman::Instance()->m_pPlayer->m_ranch->getStorage();
			i32 lack = storage ? m_itemRecipe.num - storage->getItemNum(m_itemRecipe.itemId) : m_itemRecipe.num;
			i32 price = lack * m_itemRecipe.price;
			if (UICommon::checkMoney(price, CurrencyType::PLATFORM_MONEY))
			{
				ClientNetwork::Instance()->getSender()->sendBuyRanchItem(m_itemRecipe.itemId, lack, price, 1);
			}
		}
		return true;
	}

	bool GuiRanchCommonItem::iconClick(const EventArgs & events)
	{
		RanchItemClickEvent::emit(m_itemRecipe.itemId, GetName().c_str());
		return true;
	}

	String GuiRanchCommonItem::getNumString(i32 num, i32 needNum)
	{
		m_window->GetChildByIndex(2)->SetEnabled(num >= needNum);
		if (num >= needNum)
		{
			return StringUtil::Format("▢FF66ff00%d▢FF69523b/%d", num, needNum);
		}
		else
		{
			return StringUtil::Format("▢FFFF0020%d▢FF69523b/%d", num, needNum);
		}
	}


}