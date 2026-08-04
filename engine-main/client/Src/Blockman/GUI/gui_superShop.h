#pragma once
#ifndef __GUI_SUPER_SHOP_HEADER__
#define __GUI_SUPER_SHOP_HEADER__

#include "Core.h"
#include "gui_layout.h"
#include "UI/GUIButton.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIRadioButton.h"
#include "GUI/RootGuiLayout.h"
#include <map>

using namespace LORD;

namespace BLOCKMAN
{
	enum class SuperPropStatus : ui8
	{
		DontBuy = 0,
		Unlock,
		Switch,
		Upgrade,
		CanBuy,
		HasGet,
		HasUse,
		MaxLv
	};

	class SuperProp
	{
	public:
		String m_uniqueId = "";
		String m_name = "";
		String m_content = "";
		String m_desc = "";
		String m_image = "";
		SuperPropStatus m_status = SuperPropStatus::DontBuy;
		i32 m_price = 0;
		i32 m_value = 0;
	public:
		SuperProp() = default;
		SuperProp(String uniqueId, String name, String content, String desc, String image, ui8 status, i32 price, i32 value)
			: m_uniqueId(uniqueId)
			, m_name(name)
			, m_content(content)
			, m_desc(desc)
			, m_image(image)
			, m_status(SuperPropStatus(status))
			, m_price(price)
			, m_value(value)
		{}
	};

	class gui_superShop : public gui_layout
	{
		enum class PanelId
		{
			SUPER_PROPS
		};

	private:
		GUIStaticText * m_textTip = nullptr;
		GUIButton* m_dialogContent = nullptr;

		SubscriptionGuard m_subscriptionGuard;

		ui32 m_showTipTime = 0;
		String m_tipMessage = "";

		std::list<SuperProp> m_superProps;

		map<PanelId, GUIRadioButton*>::type m_tabs;
		map<PanelId, gui_layout*>::type m_panels;

	public:
		gui_superShop();
		~gui_superShop();

		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;

	private:
		void setTabText(GUIRadioButton* tab, String text);
		void setTitleText(String text);
		bool onRadioChange(const EventArgs & events, const PanelId radioId);

		bool onDialogClick(const EventArgs & events);
		bool onCloseClick(const EventArgs & events);
		bool onSuperShopTip(const String& tip);

		bool onChangeSuperProps(const String& data);
		bool parseSuperPropsData(const String& data);
		void refreshSuperPropsUI();
		bool showSuperProp(const String & name, const String & image, const String & desc);

		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}
	};

}
#endif