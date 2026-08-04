#include "gui_ranchCommunity.h"
#include "Util/LanguageKey.h"
#include "Util/ClientEvents.h"
#include "Util/FriendManager.h"
#include "Util/RedDotManager.h"
#include "Util/MailManager.h"
#include "GUI/GuiRanchCommunityItem.h"
#include "GUI/GuiRanchFriendHelpItem.h"
#include "GUI/GuiRanchMailItem.h"
#include "GUI/GuiDef.h"
#include "Util/UICommon.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Network/ClientHttpRequest.h"
#include "game.h"
#include "Setting/GameTypeSetting.h"

using namespace LORD;

namespace BLOCKMAN
{
	gui_ranchCommunity::gui_ranchCommunity()
		: gui_layout("RanchCommunity.json")
	{
		m_radioTextMap[ViewId::FRIEDN] = getString("gui_ranch_community_tab_friend");
		m_radioTextMap[ViewId::CLAN] = getString("gui_ranch_community_tab_clan");
		m_radioTextMap[ViewId::MAIL] = getString("gui_ranch_community_tab_mail");

		m_lvOffsetMap[ViewId::FRIEDN] = 0.0f;
		m_lvOffsetMap[ViewId::CLAN] = 0.0f;
		m_lvOffsetMap[ViewId::MAIL] = 0.0f;
	}

	gui_ranchCommunity::~gui_ranchCommunity()
	{
		m_lvOffsetMap.clear();
		m_radioTextMap.clear();
		m_radioPanelMap.clear();
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_ranchCommunity::onLoad()
	{
		m_radioMap[ViewId::FRIEDN] = getWindow<GUIRadioButton>("RanchCommunity-Tab-Friend");
		m_radioMap[ViewId::CLAN] = getWindow<GUIRadioButton>("RanchCommunity-Tab-Clan");
		m_radioMap[ViewId::MAIL] = getWindow<GUIRadioButton>("RanchCommunity-Tab-Mail");

		m_radioPanelMap[ViewId::FRIEDN] = getWindow<GUIListHorizontalBox>("RanchCommunity-Panel-Friend");
		m_radioPanelMap[ViewId::CLAN] = getWindow<GUIListHorizontalBox>("RanchCommunity-Panel-Clan");
		m_radioPanelMap[ViewId::MAIL] = getWindow<GUIListHorizontalBox>("RanchCommunity-Panel-Mail");

		setText("RanchCommunity-Panel-Helps-Title", "gui_ranch_friend_help_list");

		m_tvGiftValue = getWindow<GUIStaticText>("RanchCommunity-Info-Value");
		m_helpsLayout = getWindow<GUILayout>("RanchCommunity-Panel-Helps");
		m_helpsList = getWindow<GUIListBox>("RanchCommunity-Panel-Helps-List");
		m_orderOperate = getWindow<GuiRanchOrderOperate>("RanchCommunity-Panel-Helps-Operate");
		m_friendOperate = getWindow<GuiRanchFriendOperate>("RanchCommunity-Panel-FriendOperate");
		m_orderOperate->chengeUI();
		m_friendOperate->SetVisible(false);
		m_helpsLayout->SetVisible(false);
		m_orderOperate->SetVisible(false);

		for (auto item : m_radioMap)
		{
			item.second->SetText(m_radioTextMap[item.first]);
			item.second->subscribeEvent(EventRadioStateChanged, std::bind(&gui_ranchCommunity::onRadioChange, this, std::placeholders::_1, item.first));
		}
		m_radioMap[ViewId::FRIEDN]->SetSelected(true);
		m_subscriptionGuard.add(MailUpdateEvent::subscribe(std::bind(&gui_ranchCommunity::mailChange, this)));
		m_subscriptionGuard.add(FriendUpdateEvent::subscribe(std::bind(&gui_ranchCommunity::onFriendUpdate, this)));
		m_subscriptionGuard.add(FrendDataChangeEvent::subscribe(std::bind(&gui_ranchCommunity::friendChange, this)));
		m_subscriptionGuard.add(RanchFriendOperateHideEvent::subscribe(std::bind(&gui_ranchCommunity::hideFriendOperate, this)));
		m_subscriptionGuard.add(ClanMemberDataChangeEvent::subscribe(std::bind(&gui_ranchCommunity::clanMemberChange, this)));
		m_subscriptionGuard.add(ShowRanchFriendHelpListEvent::subscribe(std::bind(&gui_ranchCommunity::onHelpsChange, this, std::placeholders::_1)));
		m_subscriptionGuard.add(RanchFriendClickEvent::subscribe(std::bind(&gui_ranchCommunity::onFriendClick, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(RanchOrderItemClickEvent::subscribe(std::bind(&gui_ranchCommunity::onOrederItemClick, this, std::placeholders::_1, std::placeholders::_2)));


		m_radioPanelMap[ViewId::FRIEDN]->subscribeEvent(EventWindowTouchDown, std::bind(&gui_ranchCommunity::onClick, this, std::placeholders::_1));
		m_radioPanelMap[ViewId::CLAN]->subscribeEvent(EventWindowTouchDown, std::bind(&gui_ranchCommunity::onClick, this, std::placeholders::_1));
		m_radioPanelMap[ViewId::MAIL]->subscribeEvent(EventWindowTouchDown, std::bind(&gui_ranchCommunity::onClick, this, std::placeholders::_1));
		m_helpsList->subscribeEvent(EventWindowTouchDown, std::bind(&gui_ranchCommunity::hideHelpOperate, this, std::placeholders::_1));
		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::Rancher)
		{
			onFriendUpdate();
		}
	}

	void gui_ranchCommunity::onHide()
	{
		hideFriendOperate();
	}

	void gui_ranchCommunity::onUpdate(ui32 nTimeElapse)
	{
		if (UICommon::checkRanchParam() && Blockman::Instance()->m_pPlayer->m_ranch->getInfo() && isShown())
		{
			RanchInfo* info = Blockman::Instance()->m_pPlayer->m_ranch->getInfo();
			m_tvGiftValue->SetText(StringUtil::ToString(info->giftNum).c_str());
		}

		if (isShown())
		{
			getWindow("RanchCommunity-Tab-Friend-RedDot")->SetVisible(RedDotManager::checkFriend());
			getWindow("RanchCommunity-Tab-Clan-RedDot")->SetVisible(RedDotManager::checkClan());
			getWindow("RanchCommunity-Tab-Mail-RedDot")->SetVisible(RedDotManager::checkMail());
		}
	}

	bool gui_ranchCommunity::onRadioChange(const EventArgs & events, ViewId radioId)
	{
		if (m_radioMap[radioId]->IsSelected())
		{
			for (auto item : m_radioPanelMap)
			{
				item.second->SetVisible(item.first == radioId);
			}
			playSoundByType(ST_Click);
		}
		hideFriendOperate();
		return true;
	}

	bool gui_ranchCommunity::mailChange()
	{
		hideFriendOperate();
		const auto & mails = MailManager::getMails();
		changeOffset(ViewId::MAIL, (int)mails.size());
		m_radioPanelMap[ViewId::MAIL]->ClearAllItem();
		for (auto item : mails)
		{
			static unsigned count = 0;
			count++;
			String itemLayout = StringUtil::Format("RanchCommunity-Panel-Mail-Item-%d", count).c_str();
			if (GUIWindowManager::Instance()->GetGUIWindow(itemLayout.c_str()))
			{
				return false;
			}
			GuiRanchMailItem* itemView = (GuiRanchMailItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_RANCH_MAIL_ITEM, itemLayout.c_str());
			itemView->setMail(item);
			itemView->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(0.f, 218.f), UDim(1.0f, 0.f));
			m_radioPanelMap[ViewId::MAIL]->AddItem(itemView, false);
		}
		m_radioPanelMap[ViewId::MAIL]->SetScrollOffset(m_lvOffsetMap[ViewId::MAIL]);
		return true;
	}

	bool gui_ranchCommunity::friendChange()
	{
		hideFriendOperate();
		auto friends = FriendManager::getFriends();
		changeOffset(ViewId::FRIEDN, (int)friends.size());
		m_radioPanelMap[ViewId::FRIEDN]->ClearAllItem();
		for (auto item : friends)
		{
			static unsigned count = 0;
			count++;
			String itemLayout = StringUtil::Format("RanchCommunity-Panel-Friend-Item-%d", count).c_str();
			if (GUIWindowManager::Instance()->GetGUIWindow(itemLayout.c_str()))
			{
				return false;
			}
			GuiRanchCommunityItem* itemView = (GuiRanchCommunityItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_RANCH_COMMUNITY_ITEM, itemLayout.c_str());
			itemView->SetTouchable(false);
			itemView->setFriendId(item->userId);
			itemView->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(0.f, 218.f), UDim(1.0f, 0.f));
			m_radioPanelMap[ViewId::FRIEDN]->AddItem(itemView, false);
		}
		m_radioPanelMap[ViewId::FRIEDN]->SetScrollOffset(m_lvOffsetMap[ViewId::FRIEDN]);
		return true;
	}

	bool gui_ranchCommunity::clanMemberChange()
	{
		hideFriendOperate();
		auto friends = FriendManager::getMembers();
		changeOffset(ViewId::CLAN, (int)friends.size());
		m_radioPanelMap[ViewId::CLAN]->ClearAllItem();
		for (auto item : friends)
		{
			static unsigned count = 0;
			count++;
			String itemLayout = StringUtil::Format("RanchCommunity-Panel-Clan-Item-%d", count).c_str();
			if (GUIWindowManager::Instance()->GetGUIWindow(itemLayout.c_str()))
			{
				return false;
			}
			GuiRanchCommunityItem* itemView = (GuiRanchCommunityItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_RANCH_COMMUNITY_ITEM, itemLayout.c_str());
			itemView->setFriendId(item->userId);
			itemView->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(0.f, 218.f), UDim(1.0f, 0.f));
			m_radioPanelMap[ViewId::CLAN]->AddItem(itemView, false);
		}
		m_radioPanelMap[ViewId::CLAN]->SetScrollOffset(m_lvOffsetMap[ViewId::CLAN]);
		return true;
	}

	bool gui_ranchCommunity::onClick(const EventArgs & args)
	{
		hideFriendOperate();
		return true;
	}

	bool gui_ranchCommunity::hideHelpOperate(const EventArgs & args)
	{
		m_orderOperate->SetVisible(false);
		return true;
	}

	bool gui_ranchCommunity::onOrederItemClick(const UVector2 & pos, const RanchOrderInfo & info)
	{
		if (isShown())
		{
			m_orderOperate->SetVisible(true);
			m_orderOperate->setOrderInfo(info);
			m_orderOperate->SetPosition(UVector2(UDim(0, -200.f), UDim(0, pos.d_y.d_offset + 34.f - 180.f)));
		}
		return true;
	}

	bool gui_ranchCommunity::hideFriendOperate()
	{
		m_helpsLayout->SetVisible(false);
		m_friendOperate->SetVisible(false);
		m_orderOperate->SetVisible(false);
		return true;
	}

	bool gui_ranchCommunity::onFriendClick(i64 userId, const String & viewName)
	{
		if (isShown())
		{
			hideFriendOperate();
			GuiRanchCommunityItem* view = (GuiRanchCommunityItem*)GUIWindowManager::Instance()->GetGUIWindow(viewName.c_str());
			if (view)
			{
				UVector2 uv = view->getAbsolutePositionFromRoot();
				UVector2 newUv = UVector2(UDim(0, uv.d_x.d_offset - 10.f - view->GetPixelSize().x / 2), UDim(0, uv.d_y.d_offset - 50.f - view->GetPixelSize().y / 2));
				m_friendOperate->SetVisible(true);
				m_friendOperate->setUserId(userId);
				auto pal = FriendManager::findUserById(userId);
				m_friendOperate->setNeedHelp(pal ? pal->needHelp : false);
				m_friendOperate->SetPosition(newUv);
			}

		}
		return true;
	}

	bool gui_ranchCommunity::onHelpsChange(i64 userId)
	{
		if (isShown())
		{
			hideFriendOperate();
			m_helpsList->ClearAllItem();
			m_helpsLayout->SetVisible(true);
			UVector2 uv = m_friendOperate->GetPosition();
			float offset = uv.d_x.d_offset + (uv.d_x.d_offset > 700.f ? -266.f : 210.f);
			UVector2 newUv = UVector2(UDim(0.f, offset), UDim(0.f, -80.f));
			m_helpsLayout->SetPosition(newUv);
			auto pal = FriendManager::findUserById(userId);
			for (auto item : pal->orders)
			{
				static unsigned count = 0;
				count++;
				String itemLayout = StringUtil::Format("RanchCommunity-Panel-Helps-List-Item-%d", count).c_str();
				if (GUIWindowManager::Instance()->GetGUIWindow(itemLayout.c_str()))
				{
					return false;
				}
				GuiRanchFriendHelpItem* itemView = (GuiRanchFriendHelpItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_RANCH_FRIEND_HELP_ITEM, itemLayout.c_str());
				itemView->setOrderInfo(item);
				itemView->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(1.f, 0.f), UDim(0.f, 104.f));
				m_helpsList->AddItem(itemView, false);
			}

		}
		return true;
	}

	bool gui_ranchCommunity::onFriendUpdate()
	{
		ClientHttpRequest::refreshFriend();
		return false;
	}

	void gui_ranchCommunity::changeOffset(ViewId viewId, int size)
	{
		float offset = m_radioPanelMap[viewId]->GetScrollOffset();
		m_lvOffsetMap[viewId] = (size > 0 && isShown()) ? offset : 0;
	}
}
