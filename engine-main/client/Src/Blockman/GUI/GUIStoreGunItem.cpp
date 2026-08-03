#include "GUIStoreGunItem.h"
#include "UI/GUIWindowManager.h"
#include "Util/ClientEvents.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"

namespace BLOCKMAN
{
	GUIStoreGunItem::GUIStoreGunItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("StoreGunItem.json"));
		renameComponents(m_window);
		initView();
		AddWindowToDrawList(*m_window);
	}

	void GUIStoreGunItem::removeComponents()
	{
		if (m_window)
		{
			RemoveWindowFromDrawList(*m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	void GUIStoreGunItem::initView()
	{
		m_status_box = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		m_image = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(1));
		m_use_line = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(3));
		m_hint_new = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(4));
		m_price = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(5));
		m_price_icon = dynamic_cast<GUIStaticImage*>(m_price->GetChildByIndex(0));
		m_price_text = dynamic_cast<GUIStaticText*>(m_price->GetChildByIndex(1));
		m_lock = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(6));
		m_lock_level = dynamic_cast<GUIButton*>(m_lock->GetChildByIndex(0));
		auto ItemClickBox = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(2));
		ItemClickBox->subscribeEvent(EventButtonClick, std::bind(&GUIStoreGunItem::OnItemChick, this, std::placeholders::_1));
	}

	void GUIStoreGunItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x }, { 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());
		BaseClass::UpdateSelf(nTimeElapse);
		if (m_data == NULL)
			return;
		if (m_data->HasChange)
		{
			InitStoreGun(m_data);
		}
	}

	void GUIStoreGunItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GUIStoreGunItem::InitStoreGun(StoreGun * gun)
	{
		m_data = gun;
		m_data->HasChange = false;
		
		if (gun->IsSelected)
			m_status_box->SetImage("set:gun_store.json image:border_use");
		else
			if (gun->Status == STATUS_LOCK || gun->Status == STATUS_NEW || gun->Status == STATUS_NOT_GET)
				m_status_box->SetImage("set:gun_store.json image:border_nor");
			else
				m_status_box->SetImage("set:gun_store.json image:border_has_get");
			
		m_image->SetImage(gun->Status == STATUS_LOCK ? gun->LockImage.c_str() : gun->Image.c_str());
		m_use_line->SetVisible(gun->Status == STATUS_HAS_USE);
		m_use_line->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_BTN_EQUIPPED).c_str());
		m_hint_new->SetVisible(gun->Status == STATUS_NEW);
		m_price->SetVisible(gun->Status != STATUS_HAS_GET && gun->Status != STATUS_HAS_USE);
		m_price_icon->SetImage(GunStoreSetting::getPriceIcon(gun->PriceType).c_str());
		String price = GunStoreSetting::getPriceText(gun->PriceType, gun->Chip, gun->Price, gun->Status).c_str();
		m_price_text->SetText(price.c_str());
		m_price_icon->SetXPosition({ 0, -(14.0f + (price.length() - 11) * 8.0f) });
		m_lock->SetVisible(gun->Status == STATUS_LOCK);
		m_lock_level->SetText(StringUtil::ToString(gun->UnlockLevel).c_str());
	}

	bool GUIStoreGunItem::OnItemChick(const EventArgs & events)
	{
		if (m_data == NULL)
			return false;
		if (m_data->Status == STATUS_LOCK)
		{
			if (!m_data->LockTip.empty())
			{
				ShowResultTipDialogEvent::emit(ResultCode::COMMON, m_data->LockTip.c_str());
			}
			return false;
		}
		if (m_data->Status == STATUS_NEW)
		{
			m_data->Status = STATUS_NOT_GET;
			m_data->HasChange = true;
			GunStoreSetting::useGunStoreFunc(StoreAction::FIRST_SHOW_GUN, m_data->Id);
		}
		GunStoreGunItemClickEvent::emit(m_data->Id);
		return true;
	}

}
