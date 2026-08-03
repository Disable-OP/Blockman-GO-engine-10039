#pragma once
#ifndef __GUI_RANCH_FARMING_HEADER__
#define __GUI_RANCH_FARMING_HEADER__

#include "gui_layout.h"
#include "UI/GUIWindow.h"
#include "UI/GUILayout.h"
#include "GUI/gui_ranch.h"
#include "UI/GUIRadioButton.h"
#include "Util/SubscriptionGuard.h"
#include "UI/GUIListHorizontalBox.h"
#include "UI/GUIGridView.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_ranchFarming : public  gui_layout
	{
	private:
		enum class ViewId {
			PLANT,
			RAISE
		};

	public:
		gui_ranchFarming();
		virtual ~gui_ranchFarming();
		void onLoad() override;
		void onShow() override;
		void onUpdate(ui32 nTimeElapse) override;
		gui_ranch* getParent()
		{
			return dynamic_cast<gui_ranch*>(m_parentLayout);
		}
		bool onRadioChange(const EventArgs & events, ViewId viewId);
		bool onRanchFarmingChange(i32 type);
	private:
		void initPlantItem();
		void initRaiseItem();
		void plantItemSizeChange();

	private:
		float m_gvPlantXSize = 0.f;

		bool m_plantDataChange = false;
		bool m_raiseDataChange = false;
		SubscriptionGuard m_subscriptionGuard;
		map<ViewId, GUIWindow*>::type m_radioPanelMap;
		map<ViewId, GUIRadioButton*>::type m_radioMap;

		GUIListHorizontalBox* m_lvRaise = nullptr;
		GUIGridView* m_gvPlant = nullptr;
	};
}

#endif