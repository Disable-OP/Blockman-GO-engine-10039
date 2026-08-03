#pragma once
#ifndef __GUI_SWITCHABLE_PROPS_PANEL_HEADER__
#define __GUI_SWITCHABLE_PROPS_PANEL_HEADER__

#include "Core.h"
#include "gui_layout.h"
#include "gui_personalShop.h"
#include "UI/GUIGridView.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_switchablePropsPanel : public gui_layout
	{
	private:
		GUIGridView* m_switchableProps = nullptr;
		std::list<SwitchableProp> m_props;
	public:
		gui_switchablePropsPanel();
		~gui_switchablePropsPanel();

		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		void refreshItems(std::list<SwitchableProp> props, float contentSize);
		
	};

}
#endif