#pragma once
#ifndef __GUI_RANCH_PROSPERITY_RANK_HEADER__
#define __GUI_RANCH_PROSPERITY_RANK_HEADER__

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
	class gui_ranchProsperityRank : public  gui_layout
	{
	private:
		enum class ViewId {
			BTN_CLOSE
		};

	public:
		gui_ranchProsperityRank();
		virtual ~gui_ranchProsperityRank();
		void onLoad() override;
		void onShow() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool onClick(const EventArgs & events, ViewId viewId);


	private:
		bool changeRanks();
		bool userActorChange(i64 userId);

	private:
		SubscriptionGuard m_subscriptionGuard;

		bool m_dataChange = false;
		GUILayout* m_rankFirstLayout = nullptr;
		GUILayout* m_rankSecondLayout = nullptr;
		GUILayout* m_rankThirdLayout = nullptr;
		GUIListBox * m_lvRanks = nullptr;
		GUIStaticText* m_tvTitle = nullptr;

		map<i64, GUILayout*>::type m_rankLayoutMap;

	};
}

#endif