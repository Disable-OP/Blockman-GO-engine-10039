#pragma once
#ifndef __GUI_RANCH_ACHIEVEMENT_ITEM_HEADER__
#define __GUI_RANCH_ACHIEVEMENT_ITEM_HEADER__
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIProgressBar.h"
#include "UI/GUILayout.h"
#include "UI/GUIListHorizontalBox.h"
#include "UI/GUIButton.h"
#include "World/Ranch.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GuiRanchAchievementItem : public CustomGuiWindow
	{
	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;
		GUIStaticImage* m_icon = nullptr;
		GUIStaticText* m_tvName = nullptr;
		GUIStaticText* m_tvDesc = nullptr;
		GUIProgressBar* m_pbExp = nullptr;
		GUIStaticText* m_tvExpValue = nullptr;
		
		GUIListHorizontalBox* m_lvLevels = nullptr;
		GUIStaticText* m_tvAchievementValue = nullptr;
		GUIStaticText* m_tvPlayerExpValue = nullptr;
		GUIButton* m_btnReceive = nullptr;
		GUIStaticText* m_tvNotReach = nullptr;
		GUIStaticText* m_tvMaxLevel = nullptr;
		GUIStaticImage* m_ivRedDot = nullptr;
		GUILayout* m_rewardLayout = nullptr;

		RanchAchievement m_achievement;


	private:
		void removeComponents();

	public:
		GuiRanchAchievementItem(const GUIWindowType& nType, const GUIString& strName);
		~GuiRanchAchievementItem() = default;
		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;
		void setAchievement(const RanchAchievement&  achievement);

	private:
		bool onClick(const EventArgs& args);

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;
	};

}
#endif
