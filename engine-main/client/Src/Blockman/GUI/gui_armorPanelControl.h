#ifndef __GUI_ARMOR_PANEL_CONTROL_HEADER__
#define __GUI_ARMOR_PANEL_CONTROL_HEADER__

#include "Core.h"
#include "GUI/gui_layout.h"
#include "GUI/GuiScrollableInventory.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIRadioButton.h"

namespace BLOCKMAN
{
	class GuiActorWindow;

	class gui_armorPanelControl : public gui_layout
	{
	private:
		enum class ViewId
		{
			ALL,
			ARMOR,
			MATERIALS
		};

	public:
		gui_armorPanelControl();
		void onLoad() override;
		void onShow() override;
		void onUpdate(ui32) override;
		bool equipArmor(const EventArgs& event);
		bool unequipArmor(const EventArgs& event);
	private:
		bool onItemStackTouchUp(const EventArgs& event);
		bool onRadioChange(const EventArgs& events, const ViewId viewId);
		void updateData();

	private:

		int m_selectionIndex = -1;
		ViewId m_showViewId = ViewId::ALL;
		static const ViewId ALL_VIEW_ID[];
		map<ViewId, GUIRadioButton*>::type m_radioMap;

		GuiScrollableInventory* m_guiScrollableInventory = nullptr;
		GuiSlotTable* m_equipedArmorTable = nullptr;
		GuiActorWindow* m_actorWindow = nullptr;
		GUIStaticText* m_actorTitleName = nullptr;
		GUIStaticText* m_actorInventoryName = nullptr;
	};
}

#endif // !__GUI_MAIN_INVENTORY_PANEL_HEADER__

