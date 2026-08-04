#pragma once
#ifndef __GUI_GAME_SETTING_HEADER__
#define __GUI_GAME_SETTING_HEADER__

#include "Core.h"
#include "gui_layout.h"
#include "UI/GUIWindow.h"
#include "UI/GUILayout.h"
#include "UI/GUISlider.h"
#include "UI/GUICheckBox.h"
#include "GUI/gui_menu.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_gameSetting : public  gui_layout
	{
	private:
		enum class LayoutId
		{
			GUI_SIZE,
			CAMERA_SENSITIVE,
			JUMP_SNEAK_STATE,
			POLE_STATE
		};

	public:
		gui_gameSetting();
		virtual ~gui_gameSetting();
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		void onShow() override;
		gui_menu* getParent()
		{
			return dynamic_cast<gui_menu*>(m_parentLayout);
		}
		
	private:
		bool onClick(const EventArgs& events, LayoutId layoutId);
		bool onCheckBoxStateChange(const EventArgs& events, LayoutId layoutId);
		void loadSettingConfig();
		void saveSettingConfig();
		void setGuiSize();
		void setCameraSensitive();
		void setJumpSneakState();
		void setPoleControlState();

		void refreshGuiSize();
		void refreshCameraSensitive();
		void refreshJumpSneakState(int checked);
		void refreshPoleControlState(int checked);

	private:
		const float GUI_MIN_SIZE = 0.5F;
		const float GUI_MAX_SIZE = 1.0F;
		GUISlider* m_sliderGuiSize = nullptr;
		GUISlider* m_sliderCameraSensitive = nullptr;
		GUICheckBox* m_checkboxJumpSneakState = nullptr;
		GUICheckBox* m_checkboxPoleControlState = nullptr;
		map<String, float>::type m_settingMap;
	};
}

#endif
