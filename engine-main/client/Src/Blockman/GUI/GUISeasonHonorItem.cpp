#include "GUISeasonHonorItem.h"
#include "UI/GUIWindowManager.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticImage.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"
#include "Setting/SeasonSetting.h"

namespace BLOCKMAN
{
	GUISeasonHonorItem::GUISeasonHonorItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("SeasonHonorItem.json"));
		renameComponents(m_window);

		AddWindowToDrawList(*m_window);
		SetTouchable(false);
	}

	void GUISeasonHonorItem::removeComponents()
	{
		if (m_window)
		{
			RemoveWindowFromDrawList(*m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	void GUISeasonHonorItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());
		BaseClass::UpdateSelf(nTimeElapse);
	}

	void GUISeasonHonorItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GUISeasonHonorItem::SetHonorInfo(HonorItemType type, int honorId, int rank, int honor)
	{
		auto Title = m_window->GetChildByIndex(0);
		auto Flag = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(1));
		auto SmallHonorIcon = dynamic_cast<GUIStaticImage*>(Flag->GetChildByIndex(0)->GetChildByIndex(0));
		auto Rank = SmallHonorIcon->GetChildByIndex(0);
		auto HonorIcon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(2));
		auto HonorName = HonorIcon->GetChildByIndex(0);
		auto HonorValue = HonorName->GetChildByIndex(0)->GetChildByIndex(0);
		Title->SetText(LanguageManager::Instance()->getString(type == LAST_SEASON ? LanguageKey::GUI_SEASON_REWARD_LAST : LanguageKey::GUI_SEASON_REWARD_CURRENT).c_str());
		Rank->SetText(StringUtil::Format("NO:%d", rank).c_str());
		HonorValue->SetText(StringUtil::ToString(honor).c_str());
		Season* season = SeasonSetting::findSeasonByHonorId((SeasonHonorId)honorId);
		if (season)
		{
			Flag->SetImage(season->HonorFlag.c_str());
			SmallHonorIcon->SetImage(season->HonorIcon.c_str());
			HonorIcon->SetImage(season->HonorIcon.c_str());
			HonorName->SetText(LanguageManager::Instance()->getString(season->HonorName).c_str());
		}
	}

}
