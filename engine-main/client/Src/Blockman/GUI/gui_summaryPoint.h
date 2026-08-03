#pragma once
#include "gui_layout.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIListBox.h"
#include "GUI/RootGuiLayout.h"

using namespace LORD;
using namespace GameClient;

namespace BLOCKMAN
{
	class gui_summaryPoint : public gui_layout
	{
	public:
		gui_summaryPoint();
		virtual ~gui_summaryPoint();
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		void onShow();
		void showContent(const LORD::UVector2& pos, const std::map<int, int>& pointMap);

	private:
		GUIListBox* m_pointList = nullptr;

		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}
	};
}


