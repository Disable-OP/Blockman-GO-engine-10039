#ifndef __GUI_GUN_STORE_PANEL_HEADER__
#define __GUI_GUN_STORE_PANEL_HEADER__

#include "gui_layout.h"
#include "UI/GUIGridView.h"
#include "Setting/GunStoreSetting.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GUIGunStorePanel : public gui_layout
	{
	private:	
		float ContentSize = 0.0f;
		map<StoreTab*, GUIGridView*>::type m_panel_map;

	public:
		GUIGunStorePanel();
		~GUIGunStorePanel();

		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		void InitPanels(float contentSize);
		void InitPanelChilds();
		void ResetPanelChilds(int type);
		void ShowPanelByType(int type);

	private:
		void InitGunChilds(StoreTab* tab, GUIGridView* childs);
		void InitPropChilds(StoreTab* tab, GUIGridView* childs);

		void ResetGunChilds(StoreTab* tab, GUIGridView* childs);
		void ResetPropChilds(StoreTab* tab, GUIGridView* childs);

	};
}

#endif