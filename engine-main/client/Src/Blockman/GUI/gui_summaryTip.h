#pragma once
#include "gui_layout.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticText.h"
#include "GUI/RootGuiLayout.h"

using namespace LORD;
using namespace GameClient;

namespace BLOCKMAN
{
	class gui_summaryTip : public gui_layout
	{
	public:
		gui_summaryTip();
		virtual ~gui_summaryTip();
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		void onShow();
		void showContent(const LORD::UVector2& pos, int total);

	private:

		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}
	};
}



