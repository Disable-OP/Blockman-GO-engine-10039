#include "gui_birdTaskTip.h"
#include "RootGuiLayout.h"
#include "rapidjson/writer.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "Util/UICommon.h"
#include "Network/ClientNetwork.h"

using namespace LORD;
using namespace rapidjson;

namespace BLOCKMAN
{

	gui_birdTaskTip::gui_birdTaskTip() :
		gui_layout("BirdTaskTip.json")
	{
	}


	gui_birdTaskTip::~gui_birdTaskTip()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_birdTaskTip::onLoad()
	{
		m_titleText = getWindow<GUIStaticText>("BirdTaskTip-Title-Name");
		m_messageText = getWindow<GUIStaticText>("BirdTaskTip-Content");
		getWindow("BirdTaskTip-Panel")->subscribeEvent(EventWindowTouchUp, std::bind(&gui_birdTaskTip::onClick, this, std::placeholders::_1));
		m_subscriptionGuard.add(ShowBirdTaskTipEvent::subscribe(std::bind(&gui_birdTaskTip::open, this, std::placeholders::_1)));
	}

	void gui_birdTaskTip::onUpdate(ui32 nTimeElapse)
	{
		if (isShown())
		{
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

	bool gui_birdTaskTip::onClick(const EventArgs)
	{
		if (m_page >= (int) m_tips.size())
		{
			if (m_entityId > 0)
			{
				ClientNetwork::Instance()->getSender()->sendBirdTask(m_entityId, m_taskId);
			}
			RootGuiLayout::Instance()->setBirdTaskTipVisible(false);
		}
		else
		{
			m_page += 1;
		}
		return true;
	}


	bool gui_birdTaskTip::open(const String & tips)
	{
		m_page = 1;
		parseData(tips);
		RootGuiLayout::Instance()->setBirdTaskTipVisible(true);
		return true;
	}

	void gui_birdTaskTip::parseData(const String & tips)
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
			m_sessionType = (SessionType)doc.FindMember("interactionType")->value.GetInt();
		}
		else 
		{
			m_sessionType = SessionType::DEFAULT;
		}

		if (doc.HasMember("entityId"))
		{
			m_entityId = doc.FindMember("entityId")->value.GetInt();
		}

		if (doc.HasMember("taskId"))
		{
			m_taskId = doc.FindMember("taskId")->value.GetInt();
		}
	}
}
