#ifndef __GUI_SWITCHABLE_PROP_ITEM_HEADER__
#define __GUI_SWITCHABLE_PROP_ITEM_HEADER__

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
	class GUISwitchablePropItem : public CustomGuiWindow
	{
	private:
		GUILayout* m_window = nullptr;
		GUIStaticText* m_name = nullptr;
		GUIStaticImage* m_icon = nullptr;
		GUILayout* m_price = nullptr;
		GUIStaticImage* m_priceIcon = nullptr;
		GUIStaticText* m_priceNum = nullptr;
		GUIButton* m_buy = nullptr;

		SwitchableProp m_prop;
		SwitchablePropStatus m_status = SwitchablePropStatus::DontBuy;

	private:
		using BaseClass = CustomGuiWindow;
		void removeComponents();
		void updateData();
		bool onBuyClick(const EventArgs & events);
		bool onPropClick(const EventArgs & events);

	public:
		GUISwitchablePropItem(const GUIWindowType& nType, const GUIString& strName);
		~GUISwitchablePropItem() = default;

		void setSwitchableProp(SwitchableProp prop);
		SwitchableProp getSwitchableProp() { return m_prop; }

		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;
	};
}

#endif