#ifndef __GUI_STORE_PROPERTY_ITEM_HEADER__
#define __GUI_STORE_PROPERTY_ITEM_HEADER__

#include "UI/GUIWindow.h"
#include "GUI/CustomGuiWindow.h"
#include "Setting/GunStoreSetting.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GUIStorePropertyItem : public CustomGuiWindow
	{
	private:
		GUIWindow* m_window = nullptr;

	private:
		using BaseClass = CustomGuiWindow;
		void removeComponents();

	public:
		GUIStorePropertyItem(const GUIWindowType& nType, const GUIString& strName);
		~GUIStorePropertyItem() = default;

		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;

		void SetStoreProperty(StoreProperty* property, bool isDetail = false);

	};
}

#endif