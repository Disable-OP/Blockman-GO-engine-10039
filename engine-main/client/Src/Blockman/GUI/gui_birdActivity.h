#pragma once
#ifndef __GUI_BIRD_ACTIVITY_HEADER__
#define __GUI_BIRD_ACTIVITY_HEADER__
#include "gui_layout.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIListHorizontalBox.h"
#include "GUI/RootGuiLayout.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_birdActivity : public  gui_layout
	{
	private:
		enum class ViewId {
			BUY,
			CLOSE
		};

	private:
		GUIStaticText* m_time = nullptr;
		GUIListHorizontalBox* m_gift = nullptr;
		GUIStaticImage* m_currencyIcon = nullptr;
		GUIStaticText* m_currencyNum = nullptr;
	public:
		gui_birdActivity();
		~gui_birdActivity();
		void onShow() override;
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool updateBirdActivity();
		void showBirdActivity();
		String timeFormat(i32 time);
	private:
		bool m_isActivityUpdate = false;
		i32 m_timeLeft = 0;
		i32 m_buyTime = 0;
	private:
		bool onClick(const EventArgs & events, ViewId viewId);
	};

}
#endif