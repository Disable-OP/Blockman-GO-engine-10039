#include "GUISeasonRankPanel.h"
#include "UI/GUIWindowManager.h"
#include "UI/GUILayout.h"
#include "GUI/GUISeasonRankItem.h"
#include "GUI/GUISeasonRewardItem.h"
#include "GUI/GuiDef.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"
#include "Setting/GameRuleSetting.h"
#include "Network/ClientHttpRequest.h"

namespace BLOCKMAN
{
	GUISeasonRankPanel::GUISeasonRankPanel(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("SeasonRankPanel.json"));
		renameComponents(m_window);
		initView();
		AddChildWindow(m_window);
		SetTouchable(false);
	}

	void GUISeasonRankPanel::removeComponents()
	{
		if (m_window)
		{
			RemoveChildWindow(m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	void GUISeasonRankPanel::initView()
	{
		m_reward_text = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(0)->GetChildByIndex(0));
		m_reward_info = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(0)->GetChildByIndex(1));
		m_honor_text = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(1)->GetChildByIndex(0));
		m_honor_value = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(1)->GetChildByIndex(0)->GetChildByIndex(0)->GetChildByIndex(0));
		auto RankLayout = m_window->GetChildByIndex(2);
		static unsigned RankIndex = 0;
		RankIndex++;
		GUIString RankName = StringUtil::Format("GUISeasonRankPanel-Rank-%d", RankIndex).c_str();
		m_rank_list = (GUIListBox*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_LIST, RankName);
		m_rank_list->SetArea(UDim(0, 0), UDim(0, 0), UDim(1.f, 0), UDim(1.f, 0));
		m_rank_list->SetInterval(4.0f);
		m_rank_list->SetAllowScroll(true);
		RankLayout->AddChildWindow(m_rank_list);
	}

	void GUISeasonRankPanel::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GUISeasonRankPanel::InitUI(SeasonHonorId honorId)
	{
		m_reward_text->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_SEASON_RANK_REWARD).c_str());
		m_honor_text->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_SEASON_RANK_HONOR).c_str());
		Season* season = SeasonSetting::findSeasonByHonorId(honorId);
		if (season)
			m_honor_value->SetText(StringUtil::ToString(season->Score).c_str());
		else
			m_honor_value->SetText("0");
	}

	void GUISeasonRankPanel::SetSeasonHonorId(SeasonHonorId honorId)
	{
		m_honor_id = honorId;
	}

	void GUISeasonRankPanel::SetSeasonRankType(SeasonRankType type)
	{
		if (!GameRuleSetting::getGameRule()->isHasSeason)
		{
			return;
		}

		m_type = type;
		if (m_honor_id == SeasonHonorId::KING && m_type == TYPE_RIGHT)
		{
			ClientHttpRequest::loadSeasonRank((int)m_type, 1, 1, 29);
		}
		else
		{
			ClientHttpRequest::loadSeasonRank((int)m_type, (int)m_type, 0, 29);
		}

		m_reward_info->CleanupChildren();
		list<int>::type rewardIds;
		Season* season = SeasonSetting::findSeasonByHonorId(m_honor_id);
		if (season != nullptr)
		{
			if (m_type == TYPE_LEFT)
			{
				rewardIds = season->UpRewardIds;
			}
			else
			{
				rewardIds = season->NormalRewardIds;
			}
			static unsigned RewardIndex = 0;
			float ItemWidth = 81.0f;
			float ItemHeight = 27.0f;
			float PositionX = 0;
			for (auto rewardId : rewardIds)
			{
				SeasonReward* reward = SeasonSetting::findSeasonRewardById(rewardId);
				if (reward != nullptr)
				{
					RewardIndex++;
					GUIString ChildName = StringUtil::Format("SeasonRankPanel-Reward-Item-%d", RewardIndex).c_str();
					GUISeasonRewardItem* RewardItem = (GUISeasonRewardItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_SEASON_REWARD_ITEM, ChildName);
					RewardItem->SetArea({ 0.0f, PositionX }, { 0.0f, 1.0f }, { 0.0f, ItemWidth }, { 0.0f, ItemHeight });
					RewardItem->SetSeasonReward(reward, false);
					m_reward_info->AddChildWindow(RewardItem);
					PositionX += ItemWidth;
				}
			}
		}
	}

	void GUISeasonRankPanel::SetSeasonRankData(list<SeasonRankItem>::type ranks)
	{
		if (m_rank_list->getContainerWindow() != nullptr)
			return;

		int seasionId = 0;
		Season* season = SeasonSetting::findSeasonByHonorId(m_honor_id);
		if (season)
			seasionId = season->Id;

		m_rank_list->ClearAllItem();
		static unsigned RankIndex = 0;
		for (auto rank : ranks)
		{
			RankIndex++;
			GUIString ChildName = StringUtil::Format("GUISeasonRankPanel-Rank-Item-%d", RankIndex).c_str();
			GUISeasonRankItem* RankItem = (GUISeasonRankItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_SEASON_RANK_ITEM, ChildName);
			RankItem->SetArea({ 0.0f, 0.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 30.0f });
			RankItem->SetSeasonRankItem(seasionId, rank);
			m_rank_list->AddItem(RankItem, false);
		}
	}

}
