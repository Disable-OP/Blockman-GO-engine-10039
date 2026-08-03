#pragma once
#ifndef __GUI_PERSONAL_SHOP_HEADER__
#define __GUI_PERSONAL_SHOP_HEADER__

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

	enum class SwitchablePropStatus : ui8
	{
		DontBuy = 0,
		CanBuy,
		HasGet,
		HasUse,
	};

	class SwitchableProp
	{
	public:
		String m_uniqueId = ""; 
		String m_name = "";
		String m_desc = "";
		String m_image = "";
		SwitchablePropStatus m_status = SwitchablePropStatus::DontBuy;
		i32 m_price = 0;
		i32 m_rare = 0;
	public:
		SwitchableProp() = default;
		SwitchableProp(String uniqueId, String name, String desc, String image, ui8 status, i32 price, i32 rare = 0)
			: m_uniqueId(uniqueId)
			, m_name(name)
			, m_desc(desc)
			, m_image(image)
			, m_status(SwitchablePropStatus(status))
			, m_price(price)
			, m_rare(rare)
		{}
	};

	enum class UpgradePropStatus : ui8
	{
		Default = 0,
		DontBuy,
		Unlock,
		Switch,
		Upgrade,
		CanBuy,
		HasGet,
		HasUse,
		MaxLv
	};

	class UpgradeProp
	{
	public:
		String m_uniqueId = "";
		String m_name = "";
		String m_desc = "";
		String m_image = "";
		i32 m_level = 0;
		i32 m_value = 0;
		i32 m_nextValue = 0;
		i32 m_price = 0;
		UpgradePropStatus m_status = UpgradePropStatus::DontBuy;
	public:
		UpgradeProp() = default;
		UpgradeProp(String uniqueId, String name, String desc, String image, i32 level, i32 value, i32 nextValue, i32 price, ui8 status)
			: m_uniqueId(uniqueId)
			, m_name(name)
			, m_desc(desc)
			, m_image(image)
			, m_level(level)
			, m_value(value)
			, m_nextValue(nextValue)
			, m_price(price)
			, m_status(UpgradePropStatus(status))
		{}
	};

	class EnchantmentProp
	{
	public:
		String m_uniqueId = "";
		String m_name = "";
		String m_desc = "";
		String m_image = "";
		i32 m_itemId = 0;
		i32 m_price = 0;
		bool m_isBuy = false;

	public:
		EnchantmentProp() = default;
		EnchantmentProp(String uniqueId, String name, String desc, String image, i32 itemId, i32 price, bool isBuy)
			:m_uniqueId(uniqueId)
			,m_name(name)
			, m_desc(desc)
			, m_image(image)
			, m_itemId(itemId)
			, m_price(price)
			, m_isBuy(isBuy)
		{
		};
	};

	enum class CustomPropStatus : ui8
	{
		Undefine = 0,
		Default,
		DontBuy,
		Unlock,
		Switch,
		Upgrade,
		CanBuy,
		HasGet,
		HasUse,
		MaxLv,
		Intensify
	};

	class CustomProp
	{
	public:
		String m_uniqueId = "";
		String m_name = "";
		String m_image = "";
		String m_desc = "";
		String m_values = "";
		i32 m_price = 0;
		CustomPropStatus m_status = CustomPropStatus::Default;
	public:
		CustomProp() = default;
		CustomProp(String uniqueId, String name, String image, String desc, String values, i32 price, ui8 status)
			: m_uniqueId(uniqueId)
			, m_name(name)
			, m_image(image)
			, m_desc(desc)
			, m_values(values)
			, m_price(price)
			, m_status(CustomPropStatus(status))
		{}
	};

	class CustomPropGroup
	{
	public:
		String m_groupId = "";
		String m_name = "";
		std::list<CustomProp> m_customProps;
	public:
		CustomPropGroup() = default;
		CustomPropGroup(String groupId, String name, std::list<CustomProp> customProps)
			: m_groupId(groupId)
			, m_name(name)
			, m_customProps(customProps)
		{}

		void addCustomProp(CustomProp prop) { m_customProps.push_back(prop); }
		void updateCustomProp(CustomProp prop);
		void clearCustomProp() { m_customProps.clear(); }
	};

	class gui_personalShop : public gui_layout
	{
		enum class PanelId
		{
			SWITCHABLE_PROPS,
			UPGRADE_PROPS,
			ENCHANTMENT_PROP
		};

	private:
		Vector3 m_minPos = Vector3::ZERO;
		Vector3 m_maxPos = Vector3::ZERO;

		GUIStaticText * m_textTip = nullptr;
		GUIButton* m_dialogContent = nullptr;
		SubscriptionGuard m_subscriptionGuard;

		ui32 m_showTipTime = 0;
		String m_tipMessage = "";

		std::list<SwitchableProp> m_switchableProps;
		std::list<UpgradeProp> m_upgradeProps;
		std::list<EnchantmentProp> m_enchantmentProps;
		std::map<String, CustomPropGroup> m_customPropGroups;

	public:
		gui_personalShop();
		~gui_personalShop();

		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;

		bool canOpen(Vector3 position, bool force);
		bool canShow();

	private:
		map<PanelId, GUIRadioButton*>::type m_tabs;
		map<PanelId, gui_layout*>::type m_panels;

		map<String, GUIRadioButton*>::type m_customTabs;
		gui_layout* m_customPanel = nullptr;

	private:
		void refreshTabUI();

		bool onCloseClick(const EventArgs & events);
		bool onDialogClick(const EventArgs & events);
		bool onRadioChange(const EventArgs & events, const PanelId radioId);
		bool onCustomRadioChange(const EventArgs & events, const String & groupId);
		bool onAreaChange(const Vector3 & startPos, const Vector3 & endPos);

		bool onPersonalShopTip(const String& tip);
		void setTabText(GUIRadioButton* tab, String text);

		bool onChangeSwitchableProps(const String& data);
		bool parseSwitchablePropsData(const String& data);
		void refreshSwitchablePropsUI();
		bool showSwitchableProp(const String & name, const String & image, const String & desc, int rare);

		bool onChangeUpgradeProps(const String& data);
		bool parseUpgradePropsData(const String& data);
		void refreshUpgradePropsUI();

		bool onChangeEnchantmentProps(const String& data);
		bool parseEnchantmentPropsData(const String& data);
		void refreshEnchantmentPropsUI();

		bool onChangeCustomProps(const String& data);
		bool parseCustomProps(const String& data);
		void removeOldCustomUI();
		void refreshCustomPropsUI();
		bool onUpdateCustomProps(const String& data);

		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}
	};

}
#endif