#pragma once
#ifndef __GUI_RANCH_HOUSE_HEADER__
#define __GUI_RANCH_HOUSE_HEADER__

#include "gui_layout.h"
#include "UI/GUIWindow.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIRadioButton.h"
#include "GUI/gui_ranch.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_ranchHouse : public  gui_layout
	{
	private:
		enum class ViewId {
			PANEL_HOUSE,
			PANEL_SCIENCE,
			BTN_REMOVE
		};

	public:
		gui_ranchHouse();
		virtual ~gui_ranchHouse();
		void onLoad() override;
		void onHide() override;
		void onShow() override;
		void onUpdate(ui32 nTimeElapse) override;
		gui_ranch* getParent()
		{
			return dynamic_cast<gui_ranch*>(m_parentLayout);
		}
		bool onRadioChange(const EventArgs & events, const ViewId radioId);
		bool onClick(const EventArgs & events, ViewId viewId);

	private:
		map<ViewId, gui_layout*>::type m_radioPanelMap;
		map<ViewId, GUIRadioButton*>::type m_radioMap;

	};
}

#endif