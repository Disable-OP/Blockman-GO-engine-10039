#include "gui_ranchRank.h"
#include "Util/LanguageKey.h"
#include "GUI/RootGuiLayout.h"
#include "GUI/GuiRanchRankItem.h"
#include "GUI/GuiDef.h"
#include "Util/ClientEvents.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"

using namespace LORD;

namespace BLOCKMAN
{
	gui_ranchRank::gui_ranchRank()
		: gui_layout("RanchRank.json")
	{
	}

	gui_ranchRank::~gui_ranchRank()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_ranchRank::onLoad()
	{
		m_tvTitle = getWindow<GUIStaticText>("RanchRank-Title");
		m_lvRanks = getWindow<GUIListBox>("RanchRank-Ranks");

		m_subscriptionGuard.add(ShowRanchRankEvent::subscribe(std::bind(&gui_ranchRank::changeRanks, this, std::placeholders::_1)));
		getWindow("RanchRank-Btn-Close")->subscribeEvent(EventButtonClick, std::bind(&gui_ranchRank::onClick, this, std::placeholders::_1, ViewId::BTN_CLOSE));
		changeRanks(1);

	}

	void gui_ranchRank::onUpdate(ui32 nTimeElapse)
	{

	}

	bool gui_ranchRank::onClick(const EventArgs & events, ViewId viewId)
	{
		RootGuiLayout::Instance()->setRanchRankVisible(false);
		return true;
	}

	bool gui_ranchRank::changeRanks(int rankType)
	{
		m_lvRanks->ClearAllItem();
		m_lvRanks->ResetScroll();
		setText("RanchRank-Title", rankType == 1 ? "gui_ranch_clan_rank" : "gui_ranch_achievement_rank");
		const vector<RanchRank>::type & ranks = Blockman::Instance()->m_pPlayer->m_ranch->getRankByType(rankType);
		for (auto rank : ranks)
		{
			static unsigned count = 0;
			count++;
			String itemLayout = StringUtil::Format("RanchRank-Ranks-Item-%d", count).c_str();
			if (GUIWindowManager::Instance()->GetGUIWindow(itemLayout.c_str()))
			{
				return false;
			}
			GuiRanchRankItem* itemView = (GuiRanchRankItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_RANCH_RANK_ITEM, itemLayout.c_str());
			itemView->setRankType(rankType);
			itemView->setRank(rank);
			itemView->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(1.0f, 0), UDim(0.0f, 49.f));
			m_lvRanks->AddItem(itemView, false);
		}
		return true;
	}
}
