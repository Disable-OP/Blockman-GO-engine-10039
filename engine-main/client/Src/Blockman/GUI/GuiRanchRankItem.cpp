#include "GuiRanchRankItem.h"
#include "UI/GUIWindowManager.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"
#include "game.h"

namespace BLOCKMAN
{

	void GuiRanchRankItem::removeComponents()
	{
		if (m_window)
		{
			RemoveWindowFromDrawList(*m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	vector<GUIWindow*>::type GuiRanchRankItem::getPrivateChildren()
	{
		return { m_window };
	}

	GuiRanchRankItem::GuiRanchRankItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("RanchRankItem.json"));
		renameComponents(m_window);
		m_window->SetTouchable(false);
		m_tvRankValue = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(0));
		m_ivRankIcon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(1));
		m_tvNickName = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(2));
		m_tvUserLevel = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(3));
		m_tvClanUserNum = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(4));
		m_ivIcon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(5));
		m_tvValue = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(6));
		m_btnVisit = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(7));
		m_btnVisit->subscribeEvent(EventButtonClick, std::bind(&GuiRanchRankItem::onClick, this, std::placeholders::_1));
		AddWindowToDrawList(*m_window);
	}

	void GuiRanchRankItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());
		BaseClass::UpdateSelf(nTimeElapse);
		changeRankValue(m_ranchRank.rank);
		m_ivRankIcon->SetVisible(m_ranchRank.rank > 0 && m_ranchRank.rank < 4);
		if (m_ranchRank.rank < 4)
		{
			m_ivRankIcon->SetImage(StringUtil::Format("set:ranch_rank.json image:rank_%d", m_ranchRank.rank).c_str());
		}
		m_tvNickName->SetText(m_ranchRank.name.c_str());
		m_tvUserLevel->SetText(StringUtil::ToString(m_ranchRank.level).c_str());
		m_tvUserLevel->SetVisible(m_rankType > 1);
		m_tvClanUserNum->SetVisible(m_rankType == 1);
		m_tvClanUserNum->SetText(m_ranchRank.userNum.c_str());
		m_ivIcon->SetImage(m_rankType == 3 ? "set:ranch_rank.json image:achievement_icon" : "set:ranch_main.json image:ranch_user_icon_prosperity");
		m_ivIcon->SetSize(m_rankType == 3 ? UVector2(UDim(0, 44.f), UDim(0, 44.f)) : UVector2(UDim(0, 65.f), UDim(0, 42.f)));
		m_tvValue->SetText(StringUtil::ToString(m_ranchRank.value).c_str());
		m_btnVisit->SetVisible(m_rankType == 2);
	}


	void GuiRanchRankItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GuiRanchRankItem::setRank(const RanchRank & rank)
	{
		m_ranchRank = rank;
	}

	bool GuiRanchRankItem::onClick(const EventArgs & args)
	{
		if (m_ranchRank.id > 0)
		{
			GameClient::CGame::Instance()->resetGame(m_ranchRank.id);
		}
		return true;
	}
	void GuiRanchRankItem::changeRankValue(i32 rank)
	{
		switch (rank)
		{
		case 1:
			m_tvRankValue->SetTextColor(Color(0.7843f, 0, 0));
			break;
		case 2:
			m_tvRankValue->SetTextColor(Color(0.9765f, 0.2667f, 0.9451f));
			break;
		case 3:
			m_tvRankValue->SetTextColor(Color(0, 0.5451f, 0.6117f));
			break;
		default:
			m_tvRankValue->SetTextColor(Color(0.4117f, 0.3215f, 0.2313f));
			break;
		}
		m_tvRankValue->SetText(StringUtil::ToString(rank).c_str());
	}
}