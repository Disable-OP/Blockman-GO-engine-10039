#include "gui_multiTipInteractionDialog.h"
#include "Util/LanguageKey.h"
#include "UI/GUIButton.h"
#include "ShellInterface.h"
#include "rapidjson/writer.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "World/Ranch.h"
#include "Util/UICommon.h"
#include "game.h"
#include "Inventory/InventoryPlayer.h"

using namespace LORD;
using namespace rapidjson;

namespace BLOCKMAN
{

	gui_multiTipInteractionDialog::gui_multiTipInteractionDialog() :
		gui_layout("MultiTipInteractionDialog.json")
	{
	}


	gui_multiTipInteractionDialog::~gui_multiTipInteractionDialog()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_multiTipInteractionDialog::onLoad()
	{
		m_titleText = getWindow<GUIStaticText>("MultiTipInteractionDialog-Title-Name");
		m_messageText = getWindow<GUIStaticText>("MultiTipInteractionDialog-Content");

		
		m_nextBtn = getWindow<GUIButton>("MultiTipInteractionDialog-Btn-Next");
		m_interactionBtn = getWindow<GUIButton>("MultiTipInteractionDialog-Btn-Interaction");

		m_interactionBtn->SetText(getString(LanguageKey::GUI_DIALOG_TIP_BTN_SURE));
		m_nextBtn->subscribeEvent(EventButtonClick, std::bind(&gui_multiTipInteractionDialog::onBtnClick, this, std::placeholders::_1, VIewId::NEXT));
		m_interactionBtn->subscribeEvent(EventButtonClick, std::bind(&gui_multiTipInteractionDialog::onBtnClick, this, std::placeholders::_1, VIewId::INTERACTION));
		getWindow<GUIStaticText>("MultiTipInteractionDialog-Btn-Next-Text")->SetText(getString(LanguageKey::GUI_BTN_CLICK_CONTINUE));
		getWindow<GUIButton>("MultiTipInteractionDialog-Title-Btn-Close")->subscribeEvent(EventButtonClick, std::bind(&gui_multiTipInteractionDialog::onClose, this, std::placeholders::_1));

		m_subscriptionGuard.add(ShowMulitTipInteractionDialogEvent::subscribe(std::bind(&gui_multiTipInteractionDialog::open, this, std::placeholders::_1)));
	}

	void gui_multiTipInteractionDialog::onUpdate(ui32 nTimeElapse)
	{
		if (isShown())
		{
			m_nextBtn->SetVisible(size_t(m_page) < m_tips.size());
			m_interactionBtn->SetVisible(size_t(m_page) == m_tips.size());
			if (size_t(m_page) >= m_tips.size())
			{
				m_rootWindow->removeAllEvents();
			}
			i32  i = 0;
			for (String& tip :  m_tips)
			{
				i++;
				if (i == m_page)
				{
					m_messageText->SetText(getString(tip.c_str()));
					break;
				}
			}
		}
	}

	bool gui_multiTipInteractionDialog::onBtnClick(const EventArgs, VIewId viewId)
	{
		switch (viewId)
		{
		case VIewId::NEXT:
			m_page = m_page + 1;
			
			break;
		case VIewId::INTERACTION:
			interaction();
			break;
		}
		return true;
	}

	bool gui_multiTipInteractionDialog::onClose(const EventArgs)
	{
		getParent()->hideMultiTipInteractionDialog();
		return true;
	}

	bool gui_multiTipInteractionDialog::open(const String & tips)
	{
		m_page = 1;
		parseData(tips);
		getParent()->showMultiTipInteractionDialog();
		return true;
	}

	void gui_multiTipInteractionDialog::interaction()
	{
		switch (m_interactionType) {
		case InteractionType::DEFAULT:
			break;
		case InteractionType::RANCH_RECEIVE:
			ClientNetwork::Instance()->getSender()->sendRanchReceive(GameClient::CGame::Instance()->getInviter());
			break;
		case InteractionType::RANCH_DESTROYHOUSE:
			ClientNetwork::Instance()->getSender()->sendRanchDestroyHouse();
			break;
		case InteractionType::RANCHERS_EXPLORE_BEGIN:
			ClientNetwork::Instance()->getSender()->sendRanchExBegin();
			break;
		case InteractionType::RANCHERS_TASK_FINISH:

			if (Blockman::Instance()
				&& Blockman::Instance()->m_pPlayer
				&& Blockman::Instance()->m_pPlayer->inventory
				&& Blockman::Instance()->m_pPlayer->inventory->getCurrentItem())
			{
				ClientNetwork::Instance()->getSender()->sendRanchExTaskFinish(Blockman::Instance()->m_pPlayer->inventory->getCurrentItem()->itemID,
					Blockman::Instance()->m_pPlayer->inventory->getCurrentItem()->getItemStackInfo().num);
			}

			break;
		case InteractionType::RANCH_GO_EXPLORE:
			if (UICommon::checkRanchParam() && Blockman::Instance()->m_pPlayer->m_ranch->getInfo())
			{
				const auto & ranchInfo = Blockman::Instance()->m_pPlayer->m_ranch->getInfo();
				if (ranchInfo->level >= ranchInfo ->exploreStartLevel)
				{
					ClientNetwork::Instance()->getSender()->sendRanchGoExplore();
				}
				else 
				{
					String str = StringUtil::Format(getString("gui_ranch_explore_start_level").c_str(), ranchInfo->exploreStartLevel);
					ShowRanchCommonTipEvent::emit(str);
				}
			}
			else
			{
				ShowRanchCommonTipEvent::emit(getString("gui_ranch_explore_not_has_manor").c_str());
			}
			break;

		case InteractionType::BRID_TASK:
			if (m_taskId > 0 && m_entityId > 0)
			{
				ClientNetwork::Instance()->getSender()->sendBirdTask(m_entityId, m_taskId);
			}
			break;
		default:
			break;
		}
		getParent()->hideMultiTipInteractionDialog();
	}

	void gui_multiTipInteractionDialog::parseData(const String & tips)
	{
		Document doc = Document();
		doc.Parse(tips.c_str());
		if (doc.HasParseError())
		{
			return;
		}

		if (doc.HasMember("title"))
		{
			m_titleText->SetText(getString(doc.FindMember("title")->value.GetString()));
		}

		if (doc.HasMember("tips"))
		{
			auto content = doc.FindMember("tips")->value.GetArray();
			m_tips.clear();
			for (const auto& item : content)
			{
				m_tips.push_back(item.GetString());
			}
		}

		if (doc.HasMember("interactionType"))
		{
			m_interactionType = (InteractionType)doc.FindMember("interactionType")->value.GetInt();
		}
		else 
		{
			m_interactionType = InteractionType::DEFAULT;
		}

		if (doc.HasMember("sureContent"))
		{
			m_interactionBtn->SetText(getString(doc.FindMember("sureContent")->value.GetString()));
		}

		if (doc.HasMember("entityId"))
		{
			m_entityId = doc.FindMember("entityId")->value.GetInt();
		}

		if (doc.HasMember("taskId"))
		{
			m_taskId = doc.FindMember("taskId")->value.GetInt();
		}

		if (m_interactionType == InteractionType::RANCH_DESTROYHOUSE)
		{
			m_interactionBtn->SetText("");
			m_interactionBtn->SetNormalImage("set:ranch_build.json image:remove");
			m_interactionBtn->SetPushedImage("set:ranch_build.json image:remove");
			m_interactionBtn->SetSize(UVector2(UDim(0, 105.f), UDim(0, 61.f)));
			m_interactionBtn->SetPosition(UVector2(UDim(0, 0.f), UDim(0, -20.f)));
			m_interactionBtn->SetHorizontalAlignment(HA_CENTRE);
		}
		else
		{
			m_interactionBtn->SetNormalImage("set:ranch_main.json image:ranch_btn_sure");
			m_interactionBtn->SetPushedImage("set:ranch_main.json image:ranch_btn_sure");
			m_interactionBtn->SetSize(UVector2(UDim(0, 100.f), UDim(0, 40.f)));
			m_interactionBtn->SetPosition(UVector2(UDim(0, -20.f), UDim(0, -20.f)));
			m_interactionBtn->SetHorizontalAlignment(HA_RIGHT);
			setText(m_interactionBtn, LanguageKey::GUI_DIALOG_TIP_BTN_SURE);
		}
		m_rootWindow->removeAllEvents();
		if (size_t(m_page) < m_tips.size())
		{
			m_rootWindow->subscribeEvent(EventWindowTouchUp, std::bind(&gui_multiTipInteractionDialog::onBtnClick, this, std::placeholders::_1, VIewId::NEXT));
		}
	}
}
