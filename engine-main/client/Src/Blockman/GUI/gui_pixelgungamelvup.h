#ifndef __GUI_PIXEL_GUNGAME_LVUP_HEADER__
#define __GUI_PIXEL_GUNGAME_LVUP_HEADER__

#include "gui_layout.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIStaticImage.h"
#include "GUI/RootGuiLayout.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_pixelgungamelvup : public gui_layout
	{
	public:
		static const int MAX_REWARD_NUM = 3;

		struct LvUpReward
		{
			String img = "";
			int num = 0;
		};

	private:
		GUIStaticImage* m_image[MAX_REWARD_NUM];
		GUIStaticText* m_num[MAX_REWARD_NUM];
		LvUpReward m_lvup_reward[MAX_REWARD_NUM];
		GUIStaticText* m_title = nullptr;
		int m_level = 0;

	public:
		gui_pixelgungamelvup();
		~gui_pixelgungamelvup();

		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		RootGuiLayout* getParent() { return dynamic_cast<RootGuiLayout*>(m_parentLayout); }

	private:
		bool onContentClick(const EventArgs & events);
		bool onSureClick(const EventArgs & events);
		bool onShowPixelGunGameLvUp(const String& data);
		bool getAllInfo(const String& data);
	};
}

#endif
