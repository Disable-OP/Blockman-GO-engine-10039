#ifndef __GUI_MAIN_INVENTORY_PANEL_CONTROL_HEADER__
#define __GUI_MAIN_INVENTORY_PANEL_CONTROL_HEADER__

#include "GUI/gui_layout.h"
#include "GUI/GuiScrollableInventory.h"
#include "GUI/RootGuiLayout.h"

namespace BLOCKMAN
{
	class gui_mainInventoryPanelControl : public gui_layout
	{
	public:
		gui_mainInventoryPanelControl();
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool onItemStackTouchUp(const EventArgs& event);
		void resetSelection() {
			m_selectionIndex = -1;
		}

	private:
		int m_selectionIndex = -1;
		GuiScrollableInventory* m_guiScrollableInventory = nullptr;
	};
}

#endif // !__GUI_MAIN_INVENTORY_PANEL_HEADER__

