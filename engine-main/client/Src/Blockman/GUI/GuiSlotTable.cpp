#include "GuiSlotTable.h"
#include "UI/GUIWindowManager.h"
#include "GUI/GuiDef.h"
#include "Util/StringUtil.h"
#include "UI/GUIMutualNonoverlappingGroup.h"

namespace BLOCKMAN
{
	vector<GUIWindow*>::type GuiSlotTable::getPrivateChildren()
	{
		return { m_container };
		//return vector<GUIWindow*>::type(m_guiItemSlots.begin(), m_guiItemSlots.end());
	}

	GuiSlotTable::GuiSlotTable(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_container = dynamic_cast<GUIMutualNonoverlappingGroup*>(GUIWindowManager::Instance()->CreateGUIWindow(GWT_MUTUAL_NONOVERLAPPING_GROUP, "Container"));
		renameComponents(m_container);
		AddWindowToDrawList(*m_container);
		subscribeToBubbleUpMouseEvents();
	}

	void GuiSlotTable::resizeSlotTable()
	{
		if (m_tableSize == m_oldTableSize)
		{
			return;
		}
		m_oldTableSize = m_tableSize;
		clear();
		size_t slotNum = m_tableSize.column * m_tableSize.row;
		m_guiItemSlots.reserve(slotNum);
		for (size_t i = 0; i < slotNum; ++i)
		{
			auto newGuiItemSlot = dynamic_cast<GuiItemSlot*>(GUIWindowManager::Instance()->CreateGUIWindow(GWT_ITEM_SLOT, GUIString("ItemSlot") + StringUtil::ToString(i).c_str()));
			renameComponents(newGuiItemSlot);
			newGuiItemSlot->setStyle(m_slotStyle);
			newGuiItemSlot->SetUserData(i);
			m_guiItemSlots.push_back(newGuiItemSlot);
			newGuiItemSlot->subscribeEvent("ItemStackTouchUp", SubscriberSlot(&GuiSlotTable::onItemStackTouchUp, this));
			newGuiItemSlot->subscribeEvent("ItemStackLongTouchStart", SubscriberSlot(&GuiSlotTable::onItemStackLongTouchStart, this));
			newGuiItemSlot->subscribeEvent("ItemStackLongTouchEnd", SubscriberSlot(&GuiSlotTable::onItemStackLongTouchEnd, this));
			m_container->AddChildWindow(newGuiItemSlot);
		}
	}

	void GuiSlotTable::layoutGuiItemSlots()
	{
		double horizontalInterval = m_horizontalInterval;
		if (0.0f < m_horizontalInterval && m_horizontalInterval <= 1.0f)
		{
			horizontalInterval = GetPixelSize().x * m_horizontalInterval;
		}
		double verticalInterval = m_verticalInterval;
		if (0.0f < m_verticalInterval && m_verticalInterval <= 1.0f)
		{
			verticalInterval = GetPixelSize().y * m_verticalInterval;
		}

		const double horizontalPadding = (GetPixelSize().x - (m_tableSize.column * m_slotSize.x + (m_tableSize.column -1) * horizontalInterval)) / 2;
		const double verticalPadding = (GetPixelSize().y - (m_tableSize.row * m_slotSize.y + (m_tableSize.row -1) * verticalInterval)) / 2;
		for (size_t i = 0; i < m_tableSize.row; ++i)
		{
			for (size_t j = 0; j < m_tableSize.column; ++j)
			{
				const double offsetX = j * (m_slotSize.x + horizontalInterval) + horizontalPadding;
				const double offsetY = i * (m_slotSize.y + verticalInterval) + verticalPadding;
				auto position = getAbsolutePositionFromRoot();
				position += { {0.0f, static_cast<float>(offsetX)}, {0.0f, static_cast<float>(offsetY)} };
				GuiItemSlot* pGuiItemSlot = m_guiItemSlots.at(childIndex(i, j));
				pGuiItemSlot->SetArea(position, { {0.0f, m_slotSize.x}, {0.0f, m_slotSize.y} });
			}
		}
	}

	bool GuiSlotTable::onItemStackTouchUp(const EventArgs & events)
	{
		auto windowEventArgs = dynamic_cast<const WindowEventArgs&>(events);
		auto pWindow = windowEventArgs.window;
		auto index = pWindow->GetUserData();
		ItemStackTouchEventArgs newEvent = {index / m_tableSize.column, index % m_tableSize.column };
		fireEvent("ItemStackTouchUp", newEvent);
		return true;
	}

	bool GuiSlotTable::onItemStackLongTouchStart(const EventArgs & events)
	{
		auto windowEventArgs = dynamic_cast<const WindowEventArgs&>(events);
		auto pWindow = windowEventArgs.window;
		auto index = pWindow->GetUserData();
		ItemStackTouchEventArgs newEvent = { index / m_tableSize.column, index % m_tableSize.column };
		fireEvent("ItemStackLongTouchStart", newEvent);
		return true;
	}

	bool GuiSlotTable::onItemStackLongTouchEnd(const EventArgs & events)
	{
		auto windowEventArgs = dynamic_cast<const WindowEventArgs&>(events);
		auto pWindow = windowEventArgs.window;
		auto index = pWindow->GetUserData();
		ItemStackTouchEventArgs newEvent = { index / m_tableSize.column, index % m_tableSize.column };
		fireEvent("ItemStackLongTouchEnd", newEvent);
		return true;
	}

	void GuiSlotTable::updateSelectionFrame()
	{
		if (m_selectionIndex == m_oldSelectionIndex)
		{
			return;
		}
		if (m_oldSelectionIndex >= 0)
		{
			m_guiItemSlots.at(m_oldSelectionIndex)->setSelected(false);
		}
		if (m_selectionIndex >= 0)
		{
			auto pSelectedSlot = m_guiItemSlots.at(m_selectionIndex);
			pSelectedSlot->setSelected(true);
			// Put it to the top of the drawlist
			m_container->RemoveWindowFromDrawList(*pSelectedSlot);
			m_container->AddWindowToDrawList(*pSelectedSlot);
		}
		m_oldSelectionIndex = m_selectionIndex;
	}

	void GuiSlotTable::updateItemStackSettings()
	{
		for (auto itemSlot : m_guiItemSlots)
		{
			itemSlot->setItemStackBackgroundColor(m_itemStackBackgroundColor);
			itemSlot->setItemStackNumberVisibility(m_itemStackNumberVisibility);
		}
	}

	void GuiSlotTable::clear()
	{
		m_container->CleanupChildren();
		m_guiItemSlots.clear();
		m_selectionIndex = m_oldSelectionIndex = -1;
	}

	void GuiSlotTable::setDropItemProgress(int index, float progress)
	{
		if (index >=0 &&  m_container && m_container->GetChildCount() > (size_t) index)
		{
			GuiItemSlot* slot = dynamic_cast<GuiItemSlot*>(m_container->GetChildByIndex(index));
			if (slot)
			{
				slot->setDropItemProgress(progress);
			}
		}
	}

	void GuiSlotTable::UpdateSelf(ui32 nTimeElapse)
	{
		resizeSlotTable();
		layoutGuiItemSlots();
		updateSelectionFrame();
		updateItemStackSettings();
		BaseClass::UpdateSelf(nTimeElapse);
	}

	bool GuiSlotTable::SetProperty(const GUIString& strName, const GUIString& strValue)
	{
		if (BaseClass::SetProperty(strName, strValue))
		{
			return true;
		}

		if (strName == GWP_TABLE_ROW_COUNT)
		{
			this->m_tableSize.row = StringUtil::ParseInt(strValue.c_str());
			resizeSlotTable();
			return true;
		}
		else if (strName == GWP_TABLE_COLUMN_COUNT)
		{
			this->m_tableSize.column = StringUtil::ParseInt(strValue.c_str());
			resizeSlotTable();
			return true;
		}
		else if (strName == GWP_TABLE_HORIZONTAL_INTERVAL)
		{
			this->m_horizontalInterval = StringUtil::ParseReal(strValue.c_str());
			return true;
		}
		else if (strName == GWP_TABLE_VERTICAL_INTERVAL)
		{
			this->m_verticalInterval = StringUtil::ParseReal(strValue.c_str());
			return true;
		}
		else if (strName == GWP_SLOT_WIDTH)
		{
			this->m_slotSize.x = StringUtil::ParseReal(strValue.c_str());
			return true;
		}
		else if (strName == GWP_SLOT_HEIGHT)
		{
			this->m_slotSize.y = StringUtil::ParseReal(strValue.c_str());
			return true;
		}
		else if (strName == GWP_SLOT_STYLE)
		{
			this->m_slotStyle = static_cast<GuiItemSlot::Style>(StringUtil::ParseInt(strValue.c_str()));
			for (GuiItemSlot* pGuiItemSlot : m_guiItemSlots)
			{
				pGuiItemSlot->setStyle(m_slotStyle);
			}
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

		return false;
	}

	bool GuiSlotTable::GetProperty(const GUIString& strName, GUIString& strValue)
	{
		if (BaseClass::GetProperty(strName, strValue))
		{
			return true;
		}

		if (strName == GWP_TABLE_ROW_COUNT)
		{
			strValue = StringUtil::ToString(this->m_tableSize.row).c_str();
			return true;
		}
		else if (strName == GWP_TABLE_COLUMN_COUNT)
		{
			strValue = StringUtil::ToString(this->m_tableSize.column).c_str();
			return true;
		}
		else if (strName == GWP_TABLE_HORIZONTAL_INTERVAL)
		{
			strValue = StringUtil::ToString(this->m_horizontalInterval).c_str();
			return true;
		}
		else if (strName == GWP_TABLE_VERTICAL_INTERVAL)
		{
			strValue = StringUtil::ToString(this->m_verticalInterval).c_str();
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
		else if (strName == GWP_SLOT_STYLE)
		{
			strValue = StringUtil::ToString(static_cast<int>(m_slotStyle)).c_str();
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
		return false;
	}

	void GuiSlotTable::GetPropertyList(GUIPropertyList& properlist)
	{
		BaseClass::GetPropertyList(properlist);
		properlist.push_back(GWP_TABLE_ROW_COUNT);
		properlist.push_back(GWP_TABLE_COLUMN_COUNT);
		properlist.push_back(GWP_TABLE_HORIZONTAL_INTERVAL);
		properlist.push_back(GWP_TABLE_VERTICAL_INTERVAL);
		properlist.push_back(GWP_SLOT_WIDTH);
		properlist.push_back(GWP_SLOT_HEIGHT);
		properlist.push_back(GWP_SLOT_STYLE);
		properlist.push_back(GWP_ITEM_STACK_BACKGROUND_COLOR);
		properlist.push_back(GWP_ITEM_STACK_NUMBER_VISIBILITY);

	}

	bool GuiSlotTable::GetPropertyType(const GUIString& strName, GUIPropertyType& type)
	{
		if (BaseClass::GetPropertyType(strName, type))
		{
			return true;
		}

		if (strName == GWP_TABLE_ROW_COUNT)
		{
			type = GPT_INT;
			return true;
		}
		else if (strName == GWP_TABLE_COLUMN_COUNT)
		{
			type = GPT_INT;
			return true;
		}
		else if (strName == GWP_TABLE_HORIZONTAL_INTERVAL)
		{
			type = GPT_FLOAT;
			return true;
		}
		else if (strName == GWP_TABLE_VERTICAL_INTERVAL)
		{
			type = GPT_FLOAT;
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
		else if (strName == GWP_SLOT_STYLE)
		{
			type = GPT_INT;
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
		return false;
	}

	void GuiSlotTable::Destroy()
	{
		m_guiItemSlots.clear();
		RemoveWindowFromDrawList(*m_container);
		GUIWindowManager::Instance()->DestroyGUIWindow(m_container);
		BaseClass::Destroy();
	}

	void GuiSlotTable::setItemSkillCdProgress(int index, int curValue, int maxValue)
	{
		if (index >= 0 && m_container && m_container->GetChildCount() > (size_t)index)
		{
			GuiItemSlot* slot = dynamic_cast<GuiItemSlot*>(m_container->GetChildByIndex(index));
			if (slot)
			{
				slot->setItemSkillCdProgress(curValue, maxValue);
			}
		}
	}
}