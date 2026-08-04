#pragma once
#ifndef __GUI_BIRD_PERSONAL_SHOP_ITEM_HEADER__
#define __GUI_BIRD_PERSONAL_SHOP_ITEM_HEADER__
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUILayout.h"
#include "World/BirdSimulator.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GuiBirdPersonalShopItem : public CustomGuiWindow
	{
	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;

		GUIStaticText* m_name = nullptr;
		GUIStaticText* m_discountNum = nullptr;
		GUIStaticText* m_money = nullptr;
		GUIStaticText* m_alreadyBuy = nullptr;
		GUIStaticImage* m_image = nullptr;
		GUIStaticImage* m_discount = nullptr;
		GUIStaticImage* m_topLeft = nullptr;
		GUIStaticImage* m_topRight = nullptr;
		GUIStaticImage* m_moneyIcon = nullptr;

		BirdGiftBag m_goods;
		i32 m_tabId = -1;

	private:
		void removeComponents();

	public:
		GuiBirdPersonalShopItem(const GUIWindowType& nType, const GUIString& strName);
		~GuiBirdPersonalShopItem() = default;

		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;
		bool onClick(const EventArgs& args);
		void setGoodInfo(BirdGiftBag goods, i32 tabId);	

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;
	};

}
#endif
