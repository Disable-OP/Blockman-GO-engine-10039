#pragma once
#ifndef __GUI_SHOP_GOODS_HEADER__
#define __GUI_SHOP_GOODS_HEADER__

#include "UI/GUIWindow.h"
#include "GUI/CustomGuiWindow.h"
#include "GUI/gui_renderItems.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIButton.h"
#include "Entity/EntityMerchant.h"
#include "Util/ClientEvents.h"

using namespace LORD;

namespace BLOCKMAN {

	class GUIShopGoods : public CustomGuiWindow
	{

	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;
		GUIStaticImage* m_goodsIcon = nullptr;
		GUIStaticText* m_goodsNum = nullptr;
		GUIStaticText* m_goodsDetails = nullptr;
		GUIStaticText* m_goodsPrice = nullptr;
		GUIButton* m_buy = nullptr;
		GUIStaticImage* m_currencyIcon = nullptr;
		GUIButton * m_tipBtn = nullptr;
		Commodity m_commodity;
		int m_merchantId = 0;
		int m_groupIndex = 0;
		int m_oldUniqueId = -1;
		int m_oldItemId = -2;
		int m_oldItemDamage = -2;
		Color m_color;

	private:
		void removeComponents();
		void updateData();
		void updateIcon(int itemId, int damage, String image, int coinItemId);
	public:
		GUIShopGoods(const GUIWindowType& nType, const GUIString& strName);
		~GUIShopGoods() = default;
		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;
		void setCommodity(Commodity commodity, int merchantId, int groupIndex);
		Commodity getCommodity() { return m_commodity ;}
		bool onButtonClick(const EventArgs & events);
		bool onButtonItemTipClick(const EventArgs & events);
		void updateUnlockedState();

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;
	};

}
#endif
