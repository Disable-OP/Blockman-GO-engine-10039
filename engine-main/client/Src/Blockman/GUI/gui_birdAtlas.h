#pragma once
#ifndef __GUI_BIRD_ATLAS_HEADER__
#define __GUI_BIRD_ATLAS_HEADER__
#include "gui_layout.h"
#include "UI/GUILayout.h"
#include "UI/GUIListBox.h"
#include "GUI/RootGuiLayout.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_birdAtlas : public  gui_layout
	{
	private:
		GUIListBox* m_lbDetails = nullptr;

		bool m_isAtlasUpdate = false;
	public:
		gui_birdAtlas();
		~gui_birdAtlas();
		void onShow() override;
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		void showItemView();
		bool updateBirdAtlas();
	private:
		bool onClose(const EventArgs & events);
	};

}
#endif