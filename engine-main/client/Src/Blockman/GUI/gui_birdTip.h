#pragma once
#ifndef __GUI_BIRD_TIP_HEADER__
#define __GUI_BIRD_TIP_HEADER__

#include "gui_layout.h"
#include "UI/GUIWindow.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIButton.h"
#include "UI/GUIStaticImage.h"
#include "Util/TipType.h"
#include "Inventory/Wallet.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_birdTip : public  gui_layout
	{
	private:
		enum class ViewId {
			CLOSE,
			CANCEL,
			SURE
		};
	public:
		gui_birdTip();
		virtual ~gui_birdTip();
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool onClick(const EventArgs & events, ViewId viewId);
		bool showCommonTip(const String & msg, BirdTipType tipType);
		bool showLockOfMoneyTip(i32 difference, i32 currencyType);
		bool showExpenditureTip(i32 price, i32 currencyType, BirdTipType tipType);
		bool showOpenChestTip(i32 entityId, i32 currencyType, i32 timeLeft);
		bool showDeleteTip(i64 birdId, const String& birdName);

	private:
		void btnSure();
		bool showGotoShop();
		bool showGotoApp();
		void changeTitleAndMessage();

	private:
		BirdTipType m_tipType = BirdTipType::COMMON;

		GUIStaticText* m_tvTitle = nullptr;
		GUIStaticText* m_tvPrice = nullptr;
		GUIStaticText* m_tvPanelMsg = nullptr;
		GUIStaticText* m_tvMoneyTipValue = nullptr;

		GUIStaticText* m_tvTime = nullptr;
		GUIButton* m_btnTimePrice = nullptr;

		GUIButton* m_btnSure = nullptr;
		GUILayout* m_panelLayout = nullptr;
		GUILayout* m_moneyTipLayout = nullptr;
		GUILayout* m_timeTipLayout = nullptr;

		i64 m_birdId = 0;
		i32 m_price = 0;
		i32 m_leftTime = 0;
		i32 m_entityId = 0;
		CurrencyType m_currencyType = CurrencyType::PLATFORM_MONEY;
	};
}

#endif