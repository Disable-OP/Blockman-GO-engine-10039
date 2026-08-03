#include "gui_seasonRank.h"
#include "UI/GUILayout.h"
#include "UI/GUIButton.h"
#include "UI/GUIStaticImage.h"
#include "GUI/GUISeasonRankPanel.h"
#include "GUI/GUISeasonRewardItem.h"
#include "GUI/GuiDef.h"
#include "Util/LanguageKey.h"
#include "Util/ClientEvents.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Setting/SeasonSetting.h"
#include "Network/ClientHttpRequest.h"
#include "Network/ClientNetwork.h"

namespace BLOCKMAN
{
	gui_seasonRank::gui_seasonRank() :
		gui_layout("SeasonRank.json")
	{
		m_season_rule = m_season_rule ? m_season_rule : LordNew GUISeasonRule;
	}

	gui_seasonRank::~gui_seasonRank()
	{
		LordSafeDelete(m_season_rule);
		m_subscriptionGuard.unsubscribeAll();
		SeasonSetting::unloadSetting();
	}

	void gui_seasonRank::onLoad()
	{
		SeasonSetting::loadSetting();

		GUILayout* SeasonRank = getWindow<GUILayout>("SeasonRank");
		m_season_rule->attachTo(this, SeasonRank);
		m_season_rule->hide();

		getWindow("SeasonRank-Title-Text")->SetText(getString(LanguageKey::GUI_SEASON_RANK_TITLE));
		getWindow("SeasonRank-Season-Countdown-Text")->SetText("");
		getWindow("SeasonRank-My-Reward-Text")->SetText(getString(LanguageKey::GUI_SEASON_RANK_REWARD));
		getWindow("SeasonRank-Armory-Text")->SetText(getString(LanguageKey::GUI_GUN_STORE_NAME));
		getWindow("SeasonRank-Battle-Text")->SetText(getString(LanguageKey::GUI_SEASON_RANK_BATTLE));

		auto BtnClose = getWindow<GUIButton>("SeasonRank-Close");
		BtnClose->subscribeEvent(EventButtonClick, std::bind(&gui_seasonRank::onCloseClick, this, std::placeholders::_1));
		auto BtnQuesition = getWindow<GUIButton>("SeasonRank-Question");
		BtnQuesition->subscribeEvent(EventButtonClick, std::bind(&gui_seasonRank::onQuestionClick, this, std::placeholders::_1));
		auto BtnArmory = getWindow<GUIButton>("SeasonRank-Armory");
		BtnArmory->subscribeEvent(EventButtonClick, std::bind(&gui_seasonRank::onArmoryClick, this, std::placeholders::_1));
		auto BtnBattle = getWindow<GUIButton>("SeasonRank-Battle");
		BtnBattle->subscribeEvent(EventButtonClick, std::bind(&gui_seasonRank::onBattleClick, this, std::placeholders::_1));

		auto LeftRank = getWindow<GUISeasonRankPanel>("SeasonRank-Left-Rank-Panel");
		auto RightRank = getWindow<GUISeasonRankPanel>("SeasonRank-Right-Rank-Panel");
		onSeasonInit(SeasonHonorId::SEASON_NONE, 0, 0, 0);
	
		m_subscriptionGuard.add(UpdateCurrentSeasonInfoEvent::subscribe(std::bind(&gui_seasonRank::onSeasonInit, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)));
		m_subscriptionGuard.add(UpdateSeasonRankDataEvent::subscribe(std::bind(&gui_seasonRank::onUpdateSeasonRankData, this, std::placeholders::_1, std::placeholders::_2)));
	
	}

	void gui_seasonRank::onUpdate(ui32 nTimeElapse)
	{

	}

	void gui_seasonRank::onShow()
	{
		auto LeftRank = getWindow<GUISeasonRankPanel>("SeasonRank-Left-Rank-Panel");
		list<SeasonRankItem>::type LeftRanks = SeasonSetting::findSeasonRankItemByType(TYPE_LEFT);
		LeftRank->SetSeasonRankData(LeftRanks);
		auto RightRank = getWindow<GUISeasonRankPanel>("SeasonRank-Right-Rank-Panel");
		list<SeasonRankItem>::type RightRanks = SeasonSetting::findSeasonRankItemByType(TYPE_RIGHT);
		RightRank->SetSeasonRankData(RightRanks);
	}

	bool gui_seasonRank::onCloseClick(const EventArgs & events)
	{
		playSoundByType(ST_ClosePanel);
		getParent()->showMainControl();
		return true;
	}

	bool gui_seasonRank::onQuestionClick(const EventArgs & events)
	{
		playSoundByType(ST_Click);
		m_season_rule->show();
		return true;
	}

	bool gui_seasonRank::onArmoryClick(const EventArgs & events)
	{
		playSoundByType(ST_Click);
		OpenGunStoreEvent::emit();
		return true;
	}

	bool gui_seasonRank::onBattleClick(const EventArgs & events)
	{
		playSoundByType(ST_Click);
		ClientNetwork::Instance()->getSender()->sendOpenModeSelect();
		return true;
	}

	bool gui_seasonRank::onSeasonInit(int honorId, int rank, int honor, int endTime)
	{
		auto SeasonName = getWindow("SeasonRank-Season-Name");
		auto LeftHonorIcon = getWindow<GUIStaticImage>("SeasonRank-Season-Honor-Left-Icon");
		auto RightHonorIcon = getWindow<GUIStaticImage>("SeasonRank-Season-Honor-Right-Icon");
		auto Countdown = getWindow("SeasonRank-Season-Countdown-Text");
		auto MyRewardInfo = getWindow("SeasonRank-My-Reward-Info");
		auto MyRank = getWindow("SeasonRank-My-Rank-Text");
		auto MyName = getWindow("SeasonRank-My-Rank-Name");
		auto MyHonor = getWindow("SeasonRank-My-Rank-Honor-Text");
		auto MyReward = getWindow<GUIStaticImage>("SeasonRank-My-Rank-Reward");
		auto MyRewardCount = getWindow("SeasonRank-My-Rank-Reward-Count");
		auto LeftRank = getWindow<GUISeasonRankPanel>("SeasonRank-Left-Rank-Panel");
		auto RightRank = getWindow<GUISeasonRankPanel>("SeasonRank-Right-Rank-Panel");
		Season* season = SeasonSetting::findSeasonByHonorId((SeasonHonorId)honorId);
		if (season)
		{
			SeasonHonorId NextHonorId = SeasonHonorId::SEASON_NONE;
			if (season->HonorId == SeasonHonorId::KING)
			{
				NextHonorId = season->HonorId;
				LeftRank->InitUI(season->HonorId);
				RightRank->InitUI(season->HonorId);
			}
			else
			{
				NextHonorId = (SeasonHonorId)(honorId + 1);
				LeftRank->InitUI(NextHonorId);
				RightRank->InitUI(season->HonorId);
			}
			LeftRank->SetSeasonHonorId(season->HonorId);
			RightRank->SetSeasonHonorId(season->HonorId);
			LeftRank->SetSeasonRankType(SeasonRankType::TYPE_LEFT);
			RightRank->SetSeasonRankType(SeasonRankType::TYPE_RIGHT);
			SeasonName->SetText(getString(season->Name).c_str());
			LeftHonorIcon->SetVisible(true);
			RightHonorIcon->SetVisible(true);
			LeftHonorIcon->SetImage(season->HonorIcon.c_str());
			RightHonorIcon->SetImage(season->HonorIcon.c_str());
			Countdown->SetText(StringUtil::Format(getString(LanguageKey::GUI_SEASON_RANK_COUNTDOWN).c_str(), endTime).c_str());
			MyRank->SetText(rank > 0 ? StringUtil::ToString(rank).c_str() : "-");
			MyHonor->SetText(StringUtil::ToString(honor).c_str());
			
			MyRewardInfo->CleanupChildren();
			list<int>::type rewardIds;
			Season* next_season = SeasonSetting::findSeasonByHonorId(NextHonorId);
			if (next_season != nullptr)
			{
				rewardIds = honor >= next_season->Score ? season->UpRewardIds : season->NormalRewardIds;
				MyReward->SetVisible(honor >= next_season->Score);
				if (honor >= next_season->Score)
				{
					SeasonRankReward* rank_reward = SeasonSetting::findSeasonRankRewardBySeasonIdAndRank(season->Id, rank);
					if (rank_reward)
					{
						MyReward->SetVisible(true);
						MyReward->SetImage(rank_reward->Icon.c_str());
						MyRewardCount->SetText(StringUtil::Format("x%d", rank_reward->Count).c_str());
					}
					else
					{
						MyReward->SetVisible(false);
					}
				}
			}	
			else
			{
				rewardIds = season->NormalRewardIds;
			}
			
			static unsigned MyRewardIndex = 0;
			float ItemWidth = 81.0f;
			float ItemHeight = 27.0f;
			float PositionX = 0;
			for (auto rewardId : rewardIds)
			{
				SeasonReward* reward = SeasonSetting::findSeasonRewardById(rewardId);
				if (reward != nullptr)
				{
					MyRewardIndex++;
					GUIString ChildName = StringUtil::Format("SeasonRank-My-Reward-Item-%d", MyRewardIndex).c_str();
					GUISeasonRewardItem* RewardItem = (GUISeasonRewardItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_SEASON_REWARD_ITEM, ChildName);
					RewardItem->SetArea({ 0.0f, PositionX }, { 0.0f, 1.0f }, { 0.0f, ItemWidth }, { 0.0f, ItemHeight });
					RewardItem->SetSeasonReward(reward, false);
					MyRewardInfo->AddChildWindow(RewardItem);
					PositionX += ItemWidth;
				}
			}
		}
		else
		{
			LeftRank->InitUI(SeasonHonorId::SEASON_NONE);
			RightRank->InitUI(SeasonHonorId::SEASON_NONE);
			SeasonName->SetText("");
			LeftHonorIcon->SetVisible(false);
			RightHonorIcon->SetVisible(false);
			Countdown->SetText("");
			MyRank->SetText("");
			MyHonor->SetText("");
			MyReward->SetVisible(false);
		}
		EntityPlayerSPMP* pPlayer = Blockman::Instance()->m_pPlayer;
		MyName->SetText(pPlayer->getCommandSenderName().c_str());
		return true;
	}

	bool gui_seasonRank::onUpdateSeasonRankData(int type, const String & data)
	{
		SeasonSetting::parseSeasonRankData(type, data);
		if (isShown())
		{
			onShow();
		}
		return true;
	}

}
