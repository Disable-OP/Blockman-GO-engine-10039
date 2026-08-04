#include "GuiRanchMailItem.h"
#include "UI/GUIWindowManager.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Network/ClientNetwork.h"
#include "Inventory/Wallet.h"
#include "Util/ClientEvents.h"
#include "Util/UICommon.h"
#include "Util/FriendManager.h"
#include "rapidjson/writer.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/document.h"
#include "game.h"

using namespace rapidjson;

namespace BLOCKMAN
{
	
	void GuiRanchMailItem::removeComponents()
	{
		if (m_window)
		{
			RemoveWindowFromDrawList(*m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	vector<GUIWindow*>::type GuiRanchMailItem::getPrivateChildren()
	{
		return { m_window };
	}

	GuiRanchMailItem::GuiRanchMailItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("RanchMailItem.json"));
		renameComponents(m_window);

		m_tvTitle = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(0));
		m_prosperityLayout = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(1));
		m_tvProsperityValue = dynamic_cast<GUIStaticText*>(m_prosperityLayout->GetChildByIndex(1));
		m_tvContent = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(2));
		m_ivIcon = dynamic_cast<GuiUrlImage*>(m_window->GetChildByIndex(3));
		m_tvName = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(4));

		m_ivGiveIcon = dynamic_cast<GUIStaticImage*>(m_ivIcon->GetChildByIndex(0));
		m_ivLevelIcon = dynamic_cast<GUIStaticImage*>(m_ivIcon->GetChildByIndex(1));

		m_tvGiveNum = dynamic_cast<GUIStaticText*>(m_ivGiveIcon->GetChildByIndex(0));
		m_tvLevel = dynamic_cast<GUIStaticText*>(m_ivLevelIcon->GetChildByIndex(0));

		m_btnSure = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(5));
		m_giveLayout = dynamic_cast<GUILayout*>(m_btnSure->GetChildByIndex(0));
		
		if (LanguageManager::Instance())
		{
			m_tvTitle->SetText(LanguageManager::Instance()->getString("gui_ranch_mail_item_title_exploration_rewards").c_str());
			m_btnSure->GetChildByIndex(0)->GetChildByIndex(1)->SetText(LanguageManager::Instance()->getString("gui_ranch_mail_item_receive").c_str());
		}

		m_btnSure->subscribeEvent(EventButtonClick, std::bind(&GuiRanchMailItem::onClick, this, std::placeholders::_1, ViewId::BTN_SUER));
		AddWindowToDrawList(*m_window);
	}

	void GuiRanchMailItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());
		BaseClass::UpdateSelf(nTimeElapse);
		
		if (!Blockman::Instance() || !Blockman::Instance()->m_pPlayer || !Blockman::Instance()->m_pPlayer->m_ranch->getInfo() || !LanguageManager::Instance())
		{
			return;
		}

		auto m_friend = FriendManager::findUserById(m_mail.fromUser);
		if (m_mail.mailType == 5 && m_friend)
		{
			m_tvName->SetText(m_friend->nickName.c_str());
			m_tvLevel->SetText(StringUtil::ToString(m_friend->level).c_str());
			m_tvGiveNum->SetText(StringUtil::ToString(m_friend->gitfNum).c_str());
			m_tvProsperityValue->SetText(StringUtil::ToString(m_friend->prosperity).c_str());
			m_ivIcon->setUrl(m_friend->picUrl);
			m_ivIcon->setDefaultImage(m_friend->sex == 1 ? "set:ranch_boy_icon.json image:ranch_boy_icon" : "set:ranch_girl_icon.json image:ranch_girl_icon");
			m_ivIcon->SetVisible(true);
			m_tvName->SetVisible(true);
			m_tvTitle->SetVisible(false );
			m_tvContent->SetVisible(false);
			m_prosperityLayout->SetVisible(true);
		}
		else 
		{
			m_ivIcon->SetVisible(false);
			m_tvName->SetVisible(false);
			m_tvTitle->SetVisible(true);
			m_tvContent->SetVisible(true);
			m_prosperityLayout->SetVisible(false);
		}
	}

	void GuiRanchMailItem::refresh(bool showUserInfo)
	{
		m_ivIcon->SetVisible(showUserInfo);
		m_tvName->SetVisible(showUserInfo);
		m_tvTitle->SetVisible(!showUserInfo);
		m_tvContent->SetVisible(!showUserInfo);
		m_prosperityLayout->SetVisible(showUserInfo);
	}

	void GuiRanchMailItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	bool GuiRanchMailItem::onClick(const EventArgs & args,  ViewId viewId)
	{
		switch (viewId)
		{
		case ViewId::BTN_SUER:
			if (m_mail.id > 0)
			{
				ClientNetwork::Instance()->getSender()->sendReceiveMailReward(m_mail.id);
			}
			break;
		}
		return true;
	}

	void GuiRanchMailItem::setMail(const MailInfo & mailInfo)
	{
		m_mail = mailInfo;
		parseContent(m_mail.content);
		m_tvTitle->SetText(LanguageManager::Instance()->getString(m_mail.title).c_str());
	}

	void GuiRanchMailItem::parseContent(String data)
	{
		Document  doc = Document();
		doc.Parse(data.c_str());
		if (!doc.HasParseError() && doc.IsObject())
		{
			if (doc.HasMember("msg") && !doc.FindMember("msg")->value.IsNull())
			{
				String key = doc.FindMember("msg")->value.GetString();
				String message = LanguageManager::Instance()->getString(key);
				String name = "Blockman Go User";
				if (m_mail.fromUser == GameClient::CGame::Instance()->getPlatformUserId() || m_mail.mailType == 1)
				{
					name = GameClient::CGame::Instance()->getPlayerName();
				}
				else
				{
					auto pal = FriendManager::findUserById(m_mail.fromUser);
					name = pal ? pal->nickName : name;
				}

				m_tvContent->SetText(StringUtil::Format(message.c_str(), name.c_str()).c_str());
			}
		}
	}
}