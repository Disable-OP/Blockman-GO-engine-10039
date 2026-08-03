#include "GuiScrollableInventory.h"
#include "Object/Root.h"
#include "UI/GUIWindowManager.h"
#include "GUI/GuiDef.h"
#include "Util/StringUtil.h"

namespace BLOCKMAN
{

	String GuiScrollableInventory::ITEM_STACK_TOUCH_DOWN = "ItemStackTouchDown";
	String GuiScrollableInventory::ITEM_STACK_TOUCH_UP = "ItemStackTouchUp";
	String GuiScrollableInventory::ITEM_STACK_LONG_TOUCH_START = "ItemStackLongTouchStart";
	String GuiScrollableInventory::ITEM_STACK_LONG_TOUCH_END = "ItemStackLongTouchEnd";

	GuiScrollableInventory::GuiScrollableInventory(GUIWindowType nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_gridView = dynamic_cast<GUIGridView*>(GUIWindowManager::Instance()->CreateGUIWindow(GWT_GRID_VIEW, "GridView"));
		renameComponents(m_gridView);
		m_gridView->SetProperty(GWP_GRID_VIEW_ITEM_ALIGNMENT, "Centre");
		m_gridView->SetArea({0, 0 }, { 0, 0 }, { 1.0, 0 }, { 1.0, 0 });
		AddWindowToDrawList(*m_gridView);
		subscribeToBubbleUpMouseEvents();
	}

	bool GuiScrollableInventory::onItemStackTouchDown(const EventArgs & events)
	{
		auto windowEventArgs = dynamic_cast<const WindowEventArgs&>(events);
		auto pWindow = windowEventArgs.window;
		auto index = pWindow->GetUserData();
		auto newEvent = ItemStackTouchEventArgs(index);
		fireEvent(ITEM_STACK_TOUCH_DOWN.c_str(), newEvent);
		return true;
	}

	bool GuiScrollableInventory::onItemStackTouchUp(const EventArgs & events)
	{
		auto windowEventArgs = dynamic_cast<const WindowEventArgs&>(events);
		auto pWindow = windowEventArgs.window;
		auto index = pWindow->GetUserData();
		auto newEvent = ItemStackTouchEventArgs(index);
		fireEvent(ITEM_STACK_TOUCH_UP.c_str(), newEvent);
		return true;
	}

	bool GuiScrollableInventory::onItemStackLongTouchStart(const EventArgs & events)
	{
		auto windowEventArgs = dynamic_cast<const WindowEventArgs&>(events);
		auto pWindow = windowEventArgs.window;
		auto index = pWindow->GetUserData();
		auto newEvent = ItemStackTouchEventArgs(index);
		fireEvent(ITEM_STACK_LONG_TOUCH_START.c_str(), newEvent);
		return false;
	}

	bool GuiScrollableInventory::onItemStackLongTouchEnd(const EventArgs & events)
	{
		auto windowEventArgs = dynamic_cast<const WindowEventArgs&>(events);
		auto pWindow = windowEventArgs.window;
		auto index = pWindow->GetUserData();
		auto newEvent = ItemStackTouchEventArgs(index);
		fireEvent(ITEM_STACK_LONG_TOUCH_END.c_str(), newEvent);
		return true;
	}

	bool GuiScrollableInventory::onItemSlotTouchDown(const EventArgs & events)
	{
		if (m_gridView)
		{
			auto mouseEventArgs = dynamic_cast<const MouseEventArgs&>(events);
			m_gridView->TouchDown(mouseEventArgs.position);
		}
		return true;
	}

	bool GuiScrollableInventory::onItemSlotTouchUp(const EventArgs & events)
	{
		if (m_gridView)
		{
			auto mouseEventArgs = dynamic_cast<const MouseEventArgs&>(events);
			m_gridView->TouchUp(mouseEventArgs.position);
		}
		return true;
	}

	bool GuiScrollableInventory::onItemSlotTouchMove(const EventArgs & events)
	{
		if (m_gridView)
		{
			auto mouseEventArgs = dynamic_cast<const MouseEventArgs&>(events);
			m_gridView->TouchMove(mouseEventArgs.position);
		}
		return true;
	}

	bool GuiScrollableInventory::onItemSlotMotionRelease(const EventArgs & events)
	{
		if (m_gridView)
		{
			m_gridView->MotionRelease();
		}
		return true;
	}

	void GuiScrollableInventory::updateGridView()
	{
		m_columnCount = m_isAutoColumnCount ? (int) (GetPixelSize().x / m_slotSize.x ): m_columnCount;
		m_gridView->InitConfig(0.0f, 0.0f, m_columnCount);
		for (int i = 0; i < int(m_gridView->GetItemCount()); ++i)
		{
			auto pGuiItemSlot = dynamic_cast<GuiItemSlot*>(m_gridView->GetItem(i));
			pGuiItemSlot->SetWidth({ 0, m_slotSize.x });
			pGuiItemSlot->SetHeight({ 0, m_slotSize.y });
		}
	}

	void GuiScrollableInventory::resize()
	{
		m_gridView->RemoveAllItems();
		for (int i = 0; i < m_inventory->getSizeInventory(); ++i)
		{
			GuiItemSlot* guiItemSlot = dynamic_cast<GuiItemSlot*>(GUIWindowManager::Instance()->CreateGUIWindow(GWT_ITEM_SLOT, m_strName + "-ItemSlot" +  StringUtil::ToString(i).c_str()));
			guiItemSlot->setStyle(GuiItemSlot::Style::METAL);
			auto pItemStack = m_inventory->getStackInSlot(i);
			guiItemSlot->setItemStack(pItemStack);
			guiItemSlot->SetUserData(i);
			guiItemSlot->subscribeEvent(ITEM_STACK_TOUCH_DOWN.c_str(), SubscriberSlot(&GuiScrollableInventory::onItemStackTouchDown, this));
			guiItemSlot->subscribeEvent(ITEM_STACK_TOUCH_UP.c_str(), SubscriberSlot(&GuiScrollableInventory::onItemStackTouchUp, this));
			guiItemSlot->subscribeEvent(ITEM_STACK_LONG_TOUCH_START.c_str(), SubscriberSlot(&GuiScrollableInventory::onItemStackLongTouchStart, this));
			guiItemSlot->subscribeEvent(ITEM_STACK_LONG_TOUCH_END.c_str(), SubscriberSlot(&GuiScrollableInventory::onItemStackLongTouchEnd, this));
			guiItemSlot->subscribeEvent(EventWindowTouchDown, SubscriberSlot(&GuiScrollableInventory::onItemSlotTouchDown, this));
			guiItemSlot->subscribeEvent(EventWindowTouchUp, SubscriberSlot(&GuiScrollableInventory::onItemSlotTouchUp, this));
			guiItemSlot->subscribeEvent(EventWindowTouchMove, SubscriberSlot(&GuiScrollableInventory::onItemSlotTouchMove, this));
			guiItemSlot->subscribeEvent(EventMotionRelease, SubscriberSlot(&GuiScrollableInventory::onItemSlotMotionRelease, this));
			guiItemSlot->SetSize({ { 0, m_slotSize.x },{ 0, m_slotSize.y } });
			guiItemSlot->setItemStackBackgroundColor(m_itemStackBackgroundColor);
			guiItemSlot->setItemStackNumberVisibility(m_itemStackNumberVisibility);
			m_gridView->AddItem(guiItemSlot);
		}
	}
	
	vector<GUIWindow*>::type GuiScrollableInventory::getPrivateChildren()
	{
		return { m_gridView };
	}

	void GuiScrollableInventory::updateSelectionFrame()
	{
		if (m_selectionIndex == m_oldSelectionIndex)
		{
			return;
		}
		if (m_oldSelectionIndex >= 0)
		{
			dynamic_cast<GuiItemSlot*>(m_gridView->GetItem(m_oldSelectionIndex))->setSelected(false);
		}
		if (m_selectionIndex >= 0)
		{
			auto pSelectedSlot = dynamic_cast<GuiItemSlot*>(m_gridView->GetItem(m_selectionIndex));
			pSelectedSlot->setSelected(true);
			pSelectedSlot->ShowOnTop();
		}
		m_oldSelectionIndex = m_selectionIndex;
	}

	bool GuiScrollableInventory::SetProperty(const GUIString& strName, const GUIString& strValue)
	{
		if (BaseClass::SetProperty(strName, strValue))
		{
			return true;
		}

		if (strName == GWP_TABLE_COLUMN_COUNT)
		{
			this->m_columnCount = StringUtil::ParseInt(strValue.c_str());
			updateGridView();
			return true;
		}
		else if (strName == GWP_SLOT_WIDTH)
		{
			this->m_slotSize.x = StringUtil::ParseReal(strValue.c_str());
			updateGridView();
			return true;
		}
		else if (strName == GWP_SLOT_HEIGHT)
		{
			this->m_slotSize.y = StringUtil::ParseReal(strValue.c_str());
			updateGridView();
			return true;
		}
		else if (strName == GWP_ITEM_STACK_BACKGROUND_COLOR)
		{
			this->m_itemStackBackgroundColor = StringUtil::ParseColor(strValue.c_str());
			return true;
		}
		else if (strName == GWP_ITEM_STACK_NUMBER_VISIBILITY)
		{
			this->m_itemStackNumberVisibility = static_cast<GuiItemStack::NumberVisibility>(StringUtil::ParseInt(strValue.c_str()));
			return true;
		}
		else if (strName == GWP_TABLE_AUTO_COLUMN_COUNT)
		{
			this->m_isAutoColumnCount = StringUtil::ParseBool(strValue.c_str());
			return true;
		}

		return false;
	}

	bool GuiScrollableInventory::GetProperty(const GUIString& strName, GUIString& strValue)
	{
		if (BaseClass::GetProperty(strName, strValue))
		{
			return true;
		}
		
		if (strName == GWP_TABLE_COLUMN_COUNT)
		{
			strValue = StringUtil::ToString(this->m_columnCount).c_str();
			return true;
		}
		else if (strName == GWP_SLOT_WIDTH)
		{
			strValue = StringUtil::ToString(this->m_slotSize.x).c_str();
			return true;
		}
		else if (strName == GWP_SLOT_HEIGHT)
		{
			strValue = StringUtil::ToString(this->m_slotSize.y).c_str();
			return true;
		}
		else if (strName == GWP_ITEM_STACK_BACKGROUND_COLOR)
		{
			strValue = StringUtil::ToString(this->m_itemStackBackgroundColor).c_str();
			return true;
		}
		else if (strName == GWP_ITEM_STACK_NUMBER_VISIBILITY)
		{
			strValue = StringUtil::ToString(static_cast<int>(m_itemStackNumberVisibility)).c_str();
			return true;
		}
		else if (strName == GWP_TABLE_AUTO_COLUMN_COUNT)
		{
			strValue = StringUtil::ToString(m_isAutoColumnCount).c_str();
			return true;
		}

		return false;
	}

	void GuiScrollableInventory::GetPropertyList(GUIPropertyList& properlist)
	{
		BaseClass::GetPropertyList(properlist);
		properlist.push_back(GWP_TABLE_COLUMN_COUNT);
		properlist.push_back(GWP_SLOT_WIDTH);
		properlist.push_back(GWP_SLOT_HEIGHT);
		properlist.push_back(GWP_ITEM_STACK_BACKGROUND_COLOR);
		properlist.push_back(GWP_ITEM_STACK_NUMBER_VISIBILITY);
		properlist.push_back(GWP_TABLE_AUTO_COLUMN_COUNT);
	}

	bool GuiScrollableInventory::GetPropertyType(const GUIString& strName, GUIPropertyType& type)
	{
		if (BaseClass::GetPropertyType(strName, type))
		{
			return true;
		}

		if (strName == GWP_TABLE_COLUMN_COUNT)
		{
			type = GPT_INT;
			return true;
		}
		else if (strName == GWP_SLOT_WIDTH)
		{
			type = GPT_FLOAT;
			return true;
		}
		else if (strName == GWP_SLOT_HEIGHT)
		{
			type = GPT_FLOAT;
			return true;
		}
		else if (strName == GWP_ITEM_STACK_BACKGROUND_COLOR)
		{
			type = GPT_COLOR;
			return true;
		}
		else if (strName == GWP_ITEM_STACK_NUMBER_VISIBILITY)
		{
			type = GPT_INT;
			return true;
		}
		else if (strName == GWP_TABLE_AUTO_COLUMN_COUNT)
		{
			type = GPT_BOOL;
			return true;
		}

		return false;
	}

	void GuiScrollableInventory::UpdateSelf(ui32 nTimeElapse)
	{
		if (m_isAutoColumnCount)
		{
			updateGridView();
		}
		m_gridView->SetArea(getAbsolutePositionFromRoot(), { { 0, GetPixelSize().x },{ 0, GetPixelSize().y } });
		if (!m_inventory)
		{
			return;
		}
		if (m_gridView->GetItemCount() != m_inventory->getSizeInventory())
		{
			resize();
		}
		for (int i = 0; i < m_inventory->getSizeInventory(); ++i)
		{
			auto guiItemSlot = dynamic_cast<GuiItemSlot*>(m_gridView->GetItem(i));
			guiItemSlot->setItemStack(m_inventory->getStackInSlot(i));
		}
		updateSelectionFrame();
		BaseClass::UpdateSelf(nTimeElapse);
	}

	void GuiScrollableInventory::Destroy()
	{
		if (m_gridView)
		{
			RemoveWindowFromDrawList(*m_gridView);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_gridView);
			m_gridView = nullptr;
		}
	}
}