#include "gui_mainInventoryPanelControl.h"

#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "game.h"
#include "Util/ClientEvents.h"
#include "Inventory/InventoryPlayer.h"

namespace BLOCKMAN
{

	gui_mainInventoryPanelControl::gui_mainInventoryPanelControl()
		: gui_layout("MainInventoryPanel.json")
	{
	}

	void gui_mainInventoryPanelControl::onLoad()
	{
		m_guiScrollableInventory = dynamic_cast<GuiScrollableInventory*>(m_rootWindow->GetChildByIndex(0));
		m_guiScrollableInventory->subscribeEvent(GuiScrollableInventory::ITEM_STACK_TOUCH_UP.c_str(), SubscriberSlot(&gui_mainInventoryPanelControl::onItemStackTouchUp, this));

	}

	void gui_mainInventoryPanelControl::onUpdate(ui32 nTimeElapse)
	{
		auto inventory = Blockman::Instance()->m_pPlayer->inventory;
		m_guiScrollableInventory->setInventory(inventory->getMainInventory());
		m_guiScrollableInventory->selectItemSlot(m_selectionIndex);
	}

	bool gui_mainInventoryPanelControl::onItemStackTouchUp(const EventArgs & event)
	{
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
		return true;
	}
}
