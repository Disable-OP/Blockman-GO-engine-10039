#ifndef __GUI_GUN_STORE_INFOS_HEADER__
#define __GUI_GUN_STORE_INFOS_HEADER__

#include "gui_layout.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIButton.h"
#include "Setting/GunStoreSetting.h"

using namespace LORD;

namespace BLOCKMAN
{
	enum DISPLAY_TYPE
	{
		DISPLAY_TYPE_NONE = 0,
		DISPLAY_TYPE_GUN,
		DISPLAY_TYPE_PROP,
		DISPLAY_TYPE_BLOCK
	};

	class GUIGunStoreInfos : public gui_layout
	{
	private:
		DISPLAY_TYPE m_display_type = DISPLAY_TYPE_NONE;
		StoreGun* m_gun = nullptr;
		StoreProp* m_prop = nullptr;

	private:
		GUILayout* m_content = nullptr;

		GUIStaticText* m_name = nullptr;

		GUILayout* m_gun_infos = nullptr;
		GUIStaticText* m_gun_damage_value = nullptr;
		GUILayout* m_gun_shootspeed = nullptr;
		GUIStaticText* m_gun_shootspeed_value = nullptr;
		GUILayout* m_gun_bullet = nullptr;
		GUIStaticText* m_gun_bullet_value = nullptr;
		GUILayout* m_gun_property = nullptr;

		GUILayout* m_prop_infos = nullptr;

		GUIButton* m_action_use = nullptr;
		GUIButton* m_action_unload = nullptr;
		GUIButton* m_action_buy = nullptr;
		GUIButton* m_action_upgrade = nullptr;

	public:
		GUIGunStoreInfos();
		~GUIGunStoreInfos();

		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;

		void SetStoreGun(StoreGun* gun);
		void SetStoreProp(StoreProp* prop);

	private:
		void onGunInfosUpdate();
		void onPropInfosUpdate();

		bool onUseClick(const EventArgs & events);
		bool onUnloadClick(const EventArgs & events);
		bool onBuyClick(const EventArgs & events);
		bool onUpgradeClick(const EventArgs & events);
	};
}

#endif