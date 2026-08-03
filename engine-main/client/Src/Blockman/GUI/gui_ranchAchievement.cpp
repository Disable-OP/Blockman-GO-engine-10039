#include "gui_ranchAchievement.h"
#include "GUI/GuiRanchAchievementItem.h"
#include "GUI/GuiDef.h"
#include "Util/UICommon.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Util/ClientEvents.h"

using namespace LORD;

namespace BLOCKMAN
{
	gui_ranchAchievement::gui_ranchAchievement()
		: gui_layout("RanchAchievement.json")
	{
	}

	gui_ranchAchievement::~gui_ranchAchievement()
	{

	}

	void gui_ranchAchievement::onLoad()
	{
		m_tvValue = getWindow<GUIStaticText>("RanchAchievement-Info-Value");
		m_lvItems = getWindow<GUIListBox>("RanchAchievement-Items");
		m_subscriptionGuard.add(RanchAchievementChangeEvent::subscribe(std::bind(&gui_ranchAchievement::onRanchAchievementChange, this)));
	}

	void gui_ranchAchievement::onShow()
	{
		if (!UICommon::checkRanchParam() || !m_dataChange)
		{
			return;
		}
		const auto& achievements = Blockman::Instance()->m_pPlayer->m_ranch->getAchievements();
		float offset = achievements.size() > 0 ? m_lvItems->GetScrollOffset() : 0.f;
		m_lvItems->ClearAllItem();
		for (auto& item : achievements)
		{
			static unsigned count = 0;
			count++;
			String itemLayout = StringUtil::Format("RanchAchievement-Items-Item-%d", count).c_str();
			if (GUIWindowManager::Instance()->GetGUIWindow(itemLayout.c_str()))
			{
				return;
			}

			GuiRanchAchievementItem* ranchItemView = (GuiRanchAchievementItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_RANCH_ACHIEVEMENT_ITEM, itemLayout.c_str());
			ranchItemView->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(1.0f, 0.f), UDim(0.0f, 136.f));
			ranchItemView->setAchievement(item);
			m_lvItems->AddItem(ranchItemView, false);
		}
		m_lvItems->SetScrollOffset(offset);
		m_dataChange = false;
	}

	void gui_ranchAchievement::onUpdate(ui32 nTimeElapse)
	{
		if (UICommon::checkRanchParam() && Blockman::Instance()->m_pPlayer->m_ranch->getInfo() && isShown())
		{
			RanchInfo* info = Blockman::Instance()->m_pPlayer->m_ranch->getInfo();
			m_tvValue->SetText(StringUtil::Format(getString("gui_ranch_achievement_value").c_str(), info->achievementPoint).c_str());
		}
	}

	bool gui_ranchAchievement::onClick(const EventArgs & events, ViewId viewId)
	{
		return false;
	}

	bool gui_ranchAchievement::onRanchAchievementChange()
	{
		m_dataChange = true;
		if (isShown())
		{
			onShow();
		}
		return true;
	}
}
