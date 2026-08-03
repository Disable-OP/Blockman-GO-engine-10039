#ifndef __GUI_STORE_PROP_DETAIL_HEADER__
#define __GUI_STORE_PROP_DETAIL_HEADER__

#include "gui_layout.h"
#include "UI/GUIButton.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUILayout.h"
#include "UI/GUIProgressBar.h"
#include "GUI/GUIGunStoreLevel.h"
#include "Setting/GunStoreSetting.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GUIStorePropDetail : public gui_layout
	{
	private:
		StoreProp * m_data = NULL;

		GUIStaticText* m_name = NULL;
		GUIStaticText* m_tag = NULL;
		GUIStaticImage* m_image = NULL;
		GUIStaticText* m_desc = NULL;
		GUIGunStoreLevel* m_level = NULL;
		GUILayout* m_attribute = NULL;
		GUILayout* m_property = NULL;
		GUILayout* m_countdown = NULL;
		GUIStaticText* m_countdown_value = NULL;
		GUIButton* m_use = NULL;
		GUIButton* m_buy = NULL;
		GUILayout* m_upgrade_action = NULL;
		GUIStaticImage* m_price_type = NULL;
		GUILayout* m_price = NULL;
		GUIStaticText* m_price_text = NULL;
		GUIButton* m_upgrade = NULL;

	public:
		GUIStorePropDetail();
		~GUIStorePropDetail();

		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;

		void SetStoreProp(StoreProp* prop);

	private:
		bool onCloseClick(const EventArgs & events);
		bool onUseClick(const EventArgs & events);
		bool onBuyClick(const EventArgs & events);
		bool onUpgradeClick(const EventArgs & events);

	};
}

#endif