#pragma once
#ifndef __GUI_CUSTOM_PROPS_PANEL_HEADER__
#define __GUI_CUSTOM_PROPS_PANEL_HEADER__

#include "Core.h"
#include "UI/GUIWindow.h"
#include "UI/GUIGridView.h"
#include "gui_layout.h"
#include "gui_personalShop.h"

using namespace LORD;

namespace BLOCKMAN
{

	class gui_customPropsPanel : public gui_layout
	{
	private:
		GUIGridView* m_customProps = nullptr;

		String m_groupId; 
		std::list<CustomProp> m_props;

	public:
		gui_customPropsPanel();
		~gui_customPropsPanel() = default;

		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;

		void setCustomPropGroup(String groupId, std::list<CustomProp> props, float contentSize);
		void updateProp(CustomProp prop);
	};

}
#endif