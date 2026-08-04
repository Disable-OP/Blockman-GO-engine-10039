#include "gui_ranchStorage.h"
#include "Util/LanguageKey.h"
#include "Util/ClientEvents.h"
#include "cWorld/Blockman.h"
#include "World/Ranch.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "GUI/GuiRanchStorageItem.h"
#include "GUI/GuiDef.h"
#include "Util/UICommon.h"
#include "Util/RedDotManager.h"
#include "Network/ClientNetwork.h"

using namespace LORD;

namespace BLOCKMAN
{
	gui_ranchStorage::gui_ranchStorage()
		: gui_layout("RanchStorage.json")
	{
		m_langMap[ViewId::ALL] = getString(LanguageKey::GUI_RANCH_STORAGE_ALL);
		m_langMap[ViewId::FARMING] = getString(LanguageKey::GUI_RANCH_STORAGE_FARMING);
		m_langMap[ViewId::MANUFACTURE] = getString(LanguageKey::GUI_RANCH_STORAGE_MANUFACTURE);
		m_langMap[ViewId::TOOL] = getString(LanguageKey::GUI_RANCH_STORAGE_TOOL);
		m_langMap[ViewId::MACHINE] = getString(LanguageKey::GUI_RANCH_STORAGE_MACHINE);
		m_langMap[ViewId::BUILDING] = getString(LanguageKey::GUI_RANCH_STORAGE_BUILDING);
		m_langMap[ViewId::PARTS] = getString(LanguageKey::GUI_RANCH_STORAGE_PARTS);
	}

	gui_ranchStorage::~gui_ranchStorage()
	{
		m_langMap.clear();
		m_radioButtonMap.clear();
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_ranchStorage::onLoad()
	{

		m_itemGridView = getWindow<GUIGridView>("RanchStorage-Panel-Grid");
		m_itemGridView->SetArea(UDim(0, 0), UDim(0, 0), UDim(1.f, 0), UDim(1.f, 0));
		m_itemGridView->InitConfig(0.f, 24.f, 10);

		m_radioButtonMap[ViewId::ALL] = getWindow<GUIRadioButton>("RanchStorage-Tab-All");
		m_radioButtonMap[ViewId::FARMING] = getWindow<GUIRadioButton>("RanchStorage-Tab-Farming");
		m_radioButtonMap[ViewId::MANUFACTURE] = getWindow<GUIRadioButton>("RanchStorage-Tab-Manufacture");
		m_radioButtonMap[ViewId::TOOL] = getWindow<GUIRadioButton>("RanchStorage-Tab-Tool");
		m_radioButtonMap[ViewId::MACHINE] = getWindow<GUIRadioButton>("RanchStorage-Tab-Machine");
		m_radioButtonMap[ViewId::BUILDING] = getWindow<GUIRadioButton>("RanchStorage-Tab-Building");
		m_radioButtonMap[ViewId::PARTS] = getWindow<GUIRadioButton>("RanchStorage-Tab-Parts");

		m_tvCapacityValue = getWindow<GUIStaticText>("RanchStorage-CapacityValue");
		m_viewSellItem = getWindow<GuiRanchSellItem>("RanchStorage-Panel-SellItem");
		m_viewSellItem->SetVisible(false);

		getWindow<GUIButton>("RanchStorage-Upgrade")->SetText(getString(LanguageKey::GUI_RANCH_HOUSE_UPGRADE));
		getWindow<GUIStaticText>("RanchStorage-CapacityName")->SetText(getString(LanguageKey::GUI_RANCH_STORAGE_CAPACITY));

		m_itemGridView->subscribeEvent(EventWindowTouchDown, std::bind(&gui_ranchStorage::onClick, this, std::placeholders::_1, Click::BTN_MAIN));
		getWindow("RanchStorage")->subscribeEvent(EventWindowTouchUp, std::bind(&gui_ranchStorage::onClick, this, std::placeholders::_1, Click::BTN_MAIN));
		getWindow<GUIButton>("RanchStorage-Upgrade")->subscribeEvent(EventButtonClick, std::bind(&gui_ranchStorage::onClick, this, std::placeholders::_1, Click::BTN_SURE));
		getWindow<GUIButton>("RanchStorage-Remove")->subscribeEvent(EventButtonClick, std::bind(&gui_ranchStorage::onClick, this, std::placeholders::_1, Click::BTN_REMOVE));
		m_subscriptionGuard.add(RanchStorageChangeEvent::subscribe(std::bind(&gui_ranchStorage::onRanchStorageChange, this)));
		m_subscriptionGuard.add(ShowRanchSellItemEvent::subscribe(std::bind(&gui_ranchStorage::showSellItem, this, std::placeholders::_1, std::placeholders::_2)));
		for (auto view : m_radioButtonMap)
		{
			view.second->SetText(m_langMap[view.first]);
			view.second->subscribeEvent(EventRadioStateChanged, std::bind(&gui_ranchStorage::onRadioChange, this, std::placeholders::_1, view.first));
		}
		m_radioButtonMap[ViewId::ALL]->SetSelected(true);
	}

	void gui_ranchStorage::onHide()
	{
		m_viewSellItem->SetVisible(false);
	}

	void gui_ranchStorage::onShow()
	{
		if (!UICommon::checkRanchParam() || !m_dataChange)
		{
			return;
		}
		auto storage = Blockman::Instance()->m_pPlayer->m_ranch->getStorage();
		String value = StringUtil::Format("%d/%d", storage->getTotalNum(), storage->capacity);
		m_tvCapacityValue->SetText(value.c_str());
		getWindow<GUIButton>("RanchStorage-Upgrade")->SetEnabled(storage->level < storage->maxLevel);
		setText("RanchStorage-Upgrade", storage->level >= storage->maxLevel ? "gui_ranch_max_level" : LanguageKey::GUI_RANCH_HOUSE_UPGRADE);
		initItem((int)m_selecdView);
		m_dataChange = false;
	}

	void gui_ranchStorage::onUpdate(ui32 nTimeElapse)
	{
		if (m_gridViewPixelSizeX != m_itemGridView->GetPixelSize().x)
		{
			m_gridViewPixelSizeX = m_itemGridView->GetPixelSize().x;
			itemSizeChange();
		}

		if (isShown())
		{
			getWindow("RanchStorage-RedDot")->SetVisible(RedDotManager::checkStorage());
		}

		if (!UICommon::checkRanchParam() || !Blockman::Instance()->m_pPlayer->m_ranch->getInfo())
		{
			getWindow("RanchStorage-Remove")->SetVisible(false);
			return;
		}

		getWindow("RanchStorage-Remove")->SetVisible(!Blockman::Instance()->m_pPlayer->m_ranch->getInfo()->isDestroyWarehouse);
	}

	bool gui_ranchStorage::onClick(const EventArgs & events, Click id)
	{
		switch (id)
		{
		case Click::BTN_REMOVE:
			ClientNetwork::Instance()->getSender()->sendRanchBuildRemove(1031000);
			RootGuiLayout::Instance()->showMainControl();
			break;

		case Click::BTN_SURE:
			auto storage = Blockman::Instance()->m_pPlayer->m_ranch->getStorage();
			if (UICommon::checkRanchLevel(storage->needPlayerLevel))
			{
				ShowRanchStorageUpgradeTipEvent::emit();
			}
			break;
		}
		m_viewSellItem->SetVisible(false);
		return true;
	}

	bool gui_ranchStorage::showSellItem(const UVector2& pos, const RanchCommon & item)
	{
		m_viewSellItem->SetVisible(true);
		m_viewSellItem->setItem(item);
		int w = (int) m_itemGridView->GetPixelSize().x / 10;
		bool isLeft = pos.d_x.d_offset < w * 8.f;
		bool isTop = pos.d_y.d_offset < (w + 24.f) * 2;
		m_viewSellItem->setBack(isLeft, isTop);
		UVector2 uv = UVector2(UDim(pos.d_x.d_scale, pos.d_x.d_offset + ( isLeft ?  w : - 291.f)), UDim(pos.d_y.d_scale, pos.d_y.d_offset + w / 2 - (isTop ? w :  265.f)));
		m_viewSellItem->SetPosition(uv);
		return false;
	}

	bool gui_ranchStorage::onRadioChange(const EventArgs & events, ViewId viewId)
	{
		if (m_radioButtonMap[viewId]->IsSelected())
		{
			m_selecdView = viewId;
			m_viewSellItem->SetVisible(false);
			initItem((int)viewId);
		}
		return true;
	}

	bool gui_ranchStorage::onRanchStorageChange()
	{
		m_dataChange = true;
		if (isShown())
		{
			onShow();
		}
		return true;
	}

	void gui_ranchStorage::initItem(i32 type)
	{
		m_itemGridView->RemoveAllItems();
		auto items = getItemsByType(type);
		for (auto item : items)
		{
			static unsigned count = 0;
			count++;
			String itemLayout = StringUtil::Format("RanchStorage-Panel-Grid-Item-%d", count).c_str();
			if (GUIWindowManager::Instance()->GetGUIWindow(itemLayout.c_str()))
			{
				return;
			}

			float w = m_itemGridView->GetPixelSize().x / 10;
			GuiRanchStorageItem* ranchStorageItem = (GuiRanchStorageItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_RANCH_STORAGE_ITEM, itemLayout.c_str());
			ranchStorageItem->setRanchItem(item);
			ranchStorageItem->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(0.0f, w), UDim(0.0f, w));
			m_itemGridView->AddItem(ranchStorageItem);
		}
	}

	void gui_ranchStorage::itemSizeChange()
	{
		for (size_t i = 0; i < m_itemGridView->GetItemCount(); i++)
		{
			float w = m_itemGridView->GetPixelSize().x / 10;
			m_itemGridView->GetItem(i)->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(0.0f, w), UDim(0.0f, w));
		}
	}
	std::vector<RanchCommon> gui_ranchStorage::getItemsByType(i32 type)
	{
		if (!Blockman::Instance()->m_pPlayer || !Blockman::Instance()->m_pPlayer->m_ranch || !Blockman::Instance()->m_pPlayer->m_ranch->getStorage())
		{
			return std::vector<RanchCommon>();
		}
		auto items = Blockman::Instance()->m_pPlayer->m_ranch->getStorage()->items;
		if (type == 0)
		{
			return items;
		}
		auto newItems = std::vector<RanchCommon>();
		for (auto item : items)
		{
			if (RanchSetting::getRanchItemType(item.itemId) == type)
			{
				newItems.push_back(item);
			}
		}
		return newItems;
	}
}
