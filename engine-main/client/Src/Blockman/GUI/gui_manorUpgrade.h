#pragma once
#ifndef __GUI_MANOR_FURNITURE_HEADER__
#define __GUI_MANOR_FURNITURE_HEADER__

#include "gui_layout.h"
#include "UI/GUIWindow.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIButton.h"
#include "GUI/RootGuiLayout.h"
#include "gui_layout.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_manorUpgrade : public  gui_layout
	{
		enum class ViewId
		{
			BTN_SURE,
			BTN_CANCEL
		};

	private:
		SubscriptionGuard m_subscriptionGuard;

	public:
		gui_manorUpgrade();
		virtual ~gui_manorUpgrade();
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		RootGuiLayout* getParent() { return dynamic_cast<RootGuiLayout*>(m_parentLayout); }
		bool onClick(const EventArgs & events, ViewId viewId);

	private:
		GUIStaticText* m_title = nullptr;
		GUIStaticText* m_price = nullptr;
		GUIStaticText* m_groundValue = nullptr;
		GUIStaticText* m_roseValue = nullptr;
		GUIStaticText* m_seedValue = nullptr;
		GUIStaticText* m_charmValue = nullptr;
		GUIStaticText* m_valueValue = nullptr;
		GUIButton* m_btnSure = nullptr;
	};
}

#endif