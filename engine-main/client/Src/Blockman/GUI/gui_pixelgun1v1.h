#pragma once
#ifndef __GUI_PIXEL_GUN_1V1_HEADER__
#define __GUI_PIXEL_GUN_1V1_HEADER__

#include "gui_layout.h"
#include "UI/GUIWindow.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIButton.h"
#include "GUI/RootGuiLayout.h"
#include "gui_layout.h"
#include "UI/GUIListBox.h"
#include <vector>
#include "GUI/GuiActorWindow.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_pixelgun1v1 : public  gui_layout
	{
	private:
		SubscriptionGuard m_subscriptionGuard;

	public:
		gui_pixelgun1v1();
		virtual ~gui_pixelgun1v1();
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		void onShow() override;

		RootGuiLayout* getParent() { return dynamic_cast<RootGuiLayout*>(m_parentLayout); }

		bool onCloseClick(const EventArgs & events);
		bool onRematchClick(const EventArgs & events);
		bool onBoxImgClick(const EventArgs & events, int index);
		void refreshBoxSelect(int index);
		bool onWenHaoClick(const EventArgs & events, int wenhao);

		enum Gun1v1State
		{
			GS_PREPARE = 0,
			GS_START = 1,
		};

		enum Gun1v1HonorStage
		{
			GHS_INVALID = 0,
			GHS_ONE = 1,
			GHS_TWO = 2,
			GHS_THREE = 3,
			GHS_FOUR = 4,
			GHS_FIVE = 5,
			GHS_COUNT = GHS_FIVE,
		};

		enum Gun1v1WenHao
		{
			G1WH_LEFT = 0,
			G1WH_RIGHT = 1,
			G1WH_FiNAL = 2,
		};

		static const int MAX_BOX_NUM = 4;

		struct Gun1v1Data
		{
			int state = 0;
			int time = 0;
			int left_entityId = 0;
			int right_entityId = 0;
			int left_itemId = 0;
			int right_itemId = 0;
			String left_name = "";
			String right_name = "";
			int left_honor_stage = 0;
			int left_honor_score = 0;
			int right_honor_stage = 0;
			int right_honor_score = 0;
			int left_select_index = 0;
			String left_select_des = "";
			int right_select_index = 0;
			String right_select_des = "";
			int self_select_index = 0;
			int box_money[MAX_BOX_NUM];
			int final_select_index = 0;
			String final_select_des = "";
		};

		bool refreshAll(const String& data);
		bool getAllInfo(const String& data);

	private:
		GUIStaticText* m_time_txt = nullptr;
		GUIStaticImage* m_left_select_box_img = nullptr;
		GUIStaticImage* m_right_select_box_img = nullptr;
		GUIStaticText* m_left_select_box_des = nullptr;
		GUIStaticText* m_right_select_box_des = nullptr;
		GUIStaticImage* m_left_rank_img = nullptr;
		GUIStaticImage* m_right_rank_img = nullptr;
		GUIStaticText* m_left_name_txt = nullptr;
		GUIStaticText* m_right_name_txt = nullptr;
		GUIStaticText* m_left_honor_txt = nullptr;
		GUIStaticText* m_right_honor_txt = nullptr;
		GUIStaticImage* m_box_img[MAX_BOX_NUM];
		GUIStaticText* m_box_money[MAX_BOX_NUM];
		GUIStaticImage* m_box_select[MAX_BOX_NUM];
		GUIButton* m_rematch_btn = nullptr;
		GUIButton* m_exit_btn = nullptr;
		GUIStaticText* m_select_box_title = nullptr;
		GUIStaticImage* m_final_select_box = nullptr;
		GUIStaticText* m_final_select_des = nullptr;
		GUIStaticImage* m_vs_img = nullptr;
		GuiActorWindow* m_leftmode = nullptr;
		GuiActorWindow* m_rightmode = nullptr;
		GUIStaticImage* m_box_left_wenhao = nullptr;
		GUIStaticImage* m_box_right_wenhao = nullptr;
		GUIStaticImage* m_box_final_wenhao = nullptr;

		Gun1v1Data m_gun_1v1_data;
	};
}

#endif
