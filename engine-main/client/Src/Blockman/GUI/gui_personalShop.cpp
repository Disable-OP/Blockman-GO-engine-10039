#include "gui_personalShop.h"
#include "gui_switchablePropsPanel.h"
#include "gui_upgradePropsPanel.h"
#include "gui_enchantmentPropsPanel.h"
#include "gui_customPropsPanel.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUILayout.h"
#include "Util/LanguageKey.h"
#include "Setting/UIDisplaySetting.h"
#include "rapidjson/document.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "cWorld/Blockman.h"
#include "GUI/GuiDef.h"
#include "game.h"

namespace BLOCKMAN
{
	void CustomPropGroup::updateCustomProp(CustomProp prop)
	{
		for (auto iter = m_customProps.begin(); iter != m_customProps.end(); ++iter)
		{
			auto cprop = *iter;
			if (cprop.m_uniqueId == prop.m_uniqueId)
			{
				if (prop.m_name.length() > size_t(0))
					cprop.m_name = prop.m_name;
				if (prop.m_image.length() > size_t(0))
					cprop.m_image = prop.m_image;
				if (prop.m_desc.length() > size_t(0))
					cprop.m_desc = prop.m_desc;
				if (prop.m_values.length() > size_t(0))
					cprop.m_values = prop.m_values;
				if (prop.m_price != -1)
					cprop.m_price = prop.m_price;
				if (cprop.m_status != CustomPropStatus::Undefine)
					cprop.m_status = prop.m_status;
				m_customProps.insert(iter, cprop);
				m_customProps.erase(iter);
				break;
			}
		}
	}

	gui_personalShop::gui_personalShop()
		: gui_layout("PersonalShop.json")
	{
		m_panels[PanelId::SWITCHABLE_PROPS] = m_panels[PanelId::SWITCHABLE_PROPS] ? m_panels[PanelId::SWITCHABLE_PROPS] : LordNew gui_switchablePropsPanel;
		m_panels[PanelId::UPGRADE_PROPS] = m_panels[PanelId::UPGRADE_PROPS] ? m_panels[PanelId::UPGRADE_PROPS] : LordNew gui_upgradePropsPanel;
		m_panels[PanelId::ENCHANTMENT_PROP] = m_panels[PanelId::ENCHANTMENT_PROP] ? m_panels[PanelId::ENCHANTMENT_PROP] : LordNew gui_enchantmentPropsPanel;
		m_customPanel = m_customPanel ? m_customPanel : LordNew gui_customPropsPanel;
	}

	gui_personalShop::~gui_personalShop()
	{
		for (auto panel : m_panels)
		{
			LordSafeDelete(panel.second);
		}
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_personalShop::onLoad()
	{
		auto title = getWindow<GUIStaticText>("PersonalShop-TitleName");
		title->SetText(getString(LanguageKey::GUI_STR_APP_SHOP_SHOP));

		m_tabs[PanelId::SWITCHABLE_PROPS] = getWindow<GUIRadioButton>("PersonalShop-Content-Tab-Switchable-Props");
		m_tabs[PanelId::UPGRADE_PROPS] = getWindow<GUIRadioButton>("PersonalShop-Content-Tab-Upgrade-Props");
		m_tabs[PanelId::ENCHANTMENT_PROP] = getWindow<GUIRadioButton>("PersonalShop-Content-Tab-Enchantment-Props");

		m_tabs[PanelId::SWITCHABLE_PROPS]->SetText(getString(LanguageKey::GUI_SHOP_TAB_ARMS));
		m_tabs[PanelId::UPGRADE_PROPS]->SetText(getString(LanguageKey::GUI_SHOP_TAB_ARMOR));
		m_tabs[PanelId::ENCHANTMENT_PROP]->SetText(getString(LanguageKey::GUI_SHOP_TAB_ENCHANTMENT));

		refreshTabUI();

		auto contentPanel = getWindow("PersonalShop-Content-Panel");
		map<PanelId, GUIRadioButton*>::iterator  iter;
		for (iter = m_tabs.begin(); iter != m_tabs.end(); iter++)
		{
			iter->second->subscribeEvent(EventRadioStateChanged, std::bind(&gui_personalShop::onRadioChange, this, std::placeholders::_1, iter->first));
			if (m_panels[iter->first])
			{
				m_panels[iter->first]->attachTo(this, contentPanel);
			}
		}
		m_customPanel->attachTo(this, contentPanel);

		m_textTip = getWindow<GUIStaticText>("PersonalShop-Content-Tip");
		auto btnClose = getWindow<GUIButton>("PersonalShop-BtnClose");
		btnClose->subscribeEvent(EventButtonClick, std::bind(&gui_personalShop::onCloseClick, this, std::placeholders::_1));

		m_dialogContent = getWindow<GUIButton>("PersonalShop-Dialog-Content");
		m_dialogContent->SetVisible(false);
		m_dialogContent->subscribeEvent(EventButtonClick, std::bind(&gui_personalShop::onDialogClick, this, std::placeholders::_1));

		m_subscriptionGuard.add(ChangeSwitchablePropsEvent::subscribe(std::bind(&gui_personalShop::onChangeSwitchableProps, this, std::placeholders::_1)));
		m_subscriptionGuard.add(ChangeUpgradePropsEvent::subscribe(std::bind(&gui_personalShop::onChangeUpgradeProps, this, std::placeholders::_1)));
		m_subscriptionGuard.add(ChangeEnchantmentPropsEvent::subscribe(std::bind(&gui_personalShop::onChangeEnchantmentProps, this, std::placeholders::_1)));
		m_subscriptionGuard.add(PersonalShopTipEvent::subscribe(std::bind(&gui_personalShop::onPersonalShopTip, this, std::placeholders::_1)));
		m_subscriptionGuard.add(ShowSwitchablePropEvent::subscribe(std::bind(&gui_personalShop::showSwitchableProp, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)));
		m_subscriptionGuard.add(PersonalShopAreaEvent::subscribe(std::bind(&gui_personalShop::onAreaChange, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(ChangeCustomPropsEvent::subscribe(std::bind(&gui_personalShop::onChangeCustomProps, this, std::placeholders::_1)));
		m_subscriptionGuard.add(UpdateCustomPropsEvent::subscribe(std::bind(&gui_personalShop::onUpdateCustomProps, this, std::placeholders::_1)));
		
	}

	void gui_personalShop::onUpdate(ui32 nTimeElapse)
	{
		if (m_showTipTime > 0 && m_showTipTime < 3000)
		{
			m_showTipTime = nTimeElapse + m_showTipTime;
		}
		else
		{
			m_showTipTime = 0;
			m_tipMessage = "";
		}
		m_textTip->SetText(m_tipMessage.c_str());
	}

	bool gui_personalShop::onAreaChange(const Vector3 & startPos, const Vector3 & endPos)
	{
		Real min_x = Math::Min(startPos.x, endPos.x);
		Real min_y = Math::Min(startPos.y, endPos.y);
		Real min_z = Math::Min(startPos.z, endPos.z);
		Real max_x = Math::Max(startPos.x, endPos.x);
		Real max_y = Math::Max(startPos.y, endPos.y);
		Real max_z = Math::Max(startPos.z, endPos.z);
		m_minPos = Vector3(min_x, min_y, min_z);
		m_maxPos = Vector3(max_x, max_y, max_z);
		return true;
	}

	bool gui_personalShop::canOpen(Vector3 position, bool force)
	{
		if (force)
			return true;
		if (m_minPos == Vector3::ZERO && m_maxPos == Vector3::ZERO)
			return true;
		return position >= m_minPos && position <= m_maxPos;
	}

	bool gui_personalShop::canShow()
	{
		bool canShow = m_switchableProps.size() > size_t(0)
			|| m_upgradeProps.size() > size_t(0) 
			|| m_enchantmentProps.size() > size_t(0)
			|| m_customPropGroups.size() > size_t(0);
		if (!canShow)
			return false;
		if (UIDisplaySetting::getUIDisplay()->isHidePersonalShop)
			return false;
		auto player = Blockman::Instance()->m_pPlayer;
		if (player)
			canShow = canOpen(player->position, false);
		return canShow;
	}

	void gui_personalShop::refreshTabUI()
	{
		m_tabs[PanelId::SWITCHABLE_PROPS]->SetVisible(m_switchableProps.size() > size_t(0));
		m_tabs[PanelId::UPGRADE_PROPS]->SetVisible(m_upgradeProps.size() > size_t(0));
		m_tabs[PanelId::ENCHANTMENT_PROP]->SetVisible(m_enchantmentProps.size() > size_t(0));
		float top = 10;
		bool need_show_custom = true;
		bool need_select_tab = true;
		for (auto tab : m_tabs)
		{
			if (tab.second->IsVisible())
			{
				tab.second->SetYPosition(UDim(0, top));
				top += 80;
				need_show_custom = false;
				if (tab.second->IsSelected())
				{
					need_select_tab = false;
				}
			}
		}

		if (!m_tabs[PanelId::SWITCHABLE_PROPS]->IsVisible())
		{
			for (auto tab : m_tabs)
			{
				if (tab.second->IsVisible())
				{
					tab.second->SetSelected(true);
					break;
				}
			}
		}
		else
		{
			if (need_select_tab)
			{
				m_tabs[PanelId::SWITCHABLE_PROPS]->SetSelected(true);
			}
		}

		for (auto customTab : m_customTabs)
		{
			customTab.second->SetYPosition(UDim(0, top));
			top += 80;
			if (need_show_custom)
			{
				customTab.second->SetSelected(true);
				need_show_custom = false;
			}
		}

	}

	bool gui_personalShop::onCloseClick(const EventArgs & events)
	{
		playSoundByType(ST_ClosePanel);
		getParent()->showMainControl();
		m_dialogContent->SetVisible(false);
		return true;
	}

	bool gui_personalShop::onDialogClick(const EventArgs & events)
	{
		m_dialogContent->SetVisible(false);
		return true;
	}

	bool gui_personalShop::onRadioChange(const EventArgs & events, const PanelId panelId)
	{
		if (m_tabs[panelId]->IsSelected())
		{
			m_customPanel->hide();
			map<PanelId, GUIRadioButton*>::iterator iter;
			for (iter = m_tabs.begin(); iter != m_tabs.end(); iter++)
			{
				if (m_panels[iter->first] && m_panels[iter->first]->isShown())
				{
					m_panels[iter->first]->hide();
				}
			}
			if (m_panels[panelId])
			{
				m_panels[panelId]->show();
			}
			playSoundByType(ST_Click);
		}
		return true;
	}

	bool gui_personalShop::onCustomRadioChange(const EventArgs & events, const String & groupId)
	{
		for (auto iter = m_tabs.begin(); iter != m_tabs.end(); iter++)
		{
			if (m_panels[iter->first] && m_panels[iter->first]->isShown())
			{
				if (iter->second->IsSelected())
				{
					iter->second->SetSelected(false);
					m_panels[iter->first]->hide();
				}
			}
		}
		auto iter = m_customPropGroups.find(groupId);
		if (iter != m_customPropGroups.end())
		{
			m_customPanel->show();
			CustomPropGroup group = iter->second;
			auto panel = dynamic_cast<gui_customPropsPanel*>(m_customPanel);
			if (panel)
			{
				auto contentPanel = getWindow("PersonalShop-Content-Panel");
				panel->setCustomPropGroup(group.m_groupId, group.m_customProps, contentPanel->GetPixelSize().x);
			}
		}
		
		playSoundByType(ST_Click);
		return true;
	}

	bool gui_personalShop::onPersonalShopTip(const String & tip)
	{
		m_showTipTime = 1;
		m_tipMessage = tip;
		m_textTip->SetText(tip.c_str());
		return true;
	}

	void gui_personalShop::setTabText(GUIRadioButton * tab, String text)
	{
		if (tab && text.length() > 0)
		{
			GUIString title = getString(text);
			if (title.length() != 0)
				tab->SetText(title.c_str());
			else
				tab->SetText(text.c_str());
		}
	}

	bool gui_personalShop::onChangeSwitchableProps(const String & data)
	{
		if (parseSwitchablePropsData(data))
			refreshSwitchablePropsUI();
		return true;
	}

	bool gui_personalShop::parseSwitchablePropsData(const String & data)
	{
		if (data.length() == 0)
			return false;
		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(data.c_str());

		if (doc->HasParseError())
		{
			LordLogError("The switchable props content  HasParseError.");
			LordDelete(doc);
			return false;
		}

		if (doc->HasMember("title"))
		{
			setTabText(m_tabs[PanelId::SWITCHABLE_PROPS], doc->FindMember("title")->value.GetString());
		}

		if (!doc->HasMember("props"))
		{
			LordLogError("The switchable props content missed some field.");
			LordDelete(doc);
			return false;
		}

		m_switchableProps.clear();

		auto propsData = doc->FindMember("props")->value.GetArray();
		for (size_t i = 0; i < propsData.Size(); ++i)
		{
			auto obj = propsData[i].GetObject();

			if (!obj.HasMember("id") || !obj.HasMember("name") || !obj.HasMember("desc")
				|| !obj.HasMember("image") || !obj.HasMember("status") || !obj.HasMember("price"))
			{
				continue;
			}

			String id = obj.FindMember("id")->value.GetString();
			String name = obj.FindMember("name")->value.GetString();
			String desc = obj.FindMember("desc")->value.GetString();
			String image = obj.FindMember("image")->value.GetString();
			int status = obj.FindMember("status")->value.GetInt();
			int price = obj.FindMember("price")->value.GetInt();

			int rare = 0;
			if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::HASHall)
			{
				if (obj.HasMember("rare"))
				{
					rare = obj.FindMember("rare")->value.GetInt();
				}
			}

			m_switchableProps.push_back(SwitchableProp(id, name, desc, image, status, price, rare));
		}

		LordDelete(doc);
		return true;
	}

	void gui_personalShop::refreshSwitchablePropsUI()
	{
		auto switchablePropsPanel = dynamic_cast<gui_switchablePropsPanel*>(m_panels[PanelId::SWITCHABLE_PROPS]);
		if (switchablePropsPanel)
		{
			auto contentPanel = getWindow("PersonalShop-Content-Panel");
			switchablePropsPanel->refreshItems(m_switchableProps, contentPanel->GetPixelSize().x);
		}	
		refreshTabUI();
	}

	bool gui_personalShop::showSwitchableProp(const String & name, const String & image, const String & desc, int rare)
	{
		m_dialogContent->SetVisible(true);
		GUIStaticText* stName = getWindow<GUIStaticText>("PersonalShop-Dialog-Title");
		GUIStaticImage* siIcon = getWindow<GUIStaticImage>("PersonalShop-Dialog-Image-Prop");
		GUIStaticImage* siBg = getWindow<GUIStaticImage>("PersonalShop-Dialog-Image-Bg");
		stName->SetText(getString(name).c_str());
		siIcon->SetImage(image.c_str());
		siBg->SetImage(rare == 0 ? "set:app_shop.json image:app_shop_item_bg" : "set:hideandseek.json image:rare_bg");

		GUILayout* lDesc = getWindow<GUILayout>("PersonalShop-Dialog-Desc");
		lDesc->CleanupChildren();
		StringArray attrs = StringUtil::Split(desc, "&");
		int index = 0;
		for (String attr : attrs)
		{
			index += 1;
			GUIStaticText* stAttr = (GUIStaticText*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_TEXT, StringUtil::Format("PersonalShop-Dialog-Desc-%d", index).c_str());
			stAttr->SetWidth({ 0, 324.0f });
			stAttr->SetHeight({ 0, 25.0f });
			stAttr->SetXPosition(UDim(0, 8.0f));
			stAttr->SetYPosition(UDim(0, index * 50.0f - 35.0f));
			stAttr->SetTextBoader(LORD::Color::BLACK);
			stAttr->SetTextScale(1.2f);
			stAttr->SetWordWrap(true);
			if (StringUtil::Find(attr, "="))
			{
				StringArray kvPair = StringUtil::Split(attr, "=");
				stAttr->SetText(StringUtil::Format(getString(kvPair[0]).c_str(), kvPair[1].c_str()).c_str());
			}
			else
			{
				stAttr->SetText(getString(attr).c_str());
			}
			lDesc->AddChildWindow(stAttr);
		}
		return true;
	}

	bool gui_personalShop::onChangeUpgradeProps(const String & data)
	{
		if (parseUpgradePropsData(data))
			refreshUpgradePropsUI();
		return true;
	}

	bool gui_personalShop::parseUpgradePropsData(const String & data)
	{
		if (data.length() == 0)
			return false;
		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(data.c_str());

		if (doc->HasParseError())
		{
			LordLogError("The upgrade props content  HasParseError.");
			LordDelete(doc);
			return false;
		}

		if (doc->HasMember("title"))
		{
			setTabText(m_tabs[PanelId::UPGRADE_PROPS], doc->FindMember("title")->value.GetString());
		}

		if (!doc->HasMember("props"))
		{
			LordLogError("The upgrade props content missed some field.");
			LordDelete(doc);
			return false;
		}

		m_upgradeProps.clear();

		auto propsData = doc->FindMember("props")->value.GetArray();
		for (size_t i = 0; i < propsData.Size(); ++i)
		{
			auto obj = propsData[i].GetObject();

			if (!obj.HasMember("id") || !obj.HasMember("name") || !obj.HasMember("desc") || !obj.HasMember("image")
				|| !obj.HasMember("level") || !obj.HasMember("value") || !obj.HasMember("nextValue")|| !obj.HasMember("price"))
			{
				continue;
			}

			String id = obj.FindMember("id")->value.GetString();
			String name = obj.FindMember("name")->value.GetString();
			String desc = obj.FindMember("desc")->value.GetString();
			String image = obj.FindMember("image")->value.GetString();
			int level = obj.FindMember("level")->value.GetInt();
			int value = obj.FindMember("value")->value.GetInt();
			int nextValue = obj.FindMember("nextValue")->value.GetInt();
			int price = obj.FindMember("price")->value.GetInt();
			int status = obj.HasMember("status") ? obj.FindMember("status")->value.GetInt() : 0;

			m_upgradeProps.push_back(UpgradeProp(id, name, desc, image, level, value, nextValue, price, status));
		}

		LordDelete(doc);
		return true;
	}

	void gui_personalShop::refreshUpgradePropsUI()
	{
		auto upgradePropsPanel = dynamic_cast<gui_upgradePropsPanel*>(m_panels[PanelId::UPGRADE_PROPS]);
		if (upgradePropsPanel)
		{
			auto contentPanel = getWindow("PersonalShop-Content-Panel");
			upgradePropsPanel->refreshItems(m_upgradeProps, contentPanel->GetPixelSize().x);
		}
		refreshTabUI();
	}

	bool gui_personalShop::onChangeEnchantmentProps(const String & data)
	{
		if (parseEnchantmentPropsData(data))
			refreshEnchantmentPropsUI();
		return true;
	}

	bool gui_personalShop::parseEnchantmentPropsData(const String & data)
	{
		if (data.length() == 0)
			return false;
		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(data.c_str());

		if (doc->HasParseError())
		{
			LordLogError("The enchantment props content  HasParseError.");
			LordDelete(doc);
			return false;
		}

		if (doc->HasMember("title"))
		{
			setTabText(m_tabs[PanelId::UPGRADE_PROPS], doc->FindMember("title")->value.GetString());
		}

		if (!doc->HasMember("props"))
		{
			LordLogError("The enchantment props content missed some field.");
			LordDelete(doc);
			return false;
		}

		m_enchantmentProps.clear();

		auto propsData = doc->FindMember("props")->value.GetArray();
		for (size_t i = 0; i < propsData.Size(); ++i)
		{
			auto obj = propsData[i].GetObject();

			if (!obj.HasMember("id") || !obj.HasMember("name") || !obj.HasMember("desc") || !obj.HasMember("image")
				|| !obj.HasMember("itemId") || !obj.HasMember("price") || !obj.HasMember("isBuy"))
			{
				continue;
			}

			String id = obj.FindMember("id")->value.GetString();
			String name = obj.FindMember("name")->value.GetString();
			String desc = obj.FindMember("desc")->value.GetString();
			String image = obj.FindMember("image")->value.GetString();
			int itemId = obj.FindMember("itemId")->value.GetInt();
			int price = obj.FindMember("price")->value.GetInt();
			int isBuy = obj.FindMember("isBuy")->value.GetBool();

			m_enchantmentProps.push_back(EnchantmentProp(id, name, desc, image, itemId,  price, isBuy));
		}

		LordDelete(doc);
		return true;
	}

	void gui_personalShop::refreshEnchantmentPropsUI()
	{
		auto enchantmentPropsPanel = dynamic_cast<gui_enchantmentPropsPanel*>(m_panels[PanelId::ENCHANTMENT_PROP]);
		if (enchantmentPropsPanel)
		{
			auto contentPanel = getWindow("PersonalShop-Content-Panel");
			enchantmentPropsPanel->refreshItems(m_enchantmentProps, contentPanel->GetPixelSize().x);
		}
		refreshTabUI();
	}

	bool gui_personalShop::onChangeCustomProps(const String & data)
	{
		if (parseCustomProps(data))
			refreshCustomPropsUI();
		return true;
	}

	bool gui_personalShop::parseCustomProps(const String & data)
	{
		if (data.length() == 0)
			return false;
		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(data.c_str());

		if (doc->HasParseError())
		{
			LordLogError("The custom props content HasParseError.");
			LordDelete(doc);
			return false;
		}

		if (!doc->IsArray())
		{
			LordLogError("The custom props content is no array.");
			LordDelete(doc);
			return false;
		}

		removeOldCustomUI();
		
		auto groupsData = doc->GetArray();
		for (size_t i = 0; i < groupsData.Size(); ++i)
		{
			auto group_obj = groupsData[i].GetObject();

			if (!group_obj.HasMember("id") || !group_obj.HasMember("name") || !group_obj.HasMember("props"))
			{
				continue;
			}

			auto propsData = group_obj.FindMember("props")->value.GetArray();
			if (propsData.Size() == size_t(0))
			{
				continue;
			}
			std::list<CustomProp> customProps;
			for (size_t i = 0; i < propsData.Size(); ++i)
			{
				auto obj = propsData[i].GetObject();
				if (!obj.HasMember("id") || !obj.HasMember("name") || !obj.HasMember("desc")
					|| !obj.HasMember("image") || !obj.HasMember("price"))
				{
					continue;
				}
				String id = obj.FindMember("id")->value.GetString();
				String name = obj.FindMember("name")->value.GetString();
				String image = obj.FindMember("image")->value.GetString();
				String desc = obj.FindMember("desc")->value.GetString();
				String values = obj.HasMember("values") ? obj.FindMember("values")->value.GetString() : "";
				int price = obj.FindMember("price")->value.GetInt();
				int status = obj.HasMember("status") ? obj.FindMember("status")->value.GetInt() : 1;
				customProps.push_back(CustomProp(id, name, image, desc, values, price, status));
			}
			String groupId = group_obj.FindMember("id")->value.GetString();
			String groupName = group_obj.FindMember("name")->value.GetString();
			m_customPropGroups.insert({ groupId , CustomPropGroup(groupId, groupName, customProps) });
		}

		LordDelete(doc);
		return true;
	}

	void gui_personalShop::removeOldCustomUI()
	{
		auto tabLayout = getWindow("PersonalShop-Content-Tabs");
		for (auto tab : m_customTabs)
		{
			tabLayout->RemoveChildWindow(tab.second);
		}
		m_customTabs.clear();
		m_customPropGroups.clear();
	}

	void gui_personalShop::refreshCustomPropsUI()
	{
		static unsigned groupCount = 0;
		auto tabLayout = getWindow("PersonalShop-Content-Tabs");
		auto contentPanel = getWindow("PersonalShop-Content-Panel");
		for (auto customGroup : m_customPropGroups)
		{
			groupCount++;
			CustomPropGroup group = customGroup.second;

			String tabName = StringUtil::Format("PersonalShop-Custom-Tab-%d", groupCount);
			GUIRadioButton* tab = (GUIRadioButton*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_RADIO, tabName.c_str());
			tab->SetArea(UDim(0, -5.f), UDim(0, 0), UDim(1.f, 10.f), UDim(0, 80.f));
			tab->SetNormalImage("set:app_shop.json image:app_shop_tab_nor");
			tab->SetPushedImage("set:app_shop.json image:app_shop_tab_pre");
			tab->SetProperty(GWP_WINDOW_STRETCH_TYPE, "NineGrid");
			tab->SetProperty(GWP_WINDOW_STRETCH_OFFSET, "15 15 15 15");
			tab->SetProperty(GWP_TEXT_BORDER, "true");
			tab->SetText(getString(group.m_name));
			tab->subscribeEvent(EventRadioStateChanged, std::bind(&gui_personalShop::onCustomRadioChange, this, std::placeholders::_1, group.m_groupId));
			tabLayout->AddChildWindow(tab);
			m_customTabs.insert({ group.m_groupId, tab });
		}
		refreshTabUI();
	}

	bool gui_personalShop::onUpdateCustomProps(const String & data)
	{
		if (data.length() == 0)
			return false;
		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(data.c_str());

		if (doc->HasParseError())
		{
			LordLogError("The update custom prop content HasParseError.");
			LordDelete(doc);
			return false;
		}

		if (!doc->IsArray())
		{
			LordLogError("The update custom props content is no array.");
			LordDelete(doc);
			return false;
		}

		auto items = doc->GetArray();
		for (size_t i = 0; i < items.Size(); ++i)
		{
			auto obj = items[i].GetObject();
			if (!obj.HasMember("groupId") || !obj.HasMember("id"))
			{
				continue;
			}
			String groupId = obj.FindMember("groupId")->value.GetString();
			auto iter = m_customPropGroups.find(groupId);
			if (iter == m_customPropGroups.end())
			{
				continue;
			}
			String id = obj.FindMember("id")->value.GetString();
			String name = obj.HasMember("name") ? obj.FindMember("name")->value.GetString() : "";
			String image = obj.HasMember("image") ? obj.FindMember("image")->value.GetString() : "";
			String desc = obj.HasMember("desc") ? obj.FindMember("desc")->value.GetString() : "";
			String values = obj.HasMember("values") ? obj.FindMember("values")->value.GetString() : "";
			int price = obj.HasMember("price") ? obj.FindMember("price")->value.GetInt() : -1;
			int status = obj.HasMember("status") ? obj.FindMember("status")->value.GetInt() : 0;
			CustomProp prop = CustomProp(id, name, image, desc, values, price, status);
			iter->second.updateCustomProp(prop);
			auto panel = dynamic_cast<gui_customPropsPanel*>(m_customPanel);
			if (panel)
				panel->updateProp(prop);
		}
		LordDelete(doc);
		return true;
	}

}