#include "gui_gunStore.h"
#include "GUI/GuiDef.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticText.h"
#include "Util/ClientEvents.h"
#include "Util/LanguageKey.h"
#include "Setting/GunStoreSetting.h"

namespace BLOCKMAN
{
	gui_gunStore::gui_gunStore() : 
		gui_layout("GunStore.json")
	{
		m_store_panel = m_store_panel ? m_store_panel : LordNew GUIGunStorePanel;
		m_store_model = m_store_model ? m_store_model : LordNew GUIGunStoreModel;
		m_store_infos = m_store_infos ? m_store_infos : LordNew GUIGunStoreInfos;
		m_store_gun_detail = m_store_gun_detail ? m_store_gun_detail : LordNew GUIStoreGunDetail;
		m_store_prop_detail = m_store_prop_detail ? m_store_prop_detail : LordNew GUIStorePropDetail;
	}

	gui_gunStore::~gui_gunStore()
	{
		LordSafeDelete(m_store_panel);
		LordSafeDelete(m_store_model);
		LordSafeDelete(m_store_infos);
		LordSafeDelete(m_store_gun_detail);
		LordSafeDelete(m_store_prop_detail);
		m_subscriptionGuard.unsubscribeAll();
		GunStoreSetting::unloadSetting();
	}

	void gui_gunStore::onLoad()
	{
		GunStoreSetting::loadSetting();
		initTabsAndPanels();

		GUILayout* Store = getWindow<GUILayout>("GunStore");
		m_store_gun_detail->attachTo(this, Store);
		m_store_prop_detail->attachTo(this, Store);
		m_store_gun_detail->hide();
		m_store_prop_detail->hide();

		GUILayout* Model = getWindow<GUILayout>("GunStore-Preview-Model");
		m_store_model->attachTo(this, Model);
		GUILayout* Infos = getWindow<GUILayout>("GunStore-Preview-Infos");
		m_store_infos->attachTo(this, Infos);

		auto Close = getWindow<GUIButton>("GunStore-Close");
		Close->subscribeEvent(EventButtonClick, std::bind(&gui_gunStore::onCloseClick, this, std::placeholders::_1));

		auto Title = getWindow<GUIStaticText>("GunStore-Title");
		Title->SetText(getString(LanguageKey::GUI_GUN_STORE_NAME));

		m_subscriptionGuard.add(GunStoreTabChangeEvent::subscribe(std::bind(&gui_gunStore::onTabChange, this, std::placeholders::_1)));
		m_subscriptionGuard.add(GunStoreGunItemClickEvent::subscribe(std::bind(&gui_gunStore::onGunItemClick, this, std::placeholders::_1)));
		m_subscriptionGuard.add(GunStorePropItemClickEvent::subscribe(std::bind(&gui_gunStore::onPropItemClick, this, std::placeholders::_1)));
		m_subscriptionGuard.add(ShowStoreGunDetailEvent::subscribe(std::bind(&gui_gunStore::onShowGunDetail, this, std::placeholders::_1)));
		m_subscriptionGuard.add(ShowStorePropDetailEvent::subscribe(std::bind(&gui_gunStore::onShowPropDetail, this, std::placeholders::_1)));

		m_subscriptionGuard.add(UpdateStoreGunDataEvent::subscribe(std::bind(&gui_gunStore::onUpdateStoreGunData, this, std::placeholders::_1)));
		m_subscriptionGuard.add(UpdateStorePropDataEvent::subscribe(std::bind(&gui_gunStore::onUpdateStorePropData, this, std::placeholders::_1)));
	}
	
	void gui_gunStore::onShow()
	{
		initPanelsChilds();
	}

	void gui_gunStore::onUpdate(ui32 nTimeElapse)
	{
		GunStoreSetting::onUpdate(nTimeElapse);
	}

	void gui_gunStore::initTabsAndPanels()
	{
		GUILayout* Tabs = getWindow<GUILayout>("GunStore-Content-Tabs");
		GUILayout* Panels = getWindow<GUILayout>("GunStore-Content-Panels");
		if (Tabs == NULL || Panels == NULL)
			return;
		list<StoreTab*>::type tabs = GunStoreSetting::getStoreTabs();
		if (tabs.empty())
			return;
		m_store_panel->attachTo(this, Panels);
		m_store_panel->InitPanels(Panels->GetPixelSize().x);
		size_t MinCount = Math::Max(tabs.size(), size_t(7));
		float TabSize = (Tabs->GetPixelSize().y - 1.0f - MinCount) / MinCount;
		Tabs->SetWidth({ 0.0f, TabSize });
		static unsigned index = 0;
		float height = 1.0f / MinCount;
		int cur_tab_count = 0;
		for (auto tab : tabs)
		{
			index++;
			cur_tab_count++;
			GUIString TabName = StringUtil::Format("GunStoreTab-%d", index).c_str();
			GUIGunStoreTab* pGunStoreTab = (GUIGunStoreTab*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_GUN_STORE_TAB, TabName);
			pGunStoreTab->SetWidth({ 0.0f, 110.0f });
			pGunStoreTab->SetHeight({ 0.0f, TabSize });
			pGunStoreTab->SetYPosition({ 0.0f, 1.0f * cur_tab_count + (cur_tab_count - 1) * TabSize });
			pGunStoreTab->InitStoreTab(tab);
			Tabs->AddChildWindow(pGunStoreTab);
			m_store_tabs.push_back(pGunStoreTab);
		}
		GUIGunStoreTab* tab = *m_store_tabs.begin();
		tab->SetTabSelected(true);
		m_store_panel->ShowPanelByType(tab->GetType());
	}

	void gui_gunStore::initPanelsChilds()
	{
		if (m_init)
			return;
		m_init = true;
		GUILayout* Panels = getWindow<GUILayout>("GunStore-Content-Panels");
		if (Panels == NULL)
			return;
		m_store_panel->InitPanelChilds();
	}

	void gui_gunStore::updateTabNewCount(int tabType)
	{
		for (auto tab : m_store_tabs)
		{
			if (tab->GetType() == tabType)
			{
				tab->UpdateNewCount();
				break;
			}
		}
	}

	bool gui_gunStore::onCloseClick(const EventArgs & events)
	{
		playSoundByType(ST_ClosePanel);
		getParent()->showMainControl();
		return true;
	}

	bool gui_gunStore::onTabChange(int type)
	{
		playSoundByType(ST_Click);
		for (auto tab : m_store_tabs)
		{
			if (tab->GetType() == type)
			{
				if (tab->GetTabType() == STORE_TAB_GUN)
				{
					StoreGun* gun = GunStoreSetting::findSelectedGunByTabType(type);
					if (gun)
					{
						onGunItemClick(gun->Id);
					}
				}
				if (tab->GetTabType() == STORE_TAB_PROP)
				{
					StoreProp* prop = GunStoreSetting::findSelectedPropByTabType(type);
					if (prop)
					{
						onPropItemClick(prop->Id);
					}
				}
				continue;
			}
			if (tab->IsTabSelected())
			{
				tab->SetTabSelected(false);
			}
		}
		m_store_panel->ShowPanelByType(type);
		return true;
	}

	bool gui_gunStore::onGunItemClick(int gunId)
	{	
		StoreGun* gun = GunStoreSetting::findStoreGunById(gunId);
		if (!gun)
			return false;
		if (m_cur_gunId == gunId)
		{
			if (gun->HasChange)
			{
				updateTabNewCount(gun->TabType);
			}
			ShowStoreGunDetailEvent::emit(gun->Id);
			return true;
		}
		StoreGun* pre_gun = GunStoreSetting::findSelectedGunByTabType(gun->TabType);
		if (pre_gun)
		{
			pre_gun->IsSelected = false;
			pre_gun->HasChange = true;
		}
		playSoundByType(ST_Click);
		gun->IsSelected = true;
		gun->HasChange = true;
		m_store_model->SetStoreGun(gun);
		m_store_infos->SetStoreGun(gun);
		m_cur_gunId = gunId;
		m_cur_propId = 0;
		updateTabNewCount(gun->TabType);
		return true;
	}

	bool gui_gunStore::onPropItemClick(int propId)
	{
		StoreProp* prop = GunStoreSetting::findStorePropById(propId);
		if (!prop)
			return false;
		if (m_cur_propId == propId)
		{
			if (prop->HasChange)
			{
				updateTabNewCount(prop->TabType);
			}
			ShowStorePropDetailEvent::emit(prop->Id);
			return true;
		}
		StoreProp* pre_prop = GunStoreSetting::findSelectedPropByTabType(prop->TabType);
		if (pre_prop)
		{
			pre_prop->IsSelected = false;
			pre_prop->HasChange = true;
		}
		playSoundByType(ST_Click);
		prop->IsSelected = true;
		prop->HasChange = true;
		m_store_model->SetStoreProp(prop);
		m_store_infos->SetStoreProp(prop);
		m_cur_propId = propId;
		m_cur_gunId = 0;
		updateTabNewCount(prop->TabType);
		return true;
	}

	bool gui_gunStore::onShowGunDetail(int gunId)
	{
		StoreGun* gun = GunStoreSetting::findStoreGunById(gunId);
		if (!gun)
			return false;
		m_store_gun_detail->show();
		m_store_gun_detail->SetStoreGun(gun);
		return true;
	}

	bool gui_gunStore::onShowPropDetail(int propId)
	{
		StoreProp* prop = GunStoreSetting::findStorePropById(propId);
		if (!prop)
			return false;
		m_store_prop_detail->show();
		m_store_prop_detail->SetStoreProp(prop);
		return true;
	}

	bool gui_gunStore::onUpdateStoreGunData(const String & data)
	{
		map<int, bool>::type needSortTypes;
		list<StoreGun*>::type result = GunStoreSetting::parseUpdateStoreGunData(data, needSortTypes);
		bool hasNew = false;
		for (auto gun : result)
		{
			if (gun->Status == STATUS_NEW)
			{
				hasNew = true;
			}
			if (gun->Id == m_cur_gunId)
			{
				if (m_store_gun_detail->isShown())
				{
					m_store_gun_detail->SetStoreGun(gun);
				}
				m_store_infos->SetStoreGun(gun);
			}
		}
		for (auto needSort : needSortTypes)
		{
			m_store_panel->ResetPanelChilds(needSort.first);
		}
		if (!hasNew)
		{
			return true;
		}
		for (auto tab : m_store_tabs)
		{
			if (tab->GetTabType() == STORE_TAB_GUN)
			{
				tab->UpdateNewCount();
			}
		}
		return true;
	}

	bool gui_gunStore::onUpdateStorePropData(const String & data)
	{
		map<int, bool>::type needSortTypes;
		list<StoreProp*>::type result = GunStoreSetting::parseUpdateStorePropData(data, needSortTypes);
		bool hasNew = false;
		for (auto prop : result)
		{
			if (prop->Status == STATUS_NEW)
			{
				hasNew = true;
			}
			if (prop->Id == m_cur_propId)
			{
				if (m_store_prop_detail->isShown())
				{
					m_store_prop_detail->SetStoreProp(prop);
				}
				m_store_infos->SetStoreProp(prop);
			}
		}
		for (auto needSort : needSortTypes)
		{
			m_store_panel->ResetPanelChilds(needSort.first);
		}
		if (!hasNew)
		{
			return true;
		}
		for (auto tab : m_store_tabs)
		{
			if (tab->GetTabType() == STORE_TAB_PROP)
			{
				tab->UpdateNewCount();
			}
		}
		return true;
	}

}