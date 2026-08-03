#ifndef __GUI_GUN_STORE_TAB_HEADER__
#define __GUI_GUN_STORE_TAB_HEADER__

#include "UI/GUIWindow.h"
#include "GUI/CustomGuiWindow.h"
#include "Setting/GunStoreSetting.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GUIGunStoreTab : public CustomGuiWindow
	{
	private:
		GUIWindow* m_window = nullptr;
		int m_type = 0;
		StoreTabType m_tab_type = STORE_TAB_NONE;

	private:
		using BaseClass = CustomGuiWindow;
		void removeComponents();

	public:
		GUIGunStoreTab(const GUIWindowType& nType, const GUIString& strName);
		~GUIGunStoreTab() = default;

		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;

		void InitStoreTab(StoreTab* tab);
		void UpdateNewCount();
		void SetTabSelected(bool selected);
		bool IsTabSelected();
		bool OnTabSelected(const EventArgs & events);
		int GetType() { return m_type; }
		StoreTabType GetTabType() { return m_tab_type; }
	};
}

#endif