#pragma once
#ifndef __GUI_CHEST_LOTTERY_HEADER__
#define __GUI_CHEST_LOTTERY_HEADER__

#include "Core.h"
#include "gui_layout.h"
#include "Util/SubscriptionGuard.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIProgressBar.h"
#include "UI/GUILayout.h"
#include "GUI/GUIChestLotteryItem.h"
#include "GUI/GUIChestRewardResult.h"
#include "GUI/RootGuiLayout.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_chestLottery : public gui_layout
	{
	private:
		SubscriptionGuard m_subscriptionGuard;

		GUIChestRewardResult* m_reward_result = nullptr;

		GUILayout* m_integral = nullptr;
		GUIProgressBar* m_integral_progress = nullptr;
		GUIStaticText* m_integral_progress_text = nullptr;

		GUIChestLotteryItem* m_chest_lv1 = nullptr;
		GUIChestLotteryItem* m_chest_lv2 = nullptr;
		GUIChestLotteryItem* m_chest_lv3 = nullptr;

	public:
		gui_chestLottery();
		~gui_chestLottery();

		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;

		bool setIntegralProgress(int curIntegral, int needIntegral);

	private:	
		bool onCloseClick(const EventArgs & events);
		bool onUltimateChestClick(const EventArgs & events);
		bool onUltimateChestQuestionClick(const EventArgs & events);
		bool onUpdateLotteryChestData(const String& data);

		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}
	};

}
#endif