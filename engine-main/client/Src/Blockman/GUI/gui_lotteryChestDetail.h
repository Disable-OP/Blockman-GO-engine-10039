#ifndef __GUI_CHEST_DETAIL_HEADER__
#define __GUI_CHEST_DETAIL_HEADER__

#include "gui_layout.h"
#include "UI/GUILayout.h"
#include "UI/GUIGridView.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIStaticImage.h"
#include "GUI/GUIChestRewardDetail.h"
#include "GUI/RootGuiLayout.h"
#include "Setting/ChestLotterySetting.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_lotteryChestDetail : public gui_layout
	{
	private:
		GUIChestRewardDetail* m_chest_reward_detail = nullptr;

		GUIStaticText* m_title = nullptr;
		GUIStaticImage* m_image = nullptr;
		GUIStaticImage* m_integral_icon = nullptr;
		GUIStaticText* m_integral_text = nullptr;

		map<LotteryChestType, GUIGridView*>::type m_rewards_map;

	public:
		gui_lotteryChestDetail();
		~gui_lotteryChestDetail();

		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;

		void ShowChestDetail(LotteryChestType type);

	private:
		void UpdateUIByType(LotteryChestType type);
		bool onCloseClick(const EventArgs & events);
		bool onShowRewardDetail(int id);
		
		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}
	};
}

#endif