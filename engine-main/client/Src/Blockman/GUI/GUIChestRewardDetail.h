#ifndef __GUI_CHEST_REWARD_DETAIL_HEADER__
#define __GUI_CHEST_REWARD_DETAIL_HEADER__

#include "gui_layout.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIStaticImage.h"
#include "Setting/ChestLotterySetting.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GUIChestRewardDetail : public gui_layout
	{
	private:
		GUIStaticText* m_title = nullptr;
		GUIStaticImage* m_image = nullptr;
		GUIStaticText* m_name = nullptr;
		GUIStaticText* m_desc = nullptr;

	public:
		GUIChestRewardDetail();
		~GUIChestRewardDetail();

		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;

		void ShowLotteryChestReward(LotteryChestReward* reward);

	private:
		bool onContentClick(const EventArgs & events);
	};
}

#endif