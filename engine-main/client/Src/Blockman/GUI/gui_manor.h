#pragma once
#ifndef __GUI_MANOR_HEADER__
#define __GUI_MANOR_HEADER__

#include "gui_layout.h"
#include "UI/GUIWindow.h"
#include "UI/GUILayout.h"
#include "UI/GUIRadioButton.h"
#include "UI/GUIStaticImage.h"
#include "GUI/RootGuiLayout.h"
#include "gui_layout.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_manor : public  gui_layout
	{
		enum class ViewId
		{
			INFO,
			HOUSE,
			FURNITURE,
			MESSAGE
		};

	public:
		gui_manor();
		virtual ~gui_manor();
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}
		bool onRadioChange(const EventArgs & events, const ViewId radioId);
		bool onClose(const EventArgs & events);
		void openManor();

	private:
		void onRadioSelected(const ViewId radioId);

	private:
		map<ViewId, GUIRadioButton*>::type m_radioMap;
		map<ViewId, gui_layout*>::type m_radioPanelMap;
	};
}

#endif