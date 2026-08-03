#pragma once

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
	class gui_gameOtherSetting : public  gui_layout
	{
	private:
		enum class LayoutId
		{
			LUMINANCE,
			HORIZON,
			VOLUME,
			BACKGROUND_MUSIC,
			FPS
		};

	public:
		gui_gameOtherSetting();
		virtual ~gui_gameOtherSetting();
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		void onShow() override;
		gui_menu* getParent()
		{
			return dynamic_cast<gui_menu*>(m_parentLayout);
		}
		void checkBackgroundMusic();

	private:
		bool onClick(const EventArgs& events, LayoutId layoutId);
		bool onCheckBoxStateChange(const EventArgs& events, LayoutId layoutId);
		void loadSettingConfig();
		void saveSettingConfig();
		void setVolume();
		void setLuminance();
		void setHorizon();
		void setBackgroundMusicSwitch();
		void setFPS();

		void refreshVolume();
		void refreshLuminance();
		void refreshHorizon();
		void refreshBackgroundMusicSwitch(int checked);
		void refreshFPS();

	private:
		GUISlider* m_sliderLuminance = nullptr;
		GUISlider* m_sliderHorizon = nullptr;
		GUISlider* m_sliderVolume = nullptr;
		GUICheckBox* m_checkBoxBackgroundMusic = nullptr;
		GUISlider* m_sliderFPS = nullptr;
		map<String, float>::type m_settingMap;
	};
}
