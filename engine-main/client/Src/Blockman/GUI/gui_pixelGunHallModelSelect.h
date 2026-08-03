#ifndef __GUI_PIXEL_GUNHALL_MODEL_SELECT_HEADER__
#define __GUI_PIXEL_GUNHALL_MODEL_SELECT_HEADER__

#include "gui_layout.h"
#include "UI/GUIWindow.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIButton.h"
#include "GUI/RootGuiLayout.h"
#include "UI/GUIGridView.h"
#include <vector>

using namespace LORD;

namespace BLOCKMAN
{
	class gui_pixelGunHallModelSelect : public  gui_layout
	{
	private:
		SubscriptionGuard m_subscriptionGuard;

	public:
		gui_pixelGunHallModelSelect();
		virtual ~gui_pixelGunHallModelSelect();
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		void onShow() override;

		RootGuiLayout* getParent() { return dynamic_cast<RootGuiLayout*>(m_parentLayout); }

		bool onCloseClick(const EventArgs & events);
		bool onStorageClick(const EventArgs & events);
		bool onFightClick(const EventArgs & events);

		struct MapInfo
		{
			int map_id = 0;
			String map_img = "";
			String map_name = "";
			String map_show_name = "";
			String small_info_img = "";
			int openMoneyType = 0;
			int openMoneyNum = 0;
			bool unlock = false;
			String map_size = "";
			int map_type = 0;
			String fragment_gun_img = "";
			String fragment_gun_num_txt = "";
			int fragment_jindu = 0;
			String random_reward_img1 = "";
			String random_reward_img2 = "";
			int random_reward_num1 = 0;
			int random_reward_num2 = 0;
		};

		struct ModeInfo
		{
			String mode_img = "";
			String gameType = "";
			String mode_name = "";
			String mode_info1 = "";
			String mode_info_num1 = "";
			String mode_info2 = "";
			String mode_info_num2 = "";
			bool mode_info_show1 = false;
			bool mode_info_show2 = false;
			String mode_tip = "";
			String mode_title = "";
			int isOpen = 0;
			int openLv = 0;
			int index = 0;
			bool unlock = false;
			int min_money = 0;
			std::vector<MapInfo> map_info;
		};

 		bool refreshAll(const String& data);
		bool getAllInfo(const String& data, std::vector<ModeInfo>& mode_info);
		bool onModeImgClick(int index);
 		void refreshAllSelf();
		void refreshAllList();
		bool refreshMapSelect(int map_index);
		bool refreshBottomReward(int map_index);

		static const int MAX_PIXEL_GUNHALL_MODE_NUM = 8;
		static const int MAX_PIXEL_GUNHALL_MODE_SMALL_INFO_NUM = 2;
		static const int MAX_PIXEL_GUNHALL_MODE_PER_NUM = 5;
		static const int MAX_PIXEL_GUNHALL_FRAGMENT_NUM = 3;
		static const int FIGHT_BTN_CD = 5000;
		static const int MAX_PIXEL_GUNHALL_REWARD_NUM = 2;
		static const int MODE_CLICK_CD = 1000;

	private:
		int m_current_mode_index = 0;
		int m_current_map_index = 0;
		int m_fragment_jindu = 0;
		int m_btn_fight_cd = 0;
		int m_mode_click_cd = 0;

		std::vector<ModeInfo> m_mode_info;

		GUIStaticText* m_mode_des_title = nullptr;
		GUIStaticText* m_mode_des = nullptr;
		GUIButton* m_closeBtn = nullptr;
		GUIButton* m_storageBtn = nullptr;
		GUIButton* m_fightBtn = nullptr;
		GUILayout* m_left_bg = nullptr;
		GUILayout* m_right_bg = nullptr;
		GUILayout* m_fragLayout = nullptr;
		GUILayout* m_rewardLayout = nullptr;
		GUIStaticImage* m_rightFragmentImg = nullptr;
		GUIStaticText* m_rightFragmentNumTxt = nullptr;
		GUIStaticImage* m_mode_bg[MAX_PIXEL_GUNHALL_MODE_NUM];
		GUIStaticText* m_mode_lv[MAX_PIXEL_GUNHALL_MODE_NUM];
		GUIStaticImage* m_mode_img[MAX_PIXEL_GUNHALL_MODE_NUM];
		GUIStaticImage* m_mode_select[MAX_PIXEL_GUNHALL_MODE_NUM];
		GUIStaticImage* m_mode_meng[MAX_PIXEL_GUNHALL_MODE_NUM];
		GUIStaticImage* m_mode_lock_img[MAX_PIXEL_GUNHALL_MODE_NUM];
		GUIStaticText* m_mode_name[MAX_PIXEL_GUNHALL_MODE_NUM];
		GUIStaticImage* m_mode_small_info[MAX_PIXEL_GUNHALL_MODE_NUM][MAX_PIXEL_GUNHALL_MODE_SMALL_INFO_NUM];
		GUIStaticText* m_mode_small_num[MAX_PIXEL_GUNHALL_MODE_NUM][MAX_PIXEL_GUNHALL_MODE_SMALL_INFO_NUM];
		GUIStaticImage* m_fragment_jindu_img[MAX_PIXEL_GUNHALL_FRAGMENT_NUM];
		GUILayout* m_mapInfoLayout = nullptr;
		GUIGridView* m_mapInfoGridView = nullptr;
		GUIStaticImage* m_reward_img[MAX_PIXEL_GUNHALL_REWARD_NUM];
		GUIStaticText* m_reward_num[MAX_PIXEL_GUNHALL_REWARD_NUM];
	};
}

#endif
