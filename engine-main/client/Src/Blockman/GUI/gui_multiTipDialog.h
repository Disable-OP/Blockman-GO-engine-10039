#pragma once
#ifndef __GUI_MULTI_TIP_DIALOG_HEADER__
#define __GUI_MULTI_TIP_DIALOG_HEADER__
#include "GUI/gui_layout.h"
#include "GUI/RootGuiLayout.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIButton.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_multiTipDialog : public gui_layout
	{
		enum class VIewId
		{
			UP,
			NEXT
		};


	public:
		gui_multiTipDialog();
		~gui_multiTipDialog();

		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool onBtnClick(const EventArgs, VIewId viewId);
		bool onClose(const EventArgs);

		bool open(const String& tips);

	private:
		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}
		void parseData(const String& tips);


	private:
		SubscriptionGuard m_subscriptionGuard;
		vector<String>::type m_tips = vector<String>::type();
		i32 m_page = 1;

	private:
		GUIStaticText*		m_titleText = nullptr;
		GUIStaticText*		m_messageText = nullptr;
		GUIButton*			m_upBtn = nullptr;
		GUIButton*			m_nextBtn = nullptr;
	};
}
#endif
