#pragma once
#include "gui_layout.h"
#include "UI/GUIButton.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIListBox.h"
#include "GUI/RootGuiLayout.h"
#include "UI/GUIProgressBar.h"

using namespace LORD;
using namespace GameClient;

namespace BLOCKMAN
{
	class gui_finalSummary : public gui_layout
	{
	private:
		bool8 m_isNextServer = true;
	public:
		using ResultEntryPtr = std::shared_ptr<GameClient::ResultEntry>;

		gui_finalSummary();
		virtual ~gui_finalSummary();
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		void onShow() override;
		void RefreshUI();

	private:
		GUIProgressBar*								m_rewardProgress = nullptr;
		GUIStaticText*								m_rewardText = nullptr;
		GUIListBox*									m_rankList = nullptr;
		GUIStaticText*								m_jewelKnightName = nullptr;
		ResultEntry::ResultEntryListPtr				m_resultEntryList;
		ResultEntry::ResultEntryPtr					m_selfResultEntry;
		SubscriptionGuard							m_subscriptionGuard;

		void RefreshSelf();
		void RefreshAllRank();

		bool onExitClick(const EventArgs & events);
		bool onBackClick(const EventArgs & events);
		bool onShareClick(const EventArgs & events);
		bool onAdClick(const EventArgs & events);
		bool onAdTipCloseClick(const EventArgs & events);
		bool onContiueClick(const EventArgs & events);
		bool onBackHallClick(const EventArgs & events);
		bool onSelfPanelClick(const EventArgs & events);
		bool onRewardTipClick(const EventArgs & events);
		bool receiveFinalSummary(const String& gameResult, const bool isNextServer);
		bool receiveFinalSummaryExtra(bool guess_right, String guess_name, int guess_reward);
		void setIsNextServer(bool isNextServer);

		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}
	};
}
