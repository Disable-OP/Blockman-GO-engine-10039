#pragma once
#ifndef __GUI_CHAT_HEADER__
#define __GUI_CHAT_HEADER__
#include "gui_layout.h"
#include "UI/GUIButton.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIListBox.h"
#include "UI/GUIEditBox.h"
#include "GUI/RootGuiLayout.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_chat : public gui_layout
	{
	public:
		 gui_chat();
		 virtual ~gui_chat();
		 void onLoad() override;
		 void onUpdate(ui32 nTimeElapse) override;
		 bool addChatMessage(const String& message);
	private:
		bool onBackClick(const EventArgs & events);
		bool onSendClick(const EventArgs & events);
		bool onInputChanged(const EventArgs & events);
		void scrollBottom();
		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}

	private:
		int m_messageCount = 0;
		GUIListBox* m_chatMessageListBox = nullptr;
		GUIEditBox* m_InputBox = nullptr;
        SubscriptionGuard m_subscriptionGuard;
	};

}
#endif
