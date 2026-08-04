#include "gui_killPanel.h"
#include "GUIKillMsgItem.h"
#include "GUI/GuiDef.h"
#include "Util/ClientEvents.h"

namespace BLOCKMAN
{
	unsigned gui_killPanel::messageIndex = 0;

	gui_killPanel::gui_killPanel()
		: gui_layout("KillPanel.json")
	{
	}

	gui_killPanel::~gui_killPanel()
	{
	}

	void gui_killPanel::onLoad()
	{
		m_killMsgList = getWindow<GUILayout>("KillPanel-Kill-Msg-List");
		m_subscriptionGuard.add(ReceiveKillMsgEvent::subscribe(std::bind(&gui_killPanel::onReceiveKillMsg, this, std::placeholders::_1)));
	}

	void gui_killPanel::onUpdate(ui32 nTimeElapse)
	{
		updateChildrens(nTimeElapse);
	}

	bool gui_killPanel::onReceiveKillMsg(const String & data)
	{
		if (data.length() == 0)
			return false;
		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(data.c_str());

		if (doc->HasParseError())
		{
			LordLogError("The kill msg content  HasParseError.");
			LordDelete(doc);
			return false;
		}

		int kills = doc->HasMember("kills") ? doc->FindMember("kills")->value.GetInt() : 0;
		int head = doc->HasMember("head") ? doc->FindMember("head")->value.GetInt() : 0;
		int armsId = doc->HasMember("armsId") ? doc->FindMember("armsId")->value.GetInt() : 0;
		String killer = doc->HasMember("killer") ? doc->FindMember("killer")->value.GetString() : "";
		String dead = doc->HasMember("dead") ? doc->FindMember("dead")->value.GetString() : "";
		String color = doc->HasMember("color") ? doc->FindMember("color")->value.GetString() : "";

		addKillMsg(new KillMsg(kills, head, armsId, killer, dead, color));
		LordDelete(doc);
		return true;
	}

	void gui_killPanel::updateChildrens(ui32 nTimeElapse)
	{
		if (m_msgs.size() == 0)
			return;
		bool isNeedUpdatePosition = false;
		for (auto iter = m_msgs.begin(); iter != m_msgs.end(); )
		{
			(*iter)->time = (*iter)->time > nTimeElapse ? (*iter)->time - nTimeElapse : 0;
			if ((*iter)->time == 0)
			{
				if ((*iter)->window)
				{
					m_killMsgList->RemoveChildWindow((*iter)->window);
				}
				LordDelete *iter;
				iter = m_msgs.erase(iter);
				isNeedUpdatePosition = true;
			}
			else
			{
				++iter;
			}
		}
		if (isNeedUpdatePosition)
		{
			const size_t child_count = m_killMsgList->GetChildCount();
			for (size_t i = 0; i < child_count; ++i)
			{
				GUIWindow* window = m_killMsgList->GetChildByIndex(i);
				window->SetYPosition({ 0, i * 50.0f });
			}
		}
	}

	void gui_killPanel::addKillMsg(KillMsg * msg)
	{
		m_msgs.push_back(msg);
		messageIndex++;
		GUIString itemName = StringUtil::Format("KillPanel-Item-%d", messageIndex).c_str();
		GUIKillMsgItem* pKillMsgItem = (GUIKillMsgItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_KILL_MSG_ITEM, itemName);
		pKillMsgItem->SetWidth({ 1, 0.0f });
		pKillMsgItem->SetHeight({ 0, 50.f });
		pKillMsgItem->SetYPosition({ 0, m_killMsgList->GetChildCount() * 50.0f });
		pKillMsgItem->setKillMsg(msg);
		m_killMsgList->AddChildWindow(pKillMsgItem);
		msg->window = pKillMsgItem;
	}

}