#include "GUIPixelGunHallModeSelectMapItem.h"
#include "UI/GUIWindowManager.h"
#include "GUI/GuiDef.h"
#include "RootGuiLayout.h"
#include "game.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"


using namespace LORD;
namespace BLOCKMAN
{
	vector<GUIWindow*>::type GUIPixelGunHallModeSelectMapItem::getPrivateChildren()
	{
		return { m_window };
	}

	GUIPixelGunHallModeSelectMapItem::GUIPixelGunHallModeSelectMapItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = GUIWindowManager::Instance()->LoadWindowFromJSON("PixelGunHallModeSelectMapItem.json");
		renameComponents(m_window);

		m_map_img = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		m_map_di = dynamic_cast<GUIStaticImage*>(m_map_img->GetChildByIndex(0));
		m_map_size = dynamic_cast<GUIStaticText*>(m_map_img->GetChildByIndex(1));
		m_map_small_info = dynamic_cast<GUIStaticImage*>(m_map_img->GetChildByIndex(2));
		m_map_name = dynamic_cast<GUIStaticText*>(m_map_img->GetChildByIndex(3));
		m_map_lock_meng = dynamic_cast<GUIStaticImage*>(m_map_img->GetChildByIndex(4));
		m_map_lock = dynamic_cast<GUIStaticImage*>(m_map_img->GetChildByIndex(5));
		m_map_select = dynamic_cast<GUIStaticImage*>(m_map_img->GetChildByIndex(6));

		m_map_img->subscribeEvent(EventWindowClick, std::bind(&GUIPixelGunHallModeSelectMapItem::onMapImgClick, this));

		AddWindowToDrawList(*m_window);
	}

	void GUIPixelGunHallModeSelectMapItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());
		BaseClass::UpdateSelf(nTimeElapse);
	}

	void GUIPixelGunHallModeSelectMapItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GUIPixelGunHallModeSelectMapItem::removeComponents()
	{
		if (m_window)
		{
			RemoveWindowFromDrawList(*m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
 		m_window = nullptr;

		m_map_di = nullptr;
		m_map_img = nullptr;
		m_map_lock_meng = nullptr;
		m_map_lock = nullptr;
		m_map_size = nullptr;
		m_map_small_info = nullptr;
		m_map_name = nullptr;
		m_map_select = nullptr;
	}

	void GUIPixelGunHallModeSelectMapItem::RefreshItem(MapItemInfo& map_info, int index)
	{
		m_map_info.map_id = map_info.map_id;
		m_map_info.map_img = map_info.map_img;
		m_map_info.small_info_img = map_info.small_info_img;
		m_map_info.map_size = map_info.map_size;
		m_map_info.unlock = map_info.unlock;
		m_map_info.map_name = map_info.map_name;
		m_map_info.map_show_name = map_info.map_show_name;
		m_map_info.openMoneyNum = map_info.openMoneyNum;
		m_map_info.openMoneyType = map_info.openMoneyType;
		m_map_info.map_type = map_info.map_type;

		m_select_index = index;

		if (map_info.map_img.length() > 0)
		{
			m_map_img->SetImage(map_info.map_img.c_str());
			m_map_img->SetVisible(true);
		}
		else
		{
			m_map_img->SetVisible(false);
		}

		if (map_info.small_info_img.length() > 0)
		{
			m_map_small_info->SetImage(map_info.small_info_img.c_str());
			m_map_small_info->SetVisible(true);
		}
		else
		{
			m_map_small_info->SetVisible(false);
		}

		m_map_size->SetText(map_info.map_size.c_str());
		m_map_size->SetVisible(true);
		m_map_name->SetText(LanguageManager::Instance()->getString(map_info.map_show_name.c_str()).c_str());
		m_map_name->SetVisible(true);

		m_map_select->SetVisible(false);

		if (map_info.openMoneyType == MOT_INVALID)
		{
			m_map_lock_meng->SetVisible(false);
			m_map_lock->SetVisible(false);
		}
		else if (map_info.openMoneyType == MOT_LV || map_info.openMoneyType == MOT_DIAMOND)
		{
			if (map_info.unlock)
			{
				m_map_lock_meng->SetVisible(false);
				m_map_lock->SetVisible(false);
			}
			else
			{
				m_map_lock_meng->SetVisible(true);
				m_map_lock->SetVisible(true);
			}
		}
		else
		{
			m_map_lock_meng->SetVisible(false);
			m_map_lock->SetVisible(false);
		}

		if (map_info.map_type == MT_RANDOM)
		{
			m_map_di->SetVisible(false);
			m_map_size->SetVisible(false);
			m_map_small_info->SetVisible(true);
		}
		else if (map_info.map_type == MT_FRAGMENT)
		{
			m_map_di->SetVisible(true);
			m_map_size->SetVisible(true);
			m_map_small_info->SetVisible(true);
		}
		else
		{
			m_map_di->SetVisible(true);
			m_map_size->SetVisible(true);
			m_map_small_info->SetVisible(false);
		}
	}

	bool GUIPixelGunHallModeSelectMapItem::onMapImgClick()
	{
		if (m_map_info.openMoneyType == MOT_NOTOPEN)
		{
			ShowPixelGunHallNotOpenEvent::emit(1);
			return true;
		}

		if (m_map_info.openMoneyType == MOT_LV && m_map_info.unlock == false)
		{
			ShowPixelGunHallLvNotEnoughEvent::emit(m_map_info.openMoneyNum, 1);
			return true;
		}

		if (m_map_info.openMoneyType == MOT_DIAMOND && m_map_info.unlock == false)
		{
			if (Blockman::Instance() && Blockman::Instance()->m_pPlayer)
			{
				Wallet * wallet = Blockman::Instance()->m_pPlayer->m_wallet;
				if (wallet)
				{
					i64 money = wallet->getMoneyByCoinId(CurrencyType::PLATFORM_MONEY);
					if (money < m_map_info.openMoneyNum)
					{
						i32 diff = (i32)(m_map_info.openMoneyNum - money);
						ShowLackOfMoneyTipDialogEvent::emit(diff, CurrencyType::PLATFORM_MONEY);
						return true;
					}
				}
				else
				{
					return true;
				}
			}

			ShowPixelGunHallLvMapLockDiamondEvent::emit(m_map_info.openMoneyNum, m_map_info.map_id);
			return true;
		}

		ShowPixelGunHallModeSelectReWardInfoEvent::emit(m_select_index);
		ShowPixelGunHallMapSelectChangeEvent::emit(m_select_index);
		m_map_select->SetVisible(true);

		return true;
	}

	void GUIPixelGunHallModeSelectMapItem::updateSelect(bool select)
	{
		m_map_select->SetVisible(select);
	}
}
