#pragma once
#ifndef __GUI_KILL_PANEL_HEADER__
#define __GUI_KILL_PANEL_HEADER__

#include "Core.h"
#include "gui_layout.h"
#include "UI/GUILayout.h"
#include "Util/SubscriptionGuard.h"

using namespace LORD;

namespace BLOCKMAN
{
	class KillMsg 
	{
	public:
		int	kills = 1;
		int head = 0;
		int armsId = 0;
		String killer = "";
		String dead = "";
		String color = "";
		ui32 time = 3000;
		GUIWindow* window = nullptr;
	public:
		KillMsg(int kills, int head, int armsId, String killer, String dead, String color) :
			kills(kills), head(head), armsId(armsId), killer(killer), dead(dead), color(color)
		{
			time = 3000;
		}
	};

	class gui_killPanel : public gui_layout
	{
	private:
		static unsigned messageIndex;

		GUILayout* m_killMsgList = nullptr;
		std::list<KillMsg*> m_msgs;
		SubscriptionGuard m_subscriptionGuard;

	private:
		void updateChildrens(ui32 nTimeElapse);
		void addKillMsg(KillMsg* msg);
	public:
		gui_killPanel();
		~gui_killPanel();

		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;

		bool onReceiveKillMsg(const String& data);
	};

}
#endif