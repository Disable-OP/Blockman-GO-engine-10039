#include "gui_superShop.h"
#include "gui_superPropsPanel.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUILayout.h"
#include "Util/LanguageKey.h"
#include "rapidjson/document.h"
#include "game.h"

namespace BLOCKMAN
{
	gui_superShop::gui_superShop()
		: gui_layout("SuperShop.json")
	{
		m_panels[PanelId::SUPER_PROPS] = m_panels[PanelId::SUPER_PROPS] ? m_panels[PanelId::SUPER_PROPS] : LordNew gui_superPropsPanel;
	}

	gui_superShop::~gui_superShop()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_superShop::onLoad()
	{
		auto title = getWindow<GUIStaticText>("SuperShop-TitleName");
		title->SetText(getString(LanguageKey::GUI_STR_SUPER_SHOP_SHOP));

		m_tabs[PanelId::SUPER_PROPS] = getWindow<GUIRadioButton>("SuperShop-Content-Tab-Super-Props");

		m_tabs[PanelId::SUPER_PROPS]->SetText(getString(LanguageKey::GUI_STR_SUPER_SHOP_SHOP));

		auto contentPanel = getWindow("SuperShop-Content-Panel");
		map<PanelId, GUIRadioButton*>::iterator  iter;
		for (iter = m_tabs.begin(); iter != m_tabs.end(); iter++)
		{
			iter->second->subscribeEvent(EventRadioStateChanged, std::bind(&gui_superShop::onRadioChange, this, std::placeholders::_1, iter->first));
			if (m_panels[iter->first])
			{
				m_panels[iter->first]->attachTo(this, contentPanel);
			}
		}
		m_tabs[PanelId::SUPER_PROPS]->SetSelected(true);

		m_textTip = getWindow<GUIStaticText>("SuperShop-Content-Tip");
		auto btnClose = getWindow<GUIButton>("SuperShop-BtnClose");
		btnClose->subscribeEvent(EventButtonClick, std::bind(&gui_superShop::onCloseClick, this, std::placeholders::_1));

		m_dialogContent = getWindow<GUIButton>("SuperShop-Dialog-Content");
		m_dialogContent->SetVisible(false);
		m_dialogContent->subscribeEvent(EventButtonClick, std::bind(&gui_superShop::onDialogClick, this, std::placeholders::_1));

		m_subscriptionGuard.add(ChangeSuperPropsEvent::subscribe(std::bind(&gui_superShop::onChangeSuperProps, this, std::placeholders::_1)));
		m_subscriptionGuard.add(SuperShopTipEvent::subscribe(std::bind(&gui_superShop::onSuperShopTip, this, std::placeholders::_1)));
		m_subscriptionGuard.add(ShowSuperPropEvent::subscribe(std::bind(&gui_superShop::showSuperProp, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));

	}

	void gui_superShop::onUpdate(ui32 nTimeElapse)
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

	bool gui_superShop::onCloseClick(const EventArgs & events)
	{
		getParent()->showMainControl();
		m_dialogContent->SetVisible(false);
		return true;
	}

	void gui_superShop::setTitleText(String text)
	{
		auto tvTitle = getWindow<GUIStaticText>("SuperShop-TitleName");
		if (tvTitle && text.length() > 0)
		{
			tvTitle->SetText(text.c_str());
		}	
	}

	void gui_superShop::setTabText(GUIRadioButton * tab, String text)
	{
		if (tab && text.length() > 0)
		{
			GUIString title = getString(text);
			if (title.length() != 0)
				tab->SetText(title.c_str());
			else
				tab->SetText(text.c_str());
			setTitleText(title.c_str());
		}
	}

	bool gui_superShop::onDialogClick(const EventArgs & events)
	{
		m_dialogContent->SetVisible(false);
		return true;
	}

	bool gui_superShop::onSuperShopTip(const String & tip)
	{
		m_showTipTime = 1;
		m_tipMessage = tip;
		m_textTip->SetText(tip.c_str());
		return true;
	}

	bool gui_superShop::onRadioChange(const EventArgs & events, const PanelId panelId)
	{
		if (m_tabs[panelId]->IsSelected())
		{
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
				setTitleText(m_tabs[panelId]->GetText().c_str());
			}
			playSoundByType(ST_Click);
		}
		return true;
	}

	bool gui_superShop::onChangeSuperProps(const String & data)
	{
		if (parseSuperPropsData(data))
			refreshSuperPropsUI();
		return true;
	}

	bool gui_superShop::parseSuperPropsData(const String & data)
	{
		if (data.length() == 0)
			return false;
		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(data.c_str());

		if (doc->HasParseError())
		{
			LordLogError("The super props content HasParseError.");
			LordDelete(doc);
			return false;
		}

		if (doc->HasMember("title"))
		{
			setTabText(m_tabs[PanelId::SUPER_PROPS], doc->FindMember("title")->value.GetString());
		}

		if (!doc->HasMember("props"))
		{
			LordLogError("The super props content missed some field.");
			LordDelete(doc);
			return false;
		}

		m_superProps.clear();

		auto propsData = doc->FindMember("props")->value.GetArray();
		for (size_t i = 0; i < propsData.Size(); ++i)
		{
			auto obj = propsData[i].GetObject();

			if (!obj.HasMember("id") || !obj.HasMember("name") || !obj.HasMember("content")
				|| !obj.HasMember("image") || !obj.HasMember("status") || !obj.HasMember("price"))
			{
				continue;
			}

			String id = obj.FindMember("id")->value.GetString();
			String name = obj.FindMember("name")->value.GetString();
			String content = obj.FindMember("content")->value.GetString();
			String desc = obj.HasMember("desc") ? obj.FindMember("desc")->value.GetString() : "";
			String image = obj.FindMember("image")->value.GetString();
			int status = obj.FindMember("status")->value.GetInt();
			int price = obj.FindMember("price")->value.GetInt();
			int value = obj.HasMember("value") ? obj.FindMember("value")->value.GetInt() : 0;

			m_superProps.push_back(SuperProp(id, name, content, desc, image, status, price, value));
		}

		LordDelete(doc);
		return true;
	}

	void gui_superShop::refreshSuperPropsUI()
	{
		auto superPropsPanel = dynamic_cast<gui_superPropsPanel*>(m_panels[PanelId::SUPER_PROPS]);
		if (superPropsPanel)
		{
			auto contentPanel = getWindow("SuperShop-Content-Panel");
			superPropsPanel->refreshItems(m_superProps, contentPanel->GetPixelSize().x);
		}
	}

	bool gui_superShop::showSuperProp(const String & name, const String & image, const String & desc)
	{
		m_dialogContent->SetVisible(true);
		GUIStaticText* stName = getWindow<GUIStaticText>("SuperShop-Dialog-Title");
		GUIStaticImage* siIcon = getWindow<GUIStaticImage>("SuperShop-Dialog-Image-Prop");
		stName->SetText(getString(name).c_str());
		siIcon->SetImage(image.c_str());

		GUILayout* lDesc = getWindow<GUILayout>("SuperShop-Dialog-Desc");
		lDesc->CleanupChildren();
		StringArray attrs = StringUtil::Split(desc, "&");
		int index = 0;
		for (String attr : attrs)
		{
			index += 1;
			GUIStaticText* stAttr = (GUIStaticText*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_TEXT, StringUtil::Format("SuperShop-Dialog-Desc-%d", index).c_str());
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

}