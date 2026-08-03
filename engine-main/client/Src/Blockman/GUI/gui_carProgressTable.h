#pragma once
#ifndef __GUI_CAR_PROGRESS_TABLE_HEADER__
#define __GUI_CAR_PROGRESS_TABLE_HEADER__

#include "GUI/gui_layout.h"
#include "GUI/RootGuiLayout.h"
#include "GUI/GUICarProgress.h"
#include "UI/GUILayout.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_carProgressTable : public gui_layout
	{

	public:
		gui_carProgressTable();
		~gui_carProgressTable();

		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;

	private:
		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}
		bool onCarDirectionUpdate(int type, int direction);
		bool onCarProgressUpdate(int type, float progress);

	private:
		SubscriptionGuard m_subscriptionGuard;
		float m_recordPosY = 0.f;
		bool m_visiable = false;
	
		GUILayout* m_progress_list = nullptr;
		GUICarProgress* m_yellow_progress = nullptr;
		GUICarProgress* m_green_progress = nullptr;
		GUICarProgress* m_red_progress = nullptr;

	};
}
#endif
