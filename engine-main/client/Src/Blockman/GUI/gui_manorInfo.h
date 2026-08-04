#pragma once
#ifndef __GUI_MANOR_INFO_HEADER__
#define __GUI_MANOR_INFO_HEADER__

#include "gui_layout.h"
#include "UI/GUIWindow.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIButton.h"
#include "GUI/RootGuiLayout.h"
#include "gui_layout.h"
#include "World/Manor.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_manorInfo : public  gui_layout
	{
		enum class ViewId
		{
			TELEPORT,
			UPGRADE
		};

	private:
		SubscriptionGuard m_subscriptionGuard;

	public:
		gui_manorInfo();
		virtual ~gui_manorInfo();
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		RootGuiLayout* getParent() { return dynamic_cast<RootGuiLayout*>(m_parentLayout); }
		bool onClick(const EventArgs & events, ViewId viewId);
		bool manorOnUpdate();

	private:
		ManorInfo* m_manorInfo = nullptr;
		ManorInfo* m_nextManorInfo = nullptr;

		GUIStaticImage* m_manorPreview = nullptr;
		GUIButton* m_btnUpgrade = nullptr;
		GUIButton* m_btnTeleport = nullptr;

		GUIStaticText* m_manorLevel = nullptr;
		GUIStaticText* m_groundValue = nullptr;
		GUIStaticText* m_roseValue = nullptr;
		GUIStaticText* m_seedValue = nullptr;
		GUIStaticText* m_charmValue = nullptr;
		GUIStaticText* m_valueValue = nullptr;

		i32 m_btnCDTime = 0;
	};
}

#endif