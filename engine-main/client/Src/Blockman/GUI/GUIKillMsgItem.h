#ifndef __GUI_KILL_MSG_ITEM_HEADER__
#define __GUI_KILL_MSG_ITEM_HEADER__

#include "UI/GUIWindow.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "GUI/CustomGuiWindow.h"
#include "gui_killPanel.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GUIKillMsgItem : public CustomGuiWindow
	{
	private:
		GUIWindow* m_window = nullptr;
		GUIStaticText* m_message = nullptr;
		GUIStaticImage* m_armsHint = nullptr;
		GUIStaticImage* m_headHint = nullptr;
		GUIStaticImage* m_killHint = nullptr;

	private:
		using BaseClass = CustomGuiWindow;
		void removeComponents();

	public:
		GUIKillMsgItem(const GUIWindowType& nType, const GUIString& strName);
		~GUIKillMsgItem() = default;

		void setKillMsg(KillMsg* msg);

		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;

	};
}

#endif