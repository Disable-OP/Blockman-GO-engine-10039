#include "GUIGunStoreTab.h"
#include "UI/GUIWindowManager.h"
#include "UI/GUILayout.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "Util/LanguageManager.h"
#include "Util/ClientEvents.h"

namespace BLOCKMAN
{
	GUIGunStoreTab::GUIGunStoreTab(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("GunStoreTab.json"));
		renameComponents(m_window);
		
		AddWindowToDrawList(*m_window);
		SetTouchable(false);
	}

	void GUIGunStoreTab::removeComponents()
	{
		if (m_window)
		{
			RemoveWindowFromDrawList(*m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	void GUIGunStoreTab::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());
		BaseClass::UpdateSelf(nTimeElapse);
	}

	void GUIGunStoreTab::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GUIGunStoreTab::InitStoreTab(StoreTab * tab)
	{
		m_type = tab->Type;
		m_tab_type = tab->TabType;
		auto border = dynamic_cast<GUICheckBox*>(m_window->GetChildByIndex(0));
		auto icon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(1));
		auto name = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(2));
		icon->SetImage(tab->Icon.c_str());
		name->SetText(LanguageManager::Instance()->getString(tab->Name).c_str());
		border->subscribeEvent(EventCheckStateChanged, std::bind(&GUIGunStoreTab::OnTabSelected, this, std::placeholders::_1));
		UpdateNewCount();
	}

	void GUIGunStoreTab::UpdateNewCount()
	{
		int count = 0;
		switch (m_tab_type)
		{
		case BLOCKMAN::STORE_TAB_GUN:
		{
			list<StoreGun*>::type guns = GunStoreSetting::findStoreGunByTabType(m_type);
			for (auto gun : guns)
			{
				if (gun->Status == STATUS_NEW)
				{
					count++;
				}
			}
			break;
		}
		case BLOCKMAN::STORE_TAB_PROP:
		{
			list<StoreProp*>::type props = GunStoreSetting::findStorePropByTabType(m_type);
			for (auto prop : props)
			{
				if (prop->Status == STATUS_NEW)
				{
					count++;
				}
			}
			break;
		}
		default:
			break;
		}
		if (count <= 0)
		{
			m_window->GetChildByIndex(3)->SetVisible(false);
			return;
		}
		m_window->GetChildByIndex(3)->SetVisible(true);
		auto new_count = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(3)->GetChildByIndex(0));
		new_count->SetText(StringUtil::ToString(count).c_str());
	}

	void GUIGunStoreTab::SetTabSelected(bool selected)
	{
		auto border = dynamic_cast<GUICheckBox*>(m_window->GetChildByIndex(0));
		border->SetChecked(selected);
		border->SetTouchable(!selected);
	}

	bool GUIGunStoreTab::IsTabSelected()
	{
		auto border = dynamic_cast<GUICheckBox*>(m_window->GetChildByIndex(0));
		return border->GetChecked();
	}

	bool GUIGunStoreTab::OnTabSelected(const EventArgs & events)
	{
		if (IsTabSelected())
		{
			auto border = dynamic_cast<GUICheckBox*>(m_window->GetChildByIndex(0));
			border->SetTouchable(false);
			GunStoreTabChangeEvent::emit(m_type);
		}
		return true;
	}
}
