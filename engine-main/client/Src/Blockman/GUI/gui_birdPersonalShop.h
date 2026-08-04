#pragma once
#ifndef __GUI_BIRD_PERSONAL_SHOP_HEADER__
#define __GUI_BIRD_PERSONAL_SHOP_HEADER__
#include "Core.h"
#include "gui_layout.h"
#include "UI/GUISlider.h"
#include "UI/GUILayout.h"
#include "UI/GUIGridView.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIRadioButton.h"
#include "World/BirdSimulator.h"
#include "World/Shop.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_birdPersonalShop : public  gui_layout
	{
	private:

	private:
		enum class ViewId {
			CLOSE,
			BUY,
			GRID_VIEW,
			SOLL_BAR
		};

	private:
		GUILayout* m_tabLayout = nullptr;
		GUILayout* m_itemListLayout = nullptr;
		GUIGridView* m_itemGridView = nullptr;
		GUIStaticImage* m_image = nullptr;
		GUIStaticText* m_des = nullptr;
		GUIStaticText* m_money = nullptr;
		GUIStaticImage* m_moneyIcon = nullptr;
		GUISlider* m_sollBar = nullptr;

		i32 m_curGoodsId = -1;
		i32 m_curGoodsTabId = -1;
		i32 m_curPrice = -1;
		i32 m_curCurrencyType = -1;
		float m_itemGridViewXSize = 0.f;
		bool m_isShopUpdate = false;


	public:
		gui_birdPersonalShop();
		~gui_birdPersonalShop();
		void onShow() override;
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		void setAlreadyBuy(i32 limit);
	private:
		bool onViewSoll(const EventArgs&, ViewId viewId);

	private:
		void onShopUpdate();
		bool showPersonalShop(i32 tabId);
		void addTabView(BirdPersonalStoreTab store, int index);
		bool onRadioChange(const EventArgs & events, BirdPersonalStoreTab store, GUIRadioButton* radio);
		bool onButtonClick(const EventArgs & events, ViewId viewId);
		bool updateBirdPersonalShopInfo(const BirdGiftBag& goods, i32 tabId);
		bool updateBirdPersonalShop();
		void sortItemSizeChange();

	};

}
#endif