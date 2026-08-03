#pragma once
#ifndef __GUI_APP_SHOP_ITEM_HEADER__
#define __GUI_APP_SHOP_ITEM_HEADER__

#include "GUI/CustomGuiWindow.h"
#include "GUI/gui_renderItems.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIButton.h"
#include "World/Shop.h"


using namespace LORD;
namespace BLOCKMAN
{
	class GuiAppShopItem : public CustomGuiWindow
	{

	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;
		GUIStaticText* m_itemName = nullptr;
		GUILayout* m_quota = nullptr;
		GUIStaticText* m_quotaValue = nullptr;
		GUIStaticText* m_itemNum = nullptr;
		GUIStaticImage* m_itemIcon = nullptr;
		GUIButton* m_btnBuy = nullptr;
		GUIStaticImage* m_currencyIcon = nullptr;
		Goods m_goods;

		int m_oldItemId = -2;
		int m_oldItemDamage = -2;
		int m_groupIndex = -1;

	private:
		void removeComponents();
		void updateIcon(int itemId, int damage, String icon);

	public:
		GuiAppShopItem(const GUIWindowType& nType, const GUIString& strName);
		~GuiAppShopItem() = default;
		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;
		bool onButtonClick(const EventArgs & events);
		void setGoods(Goods goods, int groupIndex) {
			this->m_goods = goods;  
			this->m_groupIndex = groupIndex;
		 }

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;
	};

}
#endif
