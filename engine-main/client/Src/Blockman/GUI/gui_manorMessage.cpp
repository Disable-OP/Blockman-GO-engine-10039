#include "gui_manorMessage.h"
#include "Util/LanguageKey.h"
#include "GuiManorMessageItem.h"
#include "GuiDef.h"
#include "Util/ClientEvents.h"
#include "cWorld/Blockman.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"

using namespace LORD;

namespace BLOCKMAN 
{

	gui_manorMessage::gui_manorMessage()
		: gui_layout("ManorMessage.json")
	{
		
	}

	gui_manorMessage::~gui_manorMessage()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_manorMessage::onLoad()
	{
		m_viewMessageList = (GUIListBox*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_LIST, "ManorMessage-Message-List");
		m_viewMessageList->SetArea(UDim(0, 22.f), UDim(0, 22.f), UDim(1.f, -44.0f), UDim(1.f, -44.0f));
		getWindow("ManorMessage")->AddChildWindow(m_viewMessageList);
		m_viewMessageList->SetAllowScroll(true);
		initItem();
		m_subscriptionGuard.add(ManorOnUpdateEvent::subscribe(std::bind(&gui_manorMessage::manorOnUpdate, this)));
		m_subscriptionGuard.add(ManorMessageRemoveEvent::subscribe(std::bind(&gui_manorMessage::onRemoveClick, this, std::placeholders::_1,  std::placeholders::_2)));
	}

	void gui_manorMessage::onUpdate(ui32 nTimeElapse)
	{
		if (m_removeWindowName.length() > 0)
		{
			ui32 index = m_viewMessageList->getContainerWindow()->GetChildIndex(m_removeWindowName.c_str());
			m_viewMessageList->DeleteItem(index);
			m_removeWindowName = "";
		}
	}

	bool gui_manorMessage::onClick(const EventArgs & events, ViewId viewId)
	{
	
		return true;
	}

	bool gui_manorMessage::manorOnUpdate()
	{
		
		if (Blockman::Instance()->m_pPlayer->m_manor)
		{
			m_manorMessages = Blockman::Instance()->m_pPlayer->m_manor->getMessages();
		}
		initItem();
		return true;
	}

	bool gui_manorMessage::initItem()
	{
		m_viewMessageList->ClearAllItem();
		for (ManorMessage* item : m_manorMessages)
		{
			static unsigned count = 0;
			count++;
			String itemLayout = StringUtil::Format("ManorMessage-Message-Item-%d", count).c_str();
			if (GUIWindowManager::Instance()->GetGUIWindow(itemLayout.c_str()))
			{
				return false;
			}
			GuiManorMessageItem* messageItem = (GuiManorMessageItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_MANOR_MESSAGE_ITEM, itemLayout.c_str());
			messageItem->setManorMessage(item);
			messageItem->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(1.0f, 0.f), UDim(0.0f, 77.f));
			m_viewMessageList->AddItem(messageItem, false);
		}
		return true;
	}

	bool gui_manorMessage::onRemoveClick(const String &  mesageId, const String& windowName)
	{
		m_removeWindowName = windowName;
		Blockman::Instance()->m_pPlayer->m_manor->removeMessage(Blockman::Instance()->m_pPlayer->m_manor ->findMessageById(mesageId));
		return true;
	}
}
