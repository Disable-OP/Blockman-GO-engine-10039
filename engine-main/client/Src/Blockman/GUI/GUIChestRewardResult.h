#ifndef __GUI_CHEST_REWARD_RESULT_HEADER__
#define __GUI_CHEST_REWARD_RESULT_HEADER__

#include "gui_layout.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUILayout.h"
#include "GUI/GuiActorWindow.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GUIChestRewardResult : public gui_layout
	{
	private:
		int m_wait_open_time = 0;

		GuiActorWindow* m_chest = nullptr;
		GUILayout* m_content = nullptr;
		GUIStaticImage* m_image = nullptr;
		GUIStaticText* m_name = nullptr;

	public:
		GUIChestRewardResult();
		~GUIChestRewardResult();

		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;

	private:
		bool onContentClick(const EventArgs & events);
		bool onContinueClick(const EventArgs & events);

		bool onOpenLotteryChest(int chestId);
		bool onShowChestRewardResult(int id);
	};
}

#endif