#pragma once
#ifndef __GUI_COUNTDOWN_HEADER__
#define __GUI_COUNTDOWN_HEADER__

#include "Core.h"
#include "gui_layout.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticText.h"
#include "GUI/RootGuiLayout.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_countdown : public gui_layout
	{
	public:
		gui_countdown();
		~gui_countdown();
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		void setCountdown(i8 second);
	private:
		GUIStaticText* m_lastTime = nullptr;
		GUIStaticText* m_tip = nullptr;
		int lastTime = 0;
		bool canRespawn = false;
	private:
		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}
	};

}

#endif // !__GUI_COUNTDOWN_HEADER__


