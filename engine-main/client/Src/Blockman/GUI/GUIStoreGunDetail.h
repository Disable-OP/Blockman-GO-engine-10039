#ifndef __GUI_STORE_GUN_DETAIL_HEADER__
#define __GUI_STORE_GUN_DETAIL_HEADER__

#include "gui_layout.h"
#include "GUI/GuiActorWindow.h"
#include "UI/GUIButton.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUILayout.h"
#include "UI/GUIProgressBar.h"
#include "Setting/GunStoreSetting.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GUIStoreGunDetail : public gui_layout
	{
	private:
		float m_rotate = 1.0f;
		StoreGun * m_data = NULL;

		GUIStaticText* m_name = NULL;
		GUIStaticText* m_tag = NULL;
		GuiActorWindow* m_actor = NULL;
		GUIStaticText* m_damage_text = NULL;
		GUIButton* m_damage_max = NULL;
		GUILayout* m_damage_default = NULL;
		GUIProgressBar* m_damage_value = NULL;
		GUILayout* m_attribute = NULL;
		GUIStaticText* m_shootspeed = NULL;
		GUIStaticText* m_bullet = NULL;
		GUILayout* m_property = NULL;
		GUILayout* m_countdown = NULL;
		GUIStaticText* m_countdown_value = NULL;
		GUIButton* m_use = NULL;
		GUIButton* m_buy = NULL;
		GUILayout* m_upgrade_action = NULL;
		GUILayout* m_damage_add = NULL;
		GUIStaticText* m_damage_add_text = NULL;
		GUILayout* m_price = NULL;
		GUIStaticImage* m_price_type = NULL;
		GUIStaticText* m_price_text = NULL;
		GUIButton* m_upgrade = NULL; 

	public:
		GUIStoreGunDetail();
		~GUIStoreGunDetail();

		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;

		void SetStoreGun(StoreGun* gun);

	private:
		bool onActorTouchDown(const EventArgs & events);
		bool onActorTouchUp(const EventArgs & events);
		bool onCloseClick(const EventArgs & events);
		bool onUseClick(const EventArgs & events);
		bool onBuyClick(const EventArgs & events);
		bool onUpgradeClick(const EventArgs & events);

	};
}

#endif