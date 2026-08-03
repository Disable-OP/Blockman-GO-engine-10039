#include "gui_chat.h"
#include "gui_layout.h"
#include "UI/GUIButton.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIEditBox.h"
#include "GUI/RootGuiLayout.h"
#include "Network/ClientNetwork.h"
#include "Network/ClientPacketSender.h"
#include "Util/LanguageKey.h"
#include "Util/ClientCmdMgr.h"
#include "Setting/FilterWord.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Render/RenderEntity.h"
#include "Actor/ActorObject.h"
#include "EntityRenders/EntityRenderManager.h"
#include "Render/RenderGlobal.h"

namespace BLOCKMAN {
	gui_chat::gui_chat() :
		gui_layout("Chat.json")
	{
	}

	gui_chat::~gui_chat()
	{
        m_subscriptionGuard.unsubscribeAll();
	}

	void gui_chat::onLoad()
	{
		getWindow<GUIButton>("Chat-BtnBack")->subscribeEvent(EventButtonClick, std::bind(&gui_chat::onBackClick, this, std::placeholders::_1));
		getWindow<GUIButton>("Chat-BtnSend")->subscribeEvent(EventButtonClick, std::bind(&gui_chat::onSendClick, this, std::placeholders::_1));

		m_InputBox = getWindow<GUIEditBox>("Chat-Input-Box");
		m_InputBox->subscribeEvent(EventWindowTextChanged, std::bind(&gui_chat::onInputChanged, this, std::placeholders::_1));

		m_chatMessageListBox = getWindow<GUIListBox>("Chat-Content");
		m_subscriptionGuard.add(ChatMessageReceiveEvent::subscribe(std::bind(&gui_chat::addChatMessage, this, std::placeholders::_1)));
		m_chatMessageListBox->SetAllowScroll(true);
		m_chatMessageListBox->SetTouchable(true);
		m_InputBox->SetBlinkshow(true);
	}

	void gui_chat::onUpdate(ui32 nTimeElapse)
	{

	}

	bool gui_chat::onInputChanged(const EventArgs & events)
	{
		std::string text = m_InputBox->GetText().c_str();
		FilterWord::replace(text);
		m_InputBox->SetText(text.c_str());
		return true;
	}

	bool gui_chat::onBackClick(const EventArgs & events)
	{
		getParent()->showMainControl();
		//playSonud("random.click");
		playSoundByType(ST_Click);
		return true;
	}

	bool gui_chat::onSendClick(const EventArgs & events)
	{
		if (m_InputBox->GetText().length() > 0)
		{
			// client command for debug rendering.
			String msg = m_InputBox->GetText().c_str();
			if (msg.find("time") == 0)
			{
				StringArray args = StringUtil::Split(msg, " ");
				if (args.size() == 2)
				{
					int acctime = StringUtil::ParseInt(args[1]);
					Blockman::Instance()->m_accelerateTime = acctime;
				}
			}

			else if (msg.find("drawmask") == 0)
			{
				Blockman::Instance()->m_globalRender->enableMarkRender(true);
				Blockman::Instance()->m_globalRender->setMarkRenderArea(Vector3(4.0f, 57.1f, -15.0f), Vector2(4.0f, 2.0f), Color(0.3f, 1.0f, 0.4f, 0.5f));
				return true;
			}

			else if (msg.find("debug") == 0)
			{
				StringArray args = StringUtil::Split(msg, " ");
				if (args.size() == 2)
				{
					if (args[1] == "1")
						Blockman::Instance()->getWorld()->changePlayerActor((EntityPlayer*)Blockman::Instance()->renderViewEntity, "boy.actor", "1");
					else
						Blockman::Instance()->getWorld()->changePlayerActor((EntityPlayer*)Blockman::Instance()->renderViewEntity, "girl.actor", "2");
				}
				playSoundByType(ST_Click);
				return true;
			}

			else if (msg.find("attack") == 0)
			{
				ActorObject* actor = EntityRenderManager::Instance()->getEntityActor(Blockman::Instance()->renderViewEntity);
				if (!actor)
					return true;
				actor->PlaySkill("attack");
			}

			else if (msg.find("bag") == 0)
			{
				EntityPlayer* pPlayer = (EntityPlayer*)Blockman::Instance()->renderViewEntity;
				pPlayer->m_bagId = 1;
				pPlayer->m_outLooksChanged = true;
			}

			else if (msg.find("engineFlag") == 0)
			{
				ActorObject* actor = EntityRenderManager::Instance()->getEntityActor(Blockman::Instance()->renderViewEntity);
				if (actor)
				{
					StringArray args = StringUtil::Split(msg, " ");
					if (args.size() == 2)
					{
						if (args[1] == "1")
							actor->AttachSelectEffect("sampleFlagEff.effect");
						else
							actor->DetachSelectEffect();
					}
				}
			}

			if (!ClientCmdMgr::consumeCmd(m_InputBox->GetText().c_str())) {
				ClientNetwork::Instance()->getSender()->sendChatString(m_InputBox->GetText().c_str());
			}
			m_InputBox->SetText("");
			//playSonud("random.click");
			playSoundByType(ST_Click);
		}
		return true;
	}

	bool gui_chat::addChatMessage(const String&  message)
	{
		m_messageCount = m_messageCount + 1;
		String strTextName = StringUtil::Format("Chat-Content-List-item-%d", m_messageCount).c_str();
		GUIStaticText* pStaticText = (GUIStaticText*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_TEXT, strTextName.c_str());
		pStaticText->SetText(message.c_str());
		pStaticText->SetWidth(UDim(1, 0));
		pStaticText->SetHeight(UDim(0, 30));
		pStaticText->SetTouchable(false);
		pStaticText->SetTextVertAlign(VA_CENTRE);
		pStaticText->SetYPosition(UDim(0, 0));
		pStaticText->SetTextScale(1.2f);
		m_chatMessageListBox->AddItem(pStaticText, true);
		scrollBottom();
		return true;
	}

	void gui_chat::scrollBottom()
	{
		if (m_chatMessageListBox->getContainerWindow() && m_chatMessageListBox->GetPixelSize().y - m_chatMessageListBox->getContainerWindow()->GetPixelSize().y < 0)
		{
			m_chatMessageListBox->SetScrollOffset(m_chatMessageListBox->GetPixelSize().y - m_chatMessageListBox->getContainerWindow()->GetPixelSize().y);
		}
	}
}
