#include "GUISeasonRankItem.h"
#include "UI/GUIWindowManager.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticImage.h"

namespace BLOCKMAN
{
	GUISeasonRankItem::GUISeasonRankItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("SeasonRankItem.json"));
		renameComponents(m_window);
		AddChildWindow(m_window);
		SetTouchable(false);
	}

	void GUISeasonRankItem::removeComponents()
	{
		if (m_window)
		{
			RemoveChildWindow(m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	void GUISeasonRankItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		BaseClass::UpdateSelf(nTimeElapse);
	}

	void GUISeasonRankItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GUISeasonRankItem::SetSeasonRankItem(int seasonId, SeasonRankItem rank)
	{
		m_window->GetChildByIndex(0)->SetText(StringUtil::ToString(rank.Rank).c_str());
		m_window->GetChildByIndex(1)->SetText(rank.Name.c_str());
		m_window->GetChildByIndex(2)->SetText(StringUtil::ToString(rank.Honor).c_str());
		auto RewardIcon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(3));
		auto RewardCount = RewardIcon->GetChildByIndex(0);
		SeasonRankReward* rank_reward = SeasonSetting::findSeasonRankRewardBySeasonIdAndRank(seasonId, rank.Rank);
		if (rank_reward && rank.Type == SeasonRankType::TYPE_LEFT)
		{
			RewardIcon->SetImage(rank_reward->Icon.c_str());
			RewardCount->SetText(StringUtil::Format("x%d", rank_reward->Count).c_str());
		}
		else
		{
			RewardIcon->SetVisible(false);
		}
	}

}
