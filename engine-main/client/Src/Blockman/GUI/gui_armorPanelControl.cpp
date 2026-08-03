#include "gui_armorPanelControl.h"

#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "game.h"
#include "Util/ClientEvents.h"
#include "Item/Items.h"
#include "Inventory/InventoryPlayer.h"
#include "GUI/GuiActorWindow.h"
#include "EntityRenders/EntityRenderManager.h"
#include "Actor/ActorManager.h"
#include "UI/GUIStaticImage.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"
#include "UI/GUIImagesetManager.h"

namespace BLOCKMAN
{

	const gui_armorPanelControl::ViewId  gui_armorPanelControl::ALL_VIEW_ID[] = {
		ViewId::ALL,
		ViewId::ARMOR,
		ViewId::MATERIALS
	};

	gui_armorPanelControl::gui_armorPanelControl()
		: gui_layout("ArmorPanel.json")
	{
	}

	void gui_armorPanelControl::onLoad()
	{
		m_guiScrollableInventory = getWindow<GuiScrollableInventory>("ArmorPanel-Inventory");
		m_equipedArmorTable = getWindow<GuiSlotTable>("ArmorPanel-EquipedArmors");
		m_guiScrollableInventory->subscribeEvent("ItemStackTouchUp", SubscriberSlot(&gui_armorPanelControl::equipArmor, this));
		m_guiScrollableInventory->subscribeEvent("ItemStackTouchUp", SubscriberSlot(&gui_armorPanelControl::onItemStackTouchUp, this));
		m_equipedArmorTable->subscribeEvent("ItemStackTouchUp", SubscriberSlot(&gui_armorPanelControl::unequipArmor, this));
		m_actorWindow = getWindow<GuiActorWindow>("ArmorPanel-Actor");
		m_actorTitleName = getWindow<GUIStaticText>("ArmorPanel-Player-Model-Title-Name");
		m_actorInventoryName = getWindow<GUIStaticText>("ArmorPanel-Inventory-Title-Name");
		m_radioMap[ViewId::ALL] = getWindow<GUIRadioButton>("ArmorPanel-Inventory-Radio-All");
		m_radioMap[ViewId::ARMOR] = getWindow<GUIRadioButton>("ArmorPanel-Inventory-Radio-Armor");
		m_radioMap[ViewId::MATERIALS] = getWindow<GUIRadioButton>("ArmorPanel-Inventory-Radio-Materials");

		m_radioMap[ViewId::ALL]->SetText(LanguageManager::Instance()->getString(LanguageKey::INVENTORY_PANEL_ALL).c_str());
		m_radioMap[ViewId::ARMOR]->SetText(LanguageManager::Instance()->getString(LanguageKey::INVENTORY_PANEL_EQUIPMENT).c_str());
		m_radioMap[ViewId::MATERIALS]->SetText(LanguageManager::Instance()->getString(LanguageKey::INVENTORY_PANEL_MATERIALS).c_str());
		
		for (ViewId id :ALL_VIEW_ID)
		{
			m_radioMap[id]->subscribeEvent(EventRadioStateChanged, std::bind(&gui_armorPanelControl::onRadioChange, this, std::placeholders::_1, id));
		}
		m_radioMap[ViewId::ALL]->SetSelected(true);
		onShow();
	}

	void gui_armorPanelControl::onShow()
	{
		if (GameClient::CGame::Instance()->isEnterBack())
		{
			return;
		}
		String defaultIdle = "idle";
		if (Blockman::Instance()->m_pPlayer && Blockman::Instance()->m_pPlayer->m_defaultIdle > 0)
		{
			defaultIdle = "idle_" + StringUtil::Format("%d", Blockman::Instance()->m_pPlayer->m_defaultIdle);
		}
		if (Blockman::Instance()->m_pPlayer->m_sex == 2) {
			m_actorWindow->SetActor("girl.actor", defaultIdle.c_str());
		}
		else {
			m_actorWindow->SetActor("boy.actor", defaultIdle.c_str());
		}
		m_actorWindow->reflreshArmor(Blockman::Instance()->m_pPlayer);
		// m_actorWindow->GetActor()->SetSkyBlockColor(Color(0.6f, 0.3f, 0.5f, 1.0f));
		m_actorWindow->GetActor()->SetCustomColor(Blockman::Instance()->m_pPlayer->m_skinColor);
		if (m_actorInventoryName != nullptr) {
			m_actorInventoryName->SetText(getString(LanguageKey::WORDART_BAG));
		}
		// m_actorWindow->SetRotateY(180);
	}

	void gui_armorPanelControl::onUpdate(ui32)
	{
		auto inventory = Blockman::Instance()->m_pPlayer->inventory;
		m_actorTitleName->SetText(Blockman::Instance()->m_pPlayer->getEntityName().c_str());
		if (inventory == nullptr)
			return;
		auto armorInventory = inventory->getArmorInventory();
		//m_guiScrollableInventory->setInventory(inventory->getArmorFilteredInventory());
		for (int i = 0; i < armorInventory->getSizeInventory(); ++i)
		{
			m_equipedArmorTable->setItemStackArmorType(i >> 1, i % 2, ArmorType(i));
			m_equipedArmorTable->setItemStack(i >> 1, i % 2, armorInventory->getStackInSlot(i));
		}
		updateData();
	}

	bool gui_armorPanelControl::equipArmor(const EventArgs & event)
	{

		if (m_showViewId != ViewId::ARMOR)
		{
			return false;
		}

		auto touchEvent = dynamic_cast<const GuiScrollableInventory::ItemStackTouchEventArgs&>(event);
		auto inventory = Blockman::Instance()->m_pPlayer->inventory;
		auto itemStack = inventory->getArmorFilteredInventory()->getStackInSlot(touchEvent.index);
		auto index = inventory->findItemStack(itemStack);
		inventory->equipArmor(itemStack);
		GameClient::CGame::Instance()->getNetwork()->getSender()->sendEquipArmor(index);
		Blockman::Instance()->m_pPlayer->m_outLooksChanged = true;
		m_actorWindow->reflreshArmor(Blockman::Instance()->m_pPlayer);
		return true;
	}

	bool gui_armorPanelControl::unequipArmor(const EventArgs & event)
	{
		auto touchEvent = dynamic_cast<const GuiSlotTable::ItemStackTouchEventArgs&>(event);
		auto inventory = Blockman::Instance()->m_pPlayer->inventory;
		auto itemStack = inventory->getArmorInventory()->getStackInSlot(touchEvent.row * 2 + touchEvent.column);
		auto index = inventory->findItemStack(itemStack);
		if (inventory->unequipArmor(itemStack))
		{
			GameClient::CGame::Instance()->getNetwork()->getSender()->sendUnequipArmor(index);
		}
		Blockman::Instance()->m_pPlayer->m_outLooksChanged = true;
		m_actorWindow->reflreshArmor(Blockman::Instance()->m_pPlayer);
		return true;
	}

	bool gui_armorPanelControl::onItemStackTouchUp(const EventArgs & event)
	{
		if (m_showViewId != ViewId::ALL)
		{
			return false;
		}
		auto inventory = Blockman::Instance()->m_pPlayer->inventory;
		ItemStackPtr currentItem = inventory->getCurrentItem();
		auto touchIndex = dynamic_cast<const GuiScrollableInventory::ItemStackTouchEventArgs&>(event).index;
		if (m_selectionIndex == touchIndex)
		{
			m_selectionIndex = -1;
		}
		else if (m_selectionIndex >= 0)
		{
			GameClient::CGame::Instance()->getNetwork()->getSender()->sendSwapItemPosition(m_selectionIndex, touchIndex);
			m_guiScrollableInventory->getInventory()->swapStacksInSlots(m_selectionIndex, touchIndex);
			m_selectionIndex = -1;
		}
		else if (m_guiScrollableInventory->getItemStack(touchIndex) == nullptr)
		{
			inventory->removeCurrentItemFromHotbar();
		}
		else if (m_guiScrollableInventory->getItemStack(touchIndex) == currentItem)
		{
			m_selectionIndex = touchIndex;
		}
		else
		{
			inventory->putItemToHotbar(touchIndex);
		}

		ItemStackPtr touchStack = inventory->getMainInventory()->getStackInSlot(touchIndex);
		if (touchStack && touchStack->stackSize > 0)
		{
			ShowItemNameEvent::emit(touchStack);
		}
		m_guiScrollableInventory->selectItemSlot(m_selectionIndex);
		return true;
	}

	bool gui_armorPanelControl::onRadioChange(const EventArgs & events , const ViewId viewId)
	{
		LordLogInfo("gui_armorPanelControl::onRadioChange");
		if (m_radioMap[viewId]->IsSelected()) {
			m_showViewId = viewId;
		}
		return true;
	}

	void gui_armorPanelControl::updateData()
	{
		auto inventory = Blockman::Instance()->m_pPlayer->inventory;
		if (inventory == nullptr)
			return;
		switch (m_showViewId)
		{
		case ViewId::ALL:
			m_guiScrollableInventory->setInventory(inventory->getMainInventory());
			break;
		case ViewId::ARMOR:
			m_guiScrollableInventory->setInventory(inventory->getArmorFilteredInventory());
			break;
		case ViewId::MATERIALS:
			break;
		}
	}
}
