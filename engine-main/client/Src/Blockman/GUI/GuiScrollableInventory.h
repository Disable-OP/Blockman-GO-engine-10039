#ifndef __GUI_SCROLLABLE_INVENTORY_HEADER__
#define __GUI_SCROLLABLE_INVENTORY_HEADER__

#include <memory>
#include "UI/GUIWindow.h"
#include "UI/GUIGridView.h"
#include "UI/GUIScrollablePane.h"
#include "GUI/GuiSlotTable.h"
#include "Inventory/IInventory.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GuiScrollableInventory : public CustomGuiWindow
	{
	public:
		using SlotSize = Vector2;
		class ItemStackTouchEventArgs : public EventArgs
		{
		public:
			ItemStackTouchEventArgs(size_t index) : index(index) {}
			size_t index;
		};
		static String ITEM_STACK_TOUCH_DOWN;
		static String ITEM_STACK_TOUCH_UP;
		static String ITEM_STACK_LONG_TOUCH_START;
		static String ITEM_STACK_LONG_TOUCH_END;

	private:
		using BaseClass = CustomGuiWindow;
		GUIGridView* m_gridView = nullptr;
		IInventory* m_inventory = nullptr;
		SlotSize m_slotSize{ 0, 0 };
		size_t m_columnCount = 0;
		bool m_isAutoColumnCount = false;
		int m_selectionIndex = -1;
		int m_oldSelectionIndex = -2;
		Color m_itemStackBackgroundColor = Color::TRANS;
		GuiItemStack::NumberVisibility m_itemStackNumberVisibility = GuiItemStack::NumberVisibility::AUTO;

		bool onItemStackTouchDown(const EventArgs & events);
		bool onItemStackTouchUp(const EventArgs & events);
		bool onItemStackLongTouchStart(const EventArgs & events);
		bool onItemStackLongTouchEnd(const EventArgs & events);
		bool onItemSlotTouchDown(const EventArgs & events);
		bool onItemSlotTouchUp(const EventArgs & events);
		bool onItemSlotTouchMove(const EventArgs & events);
		bool onItemSlotMotionRelease(const EventArgs & events);
		void updateGridView();
		void updateSelectionFrame();
		void resize();

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;

	public:
		GuiScrollableInventory(GUIWindowType nType, const GUIString& strName);
		~GuiScrollableInventory() = default;

		void selectItemSlot(size_t index)
		{
			m_selectionIndex = index;
		}
		void unselectItemSlot()
		{
			m_selectionIndex = -1;
		}
		IInventory* getInventory() const
		{
			return m_inventory;
		}
		ItemStackPtr getItemStack(size_t index) const
		{
			return getInventory()->getStackInSlot(index);
		}
		void setInventory(IInventory* inventory)
		{
			m_inventory = inventory;
		}
		void setGrayedOut(size_t index, bool grayedOut)
		{
			dynamic_cast<GuiItemSlot*>(m_gridView->GetItem(index))->setItemStackGrayedOut(grayedOut);
		}

		void setAutoColumnCount(bool isAutoColumnCount) { this->m_isAutoColumnCount = isAutoColumnCount; }
		bool SetProperty(const GUIString& strName, const GUIString& strValue) override;
		bool GetProperty(const GUIString& strName, GUIString& strValue) override;
		void GetPropertyList(GUIPropertyList& properlist) override;
		bool GetPropertyType(const GUIString& strName, GUIPropertyType& type) override;

		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;

		// used to create animation of moving item stack
		UVector2 getGuiItemStackPosition(size_t index)
		{
			return dynamic_cast<GuiItemSlot*>(m_gridView->GetItem(index))->getGuiItemStackPosition();
		}

		GuiItemStack* cloneGuiItemStack(size_t index, const GUIString& newName)
		{
			return dynamic_cast<GuiItemSlot*>(m_gridView->GetItem(index))->cloneGuiItemStack(newName);
		}

	};
}

#endif // !__GUI_SCROLLABLE_INVENTORY_HEADER__

