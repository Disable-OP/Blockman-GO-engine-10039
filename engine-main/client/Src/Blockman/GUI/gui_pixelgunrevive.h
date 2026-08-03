#pragma once
#ifndef __GUI_PIXEL_GUN_REVIVE_HEADER__
#define __GUI_PIXEL_GUN_REVIVE_HEADER__

#include "gui_layout.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIButton.h"
#include "GUI/RootGuiLayout.h"
#include "UI/GUILayout.h"
#include "gui_layout.h"
#include "GUI/GuiActorWindow.h"
#include "UI/GUIProgressBar.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_pixelgunrevive : public  gui_layout
	{
	private:
		SubscriptionGuard m_subscriptionGuard;

	public:
		gui_pixelgunrevive();
		virtual ~gui_pixelgunrevive();
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		void onShow() override;

		RootGuiLayout* getParent() { return dynamic_cast<RootGuiLayout*>(m_parentLayout); }

		bool onBattleClick(const EventArgs & events);

		static const int MAX_ARMS_NUM = 5;
		static const int MAX_GAD_GETS_NUM = 2;
		static const int WHITE_TIME = 200;

		struct PixelGunReviveData
		{
			String killer_use_gun_img = "";
			String killer_use_gun_name = "";
			String killer_name = "";
			String killer_arms_img[MAX_ARMS_NUM];
			String killer_arms_name[MAX_ARMS_NUM];
			String killer_gadgets_img[MAX_GAD_GETS_NUM];
			String killer_gadgets_name[MAX_GAD_GETS_NUM];
			String killer_block_img = "";
			String killer_block_name = "";
			int battle_time = 0;
			int killer_entityId = 0;
			int killer_itemId = 0;
			int cur_hp = 0;
			int max_hp = 0;
			int cur_defence = 0;
			int max_defence = 0;
		};

		bool refreshAll(const String& data);
		bool getAllInfo(const String& data);

	private:
		GUIStaticImage* m_killer_use_gun_img = nullptr;
		GUIStaticText* m_killer_use_gun_name = nullptr;
		GuiActorWindow* m_killer_mode = nullptr;
		GUIStaticImage* m_killer_arms_img[MAX_ARMS_NUM];
		GUIStaticText* m_killer_arms_name[MAX_ARMS_NUM];
		GUIStaticImage* m_killer_gadgets_img[MAX_GAD_GETS_NUM];
		GUIStaticText* m_killer_gadgets_name[MAX_GAD_GETS_NUM];
		GUIStaticImage* m_killer_block_img = nullptr;
		GUIStaticText* m_killer_block_name = nullptr;
		GUIButton* m_battle_btn = nullptr;
		GUIStaticText* m_battle_time_txt = nullptr;
		GUIProgressBar* m_health_value = nullptr;
		GUIProgressBar* m_defense_value = nullptr;
		GUIStaticText* m_health_number = nullptr;
		GUIStaticText* m_defense_number = nullptr;
		GUIStaticText* m_killer_name = nullptr;
		GUILayout* m_white_meng = nullptr;

		int m_white_time = 0;
		PixelGunReviveData m_pixel_gun_revive_data;
	};
}

#endif

