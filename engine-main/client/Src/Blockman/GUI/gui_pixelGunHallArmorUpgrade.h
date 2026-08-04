#pragma once
#ifndef __GUI_PIXEL_GUNHALL_ARMOR_UPGRADE_HEADER__
#define __GUI_PIXEL_GUNHALL_ARMOR_UPGRADE_HEADER__

#include "gui_layout.h"
#include "UI/GUIWindow.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIProgressBar.h"
#include "UI/GUIButton.h"
#include "GUI/RootGuiLayout.h"
#include "gui_layout.h"
#include "UI/GUIListBox.h"
#include <vector>

using namespace LORD;

namespace BLOCKMAN
{
	class gui_pixelGunHallArmorUpgrade : public  gui_layout
	{
	private:
		SubscriptionGuard m_subscriptionGuard;

	public:
		gui_pixelGunHallArmorUpgrade();
		virtual ~gui_pixelGunHallArmorUpgrade();
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		void onShow() override;


		bool refreshAll(const String& data);
		bool getAllInfo(const String & data, String& armorImg, String& priceImg, int& price, int& priceId,
			int& time, int& level, int& value, int& baseArmor, int& maxArmor, int& curArmor, int& maxHealth, int& maxLevel);

		bool onCloseClick(const EventArgs & events);
		bool onBtnArmorUp(const EventArgs);

		RootGuiLayout* getParent() { return dynamic_cast<RootGuiLayout*>(m_parentLayout); }

	private:

		enum  ArmorOperateRequest
		{
			ARMOR_UP = 0,
			ARMOR_SPEEDUP = 1,
		};

		int m_armor_request = ArmorOperateRequest::ARMOR_UP;

		GUIStaticText* m_time_text = nullptr;
		GUIStaticText* m_up_value = nullptr;
		GUIStaticText* m_price_num = nullptr;
		GUIStaticText* m_armor_level = nullptr;
		GUIStaticText* m_armor_Upgrade_text = nullptr;
		GUIStaticText* m_armor_describe = nullptr;
		GUIStaticText* m_armor_text = nullptr;
		GUIButton* m_armor_updage_btn = nullptr;
		GUIButton* m_closeBtn = nullptr;
		GUIStaticImage* m_priceImg = nullptr;
		GUILayout* m_time_layout = nullptr;
		GUIStaticImage* m_armorImg = nullptr;

		GUIStaticText* m_upgrade_armor_value = nullptr;
		GUIButton* m_max_armor_value = nullptr;
		GUILayout* m_text_layout = nullptr;
		GUIProgressBar* m_armorUp_bar = nullptr;

		int m_need_money = -1;
		int m_printId = 0;
		int m_armorTime = 0;

	};
}

#endif
