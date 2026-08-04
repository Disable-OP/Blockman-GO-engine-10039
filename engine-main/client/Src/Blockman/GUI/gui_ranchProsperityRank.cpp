#include "gui_ranchProsperityRank.h"
#include "Util/LanguageKey.h"
#include "GUI/RootGuiLayout.h"
#include "GUI/GuiRanchRankItem.h"
#include "GUI/GuiDef.h"
#include "Util/ClientEvents.h"
#include "Util/FriendManager.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "GUI/GuiActorWindow.h"
#include "Network/ClientHttpRequest.h"
#include "Util/UICommon.h"

using namespace LORD;

namespace BLOCKMAN
{
	gui_ranchProsperityRank::gui_ranchProsperityRank()
		: gui_layout("RanchProsperityRank.json")
	{
	}

	gui_ranchProsperityRank::~gui_ranchProsperityRank()
	{
		m_rankLayoutMap.clear();
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_ranchProsperityRank::onLoad()
	{

		m_tvTitle = getWindow<GUIStaticText>("RanchProsperityRank-Title");
		m_lvRanks = getWindow<GUIListBox>("RanchProsperityRank-Ranks");
		m_rankFirstLayout = getWindow<GUILayout>("RanchProsperityRank-First");
		m_rankSecondLayout = getWindow<GUILayout>("RanchProsperityRank-Second");
		m_rankThirdLayout = getWindow<GUILayout>("RanchProsperityRank-Third");

		setText("RanchProsperityRank-Title", "gui_ranch_prosperity_rank");
		m_subscriptionGuard.add(RanchRankChangeEvent::subscribe(std::bind(&gui_ranchProsperityRank::changeRanks, this)));
		m_subscriptionGuard.add(UserActorChangeEvent::subscribe(std::bind(&gui_ranchProsperityRank::userActorChange, this, std::placeholders::_1)));
		getWindow("RanchProsperityRank-Btn-Close")->subscribeEvent(EventButtonClick, std::bind(&gui_ranchProsperityRank::onClick, this, std::placeholders::_1, ViewId::BTN_CLOSE));
	}

	void gui_ranchProsperityRank::onShow()
	{
		if (!UICommon::checkRanchParam() || !m_dataChange)
		{
			return;
		}

		m_lvRanks->ClearAllItem();
		m_lvRanks->ResetScroll();
		m_rankLayoutMap.clear();
		const vector<RanchRank>::type & ranks = Blockman::Instance()->m_pPlayer->m_ranch->getRankByType(2);

		for (const auto& item : ranks)
		{
			switch (item.rank) {
			case 1:
			{
				GUIWindow* tvUserName = m_rankFirstLayout->GetChildByIndex(1)->GetChildByIndex(0);
				GUIWindow* tvUserLevel = m_rankFirstLayout->GetChildByIndex(1)->GetChildByIndex(1);
				tvUserName->SetText(item.name.c_str());
				tvUserLevel->SetText(StringUtil::ToString(item.level).c_str());
				m_rankLayoutMap[item.id] = m_rankFirstLayout;
			}
			break;
			case 2:
			{
				GUIWindow* tvUserName = m_rankSecondLayout->GetChildByIndex(1)->GetChildByIndex(0);
				GUIWindow* tvUserLevel = m_rankSecondLayout->GetChildByIndex(1)->GetChildByIndex(1);
				tvUserName->SetText(item.name.c_str());
				tvUserLevel->SetText(StringUtil::ToString(item.level).c_str());
				m_rankLayoutMap[item.id] = m_rankSecondLayout;
			}
			break;
			case 3:
			{
				GUIWindow* tvUserName = m_rankThirdLayout->GetChildByIndex(1)->GetChildByIndex(0);
				GUIWindow* tvUserLevel = m_rankThirdLayout->GetChildByIndex(1)->GetChildByIndex(1);
				tvUserName->SetText(item.name.c_str());
				tvUserLevel->SetText(StringUtil::ToString(item.level).c_str());
				m_rankLayoutMap[item.id] = m_rankThirdLayout;
			}
			break;
			}

			if (item.rank < 4)
			{
				ClientHttpRequest::loadUserInfo(item.id);
			}

			static unsigned count = 0;
			count++;
			String itemLayout = StringUtil::Format("RanchProsperityRank-Ranks-Item-%d", count).c_str();
			if (GUIWindowManager::Instance()->GetGUIWindow(itemLayout.c_str()))
			{
				return;
			}

			GuiRanchRankItem* itemView = (GuiRanchRankItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_RANCH_RANK_ITEM, itemLayout.c_str());
			itemView->setRankType(2);
			itemView->setRank(item);
			itemView->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(1.0f, 0), UDim(0.0f, 49.f));
			m_lvRanks->AddItem(itemView, false);
		}

		m_dataChange = false;
	}

	void gui_ranchProsperityRank::onUpdate(ui32 nTimeElapse)
	{

	}

	bool gui_ranchProsperityRank::onClick(const EventArgs & events, ViewId viewId)
	{
		RootGuiLayout::Instance()->setRanchProsperityRankVisible(false);
		return true;
	}

	bool gui_ranchProsperityRank::changeRanks()
	{
		m_dataChange = true;
		if (isShown())
		{
			onShow();
		}
		return true;
	}

	bool gui_ranchProsperityRank::userActorChange(i64 userId)
	{
		GUILayout * view = m_rankLayoutMap[userId];
		if (view)
		{
			GUIStaticText* tvClanName = dynamic_cast<GUIStaticText*>(view->GetChildByIndex(0));
			GUIStaticText* tvUserName = dynamic_cast<GUIStaticText*>(view->GetChildByIndex(1)->GetChildByIndex(0));
			GuiActorWindow* actorWindow = dynamic_cast<GuiActorWindow*>(view->GetChildByIndex(2));
			UserActor* userActor = FriendManager::findUserActor(userId);
			if (userActor)
			{
				if (userActor->nickName.length() > 0)
				{
					tvUserName->SetText(userActor->nickName.c_str());
				}
				tvClanName->SetText(userActor->clanName.c_str());
				actorWindow->SetActor(userActor->sex == 1 ? "boy.actor" : "girl.actor", "idle");
				actorWindow->SetCustomColor(userActor->actorColor);
				for (const auto& item : userActor->items)
				{
					if (item.resName.length() > 0 && item.resId.length() > 0 && item.resId != "0")
					{
						actorWindow->UseBodyPart(item.resName, item.resId);
					}
				}
			}
		}
		return true;
	}
}
