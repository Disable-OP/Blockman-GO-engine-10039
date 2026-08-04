#pragma once
#ifndef __GUI_HIDEANDSEEKHALLRESULT_HEADER__
#define __GUI_HIDEANDSEEKHALLRESULT_HEADER__

#include "gui_layout.h"
#include "UI/GUIWindow.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIButton.h"
#include "GUI/RootGuiLayout.h"
#include "gui_layout.h"
#include <vector>

using namespace LORD;

namespace BLOCKMAN
{
	class gui_hideAndSeekHallResult : public  gui_layout
	{
	private:
		SubscriptionGuard m_subscriptionGuard;

	public:
		gui_hideAndSeekHallResult();
		virtual ~gui_hideAndSeekHallResult();
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		void onShow() override;

		RootGuiLayout* getParent() { return dynamic_cast<RootGuiLayout*>(m_parentLayout); }

		bool onBtnSure(const EventArgs);
		bool onBtnAgain(const EventArgs);

		struct ItemInfo
		{
			int itemId;
			String itemImg;
			String itembgImg;
			bool isHave;

		};

		enum LuckType
		{
			LT_INVALID = -1,
			LT_ONCE = 1,
			LT_TEN = 10,
		};

		static const int MAX_ITEM_NUM = 10;
		bool getItemInfo(const String& info, std::vector<ItemInfo>& itemInfoList, String& moneyImg, int& money, int& times, int& entityId, int& coinId,int& rusultMoney);
		bool showItemInfo(const String& info);

	private:
		GUIStaticImage* m_itemBg[MAX_ITEM_NUM];
		GUIStaticImage* m_itemImg[MAX_ITEM_NUM];
		GUIStaticImage* m_moneyImg[MAX_ITEM_NUM];
		GUIStaticImage* m_newImg[MAX_ITEM_NUM]; 
		GUIStaticImage* m_frameImg[MAX_ITEM_NUM];
		GUIStaticImage* m_againIcon = nullptr;
		GUIStaticText* m_againMoney = nullptr;
		GUIStaticText* m_rusultMoneyText = nullptr;
		int m_need_money = -1;
		int m_luck_times = LT_INVALID;
		int m_luck_entityId = -1;
		int m_coinId = 0;
		int m_rusultMoney = 0;

	private:

	};
}

#endif
