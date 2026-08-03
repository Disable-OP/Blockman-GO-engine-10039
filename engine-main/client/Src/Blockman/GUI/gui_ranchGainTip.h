#pragma once
#ifndef __GUI_RANCH_GAIN_TIP_HEADER__
#define __GUI_RANCH_GAIN_TIP_HEADER__

#include "gui_layout.h"
#include "UI/GUIWindow.h"
#include "UI/GUILayout.h"
#include "GUI/gui_ranch.h"
#include "UI/GUIListHorizontalBox.h"
#include "Setting/RanchSetting.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_ranchGainTip : public  gui_layout
	{

	public:
		gui_ranchGainTip();
		virtual ~gui_ranchGainTip();
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		gui_ranch* getParent()
		{
			return dynamic_cast<gui_ranch*>(m_parentLayout);
		}
	

	private:
		bool showTip(const vector<RanchCommon>::type& items);

		void initItem(const vector<RanchCommon>::type& items);

	private:
		GUIListHorizontalBox * m_listView = nullptr;
		SubscriptionGuard m_subscriptionGuard;
		const float m_showTime = 2000.f;
		float m_leftTime = 0;
	};
}

#endif