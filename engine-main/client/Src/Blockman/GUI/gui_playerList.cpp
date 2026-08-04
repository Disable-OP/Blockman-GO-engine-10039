#include "gui_playerList.h"
#include "Util/ClientEvents.h"
#include "Util/LanguageKey.h"
#include "GuiDef.h"
#include "game.h"

namespace BLOCKMAN {

	gui_playerList::gui_playerList()
		: gui_layout("MenuPlayer.json")
	{
		using namespace  std::placeholders;
		m_subscriptionGuard.add(UserInEvent::subscribe(std::bind(&gui_playerList::onUserIn, this, _1, _2, _3, _4, _5, _6)));
		m_subscriptionGuard.add(UserOutEvent::subscribe(std::bind(&gui_playerList::onUserOut, this, _1)));
		m_subscriptionGuard.add(FriendOperationForServerEvent::subscribe(std::bind(&gui_playerList::onFriendOperationForServer, this, _1, _2)));
		m_subscriptionGuard.add(FriendOperationForAppHttpResultEvent::subscribe(std::bind(&gui_playerList::onFriendOperationForAppHttpResult, this, _1, _2)));
	}

	gui_playerList::~gui_playerList()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_playerList::onLoad()
	{
		m_playerList = getWindow<GUIListBox>("MenuPlayer-PlayerList");
		m_tipMessageView = getWindow<GUIStaticText>("MenuPlayer-Tip-Message");
		m_tipMessageView->SetText("");
	}

	void gui_playerList::onUpdate(ui32 nTimeElapse)
	{
		if (m_playerList && m_initBeforeMap.size() > 0)
		{
			map<ui64, GuiPlayerItem*>::iterator iter;
			for (iter = m_initBeforeMap.begin(); iter != m_initBeforeMap.end(); iter++)
			{
				if (iter->second)
				{
					m_playerList->AddItem(iter->second, false);
				}
			}
			m_initBeforeMap.clear();
		}

		if (m_tipShopTime == 0  || m_tipShopTime > 2000)
		{
			m_tipMessageView->SetText("");
		}
		else
		{
			m_tipShopTime += nTimeElapse;
			m_tipMessageView->SetText(m_tipMessage.c_str());
		}
	}

	bool gui_playerList::onUserIn(const ui64 platformUserId, const String & userName, const String & teamName, const int teamId, const int curMunber, const int totalMunber)
	{
		static unsigned count = 0;
		count++;
		String itemLayout = StringUtil::Format("MenuPlayer-PlayerList-Info-Item-%d", count).c_str();
		if (GUIWindowManager::Instance()->GetGUIWindow(itemLayout.c_str()) || m_playerItemMap[platformUserId])
		{
			return false;
		}
		GuiPlayerItem* playerItem = (GuiPlayerItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_PLYAER_ITEM, itemLayout.c_str());
		playerItem->setPlayerInfo(GuiPlayerItem::PlayerInfo(platformUserId, userName, teamName, teamId, itemLayout));
		playerItem->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(1.0f, 0), UDim(0, 70.0f));
		playerItem->SetTouchable(false);
		m_playerItemMap[platformUserId] = playerItem;
		if (m_playerList)
		{
			m_playerList->AddItem(playerItem, false);
		}
		else
		{
			m_initBeforeMap[platformUserId] = playerItem;
		}
		return true;
	}

	bool gui_playerList::onUserOut(const ui64 playerPlatformId)
	{
		if (m_playerItemMap[playerPlatformId])
		{
			if (m_playerList)
			{
				m_playerList->DeleteItem(getIndex(m_playerItemMap[playerPlatformId]->GetName().c_str()));
			}
			m_playerItemMap.erase(playerPlatformId);
		}
		return true;
	}

	bool gui_playerList::onFriendOperationForServer(int operationType, ui64 playerPlatformId)
	{
		switch (OperationType(operationType))
		{
		case OperationType::AGREE_ADD_FRIEND:
			if (m_playerItemMap[playerPlatformId])
			{
				m_playerItemMap[playerPlatformId]->setFriend(true);
				m_playerItemMap[playerPlatformId]->setFriendRequest(false);
			}
			break;
		case OperationType::REQUEST_ADD_FRIEND:
			if (m_playerItemMap[playerPlatformId])
			{
				m_playerItemMap[playerPlatformId]->setFriendRequest(true);
			}
			ShowRedPointEvent::emit();
			break;
		}
		return true;
	}

	bool gui_playerList::onFriendOperationForAppHttpResult(int operationType, ui64 playerPlatformId)
	{
		switch (OperationType(operationType))
		{
		case OperationType::NO_FRIEND:
			if (m_playerItemMap[playerPlatformId])
			{
				m_playerItemMap[playerPlatformId]->setFriend(false);
			}
			break;
		case OperationType::IS_FRIEND:
			if (m_playerItemMap[playerPlatformId])
			{
				m_playerItemMap[playerPlatformId]->setFriend(true);
				m_playerItemMap[playerPlatformId]->setFriendRequest(false);
			}
			break;
		case OperationType::AGREE_ADD_FRIEND:
			GameClient::CGame::Instance()->getNetwork()->getSender()->sendFriendOperation(operationType, playerPlatformId);
			if (m_playerItemMap[playerPlatformId])
			{
				m_playerItemMap[playerPlatformId]->setFriend(true);
				m_playerItemMap[playerPlatformId]->setFriendRequest(false);
			}
			break;
		case OperationType::REQUEST_ADD_FRIEND:
			GameClient::CGame::Instance()->getNetwork()->getSender()->sendFriendOperation(operationType, playerPlatformId);
			break;
		case OperationType::REQUEST_ADD_FRIEND_FAILURE:
			m_tipShopTime = 1;
			m_tipMessage = getString(LanguageKey::GUI_PLAYER_LIST_ITEM_ADD_FRIEND_MSG_SEND_FAILURE).c_str();
			if (m_playerItemMap[playerPlatformId])
			{
				m_playerItemMap[playerPlatformId]->resetAddFriendButton();
			}
			break;
		case OperationType::AGREE_ADD_FRIEND_FAILURE:
			m_tipShopTime = 1;
			m_tipMessage = getString(LanguageKey::GUI_PLAYER_LIST_ITEM_ADD_FRIEND_MSG_AGREE_FAILURE).c_str();
			break;
		}
		return true;
	}

	int gui_playerList::getIndex(String viewName)
	{
		if (m_playerList == nullptr || m_playerList->getContainerWindow() == nullptr)
		{
			return -1;
		}
		return  m_playerList->getContainerWindow()->GetChildIndex(viewName.c_str());
	}
}