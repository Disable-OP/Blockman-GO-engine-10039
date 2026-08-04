#include "GuiRanchCommunityItem.h"
#include "UI/GUIWindowManager.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Network/ClientNetwork.h"
#include "Inventory/Wallet.h"
#include "Util/ClientEvents.h"
#include "Util/UICommon.h"
#include "Network/ClientNetwork.h"
#include "Util/FriendManager.h"

namespace BLOCKMAN
{
	
	void GuiRanchCommunityItem::removeComponents()
	{
		if (m_window)
		{
			RemoveWindowFromDrawList(*m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	vector<GUIWindow*>::type GuiRanchCommunityItem::getPrivateChildren()
	{
		return { m_window };
	}

	GuiRanchCommunityItem::GuiRanchCommunityItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("RanchCommunityItem.json"));
		renameComponents(m_window);

		m_tvDesc = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(0));
		m_prosperityLayout = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(1));
		m_tvProsperityValue = dynamic_cast<GUIStaticText*>(m_prosperityLayout->GetChildByIndex(1));
		m_ivIcon = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(2));
		m_tvName = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(3));

		m_ivBackground = dynamic_cast<GuiUrlImage*>(m_ivIcon->GetChildByIndex(0));
		m_ivGiveIcon = dynamic_cast<GUIStaticImage*>(m_ivIcon->GetChildByIndex(1));
		m_ivLevelIcon = dynamic_cast<GUIStaticImage*>(m_ivIcon->GetChildByIndex(2));
		m_ivHelp = dynamic_cast<GUIStaticImage*>(m_ivIcon->GetChildByIndex(3));

		m_tvGiveNum = dynamic_cast<GUIStaticText*>(m_ivGiveIcon->GetChildByIndex(0));
		m_tvLevel = dynamic_cast<GUIStaticText*>(m_ivLevelIcon->GetChildByIndex(0));

		m_btnSure = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(4));
		m_giveLayout = dynamic_cast<GUILayout*>(m_btnSure->GetChildByIndex(0));
		
		if (LanguageManager::Instance())
		{
			m_btnSure->SetText(LanguageManager::Instance()->getString("gui_ranch_friend_invite").c_str());
			m_tvDesc->SetText(LanguageManager::Instance()->getString("gui_ranch_friend_not_receive").c_str());
			m_btnSure->GetChildByIndex(0)->GetChildByIndex(1)->SetText(LanguageManager::Instance()->getString("gui_ranch_friend_give").c_str());
		}
		m_ivIcon->SetTouchable(false);
		m_ivBackground->subscribeEvent(EventWindowTouchUp, std::bind(&GuiRanchCommunityItem::onClick, this, std::placeholders::_1, ViewId::BTN_ICON));
		m_btnSure->subscribeEvent(EventButtonClick, std::bind(&GuiRanchCommunityItem::onClick, this, std::placeholders::_1, ViewId::BTN_SUER));
		AddWindowToDrawList(*m_window);
	}

	void GuiRanchCommunityItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());
		BaseClass::UpdateSelf(nTimeElapse);
		
		if (!Blockman::Instance() || !Blockman::Instance()->m_pPlayer || !Blockman::Instance()->m_pPlayer->m_ranch->getInfo() || !LanguageManager::Instance())
		{
			return;
		}
		auto m_friend = FriendManager::findUserById(m_friendId);
		if (m_friend)
		{
			refresh(m_friend->hasRanch);
			m_ivHelp->SetVisible(m_friend->needHelp);
			m_tvName->SetText(m_friend->nickName.c_str());
			m_tvLevel->SetText(StringUtil::ToString(m_friend->level).c_str());
			m_btnSure->SetEnabled(!m_friend->isGive);
			m_tvGiveNum->SetText(StringUtil::ToString(m_friend->gitfNum).c_str());
			m_tvProsperityValue->SetText(StringUtil::ToString(m_friend->prosperity).c_str());

			if (m_friend->hasRanch)
			{
				m_btnSure->SetText(m_friend->isGive ? LanguageManager::Instance()->getString("gui_ranch_today_give_gift").c_str() : "");
				m_btnSure->SetEnabled(!m_friend->isGive);
				m_giveLayout->SetVisible(!m_friend->isGive);
			}
			else
			{
				m_btnSure->SetText(LanguageManager::Instance()->getString("gui_ranch_friend_invite").c_str());
				m_btnSure->SetEnabled(true);
				m_giveLayout->SetVisible(false);
			}
		}
	}

	void GuiRanchCommunityItem::refresh(bool hasRanch)
	{
		m_tvDesc->SetVisible(!hasRanch);
		m_ivGiveIcon->SetVisible(hasRanch);
		m_ivLevelIcon->SetVisible(hasRanch);
		m_ivIcon->SetTouchable(hasRanch);
		m_prosperityLayout->SetVisible(hasRanch);
		m_ivBackground->SetTouchable(hasRanch);

	}


	
	void GuiRanchCommunityItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	

	bool GuiRanchCommunityItem::onClick(const EventArgs & args,  ViewId viewId)
	{
		auto m_friend = FriendManager::findUserById(m_friendId);
		if (!m_friend)
		{
			return false;
		}

		switch (viewId)
		{
		case ViewId::BTN_ICON:
			RanchFriendClickEvent::emit(m_friendId, GetName().c_str());
			break;
		case ViewId::BTN_SUER:
			RanchFriendOperateHideEvent::emit();
			if (m_friend->hasRanch)
			{
				ClientNetwork::Instance()->getSender()->sendRanchGiftsOrInvite(1, m_friendId);
			}
			else 
			{
				ClientNetwork::Instance()->getSender()->sendRanchGiftsOrInvite(2, m_friendId);
			}
			break;
		}
		return true;
	}

	void GuiRanchCommunityItem::setFriendId(i64 userId)
	{
		m_friendId = userId;
		auto m_friend = FriendManager::findUserById(m_friendId);
		if (m_friend)
		{
			m_ivBackground->setDefaultImage(m_friend->sex == 1 ? "set:ranch_boy_icon.json image:ranch_boy_icon" : "set:ranch_girl_icon.json image:ranch_girl_icon");
			m_ivBackground->setUrl(m_friend->picUrl);
		}
	}

}