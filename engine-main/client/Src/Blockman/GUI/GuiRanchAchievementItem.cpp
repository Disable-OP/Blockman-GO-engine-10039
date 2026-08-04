#include "GuiRanchAchievementItem.h"
#include "UI/GUIWindowManager.h"
#include "Util/ClientEvents.h"
#include "Util/LanguageManager.h"
#include "Network/ClientNetwork.h"
#include "Util/RedDotManager.h"

namespace BLOCKMAN
{

	void GuiRanchAchievementItem::removeComponents()
	{
		if (m_window)
		{
			RemoveWindowFromDrawList(*m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	vector<GUIWindow*>::type GuiRanchAchievementItem::getPrivateChildren()
	{
		return { m_window };
	}

	GuiRanchAchievementItem::GuiRanchAchievementItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("RanchAchievementItem.json"));
		renameComponents(m_window);

		m_icon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0)->GetChildByIndex(0));
		m_tvName = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(1)->GetChildByIndex(0));
		m_pbExp = dynamic_cast<GUIProgressBar*>(m_window->GetChildByIndex(1)->GetChildByIndex(1));
		m_tvExpValue = dynamic_cast<GUIStaticText*>(m_pbExp->GetChildByIndex(0));
		m_tvDesc = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(2));
		m_lvLevels = dynamic_cast<GUIListHorizontalBox*>(m_window->GetChildByIndex(3));

		m_rewardLayout = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(4));
		m_tvAchievementValue = dynamic_cast<GUIStaticText*>(m_rewardLayout->GetChildByIndex(0)->GetChildByIndex(1));
		m_tvPlayerExpValue = dynamic_cast<GUIStaticText*>(m_rewardLayout->GetChildByIndex(0)->GetChildByIndex(3));
		m_btnReceive = dynamic_cast<GUIButton*>(m_rewardLayout->GetChildByIndex(1)->GetChildByIndex(0));
		m_tvNotReach = dynamic_cast<GUIStaticText*>(m_rewardLayout->GetChildByIndex(1)->GetChildByIndex(1));
		m_tvMaxLevel = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(5));
		m_ivRedDot = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(6));

		m_ivRedDot->SetEffectName("reddot.effect");
		m_ivRedDot->SetImage("");

		if (LanguageManager::Instance())
		{
			m_btnReceive->SetText(LanguageManager::Instance()->getString("gui_ranch_order_receive").c_str());
			m_tvNotReach->SetText(LanguageManager::Instance()->getString("gui_ranch_not_reach").c_str());
			m_tvMaxLevel->SetText(LanguageManager::Instance()->getString("gui_ranch_max_level").c_str());
		}

		m_btnReceive->subscribeEvent(EventButtonClick, std::bind(&GuiRanchAchievementItem::onClick, this, std::placeholders::_1));
		AddWindowToDrawList(*m_window);
	}

	void GuiRanchAchievementItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());
		BaseClass::UpdateSelf(nTimeElapse);
		m_icon->SetImage(m_achievement.icon.c_str());
		m_tvExpValue->SetText(StringUtil::Format("%d/%d", m_achievement.exp, m_achievement.nextExp).c_str());
		m_pbExp->SetProgress((float)m_achievement.exp / (float)m_achievement.nextExp);
		m_tvAchievementValue->SetText(StringUtil::ToString(m_achievement.achievement).c_str());
		m_tvPlayerExpValue->SetText(StringUtil::ToString(m_achievement.playerExp).c_str());

		m_pbExp->SetVisible(m_achievement.level < m_achievement.maxLevel);
		m_tvNotReach->SetVisible(m_achievement.exp < m_achievement.nextExp);
		m_btnReceive->SetVisible(m_achievement.exp >= m_achievement.nextExp);
		m_tvMaxLevel->SetVisible(m_achievement.level >= m_achievement.maxLevel);
		m_rewardLayout->SetVisible(m_achievement.level < m_achievement.maxLevel);
		m_btnReceive->SetEnabled(m_achievement.level < m_achievement.maxLevel);

		if (LanguageManager::Instance())
		{
			m_tvName->SetText(LanguageManager::Instance()->getString(m_achievement.name).c_str());
			m_tvDesc->SetText(StringUtil::Format(LanguageManager::Instance()->getString(m_achievement.desc).c_str(), m_achievement.nextExp).c_str());
		}
	}


	void GuiRanchAchievementItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GuiRanchAchievementItem::setAchievement(const RanchAchievement & achievement)
	{
		m_achievement = achievement;
		m_lvLevels->ClearAllItem();
		m_lvLevels->SetInterval(10.f);
		m_ivRedDot->SetVisible(RedDotManager::checkAchievementById(m_achievement.id));
		for (int i = 0; i < m_achievement.maxLevel; i++)
		{
			static unsigned count = 0;
			count++;
			String itemLayout = StringUtil::Format("%s-Levels-Item-%d", GetName().c_str(), count).c_str();
			if (GUIWindowManager::Instance()->GetGUIWindow(itemLayout.c_str()))
			{
				return;
			}
			GUIStaticImage* itemView = (GUIStaticImage*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, itemLayout.c_str());
			itemView->SetTouchable(false);
			itemView->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(0.0f, 38.f), UDim(0.0f, 36.f));
			itemView->SetImage(i < m_achievement.level ? "set:ranch_rank.json image:achievement_level_pre" : "set:ranch_rank.json image:achievement_level_nor");
			m_lvLevels->AddItem(itemView, false);
		}
	}

	bool GuiRanchAchievementItem::onClick(const EventArgs & args)
	{
		if (m_achievement.id > 0 && m_achievement.exp >= m_achievement.nextExp)
		{
			ClientNetwork::Instance()->getSender()->sendReceiveRanchAchievementReward(m_achievement.id);
		}
		return true;
	}


}