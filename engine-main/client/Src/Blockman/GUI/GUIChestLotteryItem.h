#ifndef __GUI_CHEST_LOTTERY_ITEM_HEADER__
#define __GUI_CHEST_LOTTERY_ITEM_HEADER__

#include "UI/GUIWindow.h"
#include "UI/GUIStaticImage.h"
#include "GUI/CustomGuiWindow.h"
#include "Setting/ChestLotterySetting.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GUIChestLotteryItem : public CustomGuiWindow
	{
	private:
		LotteryChest* m_data = nullptr;
		
		GUIWindow* m_window = nullptr;
		GUIStaticImage* m_key_icon = nullptr;

	private:
		using BaseClass = CustomGuiWindow;
		void removeComponents();
		void initView();

		bool onQuestionClick(const EventArgs & events);
		bool onOpenClick(const EventArgs & events);

	public:
		GUIChestLotteryItem(const GUIWindowType& nType, const GUIString& strName);
		~GUIChestLotteryItem() = default;

		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;

		void SetLotteryChest(LotteryChest* data);
	};
}

#endif