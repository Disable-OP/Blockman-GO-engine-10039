#pragma once
#ifndef __GUI_RANCH_MAIN_HEADER__
#define __GUI_RANCH_MAIN_HEADER__

#include "gui_layout.h"
#include "UI/GUIWindow.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticImage.h"
#include "GUI/gui_ranch.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_ranchMain : public  gui_layout
	{

	public:
		gui_ranchMain();
		virtual ~gui_ranchMain();
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		gui_ranch* getParent()
		{
			return dynamic_cast<gui_ranch*>(m_parentLayout);
		}
		bool onClick(const EventArgs & events, const gui_ranch::ViewId& viewId);

	};
}

#endif