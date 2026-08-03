#pragma once
#ifndef __GUI_BUILDWAR_GUESSRESULT_HEADER__
#define __GUI_BUILDWAR_GUESSRESULT_HEADER__

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
	class gui_buildwarGuessResult : public  gui_layout
	{
	private:
		SubscriptionGuard m_subscriptionGuard;

	public:
		gui_buildwarGuessResult();
		virtual ~gui_buildwarGuessResult();
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		void onShow() override;

		bool onClick(const EventArgs & events);
		RootGuiLayout* getParent() { return dynamic_cast<RootGuiLayout*>(m_parentLayout); }

		bool showBuildGuessResult(bool right, int rank);

	private:
		std::vector<GUILayout*> m_effectLayout;
		int m_effectTime = 0;
		const int effectInterval = 100;
		int m_curEffectIndex = 0;
	};
}

#endif