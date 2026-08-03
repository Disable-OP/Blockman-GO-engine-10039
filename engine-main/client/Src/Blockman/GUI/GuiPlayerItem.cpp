#include "GuiPlayerItem.h"
#include "ShellInterface.h"
#include "game.h"
#include "UI/GUIWindowManager.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"
#include "Setting/GameTypeSetting.h"

namespace BLOCKMAN
{
	void GuiPlayerItem::removeComponents()
	{
		if (m_window)
		{
			RemoveWindowFromDrawList(*m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	vector<GUIWindow*>::type GuiPlayerItem::getPrivateChildren()
	{
		return { m_window };
	}

	GuiPlayerItem::GuiPlayerItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("MenuPlayerItem.json"));
		renameComponents(m_window);
		m_friendIcon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		m_playerName = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(1));
		m_messageView = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(2));
		m_btnIgnore = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(3));
		m_btnAgree = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(4));
		m_btnAddFriend = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(5));
		m_btnCallOn = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(6));
		m_btnIgnore->subscribeEvent(EventButtonClick, std::bind(&GuiPlayerItem::onClick, this, std::placeholders::_1, ViewId::BTN_NEGLECT));
		m_btnAgree->subscribeEvent(EventButtonClick, std::bind(&GuiPlayerItem::onClick, this, std::placeholders::_1, ViewId::BTN_AGREE));
		m_btnAddFriend->subscribeEvent(EventButtonClick, std::bind(&GuiPlayerItem::onClick, this, std::placeholders::_1 ,ViewId::BTN_ADD_FRIEND));
		m_btnCallOn->subscribeEvent(EventButtonClick, std::bind(&GuiPlayerItem::onClick, this, std::placeholders::_1, ViewId::BTN_CALL_ON));
		m_btnAddFriend->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_PLAYER_LIST_ITEM_ADD_FRIEND).c_str());
		m_btnAgree->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_PLAYER_LIST_ITEM_ADD_FRIEND_BTN_AGREE).c_str());
		m_btnIgnore->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_PLAYER_LIST_ITEM_ADD_FRIEND_BTN_IGNORE).c_str());
		m_btnCallOn->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_MANOR_CALL_ON).c_str());
		m_messageView->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_PLAYER_LIST_ITEM_ADD_FRIEND_MSG).c_str());
		AddWindowToDrawList(*m_window);
	}

	void GuiPlayerItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());
		BaseClass::UpdateSelf(nTimeElapse);
		
		if (m_playerInfo.userId == GameClient::CGame::Instance()->getPlatformUserId())
		{
			m_playerName->SetText(StringUtil::Format("%s(me)", m_playerInfo.userName.c_str()).c_str());
		}
		else
		{
			m_playerName->SetText(m_playerInfo.userName.c_str());
		}
		
		m_friendIcon->SetVisible(m_playerInfo.isFriend && m_playerInfo.userId != GameClient::CGame::Instance()->getPlatformUserId());
		m_btnCallOn->SetVisible(isShowCallOnbtn());
		if (m_playerInfo.isFriend)
		{
			m_playerName->SetTextColor(Color(0.647059f, 0.913726f, 0.364706f));	
		} 
		else
		{
			m_playerName->SetTextColor(isFriendRequest ? Color(1.0f, 0.235294f, 0.196078f) : Color(0.92549f, 0.870588f, 0.788235f));
		}
		m_messageView->SetVisible(isFriendRequest && m_playerInfo.userId != GameClient::CGame::Instance()->getPlatformUserId());
		m_btnIgnore->SetVisible(isFriendRequest  && m_playerInfo.userId != GameClient::CGame::Instance()->getPlatformUserId());
		m_btnAgree->SetVisible(isFriendRequest && m_playerInfo.userId != GameClient::CGame::Instance()->getPlatformUserId());
		m_btnAddFriend->SetVisible(!m_playerInfo.isFriend && !isFriendRequest  && m_playerInfo.userId != GameClient::CGame::Instance()->getPlatformUserId());
		
	}

	void GuiPlayerItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	bool GuiPlayerItem::onClick(const EventArgs & events, ViewId viewId)
	{
		switch (viewId)
		{
		case ViewId::BTN_NEGLECT:
			isFriendRequest = false;
			m_btnAddFriend->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_PLAYER_LIST_ITEM_ADD_FRIEND).c_str());
			m_btnAddFriend->SetEnabled(true);
			break;
		case ViewId::BTN_AGREE:
			GameClient::CGame::Instance()->getShellInterface()->onFriendOperation(1, m_playerInfo.userId);
			break;
		case ViewId::BTN_ADD_FRIEND:
			GameClient::CGame::Instance()->getShellInterface()->onFriendOperation(2, m_playerInfo.userId);
			m_btnAddFriend->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_PLAYER_LIST_ITEM_ADD_FRIEND_MSG_SENT).c_str());
			m_btnAddFriend->SetEnabled(false);
			break;

		case ViewId::BTN_CALL_ON:
			GameClient::CGame::Instance()->getNetwork()->getSender()->sendCallOnManor(m_playerInfo.userId);
			break;
		}
		return true;
	}

	void GuiPlayerItem::resetAddFriendButton()
	{
		m_btnAddFriend->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_PLAYER_LIST_ITEM_ADD_FRIEND).c_str());
		m_btnAddFriend->SetEnabled(true);
	}
	bool GuiPlayerItem::isShowCallOnbtn()
	{
		return GameClient::CGame::Instance()->GetGameType() == ClientGameType::Tiny_Town && m_playerInfo.isFriend && m_playerInfo.userId != GameClient::CGame::Instance()->getPlatformUserId();
	}
}