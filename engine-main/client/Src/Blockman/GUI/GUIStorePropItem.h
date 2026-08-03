#ifndef __GUI_STORE_PROP_ITEM_HEADER__
#define __GUI_STORE_PROP_ITEM_HEADER__

#include "UI/GUIWindow.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUILayout.h"
#include "UI/GUIButton.h"
#include "GUI/CustomGuiWindow.h"
#include "Setting/GunStoreSetting.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GUIStorePropItem : public CustomGuiWindow
	{
	private:
		StoreProp* m_data = NULL;

		GUIWindow* m_window = nullptr;
		GUIStaticImage* m_status_box = nullptr;
		GUIStaticImage* m_image = nullptr;
		GUIButton* m_use_line = nullptr;
		GUILayout* m_hint_new = nullptr;
		GUILayout* m_price = nullptr;
		GUIStaticImage* m_price_icon = nullptr;
		GUIStaticText* m_price_text = nullptr;
		GUILayout* m_lock = nullptr;
		GUIButton* m_lock_level = nullptr;

	private:
		using BaseClass = CustomGuiWindow;
		void removeComponents();
		void initView();

	public:
		GUIStorePropItem(const GUIWindowType& nType, const GUIString& strName);
		~GUIStorePropItem() = default;

		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;

		void InitStoreProp(StoreProp* prop);

	private:
		bool OnItemChick(const EventArgs & events);
	};
}

#endif