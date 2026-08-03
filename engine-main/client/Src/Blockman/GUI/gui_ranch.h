#pragma once
#ifndef __GUI_RANCH_HEADER__
#define __GUI_RANCH_HEADER__

#include "gui_layout.h"
#include "UI/GUIWindow.h"
#include "UI/GUILayout.h"
#include "GUI/RootGuiLayout.h"
#include "UI/GUIStaticText.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_ranch : public  gui_layout
	{
	public:
		enum class ViewId
		{
			MAIN,
			HOUSE,
			STORAGE,
			FACTORY,
			FARMING,
			COMMUNITY,
			ACHIEVEMENT
		};

		enum class BtnRank
		{
			CLAN,
			PROSPERITY,
			ACHIEVEMENT
		};

	public:
		gui_ranch();
		virtual ~gui_ranch();
		void onLoad() override;
		void onHide() override;
		void onUpdate(ui32 nTimeElapse) override;
		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}
		void onClick(const ViewId & viewId);
		bool onRankClick(const EventArgs& args, const BtnRank & viewId);
		bool onClose(const EventArgs & events);
		bool onBack(const EventArgs & events);
		void openRanch();
		bool openRanchStorage();
		bool ranchInfoChange();

	private:
		SubscriptionGuard m_subscriptionGuard;
		map<ViewId, String>::type m_langMap;
		map<ViewId, gui_layout*>::type m_viewPanelMap;
		GUIStaticText* m_tvName = nullptr;
	};
}

#endif