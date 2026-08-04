#pragma once
#ifndef __GUI_MANOR_MESSAGE_HEADER__
#define __GUI_MANOR_MESSAGE_HEADER__

#include "gui_layout.h"
#include "UI/GUIWindow.h"
#include "UI/GUILayout.h"
#include "UI/GUIListBox.h"
#include "GUI/RootGuiLayout.h"
#include "gui_layout.h"
#include <list>

using namespace LORD;

namespace BLOCKMAN
{
	class gui_manorMessage : public  gui_layout
	{
		enum class ViewId
		{
		};

	private:
		SubscriptionGuard m_subscriptionGuard;

	public:
		gui_manorMessage();
		virtual ~gui_manorMessage();
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		RootGuiLayout* getParent() { return dynamic_cast<RootGuiLayout*>(m_parentLayout); }
		bool onClick(const EventArgs & events, ViewId viewId);
		bool manorOnUpdate();
		bool initItem();

	public:
		bool onRemoveClick(const String & messageId, const String& windowName);
		
	private:
		String m_removeWindowName = "";
		GUIListBox* m_viewMessageList = nullptr;
		list<ManorMessage*>::type m_manorMessages;
	};
}

#endif