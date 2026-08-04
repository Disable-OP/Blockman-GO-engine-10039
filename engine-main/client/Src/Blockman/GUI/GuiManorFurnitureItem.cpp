#include "GuiManorFurnitureItem.h"
#include "UI/GUIWindowManager.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"
#include "World/Manor.h"
#include "Util/ClientEvents.h"
#include "cWorld/Blockman.h"
#include "Network/ClientNetwork.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Enums/Enums.h"
#include "game.h"
#include "Util/UICommon.h"

namespace BLOCKMAN
{
	void GuiManorFurnitureItem::removeComponents()
	{
		if (m_window)
		{
			RemoveWindowFromDrawList(*m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	vector<GUIWindow*>::type GuiManorFurnitureItem::getPrivateChildren()
	{
		return { m_window };
	}

	GuiManorFurnitureItem::GuiManorFurnitureItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("ManorFurnitureItem.json"));
		renameComponents(m_window);
		
		m_contentIcon = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(0));
		m_furnitureIcon = dynamic_cast<GUIStaticImage*>(m_contentIcon->GetChildByIndex(0));
		m_priceInfo = dynamic_cast<GUILayout*>(m_contentIcon->GetChildByIndex(1));
		m_currencyIcon = dynamic_cast<GUIStaticImage*>(m_priceInfo->GetChildByIndex(0));
		m_priceValue = dynamic_cast<GUIStaticText*>(m_priceInfo->GetChildByIndex(1));
		m_btnOperation = dynamic_cast<GUIButton*>(m_contentIcon->GetChildByIndex(2));
		m_btnOperation->subscribeEvent(EventButtonClick, std::bind(&GuiManorFurnitureItem::onClick, this, std::placeholders::_1));
		AddWindowToDrawList(*m_window);
	}

	void GuiManorFurnitureItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());
		BaseClass::UpdateSelf(nTimeElapse);
		if (m_manorFurniture)
		{
			m_furnitureIcon->SetImage(m_manorFurniture->icon.c_str());
			String btnText = "";
			switch (m_manorFurniture->operationType)
			{
			case OperationType::USE:
				m_priceInfo->SetVisible(false);
				btnText = LanguageManager::Instance()->getString(LanguageKey::GUI_MANOR_FURNITURE_BTN_USE).c_str();
				m_btnOperation->SetNormalImage("set:jail_break.json image:jail_break_btn_nor");
				m_btnOperation->SetPushedImage("set:jail_break.json image:jail_break_btn_pre");
				break;
			case OperationType::RECYCLE:
				m_priceInfo->SetVisible(false);
				btnText = LanguageManager::Instance()->getString(LanguageKey::GUI_MANOR_FURNITURE_BTN_RECYCLE).c_str();
				m_btnOperation->SetNormalImage("set:new_gui_material.json image:btn_green_nor");
				m_btnOperation->SetPushedImage("set:new_gui_material.json image:btn_green_pre");
				break;
			default:
				m_priceInfo->SetVisible(true);
				btnText = LanguageManager::Instance()->getString(LanguageKey::GUI_MANOR_FURNITURE_BTN_BUY).c_str();
				m_btnOperation->SetNormalImage("set:jail_break.json image:jail_break_btn_nor");
				m_btnOperation->SetPushedImage("set:jail_break.json image:jail_break_btn_pre");
				break;
			}
			m_currencyIcon->SetImage(UICommon::getCurrencyIconByType(m_manorFurniture->currencyType).c_str());
			m_priceValue->SetText(StringUtil::ToString(m_manorFurniture->price).c_str());
			m_btnOperation->SetText(btnText.c_str());
			m_btnOperation->SetEnabled(Blockman::Instance()->m_pPlayer->m_manor->getInfo()->isMaster);
		}
	}

	void GuiManorFurnitureItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	bool GuiManorFurnitureItem::onClick(const EventArgs & events)
	{
		if (m_manorFurniture)
		{
			switch (m_manorFurniture->operationType)
			{
			case OperationType::USE:
				if (Blockman::Instance()->m_pPlayer->m_isActorChange)
				{
					
					ShowResultTipDialogEvent::emit(ResultCode::COMMON, LanguageKey::GUI_MANOR_IS_FURNITURE_ACTOR);
					return false;
				}
				ClientNetwork::Instance()->getSender()->sendPutFurniture(m_manorFurniture->id, PutFurnitureStatus::START);
				SetPutFurnitureIdEvent::emit(m_manorFurniture->id);
				break;
			case OperationType::RECYCLE:
				ClientNetwork::Instance()->getSender()->sendRecycleFurniture(m_manorFurniture);
				break;
			default:
				if (Blockman::Instance()->m_pPlayer->m_wallet->getMoneyByCoinId((int)m_manorFurniture->currencyType) - m_manorFurniture->price >= 0)
				{
					ClientNetwork::Instance()->getSender()->sendBuyFurniture(m_manorFurniture);
				}
				else
				{
					i64 difference = m_manorFurniture->price - Blockman::Instance()->m_pPlayer->m_wallet->getMoneyByCoinId((int)m_manorFurniture->currencyType);
					ShowLackOfMoneyTipDialogEvent::emit((i32)difference, m_manorFurniture->currencyType);
				}
				break;
			}
		}
		return true;
	}
}