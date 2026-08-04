#pragma once
#ifndef __GUI_RANCH_ACHIEVEMENT_HEADER__
#define __GUI_RANCH_ACHIEVEMENT_HEADER__

#include "gui_layout.h"
#include "UI/GUIWindow.h"
#include "UI/GUILayout.h"
#include "UI/GUIListBox.h"
#include "UI/GUIStaticText.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_ranchAchievement : public  gui_layout
	{
	private:
		enum class ViewId {

		};

	public:
		gui_ranchAchievement();
		virtual ~gui_ranchAchievement();
		void onLoad() override;
		void onShow() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool onClick(const EventArgs & events, ViewId viewId);
		bool onRanchAchievementChange();

	private:
		bool m_dataChange = false;
		GUIStaticText * m_tvValue = nullptr;
		GUIListBox* m_lvItems = nullptr;
	};
}

#endif