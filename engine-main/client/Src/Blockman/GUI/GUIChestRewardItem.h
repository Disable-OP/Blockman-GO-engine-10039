#ifndef __GUI_CHEST_REWARD_ITEM_HEADER__
#define __GUI_CHEST_REWARD_ITEM_HEADER__

#include "UI/GUIWindow.h"
#include "GUI/CustomGuiWindow.h"
#include "Setting/ChestLotterySetting.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GUIChestRewardItem : public CustomGuiWindow
	{
	private:
		int m_id = 0;
		GUIWindow* m_window = nullptr;

	private:
		using BaseClass = CustomGuiWindow;
		void removeComponents();

		bool onItemClick(const EventArgs & events);

	public:
		GUIChestRewardItem(const GUIWindowType& nType, const GUIString& strName);
		~GUIChestRewardItem() = default;

		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;

		void SetLotteryChestReward(LotteryChestReward* reward);
	};
}

#endif