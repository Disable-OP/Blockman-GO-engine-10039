#pragma once
#ifndef __GUI_SUPER_PROPS_PANEL_HEADER__
#define __GUI_SUPER_PROPS_PANEL_HEADER__

#include "Core.h"
#include "gui_layout.h"
#include "gui_superShop.h"
#include "UI/GUIGridView.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_superPropsPanel : public gui_layout
	{
	private:
		GUIGridView* m_superProps = nullptr;
		std::list<SuperProp> m_props;
	public:
		gui_superPropsPanel();
		~gui_superPropsPanel();

		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		void refreshItems(std::list<SuperProp> props, float contentSize);

	};

}
#endif