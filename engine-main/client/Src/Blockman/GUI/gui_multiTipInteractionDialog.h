#pragma once
#ifndef __GUI_MULTI_TIP_INTERACTION_DIALOG_HEADER__
#define __GUI_MULTI_TIP_INTERACTION_DIALOG_HEADER__
#include "GUI/gui_layout.h"
#include "GUI/RootGuiLayout.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIButton.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_multiTipInteractionDialog : public gui_layout
	{
		enum class VIewId
		{
			UP,
			NEXT,
			INTERACTION
		};

		enum class InteractionType
		{
			DEFAULT,
			RANCH_RECEIVE,
			RANCH_DESTROYHOUSE,
			RANCHERS_EXPLORE_BEGIN,
			RANCHERS_TASK_FINISH,
			RANCH_GO_EXPLORE,
			BRID_TASK = 104101,
		};


	public:
		gui_multiTipInteractionDialog();
		~gui_multiTipInteractionDialog();

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
		void interaction();
		void parseData(const String& tips);


	private:
		SubscriptionGuard m_subscriptionGuard;
		vector<String>::type m_tips = vector<String>::type();
		InteractionType m_interactionType = InteractionType::DEFAULT;
		i32 m_page = 1;
		i32 m_entityId = 0;
		i32 m_taskId = 0;

	private:
		GUIStaticText*		m_titleText = nullptr;
		GUIStaticText*		m_messageText = nullptr;
		GUIButton*			m_nextBtn = nullptr;
		GUIButton*			m_interactionBtn = nullptr;
	};
}
#endif
