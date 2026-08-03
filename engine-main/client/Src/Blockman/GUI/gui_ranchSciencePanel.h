#pragma once
#ifndef __GUI_RANCH_SCIENCE_PANEL_HEADER__
#define __GUI_RANCH_SCIENCE_PANEL_HEADER__

#include "gui_layout.h"
#include "UI/GUIWindow.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticImage.h"
#include "GUI/gui_ranchHouse.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_ranchSciencePanel : public  gui_layout
	{
	private:
		enum class ViewId {

		};

	public:
		gui_ranchSciencePanel();
		virtual ~gui_ranchSciencePanel();
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		gui_ranchHouse* getParent()
		{
			return dynamic_cast<gui_ranchHouse*>(m_parentLayout);
		}
		bool onClick(const EventArgs & events, ViewId viewId);

	};
}

#endif