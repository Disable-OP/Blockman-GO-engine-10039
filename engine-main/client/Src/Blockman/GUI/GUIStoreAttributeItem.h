#ifndef __GUI_STORE_ATTRIBUTE_ITEM_HEADER__
#define __GUI_STORE_ATTRIBUTE_ITEM_HEADER__

#include "UI/GUIWindow.h"
#include "GUI/CustomGuiWindow.h"
#include "Setting/GunStoreSetting.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GUIStoreAttributeItem : public CustomGuiWindow
	{
	private:
		GUIWindow* m_window = nullptr;

	private:
		using BaseClass = CustomGuiWindow;
		void removeComponents();

	public:
		GUIStoreAttributeItem(const GUIWindowType& nType, const GUIString& strName);
		~GUIStoreAttributeItem() = default;

		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;

		void SetStoreAttribute(StoreAttribute* attribute, int addValue, bool isDetail = false);

	};
}

#endif