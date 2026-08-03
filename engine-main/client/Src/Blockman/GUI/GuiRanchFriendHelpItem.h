#pragma once
#ifndef __GUI_RANCH_FRIEND_HELP_ITEM_HEADER__
#define __GUI_RANCH_FRIEND_HELP_ITEM_HEADER__
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUILayout.h"
#include "World/Ranch.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GuiRanchFriendHelpItem : public CustomGuiWindow
	{
	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;
		GUIStaticImage* m_stateIcon = nullptr;
		GUIStaticImage* m_hotIcon = nullptr;
		GUIStaticImage* m_itemIcon = nullptr;
		GUIStaticText* m_tvValue = nullptr;
		GUIStaticText* m_tvStateValue = nullptr;

		RanchOrderInfo	m_orderInfo;

	private:
		void removeComponents();

	public:
		GuiRanchFriendHelpItem(const GUIWindowType& nType, const GUIString& strName);
		~GuiRanchFriendHelpItem() = default;
		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;
		void setOrderInfo(const RanchOrderInfo& info) { m_orderInfo = info; }

	private:
		bool onClick(const EventArgs& args);
		String getStateImage();
		String getHelperImage();

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;
	};

}
#endif
