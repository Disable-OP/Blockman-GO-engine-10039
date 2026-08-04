#pragma once
#ifndef __GUI_BUILDWAR_GRADE_HEADER__
#define __GUI_BUILDWAR_GRADE_HEADER__

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
	class gui_buildwarGrade : public  gui_layout
	{
	private:
		SubscriptionGuard m_subscriptionGuard;

	public:
		gui_buildwarGrade();
		virtual ~gui_buildwarGrade();
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		void onShow() override;

		enum StarButtonId
		{
			SBI1 = 1,
			SBI2,
			SBI3,
			SBI4,
			SBI5,
			SBI6,
			SBI_END,
		};


		bool onClick(const EventArgs & events, StarButtonId starButtonId);
		RootGuiLayout* getParent() { return dynamic_cast<RootGuiLayout*>(m_parentLayout); }

		bool showBuildWarGrade(const String& msg, ui8 left_time);

	private:
		map<int, GUIStaticImage*>::type m_button_star_map;

	private:
		
	};
}

#endif