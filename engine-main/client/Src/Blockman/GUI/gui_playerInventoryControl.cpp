#include "gui_playerInventoryControl.h"

#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "GUI/GuiItemStack.h"
#include "GUI/GuiSlotTable.h"
#include "GUI/gui_mainInventoryPanelControl.h"
#include "GUI/gui_craftingPanelControl.h"
#include "GUI/gui_armorPanelControl.h"

#include "Inventory/InventoryPlayer.h"
#include "Util/LanguageKey.h"
#include <iterator>

namespace BLOCKMAN
{
	const gui_playerInventoryControl::InventoryTabId gui_playerInventoryControl::ALL_INVENTORY_TAB_ID[] = {
		InventoryTabId::ARMOR,
		InventoryTabId::CRAFTING,
		InventoryTabId::MAIN_INVENTORY
	};

	gui_playerInventoryControl::gui_playerInventoryControl()
		: gui_layout("PlayerInventory.json")
	{
	}

	gui_playerInventoryControl::~gui_playerInventoryControl()
	{
		for (auto tabId : ALL_INVENTORY_TAB_ID)
		{
			LordSafeDelete(m_panels[tabId]);
		}
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_playerInventoryControl::onLoad()
	{
		m_hotbar = getWindow<GuiSlotTable>("PlayerInventory-VisibleBar-SlotTable");
		m_hotbar->subscribeEvent("ItemStackTouchUp", SubscriberSlot(&gui_playerInventoryControl::onItemStackTouchUp, this));

		getWindow("PlayerInventory-ToggleInventoryButton")->subscribeEvent(EventWindowTouchUp, SubscriberSlot(&gui_playerInventoryControl::onCloseInventory, this));
		getWindow("PlayerInventory-CloseButton")->subscribeEvent(EventWindowTouchUp,SubscriberSlot(&gui_playerInventoryControl::onCloseInventory, this));
		getWindow<GUICheckBox>("PlayerInventory-Title")->SetText(getString(LanguageKey::WORDART_ROLE));

		m_tabs[InventoryTabId::ARMOR] = getWindow<GUIRadioButton>("PlayerInventory-ArmorTab");
		m_tabs[InventoryTabId::CRAFTING] = getWindow<GUIRadioButton>("PlayerInventory-CraftingTab");
		m_tabs[InventoryTabId::MAIN_INVENTORY] = getWindow<GUIRadioButton>("PlayerInventory-MainInventoryTab");

		m_radioNameMap[InventoryTabId::ARMOR] = getWindow<GUICheckBox>("PlayerInventory-ArmorTab-Name");
		m_radioNameMap[InventoryTabId::CRAFTING] = getWindow<GUICheckBox>("PlayerInventory-CraftingTab-Name");

		m_radioNameMap[InventoryTabId::ARMOR]->SetText(getString(LanguageKey::WORDART_ROLE));
		m_radioNameMap[InventoryTabId::CRAFTING]->SetText(getString(LanguageKey::WORDART_COMPOUND));

		m_panels[InventoryTabId::ARMOR] = m_panels[InventoryTabId::ARMOR] ? m_panels[InventoryTabId::ARMOR] : LordNew gui_armorPanelControl;
		m_panels[InventoryTabId::CRAFTING] = m_panels[InventoryTabId::CRAFTING] ? m_panels[InventoryTabId::CRAFTING] : LordNew gui_craftingPanelControl;
		m_panels[InventoryTabId::MAIN_INVENTORY] = m_panels[InventoryTabId::MAIN_INVENTORY] ? m_panels[InventoryTabId::MAIN_INVENTORY] : LordNew gui_mainInventoryPanelControl;

		auto pInventoryPanel = getWindow("PlayerInventory-InventoryPanel");
		for (auto tabId : ALL_INVENTORY_TAB_ID)
		{
			m_panels[tabId]->attachTo(this, pInventoryPanel);
			m_tabs[tabId]->subscribeEvent(EventRadioStateChanged, std::bind(&gui_playerInventoryControl::onRadioStateChanged, this, std::placeholders::_1, tabId));
			m_tabs[tabId]->subscribeEvent(EventWindowTouchUp, std::bind(&gui_playerInventoryControl::onClick, this, std::placeholders::_1));
		}

		m_subscriptionGuard.add(Blockman::Instance()->m_pPlayer->inventory->onInventoryContentSync([this] {
			m_isInventoryChanged = true;
		}));
	}

	void gui_playerInventoryControl::onShow()
	{
		if (m_isInCraftingTable)
		{
			m_selectedTab = InventoryTabId::CRAFTING;
		}
		else
		{
			m_selectedTab = InventoryTabId::ARMOR;
			dynamic_cast<gui_armorPanelControl*>(m_panels[InventoryTabId::ARMOR])->onShow();
		}
	}

	void gui_playerInventoryControl::onHide()
	{
		m_isInCraftingTable = false;
	}

	void gui_playerInventoryControl::onUpdate(ui32 nTimeElapse)
	{
		if (!isShown()) return;

		auto inventory = Blockman::Instance()->m_pPlayer->inventory;
		for (int i = 0; i < 9; ++i)
		{
			m_hotbar->setItemStack(0, i, inventory->hotbar[i]);
		}
		m_hotbar->selectItemSlot(inventory->currentItemIndex);

		if (getParent()->isPlayerInventoryShown() && m_selectedTab == CRAFTING && m_isInventoryChanged) {
			Blockman::Instance()->m_pPlayer->inventory->notifyUpdateToProxy();
			m_isInventoryChanged = false;
		}

		showTab(m_selectedTab);
	}

	bool gui_playerInventoryControl::onCloseInventory(const EventArgs& events)
	{
		getParent()->showMainControl();
		(dynamic_cast<gui_mainInventoryPanelControl*>(m_panels[InventoryTabId::MAIN_INVENTORY]))->resetSelection();
		//playSonud("random.click");
		playSoundByType(ST_Click);
		return true;
	}

	bool gui_playerInventoryControl::onClick(const EventArgs & events)
	{
		//playSonud("random.click");
		playSoundByType(ST_Click);
		return true;
	}

	bool gui_playerInventoryControl::onRadioStateChanged(const EventArgs & events, InventoryTabId tabId)
	{
		auto wEventArgs = dynamic_cast<const WindowEventArgs&>(events);
		GUIRadioButton* rButton = dynamic_cast<GUIRadioButton*>(wEventArgs.window);
		if (rButton && rButton->IsSelected()) {
			m_selectedTab = tabId;
			if (m_selectedTab == CRAFTING) {
				Blockman::Instance()->m_pPlayer->inventory->notifyUpdateToProxy();
			}
		}
		return true;
	}
	
	void gui_playerInventoryControl::setInCraftingTable(bool isInCraftingTable)
	{
		m_isInCraftingTable = isInCraftingTable;
		if (m_isInCraftingTable) {
			Blockman::Instance()->m_pPlayer->inventory->notifyUpdateToProxy();
		}
	}


	void gui_playerInventoryControl::showTab(InventoryTabId tab)
	{
		for (auto tabId : ALL_INVENTORY_TAB_ID)
		{
			if (m_panels[tabId]->isShown() && tabId != tab)
			{
				m_panels[tabId]->hide();
			}

			if (m_radioNameMap[tabId])
			{
				m_radioNameMap[tabId]->SetChecked(tab == tabId);
			}
		}
		m_panels[tab]->show();
		m_tabs[tab]->SetSelected(true);
		if (tab == InventoryTabId::CRAFTING){
			getWindow<GUICheckBox>("PlayerInventory-Title")->SetText(getString(LanguageKey::WORDART_COMPOUND));
		}
		
		if (tab == InventoryTabId::ARMOR) {
			getWindow<GUICheckBox>("PlayerInventory-Title")->SetText(getString(LanguageKey::WORDART_ROLE));
		}

		getWindow<GUICheckBox>("PlayerInventory-Title")->SetChecked(m_tabs[ARMOR]->IsSelected());
	}

	bool gui_playerInventoryControl::onItemStackTouchUp(const EventArgs & events)
	{
		auto touchEvent = dynamic_cast<const GuiSlotTable::ItemStackTouchEventArgs&>(events);
		LordAssert(touchEvent.row == 0);
		Blockman::Instance()->m_pPlayer->inventory->currentItemIndex = touchEvent.column;
		ItemStackPtr touchStack = Blockman::Instance()->m_pPlayer->inventory->getCurrentItem();
		if (touchStack && touchStack->stackSize > 0)
		{
			ShowItemNameEvent::emit(touchStack);
		}
		return false;
	}
}