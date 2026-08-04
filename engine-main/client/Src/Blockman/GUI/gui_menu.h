#pragma once
#ifndef __GUI_MENU_HEADER__
#define __GUI_MENU_HEADER__

#include "Core.h"
#include "gui_layout.h"
#include "UI/GUIWindow.h"
#include "UI/GUILayout.h"
#include "UI/GUIRadioButton.h"
#include "UI/GUIStaticImage.h"
#include "GUI/RootGuiLayout.h"
#include "gui_layout.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_menu : public  gui_layout
	{
		enum class ViewId
		{
			PLAYER_LIST,
			GAME_SETTING,
			GAME_OTHER_SETTING,
			GAME_RESUME,
			GAME_EXIT
		};

	public:
		gui_menu();
		virtual ~gui_menu();
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}
		bool onRadioChange(const EventArgs & events, const ViewId radioId);
		void openMenu();
		void checkBackgroundMusic();

	private:
		void onRadioSelected(const ViewId radioId);


	private:
		map<ViewId, GUIRadioButton*>::type m_radioMap;
		map<ViewId, gui_layout*>::type m_radioPanelMap;
	};
}

#endif