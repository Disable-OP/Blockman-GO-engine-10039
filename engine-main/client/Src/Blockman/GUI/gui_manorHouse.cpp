#include "gui_manorHouse.h"
#include "Util/LanguageKey.h"
#include "GuiManorHouseTemplateItem.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "cWorld/Blockman.h"
#include "Util/ClientEvents.h"
#include "GuiDef.h"
#include "game.h"
#include "Util/UICommon.h"

using namespace LORD;

namespace BLOCKMAN 
{

	gui_manorHouse::gui_manorHouse()
		: gui_layout("ManorHouse.json")
	{
		
	}

	gui_manorHouse::~gui_manorHouse()
	{
		m_subscriptionGuard.unsubscribeAll();
		m_manorHouses.clear();
	}

	void gui_manorHouse::onLoad()
	{
		m_itemGridView = (GUIGridView*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_GRID_VIEW, "ManorHouse-Right-Templates");
		m_itemGridView->SetArea(UDim(0, 12.f), UDim(0, 12.f), UDim(1.f, -24.0f), UDim(1.f, -24.0f));
		m_itemGridView->InitConfig(3.f, 3.f, 3);
		m_btnOperation = getWindow<GUIButton>("ManorHouse-Btn-Operation");
		
		m_subscriptionGuard.add(ManorOnUpdateEvent::subscribe(std::bind(&gui_manorHouse::manorOnUpdate, this)));
		m_subscriptionGuard.add(SureBuildManorHouseEvent::subscribe(std::bind(&gui_manorHouse::buildHoues, this)));
		m_subscriptionGuard.add(ManorHouseSelectEvent::subscribe(std::bind(&gui_manorHouse::onItemClick, this, std::placeholders::_1)));
		m_subscriptionGuard.add(RefreshManorUiEvent::subscribe(std::bind(&gui_manorHouse::resetBuildResidueCoolTime, this)));
		
		getWindow("ManorHouse-Right")->AddChildWindow(m_itemGridView);
		initItem();
	}

	void gui_manorHouse::onUpdate(ui32 nTimeElapse)
	{
		if (m_gridViewPixelSizeX != m_itemGridView->GetPixelSize().x)
		{
			m_gridViewPixelSizeX = m_itemGridView->GetPixelSize().x;
			itemSizeChange();
		}
		refreshUI(nTimeElapse);
	}

	bool gui_manorHouse::onClick(const EventArgs & events,  ViewId id)
	{
		if (!m_selectHouse || !Blockman::Instance()->m_pPlayer || !Blockman::Instance()->m_pPlayer->m_wallet)
		{
			return false;
		}
		switch (id)
		{
		case ViewId::BUY:
			{
				if (Blockman::Instance()->m_pPlayer->m_wallet->getMoneyByCoinId((int)m_selectHouse->currencyType) - m_selectHouse->price >= 0)
				{
					ClientNetwork::Instance()->getSender()->sendBuyHouse(m_selectHouse);
				}
				else
				{
					i64 difference = m_selectHouse->price - Blockman::Instance()->m_pPlayer->m_wallet->getMoneyByCoinId((int)m_selectHouse->currencyType);
					ShowLackOfMoneyTipDialogEvent::emit((i32)difference, m_selectHouse->currencyType);
				}
			}
			break;
		case ViewId::BUILD:
			ShowManorBuildTipDialogEvent::emit();
			break;
		}
		return true;
	}

	bool gui_manorHouse::onItemClick(ManorHouse* house)
	{
		this->m_selectHouse = house;
		if (m_selectHouse && Blockman::Instance()->m_pPlayer->m_manor)
		{
			Blockman::Instance()->m_pPlayer->m_manor->setHouseSelect(m_selectHouse->id);
		}
		return true;
	}

	bool gui_manorHouse::manorOnUpdate()
	{
		if (Blockman::Instance()->m_pPlayer->m_manor)
		{
			m_manorHouses = Blockman::Instance()->m_pPlayer->m_manor->getHouses();
			onItemClick(Blockman::Instance()->m_pPlayer->m_manor->getUsedHouse());
		}
		initItem();
		return true;
	}

	bool gui_manorHouse::initItem()
	{
		m_itemGridView->RemoveAllItems();
		for (ManorHouse* item : m_manorHouses)
		{
			static unsigned count = 0;
			count++;
			String itemLayout = StringUtil::Format("ManorHouse-Template-Item-%d", count).c_str();
			if (GUIWindowManager::Instance()->GetGUIWindow(itemLayout.c_str()))
			{
				return false;
			}
			float w = (m_itemGridView->GetPixelSize().x - 6.0f) / 3;
			GuiManorHouseTemplateItem* houseTemplateItem = (GuiManorHouseTemplateItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_MANOR_HOUSE_TEMPLATE_ITEM, itemLayout.c_str());
			houseTemplateItem->setManorHouse(item);
			houseTemplateItem->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(0.0f, w), UDim(0.0f, w));
			m_itemGridView->AddItem(houseTemplateItem);
		}
		return true;
	}

	bool gui_manorHouse::itemSizeChange()
	{
		for (size_t i = 0; i< m_itemGridView->GetItemCount(); i++)
		{
			float w = (m_itemGridView->GetPixelSize().x - 6.0f) / 3;
			m_itemGridView->GetItem(i)->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(0.0f, w), UDim(0.0f, w));
		}
		return true;
	}

	bool gui_manorHouse::buildHoues()
	{
		if (m_selectHouse)
		{
			m_buildResidueCoolTime = m_selectHouse->cdTime * 1000;
			ClientNetwork::Instance()->getSender()->sendBuildHouse(m_selectHouse);
		}
		return true;
	}

	void gui_manorHouse::refreshUI(ui32 timeElapse)
	{
		if (m_selectHouse)
		{
			getWindow<GUIStaticImage>("ManorHouse-Left-Preview")->SetImage(m_selectHouse->icon.c_str());
			bool isShowCurrencyIcon = !m_selectHouse->isBuy && m_selectHouse->unlockLevel <= Blockman::Instance()->m_pPlayer->m_manor->getInfo()->level;
			getWindow<GUIStaticImage>("ManorHouse-Currency-Icon")->SetVisible(isShowCurrencyIcon);
			getWindow<GUIStaticImage>("ManorHouse-Currency-Icon")->SetImage(UICommon::getCurrencyIconByType(m_selectHouse->currencyType).c_str());
			m_btnOperation->SetRenderOffset(isShowCurrencyIcon ? Vector2(15, 0) : Vector2(0, 0));
			if (m_selectHouse->unlockLevel > Blockman::Instance()->m_pPlayer->m_manor->getInfo()->level)
			{
				m_btnOperation->SetText(StringUtil::Format(getString(LanguageKey::GUI_MANOR_HOUSE_NOT_CAN_UNLOCK).c_str(), m_selectHouse->unlockLevel).c_str());
				m_btnOperation->SetEnabled(false);
			}
			else
			{
				m_btnOperation->removeAllEvents();
				m_btnOperation->subscribeEvent(EventButtonClick, std::bind(&gui_manorHouse::onClick, this, std::placeholders::_1, m_selectHouse->isBuy ? ViewId::BUILD : ViewId::BUY));
				if (m_selectHouse->isBuy)
				{
					m_buildResidueCoolTime = m_buildResidueCoolTime > 0 ? m_buildResidueCoolTime - timeElapse : 0;
					m_btnOperation->SetEnabled(m_buildResidueCoolTime <= 0 && Blockman::Instance()->m_pPlayer->m_manor->getInfo() ->isMaster);
					GUIString str =  getString(LanguageKey::GUI_MANOR_HOUSE_BTN_BUILD);
					if (m_buildResidueCoolTime > 0)
					{
						str = StringUtil::Format("%s %d", str.c_str(), m_buildResidueCoolTime / 1000 + 1).c_str();
					}
					m_btnOperation->SetText(str.c_str());
				}
				else
				{
					m_btnOperation->SetEnabled(Blockman::Instance()->m_pPlayer->m_manor->getInfo()->isMaster);
					m_btnOperation->SetText(StringUtil::Format(getString(LanguageKey::GUI_MANOR_HOUSE_BTN_BUY).c_str(), m_selectHouse->price).c_str());
				}
			}
		}
	}
}
