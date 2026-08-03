#ifndef __SLOT_TABLE_HEADER__
#define __SLOT_TABLE_HEADER__

#include <memory>
#include "UI/GUIWindow.h"
#include "GUI/GuiItemSlot.h"

using namespace LORD;

namespace LORD
{
	class GUIMutualNonoverlappingGroup;
}

namespace BLOCKMAN
{
	class Ingredient;

	class GuiSlotTable : public CustomGuiWindow
	{
	private:
		using IngredientPtr = std::shared_ptr<Ingredient>;

	public:
		using SlotSize = Vector2;
		class ItemStackTouchEventArgs : public EventArgs
		{
		public:
			ItemStackTouchEventArgs(size_t row, size_t column) : row(row), column(column) {}
			size_t row, column;
		};
		struct TableSize
		{
			size_t row, column;
			friend bool operator==(const TableSize& lhs, const TableSize& rhs)
			{
				return lhs.row == rhs.row && lhs.column == rhs.column;
			}
		};

	private:
		using BaseClass = CustomGuiWindow;
		GUIMutualNonoverlappingGroup* m_container = nullptr;
		vector<GuiItemSlot*>::type m_guiItemSlots;
		TableSize m_tableSize{0, 0};
		TableSize m_oldTableSize{0, 0};
		SlotSize m_slotSize{ 0, 0 };
		double m_horizontalInterval = 0.0;
		double m_verticalInterval = 0.0;
		int m_selectionIndex = -1;
		int m_oldSelectionIndex = -2;
		GuiItemSlot::Style m_slotStyle = GuiItemSlot::Style::ROCK_TRANS;
		Color m_itemStackBackgroundColor = Color::TRANS;
		GuiItemStack::NumberVisibility m_itemStackNumberVisibility = GuiItemStack::NumberVisibility::AUTO;

		size_t childIndex(size_t row, size_t column) const 
		{
			return row * m_tableSize.column + column;
		}
		void resizeSlotTable();
		void layoutGuiItemSlots();
		bool onItemStackTouchUp(const EventArgs & events);
		bool onItemStackLongTouchStart(const EventArgs& events);
		bool onItemStackLongTouchEnd(const EventArgs& events);
		void updateSelectionFrame();
		void updateItemStackSettings();
		void clear();

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;

	public:
		GuiSlotTable(const GUIWindowType& nType, const GUIString& strName);
		~GuiSlotTable() = default;

		bool SetProperty(const GUIString& strName, const GUIString& strValue) override;
		bool GetProperty(const GUIString& strName, GUIString& strValue) override;
		void GetPropertyList(GUIPropertyList& properlist) override;
		bool GetPropertyType(const GUIString& strName, GUIPropertyType& type) override;
		void setItemStackArmorType(size_t row, size_t column, ArmorType type)
		{
			m_guiItemSlots.at(childIndex(row, column))->setArmorType(type);
		}
		void setItemStack(size_t row, size_t column, ItemStackPtr pItemStack)
		{
			m_guiItemSlots.at(childIndex(row, column))->setItemStack(pItemStack);
		}
		void setIngredient(size_t row, size_t column, IngredientPtr pIngredient)
		{
			m_guiItemSlots.at(childIndex(row, column))->setIngredient(pIngredient);
		}
		void selectItemSlot(size_t index)
		{
			m_selectionIndex = index;
		}
		void selectItemSlot(size_t row, size_t column)
		{
			selectItemSlot(childIndex(row, column));
		}
		void setGrayedOut(size_t row, size_t column, bool grayedOut)
		{
			m_guiItemSlots.at(childIndex(row, column))->setItemStackGrayedOut(grayedOut);
		}
		void setTableSize(size_t rowCount, size_t columnCount)
		{
			m_tableSize = { rowCount, columnCount };
			resizeSlotTable();
		}

		size_t getRowCount() {
			return m_tableSize.row;
		}

		size_t getColumnCount() {
			return m_tableSize.column;
		}

		void setDropItemProgress(int index, float progress);
		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;
		void setItemSkillCdProgress(int index, int curValue, int maxValue);
	};
}

#endif
