#pragma once
#ifndef __GUI_OCCUPATION_UNLOCK_HEADER__
#define __GUI_OCCUPATION_UNLOCK_HEADER__
#include "Core.h"
#include "GUI/gui_layout.h"
#include "GUI/RootGuiLayout.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIButton.h"
#include "GUI/GuiActorWindow.h"
#include "UI/GUIListBox.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_occupationUnlock : public gui_layout
	{
		enum class VIewId
		{
			BTN_PERPETUAL,
			BTN_SINGLE,
			BTN_CLOSE
		};


	public:
		gui_occupationUnlock();
		~gui_occupationUnlock();

		void onHide() override;
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;

	private:
		bool onClick(const EventArgs, VIewId viewId);
		bool open(const String& info);
		void dataParse(const String& info);

	private:
		i32 m_occupationId = 0;
		i32 m_occupationEntityId = 0;
		i32 m_singlePrice = 0;
		i32 m_perpetualPrice = 0;
		i32 m_singleCurrencyType = 1;
		i32 m_perpetualCurrencyType = 1;

		GUIWindow*			m_contentWindow = nullptr;
		GUIStaticText*		m_titleText = nullptr;
		GUIButton*			m_btnSingle = nullptr;
		GUIButton*			m_btnPerpetual = nullptr;
		GuiActorWindow*	m_actorWindow = nullptr;


		GUIStaticText*		m_tvSinglePrice = nullptr;
		GUIStaticText*		m_tvPerpetualPrice = nullptr;
		GUIStaticImage*		m_ivSingleCurrency = nullptr;
		GUIStaticImage*		m_ivPerpetualCurrency = nullptr;

		GUIListBox* m_lvOrientation = nullptr;

	};
}
#endif
