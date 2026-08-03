#ifndef __GUI_SHOP_GRID_VIEW_HEADER__
#define __GUI_SHOP_GRID_VIEW_HEADER__

#include <memory>
#include "UI/GUIWindow.h"
#include "UI/GUIGridView.h"
#include "GUI/CustomGuiWindow.h"
#include "Entity/EntityMerchant.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GuiShopGridView : public CustomGuiWindow
	{
	public:

	private:
		using BaseClass = CustomGuiWindow;
		GUIGridView* m_gridView = nullptr;

	public:
		GuiShopGridView(GUIWindowType nType, const GUIString& strName);
		~GuiShopGridView() = default;
		bool onLackOfMoney(const EventArgs & events) {
			EventArgs args;
			args.handled = events.handled;
			fireEvent("LackOfMoney", args);
			return true;
		};
		void updateGridView(CommodityGroup group, int merchantId);
		void clearGridView();
		void Destroy() override;
		void updateUnlockedState();
	};
}

#endif // !__GUI_SHOP_GRID_VIEW_HEADER__