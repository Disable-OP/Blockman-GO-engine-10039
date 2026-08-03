#ifndef __GUI_UPGRADE_PROP_ITEM_HEADER__
#define __GUI_UPGRADE_PROP_ITEM_HEADER__

#include "UI/GUIWindow.h"
#include "UI/GUILayout.h"
#include "UI/GUIButton.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "GUI/CustomGuiWindow.h"
#include "gui_personalShop.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GUIUpgradePropItem : public CustomGuiWindow
	{
	private:
		GUILayout* m_window = nullptr;
		GUIStaticText* m_name = nullptr;
		GUIStaticImage* m_icon = nullptr;
		GUIStaticText* m_desc = nullptr;
		GUILayout* m_price = nullptr;
		GUIStaticImage* m_priceIcon = nullptr;
		GUIStaticText* m_priceNum = nullptr;
		GUIButton* m_upgrade = nullptr;

		UpgradeProp m_prop;
		UpgradePropStatus m_status = UpgradePropStatus::Default;

	private:
		using BaseClass = CustomGuiWindow;
		void removeComponents();
		void updateData();
		bool onButtonClick(const EventArgs & events);

	public:
		GUIUpgradePropItem(const GUIWindowType& nType, const GUIString& strName);
		~GUIUpgradePropItem() = default;

		void setUpgradeProp(UpgradeProp prop);
		UpgradeProp getUpgradeProp() { return m_prop; }

		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;
	};
}

#endif