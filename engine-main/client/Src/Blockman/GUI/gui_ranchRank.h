#pragma once
#ifndef __GUI_RANCH_RANK_HEADER__
#define __GUI_RANCH_RANK_HEADER__

#include "gui_layout.h"
#include "UI/GUIWindow.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIListBox.h"
#include "Util/SubscriptionGuard.h"
#include "World/Ranch.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_ranchRank : public  gui_layout
	{
	private:
		enum class ViewId {
			BTN_CLOSE
		};

	public:
		gui_ranchRank();
		virtual ~gui_ranchRank();
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool onClick(const EventArgs & events, ViewId viewId);


	private:
		bool changeRanks(int rankType);

	private:
		SubscriptionGuard m_subscriptionGuard;

		GUIListBox * m_lvRanks = nullptr;
		GUIStaticText* m_tvTitle = nullptr;


	};
}

#endif