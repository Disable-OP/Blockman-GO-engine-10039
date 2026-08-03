#pragma once
#ifndef __GUI_BUILDWAR_GUESS_HEADER__
#define __GUI_BUILDWAR_GUESS_HEADER__

#include "gui_layout.h"
#include "UI/GUIWindow.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIButton.h"
#include "UI/GUIListBox.h"
#include "GUI/RootGuiLayout.h"
#include "gui_layout.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_buildwarGuess : public  gui_layout
	{
	private:
		SubscriptionGuard m_subscriptionGuard;

	public:
		gui_buildwarGuess();
		virtual ~gui_buildwarGuess();
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		RootGuiLayout* getParent() { return dynamic_cast<RootGuiLayout*>(m_parentLayout); }

		struct PlayerInfo
		{
			ui64 user_id;
			int room_id;
			String name;
		};

		bool RefreshAllInfo(String info, int guess_room_id);
		bool getGuessInfo(const String& info, std::vector<PlayerInfo>& player_list, std::vector<int>& grade_room);
		bool setIsNeedShow(bool show) { m_isNeedShow = show; return true; }

	private:
		GUIListBox * m_List = nullptr;
		bool m_isNeedShow = false;

	};
}

#endif
