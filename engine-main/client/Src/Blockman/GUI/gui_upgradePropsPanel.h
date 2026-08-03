#pragma once
#ifndef __GUI_UPGRADE_PROPS_PANEL_HEADER__
#define __GUI_UPGRADE_PROPS_PANEL_HEADER__

#include "Core.h"
#include "gui_layout.h"
#include "gui_personalShop.h"
#include "UI/GUIGridView.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_upgradePropsPanel : public gui_layout
	{
	private:
		GUIGridView* m_upgradeProps = nullptr;
		std::list<UpgradeProp> m_props;
	public:
		gui_upgradePropsPanel();
		~gui_upgradePropsPanel();

		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		void refreshItems(std::list<UpgradeProp> props, float contentSize);
		
	};

}
#endif