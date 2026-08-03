#include "GUISeasonRewardItem.h"
#include "UI/GUIWindowManager.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUILayout.h"
#include "UI/GUIFontManager.h"
#include "UI/GUIFont.h"

namespace BLOCKMAN
{
	GUISeasonRewardItem::GUISeasonRewardItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("SeasonRewardItem.json"));
		renameComponents(m_window);
		AddChildWindow(m_window);
		SetTouchable(false);
	}

	void GUISeasonRewardItem::removeComponents()
	{
		if (m_window)
		{
			RemoveChildWindow(m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	void GUISeasonRewardItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		BaseClass::UpdateSelf(nTimeElapse);
	}

	void GUISeasonRewardItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GUISeasonRewardItem::SetSeasonReward(SeasonReward * reward, bool isBig)
	{
		auto Icon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		Icon->SetImage(reward->Icon.c_str());
		auto Count = m_window->GetChildByIndex(1);
		Count->SetText(StringUtil::ToString(reward->Count).c_str());
		if (isBig)
			Count->SetFont(GUIFontManager::Instance()->GetFont("HT20"));
		auto Hint = m_window->GetChildByIndex(2);
		if (reward->Type == SeasonRewardType::REWARD_HONOR_UP || reward->Type == SeasonRewardType::REWARD_CHEST)
		{
			Icon->SetWidth({ 0.42f, 0.0f });
			Hint->SetVisible(true);
			Count->SetVisible(false);
		}
		else
		{
			Icon->SetWidth({ 0.33f, 0.0f });
			Hint->SetVisible(false);
			Count->SetVisible(true);
		}
	}

	void GUISeasonRewardItem::SetSeasonRankReward(SeasonRankReward * reward, bool isBig)
	{
		auto Icon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		Icon->SetImage(reward->Icon.c_str());
		auto Count = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(1));
		Count->SetText(StringUtil::ToString(reward->Count).c_str());
		if (isBig)
			Count->SetFont(GUIFontManager::Instance()->GetFont("HT20"));
		
		auto Hint = m_window->GetChildByIndex(2);
		if (reward->Type == SeasonRewardType::REWARD_HONOR_UP)
		{
			Icon->SetWidth({ 0.42f, 0.0f });
			Hint->SetVisible(true);
			Count->SetVisible(false);
		}
		else
		{
			Icon->SetWidth({ 0.33f, 0.0f });
			Hint->SetVisible(false);
			Count->SetVisible(true);
		}
	}

}
