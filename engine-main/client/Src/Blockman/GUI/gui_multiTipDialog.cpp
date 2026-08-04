#include "gui_multiTipDialog.h"
#include "game.h"
#include "Util/LanguageKey.h"
#include "UI/GUIButton.h"
#include "ShellInterface.h"
#include "rapidjson/writer.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"

using namespace LORD;
using namespace rapidjson;

namespace BLOCKMAN
{

	gui_multiTipDialog::gui_multiTipDialog() :
		gui_layout("MultiTipDialog.json")
	{

	}


	gui_multiTipDialog::~gui_multiTipDialog()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_multiTipDialog::onLoad()
	{
		m_titleText = getWindow<GUIStaticText>("MultiTipDialog-Title-Name");
		m_messageText = getWindow<GUIStaticText>("MultiTipDialog-Content-Message");

		m_upBtn = getWindow<GUIButton>("MultiTipDialog-Btn-Up");
		m_nextBtn = getWindow<GUIButton>("MultiTipDialog-Btn-Next");
		
		m_upBtn->subscribeEvent(EventButtonClick, std::bind(&gui_multiTipDialog::onBtnClick, this, std::placeholders::_1, VIewId::UP));
		m_nextBtn->subscribeEvent(EventButtonClick, std::bind(&gui_multiTipDialog::onBtnClick, this, std::placeholders::_1, VIewId::NEXT));
		getWindow<GUIButton>("MultiTipDialog-Title-Btn-Close")->subscribeEvent(EventButtonClick, std::bind(&gui_multiTipDialog::onClose, this, std::placeholders::_1));

		m_subscriptionGuard.add(ShowMulitTipDialogEvent::subscribe(std::bind(&gui_multiTipDialog::open, this, std::placeholders::_1)));
	}

	void gui_multiTipDialog::onUpdate(ui32 nTimeElapse)
	{
		if (isShown())
		{
			m_upBtn->SetEnabled(m_page > 1);
			m_nextBtn->SetEnabled(size_t(m_page) < m_tips.size());
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

	bool gui_multiTipDialog::onBtnClick(const EventArgs, VIewId viewId)
	{
		switch (viewId)
		{
		case VIewId::UP:
			m_page = m_page - 1;
			break;
		case VIewId::NEXT:
			m_page = m_page + 1;
			break;
		}
		return true;
	}

	bool gui_multiTipDialog::onClose(const EventArgs)
	{
		getParent()->hideMultiTipDialog();
		return true;
	}

	bool gui_multiTipDialog::open(const String & tips)
	{
		m_page = 1;
		parseData(tips);
		getParent()->showMultiTipDialog();
		return true;
	}

	void gui_multiTipDialog::parseData(const String & tips)
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
	}
}
