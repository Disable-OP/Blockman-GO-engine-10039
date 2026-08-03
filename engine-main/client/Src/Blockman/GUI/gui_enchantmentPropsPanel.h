#pragma once
#ifndef __GUI_ENCHANTMENT_PROPS_PANEL_HEADER__
#define __GUI_ENCHANTMENT_PROPS_PANEL_HEADER__

#include "Core.h"
#include "gui_layout.h"
#include "gui_personalShop.h"
#include "UI/GUIGridView.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_enchantmentPropsPanel : public gui_layout
	{
	private:
		GUIGridView* m_enchantmentProps = nullptr;
		std::list<EnchantmentProp> m_props;
	public:
		gui_enchantmentPropsPanel();
		~gui_enchantmentPropsPanel();

		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		void refreshItems(std::list<EnchantmentProp> props, float contentSize);
		
	};

}
#endif