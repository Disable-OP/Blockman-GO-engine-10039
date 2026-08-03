#pragma once
#include "gui_layout.h"
#include "UI/GUIButton.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIListBox.h"
#include "UI/GUIEditBox.h"
#include "GUI/RootGuiLayout.h"
#include "UI/GUIProgressBar.h"
#include "UI/GUILayout.h"

using namespace LORD;
using namespace GameClient;

namespace BLOCKMAN
{
	class gui_deadSummary : public gui_layout
	{
	private:
		bool8 m_isNextServer = true;
	public:
		using ResultEntryPtr = std::shared_ptr<GameClient::ResultEntry>;		
		gui_deadSummary();
		virtual ~gui_deadSummary();
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		void RefreshUI();
		void RefreshEliminatedUI();
		void onShow();
		void showDeadSummary(const String& gameResult, bool isNextServer);
		
	private:
		GUIProgressBar*	  m_rewardProgress = nullptr;
		GUIStaticText*	  m_rewardText = nullptr;
		ResultEntryPtr    m_gameResult = nullptr;
		SubscriptionGuard m_subscriptionGuard;
		GUILayout* m_deadSummaryContentEliminated = nullptr;
		GUIStaticText *eliminatedTipTitle = nullptr;
	
		bool onNameClick(const EventArgs & events);
		bool onExitClick(const EventArgs & events);
		bool onWatchBattleClick(const EventArgs & events);
		bool onContiueClick(const EventArgs & events);
		bool onAdClick(const EventArgs & events);
		bool onRewardTipClick(const EventArgs & events);
		bool receiveDeadSummary(const String& gameResult, bool isNextServer);
		void setIsNextServer(bool isNextServer);

		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}
	};
}