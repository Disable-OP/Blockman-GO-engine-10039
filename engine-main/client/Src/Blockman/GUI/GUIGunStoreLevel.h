#ifndef __GUI_GUN_STORE_LEVEL_HEADER__
#define __GUI_GUN_STORE_LEVEL_HEADER__

#include "UI/GUIWindow.h"
#include "GUI/CustomGuiWindow.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GUIGunStoreLevel : public CustomGuiWindow
	{
	private:
		GUIWindow* m_window = nullptr;
		int m_curLevel = 1;
		int m_maxLevel = 3;
		bool m_init = false;

	private:
		using BaseClass = CustomGuiWindow;
		void removeComponents();
		void initChilds(bool force = false);

	public:
		GUIGunStoreLevel(const GUIWindowType& nType, const GUIString& strName);
		~GUIGunStoreLevel() = default;

		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;

		void InitLevel(int curLevel, int maxLevel);
	};
}

#endif