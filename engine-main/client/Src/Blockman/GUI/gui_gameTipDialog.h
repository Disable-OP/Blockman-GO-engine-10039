#pragma once
#ifndef __GUI_GAME_TIP_DIALOG_HEADER__
#define __GUI_GAME_TIP_DIALOG_HEADER__
#include "Core.h"
#include "gui_layout.h"
#include "UI/GUIButton.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticText.h"
#include "Util/ClientEvents.h"
#include "GUI/RootGuiLayout.h"
#include <atomic>


using namespace LORD;

namespace BLOCKMAN
{
	class gui_gameTipDialog : public  gui_layout
	{
	public:
		enum class ShowType
		{
			DEATH,
			CLOSE_GAME,
			NETWORK_DISCONNECT
		};
		gui_gameTipDialog();
		virtual ~gui_gameTipDialog();
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		void refreshUi();
		void setShowType(ShowType showType);
		ShowType getShowType();
	private:

		bool btnSureClick(const EventArgs);
		bool btnCancelClick(const EventArgs);
		
		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}
		GUIButton* m_btnSure;
		GUIButton* m_btnCancel;
		GUIStaticText* m_msgText;
		ShowType m_showType;
	};


}
#endif