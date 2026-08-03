#include "gui_ranch.h"
#include "UI/GUIButton.h"
#include "Util/LanguageKey.h"
#include "GUI/gui_ranchMain.h"
#include "GUI/gui_ranchHouse.h"
#include "GUI/gui_ranchStorage.h"
#include "GUI/gui_ranchFactory.h"
#include "GUI/gui_ranchFarming.h"
#include "GUI/gui_ranchCommunity.h"
#include "GUI/gui_ranchAchievement.h"
#include "GUI/GuiRanchRankTab.h"
#include "Network/ClientHttpRequest.h"
#include "Util/UICommon.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"

using namespace LORD;

namespace BLOCKMAN
{

	gui_ranch::gui_ranch()
		: gui_layout("Ranch.json")
	{
		m_viewPanelMap[ViewId::MAIN] = m_viewPanelMap[ViewId::MAIN] ? m_viewPanelMap[ViewId::MAIN] : LordNew gui_ranchMain;
		m_viewPanelMap[ViewId::HOUSE] = m_viewPanelMap[ViewId::HOUSE] ? m_viewPanelMap[ViewId::HOUSE] : LordNew gui_ranchHouse;
		m_viewPanelMap[ViewId::STORAGE] = m_viewPanelMap[ViewId::STORAGE] ? m_viewPanelMap[ViewId::STORAGE] : LordNew gui_ranchStorage;
		m_viewPanelMap[ViewId::FACTORY] = m_viewPanelMap[ViewId::FACTORY] ? m_viewPanelMap[ViewId::FACTORY] : LordNew gui_ranchFactory;
		m_viewPanelMap[ViewId::FARMING] = m_viewPanelMap[ViewId::FARMING] ? m_viewPanelMap[ViewId::FARMING] : LordNew gui_ranchFarming;
		m_viewPanelMap[ViewId::COMMUNITY] = m_viewPanelMap[ViewId::COMMUNITY] ? m_viewPanelMap[ViewId::COMMUNITY] : LordNew gui_ranchCommunity;
		m_viewPanelMap[ViewId::ACHIEVEMENT] = m_viewPanelMap[ViewId::ACHIEVEMENT] ? m_viewPanelMap[ViewId::ACHIEVEMENT] : LordNew gui_ranchAchievement;

		m_langMap[ViewId::MAIN] = LanguageKey::GUI_RANCH_MAIN_MAIN;
		m_langMap[ViewId::HOUSE] = LanguageKey::GUI_RANCH_MAIN_HOUSE;
		m_langMap[ViewId::STORAGE] = LanguageKey::GUI_RANCH_MAIN_STORAGE;
		m_langMap[ViewId::FACTORY] = LanguageKey::GUI_RANCH_MAIN_FACTORY;
		m_langMap[ViewId::FARMING] = LanguageKey::GUI_RANCH_MAIN_FARMING;
		m_langMap[ViewId::COMMUNITY] = LanguageKey::GUI_RANCH_MAIN_COMMUNITY;
		m_langMap[ViewId::ACHIEVEMENT] = LanguageKey::GUI_RANCH_MAIN_ACHIEVEMENT;
	}

	gui_ranch::~gui_ranch()
	{
		map<ViewId, gui_layout*>::iterator  iter;
		for (iter = m_viewPanelMap.begin(); iter != m_viewPanelMap.end(); iter++)
		{
			LordSafeDelete(iter->second);
		}
		m_viewPanelMap.clear();
		m_subscriptionGuard.unsubscribeAll();

	}

	void gui_ranch::onLoad()
	{
		m_tvName = getWindow<GUIStaticText>("Ranch-Title-Name");

		getWindow<GuiRanchRankTab>("Ranch-Rank-Clan")->setName(getString("gui_ranch_clan_rank_title"));
		getWindow<GuiRanchRankTab>("Ranch-Rank-Prosperity")->setName(getString("gui_ranch_prosperity_rank_title"));
		getWindow<GuiRanchRankTab>("Ranch-Rank-Achievement")->setName(getString("gui_ranch_achievement_rank_title"));

		getWindow<GUIButton>("Ranch-Btn-Close")->subscribeEvent(EventButtonClick, std::bind(&gui_ranch::onClose, this, std::placeholders::_1));
		getWindow<GUIButton>("Ranch-Btn-Back")->subscribeEvent(EventButtonClick, std::bind(&gui_ranch::onBack, this, std::placeholders::_1));
		getWindow("Ranch-Rank-Clan")->subscribeEvent(EventWindowTouchUp, std::bind(&gui_ranch::onRankClick, this, std::placeholders::_1, BtnRank::CLAN));
		getWindow("Ranch-Rank-Prosperity")->subscribeEvent(EventWindowTouchUp, std::bind(&gui_ranch::onRankClick, this, std::placeholders::_1, BtnRank::PROSPERITY));
		getWindow("Ranch-Rank-Achievement")->subscribeEvent(EventWindowTouchUp, std::bind(&gui_ranch::onRankClick, this, std::placeholders::_1, BtnRank::ACHIEVEMENT));
		m_subscriptionGuard.add(RanchStorageOpenEvent::subscribe(std::bind(&gui_ranch::openRanchStorage, this)));
		m_subscriptionGuard.add(RanchDataChangeEvent::subscribe(std::bind(&gui_ranch::ranchInfoChange, this)));
		auto radioPanel = getWindow("Ranch-Panel");
		for (auto item : m_viewPanelMap)
		{
			if (item.second)
			{
				item.second->attachTo(this, radioPanel);
				item.second->hide();
			}
		}
		onClick(ViewId::MAIN);
	}

	void gui_ranch::onHide()
	{
		for (auto view : m_viewPanelMap)
		{
			if (view.second)
			{
				view.second->hide();
			}
		}
	}

	void gui_ranch::onUpdate(ui32 nTimeElapse)
	{

	}

	void gui_ranch::onClick(const ViewId& viewId)
	{
		for (auto item : m_viewPanelMap)
		{
			if (item.second && item.second->isShown() && m_viewPanelMap[viewId])
			{
				item.second->hide();
			}
		}
		if (m_viewPanelMap[viewId])
		{
			m_viewPanelMap[viewId]->show();
			m_tvName->SetText(getString(m_langMap[viewId]));
			getWindow<GUIButton>("Ranch-Btn-Back")->SetVisible(viewId != ViewId::MAIN);
			getWindow("Ranch-Rank")->SetVisible(viewId == ViewId::MAIN);
			if (viewId == ViewId::COMMUNITY)
			{
				ClientHttpRequest::refreshFriend();
				ClientHttpRequest::loadMails();
			}
		}
		playSoundByType(ST_Click);
	}

	bool gui_ranch::onRankClick(const EventArgs & args, const BtnRank & viewId)
	{
		switch (viewId)
		{
		case BtnRank::CLAN:
			ShowRanchRankEvent::emit(1);
			RootGuiLayout::Instance()->setRanchRankVisible(true);
			break;
		case BtnRank::PROSPERITY:
			//ShowRanchRankEvent::emit(2);
			RootGuiLayout::Instance()->setRanchProsperityRankVisible(true);
			break;
		case BtnRank::ACHIEVEMENT:
			ShowRanchRankEvent::emit(3);
			RootGuiLayout::Instance()->setRanchRankVisible(true);
			break;
		}
		
		return true;
	}

	bool gui_ranch::onClose(const EventArgs & events)
	{
		getParent()->showMainControl();
		return true;
	}

	bool gui_ranch::onBack(const EventArgs & events)
	{
		onClick(ViewId::MAIN);
		return true;
	}

	void gui_ranch::openRanch()
	{
		onClick(ViewId::MAIN);
	}

	bool gui_ranch::openRanchStorage()
	{
		getParent()->showRanch();
		onClick(ViewId::STORAGE);
		return true;
	}

	bool gui_ranch::ranchInfoChange()
	{
		if (UICommon::checkRanchParam() && Blockman::Instance()->m_pPlayer->m_ranch->getInfo())
		{
			RanchInfo* ranchInfo = Blockman::Instance()->m_pPlayer->m_ranch->getInfo();
			getWindow<GuiRanchRankTab>("Ranch-Rank-Clan")->setRank(ranchInfo->clanRank);
			getWindow<GuiRanchRankTab>("Ranch-Rank-Prosperity")->setRank(ranchInfo->prosperityRank);
			getWindow<GuiRanchRankTab>("Ranch-Rank-Achievement")->setRank(ranchInfo->achievementRank);

			
		}
		return true;
	}


}
