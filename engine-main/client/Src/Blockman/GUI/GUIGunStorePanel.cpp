#include "GUIGunStorePanel.h"
#include "UI/GUIWindowManager.h"
#include "UI/GUILayout.h"
#include "GUI/GUIStoreGunItem.h"
#include "GUI/GUIStorePropItem.h"
#include "GUI/GuiDef.h"
#include "Util/ClientEvents.h"

namespace BLOCKMAN
{
	GUIGunStorePanel::GUIGunStorePanel() :
		gui_layout("GunStorePanel.json")
	{
	}

	GUIGunStorePanel::~GUIGunStorePanel()
	{
	}

	void GUIGunStorePanel::onLoad()
	{
	}

	void GUIGunStorePanel::onUpdate(ui32 nTimeElapse)
	{
	}

	void GUIGunStorePanel::InitPanels(float contentSize)
	{
		this->ContentSize = contentSize;
		auto GunStorePanel = getWindow("GunStorePanel");
		if (GunStorePanel == NULL)
			return;
		list<StoreTab*>::type tabs = GunStoreSetting::getStoreTabs();
		if (tabs.empty())
			return;
		static unsigned index = 0;
		for (auto tab : tabs)
		{
			index++;
			GUIString ChildsName = StringUtil::Format("GunStorePanel-Childs-%d", index).c_str();
			GUIGridView* pChildGV = (GUIGridView*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_GRID_VIEW, ChildsName);
			pChildGV->SetArea(UDim(0, 0), UDim(0, 0), UDim(1.f, 0), UDim(1.f, 0));
			pChildGV->InitConfig(4.f, 4.f, 5);
			GunStorePanel->AddChildWindow(pChildGV);
			m_panel_map[tab] = pChildGV;
		}
	}

	void GUIGunStorePanel::InitPanelChilds()
	{
		for (auto panel : m_panel_map)
		{
			StoreTab* tab = panel.first;
			GUIGridView* childs = panel.second;
			switch (tab->TabType)
			{
			case BLOCKMAN::STORE_TAB_GUN:
				InitGunChilds(tab, childs);
				break;
			case BLOCKMAN::STORE_TAB_PROP:
				InitPropChilds(tab, childs);
				break;
			default:
				break;
			}
		}
	}

	void GUIGunStorePanel::ResetPanelChilds(int type)
	{
		for (auto panel : m_panel_map)
		{
			StoreTab* tab = panel.first;
			GUIGridView* childs = panel.second;
			if (tab->Type == type)
			{
				switch (tab->TabType)
				{
				case BLOCKMAN::STORE_TAB_GUN:
					ResetGunChilds(tab, childs);
					break;
				case BLOCKMAN::STORE_TAB_PROP:
					ResetPropChilds(tab, childs);
					break;
				default:
					break;
				}
			}
		}
	}

	void GUIGunStorePanel::ShowPanelByType(int type)
	{
		for (auto panel : m_panel_map)
		{
			StoreTab* tab = panel.first;
			GUIGridView* childs = panel.second;
			childs->SetVisible(tab->Type == type);
		}
	}

	void GUIGunStorePanel::InitGunChilds(StoreTab * tab, GUIGridView * childs)
	{
		list<StoreGun*>::type guns = GunStoreSetting::findStoreGunByTabType(tab->Type);
		childs->RemoveAllItems();
		static unsigned GunChildCount = 0;
		float ChildWidth = (ContentSize - 4.0f * 4) / 5;
		bool IsFirst = true;
		for (auto gun : guns)
		{
			if (IsFirst)
			{
				gun->IsSelected = true;
				if (tab->Type == 1)
				{
					GunStoreGunItemClickEvent::emit(gun->Id);
				}
			}
			GunChildCount++;
			GUIString ChildName = StringUtil::Format("GUIGunStorePanel-Gun-Child-%d", GunChildCount).c_str();
			GUIStoreGunItem* GunItem = (GUIStoreGunItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STORE_GUN_ITEM, ChildName);
			GunItem->SetWidth({ 0, ChildWidth });
			GunItem->SetHeight({ 0, ChildWidth * 0.8f });
			GunItem->InitStoreGun(gun);
			childs->AddItem(GunItem);
			IsFirst = false;
		}
	}

	void GUIGunStorePanel::InitPropChilds(StoreTab * tab, GUIGridView * childs)
	{
		list<StoreProp*>::type props = GunStoreSetting::findStorePropByTabType(tab->Type);
		childs->RemoveAllItems();
		static unsigned PropChildCount = 0;
		float ChildWidth = (ContentSize - 4.0f * 4) / 5;
		bool IsFirst = true;
		for (auto prop : props)
		{
			if (IsFirst)
			{
				prop->IsSelected = true;
				if (tab->Type == 1)
				{
					GunStorePropItemClickEvent::emit(prop->Id);
				}
			}
			PropChildCount++;
			GUIString ChildName = StringUtil::Format("GUIGunStorePanel-Prop-Child-%d", PropChildCount).c_str();
			GUIStorePropItem* PropItem = (GUIStorePropItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STORE_PROP_ITEM, ChildName);
			PropItem->SetWidth({ 0, ChildWidth });
			PropItem->SetHeight({ 0, ChildWidth * 0.8f });
			PropItem->InitStoreProp(prop);
			childs->AddItem(PropItem);
			IsFirst = false;
		}
	}

	void GUIGunStorePanel::ResetGunChilds(StoreTab * tab, GUIGridView * childs)
	{
		list<StoreGun*>::type guns = GunStoreSetting::findStoreGunByTabType(tab->Type);
		size_t count = childs->GetItemCount();
		auto iter = guns.begin();
		for (size_t i = 0; i < count; i++)
		{
			if (iter == guns.end())
				break;
			GUIStoreGunItem* GunItem = (GUIStoreGunItem*)childs->GetItem(i);
			if (GunItem)
			{
				GunItem->InitStoreGun(*iter);
			}
			iter++;
		}
	}

	void GUIGunStorePanel::ResetPropChilds(StoreTab * tab, GUIGridView * childs)
	{
		list<StoreProp*>::type props = GunStoreSetting::findStorePropByTabType(tab->Type);
		size_t count = childs->GetItemCount();
		auto iter = props.begin();
		for (size_t i = 0; i < count; i++)
		{
			if (iter == props.end())
				break;
			GUIStorePropItem* PropItem = (GUIStorePropItem*)childs->GetItem(i);
			if (PropItem)
			{
				PropItem->InitStoreProp(*iter);
			}
			iter++;
		}
	}

}
