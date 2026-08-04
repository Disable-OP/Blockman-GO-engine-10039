#ifndef __GUI_SEASON_REWARD_ITEM_HEADER__
#define __GUI_SEASON_REWARD_ITEM_HEADER__

#include "UI/GUIWindow.h"
#include "GUI/CustomGuiWindow.h"
#include "Setting/SeasonSetting.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GUISeasonRewardItem : public CustomGuiWindow
	{
	private:
		GUIWindow* m_window = nullptr;

	private:
		using BaseClass = CustomGuiWindow;
		void removeComponents();

	public:
		GUISeasonRewardItem(const GUIWindowType& nType, const GUIString& strName);
		~GUISeasonRewardItem() = default;

		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;

		void SetSeasonReward(SeasonReward* reward, bool isBig);
		void SetSeasonRankReward(SeasonRankReward* reward, bool isBig);

	};
}

#endif