#include "gui_seasonReward.h"
#include "GUI/GUISeasonHonorItem.h"
#include "GUI/GUISeasonRewardItem.h"
#include "GUI/GuiDef.h"
#include "Util/LanguageKey.h"
#include "Util/ClientEvents.h"
#include "Setting/SeasonSetting.h"

namespace BLOCKMAN
{
	gui_seasonReward::gui_seasonReward() :
		gui_layout("SeasonReward.json")
	{
	}

	gui_seasonReward::~gui_seasonReward()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_seasonReward::onLoad()
	{
		getWindow("SeasonReward-Title")->SetText(getString(LanguageKey::GUI_SEASON_REWARD_TITLE));
		getWindow("SeasonReward-Rank-Text")->SetText("");
		getWindow("SeasonReward-Bottom-Hint")->SetText(getString(LanguageKey::GUI_SEASON_REWARD_BOTTOM_HINT));

		auto BtnClose = getWindow("SeasonReward-Close");
		BtnClose->subscribeEvent(EventButtonClick, std::bind(&gui_seasonReward::onCloseClick, this, std::placeholders::_1));

		m_subscriptionGuard.add(UpdateLastSeasonInfoEvent::subscribe(std::bind(&gui_seasonReward::onUpdateLastSeason, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		m_subscriptionGuard.add(UpdateCurrentSeasonInfoEvent::subscribe(std::bind(&gui_seasonReward::onUpdateCurrentSeason, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)));
	}

	void gui_seasonReward::onUpdate(ui32 nTimeElapse)
	{

	}

	bool gui_seasonReward::onCloseClick(const EventArgs & events)
	{
		playSoundByType(ST_ClosePanel);
		getParent()->showMainControl();
		return true;
	}

	bool gui_seasonReward::onUpdateLastSeason(int honorId, int rank, int honor)
	{
		auto CurrentHonor = getWindow<GUISeasonHonorItem>("SeasonReward-Left-Honor");
		CurrentHonor->SetHonorInfo(HonorItemType::LAST_SEASON, honorId, rank, honor);

		auto Rank = getWindow("SeasonReward-Rank-Text");
		Rank->SetText(StringUtil::Format(getString(LanguageKey::GUI_SEASON_REWARD_RANK).c_str(), rank).c_str());
		
		auto RewardInfo = getWindow("SeasonReward-Rewards");
		RewardInfo->CleanupChildren();
		list<int>::type rewardIds;
		Season* season = SeasonSetting::findSeasonByHonorId((SeasonHonorId)honorId);
		Season* next_season = nullptr;
		if ((SeasonHonorId)(honorId) == SeasonHonorId::KING)
		{
			next_season = season;
		}
		else
		{
			next_season = SeasonSetting::findSeasonByHonorId((SeasonHonorId)(honorId + 1));
		}
		int seasonId = 0;
		bool isUp = false;
		if (season != nullptr)
		{
			seasonId = season->Id;
			if (next_season == nullptr)
			{
				rewardIds = season->NormalRewardIds;
			}
			else
			{
				isUp = honor >= next_season->Score;
				rewardIds = isUp ? season->UpRewardIds : season->NormalRewardIds;
			}
		}
			
		static unsigned RewardIndex = 0;
		float ItemWidth = 135.0f;
		float ItemHeight = 45.0f;
		float PositionX = 0;
		
		SeasonRankReward* rank_reward = SeasonSetting::findSeasonRankRewardBySeasonIdAndRank(seasonId, rank);
		if (rank_reward && isUp)
		{
			RewardIndex++;
			GUIString ChildName = StringUtil::Format("SeasonReward-Reward-Item-%d", RewardIndex).c_str();
			GUISeasonRewardItem* RewardItem = (GUISeasonRewardItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_SEASON_REWARD_ITEM, ChildName);
			RewardItem->SetArea({ 0.0f, PositionX }, { 0.0f, 1.0f }, { 0.0f, ItemWidth }, { 0.0f, ItemHeight });
			RewardItem->SetSeasonRankReward(rank_reward, true);
			RewardInfo->AddChildWindow(RewardItem);
			PositionX += ItemWidth;
		}

		for (auto rewardId : rewardIds)
		{
			SeasonReward* reward = SeasonSetting::findSeasonRewardById(rewardId);
			if (reward != nullptr)
			{
				RewardIndex++;
				GUIString ChildName = StringUtil::Format("SeasonReward-Reward-Item-%d", RewardIndex).c_str();
				GUISeasonRewardItem* RewardItem = (GUISeasonRewardItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_SEASON_REWARD_ITEM, ChildName);
				RewardItem->SetArea({ 0.0f, PositionX }, { 0.0f, 1.0f }, { 0.0f, ItemWidth }, { 0.0f, ItemHeight });
				RewardItem->SetSeasonReward(reward, true);
				RewardInfo->AddChildWindow(RewardItem);
				PositionX += ItemWidth;
			}
		}

		return true;
	}

	bool gui_seasonReward::onUpdateCurrentSeason(int honorId, int rank, int honor, int endTime)
	{
		auto CurrentHonor = getWindow<GUISeasonHonorItem>("SeasonReward-Right-Honor");
		CurrentHonor->SetHonorInfo(HonorItemType::CURRENT_SEASON, honorId, rank, honor);
		return true;
	}

}
