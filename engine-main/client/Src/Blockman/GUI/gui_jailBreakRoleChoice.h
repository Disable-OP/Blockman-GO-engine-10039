#pragma once
#ifndef __GUI_JAIL_BREAK_ROLE_CHOICE_HEADER__
#define __GUI_JAIL_BREAK_ROLE_CHOICE_HEADER__
#include "Core.h"
#include <list>
#include "GUI/gui_layout.h"
#include "GUI/RootGuiLayout.h"
#include "Structs/ChoiceRole.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_jailBreakRoleChoice : public  gui_layout
	{

		enum class VIewId
		{
			BTN_POLICE,
			BTN_BANDIT,
			BTN_CLOSE,
		};


	public:
		gui_jailBreakRoleChoice();
		~gui_jailBreakRoleChoice();

		void onLoad() override;
		void onShow() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool onBtnClick(const EventArgs, VIewId viewId);
		void setChoiceRole(const ChoiceRole& choiceRole);

	private:

		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}

		void changeClothes(bool isPolice, list<ResInfo>::type resList);

	private:
		SubscriptionGuard m_subscriptionGuard;
		ChoiceRole m_choiceRole;

	};
}

#endif

