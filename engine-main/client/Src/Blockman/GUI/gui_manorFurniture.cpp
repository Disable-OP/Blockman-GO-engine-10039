#include "gui_manorFurniture.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "GuiManorFurnitureItem.h"
#include "Util/LanguageKey.h"
#include "UI/GUICheckBox.h"
#include "Util/ClientEvents.h"
#include "cWorld/Blockman.h"
#include "World/GameSettings.h"
#include "GuiDef.h"

using namespace LORD;

namespace BLOCKMAN 
{

	gui_manorFurniture::gui_manorFurniture()
		: gui_layout("ManorFurniture.json")
	{
		
	}

	gui_manorFurniture::~gui_manorFurniture()
	{
		m_subscriptionGuard.unsubscribeAll();
		m_manorFurnitures.clear();
	}

	void gui_manorFurniture::onLoad()
	{
		m_cbEditFurniture = getWindow<GUICheckBox>("ManorFurniture-Opreration-Select");
		getWindow("ManorFurniture-Operation-Tip")->SetText(getString(LanguageKey::GUI_MANOR_FURNITURE_OPERATION_TIP));
		m_cbEditFurniture->subscribeEvent(EventCheckStateChanged, std::bind(&gui_manorFurniture::onCheck, this, std::placeholders::_1));
		m_itemGridView = (GUIGridView*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_GRID_VIEW, "ManorFurniture-Furnitures-Grid");
		m_itemGridView->SetArea(UDim(0, 11.f), UDim(0, 9.f), UDim(1.f, -22.0f), UDim(1.f, -18.0f));
		m_itemGridView->InitConfig(3.f, 3.f, 6);
		getWindow("ManorFurniture-Furnitures")->AddChildWindow(m_itemGridView);
		m_gridViewPixelSizeX = m_itemGridView->GetPixelSize().x;
		initItem();
		m_subscriptionGuard.add(ManorOnUpdateEvent::subscribe(std::bind(&gui_manorFurniture::manorOnUpdate, this)));
	}

	void gui_manorFurniture::onUpdate(ui32 nTimeElapse)
	{
		if (m_gridViewPixelSizeX != m_itemGridView->GetPixelSize().x)
		{
			m_gridViewPixelSizeX = m_itemGridView->GetPixelSize().x;
			itemSizeChange();
		}
	}

	bool gui_manorFurniture::onCheck(const EventArgs & events)
	{
		Blockman::Instance()->m_gameSettings->m_isEditFurniture = m_cbEditFurniture->GetChecked();
		return true;
	}

	bool gui_manorFurniture::manorOnUpdate()
	{
		if (Blockman::Instance()->m_pPlayer->m_manor)
		{
			m_manorFurnitures = Blockman::Instance()->m_pPlayer->m_manor->getFurnitures();
		}
		initItem();
		return false;
	}

	bool gui_manorFurniture::initItem()
	{
		m_itemGridView->RemoveAllItems();
		for (ManorFurniture* item : m_manorFurnitures)
		{
			static unsigned count = 0;
			count++;
			String itemLayout = StringUtil::Format("ManorFurniture-Furnitures-Item-%d", count).c_str();
			if (GUIWindowManager::Instance()->GetGUIWindow(itemLayout.c_str()))
			{
				return false;
			}
			float w = (m_itemGridView->GetPixelSize().x - 15.0f) / 6;
			GuiManorFurnitureItem* furnitureItem = (GuiManorFurnitureItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_MANOR_FURNITURE_ITEM, itemLayout.c_str());
			furnitureItem->setManorFurniture(item);
			furnitureItem->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(0.0f, w), UDim(0.0f, w + 98.f));
			furnitureItem->SetTouchable(false);
			m_itemGridView->SetClipChild(true);
			m_itemGridView->AddItem(furnitureItem);
		}
		return true;
	}

	bool gui_manorFurniture::itemSizeChange()
	{
		for (size_t i = 0; i < m_itemGridView->GetItemCount(); i++)
		{
			float w = (m_itemGridView->GetPixelSize().x - 15.0f) / 6;
			m_itemGridView->GetItem(i)->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(0.0f, w), UDim(0.0f, w + 98.f));
		}
		return true;
	}



}
