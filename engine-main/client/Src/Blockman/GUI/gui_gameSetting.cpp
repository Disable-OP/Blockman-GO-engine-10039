#include "gui_gameSetting.h"
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

	gui_gameSetting::gui_gameSetting() : gui_layout("GameSetting.json")
	{ }

	gui_gameSetting::~gui_gameSetting()
	{ }

	void gui_gameSetting::onLoad()
	{
		getWindow("GameSetting-Content-Function-Gui-Size-Name")->SetText(getString(LanguageKey::GUI_SETTING_GUI_SIZE));
		getWindow("GameSetting-Content-Function-Camera-Sensitive-Name")->SetText(getString(LanguageKey::GUI_SETTING_CAMERA_SENSITIVE));
		getWindow("GameSetting-Content-Function-Toggle-Jump-Sneak-Pos-Name")->SetText(getString(LanguageKey::GUI_SETTING_JUMP_SNEAK_TOGGLE));
		getWindow("GameSetting-Content-Function-Toggle-Pole-Name")->SetText(getString(LanguageKey::GUI_SETTING_POLE_STATE_TOGGLE));

		m_sliderGuiSize = getWindow<GUISlider>("GameSetting-Content-Function-Gui-Size-Value");
		m_sliderCameraSensitive = getWindow<GUISlider>("GameSetting-Content-Function-Camera-Sensitive-Value");
		m_checkboxJumpSneakState = getWindow<GUICheckBox>("GameSetting-Content-Function-Toggle-Jump-Sneak-Pos-State");
		m_checkboxPoleControlState = getWindow<GUICheckBox>("GameSetting-Content-Function-Toggle-Pole-State");

		m_sliderGuiSize->subscribeEvent(EventWindowTouchUp, std::bind(&gui_gameSetting::onClick, this, std::placeholders::_1, LayoutId::GUI_SIZE));
		m_sliderCameraSensitive->subscribeEvent(EventWindowTouchUp, std::bind(&gui_gameSetting::onClick, this, std::placeholders::_1, LayoutId::CAMERA_SENSITIVE));

		m_sliderGuiSize->subscribeEvent(EventMotionRelease, std::bind(&gui_gameSetting::onClick, this, std::placeholders::_1, LayoutId::GUI_SIZE));
		m_sliderCameraSensitive->subscribeEvent(EventMotionRelease, std::bind(&gui_gameSetting::onClick, this, std::placeholders::_1, LayoutId::CAMERA_SENSITIVE));

		m_sliderGuiSize->subscribeEvent(EventWindowTouchMove, std::bind(&gui_gameSetting::onClick, this, std::placeholders::_1, LayoutId::GUI_SIZE));
		m_sliderCameraSensitive->subscribeEvent(EventWindowTouchMove, std::bind(&gui_gameSetting::onClick, this, std::placeholders::_1, LayoutId::CAMERA_SENSITIVE));

		m_checkboxJumpSneakState->subscribeEvent(EventCheckStateChanged, std::bind(&gui_gameSetting::onCheckBoxStateChange, this, std::placeholders::_1, LayoutId::JUMP_SNEAK_STATE));
		m_checkboxPoleControlState->subscribeEvent(EventCheckStateChanged, std::bind(&gui_gameSetting::onCheckBoxStateChange, this, std::placeholders::_1, LayoutId::POLE_STATE));
		
		loadSettingConfig();
	}

	void gui_gameSetting::onUpdate(ui32 nTimeElapse)
	{ }

	void gui_gameSetting::setGuiSize()
	{
		if (!m_sliderGuiSize)
			return;

		float guiSize = (float)Blockman::Instance()->m_gameSettings->playerActivityGuiSize;
		guiSize = Math::Min(guiSize, GUI_MAX_SIZE);
		guiSize = Math::Max(guiSize, GUI_MIN_SIZE);
		float progress = (guiSize - GUI_MIN_SIZE) / (GUI_MAX_SIZE - GUI_MIN_SIZE);
		m_sliderGuiSize->SetProgress(progress);
	}

	void gui_gameSetting::setCameraSensitive()
	{
		if (!m_sliderCameraSensitive)
			return;

		float cameraSensitive = Blockman::Instance()->m_gameSettings->getCameraSensitive();
		float minSize = 0.5;
		float maxSize = 4.5;
		float progress = (cameraSensitive - minSize) / (maxSize - minSize);
		m_sliderCameraSensitive->SetProgress(progress);
	}

	void gui_gameSetting::setJumpSneakState()
	{
		if (!m_checkboxJumpSneakState)
			return;

		bool isJumpSneakDefault = Blockman::Instance()->m_gameSettings->isJumpSneakDefault > 0;
		m_checkboxJumpSneakState->SetCheckedNoEvent(isJumpSneakDefault);
	}

	void gui_gameSetting::setPoleControlState()
	{
		if (!m_checkboxPoleControlState)
			return;

		bool usePole = Blockman::Instance()->m_gameSettings->usePole > 0;
		m_checkboxPoleControlState->SetCheckedNoEvent(usePole);
	}

	void gui_gameSetting::refreshGuiSize()
	{
		if (!m_sliderGuiSize)
			return;

		if (m_sliderGuiSize->GetProgress() < 0.1)
		{
			m_sliderGuiSize->SetProgress(0);
		}
		else if (m_sliderGuiSize->GetProgress() > 0.9)
		{
			m_sliderGuiSize->SetProgress(1.0);
		}

		float size = GUI_MIN_SIZE + (GUI_MAX_SIZE - GUI_MIN_SIZE) * m_sliderGuiSize->GetProgress();
		Blockman::Instance()->m_gameSettings->playerActivityGuiSize = size;
		
		m_settingMap["gui_size"] = (float)size;
		saveSettingConfig();
	}

	void gui_gameSetting::refreshCameraSensitive()
	{
		if (!m_sliderCameraSensitive)
			return;

		if (m_sliderCameraSensitive->GetProgress() < 0.1)
		{
			m_sliderCameraSensitive->SetProgress(0);
		}
		else if (m_sliderCameraSensitive->GetProgress() > 0.9)
		{
			m_sliderCameraSensitive->SetProgress(1.0);
		}
		
		float minSize = 0.5;
		float maxSize = 4.5;
		float size = minSize + (maxSize - minSize) * m_sliderCameraSensitive->GetProgress();
		Blockman::Instance()->m_gameSettings->setCameraSensitive(size);
		
		m_settingMap["camera_sensitive"] = size;
		saveSettingConfig();
	}

	void gui_gameSetting::refreshJumpSneakState(int isChecked)
	{
		Blockman::Instance()->m_gameSettings->isJumpSneakDefault = isChecked;

		m_settingMap["isJumpDefault"] = (float)isChecked;
		saveSettingConfig();
	}

	void gui_gameSetting::refreshPoleControlState(int isChecked)
	{
		Blockman::Instance()->m_gameSettings->usePole = isChecked;

		m_settingMap["usePole"] = (float)isChecked;
		saveSettingConfig();
	}

	void gui_gameSetting::onShow()
	{
		setGuiSize();
		setCameraSensitive();
		setJumpSneakState();
		setPoleControlState();
	}

	bool gui_gameSetting::onClick(const EventArgs & events, LayoutId layoutId)
	{
		switch (layoutId)
		{
			case LayoutId::GUI_SIZE:
				refreshGuiSize();
				break;
			case LayoutId::CAMERA_SENSITIVE:
				refreshCameraSensitive();
				break;
		}

		return true;
	}

	bool gui_gameSetting::onCheckBoxStateChange(const EventArgs& events, LayoutId layoutId)
	{
		auto windowEvents = dynamic_cast<const WindowEventArgs&>(events);
		auto checkBox = dynamic_cast<GUICheckBox*>(windowEvents.window);
		if (!checkBox)
			return false;

		int isChecked = checkBox->GetChecked() ? 1 : 0;
		switch (layoutId)
		{
			case LayoutId::JUMP_SNEAK_STATE:
				refreshJumpSneakState(isChecked);
				break;
			case LayoutId::POLE_STATE:
				refreshPoleControlState(isChecked);
				break;
		}

		return true;
	}

	void gui_gameSetting::loadSettingConfig()
	{

		String configPath = PathUtil::ConcatPath(Root::Instance()->getWriteablePath(), "c_BlockMan.cfg");
		if (!PathUtil::IsFileExist(configPath))
		{
			LordLogInfo("Config file for Controls not found [%s].", configPath.c_str());
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

		if (m_settingMap.find("gui_size") != m_settingMap.end())
		{
			float fGuiSize = m_settingMap["gui_size"];
			fGuiSize = Math::Max(fGuiSize, GUI_MIN_SIZE);
			fGuiSize = Math::Min(fGuiSize, GUI_MAX_SIZE);
			Blockman::Instance()->m_gameSettings->playerActivityGuiSize = fGuiSize;
		}

		if (m_settingMap.find("camera_sensitive") != m_settingMap.end())
		{
			Blockman::Instance()->m_gameSettings->setCameraSensitive(m_settingMap["camera_sensitive"]);
		}

		if (m_settingMap.find("usePole") != m_settingMap.end())
		{
			Blockman::Instance()->m_gameSettings->usePole = (int)m_settingMap["usePole"];
		}

		if (m_settingMap.find("isJumpDefault") != m_settingMap.end())
		{
			Blockman::Instance()->m_gameSettings->isJumpSneakDefault = (int)m_settingMap["isJumpDefault"];
		}
	}

	void gui_gameSetting::saveSettingConfig()
	{
		String configPath = PathUtil::ConcatPath(Root::Instance()->getWriteablePath(), "c_BlockMan.cfg");
		if (!PathUtil::IsFileExist(configPath))
		{
			LordLogInfo("Config file for Controls not found [%s].", configPath.c_str());
			m_settingMap["gui_size"] = (float)Blockman::Instance()->m_gameSettings->playerActivityGuiSize;
			m_settingMap["camera_sensitive"] = Blockman::Instance()->m_gameSettings->getCameraSensitive();
			m_settingMap["usePole"] = (float)Blockman::Instance()->m_gameSettings->usePole;
			m_settingMap["isJumpDefault"] = (float)Blockman::Instance()->m_gameSettings->isJumpSneakDefault;
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
