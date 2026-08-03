#include "gui_gameOtherSetting.h"
#include "Util/LanguageKey.h"
#include <functional>
#include "cWorld/Blockman.h"
#include "World/GameSettings.h"
#include "Audio/SoundSystem.h"
#include "Core.h"
#include "Util/PathUtil.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include "Object/Root.h"
#include "UI/GUIStaticText.h"

#include "game.h"

using namespace LORD;

namespace BLOCKMAN {

	gui_gameOtherSetting::gui_gameOtherSetting() : gui_layout("GameOtherSetting.json")
	{ }

	gui_gameOtherSetting::~gui_gameOtherSetting()
	{ }

	void gui_gameOtherSetting::onLoad()
	{
		getWindow("GameOtherSetting-Content-Function-Luminance-Name")->SetText(getString(LanguageKey::GUI_SETTING_LUMINANCE));
		getWindow("GameOtherSetting-Content-Function-Horizon-Name")->SetText(getString(LanguageKey::GUI_SETTING_HORIZON));
		getWindow("GameOtherSetting-Content-Function-Volume-Name")->SetText(getString(LanguageKey::GUI_SETTING_VOLUME));
		getWindow("GameOtherSetting-Content-Function-BackgroundMusic-Name")->SetText(getString(LanguageKey::GUI_GAME_SETTING_BACKGROUND_MUSIC));
		getWindow("GameOtherSetting-Content-Function-FPS-Name")->SetText(getString(LanguageKey::GUI_SETTING_FPS));

		m_sliderLuminance = getWindow<GUISlider>("GameOtherSetting-Content-Function-Luminance-Value");
		m_sliderHorizon = getWindow<GUISlider>("GameOtherSetting-Content-Function-Horizon-Value");
		m_sliderVolume = getWindow<GUISlider>("GameOtherSetting-Content-Function-Volume-Value");
		m_checkBoxBackgroundMusic = getWindow<GUICheckBox>("GameOtherSetting-Content-Function-BackgroundMusic-State");
		m_sliderFPS = getWindow<GUISlider>("GameOtherSetting-Content-Function-FPS-Value");

		m_sliderFPS->subscribeEvent(EventWindowTouchDown, std::bind(&gui_gameOtherSetting::onClick, this, std::placeholders::_1, LayoutId::FPS));

		m_sliderLuminance->subscribeEvent(EventWindowTouchUp, std::bind(&gui_gameOtherSetting::onClick, this, std::placeholders::_1, LayoutId::LUMINANCE));
		m_sliderHorizon->subscribeEvent(EventWindowTouchUp, std::bind(&gui_gameOtherSetting::onClick, this, std::placeholders::_1, LayoutId::HORIZON));
		m_sliderVolume->subscribeEvent(EventWindowTouchUp, std::bind(&gui_gameOtherSetting::onClick, this, std::placeholders::_1, LayoutId::VOLUME));
		m_sliderFPS->subscribeEvent(EventWindowTouchUp, std::bind(&gui_gameOtherSetting::onClick, this, std::placeholders::_1, LayoutId::FPS));

		m_sliderLuminance->subscribeEvent(EventMotionRelease, std::bind(&gui_gameOtherSetting::onClick, this, std::placeholders::_1, LayoutId::LUMINANCE));
		m_sliderHorizon->subscribeEvent(EventMotionRelease, std::bind(&gui_gameOtherSetting::onClick, this, std::placeholders::_1, LayoutId::HORIZON));
		m_sliderVolume->subscribeEvent(EventMotionRelease, std::bind(&gui_gameOtherSetting::onClick, this, std::placeholders::_1, LayoutId::VOLUME));
		m_sliderFPS->subscribeEvent(EventMotionRelease, std::bind(&gui_gameOtherSetting::onClick, this, std::placeholders::_1, LayoutId::FPS));

		m_sliderLuminance->subscribeEvent(EventWindowTouchMove, std::bind(&gui_gameOtherSetting::onClick, this, std::placeholders::_1, LayoutId::LUMINANCE));
		m_sliderHorizon->subscribeEvent(EventWindowTouchMove, std::bind(&gui_gameOtherSetting::onClick, this, std::placeholders::_1, LayoutId::HORIZON));
		m_sliderVolume->subscribeEvent(EventWindowTouchMove, std::bind(&gui_gameOtherSetting::onClick, this, std::placeholders::_1, LayoutId::VOLUME));
		m_sliderFPS->subscribeEvent(EventWindowTouchMove, std::bind(&gui_gameOtherSetting::onClick, this, std::placeholders::_1, LayoutId::FPS));

		m_checkBoxBackgroundMusic->subscribeEvent(EventCheckStateChanged, std::bind(&gui_gameOtherSetting::onCheckBoxStateChange, this, std::placeholders::_1, LayoutId::BACKGROUND_MUSIC));
		
		loadSettingConfig();
	}

	void gui_gameOtherSetting::onUpdate(ui32 nTimeElapse)
	{ }

	void gui_gameOtherSetting::setVolume()
	{
		if (!m_sliderVolume)
			return;

		float bgVolume = SoundSystem::Instance()->getEffectsVolume();
		m_sliderVolume->SetProgress(bgVolume);
	}

	void gui_gameOtherSetting::setLuminance()
	{
		if (!m_sliderLuminance)
			return;

		float gammaVolue = Blockman::Instance()->m_gameSettings->gammaSetting;
		m_sliderLuminance->SetProgress(gammaVolue);
	}

	void gui_gameOtherSetting::setHorizon()
	{
		if (!m_sliderHorizon)
			return;

		float fovSetting = Blockman::Instance()->m_gameSettings->getFovSetting();
		m_sliderHorizon->SetProgress(fovSetting);
	}

	void gui_gameOtherSetting::setBackgroundMusicSwitch()
	{
		if (!m_checkBoxBackgroundMusic)
			return;

		bool isOn = SoundSystem::Instance()->isSwitchOnBackgroundMusic();
		m_checkBoxBackgroundMusic->SetCheckedNoEvent(isOn);
	}

	void gui_gameOtherSetting::setFPS()
	{
		if (!m_sliderFPS)
			return;

		float fps = Blockman::Instance()->m_gameSettings->m_fpsSetting;
		m_sliderFPS->SetProgress(fps);
	}

	void gui_gameOtherSetting::refreshVolume()
	{
		if (!m_sliderVolume)
			return;

		if (m_sliderVolume->GetProgress() < 0.1)
		{
			m_sliderVolume->SetProgress(0);
		}
		else if (m_sliderVolume->GetProgress() > 0.9)
		{
			m_sliderVolume->SetProgress(1.0);
		}

		SoundSystem::Instance()->setBackgroundMusicVolume(m_sliderVolume->GetProgress());
		SoundSystem::Instance()->setEffectsVolume(m_sliderVolume->GetProgress());

		m_settingMap["volume"] = m_sliderVolume->GetProgress();
		saveSettingConfig();
	}

	void gui_gameOtherSetting::refreshLuminance()
	{
		if (!m_sliderLuminance)
			return;

		if (m_sliderLuminance->GetProgress() < 0.1)
		{
			m_sliderLuminance->SetProgress(0);
		}
		else if (m_sliderLuminance->GetProgress() > 0.9)
		{
			m_sliderLuminance->SetProgress(1.0);
		}

		Blockman::Instance()->m_gameSettings->gammaSetting = m_sliderLuminance->GetProgress();
		
		m_settingMap["luminance"] = m_sliderLuminance->GetProgress();
		saveSettingConfig();
	}

	void gui_gameOtherSetting::refreshHorizon()
	{
		if (!m_sliderHorizon)
			return;

		if (m_sliderHorizon->GetProgress() < 0.1)
		{
			m_sliderHorizon->SetProgress(0);
		}
		else if (m_sliderHorizon->GetProgress() > 0.9)
		{
			m_sliderHorizon->SetProgress(1.0);
		}

		Blockman::Instance()->m_gameSettings->setFovSetting(m_sliderHorizon->GetProgress());
		
		m_settingMap["horizon"] = m_sliderHorizon->GetProgress();
		saveSettingConfig();
	}

	void gui_gameOtherSetting::refreshBackgroundMusicSwitch(int isChecked)
	{
		SoundSystem::Instance()->setBackgroundMusicSwitch(isChecked == 1);

		m_settingMap["backgroundMusic"] = (float) isChecked;
		saveSettingConfig();
	}

	void gui_gameOtherSetting::refreshFPS()
	{
		if (!m_sliderFPS)
			return;

		unsigned int maxFPS;
		if (m_sliderFPS->GetProgress() < 0.33f)
		{
			m_sliderFPS->SetProgress(0.0f);
			maxFPS = 30;
		}
		else if (m_sliderFPS->GetProgress() > 0.66f)
		{
			m_sliderFPS->SetProgress(1.0f);
			maxFPS = 60;
		}
		else
		{
			m_sliderFPS->SetProgress(0.5f);
			maxFPS = 45;
		}

		Blockman::Instance()->m_gameSettings->m_fpsSetting = m_sliderFPS->GetProgress();
		GameClient::CGame::Instance()->SetMaxFps(maxFPS);

		m_settingMap["fps"] = m_sliderFPS->GetProgress();
		saveSettingConfig();
	}

	void gui_gameOtherSetting::checkBackgroundMusic()
	{
		if (m_settingMap.find("backgroundMusic") != m_settingMap.end())
		{
			float isChecked = m_settingMap["backgroundMusic"];
			refreshBackgroundMusicSwitch(isChecked > 0.0f);
		}
	}

	void gui_gameOtherSetting::onShow()
	{
		setVolume();
		setLuminance();
		setHorizon();
		setBackgroundMusicSwitch();
		setFPS();
	}

	bool gui_gameOtherSetting::onClick(const EventArgs & events, LayoutId layoutId)
	{
		switch (layoutId)
		{
			case LayoutId::LUMINANCE:
				refreshLuminance();
				break;
			case LayoutId::HORIZON:
				refreshHorizon();
				break;
			case LayoutId::VOLUME:
				refreshVolume();
				break;
			case LayoutId::FPS:
				refreshFPS();
				break;
		}

		return true;
	}

	bool gui_gameOtherSetting::onCheckBoxStateChange(const EventArgs& events, LayoutId layoutId)
	{
		auto windowEvents = dynamic_cast<const WindowEventArgs&>(events);
		auto checkBox = dynamic_cast<GUICheckBox*>(windowEvents.window);
		if (!checkBox)
			return false;

		int isChecked = checkBox->GetChecked() ? 1 : 0;
		switch (layoutId)
		{
			case LayoutId::BACKGROUND_MUSIC:
				refreshBackgroundMusicSwitch(isChecked);
				break;
		}

		return true;
	}

	void gui_gameOtherSetting::loadSettingConfig()
	{

		String configPath = PathUtil::ConcatPath(Root::Instance()->getWriteablePath(), "va_BlockMan.cfg");
		if (!PathUtil::IsFileExist(configPath))
		{
			LordLogInfo("Config file for Video/Audio not found [%s].", configPath.c_str());
			return;
		}

		std::ifstream fin(configPath.c_str(), std::ios::in);
		char line[1024];
		while (fin.getline(line, sizeof(line)))
		{
			String lineString = line;
			if (lineString.size() > 0 && lineString.at(0) != '#' && lineString.at(0) != '@')
			{
				StringArray array = StringUtil::Split(lineString, "=", 1);
				if (array.size() == 2)
				{
					m_settingMap[array[0]] = (float)atof(array[1].c_str());
				}
			}
		}

		fin.clear();
		fin.close();

		if (m_settingMap.find("horizon") != m_settingMap.end())
		{
			Blockman::Instance()->m_gameSettings->setFovSetting(m_settingMap["horizon"]);
		}

		if (m_settingMap.find("luminance") != m_settingMap.end())
		{
			Blockman::Instance()->m_gameSettings->gammaSetting = m_settingMap["luminance"];
		}

		if (m_settingMap.find("volume") != m_settingMap.end())
		{
			SoundSystem::Instance()->setBackgroundMusicVolume(m_settingMap["volume"]);
			SoundSystem::Instance()->setEffectsVolume(m_settingMap["volume"]);
		}

		if (m_settingMap.find("backgroundMusic") != m_settingMap.end())
		{
			SoundSystem::Instance()->setBackgroundMusicSwitch(m_settingMap["backgroundMusic"] > 0);
		}

		if (m_settingMap.find("fps") != m_settingMap.end())
		{
			unsigned int maxFPS;
			if (m_settingMap["fps"] < 0.33f)
			{
				m_settingMap["fps"] = 0.0f;
				maxFPS = 30;
			}
			else if (m_settingMap["fps"] > 0.67f)
			{
				m_settingMap["fps"] = 1.0f;
				maxFPS = 60;
			}
			else
			{
				m_settingMap["fps"] = 0.5f;
				maxFPS = 45;
			}

			Blockman::Instance()->m_gameSettings->m_fpsSetting = m_settingMap["fps"];
			GameClient::CGame::Instance()->SetMaxFps(maxFPS);
		}
	}

	void gui_gameOtherSetting::saveSettingConfig()
	{
		String configPath = PathUtil::ConcatPath(Root::Instance()->getWriteablePath(), "va_BlockMan.cfg");
		if (!PathUtil::IsFileExist(configPath))
		{
			LordLogInfo("configPath file not found [%s].", configPath.c_str());
			m_settingMap["horizon"] = Blockman::Instance()->m_gameSettings->getFovSetting();
			m_settingMap["volume"] = SoundSystem::Instance()->getBackgroundMusicVolume();
			m_settingMap["luminance"] = Blockman::Instance()->m_gameSettings->gammaSetting;
			m_settingMap["backgroundMusic"] = SoundSystem::Instance()->isSwitchOnBackgroundMusic() ? 1.0f : 0;
			m_settingMap["fps"] = Blockman::Instance()->m_gameSettings->m_fpsSetting;
		}

		std::ofstream out(configPath.c_str());
		if (out.is_open())
		{
			for (auto value : m_settingMap)
			{
				String str = StringUtil::Format("%s=%.2f\n", value.first.c_str(), value.second);
				out << str.c_str();
			}
			out.close();
		}
	}

}
