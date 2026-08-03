#pragma once
#ifndef __GUI_PLAYER_OPERATION_HEADER__
#define __GUI_PLAYER_OPERATION_HEADER__

#include "gui_layout.h"
#include "UI/GUIWindow.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIButton.h"
#include "Util/SubscriptionGuard.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_playerOperation : public  gui_layout
	{
		enum class ViewId
		{
			BTN_ADD_FRIEND,
			BTN_GIVE_ROSE,
			BTN_CALL_ON,
			BTN_GREETINGS
		};

	private:
		SubscriptionGuard m_subscriptionGuard;

	public:
		gui_playerOperation();
		virtual ~gui_playerOperation();
		void onLoad() override;
		void onShow() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool onClick(const EventArgs & events, ViewId viewId);
		void setTargetUserId(ui64 targetId) {
			this->m_targetUserId = targetId;
		}
		void setTargetUserName(String name) {
			m_targetUserName = name;
		}

	private:
		bool onGameTypeUpdate();

	private:
		ui64 m_targetUserId = -1;
		String m_targetUserName = "";
	};
}

#endif